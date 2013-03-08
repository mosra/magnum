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

#include "Math/Algorithms/GaussJordan.h"

namespace Magnum { namespace Math { namespace Algorithms { namespace Test {

class GaussJordanTest: public Corrade::TestSuite::Tester {
    public:
        explicit GaussJordanTest();

        void singular();
        void invert();
};

typedef RectangularMatrix<4, 4, Float> Matrix4;
typedef Vector<4, Float> Vector4;

GaussJordanTest::GaussJordanTest() {
    addTests({&GaussJordanTest::singular,
              &GaussJordanTest::invert});
}

void GaussJordanTest::singular() {
    Matrix4 a(Vector4(1.0f, 2.0f,  3.0f,  4.0f),
              Vector4(2.0f, 3.0f, -7.0f, 11.0f),
              Vector4(2.0f, 4.0f,  6.0f,  8.0f),
              Vector4(1.0f, 2.0f,  7.0f, 40.0f));
    RectangularMatrix<4, 1, Float> t;

    CORRADE_VERIFY(!gaussJordanInPlaceTransposed(a, t));
}

void GaussJordanTest::invert() {
    Matrix4 a(Vector4(3.0f,  5.0f, 8.0f, 4.0f),
              Vector4(4.0f,  4.0f, 7.0f, 3.0f),
              Vector4(7.0f, -1.0f, 8.0f, 0.0f),
              Vector4(9.0f,  4.0f, 5.0f, 9.0f));

    Matrix4 expectedInverse(Vector4(-60/103.0f,   71/103.0f,  -4/103.0f,  3/103.0f),
                            Vector4(-66/103.0f,  109/103.0f, -25/103.0f, -7/103.0f),
                            Vector4(177/412.0f,  -97/206.0f,  53/412.0f, -7/206.0f),
                            Vector4(259/412.0f, -185/206.0f,  31/412.0f, 27/206.0f));

    Matrix4 a2(a);
    Matrix4 inverse = Matrix4::fromDiagonal(Vector4(1.0f));
    CORRADE_VERIFY(gaussJordanInPlace(a2, inverse));

    CORRADE_COMPARE(inverse, expectedInverse);
    CORRADE_COMPARE(a*inverse, Matrix4::fromDiagonal(Vector4(1.0f)));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Algorithms::Test::GaussJordanTest)
