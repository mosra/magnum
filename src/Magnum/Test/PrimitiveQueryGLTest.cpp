/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Resource.h>

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/PrimitiveQuery.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/RenderbufferFormat.h"
#include "Magnum/Shader.h"
#include "Magnum/TransformFeedback.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

struct PrimitiveQueryGLTest: AbstractOpenGLTester {
    explicit PrimitiveQueryGLTest();

    void constructNoCreate();
    void wrap();

    #ifndef MAGNUM_TARGET_GLES
    void primitivesGenerated();
    #endif
    void transformFeedbackPrimitivesWritten();
};

PrimitiveQueryGLTest::PrimitiveQueryGLTest() {
    addTests({&PrimitiveQueryGLTest::constructNoCreate,
              &PrimitiveQueryGLTest::wrap,

              #ifndef MAGNUM_TARGET_GLES
              &PrimitiveQueryGLTest::primitivesGenerated,
              #endif
              &PrimitiveQueryGLTest::transformFeedbackPrimitivesWritten});
}

void PrimitiveQueryGLTest::constructNoCreate() {
    {
        PrimitiveQuery query{NoCreate};

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_COMPARE(query.id(), 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void PrimitiveQueryGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback2::string() + std::string(" is not available."));
    #endif

    GLuint id;
    glGenQueries(1, &id);

    /* Releasing won't delete anything */
    {
        auto query = PrimitiveQuery::wrap(id, PrimitiveQuery::Target::TransformFeedbackPrimitivesWritten, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(query.release(), id);
    }

    /* ...so we can wrap it again */
    PrimitiveQuery::wrap(id, PrimitiveQuery::Target::TransformFeedbackPrimitivesWritten);
    glDeleteQueries(1, &id);
}

#ifndef MAGNUM_TARGET_GLES
void PrimitiveQueryGLTest::primitivesGenerated() {
    if(!Context::current().isExtensionSupported<Extensions::GL::EXT::transform_feedback>())
        CORRADE_SKIP(Extensions::GL::EXT::transform_feedback::string() + std::string(" is not available."));

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
                #ifndef CORRADE_TARGET_APPLE
                Version::GL210
                #else
                Version::GL310
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

    MAGNUM_VERIFY_NO_ERROR();

    PrimitiveQuery q{PrimitiveQuery::Target::PrimitivesGenerated};
    q.begin();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    mesh.draw(shader);

    q.end();
    const bool availableBefore = q.resultAvailable();
    const UnsignedInt count = q.result<UnsignedInt>();
    const bool availableAfter = q.resultAvailable();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(!availableBefore);
    CORRADE_VERIFY(availableAfter);
    CORRADE_COMPARE(count, 3);
}
#endif

void PrimitiveQueryGLTest::transformFeedbackPrimitivesWritten() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback2::string() + std::string(" is not available."));
    #endif

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    struct MyShader: AbstractShaderProgram {
        explicit MyShader() {
            #ifndef MAGNUM_TARGET_GLES
            Shader vert(
                #ifndef CORRADE_TARGET_APPLE
                Version::GL300
                #else
                Version::GL310
                #endif
                , Shader::Type::Vertex);
            #else
            Shader vert(Version::GLES300, Shader::Type::Vertex);
            Shader frag(Version::GLES300, Shader::Type::Fragment);
            #endif

            CORRADE_INTERNAL_ASSERT_OUTPUT(vert.addSource(
                "out mediump vec2 outputData;\n"
                "void main() {\n"
                "    outputData = vec2(1.0, -1.0);\n"
                /* Mesa drivers complain that vertex shader doesn't write to
                   gl_Position otherwise */
                "    gl_Position = vec4(1.0);\n"
                "}\n").compile());
            #ifndef MAGNUM_TARGET_GLES
            attachShader(vert);
            #else
            /* ES for some reason needs both vertex and fragment shader */
            CORRADE_INTERNAL_ASSERT_OUTPUT(frag.addSource("void main() {}\n").compile());
            attachShaders({vert, frag});
            #endif

            setTransformFeedbackOutputs({"outputData"}, TransformFeedbackBufferMode::SeparateAttributes);
            CORRADE_INTERNAL_ASSERT_OUTPUT(link());
        }
    } shader;

    Buffer output;
    output.setData({nullptr, 18*sizeof(Vector2)}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Triangles)
        .setCount(9);

    MAGNUM_VERIFY_NO_ERROR();

    TransformFeedback feedback;
    feedback.attachBuffer(0, output);

    PrimitiveQuery q{PrimitiveQuery::Target::TransformFeedbackPrimitivesWritten};
    q.begin();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);

    mesh.draw(shader); /* Draw once without XFB (shouldn't be counted) */
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Triangles);
    mesh.draw(shader);
    feedback.end();

    q.end();
    const UnsignedInt count = q.result<UnsignedInt>();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(count, 3); /* Three triangles (9 vertices) */
}

}}

MAGNUM_GL_TEST_MAIN(Magnum::Test::PrimitiveQueryGLTest)
