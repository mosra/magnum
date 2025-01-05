/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
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

#include "FlatGL.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#ifndef MAGNUM_TARGET_GLES2
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Format.h>

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
        TextureUnit = 0,
        /* 1/2/3 taken by Phong (D/S/N), 4 by MeshVisualizer colormap */
        ObjectIdTextureUnit = 5 /* shared with Phong and MeshVisualizer */
    };

    #ifndef MAGNUM_TARGET_GLES2
    enum: Int {
        /* Texture transformation and joints is slots 3 and 6 in all shaders so
           haders can be switched without rebinding everything. Not using the
           zero binding to avoid conflicts with ProjectionBufferBinding from
           other shaders which can likely stay bound to the same buffer for the
           whole time. */
        TransformationProjectionBufferBinding = 1,
        DrawBufferBinding = 2,
        TextureTransformationBufferBinding = 3,
        MaterialBufferBinding = 4,
        /* 5 unused */
        JointBufferBinding = 6,
    };
    #endif
}

template<UnsignedInt dimensions> typename FlatGL<dimensions>::CompileState FlatGL<dimensions>::compile(const Configuration& configuration) {
    #ifndef CORRADE_NO_ASSERT
    {
        const bool textureTransformationNotEnabledOrTextured = !(configuration.flags() & Flag::TextureTransformation) || configuration.flags() & Flag::Textured
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
            ;
        CORRADE_ASSERT(textureTransformationNotEnabledOrTextured,
            "Shaders::FlatGL: texture transformation enabled but the shader is not textured", CompileState{NoCreate});
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(CORRADE_NO_ASSERT)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!(configuration.flags() >= Flag::ShaderStorageBuffers))
    #endif
    {
        CORRADE_ASSERT(!configuration.jointCount() == (!configuration.perVertexJointCount() && !configuration.secondaryPerVertexJointCount()),
            "Shaders::FlatGL: joint count can't be zero if per-vertex joint count is non-zero", CompileState{NoCreate});
        CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.materialCount(),
            "Shaders::FlatGL: material count can't be zero", CompileState{NoCreate});
        CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.drawCount(),
            "Shaders::FlatGL: draw count can't be zero", CompileState{NoCreate});
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() & Flag::TextureArrays) || configuration.flags() & Flag::Textured || configuration.flags() >= Flag::ObjectIdTexture,
        "Shaders::FlatGL: texture arrays enabled but the shader is not textured", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::UniformBuffers) || !(configuration.flags() & Flag::TextureArrays) || configuration.flags() >= (Flag::TextureArrays|Flag::TextureTransformation),
        "Shaders::FlatGL: texture arrays require texture transformation enabled as well if uniform buffers are used", CompileState{NoCreate});
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() & Flag::DynamicPerVertexJointCount) || (configuration.perVertexJointCount() || configuration.secondaryPerVertexJointCount()),
        "Shaders::FlatGL: dynamic per-vertex joint count enabled for zero joints", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::InstancedTransformation) || !configuration.secondaryPerVertexJointCount(),
        "Shaders::FlatGL: TransformationMatrix attribute binding conflicts with the SecondaryJointIds / SecondaryWeights attributes, use a non-instanced rendering with secondary weights instead", CompileState{NoCreate});
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

    FlatGL<dimensions> out{NoInit};
    out._flags = configuration.flags();
    #ifndef MAGNUM_TARGET_GLES2
    out._jointCount = configuration.jointCount();
    out._perVertexJointCount = configuration.perVertexJointCount();
    out._secondaryPerVertexJointCount = configuration.secondaryPerVertexJointCount();
    out._materialCount = configuration.materialCount();
    out._drawCount = configuration.drawCount();
    out._perInstanceJointCountUniform = out._jointMatricesUniform + configuration.jointCount();
    out._perVertexJointCountUniform = configuration.flags() >= Flag::UniformBuffers ?
        1 : out._perInstanceJointCountUniform + 1;
    #endif

    GL::Shader vert{version, GL::Shader::Type::Vertex};
    vert.addSource(rs.getString("compatibility.glsl"_s))
        .addSource((configuration.flags() & Flag::Textured
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
            ) ? "#define TEXTURED\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::TextureTransformation ? "#define TEXTURE_TRANSFORMATION\n"_s : ""_s)
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n"_s : ""_s)
        #endif
        .addSource(dimensions == 2 ? "#define TWO_DIMENSIONS\n"_s : "#define THREE_DIMENSIONS\n"_s)
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
           desktop only, so don't even have this branch on WebGL. OTOH,
           branching on explicit uniform location support and adding just the
           _INITIALIZER if not wouldn't really save much (have to format()
           anyway), so passing them always. */
        if(!(configuration.flags() >= Flag::UniformBuffers)) {
            vert.addSource(Utility::format(
                "#define JOINT_COUNT {}\n"
                "#define PER_VERTEX_JOINT_COUNT {}u\n"
                "#define SECONDARY_PER_VERTEX_JOINT_COUNT {}u\n"
                #ifndef MAGNUM_TARGET_GLES
                "#define JOINT_MATRIX_INITIALIZER {}\n"
                #endif
                "#define PER_INSTANCE_JOINT_COUNT_LOCATION {}\n",
                configuration.jointCount(),
                configuration.perVertexJointCount(),
                configuration.secondaryPerVertexJointCount(),
                #ifndef MAGNUM_TARGET_GLES
                ((dimensions == 2 ? "mat3(1.0), "_s : "mat4(1.0), "_s)*configuration.jointCount()).exceptSuffix(2),
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
        .addSource(rs.getString("Flat.vert"_s))
        .submitCompile();

    GL::Shader frag{version, GL::Shader::Type::Fragment};
    frag.addSource(rs.getString("compatibility.glsl"_s))
        .addSource(configuration.flags() & Flag::Textured ? "#define TEXTURED\n"_s : ""_s)
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::TextureArrays ? "#define TEXTURE_ARRAYS\n"_s : ""_s)
        #endif
        .addSource(configuration.flags() & Flag::AlphaMask ? "#define ALPHA_MASK\n"_s : ""_s)
        .addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n"_s : ""_s)
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::ObjectId ? "#define OBJECT_ID\n"_s : ""_s)
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n"_s : ""_s)
        .addSource(configuration.flags() >= Flag::ObjectIdTexture ? "#define OBJECT_ID_TEXTURE\n"_s : ""_s)
        #endif
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers) {
        #ifndef MAGNUM_TARGET_WEBGL
        /* SSBOs have unbounded per-draw and material arrays so just a plain
           string can be passed */
        if(configuration.flags() >= Flag::ShaderStorageBuffers) {
            frag.addSource(
                "#define UNIFORM_BUFFERS\n"
                "#define SHADER_STORAGE_BUFFERS\n"_s);
        } else
        #endif
        {
            frag.addSource(Utility::format(
                "#define UNIFORM_BUFFERS\n"
                "#define DRAW_COUNT {}\n"
                "#define MATERIAL_COUNT {}\n",
                configuration.drawCount(),
                configuration.materialCount()));
        }
        frag.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n"_s : ""_s);
    }
    #endif
    frag.addSource(rs.getString("generic.glsl"_s))
        .addSource(rs.getString("Flat.frag"_s))
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
        if(configuration.flags() & Flag::Textured
            #ifndef MAGNUM_TARGET_GLES2
            || configuration.flags() >= Flag::ObjectIdTexture
            #endif
        )
            out.bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates"_s);
        if(configuration.flags() & Flag::VertexColor)
            out.bindAttributeLocation(Color3::Location, "vertexColor"_s); /* Color4 is the same */
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() & Flag::ObjectId) {
            out.bindFragmentDataLocation(ColorOutput, "color"_s);
            out.bindFragmentDataLocation(ObjectIdOutput, "objectId"_s);
        }
        if(configuration.flags() >= Flag::InstancedObjectId)
            out.bindAttributeLocation(ObjectId::Location, "instanceObjectId"_s);
        #endif
        if(configuration.flags() & Flag::InstancedTransformation)
            out.bindAttributeLocation(TransformationMatrix::Location, "instancedTransformationMatrix"_s);
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

#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> typename FlatGL<dimensions>::CompileState FlatGL<dimensions>::compile(const Flags flags) {
    return compile(Configuration{}
        .setFlags(flags));
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> typename FlatGL<dimensions>::CompileState FlatGL<dimensions>::compile(const Flags flags, const UnsignedInt materialCount, const UnsignedInt drawCount) {
    return compile(Configuration{}
        .setFlags(flags)
        .setMaterialCount(materialCount)
        .setDrawCount(drawCount));
}
#endif
#endif

template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(CompileState&& state): FlatGL{static_cast<FlatGL&&>(Utility::move(state))} {
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
            _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix"_s);
            if(_flags & Flag::TextureTransformation)
                _textureMatrixUniform = uniformLocation("textureMatrix"_s);
            #ifndef MAGNUM_TARGET_GLES2
            if(_flags & Flag::TextureArrays)
                _textureLayerUniform = uniformLocation("textureLayer"_s);
            #endif
            _colorUniform = uniformLocation("color"_s);
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
    if(!context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(state._version))
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(state._version < GL::Version::GLES310)
    #endif
    {
        if(_flags & Flag::Textured) setUniform(uniformLocation("textureData"_s), TextureUnit);
        #ifndef MAGNUM_TARGET_GLES2
        if(_flags >= Flag::ObjectIdTexture) setUniform(uniformLocation("objectIdTextureData"_s), ObjectIdTextureUnit);
        /* SSBOs have bindings defined in the source always */
        if(_flags >= Flag::UniformBuffers
            #ifndef MAGNUM_TARGET_WEBGL
            && !(_flags >= Flag::ShaderStorageBuffers)
            #endif
        ) {
            setUniformBlockBinding(uniformBlockIndex("TransformationProjection"_s), TransformationProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"_s), DrawBufferBinding);
            if(_flags & Flag::TextureTransformation)
                setUniformBlockBinding(uniformBlockIndex("TextureTransformation"_s), TextureTransformationBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"_s), MaterialBufferBinding);
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
        setTransformationProjectionMatrix(MatrixTypeFor<dimensions, Float>{Math::IdentityInit});
        if(_flags & Flag::TextureTransformation)
            setTextureMatrix(Matrix3{Math::IdentityInit});
        /* Texture layer is zero by default */
        setColor(Magnum::Color4{1.0f});
        if(_flags & Flag::AlphaMask) setAlphaMask(0.5f);
        /* Object ID is zero by default */
        #ifndef MAGNUM_TARGET_GLES2
        if(_jointCount) {
            setJointMatrices(Containers::Array<MatrixTypeFor<dimensions, Float>>{DirectInit, _jointCount, Math::IdentityInit});
            /* Per-instance joint count is zero by default */
        }
        #endif
    }
    #endif
}

template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(const Configuration& configuration): FlatGL{compile(configuration)} {}

#ifdef MAGNUM_BUILD_DEPRECATED
template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(const Flags flags): FlatGL{compile(Configuration{}
    .setFlags(flags))} {}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(const Flags flags, const UnsignedInt materialCount, const UnsignedInt drawCount): FlatGL{compile(Configuration{}
    .setFlags(flags)
    .setMaterialCount(materialCount)
    .setDrawCount(drawCount))} {}
#endif
#endif

template<UnsignedInt dimensions> FlatGL<dimensions>::FlatGL(NoInitT) {}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setPerVertexJointCount(const UnsignedInt count, const UnsignedInt secondaryCount) {
    CORRADE_ASSERT(_flags >= Flag::DynamicPerVertexJointCount,
        "Shaders::FlatGL::setPerVertexJointCount(): the shader was not created with dynamic per-vertex joint count enabled", *this);
    CORRADE_ASSERT(count <= _perVertexJointCount,
        "Shaders::FlatGL::setPerVertexJointCount(): expected at most" << _perVertexJointCount << "per-vertex joints, got" << count, *this);
    CORRADE_ASSERT(secondaryCount <= _secondaryPerVertexJointCount,
        "Shaders::FlatGL::setPerVertexJointCount(): expected at most" << _secondaryPerVertexJointCount << "secondary per-vertex joints, got" << secondaryCount, *this);
    setUniform(_perVertexJointCountUniform, Vector2ui{count, secondaryCount});
    return *this;
}
#endif

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setTextureMatrix(const Matrix3& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setTextureMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::FlatGL::setTextureMatrix(): the shader was not created with texture transformation enabled", *this);
    setUniform(_textureMatrixUniform, matrix);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setTextureLayer(UnsignedInt id) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setTextureLayer(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::FlatGL::setTextureLayer(): the shader was not created with texture arrays enabled", *this);
    setUniform(_textureLayerUniform, id);
    return *this;
}
#endif

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setColor(const Magnum::Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_colorUniform, color);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setAlphaMask(Float mask) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setAlphaMask(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & Flag::AlphaMask,
        "Shaders::FlatGL::setAlphaMask(): the shader was not created with alpha mask enabled", *this);
    setUniform(_alphaMaskUniform, mask);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setObjectId(UnsignedInt id) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setObjectId(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::ObjectId,
        "Shaders::FlatGL::setObjectId(): the shader was not created with object ID enabled", *this);
    setUniform(_objectIdUniform, id);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setJointMatrices(const Containers::ArrayView<const MatrixTypeFor<dimensions, Float>> matrices) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setJointMatrices(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(matrices.size() <= _jointCount,
        "Shaders::FlatGL::setJointMatrices(): expected at most" << _jointCount << "items but got" << matrices.size(), *this);
    if(_jointCount) setUniform(_jointMatricesUniform, matrices);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setJointMatrices(const std::initializer_list<MatrixTypeFor<dimensions, Float>> matrices) {
    return setJointMatrices(Containers::arrayView(matrices));
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setJointMatrix(const UnsignedInt id, const MatrixTypeFor<dimensions, Float>& matrix) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setJointMatrix(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(id < _jointCount,
        "Shaders::FlatGL::setJointMatrix(): joint ID" << id << "is out of range for" << _jointCount << "joints", *this);
    setUniform(_jointMatricesUniform + id, matrix);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setPerInstanceJointCount(const UnsignedInt count) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::FlatGL::setPerInstanceJointCount(): the shader was created with uniform buffers enabled", *this);
    setUniform(_perInstanceJointCountUniform, count);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::setDrawOffset(const UnsignedInt offset) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::setDrawOffset(): the shader was not created with uniform buffers enabled", *this);
    #ifndef MAGNUM_TARGET_WEBGL
    CORRADE_ASSERT(_flags >= Flag::ShaderStorageBuffers || offset < _drawCount,
        "Shaders::FlatGL::setDrawOffset(): draw offset" << offset << "is out of range for" << _drawCount << "draws", *this);
    #else
    CORRADE_ASSERT(offset < _drawCount,
        "Shaders::FlatGL::setDrawOffset(): draw offset" << offset << "is out of range for" << _drawCount << "draws", *this);
    #endif
    if(_drawCount > 1
        #ifndef MAGNUM_TARGET_WEBGL
        || _flags >= Flag::ShaderStorageBuffers
        #endif
    ) setUniform(_drawOffsetUniform, offset);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindDrawBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, DrawBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindDrawBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, DrawBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTextureTransformationBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TextureTransformationBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTextureTransformationBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::FlatGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, TextureTransformationBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindMaterialBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, MaterialBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindMaterialBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, MaterialBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindJointBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindJointBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, JointBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindJointBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::FlatGL::bindJointBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(
        #ifndef MAGNUM_TARGET_WEBGL
        _flags >= Flag::ShaderStorageBuffers ? GL::Buffer::Target::ShaderStorage :
        #endif
        GL::Buffer::Target::Uniform, JointBufferBinding, offset, size);
    return *this;
}
#endif

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::Textured,
        "Shaders::FlatGL::bindTexture(): the shader was not created with texturing enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
        "Shaders::FlatGL::bindTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    texture.bind(TextureUnit);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags & Flag::Textured,
        "Shaders::FlatGL::bindTexture(): the shader was not created with texturing enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::FlatGL::bindTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    texture.bind(TextureUnit);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindObjectIdTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags >= Flag::ObjectIdTexture,
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & Flag::TextureArrays),
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    texture.bind(ObjectIdTextureUnit);
    return *this;
}

template<UnsignedInt dimensions> FlatGL<dimensions>& FlatGL<dimensions>::bindObjectIdTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags >= Flag::ObjectIdTexture,
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureArrays,
        "Shaders::FlatGL::bindObjectIdTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    texture.bind(ObjectIdTextureUnit);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> typename FlatGL<dimensions>::Configuration& FlatGL<dimensions>::Configuration::setJointCount(UnsignedInt count, UnsignedInt perVertexCount, UnsignedInt secondaryPerVertexCount) {
    CORRADE_ASSERT(perVertexCount <= 4,
        "Shaders::FlatGL::Configuration::setJointCount(): expected at most 4 per-vertex joints, got" << perVertexCount, *this);
    CORRADE_ASSERT(secondaryPerVertexCount <= 4,
        "Shaders::FlatGL::Configuration::setJointCount(): expected at most 4 secondary per-vertex joints, got" << secondaryPerVertexCount, *this);
    CORRADE_ASSERT(perVertexCount || secondaryPerVertexCount || !count,
        "Shaders::FlatGL::Configuration::setJointCount(): count has to be zero if per-vertex joint count is zero", *this);
    _jointCount = count;
    _perVertexJointCount = perVertexCount;
    _secondaryPerVertexJointCount = secondaryPerVertexCount;
    return *this;
}
#endif

template class MAGNUM_SHADERS_EXPORT FlatGL<2>;
template class MAGNUM_SHADERS_EXPORT FlatGL<3>;

namespace Implementation {

Debug& operator<<(Debug& debug, const FlatGLFlag value) {
    #ifndef MAGNUM_TARGET_GLES2
    /* Special case coming from the FlatGLFlags printer. As both flags are a
       superset of ObjectId, printing just one would result in
       `Flag::InstancedObjectId|Flag(0x800)` in the output. */
    if(value == FlatGLFlag(UnsignedShort(FlatGLFlag::InstancedObjectId|FlatGLFlag::ObjectIdTexture)))
        return debug << FlatGLFlag::InstancedObjectId << Debug::nospace << "|" << Debug::nospace << FlatGLFlag::ObjectIdTexture;
    #ifndef MAGNUM_TARGET_WEBGL
    /* Similarly here, both are a superset of UniformBuffers */
    if(value == FlatGLFlag(UnsignedShort(FlatGLFlag::MultiDraw|FlatGLFlag::ShaderStorageBuffers)))
        return debug << FlatGLFlag::MultiDraw << Debug::nospace << "|" << Debug::nospace << FlatGLFlag::ShaderStorageBuffers;
    #endif
    #endif

    debug << "Shaders::FlatGL::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case FlatGLFlag::v: return debug << "::" #v;
        _c(Textured)
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
        #ifndef MAGNUM_TARGET_WEBGL
        _c(ShaderStorageBuffers)
        #endif
        _c(MultiDraw)
        _c(TextureArrays)
        _c(DynamicPerVertexJointCount)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedShort(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const FlatGLFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::FlatGL::Flags{}", {
        FlatGLFlag::Textured,
        FlatGLFlag::AlphaMask,
        FlatGLFlag::VertexColor,
        FlatGLFlag::InstancedTextureOffset, /* Superset of TextureTransformation */
        FlatGLFlag::TextureTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        /* Both are a superset of ObjectId, meaning printing just one would
           result in `Flag::InstancedObjectId|Flag(0x800)` in the output. So we
           pass both and let the FlatGLFlag printer deal with that. */
        FlatGLFlag(UnsignedShort(FlatGLFlag::InstancedObjectId|FlatGLFlag::ObjectIdTexture)),
        FlatGLFlag::InstancedObjectId, /* Superset of ObjectId */
        FlatGLFlag::ObjectIdTexture, /* Superset of ObjectId */
        FlatGLFlag::ObjectId,
        #endif
        FlatGLFlag::InstancedTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        /* Both are a superset of UniformBuffers; similarly to ObjectId above
           letting the Flag printer deal with that */
        FlatGLFlag(UnsignedShort(FlatGLFlag::MultiDraw|FlatGLFlag::ShaderStorageBuffers)),
        #endif
        FlatGLFlag::MultiDraw, /* Superset of UniformBuffers */
        #ifndef MAGNUM_TARGET_WEBGL
        FlatGLFlag::ShaderStorageBuffers, /* Superset of UniformBuffers */
        #endif
        FlatGLFlag::UniformBuffers,
        FlatGLFlag::TextureArrays,
        FlatGLFlag::DynamicPerVertexJointCount,
        #endif
    });
}

}

}}
