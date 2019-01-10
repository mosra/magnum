/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "Flat.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

namespace {
    enum: Int { TextureLayer = 0 };

    template<UnsignedInt> constexpr const char* vertexShaderName();
    template<> constexpr const char* vertexShaderName<2>() { return "Flat2D.vert"; }
    template<> constexpr const char* vertexShaderName<3>() { return "Flat3D.vert"; }
}

template<UnsignedInt dimensions> Flat<dimensions>::Flat(const Flags flags): _flags(flags) {
    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumShaders"))
        importShaderResources();
    #endif
    Utility::Resource rs("MagnumShaders");

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GL320, GL::Version::GL310, GL::Version::GL300, GL::Version::GL210});
    #else
    const GL::Version version = GL::Context::current().supportedVersion({GL::Version::GLES300, GL::Version::GLES200});
    #endif

    GL::Shader vert = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Vertex);
    GL::Shader frag = Implementation::createCompatibilityShader(rs, version, GL::Shader::Type::Fragment);

    vert.addSource(flags & Flag::Textured ? "#define TEXTURED\n" : "")
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get(vertexShaderName<dimensions>()));
    frag.addSource(flags & Flag::Textured ? "#define TEXTURED\n" : "")
        .addSource(flags & Flag::AlphaMask ? "#define ALPHA_MASK\n" : "")
        .addSource(rs.get("Flat.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #else
    if(!GL::Context::current().isVersionSupported(GL::Version::GLES300))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        if(flags & Flag::Textured) bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        _transformationProjectionMatrixUniform = uniformLocation("transformationProjectionMatrix");
        _colorUniform = uniformLocation("color");
        if(flags & Flag::AlphaMask) _alphaMaskUniform = uniformLocation("alphaMask");
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
    #endif
    {
        if(flags & Flag::Textured) setUniform(uniformLocation("textureData"), TextureLayer);
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    setTransformationProjectionMatrix({});
    setColor(Color4{1.0f});
    if(flags & Flag::AlphaMask) setAlphaMask(0.5f);
    #endif
}

template<UnsignedInt dimensions> Flat<dimensions>& Flat<dimensions>::bindTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::Textured,
        "Shaders::Flat::bindTexture(): the shader was not created with texturing enabled", *this);
    texture.bind(TextureLayer);
    return *this;
}

template<UnsignedInt dimensions> Flat<dimensions>& Flat<dimensions>::setAlphaMask(Float mask) {
    CORRADE_ASSERT(_flags & Flag::AlphaMask,
        "Shaders::Flat::setAlphaMask(): the shader was not created with alpha mask enabled", *this);
    setUniform(_alphaMaskUniform, mask);
    return *this;
}

template class Flat<2>;
template class Flat<3>;

namespace Implementation {

Debug& operator<<(Debug& debug, const FlatFlag value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case FlatFlag::v: return debug << "Shaders::Flat::Flag::" #v;
        _c(Textured)
        _c(AlphaMask)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Shaders::Flat::Flag(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const FlatFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::Flat::Flags{}", {
        FlatFlag::Textured,
        FlatFlag::AlphaMask});
}

}

}}
