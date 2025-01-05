/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
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
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Format.h>
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

#ifdef MAGNUM_BUILD_STATIC
static void importShaderResources() {
    CORRADE_RESOURCE_INITIALIZE(MagnumShaders_RESOURCES_GL)
}
#endif

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

    #ifndef CORRADE_NO_ASSERT
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(!(configuration.flags() >= Flag::ShaderStorageBuffers))
    #endif
    {
        CORRADE_ASSERT(configuration.perDrawLightCount() <= configuration.lightCount(),
            "Shaders::PhongGL: per-draw light count expected to not be larger than total count of" << configuration.lightCount() << Debug::nospace << ", got" << configuration.perDrawLightCount(), CompileState{NoCreate});
        #ifndef MAGNUM_TARGET_GLES2
        CORRADE_ASSERT(!configuration.jointCount() == (!configuration.perVertexJointCount() && !configuration.secondaryPerVertexJointCount()),
            "Shaders::PhongGL: joint count can't be zero if per-vertex joint count is non-zero", CompileState{NoCreate});
        CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.materialCount(),
            "Shaders::PhongGL: material count can't be zero", CompileState{NoCreate});
        CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.drawCount(),
            "Shaders::PhongGL: draw count can't be zero", CompileState{NoCreate});
        #endif
    }
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
    CORRADE_ASSERT(!(configuration.flags() & Flag::DynamicPerVertexJointCount) || (configuration.perVertexJointCount() || configuration.secondaryPerVertexJointCount()),
        "Shaders::PhongGL: dynamic per-vertex joint count enabled for zero joints", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::InstancedTransformation) || !configuration.secondaryPerVertexJointCount(),
        "Shaders::PhongGL: TransformationMatrix attribute binding conflicts with the SecondaryJointIds / SecondaryWeights attributes, use a non-instanced rendering with secondary weights instead", CompileState{NoCreate});
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(configuration.flags() >= Flag::ObjectId)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::EXT::gpu_shader4);
    if(configuration.flags() >= Flag::UniformBuffers)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::uniform_buffer_object);
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(configuration.flags() >= Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::shader_storage_buffer_object);
        #else
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GLES310);
        #endif
    }
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
    if(!Utility::Resource::hasGroup("MagnumShadersGL"_s))
        importShaderResources();
    #endif
    Utility::Resource rs("MagnumShadersGL"_s);

    const GL::Context& context = GL::Context::current();

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = context.supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    #else
    const GL::Version version = context.supportedVersion({
        #ifndef MAGNUM_TARGET_WEBGL
        GL::Version::GLES310,
        #endif
        GL::Version::GLES300, GL::Version::GLES200});
    #endif

    PhongGL out{NoInit};
    out._flags = configuration.flags();
    out._lightCount = configuration.lightCount();
    out._perDrawLightCount = configuration.perDrawLightCount();
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

    #ifndef MAGNUM_TARGET_GLES
    /* Initializer for the light color / position / range arrays -- we need a
       list of initializers joined by commas. For GLES we'll simply upload the
       values directly. */
    Containers::String lightInitializer;
    if(!(configuration.flags() >= Flag::UniformBuffers) && configuration.lightCount())
        lightInitializer = Utility::format(
            "#define LIGHT_POSITION_INITIALIZER {}\n"
            "#define LIGHT_COLOR_INITIALIZER {}\n"
            "#define LIGHT_RANGE_INITIALIZER {}\n",
            ("vec4(0.0, 0.0, 1.0, 0.0), "_s*configuration.lightCount()).exceptSuffix(2),
            ("vec3(1.0), "_s*configuration.lightCount()).exceptSuffix(2),
            /* While 1.0/0.0 works on certain drivers such as Mesa and produces
               +inf, on NVidia it produces a NaN. Working around it by making
               the divisor non-zero. The PhongGLTest::renderLights() then
               verifies that both the default and explicitly passed
               Constants::pi() give the same result. */
            ("1.0/0.000000000000000000000001, "_s*configuration.lightCount()).exceptSuffix(2));
    #endif

    GL::Shader vert{version, GL::Shader::Type::Vertex};
    vert.addSource(rs.getString("compatibility.glsl"_s))
        .addSource((configuration.flags() & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture)
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
            ) ? "#define TEXTURED\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::NormalTexture ? "#define NORMAL_TEXTURE\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::Bitangent ? "#define BITANGENT\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::TextureTransformation ? "#define TEXTURE_TRANSFORMATION\n"_s : ""_s)
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n"_s : ""_s)
        #endif
        .addSource(configuration.perDrawLightCount() ? "#define HAS_LIGHTS\n"_s : ""_s)
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n"_s : ""_s)
        #endif
        .addSource(configuration.flags() & Flag::InstancedTransformation ? "#define INSTANCED_TRANSFORMATION\n"_s : ""_s)
        .addSource(configuration.flags() >= Flag::InstancedTextureOffset ? "#define INSTANCED_TEXTURE_OFFSET\n"_s : ""_s);
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.perVertexJointCount() || configuration.secondaryPerVertexJointCount()) {
        #ifndef MAGNUM_TARGET_WEBGL
        /* The _LOCATION are needed only in the non-UBO case if explicit
           uniform location (desktop / ES3.1) is supported, and _INITIALIZER is
           desktop-only, so don't even have this branch on WebGL. OTOH,
           branching on explicit uniform location support and adding just the
           _INITIALIZER if not wouldn't really save much (have to format()
           anyway), so passing them always. */
        if(!(configuration.flags() >= Flag::UniformBuffers)) {
            vert.addSource(Utility::format(
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
        } else
        #endif
        {
            vert.addSource(Utility::format(
                /* SSBOs have an unbounded joints array */
                #ifndef MAGNUM_TARGET_WEBGL
                configuration.flags() >= Flag::ShaderStorageBuffers ?
                    "#define PER_VERTEX_JOINT_COUNT {1}u\n"
                    "#define SECONDARY_PER_VERTEX_JOINT_COUNT {2}u\n" :
                #endif
                    "#define JOINT_COUNT {}\n"
                    "#define PER_VERTEX_JOINT_COUNT {1}u\n"
                    "#define SECONDARY_PER_VERTEX_JOINT_COUNT {2}u\n",
                configuration.jointCount(),
                configuration.perVertexJointCount(),
                configuration.secondaryPerVertexJointCount()));
        }
    }
    if(configuration.flags() >= Flag::DynamicPerVertexJointCount) {
        #ifndef MAGNUM_TARGET_WEBGL
        /* The _LOCATION is needed only if explicit uniform location (desktop /
           ES3.1) is supported, a plain string can be added otherwise. This is
           an immediate uniform also in the UBO / SSBO case. */
        #ifndef MAGNUM_TARGET_GLES
        if(context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
        #else
        if(version >= GL::Version::GLES310)
        #endif
        {
            vert.addSource(Utility::format(
                "#define DYNAMIC_PER_VERTEX_JOINT_COUNT\n"
                "#define PER_VERTEX_JOINT_COUNT_LOCATION {}\n",
                out._perVertexJointCountUniform));
        } else
        #endif
        {
            vert.addSource("#define DYNAMIC_PER_VERTEX_JOINT_COUNT\n"_s);
        }
    }
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers) {
        #ifndef MAGNUM_TARGET_WEBGL
        /* SSBOs have unbounded per-draw arrays so just a plain string can be
           passed */
        if(configuration.flags() >= Flag::ShaderStorageBuffers) {
            vert.addSource(
                "#define UNIFORM_BUFFERS\n"
                "#define SHADER_STORAGE_BUFFERS\n"_s);
        } else
        #endif
        {
            vert.addSource(Utility::format(
                "#define UNIFORM_BUFFERS\n"
                "#define DRAW_COUNT {}\n",
                configuration.drawCount()));
        }
        vert.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n"_s : ""_s);
    }
    #endif
    vert.addSource(rs.getString("generic.glsl"_s))
        .addSource(rs.getString("Phong.vert"_s))
        .submitCompile();

    GL::Shader frag{version, GL::Shader::Type::Fragment};
    frag.addSource(rs.getString("compatibility.glsl"_s))
        .addSource(configuration.flags() & Flag::AmbientTexture ? "#define AMBIENT_TEXTURE\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::DiffuseTexture ? "#define DIFFUSE_TEXTURE\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::SpecularTexture ? "#define SPECULAR_TEXTURE\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::NormalTexture ? "#define NORMAL_TEXTURE\n"_s : ""_s)
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n"_s : ""_s)
        #endif
        .addSource(configuration.flags() & Flag::Bitangent ? "#define BITANGENT\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::DoubleSided ? "#define DOUBLE_SIDED\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::AlphaMask ? "#define ALPHA_MASK\n"_s : ""_s)
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::ObjectId ? "#define OBJECT_ID\n"_s : ""_s)
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n"_s : ""_s)
        .addSource(configuration.flags() >= Flag::ObjectIdTexture ? "#define OBJECT_ID_TEXTURE\n"_s : ""_s)
        #endif
        .addSource(configuration.flags() & Flag::NoSpecular ? "#define NO_SPECULAR\n"_s : ""_s)
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers) {
        frag.addSource(Utility::format(
            #ifndef MAGNUM_TARGET_WEBGL
            /* SSBOs have unbounded per-draw, material and light arrays */
            configuration.flags() >= Flag::ShaderStorageBuffers ?
                "#define UNIFORM_BUFFERS\n"
                "#define SHADER_STORAGE_BUFFERS\n"
                "#define PER_DRAW_LIGHT_COUNT {3}\n" :
            #endif
                "#define UNIFORM_BUFFERS\n"
                "#define DRAW_COUNT {0}\n"
                "#define MATERIAL_COUNT {1}\n"
                "#define LIGHT_COUNT {2}\n"
                "#define PER_DRAW_LIGHT_COUNT {3}\n",
                configuration.drawCount(),
                configuration.materialCount(),
                configuration.lightCount(),
                configuration.perDrawLightCount()));
        frag.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n"_s : ""_s)
            .addSource(configuration.flags() >= Flag::LightCulling ? "#define LIGHT_CULLING\n"_s : ""_s);
    } else
    #endif
    {
        /* The _LOCATION are needed only if explicit uniform location (desktop
           / ES3.1) is supported, thus not even defining them on ES2 & WebGL.
           OTOH, branching on explicit uniform location support and adding just
           the first two wouldn't really save much (have to format() anyway),
           so passing them otherwise always. */
        frag.addSource(Utility::format(
            "#define LIGHT_COUNT {}\n"
            "#define PER_DRAW_LIGHT_COUNT {}\n"
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            "#define LIGHT_COLORS_LOCATION {}\n"
            "#define LIGHT_SPECULAR_COLORS_LOCATION {}\n"
            "#define LIGHT_RANGES_LOCATION {}\n"
            #endif
            ,
            configuration.lightCount(),
            configuration.perDrawLightCount()
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            ,
            out._lightColorsUniform,
            out._lightSpecularColorsUniform,
            out._lightRangesUniform
            #endif
            ));
    }
    #ifndef MAGNUM_TARGET_GLES
    if(!(configuration.flags() >= Flag::UniformBuffers) && configuration.lightCount())
        frag.addSource(Utility::move(lightInitializer));
    #endif
    frag.addSource(rs.getString("generic.glsl"_s))
        .addSource(rs.getString("Phong.frag"_s))
        .submitCompile();

    out.attachShaders({vert, frag});

    /* ES3 has this done in the shader directly and doesn't even provide
       bindFragmentDataLocation() */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        out.bindAttributeLocation(Position::Location, "position"_s);
        if(configuration.perDrawLightCount())
            out.bindAttributeLocation(Normal::Location, "normal"_s);
        if((configuration.flags() & Flag::NormalTexture) && configuration.perDrawLightCount()) {
            out.bindAttributeLocation(Tangent::Location, "tangent"_s);
            if(configuration.flags() & Flag::Bitangent)
                out.bindAttributeLocation(Bitangent::Location, "bitangent"_s);
        }
        if(configuration.flags() & Flag::VertexColor)
            out.bindAttributeLocation(Color3::Location, "vertexColor"_s); /* Color4 is the same */
        if(configuration.flags() & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture)
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
        )
            out.bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates"_s);
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() & Flag::ObjectId) {
            out.bindFragmentDataLocation(ColorOutput, "color"_s);
            out.bindFragmentDataLocation(ObjectIdOutput, "objectId"_s);
        }
        if(configuration.flags() >= Flag::InstancedObjectId)
            out.bindAttributeLocation(ObjectId::Location, "instanceObjectId"_s);
        #endif
        if(configuration.flags() & Flag::InstancedTransformation) {
            out.bindAttributeLocation(TransformationMatrix::Location, "instancedTransformationMatrix"_s);
            if(configuration.perDrawLightCount())
                out.bindAttributeLocation(NormalMatrix::Location, "instancedNormalMatrix"_s);
        }
        if(configuration.flags() >= Flag::InstancedTextureOffset)
            out.bindAttributeLocation(TextureOffset::Location, "instancedTextureOffset"_s);
        #ifndef MAGNUM_TARGET_GLES2
        /* Configuration::setJointCount() checks that jointCount and
           perVertexJointCount / secondaryPerVertexJointCount are either all
           zero or non-zero so we don't need to check for jointCount() here */
        if(configuration.perVertexJointCount()) {
            out.bindAttributeLocation(Weights::Location, "weights"_s);
            out.bindAttributeLocation(JointIds::Location, "jointIds"_s);
        }
        if(configuration.secondaryPerVertexJointCount()) {
            out.bindAttributeLocation(SecondaryWeights::Location, "secondaryWeights"_s);
            out.bindAttributeLocation(SecondaryJointIds::Location, "secondaryJointIds"_s);
        }
        #endif
    }
    #endif

    out.submitLink();

    return CompileState{Utility::move(out), Utility::move(vert), Utility::move(frag)
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , version
        #endif
    };
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

PhongGL::PhongGL(CompileState&& state): PhongGL{static_cast<PhongGL&&>(Utility::move(state))} {
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* When graceful assertions fire from within compile(), we get a NoCreate'd
       CompileState. Exiting makes it possible to test the assert. */
    if(!id()) return;
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag)}));

    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(state._version))
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(state._version < GL::Version::GLES310)
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES2
        if(_flags >= Flag::DynamicPerVertexJointCount)
            _perVertexJointCountUniform = uniformLocation("perVertexJointCount"_s);
        if(_flags >= Flag::UniformBuffers) {
            if(_drawCount > 1
                #ifndef MAGNUM_TARGET_WEBGL
                || flags() >= Flag::ShaderStorageBuffers
                #endif
            ) _drawOffsetUniform = uniformLocation("drawOffset"_s);
        } else
        #endif
        {
            _transformationMatrixUniform = uniformLocation("transformationMatrix"_s);
            if(_flags & Flag::TextureTransformation)
                _textureMatrixUniform = uniformLocation("textureMatrix"_s);
            #ifndef MAGNUM_TARGET_GLES2
            if(_flags & Flag::TextureArrays)
                _textureLayerUniform = uniformLocation("textureLayer"_s);
            #endif
            _projectionMatrixUniform = uniformLocation("projectionMatrix"_s);
            _ambientColorUniform = uniformLocation("ambientColor"_s);
            if(_perDrawLightCount) {
                _normalMatrixUniform = uniformLocation("normalMatrix"_s);
                _diffuseColorUniform = uniformLocation("diffuseColor"_s);
                if(!(_flags & Flag::NoSpecular)) {
                    _specularColorUniform = uniformLocation("specularColor"_s);
                    _shininessUniform = uniformLocation("shininess"_s);
                }
                if(_flags & Flag::NormalTexture)
                    _normalTextureScaleUniform = uniformLocation("normalTextureScale"_s);
                _lightPositionsUniform = uniformLocation("lightPositions"_s);
                _lightColorsUniform = uniformLocation("lightColors"_s);
                if(!(_flags & Flag::NoSpecular))
                    _lightSpecularColorsUniform = uniformLocation("lightSpecularColors"_s);
                _lightRangesUniform = uniformLocation("lightRanges"_s);
            }
            if(_flags & Flag::AlphaMask) _alphaMaskUniform = uniformLocation("alphaMask"_s);
            #ifndef MAGNUM_TARGET_GLES2
            if(_flags & Flag::ObjectId) _objectIdUniform = uniformLocation("objectId"_s);
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            if(_jointCount) {
                _jointMatricesUniform = uniformLocation("jointMatrices"_s);
                _perInstanceJointCountUniform = uniformLocation("perInstanceJointCount"_s);
            }
            #endif
        }
    }

    #ifndef MAGNUM_TARGET_GLES
    if(_flags && !context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(state._version))
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(state._version < GL::Version::GLES310)
    #endif
    {
        if(_flags & Flag::AmbientTexture) setUniform(uniformLocation("ambientTexture"_s), AmbientTextureUnit);
        if(_perDrawLightCount) {
            if(_flags & Flag::DiffuseTexture) setUniform(uniformLocation("diffuseTexture"_s), DiffuseTextureUnit);
            if(_flags & Flag::SpecularTexture) setUniform(uniformLocation("specularTexture"_s), SpecularTextureUnit);
            if(_flags & Flag::NormalTexture) setUniform(uniformLocation("normalTexture"_s), NormalTextureUnit);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(_flags >= Flag::ObjectIdTexture) setUniform(uniformLocation("objectIdTextureData"_s), ObjectIdTextureUnit);
        /* SSBOs have bindings defined in the source always */
        if(_flags >= Flag::UniformBuffers
            #ifndef MAGNUM_TARGET_WEBGL
            && !(_flags >= Flag::ShaderStorageBuffers)
            #endif
        ) {
            setUniformBlockBinding(uniformBlockIndex("Projection"_s), ProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Transformation"_s), TransformationBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"_s), DrawBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"_s), MaterialBufferBinding);
            if(_flags & Flag::TextureTransformation)
                setUniformBlockBinding(uniformBlockIndex("TextureTransformation"_s), TextureTransformationBufferBinding);
            if(_lightCount)
                setUniformBlockBinding(uniformBlockIndex("Light"_s), LightBufferBinding);
            if(_jointCount)
                setUniformBlockBinding(uniformBlockIndex("Joint"_s), JointBufferBinding);
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
    if(_perDrawLightCount) setUniform(_diffuseColorUniform, color);
    return *this;
}

PhongGL& PhongGL::setSpecularColor(const Magnum::Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setSpecularColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(!(_flags >= Flag::NoSpecular),
        "Shaders::PhongGL::setSpecularColor(): the shader was created with specular disabled", *this);
    if(_perDrawLightCount) setUniform(_specularColorUniform, color);
    return *this;
}

PhongGL& PhongGL::setShininess(Float shininess) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setShininess(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(!(_flags >= Flag::NoSpecular),
        "Shaders::PhongGL::setShininess(): the shader was created with specular disabled", *this);
    if(_perDrawLightCount) setUniform(_shininessUniform, shininess);
    return *this;
}

PhongGL& PhongGL::setNormalTextureScale(const Float scale) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::PhongGL::setNormalTextureScale(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & Flag::NormalTexture,
        "Shaders::PhongGL::setNormalTextureScale(): the shader was not created with normal texture enabled", *this);
    if(_perDrawLightCount) setUniform(_normalTextureScaleUniform, scale);
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
    if(_perDrawLightCount) setUniform(_normalMatrixUniform, matrix);
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
        "Shaders::PhongGL::setLightPosition(): light ID" << id << "is out of range for" << _lightCount << "lights", *this);
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
        "Shaders::PhongGL::setLightColor(): light ID" << id << "is out of range for" << _lightCount << "lights", *this);
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
        "Shaders::PhongGL::setLightSpecularColor(): light ID" << id << "is out of range for" << _lightCount << "lights", *this);
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
        "Shaders::PhongGL::setLightRange(): light ID" << id << "is out of range for" << _lightCount << "lights", *this);
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
        "Shaders::PhongGL::setJointMatrix(): joint ID" << id << "is out of range for" << _jointCount << "joints", *this);
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
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(_flags >= Flag::ShaderStorageBuffers || offset < _drawCount,
        "Shaders::PhongGL::setDrawOffset(): draw offset" << offset << "is out of range for" << _drawCount << "draws", *this);
    #else
    CORRADE_ASSERT(offset < _drawCount,
        "Shaders::PhongGL::setDrawOffset(): draw offset" << offset << "is out of range for" << _drawCount << "draws", *this);
    #endif
    if(_drawCount > 1
        #ifndef MAGNUM_TARGET_WEBGL
        || _flags >= Flag::ShaderStorageBuffers
        #endif
    ) setUniform(_drawOffsetUniform, offset);
    return *this;
}

PhongGL& PhongGL::bindProjectionBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, ProjectionBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindProjectionBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, ProjectionBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindTransformationBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TransformationBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindTransformationBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TransformationBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindDrawBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, DrawBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindDrawBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, DrawBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindTextureTransformationBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TextureTransformationBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindTextureTransformationBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::PhongGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TextureTransformationBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindMaterialBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, MaterialBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindMaterialBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, MaterialBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindLightBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindLightBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, LightBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindLightBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindLightBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, LightBufferBinding, offset, size);
    return *this;
}

PhongGL& PhongGL::bindJointBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindJointBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, JointBufferBinding);
    return *this;
}

PhongGL& PhongGL::bindJointBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::PhongGL::bindJointBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, JointBufferBinding, offset, size);
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
    if(_perDrawLightCount) texture.bind(DiffuseTextureUnit);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::bindDiffuseTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags & Flag::DiffuseTexture,
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with diffuse texture enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::PhongGL::bindDiffuseTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    if(_perDrawLightCount) texture.bind(DiffuseTextureUnit);
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
    if(_perDrawLightCount) texture.bind(SpecularTextureUnit);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::bindSpecularTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags & Flag::SpecularTexture,
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with specular texture enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::PhongGL::bindSpecularTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    if(_perDrawLightCount) texture.bind(SpecularTextureUnit);
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
    if(_perDrawLightCount) texture.bind(NormalTextureUnit);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL& PhongGL::bindNormalTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags & Flag::NormalTexture,
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with normal texture enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::PhongGL::bindNormalTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    if(_perDrawLightCount) texture.bind(NormalTextureUnit);
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

PhongGL::Configuration& PhongGL::Configuration::setLightCount(const UnsignedInt count, const UnsignedInt perDrawCount) {
    CORRADE_ASSERT(perDrawCount || !count,
        "Shaders::PhongGL::Configuration::setLightCount(): count has to be zero if per-draw count is zero", *this);
    _lightCount = count;
    _perDrawLightCount = perDrawCount;
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
PhongGL::Configuration& PhongGL::Configuration::setJointCount(UnsignedInt count, UnsignedInt perVertexCount, UnsignedInt secondaryPerVertexCount) {
    CORRADE_ASSERT(perVertexCount <= 4,
        "Shaders::PhongGL::Configuration::setJointCount(): expected at most 4 per-vertex joints, got" << perVertexCount, *this);
    CORRADE_ASSERT(secondaryPerVertexCount <= 4,
        "Shaders::PhongGL::Configuration::setJointCount(): expected at most 4 secondary per-vertex joints, got" << secondaryPerVertexCount, *this);
    CORRADE_ASSERT(perVertexCount || secondaryPerVertexCount || !count,
        "Shaders::PhongGL::Configuration::setJointCount(): count has to be zero if per-vertex joint count is zero", *this);
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
    #ifndef MAGNUM_TARGET_WEBGL
    /* Similarly here, both are a superset of UniformBuffers */
    if(value == PhongGL::Flag(UnsignedInt(PhongGL::Flag::MultiDraw|PhongGL::Flag::ShaderStorageBuffers)))
        return debug << PhongGL::Flag::MultiDraw << Debug::nospace << "|" << Debug::nospace << PhongGL::Flag::ShaderStorageBuffers;
    #endif
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
        _c(DoubleSided)
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
        #ifndef MAGNUM_TARGET_WEBGL
        _c(ShaderStorageBuffers)
        #endif
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

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedInt(value) << Debug::nospace << ")";
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
        PhongGL::Flag::DoubleSided,
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
        #ifndef MAGNUM_TARGET_WEBGL
        /* Both are a superset of UniformBuffers; similarly to ObjectId above
           letting the Flag printer deal with that */
        PhongGL::Flag(UnsignedInt(PhongGL::Flag::MultiDraw|PhongGL::Flag::ShaderStorageBuffers)),
        #endif
        PhongGL::Flag::MultiDraw, /* Superset of UniformBuffers */
        #ifndef MAGNUM_TARGET_WEBGL
        PhongGL::Flag::ShaderStorageBuffers, /* Superset of UniformBuffers */
        #endif
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
