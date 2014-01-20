/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "Magnum/Buffer.h"
#include "Magnum/ColorFormat.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Image.h"
#include "Magnum/Mesh.h"
#include "Magnum/MeshView.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/RenderbufferFormat.h"
#include "Magnum/Shader.h"
#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Test/AbstractOpenGLTester.h"

namespace Magnum { namespace Test {

/* Tests also MeshView class. */

class MeshGLTest: public AbstractOpenGLTester {
    public:
        explicit MeshGLTest();

        void construct();
        void constructCopy();
        void constructMove();

        void label();

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
        /* Other Float types omitted (covered by addVertexBufferNormalized()) */
        void addVertexBufferFloatWithHalfFloat();
        #ifndef MAGNUM_TARGET_GLES
        void addVertexBufferFloatWithDouble();
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

        void setIndexBuffer();
        void setIndexBufferRange();
        void setIndexBufferUnsignedInt();
};

MeshGLTest::MeshGLTest() {
    addTests({&MeshGLTest::construct,
              &MeshGLTest::constructCopy,
              &MeshGLTest::constructMove,

              &MeshGLTest::label,

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
              &MeshGLTest::addVertexBufferFloatWithHalfFloat,
              #ifndef MAGNUM_TARGET_GLES
              &MeshGLTest::addVertexBufferFloatWithDouble,
              #endif
              #ifndef MAGNUM_TARGET_GLES2
              &MeshGLTest::addVertexBufferVector4WithUnsignedInt2101010Rev,
              &MeshGLTest::addVertexBufferVector4WithInt2101010Rev,
              #endif

              &MeshGLTest::addVertexBufferLessVectorComponents,
              &MeshGLTest::addVertexBufferNormalized,
              #ifndef MAGNUM_TARGET_GLES
              &MeshGLTest::addVertexBufferBGRA,
              #endif

              &MeshGLTest::addVertexBufferMultiple,
              &MeshGLTest::addVertexBufferMultipleGaps,

              &MeshGLTest::setIndexBuffer,
              &MeshGLTest::setIndexBufferRange,
              &MeshGLTest::setIndexBufferUnsignedInt});
}

void MeshGLTest::construct() {
    {
        const Mesh mesh;

        MAGNUM_VERIFY_NO_ERROR();

        #ifndef MAGNUM_TARGET_GLES
        if(Context::current()->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>())
        #elif defined(MAGNUM_TARGET_GLES2)
        if(Context::current()->isExtensionSupported<Extensions::GL::OES::vertex_array_object>())
        #endif
        {
            CORRADE_VERIFY(mesh.id() > 0);
        }
    }

    MAGNUM_VERIFY_NO_ERROR();
}

void MeshGLTest::constructCopy() {
    #ifndef CORRADE_GCC44_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_constructible<Mesh, const Mesh&>::value));
    /* GCC 4.6 doesn't have std::is_assignable */
    #ifndef CORRADE_GCC46_COMPATIBILITY
    CORRADE_VERIFY(!(std::is_assignable<Mesh, const Mesh&>{}));
    #endif
    #else
    CORRADE_SKIP("Type traits needed to test this are not available on GCC 4.4.");
    #endif
}

void MeshGLTest::constructMove() {
    Mesh a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current()->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::vertex_array_object>())
    #endif
    {
        CORRADE_VERIFY(id > 0);
    }

    Mesh b(std::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    Mesh c;
    const Int cId = c.id();
    c = std::move(b);

    MAGNUM_VERIFY_NO_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current()->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current()->isExtensionSupported<Extensions::GL::OES::vertex_array_object>())
    #endif
    {
        CORRADE_VERIFY(cId > 0);
    }

    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);
}

void MeshGLTest::label() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>() &&
       !Context::current()->isExtensionSupported<Extensions::GL::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Mesh mesh;
    CORRADE_COMPARE(mesh.label(), "");

    mesh.setLabel("MyMesh");
    CORRADE_COMPARE(mesh.label(), "MyMesh");

    MAGNUM_VERIFY_NO_ERROR();
}

namespace {
    struct FloatShader: AbstractShaderProgram {
        explicit FloatShader(const std::string& type, const std::string& conversion);
    };

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

        template<class T> T get(ColorFormat format, ColorType type);

        Renderbuffer renderbuffer;
        Framebuffer framebuffer;
    };
}

#ifndef DOXYGEN_GENERATING_OUTPUT
FloatShader::FloatShader(const std::string& type, const std::string& conversion) {
    /* We need special version for ES3, because GLSL in ES2 doesn't support
       rectangle matrices */
    #ifndef MAGNUM_TARGET_GLES
    Shader vert(Version::GL210, Shader::Type::Vertex);
    #elif defined(MAGNUM_TARGET_GLES2)
    Shader vert(Version::GLES200, Shader::Type::Vertex);
    #else
    Shader vert(Version::GLES300, Shader::Type::Vertex);
    #endif

    vert.addSource(
        #ifndef MAGNUM_TARGET_GLES3
        "attribute mediump " + type + " value;\n"
        "varying mediump " + type + " valueInterpolated;\n"
        #else
        "in mediump " + type + " value;\n"
        "out mediump " + type + " valueInterpolated;\n"
        #endif
        "void main() {\n"
        "    valueInterpolated = value;\n"
        "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}\n");
    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
    attachShader(vert);

    #ifndef MAGNUM_TARGET_GLES
    Shader frag(Version::GL210, Shader::Type::Fragment);
    #elif defined(MAGNUM_TARGET_GLES2)
    Shader frag(Version::GLES200, Shader::Type::Fragment);
    #else
    Shader frag(Version::GLES300, Shader::Type::Fragment);
    #endif

    #ifndef MAGNUM_TARGET_GLES3
    frag.addSource("varying mediump " + type + " valueInterpolated;\n"
                   "void main() { gl_FragColor = " + conversion + "; }\n");
    #else
    frag.addSource("in mediump " + type + " valueInterpolated;\n"
                   "out mediump vec4 output;\n"
                   "void main() { output = " + conversion + "; }\n");
    #endif
    CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
    attachShader(frag);

    bindAttributeLocation(0, "value");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

#ifndef MAGNUM_TARGET_GLES2
IntegerShader::IntegerShader(const std::string& type) {
    #ifndef MAGNUM_TARGET_GLES
    Shader vert(Version::GL300, Shader::Type::Vertex);
    #else
    Shader vert(Version::GLES300, Shader::Type::Vertex);
    #endif
    vert.addSource("in mediump " + type + " value;\n"
                   "flat out mediump " + type + " valueInterpolated;\n"
                   "void main() {\n"
                   "    valueInterpolated = value;\n"
                   "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
                   "}\n");
    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
    attachShader(vert);

    #ifndef MAGNUM_TARGET_GLES
    Shader frag(Version::GL300, Shader::Type::Fragment);
    #else
    Shader frag(Version::GLES300, Shader::Type::Fragment);
    #endif
    frag.addSource("flat in mediump " + type + " valueInterpolated;\n"
                   "out mediump " + type + " result;\n"
                   "void main() { result = valueInterpolated; }\n");
    CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
    attachShader(frag);

    bindAttributeLocation(0, "value");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
#endif

#ifndef MAGNUM_TARGET_GLES
DoubleShader::DoubleShader(const std::string& type, const std::string& outputType, const std::string& conversion) {
    Shader vert(Version::GL410, Shader::Type::Vertex);
    vert.addSource("in " + type + " value;\n"
                   "out " + outputType + " valueInterpolated;\n"
                   "void main() {\n"
                   "    valueInterpolated = " + conversion + ";\n"
                   "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
                   "}\n");
    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
    attachShader(vert);

    Shader frag(Version::GL410, Shader::Type::Fragment);
    frag.addSource("in " + outputType + " valueInterpolated;\n"
                   "out " + outputType + " result;\n"
                   "void main() { result = valueInterpolated; }\n");
    CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
    attachShader(frag);

    bindAttributeLocation(0, "value");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
#endif

Checker::Checker(AbstractShaderProgram&& shader, RenderbufferFormat format, Mesh& mesh): framebuffer({{}, Vector2i(1)}) {
    renderbuffer.setStorage(format, Vector2i(1));
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer);

    framebuffer.bind(FramebufferTarget::ReadDraw);
    shader.use();
    mesh.setVertexCount(2)
        .setPrimitive(MeshPrimitive::Points);

    /* Skip first vertex so we test also offsets */
    MeshView(mesh).setVertexRange(1, 1).draw();
}

template<class T> T Checker::get(ColorFormat format, ColorType type) {
    Image2D image(format, type);
    framebuffer.read(Vector2i{}, Vector2i(1), image);
    return image.data<T>()[0];
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferUnsignedInt() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::GL::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, UnsignedInt> Attribute;

    constexpr UnsignedInt data[] = { 0, 157, 35681 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(IntegerShader("uint"), RenderbufferFormat::R32UI, mesh)
        .get<UnsignedInt>(ColorFormat::RedInteger, ColorType::UnsignedInt);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, 35681);
}

void MeshGLTest::addVertexBufferInt() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::GL::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, Int> Attribute;

    constexpr Int data[] = { 0, 457931, 27530 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(IntegerShader("int"), RenderbufferFormat::R32I, mesh)
        .get<Int>(ColorFormat::RedInteger, ColorType::Int);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, 27530);
}
#endif

void MeshGLTest::addVertexBufferFloat() {
    typedef AbstractShaderProgram::Attribute<0, Float> Attribute;

    const Float data[] = { 0.0f, -0.7f, Math::normalize<Float, UnsignedByte>(96) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(ColorFormat::RGBA, ColorType::UnsignedByte);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, 96);
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferDouble() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::GL::ARB::vertex_attrib_64bit::string() + std::string(" is not available."));

    typedef AbstractShaderProgram::Attribute<0, Double> Attribute;

    const Double data[] = { 0.0, -0.7, Math::normalize<Double, UnsignedShort>(45828) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 8, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(DoubleShader("double", "float", "vec4(value, 0.0, 0.0, 0.0)"),
        RenderbufferFormat::R16, mesh).get<UnsignedShort>(ColorFormat::Red, ColorType::UnsignedShort);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, 45828);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferVectorNui() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::GL::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, Vector3ui> Attribute;

    constexpr Vector3ui data[] = { {}, {37448, 547686, 156}, {27592, 157, 25} };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 3*4, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(IntegerShader("uvec3"), RenderbufferFormat::RGBA32UI, mesh)
        .get<Vector3ui>(ColorFormat::RGBAInteger, ColorType::UnsignedInt);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Vector3ui(27592, 157, 25));
}

void MeshGLTest::addVertexBufferVectorNi() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::GL::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, Vector2i> Attribute;

    constexpr Vector2i data[] = { {}, {-37448, 547686}, {27592, -157} };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 2*4, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(IntegerShader("ivec2"), RenderbufferFormat::RG32I, mesh)
        .get<Vector2i>(ColorFormat::RGInteger, ColorType::Int);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Vector2i(27592, -157));
}
#endif

void MeshGLTest::addVertexBufferVectorN() {
    typedef AbstractShaderProgram::Attribute<0, Vector3> Attribute;

    const Vector3 data[] = { {}, {0.0f, -0.9f, 1.0f}, Math::normalize<Vector3>(Color3ub(96, 24, 156)) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 3*4, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(FloatShader("vec3", "vec4(valueInterpolated, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color3ub>(ColorFormat::RGBA, ColorType::UnsignedByte);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Color3ub(96, 24, 156));
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferVectorNd() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::GL::ARB::vertex_attrib_64bit::string() + std::string(" is not available."));

    typedef AbstractShaderProgram::Attribute<0, Vector4d> Attribute;

    const Vector4d data[] = {
        {}, {0.0, -0.9, 1.0, 1.25},
        Math::normalize<Vector4d>(Math::Vector4<UnsignedShort>(315, 65201, 2576, 12))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4*8, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(DoubleShader("dvec4", "vec4", "vec4(value)"),
        RenderbufferFormat::RGBA16, mesh).get<Math::Vector4<UnsignedShort>>(ColorFormat::RGBA, ColorType::UnsignedShort);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Math::Vector4<UnsignedShort>(315, 65201, 2576, 12));
}
#endif

void MeshGLTest::addVertexBufferMatrixNxN() {
    typedef AbstractShaderProgram::Attribute<0, Matrix3x3> Attribute;

    const Matrix3x3 data[] = {
        {},
        Matrix3x3::fromDiagonal({0.0f, -0.9f, 1.0f}),
        Matrix3x3::fromDiagonal(Math::normalize<Vector3>(Color3ub(96, 24, 156)))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 3*3*4, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(FloatShader("mat3",
        "vec4(valueInterpolated[0][0], valueInterpolated[1][1], valueInterpolated[2][2], 0.0)"),
        RenderbufferFormat::RGBA8, mesh).get<Color3ub>(ColorFormat::RGBA, ColorType::UnsignedByte);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Color3ub(96, 24, 156));
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferMatrixNxNd() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::GL::ARB::vertex_attrib_64bit::string() + std::string(" is not available."));

    typedef AbstractShaderProgram::Attribute<0, Matrix3x3d> Attribute;

    const Matrix3x3d data[] = {
        {},
        Matrix3x3d::fromDiagonal({0.0, -0.9, 1.0}),
        Matrix3x3d::fromDiagonal(Math::normalize<Vector3d>(Math::Vector3<UnsignedShort>(315, 65201, 2576)))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 3*3*8, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(DoubleShader("dmat3", "vec4",
        "vec4(value[0][0], value[1][1], value[2][2], 0.0)"),
        RenderbufferFormat::RGBA16, mesh).get<Math::Vector3<UnsignedShort>>(ColorFormat::RGB, ColorType::UnsignedShort);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Math::Vector3<UnsignedShort>(315, 65201, 2576));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferMatrixMxN() {
    typedef AbstractShaderProgram::Attribute<0, Matrix3x4> Attribute;

    const Matrix3x4 data[] = {
        {},
        Matrix3x4::fromDiagonal({0.0f, -0.9f, 1.0f}),
        Matrix3x4::fromDiagonal(Math::normalize<Vector3>(Color3ub(96, 24, 156)))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 3*4*4, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(FloatShader("mat3x4",
        "vec4(valueInterpolated[0][0], valueInterpolated[1][1], valueInterpolated[2][2], 0.0)"),
        RenderbufferFormat::RGBA8, mesh).get<Color3ub>(ColorFormat::RGBA, ColorType::UnsignedByte);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Color3ub(96, 24, 156));
}
#endif

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferMatrixMxNd() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::GL::ARB::vertex_attrib_64bit::string() + std::string(" is not available."));

    typedef AbstractShaderProgram::Attribute<0, Matrix3x4d> Attribute;

    const Matrix3x4d data[] = {
        {},
        Matrix3x4d::fromDiagonal({0.0f, -0.9f, 1.0f}),
        Matrix3x4d::fromDiagonal(Math::normalize<Vector3d>(Math::Vector3<UnsignedShort>(315, 65201, 2576)))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 3*4*8, Attribute());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(DoubleShader("dmat3x4", "vec4",
        "vec4(value[0][0], value[1][1], value[2][2], 0.0)"),
        RenderbufferFormat::RGBA16, mesh).get<Math::Vector3<UnsignedShort>>(ColorFormat::RGB, ColorType::UnsignedShort);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Math::Vector3<UnsignedShort>(315, 65201, 2576));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferUnsignedIntWithUnsignedShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::GL::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, UnsignedInt> Attribute;

    constexpr UnsignedShort data[] = { 0, 49563, 16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 2, Attribute(Attribute::DataType::UnsignedShort));

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(IntegerShader("uint"), RenderbufferFormat::R16UI, mesh)
        .get<UnsignedShort>(ColorFormat::RedInteger, ColorType::UnsignedShort);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, 16583);
}

void MeshGLTest::addVertexBufferUnsignedIntWithShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::GL::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, UnsignedInt> Attribute;

    constexpr Short data[] = { 0, 24563, 16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 2, Attribute(Attribute::DataType::Short));

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(IntegerShader("uint"), RenderbufferFormat::R16I, mesh)
        .get<Short>(ColorFormat::RedInteger, ColorType::Short);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, 16583);
}

void MeshGLTest::addVertexBufferIntWithUnsignedShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::GL::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, Int> Attribute;

    constexpr UnsignedShort data[] = { 0, 49563, 16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 2, Attribute(Attribute::DataType::UnsignedShort));

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(IntegerShader("int"), RenderbufferFormat::R16UI, mesh)
        .get<UnsignedShort>(ColorFormat::RedInteger, ColorType::UnsignedShort);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, 16583);
}

void MeshGLTest::addVertexBufferIntWithShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::GL::EXT::gpu_shader4::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, Int> Attribute;

    constexpr Short data[] = { 0, 24563, -16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 2, Attribute(Attribute::DataType::Short));

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(IntegerShader("int"), RenderbufferFormat::R16I, mesh)
        .get<Short>(ColorFormat::RedInteger, ColorType::Short);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, -16583);
}
#endif

void MeshGLTest::addVertexBufferFloatWithHalfFloat() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::NV::half_float>())
        CORRADE_SKIP(Extensions::GL::NV::half_float::string() + std::string(" is not supported."));
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::vertex_half_float>())
        CORRADE_SKIP(Extensions::GL::OES::vertex_half_float::string() + std::string(" is not supported."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, Float> Attribute;

    Buffer buffer;
    buffer.setData({nullptr, 6}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 2, Attribute(Attribute::DataType::HalfFloat));

    MAGNUM_VERIFY_NO_ERROR();

    /* Won't test the actual values */
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferFloatWithDouble() {
    typedef AbstractShaderProgram::Attribute<0, Float> Attribute;

    const Double data[] = { 0.0, -0.7, Math::normalize<Double, UnsignedByte>(186) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 8, Attribute(Attribute::DataType::Double));

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        RenderbufferFormat::RGBA8, mesh).get<UnsignedByte>(ColorFormat::RGBA, ColorType::UnsignedShort);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, 186);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferVector4WithUnsignedInt2101010Rev() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::vertex_type_2_10_10_10_rev>())
        CORRADE_SKIP(Extensions::GL::ARB::vertex_type_2_10_10_10_rev::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, Vector4> Attribute;

    Buffer buffer;
    buffer.setData({nullptr, 12}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute(Attribute::DataType::UnsignedInt2101010Rev));

    MAGNUM_VERIFY_NO_ERROR();
    /* Won't test the actual values */
}

void MeshGLTest::addVertexBufferVector4WithInt2101010Rev() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::vertex_type_2_10_10_10_rev>())
        CORRADE_SKIP(Extensions::GL::ARB::vertex_type_2_10_10_10_rev::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, Vector4> Attribute;

    Buffer buffer;
    buffer.setData({nullptr, 12}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute(Attribute::DataType::Int2101010Rev));

    MAGNUM_VERIFY_NO_ERROR();
    /* Won't test the actual values */
}
#endif

void MeshGLTest::addVertexBufferLessVectorComponents() {
    typedef AbstractShaderProgram::Attribute<0, Vector4> Attribute;

    const Vector3 data[] = {
        {}, {0.0f, -0.9f, 1.0f},
        Math::normalize<Vector3>(Color3ub(96, 24, 156))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 3*4, Attribute(Attribute::Components::Three));

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(FloatShader("vec4", "valueInterpolated"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(ColorFormat::RGBA, ColorType::UnsignedByte);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Color4ub(96, 24, 156, 255));
}

void MeshGLTest::addVertexBufferNormalized() {
    typedef AbstractShaderProgram::Attribute<0, Vector3> Attribute;

    constexpr Color3ub data[] = { {}, {0, 128, 64}, {32, 156, 228} };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 3, Attribute(Attribute::DataType::UnsignedByte, Attribute::DataOption::Normalized));

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(FloatShader("vec3", "vec4(valueInterpolated, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color3ub>(ColorFormat::RGBA, ColorType::UnsignedByte);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Color3ub(32, 156, 228));
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferBGRA() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::vertex_array_bgra>())
        CORRADE_SKIP(Extensions::GL::ARB::vertex_array_bgra::string() + std::string(" is not available."));
    #endif

    typedef AbstractShaderProgram::Attribute<0, Vector4> Attribute;

    constexpr Color4ub data[] = { {}, {0, 128, 64, 161}, {96, 24, 156, 225} };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute(Attribute::Components::BGRA, Attribute::DataType::UnsignedByte, Attribute::DataOption::Normalized));

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(FloatShader("vec4", "valueInterpolated"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(ColorFormat::RGBA, ColorType::UnsignedByte);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Color4ub(156, 24, 96, 225));
}
#endif

namespace {
    struct MultipleShader: AbstractShaderProgram {
        typedef Attribute<0, Vector3> Position;
        typedef Attribute<1, Vector3> Normal;
        typedef Attribute<2, Vector2> TextureCoordinates;

        explicit MultipleShader();
    };
}

#ifndef DOXYGEN_GENERATING_OUTPUT
MultipleShader::MultipleShader() {
    #ifndef MAGNUM_TARGET_GLES
    Shader vert(Version::GL210, Shader::Type::Vertex);
    #else
    Shader vert(Version::GLES200, Shader::Type::Vertex);
    #endif

    vert.addSource("attribute mediump vec4 position;\n"
                   "attribute mediump vec3 normal;\n"
                   "attribute mediump vec2 textureCoordinates;\n"
                   "varying mediump vec4 valueInterpolated;\n"
                   "void main() {\n"
                   "    valueInterpolated = position + vec4(normal, 0.0) + vec4(textureCoordinates, 0.0, 0.0);\n"
                   "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
                   "}\n");
    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile());
    attachShader(vert);

    #ifndef MAGNUM_TARGET_GLES
    Shader frag(Version::GL210, Shader::Type::Fragment);
    #else
    Shader frag(Version::GLES200, Shader::Type::Fragment);
    #endif

    frag.addSource("varying mediump vec4 valueInterpolated;\n"
                   "void main() { gl_FragColor = valueInterpolated; }\n");
    CORRADE_INTERNAL_ASSERT_OUTPUT(frag.compile());
    attachShader(frag);

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
        Math::normalize<Float, UnsignedByte>(64),
            Math::normalize<Float, UnsignedByte>(17),
                Math::normalize<Float, UnsignedByte>(56),
        Math::normalize<Float, UnsignedByte>(15),
            Math::normalize<Float, UnsignedByte>(164),
                Math::normalize<Float, UnsignedByte>(17),
        Math::normalize<Float, UnsignedByte>(97),
            Math::normalize<Float, UnsignedByte>(28)
    };

    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 1*4, MultipleShader::Position(),
        MultipleShader::Normal(), MultipleShader::TextureCoordinates());

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(MultipleShader(),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(ColorFormat::RGBA, ColorType::UnsignedByte);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Color4ub(64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255));
}

void MeshGLTest::addVertexBufferMultipleGaps() {
     const Float data[] = {
        0.0f, 0.0f, 0.0f, 0.0f, /* Offset */

        /* First attribute */
        0.3f, 0.1f, 0.5f, 0.0f,
            0.4f, 0.0f, -0.9f, 0.0f,
                1.0f, -0.5f, 0.0f, 0.0f,

        /* Second attribute */
        Math::normalize<Float, UnsignedByte>(64),
            Math::normalize<Float, UnsignedByte>(17),
                Math::normalize<Float, UnsignedByte>(56), 0.0f,
        Math::normalize<Float, UnsignedByte>(15),
            Math::normalize<Float, UnsignedByte>(164),
                Math::normalize<Float, UnsignedByte>(17), 0.0f,
        Math::normalize<Float, UnsignedByte>(97),
            Math::normalize<Float, UnsignedByte>(28), 0.0f, 0.0f
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4*4,
        MultipleShader::Position(), 1*4,
        MultipleShader::Normal(), 1*4,
        MultipleShader::TextureCoordinates(), 2*4);

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = Checker(MultipleShader(),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(ColorFormat::RGBA, ColorType::UnsignedByte);

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, Color4ub(64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255));
}

namespace {
    struct IndexChecker {
        explicit IndexChecker(Mesh& mesh);
        Color4ub get();

        Renderbuffer renderbuffer;
        Framebuffer framebuffer;
    };

    const Float indexedVertexData[] = {
        0.0f, /* Offset */

        /* First attribute */
        Math::normalize<Float, UnsignedByte>(64),
            Math::normalize<Float, UnsignedByte>(17),
                Math::normalize<Float, UnsignedByte>(56),
        Math::normalize<Float, UnsignedByte>(15),
            Math::normalize<Float, UnsignedByte>(164),
                Math::normalize<Float, UnsignedByte>(17),
        Math::normalize<Float, UnsignedByte>(97),
            Math::normalize<Float, UnsignedByte>(28),

        /* Second attribute */
        0.3f, 0.1f, 0.5f,
            0.4f, 0.0f, -0.9f,
                1.0f, -0.5f
    };

    constexpr Color4ub indexedResult(64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
IndexChecker::IndexChecker(Mesh& mesh): framebuffer({{}, Vector2i(1)}) {
    #ifndef MAGNUM_TARGET_GLES2
    renderbuffer.setStorage(RenderbufferFormat::RGBA8, Vector2i(1));
    #else
    renderbuffer.setStorage(RenderbufferFormat::RGBA4, Vector2i(1));
    #endif
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer);

    framebuffer.bind(FramebufferTarget::ReadDraw);
    MultipleShader shader;
    shader.use();
    mesh.setIndexCount(2)
        .setPrimitive(MeshPrimitive::Points);

    /* Skip first vertex so we test also offsets */
    MeshView(mesh).setIndexRange(1, 1).draw();
}

Color4ub IndexChecker::get() {
    Image2D image(ColorFormat::RGBA, ColorType::UnsignedByte);
    framebuffer.read({}, Vector2i(1), image);
    return image.data<Color4ub>()[0];
}
#endif

void MeshGLTest::setIndexBuffer() {
    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices(Buffer::Target::ElementArray);
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, Mesh::IndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = IndexChecker(mesh).get();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

void MeshGLTest::setIndexBufferRange() {
    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices(Buffer::Target::ElementArray);
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, Mesh::IndexType::UnsignedShort, 0, 1);

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = IndexChecker(mesh).get();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

void MeshGLTest::setIndexBufferUnsignedInt() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::element_index_uint>())
        CORRADE_SKIP(Extensions::GL::OES::element_index_uint::string() + std::string(" is not available."));
    #endif

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedInt indexData[] = { 2, 1, 0 };
    Buffer indices(Buffer::Target::ElementArray);
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 4, Mesh::IndexType::UnsignedInt);

    MAGNUM_VERIFY_NO_ERROR();

    const auto value = IndexChecker(mesh).get();

    MAGNUM_VERIFY_NO_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

}}

CORRADE_TEST_MAIN(Magnum::Test::MeshGLTest)
