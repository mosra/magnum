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

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/SampleQuery.h"
#include "Magnum/GL/Shader.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct SampleQueryGLTest: OpenGLTester {
    explicit SampleQueryGLTest();

    void constructMove();
    void wrap();

    void querySamplesPassed();
    #ifndef MAGNUM_TARGET_GLES
    void conditionalRender();
    #endif
};

SampleQueryGLTest::SampleQueryGLTest() {
    addTests({&SampleQueryGLTest::constructMove,
              &SampleQueryGLTest::wrap,

              &SampleQueryGLTest::querySamplesPassed,
              #ifndef MAGNUM_TARGET_GLES
              &SampleQueryGLTest::conditionalRender
              #endif
              });
}

void SampleQueryGLTest::constructMove() {
    /* Move constructor tested in AbstractQuery, here we just verify there
       are no extra members that would need to be taken care of */
    CORRADE_COMPARE(sizeof(SampleQuery), sizeof(AbstractQuery));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<SampleQuery>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<SampleQuery>::value);
}

void SampleQueryGLTest::wrap() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::EXT::occlusion_query_boolean>())
        CORRADE_SKIP(Extensions::EXT::occlusion_query_boolean::string() + std::string(" is not available."));
    #endif

    GLuint id;
    #ifndef MAGNUM_TARGET_GLES2
    glGenQueries(1, &id);
    #else
    glGenQueriesEXT(1, &id);
    #endif

    /* Releasing won't delete anything */
    {
        auto query = SampleQuery::wrap(id, SampleQuery::Target::AnySamplesPassed, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(query.release(), id);
    }

    /* ...so we can wrap it again */
    SampleQuery::wrap(id, SampleQuery::Target::AnySamplesPassed);
    #ifndef MAGNUM_TARGET_GLES2
    glDeleteQueries(1, &id);
    #else
    glDeleteQueriesEXT(1, &id);
    #endif
}

struct MyShader: public AbstractShaderProgram {
    typedef Attribute<0, Vector2> Position;

    explicit MyShader();
};

#ifndef DOXYGEN_GENERATING_OUTPUT
MyShader::MyShader() {
    #ifndef MAGNUM_TARGET_GLES
    Shader vert(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    #else
    Shader vert(Version::GLES200, Shader::Type::Vertex);
    Shader frag(Version::GLES200, Shader::Type::Fragment);
    #endif

    vert.addSource(
        "#if !defined(GL_ES) && __VERSION__ == 120\n"
        "#define lowp\n"
        "#endif\n"
        "#if defined(GL_ES) || __VERSION__ == 120\n"
        "#define in attribute\n"
        "#endif\n"
        "in lowp vec4 position;\n"
        "void main() {\n"
        "    gl_Position = position;\n"
        "}\n");
    frag.addSource(
        "#if !defined(GL_ES) && __VERSION__ >= 130\n"
        "out vec4 color;\n"
        "#else\n"
        "#define color gl_FragColor\n"
        "#endif\n"
        "void main() {\n"
        "    color = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "}\n");

    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
#endif

void SampleQueryGLTest::querySamplesPassed() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::EXT::occlusion_query_boolean>())
        CORRADE_SKIP(Extensions::EXT::occlusion_query_boolean::string() + std::string(" is not available."));
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
        .setCount(3)
        .addVertexBuffer(buffer, 0, Attribute<0, Vector2>{});

    MyShader shader;

    MAGNUM_VERIFY_NO_GL_ERROR();

    framebuffer.bind();

    #ifndef MAGNUM_TARGET_GLES
    SampleQuery q{SampleQuery::Target::SamplesPassed};
    #else
    SampleQuery q{SampleQuery::Target::AnySamplesPassed};
    #endif
    q.begin();

    shader.draw(mesh);

    q.end();
    const bool availableBefore = q.resultAvailable();
    const UnsignedInt count = q.result<UnsignedInt>();
    const bool availableAfter = q.resultAvailable();

    MAGNUM_VERIFY_NO_GL_ERROR();
    {
        CORRADE_EXPECT_FAIL_IF(availableBefore, "GPU faster than light?");
        CORRADE_VERIFY(!availableBefore);
    }
    CORRADE_VERIFY(availableAfter);
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(count, 32*32);
    #else
    CORRADE_VERIFY(count > 0);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void SampleQueryGLTest::conditionalRender() {
    if(!Context::current().isExtensionSupported<Extensions::NV::conditional_render>())
        CORRADE_SKIP(Extensions::NV::conditional_render::string() + std::string(" is not available."));

    Renderbuffer renderbuffer;
    renderbuffer.setStorage(RenderbufferFormat::RGBA8, Vector2i(32));

    Framebuffer framebuffer({{}, Vector2i(32)});
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer);

    Buffer buffer;
    constexpr Vector2 triangle[] = {{-1.0f, 1.0f}, {-1.0f, -3.0f}, {3.0f, 1.0f}};
    buffer.setData(triangle, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Triangles)
        .setCount(3)
        .addVertexBuffer(buffer, 0, Attribute<0, Vector2>{});

    MyShader shader;
    framebuffer.bind();

    MAGNUM_VERIFY_NO_GL_ERROR();

    SampleQuery qYes{SampleQuery::Target::SamplesPassed},
        qNo{SampleQuery::Target::SamplesPassed},
        q{SampleQuery::Target::SamplesPassed};

    /* This should generate some samples */
    qYes.begin();
    shader.draw(mesh);
    qYes.end();

    /* Thus this should be rendered */
    qYes.beginConditionalRender(SampleQuery::ConditionalRenderMode::Wait);
    q.begin();
    shader.draw(mesh);
    q.end();
    qYes.endConditionalRender();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(qYes.result<bool>());
    CORRADE_VERIFY(q.result<bool>());

    /* This shouldn't generate any samples */
    qNo.begin();
    qNo.end();

    /* Thus this should not be rendered */
    qNo.beginConditionalRender(SampleQuery::ConditionalRenderMode::Wait);
    q.begin();
    shader.draw(mesh);
    q.end();
    qNo.endConditionalRender();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(!qNo.result<bool>());
    CORRADE_VERIFY(!q.result<bool>());
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::SampleQueryGLTest)
