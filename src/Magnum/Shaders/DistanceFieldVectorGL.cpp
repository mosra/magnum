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

#include "DistanceFieldVectorGL.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#ifndef MAGNUM_TARGET_GLES2
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/GL/Buffer.h"
#endif

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

namespace {
    enum: Int { TextureUnit = 6 };

    #ifndef MAGNUM_TARGET_GLES2
    enum: Int {
        /* Not using the zero binding to avoid conflicts with
           ProjectionBufferBinding from other shaders which can likely stay
           bound to the same buffer for the whole time */
        TransformationProjectionBufferBinding = 1,
        DrawBufferBinding = 2,
        TextureTransformationBufferBinding = 3,
        MaterialBufferBinding = 4
    };
    #endif
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>::DistanceFieldVectorGL(const Flags flags
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
    CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || materialCount,
        "Shaders::DistanceFieldVectorGL: material count can't be zero", );
    CORRADE_ASSERT(!(flags >= Flag::UniformBuffers) || drawCount,
        "Shaders::DistanceFieldVectorGL: draw count can't be zero", );
    #endif

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

    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShadersGL"))
        importShaderResources();
    #endif
    Utility::Resource rs("MagnumShadersGL");

    const GL::Context& context = GL::Context::current();

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = context.supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    #else
    const GL::Version version = context.supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    GL::Shader vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    GL::Shader frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    vert.addSource(flags & Flag::TextureTransformation ? "#define TEXTURE_TRANSFORMATION\n" : "")
        .addSource(dimensions == 2 ? "#define TWO_DIMENSIONS\n" : "#define THREE_DIMENSIONS\n");
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= Flag::UniformBuffers) {
        vert.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define DRAW_COUNT {}\n",
            drawCount));
        vert.addSource(flags >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif
    vert.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("Vector.vert"));
    #ifndef MAGNUM_TARGET_GLES2
    if(flags >= Flag::UniformBuffers) {
        frag.addSource(Utility::formatString(
            "#define UNIFORM_BUFFERS\n"
            "#define MATERIAL_COUNT {}\n"
            "#define DRAW_COUNT {}\n",
            materialCount,
            drawCount));
        frag.addSource(flags >= Flag::MultiDraw ? "#define MULTI_DRAW\n" : "");
    }
    #endif
    frag.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("DistanceFieldVector.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    /* ES3 has this done in the shader directly */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");
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
            _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
            if(flags & Flag::TextureTransformation)
                _textureMatrixUniform = uniformLocation("textureMatrix");
            _colorUniform = uniformLocation("color");
            _outlineColorUniform = uniformLocation("outlineColor");
            _outlineRangeUniform = uniformLocation("outlineRange");
            _smoothnessUniform = uniformLocation("smoothness");
        }
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
    #endif
    {
        setUniform(uniformLocation("vectorTexture"), TextureUnit);
        #ifndef MAGNUM_TARGET_GLES2
        if(flags >= Flag::UniformBuffers) {
            setUniformBlockBinding(uniformBlockIndex("TransformationProjection"), TransformationProjectionBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Draw"), DrawBufferBinding);
            setUniformBlockBinding(uniformBlockIndex("Material"), MaterialBufferBinding);
            if(flags & Flag::TextureTransformation)
                setUniformBlockBinding(uniformBlockIndex("TextureTransformation"), TextureTransformationBufferBinding);
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
        setTransformationProjectionMatrix(MatrixTypeFor<dimensions, Float>{Math::IdentityInit});
        if(flags & Flag::TextureTransformation)
            setTextureMatrix(Matrix3{Math::IdentityInit});
        setColor(Color4{1.0f});
        /* Outline color is zero by default */
        setOutlineRange(0.5f, 1.0f);
        setSmoothness(0.04f);
    }
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>::DistanceFieldVectorGL(const Flags flags): DistanceFieldVectorGL{flags, 1, 1} {}
#endif

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::DistanceFieldVectorGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setTextureMatrix(const Matrix3& matrix) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::DistanceFieldVectorGL::setTextureMatrix(): the shader was created with uniform buffers enabled", *this);
    #endif
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::DistanceFieldVectorGL::setTextureMatrix(): the shader was not created with texture transformation enabled", *this);
    setUniform(_textureMatrixUniform, matrix);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setColor(const Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::DistanceFieldVectorGL::setColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_colorUniform, color);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setOutlineColor(const Color4& color) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::DistanceFieldVectorGL::setOutlineColor(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_outlineColorUniform, color);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setOutlineRange(Float start, Float end) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::DistanceFieldVectorGL::setOutlineRange(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_outlineRangeUniform, Vector2(start, end));
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setSmoothness(Float value) {
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(!(_flags >= Flag::UniformBuffers),
        "Shaders::DistanceFieldVectorGL::setSmoothness(): the shader was created with uniform buffers enabled", *this);
    #endif
    setUniform(_smoothnessUniform, value);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setDrawOffset(const UnsignedInt offset) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::DistanceFieldVectorGL::setDrawOffset(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(offset < _drawCount,
        "Shaders::DistanceFieldVectorGL::setDrawOffset(): draw offset" << offset << "is out of bounds for" << _drawCount << "draws", *this);
    if(_drawCount > 1) setUniform(_drawOffsetUniform, offset);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::DistanceFieldVectorGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::bindTransformationProjectionBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::DistanceFieldVectorGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TransformationProjectionBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::bindDrawBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::DistanceFieldVectorGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::bindDrawBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::DistanceFieldVectorGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, DrawBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::bindTextureTransformationBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::DistanceFieldVectorGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::DistanceFieldVectorGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TextureTransformationBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::bindTextureTransformationBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::DistanceFieldVectorGL::bindTextureTransformationBuffer(): the shader was not created with uniform buffers enabled", *this);
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::DistanceFieldVectorGL::bindTextureTransformationBuffer(): the shader was not created with texture transformation enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, TextureTransformationBufferBinding, offset, size);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::bindMaterialBuffer(GL::Buffer& buffer) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::DistanceFieldVectorGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::bindMaterialBuffer(GL::Buffer& buffer, const GLintptr offset, const GLsizeiptr size) {
    CORRADE_ASSERT(_flags >= Flag::UniformBuffers,
        "Shaders::DistanceFieldVectorGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled", *this);
    buffer.bind(GL::Buffer::Target::Uniform, MaterialBufferBinding, offset, size);
    return *this;
}
#endif

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::bindVectorTexture(GL::Texture2D& texture) {
    texture.bind(TextureUnit);
    return *this;
}

template class MAGNUM_SHADERS_EXPORT DistanceFieldVectorGL<2>;
template class MAGNUM_SHADERS_EXPORT DistanceFieldVectorGL<3>;

namespace Implementation {

Debug& operator<<(Debug& debug, const DistanceFieldVectorGLFlag value) {
    debug << "Shaders::DistanceFieldVectorGL::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case DistanceFieldVectorGLFlag::v: return debug << "::" #v;
        _c(TextureTransformation)
        #ifndef MAGNUM_TARGET_GLES2
        _c(UniformBuffers)
        _c(MultiDraw)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DistanceFieldVectorGLFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::DistanceFieldVectorGL::Flags{}", {
        DistanceFieldVectorGLFlag::TextureTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        DistanceFieldVectorGLFlag::MultiDraw, /* Superset of UniformBuffers */
        DistanceFieldVectorGLFlag::UniformBuffers
        #endif
    });
}

}

}}
