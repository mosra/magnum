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

#include "Magnum/configure.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/BufferImage.h"
#endif
#include "Magnum/ColorFormat.h"
#include "Magnum/Image.h"
#include "Magnum/Texture.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class TextureGLTest: public AbstractOpenGLTester {
    public:
        explicit TextureGLTest();

        #ifndef MAGNUM_TARGET_GLES
        void construct1D();
        #endif
        void construct2D();
        void construct3D();
        #ifndef MAGNUM_TARGET_GLES
        void construct1DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void construct2DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void construct2DMultisample();
        void construct2DMultisampleArray();
        void constructRectangle();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void sampling1D();
        #endif
        void sampling2D();
        void sampling3D();
        #ifndef MAGNUM_TARGET_GLES
        void sampling1DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void sampling2DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void samplingRectangle();
        #endif

        #ifdef MAGNUM_TARGET_GLES
        void samplingBorder2D();
        void samplingBorder3D();
        #ifndef MAGNUM_TARGET_GLES2
        void samplingBorder2DArray();
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void storage1D();
        #endif
        void storage2D();
        void storage3D();
        #ifndef MAGNUM_TARGET_GLES
        void storage1DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void storage2DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void storage2DMultisample();
        void storage2DMultisampleArray();
        void storageRectangle();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void image1D();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void image1DBuffer();
        #endif
        void image2D();
        #ifndef MAGNUM_TARGET_GLES2
        void image2DBuffer();
        #endif
        void image3D();
        #ifndef MAGNUM_TARGET_GLES2
        void image3DBuffer();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void image1DArray();
        void image1DArrayBuffer();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void image2DArray();
        void image2DArrayBuffer();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void image2DMultisample();
        void image2DMultisampleBuffer();
        void image2DMultisampleArray();
        void image2DMultisampleArrayBuffer();
        void imageRectangle();
        void imageRectangleBuffer();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void subImage1D();
        void subImage1DBuffer();
        #endif
        void subImage2D();
        #ifndef MAGNUM_TARGET_GLES2
        void subImage2DBuffer();
        #endif
        void subImage3D();
        #ifndef MAGNUM_TARGET_GLES2
        void subImage3DBuffer();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void subImage1DArray();
        void subImage1DArrayBuffer();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void subImage2DArray();
        void subImage2DArrayBuffer();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void subImage2DMultisample();
        void subImage2DMultisampleBuffer();
        void subImage2DMultisampleArray();
        void subImage2DMultisampleArrayBuffer();
        void subImageRectangle();
        void subImageRectangleBuffer();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void generateMipmap1D();
        #endif
        void generateMipmap2D();
        void generateMipmap3D();
        #ifndef MAGNUM_TARGET_GLES
        void generateMipmap1DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void generateMipmap2DArray();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void invalidateImage1D();
        #endif
        void invalidateImage2D();
        void invalidateImage3D();
        #ifndef MAGNUM_TARGET_GLES
        void invalidateImage1DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void invalidateImage2DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void invalidateImage2DMultisample();
        void invalidateImage2DMultisampleArray();
        void invalidateImageRectangle();
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void invalidateSubImage1D();
        #endif
        void invalidateSubImage2D();
        void invalidateSubImage3D();
        #ifndef MAGNUM_TARGET_GLES
        void invalidateSubImage1DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        void invalidateSubImage2DArray();
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void invalidateSubImage2DMultisample();
        void invalidateSubImage2DMultisampleArray();
        void invalidateSubImageRectangle();
        #endif
};

TextureGLTest::TextureGLTest() {
    addTests({
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::construct1D,
        #endif
        &TextureGLTest::construct2D,
        &TextureGLTest::construct3D,
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::construct1DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::construct2DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::construct2DMultisample,
        &TextureGLTest::construct2DMultisampleArray,
        &TextureGLTest::constructRectangle,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::sampling1D,
        #endif
        &TextureGLTest::sampling2D,
        &TextureGLTest::sampling3D,
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::sampling1DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::sampling2DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingRectangle,
        #endif

        #ifdef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingBorder2D,
        &TextureGLTest::samplingBorder3D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::samplingBorder2DArray,
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::storage1D,
        #endif
        &TextureGLTest::storage2D,
        &TextureGLTest::storage3D,
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::storage1DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::storage2DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::storage2DMultisample,
        &TextureGLTest::storage2DMultisampleArray,
        &TextureGLTest::storageRectangle,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::image1D,
        &TextureGLTest::image1DBuffer,
        #endif
        &TextureGLTest::image2D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::image2DBuffer,
        #endif
        &TextureGLTest::image3D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::image3DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::image1DArray,
        &TextureGLTest::image1DArrayBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::image2DArray,
        &TextureGLTest::image2DArrayBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::image2DMultisample,
        &TextureGLTest::image2DMultisampleBuffer,
        &TextureGLTest::image2DMultisampleArray,
        &TextureGLTest::image2DMultisampleArrayBuffer,
        &TextureGLTest::imageRectangle,
        &TextureGLTest::imageRectangleBuffer,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::subImage1D,
        &TextureGLTest::subImage1DBuffer,
        #endif
        &TextureGLTest::subImage2D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::subImage2DBuffer,
        #endif
        &TextureGLTest::subImage3D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::subImage3DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::subImage1DArray,
        &TextureGLTest::subImage1DArrayBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::subImage2DArray,
        &TextureGLTest::subImage2DArrayBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::subImage2DMultisample,
        &TextureGLTest::subImage2DMultisampleBuffer,
        &TextureGLTest::subImage2DMultisampleArray,
        &TextureGLTest::subImage2DMultisampleArrayBuffer,
        &TextureGLTest::subImageRectangle,
        &TextureGLTest::subImageRectangleBuffer,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::generateMipmap1D,
        #endif
        &TextureGLTest::generateMipmap2D,
        &TextureGLTest::generateMipmap3D,
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::generateMipmap1DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::generateMipmap2DArray,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::invalidateImage1D,
        #endif
        &TextureGLTest::invalidateImage2D,
        &TextureGLTest::invalidateImage3D,
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::invalidateImage1DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::invalidateImage2DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::invalidateImage2DMultisample,
        &TextureGLTest::invalidateImage2DMultisampleArray,
        &TextureGLTest::invalidateImageRectangle,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::invalidateSubImage1D,
        #endif
        &TextureGLTest::invalidateSubImage2D,
        &TextureGLTest::invalidateSubImage3D,
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::invalidateSubImage1DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::invalidateSubImage2DArray,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::invalidateSubImage2DMultisample,
        &TextureGLTest::invalidateSubImage2DMultisampleArray,
        &TextureGLTest::invalidateSubImageRectangle
        #endif
    });
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::construct1D() {
    {
        Texture1D texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::construct2D() {
    {
        Texture2D texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::construct3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    {
        Texture3D texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::construct1DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    {
        Texture2D texture(Texture2D::Target::Texture1DArray);

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::construct2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    {
        Texture3D texture(Texture3D::Target::Texture2DArray);

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::construct2DMultisample() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    {
        Texture2D texture(Texture2D::Target::Texture2DMultisample);

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::construct2DMultisampleArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    {
        Texture3D texture(Texture3D::Target::Texture2DMultisampleArray);

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::constructRectangle() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    {
        Texture2D texture(Texture2D::Target::Rectangle);

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::sampling1D() {
    Texture1D texture;
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::sampling2D() {
    Texture2D texture;
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           #ifndef MAGNUM_TARGET_GLES
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           #else
           .setWrapping(Sampler::Wrapping::ClampToEdge)
           #endif
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

    MAGNUM_VERIFY_NO_ERROR();
}

#ifdef MAGNUM_TARGET_GLES
void TextureGLTest::samplingBorder2D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::NV::texture_border_clamp>())
        CORRADE_SKIP(Extensions::GL::NV::texture_border_clamp::string() + std::string(" is not supported."));

    Texture2D texture;
    texture.setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::sampling3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           #ifndef MAGNUM_TARGET_GLES
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           #else
           .setWrapping(Sampler::Wrapping::ClampToEdge)
           #endif
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

    MAGNUM_VERIFY_NO_ERROR();
}

#ifdef MAGNUM_TARGET_GLES
void TextureGLTest::samplingBorder3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    if(!Context::current()->isExtensionSupported<Extensions::GL::NV::texture_border_clamp>())
        CORRADE_SKIP(Extensions::GL::NV::texture_border_clamp::string() + std::string(" is not supported."));

    Texture3D texture;
    texture.setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::sampling1DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture2D texture(Texture2D::Target::Texture1DArray);
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::sampling2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture3D texture(Texture3D::Target::Texture2DArray);
    texture.setMinificationFilter(Sampler::Filter::Linear, Sampler::Mipmap::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           #ifndef MAGNUM_TARGET_GLES
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           #else
           .setWrapping(Sampler::Wrapping::ClampToEdge)
           #endif
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

    MAGNUM_VERIFY_NO_ERROR();
}

#ifdef MAGNUM_TARGET_GLES
void TextureGLTest::samplingBorder2DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::NV::texture_border_clamp>())
        CORRADE_SKIP(Extensions::GL::NV::texture_border_clamp::string() + std::string(" is not supported."));

    Texture3D texture(Texture3D::Target::Texture2DArray);
    texture.setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::samplingRectangle() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    Texture2D texture(Texture2D::Target::Rectangle);
    texture.setMinificationFilter(Sampler::Filter::Linear)
           .setMagnificationFilter(Sampler::Filter::Linear)
           .setWrapping(Sampler::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::storage1D() {
    Texture1D texture;
    texture.setStorage(5, TextureFormat::RGBA8, 32);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), 32);
    CORRADE_COMPARE(texture.imageSize(1), 16);
    CORRADE_COMPARE(texture.imageSize(2),  8);
    CORRADE_COMPARE(texture.imageSize(3),  4);
    CORRADE_COMPARE(texture.imageSize(4),  2);
    CORRADE_COMPARE(texture.imageSize(5),  0); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::storage2D() {
    Texture2D texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

void TextureGLTest::storage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector3i(32));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::storage1DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture2D texture(Texture2D::Target::Texture1DArray);
    texture.setStorage(5, TextureFormat::RGBA8, Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 0,  0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::storage2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture3D texture(Texture3D::Target::Texture2DArray);
    texture.setStorage(5, TextureFormat::RGBA8, Vector3i(32));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 0,  0,  0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::storage2DMultisample() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void TextureGLTest::storage2DMultisampleArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void TextureGLTest::storageRectangle() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    Texture2D texture(Texture2D::Target::Rectangle);
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i(32));

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i( 0)); /* not available */

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::image1D() {
    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07 };
    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference1D(ColorFormat::RGBA, ColorType::UnsignedByte, 2, data));

    MAGNUM_VERIFY_NO_ERROR();

    Image1D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
        std::vector<UnsignedByte>(data, data + 8), TestSuite::Compare::Container);
}

void TextureGLTest::image1DBuffer() {
    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07 };
    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        BufferImage1D(ColorFormat::RGBA, ColorType::UnsignedByte, 2, data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage1D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticDraw);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
        std::vector<UnsignedByte>(data, data + 8), TestSuite::Compare::Container);
}
#endif

void TextureGLTest::image2D() {
    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
       std::vector<UnsignedByte>(data, data + 16), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::image2DBuffer() {
    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
        std::vector<UnsignedByte>(data, data + 16), TestSuite::Compare::Container);
    #endif
}
#endif

void TextureGLTest::image3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f,
                                      0x10, 0x11, 0x12, 0x13,
                                      0x14, 0x15, 0x16, 0x17,
                                      0x18, 0x19, 0x1a, 0x1b,
                                      0x1c, 0x1d, 0x1e, 0x1f };
    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), data));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
       std::vector<UnsignedByte>(data, data + 32), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::image3DBuffer() {
    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f,
                                      0x10, 0x11, 0x12, 0x13,
                                      0x14, 0x15, 0x16, 0x17,
                                      0x18, 0x19, 0x1a, 0x1b,
                                      0x1c, 0x1d, 0x1e, 0x1f };
    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        BufferImage3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
        std::vector<UnsignedByte>(data, data + 32), TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::image1DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture(Texture2D::Target::Texture1DArray);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data));

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
       std::vector<UnsignedByte>(data, data + 16), TestSuite::Compare::Container);
}

void TextureGLTest::image1DArrayBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture(Texture2D::Target::Texture1DArray);
    texture.setImage(0, TextureFormat::RGBA8,
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
        std::vector<UnsignedByte>(data, data + 16), TestSuite::Compare::Container);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::image2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f,
                                      0x10, 0x11, 0x12, 0x13,
                                      0x14, 0x15, 0x16, 0x17,
                                      0x18, 0x19, 0x1a, 0x1b,
                                      0x1c, 0x1d, 0x1e, 0x1f };
    Texture3D texture(Texture3D::Target::Texture2DArray);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), data));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
       std::vector<UnsignedByte>(data, data + 32), TestSuite::Compare::Container);
    #endif
}

void TextureGLTest::image2DArrayBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f,
                                      0x10, 0x11, 0x12, 0x13,
                                      0x14, 0x15, 0x16, 0x17,
                                      0x18, 0x19, 0x1a, 0x1b,
                                      0x1c, 0x1d, 0x1e, 0x1f };
    Texture3D texture(Texture3D::Target::Texture2DArray);
    texture.setImage(0, TextureFormat::RGBA8,
        BufferImage3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
        std::vector<UnsignedByte>(data, data + 32), TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::image2DMultisample() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void TextureGLTest::image2DMultisampleBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void TextureGLTest::image2DMultisampleArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void TextureGLTest::image2DMultisampleArrayBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void TextureGLTest::imageRectangle() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture(Texture2D::Target::Rectangle);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data));

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()),
       std::vector<UnsignedByte>(data, data + 16), TestSuite::Compare::Container);
}

void TextureGLTest::imageRectangleBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    constexpr UnsignedByte data[] = { 0x00, 0x01, 0x02, 0x03,
                                      0x04, 0x05, 0x06, 0x07,
                                      0x08, 0x09, 0x0a, 0x0b,
                                      0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), data, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()),
        std::vector<UnsignedByte>(data, data+16), TestSuite::Compare::Container);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::subImage1D() {
    constexpr UnsignedByte zero[4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07 };
    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference1D(ColorFormat::RGBA, ColorType::UnsignedByte, 4, zero));
    texture.setSubImage(0, 1,
        ImageReference1D(ColorFormat::RGBA, ColorType::UnsignedByte, 2, subData));

    MAGNUM_VERIFY_NO_ERROR();

    Image1D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), 4);
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()), (std::vector<UnsignedByte>{
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}

void TextureGLTest::subImage1DBuffer() {
    constexpr UnsignedByte zero[4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07 };
    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference1D(ColorFormat::RGBA, ColorType::UnsignedByte, 4, zero));
    texture.setSubImage(0, 1,
        BufferImage1D(ColorFormat::RGBA, ColorType::UnsignedByte, 2, subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage1D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), 4);
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()), (std::vector<UnsignedByte>{
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}
#endif

void TextureGLTest::subImage2D() {
    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(0, Vector2i(1),
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::subImage2DBuffer() {
    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(0, Vector2i(1),
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
    #endif
}
#endif

void TextureGLTest::subImage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    constexpr UnsignedByte zero[4*4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f,
                                         0x10, 0x11, 0x12, 0x13,
                                         0x14, 0x15, 0x16, 0x17,
                                         0x18, 0x19, 0x1a, 0x1b,
                                         0x1c, 0x1d, 0x1e, 0x1f };
    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(4), zero));
    texture.setSubImage(0, Vector3i(1),
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), subData));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
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
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::subImage3DBuffer() {
    constexpr UnsignedByte zero[4*4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f,
                                         0x10, 0x11, 0x12, 0x13,
                                         0x14, 0x15, 0x16, 0x17,
                                         0x18, 0x19, 0x1a, 0x1b,
                                         0x1c, 0x1d, 0x1e, 0x1f };
    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(4), zero));
    texture.setSubImage(0, Vector3i(1),
        BufferImage3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
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
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::subImage1DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture(Texture2D::Target::Texture1DArray);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(0, Vector2i(1),
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData));

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}

void TextureGLTest::subImage1DArrayBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture(Texture2D::Target::Texture1DArray);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(0, Vector2i(1),
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::subImage2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    constexpr UnsignedByte zero[4*4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f,
                                         0x10, 0x11, 0x12, 0x13,
                                         0x14, 0x15, 0x16, 0x17,
                                         0x18, 0x19, 0x1a, 0x1b,
                                         0x1c, 0x1d, 0x1e, 0x1f };
    Texture3D texture(Texture3D::Target::Texture2DArray);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(4), zero));
    texture.setSubImage(0, Vector3i(1),
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), subData));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
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
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
    #endif
}

void TextureGLTest::subImage2DArrayBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    constexpr UnsignedByte zero[4*4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f,
                                         0x10, 0x11, 0x12, 0x13,
                                         0x14, 0x15, 0x16, 0x17,
                                         0x18, 0x19, 0x1a, 0x1b,
                                         0x1c, 0x1d, 0x1e, 0x1f };
    Texture3D texture(Texture3D::Target::Texture2DArray);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(4), zero));
    texture.setSubImage(0, Vector3i(1),
        BufferImage3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(2), subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
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
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::subImage2DMultisample() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void TextureGLTest::subImage2DMultisampleBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void TextureGLTest::subImage2DMultisampleArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void TextureGLTest::subImage2DMultisampleArrayBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Not implemented yet.");
}

void TextureGLTest::subImageRectangle() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture(Texture2D::Target::Rectangle);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(0, Vector2i(1),
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData));

    MAGNUM_VERIFY_NO_ERROR();

    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(image.data(), image.data()+image.pixelSize()*image.size().product()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}

void TextureGLTest::subImageRectangleBuffer() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    constexpr UnsignedByte zero[4*4*4] = {};
    constexpr UnsignedByte subData[] = { 0x00, 0x01, 0x02, 0x03,
                                         0x04, 0x05, 0x06, 0x07,
                                         0x08, 0x09, 0x0a, 0x0b,
                                         0x0c, 0x0d, 0x0e, 0x0f };
    Texture2D texture(Texture2D::Target::Rectangle);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(4), zero));
    texture.setSubImage(0, Vector2i(1),
        BufferImage2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(2), subData, BufferUsage::StaticDraw));

    MAGNUM_VERIFY_NO_ERROR();

    BufferImage2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    texture.image(0, image, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data<UnsignedByte>();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(std::vector<UnsignedByte>(imageData.begin(), imageData.end()), (std::vector<UnsignedByte>{
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
        0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
        0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
    }), TestSuite::Compare::Container);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::generateMipmap1D() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));

    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference1D(ColorFormat::RGBA, ColorType::UnsignedByte, 32));

    CORRADE_COMPARE(texture.imageSize(0), 32);
    CORRADE_COMPARE(texture.imageSize(1),  0);

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), 32);
    CORRADE_COMPARE(texture.imageSize(1), 16);
    CORRADE_COMPARE(texture.imageSize(2),  8);
    CORRADE_COMPARE(texture.imageSize(3),  4);
    CORRADE_COMPARE(texture.imageSize(4),  2);
    CORRADE_COMPARE(texture.imageSize(5),  1);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::generateMipmap2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    #endif

    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(32)));

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i( 0));
    #endif

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 1));

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

void TextureGLTest::generateMipmap3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(32)));

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i( 0));
    #endif

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 1));

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::generateMipmap1DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture2D texture(Texture2D::Target::Texture1DArray);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference2D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector2i(32)));

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i( 0));

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 1, 32));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::generateMipmap2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not supported."));
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture3D texture(Texture3D::Target::Texture2DArray);
    texture.setImage(0, TextureFormat::RGBA8,
        ImageReference3D(ColorFormat::RGBA, ColorType::UnsignedByte, Vector3i(32)));

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i( 0));
    #endif

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32, 32, 32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16, 16, 32));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8,  8, 32));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4,  4, 32));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2,  2, 32));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 1,  1, 32));

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::invalidateImage1D() {
    Texture1D texture;
    texture.setStorage(2, TextureFormat::RGBA8, 32);
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::invalidateImage2D() {
    Texture2D texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::invalidateImage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::invalidateImage1DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture2D texture(Texture2D::Target::Texture1DArray);
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::invalidateImage2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture3D texture(Texture3D::Target::Texture2DArray);
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::invalidateImage2DMultisample() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Multisample storage is not implemented yet.");
}

void TextureGLTest::invalidateImage2DMultisampleArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Multisample storage is not implemented yet.");
}

void TextureGLTest::invalidateImageRectangle() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    Texture2D texture(Texture2D::Target::Rectangle);
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateImage(0);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::invalidateSubImage1D() {
    Texture1D texture;
    texture.setStorage(2, TextureFormat::RGBA8, 32);
    texture.invalidateSubImage(1, 2, 8);

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

void TextureGLTest::invalidateSubImage2D() {
    Texture2D texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateSubImage(1, Vector2i(2), Vector2i(8));

    MAGNUM_VERIFY_NO_ERROR();
}

void TextureGLTest::invalidateSubImage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not supported."));
    #endif

    Texture3D texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::invalidateSubImage1DArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));

    Texture2D texture(Texture2D::Target::Texture1DArray);
    texture.setStorage(2, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateSubImage(1, Vector2i(2), Vector2i(8));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::invalidateSubImage2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not supported."));
    #endif

    Texture3D texture(Texture3D::Target::Texture2DArray);
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::invalidateSubImage2DMultisample() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Multisample storage is not implemented yet.");
}

void TextureGLTest::invalidateSubImage2DMultisampleArray() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));

    CORRADE_SKIP("Multisample storage is not implemented yet.");
}

void TextureGLTest::invalidateSubImageRectangle() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not supported."));

    Texture2D texture(Texture2D::Target::Rectangle);
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i(32));
    texture.invalidateSubImage(0, Vector2i(4), Vector2i(16));

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

}}

CORRADE_TEST_MAIN(Magnum::Test::TextureGLTest)
