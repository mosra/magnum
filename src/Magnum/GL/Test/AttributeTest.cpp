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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/VertexFormat.h"
#include "Magnum/GL/Attribute.h"

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
    void attributeMatrixNxNCustomStride();
    void attributeMatrixNxNd();
    void attributeMatrixMxNd();

    void attributeFromGenericFormat();
    #ifndef MAGNUM_TARGET_GLES2
    void attributeFromGenericFormatIntegral();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void attributeFromGenericFormatLong();
    #endif
    void attributeFromGenericFormatMatrixNxN();
    #ifndef MAGNUM_TARGET_GLES2
    void attributeFromGenericFormatMatrixMxN();
    #endif
    void attributeFromGenericFormatEnableNormalized();
    void attributeFromGenericFormatUnexpectedForNormalizedKind();
    #ifndef MAGNUM_TARGET_GLES2
    void attributeFromGenericFormatUnexpectedForIntegralKind();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void attributeFromGenericFormatUnexpectedForLongKind();
    #endif
    void attributeFromGenericFormatTooManyVectors();
    void attributeFromGenericFormatTooManyComponents();
    void attributeFromGenericFormatNotAvailable();

    void hasVertexFormat();

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

    void debugDynamicKind();
    void debugDynamicComponents();
    void debugDynamicDataType();
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
              &AttributeTest::attributeMatrixNxNCustomStride,
              &AttributeTest::attributeMatrixNxNd,
              &AttributeTest::attributeMatrixMxNd,

              &AttributeTest::attributeFromGenericFormat,
              #ifndef MAGNUM_TARGET_GLES2
              &AttributeTest::attributeFromGenericFormatIntegral,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &AttributeTest::attributeFromGenericFormatLong,
              #endif
              &AttributeTest::attributeFromGenericFormatMatrixNxN,
              #ifndef MAGNUM_TARGET_GLES2
              &AttributeTest::attributeFromGenericFormatMatrixMxN,
              #endif
              &AttributeTest::attributeFromGenericFormatEnableNormalized,
              &AttributeTest::attributeFromGenericFormatUnexpectedForNormalizedKind,
              #ifndef MAGNUM_TARGET_GLES2
              &AttributeTest::attributeFromGenericFormatUnexpectedForIntegralKind,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &AttributeTest::attributeFromGenericFormatUnexpectedForLongKind,
              #endif
              &AttributeTest::attributeFromGenericFormatTooManyVectors,
              &AttributeTest::attributeFromGenericFormatTooManyComponents,
              &AttributeTest::attributeFromGenericFormatNotAvailable,

              &AttributeTest::hasVertexFormat,

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
              &AttributeTest::debugDataTypeVector4,

              &AttributeTest::debugDynamicKind,
              &AttributeTest::debugDynamicComponents,
              &AttributeTest::debugDynamicDataType});
}

void AttributeTest::attributeScalar() {
    typedef Attribute<3, Float> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::Location, 3);
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::One);
    CORRADE_VERIFY(!a.dataOptions());
    CORRADE_COMPARE(a.vectorStride(), 4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(da.vectorStride(), 4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);

    /* Options */
    Attribute b(Attribute::DataType::UnsignedShort, Attribute::DataOption::Normalized);
    CORRADE_COMPARE(b.vectorStride(), 2);
    CORRADE_VERIFY(b.dataOptions() <= Attribute::DataOption::Normalized);

    DynamicAttribute db{b};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(db.vectorStride(), 2);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::UnsignedShort);
}

void AttributeTest::attributeScalarInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<2, Int> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Int>{}));
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.vectorStride(), 4);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(da.location(), 2);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(da.vectorStride(), 4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Int);

    /* Options */
    Attribute b(Attribute::DataType::Short);
    CORRADE_COMPARE(b.vectorStride(), 2);

    DynamicAttribute db{b};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(db.location(), 2);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(db.vectorStride(), 2);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Short);
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeScalarUnsignedInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<3, UnsignedInt> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, UnsignedInt>{}));
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.vectorStride(), 4);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(da.vectorStride(), 4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::UnsignedInt);

    /* Options */
    Attribute b(Attribute::DataType::UnsignedByte);
    CORRADE_COMPARE(b.vectorStride(), 1);

    DynamicAttribute db{b};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(db.vectorStride(), 1);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::UnsignedByte);
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeScalarDouble() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Double> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Double>{}));
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.vectorStride(), 8);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(da.vectorStride(), 8);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeVector() {
    typedef Attribute<3, Vector3> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Three);
    CORRADE_COMPARE(a.vectorStride(), 3*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(da.vectorStride(), 3*4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);

    /* Options */
    #ifndef MAGNUM_TARGET_GLES
    Attribute b(Attribute::Components::Two, Attribute::DataType::Double);
    CORRADE_COMPARE(b.components(), Attribute::Components::Two);
    CORRADE_COMPARE(b.vectorStride(), 2*8);

    DynamicAttribute db{b};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(db.vectorStride(), 2*8);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Double);
    #else
    Attribute b(Attribute::Components::Two, Attribute::DataType::Float);
    CORRADE_COMPARE(b.components(), Attribute::Components::Two);
    CORRADE_COMPARE(b.vectorStride(), 2*4);

    DynamicAttribute db{b};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(db.vectorStride(), 2*4);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Float);
    #endif
}

void AttributeTest::attributeVectorInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<3, Vector2i> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Int>{}));
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.vectorStride(), 2*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Int);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(da.vectorStride(), 2*4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Int);

    /* Options */
    Attribute b(Attribute::Components::One, Attribute::DataType::Int);
    CORRADE_COMPARE(b.vectorStride(), 4);

    DynamicAttribute db{b};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(db.vectorStride(), 4);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Int);
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeVectorUnsignedInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<3, Vector4ui> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, UnsignedInt>{}));
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.vectorStride(), 4*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::UnsignedInt);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(da.vectorStride(), 4*4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::UnsignedInt);

    /* Options */
    Attribute b(Attribute::Components::Three, Attribute::DataType::UnsignedShort);
    CORRADE_COMPARE(b.vectorStride(), 3*2);

    DynamicAttribute db{b};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(db.vectorStride(), 3*2);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::UnsignedShort);
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeVectorDouble() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Vector2d> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Double>{}));
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.vectorStride(), 2*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(da.vectorStride(), 2*8);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Double);

    /* Options */
    Attribute b(Attribute::Components::One);
    CORRADE_COMPARE(b.vectorStride(), 8);

    DynamicAttribute db{b};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(db.vectorStride(), 8);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeVector4() {
    typedef Attribute<3, Vector4> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Custom type */
    #ifndef MAGNUM_TARGET_GLES
    Attribute a(Attribute::DataType::UnsignedInt2101010Rev);
    CORRADE_COMPARE(a.vectorStride(), 4);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(da.vectorStride(), 4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::UnsignedInt2101010Rev);
    #elif !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    Attribute a(Attribute::DataType::Half);
    CORRADE_COMPARE(a.vectorStride(), 8);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(da.vectorStride(), 8);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Half);
    #else
    Attribute a(Attribute::DataType::Float);
    CORRADE_COMPARE(a.vectorStride(), 16);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(da.vectorStride(), 16);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);
    #endif
}

void AttributeTest::attributeVectorBGRA() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Vector4> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* BGRA */
    Attribute a(Attribute::Components::BGRA);
    CORRADE_COMPARE(a.vectorStride(), 4*4);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::BGRA);
    CORRADE_COMPARE(da.vectorStride(), 4*4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);
    #else
    CORRADE_SKIP("BGRA attribute component ordering is not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeMatrixNxN() {
    typedef Attribute<3, Matrix3> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::Vectors, 3);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Three);
    CORRADE_COMPARE(a.vectorStride(), 3*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(da.vectorStride(), 3*4);
    CORRADE_COMPARE(da.vectors(), 3);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);
}

void AttributeTest::attributeMatrixNxNCustomStride() {
    typedef Attribute<3, Matrix3> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::Vectors, 3);

    /* Default stride */
    Attribute a{Attribute::DataType::Short};
    CORRADE_COMPARE(a.components(), Attribute::Components::Three);
    CORRADE_COMPARE(a.vectorStride(), 6);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Short);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(da.vectorStride(), 6);
    CORRADE_COMPARE(da.vectors(), 3);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Short);

    /* Custom stride */
    Attribute b{8, Attribute::DataType::Short};
    CORRADE_COMPARE(b.components(), Attribute::Components::Three);
    CORRADE_COMPARE(b.vectorStride(), 8);
    CORRADE_COMPARE(b.dataType(), Attribute::DataType::Short);

    DynamicAttribute db{b};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(db.vectorStride(), 8);
    CORRADE_COMPARE(db.vectors(), 3);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Short);
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::attributeMatrixMxN() {
    typedef Attribute<3, Matrix3x4> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Float>{}));
    CORRADE_COMPARE(Attribute::Vectors, 3);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.vectorStride(), 4*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(da.vectorStride(), 4*4);
    CORRADE_COMPARE(da.vectors(), 3);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);
}
#endif

void AttributeTest::attributeMatrixNxNd() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Matrix4d> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Double>{}));
    CORRADE_COMPARE(Attribute::Vectors, 4);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.vectorStride(), 4*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(da.vectorStride(), 4*8);
    CORRADE_COMPARE(da.vectors(), 4);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeMatrixMxNd() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Matrix4x2d> Attribute;
    CORRADE_VERIFY((std::is_same<Attribute::ScalarType, Double>{}));
    CORRADE_COMPARE(Attribute::Vectors, 4);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.vectorStride(), 2*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);

    DynamicAttribute da{a};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(da.vectorStride(), 2*8);
    CORRADE_COMPARE(da.vectors(), 4);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeFromGenericFormat() {
    DynamicAttribute a{DynamicAttribute::Kind::Generic, 3,
        VertexFormat::UnsignedShort};
    CORRADE_COMPARE(a.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(a.location(), 3);
    CORRADE_COMPARE(a.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(a.vectorStride(), 2);
    CORRADE_COMPARE(a.vectors(), 1);
    CORRADE_COMPARE(a.dataType(), DynamicAttribute::DataType::UnsignedShort);

    /* Check that compile-time attribs work too */
    DynamicAttribute a2{Attribute<7, Vector3>{},
        VertexFormat::UnsignedShort};
    CORRADE_COMPARE(a2.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(a2.location(), 7);
    CORRADE_COMPARE(a2.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(a2.vectorStride(), 2);
    CORRADE_COMPARE(a2.vectors(), 1);
    CORRADE_COMPARE(a2.dataType(), DynamicAttribute::DataType::UnsignedShort);

    DynamicAttribute b{DynamicAttribute::Kind::GenericNormalized, 3,
        VertexFormat::Vector2bNormalized};
    CORRADE_COMPARE(b.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(b.location(), 3);
    CORRADE_COMPARE(b.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(b.vectorStride(), 2);
    CORRADE_COMPARE(b.vectors(), 1);
    CORRADE_COMPARE(b.dataType(), DynamicAttribute::DataType::Byte);

    DynamicAttribute c{DynamicAttribute::Kind::Generic, 3,
        VertexFormat::Vector4ui};
    CORRADE_COMPARE(c.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(c.location(), 3);
    CORRADE_COMPARE(c.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(c.dataType(), DynamicAttribute::DataType::UnsignedInt);

    /* This one shouldn't fail even though the normalization is (probably?)
       ignored. Not exactly sure. */
    DynamicAttribute d{DynamicAttribute::Kind::GenericNormalized, 3,
        VertexFormat::Float};
    CORRADE_COMPARE(d.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(d.location(), 3);
    CORRADE_COMPARE(d.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(d.dataType(), DynamicAttribute::DataType::Float);

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    DynamicAttribute e{DynamicAttribute::Kind::Generic, 15,
        VertexFormat::Vector2h};
    CORRADE_COMPARE(e.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(e.location(), 15);
    CORRADE_COMPARE(e.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(e.dataType(), DynamicAttribute::DataType::Half);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::attributeFromGenericFormatIntegral() {
    DynamicAttribute a{DynamicAttribute::Kind::Integral, 3,
        VertexFormat::Vector3s};
    CORRADE_COMPARE(a.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(a.location(), 3);
    CORRADE_COMPARE(a.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(a.dataType(), DynamicAttribute::DataType::Short);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AttributeTest::attributeFromGenericFormatLong() {
    DynamicAttribute a{DynamicAttribute::Kind::Long, 15,
        VertexFormat::Vector2d};
    CORRADE_COMPARE(a.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(a.location(), 15);
    CORRADE_COMPARE(a.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(a.dataType(), DynamicAttribute::DataType::Double);
}
#endif

void AttributeTest::attributeFromGenericFormatMatrixNxN() {
    DynamicAttribute a{DynamicAttribute::Kind::Generic, 13,
        VertexFormat::Matrix2x2bNormalizedAligned};
    CORRADE_COMPARE(a.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(a.location(), 13);
    CORRADE_COMPARE(a.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(a.vectorStride(), 4);
    CORRADE_COMPARE(a.vectors(), 2);
    CORRADE_COMPARE(a.dataType(), DynamicAttribute::DataType::Byte);

    /* Check that compile-time attribs work too */
    DynamicAttribute a2{Attribute<7, Matrix3x3>{},
        VertexFormat::Matrix2x2};
    CORRADE_COMPARE(a2.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(a2.location(), 7);
    CORRADE_COMPARE(a2.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(a2.vectorStride(), 8);
    CORRADE_COMPARE(a2.vectors(), 2);
    CORRADE_COMPARE(a2.dataType(), DynamicAttribute::DataType::Float);
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::attributeFromGenericFormatMatrixMxN() {
    DynamicAttribute a{DynamicAttribute::Kind::Generic, 13,
        VertexFormat::Matrix4x3h};
    CORRADE_COMPARE(a.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(a.location(), 13);
    CORRADE_COMPARE(a.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(a.vectorStride(), 6);
    CORRADE_COMPARE(a.vectors(), 4);
    CORRADE_COMPARE(a.dataType(), DynamicAttribute::DataType::Half);

    /* Check that compile-time attribs work too */
    DynamicAttribute a2{Attribute<7, Matrix4x4>{},
        VertexFormat::Matrix4x3sNormalizedAligned};
    CORRADE_COMPARE(a2.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(a2.location(), 7);
    CORRADE_COMPARE(a2.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(a2.vectorStride(), 8);
    CORRADE_COMPARE(a2.vectors(), 4);
    CORRADE_COMPARE(a2.dataType(), DynamicAttribute::DataType::Short);
}
#endif

void AttributeTest::attributeFromGenericFormatEnableNormalized() {
    DynamicAttribute a{DynamicAttribute::Kind::Generic, 3,
        VertexFormat::Vector3ubNormalized};
    /* Generic is automatically switched to GenericNormalized */
    CORRADE_COMPARE(a.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(a.location(), 3);
    CORRADE_COMPARE(a.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(a.dataType(), DynamicAttribute::DataType::UnsignedByte);
}

void AttributeTest::attributeFromGenericFormatUnexpectedForNormalizedKind() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    DynamicAttribute{DynamicAttribute::Kind::GenericNormalized, 3,
        VertexFormat::Int};
    CORRADE_COMPARE(out.str(),
        "GL::DynamicAttribute: can't use VertexFormat::Int for a normalized attribute\n");
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::attributeFromGenericFormatUnexpectedForIntegralKind() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    DynamicAttribute{DynamicAttribute::Kind::Integral, 3,
        VertexFormat::Vector2bNormalized};
    DynamicAttribute{DynamicAttribute::Kind::Integral, 3,
        VertexFormat::Vector3};
    CORRADE_COMPARE(out.str(),
        "GL::DynamicAttribute: can't use VertexFormat::Vector2bNormalized for a GL::DynamicAttribute::Kind::Integral attribute\n"
        "GL::DynamicAttribute: can't use VertexFormat::Vector3 for an integral attribute\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AttributeTest::attributeFromGenericFormatUnexpectedForLongKind() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    DynamicAttribute{DynamicAttribute::Kind::Long, 3,
        VertexFormat::UnsignedShortNormalized};
    CORRADE_COMPARE(out.str(),
        "GL::DynamicAttribute: can't use VertexFormat::UnsignedShortNormalized for a GL::DynamicAttribute::Kind::Long attribute\n");
}
#endif

void AttributeTest::attributeFromGenericFormatTooManyVectors() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    DynamicAttribute{Attribute<7, Vector2>{}, VertexFormat::Matrix2x2};
    CORRADE_COMPARE(out.str(),
        "GL::DynamicAttribute: can't use VertexFormat::Matrix2x2 for a 1-vector attribute\n");
}

void AttributeTest::attributeFromGenericFormatTooManyComponents() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    DynamicAttribute{Attribute<7, Vector2>{}, VertexFormat::Vector3};
    CORRADE_COMPARE(out.str(),
        "GL::DynamicAttribute: can't use VertexFormat::Vector3 for a 2-component attribute\n");
}

void AttributeTest::attributeFromGenericFormatNotAvailable() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_SKIP("All attribute formats available on desktop GL.");
    #else
    std::ostringstream out;
    Error redirectError{&out};
    DynamicAttribute{Attribute<7, Vector2>{}, VertexFormat::Vector3d};
    CORRADE_COMPARE(out.str(),
        "GL::DynamicAttribute: VertexFormat::Vector3d isn't available on this target\n");
    #endif
}

void AttributeTest::hasVertexFormat() {
    CORRADE_VERIFY(GL::hasVertexFormat(Magnum::VertexFormat::Vector2i));
    CORRADE_VERIFY(GL::hasVertexFormat(Magnum::VertexFormat::Matrix2x2));
    #ifdef MAGNUM_TARGET_GLES
    CORRADE_VERIFY(!GL::hasVertexFormat(Magnum::VertexFormat::Vector3d));
    CORRADE_VERIFY(!GL::hasVertexFormat(Magnum::VertexFormat::Matrix2x3d));
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_VERIFY(GL::hasVertexFormat(Magnum::VertexFormat::Matrix2x3));
    #else
    CORRADE_VERIFY(!GL::hasVertexFormat(Magnum::VertexFormat::Matrix2x3));
    #endif

    /* Ensure all generic formats are handled by going though all and executing
       out functions on those. This goes through the first 16 bits, which
       should be enough. Going through 32 bits takes 8 seconds, too much. */
    for(UnsignedInt i = 1; i <= 0xffff; ++i) {
        const auto format = Magnum::VertexFormat(i);
        /* Each case only verifies that hasVertexFormat() handles the format
           and doesn't fall into unreachable code */
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(format) {
            #define _c(format) \
                case Magnum::VertexFormat::format: \
                    GL::hasVertexFormat(Magnum::VertexFormat::format); \
                    break;
            #include "Magnum/Implementation/vertexFormatMapping.hpp"
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif
    }
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
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    Debug{&out} << Attribute::DataType::Half << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::DataType::Half GL::Attribute::DataType(0xdead)\n");
    #else
    Debug{&out} << Attribute::DataType::Float << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::DataType::Float GL::Attribute::DataType(0xdead)\n");
    #endif
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
    Debug{&out} << Attribute::DataType::Float << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::DataType::Float GL::Attribute::DataType(0xdead)\n");
}

void AttributeTest::debugDataTypeVector4() {
    typedef Attribute<3, Vector4> Attribute;

    std::ostringstream out;
    Debug{&out} << Attribute::DataType::Float << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::Attribute::DataType::Float GL::Attribute::DataType(0xdead)\n");
}

void AttributeTest::debugDynamicKind() {
    std::ostringstream out;
    Debug{&out} << DynamicAttribute::Kind::GenericNormalized << DynamicAttribute::Kind(0xdead);
    CORRADE_COMPARE(out.str(), "GL::DynamicAttribute::Kind::GenericNormalized GL::DynamicAttribute::Kind(0xdead)\n");
}

void AttributeTest::debugDynamicComponents() {
    std::ostringstream out;
    Debug{&out} << DynamicAttribute::Components::Three << DynamicAttribute::Components(0xdead);
    CORRADE_COMPARE(out.str(), "GL::DynamicAttribute::Components::Three GL::DynamicAttribute::Components(0xdead)\n");
}

void AttributeTest::debugDynamicDataType() {
    std::ostringstream out;
    Debug{&out} << DynamicAttribute::DataType::Float << DynamicAttribute::DataType(0xdead);
    CORRADE_COMPARE(out.str(), "GL::DynamicAttribute::DataType::Float GL::DynamicAttribute::DataType(0xdead)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::AttributeTest)
