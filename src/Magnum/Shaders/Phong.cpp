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

#include "Phong.h"

#ifdef MAGNUM_TARGET_GLES
#include <Corrade/Containers/Array.h>
#endif
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

namespace {
    enum: Int {
        AmbientTextureLayer = 0,
        DiffuseTextureLayer = 1,
        SpecularTextureLayer = 2
    };
}

Phong::Phong(const Flags flags, const UnsignedInt lightCount): _flags{flags}, _lightCount{lightCount}, _lightColorsUniform{9 + Int(lightCount)} {
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

    #ifndef MAGNUM_TARGET_GLES
    /* Initializer for the light color array -- we need a list of vec4(1.0)
       joined by commas. For GLES we'll simply upload the values directly. */
    constexpr const char lightInitializerPreamble[] = "#define LIGHT_COLOR_INITIALIZER ";
    constexpr std::size_t lightInitializerPreambleSize =
        Containers::arraySize(lightInitializerPreamble) - 1;
    constexpr const char lightInitializerItem[] = "vec4(1.0), ";
    constexpr std::size_t lightInitializerItemSize =
        Containers::arraySize(lightInitializerItem) - 1;
    std::string lightInitializer;
    lightInitializer.reserve(Containers::arraySize(lightInitializerPreamble) - 1 + lightCount*lightInitializerItemSize);
    lightInitializer.append(lightInitializerPreamble, lightInitializerPreambleSize);
    for(std::size_t i = 0; i != lightCount; ++i)
        lightInitializer.append(lightInitializerItem, lightInitializerItemSize);

    /* Drop the last comma and add a newline at the end */
    lightInitializer[lightInitializer.size() - 2] = '\n';
    lightInitializer.resize(lightInitializer.size() - 1);
    #endif

    vert.addSource(flags ? "#define TEXTURED\n" : "")
        .addSource(Utility::formatString("#define LIGHT_COUNT {}\n", lightCount))
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("Phong.vert"));
    frag.addSource(flags & Flag::AmbientTexture ? "#define AMBIENT_TEXTURE\n" : "")
        .addSource(flags & Flag::DiffuseTexture ? "#define DIFFUSE_TEXTURE\n" : "")
        .addSource(flags & Flag::SpecularTexture ? "#define SPECULAR_TEXTURE\n" : "")
        .addSource(flags & Flag::AlphaMask ? "#define ALPHA_MASK\n" : "")
        .addSource(Utility::formatString(
            "#define LIGHT_COUNT {}\n"
            "#define LIGHT_COLORS_LOCATION {}\n", lightCount, 9 + lightCount))
        #ifndef MAGNUM_TARGET_GLES
        .addSource(std::move(lightInitializer))
        #endif
        .addSource(rs.get("Phong.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #else
    if(!GL::Context::current().isVersionSupported(GL::Version::GLES300))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        bindAttributeLocation(Normal::Location, "normal");
        if(flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture))
            bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        _transformationMatrixUniform = uniformLocation("transformationMatrix");
        _projectionMatrixUniform = uniformLocation("projectionMatrix");
        _normalMatrixUniform = uniformLocation("normalMatrix");
        _ambientColorUniform = uniformLocation("ambientColor");
        _diffuseColorUniform = uniformLocation("diffuseColor");
        _specularColorUniform = uniformLocation("specularColor");
        _shininessUniform = uniformLocation("shininess");
        if(flags & Flag::AlphaMask) _alphaMaskUniform = uniformLocation("alphaMask");
        _lightPositionsUniform = uniformLocation("lightPositions");
        _lightColorsUniform = uniformLocation("lightColors");
    }

    #ifndef MAGNUM_TARGET_GLES
    if(flags && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
    #endif
    {
        if(flags & Flag::AmbientTexture) setUniform(uniformLocation("ambientTexture"), AmbientTextureLayer);
        if(flags & Flag::DiffuseTexture) setUniform(uniformLocation("diffuseTexture"), DiffuseTextureLayer);
        if(flags & Flag::SpecularTexture) setUniform(uniformLocation("specularTexture"), SpecularTextureLayer);
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    /* Default to fully opaque white so we can see the textures */
    if(flags & Flag::AmbientTexture) setAmbientColor(Color4{1.0f});
    else setAmbientColor(Color4{0.0f});
    setDiffuseColor(Color4{1.0f});
    setSpecularColor(Color4{1.0f});
    setShininess(80.0f);
    if(flags & Flag::AlphaMask) setAlphaMask(0.5f);
    setLightColors(Containers::Array<Color4>{Containers::DirectInit, lightCount, Color4{1.0f}});

    setTransformationMatrix({});
    setProjectionMatrix({});
    setNormalMatrix({});
    /* Light position is zero by default */
    #endif
}

Phong& Phong::bindAmbientTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::AmbientTexture,
        "Shaders::Phong::bindAmbientTexture(): the shader was not created with ambient texture enabled", *this);
    texture.bind(AmbientTextureLayer);
    return *this;
}

Phong& Phong::bindDiffuseTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::DiffuseTexture,
        "Shaders::Phong::bindDiffuseTexture(): the shader was not created with diffuse texture enabled", *this);
    texture.bind(DiffuseTextureLayer);
    return *this;
}

Phong& Phong::bindSpecularTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::SpecularTexture,
        "Shaders::Phong::bindSpecularTexture(): the shader was not created with specular texture enabled", *this);
    texture.bind(SpecularTextureLayer);
    return *this;
}

Phong& Phong::bindTextures(GL::Texture2D* ambient, GL::Texture2D* diffuse, GL::Texture2D* specular) {
    CORRADE_ASSERT(_flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture),
        "Shaders::Phong::bindTextures(): the shader was not created with any textures enabled", *this);
    GL::AbstractTexture::bind(AmbientTextureLayer, {ambient, diffuse, specular});
    return *this;
}

Phong& Phong::setAlphaMask(Float mask) {
    CORRADE_ASSERT(_flags & Flag::AlphaMask,
        "Shaders::Phong::setAlphaMask(): the shader was not created with alpha mask enabled", *this);
    setUniform(_alphaMaskUniform, mask);
    return *this;
}

Phong& Phong::setLightPositions(const Containers::ArrayView<const Vector3> positions) {
    CORRADE_ASSERT(_lightCount == positions.size(),
        "Shaders::Phong::setLightPositions(): expected" << _lightCount << "items but got" << positions.size(), *this);
    setUniform(_lightPositionsUniform, positions);
    return *this;
}

Phong& Phong::setLightPosition(UnsignedInt id, const Vector3& position) {
    CORRADE_ASSERT(id < _lightCount,
        "Shaders::Phong::setLightPosition(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
    setUniform(_lightPositionsUniform + id, position);
    return *this;
}

Phong& Phong::setLightColors(const Containers::ArrayView<const Color4> colors) {
    CORRADE_ASSERT(_lightCount == colors.size(),
        "Shaders::Phong::setLightColors(): expected" << _lightCount << "items but got" << colors.size(), *this);
    setUniform(_lightColorsUniform, colors);
    return *this;
}

Phong& Phong::setLightColor(UnsignedInt id, const Color4& color) {
    CORRADE_ASSERT(id < _lightCount,
        "Shaders::Phong::setLightColor(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
    setUniform(_lightColorsUniform + id, color);
    return *this;
}

Debug& operator<<(Debug& debug, const Phong::Flag value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case Phong::Flag::v: return debug << "Shaders::Phong::Flag::" #v;
        _c(AmbientTexture)
        _c(DiffuseTexture)
        _c(SpecularTexture)
        _c(AlphaMask)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Shaders::Phong::Flag(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Phong::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::Phong::Flags{}", {
        Phong::Flag::AmbientTexture,
        Phong::Flag::DiffuseTexture,
        Phong::Flag::SpecularTexture,
        Phong::Flag::AlphaMask});
}

}}
