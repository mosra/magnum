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
#include <Corrade/Containers/Reference.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Image.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Version.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct RendererGLTest: OpenGLTester {
    explicit RendererGLTest();

    void maxLineWidth();
    void pointCoord();
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void patchParameters();
    #endif
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void drawBuffersIndexed();
    void drawBuffersBlend();
    #endif

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
        std::string _testDir;

        GL::Renderbuffer _color{NoCreate};
        GL::Framebuffer _framebuffer{NoCreate};
};

using namespace Math::Literals;

RendererGLTest::RendererGLTest() {
    addTests({&RendererGLTest::maxLineWidth,
              &RendererGLTest::pointCoord,
              #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
              &RendererGLTest::patchParameters,
              #endif
              #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
              &RendererGLTest::drawBuffersIndexed,
              &RendererGLTest::drawBuffersBlend
              #endif
              });

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
        _testDir = Utility::Directory::join(Utility::Directory::path(Utility::Directory::executableLocation()), "RendererGLTestFiles");
    } else
    #endif
    {
        _testDir = RENDERERGLTEST_FILES_DIR;
    }
}

void RendererGLTest::maxLineWidth() {
    #ifndef MAGNUM_TARGET_GLES
    Debug{} << "Core profile:" << Context::current().isCoreProfile();
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    Debug{} << "Context flags:" << Context::current().flags();
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    Range1D lineWidthRange = Renderer::lineWidthRange();
    Debug{} << "Line width range:" << lineWidthRange;

    MAGNUM_VERIFY_NO_GL_ERROR();

    Renderer::setLineWidth(lineWidthRange.max());

    MAGNUM_VERIFY_NO_GL_ERROR();
}

constexpr Vector2i RenderSize{16, 16};

void RendererGLTest::pointCoord() {
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

    /* Verify that gl_PointCoord works. On desktop compatibility profile this
       needs an explicit glEnable(GL_POINT_SPRITE), which is done in
       RendererState */

    struct SpriteShader: AbstractShaderProgram {
        explicit SpriteShader() {
            #ifndef MAGNUM_TARGET_GLES
            Shader vert{
                #ifndef CORRADE_TARGET_APPLE
                Version::GL210
                #else
                Version::GL310
                #endif
                , Shader::Type::Vertex};
            Shader frag{
                #ifndef CORRADE_TARGET_APPLE
                Version::GL210
                #else
                Version::GL310
                #endif
                , Shader::Type::Fragment};
            #elif defined(MAGNUM_TARGET_GLES2)
            Shader vert(Version::GLES200, Shader::Type::Vertex);
            Shader frag(Version::GLES200, Shader::Type::Fragment);
            #else
            Shader vert(Version::GLES300, Shader::Type::Vertex);
            Shader frag(Version::GLES300, Shader::Type::Fragment);
            #endif
            vert.addSource("#line " CORRADE_LINE_STRING "\n" R"GLSL(
                void main() {
                    gl_PointSize = 12.0;
                    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
                })GLSL");
            frag.addSource("#line " CORRADE_LINE_STRING "\n" R"GLSL(
                #if !defined(GL_ES) && __VERSION__ == 120
                #define lowp
                #endif

                #if (defined(GL_ES) && __VERSION__ < 300) || __VERSION__ == 120
                #define color gl_FragColor
                #else
                out lowp vec4 color;
                #endif
                void main() {
                    color = vec4(gl_PointCoord.x, gl_PointCoord.y, 0.0, 1.0);
                })GLSL");

            CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

            attachShaders({vert, frag});

            CORRADE_INTERNAL_ASSERT_OUTPUT(link());
        }
    } shader;

    #ifndef MAGNUM_TARGET_GLES
    Renderer::enable(Renderer::Feature::ProgramPointSize);
    #endif

    Mesh mesh{MeshPrimitive::Points};
    mesh.setCount(1);
    shader.draw(mesh);

    #ifndef MAGNUM_TARGET_GLES
    Renderer::disable(Renderer::Feature::ProgramPointSize);
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    const Float maxThreshold = 0.0f, meanThreshold = 0.0f;
    #else
    /* WebGL 1 doesn't have 8bit renderbuffer storage */
    const Float maxThreshold = 7.334f, meanThreshold = 2.063f;
    #endif
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {Magnum::PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Directory::join(_testDir, "pointcoord.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void RendererGLTest::patchParameters() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::tessellation_shader>())
        CORRADE_SKIP(Extensions::ARB::tessellation_shader::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::tessellation_shader>())
        CORRADE_SKIP(Extensions::EXT::tessellation_shader::string() + std::string(" is not available."));
    #endif

    /* All we can do is check for GL errors */
    Renderer::setPatchVertexCount(Renderer::maxPatchVertexCount());
    #ifndef MAGNUM_TARGET_GLES
    Renderer::setPatchDefaultInnerLevel({0.3f, 1.2f});
    Renderer::setPatchDefaultOuterLevel({0.3f, 2.2f, 1.0f, 1.2f});
    #endif
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void RendererGLTest::drawBuffersIndexed() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::draw_buffers2>())
        CORRADE_SKIP(Extensions::EXT::draw_buffers2::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::draw_buffers_indexed>())
        CORRADE_SKIP(Extensions::EXT::draw_buffers_indexed::string() + std::string(" is not available."));
    #endif

    /* Call the draw-buffer dependent functions, only expect that no GL error
       is emitted to ensure we didn't mess up argument order or something */
    Renderer::enable(Renderer::Feature::Blending, 1);
    Renderer::disable(Renderer::Feature::Blending, 1);
    Renderer::setColorMask(1, true, false, true, false);
    MAGNUM_VERIFY_NO_GL_ERROR();
}

void RendererGLTest::drawBuffersBlend() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_buffers_blend>())
        CORRADE_SKIP(Extensions::ARB::draw_buffers_blend::string() + std::string(" is not available."));
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::draw_buffers_indexed>())
        CORRADE_SKIP(Extensions::EXT::draw_buffers_indexed::string() + std::string(" is not available."));
    #endif

    /* Call the draw-buffer dependent functions, only expect that no GL error
       is emitted to ensure we didn't mess up argument order or something */
    Renderer::setBlendFunction(1, Renderer::BlendFunction::One, Renderer::BlendFunction::OneMinusSourceAlpha);
    Renderer::setBlendFunction(1, Renderer::BlendFunction::One, Renderer::BlendFunction::Zero, Renderer::BlendFunction::OneMinusSourceAlpha, Renderer::BlendFunction::SourceAlpha);
    Renderer::setBlendEquation(1, Renderer::BlendEquation::Subtract);
    Renderer::setBlendEquation(1, Renderer::BlendEquation::Add, Renderer::BlendEquation::Subtract);
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::RendererGLTest)
