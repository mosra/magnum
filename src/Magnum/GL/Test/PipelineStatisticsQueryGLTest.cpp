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

#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PipelineStatisticsQuery.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct PipelineStatisticsQueryGLTest: OpenGLTester {
    explicit PipelineStatisticsQueryGLTest();

    void constructMove();
    void wrap();

    void queryVerticesSubmitted();
};

PipelineStatisticsQueryGLTest::PipelineStatisticsQueryGLTest() {
    addTests({&PipelineStatisticsQueryGLTest::constructMove,
              &PipelineStatisticsQueryGLTest::wrap,

              &PipelineStatisticsQueryGLTest::queryVerticesSubmitted});
}

void PipelineStatisticsQueryGLTest::constructMove() {
    /* Move constructor tested in AbstractQuery, here we just verify there
       are no extra members that would need to be taken care of */
    CORRADE_COMPARE(sizeof(PipelineStatisticsQuery), sizeof(AbstractQuery));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<PipelineStatisticsQuery>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<PipelineStatisticsQuery>::value);
}

void PipelineStatisticsQueryGLTest::wrap() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::pipeline_statistics_query>())
        CORRADE_SKIP(Extensions::ARB::pipeline_statistics_query::string() + std::string(" is not available"));

    GLuint id;
    glGenQueries(1, &id);

    /* Releasing won't delete anything */
    {
        auto query = PipelineStatisticsQuery::wrap(id, PipelineStatisticsQuery::Target::ClippingInputPrimitives, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(query.release(), id);
    }

    /* ...so we can wrap it again */
    PipelineStatisticsQuery::wrap(id, PipelineStatisticsQuery::Target::ClippingInputPrimitives);
    glDeleteQueries(1, &id);
}

void PipelineStatisticsQueryGLTest::queryVerticesSubmitted() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::pipeline_statistics_query>())
        CORRADE_SKIP(Extensions::ARB::pipeline_statistics_query::string() + std::string(" is not available"));

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    struct MyShader: AbstractShaderProgram {
        typedef Attribute<0, Vector2> Position;

        explicit MyShader() {
            Shader vert(
                #ifdef CORRADE_TARGET_APPLE
                Version::GL310
                #else
                Version::GL210
                #endif
                , Shader::Type::Vertex);

            CORRADE_INTERNAL_ASSERT_OUTPUT(vert.addSource(
                "#if __VERSION__ >= 130\n"
                "#define attribute in\n"
                "#endif\n"
                "attribute vec4 position;\n"
                "void main() {\n"
                "    gl_Position = position;\n"
                "}\n").compile());

            attachShader(vert);
            bindAttributeLocation(Position::Location, "position");
            CORRADE_INTERNAL_ASSERT_OUTPUT(link());
        }
    } shader;

    Buffer vertices;
    vertices.setData({nullptr, 9*sizeof(Vector2)}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Triangles)
        .setCount(9)
        .addVertexBuffer(vertices, 0, MyShader::Position());

    MAGNUM_VERIFY_NO_GL_ERROR();

    PipelineStatisticsQuery q{PipelineStatisticsQuery::Target::VerticesSubmitted};
    q.begin();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
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
    CORRADE_COMPARE(count, 9);
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::PipelineStatisticsQueryGLTest)
