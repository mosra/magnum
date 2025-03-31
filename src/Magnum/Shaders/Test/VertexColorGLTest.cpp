/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © Vladislav Oleshko <vladislav.oleshko@gmail.com>

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

#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/System.h>

#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Shaders/VertexColorGL.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/MeshData.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/MeshView.h"
#include "Magnum/MeshTools/Concatenate.h"
#include "Magnum/MeshTools/GenerateIndices.h"
#include "Magnum/Primitives/Cone.h"
#include "Magnum/Primitives/Plane.h"
#include "Magnum/Primitives/Square.h"
#include "Magnum/Shaders/Generic.h"
#endif

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct VertexColorGLTest: GL::OpenGLTester {
    explicit VertexColorGLTest();

    template<UnsignedInt dimensions> void construct();
    template<UnsignedInt dimensions> void constructAsync();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructUniformBuffers();
    template<UnsignedInt dimensions> void constructUniformBuffersAsync();
    #endif

    template<UnsignedInt dimensions> void constructMove();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructMoveUniformBuffers();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructUniformBuffersZeroDraws();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setUniformUniformBuffersEnabled();
    template<UnsignedInt dimensions> void bindBufferUniformBuffersNotEnabled();
    template<UnsignedInt dimensions> void setWrongDrawOffset();
    #endif

    void renderSetup();
    void renderTeardown();

    template<class T, VertexColorGL2D::Flag flag = VertexColorGL2D::Flag{}> void renderDefaults2D();
    template<class T, VertexColorGL3D::Flag flag = VertexColorGL3D::Flag{}> void renderDefaults3D();

    template<class T, VertexColorGL2D::Flag flag = VertexColorGL2D::Flag{}> void render2D();
    template<class T, VertexColorGL3D::Flag flag = VertexColorGL3D::Flag{}> void render3D();

    #ifndef MAGNUM_TARGET_GLES2
    void renderMulti2D();
    void renderMulti3D();
    #endif

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
        Containers::String _testDir;

        GL::Renderbuffer _color{NoCreate};
        #ifndef MAGNUM_TARGET_GLES2
        GL::Renderbuffer _objectId{NoCreate};
        #endif
        GL::Framebuffer _framebuffer{NoCreate};
};

/*
    Rendering tests done:

    [B] base
    [O] UBOs + draw offset
    [M] multidraw

    Mesa Intel                      BOM
               ES2                   xx
               ES3                  BOx
    Mesa AMD                        B
    Mesa llvmpipe                   B
    SwiftShader ES2                 Bxx
                ES3                 B
    ANGLE ES2                        xx
          ES3                       BOM
    ARM Mali (Huawei P10) ES2       Bxx
                          ES3       BOx
    WebGL (on Mesa Intel) 1.0       Bxx
                          2.0       BOM
    NVidia
    Intel Windows
    AMD macOS                         x
    Intel macOS                     BOx
    iPhone 6 w/ iOS 12.4 ES3        B x
*/

using namespace Math::Literals;

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    VertexColorGL2D::Flags flags;
    UnsignedInt drawCount;
} ConstructUniformBuffersData[]{
    {"classic fallback", {}, 1},
    {"", VertexColorGL2D::Flag::UniformBuffers, 1},
    /* SwiftShader has 256 uniform vectors at most, per-draw is 4 in 3D case
       and 3 in 2D; one needs to be reserved for drawOffset */
    {"multiple draws", VertexColorGL2D::Flag::UniformBuffers, 63},
    {"multidraw with all the things", VertexColorGL2D::Flag::MultiDraw, 63},
    #ifndef MAGNUM_TARGET_WEBGL
    {"shader storage + multidraw with all the things", VertexColorGL2D::Flag::ShaderStorageBuffers|VertexColorGL2D::Flag::MultiDraw, 0}
    #endif
};
#endif

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    const char* expected2D;
    const char* expected3D;
    VertexColorGL2D::Flags flags;
    UnsignedInt drawCount;
    bool bindWithOffset;
    UnsignedInt uniformIncrement;
    Float maxThreshold, meanThreshold;
} RenderMultiData[] {
    {"bind with offset", "multidraw2D.tga", "multidraw3D.tga",
        {}, 1, true, 16,
        /* Minor differences on ARM Mali, NVidia */
        0.667f, 0.01f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"bind with offset, shader storage", "multidraw2D.tga", "multidraw3D.tga",
        VertexColorGL2D::Flag::ShaderStorageBuffers, 1, true, 16,
        /* Minor differences on ARM Mali, NVidia */
        0.667f, 0.01f},
    #endif
    {"draw offset", "multidraw2D.tga", "multidraw3D.tga",
        {}, 3, false, 1,
        /* Minor differences on ARM Mali, NVidia */
        0.667f, 0.01f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"draw offset, shader storage", "multidraw2D.tga", "multidraw3D.tga",
        VertexColorGL2D::Flag::ShaderStorageBuffers, 3, false, 1,
        /* Minor differences on ARM Mali, NVidia */
        0.667f, 0.01f},
    #endif
    {"multidraw", "multidraw2D.tga", "multidraw3D.tga",
        VertexColorGL2D::Flag::MultiDraw, 3, false, 1,
        /* Minor differences on ARM Mali, NVidia */
        0.667f, 0.01f},
    #ifndef MAGNUM_TARGET_WEBGL
    {"multidraw, shader storage", "multidraw2D.tga", "multidraw3D.tga",
        VertexColorGL2D::Flag::ShaderStorageBuffers|VertexColorGL2D::Flag::MultiDraw, 0, false, 1,
        /* Minor differences on ARM Mali, NVidia */
        0.667f, 0.01f},
    #endif
};
#endif

VertexColorGLTest::VertexColorGLTest() {
    addTests<VertexColorGLTest>({
        &VertexColorGLTest::construct<2>,
        &VertexColorGLTest::construct<3>,
        &VertexColorGLTest::constructAsync<2>,
        &VertexColorGLTest::constructAsync<3>});

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests<VertexColorGLTest>({
        &VertexColorGLTest::constructUniformBuffers<2>,
        &VertexColorGLTest::constructUniformBuffers<3>},
        Containers::arraySize(ConstructUniformBuffersData));

    addTests<VertexColorGLTest>({
        &VertexColorGLTest::constructUniformBuffersAsync<2>,
        &VertexColorGLTest::constructUniformBuffersAsync<3>});
    #endif

    addTests<VertexColorGLTest>({
        &VertexColorGLTest::constructMove<2>,
        &VertexColorGLTest::constructMove<3>,

        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::constructMoveUniformBuffers<2>,
        &VertexColorGLTest::constructMoveUniformBuffers<3>,
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::constructUniformBuffersZeroDraws<2>,
        &VertexColorGLTest::constructUniformBuffersZeroDraws<3>,
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::setUniformUniformBuffersEnabled<2>,
        &VertexColorGLTest::setUniformUniformBuffersEnabled<3>,
        &VertexColorGLTest::bindBufferUniformBuffersNotEnabled<2>,
        &VertexColorGLTest::bindBufferUniformBuffersNotEnabled<3>,
        &VertexColorGLTest::setWrongDrawOffset<2>,
        &VertexColorGLTest::setWrongDrawOffset<3>
        #endif
        });

    /* MSVC needs explicit type due to default template args */
    addTests<VertexColorGLTest>({
        &VertexColorGLTest::renderDefaults2D<Color3>,
        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::renderDefaults2D<Color3, VertexColorGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VertexColorGLTest::renderDefaults2D<Color3, VertexColorGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &VertexColorGLTest::renderDefaults2D<Color4>,
        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::renderDefaults2D<Color4, VertexColorGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VertexColorGLTest::renderDefaults2D<Color4, VertexColorGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &VertexColorGLTest::renderDefaults3D<Color3>,
        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::renderDefaults3D<Color3, VertexColorGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VertexColorGLTest::renderDefaults3D<Color3, VertexColorGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &VertexColorGLTest::renderDefaults3D<Color4>,
        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::renderDefaults3D<Color4, VertexColorGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VertexColorGLTest::renderDefaults3D<Color4, VertexColorGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif

        &VertexColorGLTest::render2D<Color3>,
        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::render2D<Color3, VertexColorGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VertexColorGLTest::render2D<Color3, VertexColorGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &VertexColorGLTest::render2D<Color4>,
        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::render2D<Color4, VertexColorGL2D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VertexColorGLTest::render2D<Color4, VertexColorGL2D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &VertexColorGLTest::render3D<Color3>,
        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::render3D<Color3, VertexColorGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VertexColorGLTest::render3D<Color3, VertexColorGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        &VertexColorGLTest::render3D<Color4>,
        #ifndef MAGNUM_TARGET_GLES2
        &VertexColorGLTest::render3D<Color4, VertexColorGL3D::Flag::UniformBuffers>,
        #ifndef MAGNUM_TARGET_WEBGL
        &VertexColorGLTest::render3D<Color4, VertexColorGL3D::Flag::ShaderStorageBuffers>,
        #endif
        #endif
        },
        &VertexColorGLTest::renderSetup,
        &VertexColorGLTest::renderTeardown);

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests({&VertexColorGLTest::renderMulti2D,
                       &VertexColorGLTest::renderMulti3D},
        Containers::arraySize(RenderMultiData),
        &VertexColorGLTest::renderSetup,
        &VertexColorGLTest::renderTeardown);
    #endif

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif

    #ifdef CORRADE_TARGET_APPLE
    if(Utility::System::isSandboxed()
        #if defined(CORRADE_TARGET_IOS) && defined(CORRADE_TESTSUITE_TARGET_XCTEST)
        /** @todo Fix this once I persuade CMake to run XCTest tests properly */
        && std::getenv("SIMULATOR_UDID")
        #endif
    ) {
        _testDir = Utility::Path::path(*Utility::Path::executableLocation());
    } else
    #endif
    {
        _testDir = SHADERS_TEST_DIR;
    }
}

template<UnsignedInt dimensions> void VertexColorGLTest::construct() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    VertexColorGL<dimensions> shader;
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void VertexColorGLTest::constructAsync() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    typename VertexColorGL<dimensions>::CompileState state = VertexColorGL<dimensions>::compile();

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    VertexColorGL<dimensions> shader{Utility::move(state)};
    CORRADE_VERIFY(shader.isLinkFinished());
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}


#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void VertexColorGLTest::constructUniformBuffers() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    auto&& data = ConstructUniformBuffersData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & VertexColorGL<dimensions>::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= VertexColorGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif
    }
    #endif

    if(data.flags >= VertexColorGL2D::Flag::MultiDraw) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_draw_parameters>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_draw_parameters::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
            CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
        #endif
    }

    VertexColorGL<dimensions> shader{typename VertexColorGL<dimensions>::Configuration{}
        .setFlags(data.flags)
        .setDrawCount(data.drawCount)};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_COMPARE(shader.drawCount(), data.drawCount);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void VertexColorGLTest::constructUniformBuffersAsync() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    typename VertexColorGL<dimensions>::CompileState state = VertexColorGL<dimensions>::compile(typename VertexColorGL<dimensions>::Configuration{}
        .setFlags(VertexColorGL2D::Flag::UniformBuffers)
        .setDrawCount(63));
    CORRADE_COMPARE(state.flags(), VertexColorGL2D::Flag::UniformBuffers);
    CORRADE_COMPARE(state.drawCount(), 63);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    VertexColorGL<dimensions> shader{Utility::move(state)};
    CORRADE_COMPARE(shader.flags(), VertexColorGL2D::Flag::UniformBuffers);
    CORRADE_COMPARE(shader.drawCount(), 63);
    CORRADE_VERIFY(shader.isLinkFinished());
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first());
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

template<UnsignedInt dimensions> void VertexColorGLTest::constructMove() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    VertexColorGL<dimensions> a;
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    VertexColorGL<dimensions> b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_VERIFY(!a.id());

    VertexColorGL<dimensions> c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_VERIFY(!b.id());
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void VertexColorGLTest::constructMoveUniformBuffers() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    VertexColorGL<dimensions> a{typename VertexColorGL<dimensions>::Configuration{}
        .setFlags(VertexColorGL<dimensions>::Flag::UniformBuffers)
        .setDrawCount(5)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    VertexColorGL<dimensions> b{Utility::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), VertexColorGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(b.drawCount(), 5);
    CORRADE_VERIFY(!a.id());

    VertexColorGL<dimensions> c{NoCreate};
    c = Utility::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), VertexColorGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(c.drawCount(), 5);
    CORRADE_VERIFY(!b.id());
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void VertexColorGLTest::constructUniformBuffersZeroDraws() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    /* This fails for UBOs but not SSBOs */
    Containers::String out;
    Error redirectError{&out};
    VertexColorGL<dimensions>{typename VertexColorGL<dimensions>::Configuration{}
        .setFlags(VertexColorGL<dimensions>::Flag::UniformBuffers)
        .setDrawCount(0)};
    CORRADE_COMPARE(out,
        "Shaders::VertexColorGL: draw count can't be zero\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void VertexColorGLTest::setUniformUniformBuffersEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    VertexColorGL<dimensions> shader{typename VertexColorGL<dimensions>::Configuration{}
        .setFlags(VertexColorGL<dimensions>::Flag::UniformBuffers)};

    Containers::String out;
    Error redirectError{&out};
    shader.setTransformationProjectionMatrix({});
    CORRADE_COMPARE(out,
        "Shaders::VertexColorGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled\n");
}

template<UnsignedInt dimensions> void VertexColorGLTest::bindBufferUniformBuffersNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    GL::Buffer buffer;
    VertexColorGL<dimensions> shader;

    Containers::String out;
    Error redirectError{&out};
    shader.bindTransformationProjectionBuffer(buffer)
          .bindTransformationProjectionBuffer(buffer, 0, 16)
          .setDrawOffset(0);
    CORRADE_COMPARE(out,
        "Shaders::VertexColorGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::VertexColorGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::VertexColorGL::setDrawOffset(): the shader was not created with uniform buffers enabled\n");
}

template<UnsignedInt dimensions> void VertexColorGLTest::setWrongDrawOffset() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    VertexColorGL<dimensions> shader{typename VertexColorGL<dimensions>::Configuration{}
        .setFlags(VertexColorGL<dimensions>::Flag::UniformBuffers)
        .setDrawCount(5)};

    Containers::String out;
    Error redirectError{&out};
    shader.setDrawOffset(5);
    CORRADE_COMPARE(out,
        "Shaders::VertexColorGL::setDrawOffset(): draw offset 5 is out of range for 5 draws\n");
}
#endif

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

template<class T, VertexColorGL2D::Flag flag> void VertexColorGLTest::renderDefaults2D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == VertexColorGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::ShaderStorageBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 1)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == VertexColorGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::UniformBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    } else
    #endif
    {
        setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");
    }

    Trade::MeshData circleData = Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates);

    /* All a single color */
    Containers::Array<T> colorData{DirectInit, circleData.vertexCount(), 0xffffff_rgbf};

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh circle = MeshTools::compile(circleData);
    circle.addVertexBuffer(colors, 0, GL::Attribute<VertexColorGL2D::Color3::Location, T>{});

    VertexColorGL2D shader{VertexColorGL2D::Configuration{}
        .setFlags(flag)};

    if(flag == VertexColorGL2D::Flag{}) {
        shader.draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == VertexColorGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == VertexColorGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .draw(circle);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T, VertexColorGL2D::Flag flag> void VertexColorGLTest::renderDefaults3D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == VertexColorGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::ShaderStorageBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 1)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == VertexColorGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::UniformBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    } else
    #endif
    {
        setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");
    }

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Trade::MeshData sphereData = Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates);

    /* All a single color */
    Containers::Array<T> colorData{DirectInit, sphereData.vertexCount(), 0xffffff_rgbf};

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh sphere = MeshTools::compile(sphereData);
    sphere.addVertexBuffer(colors, 0, GL::Attribute<VertexColorGL3D::Color4::Location, T>{});

    VertexColorGL3D shader{VertexColorGL3D::Configuration{}
        .setFlags(flag)};

    if(flag == VertexColorGL3D::Flag{}) {
        shader.draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == VertexColorGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == VertexColorGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "FlatTestFiles/defaults.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T, VertexColorGL2D::Flag flag> void VertexColorGLTest::render2D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == VertexColorGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::ShaderStorageBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 1)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == VertexColorGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::UniformBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    } else
    #endif
    {
        setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");
    }

    Trade::MeshData circleData = Primitives::circle2DSolid(32,
        Primitives::Circle2DFlag::TextureCoordinates);

    /* Highlight a quarter */
    Containers::Array<T> colorData{DirectInit, circleData.vertexCount(), 0x9999ff_rgbf};
    for(std::size_t i = 8; i != 16; ++i)
        colorData[i + 1] = 0xffff99_rgbf;

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh circle = MeshTools::compile(circleData);
    circle.addVertexBuffer(colors, 0, GL::Attribute<VertexColorGL2D::Color3::Location, T>{});

    VertexColorGL2D shader{VertexColorGL2D::Configuration{}
        .setFlags(flag)};

    if(flag == VertexColorGL2D::Flag{}) {
        shader.setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        .draw(circle);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == VertexColorGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == VertexColorGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
                .setTransformationProjectionMatrix(Matrix3::projection({2.1f, 2.1f}))
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .draw(circle);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "VertexColorTestFiles/vertexColor2D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

template<class T, VertexColorGL3D::Flag flag> void VertexColorGLTest::render3D() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(flag == VertexColorGL2D::Flag::ShaderStorageBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::ShaderStorageBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 1)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    } else
    #endif
    if(flag == VertexColorGL3D::Flag::UniformBuffers) {
        setTestCaseTemplateName({T::Size == 3 ? "Color3" : "Color4", "Flag::UniformBuffers"});

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    } else
    #endif
    {
        setTestCaseTemplateName(T::Size == 3 ? "Color3" : "Color4");
    }

    Trade::MeshData sphereData = Primitives::uvSphereSolid(16, 32,
        Primitives::UVSphereFlag::TextureCoordinates);

    /* Highlight the middle rings */
    Containers::Array<T> colorData{DirectInit, sphereData.vertexCount(), 0x9999ff_rgbf};
    for(std::size_t i = 6*33; i != 9*33; ++i)
        colorData[i + 1] = 0xffff99_rgbf;

    GL::Buffer colors;
    colors.setData(colorData);
    GL::Mesh sphere = MeshTools::compile(sphereData);
    sphere.addVertexBuffer(colors, 0, GL::Attribute<VertexColorGL3D::Color4::Location, T>{});

    VertexColorGL3D shader{VertexColorGL3D::Configuration{}
        .setFlags(flag)};

    if(flag == VertexColorGL3D::Flag{}) {
        shader.setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::rotationY(-15.0_degf)*
            Matrix4::rotationX(15.0_degf))
        .draw(sphere);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == VertexColorGL2D::Flag::UniformBuffers
        #ifndef MAGNUM_TARGET_WEBGL
        || flag == VertexColorGL2D::Flag::ShaderStorageBuffers
        #endif
    ) {
        /* Target hints matter just on WebGL (which doesn't have SSBOs) */
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform3D{}
                .setTransformationProjectionMatrix(
                    Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
                    Matrix4::translation(Vector3::zAxis(-2.15f))*
                    Matrix4::rotationY(-15.0_degf)*
                    Matrix4::rotationX(15.0_degf)
                )
        }};
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform)
            .draw(sphere);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

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
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join(_testDir, "VertexColorTestFiles/vertexColor3D.tga"),
        (DebugTools::CompareImageToFile{_manager, maxThreshold, meanThreshold}));
}

#ifndef MAGNUM_TARGET_GLES2
void VertexColorGLTest::renderMulti2D() {
    auto&& data = RenderMultiData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= VertexColorGL2D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 1)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
    #endif

    if(data.flags >= VertexColorGL2D::Flag::MultiDraw) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_draw_parameters>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_draw_parameters::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
            CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
        #endif
    }

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
        CORRADE_SKIP("UBOs with dynamically indexed arrays are a crashy dumpster fire on SwiftShader, can't test.");
    #endif

    /* Circle is a fan, plane is a strip, make it indexed first */
    Trade::MeshData circleData = MeshTools::generateIndices(Primitives::circle2DSolid(32));
    Trade::MeshData squareData = MeshTools::generateIndices(Primitives::squareSolid());
    Trade::MeshData triangleData = MeshTools::generateIndices(Primitives::circle2DSolid(3));
    /* Concatenate the meshes, reserve a vertex color attribute and fill it
       with a ... RAINBOW! */
    Trade::MeshData colored = MeshTools::interleave(MeshTools::concatenate({circleData, squareData, triangleData}), {
        Trade::MeshAttributeData{Trade::MeshAttribute::Color, VertexFormat::Vector3, nullptr}
    });
    Deg angle = 0.0_degf;
    for(Vector3& i: colored.mutableAttribute<Vector3>(Trade::MeshAttribute::Color))
        i = Color3::fromHsv({angle += 360.0_degf/colored.vertexCount(), 1.0f, 1.0f});
    GL::Mesh mesh = MeshTools::compile(colored);
    GL::MeshView circle{mesh};
    circle.setCount(circleData.indexCount());
    GL::MeshView square{mesh};
    square.setCount(squareData.indexCount())
        .setIndexOffset(circleData.indexCount());
    GL::MeshView triangle{mesh};
    triangle.setCount(triangleData.indexCount())
        .setIndexOffset(circleData.indexCount() + squareData.indexCount());

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<TransformationProjectionUniform2D> transformationProjectionData{2*data.uniformIncrement + 1};
    transformationProjectionData[0*data.uniformIncrement] = TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(
            Matrix3::projection({2.1f, 2.1f})*
            Matrix3::scaling(Vector2{0.4f})*
            Matrix3::translation({-1.25f, -1.25f})
        );
    transformationProjectionData[1*data.uniformIncrement] = TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(
            Matrix3::projection({2.1f, 2.1f})*
            Matrix3::scaling(Vector2{0.4f})*
            Matrix3::translation({ 1.25f, -1.25f})
        );
    transformationProjectionData[2*data.uniformIncrement] = TransformationProjectionUniform2D{}
        .setTransformationProjectionMatrix(
            Matrix3::projection({2.1f, 2.1f})*
            Matrix3::scaling(Vector2{0.4f})*
            Matrix3::translation({ 0.00f,  1.25f})
        );
    GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, transformationProjectionData};

    VertexColorGL2D shader{VertexColorGL2D::Configuration{}
        .setFlags(VertexColorGL2D::Flag::UniformBuffers|data.flags)
        .setDrawCount(data.drawCount)};

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.draw(circle);

        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.draw(square);

        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform2D),
            sizeof(TransformationProjectionUniform2D));
        shader.draw(triangle);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform);

        if(data.flags >= VertexColorGL2D::Flag::MultiDraw)
            shader.draw({circle, square, triangle});
        else {
            shader.setDrawOffset(0)
                .draw(circle);
            shader.setDrawOffset(1)
                .draw(square);
            shader.setDrawOffset(2)
                .draw(triangle);
        }
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /*
        -   Circle should be lower left
        -   Square lower right
        -   Triangle up center
    */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "VertexColorTestFiles", data.expected2D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));
}

void VertexColorGLTest::renderMulti3D() {
    auto&& data = RenderMultiData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(data.flags >= VertexColorGL3D::Flag::ShaderStorageBuffers) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_storage_buffer_object::string() << "is not supported.");
        #else
        if(!GL::Context::current().isVersionSupported(GL::Version::GLES310))
            CORRADE_SKIP(GL::Version::GLES310 << "is not supported.");
        #endif

        /* Some drivers (ARM Mali-G71) don't support SSBOs in vertex shaders */
        if(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) < 1)
            CORRADE_SKIP("Only" << GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex) << "shader storage blocks supported in vertex shaders.");
    }
    #endif

    if(data.flags >= VertexColorGL3D::Flag::MultiDraw) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_draw_parameters>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_draw_parameters::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
            CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
        #endif
    }

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(GL::Context::current().detectedDriver() & GL::Context::DetectedDriver::SwiftShader)
        CORRADE_SKIP("UBOs with dynamically indexed arrays are a crashy dumpster fire on SwiftShader, can't test.");
    #endif

    Trade::MeshData sphereData = Primitives::uvSphereSolid(16, 32);
    /* Plane is a strip, make it indexed first */
    Trade::MeshData planeData = MeshTools::generateIndices(Primitives::planeSolid());
    Trade::MeshData coneData = Primitives::coneSolid(1, 32, 1.0f);
    /* Concatenate the meshes, reserve a vertex color attribute and fill it
       with a ... RAINBOW! */
    Trade::MeshData colored = MeshTools::interleave(MeshTools::concatenate({sphereData, planeData, coneData}), {
        Trade::MeshAttributeData{Trade::MeshAttribute::Color, VertexFormat::Vector3, nullptr}
    });
    Deg angle = 0.0_degf;
    for(Vector3& i: colored.mutableAttribute<Vector3>(Trade::MeshAttribute::Color))
        i = Color3::fromHsv({angle += 360.0_degf/colored.vertexCount(), 1.0f, 1.0f});
    GL::Mesh mesh = MeshTools::compile(colored);
    GL::MeshView sphere{mesh};
    sphere.setCount(sphereData.indexCount());
    GL::MeshView plane{mesh};
    plane.setCount(planeData.indexCount())
        .setIndexOffset(sphereData.indexCount());
    GL::MeshView cone{mesh};
    cone.setCount(coneData.indexCount())
        .setIndexOffset(sphereData.indexCount() + planeData.indexCount());

    /* Some drivers have uniform offset alignment as high as 256, which means
       the subsequent sets of uniforms have to be aligned to a multiply of it.
       The data.uniformIncrement is set high enough to ensure that, in the
       non-offset-bind case this value is 1. */

    Containers::Array<TransformationProjectionUniform3D> transformationProjectionData{2*data.uniformIncrement + 1};
    transformationProjectionData[0*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({-1.25f, -1.25f, 0.0f})
        );
    transformationProjectionData[1*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({ 1.25f, -1.25f, 0.0f})
        );
    transformationProjectionData[2*data.uniformIncrement] = TransformationProjectionUniform3D{}
        .setTransformationProjectionMatrix(
            Matrix4::perspectiveProjection(60.0_degf, 1.0f, 0.1f, 10.0f)*
            Matrix4::translation(Vector3::zAxis(-2.15f))*
            Matrix4::scaling(Vector3{0.4f})*
            Matrix4::translation({  0.0f,  1.0f, 1.0f})
        );
    GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, transformationProjectionData};

    VertexColorGL3D shader{VertexColorGL3D::Configuration{}
        .setFlags(VertexColorGL3D::Flag::UniformBuffers|data.flags)
        .setDrawCount(data.drawCount)};

    /* Rebinding UBOs / SSBOs each time */
    if(data.bindWithOffset) {
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            0*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.draw(sphere);

        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            1*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.draw(plane);

        shader.bindTransformationProjectionBuffer(transformationProjectionUniform,
            2*data.uniformIncrement*sizeof(TransformationProjectionUniform3D),
            sizeof(TransformationProjectionUniform3D));
        shader.draw(cone);

    /* Otherwise using the draw offset / multidraw */
    } else {
        shader.bindTransformationProjectionBuffer(transformationProjectionUniform);

        if(data.flags >= VertexColorGL3D::Flag::MultiDraw)
            shader.draw({sphere, plane, cone});
        else {
            shader.setDrawOffset(0)
                .draw(sphere);
            shader.setDrawOffset(1)
                .draw(plane);
            shader.setDrawOffset(2)
                .draw(cone);
        }
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    /*
        -   Sphere should be lower left
        -   Plane lower right
        -   Cone up center
    */
    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        _framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>().slice(&Color4ub::rgb),
        Utility::Path::join({_testDir, "VertexColorTestFiles", data.expected3D}),
        (DebugTools::CompareImageToFile{_manager, data.maxThreshold, data.meanThreshold}));
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::VertexColorGLTest)
