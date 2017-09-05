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
#include "Magnum/CubeMapTexture.h"
#include "Magnum/Extensions.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/RenderbufferFormat.h"
#include "Magnum/Texture.h"
#include "Magnum/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/OpenGLTester.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/BufferImage.h"
#include "Magnum/MultisampleTexture.h"
#include "Magnum/TextureArray.h"
#endif
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/CubeMapTextureArray.h"
#endif
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/RectangleTexture.h"
#endif

namespace Magnum { namespace Test {

struct FramebufferGLTest: OpenGLTester {
    explicit FramebufferGLTest();

    void construct();
    void constructCopy();
    void constructMove();
    void wrap();

    void label();

    void attachRenderbuffer();
    void attachRenderbufferMultisample();

    #ifndef MAGNUM_TARGET_GLES
    void attachTexture1D();
    #endif
    void attachTexture2D();
    void attachTexture3D();
    #ifndef MAGNUM_TARGET_GLES
    void attachTexture1DArray();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void attachTexture2DArray();
    void attachTexture2DMultisample();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void attachTexture2DMultisampleArray();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void attachRectangleTexture();
    #endif
    void attachCubeMapTexture();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void attachCubeMapTextureArray();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void attachLayeredTexture3D();
    #ifndef MAGNUM_TARGET_GLES
    void attachLayeredTexture1DArray();
    #endif
    void attachLayeredTexture2DArray();
    void attachLayeredCubeMapTexture();
    void attachLayeredCubeMapTextureArray();
    void attachLayeredTexture2DMultisampleArray();
    #endif
    void detach();

    void multipleColorOutputs();

    void clear();
    #ifndef MAGNUM_TARGET_GLES2
    void clearColorI();
    void clearColorUI();
    void clearColorF();
    void clearDepth();
    void clearStencil();
    void clearDepthStencil();
    #endif
    void invalidate();
    #ifndef MAGNUM_TARGET_GLES2
    void invalidateSub();
    #endif
    void read();
    #ifndef MAGNUM_TARGET_GLES2
    void readBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void copyImageTexture1D();
    #endif
    void copyImageTexture2D();
    #ifndef MAGNUM_TARGET_GLES
    void copyImageTexture1DArray();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void copyImageRectangleTexture();
    #endif
    void copyImageCubeMapTexture();
    #ifndef MAGNUM_TARGET_GLES
    void copySubImageTexture1D();
    #endif
    void copySubImageTexture2D();
    void copySubImageTexture3D();
    #ifndef MAGNUM_TARGET_GLES
    void copySubImageTexture1DArray();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void copySubImageTexture2DArray();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void copySubImageRectangleTexture();
    #endif
    void copySubImageCubeMapTexture();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void copySubImageCubeMapTextureArray();
    #endif
    void blit();

    #ifdef MAGNUM_TARGET_GLES2
    private:
        TextureFormat rgbaFormatES2, depthStencilFormatES2;
    #endif
};

FramebufferGLTest::FramebufferGLTest() {
    addTests({&FramebufferGLTest::construct,
              &FramebufferGLTest::constructCopy,
              &FramebufferGLTest::constructMove,
              &FramebufferGLTest::wrap,

              &FramebufferGLTest::label,

              &FramebufferGLTest::attachRenderbuffer,
              &FramebufferGLTest::attachRenderbufferMultisample,

              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::attachTexture1D,
              #endif
              &FramebufferGLTest::attachTexture2D,
              &FramebufferGLTest::attachTexture3D,
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::attachTexture1DArray,
              #endif
              #ifndef MAGNUM_TARGET_GLES2
              &FramebufferGLTest::attachTexture2DArray,
              &FramebufferGLTest::attachTexture2DMultisample,
              #endif
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &FramebufferGLTest::attachTexture2DMultisampleArray,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::attachRectangleTexture,
              #endif
              &FramebufferGLTest::attachCubeMapTexture,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &FramebufferGLTest::attachCubeMapTextureArray,
              #endif
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &FramebufferGLTest::attachLayeredTexture3D,
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::attachLayeredTexture1DArray,
              #endif
              &FramebufferGLTest::attachLayeredTexture2DArray,
              &FramebufferGLTest::attachLayeredCubeMapTexture,
              &FramebufferGLTest::attachLayeredCubeMapTextureArray,
              &FramebufferGLTest::attachLayeredTexture2DMultisampleArray,
              #endif
              &FramebufferGLTest::detach,

              &FramebufferGLTest::multipleColorOutputs,

              &FramebufferGLTest::clear,
              #ifndef MAGNUM_TARGET_GLES2
              &FramebufferGLTest::clearColorI,
              &FramebufferGLTest::clearColorUI,
              &FramebufferGLTest::clearColorF,
              &FramebufferGLTest::clearDepth,
              &FramebufferGLTest::clearStencil,
              &FramebufferGLTest::clearDepthStencil,
              #endif
              &FramebufferGLTest::invalidate,
              #ifndef MAGNUM_TARGET_GLES2
              &FramebufferGLTest::invalidateSub,
              #endif
              &FramebufferGLTest::read,
              #ifndef MAGNUM_TARGET_GLES2
              &FramebufferGLTest::readBuffer,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::copyImageTexture1D,
              #endif
              &FramebufferGLTest::copyImageTexture2D,
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::copyImageTexture1DArray,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::copyImageRectangleTexture,
              #endif
              &FramebufferGLTest::copyImageCubeMapTexture,
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::copySubImageTexture1D,
              #endif
              &FramebufferGLTest::copySubImageTexture2D,
              &FramebufferGLTest::copySubImageTexture3D,
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::copySubImageTexture1DArray,
              #endif
              #ifndef MAGNUM_TARGET_GLES2
              &FramebufferGLTest::copySubImageTexture2DArray,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &FramebufferGLTest::copySubImageRectangleTexture,
              #endif
              &FramebufferGLTest::copySubImageCubeMapTexture,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &FramebufferGLTest::copySubImageCubeMapTextureArray,
              #endif
              &FramebufferGLTest::blit});

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::GL::EXT::texture_storage>()) {
        rgbaFormatES2 = TextureFormat::RGBA8;
        depthStencilFormatES2 = TextureFormat::Depth24Stencil8;
    } else {
        rgbaFormatES2 = TextureFormat::RGBA;
        depthStencilFormatES2 = TextureFormat::DepthStencil;
    }
    #endif
}

void FramebufferGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    {
        const Framebuffer framebuffer({{32, 16}, {128, 256}});

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(framebuffer.id() > 0);
        CORRADE_COMPARE(framebuffer.viewport(), Range2Di({32, 16}, {128, 256}));
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void FramebufferGLTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<Framebuffer, const Framebuffer&>{}));
    CORRADE_VERIFY(!(std::is_assignable<Framebuffer, const Framebuffer&>{}));
}

void FramebufferGLTest::constructMove() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Framebuffer a({{32, 16}, {128, 256}});
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    Framebuffer b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.viewport(), Range2Di({32, 16}, {128, 256}));

    Framebuffer c({{128, 256}, {32, 16}});
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.viewport(), Range2Di({32, 16}, {128, 256}));
}

void FramebufferGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    GLuint id;
    glGenFramebuffers(1, &id);

    /* Releasing won't delete anything */
    {
        auto framebuffer = Framebuffer::wrap(id, {}, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(framebuffer.release(), id);
    }

    /* ...so we can wrap it again */
    Framebuffer::wrap(id, {});
    glDeleteFramebuffers(1, &id);
}

void FramebufferGLTest::label() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Framebuffer framebuffer({{}, Vector2i(32)});

    CORRADE_COMPARE(framebuffer.label(), "");
    MAGNUM_VERIFY_NO_ERROR();

    framebuffer.setLabel("MyFramebuffer");
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(framebuffer.label(), "MyFramebuffer");
}

void FramebufferGLTest::attachRenderbuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    /* Separate depth and stencil renderbuffers are not supported (or at least
       on my NVidia, thus we need to do this juggling with one renderbuffer */
    Renderbuffer depthStencil;
    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::OES::packed_depth_stencil::string();
        #endif
        depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i(128));
    }
    #ifdef MAGNUM_TARGET_GLES2
    else depthStencil.setStorage(RenderbufferFormat::DepthComponent16, Vector2i(128));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depthStencil);

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        framebuffer.attachRenderbuffer(Framebuffer::BufferAttachment::Stencil, depthStencil);
    }

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}

void FramebufferGLTest::attachRenderbufferMultisample() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::GL::ANGLE::framebuffer_multisample>() &&
       !Context::current().isExtensionSupported<Extensions::GL::NV::framebuffer_multisample>())
        CORRADE_SKIP("Required extension is not available.");
    #endif

    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorageMultisample(Renderbuffer::maxSamples(), RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorageMultisample(Renderbuffer::maxSamples(), RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    #ifdef MAGNUM_TARGET_GLES2
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::OES::packed_depth_stencil);
    #endif

    Renderbuffer depthStencil;
    depthStencil.setStorageMultisample(Renderbuffer::maxSamples(), RenderbufferFormat::Depth24Stencil8, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               #ifndef MAGNUM_TARGET_GLES2
               .attachRenderbuffer(Framebuffer::BufferAttachment::DepthStencil, depthStencil);
               #else
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depthStencil)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Stencil, depthStencil);
               #endif

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::attachTexture1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));

    Texture1D color;
    color.setStorage(1, TextureFormat::RGBA8, 128);

    Texture1D depthStencil;
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, 128);

    Framebuffer framebuffer({{}, {128, 1}});
    framebuffer.attachTexture(Framebuffer::ColorAttachment(0), color, 0)
               .attachTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}
#endif

void FramebufferGLTest::attachTexture2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    MAGNUM_VERIFY_NO_ERROR();

    Framebuffer framebuffer({{}, Vector2i(128)});

    MAGNUM_VERIFY_NO_ERROR();

    Texture2D color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(1, TextureFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(1, rgbaFormatES2, Vector2i(128));
    #endif

    MAGNUM_VERIFY_NO_ERROR();

    framebuffer.attachTexture(Framebuffer::ColorAttachment(0), color, 0);

    MAGNUM_VERIFY_NO_ERROR();

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::OES::packed_depth_stencil::string();
        #endif

        /** @todo Is there any better way to select proper sized/unsized format on ES2? */
        Texture2D depthStencil;
        #ifndef MAGNUM_TARGET_GLES2
        depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, Vector2i(128));
        framebuffer.attachTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0);
        #else
        depthStencil.setStorage(1, depthStencilFormatES2, Vector2i(128));
        framebuffer.attachTexture(Framebuffer::BufferAttachment::Depth, depthStencil, 0)
                   .attachTexture(Framebuffer::BufferAttachment::Stencil, depthStencil, 0);
        #endif
    }

    #ifdef MAGNUM_TARGET_GLES2
    else if(Context::current().isExtensionSupported<Extensions::GL::OES::depth_texture>()) {
        Debug() << "Using" << Extensions::GL::OES::depth_texture::string();

        Texture2D depth;
        depth.setStorage(1, TextureFormat::DepthComponent16, Vector2i(128));
        framebuffer.attachTexture(Framebuffer::BufferAttachment::Depth, depth, 0);
    }
    #endif

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}

void FramebufferGLTest::attachTexture3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not available."));
    #endif

    Texture3D color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(1, TextureFormat::RGBA8, Vector3i(128));
    #else
    color.setStorage(1, rgbaFormatES2, Vector3i(128));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTextureLayer(Framebuffer::ColorAttachment(0), color, 0, 0);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::attachTexture1DArray() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not available."));

    Texture1DArray color;
    color.setStorage(1, TextureFormat::RGBA8, {128, 8});

    Texture1DArray depthStencil;
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, {128, 8});

    Framebuffer framebuffer({{}, {128, 1}});
    framebuffer.attachTextureLayer(Framebuffer::ColorAttachment(0), color, 0, 3)
               .attachTextureLayer(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0, 3);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void FramebufferGLTest::attachTexture2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not available."));
    #endif

    Texture2DArray color;
    color.setStorage(1, TextureFormat::RGBA8, {128, 128, 8});

    Texture2DArray depthStencil;
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, {128, 128, 8});

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTextureLayer(Framebuffer::ColorAttachment(0), color, 0, 3)
               .attachTextureLayer(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0, 3);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void FramebufferGLTest::attachTexture2DMultisample() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not available."));
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    MultisampleTexture2D color;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    color.setStorage(Math::min(4, MultisampleTexture2D::maxColorSamples()),
        TextureFormat::RGBA8, {128, 128});

    MultisampleTexture2D depthStencil;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    depthStencil.setStorage(Math::min(4, MultisampleTexture2D::maxDepthSamples()),
        TextureFormat::Depth24Stencil8, {128, 128});

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTexture(Framebuffer::ColorAttachment(0), color)
               .attachTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void FramebufferGLTest::attachTexture2DMultisampleArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_storage_multisample_2d_array>())
        CORRADE_SKIP(Extensions::GL::OES::texture_storage_multisample_2d_array::string() + std::string(" is not available."));
    #endif

    MultisampleTexture2DArray color;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    color.setStorage(Math::min(4, MultisampleTexture2DArray::maxColorSamples()),
        TextureFormat::RGBA8, {128, 128, 8});

    MultisampleTexture2DArray depthStencil;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    depthStencil.setStorage(Math::min(4, MultisampleTexture2DArray::maxDepthSamples()),
        TextureFormat::Depth24Stencil8, {128, 128, 8});

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTextureLayer(Framebuffer::ColorAttachment(0), color, 3)
               .attachTextureLayer(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 3);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::attachRectangleTexture() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not available."));

    RectangleTexture color;
    color.setStorage(TextureFormat::RGBA8, Vector2i(128));

    RectangleTexture depthStencil;
    depthStencil.setStorage(TextureFormat::Depth24Stencil8, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTexture(Framebuffer::ColorAttachment(0), color)
               .attachTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}
#endif

void FramebufferGLTest::attachCubeMapTexture() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});

    CubeMapTexture color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(1, TextureFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(1, rgbaFormatES2, Vector2i(128));
    #endif
    framebuffer.attachCubeMapTexture(Framebuffer::ColorAttachment(0), color, CubeMapCoordinate::NegativeZ, 0);

    CubeMapTexture depthStencil;

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::OES::packed_depth_stencil::string();
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, Vector2i(128));
        framebuffer.attachCubeMapTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil, CubeMapCoordinate::NegativeZ, 0);
        #else
        depthStencil.setStorage(1, depthStencilFormatES2, Vector2i(128));
        framebuffer.attachCubeMapTexture(Framebuffer::BufferAttachment::Depth, depthStencil, CubeMapCoordinate::NegativeZ, 0)
                   .attachCubeMapTexture(Framebuffer::BufferAttachment::Stencil, depthStencil, CubeMapCoordinate::NegativeZ, 0);
        #endif
    }

    #ifdef MAGNUM_TARGET_GLES2
    else if(Context::current().isExtensionSupported<Extensions::GL::OES::depth_texture>()) {
        Debug() << "Using" << Extensions::GL::OES::depth_texture::string();

        depthStencil.setStorage(1, TextureFormat::DepthComponent16, Vector2i(128));
        framebuffer.attachCubeMapTexture(Framebuffer::BufferAttachment::Depth, depthStencil, CubeMapCoordinate::NegativeZ, 0);
    }
    #endif

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void FramebufferGLTest::attachCubeMapTextureArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not available."));
    #endif

    CubeMapTextureArray color;
    color.setStorage(1, TextureFormat::RGBA8, {128, 128, 18});

    CubeMapTextureArray depthStencil;
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, {128, 128, 18});

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTextureLayer(Framebuffer::ColorAttachment(0), color, 0, 3)
               .attachTextureLayer(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0, 3);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void FramebufferGLTest::attachLayeredTexture3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        CORRADE_SKIP(Extensions::GL::ARB::geometry_shader4::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::geometry_shader>())
        CORRADE_SKIP(Extensions::GL::EXT::geometry_shader::string() + std::string(" is not available."));
    #endif

    Texture3D color;
    color.setStorage(1, TextureFormat::RGBA8, Vector3i{128});

    Framebuffer framebuffer{{{}, Vector2i{128}}};
    framebuffer.attachLayeredTexture(Framebuffer::ColorAttachment{0}, color, 0);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::attachLayeredTexture1DArray() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        CORRADE_SKIP(Extensions::GL::ARB::geometry_shader4::string() + std::string(" is not available."));

    Texture1DArray color;
    color.setStorage(1, TextureFormat::RGBA8, {128, 8});

    Texture1DArray depthStencil;
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, {128, 8});

    Framebuffer framebuffer{{{}, {128, 1}}};
    framebuffer.attachLayeredTexture(Framebuffer::ColorAttachment{0}, color, 0)
               .attachLayeredTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}
#endif

void FramebufferGLTest::attachLayeredTexture2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        CORRADE_SKIP(Extensions::GL::ARB::geometry_shader4::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::geometry_shader>())
        CORRADE_SKIP(Extensions::GL::EXT::geometry_shader::string() + std::string(" is not available."));
    #endif

    Texture2DArray color;
    color.setStorage(1, TextureFormat::RGBA8, {128, 128, 8});

    Texture2DArray depthStencil;
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, {128, 128, 8});

    Framebuffer framebuffer{{{}, Vector2i{128}}};
    framebuffer.attachLayeredTexture(Framebuffer::ColorAttachment{0}, color, 0)
               .attachLayeredTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}

void FramebufferGLTest::attachLayeredCubeMapTexture() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        CORRADE_SKIP(Extensions::GL::ARB::geometry_shader4::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::geometry_shader>())
        CORRADE_SKIP(Extensions::GL::EXT::geometry_shader::string() + std::string(" is not available."));
    #endif

    CubeMapTexture color;
    color.setStorage(1, TextureFormat::RGBA8, Vector2i{128});

    CubeMapTexture depthStencil;
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, Vector2i{128});

    Framebuffer framebuffer{{{}, Vector2i{128}}};
    framebuffer.attachLayeredTexture(Framebuffer::ColorAttachment{0}, color, 0)
               .attachLayeredTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}

void FramebufferGLTest::attachLayeredCubeMapTextureArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        CORRADE_SKIP(Extensions::GL::ARB::geometry_shader4::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::geometry_shader>())
        CORRADE_SKIP(Extensions::GL::EXT::geometry_shader::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not available."));
    #endif

    CubeMapTextureArray color;
    color.setStorage(1, TextureFormat::RGBA8, {128, 128, 18});

    CubeMapTextureArray depthStencil;
    depthStencil.setStorage(1, TextureFormat::Depth24Stencil8, {128, 128, 18});

    Framebuffer framebuffer{{{}, Vector2i{128}}};
    framebuffer.attachLayeredTexture(Framebuffer::ColorAttachment{0}, color, 0)
               .attachLayeredTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil, 0);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}

void FramebufferGLTest::attachLayeredTexture2DMultisampleArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        CORRADE_SKIP(Extensions::GL::ARB::geometry_shader4::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_multisample>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_multisample::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::geometry_shader>())
        CORRADE_SKIP(Extensions::GL::EXT::geometry_shader::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_storage_multisample_2d_array>())
        CORRADE_SKIP(Extensions::GL::OES::texture_storage_multisample_2d_array::string() + std::string(" is not available."));
    #endif

    MultisampleTexture2DArray color;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    color.setStorage(Math::min(4, MultisampleTexture2DArray::maxColorSamples()),
        TextureFormat::RGBA8, {128, 128, 8});

    MultisampleTexture2DArray depthStencil;
    /* Mesa software implementation supports only 1 sample so we have to clamp */
    depthStencil.setStorage(Math::min(4, MultisampleTexture2DArray::maxDepthSamples()),
        TextureFormat::Depth24Stencil8, {128, 128, 8});

    Framebuffer framebuffer{{{}, Vector2i{128}}};
    framebuffer.attachLayeredTexture(Framebuffer::ColorAttachment{0}, color)
               .attachLayeredTexture(Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}
#endif

void FramebufferGLTest::detach() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.detach(Framebuffer::ColorAttachment(0))
               .detach(Framebuffer::BufferAttachment::Depth)
               .detach(Framebuffer::BufferAttachment::Stencil);

    MAGNUM_VERIFY_NO_ERROR();
}

void FramebufferGLTest::multipleColorOutputs() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::draw_buffers>() &&
       !Context::current().isExtensionSupported<Extensions::GL::NV::draw_buffers>())
        CORRADE_SKIP("No required extension available.");
    #endif

    Texture2D color1;
    #ifndef MAGNUM_TARGET_GLES2
    color1.setStorage(1, TextureFormat::RGBA8, Vector2i(128));
    #else
    color1.setStorage(1, rgbaFormatES2, Vector2i(128));
    #endif

    Texture2D color2;
    #ifndef MAGNUM_TARGET_GLES2
    color2.setStorage(1, TextureFormat::RGBA8, Vector2i(128));
    #else
    color2.setStorage(1, rgbaFormatES2, Vector2i(128));
    #endif

    Renderbuffer depth;
    depth.setStorage(RenderbufferFormat::DepthComponent16, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachTexture(Framebuffer::ColorAttachment(0), color1, 0)
               .attachTexture(Framebuffer::ColorAttachment(1), color2, 0)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depth)
               .mapForDraw({{0, Framebuffer::ColorAttachment(1)},
                            {1, Framebuffer::ColorAttachment(0)},
                            {2, Framebuffer::DrawAttachment::None}});

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::GL::NV::read_buffer>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::NV::read_buffer::string();
        #endif
        framebuffer.mapForRead(Framebuffer::ColorAttachment(1));
    }

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
}

void FramebufferGLTest::clear() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    /* Separate depth and stencil renderbuffers are not supported (or at least
       on my NVidia, thus we need to do this juggling with one renderbuffer */
    Renderbuffer depthStencil;
    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::OES::packed_depth_stencil::string();
        #endif
        depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i(128));
    }
    #ifdef MAGNUM_TARGET_GLES2
    else depthStencil.setStorage(RenderbufferFormat::DepthComponent16, Vector2i(128));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depthStencil);

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        framebuffer.attachRenderbuffer(Framebuffer::BufferAttachment::Stencil, depthStencil);
    }

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    framebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth|FramebufferClear::Stencil);

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES2
void FramebufferGLTest::clearColorI() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GL300))
        CORRADE_SKIP("GL 3.0 is not available.");
    #endif

    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8I, Vector2i{16});

    Framebuffer framebuffer({{}, Vector2i{16}});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    framebuffer.clearColor(0, Vector4i{-124, 67, 37, 17});

    MAGNUM_VERIFY_NO_ERROR();

    Image2D colorImage = framebuffer.read({{}, Vector2i{1}},
        {PixelFormat::RGBAInteger, PixelType::Int});

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(colorImage.data<Vector4i>()[0], (Vector4i{-124, 67, 37, 17}));
}

void FramebufferGLTest::clearColorUI() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GL300))
        CORRADE_SKIP("GL 3.0 is not available.");
    #endif

    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8UI, Vector2i{16});

    Framebuffer framebuffer({{}, Vector2i{16}});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    framebuffer.clearColor(0, Vector4ui{240, 67, 37, 17});

    MAGNUM_VERIFY_NO_ERROR();

    Image2D colorImage = framebuffer.read({{}, Vector2i{1}},
        {PixelFormat::RGBAInteger, PixelType::UnsignedInt});

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(colorImage.data<Vector4ui>()[0], (Vector4ui{240, 67, 37, 17}));
}

void FramebufferGLTest::clearColorF() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GL300))
        CORRADE_SKIP("GL 3.0 is not available.");
    #endif

    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{16});

    Framebuffer framebuffer({{}, Vector2i{16}});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    framebuffer.clearColor(0, Math::unpack<Color4>(Color4ub{128, 64, 32, 17}));

    MAGNUM_VERIFY_NO_ERROR();

    Image2D colorImage = framebuffer.read({{}, Vector2i{1}},
        {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(colorImage.data<Color4ub>()[0], (Color4ub{128, 64, 32, 17}));
}

void FramebufferGLTest::clearDepth() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GL300))
        CORRADE_SKIP("GL 3.0 is not available.");
    #endif

    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{16});

    /* Separate depth and stencil renderbuffers are not supported (or at least
       on my NVidia, thus we need to do this juggling with one renderbuffer */
    Renderbuffer depthStencil;
    depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i{16});

    Framebuffer framebuffer({{}, Vector2i{16}});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    framebuffer.clearDepth(Math::unpack<Float, UnsignedShort>(48352));

    MAGNUM_VERIFY_NO_ERROR();

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::NV::read_depth>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES
        Debug() << "Using" << Extensions::GL::NV::read_depth::string();
        #endif

        Image2D depthImage = framebuffer.read({{}, Vector2i{1}}, {PixelFormat::DepthComponent, PixelType::UnsignedShort});

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_COMPARE(depthImage.data<UnsignedShort>()[0], 48352);
    }
}

void FramebufferGLTest::clearStencil() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GL300))
        CORRADE_SKIP("GL 3.0 is not available.");
    #endif

    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{16});

    Renderbuffer depthStencil;
    depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i{16});

    Framebuffer framebuffer({{}, Vector2i{16}});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    framebuffer.clearStencil(67);

    MAGNUM_VERIFY_NO_ERROR();

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::NV::read_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES
        Debug() << "Using" << Extensions::GL::NV::read_stencil::string();
        #endif

        Image2D stencilImage = framebuffer.read({{}, Vector2i{1}}, {PixelFormat::StencilIndex, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_COMPARE(stencilImage.data<UnsignedByte>()[0], 67);
    }
}

void FramebufferGLTest::clearDepthStencil() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GL300))
        CORRADE_SKIP("GL 3.0 is not available.");
    #endif

    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{16});

    /* Separate depth and stencil renderbuffers are not supported (or at least
       on my NVidia, thus we need to do this juggling with one renderbuffer */
    Renderbuffer depthStencil;
    depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i{16});

    Framebuffer framebuffer({{}, Vector2i{16}});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    framebuffer.clearDepthStencil(Math::unpack<Float, UnsignedShort>(48352), 67);

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::NV::read_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES
        Debug() << "Using" << Extensions::GL::NV::read_depth_stencil::string();
        #endif

        Image2D depthStencilImage = framebuffer.read({{}, Vector2i{1}}, {PixelFormat::DepthStencil, PixelType::UnsignedInt248});

        MAGNUM_VERIFY_NO_ERROR();
        /** @todo This will probably fail on different systems */
        CORRADE_COMPARE(depthStencilImage.data<UnsignedInt>()[0] >> 8, 12378300);
        CORRADE_COMPARE(depthStencilImage.data<UnsignedByte>()[0], 67);
    }
}
#endif

void FramebufferGLTest::invalidate() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    Renderbuffer stencil;
    stencil.setStorage(RenderbufferFormat::StencilIndex8, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Stencil, stencil);

    MAGNUM_VERIFY_NO_ERROR();

    framebuffer.invalidate({Framebuffer::InvalidationAttachment::Depth, Framebuffer::ColorAttachment(0)});

    MAGNUM_VERIFY_NO_ERROR();
}

#ifndef MAGNUM_TARGET_GLES2
void FramebufferGLTest::invalidateSub() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));

    Renderbuffer depth;
    depth.setStorage(RenderbufferFormat::DepthComponent16, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depth);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    framebuffer.invalidate({Framebuffer::InvalidationAttachment::Depth, Framebuffer::ColorAttachment(0)},
                           {{32, 16}, {79, 64}});

    MAGNUM_VERIFY_NO_ERROR();
}
#endif

namespace {
    const auto DataStorage = PixelStorage{}.setSkip({0, 16, 0});
    const std::size_t DataOffset = 16*8;
}

void FramebufferGLTest::read() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    #ifndef MAGNUM_TARGET_GLES2
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    color.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    /* Separate depth and stencil renderbuffers are not supported (or at least
       on my NVidia, thus we need to do this juggling with one renderbuffer */
    Renderbuffer depthStencil;
    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES2
        Debug() << "Using" << Extensions::GL::OES::packed_depth_stencil::string();
        #endif
        depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i(128));
    }
    #ifdef MAGNUM_TARGET_GLES2
    else depthStencil.setStorage(RenderbufferFormat::DepthComponent16, Vector2i(128));
    #endif

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::Depth, depthStencil);

    #ifdef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::GL::OES::packed_depth_stencil>())
    #endif
    {
        framebuffer.attachRenderbuffer(Framebuffer::BufferAttachment::Stencil, depthStencil);
    }

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    Renderer::setClearColor(Math::unpack<Color4>(Color4ub(128, 64, 32, 17)));
    Renderer::setClearDepth(Math::unpack<Float, UnsignedShort>(48352));
    Renderer::setClearStencil(67);
    framebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth|FramebufferClear::Stencil);

    Image2D colorImage = framebuffer.read(Range2Di::fromSize({16, 8}, {8, 16}),
        {DataStorage, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(colorImage.size(), Vector2i(8, 16));
    CORRADE_COMPARE(colorImage.data().size(), (DataOffset + 8*16)*sizeof(Color4ub));
    CORRADE_COMPARE(colorImage.data<Color4ub>()[DataOffset], Color4ub(128, 64, 32, 17));

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::NV::read_depth>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES
        Debug() << "Using" << Extensions::GL::NV::read_depth::string();
        #endif

        Image2D depthImage = framebuffer.read({{}, Vector2i{1}}, {PixelFormat::DepthComponent, PixelType::UnsignedShort});

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_COMPARE(depthImage.data<UnsignedShort>()[0], 48352);
    }

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::NV::read_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES
        Debug() << "Using" << Extensions::GL::NV::read_stencil::string();
        #endif

        Image2D stencilImage = framebuffer.read({{}, Vector2i{1}}, {PixelFormat::StencilIndex, PixelType::UnsignedByte});

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_COMPARE(stencilImage.data<UnsignedByte>()[0], 67);
    }

    #ifdef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::GL::NV::read_depth_stencil>())
    #endif
    {
        #ifdef MAGNUM_TARGET_GLES
        Debug() << "Using" << Extensions::GL::NV::read_depth_stencil::string();
        #endif

        Image2D depthStencilImage = framebuffer.read({{}, Vector2i{1}}, {PixelFormat::DepthStencil, PixelType::UnsignedInt248});

        MAGNUM_VERIFY_NO_ERROR();
        /** @todo This will probably fail on different systems */
        CORRADE_COMPARE(depthStencilImage.data<UnsignedInt>()[0] >> 8, 12378300);
        CORRADE_COMPARE(depthStencilImage.data<UnsignedByte>()[0], 67);
    }
}

#ifndef MAGNUM_TARGET_GLES2
void FramebufferGLTest::readBuffer() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));

    Renderbuffer depthStencil;
    depthStencil.setStorage(RenderbufferFormat::Depth24Stencil8, Vector2i(128));

    Framebuffer framebuffer({{}, Vector2i(128)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), color)
               .attachRenderbuffer(Framebuffer::BufferAttachment::DepthStencil, depthStencil);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(framebuffer.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    Renderer::setClearColor(Math::unpack<Color4>(Color4ub(128, 64, 32, 17)));
    Renderer::setClearDepth(Math::unpack<Float, UnsignedShort>(48352));
    Renderer::setClearStencil(67);
    framebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth|FramebufferClear::Stencil);

    BufferImage2D colorImage = framebuffer.read(Range2Di::fromSize({16, 8}, {8, 16}),
        {DataStorage, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    CORRADE_COMPARE(colorImage.size(), Vector2i(8, 16));

    MAGNUM_VERIFY_NO_ERROR();
    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    auto colorData = colorImage.buffer().data();
    CORRADE_COMPARE(colorData.size(), (DataOffset + 8*16)*sizeof(Color4ub));
    CORRADE_COMPARE(Containers::arrayCast<Color4ub>(colorData)[DataOffset], Color4ub(128, 64, 32, 17));
    #endif
}
#endif

namespace {
    constexpr char StorageData[]{
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
    };

    constexpr char ZeroStorage[4*4*4*6]{};
}

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::copyImageTexture1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));

    Texture2D storage;
    storage.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    Texture1D texture;
    fb.copyImage(Range2Di::fromSize(Vector2i{1}, {2, 1}), texture, 0, TextureFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0)[0], 2);
    CORRADE_COMPARE_AS(texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b}}),
        TestSuite::Compare::Container);
}
#endif

void FramebufferGLTest::copyImageTexture2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Texture2D storage;
    storage.setStorage(1,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::RGBA8,
        #else
        rgbaFormatES2,
        #endif
        Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    Texture2D texture;
    fb.copyImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, 0,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::RGBA8
        #else
        rgbaFormatES2
        #endif
        );

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i{2});
    CORRADE_COMPARE_AS(texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
            0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b}}),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::copyImageTexture1DArray() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not available."));

    Texture2D storage;
    storage.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    Texture1DArray texture;
    fb.copyImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, 0, TextureFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), Vector2i{2});
    CORRADE_COMPARE_AS(texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
            0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b}}),
        TestSuite::Compare::Container);
}

void FramebufferGLTest::copyImageRectangleTexture() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not available."));

    Texture2D storage;
    storage.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    RectangleTexture texture;
    fb.copyImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, TextureFormat::RGBA8);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(texture.imageSize(), Vector2i{2});
    CORRADE_COMPARE_AS(texture.image({PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
            0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b}}),
        TestSuite::Compare::Container);
}
#endif

void FramebufferGLTest::copyImageCubeMapTexture() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Texture2D storage;
    storage.setStorage(1,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::RGBA8,
        #else
        rgbaFormatES2,
        #endif
        Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    CubeMapTexture texture;
    fb.copyImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, CubeMapCoordinate::PositiveX, 0,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::RGBA8
        #else
        rgbaFormatES2
        #endif
        );

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i{2});
    CORRADE_COMPARE_AS(texture.image(CubeMapCoordinate::PositiveX, 0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
            0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b}}),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::copySubImageTexture1D() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));

    Texture2D storage;
    storage.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    Texture1D texture;
    texture.setStorage(1, TextureFormat::RGBA8, 4)
        .setSubImage(0, {}, ImageView1D{PixelFormat::RGBA, PixelType::UnsignedByte, 4, ZeroStorage});
    fb.copySubImage(Range2Di::fromSize(Vector2i{1}, {2, 1}), texture, 0, 1);

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE_AS(texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00, 0x00, 0x00, 0x00}}),
        TestSuite::Compare::Container);
}
#endif

void FramebufferGLTest::copySubImageTexture2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Texture2D storage;
    storage.setStorage(1,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::RGBA8,
        #else
        rgbaFormatES2,
        #endif
        Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    Texture2D texture;
    texture.setStorage(1,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::RGBA8,
        #else
        rgbaFormatES2,
        #endif
        Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, ZeroStorage});
    fb.copySubImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, 0, Vector2i{1});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}),
        TestSuite::Compare::Container);
    #endif
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void FramebufferGLTest::copySubImageTexture3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::GL::OES::texture_3D>())
        CORRADE_SKIP(Extensions::GL::OES::texture_3D::string() + std::string(" is not available."));
    #endif

    Texture2D storage;
    storage.setStorage(1,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::RGBA8,
        #else
        rgbaFormatES2,
        #endif
        Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    Texture3D texture;
    texture.setStorage(1,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::RGBA8,
        #else
        rgbaFormatES2,
        #endif
        {4, 4, 2})
        .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4, 2}, ZeroStorage});
    fb.copySubImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, 0, Vector3i{1});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}),
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::copySubImageTexture1DArray() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not available."));

    Texture2D storage;
    storage.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    Texture1DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, ZeroStorage});
    fb.copySubImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, 0, Vector2i{1});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE_AS(texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}),
        TestSuite::Compare::Container);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void FramebufferGLTest::copySubImageTexture2DArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_array::string() + std::string(" is not available."));
    #endif

    Texture2D storage;
    storage.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    Texture2DArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {4, 4, 2})
        .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4, 2}, ZeroStorage});
    fb.copySubImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, 0, Vector3i{1});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}),
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void FramebufferGLTest::copySubImageRectangleTexture() {
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_rectangle>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_rectangle::string() + std::string(" is not available."));

    Texture2D storage;
    storage.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    RectangleTexture texture;
    texture.setStorage(TextureFormat::RGBA8, Vector2i{4})
        .setSubImage({}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, ZeroStorage});
    fb.copySubImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, Vector2i{1});

    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE_AS(texture.image({PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}),
        TestSuite::Compare::Container);
}
#endif

void FramebufferGLTest::copySubImageCubeMapTexture() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #endif

    Texture2D storage;
    storage.setStorage(1,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::RGBA8,
        #else
        rgbaFormatES2,
        #endif
        Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    CubeMapTexture texture;
    texture.setStorage(1,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::RGBA8,
        #else
        rgbaFormatES2,
        #endif
        Vector2i{4})
        .setSubImage(CubeMapCoordinate::NegativeY, 0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, ZeroStorage});
    fb.copySubImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, 0, {1, 1, 3});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(texture.image(CubeMapCoordinate::NegativeY, 0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}),
        TestSuite::Compare::Container);
    #endif
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void FramebufferGLTest::copySubImageCubeMapTextureArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::ARB::texture_cube_map_array::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_cube_map_array::string() + std::string(" is not available."));
    #endif

    Texture2D storage;
    storage.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
        .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, StorageData});

    Framebuffer fb{{{}, Vector2i{4}}};
    fb.attachTexture(Framebuffer::ColorAttachment{0}, storage, 0);

    CubeMapTextureArray texture;
    texture.setStorage(1, TextureFormat::RGBA8, {4, 4, 6})
        .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, {4, 4, 6}, ZeroStorage});
    fb.copySubImage(Range2Di::fromSize(Vector2i{1}, Vector2i{2}), texture, 0, {1, 1, 3});

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE_AS(texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}).release(),
        (Containers::Array<char>{Containers::InPlaceInit, {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}),
        TestSuite::Compare::Container);
    #endif
}
#endif

void FramebufferGLTest::blit() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::GL::ARB::framebuffer_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::GL::NV::framebuffer_blit>() &&
       !Context::current().isExtensionSupported<Extensions::GL::ANGLE::framebuffer_blit>())
        CORRADE_SKIP("Required extension is not available.");
    #endif

    Renderbuffer colorA, colorB;
    #ifndef MAGNUM_TARGET_GLES2
    colorA.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    colorB.setStorage(RenderbufferFormat::RGBA8, Vector2i(128));
    #else
    colorA.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    colorB.setStorage(RenderbufferFormat::RGBA4, Vector2i(128));
    #endif

    Framebuffer a({{}, Vector2i(128)}), b({{}, Vector2i(128)});;
    a.attachRenderbuffer(Framebuffer::ColorAttachment(0), colorA);
    b.attachRenderbuffer(Framebuffer::ColorAttachment(0), colorB);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(a.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(a.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);
    CORRADE_COMPARE(b.checkStatus(FramebufferTarget::Read), Framebuffer::Status::Complete);
    CORRADE_COMPARE(b.checkStatus(FramebufferTarget::Draw), Framebuffer::Status::Complete);

    /* Clear first with some color and second with another */
    Renderer::setClearColor(Math::unpack<Color4>(Color4ub(128, 64, 32, 17)));
    a.clear(FramebufferClear::Color);
    Renderer::setClearColor({});
    b.clear(FramebufferClear::Color);

    /* The framebuffer should be black before */
    Image2D imageBefore = b.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(imageBefore.data<Color4ub>()[0], Color4ub());

    /* And have given color after */
    Framebuffer::blit(a, b, a.viewport(), FramebufferBlit::Color);
    Image2D imageAfter = b.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(imageAfter.data<Color4ub>()[0], Color4ub(128, 64, 32, 17));
}

}}

CORRADE_TEST_MAIN(Magnum::Test::FramebufferGLTest)
