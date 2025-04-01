/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "DistanceFieldGL.h"

#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/Math/Range.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"

#ifdef MAGNUM_BUILD_STATIC
static void importTextureToolResources() {
    CORRADE_RESOURCE_INITIALIZE(MagnumTextureTools_RESOURCES)
}
#endif

namespace Magnum { namespace TextureTools {

using namespace Containers::Literals;

namespace {

class DistanceFieldShader: public GL::AbstractShaderProgram {
    public:
        typedef GL::Attribute<0, Vector2> Position;

        explicit DistanceFieldShader(UnsignedInt radius);

        DistanceFieldShader& setImageSizeInverted(const Vector2& size) {
            setUniform(_imageSizeInvertedUniform, size);
            return *this;
        }

        DistanceFieldShader& bindTexture(GL::Texture2D& texture) {
            texture.bind(TextureUnit);
            return *this;
        }

    private:
        /* ES2 on iOS (apparently independent on the device) has only 8 texture
           units, so be careful to not step over that. ES3 on the same has 16.
           Not using the default (0) because this shader is quite specific.
           Unit 6 is used by Shaders::Vector and Shaders::DistanceFieldVector. */
        enum: Int { TextureUnit = 7 };

        Int _imageSizeInvertedUniform{0};
};

DistanceFieldShader::DistanceFieldShader(const UnsignedInt radius) {
    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumTextureTools"_s))
        importTextureToolResources();
    #endif
    Utility::Resource rs("MagnumTextureTools"_s);

    #ifndef MAGNUM_TARGET_GLES
    const GL::Version v = GL::Context::current().supportedVersion({GL::Version::GL320, GL::Version::GL300, GL::Version::GL210});
    #else
    const GL::Version v = GL::Context::current().supportedVersion({
        #ifndef MAGNUM_TARGET_WEBGL
        GL::Version::GLES310,
        #endif
        GL::Version::GLES300, GL::Version::GLES200});
    #endif

    GL::Shader vert{v, GL::Shader::Type::Vertex};
    vert.addSource(rs.getString("compatibility.glsl"_s))
        .addSource(rs.getString("FullScreenTriangle.glsl"_s))
        .addSource(rs.getString("DistanceFieldShader.vert"_s));

    GL::Shader frag{v, GL::Shader::Type::Fragment};
    frag.addSource(rs.getString("compatibility.glsl"_s))
        .addSource(Utility::format("#define RADIUS {}\n", radius))
        .addSource(rs.getString("DistanceFieldShader.frag"_s));

    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

    attachShaders({vert, frag});

    #ifndef MAGNUM_TARGET_GLES2
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
    #endif
    {
        bindAttributeLocation(Position::Location, "position"_s);
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>())
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(v < GL::Version::GLES310)
    #endif
    {
        _imageSizeInvertedUniform = uniformLocation("imageSizeInverted"_s);
    }

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shading_language_420pack>())
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(v < GL::Version::GLES310)
    #endif
    {
        setUniform(uniformLocation("textureData"_s), TextureUnit);
    }
}

}

struct DistanceFieldGL::State {
    explicit State(UnsignedInt radius): shader{radius}, radius{radius} {}

    DistanceFieldShader shader;
    UnsignedInt radius;
    GL::Mesh mesh;
};

DistanceFieldGL::DistanceFieldGL(const UnsignedInt radius): _state{new State{radius}} {
    #ifndef MAGNUM_TARGET_GLES
    MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(GL::Extensions::ARB::framebuffer_object);
    #endif

    _state->mesh.setPrimitive(GL::MeshPrimitive::Triangles)
        .setCount(3);

    #ifndef MAGNUM_TARGET_GLES2
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
    #endif
    {
        constexpr Vector2 triangle[]{
            {-1.0f,  1.0f},
            {-1.0f, -3.0f},
            { 3.0f,  1.0f}
        };
        _state->mesh.addVertexBuffer(GL::Buffer{triangle}, 0, DistanceFieldShader::Position());
    }
}

DistanceFieldGL::DistanceFieldGL(NoCreateT) noexcept {}

DistanceFieldGL::DistanceFieldGL(DistanceFieldGL&&) noexcept = default;

DistanceFieldGL::~DistanceFieldGL() = default;

DistanceFieldGL& DistanceFieldGL::operator=(DistanceFieldGL&&) noexcept = default;

UnsignedInt DistanceFieldGL::radius() const { return _state->radius; }

void DistanceFieldGL::operator()(GL::Texture2D& input, GL::Framebuffer& output, const Range2Di& rectangle, const Vector2i&
    #ifdef MAGNUM_TARGET_GLES
    imageSize
    #endif
) {
    /** @todo Disable depth test, blending and then enable it back (if was previously) */

    #ifndef MAGNUM_TARGET_GLES
    Vector2i imageSize = input.imageSize(0);
    #endif

    CORRADE_ASSERT(output.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete,
        "TextureTools::DistanceFieldGL: output texture format not framebuffer-drawable:" << output.checkStatus(GL::FramebufferTarget::Draw), );

    /* The shader assumes that the ratio between the output and input is a
       multiple of 2, causing output pixel *centers* to be aligned with input
       pixel *edges* */
    CORRADE_ASSERT(imageSize % rectangle.size() == Vector2i{0} &&
                   (imageSize/rectangle.size()) % 2 == Vector2i{0},
        "TextureTools::DistanceFieldGL: expected input and output size ratio to be a multiple of 2, got" << Debug::packed << imageSize << "and" << Debug::packed << rectangle.size(), );

    /* Save existing viewport to restore it back after */
    const Range2Di previousViewport = output.viewport();

    output
        .setViewport(rectangle)
        .bind();

    _state->shader
        .bindTexture(input)
        .setImageSizeInverted(1.0f/Vector2(imageSize))
        .draw(_state->mesh);

    /* Restore the previous viewport */
    output.setViewport(previousViewport);
}

#ifndef MAGNUM_TARGET_GLES
void DistanceFieldGL::operator()(GL::Texture2D& input, GL::Framebuffer& output, const Range2Di& rectangle) {
    return operator()(input, output, rectangle, {});
}
#endif

void DistanceFieldGL::operator()(GL::Texture2D& input, GL::Texture2D& output, const Range2Di& rectangle, const Vector2i&
    #ifdef MAGNUM_TARGET_GLES
    imageSize
    #endif
) {
    GL::Framebuffer framebuffer{rectangle};
    framebuffer.attachTexture(GL::Framebuffer::ColorAttachment(0), output, 0);

    operator()(input, framebuffer, rectangle
        #ifdef MAGNUM_TARGET_GLES
        , imageSize
        #endif
        );
}

#ifndef MAGNUM_TARGET_GLES
void DistanceFieldGL::operator()(GL::Texture2D& input, GL::Texture2D& output, const Range2Di& rectangle) {
    return operator()(input, output, rectangle, {});
}
#endif

}}
