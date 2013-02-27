/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <TestSuite/Tester.h>

#include "AbstractShaderProgram.h"

namespace Magnum { namespace Test {

class AbstractShaderProgramTest: public Corrade::TestSuite::Tester {
    public:
        explicit AbstractShaderProgramTest();

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

        void attributeMatrix();
        void attributeMatrixDouble();
};

AbstractShaderProgramTest::AbstractShaderProgramTest() {
    addTests(&AbstractShaderProgramTest::attributeScalar,
             &AbstractShaderProgramTest::attributeScalarInt,
             &AbstractShaderProgramTest::attributeScalarUnsignedInt,
             &AbstractShaderProgramTest::attributeScalarDouble,

             &AbstractShaderProgramTest::attributeVector,
             &AbstractShaderProgramTest::attributeVectorInt,
             &AbstractShaderProgramTest::attributeVectorUnsignedInt,
             &AbstractShaderProgramTest::attributeVectorDouble,
             &AbstractShaderProgramTest::attributeVector4,
             &AbstractShaderProgramTest::attributeVectorBGRA,

             &AbstractShaderProgramTest::attributeMatrix,
             &AbstractShaderProgramTest::attributeMatrixDouble);
}

void AbstractShaderProgramTest::attributeScalar() {
    typedef AbstractShaderProgram::Attribute<3, Float> Attribute;
    CORRADE_COMPARE(Attribute::Location, 3);

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::One);
    CORRADE_VERIFY(!a.dataOptions());
    CORRADE_COMPARE(a.dataSize(), 4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);

    /* Options */
    Attribute b(Attribute::DataType::UnsignedShort, Attribute::DataOption::Normalized);
    CORRADE_COMPARE(b.dataSize(), 2);
    CORRADE_VERIFY(b.dataOptions() <= Attribute::DataOption::Normalized);
}

void AbstractShaderProgramTest::attributeScalarInt() {
    typedef AbstractShaderProgram::Attribute<3, Int> Attribute;

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.dataSize(), 4);

    /* Options */
    Attribute b(Attribute::DataType::Short);
    CORRADE_COMPARE(b.dataSize(), 2);
}

void AbstractShaderProgramTest::attributeScalarUnsignedInt() {
    typedef AbstractShaderProgram::Attribute<3, UnsignedInt> Attribute;

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.dataSize(), 4);

    /* Options */
    Attribute b(Attribute::DataType::UnsignedByte);
    CORRADE_COMPARE(b.dataSize(), 1);
}

void AbstractShaderProgramTest::attributeScalarDouble() {
    #ifndef MAGNUM_TARGET_GLES
    typedef AbstractShaderProgram::Attribute<3, Double> Attribute;

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.dataSize(), 8);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AbstractShaderProgramTest::attributeVector() {
    typedef AbstractShaderProgram::Attribute<3, Vector3> Attribute;

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Three);
    CORRADE_COMPARE(a.dataSize(), 3*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);

    /* Options */
    Attribute b(Attribute::Components::Two, Attribute::DataType::Double);
    CORRADE_COMPARE(b.components(), Attribute::Components::Two);
    CORRADE_COMPARE(b.dataSize(), 2*8);
}

void AbstractShaderProgramTest::attributeVectorInt() {
    typedef AbstractShaderProgram::Attribute<3, Vector2i> Attribute;

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.dataSize(), 2*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Int);

    /* Options */
    Attribute b(Attribute::Components::One, Attribute::DataType::Int);
    CORRADE_COMPARE(b.dataSize(), 4);
}

void AbstractShaderProgramTest::attributeVectorUnsignedInt() {
    typedef AbstractShaderProgram::Attribute<3, Vector4ui> Attribute;

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.dataSize(), 4*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::UnsignedInt);

    /* Options */
    Attribute b(Attribute::Components::Three, Attribute::DataType::UnsignedShort);
    CORRADE_COMPARE(b.dataSize(), 3*2);
}

void AbstractShaderProgramTest::attributeVectorDouble() {
    #ifndef MAGNUM_TARGET_GLES
    typedef AbstractShaderProgram::Attribute<3, Vector2d> Attribute;

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Two);
    CORRADE_COMPARE(a.dataSize(), 2*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);

    /* Options */
    Attribute b(Attribute::Components::One);
    CORRADE_COMPARE(b.dataSize(), 8);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

void AbstractShaderProgramTest::attributeVector4() {
    typedef AbstractShaderProgram::Attribute<3, Vector4> Attribute;

    /* Custom type */
    Attribute a(Attribute::DataType::UnsignedInt2101010Rev);
    CORRADE_COMPARE(a.dataSize(), 4);
}

void AbstractShaderProgramTest::attributeVectorBGRA() {
    #ifndef MAGNUM_TARGET_GLES
    typedef AbstractShaderProgram::Attribute<3, Vector4> Attribute;

    /* BGRA */
    Attribute a(Attribute::Components::BGRA);
    CORRADE_COMPARE(a.dataSize(), 4*4);
    #else
    CORRADE_SKIP("BGRA attribute component ordering is not available in OpenGL ES.");
    #endif
}

void AbstractShaderProgramTest::attributeMatrix() {
    typedef AbstractShaderProgram::Attribute<3, Matrix3> Attribute;

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Three);
    CORRADE_COMPARE(a.dataSize(), 3*3*4);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Float);
}

void AbstractShaderProgramTest::attributeMatrixDouble() {
    #ifndef MAGNUM_TARGET_GLES
    typedef AbstractShaderProgram::Attribute<3, Matrix4d> Attribute;

    /* Default constructor */
    Attribute a;
    CORRADE_COMPARE(a.components(), Attribute::Components::Four);
    CORRADE_COMPARE(a.dataSize(), 4*4*8);
    CORRADE_COMPARE(a.dataType(), Attribute::DataType::Double);
    #else
    CORRADE_SKIP("Double attributes are not available in OpenGL ES.");
    #endif
}

}}

CORRADE_TEST_MAIN(Magnum::Test::AbstractShaderProgramTest)
