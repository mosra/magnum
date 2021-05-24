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
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>::DistanceFieldVectorGL(const Flags flags): _flags{flags} {
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
        .addSource(dimensions == 2 ? "#define TWO_DIMENSIONS\n" : "#define THREE_DIMENSIONS\n")
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("AbstractVector.vert"));
    frag.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("DistanceFieldVector.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    GL::AbstractShaderProgram::attachShaders({vert, frag});

    /* ES3 has this done in the shader directly */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        GL::AbstractShaderProgram::bindAttributeLocation(AbstractVectorGL<dimensions>::Position::Location, "position");
        GL::AbstractShaderProgram::bindAttributeLocation(AbstractVectorGL<dimensions>::TextureCoordinates::Location, "textureCoordinates");
    }
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::AbstractShaderProgram::link());

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        _transformationProjectionMatrixUniform = GL::AbstractShaderProgram::uniformLocation("transformationProjectionMatrix");
        if(flags & Flag::TextureTransformation)
            _textureMatrixUniform = GL::AbstractShaderProgram::uniformLocation("textureMatrix");
        _colorUniform = GL::AbstractShaderProgram::uniformLocation("color");
        _outlineColorUniform = GL::AbstractShaderProgram::uniformLocation("outlineColor");
        _outlineRangeUniform = GL::AbstractShaderProgram::uniformLocation("outlineRange");
        _smoothnessUniform = GL::AbstractShaderProgram::uniformLocation("smoothness");
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!context.isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
    #endif
    {
        GL::AbstractShaderProgram::setUniform(GL::AbstractShaderProgram::uniformLocation("vectorTexture"),
            AbstractVectorGL<dimensions>::VectorTextureUnit);
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    setTransformationProjectionMatrix(MatrixTypeFor<dimensions, Float>{Math::IdentityInit});
    if(flags & Flag::TextureTransformation)
        setTextureMatrix(Matrix3{Math::IdentityInit});
    setColor(Color4{1.0f}); /* Outline color is zero by default */
    setOutlineRange(0.5f, 1.0f);
    setSmoothness(0.04f);
    #endif
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
    GL::AbstractShaderProgram::setUniform(_transformationProjectionMatrixUniform, matrix);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setTextureMatrix(const Matrix3& matrix) {
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::DistanceFieldVectorGL::setTextureMatrix(): the shader was not created with texture transformation enabled", *this);
    GL::AbstractShaderProgram::setUniform(_textureMatrixUniform, matrix);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setColor(const Color4& color) {
    GL::AbstractShaderProgram::setUniform(_colorUniform, color);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setOutlineColor(const Color4& color) {
    GL::AbstractShaderProgram::setUniform(_outlineColorUniform, color);
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setOutlineRange(Float start, Float end) {
    GL::AbstractShaderProgram::setUniform(_outlineRangeUniform, Vector2(start, end));
    return *this;
}

template<UnsignedInt dimensions> DistanceFieldVectorGL<dimensions>& DistanceFieldVectorGL<dimensions>::setSmoothness(Float value) {
    GL::AbstractShaderProgram::setUniform(_smoothnessUniform, value);
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
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DistanceFieldVectorGLFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::DistanceFieldVectorGL::Flags{}", {
        DistanceFieldVectorGLFlag::TextureTransformation
        });
}

}

}}
