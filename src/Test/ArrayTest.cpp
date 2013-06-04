/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <TestSuite/Tester.h>

#include "Array.h"

namespace Magnum { namespace Test {

class ArrayTest: public TestSuite::Tester {
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
    addTests({&ArrayTest::construct,
              &ArrayTest::constexprConstruct,
              &ArrayTest::equality,
              &ArrayTest::access});
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
