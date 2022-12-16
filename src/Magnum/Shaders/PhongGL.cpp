/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "PhongGL.h"

#if defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_BUILD_DEPRECATED)
#include <Corrade/Containers/Array.h>
#endif
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/TextureArray.h"
#endif

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

using namespace Containers::Literals;

namespace {
    enum: Int {
        AmbientTextureUnit = 0,
        DiffuseTextureUnit = 1,
        SpecularTextureUnit = 2,
        NormalTextureUnit = 3,
        /* 4 taken by MeshVisualizer colormap */
        ObjectIdTextureUnit = 5 /* shared with Flat and MeshVisualizer */
    };

    #ifndef MAGNUM_TARGET_GLES2
    enum: Int {
        /* Projection, transformation, texture transformation and joints is
           slots 0, 1, 3, 6 in all shaders so shaders can be switched without
           rebinding everything */
        ProjectionBufferBinding = 0,
        TransformationBufferBinding = 1,
        DrawBufferBinding = 2,
        TextureTransformationBufferBinding = 3,
        MaterialBufferBinding = 4,
        LightBufferBinding = 5,
        JointBufferBinding = 6,
    };
    #endif
}

PhongGL::CompileState PhongGL::compile(const Configuration& configuration) {
    #ifndef CORRADE_NO_ASSERT
    {
        const bool textureTransformationNotEnabledOrTextured = !(configuration.flags() & Flag::TextureTransformation) || (configuration.flags() & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture))
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
            ;
        CORRADE_ASSERT(textureTransformationNotEnabledOrTextured,
            "Shaders::PhongGL: texture transformation enabled but the shader is not textured", CompileState{NoCreate});
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() >= Flag::InstancedObjectId) || !(configuration.flags() & Flag::Bitangent),
        "Shaders::PhongGL: Bitangent attribute binding conflicts with the ObjectId attribute, use a Tangent4 attribute with instanced object ID rendering instead", CompileState{NoCreate});
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.materialCount(),
        "Shaders::PhongGL: material count can't be zero", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.drawCount(),
        "Shaders::PhongGL: draw count can't be zero", CompileState{NoCreate});
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() & Flag::TextureArrays) || (configuration.flags() & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture)) || configuration.flags() >= Flag::ObjectIdTexture,
        "Shaders::PhongGL: texture arrays enabled but the shader is not textured", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::UniformBuffers) || !(configuration.flags() & Flag::TextureArrays) || configuration.flags() >= (Flag::TextureArrays|Flag::TextureTransformation),
        "Shaders::PhongGL: texture arrays require texture transformation enabled as well if uniform buffers are used", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::LightCulling) || (configuration.flags() & Flag::UniformBuffers),
        "Shaders::PhongGL: light culling requires uniform buffers to be enabled", CompileState{NoCreate});
    #endif

    CORRADE_ASSERT(!(configuration.flags() & Flag::SpecularTexture) || !(configuration.flags() & (Flag::NoSpecular)),
        "Shaders::PhongGL: specular texture requires the shader to not have specular disabled", CompileState{NoCreate});

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() & Flag::DynamicPerVertexJointCount) || configuration.jointCount(),
        "Shaders::PhongGL: dynamic per-vertex joint count enabled for zero joints", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::InstancedTransformation) || !configuration.secondaryPerVertexJointCount(),
        "Shaders::PhongGL: TransformationMatrix attribute binding conflicts with the SecondaryJointIds / SecondaryWeights attributes, use a non-instanced rendering with secondary weights instead", CompileState{NoCreate});
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(configuration.flags() >= Flag::UniformBuffers)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::uniform_buffer_object);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::MultiDraw) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::shader_draw_parameters);
        #elif !defined(MAGNUM_TARGET_WEBGL)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ANGLE::multi_draw);
        #else
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::WEBGL::multi_draw);
        #endif
    }
    #endif
    #ifndef MAGNUM_TARGET_GLES
    if(configuration.flags() >= Flag::TextureArrays)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::EXT::texture_array);
    #endif

    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShadersGL"))
        importShaderResources();
    #endif
    Utility::Resource rs("MagnumShadersGL");

    const GL::Context& context = GL::Context::current();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || context.isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>(),
        "Shaders::PhongGL: uniform buffers require" << GL::Extensions::ARB::uniform_buffer_object::string(), CompileState{NoCreate});
    #endif

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = context.supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    #else
    const GL::Version version = context.supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    PhongGL out{NoInit};
    out._flags = configuration.flags();
    out._lightCount = configuration.lightCount();
    #ifndef MAGNUM_TARGET_GLES2
    out._jointCount = configuration.jointCount();
    out._perVertexJointCount = configuration.perVertexJointCount();
    out._secondaryPerVertexJointCount = configuration.secondaryPerVertexJointCount();
    out._materialCount = configuration.materialCount();
    out._drawCount = configuration.drawCount();
    #endif
    out._lightColorsUniform = out._lightPositionsUniform + Int(configuration.lightCount());
    out._lightSpecularColorsUniform = out._lightColorsUniform + Int(configuration.lightCount());
    out._lightRangesUniform = out._lightSpecularColorsUniform + Int(configuration.lightCount());
    #ifndef MAGNUM_TARGET_GLES2
    out._jointMatricesUniform = out._lightRangesUniform + Int(configuration.lightCount());
    out._perInstanceJointCountUniform = out._jointMatricesUniform + configuration.jointCount();
    out._perVertexJointCountUniform = configuration.flags() >= Flag::UniformBuffers ?
        1 : out._perInstanceJointCountUniform + 1;
    #endif

    GL::Shader vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    GL::Shader frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    #ifndef MAGNUM_TARGET_GLES
    std::string lightInitializer;
    if(!(configuration.flags() >= Flag::UniformBuffers) && configuration.lightCount()) {
        using namespace Containers::Literals;

        /* Initializer for the light color / position / range arrays -- we need
           a list of initializers joined by commas. For GLES we'll simply
           upload the values directly. */
        constexpr Containers::StringView lightPositionInitializerPreamble = "#define LIGHT_POSITION_INITIALIZER "_s;
        constexpr Containers::StringView lightColorInitializerPreamble = "#define LIGHT_COLOR_INITIALIZER "_s;
        constexpr Containers::StringView lightRangeInitializerPreamble = "#define LIGHT_RANGE_INITIALIZER "_s;
        constexpr Containers::StringView lightPositionInitializerItem = "vec4(0.0, 0.0, 1.0, 0.0), "_s;
        constexpr Containers::StringView lightColorInitializerItem = "vec3(1.0), "_s;
        constexpr Containers::StringView lightRangeInitializerItem = "1.0/0.0, "_s;

        lightInitializer.reserve(
            lightPositionInitializerPreamble.size() +
            lightColorInitializerPreamble.size() +
            lightRangeInitializerPreamble.size() +
            configuration.lightCount()*(lightPositionInitializerItem.size() +
                                        lightColorInitializerItem.size() +
                                        lightRangeInitializerItem.size()));

        lightInitializer.append(lightPositionInitializerPreamble);
        for(std::size_t i = 0; i != configuration.lightCount(); ++i)
            lightInitializer.append(lightPositionInitializerItem);

        /* Drop the last comma and add a newline at the end */
        lightInitializer[lightInitializer.size() - 2] = '\n';
        lightInitializer.resize(lightInitializer.size() - 1);

        lightInitializer.append(lightColorInitializerPreamble);
        for(std::size_t i = 0; i != configuration.lightCount(); ++i)
            lightInitializer.append(lightColorInitializerItem);

        /* Drop the last comma and add a newline at the end */
        lightInitializer[lightInitializer.size() - 2] = '\n';
        lightInitializer.resize(lightInitializer.size() - 1);

        lightInitializer.append(lightRangeInitializerPreamble);
        for(std::size_t i = 0; i != configuration.lightCount(); ++i)
            lightInitializer.append(lightRangeInitializerItem);

        /* Drop the last comma and add a newline at the end */
        lightInitializer[lightInitializer.size() - 2] = '\n';
        lightInitializer.resize(lightInitializer.size() - 1);
    }
    #endif

    vert.addSource((configuration.flags() & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture)
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
            ) ? "#define TEXTURED\n" : "")
        .addSource(configuration.flags() & Flag::NormalTexture ? "#define NORMAL_TEXTURE\n" : "")
        .addSource(configuration.flags() & Flag::Bitangent ? "#define BITANGENT\n" : "")
        .addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
        .addSource(configuration.flags() & Flag::TextureTransformation ? "#define TEXTURE_TRANSFORMATION\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
        #endif
        .addSource(configuration.lightCount() ? "#define HAS_LIGHTS\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        #endif
        .addSource(configuration.flags() & Flag::InstancedTransformation ? "#define INSTANCED_TRANSFORMATION\n" : "")
        .addSource(configuration.flags() >= Flag::InstancedTextureOffset ? "#define INSTANCED_TEXTURE_OFFSET\n" : "");
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.jointCount()) {
        vert.addSource(Utility::formatString(
            "#define JOINT_COUNT {}\n"
            "#define PER_VERTEX_JOINT_COUNT {}u\n"
            "#define SECONDARY_PER_VERTEX_JOINT_COUNT {}u\n"
            "#define JOINT_MATRICES_LOCATION {}\n"
            #ifndef MAGNUM_TARGET_GLES
            "#define JOINT_MATRIX_INITIALIZER {}\n"
            #endif
            "#define PER_INSTANCE_JOINT_COUNT_LOCATION {}\n",
            configuration.jointCount(),
            configuration.perVertexJointCount(),
            configuration.secondaryPerVertexJointCount(),
            out._jointMatricesUniform,
            #ifndef MAGNUM_TARGET_GLES
            ("mat4(1.0), "_s*configuration.jointCount()).exceptSuffix(2),
            #endif
            out._perInstanceJointCountUniform));
    }
    if(configuration.flags() >= Flag::DynamicPerVertexJointCount) {
        vert.addSource(Utility::formatString(
            "#define DYNAMIC_PER_VERTEX_JOINT_COUNT\n"
            "#define PER_VERTEX_JOINT_COUNT_LOCATION {}\n",
            out._perVertexJointCountUniform));
    }
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers) {
        vert.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n",
            configuration.drawCount(),
            configuration.lightCount()));
        vert.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif
    vert.addSource(rs.getString("generic.glsl"))
        .addSource(rs.getString("Phong.vert"));
    frag.addSource(configuration.flags() & Flag::AmbientTexture ? "#define AMBIENT_TEXTURE\n" : "")
        .addSource(configuration.flags() & Flag::DiffuseTexture ? "#define DIFFUSE_TEXTURE\n" : "")
        .addSource(configuration.flags() & Flag::SpecularTexture ? "#define SPECULAR_TEXTURE\n" : "")
        .addSource(configuration.flags() & Flag::NormalTexture ? "#define NORMAL_TEXTURE\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
        #endif
        .addSource(configuration.flags() & Flag::Bitangent ? "#define BITANGENT\n" : "")
        .addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
        .addSource(configuration.flags() & Flag::AlphaMask ? "#define ALPHA_MASK\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::ObjectId ? "#define OBJECT_ID\n" : "")
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        .addSource(configuration.flags() >= Flag::ObjectIdTexture ? "#define OBJECT_ID_TEXTURE\n" : "")
        #endif
        .addSource(configuration.flags() & Flag::NoSpecular ? "#define NO_SPECULAR\n" : "")
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers) {
        frag.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n"
            "#define MATERIAL_COUNT {}\n"
            "#define LIGHT_COUNT {}\n",
            configuration.drawCount(),
            configuration.materialCount(),
            configuration.lightCount()));
        frag.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "")
            .addSource(configuration.flags() >= Flag::LightCulling ? "#define LIGHT_CULLING\n" : "");
    } else
    #endif
    {
        frag.addSource(Utility::formatString(
            "#define LIGHT_COUNT {}\n"
            "#define LIGHT_COLORS_LOCATION {}\n"
            "#define LIGHT_SPECULAR_COLORS_LOCATION {}\n"
            "#define LIGHT_RANGES_LOCATION {}\n",
            configuration.lightCount(),
            out._lightColorsUniform,
            out._lightSpecularColorsUniform,
            out._lightRangesUniform));
    }
    #ifndef MAGNUM_TARGET_GLES
    if(!(configuration.flags() >= Flag::UniformBuffers) && configuration.lightCount())
        frag.addSource(std::move(lightInitializer));
    #endif
    frag.addSource(rs.getString("generic.glsl"))
        .addSource(rs.getString("Phong.frag"));

    vert.submitCompile();
    frag.submitCompile();

    out.attachShaders({vert, frag});

    /* ES3 has this done in the shader directly and doesn't even provide
       bindFragmentDataLocation() */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        out.bindAttributeLocation(Position::Location, "position");
        if(configuration.lightCount())
            out.bindAttributeLocation(Normal::Location, "normal");
        if((configuration.flags() & Flag::NormalTexture) && configuration.lightCount()) {
            out.bindAttributeLocation(Tangent::Location, "tangent");
            if(configuration.flags() & Flag::Bitangent)
                out.bindAttributeLocation(Bitangent::Location, "bitangent");
        }
        if(configuration.flags() & Flag::VertexColor)
            out.bindAttributeLocation(Color3::Location, "vertexColor"); /* Color4 is the same */
        if(configuration.flags() & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture)
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
        )
            out.bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() & Flag::ObjectId) {
            out.bindFragmentDataLocation(ColorOutput, "color");
            out.bindFragmentDataLocation(ObjectIdOutput, "objectId");
        }
        if(configuration.flags() >= Flag::InstancedObjectId)
            out.bindAttributeLocation(ObjectId::Location, "instanceObjectId");
        #endif
        if(configuration.flags() & Flag::InstancedTransformation) {
            out.bindAttributeLocation(TransformationMatrix::Location, "instancedTransformationMatrix");
            if(configuration.lightCount())
                out.bindAttributeLocation(NormalMatrix::Location, "instancedNormalMatrix");
        }
        if(configuration.flags() >= Flag::InstancedTextureOffset)
            out.bindAttributeLocation(TextureOffset::Location, "instancedTextureOffset");
        #ifndef MAGNUM_TARGET_GLES2
        /* Configuration::setJointCount() checks that jointCount and
           perVertexJointCount / secondaryPerVertexJointCount are either all
           zero or non-zero so we don't need to check for jointCount() here */
        if(configuration.perVertexJointCount()) {
            out.bindAttributeLocation(Weights::Location, "weights");
            out.bindAttributeLocation(JointIds::Location, "jointIds");
        }
        if(configuration.secondaryPerVertexJointCount()) {
            out.bindAttributeLocation(SecondaryWeights::Location, "secondaryWeights");
            out.bindAttributeLocation(SecondaryJointIds::Location, "secondaryJointIds");
        }
        #endif
    }
    #endif

    out.submitLink();

    return CompileState{std::move(out), std::move(vert), std::move(frag), version};
}

PhongGL::CompileState PhongGL::compile() {
    return compile(Configuration{});
}

#ifdef MAGNUM_BUILD_DEPRECATED
PhongGL::CompileState PhongGL::compile(const Flags flags, const UnsignedInt lightCount) {
    return compile(Configuration{}
        .setFlags(flags)
        .setLightCount(lightCount));
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL::CompileState PhongGL::compile(const Flags flags, const UnsignedInt lightCount, const UnsignedInt materialCount, const UnsignedInt drawCount) {
    return compile(Configuration{}
        .setFlags(flags)
        .setLightCount(lightCount)
        .setMaterialCount(materialCount)
        .setDrawCount(drawCount));
}
#endif
#endif

PhongGL::PhongGL(CompileState&& state): PhongGL{static_cast<PhongGL&&>(std::move(state))} {
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* When graceful assertions fire from within compile(), we get a NoCreate'd
       CompileState. Exiting makes it possible to test the assert. */
    if(!id()) return;
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag)}));

    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(state._version))
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES2
        if(_flags >= Flag::DynamicPerVertexJointCount)
            _perVertexJointCountUniform = uniformLocation("perVertexJointCount");
        if(_flags >= Flag::UniformBuffers) {
            if(_drawCount > 1) _drawOffsetUniform = uniformLocation("drawOffset");
        } else
        #endif
        {
            _transformationMatrixUniform = uniformLocation("transformationMatrix");
            if(_flags & Flag::TextureTransformation)
                _textureMatrixUniform = uniformLocation("textureMatrix");
            #ifndef MAGNUM_TARGET_GLES2
            if(_flags & Flag::TextureArrays)
                _textureLayerUniform = uniformLocation("textureLayer");
            #endif
            _projectionMatrixUniform = uniformLocation("projectionMatrix");
            _ambientColorUniform = uniformLocation("ambientColor");
            if(_lightCount) {
                _normalMatrixUniform = uniformLocation("normalMatrix");
                _diffuseColorUniform = uniformLocation("diffuseColor");
                if(!(_flags & Flag::NoSpecular)) {
                    _specularColorUniform = uniformLocation("specularColor");
                    _shininessUniform = uniformLocation("shininess");
                }
                if(_flags & Flag::NormalTexture)
                    _normalTextureScaleUniform = uniformLocation("normalTextureScale");
                _lightPositionsUniform = uniformLocation("lightPositions");
                _lightColorsUniform = uniformLocation("lightColors");
                if(!(_flags & Flag::NoSpecular))
                    _lightSpecularColorsUniform = uniformLocation("lightSpecularColors");
                _lightRangesUniform = uniformLocation("lightRanges");
            }
            if(_flags & Flag::AlphaMask) _alphaMaskUniform = uniformLocation("alphaMask");
            #ifndef MAGNUM_TARGET_GLES2
            if(_flags & Flag::ObjectId) _objectIdUniform = uniformLocation("objectId");
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            if(_jointCount) {
                _jointMatricesUniform = uniformLocation("jointMatrices");
                _perInstanceJointCountUniform = uniformLocation("perInstanceJointCount");
            }
            #endif
        }
    }

    #ifndef MAGNUM_TARGET_GLES
    if(_flags && !context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(state._version))
    #endif
    {
        if(_flags & Flag::AmbientTexture) setUniform(uniformLocation("ambientTexture"), AmbientTextureUnit);
        if(_lightCount) {
            if(_flags & Flag::DiffuseTexture) setUniform(uniformLocation("diffuseTexture"), DiffuseTextureUnit);
            if(_flags & Flag::SpecularTexture) setUniform(uniformLocation("specularTexture"), SpecularTextureUnit);
            if(_flags & Flag::NormalTexture) setUniform(uniformLocation("normalTexture"), NormalTextureUnit);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(_flags >= Flag::ObjectIdTexture) setUniform(uniformLocation("objectIdTextureData"), ObjectIdTextureUnit);
        if(_flags >= Flag::UniformBuffers) {
            setUniformBlockBinding(uniformBlockIndex("Projection"), ProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Transformation"), TransformationBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"), DrawBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"), MaterialBufferBinding);
            if(_flags & Flag::TextureTransformation)
                setUniformBlockBinding(uniformBlockIndex("TextureTransformation"), TextureTransformationBufferBinding);
            if(_lightCount)
                setUniformBlockBinding(uniformBlockIndex("Light"), LightBufferBinding);
            if(_jointCount)
                setUniformBlockBinding(uniformBlockIndex("Joint"), JointBufferBinding);
        }
        #endif
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_GLES2
    if(_flags >= Flag::DynamicPerVertexJointCount)
        setPerVertexJointCount(_perVertexJointCount, _secondaryPerVertexJointCount);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    if(_flags >= Flag::UniformBuffers) {
        /* Draw offset is zero by default */
    } else
    #endif
    {
        /* Default to fully opaque white so we can see the textures */
        if(_flags & Flag::AmbientTexture) setAmbientColor(Magnum::Color4{1.0f});
        else setAmbientColor(Magnum::Color4{0.0f});
        setTransformationMatrix(Matrix4{Math::IdentityInit});
        setProjectionMatrix(Matrix4{Math::IdentityInit});
        if(_lightCount) {
            setDiffuseColor(Magnum::Color4{1.0f});
            if(!(_flags & Flag::NoSpecular)) {
                setSpecularColor(Magnum::Color4{1.0f, 0.0f});
                setShininess(80.0f);
            }
            if(_flags & Flag::NormalTexture)
                setNormalTextureScale(1.0f);
            setLightPositions(Containers::Array<Vector4>{DirectInit, _lightCount, Vector4{0.0f, 0.0f, 1.0f, 0.0f}});
            Containers::Array<Magnum::Color3> colors{DirectInit, _lightCount, Magnum::Color3{1.0f}};
            setLightColors(colors);
            if(!(_flags & Flag::NoSpecular))
                setLightSpecularColors(colors);
            setLightRanges(Containers::Array<Float>{DirectInit, _lightCount, Constants::inf()});
            /* Light position is zero by default */
            setNormalMatrix(Matrix3x3{Math::IdentityInit});
        }
        if(_flags & Flag::TextureTransformation)
            setTextureMatrix(Matrix3{Math::IdentityInit});
        /* Texture layer is zero by default */
        if(_flags & Flag::AlphaMask) setAlphaMask(0.5f);
        /* Object ID is zero by default */
        #ifndef MAGNUM_TARGET_GLES2
        if(_jointCount) {
            setJointMatrices(Containers::Array<Matrix4>{DirectInit, _jointCount, Math::IdentityInit});
            /* Per-instance joint count is zero by default */
        }
        #endif
    }
    #endif
}

PhongGL::PhongGL(const Configuration& configuration): PhongGL{compile(configuration)} {}

PhongGL::PhongGL(): PhongGL{Configuration{}} {}

#ifdef MAGNUM_BUILD_DEPRECATED
PhongGL::PhongGL(const Flags flags, const UnsignedInt lightCount): PhongGL{Configuration{}
    .setFlags(flags)
    .setLightCount(lightCount)} {}

#ifndef MAGNUM_TARGET_GLES2
PhongGL::PhongGL(const Flags flags, const UnsignedInt lightCount, const UnsignedInt materialCount, const UnsignedInt drawCount): PhongGL{compile(Configuration{}
    .setFlags(flags)
    .setLightCount(lightCount)
    .setMaterialCount(materialCount)
    .setDrawCount(drawCount))} {}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::setPerVertexJointCount(const UnsignedInt count, const UnsignedInt secondaryCount) {
    CORRADE_ASSERT(_flags >= Flag::DynamicPerVertexJointCount,
        "Shaders::PhongGL::setPerVertexJointCount(): the shader was not created with dynamic per-vertex joint count enabled", *this);
    CORRADE_ASSERT(count <= _perVertexJointCount,
        "Shaders::PhongGL::setPerVertexJointCount(): expected at most" << _perVertexJointCount << "per-vertex joints, got" << count, *this);
    CORRADE_ASSERT(secondaryCount <= _secondaryPerVertexJointCount,
        "Shaders::PhongGL::setPerVertexJointCount(): expected at most" << _secondaryPerVertexJointCount << "secondary per-vertex joints, got" << secondaryCount, *this);
    setUniform(_perVertexJointCountUniform, Vector2ui{count, secondaryCount});
    return *this;
}
#endif

PhongGL& PhongGL::setAmbientColor(const Magnum::Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setAmbientColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_ambientColorUniform, color);
    return *this;
}

PhongGL& PhongGL::setDiffuseColor(const Magnum::Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setDiffuseColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    if(_lightCount) setUniform(_diffuseColorUniform, color);
    return *this;
}

PhongGL& PhongGL::setSpecularColor(const Magnum::Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setSpecularColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(!(_flags >= Flag::NoSpecular),
        "Shaders::PhongGL::setSpecularColor(): the shader was created with specular disabled", *this);
    if(_lightCount) setUniform(_specularColorUniform, color);
    return *this;
}

PhongGL& PhongGL::setShininess(Float shininess) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setShininess(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(!(_flags >= Flag::NoSpecular),
        "Shaders::PhongGL::setShininess(): the shader was created with specular disabled", *this);
    if(_lightCount) setUniform(_shininessUniform, shininess);
    return *this;
}

PhongGL& PhongGL::setNormalTextureScale(const Float scale) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setNormalTextureScale(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & Flag::NormalTexture,
        "Shaders::PhongGL::setNormalTextureScale(): the shader was not created with normal texture enabled", *this);
    if(_lightCount) setUniform(_normalTextureScaleUniform, scale);
    return *this;
}

PhongGL& PhongGL::setAlphaMask(Float mask) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setAlphaMask(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & Flag::AlphaMask,
        "Shaders::PhongGL::setAlphaMask(): the shader was not created with alpha mask enabled", *this);
    setUniform(_alphaMaskUniform, mask);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::setObjectId(UnsignedInt id) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setObjectId(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::ObjectId,
        "Shaders::PhongGL::setObjectId(): the shader was not created with object ID enabled", *this);
    setUniform(_objectIdUniform, id);
    return *this;
}
#endif

PhongGL& PhongGL::setTransformationMatrix(const Matrix4& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setTransformationMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_transformationMatrixUniform, matrix);
    return *this;
}

PhongGL& PhongGL::setNormalMatrix(const Matrix3x3& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setNormalMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    if(_lightCount) setUniform(_normalMatrixUniform, matrix);
    return *this;
}

PhongGL& PhongGL::setProjectionMatrix(const Matrix4& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setProjectionMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_projectionMatrixUniform, matrix);
    return *this;
}

PhongGL& PhongGL::setTextureMatrix(const Matrix3& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setTextureMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::PhongGL::setTextureMatrix(): the shader was not created with texture transformation enabled", *this);
    setUniform(_textureMatrixUniform, matrix);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::setTextureLayer(UnsignedInt id) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setTextureLayer(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::PhongGL::setTextureLayer(): the shader was not created with texture arrays enabled", *this);
    setUniform(_textureLayerUniform, id);
    return *this;
}
#endif

PhongGL& PhongGL::setLightPositions(const Containers::ArrayView<const Vector4> positions) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setLightPositions(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_lightCount == positions.size(),
        "Shaders::PhongGL::setLightPositions(): expected" << _lightCount << "items but got" << positions.size(), *this);
    if(_lightCount) setUniform(_lightPositionsUniform, positions);
    return *this;
}

/* It's light, but can't be in the header because MSVC needs to know the size
   of Vector3 for the initializer list use */
PhongGL& PhongGL::setLightPositions(const std::initializer_list<Vector4> positions) {
    return setLightPositions(Containers::arrayView(positions));
}

#ifdef MAGNUM_BUILD_DEPRECATED
PhongGL& PhongGL::setLightPositions(const Containers::ArrayView<const Vector3> positions) {
    Containers::Array<Vector4> fourComponent{NoInit, positions.size()};
    for(std::size_t i = 0; i != positions.size(); ++i)
        fourComponent[i] = Vector4{positions[i], 0.0f};
    setLightPositions(fourComponent);
    return *this;
}

PhongGL& PhongGL::setLightPositions(const std::initializer_list<Vector3> positions) {
    CORRADE_IGNORE_DEPRECATED_PUSH
    return setLightPositions(Containers::arrayView(positions));
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

PhongGL& PhongGL::setLightPosition(const UnsignedInt id, const Vector4& position) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setLightPosition(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(id < _lightCount,
        "Shaders::PhongGL::setLightPosition(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
    setUniform(_lightPositionsUniform + id, position);
    return *this;
}

#ifdef MAGNUM_BUILD_DEPRECATED
PhongGL& PhongGL::setLightPosition(UnsignedInt id, const Vector3& position) {
    return setLightPosition(id, Vector4{position, 0.0f});
}

PhongGL& PhongGL::setLightPosition(const Vector3& position) {
    /* Use the list variant to check the shader really has just one light */
    return setLightPositions({Vector4{position, 0.0f}});
}
#endif

PhongGL& PhongGL::setLightColors(const Containers::ArrayView<const Magnum::Color3> colors) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setLightColors(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_lightCount == colors.size(),
        "Shaders::PhongGL::setLightColors(): expected" << _lightCount << "items but got" << colors.size(), *this);
    if(_lightCount) setUniform(_lightColorsUniform, colors);
    return *this;
}

#ifdef MAGNUM_BUILD_DEPRECATED
PhongGL& PhongGL::setLightColors(const Containers::ArrayView<const Magnum::Color4> colors) {
    Containers::Array<Magnum::Color3> threeComponent{NoInit, colors.size()};
    for(std::size_t i = 0; i != colors.size(); ++i)
        threeComponent[i] = colors[i].rgb();
    setLightColors(threeComponent);
    return *this;
}

PhongGL& PhongGL::setLightColors(const std::initializer_list<Magnum::Color4> colors) {
    CORRADE_IGNORE_DEPRECATED_PUSH
    return setLightColors(Containers::arrayView(colors));
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

PhongGL& PhongGL::setLightColors(const std::initializer_list<Magnum::Color3> colors) {
    return setLightColors(Containers::arrayView(colors));
}

PhongGL& PhongGL::setLightColor(const UnsignedInt id, const Magnum::Color3& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setLightColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(id < _lightCount,
        "Shaders::PhongGL::setLightColor(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
    setUniform(_lightColorsUniform + id, color);
    return *this;
}

#ifdef MAGNUM_BUILD_DEPRECATED
PhongGL& PhongGL::setLightColor(UnsignedInt id, const Magnum::Color4& color) {
    return setLightColor(id, color.rgb());
}

PhongGL& PhongGL::setLightColor(const Magnum::Color4& color) {
    /* Use the list variant to check the shader really has just one light */
    return setLightColors({color.rgb()});
}
#endif

PhongGL& PhongGL::setLightSpecularColors(const Containers::ArrayView<const Magnum::Color3> colors) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setLightSpecularColors(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_lightCount == colors.size(),
        "Shaders::PhongGL::setLightSpecularColors(): expected" << _lightCount << "items but got" << colors.size(), *this);
    CORRADE_ASSERT(!(_flags >= Flag::NoSpecular),
        "Shaders::PhongGL::setLightSpecularColors(): the shader was created with specular disabled", *this);
    if(_lightCount) setUniform(_lightSpecularColorsUniform, colors);
    return *this;
}

PhongGL& PhongGL::setLightSpecularColors(const std::initializer_list<Magnum::Color3> colors) {
    return setLightSpecularColors(Containers::arrayView(colors));
}

PhongGL& PhongGL::setLightSpecularColor(const UnsignedInt id, const Magnum::Color3& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setLightSpecularColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(id < _lightCount,
        "Shaders::PhongGL::setLightSpecularColor(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
    CORRADE_ASSERT(!(_flags >= Flag::NoSpecular),
        "Shaders::PhongGL::setLightSpecularColor(): the shader was created with specular disabled", *this);
    setUniform(_lightSpecularColorsUniform + id, color);
    return *this;
}

PhongGL& PhongGL::setLightRanges(const Containers::ArrayView<const Float> ranges) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setLightRanges(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_lightCount == ranges.size(),
        "Shaders::PhongGL::setLightRanges(): expected" << _lightCount << "items but got" << ranges.size(), *this);
    if(_lightCount) setUniform(_lightRangesUniform, ranges);
    return *this;
}

PhongGL& PhongGL::setLightRanges(const std::initializer_list<Float> ranges) {
    return setLightRanges(Containers::arrayView(ranges));
}

PhongGL& PhongGL::setLightRange(const UnsignedInt id, const Float range) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setLightRange(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(id < _lightCount,
        "Shaders::PhongGL::setLightRange(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
    setUniform(_lightRangesUniform + id, range);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::setJointMatrices(const Containers::ArrayView<const Matrix4> matrices) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setJointMatrices(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(matrices.size() <= _jointCount,
        "Shaders::PhongGL::setJointMatrices(): expected at most" << _jointCount << "items but got" << matrices.size(), *this);
    if(_jointCount) setUniform(_jointMatricesUniform, matrices);
    return *this;
}

PhongGL& PhongGL::setJointMatrices(const std::initializer_list<Matrix4> matrices) {
    return setJointMatrices(Containers::arrayView(matrices));
}

PhongGL& PhongGL::setJointMatrix(const UnsignedInt id, const Matrix4& matrix) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setJointMatrix(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(id < _jointCount,
        "Shaders::PhongGL::setJointMatrix(): joint ID" << id << "is out of bounds for" << _jointCount << "joints", *this);
    setUniform(_jointMatricesUniform + id, matrix);
    return *this;
}

PhongGL& PhongGL::setPerInstanceJointCount(const UnsignedInt count) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setPerInstanceJointCount(): the shader was created with uniform buffers enabled", *this);
    setUniform(_perInstanceJointCountUniform, count);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::setDrawOffset(const UnsignedInt offset) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::setDrawOffset(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(offset < _drawCount,
        "Shaders::PhongGL::setDrawOffset(): draw offset" << offset << "is out of bounds for" << _drawCount << "draws", *this);
    if(_drawCount > 1) setUniform(_drawOffsetUniform, offset);
    return *this;
}

PhongGL& PhongGL::bindProjectionBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, ProjectionBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindProjectionBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, ProjectionBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindTransformationBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindTransformationBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindDrawBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindDrawBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindTextureTransformationBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TextureTransformationBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindTextureTransformationBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TextureTransformationBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindMaterialBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindMaterialBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindLightBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindLightBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, LightBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindLightBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindLightBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, LightBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindJointBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindJointBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, JointBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindJointBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindJointBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, JointBufferBinding, offset, size);
    return *this;
}
#endif

PhongGL& PhongGL::bindAmbientTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::AmbientTexture,
        "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with ambient texture enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
        "Shaders::PhongGL::bindAmbientTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    texture.bind(AmbientTextureUnit);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::bindAmbientTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags & Flag::AmbientTexture,
        "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with ambient texture enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::PhongGL::bindAmbientTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    texture.bind(AmbientTextureUnit);
    return *this;
}
#endif

PhongGL& PhongGL::bindDiffuseTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::DiffuseTexture,
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with diffuse texture enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    if(_lightCount) texture.bind(DiffuseTextureUnit);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::bindDiffuseTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags & Flag::DiffuseTexture,
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with diffuse texture enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    if(_lightCount) texture.bind(DiffuseTextureUnit);
    return *this;
}
#endif

PhongGL& PhongGL::bindSpecularTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::SpecularTexture,
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with specular texture enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
        "Shaders::PhongGL::bindSpecularTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    if(_lightCount) texture.bind(SpecularTextureUnit);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::bindSpecularTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags & Flag::SpecularTexture,
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with specular texture enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    if(_lightCount) texture.bind(SpecularTextureUnit);
    return *this;
}
#endif

PhongGL& PhongGL::bindNormalTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::NormalTexture,
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with normal texture enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
        "Shaders::PhongGL::bindNormalTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    if(_lightCount) texture.bind(NormalTextureUnit);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::bindNormalTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags & Flag::NormalTexture,
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with normal texture enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    if(_lightCount) texture.bind(NormalTextureUnit);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::bindObjectIdTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags >= Flag::ObjectIdTexture,
        "Shaders::PhongGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
        "Shaders::PhongGL::bindObjectIdTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    texture.bind(ObjectIdTextureUnit);
    return *this;
}

PhongGL& PhongGL::bindObjectIdTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags >= Flag::ObjectIdTexture,
        "Shaders::PhongGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::PhongGL::bindObjectIdTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    texture.bind(ObjectIdTextureUnit);
    return *this;
}
#endif

PhongGL& PhongGL::bindTextures(GL::Texture2D* ambient, GL::Texture2D* diffuse, GL::Texture2D* specular, GL::Texture2D* normal) {
    CORRADE_ASSERT(_flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture),
        "Shaders::PhongGL::bindTextures(): the shader was not created with any textures enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
        "Shaders::PhongGL::bindTextures(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    GL::AbstractTexture::bind(AmbientTextureUnit, {ambient, diffuse, specular, normal});
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL::Configuration& PhongGL::Configuration::setJointCount(UnsignedInt count, UnsignedInt perVertexCount, UnsignedInt secondaryPerVertexCount) {
    CORRADE_ASSERT(perVertexCount <= 4,
        "Shaders::PhongGL::Configuration::setJointCount(): expected at most 4 per-vertex joints, got" << perVertexCount, *this);
    CORRADE_ASSERT(secondaryPerVertexCount <= 4,
        "Shaders::PhongGL::Configuration::setJointCount(): expected at most 4 secondary per-vertex joints, got" << secondaryPerVertexCount, *this);
    CORRADE_ASSERT(!count == (!perVertexCount && !secondaryPerVertexCount),
        "Shaders::PhongGL::Configuration::setJointCount(): either both joint count and (secondary) per-vertex joint count has to be non-zero, or all zero", *this);
    _jointCount = count;
    _perVertexJointCount = perVertexCount;
    _secondaryPerVertexJointCount = secondaryPerVertexCount;
    return *this;
}
#endif

Debug& operator<<(Debug& debug, const PhongGL::Flag value) {
    #ifndef MAGNUM_TARGET_GLES2
    /* Special case coming from the Flags printer. As both flags are a superset
       of ObjectId, printing just one would result in
       `Flag::InstancedObjectId|Flag(0x20000)` in the output. */
    if(value == PhongGL::Flag(UnsignedInt(PhongGL::Flag::InstancedObjectId|PhongGL::Flag::ObjectIdTexture)))
        return debug << PhongGL::Flag::InstancedObjectId << Debug::nospace << "|" << Debug::nospace << PhongGL::Flag::ObjectIdTexture;
    #endif

    debug << "Shaders::PhongGL::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case PhongGL::Flag::v: return debug << "::" #v;
        _c(AmbientTexture)
        _c(DiffuseTexture)
        _c(SpecularTexture)
        _c(NormalTexture)
        _c(Bitangent)
        _c(AlphaMask)
        _c(VertexColor)
        _c(TextureTransformation)
        #ifndef MAGNUM_TARGET_GLES2
        _c(ObjectId)
        _c(InstancedObjectId)
        _c(ObjectIdTexture)
        #endif
        _c(InstancedTransformation)
        _c(InstancedTextureOffset)
        #ifndef MAGNUM_TARGET_GLES2
        _c(UniformBuffers)
        _c(MultiDraw)
        _c(TextureArrays)
        _c(LightCulling)
        #endif
        _c(NoSpecular)
        #ifndef MAGNUM_TARGET_GLES2
        _c(DynamicPerVertexJointCount)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const PhongGL::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::PhongGL::Flags{}", {
        PhongGL::Flag::AmbientTexture,
        PhongGL::Flag::DiffuseTexture,
        PhongGL::Flag::SpecularTexture,
        PhongGL::Flag::NormalTexture,
        PhongGL::Flag::Bitangent,
        PhongGL::Flag::AlphaMask,
        PhongGL::Flag::VertexColor,
        PhongGL::Flag::InstancedTextureOffset, /* Superset of TextureTransformation */
        PhongGL::Flag::TextureTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        /* Both are a superset of ObjectId, meaning printing just one would
           result in `Flag::InstancedObjectId|Flag(0x20000)` in the output. So
           we pass both and let the Flag printer deal with that. */
        PhongGL::Flag(UnsignedInt(PhongGL::Flag::InstancedObjectId|PhongGL::Flag::ObjectIdTexture)),
        PhongGL::Flag::InstancedObjectId, /* Superset of ObjectId */
        PhongGL::Flag::ObjectIdTexture, /* Superset of ObjectId */
        PhongGL::Flag::ObjectId,
        #endif
        PhongGL::Flag::InstancedTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        PhongGL::Flag::MultiDraw, /* Superset of UniformBuffers */
        PhongGL::Flag::UniformBuffers,
        PhongGL::Flag::TextureArrays,
        PhongGL::Flag::LightCulling,
        #endif
        PhongGL::Flag::NoSpecular,
        #ifndef MAGNUM_TARGET_GLES2
        PhongGL::Flag::DynamicPerVertexJointCount,
        #endif
    });
}

}}
