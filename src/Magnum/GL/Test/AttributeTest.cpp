/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

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

    void dynamicAttribute();
    void dynamicAttributeMatrix();

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
    void debugDataTypeUnsignedInt();
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

              &AttributeTest::dynamicAttribute,
              &AttributeTest::dynamicAttributeMatrix,

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
              &AttributeTest::debugDataTypeUnsignedInt,
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
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Float>{});
    CORRADE_COMPARE(Attribute::Location, 3);
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.components(), Attribute::Components::One);
    CORRADE_COMPARE(ca.components(), Attribute::Components::One);
    CORRADE_VERIFY(!a.dataOptions());
    CORRADE_VERIFY(!ca.dataOptions());
    CORRADE_COMPARE(a.vectorStride(), 4);
    CORRADE_COMPARE(ca.vectorStride(), 4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);
    CORRADE_COMPARE(ca.dataType(), Attribute::DataType::Float);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(da.vectorStride(), 4);
    CORRADE_COMPARE(cda.vectorStride(), 4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Float);

    /* Options */
    Attribute b{Attribute::DataType::UnsignedShort, Attribute::DataOption::Normalized};
    CORRADE_COMPARE(b.vectorStride(), 2);
    CORRADE_VERIFY(b.dataOptions() <= Attribute::DataOption::Normalized);

    /* The constexpr variant needs to specify vector stride explicitly.
       Constexpr accessors tested just here, other variants don't have any
       difference. */
    constexpr Attribute cb{3, Attribute::DataType::UnsignedShort, Attribute::DataOption::Normalized};
    constexpr Attribute::Components components = cb.components();
    constexpr Attribute::DataType dataType = cb.dataType();
    constexpr UnsignedInt vectorStride = cb.vectorStride();
    constexpr Attribute::DataOptions dataOptions = cb.dataOptions();
    CORRADE_COMPARE(components, Attribute::Components::One);
    CORRADE_COMPARE(dataType, Attribute::DataType::UnsignedShort);
    CORRADE_COMPARE(vectorStride, 3);
    CORRADE_VERIFY(dataOptions <= Attribute::DataOption::Normalized);

    /* Constexpr dynamic attribute accessors tested in dynamicAttributeMatrix() */
    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(db.vectorStride(), 2);
    CORRADE_COMPARE(cdb.vectorStride(), 3);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::UnsignedShort);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::UnsignedShort);
}

void AttributeTest::attributeScalarInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<2, Int> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Int>{});
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.vectorStride(), 4);
    CORRADE_COMPARE(ca.vectorStride(), 4);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(da.location(), 2);
    CORRADE_COMPARE(cda.location(), 2);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(da.vectorStride(), 4);
    CORRADE_COMPARE(cda.vectorStride(), 4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(cda.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Int);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Int);

    /* Options. The constexpr variant needs to specify vector stride
       explicitly. */
    Attribute b{Attribute::DataType::Short};
    constexpr Attribute cb{3, Attribute::DataType::Short};
    CORRADE_COMPARE(b.vectorStride(), 2);
    CORRADE_COMPARE(cb.vectorStride(), 3);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(db.location(), 2);
    CORRADE_COMPARE(cdb.location(), 2);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(db.vectorStride(), 2);
    CORRADE_COMPARE(cdb.vectorStride(), 3);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Short);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::Short);
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeScalarUnsignedInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<3, UnsignedInt> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, UnsignedInt>{});
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.vectorStride(), 4);
    CORRADE_COMPARE(ca.vectorStride(), 4);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(da.vectorStride(), 4);
    CORRADE_COMPARE(cda.vectorStride(), 4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(cda.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::UnsignedInt);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::UnsignedInt);

    /* Options. The constexpr variant needs to specify vector stride
       explicitly. */
    Attribute b{Attribute::DataType::UnsignedByte};
    constexpr Attribute cb{2, Attribute::DataType::UnsignedByte};
    CORRADE_COMPARE(b.vectorStride(), 1);
    CORRADE_COMPARE(cb.vectorStride(), 2);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(db.vectorStride(), 1);
    CORRADE_COMPARE(cdb.vectorStride(), 2);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::UnsignedByte);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::UnsignedByte);
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeScalarDouble() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Double> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Double>{});
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.vectorStride(), 8);
    CORRADE_COMPARE(ca.vectorStride(), 8);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(da.vectorStride(), 8);
    CORRADE_COMPARE(cda.vectorStride(), 8);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(cda.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Double);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeVector() {
    typedef Attribute<3, Vector3> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Float>{});
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.components(), Attribute::Components::Three);
    CORRADE_COMPARE(ca.components(), Attribute::Components::Three);
    CORRADE_COMPARE(a.vectorStride(), 3*4);
    CORRADE_COMPARE(ca.vectorStride(), 3*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);
    CORRADE_COMPARE(ca.dataType(), Attribute::DataType::Float);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(da.vectorStride(), 3*4);
    CORRADE_COMPARE(cda.vectorStride(), 3*4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(cda.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Float);

    /* Options. The constexpr variant needs to specify vector stride
       explicitly. */
    #ifndef MAGNUM_TARGET_GLES
    Attribute b{Attribute::Components::Two, Attribute::DataType::Double};
    constexpr Attribute cb{Attribute::Components::Two, 18, Attribute::DataType::Double};
    CORRADE_COMPARE(b.components(), Attribute::Components::Two);
    CORRADE_COMPARE(cb.components(), Attribute::Components::Two);
    CORRADE_COMPARE(b.vectorStride(), 2*8);
    CORRADE_COMPARE(cb.vectorStride(), 18);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(db.vectorStride(), 2*8);
    CORRADE_COMPARE(cdb.vectorStride(), 18);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Double);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::Double);
    #else
    Attribute b{Attribute::Components::Two, Attribute::DataType::Float};
    constexpr Attribute cb{Attribute::Components::Two, 14, Attribute::DataType::Float};
    CORRADE_COMPARE(b.components(), Attribute::Components::Two);
    CORRADE_COMPARE(cb.components(), Attribute::Components::Two);
    CORRADE_COMPARE(b.vectorStride(), 2*4);
    CORRADE_COMPARE(cb.vectorStride(), 14);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(db.vectorStride(), 2*4);
    CORRADE_COMPARE(cdb.vectorStride(), 14);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Float);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::Float);
    #endif
}

void AttributeTest::attributeVectorInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<3, Vector2i> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Int>{});
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(ca.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.vectorStride(), 2*4);
    CORRADE_COMPARE(ca.vectorStride(), 2*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Int);
    CORRADE_COMPARE(ca.dataType(), Attribute::DataType::Int);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(da.vectorStride(), 2*4);
    CORRADE_COMPARE(cda.vectorStride(), 2*4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(cda.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Int);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Int);

    /* Options. The constexpr variant needs to specify vector stride
       explicitly. */
    Attribute b{Attribute::Components::One, Attribute::DataType::Int};
    constexpr Attribute cb{Attribute::Components::One, 6, Attribute::DataType::Int};
    CORRADE_COMPARE(b.vectorStride(), 4);
    CORRADE_COMPARE(cb.vectorStride(), 6);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(db.vectorStride(), 4);
    CORRADE_COMPARE(cdb.vectorStride(), 6);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Int);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::Int);

    /* Unsigned types for signed attributes are not supported on WebGL, exposed
       just as deprecated for backwards compatibility */
    #if !defined(MAGNUM_TARGET_WEBGL) || defined(MAGNUM_BUILD_DEPRECATED)
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_IGNORE_DEPRECATED_PUSH
    #endif
    Attribute c{Attribute::Components::One, Attribute::DataType::UnsignedShort};
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
    CORRADE_COMPARE(c.vectorStride(), 2);
    #endif
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeVectorUnsignedInt() {
    #ifndef MAGNUM_TARGET_GLES2
    typedef Attribute<3, Vector4ui> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, UnsignedInt>{});
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(ca.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.vectorStride(), 4*4);
    CORRADE_COMPARE(ca.vectorStride(), 4*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::UnsignedInt);
    CORRADE_COMPARE(ca.dataType(), Attribute::DataType::UnsignedInt);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(da.vectorStride(), 4*4);
    CORRADE_COMPARE(cda.vectorStride(), 4*4);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(cda.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::UnsignedInt);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::UnsignedInt);

    /* Options. The constexpr variant needs to specify vector stride
       explicitly. */
    Attribute b{Attribute::Components::Three, Attribute::DataType::UnsignedShort};
    constexpr Attribute cb{Attribute::Components::Three, 7, Attribute::DataType::UnsignedShort};
    CORRADE_COMPARE(b.vectorStride(), 3*2);
    CORRADE_COMPARE(cb.vectorStride(), 7);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Integral);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(db.vectorStride(), 3*2);
    CORRADE_COMPARE(cdb.vectorStride(), 7);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::UnsignedShort);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::UnsignedShort);

    /* Signed types for unsigned attributes are not supported on WebGL, exposed
       just as deprecated for backwards compatibility */
    #if !defined(MAGNUM_TARGET_WEBGL) || defined(MAGNUM_BUILD_DEPRECATED)
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_IGNORE_DEPRECATED_PUSH
    #endif
    Attribute c{Attribute::Components::One, Attribute::DataType::Short};
    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_IGNORE_DEPRECATED_POP
    #endif
    CORRADE_COMPARE(c.vectorStride(), 2);
    #endif
    #else
    CORRADE_SKIP("Integer attributes are not available in OpenGL ES 2.");
    #endif
}

void AttributeTest::attributeVectorDouble() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Vector2d> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Double>{});
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(ca.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.vectorStride(), 2*8);
    CORRADE_COMPARE(ca.vectorStride(), 2*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);
    CORRADE_COMPARE(ca.dataType(), Attribute::DataType::Double);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(da.vectorStride(), 2*8);
    CORRADE_COMPARE(cda.vectorStride(), 2*8);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(cda.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Double);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Double);

    /* Options. The constexpr variant needs to specify vector stride
       explicitly. */
    Attribute b{Attribute::Components::One};
    constexpr Attribute cb{Attribute::Components::One, 10};
    CORRADE_COMPARE(b.vectorStride(), 8);
    CORRADE_COMPARE(cb.vectorStride(), 10);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::One);
    CORRADE_COMPARE(db.vectorStride(), 8);
    CORRADE_COMPARE(cdb.vectorStride(), 10);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Double);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeVector4() {
    typedef Attribute<3, Vector4> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Float>{});
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(ca.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.vectorStride(), 4*4);
    CORRADE_COMPARE(ca.vectorStride(), 4*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);
    CORRADE_COMPARE(ca.dataType(), Attribute::DataType::Float);

    /* Custom type. The constexpr variant needs to specify vector stride
       explicitly. */
    #ifndef MAGNUM_TARGET_GLES
    Attribute b{Attribute::DataType::UnsignedInt2101010Rev};
    constexpr Attribute cb{6, Attribute::DataType::UnsignedInt2101010Rev};
    CORRADE_COMPARE(b.vectorStride(), 4);
    CORRADE_COMPARE(cb.vectorStride(), 6);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(db.vectorStride(), 4);
    CORRADE_COMPARE(cdb.vectorStride(), 6);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::UnsignedInt2101010Rev);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::UnsignedInt2101010Rev);
    #elif !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    Attribute b(Attribute::DataType::Half);
    constexpr Attribute cb(10, Attribute::DataType::Half);
    CORRADE_COMPARE(b.vectorStride(), 8);
    CORRADE_COMPARE(cb.vectorStride(), 10);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(db.vectorStride(), 8);
    CORRADE_COMPARE(cdb.vectorStride(), 10);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Half);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::Half);
    #else
    Attribute b{Attribute::DataType::Float};
    constexpr Attribute cb{18, Attribute::DataType::Float};
    CORRADE_COMPARE(b.vectorStride(), 16);
    CORRADE_COMPARE(cb.vectorStride(), 18);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(db.vectorStride(), 16);
    CORRADE_COMPARE(cdb.vectorStride(), 18);
    CORRADE_COMPARE(db.vectors(), 1);
    CORRADE_COMPARE(cdb.vectors(), 1);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Float);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::Float);
    #endif
}

void AttributeTest::attributeVectorBGRA() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Vector4> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Float>{});
    CORRADE_COMPARE(Attribute::Vectors, 1);

    /* BGRA. The constexpr variant needs to specify vector stride
       explicitly. */
    Attribute a{Attribute::Components::BGRA};
    constexpr Attribute ca{Attribute::Components::BGRA, 18};
    CORRADE_COMPARE(a.vectorStride(), 4*4);
    CORRADE_COMPARE(ca.vectorStride(), 18);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::BGRA);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::BGRA);
    CORRADE_COMPARE(da.vectorStride(), 4*4);
    CORRADE_COMPARE(cda.vectorStride(), 18);
    CORRADE_COMPARE(da.vectors(), 1);
    CORRADE_COMPARE(cda.vectors(), 1);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Float);
    #else
    CORRADE_SKIP("BGRA attribute component ordering is not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeMatrixNxN() {
    typedef Attribute<3, Matrix3> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Float>{});
    CORRADE_COMPARE(Attribute::Vectors, 3);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.components(), Attribute::Components::Three);
    CORRADE_COMPARE(ca.components(), Attribute::Components::Three);
    CORRADE_COMPARE(a.vectorStride(), 3*4);
    CORRADE_COMPARE(ca.vectorStride(), 3*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);
    CORRADE_COMPARE(ca.dataType(), Attribute::DataType::Float);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(da.vectorStride(), 3*4);
    CORRADE_COMPARE(cda.vectorStride(), 3*4);
    CORRADE_COMPARE(da.vectors(), 3);
    CORRADE_COMPARE(cda.vectors(), 3);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Float);
}

void AttributeTest::attributeMatrixNxNCustomStride() {
    typedef Attribute<3, Matrix3> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Float>{});
    CORRADE_COMPARE(Attribute::Vectors, 3);

    /* Default stride, not possible to do in a constexpr way */
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
    constexpr Attribute cb{8, Attribute::DataType::Short};
    CORRADE_COMPARE(b.components(), Attribute::Components::Three);
    CORRADE_COMPARE(cb.components(), Attribute::Components::Three);
    CORRADE_COMPARE(b.vectorStride(), 8);
    CORRADE_COMPARE(cb.vectorStride(), 8);
    CORRADE_COMPARE(b.dataType(), Attribute::DataType::Short);
    CORRADE_COMPARE(cb.dataType(), Attribute::DataType::Short);

    DynamicAttribute db{b};
    constexpr DynamicAttribute cdb{cb};
    CORRADE_COMPARE(db.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cdb.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(db.location(), 3);
    CORRADE_COMPARE(cdb.location(), 3);
    CORRADE_COMPARE(db.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(cdb.components(), DynamicAttribute::Components::Three);
    CORRADE_COMPARE(db.vectorStride(), 8);
    CORRADE_COMPARE(cdb.vectorStride(), 8);
    CORRADE_COMPARE(db.vectors(), 3);
    CORRADE_COMPARE(cdb.vectors(), 3);
    CORRADE_COMPARE(db.dataType(), DynamicAttribute::DataType::Short);
    CORRADE_COMPARE(cdb.dataType(), DynamicAttribute::DataType::Short);
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::attributeMatrixMxN() {
    typedef Attribute<3, Matrix3x4> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Float>{});
    CORRADE_COMPARE(Attribute::Vectors, 3);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(ca.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.vectorStride(), 4*4);
    CORRADE_COMPARE(ca.vectorStride(), 4*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);
    CORRADE_COMPARE(ca.dataType(), Attribute::DataType::Float);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Generic);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(da.vectorStride(), 4*4);
    CORRADE_COMPARE(cda.vectorStride(), 4*4);
    CORRADE_COMPARE(da.vectors(), 3);
    CORRADE_COMPARE(cda.vectors(), 3);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Float);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Float);
}
#endif

void AttributeTest::attributeMatrixNxNd() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Matrix4d> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Double>{});
    CORRADE_COMPARE(Attribute::Vectors, 4);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(ca.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.vectorStride(), 4*8);
    CORRADE_COMPARE(ca.vectorStride(), 4*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);
    CORRADE_COMPARE(ca.dataType(), Attribute::DataType::Double);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::Four);
    CORRADE_COMPARE(da.vectorStride(), 4*8);
    CORRADE_COMPARE(cda.vectorStride(), 4*8);
    CORRADE_COMPARE(da.vectors(), 4);
    CORRADE_COMPARE(cda.vectors(), 4);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Double);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::attributeMatrixMxNd() {
    #ifndef MAGNUM_TARGET_GLES
    typedef Attribute<3, Matrix4x2d> Attribute;
    CORRADE_VERIFY(std::is_same<Attribute::ScalarType, Double>{});
    CORRADE_COMPARE(Attribute::Vectors, 4);

    /* Default constructor */
    Attribute a;
    constexpr Attribute ca;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(ca.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.vectorStride(), 2*8);
    CORRADE_COMPARE(ca.vectorStride(), 2*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);
    CORRADE_COMPARE(ca.dataType(), Attribute::DataType::Double);

    DynamicAttribute da{a};
    constexpr DynamicAttribute cda{ca};
    CORRADE_COMPARE(da.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(cda.kind(), DynamicAttribute::Kind::Long);
    CORRADE_COMPARE(da.location(), 3);
    CORRADE_COMPARE(cda.location(), 3);
    CORRADE_COMPARE(da.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(cda.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(da.vectorStride(), 2*8);
    CORRADE_COMPARE(cda.vectorStride(), 2*8);
    CORRADE_COMPARE(da.vectors(), 4);
    CORRADE_COMPARE(cda.vectors(), 4);
    CORRADE_COMPARE(da.dataType(), DynamicAttribute::DataType::Double);
    CORRADE_COMPARE(cda.dataType(), DynamicAttribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AttributeTest::dynamicAttribute() {
    /* This one isn't constexpr as the vector stride has to be calculated */
    DynamicAttribute a{DynamicAttribute::Kind::GenericNormalized, 3,
        DynamicAttribute::Components::Two, DynamicAttribute::DataType::Byte};
    CORRADE_COMPARE(a.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(a.location(), 3);
    CORRADE_COMPARE(a.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(a.vectors(), 1);
    CORRADE_COMPARE(a.vectorStride(), 2);
    CORRADE_COMPARE(a.dataType(), DynamicAttribute::DataType::Byte);
}

void AttributeTest::dynamicAttributeMatrix() {
    DynamicAttribute a{DynamicAttribute::Kind::GenericNormalized, 3,
        DynamicAttribute::Components::Two, 3, DynamicAttribute::DataType::Byte};
    CORRADE_COMPARE(a.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(a.location(), 3);
    CORRADE_COMPARE(a.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(a.vectors(), 3);
    CORRADE_COMPARE(a.vectorStride(), 2);
    CORRADE_COMPARE(a.dataType(), DynamicAttribute::DataType::Byte);

    DynamicAttribute b{DynamicAttribute::Kind::GenericNormalized, 3,
        DynamicAttribute::Components::Two, 3, 4, DynamicAttribute::DataType::Byte};
    CORRADE_COMPARE(b.kind(), DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(b.location(), 3);
    CORRADE_COMPARE(b.components(), DynamicAttribute::Components::Two);
    CORRADE_COMPARE(b.vectors(), 3);
    CORRADE_COMPARE(b.vectorStride(), 4);
    CORRADE_COMPARE(b.dataType(), DynamicAttribute::DataType::Byte);

    constexpr DynamicAttribute cb{DynamicAttribute::Kind::GenericNormalized, 3,
        DynamicAttribute::Components::Two, 3, 4, DynamicAttribute::DataType::Byte};
    constexpr DynamicAttribute::Kind kind = cb.kind();
    constexpr UnsignedInt location = cb.location();
    constexpr DynamicAttribute::Components components = cb.components();
    constexpr UnsignedInt vectors = cb.vectors();
    constexpr UnsignedInt vectorStride = cb.vectorStride();
    constexpr DynamicAttribute::DataType dataType = cb.dataType();
    CORRADE_COMPARE(kind, DynamicAttribute::Kind::GenericNormalized);
    CORRADE_COMPARE(location, 3);
    CORRADE_COMPARE(components, DynamicAttribute::Components::Two);
    CORRADE_COMPARE(vectors, 3);
    CORRADE_COMPARE(vectorStride, 4);
    CORRADE_COMPARE(dataType, DynamicAttribute::DataType::Byte);
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
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    DynamicAttribute{DynamicAttribute::Kind::GenericNormalized, 3,
        VertexFormat::Int};
    CORRADE_COMPARE(out,
        "GL::DynamicAttribute: can't use VertexFormat::Int for a normalized attribute\n");
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::attributeFromGenericFormatUnexpectedForIntegralKind() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    DynamicAttribute{DynamicAttribute::Kind::Integral, 3,
        VertexFormat::Vector2bNormalized};
    DynamicAttribute{DynamicAttribute::Kind::Integral, 3,
        VertexFormat::Vector3};
    CORRADE_COMPARE(out,
        "GL::DynamicAttribute: can't use VertexFormat::Vector2bNormalized for a GL::DynamicAttribute::Kind::Integral attribute\n"
        "GL::DynamicAttribute: can't use VertexFormat::Vector3 for an integral attribute\n");
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AttributeTest::attributeFromGenericFormatUnexpectedForLongKind() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    DynamicAttribute{DynamicAttribute::Kind::Long, 3,
        VertexFormat::UnsignedShortNormalized};
    CORRADE_COMPARE(out,
        "GL::DynamicAttribute: can't use VertexFormat::UnsignedShortNormalized for a GL::DynamicAttribute::Kind::Long attribute\n");
}
#endif

void AttributeTest::attributeFromGenericFormatTooManyVectors() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    DynamicAttribute{Attribute<7, Vector2>{}, VertexFormat::Matrix2x2};
    CORRADE_COMPARE(out,
        "GL::DynamicAttribute: can't use VertexFormat::Matrix2x2 for a 1-vector attribute\n");
}

void AttributeTest::attributeFromGenericFormatTooManyComponents() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    DynamicAttribute{Attribute<7, Vector2>{}, VertexFormat::Vector3};
    CORRADE_COMPARE(out,
        "GL::DynamicAttribute: can't use VertexFormat::Vector3 for a 2-component attribute\n");
}

void AttributeTest::attributeFromGenericFormatNotAvailable() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_SKIP("All attribute formats available on desktop GL.");
    #else
    Containers::String out;
    Error redirectError{&out};
    DynamicAttribute{Attribute<7, Vector2>{}, VertexFormat::Vector3d};
    CORRADE_COMPARE(out,
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
        #ifdef CORRADE_TARGET_GCC
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
        #ifdef CORRADE_TARGET_GCC
        #pragma GCC diagnostic pop
        #endif
    }
}

void AttributeTest::debugComponents1() {
    typedef Attribute<3, Float> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::Components::One << Attribute::Components(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::Components::One GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugComponents2() {
    typedef Attribute<3, Vector2> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::Components::Two << Attribute::Components(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::Components::Two GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugComponents3() {
    typedef Attribute<3, Vector3> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::Components::Three << Attribute::Components(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::Components::Three GL::Attribute::Components(0xdead)\n");
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::debugComponents4() {
    typedef Attribute<3, Vector4i> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::Components::Four << Attribute::Components(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::Components::Four GL::Attribute::Components(0xdead)\n");
}
#endif

void AttributeTest::debugComponentsMatrix2() {
    typedef Attribute<3, Matrix2x2> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::Components::Two << Attribute::Components(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::Components::Two GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugComponentsMatrix3() {
    typedef Attribute<3, Matrix3> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::Components::Three << Attribute::Components(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::Components::Three GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugComponentsMatrix4() {
    typedef Attribute<3, Matrix4> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::Components::Four << Attribute::Components(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::Components::Four GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugComponentsVector4() {
    typedef Attribute<3, Vector4> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::Components::Three << Attribute::Components(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::Components::Three GL::Attribute::Components(0xdead)\n");
}

void AttributeTest::debugDataTypeFloat() {
    typedef Attribute<3, Float> Attribute;

    Containers::String out;
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    Debug{&out} << Attribute::DataType::Half << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::DataType::Half GL::Attribute::DataType(0xdead)\n");
    #else
    Debug{&out} << Attribute::DataType::Float << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::DataType::Float GL::Attribute::DataType(0xdead)\n");
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void AttributeTest::debugDataTypeInt() {
    typedef Attribute<3, Int> Attribute;

    {
        Containers::String out;
        Debug{&out} << Attribute::DataType::Short << Attribute::DataType(0xdead);
        CORRADE_COMPARE(out, "GL::Attribute::DataType::Short GL::Attribute::DataType(0xdead)\n");
    }

    /* Unsigned types for signed attributes are not supported on WebGL, exposed
       just as deprecated for backwards compatibility */
    #if !defined(MAGNUM_TARGET_WEBGL) || defined(MAGNUM_BUILD_DEPRECATED)
    {
        Containers::String out;
        #ifdef MAGNUM_TARGET_WEBGL
        CORRADE_IGNORE_DEPRECATED_PUSH
        #endif
        Debug{&out} << Attribute::DataType::UnsignedInt << Attribute::DataType::UnsignedByte;
        #ifdef MAGNUM_TARGET_WEBGL
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
        CORRADE_COMPARE(out, "GL::Attribute::DataType::UnsignedInt GL::Attribute::DataType::UnsignedByte\n");
    }
    #endif
}

void AttributeTest::debugDataTypeUnsignedInt() {
    typedef Attribute<3, UnsignedInt> Attribute;

    {
        Containers::String out;
        Debug{&out} << Attribute::DataType::UnsignedShort << Attribute::DataType(0xdead);
        CORRADE_COMPARE(out, "GL::Attribute::DataType::UnsignedShort GL::Attribute::DataType(0xdead)\n");
    }

    /* Signed types for unsigned attributes are not supported on WebGL, exposed
       just as deprecated for backwards compatibility */
    #if !defined(MAGNUM_TARGET_WEBGL) || defined(MAGNUM_BUILD_DEPRECATED)
    {
        Containers::String out;
        #ifdef MAGNUM_TARGET_WEBGL
        CORRADE_IGNORE_DEPRECATED_PUSH
        #endif
        Debug{&out} << Attribute::DataType::Int << Attribute::DataType::Byte;
        #ifdef MAGNUM_TARGET_WEBGL
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
        CORRADE_COMPARE(out, "GL::Attribute::DataType::Int GL::Attribute::DataType::Byte\n");
    }
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AttributeTest::debugDataTypeDouble() {
    typedef Attribute<3, Double> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::DataType::Double << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::DataType::Double GL::Attribute::DataType(0xdead)\n");
}
#endif

void AttributeTest::debugDataTypeVector3() {
    typedef Attribute<3, Vector3> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::DataType::Float << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::DataType::Float GL::Attribute::DataType(0xdead)\n");
}

void AttributeTest::debugDataTypeVector4() {
    typedef Attribute<3, Vector4> Attribute;

    Containers::String out;
    Debug{&out} << Attribute::DataType::Float << Attribute::DataType(0xdead);
    CORRADE_COMPARE(out, "GL::Attribute::DataType::Float GL::Attribute::DataType(0xdead)\n");
}

void AttributeTest::debugDynamicKind() {
    Containers::String out;
    Debug{&out} << DynamicAttribute::Kind::GenericNormalized << DynamicAttribute::Kind(0xfe);
    CORRADE_COMPARE(out, "GL::DynamicAttribute::Kind::GenericNormalized GL::DynamicAttribute::Kind(0xfe)\n");
}

void AttributeTest::debugDynamicComponents() {
    Containers::String out;
    Debug{&out} << DynamicAttribute::Components::Three << DynamicAttribute::Components(0xdead);
    CORRADE_COMPARE(out, "GL::DynamicAttribute::Components::Three GL::DynamicAttribute::Components(0xdead)\n");
}

void AttributeTest::debugDynamicDataType() {
    Containers::String out;
    Debug{&out} << DynamicAttribute::DataType::Float << DynamicAttribute::DataType(0xdead);
    CORRADE_COMPARE(out, "GL::DynamicAttribute::DataType::Float GL::DynamicAttribute::DataType(0xdead)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::AttributeTest)
