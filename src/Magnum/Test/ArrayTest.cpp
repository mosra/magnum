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

#include "Magnum/Array.h"

namespace Magnum { namespace Test { namespace {

struct ArrayTest: TestSuite::Tester {
    explicit ArrayTest();

    void construct();
    void equality();
    void access();

    void debug();
};

typedef Magnum::Array1D<Int> Array1D;
typedef Magnum::Array2D<Int> Array2D;
typedef Magnum::Array3D<Int> Array3D;

ArrayTest::ArrayTest() {
    addTests({&ArrayTest::construct,
              &ArrayTest::equality,
              &ArrayTest::access,

              &ArrayTest::debug});
}

void ArrayTest::construct() {
    constexpr Array<3, Int> a = {5, 6, 7};
    CORRADE_COMPARE(a, (Array<3, Int>(5, 6, 7)));

    constexpr Array<3, Int> a2 = 5;
    CORRADE_COMPARE(a2, (Array<3, Int>(5, 5, 5)));

    constexpr Array1D b = 5;
    CORRADE_COMPARE(b, (Array<1, Int>(5)));

    constexpr Array2D c = {5, 3};
    CORRADE_COMPARE(c, (Array<2, Int>(5, 3)));

    constexpr Array2D c2 = 5;
    CORRADE_COMPARE(c2, (Array<2, Int>(5, 5)));

    constexpr Array3D d = {5, 3, -2};
    CORRADE_COMPARE(d, (Array<3, Int>(5, 3, -2)));

    constexpr Array3D d2 = 5;
    CORRADE_COMPARE(d2, (Array<3, Int>(5, 5, 5)));
}

void ArrayTest::equality() {
    CORRADE_VERIFY((Array<3, Int>(5, 6, 7) == Array<3, Int>(5, 6, 7)));
    CORRADE_VERIFY((Array<3, Int>(5, 6, 7) != Array<3, Int>(5, 6, 8)));
}

void ArrayTest::access() {
    Array1D a(50);
    constexpr Array1D ac(50);
    Array2D b(5, 3);
    constexpr Array2D bc(5, 3);
    Array3D c(-5, 6, 7);
    constexpr Array3D cc(-5, 6, 7);

    CORRADE_COMPARE(a[0], 50);
    CORRADE_COMPARE(ac[0], 50);
    CORRADE_COMPARE(b[1], 3);
    CORRADE_COMPARE(bc[1], 3);
    CORRADE_COMPARE(c[2], 7);
    CORRADE_COMPARE(cc[2], 7);

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

    CORRADE_COMPARE(c.xy(), Array2D(-5, 6));
    CORRADE_COMPARE(cc.xy(), Array2D(-5, 6));
}

void ArrayTest::debug() {
    std::ostringstream out;

    Debug{&out} << Array<4, Int>{5, 6, 7, 8} << Array1D{13} << Array2D{71, 2} << Array3D{1, 2, 3};
    CORRADE_COMPARE(out.str(), "Array(5, 6, 7, 8) Array(13) Array(71, 2) Array(1, 2, 3)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::ArrayTest)
