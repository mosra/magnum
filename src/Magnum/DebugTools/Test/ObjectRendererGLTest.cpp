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

#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/DebugTools/ObjectRenderer.h"
#include "Magnum/DebugTools/ResourceManager.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/SceneGraph/Camera.h"
#include "Magnum/SceneGraph/Scene.h"
#include "Magnum/SceneGraph/MatrixTransformation2D.h"
#include "Magnum/SceneGraph/MatrixTransformation3D.h"
#include "Magnum/Trade/AbstractImporter.h"

#include "configure.h"

#ifdef CORRADE_TARGET_ANDROID
#include "Magnum/GL/Context.h"
#endif

namespace Magnum { namespace DebugTools { namespace Test { namespace {

struct ObjectRendererGLTest: GL::OpenGLTester {
    explicit ObjectRendererGLTest();

    void render2D();
    void render3D();

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
};

ObjectRendererGLTest::ObjectRendererGLTest() {
    addTests({&ObjectRendererGLTest::render2D,
              &ObjectRendererGLTest::render3D});

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

using namespace Math::Literals;

void ObjectRendererGLTest::render2D() {
    SceneGraph::Scene<SceneGraph::MatrixTransformation2D> scene;

    SceneGraph::DrawableGroup2D drawables;
    SceneGraph::Camera2D camera{scene};
    camera.setProjectionMatrix(Matrix3::projection({4.0f, 4.0f}));

    ResourceManager manager;
    manager.set("my", ObjectRendererOptions{}.setSize(2.2f));

    SceneGraph::Object<SceneGraph::MatrixTransformation2D> object{&scene};
    object
        .rotate(-17.3_degf)
        .translate({-1.0f, -1.0f});
    ObjectRenderer2D renderer{manager, object, "my", &drawables};

    GL::Renderbuffer color;
    color.setStorage(
        #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        {64, 64});
    GL::Framebuffer framebuffer{{{}, {64, 64}}};
    framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, color)
        .clear(GL::FramebufferClear::Color)
        .bind();

    camera.draw(drawables);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        framebuffer.read({{}, {64, 64}}, {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "ObjectRenderer2D.tga"),
        /* SwiftShader misplaces two pixels here. Nothing serious. */
        (CompareImageToFile{_manager, 71.6f, 0.07f}));
}

void ObjectRendererGLTest::render3D() {
    SceneGraph::Scene<SceneGraph::MatrixTransformation3D> scene;

    SceneGraph::DrawableGroup3D drawables;
    SceneGraph::Camera3D camera{scene};
    camera.setProjectionMatrix(Matrix4::orthographicProjection({4.0f, 4.0f}, 0.1f, 2.0f));

    ResourceManager manager;
    manager.set("my", ObjectRendererOptions{}.setSize(2.2f));

    SceneGraph::Object<SceneGraph::MatrixTransformation3D> object{&scene};
    object
        .rotateZ(17.3_degf)
        .rotateY(45.0_degf)
        .translate({-1.0f, -1.0f, -1.0f});
    ObjectRenderer3D renderer{manager, object, "my", &drawables};

    GL::Renderbuffer color;
    color.setStorage(
        #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        {64, 64});
    GL::Framebuffer framebuffer{{{}, {64, 64}}};
    framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, color)
        .clear(GL::FramebufferClear::Color)
        .bind();

    camera.draw(drawables);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /* Intel Windows drivers misplace one pixel here. Nothing serious.
       ARM Mali G71 (Huawei P10) has some rounding differences causing the
       the bottom blue line to be on a different place (but the rest is
       okay and the 2D case matches exactly), however to avoid false negatives
       elsewhere I'm making it conditional. */
    Containers::Optional<CompareImageToFile> comparator{Containers::InPlaceInit, _manager,  71.6f, 0.018f};
    #ifdef CORRADE_TARGET_ANDROID
    if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::ArmMali)
        comparator.emplace(_manager, 127.6f, 0.54f);
    #endif

    CORRADE_COMPARE_WITH(
        framebuffer.read({{}, {64, 64}}, {PixelFormat::RGBA8Unorm}),
        Utility::Directory::join(DEBUGTOOLS_TEST_DIR, "ObjectRenderer3D.tga"),
        *comparator);
}

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::ObjectRendererGLTest)
