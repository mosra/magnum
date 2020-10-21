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

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Shaders/VertexColor.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct VertexColorGLTest: GL::OpenGLTester {
    explicit VertexColorGLTest();

    template<UnsignedInt dimensions> void construct();
    template<UnsignedInt dimensions> void constructMove();

    void renderSetup();
    void renderTeardown();

    template<class T> void renderDefaults2D();
    template<class T> void renderDefaults3D();

    template<class T> void render2D();
    template<class T> void render3D();

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
        std::string _testDir;

        GL::Renderbuffer _color{NoCreate};
        #ifndef MAGNUM_TARGET_GLES2
        GL::Renderbuffer _objectId{NoCreate};
        #endif
        GL::Framebuffer _framebuffer{NoCreate};
};

/*
    Rendering tests done on:

    -   Mesa Intel
    -   Mesa AMD
    -   SwiftShader ES2/ES3
    -   ARM Mali (Huawei P10) ES2/ES3
    -   WebGL 1 / 2 (on Mesa Intel)
    -   iPhone 6 w/ iOS 12.4
*/

using namespace Math::Literals;

VertexColorGLTest::VertexColorGLTest() {
    addTests<VertexColorGLTest>({
        &VertexColorGLTest::construct<2>,
        &VertexColorGLTest::construct<3>,
        &VertexColorGLTest::constructMove<2>,
        &VertexColorGLTest::constructMove<3>});

    addTests({&VertexColorGLTest::renderDefaults2D<Color3>,
              &VertexColorGLTest::renderDefaults2D<Color4>,
              &VertexColorGLTest::renderDefaults3D<Color3>,
              &VertexColorGLTest::renderDefaults3D<Color4>,

              &VertexColorGLTest::render2D<Color3>,
              &VertexColorGLTest::render2D<Color4>,
              &VertexColorGLTest::render3D<Color3>,
              &VertexColorGLTest::render3D<Color4>},
        &VertexColorGLTest::renderSetup,
        &VertexColorGLTest::renderTeardown);

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    #ifdef CORRADE_TARGET_APPLE
    if(Utility::Directory::isSandboxed()
        #if defined(CORRADE_TARGET_IOS) && defined(CORRADE_TESTSUITE_TARGET_XCTEST)
        /** @todo Fix this once I persuade CMake to run XCTest tests properly */
        && std::getenv("SIMULATOR_UDID")
        #endif
    ) {
        _testDir = Utility::Directory::path(Utility::Directory::executableLocation());
    } else
    #endif
    {
        _testDir = SHADERS_TEST_DIR;
    }
}

template<UnsignedInt dimensions> void VertexColorGLTest::construct() {
    setTestCaseTemplateName(std::to_string(dimensions));

    VertexColor<dimensions> shader;
    CORRADE_VERIFY(shader.id());
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void VertexColorGLTest::constructMove() {
    setTestCaseTemplateName(std::to_string(dimensions));

    VertexColor<dimensions> a;
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    VertexColor<dimensions> b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_VERIFY(!a.id());

    VertexColor<dimensions> c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_VERIFY(!b.id());
}

constexpr Vector2i RenderSize{80, 80};

void VertexColorGLTest::renderSetup() {
    /* Pick a color that's directly representable on RGBA4 as well to reduce
       artifacts */
    GL::Renderer::setClearColor(0x111111_rgbf);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    _color = GL::Renderbuffer{};
    _color.setStorage(
        #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
        GL::RenderbufferFormat::RGBA8,
        #else
        GL::RenderbufferFormat::RGBA4,
        #endif
        RenderSize);
    _framebuffer = GL::Framebuffer{{{}, RenderSize}};
    _framebuffer.attachRenderbuffer(GL::Framebuffer::ColorAttachment{0}, _color)
        .clear(GL::FramebufferClear::Color)
        .bind();
}

void VertexColorGLTest::renderTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
}

template<class T> void VertexColorGLTest::renderDefaults2D() {
    setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");

    Trade::MeshData circleData = Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates);

    /* All a single color */
    Containers::Array<T> colorData{Containers::DirectInit, circleData.vertexCount(), 0xffffff_rgbf};

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh circle = MeshTools::compile(circleData);
    circle.addVertexBuffer(colors, 0, GL::Attribute<Shaders::VertexColor2D::Color3::Location, T>{});

    VertexColor2D{}
        .draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has differently rasterized edges on eight pixels */
    const Float maxThreshold = 238.0f, meanThreshold = 0.298f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 238.0f, meanThreshold = 0.298f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T> void VertexColorGLTest::renderDefaults3D() {
    setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Trade::MeshData sphereData = Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates);

    /* All a single color */
    Containers::Array<T> colorData{Containers::DirectInit, sphereData.vertexCount(), 0xffffff_rgbf};

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh sphere = MeshTools::compile(sphereData);
    sphere.addVertexBuffer(colors, 0, GL::Attribute<Shaders::VertexColor2D::Color4::Location, T>{});

    VertexColor3D{}
        .draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has differently rasterized edges on eight pixels */
    const Float maxThreshold = 238.0f, meanThreshold = 0.298f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 238.0f, meanThreshold = 0.298f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T> void VertexColorGLTest::render2D() {
    setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");

    Trade::MeshData circleData = Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates);

    /* Highlight a quarter */
    Containers::Array<T> colorData{Containers::DirectInit, circleData.vertexCount(), 0x9999ff_rgbf};
    for(std::size_t i = 8; i != 16; ++i)
        colorData[i + 1] = 0xffff99_rgbf;

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh circle = MeshTools::compile(circleData);
    circle.addVertexBuffer(colors, 0, GL::Attribute<Shaders::VertexColor2D::Color3::Location, T>{});

    VertexColor2D{}
        .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        .draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* AMD has minor rounding differences in the gradient compared to Intel,
       SwiftShader as well */
    const Float maxThreshold = 1.0f, meanThreshold = 0.667f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 11.34f, meanThreshold = 1.479f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "VertexColorTestFiles/vertexColor2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T> void VertexColorGLTest::render3D() {
    setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Trade::MeshData sphereData = Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates);

    /* Highlight the middle rings */
    Containers::Array<T> colorData{Containers::DirectInit, sphereData.vertexCount(), 0x9999ff_rgbf};
    for(std::size_t i = 6*33; i != 9*33; ++i)
        colorData[i + 1] = 0xffff99_rgbf;

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh sphere = MeshTools::compile(sphereData);
    sphere.addVertexBuffer(colors, 0, GL::Attribute<Shaders::VertexColor2D::Color4::Location, T>{});

    VertexColor3D{}
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* AMD has one different pixel compared to Intel, SwiftShader has
       differently rasterized edges on five pixels. Apple A8 some more. */
    const Float maxThreshold = 204.0f, meanThreshold = 0.167f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 204.0f, meanThreshold = 1.284f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "VertexColorTestFiles/vertexColor3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VertexColorGLTest)
