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

#include <sstream>
#include <Corrade/Containers/ScopeGuard.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/Screenshot.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"

#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/DebugOutput.h"
#endif

#ifdef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#endif

#include "configure.h"

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct ScreenshotGLTest: GL::OpenGLTester {
    explicit ScreenshotGLTest();

    void rgba8();
    void r8();
    void unknownFormat();
    void pluginLoadFailed();
    void saveFailed();

    private:
        PluginManager::Manager<Trade::AbstractImageConverter> _converterManager{"nonexistent"};
        PluginManager::Manager<Trade::AbstractImporter> _importerManager{"nonexistent"};
};

ScreenshotGLTest::ScreenshotGLTest() {
    addTests({&ScreenshotGLTest::rgba8,
              &ScreenshotGLTest::r8,
              &ScreenshotGLTest::unknownFormat,
              &ScreenshotGLTest::pluginLoadFailed,
              &ScreenshotGLTest::saveFailed});

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_converterManager.load(ANYIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_converterManager.load(TGAIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_importerManager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

using namespace Math::Literals;

constexpr Color4ub DataRgba8[]{
    0x11223344_rgba, 0x22334455_rgba, 0x33445566_rgba, 0x44556677_rgba,
    0x55667788_rgba, 0x66778899_rgba, 0x778899aa_rgba, 0x8899aabb_rgba,
    0x99aabbcc_rgba, 0xaabbccdd_rgba, 0xbbccddee_rgba, 0xccddeeff_rgba
};

void ScreenshotGLTest::rgba8() {
    if(!(_converterManager.loadState("AnyImageConverter") & PluginManager::LoadState::Loaded) ||
       !(_converterManager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter / TgaImageConverter plugins not found.");

    ImageView2D rgba{PixelFormat::RGBA8Unorm, {4, 3}, DataRgba8};

    GL::Texture2D texture;
    texture.setStorage(1,
            #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
            GL::TextureFormat::RGBA8,
            #else
            GL::TextureFormat::RGBA,
            #endif
            {4, 3})
        .setSubImage(0, {}, rgba);
    GL::Framebuffer framebuffer{{{}, {4, 3}}};
    framebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, texture, 0);

    CORRADE_COMPARE(framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);

    std::string file = Utility::Directory::join(SCREENSHOTTEST_SAVE_DIR, "image.tga");
    if(Utility::Directory::exists(file))
        CORRADE_VERIFY(Utility::Directory::rm(file));
    else
        CORRADE_VERIFY(Utility::Directory::mkpath(SCREENSHOTTEST_SAVE_DIR));

    std::ostringstream out;
    bool succeeded;
    {
        #ifndef MAGNUM_TARGET_WEBGL
        /* Disable messages from the GL driver so we can verify our message */
        GL::DebugOutput::setCallback(nullptr, nullptr);
        Containers::ScopeGuard e{GL::DebugOutput::setDefaultCallback};
        #endif

        Debug redirectOutput{&out};
        succeeded = DebugTools::screenshot(_converterManager, framebuffer, file);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(succeeded);
    CORRADE_COMPARE(out.str(),
        Utility::formatString("DebugTools::screenshot(): saved a PixelFormat::RGBA8Unorm image of size Vector(4, 3) to {}\n", file));

    if(!(_importerManager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(file, rgba, CompareFileToImage{_importerManager});
}

#ifndef MAGNUM_TARGET_GLES2
constexpr unsigned char DataR8[]{
    0x11, 0x22, 0x33, 0x44,
    0x55, 0x66, 0x77, 0x88,
    0x99, 0xaa, 0xbb, 0xcc
};
#endif

void ScreenshotGLTest::r8() {
    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_SKIP("Lumimance isn't renderable and the API doesn't support forcing a specific GL pixel format for EXT_texture_rg, can't test.");
    #else
    if(!(_converterManager.loadState("AnyImageConverter") & PluginManager::LoadState::Loaded) ||
       !(_converterManager.loadState("TgaImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter / TgaImageConverter plugins not found.");

    ImageView2D r{PixelFormat::R8Unorm, {4, 3}, DataR8};

    GL::Texture2D texture;
    texture.setStorage(1, GL::TextureFormat::R8, {4, 3})
        .setSubImage(0, {}, r);
    GL::Framebuffer framebuffer{{{}, {4, 3}}};
    framebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, texture, 0);

    CORRADE_COMPARE(framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);

    std::string file = Utility::Directory::join(SCREENSHOTTEST_SAVE_DIR, "image.tga");
    if(Utility::Directory::exists(file))
        CORRADE_VERIFY(Utility::Directory::rm(file));
    else
        CORRADE_VERIFY(Utility::Directory::mkpath(SCREENSHOTTEST_SAVE_DIR));

    std::ostringstream out;
    bool succeeded;
    {
        #ifndef MAGNUM_TARGET_WEBGL
        /* Disable messages from the GL driver so we can verify our message */
        GL::DebugOutput::setCallback(nullptr, nullptr);
        Containers::ScopeGuard e{GL::DebugOutput::setDefaultCallback};
        #endif

        Debug redirectOutput{&out};
        succeeded = DebugTools::screenshot(_converterManager, framebuffer, file);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(succeeded);

    {
        CORRADE_EXPECT_FAIL_IF(framebuffer.implementationColorReadFormat() != GL::PixelFormat::Red,
            "Implementation-defined color read format is not single-channel.");

        CORRADE_COMPARE(out.str(),
            Utility::formatString("DebugTools::screenshot(): saved a PixelFormat::R8Unorm image of size Vector(4, 3) to {}\n", file));

        if(!(_importerManager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
        !(_importerManager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
            CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

        CORRADE_COMPARE_WITH(file, r, CompareFileToImage{_importerManager});
    }
    #endif
}

void ScreenshotGLTest::unknownFormat() {
    ImageView2D rgba{GL::PixelFormat::RGB, GL::PixelType::UnsignedShort565, {4, 3}, DataRgba8};

    GL::Texture2D texture;
    texture.setStorage(1, GL::TextureFormat::RGB565, {4, 3})
        .setSubImage(0, {}, rgba);
    GL::Framebuffer framebuffer{{{}, {4, 3}}};
    framebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, texture, 0);

    CORRADE_COMPARE(framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);

    if(framebuffer.implementationColorReadFormat() == GL::PixelFormat::RGBA &&
       framebuffer.implementationColorReadType() == GL::PixelType::UnsignedByte)
        CORRADE_SKIP("The framebuffer read format is RGBA8, can't test.");

    std::ostringstream out;
    bool succeeded;
    {
        Error redirectOutput{&out};
        succeeded = DebugTools::screenshot(_converterManager, framebuffer, Utility::Directory::join(SCREENSHOTTEST_SAVE_DIR, "image.tga"));
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(!succeeded);

    if(framebuffer.implementationColorReadFormat() == GL::PixelFormat::RGBA)
        CORRADE_COMPARE(out.str(), "DebugTools::screenshot(): can't map (GL::PixelFormat::RGBA, GL::PixelType::UnsignedShort565) to a generic pixel format\n");
    else
        CORRADE_COMPARE(out.str(), "DebugTools::screenshot(): can't map (GL::PixelFormat::RGB, GL::PixelType::UnsignedShort565) to a generic pixel format\n");
}

void ScreenshotGLTest::pluginLoadFailed() {
    ImageView2D rgba{PixelFormat::RGBA8Unorm, {4, 3}, DataRgba8};

    GL::Texture2D texture;
    texture.setStorage(1,
            #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
            GL::TextureFormat::RGBA8,
            #else
            GL::TextureFormat::RGBA,
            #endif
            {4, 3})
        .setSubImage(0, {}, rgba);
    GL::Framebuffer framebuffer{{{}, {4, 3}}};
    framebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, texture, 0);

    CORRADE_COMPARE(framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);

    std::ostringstream out;
    bool succeeded;
    {
        Error redirectOutput{&out};
        PluginManager::Manager<Trade::AbstractImageConverter> manager{"nowhere"};
        succeeded = DebugTools::screenshot(manager, framebuffer, Utility::Directory::join(SCREENSHOTTEST_SAVE_DIR, "image.poo"));
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(!succeeded);
    #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    CORRADE_COMPARE(out.str(), "PluginManager::Manager::load(): plugin AnyImageConverter is not static and was not found in nowhere\n");
    #else
    CORRADE_COMPARE(out.str(), "PluginManager::Manager::load(): plugin AnyImageConverter was not found\n");
    #endif
}

void ScreenshotGLTest::saveFailed() {
    if(!(_converterManager.loadState("AnyImageConverter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageConverter plugin not found.");

    ImageView2D rgba{PixelFormat::RGBA8Unorm, {4, 3}, DataRgba8};

    GL::Texture2D texture;
    texture.setStorage(1,
            #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
            GL::TextureFormat::RGBA8,
            #else
            GL::TextureFormat::RGBA,
            #endif
            {4, 3})
        .setSubImage(0, {}, rgba);
    GL::Framebuffer framebuffer{{{}, {4, 3}}};
    framebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, texture, 0);

    CORRADE_COMPARE(framebuffer.checkStatus(GL::FramebufferTarget::Read), GL::Framebuffer::Status::Complete);

    std::ostringstream out;
    bool succeeded;
    {
        Error redirectOutput{&out};
        succeeded = DebugTools::screenshot(_converterManager, framebuffer, "image.poo");
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(!succeeded);
    CORRADE_COMPARE(out.str(), "Trade::AnyImageConverter::exportToFile(): cannot determine the format of image.poo\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::ScreenshotGLTest)
