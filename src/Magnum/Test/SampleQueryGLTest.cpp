/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/Query.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/RenderbufferFormat.h"
#include "Magnum/Shader.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

class SampleQueryGLTest: public AbstractOpenGLTester {
    public:
        explicit SampleQueryGLTest();

        void querySamplesPassed();
        #ifndef MAGNUM_TARGET_GLES
        void conditionalRender();
        #endif
};

SampleQueryGLTest::SampleQueryGLTest() {
    addTests({
        &SampleQueryGLTest::querySamplesPassed,
        #ifndef MAGNUM_TARGET_GLES
        &SampleQueryGLTest::conditionalRender
        #endif
    });
}

namespace {
    struct MyShader: public AbstractShaderProgram {
        typedef Attribute<0, Vector2> Position;

        explicit MyShader();
    };
}

#ifndef DOXYGEN_GENERATING_OUTPUT
MyShader::MyShader() {
    Utility::Resource rs("QueryGLTest");
    #ifndef MAGNUM_TARGET_GLES
    Shader vert(Version::GL210, Shader::Type::Vertex);
    #else
    Shader vert(Version::GLES200, Shader::Type::Vertex);
    #endif
    vert.addSource(rs.get("MyShader.vert"));
    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
    attachShader(vert);

    #ifndef MAGNUM_TARGET_GLES
    Shader frag(Version::GL210, Shader::Type::Fragment);
    #else
    Shader frag(Version::GLES200, Shader::Type::Fragment);
    #endif
    frag.addSource(rs.get("MyShader.frag"));
    CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
    attachShader(frag);

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
#endif

void SampleQueryGLTest::querySamplesPassed() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::occlusion_query_boolean>())
        CORRADE_SKIP(Extensions::GL::EXT::occlusion_query_boolean::string() + std::string(" is not available."));
    #endif

    Renderbuffer renderbuffer;
    #ifndef MAGNUM_TARGET_GLES2
    renderbuffer.setStorage(RenderbufferFormat::RGBA8, Vector2i(32));
    #else
    renderbuffer.setStorage(RenderbufferFormat::RGBA4, Vector2i(32));
    #endif

    Framebuffer framebuffer({{}, Vector2i(32)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer);

    Buffer buffer;
    constexpr Vector2 triangle[] = {{-1.0f, 1.0f}, {-1.0f, -3.0f}, {3.0f, 1.0f}};
    buffer.setData(triangle, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Triangles)
        .setVertexCount(3)
        .addVertexBuffer(buffer, 0, AbstractShaderProgram::Attribute<0, Vector2>());

    MyShader shader;

    MAGNUM_VERIFY_NO_ERROR();

    SampleQuery q;
    #ifndef MAGNUM_TARGET_GLES
    q.begin(SampleQuery::Target::SamplesPassed);
    #else
    q.begin(SampleQuery::Target::AnySamplesPassed);
    #endif

    framebuffer.bind(FramebufferTarget::ReadDraw);
    shader.use();
    mesh.draw();

    q.end();
    const bool availableBefore = q.resultAvailable();
    const UnsignedInt count = q.result<UnsignedInt>();
    const bool availableAfter = q.resultAvailable();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(!availableBefore);
    CORRADE_VERIFY(availableAfter);
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(count, 32*32);
    #else
    CORRADE_VERIFY(count > 0);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void SampleQueryGLTest::conditionalRender() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::NV::conditional_render>())
        CORRADE_SKIP(Extensions::GL::NV::conditional_render::string() + std::string(" is not available."));

    Renderbuffer renderbuffer;
    renderbuffer.setStorage(RenderbufferFormat::RGBA8, Vector2i(32));

    Framebuffer framebuffer({{}, Vector2i(32)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer);

    Buffer buffer;
    constexpr Vector2 triangle[] = {{-1.0f, 1.0f}, {-1.0f, -3.0f}, {3.0f, 1.0f}};
    buffer.setData(triangle, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Triangles)
        .setVertexCount(3)
        .addVertexBuffer(buffer, 0, AbstractShaderProgram::Attribute<0, Vector2>());

    MyShader shader;
    framebuffer.bind(FramebufferTarget::ReadDraw);
    shader.use();

    MAGNUM_VERIFY_NO_ERROR();

    SampleQuery qYes, qNo, q;

    /* This should generate some samples */
    qYes.begin(SampleQuery::Target::SamplesPassed);
    mesh.draw();
    qYes.end();

    /* Thus this should be rendered */
    qYes.beginConditionalRender(SampleQuery::ConditionalRenderMode::Wait);
    q.begin(SampleQuery::Target::SamplesPassed);
    mesh.draw();
    q.end();
    qYes.endConditionalRender();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(qYes.result<bool>());
    CORRADE_VERIFY(q.result<bool>());

    /* This shouldn't generate any samples */
    qNo.begin(SampleQuery::Target::SamplesPassed);
    qNo.end();

    /* Thus this should not be rendered */
    qNo.beginConditionalRender(SampleQuery::ConditionalRenderMode::Wait);
    q.begin(SampleQuery::Target::SamplesPassed);
    mesh.draw();
    q.end();
    qNo.endConditionalRender();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(!qNo.result<bool>());
    CORRADE_VERIFY(!q.result<bool>());
}
#endif

}}

CORRADE_TEST_MAIN(Magnum::Test::SampleQueryGLTest)
