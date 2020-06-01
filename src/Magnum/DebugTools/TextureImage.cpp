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

#include "TextureImage.h"

#include "Magnum/Image.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/BufferImage.h"
#endif
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Texture.h"

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/GL/Version.h"

#ifdef MAGNUM_BUILD_STATIC
static void importDebugToolsResources() {
    CORRADE_RESOURCE_INITIALIZE(MagnumDebugTools_RESOURCES)
}
#endif
#endif

namespace Magnum { namespace DebugTools {

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
namespace {

class FloatReinterpretShader: public GL::AbstractShaderProgram {
    public:
        explicit FloatReinterpretShader();

        FloatReinterpretShader& setTexture(GL::Texture2D& texture, Int level) {
            texture.bind(0);
            setUniform(levelUniform, level);
            return *this;
        }

    private:
        Int levelUniform;
};

FloatReinterpretShader::FloatReinterpretShader() {
    #ifdef MAGNUM_BUILD_STATIC
    /* Import resources on static build, if not already */
    if(!Utility::Resource::hasGroup("MagnumDebugTools"))
        importDebugToolsResources();
    #endif
    Utility::Resource rs{"MagnumDebugTools"};

    GL::Shader vert{GL::Version::GLES300, GL::Shader::Type::Vertex};
    GL::Shader frag{GL::Version::GLES300, GL::Shader::Type::Fragment};
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        vert.addSource("#define DISABLE_GL_MAGNUM_shader_vertex_id\n");
    vert.addSource(rs.get("TextureImage.vert"));
    frag.addSource(rs.get("TextureImage.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({vert, frag}));
    attachShaders({vert, frag});

    if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>()) {
        bindAttributeLocation(0, "position");
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    levelUniform = uniformLocation("level");
    setUniform(uniformLocation("textureData"), 0);
}

}
#endif

void textureSubImage(GL::Texture2D& texture, const Int level, const Range2Di& range, Image2D& image) {
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::get_texture_sub_image>()) {
        texture.subImage(level, range, image);
        return;
    }
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
    GL::PixelType type = GL::pixelType(image.format(), image.formatExtra());
    if(type == GL::PixelType::Float) {
        GL::TextureFormat textureFormat;
        GL::PixelFormat reinterpretFormat;
        GL::PixelFormat format = GL::pixelFormat(image.format());
        switch(format) {
            case GL::PixelFormat::Red:
                textureFormat = GL::TextureFormat::R32UI;
                reinterpretFormat = GL::PixelFormat::RedInteger;
                break;
            case GL::PixelFormat::RG:
                textureFormat = GL::TextureFormat::RG32UI;
                reinterpretFormat = GL::PixelFormat::RGInteger;
                break;
            case GL::PixelFormat::RGB:
                textureFormat = GL::TextureFormat::RGB32UI;
                reinterpretFormat = GL::PixelFormat::RGBInteger;
                break;
            case GL::PixelFormat::RGBA:
                textureFormat = GL::TextureFormat::RGBA32UI;
                reinterpretFormat = GL::PixelFormat::RGBAInteger;
                break;
            default:
                CORRADE_ASSERT_UNREACHABLE("DebugTools::textureSubImage(): unsupported pixel format" << format, );
        }

        GL::Texture2D output;
        output.setStorage(1, textureFormat, range.max());

        GL::Framebuffer fb{range};
        fb.attachTexture(GL::Framebuffer::ColorAttachment{0}, output, 0)
          .bind();

        CORRADE_INTERNAL_ASSERT(fb.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);
        CORRADE_INTERNAL_ASSERT(fb.checkStatus(GL::FramebufferTarget::Read) == GL::Framebuffer::Status::Complete);

        FloatReinterpretShader shader;
        shader.setTexture(texture, level);

        GL::Mesh mesh;
        mesh.setCount(3);

        if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>()) {
            constexpr Vector2 triangle[]{
                {-1.0f,  1.0f},
                {-1.0f, -3.0f},
                { 3.0f,  1.0f}
            };
            GL::Buffer buffer{GL::Buffer::TargetHint::Array};
            buffer.setData(triangle, GL::BufferUsage::StaticDraw);
            mesh.addVertexBuffer(std::move(buffer), 0, GL::Attribute<0, Vector2>{});
        }

        shader.draw(mesh);

        /* release() needs to be called after querying the size to avoid zeroing it out */
        const Vector2i imageSize = image.size();
        Image2D temp{image.storage(), reinterpretFormat, GL::PixelType::UnsignedInt, imageSize, image.release()};
        fb.read(range, temp);
        image = Image2D{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), range.size(), temp.release()};
        return;
    }
    #endif

    GL::Framebuffer fb{range};
    fb.attachTexture(GL::Framebuffer::ColorAttachment{0}, texture, level)
      .read(range, image);
}

Image2D textureSubImage(GL::Texture2D& texture, const Int level, const Range2Di& range, Image2D&& image) {
    textureSubImage(texture, level, range, image);
    return std::move(image);
}

#ifndef MAGNUM_TARGET_GLES2
void textureSubImage(GL::Texture2D& texture, const Int level, const Range2Di& range, GL::BufferImage2D& image, const GL::BufferUsage usage) {
    #ifndef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::ARB::get_texture_sub_image>()) {
        texture.subImage(level, range, image, usage);
        return;
    }
    #endif

    GL::Framebuffer fb{range};
    fb.attachTexture(GL::Framebuffer::ColorAttachment{0}, texture, level)
      .read(range, image, usage);
}

GL::BufferImage2D textureSubImage(GL::Texture2D& texture, const Int level, const Range2Di& range, GL::BufferImage2D&& image, const GL::BufferUsage usage) {
    textureSubImage(texture, level, range, image, usage);
    return std::move(image);
}
#endif

void textureSubImage(GL::CubeMapTexture& texture, const GL::CubeMapCoordinate coordinate, const Int level, const Range2Di& range, Image2D& image) {
    GL::Framebuffer fb{range};
    fb.attachCubeMapTexture(GL::Framebuffer::ColorAttachment{0}, texture, coordinate, level)
      .read(range, image);
}

Image2D textureSubImage(GL::CubeMapTexture& texture, const GL::CubeMapCoordinate coordinate, const Int level, const Range2Di& range, Image2D&& image) {
    textureSubImage(texture, coordinate, level, range, image);
    return std::move(image);
}

#ifndef MAGNUM_TARGET_GLES2
void textureSubImage(GL::CubeMapTexture& texture, const GL::CubeMapCoordinate coordinate, const Int level, const Range2Di& range, GL::BufferImage2D& image, const GL::BufferUsage usage) {
    GL::Framebuffer fb{range};
    fb.attachCubeMapTexture(GL::Framebuffer::ColorAttachment{0}, texture, coordinate, level)
      .read(range, image, usage);
}

GL::BufferImage2D textureSubImage(GL::CubeMapTexture& texture, const GL::CubeMapCoordinate coordinate, const Int level, const Range2Di& range, GL::BufferImage2D&& image, const GL::BufferUsage usage) {
    textureSubImage(texture, coordinate, level, range, image, usage);
    return std::move(image);
}
#endif

}}
