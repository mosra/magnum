/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/GL/AbstractShaderProgram.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct AttributeTest: TestSuite::Tester {
    explicit AttributeTest();

    void attributeScalar();
    void attributeScalarInt();
    void attributeScalarUnsignedInt();
    void attributeScalarDouble();

    void attributeVector();
    void attributeVectorInt();
    void attributeVectorUnsignedInt();
    void attributeVectorDouble();
    void attributeVector4();
    void attributeVectorBGRA();

    void attributeMatrixNxN();
    #ifndef MAGNUM_TARGET_GLES2
    void attributeMatrixMxN();
    #endif
    void attributeMatrixNxNd();
    void attributeMatrixMxNd();

    void debugComponents1();
    void debugComponents2();
    void debugComponents3();
    #ifndef MAGNUM_TARGET_GLES2
    void debugComponents4();
    #endif
    void debugComponentsMatrix2();
    void debugComponentsMatrix3();
    void debugComponentsMatrix4();
    void debugComponentsVector4();
    void debugDataTypeFloat();
    #ifndef MAGNUM_TARGET_GLES2
    void debugDataTypeInt();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void debugDataTypeDouble();
    #endif
    void debugDataTypeVector3();
    void debugDataTypeVector4();
};

AttributeTest::AttributeTest() {
    addTests({&AttributeTest::attributeScalar,
              &AttributeTest::attributeScalarInt,
              &AttributeTest::attributeScalarUnsignedInt,
              &AttributeTest::attributeScalarDouble,

              &AttributeTest::attributeVector,
              &AttributeTest::attributeVectorInt,
              &AttributeTest::attributeVectorUnsignedInt,
              &AttributeTest::attributeVectorDouble,
              &AttributeTest::attributeVector4,
              &AttributeTest::attributeVectorBGRA,

              &AttributeTest::attributeMatrixNxN,
              #ifndef MAGNUM_TARGET_GLES2
              &AttributeTest::attributeMatrixMxN,
              #endif
              &AttributeTest::attributeMatrixNxNd,
              &AttributeTest::attributeMatrixMxNd,

              &AttributeTest::debugComponents1,
              &AttributeTest::debugComponents2,
              &AttributeTest::debugComponents3,
              #ifndef MAGNUM_TARGET_GLES2
              &AttributeTest::debugComponents4,
              #endif
              &AttributeTest::debugComponentsMatrix2,
              &AttributeTest::debugComponentsMatrix3,
              &AttributeTest::debugComponentsMatrix4,
              &AttributeTest::debugComponentsVector4,
              &AttributeTest::debugDataTypeFloat,
              #ifndef MAGNUM_TARGET_GLES2
              &AttributeTest::debugDataTypeInt,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &AttributeTest::debugDataTypeDouble,
              #endif
              &AttributeTest::debugDataTypeVector3,
              &AttributeTest::debugDataTypeVector4});
}

void AttributeTest::attributeScalar() {
    typedef Attribute<3, Float> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::Location, 3);
    CORRADE_COMPARE(Attribute::VectorCount, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::One);
    CORRADE_VERIFY(!a.dataOptions());
    CORRADE_COMPARE(a.vectorSize(), 4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);

    /* Options */
    Attribute b(Attribute::DataType::UnsignedShort, Attribute::DataOption::Normalized);
    CORRADE_COMPARE(b.vectorSize(), 2);
    CORRADE_VERIFY(b.dataOptions() <= Attribute::DataOption::Normalized);
}

void AttributeTest::attributeScalarInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<3, Int> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Int>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.vectorSize(), 4);

    /* Options */
    Attribute b(Attribute::DataType::Short);
    CORRADE_COMPARE(b.vectorSize(), 2);
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeScalarUnsignedInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<3, UnsignedInt> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, UnsignedInt>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.vectorSize(), 4);

    /* Options */
    Attribute b(Attribute::DataType::UnsignedByte);
    CORRADE_COMPARE(b.vectorSize(), 1);
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeScalarDouble() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Double> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Double>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.vectorSize(), 8);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeVector() {
    typedef Attribute<3, Vector3> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Three);
    CORRADE_COMPARE(a.vectorSize(), 3*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);

    /* Options */
    #ifndef MAGNUM_TARGET_GLES
    Attribute b(Attribute::Components::Two, Attribute::DataType::Double);
    CORRADE_COMPARE(b.components(), Attribute::Components::Two);
    CORRADE_COMPARE(b.vectorSize(), 2*8);
    #else
    Attribute b(Attribute::Components::Two, Attribute::DataType::Float);
    CORRADE_COMPARE(b.components(), Attribute::Components::Two);
    CORRADE_COMPARE(b.vectorSize(), 2*4);
    #endif
}

void AttributeTest::attributeVectorInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<3, Vector2i> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Int>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.vectorSize(), 2*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Int);

    /* Options */
    Attribute b(Attribute::Components::One, Attribute::DataType::Int);
    CORRADE_COMPARE(b.vectorSize(), 4);
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeVectorUnsignedInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<3, Vector4ui> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, UnsignedInt>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.vectorSize(), 4*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::UnsignedInt);

    /* Options */
    Attribute b(Attribute::Components::Three, Attribute::DataType::UnsignedShort);
    CORRADE_COMPARE(b.vectorSize(), 3*2);
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeVectorDouble() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Vector2d> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Double>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.vectorSize(), 2*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);

    /* Options */
    Attribute b(Attribute::Components::One);
    CORRADE_COMPARE(b.vectorSize(), 8);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeVector4() {
    typedef Attribute<3, Vector4> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 1);

    /* Custom type */
    #ifndef MAGNUM_TARGET_GLES
    Attribute a(Attribute::DataType::UnsignedInt2101010Rev);
    CORRADE_COMPARE(a.vectorSize(), 4);
    #else
    Attribute a(Attribute::DataType::HalfFloat);
    CORRADE_COMPARE(a.vectorSize(), 8);
    #endif
}

void AttributeTest::attributeVectorBGRA() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Vector4> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 1);

    /* BGRA */
    Attribute a(Attribute::Components::BGRA);
    CORRADE_COMPARE(a.vectorSize(), 4*4);
    #else
    CORRADE_SKIP("BGRA attribute component ordering is not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeMatrixNxN() {
    typedef Attribute<3, Matrix3> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 3);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Three);
    CORRADE_COMPARE(a.vectorSize(), 3*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::attributeMatrixMxN() {
    typedef Attribute<3, Matrix3x4> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 3);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.vectorSize(), 4*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);
}
#endif

void AttributeTest::attributeMatrixNxNd() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Matrix4d> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Double>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 4);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.vectorSize(), 4*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeMatrixMxNd() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Matrix4x2d> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Double>{}));
    CORRADE_COMPARE(Attribute::VectorCount, 4);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.vectorSize(), 2*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::debugComponents1() {
    typedef Attribute<3, Float> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::Components::One << Attribute::Components(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::Components::One GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugComponents2() {
    typedef Attribute<3, Vector2> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::Components::Two << Attribute::Components(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::Components::Two GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugComponents3() {
    typedef Attribute<3, Vector3> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::Components::Three << Attribute::Components(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::Components::Three GL::Attribute::Components(0xdead)\n");
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::debugComponents4() {
    typedef Attribute<3, Vector4i> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::Components::Four << Attribute::Components(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::Components::Four GL::Attribute::Components(0xdead)\n");
}
#endif

void AttributeTest::debugComponentsMatrix2() {
    typedef Attribute<3, Matrix2x2> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::Components::Two << Attribute::Components(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::Components::Two GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugComponentsMatrix3() {
    typedef Attribute<3, Matrix3> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::Components::Three << Attribute::Components(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::Components::Three GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugComponentsMatrix4() {
    typedef Attribute<3, Matrix4> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::Components::Four << Attribute::Components(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::Components::Four GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugComponentsVector4() {
    typedef Attribute<3, Vector4> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::Components::Three << Attribute::Components(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::Components::Three GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugDataTypeFloat() {
    typedef Attribute<3, Float> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::DataType::HalfFloat << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::DataType::HalfFloat GL::Attribute::DataType(0xdead)\n");
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::debugDataTypeInt() {
    typedef Attribute<3, Int> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::DataType::Short << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::DataType::Short GL::Attribute::DataType(0xdead)\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AttributeTest::debugDataTypeDouble() {
    typedef Attribute<3, Double> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::DataType::Double << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::DataType::Double GL::Attribute::DataType(0xdead)\n");
}
#endif

void AttributeTest::debugDataTypeVector3() {
    typedef Attribute<3, Vector3> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::DataType::HalfFloat << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::DataType::HalfFloat GL::Attribute::DataType(0xdead)\n");
}

void AttributeTest::debugDataTypeVector4() {
    typedef Attribute<3, Vector4> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::DataType::HalfFloat << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::DataType::HalfFloat GL::Attribute::DataType(0xdead)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::AttributeTest)
