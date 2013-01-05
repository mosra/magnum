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

#include "Math/Algorithms/GramSchmidt.h"

namespace Magnum { namespace Math { namespace Algorithms { namespace Test {

class GramSchmidtTest: public Corrade::TestSuite::Tester {
    public:
        GramSchmidtTest();

        void test();
};

typedef Matrix<3, float> Matrix3;
typedef Vector<3, float> Vector3;

GramSchmidtTest::GramSchmidtTest() {
    addTests(&GramSchmidtTest::test);
}

void GramSchmidtTest::test() {
    Matrix3 m(
        3.0f, 5.0f, 8.0f,
        4.0f, 4.0f, 7.0f,
        7.0f, -1.0f, 8.0f
    );

    Matrix3 normalized = Algorithms::gramSchmidt(m);

    /* Verify the first vector is in direction of first original */
    CORRADE_COMPARE(normalized[0], m[0].normalized());

    /* Verify the vectors have unit length */
    CORRADE_COMPARE(normalized[0].length(), 1.0f);
    CORRADE_COMPARE(normalized[1].length(), 1.0f);
    CORRADE_COMPARE(normalized[2].length(), 1.0f);

    /* Verify the vectors are orthogonal */
    CORRADE_COMPARE(Vector3::dot(normalized[0], normalized[1]), 0.0f);
    CORRADE_COMPARE(Vector3::dot(normalized[0], normalized[2]), 0.0f);
    CORRADE_COMPARE(Vector3::dot(normalized[1], normalized[2]), 0.0f);

    /* Just to be sure */
    Matrix3 expected( 0.303046f,  0.505076f,  0.808122f,
                      0.928316f, -0.348119f, -0.130544f,
                     -0.215388f, -0.789754f,  0.574367f);
    CORRADE_COMPARE(normalized, expected);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Algorithms::Test::GramSchmidtTest)
