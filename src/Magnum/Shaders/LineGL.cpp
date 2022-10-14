/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "LineGL.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

#ifndef MAGNUM_TARGET_GLES2
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/GL/Buffer.h"
#endif

namespace Magnum { namespace Shaders {

namespace {
    enum: Int {
        /* 0/1/2/3 taken by Phong (A/D/S/N), 4 by MeshVisualizer colormap, 5 by
           object ID textures, 6 by Vector */
        TextureUnit = 7
    };

    #ifndef MAGNUM_TARGET_GLES2
    enum: Int {
        /* Not using the zero binding to avoid conflicts with
           ProjectionBufferBinding from other shaders which can likely stay
           bound to the same buffer for the whole time */
        TransformationProjectionBufferBinding = 1,
        DrawBufferBinding = 2,
        MaterialBufferBinding = 3
    };
    #endif
}

template<UnsignedInt dimensions> typename LineGL<dimensions>::CompileState LineGL<dimensions>::compile(const Configuration& configuration) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.materialCount(),
        "Shaders::LineGL: material count can't be zero", CompileState{NoCreate});
    CORRADE_ASSERT(!(configuration.flags() >= Flag::UniformBuffers) || configuration.drawCount(),
        "Shaders::LineGL: draw count can't be zero", CompileState{NoCreate});
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

    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShadersGL"))
        importShaderResources();
    #endif
    Utility::Resource rs("MagnumShadersGL");

    const GL::Context& context = GL::Context::current();

    const GL::Version version = context.supportedVersion({
        #ifndef MAGNUM_TARGET_GLES
        GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210
        #else
        GL::Version::GLES300, GL::Version::GLES200
        #endif
    });

    GL::Shader vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    GL::Shader frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    vert.addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
        .addSource(dimensions == 2 ? "#define TWO_DIMENSIONS\n" : "#define THREE_DIMENSIONS\n")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        #endif
        .addSource(configuration.flags() & Flag::InstancedTransformation ? "#define INSTANCED_TRANSFORMATION\n" : "");
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers) {
        vert.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n",
            configuration.drawCount()));
        vert.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif
    vert.addSource(rs.getString("generic.glsl"))
        .addSource(rs.getString("Line.vert"));
    frag.addSource(configuration.flags() & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(configuration.flags() & Flag::ObjectId ? "#define OBJECT_ID\n" : "")
        .addSource(configuration.flags() >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        #endif
        ;
    if(configuration.capStyle() == CapStyle::Square)
        frag.addSource("#define CAP_STYLE_SQUARE\n");
    else if(configuration.capStyle() == CapStyle::Round)
        frag.addSource("#define CAP_STYLE_ROUND\n");
    else if(configuration.capStyle() == CapStyle::Triangle)
        frag.addSource("#define CAP_STYLE_TRIANGLE\n");
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #ifndef MAGNUM_TARGET_GLES2
    if(configuration.flags() >= Flag::UniformBuffers) {
        frag.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n"
            "#define MATERIAL_COUNT {}\n",
            configuration.drawCount(),
            configuration.materialCount()));
        frag.addSource(configuration.flags() >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif
    frag.addSource(rs.getString("generic.glsl"))
        .addSource(rs.getString("Line.frag"));

    vert.submitCompile();
    frag.submitCompile();

    LineGL<dimensions> out{NoInit};
    out._flags = configuration.flags();
    out._capStyle = configuration.capStyle();
    #ifndef MAGNUM_TARGET_GLES2
    out._materialCount = configuration.materialCount();
    out._drawCount = configuration.drawCount();
    #endif

    out.attachShaders({vert, frag});

    /* ES3 has this done in the shader directly and doesn't even provide
       bindFragmentDataLocation() */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        out.bindAttributeLocation(Position::Location, "position");
        out.bindAttributeLocation(PreviousPosition::Location, "direction");
        out.bindAttributeLocation(NextPosition::Location, "neighborDirection");
        if(configuration.flags() & Flag::VertexColor)
            out.bindAttributeLocation(Color3::Location, "vertexColor"); /* Color4 is the same */
        #ifndef MAGNUM_TARGET_GLES2
        if(configuration.flags() & Flag::ObjectId) {
            out.bindFragmentDataLocation(ColorOutput, "color");
            out.bindFragmentDataLocation(ObjectIdOutput, "objectId");
        }
        if(configuration.flags() >= Flag::InstancedObjectId)
            out.bindAttributeLocation(ObjectId::Location, "instanceObjectId");
        #endif
        if(configuration.flags() & Flag::InstancedTransformation)
            out.bindAttributeLocation(TransformationMatrix::Location, "instancedTransformationMatrix");
    }
    #endif

    out.submitLink();

    return CompileState{std::move(out), std::move(vert), std::move(frag), version};
}

template<UnsignedInt dimensions> LineGL<dimensions>::LineGL(CompileState&& state): LineGL{static_cast<LineGL&&>(std::move(state))} {
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* When graceful assertions fire from within compile(), we get a NoCreate'd
       CompileState. Exiting makes it possible to test the assert. */
    if(!id()) return;
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(checkLink({GL::Shader(state._vert), GL::Shader(state._frag)}));

    const GL::Context& context = GL::Context::current();
    const GL::Version version = state._version;

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        _viewportSizeUniform = uniformLocation("viewportSize");
        #ifndef MAGNUM_TARGET_GLES2
        if(_flags >= Flag::UniformBuffers) {
            if(_drawCount > 1) _drawOffsetUniform = uniformLocation("drawOffset");
        } else
        #endif
        {
            _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
            _widthUniform = uniformLocation("width");
            _smoothnessUniform = uniformLocation("smoothness");
            _backgroundColorUniform = uniformLocation("backgroundColor");
            _colorUniform = uniformLocation("color");
            #ifndef MAGNUM_TARGET_GLES2
            if(_flags & Flag::ObjectId) _objectIdUniform = uniformLocation("objectId");
            #endif
        }
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES2
        if(_flags >= Flag::UniformBuffers) {
            setUniformBlockBinding(uniformBlockIndex("TransformationProjection"), TransformationProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"), DrawBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"), MaterialBufferBinding);
        }
        #endif
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_GLES2
    if(_flags >= Flag::UniformBuffers) {
        /* Draw offset is zero by default */
    } else
    #endif
    {
        setTransformationProjectionMatrix(MatrixTypeFor<dimensions, Float>{Math::IdentityInit});
        setColor(Magnum::Color4{1.0f});
        /* Object ID is zero by default */
    }
    #endif

    static_cast<void>(version);
    static_cast<void>(context);
}

template<UnsignedInt dimensions> LineGL<dimensions>::LineGL(const Configuration& configuration): LineGL{compile(configuration)} {}

template<UnsignedInt dimensions> LineGL<dimensions>::LineGL(NoInitT) {}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setViewportSize(const Vector2& size) {
    setUniform(_viewportSizeUniform, size);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setWidth(const Float width) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_widthUniform, width);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setSmoothness(const Float smoothness) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_smoothnessUniform, smoothness);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setBackgroundColor(const Magnum::Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_backgroundColorUniform, color);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setColor(const Magnum::Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_colorUniform, color);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setObjectId(UnsignedInt id) {
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::LineGL::setObjectId(): the shader was created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::ObjectId,
        "Shaders::LineGL::setObjectId(): the shader was not created with object ID enabled", *this);
    setUniform(_objectIdUniform, id);
    return *this;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::setDrawOffset(const UnsignedInt offset) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::setDrawOffset(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(offset < _drawCount,
        "Shaders::LineGL::setDrawOffset(): draw offset" << offset << "is out of bounds for" << _drawCount << "draws", *this);
    if(_drawCount > 1) setUniform(_drawOffsetUniform, offset);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindDrawBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindDrawBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindMaterialBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> LineGL<dimensions>& LineGL<dimensions>::bindMaterialBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::LineGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding, offset, size);
    return *this;
}
#endif

template class MAGNUM_SHADERS_EXPORT LineGL<2>;
template class MAGNUM_SHADERS_EXPORT LineGL<3>;

namespace Implementation {

Debug& operator<<(Debug& debug, const LineGLFlag value) {
    debug << "Shaders::LineGL::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case LineGLFlag::v: return debug << "::" #v;
        _c(VertexColor)
        #ifndef MAGNUM_TARGET_GLES2
        _c(ObjectId)
        _c(InstancedObjectId)
        #endif
        _c(InstancedTransformation)
        #ifndef MAGNUM_TARGET_GLES2
        _c(UniformBuffers)
        _c(MultiDraw)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const LineGLFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::LineGL::Flags{}", {
        LineGLFlag::VertexColor,
        #ifndef MAGNUM_TARGET_GLES2
        LineGLFlag::InstancedObjectId, /* Superset of ObjectId */
        LineGLFlag::ObjectId,
        #endif
        LineGLFlag::InstancedTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        LineGLFlag::MultiDraw, /* Superset of UniformBuffers */
        LineGLFlag::UniformBuffers
        #endif
    });
}

}

}}
