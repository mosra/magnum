/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/BufferImage.h"
#include "Magnum/Color.h"
#include "Magnum/ColorFormat.h"
#include "Magnum/CubeMapTextureArray.h"
#include "Magnum/Image.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class CubeMapTextureArrayGLTest: public AbstractOpenGLTester {
    public:
        explicit CubeMapTextureArrayGLTest();

        void construct();
        void bind();

        void sampling();
        void samplingBorderInteger();

        void storage();

        void image();
        void imageBuffer();
        void subImage();
        void subImageBuffer();

        void generateMipmap();

        void invalidateImage();
        void invalidateSubImage();
};

CubeMapTextureArrayGLTest::CubeMapTextureArrayGLTest() {
    addTests({&CubeMapTextureArrayGLTest::construct,
              &CubeMapTextureArrayGLTest::bind,

              &CubeMapTextureArrayGLTest::sampling,
              &CubeMapTextureArrayGLTest::samplingBorderInteger,

              &CubeMapTextureArrayGLTest::storage,

              &CubeMapTextureArrayGLTest::image,
              &CubeMapTextureArrayGLTest::imageBuffer,
              &CubeMapTextureArrayGLTest::subImage,
              &CubeMapTextureArrayGLTest::subImageBuffer,

              &CubeMapTextureArrayGLTest::generateMipmap,

              &CubeMapTextureArrayGLTest::invalidateImage,
              &CubeMapTextureArrayGLTest::invalidateSubImage});
}

void CubeMapTextureArrayGLTest::construct() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    {
        CubeMapTextureArray texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::bind() {
    CubeMapTextureArray texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::sampling() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           .setBaseLevel(1)
           .setMaxLevel(750)
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

   MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::samplingBorderInteger() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_integer::string() + std::string(" is not supported."));

    CubeMapTextureArray a;
    a.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    CubeMapTextureArray b;
    b.setWrapping(Sampler::Wrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::storage() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, {32, 32, 24});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 24));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 24));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 24));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 24));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 24));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i(0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::image() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte data[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,

        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,

        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,

        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x16, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x1e, 0x4f,

        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f
    };
    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, {2, 2, 6}, data));

    MAGNUM_VERIFY_NO_ERROR();

    Image3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
       std::vector<UnsignedByte>(data, data + 96), TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::imageBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte data[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,

        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,

        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,

        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x16, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x1e, 0x4f,

        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f
    };
    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        BufferImage3D(ColorFormat::RGBA, ColorType::UnsignedByte, {2, 2, 6}, data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2, 2, 6));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
       std::vector<UnsignedByte>(data, data + 96), TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::subImage() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte zero[4*4*4*6] = {};
    constexpr UnsignedByte subData[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,

        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,

        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
    };
    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, {4, 4, 6}, zero));
    texture.setSubImage(0, Vector3i(1),
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, {2, 2, 4}, subData));

    MAGNUM_VERIFY_NO_ERROR();

    Image3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4, 4, 6));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0, 0, 0, 0,
        0, 0, 0, 0, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0, 0, 0, 0,
        0, 0, 0, 0, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0, 0, 0, 0,
        0, 0, 0, 0, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::subImageBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte zero[4*4*4*6] = {};
    constexpr UnsignedByte subData[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,

        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,

        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
    };
    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, {4, 4, 6}, zero));
    texture.setSubImage(0, Vector3i(1),
        BufferImage3D(ColorFormat::RGBA, ColorType::UnsignedByte, {2, 2, 4}, subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4, 4, 6));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0, 0, 0, 0,
        0, 0, 0, 0, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0, 0, 0, 0,
        0, 0, 0, 0, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0, 0, 0, 0,
        0, 0, 0, 0, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}

void CubeMapTextureArrayGLTest::generateMipmap() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, {32, 32, 24}));

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 24));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(0));

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 24));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 24));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 24));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 24));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 24));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 1,  1, 24));

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::invalidateImage() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, {32, 32, 24});
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}

void CubeMapTextureArrayGLTest::invalidateSubImage() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not supported."));

    CubeMapTextureArray texture;
    texture.setStorage(2, TextureFormat::RGBA8, {32, 32, 24});
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::CubeMapTextureArrayGLTest)
