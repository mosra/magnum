/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "TextureImage.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/BufferImage.h"
#endif
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Image.h"
#include "Magnum/Texture.h"

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
#include <Corrade/Utility/Resource.h>

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Mesh.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Shader.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Version.h"

#ifdef MAGNUM_BUILD_STATIC
static void importDebugToolsResources() {
    CORRADE_RESOURCE_INITIALIZE(MagnumDebugTools_RESOURCES)
}
#endif
#endif

namespace Magnum { namespace DebugTools {

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
namespace {

class FloatReinterpretShader: public AbstractShaderProgram {
    public:
        explicit FloatReinterpretShader();

        FloatReinterpretShader& setTexture(Texture2D& texture, Int level) {
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

    Shader vert{Version::GLES300, Shader::Type::Vertex};
    Shader frag{Version::GLES300, Shader::Type::Fragment};
    vert.addSource(rs.get("TextureImage.vert"));
    frag.addSource(rs.get("TextureImage.frag"));

    CORRADE_INTERNAL_ASSERT(Shader::compile({vert, frag}));
    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT(link());

    levelUniform = uniformLocation("level");
    setUniform(uniformLocation("textureData"), 0);
}

}
#endif

void textureSubImage(Texture2D& texture, const Int level, const Range2Di& range, Image2D& image) {
    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>()) {
        texture.subImage(level, range, image);
        return;
    }
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
    if(image.type() == PixelType::Float) {
        const PixelFormat imageFormat = image.format();
        TextureFormat textureFormat;
        PixelFormat reinterpretFormat;
        switch(imageFormat) {
            case PixelFormat::Red:
                textureFormat = TextureFormat::R32UI;
                reinterpretFormat = PixelFormat::RedInteger;
                break;
            case PixelFormat::RG:
                textureFormat = TextureFormat::RG32UI;
                reinterpretFormat = PixelFormat::RGInteger;
                break;
            case PixelFormat::RGB:
                textureFormat = TextureFormat::RGB32UI;
                reinterpretFormat = PixelFormat::RGBInteger;
                break;
            case PixelFormat::RGBA:
                textureFormat = TextureFormat::RGBA32UI;
                reinterpretFormat = PixelFormat::RGBAInteger;
                break;
            default:
                CORRADE_ASSERT(false, "DebugTools::textureSubImage(): unsupported pixel format" << image.format(), );
        }

        Texture2D output;
        output.setStorage(1, textureFormat, range.max());

        Framebuffer fb{range};
        fb.attachTexture(Framebuffer::ColorAttachment{0}, output, 0)
          .bind();

        CORRADE_INTERNAL_ASSERT(fb.checkStatus(FramebufferTarget::Draw) == Framebuffer::Status::Complete);
        CORRADE_INTERNAL_ASSERT(fb.checkStatus(FramebufferTarget::Read) == Framebuffer::Status::Complete);

        FloatReinterpretShader shader;
        shader.setTexture(texture, level);

        Mesh mesh;
        mesh.setCount(3)
            .draw(shader);

        /* release() needs to be called after querying the size to avoid zeroing it out */
        {
            Vector2i imageSize = image.size();
            image.setData(image.storage(), reinterpretFormat, PixelType::UnsignedInt, imageSize, image.release());
        }

        fb.read(range, image);

        /* release() needs to be called after querying the size to avoid zeroing it out */
        {
            Vector2i imageSize = image.size();
            image.setData(image.storage(), imageFormat, PixelType::Float, imageSize, image.release());
        }
        return;
    }
    #endif

    Framebuffer fb{range};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, texture, level)
      .read(range, image);
}

Image2D textureSubImage(Texture2D& texture, const Int level, const Range2Di& range, Image2D&& image) {
    textureSubImage(texture, level, range, image);
    return std::move(image);
}

#ifndef MAGNUM_TARGET_GLES2
void textureSubImage(Texture2D& texture, const Int level, const Range2Di& range, BufferImage2D& image, const BufferUsage usage) {
    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>()) {
        texture.subImage(level, range, image, usage);
        return;
    }
    #endif

    Framebuffer fb{range};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, texture, level)
      .read(range, image, usage);
}

BufferImage2D textureSubImage(Texture2D& texture, const Int level, const Range2Di& range, BufferImage2D&& image, const BufferUsage usage) {
    textureSubImage(texture, level, range, image, usage);
    return std::move(image);
}
#endif

void textureSubImage(CubeMapTexture& texture, const CubeMapCoordinate coordinate, const Int level, const Range2Di& range, Image2D& image) {
    Framebuffer fb{range};
    fb.attachCubeMapTexture(Framebuffer::ColorAttachment{0}, texture, coordinate, level)
      .read(range, image);
}

Image2D textureSubImage(CubeMapTexture& texture, const CubeMapCoordinate coordinate, const Int level, const Range2Di& range, Image2D&& image) {
    textureSubImage(texture, coordinate, level, range, image);
    return std::move(image);
}

#ifndef MAGNUM_TARGET_GLES2
void textureSubImage(CubeMapTexture& texture, const CubeMapCoordinate coordinate, const Int level, const Range2Di& range, BufferImage2D& image, const BufferUsage usage) {
    Framebuffer fb{range};
    fb.attachCubeMapTexture(Framebuffer::ColorAttachment{0}, texture, coordinate, level)
      .read(range, image, usage);
}

BufferImage2D textureSubImage(CubeMapTexture& texture, const CubeMapCoordinate coordinate, const Int level, const Range2Di& range, BufferImage2D&& image, const BufferUsage usage) {
    textureSubImage(texture, coordinate, level, range, image, usage);
    return std::move(image);
}
#endif

}}
