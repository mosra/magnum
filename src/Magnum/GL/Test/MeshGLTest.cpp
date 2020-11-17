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

#include <sstream>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Image.h"
#include "Magnum/Mesh.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Vector4.h"

namespace Magnum { namespace GL { namespace Test { namespace {

/* Tests also the MeshView class. */

struct MeshGLTest: OpenGLTester {
    explicit MeshGLTest();

    void construct();
    void constructMove();
    void wrap();

    template<class T> void primitive();

    #ifndef MAGNUM_TARGET_WEBGL
    void label();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferUnsignedInt();
    void addVertexBufferInt();
    #endif
    void addVertexBufferFloat();
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferDouble();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferVectorNui();
    void addVertexBufferVectorNi();
    #endif
    void addVertexBufferVectorN();
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferVectorNd();
    #endif
    void addVertexBufferMatrixNxN();
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferMatrixNxNd();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferMatrixMxN();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferMatrixMxNd();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferUnsignedIntWithUnsignedShort();
    void addVertexBufferUnsignedIntWithShort();
    void addVertexBufferIntWithUnsignedShort();
    void addVertexBufferIntWithShort();
    #endif
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /* Other Float types omitted (covered by addVertexBufferNormalized()) */
    void addVertexBufferFloatWithHalf();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferFloatWithDouble();
    void addVertexBufferVector3WithUnsignedInt10f11f11fRev();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferVector4WithUnsignedInt2101010Rev();
    void addVertexBufferVector4WithInt2101010Rev();
    #endif

    void addVertexBufferLessVectorComponents();
    void addVertexBufferNormalized();
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferBGRA();
    #endif

    void addVertexBufferMultiple();
    void addVertexBufferMultipleGaps();

    void addVertexBufferMovedOutInstance();
    void addVertexBufferTransferOwnwership();
    void addVertexBufferInstancedTransferOwnwership();
    void addVertexBufferDynamicTransferOwnwership();
    void addVertexBufferInstancedDynamicTransferOwnwership();

    template<class T> void setIndexBuffer();
    template<class T> void setIndexBufferRange();
    void setIndexBufferUnsignedInt();

    void setIndexBufferMovedOutInstance();
    template<class T> void setIndexBufferTransferOwnership();
    template<class T> void setIndexBufferRangeTransferOwnership();

    void unbindVAOWhenSettingIndexBufferData();
    void unbindIndexBufferWhenBindingVao();
    void resetIndexBufferBindingWhenBindingVao();
    void unbindVAOBeforeEnteringExternalSection();
    void bindScratchVaoWhenEnteringExternalSection();

    #ifndef MAGNUM_TARGET_GLES
    void setBaseVertex();
    #endif
    void setInstanceCount();
    void setInstanceCountIndexed();
    #ifndef MAGNUM_TARGET_GLES
    void setInstanceCountBaseInstance();
    void setInstanceCountBaseInstanceIndexed();
    void setInstanceCountBaseVertex();
    void setInstanceCountBaseVertexBaseInstance();
    #endif

    void addVertexBufferInstancedFloat();
    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferInstancedInteger();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferInstancedDouble();
    #endif
    void resetDivisorAfterInstancedDraw();

    void multiDraw();
    void multiDrawIndexed();
    #ifndef MAGNUM_TARGET_GLES
    void multiDrawBaseVertex();
    #endif
};

MeshGLTest::MeshGLTest() {
    addTests({&MeshGLTest::construct,
              &MeshGLTest::constructMove,
              &MeshGLTest::wrap,

              &MeshGLTest::primitive<GL::MeshPrimitive>,
              &MeshGLTest::primitive<Magnum::MeshPrimitive>,

              #ifndef MAGNUM_TARGET_WEBGL
              &MeshGLTest::label
              #endif
              });

    /* First instance is always using Attribute, second DynamicAttribute */
    addInstancedTests({
        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::addVertexBufferUnsignedInt,
        &MeshGLTest::addVertexBufferInt,
        #endif
        &MeshGLTest::addVertexBufferFloat,
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferDouble,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::addVertexBufferVectorNui,
        &MeshGLTest::addVertexBufferVectorNi,
        #endif
        &MeshGLTest::addVertexBufferVectorN,
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferVectorNd,
        #endif
        &MeshGLTest::addVertexBufferMatrixNxN,
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferMatrixNxNd,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::addVertexBufferMatrixMxN,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferMatrixMxNd,
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::addVertexBufferUnsignedIntWithUnsignedShort,
        &MeshGLTest::addVertexBufferUnsignedIntWithShort,
        &MeshGLTest::addVertexBufferIntWithUnsignedShort,
        &MeshGLTest::addVertexBufferIntWithShort,
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        &MeshGLTest::addVertexBufferFloatWithHalf,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferFloatWithDouble,
        &MeshGLTest::addVertexBufferVector3WithUnsignedInt10f11f11fRev,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::addVertexBufferVector4WithUnsignedInt2101010Rev,
        &MeshGLTest::addVertexBufferVector4WithInt2101010Rev,
        #endif

        &MeshGLTest::addVertexBufferLessVectorComponents,
        &MeshGLTest::addVertexBufferNormalized,
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferBGRA
        #endif
        }, 2);

    addTests({&MeshGLTest::addVertexBufferMultiple,
              &MeshGLTest::addVertexBufferMultipleGaps,

              &MeshGLTest::addVertexBufferMovedOutInstance,
              &MeshGLTest::addVertexBufferTransferOwnwership,
              &MeshGLTest::addVertexBufferInstancedTransferOwnwership,
              &MeshGLTest::addVertexBufferDynamicTransferOwnwership,
              &MeshGLTest::addVertexBufferInstancedDynamicTransferOwnwership,

              &MeshGLTest::setIndexBuffer<GL::MeshIndexType>,
              &MeshGLTest::setIndexBuffer<Magnum::MeshIndexType>,
              &MeshGLTest::setIndexBufferRange<GL::MeshIndexType>,
              &MeshGLTest::setIndexBufferRange<Magnum::MeshIndexType>,
              &MeshGLTest::setIndexBufferUnsignedInt,

              &MeshGLTest::setIndexBufferMovedOutInstance,
              &MeshGLTest::setIndexBufferTransferOwnership<GL::MeshIndexType>,
              &MeshGLTest::setIndexBufferTransferOwnership<Magnum::MeshIndexType>,
              &MeshGLTest::setIndexBufferRangeTransferOwnership<GL::MeshIndexType>,
              &MeshGLTest::setIndexBufferRangeTransferOwnership<Magnum::MeshIndexType>,

              &MeshGLTest::unbindVAOWhenSettingIndexBufferData,
              &MeshGLTest::unbindIndexBufferWhenBindingVao,
              &MeshGLTest::resetIndexBufferBindingWhenBindingVao,
              &MeshGLTest::unbindVAOBeforeEnteringExternalSection,
              &MeshGLTest::bindScratchVaoWhenEnteringExternalSection,

              #ifndef MAGNUM_TARGET_GLES
              &MeshGLTest::setBaseVertex,
              #endif
              &MeshGLTest::setInstanceCount,
              &MeshGLTest::setInstanceCountIndexed,
              #ifndef MAGNUM_TARGET_GLES
              &MeshGLTest::setInstanceCountBaseInstance,
              &MeshGLTest::setInstanceCountBaseInstanceIndexed,
              &MeshGLTest::setInstanceCountBaseVertex,
              &MeshGLTest::setInstanceCountBaseVertexBaseInstance,
              #endif

              &MeshGLTest::addVertexBufferInstancedFloat,
              #ifndef MAGNUM_TARGET_GLES2
              &MeshGLTest::addVertexBufferInstancedInteger,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &MeshGLTest::addVertexBufferInstancedDouble,
              #endif
              &MeshGLTest::resetDivisorAfterInstancedDraw,

              &MeshGLTest::multiDraw,
              &MeshGLTest::multiDrawIndexed,
              #ifndef MAGNUM_TARGET_GLES
              &MeshGLTest::multiDrawBaseVertex
              #endif
              });
}

using namespace Math::Literals;

void MeshGLTest::construct() {
    {
        const Mesh mesh;

        MAGNUM_VERIFY_NO_GL_ERROR();

        #ifndef MAGNUM_TARGET_GLES
        if(Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        #elif defined(MAGNUM_TARGET_GLES2)
        if(Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        #endif
        {
            CORRADE_VERIFY(mesh.id() > 0);
        }
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

struct FloatShader: AbstractShaderProgram {
    explicit FloatShader(const std::string& type, const std::string& conversion);
};

void MeshGLTest::constructMove() {
    const Float data = Math::unpack<Float, UnsignedByte>(
        #ifndef MAGNUM_TARGET_GLES2
        96
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        85
        #endif
    );
    Buffer buffer1, buffer2;
    buffer1.setData({&data, 1}, BufferUsage::StaticDraw);
    buffer2.setData({nullptr, 8}, BufferUsage::StaticDraw);

    Mesh a;
    a.addVertexBuffer(buffer1, 0, Attribute<0, Float>{});
    const Int id = a.id();

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
    #endif
    {
        CORRADE_VERIFY(id > 0);
    }

    /* Move construct */
    Mesh b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    /* Move assign */
    Mesh c;
    c.addVertexBuffer(buffer2, 4, Attribute<1, Float>{});
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
    #endif
    {
        CORRADE_VERIFY(cId > 0);
    }

    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);

    /* Move assign to a NoCreate instance */
    Mesh d{NoCreate};
    d = std::move(c);

    CORRADE_COMPARE(c.id(), 0);
    CORRADE_COMPARE(d.id(), id);

    /* Destroy */
    b = Mesh{NoCreate};

    /* Test that drawing still works properly */
    {
        MAGNUM_VERIFY_NO_GL_ERROR();

        Renderbuffer renderbuffer;
        renderbuffer.setStorage(
            #ifndef MAGNUM_TARGET_GLES2
            RenderbufferFormat::RGBA8,
            #else
            RenderbufferFormat::RGBA4,
            #endif
            Vector2i(1));
        Framebuffer framebuffer{{{}, Vector2i(1)}};
        framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer)
                   .bind();

        d.setPrimitive(MeshPrimitive::Points)
         .setCount(1);
        FloatShader shader{"float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"};
        shader.draw(d);

        MAGNUM_VERIFY_NO_GL_ERROR();

        #ifndef MAGNUM_TARGET_GLES2
        CORRADE_COMPARE(Containers::arrayCast<UnsignedByte>(framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 96);
        #else /* RGBA4, so less precision */
        CORRADE_COMPARE(Containers::arrayCast<UnsignedByte>(framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 85);
        #endif
    }

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Mesh>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Mesh>::value);
}

void MeshGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() + std::string{" is not supported."});
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() + std::string{" is not supported."});
    #endif

    GLuint id;
    #ifndef MAGNUM_TARGET_GLES2
    glGenVertexArrays(1, &id);
    #else
    glGenVertexArraysOES(1, &id);
    #endif

    /* Releasing won't delete anything */
    {
        auto mesh = Mesh::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(mesh.release(), id);
    }

    /* ...so we can wrap it again */
    Mesh::wrap(id);
    #ifndef MAGNUM_TARGET_GLES2
    glDeleteVertexArrays(1, &id);
    #else
    glDeleteVertexArraysOES(1, &id);
    #endif
}

template<class T> void MeshGLTest::primitive() {
    setTestCaseTemplateName(std::is_same<T, MeshPrimitive>::value ?
        "GL::MeshPrimitive" : "Magnum::MeshPrimitive");

    {
        Mesh mesh{T::LineLoop};
        CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::LineLoop);
    } {
        Mesh mesh;
        mesh.setPrimitive(T::TriangleFan);
        CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::TriangleFan);
    }
}

#ifndef MAGNUM_TARGET_WEBGL
void MeshGLTest::label() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() + std::string(" is not available."));
    #endif

    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Mesh mesh;

    CORRADE_COMPARE(mesh.label(), "");
    MAGNUM_VERIFY_NO_GL_ERROR();

    mesh.setLabel("MyMesh");
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(mesh.label(), "MyMesh");
}
#endif

#ifndef MAGNUM_TARGET_GLES2
struct IntegerShader: AbstractShaderProgram {
    explicit IntegerShader(const std::string& type);
};
#endif

#ifndef MAGNUM_TARGET_GLES
struct DoubleShader: AbstractShaderProgram {
    explicit DoubleShader(const std::string& type, const std::string& outputType, const std::string& conversion);
};
#endif

struct Checker {
    Checker(AbstractShaderProgram&& shader, RenderbufferFormat format, Mesh& mesh);

    template<class T> T get(PixelFormat format, PixelType type);

    Renderbuffer renderbuffer;
    Framebuffer framebuffer;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
FloatShader::FloatShader(const std::string& type, const std::string& conversion) {
    /* We need special version for ES3, because GLSL in ES2 doesn't support
       rectangle matrices */
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
    #elif defined(MAGNUM_TARGET_GLES2)
    Shader vert(Version::GLES200, Shader::Type::Vertex);
    Shader frag(Version::GLES200, Shader::Type::Fragment);
    #else
    Shader vert(Version::GLES300, Shader::Type::Vertex);
    Shader frag(Version::GLES300, Shader::Type::Fragment);
    #endif

    vert.addSource(
        "#if !defined(GL_ES) && __VERSION__ == 120\n"
        "#define mediump\n"
        "#endif\n"
        "#if (defined(GL_ES) && __VERSION__ < 300) || __VERSION__ == 120\n"
        "#define in attribute\n"
        "#define out varying\n"
        "#endif\n"
        "in mediump " + type + " value;\n"
        "out mediump " + type + " valueInterpolated;\n"
        "void main() {\n"
        "    valueInterpolated = value;\n"
        "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}\n");
    frag.addSource(
        "#if !defined(GL_ES) && __VERSION__ == 120\n"
        "#define mediump\n"
        "#endif\n"
        "#if (defined(GL_ES) && __VERSION__ < 300) || __VERSION__ == 120\n"
        "#define in varying\n"
        "#define result gl_FragColor\n"
        "#endif\n"
        "in mediump " + type + " valueInterpolated;\n"
        "#if (defined(GL_ES) && __VERSION__ >= 300) || (!defined(GL_ES) && __VERSION__ >= 130)\n"
        "out mediump vec4 result;\n"
        "#endif\n"
        "void main() { result = " + conversion + "; }\n");

    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    bindAttributeLocation(0, "value");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

#ifndef MAGNUM_TARGET_GLES2
IntegerShader::IntegerShader(const std::string& type) {
    #ifndef MAGNUM_TARGET_GLES
    Shader vert(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL300
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL300
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    #else
    Shader vert(Version::GLES300, Shader::Type::Vertex);
    Shader frag(Version::GLES300, Shader::Type::Fragment);
    #endif

    vert.addSource("in mediump " + type + " value;\n"
                   "flat out mediump " + type + " valueInterpolated;\n"
                   "void main() {\n"
                   "    valueInterpolated = value;\n"
                   "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
                   "}\n");
    frag.addSource("flat in mediump " + type + " valueInterpolated;\n"
                   "out mediump " + type + " result;\n"
                   "void main() { result = valueInterpolated; }\n");

    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    bindAttributeLocation(0, "value");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
#endif

#ifndef MAGNUM_TARGET_GLES
DoubleShader::DoubleShader(const std::string& type, const std::string& outputType, const std::string& conversion) {
    constexpr const Version version =
        #ifndef CORRADE_TARGET_APPLE
        Version::GL300;
        #else
        Version::GL400;
        #endif
    Shader vert{version, Shader::Type::Vertex};
    Shader frag(version, Shader::Type::Fragment);

    vert.addSource(
        "#extension GL_ARB_vertex_attrib_64bit: require\n"
        "#extension GL_ARB_gpu_shader_fp64: require\n"
        "in " + type + " value;\n"
        "out " + outputType + " valueInterpolated;\n"
        "void main() {\n"
        "    valueInterpolated = " + conversion + ";\n"
        "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}\n");
    frag.addSource("in " + outputType + " valueInterpolated;\n"
                   "out " + outputType + " result;\n"
                   "void main() { result = valueInterpolated; }\n");

    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    bindAttributeLocation(0, "value");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
#endif

Checker::Checker(AbstractShaderProgram&& shader, RenderbufferFormat format, Mesh& mesh): framebuffer({{}, Vector2i(1)}) {
    renderbuffer.setStorage(format, Vector2i(1));
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer);

    framebuffer.bind();
    mesh.setPrimitive(MeshPrimitive::Points)
        .setCount(2);

    /* Skip first vertex so we test also offsets */
    MeshView view{mesh};
    view.setCount(1)
        .setBaseVertex(mesh.baseVertex())
        .setInstanceCount(mesh.instanceCount())
        #ifndef MAGNUM_TARGET_GLES
        .setBaseInstance(mesh.baseInstance())
        #endif
        ;

    if(view.mesh().isIndexed()) view.setIndexRange(1);

    shader.draw(view);
}

template<class T> T Checker::get(PixelFormat format, PixelType type) {
    return Containers::arrayCast<T>(framebuffer.read({{}, Vector2i{1}}, {format, type}).data())[0];
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferUnsignedInt() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    constexpr UnsignedInt data[] = { 0, 157, 35681 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, UnsignedInt>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::UnsignedInt});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("uint"), RenderbufferFormat::R32UI, mesh)
        .get<UnsignedInt>(PixelFormat::RedInteger, PixelType::UnsignedInt);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 35681);
}

void MeshGLTest::addVertexBufferInt() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    constexpr Int data[] = { 0, 457931, 27530 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, Int>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Int});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("int"), RenderbufferFormat::R32I, mesh)
        .get<Int>(PixelFormat::RedInteger, PixelType::Int);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 27530);
}
#endif

void MeshGLTest::addVertexBufferFloat() {
    const Float data[] = { 0.0f, -0.7f, Math::unpack<Float, UnsignedByte>(
        #ifndef MAGNUM_TARGET_GLES2
        96
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        85
        #endif
    ) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, Float>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Float});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 96);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value, 85);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferDouble() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::ARB::vertex_attrib_64bit::string() + std::string(" is not available."));

    const Double data[] = { 0.0, -0.7, Math::unpack<Double, UnsignedShort>(45828) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 8, Attribute<0, Double>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 8, 8, DynamicAttribute{
            DynamicAttribute::Kind::Long, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Double});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(DoubleShader("double", "float", "float(value)"),
        RenderbufferFormat::R16, mesh).get<UnsignedShort>(PixelFormat::Red, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 45828);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferVectorNui() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    constexpr Vector3ui data[] = { {}, {37448, 547686, 156}, {27592, 157, 25} };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*4, Attribute<0, Vector3ui>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*4, 3*4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::Three,
            DynamicAttribute::DataType::UnsignedInt});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("uvec3"), RenderbufferFormat::RGBA32UI, mesh)
        .get<Vector4ui>(PixelFormat::RGBAInteger, PixelType::UnsignedInt);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value.xyz(), Vector3ui(27592, 157, 25));
}

void MeshGLTest::addVertexBufferVectorNi() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    constexpr Vector2i data[] = { {}, {-37448, 547686}, {27592, -157} };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 2*4, Attribute<0, Vector2i>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 2*4, 2*4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::Two,
            DynamicAttribute::DataType::Int});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("ivec2"), RenderbufferFormat::RG32I, mesh)
        .get<Vector2i>(PixelFormat::RGInteger, PixelType::Int);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, Vector2i(27592, -157));
}
#endif

void MeshGLTest::addVertexBufferVectorN() {
    const Vector3 data[] = { {}, {0.0f, -0.9f, 1.0f},
        #ifndef MAGNUM_TARGET_GLES2
        0x60189c_rgbf
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        0x551199_rgbf
        #endif
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*4, Attribute<0, Vector3>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*4, 3*4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Three,
            DynamicAttribute::DataType::Float});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("vec3", "vec4(valueInterpolated, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value.xyz(), 0x60189c_rgb);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value.xyz(), 0x551199_rgb);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferVectorNd() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::ARB::vertex_attrib_64bit::string() + std::string(" is not available."));

    const Vector4d data[] = {
        {}, {0.0, -0.9, 1.0, 1.25},
        Math::unpack<Vector4d>(Vector4us{315, 65201, 2576, 12})
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4*8, Attribute<0, Vector4d>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4*8, 4*8, DynamicAttribute{
            DynamicAttribute::Kind::Long, 0,
            DynamicAttribute::Components::Four,
            DynamicAttribute::DataType::Double});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(DoubleShader("dvec4", "vec4", "vec4(value)"),
        RenderbufferFormat::RGBA16, mesh).get<Vector4us>(PixelFormat::RGBA, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, (Vector4us{315, 65201, 2576, 12}));
}
#endif

void MeshGLTest::addVertexBufferMatrixNxN() {
    const Matrix3x3 data[] = {
        {},
        Matrix3x3::fromDiagonal({0.0f, -0.9f, 1.0f}),
        Matrix3x3::fromDiagonal(
            #ifndef MAGNUM_TARGET_GLES2
            0x60189c_rgbf
            #else
            /* Using only RGBA4, supply less precision. This has to be one on
               the input because SwiftShader stores RGBA4 as RGBA8 internally,
               thus preserving the full precision of the input. */
            0x551199_rgbf
            #endif
        )
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*3*4, Attribute<0, Matrix3x3>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*3*4, 3*3*4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Three, 3,
            DynamicAttribute::DataType::Float});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("mat3",
        "vec4(valueInterpolated[0][0], valueInterpolated[1][1], valueInterpolated[2][2], 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value.xyz(), 0x60189c_rgb);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value.xyz(), 0x551199_rgb);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferMatrixNxNd() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::ARB::vertex_attrib_64bit::string() + std::string(" is not available."));

    const Matrix3x3d data[] = {
        {},
        Matrix3x3d::fromDiagonal({0.0, -0.9, 1.0}),
        Matrix3x3d::fromDiagonal(Math::unpack<Vector3d>(Vector3us{315, 65201, 2576}))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*3*8, Attribute<0, Matrix3x3d>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*3*8, 3*3*8, DynamicAttribute{
            DynamicAttribute::Kind::Long, 0,
            DynamicAttribute::Components::Three, 3,
            DynamicAttribute::DataType::Double});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(DoubleShader("dmat3", "vec4",
        "vec4(value[0][0], value[1][1], value[2][2], 0.0)"),
        RenderbufferFormat::RGBA16, mesh).get<Vector4us>(PixelFormat::RGBA, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    {
        /* Used to be a problem on Intel Windows drivers 23, not a problem on
           26 anymore */
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & (Context::DetectedDriver::Amd|Context::DetectedDriver::NVidia), "Somehow only first two values are extracted on AMD and NVidia drivers.");
        CORRADE_COMPARE(value.xyz(), (Vector3us{315, 65201, 2576}));
    }

    /* This is wrong, but check if it's still the right wrong. Fails on AMD
       15.201.1151 but seems to be fixed in 15.300.1025.0 */
    if(Context::current().detectedDriver() & (Context::DetectedDriver::Amd|Context::DetectedDriver::NVidia))
        CORRADE_COMPARE(value.xyz(), (Vector3us{315, 65201, 0}));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferMatrixMxN() {
    const Matrix3x4 data[] = {
        {},
        Matrix3x4::fromDiagonal({0.0f, -0.9f, 1.0f}),
        Matrix3x4::fromDiagonal(Math::unpack<Vector3>(Color3ub(96, 24, 156)))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*4*4, Attribute<0, Matrix3x4>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*4*4, 3*4*4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Four, 3,
            DynamicAttribute::DataType::Float});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("mat3x4",
        "vec4(valueInterpolated[0][0], valueInterpolated[1][1], valueInterpolated[2][2], 0.0)"),
        RenderbufferFormat::RGBA8, mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value.xyz(), Color3ub(96, 24, 156));
}
#endif

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferMatrixMxNd() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::ARB::vertex_attrib_64bit::string() + std::string(" is not available."));

    const Matrix3x4d data[] = {
        {},
        Matrix3x4d::fromDiagonal({0.0, -0.9, 1.0}),
        Matrix3x4d::fromDiagonal(Math::unpack<Vector3d>(Vector3us{315, 65201, 2576}))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*4*8, Attribute<0, Matrix3x4d>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*4*8, 3*4*8, DynamicAttribute{
            DynamicAttribute::Kind::Long, 0,
            DynamicAttribute::Components::Four, 3,
            DynamicAttribute::DataType::Double});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(DoubleShader("dmat3x4", "vec4",
        "vec4(value[0][0], value[1][1], value[2][2], 0.0)"),
        RenderbufferFormat::RGBA16, mesh).get<Vector4us>(PixelFormat::RGBA, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    {
        /* Used to be a problem on Intel Windows drivers 23, not a problem on
           26 anymore */
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & (Context::DetectedDriver::Amd|Context::DetectedDriver::NVidia), "Somehow only first two values are extracted on AMD and NVidia drivers.");
        CORRADE_COMPARE(value.xyz(), (Vector3us{315, 65201, 2576}));
    }

    /* This is wrong, but check if it's still the right wrong. Fails on AMD
       15.201.1151 but seems to be fixed in 15.300.1025.0 */
    if(Context::current().detectedDriver() & (Context::DetectedDriver::Amd|Context::DetectedDriver::NVidia))
        CORRADE_COMPARE(value.xyz(), (Vector3us{315, 65201, 0}));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferUnsignedIntWithUnsignedShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    constexpr UnsignedShort data[] = { 0, 49563, 2128, 3821, 16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 2, 2, Attribute<0, UnsignedInt>{Attribute<0, UnsignedInt>::DataType::UnsignedShort});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::UnsignedShort});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("uint"), RenderbufferFormat::R16UI, mesh)
        .get<UnsignedShort>(PixelFormat::RedInteger, PixelType::UnsignedShort);

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::SwiftShader,
        "SwiftShader doesn't like integer buffers with anything else than (Unsigned)Int");
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 16583);
}

void MeshGLTest::addVertexBufferUnsignedIntWithShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    constexpr Short data[] = { 0, 24563, 2128, 3821, 16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 2, 2, Attribute<0, UnsignedInt>{Attribute<0, UnsignedInt>::DataType::Short});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Short});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("uint"), RenderbufferFormat::R16I, mesh)
        .get<Short>(PixelFormat::RedInteger, PixelType::Short);

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::SwiftShader,
        "SwiftShader doesn't like integer buffers with anything else than (Unsigned)Int");
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 16583);
}

void MeshGLTest::addVertexBufferIntWithUnsignedShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    constexpr UnsignedShort data[] = { 0, 49563, 2128, 3821, 16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 2, 2, Attribute<0, Int>{Attribute<0, Int>::DataType::UnsignedShort});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::UnsignedShort});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("int"), RenderbufferFormat::R16UI, mesh)
        .get<UnsignedShort>(PixelFormat::RedInteger, PixelType::UnsignedShort);

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::SwiftShader,
        "SwiftShader doesn't like integer buffers with anything else than (Unsigned)Int");
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 16583);
}

void MeshGLTest::addVertexBufferIntWithShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    constexpr Short data[] = { 0, 24563, 2128, 3821, -16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 2, 2, Attribute<0, Int>{Attribute<0, Int>::DataType::Short});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Short});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("int"), RenderbufferFormat::R16I, mesh)
        .get<Short>(PixelFormat::RedInteger, PixelType::Short);

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::SwiftShader,
        "SwiftShader doesn't like integer buffers with anything else than (Unsigned)Int");
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, -16583);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void MeshGLTest::addVertexBufferFloatWithHalf() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::half_float_vertex>())
        CORRADE_SKIP(Extensions::ARB::half_float_vertex::string() + std::string(" is not supported."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_half_float>())
        CORRADE_SKIP(Extensions::OES::vertex_half_float::string() + std::string(" is not supported."));
    #endif

    const Half data[] = { 0.0_h, -0.7_h, Half(Math::unpack<Float, UnsignedByte>(186)) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 2, Attribute<0, Float>{Attribute<0, Float>::DataType::Half});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 2, 2, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Half});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        RenderbufferFormat::RGBA8, mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 186);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferFloatWithDouble() {
    const Double data[] = { 0.0, -0.7, Math::unpack<Double, UnsignedByte>(186) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1)
        .addVertexBuffer(buffer, 8, Attribute<0, Float>(Attribute<0, Float>::DataType::Double));

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 8, Attribute<0, Float>{Attribute<0, Float>::DataType::Double});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 8, 8, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Double});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        RenderbufferFormat::RGBA8, mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 186);
}

void MeshGLTest::addVertexBufferVector3WithUnsignedInt10f11f11fRev() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_type_10f_11f_11f_rev>())
        CORRADE_SKIP(Extensions::ARB::vertex_type_10f_11f_11f_rev::string() + std::string(" is not available."));
    #endif

    Buffer buffer;
    buffer.setData({nullptr, 12}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, Vector3>{Attribute<0, Vector3>::DataType::UnsignedInt10f11f11fRev});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Three,
            DynamicAttribute::DataType::UnsignedInt10f11f11fRev});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();
    /* Won't test the actual values */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferVector4WithUnsignedInt2101010Rev() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_type_2_10_10_10_rev>())
        CORRADE_SKIP(Extensions::ARB::vertex_type_2_10_10_10_rev::string() + std::string(" is not available."));
    #endif

    Buffer buffer;
    buffer.setData({nullptr, 12}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, Vector4>{Attribute<0, Vector4>::DataType::UnsignedInt2101010Rev});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Four,
            DynamicAttribute::DataType::UnsignedInt2101010Rev});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();
    /* Won't test the actual values */
}

void MeshGLTest::addVertexBufferVector4WithInt2101010Rev() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_type_2_10_10_10_rev>())
        CORRADE_SKIP(Extensions::ARB::vertex_type_2_10_10_10_rev::string() + std::string(" is not available."));
    #endif

    Buffer buffer;
    buffer.setData({nullptr, 12}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, Vector4>{Attribute<0, Vector4>::DataType::Int2101010Rev});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Four,
            DynamicAttribute::DataType::Int2101010Rev});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();
    /* Won't test the actual values */
}
#endif

void MeshGLTest::addVertexBufferLessVectorComponents() {
    const Vector3 data[] = {
        {}, {0.0f, -0.9f, 1.0f},
        #ifndef MAGNUM_TARGET_GLES2
        0x60189c_rgbf
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        0x551199_rgbf
        #endif
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*4, Attribute<0, Vector4>{Attribute<0, Vector4>::Components::Three});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*4, 3*4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Three,
            DynamicAttribute::DataType::Float});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("vec4", "valueInterpolated"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 0x60189cff_rgba);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value, 0x551199ff_rgba);
    #endif
}

void MeshGLTest::addVertexBufferNormalized() {
    constexpr Color4ub data[] = { {}, {0, 128, 64},
        #ifndef MAGNUM_TARGET_GLES2
        0x209ce4_rgb
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        0x1199dd_rgb
        #endif
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        typedef Attribute<0, Vector3> Attribute;
        mesh.addVertexBuffer(buffer, 4, Attribute(Attribute::DataType::UnsignedByte, Attribute::DataOption::Normalized), 1);
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::Three,
            DynamicAttribute::DataType::UnsignedByte});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("vec3", "vec4(valueInterpolated, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value.xyz(), 0x209ce4_rgb);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value.xyz(), 0x1199dd_rgb);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferBGRA() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_bgra>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_bgra::string() + std::string(" is not available."));

    constexpr Color4ub data[] = { {}, {0, 128, 64, 161}, {96, 24, 156, 225} };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        typedef Attribute<0, Vector4> Attribute;
        mesh.addVertexBuffer(buffer, 4, Attribute(Attribute::Components::BGRA, Attribute::DataType::UnsignedByte, Attribute::DataOption::Normalized));
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::BGRA,
            DynamicAttribute::DataType::UnsignedByte});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("vec4", "valueInterpolated"),
        RenderbufferFormat::RGBA8, mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, Color4ub(156, 24, 96, 225));
}
#endif

struct MultipleShader: AbstractShaderProgram {
    typedef Attribute<0, Vector3> Position;
    typedef Attribute<1, Vector3> Normal;
    typedef Attribute<2, Vector2> TextureCoordinates;

    explicit MultipleShader();
};

#ifndef DOXYGEN_GENERATING_OUTPUT
MultipleShader::MultipleShader() {
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
        "#define mediump\n"
        "#endif\n"
        "#if defined(GL_ES) || __VERSION__ == 120\n"
        "#define in attribute\n"
        "#define out varying\n"
        "#endif\n"
        "in mediump vec4 position;\n"
        "in mediump vec3 normal;\n"
        "in mediump vec2 textureCoordinates;\n"
        "out mediump vec4 valueInterpolated;\n"
        "void main() {\n"
        "    valueInterpolated = position + vec4(normal, 0.0) + vec4(textureCoordinates, 0.0, 0.0);\n"
        "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}\n");
    frag.addSource(
        "#if !defined(GL_ES) && __VERSION__ == 120\n"
        "#define mediump\n"
        "#endif\n"
        "#if defined(GL_ES) || __VERSION__ == 120\n"
        "#define in varying\n"
        "#define result gl_FragColor\n"
        "#endif\n"
        "in mediump vec4 valueInterpolated;\n"
        "#if !defined(GL_ES) && __VERSION__ >= 130\n"
        "out mediump vec4 result;\n"
        "#endif\n"
        "void main() { result = valueInterpolated; }\n");

    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    bindAttributeLocation(Position::Location, "position");
    bindAttributeLocation(Normal::Location, "normal");
    bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
#endif

void MeshGLTest::addVertexBufferMultiple() {
    const Float data[] = {
        0.0f, /* Offset */

        /* First attribute */
        0.3f, 0.1f, 0.5f,
            0.4f, 0.0f, -0.9f,
                1.0f, -0.5f,

        /* Second attribute */
        Math::unpack<Float, UnsignedByte>(64),
            Math::unpack<Float, UnsignedByte>(17),
                Math::unpack<Float, UnsignedByte>(56),
        #ifndef MAGNUM_TARGET_GLES2
        Math::unpack<Float, UnsignedByte>(15),
            Math::unpack<Float, UnsignedByte>(164),
                Math::unpack<Float, UnsignedByte>(17),
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        Math::unpack<Float, UnsignedByte>(9),
            Math::unpack<Float, UnsignedByte>(159),
                Math::unpack<Float, UnsignedByte>(12),
        #endif
        Math::unpack<Float, UnsignedByte>(97),
            Math::unpack<Float, UnsignedByte>(28)
    };

    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1)
        .addVertexBuffer(buffer, 1*4, MultipleShader::Position(),
        MultipleShader::Normal(), MultipleShader::TextureCoordinates());

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader(),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, Color4ub(64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255));
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value, 0xaacc44ff_rgba);
    #endif
}

void MeshGLTest::addVertexBufferMultipleGaps() {
     const Float data[] = {
        0.0f, 0.0f, 0.0f, 0.0f, /* Offset */

        /* First attribute */
        0.3f, 0.1f, 0.5f, 0.0f,
            0.4f, 0.0f, -0.9f, 0.0f,
                1.0f, -0.5f, 0.0f, 0.0f,

        /* Second attribute */
        Math::unpack<Float, UnsignedByte>(64),
            Math::unpack<Float, UnsignedByte>(17),
                Math::unpack<Float, UnsignedByte>(56), 0.0f,
        #ifndef MAGNUM_TARGET_GLES2
        Math::unpack<Float, UnsignedByte>(15),
            Math::unpack<Float, UnsignedByte>(164),
                Math::unpack<Float, UnsignedByte>(17), 0.0f,
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        Math::unpack<Float, UnsignedByte>(9),
            Math::unpack<Float, UnsignedByte>(159),
                Math::unpack<Float, UnsignedByte>(12), 0.0f,
        #endif
        Math::unpack<Float, UnsignedByte>(97),
            Math::unpack<Float, UnsignedByte>(28), 0.0f, 0.0f
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1)
        .addVertexBuffer(buffer, 4*4,
        MultipleShader::Position(), 1*4,
        MultipleShader::Normal(), 1*4,
        MultipleShader::TextureCoordinates(), 2*4);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader(),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, Color4ub(64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255));
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value, 0xaacc44ff_rgba);
    #endif
}

void MeshGLTest::addVertexBufferMovedOutInstance() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Buffer buffer{NoCreate};
    Mesh mesh;

    std::ostringstream out;
    Error redirectError{&out};

    mesh.addVertexBuffer(buffer, 0, Attribute<0, Float>{});

    CORRADE_COMPARE(out.str(), "GL::Mesh::addVertexBuffer(): empty or moved-out Buffer instance was passed\n");
}

void MeshGLTest::addVertexBufferTransferOwnwership() {
    const Float data = 1.0f;
    Buffer buffer;
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBuffer(buffer, 0, Attribute<0, Float>{});
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBuffer(std::move(buffer), 0, Attribute<0, Float>{});
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

void MeshGLTest::addVertexBufferInstancedTransferOwnwership() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("Required extension is not available.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() + std::string(" is not available."));
    #endif
    #endif

    const Float data = 1.0f;
    Buffer buffer;
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBufferInstanced(buffer, 1, 0, Attribute<0, Float>{});
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBufferInstanced(std::move(buffer), 1, 0, Attribute<0, Float>{});
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

void MeshGLTest::addVertexBufferDynamicTransferOwnwership() {
    const Float data = 1.0f;
    Buffer buffer;
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBuffer(buffer, 0, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Float});
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBuffer(std::move(buffer), 0, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Float});
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

void MeshGLTest::addVertexBufferInstancedDynamicTransferOwnwership() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("Required extension is not available.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() + std::string(" is not available."));
    #endif
    #endif

    const Float data = 1.0f;
    Buffer buffer;
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBufferInstanced(buffer, 1, 0, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Float});
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBufferInstanced(std::move(buffer), 1, 0, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Float});
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

const Float indexedVertexData[] = {
    0.0f, /* Offset */

    /* First vertex */
    Math::unpack<Float, UnsignedByte>(64),
        Math::unpack<Float, UnsignedByte>(17),
            Math::unpack<Float, UnsignedByte>(56),
    #ifndef MAGNUM_TARGET_GLES2
    Math::unpack<Float, UnsignedByte>(15),
        Math::unpack<Float, UnsignedByte>(164),
            Math::unpack<Float, UnsignedByte>(17),
    #else
    /* Using only RGBA4, supply less precision. This has to be one on the input
       because SwiftShader stores RGBA4 as RGBA8 internally, thus preserving
       the full precision of the input. */
    Math::unpack<Float, UnsignedByte>(9),
        Math::unpack<Float, UnsignedByte>(159),
            Math::unpack<Float, UnsignedByte>(12),
    #endif
    Math::unpack<Float, UnsignedByte>(97),
        Math::unpack<Float, UnsignedByte>(28),

    /* Second vertex */
    0.3f, 0.1f, 0.5f,
        0.4f, 0.0f, -0.9f,
            1.0f, -0.5f
};

#ifndef MAGNUM_TARGET_GLES
const Float indexedVertexDataBaseVertex[] = {
    0.0f, 0.0f, /* Offset */

    /* First vertex */
    0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,

    /* Second vertex */
    0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,

    /* Third vertex */
    Math::unpack<Float, UnsignedByte>(64),
        Math::unpack<Float, UnsignedByte>(17),
            Math::unpack<Float, UnsignedByte>(56),
    Math::unpack<Float, UnsignedByte>(15),
        Math::unpack<Float, UnsignedByte>(164),
            Math::unpack<Float, UnsignedByte>(17),
    Math::unpack<Float, UnsignedByte>(97),
        Math::unpack<Float, UnsignedByte>(28),

    /* Fourth vertex */
    0.3f, 0.1f, 0.5f,
        0.4f, 0.0f, -0.9f,
            1.0f, -0.5f
};
#endif

#ifndef MAGNUM_TARGET_GLES2
constexpr Color4ub indexedResult{64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255};
#else /* RGBA4, so less precision */
constexpr Color4ub indexedResult{0xaa, 0xcc, 0x44, 0xff};
#endif

template<class T> void MeshGLTest::setIndexBuffer() {
    setTestCaseTemplateName(std::is_same<T, MeshIndexType>::value ?
        "GL::MeshIndexType" : "Magnum::MeshIndexType");

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedByte indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 1, T::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(mesh.indexType(), MeshIndexType::UnsignedByte);
    CORRADE_COMPARE(mesh.indexTypeSize(), 1);

    const auto value = Checker(MultipleShader{},
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

template<class T> void MeshGLTest::setIndexBufferRange() {
    setTestCaseTemplateName(std::is_same<T, MeshIndexType>::value ?
        "GL::MeshIndexType" : "Magnum::MeshIndexType");

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, T::UnsignedShort, 0, 1);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(mesh.indexType(), GL::MeshIndexType::UnsignedShort);
    CORRADE_COMPARE(mesh.indexTypeSize(), 2);

    const auto value = Checker(MultipleShader{},
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

void MeshGLTest::setIndexBufferUnsignedInt() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::element_index_uint>())
        CORRADE_SKIP(Extensions::OES::element_index_uint::string() + std::string(" is not available."));
    #endif

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedInt indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 4, MeshIndexType::UnsignedInt);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(mesh.indexType(), GL::MeshIndexType::UnsignedInt);
    CORRADE_COMPARE(mesh.indexTypeSize(), 4);

    const auto value = Checker(MultipleShader{},
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

void MeshGLTest::setIndexBufferMovedOutInstance() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Buffer buffer{NoCreate};
    Mesh mesh;

    std::ostringstream out;
    Error redirectError{&out};

    mesh.setIndexBuffer(buffer, 0, MeshIndexType::UnsignedByte);

    CORRADE_COMPARE(out.str(), "GL::Mesh::setIndexBuffer(): empty or moved-out Buffer instance was passed\n");
}

template<class T> void MeshGLTest::setIndexBufferTransferOwnership() {
    setTestCaseTemplateName(std::is_same<T, MeshIndexType>::value ?
        "GL::MeshIndexType" : "Magnum::MeshIndexType");

    const UnsignedShort data = 0;
    Buffer buffer{Buffer::TargetHint::ElementArray};
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {

        Mesh mesh;
        mesh.setIndexBuffer(buffer, 0, T::UnsignedShort);
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {

        Mesh mesh;
        mesh.setIndexBuffer(std::move(buffer), 0, T::UnsignedShort);
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

template<class T> void MeshGLTest::setIndexBufferRangeTransferOwnership() {
    setTestCaseTemplateName(std::is_same<T, MeshIndexType>::value ?
        "GL::MeshIndexType" : "Magnum::MeshIndexType");

    const UnsignedShort data = 0;
    Buffer buffer{Buffer::TargetHint::ElementArray};
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.setIndexBuffer(buffer, 0, T::UnsignedShort, 0, 1);
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.setIndexBuffer(std::move(buffer), 0, T::UnsignedShort, 0, 1);
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

void MeshGLTest::unbindVAOWhenSettingIndexBufferData() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() + std::string(" is not available."));
    if(Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() + std::string(" is active which circumvents the issue tested here."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() + std::string(" is not available."));
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = { -0.7f, Math::unpack<Float, UnsignedByte>(
        #ifndef MAGNUM_TARGET_GLES2
        92
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        85
        #endif
    ), Math::unpack<Float, UnsignedByte>(32) };
    Buffer buffer{Buffer::TargetHint::Array};
    buffer.setData(data, BufferUsage::StaticDraw);

    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(std::initializer_list<UnsignedByte>{5, 0}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute{})
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedByte);

    /* This buffer should have no effect on the mesh above */
    Buffer otherIndices{Buffer::TargetHint::ElementArray};
    otherIndices.setData(std::initializer_list<UnsignedByte>{100, 1}, BufferUsage::StaticDraw);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 92);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value, 85);
    #endif
}

void MeshGLTest::unbindIndexBufferWhenBindingVao() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() + std::string(" is not available."));
    if(Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() + std::string(" is active which circumvents the issue tested here."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() + std::string(" is not available."));
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = { -0.7f, Math::unpack<Float, UnsignedByte>(
        #ifndef MAGNUM_TARGET_GLES2
        92
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        85
        #endif
    ), Math::unpack<Float, UnsignedByte>(32) };
    Buffer vertices{Buffer::TargetHint::Array};
    vertices.setData(data, BufferUsage::StaticDraw);

    Buffer indices{Buffer::TargetHint::ElementArray};
    /* Just reserve the memory first */
    indices.setData({nullptr, 2}, BufferUsage::StaticDraw);

    /* Create an indexed mesh first */
    Mesh indexed;
    indexed.addVertexBuffer(vertices, 0, Attribute{})
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedByte);

    /* Now bind an nonindexed mesh */
    Mesh nonindexed;
    nonindexed.addVertexBuffer(vertices, 0, Attribute{});

    /* Fill index buffer for the indexed mesh */
    indices.setData(std::initializer_list<UnsignedByte>{5, 1}, BufferUsage::StaticDraw);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Draw the indexed mesh. The index buffer should be correctly updated,
       picking the second vertex with value of 92. */
    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        indexed).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 92);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value, 85);
    #endif
}

void MeshGLTest::resetIndexBufferBindingWhenBindingVao() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() + std::string(" is not available."));
    if(Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() + std::string(" is active which circumvents the issue tested here."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() + std::string(" is not available."));
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = { -0.7f, Math::unpack<Float, UnsignedByte>(
        #ifndef MAGNUM_TARGET_GLES2
        92
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        85
        #endif
    ), Math::unpack<Float, UnsignedByte>(32) };
    Buffer vertices{Buffer::TargetHint::Array};
    vertices.setData(data);

    /* Create an indexed mesh */
    Mesh indexed;
    indexed.addVertexBuffer(vertices, 0, Attribute{});

    /* Create an index buffer and fill it (the VAO is bound now, so it'll get
       unbound to avoid messing with its state). */
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(std::initializer_list<UnsignedByte>{5, 1});

    /* Add the index buffer. The VAO is unbound, so it gets bound. That resets
       the element array buffer binding and then the buffer gets bound to the
       VAO. */
    indexed.setIndexBuffer(indices, 0, MeshIndexType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Draw the indexed mesh. The index buffer should be correctly bound,
       picking the second vertex with value of 92. */
    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        indexed).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 92);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value, 85);
    #endif
}

void MeshGLTest::unbindVAOBeforeEnteringExternalSection() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() + std::string(" is not available."));
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = { -0.7f, Math::unpack<Float, UnsignedByte>(
        #ifndef MAGNUM_TARGET_GLES2
        92
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        85
        #endif
    ), Math::unpack<Float, UnsignedByte>(32) };
    Buffer buffer{Buffer::TargetHint::Array};
    buffer.setData(data, BufferUsage::StaticDraw);

    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(std::initializer_list<UnsignedByte>{5, 0}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute{})
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedByte);

    {
        /* Comment this out to watch the world burn */
        Context::current().resetState(Context::State::MeshVao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        /* Be nice to the other tests */
        Context::current().resetState(Context::State::ExitExternal);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 92);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value, 85);
    #endif
}

void MeshGLTest::bindScratchVaoWhenEnteringExternalSection() {
    typedef Attribute<0, Float> Attribute;

    const Float data[] = { -0.7f, Math::unpack<Float, UnsignedByte>(
        #ifndef MAGNUM_TARGET_GLES2
        92
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        85
        #endif
    ), Math::unpack<Float, UnsignedByte>(32) };
    Buffer buffer{Buffer::TargetHint::Array};
    buffer.setData(data, BufferUsage::StaticDraw);

    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(std::initializer_list<UnsignedByte>{5, 0}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute{})
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedByte);

    {
        /* Bind a scratch framebuffer so glDrawArrays() doesn't complain about
           an incomplete framebuffer in case we're on a framebuffer-less
           context */
        Renderbuffer renderbuffer;
        renderbuffer.setStorage(
            #ifndef MAGNUM_TARGET_GLES2
            RenderbufferFormat::RGBA8,
            #else
            RenderbufferFormat::RGBA4,
            #endif
            Vector2i{1});
        Framebuffer framebuffer{{{}, Vector2i{1}}};
        framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, renderbuffer)
            .bind();

        /* Should bind a scratch VAO only on desktop with core profile and be
           a no-op everywhere else */
        Context::current().resetState(Context::State::EnterExternal
            |Context::State::BindScratchVao /* Comment this out to watch the world burn */
            );

        /* Should throw no GL error if scratch VAO is bound */
        glDrawArrays(GL_POINTS, 0, 0);

        /* Be nice to the other tests */
        Context::current().resetState(Context::State::ExitExternal);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 92);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value, 85);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::setBaseVertex() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::ARB::draw_elements_base_vertex::string() + std::string(" is not available."));

    Buffer vertices;
    vertices.setData(indexedVertexDataBaseVertex, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(2)
        .addVertexBuffer(vertices, 2*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader{}, RenderbufferFormat::RGBA8,
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}
#endif

void MeshGLTest::setInstanceCount() {
    /* Verbatim copy of addVertexBufferFloat() with added extension check and
       setInstanceCount() call. It would just render three times the same
       value. I'm too lazy to invent proper test case, so I'll just check that
       it didn't generate any error and rendered something */

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::draw_instanced>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::draw_instanced>())
        CORRADE_SKIP("Required extension is not available.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() + std::string(" is not available."));
    #endif
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = { 0.0f, -0.7f, Math::unpack<Float, UnsignedByte>(96) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1)
        .setInstanceCount(3)
        .addVertexBuffer(buffer, 4, Attribute());

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 96);
}

void MeshGLTest::setInstanceCountIndexed() {
    /* Verbatim copy of setIndexBuffer() with added extension check and
       setInstanceCount() call. It would just render three times the same
       value. I'm too lazy to invent proper test case, so I'll just check that
       it didn't generate any error and rendered something */

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::draw_instanced>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::draw_instanced>())
        CORRADE_SKIP("Required extension is not available.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() + std::string(" is not available."));
    #endif
    #endif

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setInstanceCount(3)
        .addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader{},
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::setInstanceCountBaseInstance() {
    /* Verbatim copy of setInstanceCount() with additional extension check and
       setBaseInstance() call. It would just render three times the same
       value. I'm too lazy to invent proper test case, so I'll just check that
       it didn't generate any error and rendered something */

    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::base_instance>())
        CORRADE_SKIP(Extensions::ARB::base_instance::string() + std::string(" is not available."));

    typedef Attribute<0, Float> Attribute;

    const Float data[] = { 0.0f, -0.7f, Math::unpack<Float, UnsignedByte>(96) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1)
        .setInstanceCount(3)
        .setBaseInstance(72)
        .addVertexBuffer(buffer, 4, Attribute());

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        RenderbufferFormat::RGBA8,
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 96);
}

void MeshGLTest::setInstanceCountBaseInstanceIndexed() {
    /* Verbatim copy of setInstanceCountIndexed() with additional extension
       check and setBaseInstance() call. It would just render three times the
       same value. I'm too lazy to invent proper test case, so I'll just check
       that it didn't generate any error and rendered something */

    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::base_instance>())
        CORRADE_SKIP(Extensions::ARB::base_instance::string() + std::string(" is not available."));

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setInstanceCount(3)
        .setBaseInstance(72)
        .addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader{}, RenderbufferFormat::RGBA8,
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

void MeshGLTest::setInstanceCountBaseVertex() {
    /* Verbatim copy of setBaseVertex() with additional extension check and
       setInstanceCount() call. It would just render three times the same
       value. I'm too lazy to invent proper test case, so I'll just check
       that it didn't generate any error and rendered something */

    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::ARB::draw_elements_base_vertex::string() + std::string(" is not available."));

    Buffer vertices;
    vertices.setData(indexedVertexDataBaseVertex, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(2)
        .setInstanceCount(3)
        .addVertexBuffer(vertices, 2*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader{}, RenderbufferFormat::RGBA8,
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

void MeshGLTest::setInstanceCountBaseVertexBaseInstance() {
    /* Verbatim copy of setInstanceCountBaseVertex() with added extension check
       and setBaseInstance() call. It would just render three times the same
       value. I'm too lazy to invent proper test case, so I'll just check
       that it didn't generate any error and rendered something */

    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::ARB::draw_elements_base_vertex::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::base_instance>())
        CORRADE_SKIP(Extensions::ARB::base_instance::string() + std::string(" is not available."));

    Buffer vertices;
    vertices.setData(indexedVertexDataBaseVertex, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(2)
        .setInstanceCount(3)
        .setBaseInstance(72)
        .addVertexBuffer(vertices, 2*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader{}, RenderbufferFormat::RGBA8,
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}
#endif

void MeshGLTest::addVertexBufferInstancedFloat() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(Extensions::ARB::instanced_arrays::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("Required extension is not available.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() + std::string(" is not available."));
    #endif
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = {
        0.0f,   /* Offset */
                /* Base vertex is ignored for instanced arrays */
        -0.7f,  /* First instance */
        0.3f,   /* Second instance */
        Math::unpack<Float, UnsignedByte>(96) /* Third instance */
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setInstanceCount(3)
        .addVertexBufferInstanced(buffer, 1, 4, Attribute{});

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 96);
}

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferInstancedInteger() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(Extensions::ARB::instanced_arrays::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    typedef Attribute<0, UnsignedInt> Attribute;

    constexpr UnsignedInt data[] = {
        0,      /* Offset */
                /* Base vertex is ignored for instanced arrays */
        157,    /* First instance */
        25,     /* Second instance */
        35681   /* Third instance */
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setInstanceCount(3)
        .addVertexBufferInstanced(buffer, 1, 4, Attribute{});

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("uint"), RenderbufferFormat::R32UI, mesh)
        .get<UnsignedInt>(PixelFormat::RedInteger, PixelType::UnsignedInt);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 35681);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferInstancedDouble() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(Extensions::ARB::instanced_arrays::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::ARB::vertex_attrib_64bit::string() + std::string(" is not available."));

    typedef Attribute<0, Double> Attribute;

    const Double data[] = {
        0.0,    /* Offset */
                /* Base vertex is ignored for instanced arrays */
        -0.7,   /* First instance */
        0.3,    /* Second instance */
        Math::unpack<Double, UnsignedShort>(45828) /* Third instance */
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setInstanceCount(3)
        .addVertexBufferInstanced(buffer, 1, 8, Attribute{});

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(DoubleShader("double", "float", "float(value)"),
        RenderbufferFormat::R16, mesh).get<UnsignedShort>(PixelFormat::Red, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 45828);
}
#endif

void MeshGLTest::resetDivisorAfterInstancedDraw() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() + std::string(" is not available."));
    if(!Context::current().isExtensionSupported<Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(Extensions::ARB::instanced_arrays::string() + std::string(" is not available."));
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("Required extension is not available.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() + std::string(" is not available."));
    #endif
    #endif

    /* This doesn't affect VAOs, because they encapsulate the state */
    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() + std::string(" is enabled, can't test."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() + std::string(" is enabled, can't test."));
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[]{
        0,
        Math::unpack<Float, UnsignedByte>(96),
        Math::unpack<Float, UnsignedByte>(48),
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Renderbuffer renderbuffer;
    renderbuffer.setStorage(
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        Vector2i(1));
    Framebuffer framebuffer{{{}, Vector2i(1)}};
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer)
                .bind();

    FloatShader shader{"float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"};

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Draw instanced first. Two single-vertex instances of an attribute with
       divisor 1, first draws 0, second draws 96 */
    {
        Mesh mesh;
        mesh.setInstanceCount(2)
            .addVertexBufferInstanced(buffer, 1, 0, Attribute{})
            .setPrimitive(MeshPrimitive::Points)
            .setCount(1);
        shader.draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(Containers::arrayCast<UnsignedByte>(framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 96);
    }

    /* Draw normal after. One two-vertex instance of an attribute with divisor
       0, first draws 96, second 48. In case divisor is not properly reset,
       I'll get 96 on both. */
    {
        Mesh mesh;
        mesh.setInstanceCount(1)
            .addVertexBuffer(buffer, 4, Attribute{})
            .setPrimitive(MeshPrimitive::Points)
            .setCount(2);
        shader.draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(Containers::arrayCast<UnsignedByte>(framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 48);
    }
}

struct MultiChecker {
    MultiChecker(AbstractShaderProgram&& shader, Mesh& mesh);

    template<class T> T get(PixelFormat format, PixelType type);

    Renderbuffer renderbuffer;
    Framebuffer framebuffer;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
MultiChecker::MultiChecker(AbstractShaderProgram&& shader, Mesh& mesh): framebuffer({{}, Vector2i(1)}) {
    renderbuffer.setStorage(
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        Vector2i(1));
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer);

    framebuffer.bind();
    mesh.setPrimitive(MeshPrimitive::Points)
        .setCount(2);

    /* Set zero count so we test mesh skipping */
    MeshView a{mesh};
    a.setCount(0);

    /* Skip first vertex so we test also offsets */
    MeshView b{mesh};
    b.setCount(1)
     .setBaseVertex(mesh.baseVertex());

    MeshView c{mesh};
    c.setCount(1);
    if(mesh.isIndexed()) {
        c.setBaseVertex(mesh.baseVertex())
         .setIndexRange(1);
    } else c.setBaseVertex(1);

    shader.draw({a, b, c});
}

template<class T> T MultiChecker::get(PixelFormat format, PixelType type) {
    return Containers::arrayCast<T>(framebuffer.read({{}, Vector2i{1}}, {format, type}).data())[0];
}
#endif

void MeshGLTest::multiDraw() {
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>())
        Debug() << Extensions::EXT::multi_draw_arrays::string() << "not supported, using fallback implementation";
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = { 0.0f, -0.7f, Math::unpack<Float, UnsignedByte>(
        #ifndef MAGNUM_TARGET_GLES2
        96
        #else
        /* Using only RGBA4, supply less precision. This has to be one on the
           input because SwiftShader stores RGBA4 as RGBA8 internally, thus
           preserving the full precision of the input. */
        85
        #endif
    ) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute());

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = MultiChecker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 96);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE(value, 85);
    #endif
}

void MeshGLTest::multiDrawIndexed() {
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>())
        Debug() << Extensions::EXT::multi_draw_arrays::string() << "not supported, using fallback implementation";
    #endif

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = MultiChecker(MultipleShader{}, mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::multiDrawBaseVertex() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::ARB::draw_elements_base_vertex::string() + std::string(" is not available."));

    Buffer vertices;
    vertices.setData(indexedVertexDataBaseVertex, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(2)
        .addVertexBuffer(vertices, 2*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = MultiChecker(MultipleShader{}, mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::MeshGLTest)
