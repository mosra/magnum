/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/RenderbufferFormat.h"
#include "Magnum/Shader.h"
#include "Magnum/TransformFeedback.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

struct TransformFeedbackGLTest: AbstractOpenGLTester {
    explicit TransformFeedbackGLTest();

    void construct();
    void constructNoCreate();
    void constructCopy();
    void constructMove();
    void wrap();

    void label();

    void attachBase();
    void attachRange();
    void attachBases();
    void attachRanges();

    #ifndef MAGNUM_TARGET_GLES
    void interleaved();
    #endif
};

TransformFeedbackGLTest::TransformFeedbackGLTest() {
    addTests({&TransformFeedbackGLTest::construct,
              &TransformFeedbackGLTest::constructNoCreate,
              &TransformFeedbackGLTest::constructCopy,
              &TransformFeedbackGLTest::constructMove,
              &TransformFeedbackGLTest::wrap,

              &TransformFeedbackGLTest::label,

              &TransformFeedbackGLTest::attachBase,
              &TransformFeedbackGLTest::attachRange,
              &TransformFeedbackGLTest::attachBases,
              &TransformFeedbackGLTest::attachRanges,

              #ifndef MAGNUM_TARGET_GLES
              &TransformFeedbackGLTest::interleaved
              #endif
              });
}

void TransformFeedbackGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    {
        TransformFeedback feedback;

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_VERIFY(feedback.id() > 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void TransformFeedbackGLTest::constructNoCreate() {
    {
        TransformFeedback feedback{NoCreate};

        MAGNUM_VERIFY_NO_ERROR();
        CORRADE_COMPARE(feedback.id(), 0);
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void TransformFeedbackGLTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<TransformFeedback, const TransformFeedback&>{}));
    CORRADE_VERIFY(!(std::is_assignable<TransformFeedback, const TransformFeedback&>{}));
}

void TransformFeedbackGLTest::constructMove() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    TransformFeedback a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(id > 0);

    TransformFeedback b{std::move(a)};

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    TransformFeedback c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
}

void TransformFeedbackGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    GLuint id;
    glGenTransformFeedbacks(1, &id);

    /* Releasing won't delete anything */
    {
        auto feedback = TransformFeedback::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(feedback.release(), id);
    }

    /* ...so we can wrap it again */
    TransformFeedback::wrap(id);
    glDeleteTransformFeedbacks(1, &id);
}

void TransformFeedbackGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    TransformFeedback feedback;

    CORRADE_COMPARE(feedback.label(), "");
    MAGNUM_VERIFY_NO_ERROR();

    feedback.setLabel("MyXfb");
    MAGNUM_VERIFY_NO_ERROR();

    CORRADE_COMPARE(feedback.label(), "MyXfb");
}

namespace {

constexpr const Vector2 inputData[] = {
    {0.0f, 0.0f},
    {-1.0f, 1.0f}
};

struct XfbShader: AbstractShaderProgram {
    typedef Attribute<0, Vector2> Input;

    explicit XfbShader();
};

XfbShader::XfbShader() {
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
        "in mediump vec2 inputData;\n"
        "out mediump vec2 outputData;\n"
        "void main() {\n"
        "    outputData = inputData + vec2(1.0, -1.0);\n"
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
    bindAttributeLocation(Input::Location, "inputData");
    setTransformFeedbackOutputs({"outputData"}, TransformFeedbackBufferMode::SeparateAttributes);
    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

}

void TransformFeedbackGLTest::attachBase() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    XfbShader shader;

    Buffer input;
    input.setData(inputData, BufferUsage::StaticDraw);
    Buffer output;
    output.setData({nullptr, 2*sizeof(Vector2)}, BufferUsage::StaticRead);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Points)
        .addVertexBuffer(input, 0, XfbShader::Input{})
        .setCount(2);

    TransformFeedback feedback;
    feedback.attachBuffer(0, output);

    MAGNUM_VERIFY_NO_ERROR();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Points);
    mesh.draw(shader);
    feedback.end();

    MAGNUM_VERIFY_NO_ERROR();

    Vector2* data = output.map<Vector2>(0, 2*sizeof(Vector2), Buffer::MapFlag::Read);
    CORRADE_COMPARE(data[0], Vector2(1.0f, -1.0f));
    CORRADE_COMPARE(data[1], Vector2(0.0f, 0.0f));
    output.unmap();
}

void TransformFeedbackGLTest::attachRange() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    XfbShader shader;

    Buffer input;
    input.setData(inputData, BufferUsage::StaticDraw);
    Buffer output;
    output.setData({nullptr, 512 + 2*sizeof(Vector2)}, BufferUsage::StaticRead);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Points)
        .addVertexBuffer(input, 0, XfbShader::Input{})
        .setCount(2);

    TransformFeedback feedback;
    feedback.attachBuffer(0, output, 256, 2*sizeof(Vector2));

    MAGNUM_VERIFY_NO_ERROR();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Points);
    mesh.draw(shader);
    feedback.end();

    MAGNUM_VERIFY_NO_ERROR();

    Vector2* data = output.map<Vector2>(256, 2*sizeof(Vector2), Buffer::MapFlag::Read);
    CORRADE_COMPARE(data[0], Vector2(1.0f, -1.0f));
    CORRADE_COMPARE(data[1], Vector2(0.0f, 0.0f));
    output.unmap();
}

namespace {

struct XfbMultiShader: AbstractShaderProgram {
    typedef Attribute<0, Vector2> Input;

    explicit XfbMultiShader();
};

XfbMultiShader::XfbMultiShader() {
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
        "in mediump vec2 inputData;\n"
        "out mediump vec2 output1;\n"
        "out mediump float output2;\n"
        "void main() {\n"
        "    output1 = inputData + vec2(1.0, -1.0);\n"
        "    output2 = inputData.x - inputData.y;\n"
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
    bindAttributeLocation(Input::Location, "inputData");
    setTransformFeedbackOutputs({"output1", "output2"}, TransformFeedbackBufferMode::SeparateAttributes);
    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

}

void TransformFeedbackGLTest::attachBases() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    XfbMultiShader shader;

    Buffer input;
    input.setData(inputData, BufferUsage::StaticDraw);
    Buffer output1, output2;
    output1.setData({nullptr, 2*sizeof(Vector2)}, BufferUsage::StaticRead);
    output2.setData({nullptr, 2*sizeof(Float)}, BufferUsage::StaticRead);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Points)
        .addVertexBuffer(input, 0, XfbMultiShader::Input{})
        .setCount(2);

    TransformFeedback feedback;
    feedback.attachBuffers(0, {&output1, &output2});

    MAGNUM_VERIFY_NO_ERROR();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Points);
    mesh.draw(shader);
    feedback.end();

    MAGNUM_VERIFY_NO_ERROR();

    Vector2* data1 = output1.map<Vector2>(0, 2*sizeof(Vector2), Buffer::MapFlag::Read);
    CORRADE_COMPARE(data1[0], Vector2(1.0f, -1.0f));
    CORRADE_COMPARE(data1[1], Vector2(0.0f, 0.0f));
    output1.unmap();

    Float* data2 = output2.map<Float>(0, 2*sizeof(Float), Buffer::MapFlag::Read);
    CORRADE_COMPARE(data2[0], 0.0f);
    CORRADE_COMPARE(data2[1], -2.0f);
    output2.unmap();
}

void TransformFeedbackGLTest::attachRanges() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    Buffer input;
    input.setData(inputData, BufferUsage::StaticDraw);
    Buffer output1, output2;
    output1.setData({nullptr, 512 + 2*sizeof(Vector2)}, BufferUsage::StaticRead);
    output2.setData({nullptr, 768 + 2*sizeof(Float)}, BufferUsage::StaticRead);

    XfbMultiShader shader;

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Points)
        .addVertexBuffer(input, 0, XfbMultiShader::Input{})
        .setCount(2);

    TransformFeedback feedback;
    feedback.attachBuffers(0, {
        std::make_tuple(&output1, 256, 2*sizeof(Vector2)),
        std::make_tuple(&output2, 512, 2*sizeof(Float))
    });

    MAGNUM_VERIFY_NO_ERROR();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Points);
    mesh.draw(shader);
    feedback.end();

    MAGNUM_VERIFY_NO_ERROR();

    Vector2* data1 = output1.map<Vector2>(256, 2*sizeof(Vector2), Buffer::MapFlag::Read);
    CORRADE_COMPARE(data1[0], Vector2(1.0f, -1.0f));
    CORRADE_COMPARE(data1[1], Vector2(0.0f, 0.0f));
    output1.unmap();

    Float* data2 = output2.map<Float>(512, 2*sizeof(Float), Buffer::MapFlag::Read);
    CORRADE_COMPARE(data2[0], 0.0f);
    CORRADE_COMPARE(data2[1], -2.0f);
    output2.unmap();
}

#ifndef MAGNUM_TARGET_GLES
void TransformFeedbackGLTest::interleaved() {
    /* ARB_transform_feedback3 needed for gl_SkipComponents1 */
    if(!Context::current().isExtensionSupported<Extensions::GL::ARB::transform_feedback3>())
        CORRADE_SKIP(Extensions::GL::ARB::transform_feedback3::string() + std::string(" is not supported."));

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    struct XfbInterleavedShader: AbstractShaderProgram {
        typedef Attribute<0, Vector2> Input;

        explicit XfbInterleavedShader() {
            Shader vert(
                #ifndef CORRADE_TARGET_APPLE
                Version::GL300
                #else
                Version::GL310
                #endif
                , Shader::Type::Vertex);
            CORRADE_INTERNAL_ASSERT_OUTPUT(vert.addSource(
                "in mediump vec2 inputData;\n"
                "out mediump vec2 output1;\n"
                "out mediump float output2;\n"
                "void main() {\n"
                "    output1 = inputData + vec2(1.0, -1.0);\n"
                "    output2 = inputData.x - inputData.y + 5.0;\n"
                /* Mesa drivers complain that vertex shader doesn't write to
                   gl_Position otherwise */
                "    gl_Position = vec4(1.0);\n"
                "}\n").compile());
            attachShader(vert);
            bindAttributeLocation(Input::Location, "inputData");
            setTransformFeedbackOutputs({"output1", "gl_SkipComponents1", "output2"}, TransformFeedbackBufferMode::InterleavedAttributes);
            CORRADE_INTERNAL_ASSERT_OUTPUT(link());
        }
    } shader;

    Buffer input;
    input.setData(inputData, BufferUsage::StaticDraw);
    Buffer output;
    output.setData({nullptr, 4*sizeof(Vector2)}, BufferUsage::StaticRead);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Points)
        .addVertexBuffer(input, 0, XfbInterleavedShader::Input{})
        .setCount(2);

    TransformFeedback feedback;
    feedback.attachBuffer(0, output);

    MAGNUM_VERIFY_NO_ERROR();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Points);
    mesh.draw(shader);
    feedback.end();

    MAGNUM_VERIFY_NO_ERROR();

    Vector2* data = output.map<Vector2>(0, 4*sizeof(Vector2), Buffer::MapFlag::Read);
    CORRADE_COMPARE(data[0], Vector2(1.0f, -1.0f));
    CORRADE_COMPARE(data[1].y(), 5.0f);
    CORRADE_COMPARE(data[2], Vector2(0.0f, 0.0f));
    CORRADE_COMPARE(data[3].y(), 3.0f);
    output.unmap();
}
#endif

}}

MAGNUM_GL_TEST_MAIN(Magnum::Test::TransformFeedbackGLTest)
