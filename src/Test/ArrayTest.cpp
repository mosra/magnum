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

#include "Array.h"

namespace Magnum { namespace Test {

class ArrayTest: public Corrade::TestSuite::Tester {
    public:
        ArrayTest();

        void construct();
        void constexprConstruct();
        void equality();
        void access();
};

typedef Magnum::Array1D<int> Array1D;
typedef Magnum::Array2D<int> Array2D;
typedef Magnum::Array3D<int> Array3D;

ArrayTest::ArrayTest() {
    addTests(&ArrayTest::construct,
             &ArrayTest::constexprConstruct,
             &ArrayTest::equality,
             &ArrayTest::access);
}

void ArrayTest::construct() {
    CORRADE_COMPARE(Array1D(5), (Array<1, int>(5)));
    CORRADE_COMPARE(Array2D(5, 3), (Array<2, int>(5, 3)));
    CORRADE_COMPARE(Array3D(5, 3, -2), (Array<3, int>(5, 3, -2)));

    /* Verify proper expansion */
    CORRADE_COMPARE((Array<3, int>(5)), (Array<3, int>(5, 5, 5)));
    CORRADE_COMPARE(Array2D(5), (Array<2, int>(5, 5)));
    CORRADE_COMPARE(Array3D(5), (Array<3, int>(5, 5, 5)));
}

void ArrayTest::constexprConstruct() {
    /* Verify that all full constructors can be called as constexpr */
    constexpr Array1D a(5);
    constexpr Array2D b(5, 3);
    constexpr Array2D b2(5);
    constexpr Array3D c(5, 6, 7);
    constexpr Array3D c2(5);
    constexpr Array<3, int> d(5, 6, 7);

    CORRADE_COMPARE(a, Array1D(5));
    CORRADE_COMPARE(b, Array2D(5, 3));
    CORRADE_COMPARE(b2, Array2D(5));
    CORRADE_COMPARE(c, Array3D(5, 6, 7));
    CORRADE_COMPARE(c2, Array3D(5));
    CORRADE_COMPARE(d, (Array<3, int>(5, 6, 7)));
}

void ArrayTest::equality() {
    CORRADE_VERIFY((Array<3, int>(5, 6, 7) == Array<3, int>(5, 6, 7)));
    CORRADE_VERIFY((Array<3, int>(5, 6, 7) != Array<3, int>(5, 6, 8)));
}

void ArrayTest::access() {
    Array1D a(50);
    const Array1D ac(50);
    Array2D b(5, 3);
    const Array2D bc(5, 3);
    Array3D c(-5, 6, 7);
    const Array3D cc(-5, 6, 7);

    CORRADE_COMPARE(a.x(), 50);
    CORRADE_COMPARE(ac.x(), 50);

    CORRADE_COMPARE(b.x(), 5);
    CORRADE_COMPARE(b.y(), 3);
    CORRADE_COMPARE(bc.x(), 5);
    CORRADE_COMPARE(bc.y(), 3);

    CORRADE_COMPARE(c.x(), -5);
    CORRADE_COMPARE(c.y(), 6);
    CORRADE_COMPARE(c.z(), 7);
    CORRADE_COMPARE(cc.x(), -5);
    CORRADE_COMPARE(cc.y(), 6);
    CORRADE_COMPARE(cc.z(), 7);
}

}}

CORRADE_TEST_MAIN(Magnum::Test::ArrayTest)
