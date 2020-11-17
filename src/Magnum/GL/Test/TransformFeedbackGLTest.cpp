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

#include <tuple>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Image.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/PrimitiveQuery.h"
#include "Magnum/GL/SampleQuery.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/TransformFeedback.h"
#include "Magnum/Math/Vector2.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct TransformFeedbackGLTest: OpenGLTester {
    explicit TransformFeedbackGLTest();

    void construct();
    void constructMove();
    void wrap();

    #ifndef MAGNUM_TARGET_WEBGL
    void label();
    #endif

    void attachBase();
    void attachRange();
    void attachBases();
    void attachRanges();

    #ifndef MAGNUM_TARGET_GLES
    void interleaved();

    void draw();
    #endif
};

#ifndef MAGNUM_TARGET_GLES
enum: std::size_t { DrawDataCount = 4 };

const struct {
    const char* name;
    UnsignedInt stream;
    Int instances;
    UnsignedInt countStream0;
    UnsignedInt countStreamN;
    UnsignedInt countDraw;
} DrawData[DrawDataCount] = {
    {"basic", 0, 1, 6, 6, 6},
    {"instanced", 0, 5, 6, 6, 30},
    {"stream", 1, 1, 0, 6, 6},
    {"streamInstanced", 1, 5, 0, 6, 30}
};
#endif

TransformFeedbackGLTest::TransformFeedbackGLTest() {
    addTests({&TransformFeedbackGLTest::construct,
              &TransformFeedbackGLTest::constructMove,
              &TransformFeedbackGLTest::wrap,

              #ifndef MAGNUM_TARGET_WEBGL
              &TransformFeedbackGLTest::label,
              #endif

              &TransformFeedbackGLTest::attachBase,
              &TransformFeedbackGLTest::attachRange,
              &TransformFeedbackGLTest::attachBases,
              &TransformFeedbackGLTest::attachRanges,

              #ifndef MAGNUM_TARGET_GLES
              &TransformFeedbackGLTest::interleaved,
              #endif
              });

    #ifndef MAGNUM_TARGET_GLES
    addInstancedTests({&TransformFeedbackGLTest::draw}, DrawDataCount);
    #endif
}

void TransformFeedbackGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    {
        TransformFeedback feedback;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(feedback.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TransformFeedbackGLTest::constructMove() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    TransformFeedback a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    TransformFeedback b{std::move(a)};

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    TransformFeedback c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<TransformFeedback>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<TransformFeedback>::value);
}

void TransformFeedbackGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback2::string() + std::string(" is not supported."));
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

#ifndef MAGNUM_TARGET_WEBGL
void TransformFeedbackGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    TransformFeedback feedback;

    CORRADE_COMPARE(feedback.label(), "");
    {
        #ifdef MAGNUM_TARGET_GLES
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::NVidia &&
                              !Context::current().isExtensionSupported<Extensions::KHR::debug>(),
            "NVidia 387.34 ES3.2 complains that GL_TRANSFORM_FEEDBACK can't be used with glGetObjectLabelEXT().");
        #endif
        MAGNUM_VERIFY_NO_GL_ERROR();
    }

    feedback.setLabel("MyXfb");
    {
        #ifdef MAGNUM_TARGET_GLES
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::NVidia &&
                              !Context::current().isExtensionSupported<Extensions::KHR::debug>(),
            "NVidia 387.34 ES3.2 complains that GL_TRANSFORM_FEEDBACK can't be used with glGetObjectLabelEXT().");
        #endif
        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(feedback.label(), "MyXfb");
        MAGNUM_VERIFY_NO_GL_ERROR(); /* Check for errors again to flush the error state */
    }
}
#endif

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

void TransformFeedbackGLTest::attachBase() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    XfbShader shader;

    Buffer input{Buffer::TargetHint::Array};
    input.setData(inputData, BufferUsage::StaticDraw);
    Buffer output{Buffer::TargetHint::TransformFeedback};
    output.setData({nullptr, 2*sizeof(Vector2)}, BufferUsage::StaticRead);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Points)
        .addVertexBuffer(input, 0, XfbShader::Input{})
        .setCount(2);

    TransformFeedback feedback;
    feedback.attachBuffer(0, output);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Points);
    shader.draw(mesh);
    feedback.end();

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_SKIP("Can't map buffers on WebGL.");
    #else
    auto data = Containers::arrayCast<const Vector2>(output.mapRead(0, 2*sizeof(Vector2)));
    CORRADE_COMPARE(data[0], Vector2(1.0f, -1.0f));
    CORRADE_COMPARE(data[1], Vector2(0.0f, 0.0f));
    output.unmap();
    #endif
}

void TransformFeedbackGLTest::attachRange() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    XfbShader shader;

    Buffer input{Buffer::TargetHint::Array};
    input.setData(inputData, BufferUsage::StaticDraw);
    Buffer output{Buffer::TargetHint::TransformFeedback};
    output.setData({nullptr, 512 + 2*sizeof(Vector2)}, BufferUsage::StaticRead);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Points)
        .addVertexBuffer(input, 0, XfbShader::Input{})
        .setCount(2);

    TransformFeedback feedback;
    feedback.attachBuffer(0, output, 256, 2*sizeof(Vector2));

    MAGNUM_VERIFY_NO_GL_ERROR();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Points);
    shader.draw(mesh);
    feedback.end();

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_SKIP("Can't map buffers on WebGL.");
    #else
    auto data = Containers::arrayCast<const Vector2>(output.mapRead(256, 2*sizeof(Vector2)));
    CORRADE_COMPARE(data[0], Vector2(1.0f, -1.0f));
    CORRADE_COMPARE(data[1], Vector2(0.0f, 0.0f));
    output.unmap();
    #endif
}

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

void TransformFeedbackGLTest::attachBases() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    XfbMultiShader shader;

    Buffer input{Buffer::TargetHint::Array};
    input.setData(inputData, BufferUsage::StaticDraw);
    Buffer output1{Buffer::TargetHint::TransformFeedback},
        output2{Buffer::TargetHint::TransformFeedback};
    output1.setData({nullptr, 2*sizeof(Vector2)}, BufferUsage::StaticRead);
    output2.setData({nullptr, 2*sizeof(Float)}, BufferUsage::StaticRead);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Points)
        .addVertexBuffer(input, 0, XfbMultiShader::Input{})
        .setCount(2);

    TransformFeedback feedback;
    feedback.attachBuffers(0, {&output1, &output2});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Points);
    shader.draw(mesh);
    feedback.end();

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_SKIP("Can't map buffers on WebGL.");
    #else
    auto data1 = Containers::arrayCast<const Vector2>(output1.mapRead(0, 2*sizeof(Vector2)));
    CORRADE_COMPARE(data1[0], Vector2(1.0f, -1.0f));
    CORRADE_COMPARE(data1[1], Vector2(0.0f, 0.0f));
    output1.unmap();

    auto data2 = Containers::arrayCast<const Float>(output2.mapRead(0, 2*sizeof(Float)));
    CORRADE_COMPARE(data2[0], 0.0f);
    CORRADE_COMPARE(data2[1], -2.0f);
    output2.unmap();
    #endif
}

void TransformFeedbackGLTest::attachRanges() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback2::string() + std::string(" is not supported."));
    #endif

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{32});
    Framebuffer fb{{{}, Vector2i{32}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    Buffer input{Buffer::TargetHint::Array};
    input.setData(inputData, BufferUsage::StaticDraw);
    Buffer output1{Buffer::TargetHint::TransformFeedback},
        output2{Buffer::TargetHint::TransformFeedback};
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

    MAGNUM_VERIFY_NO_GL_ERROR();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Points);
    shader.draw(mesh);
    feedback.end();

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_SKIP("Can't map buffers on WebGL.");
    #else
    auto data1 = Containers::arrayCast<const Vector2>(output1.mapRead(256, 2*sizeof(Vector2)));
    CORRADE_COMPARE(data1[0], Vector2(1.0f, -1.0f));
    CORRADE_COMPARE(data1[1], Vector2(0.0f, 0.0f));
    output1.unmap();

    auto data2 = Containers::arrayCast<const Float>(output2.mapRead(512, 2*sizeof(Float)));
    CORRADE_COMPARE(data2[0], 0.0f);
    CORRADE_COMPARE(data2[1], -2.0f);
    output2.unmap();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void TransformFeedbackGLTest::interleaved() {
    /* ARB_transform_feedback3 needed for gl_SkipComponents1 */
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback3>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback3::string() + std::string(" is not supported."));

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

    Buffer input{Buffer::TargetHint::Array};
    input.setData(inputData, BufferUsage::StaticDraw);
    Buffer output{Buffer::TargetHint::TransformFeedback};
    output.setData({nullptr, 4*sizeof(Vector2)}, BufferUsage::StaticRead);

    Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Points)
        .addVertexBuffer(input, 0, XfbInterleavedShader::Input{})
        .setCount(2);

    TransformFeedback feedback;
    feedback.attachBuffer(0, output);

    MAGNUM_VERIFY_NO_GL_ERROR();

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(shader, TransformFeedback::PrimitiveMode::Points);
    shader.draw(mesh);
    feedback.end();

    MAGNUM_VERIFY_NO_GL_ERROR();

    auto data = Containers::arrayCast<const Vector2>(output.mapRead(0, 4*sizeof(Vector2)));
    CORRADE_COMPARE(data[0], Vector2(1.0f, -1.0f));
    CORRADE_COMPARE(data[1].y(), 5.0f);
    CORRADE_COMPARE(data[2], Vector2(0.0f, 0.0f));
    CORRADE_COMPARE(data[3].y(), 3.0f);
    output.unmap();
}

void TransformFeedbackGLTest::draw() {
    setTestCaseDescription(DrawData[testCaseInstanceId()].name);

    /* ARB_transform_feedback2 needed as base, other optional */
    if(!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback2>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback2::string() + std::string(" is not supported."));
    if(DrawData[testCaseInstanceId()].stream && (!Context::current().isExtensionSupported<Extensions::ARB::transform_feedback3>() || TransformFeedback::maxVertexStreams() < 2))
        CORRADE_SKIP(Extensions::ARB::transform_feedback3::string() + std::string(" is not supported well enough."));
    if(DrawData[testCaseInstanceId()].instances && !Context::current().isExtensionSupported<Extensions::ARB::transform_feedback_instanced>())
        CORRADE_SKIP(Extensions::ARB::transform_feedback_instanced::string() + std::string(" is not supported."));

    /* Bind some FB to avoid errors on contexts w/o default FB */
    Renderbuffer color;
    color.setStorage(RenderbufferFormat::RGBA8, Vector2i{1});
    Framebuffer fb{{{}, Vector2i{1}}};
    fb.attachRenderbuffer(Framebuffer::ColorAttachment{0}, color)
      .bind();

    struct XfbShader: AbstractShaderProgram {
        explicit XfbShader(UnsignedInt stream) {
            Shader vert{Version::GL320, Shader::Type::Vertex},
                geom{Version::GL320, Shader::Type::Geometry};
            vert.addSource(
                "out mediump vec2 vertexOutput;\n"
                "void main() {\n"
                "    vertexOutput = vec2(0.3);\n"
                "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
                "}\n");
            if(stream) geom.addSource(
                "#extension GL_ARB_gpu_shader5: require\n"
                "#define STREAM " + std::to_string(stream) + "\n" +
                "layout(stream = 0) out mediump float otherOutput;\n" +
                "layout(stream = STREAM) out mediump vec2 geomOutput;\n");
            else geom.addSource(
                "out mediump vec2 geomOutput;\n");
            geom.addSource(
                "layout(points) in;\n"
                "layout(points, max_vertices = 1) out;\n"
                "in mediump vec2 vertexOutput[];\n"
                "void main() {\n"
                "    geomOutput = vertexOutput[0] - vec2(0.1);\n");
            if(stream) geom.addSource(
                "    EmitStreamVertex(STREAM);\n");
            else geom.addSource(
                "    EmitVertex();\n");
            geom.addSource("}\n");
            CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, geom}));
            attachShaders({vert, geom});
            setTransformFeedbackOutputs({"geomOutput"}, TransformFeedbackBufferMode::SeparateAttributes);
            CORRADE_INTERNAL_ASSERT_OUTPUT(link());
        }
    } xfbShader{DrawData[testCaseInstanceId()].stream};

    Buffer outputBuffer;
    outputBuffer.setData({nullptr, 32*sizeof(Vector2)}, BufferUsage::StaticDraw);

    Mesh inputMesh;
    inputMesh.setPrimitive(MeshPrimitive::Points)
        .setCount(6);

    TransformFeedback feedback;
    feedback.attachBuffer(0, outputBuffer);

    MAGNUM_VERIFY_NO_GL_ERROR();

    PrimitiveQuery queryStream0{PrimitiveQuery::Target::TransformFeedbackPrimitivesWritten},
        queryStreamN{PrimitiveQuery::Target::TransformFeedbackPrimitivesWritten};

    queryStream0.begin();
    if(DrawData[testCaseInstanceId()].stream)
        queryStreamN.begin(DrawData[testCaseInstanceId()].stream);

    Renderer::enable(Renderer::Feature::RasterizerDiscard);
    feedback.begin(xfbShader, TransformFeedback::PrimitiveMode::Points);
    xfbShader.draw(inputMesh);
    feedback.end();
    Renderer::disable(Renderer::Feature::RasterizerDiscard);

    if(DrawData[testCaseInstanceId()].stream)
        queryStreamN.end();
    queryStream0.end();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(queryStream0.result<UnsignedInt>(), DrawData[testCaseInstanceId()].countStream0);
    if(DrawData[testCaseInstanceId()].stream)
        CORRADE_COMPARE(queryStreamN.result<UnsignedInt>(), DrawData[testCaseInstanceId()].countStreamN);

    struct DrawShader: AbstractShaderProgram {
        typedef Attribute<0, Vector2> Input;

        explicit DrawShader() {
            Shader vert{Version::GL320, Shader::Type::Vertex},
                frag{Version::GL320, Shader::Type::Fragment};
            vert.addSource(
                "in mediump vec2 inputData;\n"
                "out mediump vec2 interleaved;\n"
                "void main() {\n"
                "    interleaved = inputData;\n"
                "    gl_Position = vec4(1.0);\n"
                "}\n");
            frag.addSource(
                "in mediump vec2 interleaved;\n"
                "out mediump float outputData;\n"
                "void main() {\n"
                "    outputData = interleaved.x + 2*interleaved.y;\n"
                "}\n");

            CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));
            attachShaders({vert, frag});
            bindAttributeLocation(Input::Location, "inputData");
            CORRADE_INTERNAL_ASSERT_OUTPUT(link());
        }
    } drawShader;

    Renderer::setPointSize(2.0f);

    Mesh outputMesh;
    outputMesh.setPrimitive(MeshPrimitive::Points)
        .setInstanceCount(DrawData[testCaseInstanceId()].instances)
        .addVertexBuffer(outputBuffer, 0, DrawShader::Input{});

    PrimitiveQuery q{PrimitiveQuery::Target::PrimitivesGenerated};
    q.begin();
    drawShader.drawTransformFeedback(outputMesh, feedback, DrawData[testCaseInstanceId()].stream);
    q.end();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(q.result<UnsignedInt>(), DrawData[testCaseInstanceId()].countDraw);
    CORRADE_COMPARE(Containers::arrayCast<UnsignedByte>(fb.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 153);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::TransformFeedbackGLTest)
