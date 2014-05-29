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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/BoolVector.h"

namespace Magnum { namespace Math { namespace Test {

class BoolVectorTest: public Corrade::TestSuite::Tester {
    public:
        explicit BoolVectorTest();

        void construct();
        void constructDefault();
        void constructOneValue();
        void constructOneElement();
        void constructCopy();
        void data();

        void compare();
        void compareUndefined();
        void all();
        void none();
        void any();

        void bitInverse();
        void bitAndOrXor();

        void debug();
};

static_assert(BoolVector<15>::DataSize == 2, "Improper DataSize");
static_assert(BoolVector<16>::DataSize == 2, "Improper DataSize");
static_assert(BoolVector<17>::DataSize == 3, "Improper DataSize");

typedef Math::BoolVector<19> BoolVector19;

BoolVectorTest::BoolVectorTest() {
    addTests({&BoolVectorTest::construct,
              &BoolVectorTest::constructDefault,
              &BoolVectorTest::constructOneValue,
              &BoolVectorTest::constructOneElement,
              &BoolVectorTest::constructCopy,
              &BoolVectorTest::data,

              &BoolVectorTest::compare,
              &BoolVectorTest::compareUndefined,
              &BoolVectorTest::all,
              &BoolVectorTest::none,
              &BoolVectorTest::any,

              &BoolVectorTest::bitInverse,
              &BoolVectorTest::bitAndOrXor,

              &BoolVectorTest::debug});
}

void BoolVectorTest::construct() {
    constexpr BoolVector19 a = {0xa5, 0x5f, 0x07};
    CORRADE_COMPARE(a, BoolVector19(0xa5, 0x5f, 0x07));
}

void BoolVectorTest::constructDefault() {
    constexpr BoolVector19 a;
    CORRADE_COMPARE(a, BoolVector19(0x00, 0x00, 0x00));
}

void BoolVectorTest::constructOneValue() {
    constexpr BoolVector19 a(false);
    CORRADE_COMPARE(a, BoolVector19(0x00, 0x00, 0x00));

    constexpr BoolVector19 b(true);
    CORRADE_COMPARE(b, BoolVector19(0xff, 0xff, 0x07));

    CORRADE_VERIFY(!(std::is_convertible<bool, BoolVector19>::value));
}

void BoolVectorTest::constructOneElement() {
    typedef BoolVector<1> BoolVector1;

    constexpr BoolVector1 a = 0x01;
    CORRADE_COMPARE(a, BoolVector1(0x01));
}

void BoolVectorTest::constructCopy() {
    constexpr BoolVector19 a = {0xa5, 0x5f, 0x07};
    constexpr BoolVector19 b(a);
    CORRADE_COMPARE(b, BoolVector19(0xa5, 0x5f, 0x07));
}

void BoolVectorTest::data() {
    constexpr BoolVector19 a(0x08, 0x03, 0x04);

    CORRADE_VERIFY(!a[0] && !a[1] && !a[2]);
    CORRADE_VERIFY(a[3]);
    CORRADE_VERIFY(!a[4] && !a[5] && !a[6] && !a[7]);
    CORRADE_VERIFY(a[8]);
    CORRADE_VERIFY(a[9]);
    CORRADE_VERIFY(!a[10] && !a[11] && !a[12] && !a[13] && !a[14] && !a[15] && !a[16] && !a[17]);
    CORRADE_VERIFY(a[18]);

    constexpr bool b = a[9];
    CORRADE_COMPARE(b, true);

    constexpr UnsignedByte c = *a.data();
    CORRADE_COMPARE(c, 0x08);

    BoolVector19 d(0x08, 0x03, 0x04);
    d.set(15, true);
    CORRADE_VERIFY(d[15]);
    CORRADE_COMPARE(d, BoolVector19(0x08, 0x83, 0x04));
}

void BoolVectorTest::compare() {
    constexpr BoolVector19 a(0xa5, 0x5f, 0x07);
    constexpr auto b = a == a;
    CORRADE_VERIFY(b);

    /* Change in full segments */
    constexpr BoolVector19 c(0xa3, 0x5f, 0x07);
    constexpr BoolVector19 d(0xa5, 0x98, 0x07);
    constexpr auto e = a != c;
    constexpr auto f = a != d;
    CORRADE_VERIFY(e);
    CORRADE_VERIFY(f);

    /* Change in last bit */
    constexpr BoolVector19 g(0xa5, 0x5f, 0x06);
    constexpr auto h = a != g;
    CORRADE_VERIFY(h);
}

void BoolVectorTest::compareUndefined() {
    constexpr BoolVector19 a(0xa5, 0x5f, 0x07);

    /* Change in unused part of last segment */
    constexpr BoolVector19 b(0xa5, 0x5f, 0x0f);
    constexpr auto c = a == b;
    CORRADE_VERIFY(c);

    /* Change in used part of last segment */
    constexpr BoolVector19 d(0xa5, 0x5f, 0x03);
    constexpr auto e = a != d;
    CORRADE_VERIFY(e);
}

void BoolVectorTest::all() {
    constexpr auto a = BoolVector19(0xff, 0xff, 0x07).all();
    CORRADE_VERIFY(a);

    /* Last segment - bit in used and unused part */
    constexpr auto b = BoolVector19(0xff, 0xff, 0x0f).all();
    constexpr auto c = BoolVector19(0xff, 0xff, 0x04).all();
    CORRADE_VERIFY(b);
    CORRADE_VERIFY(!c);
}

void BoolVectorTest::none() {
    constexpr auto a = BoolVector19(0x00, 0x00, 0x00).none();
    CORRADE_VERIFY(a);

    /* Last segment - bit in used and unused part */
    constexpr auto b = BoolVector19(0x00, 0x00, 0x08).none();
    constexpr auto c = BoolVector19(0x00, 0x00, 0x04).none();
    CORRADE_VERIFY(b);
    CORRADE_VERIFY(!c);
}

void BoolVectorTest::any() {
    constexpr auto a = BoolVector19(0x00, 0x01, 0x00).any();
    CORRADE_VERIFY(a);

    /* Last segment - bit in used and unused part */
    constexpr auto b = BoolVector19(0x00, 0x00, 0x04).any();
    constexpr auto c = BoolVector19(0x00, 0x00, 0x08).any();
    CORRADE_VERIFY(b);
    CORRADE_VERIFY(!c);
}

void BoolVectorTest::bitInverse() {
    constexpr auto a = ~BoolVector19(0xa5, 0x5f, 0x03);
    CORRADE_COMPARE(a, BoolVector19(0x5a, 0xa0, 0x04));
}

void BoolVectorTest::bitAndOrXor() {
    constexpr BoolVector19 a(0xa5, 0x5f, 0x03);
    constexpr BoolVector19 b(0x37, 0xf3, 0x06);

    constexpr auto c = a & b;
    constexpr auto d = a | b;
    constexpr auto e = a ^ b;

    CORRADE_COMPARE(c, BoolVector19(0x25, 0x53, 0x02));
    CORRADE_COMPARE(d, BoolVector19(0xb7, 0xff, 0x07));
    CORRADE_COMPARE(e, BoolVector19(0x92, 0xac, 0x05));
}

void BoolVectorTest::debug() {
    std::ostringstream o;

    Debug(&o) << BoolVector19(0x25, 0x53, 0x02);

    CORRADE_COMPARE(o.str(), "BoolVector(10100100 11001010 010)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::BoolVectorTest)
