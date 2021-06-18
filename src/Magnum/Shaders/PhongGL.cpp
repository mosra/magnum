/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/Reference.h>
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

namespace {
    enum: Int {
        AmbientTextureUnit = 0,
        DiffuseTextureUnit = 1,
        SpecularTextureUnit = 2,
        NormalTextureUnit = 3
    };

    #ifndef MAGNUM_TARGET_GLES2
    enum: Int {
        ProjectionBufferBinding = 0,
        TransformationBufferBinding = 1,
        DrawBufferBinding = 2,
        TextureTransformationBufferBinding = 3,
        MaterialBufferBinding = 4,
        LightBufferBinding = 5
    };
    #endif
}

PhongGL::PhongGL(const Flags flags, const UnsignedInt lightCount
    #ifndef MAGNUM_TARGET_GLES2
    , const UnsignedInt materialCount, const UnsignedInt drawCount
    #endif
):
    _flags{flags},
    _lightCount{lightCount},
    #ifndef MAGNUM_TARGET_GLES2
    _materialCount{materialCount},
    _drawCount{drawCount},
    #endif
    _lightColorsUniform{_lightPositionsUniform + Int(lightCount)},
    _lightSpecularColorsUniform{_lightPositionsUniform + 2*Int(lightCount)},
    _lightRangesUniform{_lightPositionsUniform + 3*Int(lightCount)}
{
    CORRADE_ASSERT(!(flags & Flag::TextureTransformation) || (flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture)),
        "Shaders::PhongGL: texture transformation enabled but the shader is not textured", );

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags >= Flag::InstancedObjectId) || !(flags & Flag::Bitangent),
        "Shaders::PhongGL: Bitangent attribute binding conflicts with the ObjectId attribute, use a Tangent4 attribute with instanced object ID rendering instead", );
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || materialCount,
        "Shaders::PhongGL: material count can't be zero", );
    CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || drawCount,
        "Shaders::PhongGL: draw count can't be zero", );
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags & Flag::TextureArrays) || (flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture)),
        "Shaders::PhongGL: texture arrays enabled but the shader is not textured", );
    CORRADE_ASSERT(!(flags & Flag::UniformBuffers) || !(flags & Flag::TextureArrays) || flags >= (Flag::TextureArrays|Flag::TextureTransformation),
        "Shaders::PhongGL: texture arrays require texture transformation enabled as well if uniform buffers are used", );
    CORRADE_ASSERT(!(flags & Flag::LightCulling) || (flags & Flag::UniformBuffers),
        "Shaders::PhongGL: light culling requires uniform buffers to be enabled", );
    #endif

    CORRADE_ASSERT(!(flags & Flag::SpecularTexture) || !(flags & (Flag::NoSpecular)),
        "Shaders::PhongGL: specular texture requires the shader to not have specular disabled", );

    #ifndef MAGNUM_TARGET_GLES
    if(flags >= Flag::UniformBuffers)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::uniform_buffer_object);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= Flag::MultiDraw) {
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
    if(flags >= Flag::TextureArrays)
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
    CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || context.isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>(),
        "Shaders::PhongGL: uniform buffers require" << GL::Extensions::ARB::uniform_buffer_object::string(), );
    #endif

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = context.supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    #else
    const GL::Version version = context.supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    GL::Shader vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    GL::Shader frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    #ifndef MAGNUM_TARGET_GLES
    std::string lightInitializer;
    if(!(flags >= Flag::UniformBuffers) && lightCount) {
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
            lightCount*(lightPositionInitializerItem.size() +
                        lightColorInitializerItem.size() +
                        lightRangeInitializerItem.size()));

        lightInitializer.append(lightPositionInitializerPreamble);
        for(std::size_t i = 0; i != lightCount; ++i)
            lightInitializer.append(lightPositionInitializerItem);

        /* Drop the last comma and add a newline at the end */
        lightInitializer[lightInitializer.size() - 2] = '\n';
        lightInitializer.resize(lightInitializer.size() - 1);

        lightInitializer.append(lightColorInitializerPreamble);
        for(std::size_t i = 0; i != lightCount; ++i)
            lightInitializer.append(lightColorInitializerItem);

        /* Drop the last comma and add a newline at the end */
        lightInitializer[lightInitializer.size() - 2] = '\n';
        lightInitializer.resize(lightInitializer.size() - 1);

        lightInitializer.append(lightRangeInitializerPreamble);
        for(std::size_t i = 0; i != lightCount; ++i)
            lightInitializer.append(lightRangeInitializerItem);

        /* Drop the last comma and add a newline at the end */
        lightInitializer[lightInitializer.size() - 2] = '\n';
        lightInitializer.resize(lightInitializer.size() - 1);
    }
    #endif

    vert.addSource(flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture) ? "#define TEXTURED\n" : "")
        .addSource(flags & Flag::NormalTexture ? "#define NORMAL_TEXTURE\n" : "")
        .addSource(flags & Flag::Bitangent ? "#define BITANGENT\n" : "")
        .addSource(flags & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
        .addSource(flags & Flag::TextureTransformation ? "#define TEXTURE_TRANSFORMATION\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(flags & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
        #endif
        .addSource(lightCount ? "#define HAS_LIGHTS\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(flags >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        #endif
        .addSource(flags & Flag::InstancedTransformation ? "#define INSTANCED_TRANSFORMATION\n" : "")
        .addSource(flags >= Flag::InstancedTextureOffset ? "#define INSTANCED_TEXTURE_OFFSET\n" : "");
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= Flag::UniformBuffers) {
        vert.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n",
            drawCount,
            lightCount));
        vert.addSource(flags >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif
    vert.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("Phong.vert"));
    frag.addSource(flags & Flag::AmbientTexture ? "#define AMBIENT_TEXTURE\n" : "")
        .addSource(flags & Flag::DiffuseTexture ? "#define DIFFUSE_TEXTURE\n" : "")
        .addSource(flags & Flag::SpecularTexture ? "#define SPECULAR_TEXTURE\n" : "")
        .addSource(flags & Flag::NormalTexture ? "#define NORMAL_TEXTURE\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(flags & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
        #endif
        .addSource(flags & Flag::Bitangent ? "#define BITANGENT\n" : "")
        .addSource(flags & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
        .addSource(flags & Flag::AlphaMask ? "#define ALPHA_MASK\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(flags & Flag::ObjectId ? "#define OBJECT_ID\n" : "")
        .addSource(flags >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        #endif
        .addSource(flags & Flag::NoSpecular ? "#define NO_SPECULAR\n" : "")
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= Flag::UniformBuffers) {
        frag.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n"
            "#define MATERIAL_COUNT {}\n"
            "#define LIGHT_COUNT {}\n",
            drawCount,
            materialCount,
            lightCount));
        frag.addSource(flags >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "")
            .addSource(flags >= Flag::LightCulling ? "#define LIGHT_CULLING\n" : "");
    } else
    #endif
    {
        frag.addSource(Utility::formatString(
            "#define LIGHT_COUNT {}\n"
            "#define LIGHT_COLORS_LOCATION {}\n"
            "#define LIGHT_SPECULAR_COLORS_LOCATION {}\n"
            "#define LIGHT_RANGES_LOCATION {}\n",
            lightCount,
            _lightPositionsUniform + lightCount,
            _lightPositionsUniform + 2*lightCount,
            _lightPositionsUniform + 3*lightCount));
    }
    #ifndef MAGNUM_TARGET_GLES
    if(!(flags >= Flag::UniformBuffers) && lightCount)
        frag.addSource(std::move(lightInitializer));
    #endif
    frag.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("Phong.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    /* ES3 has this done in the shader directly and doesn't even provide
       bindFragmentDataLocation() */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        if(lightCount)
            bindAttributeLocation(Normal::Location, "normal");
        if((flags & Flag::NormalTexture) && lightCount) {
            bindAttributeLocation(Tangent::Location, "tangent");
            if(flags & Flag::Bitangent)
                bindAttributeLocation(Bitangent::Location, "bitangent");
        }
        if(flags & Flag::VertexColor)
            bindAttributeLocation(Color3::Location, "vertexColor"); /* Color4 is the same */
        if(flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture))
            bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");
        #ifndef MAGNUM_TARGET_GLES2
        if(flags & Flag::ObjectId) {
            bindFragmentDataLocation(ColorOutput, "color");
            bindFragmentDataLocation(ObjectIdOutput, "objectId");
        }
        if(flags >= Flag::InstancedObjectId)
            bindAttributeLocation(ObjectId::Location, "instanceObjectId");
        #endif
        if(flags & Flag::InstancedTransformation)
            bindAttributeLocation(TransformationMatrix::Location, "instancedTransformationMatrix");
        if(flags >= Flag::InstancedTextureOffset)
            bindAttributeLocation(TextureOffset::Location, "instancedTextureOffset");
    }
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::UniformBuffers) {
            if(_drawCount > 1) _drawOffsetUniform = uniformLocation("drawOffset");
        } else
        #endif
        {
            _transformationMatrixUniform = uniformLocation("transformationMatrix");
            if(flags & Flag::TextureTransformation)
                _textureMatrixUniform = uniformLocation("textureMatrix");
            #ifndef MAGNUM_TARGET_GLES2
            if(flags & Flag::TextureArrays)
                _textureLayerUniform = uniformLocation("textureLayer");
            #endif
            _projectionMatrixUniform = uniformLocation("projectionMatrix");
            _ambientColorUniform = uniformLocation("ambientColor");
            if(lightCount) {
                _normalMatrixUniform = uniformLocation("normalMatrix");
                _diffuseColorUniform = uniformLocation("diffuseColor");
                if(!(flags & Flag::NoSpecular)) {
                    _specularColorUniform = uniformLocation("specularColor");
                    _shininessUniform = uniformLocation("shininess");
                }
                if(flags & Flag::NormalTexture)
                    _normalTextureScaleUniform = uniformLocation("normalTextureScale");
                _lightPositionsUniform = uniformLocation("lightPositions");
                _lightColorsUniform = uniformLocation("lightColors");
                if(!(flags & Flag::NoSpecular))
                    _lightSpecularColorsUniform = uniformLocation("lightSpecularColors");
                _lightRangesUniform = uniformLocation("lightRanges");
            }
            if(flags & Flag::AlphaMask) _alphaMaskUniform = uniformLocation("alphaMask");
            #ifndef MAGNUM_TARGET_GLES2
            if(flags & Flag::ObjectId) _objectIdUniform = uniformLocation("objectId");
            #endif
        }
    }

    #ifndef MAGNUM_TARGET_GLES
    if(flags && !context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
    #endif
    {
        if(flags & Flag::AmbientTexture) setUniform(uniformLocation("ambientTexture"), AmbientTextureUnit);
        if(lightCount) {
            if(flags & Flag::DiffuseTexture) setUniform(uniformLocation("diffuseTexture"), DiffuseTextureUnit);
            if(flags & Flag::SpecularTexture) setUniform(uniformLocation("specularTexture"), SpecularTextureUnit);
            if(flags & Flag::NormalTexture) setUniform(uniformLocation("normalTexture"), NormalTextureUnit);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::UniformBuffers) {
            setUniformBlockBinding(uniformBlockIndex("Projection"), ProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Transformation"), TransformationBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"), DrawBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"), MaterialBufferBinding);
            if(flags & Flag::TextureTransformation)
                setUniformBlockBinding(uniformBlockIndex("TextureTransformation"), TextureTransformationBufferBinding);
            if(lightCount)
                setUniformBlockBinding(uniformBlockIndex("Light"), LightBufferBinding);
        }
        #endif
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= Flag::UniformBuffers) {
        /* Draw offset is zero by default */
    } else
    #endif
    {
        /* Default to fully opaque white so we can see the textures */
        if(flags & Flag::AmbientTexture) setAmbientColor(Magnum::Color4{1.0f});
        else setAmbientColor(Magnum::Color4{0.0f});
        setTransformationMatrix(Matrix4{Math::IdentityInit});
        setProjectionMatrix(Matrix4{Math::IdentityInit});
        if(lightCount) {
            setDiffuseColor(Magnum::Color4{1.0f});
            if(!(flags & Flag::NoSpecular)) {
                setSpecularColor(Magnum::Color4{1.0f, 0.0f});
                setShininess(80.0f);
            }
            if(flags & Flag::NormalTexture)
                setNormalTextureScale(1.0f);
            setLightPositions(Containers::Array<Vector4>{DirectInit, lightCount, Vector4{0.0f, 0.0f, 1.0f, 0.0f}});
            Containers::Array<Magnum::Color3> colors{DirectInit, lightCount, Magnum::Color3{1.0f}};
            setLightColors(colors);
            if(!(flags & Flag::NoSpecular))
                setLightSpecularColors(colors);
            setLightRanges(Containers::Array<Float>{DirectInit, lightCount, Constants::inf()});
            /* Light position is zero by default */
            setNormalMatrix(Matrix3x3{Math::IdentityInit});
        }
        if(flags & Flag::TextureTransformation)
            setTextureMatrix(Matrix3{Math::IdentityInit});
        /* Texture layer is zero by default */
        if(flags & Flag::AlphaMask) setAlphaMask(0.5f);
        /* Object ID is zero by default */
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL::PhongGL(const Flags flags, const UnsignedInt lightCount): PhongGL{flags, lightCount, 1, 1} {}
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

Debug& operator<<(Debug& debug, const PhongGL::Flag value) {
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
        _c(InstancedObjectId)
        _c(ObjectId)
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
        PhongGL::Flag::InstancedObjectId, /* Superset of ObjectId */
        PhongGL::Flag::ObjectId,
        #endif
        PhongGL::Flag::InstancedTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        PhongGL::Flag::MultiDraw, /* Superset of UniformBuffers */
        PhongGL::Flag::UniformBuffers,
        PhongGL::Flag::TextureArrays,
        PhongGL::Flag::LightCulling,
        #endif
        PhongGL::Flag::NoSpecular
    });
}

}}
