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

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/ImageFormat.h"
#include "Magnum/MultisampleTexture.h"
#include "Magnum/OpenGLTester.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Functions.h"

namespace Magnum { namespace Test {

struct MultisampleTextureGLTest: OpenGLTester {
    explicit MultisampleTextureGLTest();

    void construct2D();
    void construct2DArray();

    void wrap2D();
    void wrap2DArray();

    void bind2D();
    void bind2DArray();

    void bindImage2D();
    void bindImage2DArray();

    void storage2D();
    void storage2DArray();

    void invalidateImage2D();
    void invalidateImage2DArray();

    void invalidateSubImage2D();
    void invalidateSubImage2DArray();
};

MultisampleTextureGLTest::MultisampleTextureGLTest() {
    addTests({&MultisampleTextureGLTest::construct2D,
              &MultisampleTextureGLTest::construct2DArray,

              &MultisampleTextureGLTest::wrap2D,
              &MultisampleTextureGLTest::wrap2DArray,

              &MultisampleTextureGLTest::bind2D,
              &MultisampleTextureGLTest::bind2DArray,

              &MultisampleTextureGLTest::bindImage2D,
              &MultisampleTextureGLTest::bindImage2DArray,

              &MultisampleTextureGLTest::storage2D,
              &MultisampleTextureGLTest::storage2DArray,

              &MultisampleTextureGLTest::invalidateImage2D,
              &MultisampleTextureGLTest::invalidateImage2DArray,

              &MultisampleTextureGLTest::invalidateSubImage2D,
              &MultisampleTextureGLTest::invalidateSubImage2DArray});
}

void MultisampleTextureGLTest::construct2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    {
        MultisampleTexture2D texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::construct2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_storage_multisample_2d_array>())
        CORRADE_SKIP(Extensions::GL::OES::texture_storage_multisample_2d_array::string() + std::string(" is not supported."));
    #endif

    {
        MultisampleTexture2DArray texture;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::wrap2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string{" is not supported."});
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = MultisampleTexture2D::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    MultisampleTexture2D::wrap(id);
    glDeleteTextures(1, &id);
}

void MultisampleTextureGLTest::wrap2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_storage_multisample_2d_array>())
        CORRADE_SKIP(Extensions::GL::OES::texture_storage_multisample_2d_array::string() + std::string(" is not supported."));
    #endif

    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = MultisampleTexture2DArray::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    MultisampleTexture2DArray::wrap(id);
    glDeleteTextures(1, &id);
}

void MultisampleTextureGLTest::bind2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    MultisampleTexture2D texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::bind2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_storage_multisample_2d_array>())
        CORRADE_SKIP(Extensions::GL::OES::texture_storage_multisample_2d_array::string() + std::string(" is not supported."));
    #endif

    MultisampleTexture2DArray texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::bindImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::GL::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_storage_multisample_2d_array>())
        CORRADE_SKIP(Extensions::GL::OES::texture_storage_multisample_2d_array::string() + std::string(" is not supported."));
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    MultisampleTexture2D texture;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    texture.setStorage(Math::min(4, MultisampleTexture2D::maxColorSamples()),
            TextureFormat::RGBA8, Vector2i{32})
        .bindImage(2, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImage(2);

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

void MultisampleTextureGLTest::bindImage2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::GL::ARB::shader_image_load_store::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_storage_multisample_2d_array>())
        CORRADE_SKIP(Extensions::GL::OES::texture_storage_multisample_2d_array::string() + std::string(" is not supported."));
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    MultisampleTexture2DArray texture;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    texture.setStorage(Math::min(4, MultisampleTexture2DArray::maxColorSamples()),
            TextureFormat::RGBA8, {32, 32, 4})
        .bindImage(2, 1, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    texture.bindImageLayered(3, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    AbstractTexture::unbindImage(2);
    AbstractTexture::unbindImage(3);

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_ERROR();
    #endif
}

void MultisampleTextureGLTest::storage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    MultisampleTexture2D texture;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    texture.setStorage(Math::min(4, MultisampleTexture2D::maxColorSamples()),
        TextureFormat::RGBA8, {16, 16});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(), Vector2i(16, 16));

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::storage2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_storage_multisample_2d_array>())
        CORRADE_SKIP(Extensions::GL::OES::texture_storage_multisample_2d_array::string() + std::string(" is not supported."));
    #endif

    MultisampleTexture2DArray texture;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    texture.setStorage(Math::min(4, MultisampleTexture2DArray::maxColorSamples()),
        TextureFormat::RGBA8, {16, 16, 5});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(), Vector3i(16, 16, 5));

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::invalidateImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    MultisampleTexture2D texture;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    texture.setStorage(Math::min(4, MultisampleTexture2D::maxColorSamples()),
        TextureFormat::RGBA8, {16, 16});
    texture.invalidateImage();

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::invalidateImage2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_storage_multisample_2d_array>())
        CORRADE_SKIP(Extensions::GL::OES::texture_storage_multisample_2d_array::string() + std::string(" is not supported."));
    #endif

    MultisampleTexture2DArray texture;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    texture.setStorage(Math::min(4, MultisampleTexture2DArray::maxColorSamples()),
        TextureFormat::RGBA8, {16, 16, 5});
    texture.invalidateImage();

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::invalidateSubImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    MultisampleTexture2D texture;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    texture.setStorage(Math::min(4, MultisampleTexture2D::maxColorSamples()),
        TextureFormat::RGBA8, {16, 16});
    texture.invalidateSubImage({3, 4}, {5, 6});

    MAGNUM_VERIFY_NO_ERROR();
}

void MultisampleTextureGLTest::invalidateSubImage2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not supported."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_storage_multisample_2d_array>())
        CORRADE_SKIP(Extensions::GL::OES::texture_storage_multisample_2d_array::string() + std::string(" is not supported."));
    #endif

    MultisampleTexture2DArray texture;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    texture.setStorage(Math::min(4, MultisampleTexture2DArray::maxColorSamples()),
        TextureFormat::RGBA8, {16, 16, 5});
    texture.invalidateSubImage({3, 4, 1}, {5, 6, 3});

    MAGNUM_VERIFY_NO_ERROR();
}

}}

CORRADE_TEST_MAIN(Magnum::Test::MultisampleTextureGLTest)
