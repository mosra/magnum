/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"

#include "Magnum/Shaders/Implementation/CreateCompatibilityShader.h"

namespace Magnum { namespace Shaders {

namespace {
    enum: Int {
        AmbientTextureUnit = 0,
        DiffuseTextureUnit = 1,
        SpecularTextureUnit = 2,
        NormalTextureUnit = 3
    };
}

Phong::Phong(const Flags flags, const UnsignedInt lightCount): _flags{flags}, _lightCount{lightCount}, _lightColorsUniform{_lightPositionsUniform + Int(lightCount)} {
    CORRADE_ASSERT(!(flags & Flag::TextureTransformation) || (flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture)),
        "Shaders::Phong: texture transformation enabled but the shader is not textured", );

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
    std::string lightInitializer;
    if(lightCount) {
        /* Initializer for the light color array -- we need a list of vec4(1.0)
           joined by commas. For GLES we'll simply upload the values directly. */
        constexpr const char lightInitializerPreamble[] = "#define LIGHT_COLOR_INITIALIZER ";
        constexpr std::size_t lightInitializerPreambleSize =
            Containers::arraySize(lightInitializerPreamble) - 1;
        constexpr const char lightInitializerItem[] = "vec4(1.0), ";
        constexpr std::size_t lightInitializerItemSize =
            Containers::arraySize(lightInitializerItem) - 1;
        lightInitializer.reserve(Containers::arraySize(lightInitializerPreamble) - 1 + lightCount*lightInitializerItemSize);
        lightInitializer.append(lightInitializerPreamble, lightInitializerPreambleSize);
        for(std::size_t i = 0; i != lightCount; ++i)
            lightInitializer.append(lightInitializerItem, lightInitializerItemSize);

        /* Drop the last comma and add a newline at the end */
        lightInitializer[lightInitializer.size() - 2] = '\n';
        lightInitializer.resize(lightInitializer.size() - 1);
    }
    #endif

    vert.addSource(flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture) ? "#define TEXTURED\n" : "")
        .addSource(flags & Flag::NormalTexture ? "#define NORMAL_TEXTURE\n" : "")
        .addSource(flags & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
        .addSource(flags & Flag::TextureTransformation ? "#define TEXTURE_TRANSFORMATION\n" : "")
        .addSource(Utility::formatString("#define LIGHT_COUNT {}\n", lightCount))
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(flags >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        #endif
        .addSource(flags & Flag::InstancedTransformation ? "#define INSTANCED_TRANSFORMATION\n" : "")
        .addSource(flags >= Flag::InstancedTextureOffset ? "#define INSTANCED_TEXTURE_OFFSET\n" : "")
        .addSource(rs.get("generic.glsl"))
        .addSource(rs.get("Phong.vert"));
    frag.addSource(flags & Flag::AmbientTexture ? "#define AMBIENT_TEXTURE\n" : "")
        .addSource(flags & Flag::DiffuseTexture ? "#define DIFFUSE_TEXTURE\n" : "")
        .addSource(flags & Flag::SpecularTexture ? "#define SPECULAR_TEXTURE\n" : "")
        .addSource(flags & Flag::NormalTexture ? "#define NORMAL_TEXTURE\n" : "")
        .addSource(flags & Flag::VertexColor ? "#define VERTEX_COLOR\n" : "")
        .addSource(flags & Flag::AlphaMask ? "#define ALPHA_MASK\n" : "")
        #ifndef MAGNUM_TARGET_GLES2
        .addSource(flags & Flag::ObjectId ? "#define OBJECT_ID\n" : "")
        .addSource(flags >= Flag::InstancedObjectId ? "#define INSTANCED_OBJECT_ID\n" : "")
        #endif
        .addSource(Utility::formatString(
            "#define LIGHT_COUNT {}\n"
            "#define LIGHT_COLORS_LOCATION {}\n", lightCount, _lightPositionsUniform + lightCount));
    #ifndef MAGNUM_TARGET_GLES
    if(lightCount) frag.addSource(std::move(lightInitializer));
    #endif
    frag.addSource(rs.get("generic.glsl"))
        .addSource(rs.get("Phong.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    /* ES3 has this done in the shader directly and doesn't even provide
       bindFragmentDataLocation() */
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_attrib_location>(version))
    #endif
    {
        bindAttributeLocation(Position::Location, "position");
        if(lightCount)
            bindAttributeLocation(Normal::Location, "normal");
        if((flags & Flag::NormalTexture) && lightCount)
            bindAttributeLocation(Tangent::Location, "tangent");
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
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>(version))
    #endif
    {
        _transformationMatrixUniform = uniformLocation("transformationMatrix");
        if(flags & Flag::TextureTransformation)
            _textureMatrixUniform = uniformLocation("textureMatrix");
        _projectionMatrixUniform = uniformLocation("projectionMatrix");
        _ambientColorUniform = uniformLocation("ambientColor");
        if(lightCount) {
            _normalMatrixUniform = uniformLocation("normalMatrix");
            _diffuseColorUniform = uniformLocation("diffuseColor");
            _specularColorUniform = uniformLocation("specularColor");
            _shininessUniform = uniformLocation("shininess");
            _lightPositionsUniform = uniformLocation("lightPositions");
            _lightColorsUniform = uniformLocation("lightColors");
        }
        if(flags & Flag::AlphaMask) _alphaMaskUniform = uniformLocation("alphaMask");
        #ifndef MAGNUM_TARGET_GLES2
        if(flags & Flag::ObjectId) _objectIdUniform = uniformLocation("objectId");
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(flags && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>(version))
    #endif
    {
        if(flags & Flag::AmbientTexture) setUniform(uniformLocation("ambientTexture"), AmbientTextureUnit);
        if(lightCount) {
            if(flags & Flag::DiffuseTexture) setUniform(uniformLocation("diffuseTexture"), DiffuseTextureUnit);
            if(flags & Flag::SpecularTexture) setUniform(uniformLocation("specularTexture"), SpecularTextureUnit);
            if(flags & Flag::NormalTexture) setUniform(uniformLocation("normalTexture"), NormalTextureUnit);
        }
    }

    /* Set defaults in OpenGL ES (for desktop they are set in shader code itself) */
    #ifdef MAGNUM_TARGET_GLES
    /* Default to fully opaque white so we can see the textures */
    if(flags & Flag::AmbientTexture) setAmbientColor(Magnum::Color4{1.0f});
    else setAmbientColor(Magnum::Color4{0.0f});
    setTransformationMatrix({});
    setProjectionMatrix({});
    if(lightCount) {
        setDiffuseColor(Magnum::Color4{1.0f});
        setSpecularColor(Magnum::Color4{1.0f});
        setShininess(80.0f);
        setLightColors(Containers::Array<Magnum::Color4>{Containers::DirectInit, lightCount, Magnum::Color4{1.0f}});
        /* Light position is zero by default */
        setNormalMatrix({});
    }
    if(flags & Flag::TextureTransformation) setTextureMatrix({});
    if(flags & Flag::AlphaMask) setAlphaMask(0.5f);
    /* Object ID is zero by default */
    #endif
}

Phong& Phong::setAmbientColor(const Magnum::Color4& color) {
    setUniform(_ambientColorUniform, color);
    return *this;
}

Phong& Phong::bindAmbientTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::AmbientTexture,
        "Shaders::Phong::bindAmbientTexture(): the shader was not created with ambient texture enabled", *this);
    texture.bind(AmbientTextureUnit);
    return *this;
}

Phong& Phong::setDiffuseColor(const Magnum::Color4& color) {
    if(_lightCount) setUniform(_diffuseColorUniform, color);
    return *this;
}

Phong& Phong::bindDiffuseTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::DiffuseTexture,
        "Shaders::Phong::bindDiffuseTexture(): the shader was not created with diffuse texture enabled", *this);
    if(_lightCount) texture.bind(DiffuseTextureUnit);
    return *this;
}

Phong& Phong::setSpecularColor(const Magnum::Color4& color) {
    if(_lightCount) setUniform(_specularColorUniform, color);
    return *this;
}

Phong& Phong::bindSpecularTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::SpecularTexture,
        "Shaders::Phong::bindSpecularTexture(): the shader was not created with specular texture enabled", *this);
    if(_lightCount) texture.bind(SpecularTextureUnit);
    return *this;
}

Phong& Phong::bindNormalTexture(GL::Texture2D& texture) {
    CORRADE_ASSERT(_flags & Flag::NormalTexture,
        "Shaders::Phong::bindNormalTexture(): the shader was not created with normal texture enabled", *this);
    if(_lightCount) texture.bind(NormalTextureUnit);
    return *this;
}

Phong& Phong::bindTextures(GL::Texture2D* ambient, GL::Texture2D* diffuse, GL::Texture2D* specular, GL::Texture2D* normal) {
    CORRADE_ASSERT(_flags & (Flag::AmbientTexture|Flag::DiffuseTexture|Flag::SpecularTexture|Flag::NormalTexture),
        "Shaders::Phong::bindTextures(): the shader was not created with any textures enabled", *this);
    GL::AbstractTexture::bind(AmbientTextureUnit, {ambient, diffuse, specular, normal});
    return *this;
}

Phong& Phong::setShininess(Float shininess) {
    if(_lightCount) setUniform(_shininessUniform, shininess);
    return *this;
}

Phong& Phong::setAlphaMask(Float mask) {
    CORRADE_ASSERT(_flags & Flag::AlphaMask,
        "Shaders::Phong::setAlphaMask(): the shader was not created with alpha mask enabled", *this);
    setUniform(_alphaMaskUniform, mask);
    return *this;
}

#ifndef MAGNUM_TARGET_GLES2
Phong& Phong::setObjectId(UnsignedInt id) {
    CORRADE_ASSERT(_flags & Flag::ObjectId,
        "Shaders::Phong::setObjectId(): the shader was not created with object ID enabled", *this);
    setUniform(_objectIdUniform, id);
    return *this;
}
#endif

Phong& Phong::setTransformationMatrix(const Matrix4& matrix) {
    setUniform(_transformationMatrixUniform, matrix);
    return *this;
}

Phong& Phong::setNormalMatrix(const Matrix3x3& matrix) {
    if(_lightCount) setUniform(_normalMatrixUniform, matrix);
    return *this;
}

Phong& Phong::setProjectionMatrix(const Matrix4& matrix) {
    setUniform(_projectionMatrixUniform, matrix);
    return *this;
}

Phong& Phong::setTextureMatrix(const Matrix3& matrix) {
    CORRADE_ASSERT(_flags & Flag::TextureTransformation,
        "Shaders::Phong::setTextureMatrix(): the shader was not created with texture transformation enabled", *this);
    setUniform(_textureMatrixUniform, matrix);
    return *this;
}

Phong& Phong::setLightPositions(const Containers::ArrayView<const Vector3> positions) {
    CORRADE_ASSERT(_lightCount == positions.size(),
        "Shaders::Phong::setLightPositions(): expected" << _lightCount << "items but got" << positions.size(), *this);
    if(_lightCount) setUniform(_lightPositionsUniform, positions);
    return *this;
}

Phong& Phong::setLightPosition(UnsignedInt id, const Vector3& position) {
    CORRADE_ASSERT(id < _lightCount,
        "Shaders::Phong::setLightPosition(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
    setUniform(_lightPositionsUniform + id, position);
    return *this;
}

/* It's light, but can't be in the header because MSVC needs to know the size
   of Vector3 for the initializer list use */
Phong& Phong::setLightPositions(std::initializer_list<Vector3> lights) {
    return setLightPositions({lights.begin(), lights.size()});
}

Phong& Phong::setLightColors(const Containers::ArrayView<const Magnum::Color4> colors) {
    CORRADE_ASSERT(_lightCount == colors.size(),
        "Shaders::Phong::setLightColors(): expected" << _lightCount << "items but got" << colors.size(), *this);
    if(_lightCount) setUniform(_lightColorsUniform, colors);
    return *this;
}

/* It's light, but can't be in the header because MSVC needs to know the size
   of Color for the initializer list use */
Phong& Phong::setLightColors(std::initializer_list<Magnum::Color4> colors) {
    return setLightColors({colors.begin(), colors.size()});
}

Phong& Phong::setLightColor(UnsignedInt id, const Magnum::Color4& color) {
    CORRADE_ASSERT(id < _lightCount,
        "Shaders::Phong::setLightColor(): light ID" << id << "is out of bounds for" << _lightCount << "lights", *this);
    setUniform(_lightColorsUniform + id, color);
    return *this;
}

Debug& operator<<(Debug& debug, const Phong::Flag value) {
    debug << "Shaders::Phong::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case Phong::Flag::v: return debug << "::" #v;
        _c(AmbientTexture)
        _c(DiffuseTexture)
        _c(SpecularTexture)
        _c(NormalTexture)
        _c(AlphaMask)
        _c(VertexColor)
        _c(TextureTransformation)
        #ifndef MAGNUM_TARGET_GLES2
        _c(InstancedObjectId)
        _c(ObjectId)
        #endif
        _c(InstancedTransformation)
        _c(InstancedTextureOffset)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Phong::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Shaders::Phong::Flags{}", {
        Phong::Flag::AmbientTexture,
        Phong::Flag::DiffuseTexture,
        Phong::Flag::SpecularTexture,
        Phong::Flag::NormalTexture,
        Phong::Flag::AlphaMask,
        Phong::Flag::VertexColor,
        Phong::Flag::InstancedTextureOffset, /* Superset of TextureTransformation */
        Phong::Flag::TextureTransformation,
        #ifndef MAGNUM_TARGET_GLES2
        Phong::Flag::InstancedObjectId, /* Superset of ObjectId */
        Phong::Flag::ObjectId,
        #endif
        Phong::Flag::InstancedTransformation});
}

}}
