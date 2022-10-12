/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/System.h>

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/Line.h"
#include "Magnum/Shaders/LineGL.h"
#include "Magnum/Trade/AbstractImporter.h"

#include "configure.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct LineGLTest: GL::OpenGLTester {
    explicit LineGLTest();

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

    // template<UnsignedInt dimensions> void constructInvalid();
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void constructUniformBuffersInvalid();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setUniformUniformBuffersEnabled();
    template<UnsignedInt dimensions> void bindBufferUniformBuffersNotEnabled();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setObjectIdNotEnabled();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    template<UnsignedInt dimensions> void setWrongDrawOffset();
    #endif

    void renderSetup();
    void renderTeardown();

    template<LineGL2D::Flag flag = LineGL2D::Flag{}> void renderDefaults2D();
    template<LineGL3D::Flag flag = LineGL3D::Flag{}> void renderDefaults3D();

    template<LineGL2D::Flag flag = LineGL2D::Flag{}> void render2D();
    template<LineGL3D::Flag flag = LineGL3D::Flag{}> void render3D();

    template<class T, LineGL2D::Flag flag = LineGL2D::Flag{}> void renderVertexColor2D();
    template<class T, LineGL3D::Flag flag = LineGL3D::Flag{}> void renderVertexColor3D();

    #ifndef MAGNUM_TARGET_GLES2
    void renderObjectIdSetup();
    void renderObjectIdTeardown();

    template<LineGL2D::Flag flag = LineGL2D::Flag{}> void renderObjectId2D();
    template<LineGL3D::Flag flag = LineGL3D::Flag{}> void renderObjectId3D();
    #endif

    template<LineGL2D::Flag flag = LineGL2D::Flag{}> void renderInstanced2D();
    template<LineGL3D::Flag flag = LineGL3D::Flag{}> void renderInstanced3D();

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

using namespace Math::Literals;

const struct {
    const char* name;
    LineGL2D::Flags flags;
} ConstructData[]{
    {"", {}},
    {"vertex colors", LineGL2D::Flag::VertexColor},
    #ifndef MAGNUM_TARGET_GLES2
    {"object ID", LineGL2D::Flag::ObjectId},
    {"instanced object ID", LineGL2D::Flag::InstancedObjectId},
    #endif
    {"instanced transformation", LineGL2D::Flag::InstancedTransformation},
};

#ifndef MAGNUM_TARGET_GLES2
const struct {
    const char* name;
    LineGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
} ConstructUniformBuffersData[]{
    {"classic fallback", {}, 1, 1},
    {"", LineGL2D::Flag::UniformBuffers, 1, 1},
    /* SwiftShader has 256 uniform vectors at most, per-draw is 4+1 in 3D case
       and 3+1 in 2D, per-material 1 */
    {"multiple materials, draws", LineGL2D::Flag::UniformBuffers, 16, 48},
    {"object ID", LineGL2D::Flag::UniformBuffers|LineGL2D::Flag::ObjectId, 1, 1},
    {"instanced object ID", LineGL2D::Flag::UniformBuffers|LineGL2D::Flag::InstancedObjectId, 1, 1},
    {"multidraw with all the things", LineGL2D::Flag::MultiDraw|LineGL2D::Flag::ObjectId|LineGL2D::Flag::InstancedTransformation|LineGL2D::Flag::InstancedObjectId, 16, 48}
};
#endif

// const struct {
//     const char* name;
//     LineGL2D::Flags flags;
//     const char* message;
// } ConstructInvalidData[]{
// };

#ifndef MAGNUM_TARGET_GLES2
constexpr struct {
    const char* name;
    LineGL2D::Flags flags;
    UnsignedInt materialCount, drawCount;
    const char* message;
} ConstructUniformBuffersInvalidData[]{
    {"zero draws", LineGL2D::Flag::UniformBuffers, 1, 0,
        "draw count can't be zero"},
    {"zero materials", LineGL2D::Flag::UniformBuffers, 0, 1,
        "material count can't be zero"}
};
#endif

LineGLTest::LineGLTest() {
    addInstancedTests<LineGLTest>({
        &LineGLTest::construct<2>,
        // &LineGLTest::construct<3>

    },
        Containers::arraySize(ConstructData));

    addTests<LineGLTest>({
        &LineGLTest::constructAsync<2>,
        // &LineGLTest::constructAsync<3>

    });

    #ifndef MAGNUM_TARGET_GLES2
    // addInstancedTests<LineGLTest>({
    //     &LineGLTest::constructUniformBuffers<2>,
    //     &LineGLTest::constructUniformBuffers<3>},
    //     Containers::arraySize(ConstructUniformBuffersData));
    //
    // addTests<LineGLTest>({
    //     &LineGLTest::constructUniformBuffersAsync<2>,
    //     &LineGLTest::constructUniformBuffersAsync<3>});
    #endif

    addTests<LineGLTest>({
        &LineGLTest::constructMove<2>,
        // &LineGLTest::constructMove<3>,

        // #ifndef MAGNUM_TARGET_GLES2
        // &LineGLTest::constructMoveUniformBuffers<2>,
        // &LineGLTest::constructMoveUniformBuffers<3>,
        // #endif
        });

    // addInstancedTests<LineGLTest>({
    //     &LineGLTest::constructInvalid<2>,
    //     &LineGLTest::constructInvalid<3>},
    //     Containers::arraySize(ConstructInvalidData));

    #ifndef MAGNUM_TARGET_GLES2
    addInstancedTests<LineGLTest>({
        &LineGLTest::constructUniformBuffersInvalid<2>,
        // &LineGLTest::constructUniformBuffersInvalid<3>

    },
        Containers::arraySize(ConstructUniformBuffersInvalidData));
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    addTests<LineGLTest>({
        // &LineGLTest::setUniformUniformBuffersEnabled<2>,
        // &LineGLTest::setUniformUniformBuffersEnabled<3>,
        &LineGLTest::bindBufferUniformBuffersNotEnabled<2>,
        // &LineGLTest::bindBufferUniformBuffersNotEnabled<3>,
        &LineGLTest::setObjectIdNotEnabled<2>,
        // &LineGLTest::setObjectIdNotEnabled<3>,
        // &LineGLTest::setWrongDrawOffset<2>,
        // &LineGLTest::setWrongDrawOffset<3>

    });
    #endif

    /* MSVC needs explicit type due to default template args */
    addTests<LineGLTest>({
        &LineGLTest::renderDefaults2D,
        #ifndef MAGNUM_TARGET_GLES2
        // &LineGLTest::renderDefaults2D<LineGL2D::Flag::UniformBuffers>,
        #endif
        // &LineGLTest::renderDefaults3D,
        // #ifndef MAGNUM_TARGET_GLES2
        // &LineGLTest::renderDefaults3D<LineGL3D::Flag::UniformBuffers>,
        // #endif
        },
        &LineGLTest::renderSetup,
        &LineGLTest::renderTeardown);

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

template<UnsignedInt dimensions> void LineGLTest::construct() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    auto&& data = ConstructData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & LineGL2D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    LineGL<dimensions> shader{typename LineGL<dimensions>::Configuration{}
        .setFlags(data.flags)};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void LineGLTest::constructAsync() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    typename LineGL<dimensions>::CompileState state = LineGL<dimensions>::compile(typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL2D::Flag::VertexColor));
    CORRADE_COMPARE(state.flags(),  LineGL2D::Flag::VertexColor);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    LineGL<dimensions> shader{std::move(state)};
    CORRADE_COMPARE(shader.flags(), LineGL2D::Flag::VertexColor);

    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void LineGLTest::constructUniformBuffers() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    auto&& data = ConstructUniformBuffersData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if((data.flags & LineGL2D::Flag::UniformBuffers) && !GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    if((data.flags & LineGL2D::Flag::ObjectId) && !GL::Context::current().isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(GL::Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    if(data.flags >= LineGL2D::Flag::MultiDraw) {
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

    LineGL<dimensions> shader{typename LineGL<dimensions>::Configuration{}
        .setFlags(data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    CORRADE_COMPARE(shader.flags(), data.flags);
    CORRADE_COMPARE(shader.materialCount(), data.materialCount);
    CORRADE_COMPARE(shader.drawCount(), data.drawCount);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

template<UnsignedInt dimensions> void LineGLTest::constructUniformBuffersAsync() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    typename LineGL<dimensions>::CompileState state = LineGL<dimensions>::compile(typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL2D::Flag::UniformBuffers|LineGL2D::Flag::VertexColor)
        .setMaterialCount(16)
        .setDrawCount(48));
    CORRADE_COMPARE(state.flags(), LineGL2D::Flag::UniformBuffers|LineGL2D::Flag::VertexColor);
    CORRADE_COMPARE(state.materialCount(), 16);
    CORRADE_COMPARE(state.drawCount(), 48);

    while(!state.isLinkFinished())
        Utility::System::sleep(100);

    LineGL<dimensions> shader{std::move(state)};
    CORRADE_COMPARE(shader.flags(), LineGL2D::Flag::UniformBuffers|LineGL2D::Flag::VertexColor);
    CORRADE_COMPARE(shader.materialCount(), 16);
    CORRADE_COMPARE(shader.drawCount(), 48);
    CORRADE_VERIFY(shader.id());
    {
        #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
        CORRADE_EXPECT_FAIL("macOS drivers need insane amount of state to validate properly.");
        #endif
        CORRADE_VERIFY(shader.validate().first);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#endif

template<UnsignedInt dimensions> void LineGLTest::constructMove() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    LineGL<dimensions> a{typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::VertexColor)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    LineGL<dimensions> b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_VERIFY(!a.id());

    LineGL<dimensions> c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), LineGL<dimensions>::Flag::VertexColor);
    CORRADE_VERIFY(!b.id());
}

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void LineGLTest::constructMoveUniformBuffers() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    LineGL<dimensions> a{typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::UniformBuffers)
        .setMaterialCount(2)
        .setDrawCount(5)};
    const GLuint id = a.id();
    CORRADE_VERIFY(id);

    MAGNUM_VERIFY_NO_GL_ERROR();

    LineGL<dimensions> b{std::move(a)};
    CORRADE_COMPARE(b.id(), id);
    CORRADE_COMPARE(b.flags(), LineGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(b.materialCount(), 2);
    CORRADE_COMPARE(b.drawCount(), 5);
    CORRADE_VERIFY(!a.id());

    LineGL<dimensions> c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.id(), id);
    CORRADE_COMPARE(c.flags(), LineGL<dimensions>::Flag::UniformBuffers);
    CORRADE_COMPARE(c.materialCount(), 2);
    CORRADE_COMPARE(c.drawCount(), 5);
    CORRADE_VERIFY(!b.id());
}
#endif

// template<UnsignedInt dimensions> void LineGLTest::constructInvalid() {
//     auto&& data = ConstructInvalidData[testCaseInstanceId()];
//     setTestCaseTemplateName(Utility::format("{}", dimensions));
//     setTestCaseDescription(data.name);
//
//     CORRADE_SKIP_IF_NO_ASSERT();
//
//     std::ostringstream out;
//     Error redirectError{&out};
//     LineGL<dimensions>{data.flags};
//     CORRADE_COMPARE(out.str(), Utility::formatString(
//         "Shaders::LineGL: {}\n", data.message));
// }

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void LineGLTest::constructUniformBuffersInvalid() {
    auto&& data = ConstructUniformBuffersInvalidData[testCaseInstanceId()];
    setTestCaseTemplateName(Utility::format("{}", dimensions));
    setTestCaseDescription(data.name);

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    LineGL<dimensions>{typename LineGL<dimensions>::Configuration{}
        .setFlags(data.flags)
        .setMaterialCount(data.materialCount)
        .setDrawCount(data.drawCount)};
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Shaders::LineGL: {}\n", data.message));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void LineGLTest::setUniformUniformBuffersEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    LineGL<dimensions> shader{typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::UniformBuffers)};

    /* This should work fine */
    shader.setViewportSize({});

    std::ostringstream out;
    Error redirectError{&out};
    shader.setTransformationProjectionMatrix({})
        .setWidth({})
        .setSmoothness({})
        .setBackgroundColor({})
        .setColor({})
        .setObjectId({});
    CORRADE_COMPARE(out.str(),
        "Shaders::LineGL::setTransformationProjectionMatrix(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setWidth(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setSmoothness(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setBackgroundColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setColor(): the shader was created with uniform buffers enabled\n"
        "Shaders::LineGL::setObjectId(): the shader was created with uniform buffers enabled\n");
}

template<UnsignedInt dimensions> void LineGLTest::bindBufferUniformBuffersNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    std::ostringstream out;
    Error redirectError{&out};

    GL::Buffer buffer;
    LineGL<dimensions> shader;
    shader.bindTransformationProjectionBuffer(buffer)
          .bindTransformationProjectionBuffer(buffer, 0, 16)
          .bindDrawBuffer(buffer)
          .bindDrawBuffer(buffer, 0, 16)
          .bindMaterialBuffer(buffer)
          .bindMaterialBuffer(buffer, 0, 16)
          .setDrawOffset(0);
    CORRADE_COMPARE(out.str(),
        "Shaders::LineGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::bindTransformationProjectionBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::bindDrawBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::bindMaterialBuffer(): the shader was not created with uniform buffers enabled\n"
        "Shaders::LineGL::setDrawOffset(): the shader was not created with uniform buffers enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void LineGLTest::setObjectIdNotEnabled() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    LineGL<dimensions> shader;

    std::ostringstream out;
    Error redirectError{&out};
    shader.setObjectId(33376);
    CORRADE_COMPARE(out.str(),
        "Shaders::LineGL::setObjectId(): the shader was not created with object ID enabled\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> void LineGLTest::setWrongDrawOffset() {
    setTestCaseTemplateName(Utility::format("{}", dimensions));

    CORRADE_SKIP_IF_NO_ASSERT();

    #ifndef MAGNUM_TARGET_GLES
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
        CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
    #endif

    LineGL<dimensions> shader{typename LineGL<dimensions>::Configuration{}
        .setFlags(LineGL<dimensions>::Flag::UniformBuffers)
        .setMaterialCount(2)
        .setDrawCount(5)};

    std::ostringstream out;
    Error redirectError{&out};
    shader.setDrawOffset(5);
    CORRADE_COMPARE(out.str(),
        "Shaders::LineGL::setDrawOffset(): draw offset 5 is out of bounds for 5 draws\n");
}
#endif

constexpr Vector2i RenderSize{80, 80};

void LineGLTest::renderSetup() {
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

void LineGLTest::renderTeardown() {
    _framebuffer = GL::Framebuffer{NoCreate};
    _color = GL::Renderbuffer{NoCreate};
}

template<LineGL2D::Flag flag> void LineGLTest::renderDefaults2D() {
    #ifndef MAGNUM_TARGET_GLES2
    if(flag == LineGL2D::Flag::UniformBuffers) {
        setTestCaseTemplateName("Flag::UniformBuffers");

        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
            CORRADE_SKIP(GL::Extensions::ARB::uniform_buffer_object::string() << "is not supported.");
        #endif
    }
    #endif

    // TODO drop this crap, use MeshTools once things settle down
    struct Vertex {
        Vector2 neighborDirection;
        Vector2 position;
        Vector2 direction;
    } vertices[]{
        /* Two connected line segments, down and then to the right */
        {Vector2{Constants::nan()}, {-0.25f, 0.5f}, {}},
        {Vector2{Constants::nan()}, {-0.25f, 0.5f}, {}},
        {{}, {-0.5f, -0.5f}, {}},
        {{}, {-0.5f, -0.5f}, {}},
        {{}, {-0.5f, -0.5f}, {}},
        {{}, {-0.5f, -0.5f}, {}},
        {Vector2{Constants::nan()}, {0.5f, -0.25f}, {}},
        {Vector2{Constants::nan()}, {0.5f, -0.25f}, {}},

        /* Singular horizontal and vertical segment */
        {Vector2{Constants::nan()}, {-0.75f, 0.25f}, {}},
        {Vector2{Constants::nan()}, {-0.75f, 0.25f}, {}},
        {Vector2{Constants::nan()}, {-0.75f, 0.75f}, {}},
        {Vector2{Constants::nan()}, {-0.75f, 0.75f}, {}},

        {Vector2{Constants::nan()}, {-0.25f, -0.75f}, {}},
        {Vector2{Constants::nan()}, {-0.25f, -0.75f}, {}},
        {Vector2{Constants::nan()}, {0.75f, -0.75f}, {}},
        {Vector2{Constants::nan()}, {0.75f, -0.75f}, {}},

        /* A single point */
        {Vector2{Constants::nan()}, {0.5f, 0.5f}, {}},
        {Vector2{Constants::nan()}, {0.5f, 0.5f}, {}},
        {Vector2{Constants::nan()}, {0.5f, 0.5f}, {}},
        {Vector2{Constants::nan()}, {0.5f, 0.5f}, {}},
    };

    /* Prev directions */
    for(std::size_t i = 4; i < Containers::arraySize(vertices); i += 4) {
        if(Math::isNan(vertices[i + 0].neighborDirection)) continue;
        vertices[i + 0].neighborDirection = vertices[i + 1].neighborDirection =
            vertices[i - 4].position - vertices[i + 0].position;
    }
    /* Segment directions */
    for(std::size_t i = 0; i < Containers::arraySize(vertices); i += 4) {
        vertices[i + 0].direction = vertices[i + 1].direction =
            vertices[i + 2].direction = vertices[i + 3].direction =
                vertices[i + 2].position - vertices[i + 0].position;
    }
    /* Next directions */
    for(std::size_t i = 2; i < Containers::arraySize(vertices) - 4; i += 4) {
        if(Math::isNan(vertices[i + 0].neighborDirection)) continue;
        vertices[i + 0].neighborDirection = vertices[i + 1].neighborDirection =
            vertices[i + 4].position - vertices[i + 0].position;
    }

    !Debug{} << Containers::stridedArrayView(vertices).slice(&Vertex::position).prefix(8);
    !Debug{} << Containers::stridedArrayView(vertices).slice(&Vertex::direction).prefix(8);
    !Debug{} << Containers::stridedArrayView(vertices).slice(&Vertex::neighborDirection).prefix(8);

    const UnsignedInt indices[]{
        0, 1, 2, 2, 1, 3,
        4, 5, 6, 6, 5, 7,
        8, 9, 10, 10, 9, 11,
        12, 13, 14, 14, 13, 15,
        16, 17, 18, 18, 17, 19
    };

    GL::Mesh lines;
    lines.addVertexBuffer(GL::Buffer{vertices}, 0,
            LineGL2D::LineNeighborDirection{},
            LineGL2D::Position{},
            LineGL2D::LineDirection{})
        .setIndexBuffer(GL::Buffer{indices}, 0, GL::MeshIndexType::UnsignedInt)
        .setCount(Containers::arraySize(indices));

    LineGL2D shader{LineGL2D::Configuration{}
        .setFlags(flag)};
    shader.setViewportSize(Vector2{RenderSize});
    shader
        .setWidth(5.0f)
        .setSmoothness(1.0f)
        // .setBackgroundColor(0xff0000ff_rgbaf)
        // .setColor(0x557766_rgbf)
        ;

    if(flag == LineGL2D::Flag{}) {
        shader.draw(lines);
    }
    #ifndef MAGNUM_TARGET_GLES2
    else if(flag == LineGL2D::Flag::UniformBuffers) {
        GL::Buffer transformationProjectionUniform{GL::Buffer::TargetHint::Uniform, {
            TransformationProjectionUniform2D{}
        }};
        GL::Buffer drawUniform{GL::Buffer::TargetHint::Uniform, {
            LineDrawUniform{}
        }};
        GL::Buffer materialUniform{GL::Buffer::TargetHint::Uniform, {
            LineMaterialUniform{}
        }};
        shader
            .bindTransformationProjectionBuffer(transformationProjectionUniform)
            .bindDrawBuffer(drawUniform)
            .bindMaterialBuffer(materialUniform)
            .draw(lines);
    }
    #endif
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH(
        /* Dropping the alpha channel, as it's always 1.0 */
        Containers::arrayCast<Color3ub>(_framebuffer.read(_framebuffer.viewport(), {PixelFormat::RGBA8Unorm}).pixels<Color4ub>()),
        Utility::Path::join(_testDir, "LineTestFiles/defaults2D.tga"),
        /* SwiftShader has 8 different pixels on the edges */
        (DebugTools::CompareImageToFile{_manager, 238.0f, 0.2975f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::LineGLTest)
