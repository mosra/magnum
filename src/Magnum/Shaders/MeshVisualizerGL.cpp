/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>
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

#include "MeshVisualizerGL.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/TextureArray.h"
#endif

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

using namespace Containers::Literals;

namespace {
    enum: Int {
        /* First four taken by Phong (A/D/S/N) */
        ColorMapTextureUnit = 4,
        ObjectIdTextureUnit = 5 /* shared with Flat and Phong */
    };

    #ifndef MAGNUM_TARGET_GLES2
    enum: Int {
        /* Projection, transformation, texture transformation and joints is
           slots 0, 1, 3, 6 in all shaders so shaders can be switched without
           rebinding everything */
        ProjectionBufferBinding = 0,
        /* Not using the zero binding to avoid conflicts with
           ProjectionBufferBinding from the 3D variant which can likely stay
           bound to the same buffer for the whole time */
        TransformationProjectionBufferBinding = 1,
        TransformationBufferBinding = 1,
        DrawBufferBinding = 2,
        TextureTransformationBufferBinding = 3,
        MaterialBufferBinding = 4,
        /* 5 unused */
        JointBufferBinding = 6,
    };
    #endif
}

namespace Implementation {

void MeshVisualizerGLBase::assertExtensions(const FlagsBase flags) {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef CORRADE_NO_ASSERT
    Int countMutuallyExclusive = 0;
    if(flags & FlagBase::ObjectId) ++countMutuallyExclusive;
    if(flags & FlagBase::VertexId) ++countMutuallyExclusive;
    if(flags & FlagBase::PrimitiveIdFromVertexId) ++countMutuallyExclusive;
    #endif
    CORRADE_ASSERT(countMutuallyExclusive <= 1,
        "Shaders::MeshVisualizerGL: Flag::ObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive", );
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags & FlagBase::TextureTransformation) || flags >= FlagBase::ObjectIdTexture,
        "Shaders::MeshVisualizerGL: texture transformation enabled but the shader is not textured", );
    CORRADE_ASSERT(!(flags & FlagBase::TextureArrays) || flags >= FlagBase::ObjectIdTexture,
        "Shaders::MeshVisualizerGL: texture arrays enabled but the shader is not textured", );
    CORRADE_ASSERT(!(flags & FlagBase::UniformBuffers) || !(flags & FlagBase::TextureArrays) || flags >= (FlagBase::TextureArrays|FlagBase::TextureTransformation),
        "Shaders::MeshVisualizerGL: texture arrays require texture transformation enabled as well if uniform buffers are used", );
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(flags >= FlagBase::UniformBuffers)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::uniform_buffer_object);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= FlagBase::MultiDraw) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::shader_draw_parameters);
        #elif !defined(MAGNUM_TARGET_WEBGL)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ANGLE::multi_draw);
        #else
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::WEBGL::multi_draw);
        #endif
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(flags & FlagBase::Wireframe && !(flags & FlagBase::NoGeometryShader)) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL320);
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::geometry_shader4);
        #elif !defined(MAGNUM_TARGET_WEBGL)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::EXT::geometry_shader);
        #endif
    }
    #else
    if(flags & FlagBase::Wireframe)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::OES::standard_derivatives);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(flags & FlagBase::PrimitiveId && !(flags >= FlagBase::PrimitiveIdFromVertexId)) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL320);
        #else
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GLES320);
        #endif
    }
    #endif

    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShadersGL"))
        importShaderResources();
    #endif
}

GL::Version MeshVisualizerGLBase::setupShaders(GL::Shader& vert, GL::Shader& frag, const Utility::Resource& rs, const FlagsBase flags
    #ifndef MAGNUM_TARGET_GLES2
    , const UnsignedInt
        #ifndef MAGNUM_TARGET_GLES
        dimensions /* used for a uniform initializer, which isn't on GLSL ES */
        #endif
    , const UnsignedInt jointCount, const UnsignedInt perVertexJointCount, const UnsignedInt secondaryPerVertexJointCount, const UnsignedInt materialCount, const UnsignedInt drawCount, const UnsignedInt perInstanceJointCountUniform, const UnsignedInt perVertexJointCountUniform
    #endif
) {
    GL::Context& context = GL::Context::current();

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = context.supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    /* Extended in MeshVisualizerGL3D for TBN visualization */
    CORRADE_INTERNAL_ASSERT(!(flags & FlagBase::Wireframe) || flags & FlagBase::NoGeometryShader || version >= GL::Version::GL320);
    #elif !defined(MAGNUM_TARGET_WEBGL)
    /* ES 3.2 needed for gl_PrimitiveID */
    const GL::Version version = context.supportedVersion({GL::Version::GLES320, GL::Version::GLES310, GL::Version::GLES300, GL::Version::GLES200});
    /* Extended in MeshVisualizerGL3D for TBN visualization */
    CORRADE_INTERNAL_ASSERT(!(flags & FlagBase::Wireframe) || flags & FlagBase::NoGeometryShader || version >= GL::Version::GLES310);
    #else
    const GL::Version version = context.supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    vert.addSource(flags & FlagBase::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(flags >= FlagBase::ObjectIdTexture ? "#define TEXTURED\n" : "")
        .addSource(flags & FlagBase::TextureTransformation ? "#define TEXTURE_TRANSFORMATION\n" : "")
        .addSource(flags & FlagBase::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
        .addSource(flags >= FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        #endif
        .addSource(flags & FlagBase::InstancedTransformation ? "#define INSTANCED_TRANSFORMATION\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(flags >= FlagBase::InstancedTextureOffset ? "#define INSTANCED_TEXTURE_OFFSET\n" : "")
        .addSource(flags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
        .addSource(flags >= FlagBase::PrimitiveIdFromVertexId ? "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" : "")
        #endif
        #ifdef MAGNUM_TARGET_WEBGL
        .addSource("#define SUBSCRIPTING_WORKAROUND\n")
        #elif defined(MAGNUM_TARGET_GLES2)
        .addSource(context.detectedDriver() & GL::Context::DetectedDriver::Angle ?
            "#define SUBSCRIPTING_WORKAROUND\n" : "")
        #endif
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(jointCount) {
        vert.addSource(Utility::formatString(
            "#define JOINT_COUNT {}\n"
            "#define PER_VERTEX_JOINT_COUNT {}u\n"
            "#define SECONDARY_PER_VERTEX_JOINT_COUNT {}u\n"
            #ifndef MAGNUM_TARGET_GLES
            "#define JOINT_MATRIX_INITIALIZER {}\n"
            #endif
            "#define PER_INSTANCE_JOINT_COUNT_LOCATION {}\n",
            jointCount,
            perVertexJointCount,
            secondaryPerVertexJointCount,
            #ifndef MAGNUM_TARGET_GLES
            ((dimensions == 2 ? "mat3(1.0), "_s : "mat4(1.0), "_s)*jointCount).exceptSuffix(2),
            #endif
            perInstanceJointCountUniform));
    }
    if(flags >= FlagBase::DynamicPerVertexJointCount) {
        vert.addSource(Utility::formatString(
            "#define DYNAMIC_PER_VERTEX_JOINT_COUNT\n"
            "#define PER_VERTEX_JOINT_COUNT_LOCATION {}\n",
            perVertexJointCountUniform));
    }
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= FlagBase::UniformBuffers) {
        vert.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n"
            "#define MATERIAL_COUNT {}\n",
            drawCount,
            materialCount));
        vert.addSource(flags >= FlagBase::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif
    frag.addSource(flags & FlagBase::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(flags & FlagBase::ObjectId ? "#define OBJECT_ID\n" : "")
        .addSource(flags >= FlagBase::ObjectIdTexture ? "#define OBJECT_ID_TEXTURE\n" : "")
        .addSource(flags & FlagBase::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
        .addSource(flags >= FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        .addSource(flags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
        .addSource(flags & FlagBase::PrimitiveId ?
            (flags >= FlagBase::PrimitiveIdFromVertexId ?
                "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" :
                "#define PRIMITIVE_ID\n") : "")
        #endif
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= FlagBase::UniformBuffers) {
        frag.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n"
            "#define MATERIAL_COUNT {}\n",
            drawCount,
            materialCount));
        frag.addSource(flags >= FlagBase::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif

    return version;
}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGLBase& MeshVisualizerGLBase::setPerVertexJointCount(const UnsignedInt count, const UnsignedInt secondaryCount) {
    CORRADE_ASSERT(_flags >= FlagBase::DynamicPerVertexJointCount,
        "Shaders::MeshVisualizerGL::setPerVertexJointCount(): the shader was not created with dynamic per-vertex joint count enabled", *this);
    CORRADE_ASSERT(count <= _perVertexJointCount,
        "Shaders::MeshVisualizerGL::setPerVertexJointCount(): expected at most" << _perVertexJointCount << "per-vertex joints, got" << count, *this);
    CORRADE_ASSERT(secondaryCount <= _secondaryPerVertexJointCount,
        "Shaders::MeshVisualizerGL::setPerVertexJointCount(): expected at most" << _secondaryPerVertexJointCount << "secondary per-vertex joints, got" << secondaryCount, *this);
    setUniform(_perVertexJointCountUniform, Vector2ui{count, secondaryCount});
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGLBase& MeshVisualizerGLBase::setTextureMatrix(const Matrix3& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= FlagBase::UniformBuffers),
        "Shaders::MeshVisualizerGL::setTextureMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & FlagBase::TextureTransformation,
        "Shaders::MeshVisualizerGL::setTextureMatrix(): the shader was not created with texture transformation enabled", *this);
    setUniform(_textureMatrixUniform, matrix);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::setTextureLayer(UnsignedInt id) {
    CORRADE_ASSERT(!(_flags >= FlagBase::UniformBuffers),
        "Shaders::MeshVisualizerGL::setTextureLayer(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & FlagBase::TextureArrays,
        "Shaders::MeshVisualizerGL::setTextureLayer(): the shader was not created with texture arrays enabled", *this);
    setUniform(_textureLayerUniform, id);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::setObjectId(UnsignedInt id) {
    CORRADE_ASSERT(!(_flags >= FlagBase::UniformBuffers),
        "Shaders::MeshVisualizerGL::setObjectId(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & FlagBase::ObjectId,
        "Shaders::MeshVisualizerGL::setObjectId(): the shader was not created with object ID enabled", *this);
    setUniform(_objectIdUniform, id);
    return *this;
}
#endif

MeshVisualizerGLBase& MeshVisualizerGLBase::setColor(const Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= FlagBase::UniformBuffers),
        "Shaders::MeshVisualizerGL::setColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(_flags & (FlagBase::Wireframe|FlagBase::ObjectId|FlagBase::VertexId|FlagBase::PrimitiveId),
        "Shaders::MeshVisualizerGL::setColor(): the shader was not created with wireframe or object/vertex/primitive ID enabled", *this);
    #else
    CORRADE_ASSERT(_flags & FlagBase::Wireframe,
        "Shaders::MeshVisualizerGL::setColor(): the shader was not created with wireframe enabled", *this);
    #endif
    setUniform(_colorUniform, color);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::setWireframeColor(const Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= FlagBase::UniformBuffers),
        "Shaders::MeshVisualizerGL::setWireframeColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & FlagBase::Wireframe,
        "Shaders::MeshVisualizerGL::setWireframeColor(): the shader was not created with wireframe enabled", *this);
    setUniform(_wireframeColorUniform, color);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::setWireframeWidth(const Float width) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= FlagBase::UniformBuffers),
        "Shaders::MeshVisualizerGL::setWireframeWidth(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & FlagBase::Wireframe,
        "Shaders::MeshVisualizerGL::setWireframeWidth(): the shader was not created with wireframe enabled", *this);
    setUniform(_wireframeWidthUniform, width);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGLBase& MeshVisualizerGLBase::setColorMapTransformation(const Float offset, const Float scale) {
    CORRADE_ASSERT(!(_flags >= FlagBase::UniformBuffers),
        "Shaders::MeshVisualizerGL::setColorMapTransformation(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & (FlagBase::ObjectId|FlagBase::VertexId|FlagBase::PrimitiveId),
        "Shaders::MeshVisualizerGL::setColorMapTransformation(): the shader was not created with object/vertex/primitive ID enabled", *this);
    setUniform(_colorMapOffsetScaleUniform, Vector2{offset, scale});
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::setPerInstanceJointCount(const UnsignedInt count) {
    CORRADE_ASSERT(!(_flags >= FlagBase::UniformBuffers),
        "Shaders::MeshVisualizerGL::setPerInstanceJointCount(): the shader was created with uniform buffers enabled", *this);
    setUniform(_perInstanceJointCountUniform, count);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGLBase& MeshVisualizerGLBase::setDrawOffset(const UnsignedInt offset) {
    CORRADE_ASSERT(_flags >= FlagBase::UniformBuffers,
        "Shaders::MeshVisualizerGL::setDrawOffset(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(offset < _drawCount,
        "Shaders::MeshVisualizerGL::setDrawOffset(): draw offset" << offset << "is out of bounds for" << _drawCount << "draws", *this);
    if(_drawCount > 1) setUniform(_drawOffsetUniform, offset);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::bindTextureTransformationBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= FlagBase::UniformBuffers,
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & FlagBase::TextureTransformation,
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TextureTransformationBufferBinding);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::bindTextureTransformationBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= FlagBase::UniformBuffers,
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & FlagBase::TextureTransformation,
        "Shaders::MeshVisualizerGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TextureTransformationBufferBinding, offset, size);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::bindMaterialBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= FlagBase::UniformBuffers,
        "Shaders::MeshVisualizerGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::bindMaterialBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= FlagBase::UniformBuffers,
        "Shaders::MeshVisualizerGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding, offset, size);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::bindJointBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= FlagBase::UniformBuffers,
        "Shaders::MeshVisualizerGL::bindJointBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, JointBufferBinding);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::bindJointBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= FlagBase::UniformBuffers,
        "Shaders::MeshVisualizerGL::bindJointBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, JointBufferBinding, offset, size);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGLBase& MeshVisualizerGLBase::bindColorMapTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & (FlagBase::ObjectId|FlagBase::VertexId|FlagBase::PrimitiveId),
        "Shaders::MeshVisualizerGL::bindColorMapTexture(): the shader was not created with object/vertex/primitive ID enabled", *this);
    texture.bind(ColorMapTextureUnit);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::bindObjectIdTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags >= FlagBase::ObjectIdTexture,
        "Shaders::MeshVisualizerGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled", *this);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags & FlagBase::TextureArrays),
        "Shaders::MeshVisualizerGL::bindObjectIdTexture(): the shader was created with texture arrays enabled, use a Texture2DArray instead", *this);
    #endif
    texture.bind(ObjectIdTextureUnit);
    return *this;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::bindObjectIdTexture(GL::Texture2DArray& texture) {
    CORRADE_ASSERT(_flags >= FlagBase::ObjectIdTexture,
        "Shaders::MeshVisualizerGL::bindObjectIdTexture(): the shader was not created with object ID texture enabled", *this);
    CORRADE_ASSERT(_flags & FlagBase::TextureArrays,
        "Shaders::MeshVisualizerGL::bindObjectIdTexture(): the shader was not created with texture arrays enabled, use a Texture2D instead", *this);
    texture.bind(ObjectIdTextureUnit);
    return *this;
}
#endif

}

MeshVisualizerGL2D::CompileState MeshVisualizerGL2D::compile(const Configuration& configuration) {
    const FlagsBase baseFlags = Implementation::MeshVisualizerGLBase::FlagBase(UnsignedInt(configuration.flags()));
    assertExtensions(baseFlags);

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(configuration.flags() & ((Flag::Wireframe|Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId) & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizerGL2D: at least one visualization feature has to be enabled", CompileState{NoCreate});
    #else
    CORRADE_ASSERT(configuration.flags() & (Flag::Wireframe & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizerGL2D: at least Flag::Wireframe has to be enabled", CompileState{NoCreate});
    #endif

    /* Has to be here and not in the base class in order to have it exit the
       constructor when testing for asserts -- GLSL compilation would fail
       otherwise */
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.materialCount(),
        "Shaders::MeshVisualizerGL2D: material count can't be zero", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.drawCount(),
        "Shaders::MeshVisualizerGL2D: draw count can't be zero", CompileState{NoCreate});
    #endif

    /* Has to be here and not in the base class in order to have it exit the
       constructor when testing for asserts -- GLSL compilation would fail
       otherwise */
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() & Flag::DynamicPerVertexJointCount) || configuration.jointCount(),
        "Shaders::MeshVisualizerGL2D: dynamic per-vertex joint count enabled for zero joints", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::InstancedTransformation) || !configuration.secondaryPerVertexJointCount(),
        "Shaders::MeshVisualizerGL2D: TransformationMatrix attribute binding conflicts with the SecondaryJointIds / SecondaryWeights attributes, use a non-instanced rendering with secondary weights instead", CompileState{NoCreate});
    #endif

    MeshVisualizerGL2D out{NoInit};
    out._flags = baseFlags;
    #ifndef MAGNUM_TARGET_GLES2
    out._jointCount = configuration.jointCount();
    out._perVertexJointCount = configuration.perVertexJointCount();
    out._secondaryPerVertexJointCount = configuration.secondaryPerVertexJointCount();
    out._materialCount = configuration.materialCount();
    out._drawCount = configuration.drawCount();
    out._perInstanceJointCountUniform = out._jointMatricesUniform + configuration.jointCount();
    out._perVertexJointCountUniform = configuration.flags() >= Flag::UniformBuffers ?
        2 : out._perInstanceJointCountUniform + 1;
    #endif

    Utility::Resource rs{"MagnumShadersGL"};
    GL::Shader vert{NoCreate};
    GL::Shader frag{NoCreate};
    const GL::Version version = setupShaders(vert, frag, rs, baseFlags
        #ifndef MAGNUM_TARGET_GLES2
        , 2, configuration.jointCount(), configuration.perVertexJointCount(), configuration.secondaryPerVertexJointCount(), configuration.materialCount(), configuration.drawCount(), out._perInstanceJointCountUniform, out._perVertexJointCountUniform
        #endif
    );
    Containers::Optional<GL::Shader> geom;

    vert.addSource("#define TWO_DIMENSIONS\n")
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the shader code */
        .addSource((configuration.flags() & Flag::NoGeometryShader) || !(configuration.flags() & Flag::Wireframe) ?
            "#define NO_GEOMETRY_SHADER\n" : "")
        .addSource(rs.getString("generic.glsl"))
        .addSource(rs.getString("MeshVisualizer.vert"));
    frag
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the shader code */
        .addSource((configuration.flags() & Flag::NoGeometryShader) || !(configuration.flags() & Flag::Wireframe) ?
            "#define NO_GEOMETRY_SHADER\n" : "");
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers)
        frag.addSource("#define TWO_DIMENSIONS\n");
    #endif
    frag.addSource(rs.getString("generic.glsl"))
        .addSource(rs.getString("MeshVisualizer.frag"));


    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(configuration.flags() & Flag::Wireframe && !(configuration.flags() & Flag::NoGeometryShader)) {
        geom = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Geometry);
        (*geom)
            .addSource("#define WIREFRAME_RENDERING\n#define MAX_VERTICES 3\n")
            .addSource(baseFlags >= FlagBase::ObjectIdTexture ? "#define TEXTURED\n" : "")
            .addSource(baseFlags & FlagBase::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
            .addSource(baseFlags & FlagBase::ObjectId ? "#define OBJECT_ID\n" : "")
            .addSource(baseFlags >= FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
            .addSource(baseFlags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
            .addSource(baseFlags & FlagBase::PrimitiveId ?
                (baseFlags >= FlagBase::PrimitiveIdFromVertexId ?
                    "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" :
                    "#define PRIMITIVE_ID\n") : "");
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() >= Flag::UniformBuffers) {
            geom->addSource(Utility::formatString(
                "#define TWO_DIMENSIONS\n"
                "#define UNIFORM_BUFFERS\n"
                "#define DRAW_COUNT {}\n"
                "#define MATERIAL_COUNT {}\n",
                configuration.drawCount(),
                configuration.materialCount()));
            geom->addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
        }
        #endif
        geom->addSource(rs.getString("MeshVisualizer.geom"));
    }
    #else
    static_cast<void>(version);
    #endif

    vert.submitCompile();
    frag.submitCompile();
    if(geom) geom->submitCompile();

    out.attachShaders({vert, frag});
    if(geom) out.attachShader(*geom);

    /* ES3 has this done in the shader directly */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        out.bindAttributeLocation(Position::Location, "position");
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() >= Flag::ObjectIdTexture)
            out.bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");
        if(configuration.flags() >= Flag::InstancedObjectId)
            out.bindAttributeLocation(ObjectId::Location, "instanceObjectId");
        #endif
        if(configuration.flags() & Flag::InstancedTransformation)
            out.bindAttributeLocation(TransformationMatrix::Location, "instancedTransformationMatrix");
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() >= Flag::InstancedTextureOffset)
            out.bindAttributeLocation(TextureOffset::Location, "instancedTextureOffset");
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        #ifndef MAGNUM_TARGET_GLES
        if(!context.isVersionSupported(GL::Version::GL310))
        #endif
        {
            out.bindAttributeLocation(VertexIndex::Location, "vertexIndex");
        }
        #endif
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

    return CompileState{std::move(out), std::move(vert), std::move(frag), geom ? &*geom : nullptr, version};
}

#ifdef MAGNUM_BUILD_DEPRECATED
MeshVisualizerGL2D::CompileState MeshVisualizerGL2D::compile(const Flags flags) {
    return compile(Configuration{}
        .setFlags(flags));
}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL2D::CompileState MeshVisualizerGL2D::compile(const Flags flags, const UnsignedInt materialCount, const UnsignedInt drawCount) {
    return compile(Configuration{}
        .setFlags(flags)
        .setMaterialCount(materialCount)
        .setDrawCount(drawCount));
}
#endif
#endif

MeshVisualizerGL2D::MeshVisualizerGL2D(CompileState&& state): MeshVisualizerGL2D{static_cast<MeshVisualizerGL2D&&>(std::move(state))} {
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* When graceful assertions fire from within compile(), we get a NoCreate'd
       CompileState. Exiting makes it possible to test the assert. */
    if(!id()) return;
    #endif

    if(state._geom.id)
        CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag), GL::Shader(state._geom)}));
    else
        CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag)}));

    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(state._version))
    #endif
    {
        /* This one is used also in the UBO case as it's usually a global
           setting */
        if((flags() & Flag::Wireframe) && !(flags() & Flag::NoGeometryShader))
            _viewportSizeUniform = uniformLocation("viewportSize");

        #ifndef MAGNUM_TARGET_GLES2
        if(flags() >= Flag::DynamicPerVertexJointCount)
            _perVertexJointCountUniform = uniformLocation("perVertexJointCount");
        if(flags() >= Flag::UniformBuffers) {
            if(_drawCount > 1) _drawOffsetUniform = uniformLocation("drawOffset");
        } else
        #endif
        {
            _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
            #ifndef MAGNUM_TARGET_GLES2
            if(flags() & Flag::TextureTransformation)
                _textureMatrixUniform = uniformLocation("textureMatrix");
            if(flags() & Flag::TextureArrays)
                _textureLayerUniform = uniformLocation("textureLayer");
            #endif
            if(flags() & (Flag::Wireframe
                #ifndef MAGNUM_TARGET_GLES2
                |Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
                #endif
            ))
                _colorUniform = uniformLocation("color");
            if(flags() & Flag::Wireframe) {
                _wireframeColorUniform = uniformLocation("wireframeColor");
                _wireframeWidthUniform = uniformLocation("wireframeWidth");
                _smoothnessUniform = uniformLocation("smoothness");
            }
            #ifndef MAGNUM_TARGET_GLES2
            if(flags() & (Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId)) {
                _colorMapOffsetScaleUniform = uniformLocation("colorMapOffsetScale");
            }
            if(flags() & Flag::ObjectId)
                _objectIdUniform = uniformLocation("objectId");
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            if(_jointCount) {
                _jointMatricesUniform = uniformLocation("jointMatrices");
                _perInstanceJointCountUniform = uniformLocation("perInstanceJointCount");
            }
            #endif
        }
    }

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(flags() && !context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(state._version))
    #endif
    {
        if(flags() & (Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId)) {
            setUniform(uniformLocation("colorMapTexture"), ColorMapTextureUnit);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags() >= Flag::ObjectIdTexture)
            setUniform(uniformLocation("objectIdTextureData"), ObjectIdTextureUnit);
        if(flags() >= Flag::UniformBuffers) {
            setUniformBlockBinding(uniformBlockIndex("TransformationProjection"), TransformationProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"), DrawBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"), MaterialBufferBinding);
            if(flags() & Flag::TextureTransformation)
                setUniformBlockBinding(uniformBlockIndex("TextureTransformation"), TextureTransformationBufferBinding);
            if(_jointCount)
                setUniformBlockBinding(uniformBlockIndex("Joint"), JointBufferBinding);
        }
        #endif
    }
    #endif

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_GLES2
    if(flags() >= Flag::DynamicPerVertexJointCount)
        setPerVertexJointCount(_perVertexJointCount, _secondaryPerVertexJointCount);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    if(flags() >= Flag::UniformBuffers) {
        /* Viewport size is zero by default */
        /* Draw offset is zero by default */
    } else
    #endif
    {
        setTransformationProjectionMatrix(Matrix3{Math::IdentityInit});
        if(flags() & (Flag::Wireframe
            #ifndef MAGNUM_TARGET_GLES2
            |Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
            #endif
        ))
            setColor(Color3(1.0f));
        if(flags() & Flag::Wireframe) {
            /* Viewport size is zero by default */
            setWireframeColor(Color3{0.0f});
            setWireframeWidth(1.0f);
            setSmoothness(2.0f);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags() & (Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId))
            setColorMapTransformation(1.0f/512.0f, 1.0f/256.0f);
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        if(_jointCount) {
            setJointMatrices(Containers::Array<Matrix3>{DirectInit, _jointCount, Math::IdentityInit});
            /* Per-instance joint count is zero by default */
        }
        #endif
    }
    #endif
}

MeshVisualizerGL2D::MeshVisualizerGL2D(const Configuration& configuration): MeshVisualizerGL2D{compile(configuration)} {}

#ifdef MAGNUM_BUILD_DEPRECATED
MeshVisualizerGL2D::MeshVisualizerGL2D(const Flags flags): MeshVisualizerGL2D{Configuration{}
    .setFlags(flags)} {}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL2D::MeshVisualizerGL2D(const Flags flags, const UnsignedInt materialCount, const UnsignedInt drawCount): MeshVisualizerGL2D{compile(Configuration{}
    .setFlags(flags)
    .setMaterialCount(materialCount)
    .setDrawCount(drawCount))} {}
#endif
#endif

MeshVisualizerGL2D& MeshVisualizerGL2D::setViewportSize(const Vector2& size) {
    /* Not asserting here, since the relation to wireframe is a bit vague.
       Also it's an ugly hack that should be removed, ideally. */
    if(flags() & Flag::Wireframe && !(flags() & Flag::NoGeometryShader))
        setUniform(_viewportSizeUniform, size);
    return *this;
}

MeshVisualizerGL2D& MeshVisualizerGL2D::setTransformationProjectionMatrix(const Matrix3& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL2D::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

MeshVisualizerGL2D& MeshVisualizerGL2D::setSmoothness(const Float smoothness) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL2D::setSmoothness(): the shader was created with uniform buffers enabled", *this);
    #endif
    /* This is a bit vaguely related but less vague than setViewportSize() so
       asserting in this case. */
    CORRADE_ASSERT(flags() & Flag::Wireframe,
        "Shaders::MeshVisualizerGL2D::setSmoothness(): the shader was not created with wireframe enabled", *this);
    setUniform(_smoothnessUniform, smoothness);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL2D& MeshVisualizerGL2D::setJointMatrices(const Containers::ArrayView<const Matrix3> matrices) {
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL2D::setJointMatrices(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(matrices.size() <= _jointCount,
        "Shaders::MeshVisualizerGL2D::setJointMatrices(): expected at most" << _jointCount << "items but got" << matrices.size(), *this);
    if(_jointCount) setUniform(_jointMatricesUniform, matrices);
    return *this;
}

MeshVisualizerGL2D& MeshVisualizerGL2D::setJointMatrices(const std::initializer_list<Matrix3> matrices) {
    return setJointMatrices(Containers::arrayView(matrices));
}

MeshVisualizerGL2D& MeshVisualizerGL2D::setJointMatrix(const UnsignedInt id, const Matrix3& matrix) {
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL2D::setJointMatrix(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(id < _jointCount,
        "Shaders::MeshVisualizerGL2D::setJointMatrix(): joint ID" << id << "is out of bounds for" << _jointCount << "joints", *this);
    setUniform(_jointMatricesUniform + id, matrix);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL2D& MeshVisualizerGL2D::bindTransformationProjectionBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(flags() >= Flag::UniformBuffers,
        "Shaders::MeshVisualizerGL2D::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding);
    return *this;
}

MeshVisualizerGL2D& MeshVisualizerGL2D::bindTransformationProjectionBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(flags() >= Flag::UniformBuffers,
        "Shaders::MeshVisualizerGL2D::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding, offset, size);
    return *this;
}

MeshVisualizerGL2D& MeshVisualizerGL2D::bindDrawBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(flags() >= Flag::UniformBuffers,
        "Shaders::MeshVisualizerGL2D::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding);
    return *this;
}

MeshVisualizerGL2D& MeshVisualizerGL2D::bindDrawBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(flags() >= Flag::UniformBuffers,
        "Shaders::MeshVisualizerGL2D::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding, offset, size);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL2D::Configuration& MeshVisualizerGL2D::Configuration::setJointCount(UnsignedInt count, UnsignedInt perVertexCount, UnsignedInt secondaryPerVertexCount) {
    CORRADE_ASSERT(perVertexCount <= 4,
        "Shaders::MeshVisualizerGL2D::Configuration::setJointCount(): expected at most 4 per-vertex joints, got" << perVertexCount, *this);
    CORRADE_ASSERT(secondaryPerVertexCount <= 4,
        "Shaders::MeshVisualizerGL2D::Configuration::setJointCount(): expected at most 4 secondary per-vertex joints, got" << secondaryPerVertexCount, *this);
    CORRADE_ASSERT(!count == (!perVertexCount && !secondaryPerVertexCount),
        "Shaders::MeshVisualizerGL2D::Configuration::setJointCount(): either both joint count and (secondary) per-vertex joint count has to be non-zero, or all zero", *this);
    _jointCount = count;
    _perVertexJointCount = perVertexCount;
    _secondaryPerVertexJointCount = secondaryPerVertexCount;
    return *this;
}
#endif

MeshVisualizerGL3D::CompileState MeshVisualizerGL3D::compile(const Configuration& configuration) {
    FlagsBase baseFlags = Implementation::MeshVisualizerGLBase::FlagBase(UnsignedInt(configuration.flags()));
    assertExtensions(baseFlags);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_ASSERT(configuration.flags() & ((Flag::Wireframe|Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection|Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId) & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizerGL3D: at least one visualization feature has to be enabled", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::NoGeometryShader && configuration.flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)),
        "Shaders::MeshVisualizerGL3D: geometry shader has to be enabled when rendering TBN direction", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::BitangentDirection && configuration.flags() & Flag::BitangentFromTangentDirection),
        "Shaders::MeshVisualizerGL3D: Flag::BitangentDirection and Flag::BitangentFromTangentDirection are mutually exclusive", CompileState{NoCreate});
    #elif !defined(MAGNUM_TARGET_GLES2)
    CORRADE_ASSERT(configuration.flags() & ((Flag::Wireframe|Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId) & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizerGL3D: at least one visualization feature has to be enabled", CompileState{NoCreate});
    #else
    CORRADE_ASSERT(configuration.flags() & (Flag::Wireframe & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizerGL3D: at least Flag::Wireframe has to be enabled", CompileState{NoCreate});
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_ASSERT(!(configuration.flags() >= Flag::InstancedObjectId) || !(configuration.flags() & Flag::BitangentDirection),
        "Shaders::MeshVisualizerGL3D: Bitangent attribute binding conflicts with the ObjectId attribute, use a Tangent4 attribute with instanced object ID rendering instead", CompileState{NoCreate});
    #endif

    /* Has to be here and not in the base class in order to have it exit the
       constructor when testing for asserts -- GLSL compilation would fail
       otherwise */
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.materialCount(),
        "Shaders::MeshVisualizerGL3D: material count can't be zero", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.drawCount(),
        "Shaders::MeshVisualizerGL3D: draw count can't be zero", CompileState{NoCreate});
    #endif

    /* Has to be here and not in the base class in order to have it exit the
       constructor when testing for asserts -- GLSL compilation would fail
       otherwise */
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() & Flag::DynamicPerVertexJointCount) || configuration.jointCount(),
        "Shaders::MeshVisualizerGL3D: dynamic per-vertex joint count enabled for zero joints", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() & Flag::InstancedTransformation) || !configuration.secondaryPerVertexJointCount(),
        "Shaders::MeshVisualizerGL3D: TransformationMatrix attribute binding conflicts with the SecondaryJointIds / SecondaryWeights attributes, use a non-instanced rendering with secondary weights instead", CompileState{NoCreate});
    #endif

    MeshVisualizerGL3D out{NoInit};
    out._flags = baseFlags;
    #ifndef MAGNUM_TARGET_GLES2
    out._jointCount = configuration.jointCount();
    out._perVertexJointCount = configuration.perVertexJointCount();
    out._secondaryPerVertexJointCount = configuration.secondaryPerVertexJointCount();
    out._materialCount = configuration.materialCount();
    out._drawCount = configuration.drawCount();
    out._perInstanceJointCountUniform = out._jointMatricesUniform + configuration.jointCount();
    out._perVertexJointCountUniform = configuration.flags() >= Flag::UniformBuffers ?
        2 : out._perInstanceJointCountUniform + 1;
    #endif

    Utility::Resource rs{"MagnumShadersGL"};
    GL::Shader vert{NoCreate};
    GL::Shader frag{NoCreate};
    const GL::Version version = setupShaders(vert, frag, rs, baseFlags
        #ifndef MAGNUM_TARGET_GLES2
        , 3, configuration.jointCount(), configuration.perVertexJointCount(), configuration.secondaryPerVertexJointCount(), configuration.materialCount(), configuration.drawCount(), out._perInstanceJointCountUniform, out._perVertexJointCountUniform
        #endif
    );
    Containers::Optional<GL::Shader> geom;

    /* Expands the check done for wireframe in MeshVisualizerBase with TBN */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_INTERNAL_ASSERT(!(configuration.flags() & (Flag::NormalDirection|Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection)) || version >= GL::Version::GL320);
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_INTERNAL_ASSERT(!(configuration.flags() & (Flag::NormalDirection|Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection)) || version >= GL::Version::GLES310);
    #endif

    vert.addSource("#define THREE_DIMENSIONS\n")
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the vertex shader code */
        .addSource((configuration.flags() & Flag::NoGeometryShader) || !(configuration.flags() & (Flag::Wireframe
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            |Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection|Flag::NormalDirection
            #endif
            )) ? "#define NO_GEOMETRY_SHADER\n" : "")
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        .addSource(configuration.flags() & Flag::TangentDirection ? "#define TANGENT_DIRECTION\n" : "")
        .addSource(configuration.flags() & Flag::BitangentFromTangentDirection ? "#define BITANGENT_FROM_TANGENT_DIRECTION\n" : "")
        .addSource(configuration.flags() & Flag::BitangentDirection ? "#define BITANGENT_DIRECTION\n" : "")
        .addSource(configuration.flags() & Flag::NormalDirection ? "#define NORMAL_DIRECTION\n" : "")
        #endif
        ;
    vert.addSource(rs.getString("generic.glsl"))
        .addSource(rs.getString("MeshVisualizer.vert"));
    frag
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the vertex shader code */
        .addSource((configuration.flags() & Flag::NoGeometryShader) || !(configuration.flags() & (Flag::Wireframe
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            |Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection|Flag::NormalDirection
            #endif
            )) ? "#define NO_GEOMETRY_SHADER\n" : "")
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        .addSource(configuration.flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection) ? "#define TBN_DIRECTION\n" : "")
        #endif
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers)
        frag.addSource("#define THREE_DIMENSIONS\n");
    #endif
    frag.addSource(rs.getString("generic.glsl"))
        .addSource(rs.getString("MeshVisualizer.frag"));

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(configuration.flags() & (Flag::Wireframe|Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection) && !(configuration.flags() & Flag::NoGeometryShader)) {
        Int maxVertices = 0;
        if(configuration.flags() & Flag::Wireframe) maxVertices += 3;
        if(configuration.flags() & Flag::TangentDirection) maxVertices += 3*6;
        if(configuration.flags() & (Flag::BitangentDirection|Flag::BitangentFromTangentDirection))
            maxVertices += 3*6;
        if(configuration.flags() & Flag::NormalDirection) maxVertices += 3*6;

        geom = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Geometry);
        (*geom)
            .addSource(Utility::formatString("#define MAX_VERTICES {}\n", maxVertices))
            .addSource(configuration.flags() & Flag::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
            .addSource(baseFlags >= FlagBase::ObjectIdTexture ? "#define TEXTURED\n" : "")
            .addSource(baseFlags & FlagBase::TextureArrays ? "#define TEXTURE_ARRAYS\n" : "")
            .addSource(baseFlags & FlagBase::ObjectId ? "#define OBJECT_ID\n" : "")
            .addSource(baseFlags >= FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
            .addSource(baseFlags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
            .addSource(baseFlags & FlagBase::PrimitiveId ?
                (baseFlags >= FlagBase::PrimitiveIdFromVertexId ?
                    "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" :
                    "#define PRIMITIVE_ID\n") : "")
            .addSource(configuration.flags() & Flag::TangentDirection ? "#define TANGENT_DIRECTION\n" : "")
            .addSource(configuration.flags() & (Flag::BitangentDirection|Flag::BitangentFromTangentDirection) ? "#define BITANGENT_DIRECTION\n" : "")
            .addSource(configuration.flags() & Flag::NormalDirection ? "#define NORMAL_DIRECTION\n" : "");
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() >= Flag::UniformBuffers) {
            geom->addSource(Utility::formatString(
                "#define THREE_DIMENSIONS\n"
                "#define UNIFORM_BUFFERS\n"
                "#define DRAW_COUNT {}\n"
                "#define MATERIAL_COUNT {}\n",
                configuration.drawCount(),
                configuration.materialCount()));
            geom->addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
        }
        #endif
        geom->addSource(rs.getString("MeshVisualizer.geom"));
    }
    #else
    static_cast<void>(version);
    #endif

    vert.submitCompile();
    frag.submitCompile();
    if(geom) geom->submitCompile();

    out.attachShaders({vert, frag});
    if(geom) out.attachShader(*geom);

    /* ES3 has this done in the shader directly */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        out.bindAttributeLocation(Position::Location, "position");
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() >= Flag::ObjectIdTexture)
            out.bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");
        if(configuration.flags() >= Flag::InstancedObjectId)
            out.bindAttributeLocation(ObjectId::Location, "instanceObjectId");
        #endif
        if(configuration.flags() & Flag::InstancedTransformation) {
            out.bindAttributeLocation(TransformationMatrix::Location, "instancedTransformationMatrix");
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            if(configuration.flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection))
                out.bindAttributeLocation(NormalMatrix::Location, "instancedNormalMatrix");
            #endif
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() >= Flag::InstancedTextureOffset)
            out.bindAttributeLocation(TextureOffset::Location, "instancedTextureOffset");
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(configuration.flags() & Flag::TangentDirection ||
           configuration.flags() & Flag::BitangentFromTangentDirection)
            out.bindAttributeLocation(Tangent4::Location, "tangent");
        if(configuration.flags() & Flag::BitangentDirection)
            out.bindAttributeLocation(Bitangent::Location, "bitangent");
        if(configuration.flags() & Flag::NormalDirection ||
           configuration.flags() & Flag::BitangentFromTangentDirection)
            out.bindAttributeLocation(Normal::Location, "normal");
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        #ifndef MAGNUM_TARGET_GLES
        if(!context.isVersionSupported(GL::Version::GL310))
        #endif
        {
            out.bindAttributeLocation(VertexIndex::Location, "vertexIndex");
        }
        #endif
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

    return CompileState{std::move(out), std::move(vert), std::move(frag), geom ? &*geom : nullptr, version};
}

#ifdef MAGNUM_BUILD_DEPRECATED
MeshVisualizerGL3D::CompileState MeshVisualizerGL3D::compile(const Flags flags) {
    return compile(Configuration{}
        .setFlags(flags));
}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL3D::CompileState MeshVisualizerGL3D::compile(const Flags flags, const UnsignedInt materialCount, const UnsignedInt drawCount) {
    return compile(Configuration{}
        .setFlags(flags)
        .setMaterialCount(materialCount)
        .setDrawCount(drawCount));
}
#endif
#endif

MeshVisualizerGL3D::MeshVisualizerGL3D(CompileState&& state): MeshVisualizerGL3D{static_cast<MeshVisualizerGL3D&&>(std::move(state))} {
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* When graceful assertions fire from within compile(), we get a NoCreate'd
       CompileState. Exiting makes it possible to test the assert. */
    if(!id()) return;
    #endif

    if(state._geom.id)
        CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag), GL::Shader(state._geom)}));
    else
        CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag)}));

    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(state._version))
    #endif
    {
        /* This one is used also in the UBO case as it's usually a global
           setting */
        if(((flags() & Flag::Wireframe) && !(flags() & Flag::NoGeometryShader))
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            || (flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection))
            #endif
        )
            _viewportSizeUniform = uniformLocation("viewportSize");

        #ifndef MAGNUM_TARGET_GLES2
        if(flags() >= Flag::DynamicPerVertexJointCount)
            _perVertexJointCountUniform = uniformLocation("perVertexJointCount");
        if(flags() >= Flag::UniformBuffers) {
            if(_drawCount > 1) _drawOffsetUniform = uniformLocation("drawOffset");
        } else
        #endif
        {
            _transformationMatrixUniform = uniformLocation("transformationMatrix");
            _projectionMatrixUniform = uniformLocation("projectionMatrix");
            #ifndef MAGNUM_TARGET_GLES2
            if(flags() & Flag::TextureTransformation)
                _textureMatrixUniform = uniformLocation("textureMatrix");
            if(flags() & Flag::TextureArrays)
                _textureLayerUniform = uniformLocation("textureLayer");
            #endif
            if(flags() & (Flag::Wireframe
                #ifndef MAGNUM_TARGET_GLES2
                |Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
                #endif
            ))
                _colorUniform = uniformLocation("color");
            if(flags() & Flag::Wireframe) {
                _wireframeColorUniform = uniformLocation("wireframeColor");
                _wireframeWidthUniform = uniformLocation("wireframeWidth");
            }
            if(flags() & (Flag::Wireframe
                #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
                |Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection
                #endif
            )) {
                _smoothnessUniform = uniformLocation("smoothness");
            }
            #ifndef MAGNUM_TARGET_GLES2
            if(flags() & (Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId)) {
                _colorMapOffsetScaleUniform = uniformLocation("colorMapOffsetScale");
            }
            if(flags() & Flag::ObjectId)
                _objectIdUniform = uniformLocation("objectId");
            #endif
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            if(flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)) {
                _normalMatrixUniform = uniformLocation("normalMatrix");
                _lineWidthUniform = uniformLocation("lineWidth");
                _lineLengthUniform = uniformLocation("lineLength");
            }
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            if(_jointCount) {
                _jointMatricesUniform = uniformLocation("jointMatrices");
                _perInstanceJointCountUniform = uniformLocation("perInstanceJointCount");
            }
            #endif
        }
    }

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(flags() && !context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(state._version))
    #endif
    {
        if(flags() & (Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId)) {
            setUniform(uniformLocation("colorMapTexture"), ColorMapTextureUnit);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags() >= Flag::ObjectIdTexture)
            setUniform(uniformLocation("objectIdTextureData"), ObjectIdTextureUnit);
        if(flags() >= Flag::UniformBuffers) {
            setUniformBlockBinding(uniformBlockIndex("Projection"), ProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Transformation"), TransformationBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"), DrawBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"), MaterialBufferBinding);
            if(flags() & Flag::TextureTransformation)
                setUniformBlockBinding(uniformBlockIndex("TextureTransformation"), TextureTransformationBufferBinding);
            if(_jointCount)
                setUniformBlockBinding(uniformBlockIndex("Joint"), JointBufferBinding);
        }
        #endif
    }
    #endif

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_GLES2
    if(flags() >= Flag::DynamicPerVertexJointCount)
        setPerVertexJointCount(_perVertexJointCount, _secondaryPerVertexJointCount);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    if(flags() >= Flag::UniformBuffers) {
        /* Viewport size is zero by default */
        /* Draw offset is zero by default */
    } else
    #endif
    {
        setTransformationMatrix(Matrix4{Math::IdentityInit});
        setProjectionMatrix(Matrix4{Math::IdentityInit});
        if(flags() & (Flag::Wireframe
            #ifndef MAGNUM_TARGET_GLES2
            |Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
            #endif
        ))
            setColor(Color3(1.0f));
        if(flags() & Flag::Wireframe) {
            /* Viewport size is zero by default */
            setWireframeColor(Color3{0.0f});
            setWireframeWidth(1.0f);
        }
        if(flags() & (Flag::Wireframe
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            |Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection
            #endif
        )) {
            setSmoothness(2.0f);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags() & (Flag::ObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId))
            setColorMapTransformation(1.0f/512.0f, 1.0f/256.0f);
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)) {
            setNormalMatrix(Matrix3x3{Math::IdentityInit});
            setLineWidth(1.0f);
            setLineLength(1.0f);
        }
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        if(_jointCount) {
            setJointMatrices(Containers::Array<Matrix4>{DirectInit, _jointCount, Math::IdentityInit});
            /* Per-instance joint count is zero by default */
        }
        #endif
    }
    #endif
}

MeshVisualizerGL3D::MeshVisualizerGL3D(const Configuration& configuration): MeshVisualizerGL3D{compile(configuration)} {}

#ifdef MAGNUM_BUILD_DEPRECATED
MeshVisualizerGL3D::MeshVisualizerGL3D(): MeshVisualizerGL3D{Configuration{}} {}

MeshVisualizerGL3D::MeshVisualizerGL3D(const Flags flags): MeshVisualizerGL3D{Configuration{}
    .setFlags(flags)} {}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL3D::MeshVisualizerGL3D(const Flags flags, const UnsignedInt materialCount, const UnsignedInt drawCount): MeshVisualizerGL3D{compile(Configuration{}
    .setFlags(flags)
    .setMaterialCount(materialCount)
    .setDrawCount(drawCount))} {}
#endif
#endif

MeshVisualizerGL3D& MeshVisualizerGL3D::setTransformationMatrix(const Matrix4& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL3D::setTransformationMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_transformationMatrixUniform, matrix);
    return *this;
}

MeshVisualizerGL3D& MeshVisualizerGL3D::setProjectionMatrix(const Matrix4& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL3D::setProjectionMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_projectionMatrixUniform, matrix);
    return *this;
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
MeshVisualizerGL3D& MeshVisualizerGL3D::setNormalMatrix(const Matrix3x3& matrix) {
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL3D::setNormalMatrix(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection),
        "Shaders::MeshVisualizerGL3D::setNormalMatrix(): the shader was not created with TBN direction enabled", *this);
    setUniform(_normalMatrixUniform, matrix);
    return *this;
}
#endif

MeshVisualizerGL3D& MeshVisualizerGL3D::setViewportSize(const Vector2& size) {
    /* Not asserting here, since the relation to wireframe is a bit vague.
       Also it's an ugly hack that should be removed, ideally. */
    if((flags() & Flag::Wireframe && !(flags() & Flag::NoGeometryShader))
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        || flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)
        #endif
    )
        setUniform(_viewportSizeUniform, size);
    return *this;
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
MeshVisualizerGL3D& MeshVisualizerGL3D::setLineWidth(const Float width) {
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL3D::setLineWidth(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection),
        "Shaders::MeshVisualizerGL3D::setLineWidth(): the shader was not created with TBN direction enabled", *this);
    setUniform(_lineWidthUniform, width);
    return *this;
}

MeshVisualizerGL3D& MeshVisualizerGL3D::setLineLength(const Float length) {
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL3D::setLineLength(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(flags() & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection),
        "Shaders::MeshVisualizerGL3D::setLineLength(): the shader was not created with TBN direction enabled", *this);
    setUniform(_lineLengthUniform, length);
    return *this;
}
#endif

MeshVisualizerGL3D& MeshVisualizerGL3D::setSmoothness(const Float smoothness) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL3D::setSmoothness(): the shader was created with uniform buffers enabled", *this);
    #endif
    #ifndef CORRADE_NO_ASSERT
    /* This is a bit vaguely related but less vague than setViewportSize() so
       asserting in this case. */
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    constexpr Flags allowed = Flag::Wireframe|Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection;
    #else
    constexpr Flags allowed = Flag::Wireframe;
    #endif
    CORRADE_ASSERT(flags() & allowed,
        "Shaders::MeshVisualizerGL3D::setSmoothness(): the shader was not created with wireframe or TBN direction enabled", *this);
    #endif
    setUniform(_smoothnessUniform, smoothness);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL3D& MeshVisualizerGL3D::setJointMatrices(const Containers::ArrayView<const Matrix4> matrices) {
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL3D::setJointMatrices(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(matrices.size() <= _jointCount,
        "Shaders::MeshVisualizerGL3D::setJointMatrices(): expected at most" << _jointCount << "items but got" << matrices.size(), *this);
    if(_jointCount) setUniform(_jointMatricesUniform, matrices);
    return *this;
}

MeshVisualizerGL3D& MeshVisualizerGL3D::setJointMatrices(const std::initializer_list<Matrix4> matrices) {
    return setJointMatrices(Containers::arrayView(matrices));
}

MeshVisualizerGL3D& MeshVisualizerGL3D::setJointMatrix(const UnsignedInt id, const Matrix4& matrix) {
    CORRADE_ASSERT(!(flags() >= Flag::UniformBuffers),
        "Shaders::MeshVisualizerGL3D::setJointMatrix(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(id < _jointCount,
        "Shaders::MeshVisualizerGL3D::setJointMatrix(): joint ID" << id << "is out of bounds for" << _jointCount << "joints", *this);
    setUniform(_jointMatricesUniform + id, matrix);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL3D& MeshVisualizerGL3D::bindProjectionBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(flags() >= Flag::UniformBuffers,
        "Shaders::MeshVisualizerGL3D::bindProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, ProjectionBufferBinding);
    return *this;
}

MeshVisualizerGL3D& MeshVisualizerGL3D::bindProjectionBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(flags() >= Flag::UniformBuffers,
        "Shaders::MeshVisualizerGL3D::bindProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, ProjectionBufferBinding, offset, size);
    return *this;
}

MeshVisualizerGL3D& MeshVisualizerGL3D::bindTransformationBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(flags() >= Flag::UniformBuffers,
        "Shaders::MeshVisualizerGL3D::bindTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationBufferBinding);
    return *this;
}

MeshVisualizerGL3D& MeshVisualizerGL3D::bindTransformationBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(flags() >= Flag::UniformBuffers,
        "Shaders::MeshVisualizerGL3D::bindTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationBufferBinding, offset, size);
    return *this;
}

MeshVisualizerGL3D& MeshVisualizerGL3D::bindDrawBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(flags() >= Flag::UniformBuffers,
        "Shaders::MeshVisualizerGL3D::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding);
    return *this;
}

MeshVisualizerGL3D& MeshVisualizerGL3D::bindDrawBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(flags() >= Flag::UniformBuffers,
        "Shaders::MeshVisualizerGL3D::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding, offset, size);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL3D::Configuration& MeshVisualizerGL3D::Configuration::setJointCount(UnsignedInt count, UnsignedInt perVertexCount, UnsignedInt secondaryPerVertexCount) {
    CORRADE_ASSERT(perVertexCount <= 4,
        "Shaders::MeshVisualizerGL3D::Configuration::setJointCount(): expected at most 4 per-vertex joints, got" << perVertexCount, *this);
    CORRADE_ASSERT(secondaryPerVertexCount <= 4,
        "Shaders::MeshVisualizerGL3D::Configuration::setJointCount(): expected at most 4 secondary per-vertex joints, got" << secondaryPerVertexCount, *this);
    CORRADE_ASSERT(!count == (!perVertexCount && !secondaryPerVertexCount),
        "Shaders::MeshVisualizerGL3D::Configuration::setJointCount(): either both joint count and (secondary) per-vertex joint count has to be non-zero, or all zero", *this);
    _jointCount = count;
    _perVertexJointCount = perVertexCount;
    _secondaryPerVertexJointCount = secondaryPerVertexCount;
    return *this;
}
#endif

Debug& operator<<(Debug& debug, const MeshVisualizerGL2D::Flag value) {
    #ifndef MAGNUM_TARGET_GLES2
    /* Special case coming from the Flags printer. As both flags are a superset
       of ObjectId, printing just one would result in
       `Flag::InstancedObjectId|Flag(0x4000)` in the output. */
    if(value == MeshVisualizerGL2D::Flag(UnsignedInt(MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::ObjectIdTexture)))
        return debug << MeshVisualizerGL2D::Flag::InstancedObjectId << Debug::nospace << "|" << Debug::nospace << MeshVisualizerGL2D::Flag::ObjectIdTexture;
    #endif

    debug << "Shaders::MeshVisualizerGL2D::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case MeshVisualizerGL2D::Flag::v: return debug << "::" #v;
        _c(NoGeometryShader)
        _c(Wireframe)
        #ifndef MAGNUM_TARGET_GLES2
        _c(TextureTransformation)
        _c(ObjectId)
        _c(InstancedObjectId)
        _c(ObjectIdTexture)
        #endif
        _c(InstancedTransformation)
        #ifndef MAGNUM_TARGET_GLES2
        _c(InstancedTextureOffset)
        _c(VertexId)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(PrimitiveId)
        #endif
        _c(PrimitiveIdFromVertexId)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(UniformBuffers)
        _c(MultiDraw)
        _c(TextureArrays)
        _c(DynamicPerVertexJointCount)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MeshVisualizerGL3D::Flag value) {
    #ifndef MAGNUM_TARGET_GLES2
    /* Special case coming from the Flags printer. As both flags are a superset
       of ObjectId, printing just one would result in
       `Flag::InstancedObjectId|Flag(0x4000)` in the output. */
    if(value == MeshVisualizerGL3D::Flag(UnsignedInt(MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::ObjectIdTexture)))
        return debug << MeshVisualizerGL3D::Flag::InstancedObjectId << Debug::nospace << "|" << Debug::nospace << MeshVisualizerGL3D::Flag::ObjectIdTexture;
    #endif

    debug << "Shaders::MeshVisualizerGL3D::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case MeshVisualizerGL3D::Flag::v: return debug << "::" #v;
        _c(NoGeometryShader)
        _c(Wireframe)
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        _c(TangentDirection)
        _c(BitangentFromTangentDirection)
        _c(BitangentDirection)
        _c(NormalDirection)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(TextureTransformation)
        _c(ObjectId)
        _c(InstancedObjectId)
        _c(ObjectIdTexture)
        #endif
        _c(InstancedTransformation)
        #ifndef MAGNUM_TARGET_GLES2
        _c(InstancedTextureOffset)
        _c(VertexId)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(PrimitiveId)
        #endif
        _c(PrimitiveIdFromVertexId)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(UniformBuffers)
        _c(MultiDraw)
        _c(TextureArrays)
        _c(DynamicPerVertexJointCount)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MeshVisualizerGL2D::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::MeshVisualizerGL2D::Flags{}", {
        MeshVisualizerGL2D::Flag::Wireframe,
        /* Wireframe contains this on ES2 and WebGL 1 so it's not reported
           there */
        MeshVisualizerGL2D::Flag::NoGeometryShader,
        #ifndef MAGNUM_TARGET_GLES2
        /* Both are a superset of ObjectId, meaning printing just one would
           result in `Flag::InstancedObjectId|Flag(0x4000)` in the output. So
           we pass both and let the Flag printer deal with that. */
        MeshVisualizerGL2D::Flag(UnsignedInt(MeshVisualizerGL2D::Flag::InstancedObjectId|MeshVisualizerGL2D::Flag::ObjectIdTexture)),
        MeshVisualizerGL2D::Flag::InstancedObjectId, /* Superset of ObjectId */
        MeshVisualizerGL2D::Flag::ObjectIdTexture, /* Superset of ObjectId */
        MeshVisualizerGL2D::Flag::ObjectId,
        #endif
        MeshVisualizerGL2D::Flag::InstancedTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerGL2D::Flag::InstancedTextureOffset, /* Superset of TextureTransformation */
        MeshVisualizerGL2D::Flag::TextureTransformation,
        MeshVisualizerGL2D::Flag::VertexId,
        MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId, /* Superset of PrimitiveId */
        #ifndef MAGNUM_TARGET_WEBGL
        MeshVisualizerGL2D::Flag::PrimitiveId,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerGL2D::Flag::MultiDraw, /* Superset of UniformBuffers */
        MeshVisualizerGL2D::Flag::UniformBuffers,
        MeshVisualizerGL2D::Flag::TextureArrays,
        MeshVisualizerGL2D::Flag::DynamicPerVertexJointCount,
        #endif
        #endif
    });
}

Debug& operator<<(Debug& debug, const MeshVisualizerGL3D::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::MeshVisualizerGL3D::Flags{}", {
        MeshVisualizerGL3D::Flag::Wireframe,
        /* Wireframe contains this on ES2 and WebGL 1 so it's not reported
           there */
        MeshVisualizerGL3D::Flag::NoGeometryShader,
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        MeshVisualizerGL3D::Flag::TangentDirection,
        MeshVisualizerGL3D::Flag::BitangentFromTangentDirection,
        MeshVisualizerGL3D::Flag::BitangentDirection,
        MeshVisualizerGL3D::Flag::NormalDirection,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        /* Both are a superset of ObjectId, meaning printing just one would
           result in `Flag::InstancedObjectId|Flag(0x4000)` in the output. So
           we pass both and let the Flag printer deal with that. */
        MeshVisualizerGL3D::Flag(UnsignedInt(MeshVisualizerGL3D::Flag::InstancedObjectId|MeshVisualizerGL3D::Flag::ObjectIdTexture)),
        MeshVisualizerGL3D::Flag::InstancedObjectId, /* Superset of ObjectId */
        MeshVisualizerGL3D::Flag::ObjectIdTexture, /* Superset of ObjectId */
        MeshVisualizerGL3D::Flag::ObjectId,
        #endif
        MeshVisualizerGL3D::Flag::InstancedTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerGL3D::Flag::InstancedTextureOffset, /* Superset of TextureTransformation */
        MeshVisualizerGL3D::Flag::TextureTransformation,
        MeshVisualizerGL3D::Flag::VertexId,
        MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId, /* Superset of PrimitiveId */
        #ifndef MAGNUM_TARGET_WEBGL
        MeshVisualizerGL3D::Flag::PrimitiveId,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerGL3D::Flag::MultiDraw, /* Superset of UniformBuffers */
        MeshVisualizerGL3D::Flag::UniformBuffers,
        MeshVisualizerGL3D::Flag::TextureArrays,
        MeshVisualizerGL3D::Flag::DynamicPerVertexJointCount,
        #endif
        #endif
    });
}

}}
