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

#include "MeshVisualizerGL.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Reference.h>
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
#endif

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

namespace {
    enum: Int {
        /* First four taken by Phong (A/D/S/N) */
        ColorMapTextureUnit = 4
    };

    #ifndef MAGNUM_TARGET_GLES2
    enum: Int {
        ProjectionBufferBinding = 0,
        /* Not using the zero binding to avoid conflicts with
           ProjectionBufferBinding from the 3D variant which can likely stay
           bound to the same buffer for the whole time */
        TransformationProjectionBufferBinding = 1,
        TransformationBufferBinding = 1,
        DrawBufferBinding = 2,
        /* Binding 3 is commonly used by TextureTransformationBufferBinding,
           leave it reserved */
        MaterialBufferBinding = 4,
    };
    #endif
}

namespace Implementation {

MeshVisualizerGLBase::MeshVisualizerGLBase(FlagsBase flags
    #ifndef MAGNUM_TARGET_GLES2
    , const UnsignedInt materialCount, const UnsignedInt drawCount
    #endif
):
    _flags{flags}
    #ifndef MAGNUM_TARGET_GLES2
    , _materialCount{materialCount},
    _drawCount{drawCount}
    #endif
{
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef CORRADE_NO_ASSERT
    Int countMutuallyExclusive = 0;
    if(flags & FlagBase::InstancedObjectId) ++countMutuallyExclusive;
    if(flags & FlagBase::VertexId) ++countMutuallyExclusive;
    if(flags & FlagBase::PrimitiveIdFromVertexId) ++countMutuallyExclusive;
    #endif
    CORRADE_ASSERT(countMutuallyExclusive <= 1,
        "Shaders::MeshVisualizerGL: Flag::InstancedObjectId, Flag::VertexId and Flag::PrimitiveId are mutually exclusive", );
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
    if(_flags & FlagBase::Wireframe && !(_flags & FlagBase::NoGeometryShader)) {
        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL320);
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::geometry_shader4);
        #elif !defined(MAGNUM_TARGET_WEBGL)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::EXT::geometry_shader);
        #endif
    }
    #else
    if(_flags & FlagBase::Wireframe)
        MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::OES::standard_derivatives);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(_flags & FlagBase::PrimitiveId && !(_flags >= FlagBase::PrimitiveIdFromVertexId)) {
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

GL::Version MeshVisualizerGLBase::setupShaders(GL::Shader& vert, GL::Shader& frag, const Utility::Resource& rs) const {
    GL::Context& context = GL::Context::current();

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = context.supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    /* Extended in MeshVisualizerGL3D for TBN visualization */
    CORRADE_INTERNAL_ASSERT(!(_flags & FlagBase::Wireframe) || _flags & FlagBase::NoGeometryShader || version >= GL::Version::GL320);
    #elif !defined(MAGNUM_TARGET_WEBGL)
    /* ES 3.2 needed for gl_PrimitiveID */
    const GL::Version version = context.supportedVersion({GL::Version::GLES320, GL::Version::GLES310, GL::Version::GLES300, GL::Version::GLES200});
    /* Extended in MeshVisualizerGL3D for TBN visualization */
    CORRADE_INTERNAL_ASSERT(!(_flags & FlagBase::Wireframe) || _flags & FlagBase::NoGeometryShader || version >= GL::Version::GLES310);
    #else
    const GL::Version version = context.supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    vert.addSource(_flags & FlagBase::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(_flags & FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        .addSource(_flags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
        .addSource(_flags >= FlagBase::PrimitiveIdFromVertexId ? "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" : "")
        #endif
        #ifdef MAGNUM_TARGET_WEBGL
        .addSource("#define SUBSCRIPTING_WORKAROUND\n")
        #elif defined(MAGNUM_TARGET_GLES2)
        .addSource(context.detectedDriver() & GL::Context::DetectedDriver::Angle ?
            "#define SUBSCRIPTING_WORKAROUND\n" : "")
        #endif
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(_flags >= FlagBase::UniformBuffers) {
        vert.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n"
            "#define MATERIAL_COUNT {}\n",
            _drawCount,
            _materialCount));
        vert.addSource(_flags >= FlagBase::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif
    frag.addSource(_flags & FlagBase::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(_flags & FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        .addSource(_flags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
        .addSource(_flags & FlagBase::PrimitiveId ?
            (_flags >= FlagBase::PrimitiveIdFromVertexId ?
                "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" :
                "#define PRIMITIVE_ID\n") : "")
        #endif
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(_flags >= FlagBase::UniformBuffers) {
        frag.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n"
            "#define MATERIAL_COUNT {}\n",
            _drawCount,
            _materialCount));
        frag.addSource(_flags >= FlagBase::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif

    return version;
}

MeshVisualizerGLBase& MeshVisualizerGLBase::setColor(const Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= FlagBase::UniformBuffers),
        "Shaders::MeshVisualizerGL::setColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(_flags & (FlagBase::Wireframe|FlagBase::InstancedObjectId|FlagBase::VertexId|FlagBase::PrimitiveId),
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
    CORRADE_ASSERT(_flags & (FlagBase::InstancedObjectId|FlagBase::VertexId|FlagBase::PrimitiveId),
        "Shaders::MeshVisualizerGL::setColorMapTransformation(): the shader was not created with object/vertex/primitive ID enabled", *this);
    setUniform(_colorMapOffsetScaleUniform, Vector2{offset, scale});
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
#endif

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGLBase& MeshVisualizerGLBase::bindColorMapTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & (FlagBase::InstancedObjectId|FlagBase::VertexId|FlagBase::PrimitiveId),
        "Shaders::MeshVisualizerGL::bindColorMapTexture(): the shader was not created with object/vertex/primitive ID enabled", *this);
    texture.bind(ColorMapTextureUnit);
    return *this;
}
#endif

}

MeshVisualizerGL2D::MeshVisualizerGL2D(const Flags flags
    #ifndef MAGNUM_TARGET_GLES2
    , const UnsignedInt materialCount, const UnsignedInt drawCount
    #endif
): Implementation::MeshVisualizerGLBase{Implementation::MeshVisualizerGLBase::FlagBase(UnsignedShort(flags))
    #ifndef MAGNUM_TARGET_GLES2
    , materialCount, drawCount
    #endif
} {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(flags & ((Flag::Wireframe|Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId) & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizerGL2D: at least one visualization feature has to be enabled", );
    #else
    CORRADE_ASSERT(flags & (Flag::Wireframe & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizerGL2D: at least Flag::Wireframe has to be enabled", );
    #endif

    /* Has to be here and not in the base class in order to have it exit the
       constructor when testing for asserts -- GLSL compilation would fail
       otherwise */
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || materialCount,
        "Shaders::MeshVisualizerGL2D: material count can't be zero", );
    CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || drawCount,
        "Shaders::MeshVisualizerGL2D: draw count can't be zero", );
    #endif

    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    #endif

    Utility::Resource rs{"MagnumShadersGL"};
    GL::Shader vert{NoCreate};
    GL::Shader frag{NoCreate};
    const GL::Version version = setupShaders(vert, frag, rs);

    vert.addSource("#define TWO_DIMENSIONS\n")
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the shader code */
        .addSource((flags & Flag::NoGeometryShader) || !(flags & Flag::Wireframe) ?
            "#define NO_GEOMETRY_SHADER\n" : "")
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.vert"));
    frag
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the shader code */
        .addSource((flags & Flag::NoGeometryShader) || !(flags & Flag::Wireframe) ?
            "#define NO_GEOMETRY_SHADER\n" : "");
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= Flag::UniformBuffers)
        frag.addSource("#define TWO_DIMENSIONS\n");
    #endif
    frag.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.frag"));

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    Containers::Optional<GL::Shader> geom;
    if(flags & Flag::Wireframe && !(flags & Flag::NoGeometryShader)) {
        geom = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Geometry);
        (*geom)
            .addSource("#define WIREFRAME_RENDERING\n#define MAX_VERTICES 3\n")
            .addSource(_flags & FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
            .addSource(_flags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
            .addSource(_flags & FlagBase::PrimitiveId ?
                (_flags >= FlagBase::PrimitiveIdFromVertexId ?
                    "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" :
                    "#define PRIMITIVE_ID\n") : "");
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::UniformBuffers) {
            geom->addSource(Utility::formatString(
                "#define TWO_DIMENSIONS\n"
                "#define UNIFORM_BUFFERS\n"
                "#define DRAW_COUNT {}\n"
                "#define MATERIAL_COUNT {}\n",
                _drawCount,
                _materialCount));
            geom->addSource(flags >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
        }
        #endif
        geom->addSource(rs.get("MeshVisualizer.geom"));
    }
    #else
    static_cast<void>(version);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(geom) CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, *geom, frag}));
    else
    #endif
        CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(geom) attachShader(*geom);
    #endif

    /* ES3 has this done in the shader directly */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::InstancedObjectId)
            bindAttributeLocation(ObjectId::Location, "instanceObjectId");
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        #ifndef MAGNUM_TARGET_GLES
        if(!context.isVersionSupported(GL::Version::GL310))
        #endif
        {
            bindAttributeLocation(VertexIndex::Location, "vertexIndex");
        }
        #endif
    }
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        /* This one is used also in the UBO case as it's usually a global
           setting */
        if((flags & Flag::Wireframe) && !(flags & Flag::NoGeometryShader))
            _viewportSizeUniform = uniformLocation("viewportSize");

        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::UniformBuffers) {
            if(_drawCount > 1) _drawOffsetUniform = uniformLocation("drawOffset");
        } else
        #endif
        {
            _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
            if(flags & (Flag::Wireframe
                #ifndef MAGNUM_TARGET_GLES2
                |Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
                #endif
            ))
                _colorUniform = uniformLocation("color");
            if(flags & Flag::Wireframe) {
                _wireframeColorUniform = uniformLocation("wireframeColor");
                _wireframeWidthUniform = uniformLocation("wireframeWidth");
                _smoothnessUniform = uniformLocation("smoothness");
            }
            #ifndef MAGNUM_TARGET_GLES2
            if(flags & (Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId)) {
                _colorMapOffsetScaleUniform = uniformLocation("colorMapOffsetScale");
            }
            #endif
        }
    }

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(flags && !context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
    #endif
    {
        if(flags & (Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId)) {
            setUniform(uniformLocation("colorMapTexture"), ColorMapTextureUnit);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::UniformBuffers) {
            setUniformBlockBinding(uniformBlockIndex("TransformationProjection"), TransformationProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"), DrawBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"), MaterialBufferBinding);
        }
        #endif
    }
    #endif

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= Flag::UniformBuffers) {
        /* Viewport size is zero by default */
        /* Draw offset is zero by default */
    } else
    #endif
    {
        setTransformationProjectionMatrix(Matrix3{Math::IdentityInit});
        if(flags & (Flag::Wireframe
            #ifndef MAGNUM_TARGET_GLES2
            |Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
            #endif
        ))
            setColor(Color3(1.0f));
        if(flags & Flag::Wireframe) {
            /* Viewport size is zero by default */
            setWireframeColor(Color3{0.0f});
            setWireframeWidth(1.0f);
            setSmoothness(2.0f);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags & (Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId))
            setColorMapTransformation(1.0f/512.0f, 1.0f/256.0f);
        #endif
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL2D::MeshVisualizerGL2D(const Flags flags): MeshVisualizerGL2D{flags, 1, 1} {}
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

MeshVisualizerGL3D::MeshVisualizerGL3D(const Flags flags
    #ifndef MAGNUM_TARGET_GLES2
    , const UnsignedInt materialCount, const UnsignedInt drawCount
    #endif
): Implementation::MeshVisualizerGLBase{Implementation::MeshVisualizerGLBase::FlagBase(UnsignedShort(flags))
    #ifndef MAGNUM_TARGET_GLES2
    , materialCount, drawCount
    #endif
} {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_ASSERT(flags & ((Flag::Wireframe|Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection|Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId) & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizerGL3D: at least one visualization feature has to be enabled", );
    CORRADE_ASSERT(!(flags & Flag::NoGeometryShader && flags & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)),
        "Shaders::MeshVisualizerGL3D: geometry shader has to be enabled when rendering TBN direction", );
    CORRADE_ASSERT(!(flags & Flag::BitangentDirection && flags & Flag::BitangentFromTangentDirection),
        "Shaders::MeshVisualizerGL3D: Flag::BitangentDirection and Flag::BitangentFromTangentDirection are mutually exclusive", );
    #elif !defined(MAGNUM_TARGET_GLES2)
    CORRADE_ASSERT(flags & ((Flag::Wireframe|Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId) & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizerGL3D: at least one visualization feature has to be enabled", );
    #else
    CORRADE_ASSERT(flags & (Flag::Wireframe & ~Flag::NoGeometryShader),
        "Shaders::MeshVisualizerGL3D: at least Flag::Wireframe has to be enabled", );
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_ASSERT(!(flags & Flag::InstancedObjectId) || !(flags & Flag::BitangentDirection),
        "Shaders::MeshVisualizerGL3D: Bitangent attribute binding conflicts with the ObjectId attribute, use a Tangent4 attribute with instanced object ID rendering instead", );
    #endif

    /* Has to be here and not in the base class in order to have it exit the
       constructor when testing for asserts -- GLSL compilation would fail
       otherwise */
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || materialCount,
        "Shaders::MeshVisualizerGL3D: material count can't be zero", );
    CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || drawCount,
        "Shaders::MeshVisualizerGL3D: draw count can't be zero", );
    #endif

    #ifndef MAGNUM_TARGET_GLES
    const GL::Context& context = GL::Context::current();
    #endif

    Utility::Resource rs{"MagnumShadersGL"};
    GL::Shader vert{NoCreate};
    GL::Shader frag{NoCreate};
    const GL::Version version = setupShaders(vert, frag, rs);

    /* Expands the check done for wireframe in MeshVisualizerBase with TBN */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_INTERNAL_ASSERT(!(flags & (Flag::NormalDirection|Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection)) || version >= GL::Version::GL320);
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_INTERNAL_ASSERT(!(flags & (Flag::NormalDirection|Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection)) || version >= GL::Version::GLES310);
    #endif

    vert.addSource("#define THREE_DIMENSIONS\n")
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the vertex shader code */
        .addSource((flags & Flag::NoGeometryShader) || !(flags & (Flag::Wireframe
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            |Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection|Flag::NormalDirection
            #endif
            )) ? "#define NO_GEOMETRY_SHADER\n" : "")
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        .addSource(flags & Flag::TangentDirection ? "#define TANGENT_DIRECTION\n" : "")
        .addSource(flags & Flag::BitangentFromTangentDirection ? "#define BITANGENT_FROM_TANGENT_DIRECTION\n" : "")
        .addSource(flags & Flag::BitangentDirection ? "#define BITANGENT_DIRECTION\n" : "")
        .addSource(flags & Flag::NormalDirection ? "#define NORMAL_DIRECTION\n" : "")
        #endif
        ;
    vert.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.vert"));
    frag
        /* Pass NO_GEOMETRY_SHADER not only when NoGeometryShader but also when
           nothing actually needs it, as that makes checks much simpler in
           the vertex shader code */
        .addSource((flags & Flag::NoGeometryShader) || !(flags & (Flag::Wireframe
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            |Flag::TangentDirection|Flag::BitangentDirection|Flag::BitangentFromTangentDirection|Flag::NormalDirection
            #endif
            )) ? "#define NO_GEOMETRY_SHADER\n" : "")
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        .addSource(flags & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection) ? "#define TBN_DIRECTION\n" : "")
        #endif
        ;
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= Flag::UniformBuffers)
        frag.addSource("#define THREE_DIMENSIONS\n");
    #endif
    frag.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("MeshVisualizer.frag"));

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    Containers::Optional<GL::Shader> geom;
    if(flags & (Flag::Wireframe|Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection) && !(flags & Flag::NoGeometryShader)) {
        Int maxVertices = 0;
        if(flags & Flag::Wireframe) maxVertices += 3;
        if(flags & Flag::TangentDirection) maxVertices += 3*6;
        if(flags & (Flag::BitangentDirection|Flag::BitangentFromTangentDirection))
            maxVertices += 3*6;
        if(flags & Flag::NormalDirection) maxVertices += 3*6;

        geom = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Geometry);
        (*geom)
            .addSource(Utility::formatString("#define MAX_VERTICES {}\n", maxVertices))
            .addSource(flags & Flag::Wireframe ? "#define WIREFRAME_RENDERING\n" : "")
            .addSource(_flags & FlagBase::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
            .addSource(_flags & FlagBase::VertexId ? "#define VERTEX_ID\n" : "")
            .addSource(_flags & FlagBase::PrimitiveId ?
                (_flags >= FlagBase::PrimitiveIdFromVertexId ?
                    "#define PRIMITIVE_ID_FROM_VERTEX_ID\n" :
                    "#define PRIMITIVE_ID\n") : "")
            .addSource(flags & Flag::TangentDirection ? "#define TANGENT_DIRECTION\n" : "")
            .addSource(flags & (Flag::BitangentDirection|Flag::BitangentFromTangentDirection) ? "#define BITANGENT_DIRECTION\n" : "")
            .addSource(flags & Flag::NormalDirection ? "#define NORMAL_DIRECTION\n" : "");
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::UniformBuffers) {
            geom->addSource(Utility::formatString(
                "#define THREE_DIMENSIONS\n"
                "#define UNIFORM_BUFFERS\n"
                "#define DRAW_COUNT {}\n"
                "#define MATERIAL_COUNT {}\n",
                _drawCount,
                _materialCount));
            geom->addSource(flags >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
        }
        #endif
        geom->addSource(rs.get("MeshVisualizer.geom"));
    }
    #else
    static_cast<void>(version);
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(geom) CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, *geom, frag}));
    else
    #endif
        CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(geom) attachShader(*geom);
    #endif

    /* ES3 has this done in the shader directly */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::InstancedObjectId)
            bindAttributeLocation(ObjectId::Location, "instanceObjectId");
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(flags & Flag::TangentDirection ||
           flags & Flag::BitangentFromTangentDirection)
            bindAttributeLocation(Tangent4::Location, "tangent");
        if(flags & Flag::BitangentDirection)
            bindAttributeLocation(Bitangent::Location, "bitangent");
        if(flags & Flag::NormalDirection ||
           flags & Flag::BitangentFromTangentDirection)
            bindAttributeLocation(Normal::Location, "normal");
        #endif

        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        #ifndef MAGNUM_TARGET_GLES
        if(!context.isVersionSupported(GL::Version::GL310))
        #endif
        {
            bindAttributeLocation(VertexIndex::Location, "vertexIndex");
        }
        #endif
    }
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        /* This one is used also in the UBO case as it's usually a global
           setting */
        if(((flags & Flag::Wireframe) && !(flags & Flag::NoGeometryShader))
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            || (flags & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection))
            #endif
        )
            _viewportSizeUniform = uniformLocation("viewportSize");

        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::UniformBuffers) {
            if(_drawCount > 1) _drawOffsetUniform = uniformLocation("drawOffset");
        } else
        #endif
        {
            _transformationMatrixUniform = uniformLocation("transformationMatrix");
            _projectionMatrixUniform = uniformLocation("projectionMatrix");
            if(flags & (Flag::Wireframe
                #ifndef MAGNUM_TARGET_GLES2
                |Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
                #endif
            ))
                _colorUniform = uniformLocation("color");
            if(flags & Flag::Wireframe) {
                _wireframeColorUniform = uniformLocation("wireframeColor");
                _wireframeWidthUniform = uniformLocation("wireframeWidth");
            }
            if(flags & (Flag::Wireframe
                #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
                |Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection
                #endif
            )) {
                _smoothnessUniform = uniformLocation("smoothness");
            }
            #ifndef MAGNUM_TARGET_GLES2
            if(flags & (Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId)) {
                _colorMapOffsetScaleUniform = uniformLocation("colorMapOffsetScale");
            }
            #endif
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            if(flags & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)) {
                _normalMatrixUniform = uniformLocation("normalMatrix");
                _lineWidthUniform = uniformLocation("lineWidth");
                _lineLengthUniform = uniformLocation("lineLength");
            }
            #endif
        }
    }

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(flags && !context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
    #endif
    {
        if(flags & (Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId)) {
            setUniform(uniformLocation("colorMapTexture"), ColorMapTextureUnit);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::UniformBuffers) {
            setUniformBlockBinding(uniformBlockIndex("Projection"), ProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Transformation"), TransformationBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"), DrawBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"), MaterialBufferBinding);
        }
        #endif
    }
    #endif

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= Flag::UniformBuffers) {
        /* Viewport size is zero by default */
        /* Draw offset is zero by default */
    } else
    #endif
    {
        setTransformationMatrix(Matrix4{Math::IdentityInit});
        setProjectionMatrix(Matrix4{Math::IdentityInit});
        if(flags & (Flag::Wireframe
            #ifndef MAGNUM_TARGET_GLES2
            |Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId
            #endif
        ))
            setColor(Color3(1.0f));
        if(flags & Flag::Wireframe) {
            /* Viewport size is zero by default */
            setWireframeColor(Color3{0.0f});
            setWireframeWidth(1.0f);
        }
        if(flags & (Flag::Wireframe
            #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
            |Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection
            #endif
        )) {
            setSmoothness(2.0f);
        }
        #ifndef MAGNUM_TARGET_GLES2
        if(flags & (Flag::InstancedObjectId|Flag::VertexId|Flag::PrimitiveIdFromVertexId))
            setColorMapTransformation(1.0f/512.0f, 1.0f/256.0f);
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(flags & (Flag::TangentDirection|Flag::BitangentFromTangentDirection|Flag::BitangentDirection|Flag::NormalDirection)) {
            setNormalMatrix(Matrix3x3{Math::IdentityInit});
            setLineWidth(1.0f);
            setLineLength(1.0f);
        }
        #endif
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
MeshVisualizerGL3D::MeshVisualizerGL3D(const Flags flags): MeshVisualizerGL3D{flags, 1, 1} {}
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

Debug& operator<<(Debug& debug, const MeshVisualizerGL2D::Flag value) {
    debug << "Shaders::MeshVisualizerGL2D::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case MeshVisualizerGL2D::Flag::v: return debug << "::" #v;
        _c(NoGeometryShader)
        _c(Wireframe)
        #ifndef MAGNUM_TARGET_GLES2
        _c(InstancedObjectId)
        _c(VertexId)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(PrimitiveId)
        #endif
        _c(PrimitiveIdFromVertexId)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(UniformBuffers)
        _c(MultiDraw)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MeshVisualizerGL3D::Flag value) {
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
        _c(InstancedObjectId)
        _c(VertexId)
        #ifndef MAGNUM_TARGET_WEBGL
        _c(PrimitiveId)
        #endif
        _c(PrimitiveIdFromVertexId)
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _c(UniformBuffers)
        _c(MultiDraw)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MeshVisualizerGL2D::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::MeshVisualizerGL2D::Flags{}", {
        MeshVisualizerGL2D::Flag::Wireframe,
        /* Wireframe contains this on ES2 and WebGL 1 so it's not reported
           there */
        MeshVisualizerGL2D::Flag::NoGeometryShader,
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerGL2D::Flag::InstancedObjectId,
        MeshVisualizerGL2D::Flag::VertexId,
        MeshVisualizerGL2D::Flag::PrimitiveIdFromVertexId, /* Superset of PrimitiveId */
        #ifndef MAGNUM_TARGET_WEBGL
        MeshVisualizerGL2D::Flag::PrimitiveId,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerGL2D::Flag::MultiDraw, /* Superset of UniformBuffers */
        MeshVisualizerGL2D::Flag::UniformBuffers
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
        MeshVisualizerGL3D::Flag::InstancedObjectId,
        MeshVisualizerGL3D::Flag::VertexId,
        MeshVisualizerGL3D::Flag::PrimitiveIdFromVertexId, /* Superset of PrimitiveId */
        #ifndef MAGNUM_TARGET_WEBGL
        MeshVisualizerGL3D::Flag::PrimitiveId,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        MeshVisualizerGL3D::Flag::MultiDraw, /* Superset of UniformBuffers */
        MeshVisualizerGL3D::Flag::UniformBuffers
        #endif
        #endif
    });
}

}}
