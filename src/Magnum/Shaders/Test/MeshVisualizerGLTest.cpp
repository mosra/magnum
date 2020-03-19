/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <numeric>
#include <sstream>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Primitives/Icosphere.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Shaders/MeshVisualizer.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct MeshVisualizerGLTest: GL::OpenGLTester {
    explicit MeshVisualizerGLTest();

    void construct2D();
    void construct3D();

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void constructWireframeGeometryShader2D();
    void constructWireframeGeometryShader3D();
    #endif

    void constructMove2D();
    void constructMove3D();

    void setWireframeNotEnabled2D();
    void setWireframeNotEnabled3D();

    void renderSetup();
    void renderTeardown();

    void renderDefaults2D();
    void renderDefaults3D();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void renderDefaultsWireframe2D();
    void renderDefaultsWireframe3D();
    #endif
    void render2D();
    void render3D();
    void renderWireframe2D();
    void renderWireframe3D();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void renderWireframe3DPerspective();
    #endif

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

constexpr struct {
    const char* name;
    MeshVisualizer2D::Flags flags;
} ConstructData2D[] {
    {"", {}},
    {"wireframe w/o GS", MeshVisualizer2D::Flag::Wireframe|MeshVisualizer2D::Flag::NoGeometryShader},
};

constexpr struct {
    const char* name;
    MeshVisualizer3D::Flags flags;
} ConstructData3D[] {
    {"", {}},
    {"wireframe w/o GS", MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::NoGeometryShader}
};

constexpr struct {
    const char* name;
    MeshVisualizer2D::Flags flags;
    Float width, smoothness;
    const char* file;
    const char* fileXfail;
} WireframeData2D[] {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"", MeshVisualizer2D::Flags{},
        1.0f, 2.0f, "wireframe2D.tga", nullptr},
    {"wide/sharp", MeshVisualizer2D::Flags{},
        3.0f, 1.0f, "wireframe-wide2D.tga", nullptr},
    #endif
    {"no geometry shader", MeshVisualizer2D::Flag::NoGeometryShader,
        1.0f, 2.0f, "wireframe2D.tga", "wireframe-nogeo2D.tga"},
    {"no geometry shader, wide/sharp", MeshVisualizer2D::Flag::NoGeometryShader,
        3.0f, 1.0f, "wireframe-wide2D.tga", "wireframe-nogeo2D.tga"}
};

constexpr struct {
    const char* name;
    MeshVisualizer3D::Flags flags;
    Float width, smoothness;
    const char* file;
    const char* fileXfail;
} WireframeData3D[] {
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    {"", MeshVisualizer3D::Flags{},
        1.0f, 2.0f, "wireframe3D.tga", nullptr},
    {"wide/sharp", MeshVisualizer3D::Flags{},
        3.0f, 1.0f, "wireframe-wide3D.tga", nullptr},
    #endif
    {"no geometry shader",
        MeshVisualizer3D::Flag::NoGeometryShader,
        1.0f, 2.0f, "wireframe3D.tga", "wireframe-nogeo3D.tga"},
    {"no geometry shader, wide/sharp",
        MeshVisualizer3D::Flag::NoGeometryShader,
        3.0f, 1.0f, "wireframe-wide3D.tga", "wireframe-nogeo3D.tga"}
};

MeshVisualizerGLTest::MeshVisualizerGLTest() {
    addInstancedTests({&MeshVisualizerGLTest::construct2D},
        Containers::arraySize(ConstructData2D));

    addInstancedTests({&MeshVisualizerGLTest::construct3D},
        Containers::arraySize(ConstructData3D));

    addTests({
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &MeshVisualizerGLTest::constructWireframeGeometryShader2D,
              &MeshVisualizerGLTest::constructWireframeGeometryShader3D,
              #endif

              &MeshVisualizerGLTest::constructMove2D,
              &MeshVisualizerGLTest::constructMove3D,

              &MeshVisualizerGLTest::setWireframeNotEnabled2D,
              &MeshVisualizerGLTest::setWireframeNotEnabled3D});

    addTests({&MeshVisualizerGLTest::renderDefaults2D,
              &MeshVisualizerGLTest::renderDefaults3D,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &MeshVisualizerGLTest::renderDefaultsWireframe2D,
              &MeshVisualizerGLTest::renderDefaultsWireframe3D,
              #endif
              &MeshVisualizerGLTest::render2D,
              &MeshVisualizerGLTest::render3D},
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    addInstancedTests({&MeshVisualizerGLTest::renderWireframe2D},
        Containers::arraySize(WireframeData2D),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    addInstancedTests({&MeshVisualizerGLTest::renderWireframe3D},
        Containers::arraySize(WireframeData3D),
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    addTests({&MeshVisualizerGLTest::renderWireframe3DPerspective},
        &MeshVisualizerGLTest::renderSetup,
        &MeshVisualizerGLTest::renderTeardown);
    #endif

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
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

void MeshVisualizerGLTest::construct2D() {
    auto&& data = ConstructData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    MeshVisualizer2D shader{data.flags};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_VERIFY(shader.id());
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void MeshVisualizerGLTest::construct3D() {
    auto&& data = ConstructData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    MeshVisualizer3D shader{data.flags};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_VERIFY(shader.id());
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::constructWireframeGeometryShader2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    MeshVisualizer2D shader{MeshVisualizer2D::Flag::Wireframe};
    CORRADE_COMPARE(shader.flags(), MeshVisualizer2D::Flag::Wireframe);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}

void MeshVisualizerGLTest::constructWireframeGeometryShader3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    MeshVisualizer3D shader{MeshVisualizer3D::Flag::Wireframe};
    CORRADE_COMPARE(shader.flags(), MeshVisualizer3D::Flag::Wireframe);
    {
        #ifdef CORRADE_TARGET_APPLE
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.id());
        CORRADE_VERIFY(shader.validate().first);
    }
}
#endif

void MeshVisualizerGLTest::constructMove2D() {
    MeshVisualizer2D a{MeshVisualizer2D::Flag::Wireframe|MeshVisualizer2D::Flag::NoGeometryShader};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MeshVisualizer2D b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), MeshVisualizer2D::Flag::Wireframe|MeshVisualizer2D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!a.id());

    MeshVisualizer2D c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), MeshVisualizer2D::Flag::Wireframe|MeshVisualizer2D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!b.id());
}

void MeshVisualizerGLTest::constructMove3D() {
    MeshVisualizer3D a{MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::NoGeometryShader};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MeshVisualizer3D b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!a.id());

    MeshVisualizer3D c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), MeshVisualizer3D::Flag::Wireframe|MeshVisualizer3D::Flag::NoGeometryShader);
    CORRADE_VERIFY(!b.id());
}

void MeshVisualizerGLTest::setWireframeNotEnabled2D() {
    std::ostringstream out;
    Error redirectError{&out};

    MeshVisualizer2D shader;
    shader.setWireframeColor({})
        .setWireframeWidth({})
        .setSmoothness({});

    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer::setWireframeColor(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizer::setWireframeWidth(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizer::setSmoothness(): the shader was not created with wireframe enabled\n");
}

void MeshVisualizerGLTest::setWireframeNotEnabled3D() {
    std::ostringstream out;
    Error redirectError{&out};

    MeshVisualizer3D shader;
    shader.setWireframeColor({})
        .setWireframeWidth({})
        .setSmoothness({});

    CORRADE_COMPARE(out.str(),
        "Shaders::MeshVisualizer::setWireframeColor(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizer::setWireframeWidth(): the shader was not created with wireframe enabled\n"
        "Shaders::MeshVisualizer::setSmoothness(): the shader was not created with wireframe enabled\n");
}

constexpr Vector2i RenderSize{80, 80};

void MeshVisualizerGLTest::renderSetup() {
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

void MeshVisualizerGLTest::renderTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
}

void MeshVisualizerGLTest::renderDefaults2D() {
    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32));

    MeshVisualizer2D{}
        .draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has differently rasterized edges on four pixels */
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

void MeshVisualizerGLTest::renderDefaults3D() {
    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    MeshVisualizer3D{}
        .draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has differently rasterized edges on four pixels */
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

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::renderDefaultsWireframe2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(16));

    MeshVisualizer2D shader{MeshVisualizer2D::Flag::Wireframe};
    shader.draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    {
        CORRADE_EXPECT_FAIL("Defaults don't work for wireframe as line width is derived from viewport size.");
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe2D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /** @todo make this unnecessary */
    shader
        .setViewportSize({80, 80})
        .draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe2D.tga"),
        /* AMD has off-by-one errors on edges compared to Intel */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.082f}));
}

void MeshVisualizerGLTest::renderDefaultsWireframe3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif

    GL::Mesh sphere = MeshTools::compile(Primitives::icosphereSolid(1));

    MeshVisualizer3D shader{MeshVisualizer3D::Flag::Wireframe};
    shader.draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    {
        CORRADE_EXPECT_FAIL("Defaults don't work for wireframe as line width is derived from viewport size.");
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe3D.tga"),
            (DebugTools::CompareImageToFile{_manager}));
    }

    /** @todo make this unnecessary */
    shader
        .setViewportSize({80, 80})
        .draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/defaults-wireframe3D.tga"),
        /* AMD has off-by-one errors on edges compared to Intel */
        (DebugTools::CompareImageToFile{_manager, 1.0f, 0.06f}));
}
#endif

void MeshVisualizerGLTest::render2D() {
    GL::Mesh circle = MeshTools::compile(Primitives::circle2DSolid(32));

    MeshVisualizer2D{}
        .setColor(0x9999ff_rgbf)
        .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        .draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has differently rasterized edges on four pixels */
    const Float maxThreshold = 170.0f, meanThreshold = 0.133f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 170.0f, meanThreshold = 0.456f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/colored2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void MeshVisualizerGLTest::render3D() {
    GL::Mesh sphere = MeshTools::compile(Primitives::uvSphereSolid(16, 32));

    MeshVisualizer3D{}
        .setColor(0x9999ff_rgbf)
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* SwiftShader has differently rasterized edges on four pixels */
    const Float maxThreshold = 170.0f, meanThreshold = 0.133f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
    const Float maxThreshold = 170.0f, meanThreshold = 0.456f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "FlatTestFiles/colored3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

void MeshVisualizerGLTest::renderWireframe2D() {
    auto&& data = WireframeData2D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(!(data.flags & MeshVisualizer2D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!(data.flags & MeshVisualizer2D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif
    #endif

    const Trade::MeshData circleData = Primitives::circle2DSolid(16);

    GL::Mesh circle{NoCreate};
    if(data.flags & MeshVisualizer2D::Flag::NoGeometryShader) {
        /* Duplicate the vertices. The circle primitive is  */
        const Trade::MeshData circleDataIndexed =
            MeshTools::generateIndices(circleData);
        circle = MeshTools::compile(MeshTools::duplicate(circleDataIndexed));

        /* Supply also the vertex ID, if needed */
        #ifndef MAGNUM_TARGET_GLES2
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        #endif
        {
            Containers::Array<Float> vertexIndex{circleDataIndexed.indexCount()};
            std::iota(vertexIndex.begin(), vertexIndex.end(), 0.0f);

            GL::Buffer vertexId;
            vertexId.setData(vertexIndex);
            circle.addVertexBuffer(std::move(vertexId), 0, MeshVisualizer2D::VertexIndex{});
        }
    } else circle = MeshTools::compile(circleData);

    MeshVisualizer2D{data.flags|MeshVisualizer2D::Flag::Wireframe}
        .setColor(0xffff99_rgbf)
        .setWireframeColor(0x9999ff_rgbf)
        .setWireframeWidth(data.width)
        .setSmoothness(data.smoothness)
        .setViewportSize({80, 80})
        .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        .draw(circle);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    {
        CORRADE_EXPECT_FAIL_IF(data.flags & MeshVisualizer2D::Flag::NoGeometryShader,
            "Line width is currently not configurable w/o geometry shader.");
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* SwiftShader has differently rasterized edges on four pixels */
        const Float maxThreshold = 170.0f, meanThreshold = 0.327f;
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
        const Float maxThreshold = 170.0f, meanThreshold = 1.699f;
        #endif
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.file}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }

    /* Test it's not *too* off, at least */
    if(data.flags & MeshVisualizer2D::Flag::NoGeometryShader) {
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* SwiftShader has differently rasterized edges on four pixels. Apple
           A8 on more. */
        const Float maxThreshold = 170.0f, meanThreshold = 0.330f;
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
        const Float maxThreshold = 170.0f, meanThreshold = 2.077f;
        #endif
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.fileXfail}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }
}

void MeshVisualizerGLTest::renderWireframe3D() {
    auto&& data = WireframeData3D[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(!(data.flags & MeshVisualizer3D::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!(data.flags & MeshVisualizer::Flag::NoGeometryShader) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    #ifdef MAGNUM_TARGET_GLES
    if(GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>())
        Debug() << "Using" << GL::Extensions::NV::shader_noperspective_interpolation::string();
    #endif
    #endif

    const Trade::MeshData sphereData = Primitives::icosphereSolid(1);

    GL::Mesh sphere{NoCreate};
    if(data.flags & MeshVisualizer3D::Flag::NoGeometryShader) {
        /* Duplicate the vertices */
        sphere = MeshTools::compile(MeshTools::duplicate(sphereData));

        /* Supply also the vertex ID, if needed */
        #ifndef MAGNUM_TARGET_GLES2
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        #endif
        {
            Containers::Array<Float> vertexIndex{sphereData.indexCount()};
            std::iota(vertexIndex.begin(), vertexIndex.end(), 0.0f);

            GL::Buffer vertexId;
            vertexId.setData(vertexIndex);
            sphere.addVertexBuffer(std::move(vertexId), 0, MeshVisualizer3D::VertexIndex{});
        }
    } else sphere = MeshTools::compile(sphereData);

    MeshVisualizer3D{data.flags|MeshVisualizer3D::Flag::Wireframe}
        .setColor(0xffff99_rgbf)
        .setWireframeColor(0x9999ff_rgbf)
        .setWireframeWidth(data.width)
        .setSmoothness(data.smoothness)
        .setViewportSize({80, 80})
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .draw(sphere);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    {
        CORRADE_EXPECT_FAIL_IF(data.flags & MeshVisualizer3D::Flag::NoGeometryShader,
            "Line width is currently not configurable w/o geometry shader.");
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* SwiftShader has differently rasterized edges on four pixels */
        const Float maxThreshold = 170.0f, meanThreshold = 0.327f;
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
        const Float maxThreshold = 170.0f, meanThreshold = 1.699f;
        #endif
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.file}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }

    /* Test it's not *too* off, at least */
    if(data.flags & MeshVisualizer3D::Flag::NoGeometryShader) {
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        /* SwiftShader has differently rasterized edges on four pixels. Apple
           A8 on more. */
        const Float maxThreshold = 170.0f, meanThreshold = 0.330f;
        #else
        /* WebGL 1 doesn't have 8bit renderbuffer storage, so it's way worse */
        const Float maxThreshold = 170.0f, meanThreshold = 1.699f;
        #endif
        CORRADE_COMPARE_WITH(
            /* Dropping the alpha channel, as it's always 1.0 */
            Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
            Utility::Directory::join({_testDir, "MeshVisualizerTestFiles", data.fileXfail}),
            (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
    }
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void MeshVisualizerGLTest::renderWireframe3DPerspective() {
    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
        CORRADE_SKIP(GL::Extensions::ARB::geometry_shader4::string() + std::string(" is not supported"));
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
        CORRADE_SKIP(GL::Extensions::EXT::geometry_shader::string() + std::string(" is not supported"));
    #endif

    GL::Mesh plane = MeshTools::compile(Primitives::planeSolid());

    MeshVisualizer3D{MeshVisualizer3D::Flag::Wireframe}
        .setWireframeWidth(8.0f)
        .setWireframeColor(0xff0000_rgbf)
        .setViewportSize({80, 80})
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation({0.0f, 0.5f, -3.5f})*
            Matrix4::rotationX(-60.0_degf)*
            Matrix4::scaling(Vector3::yScale(2.0f)))
        .draw(plane);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImageImporter plugins not found.");

    #ifdef MAGNUM_TARGET_GLES
    CORRADE_EXPECT_FAIL_IF(!GL::Context::current().isExtensionSupported<GL::Extensions::NV::shader_noperspective_interpolation>(),
        "GL_NV_shader_noperspective_interpolation not available.");
    #endif

    /* Slight rasterization differences on AMD. */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "MeshVisualizerTestFiles/wireframe-perspective.tga"),
        (DebugTools::CompareImageToFile{_manager, 0.667f, 0.002f}));
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::MeshVisualizerGLTest)
