/*
    Copyright © 1910, 1911, 1912 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <sstream>
#include <TestSuite/Tester.h>

#include "Math/BoolVector.h"

namespace Magnum { namespace Math { namespace Test {

class BoolVectorTest: public Corrade::TestSuite::Tester {
    public:
        explicit BoolVectorTest();

        void constructDefault();
        void constructOneValue();
        void constructOneElement();
        void data();

        void constExpressions();

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
    addTests({&BoolVectorTest::constructDefault,
              &BoolVectorTest::constructOneValue,
              &BoolVectorTest::constructOneElement,
              &BoolVectorTest::data,

              &BoolVectorTest::constExpressions,

              &BoolVectorTest::compare,
              &BoolVectorTest::compareUndefined,
              &BoolVectorTest::all,
              &BoolVectorTest::none,
              &BoolVectorTest::any,

              &BoolVectorTest::bitInverse,
              &BoolVectorTest::bitAndOrXor,

              &BoolVectorTest::debug});
}

void BoolVectorTest::constructDefault() {
    CORRADE_COMPARE(BoolVector19(), BoolVector19(0x00, 0x00, 0x00));
}

void BoolVectorTest::constructOneValue() {
    CORRADE_COMPARE(BoolVector19(false), BoolVector19(0x00, 0x00, 0x00));
    CORRADE_COMPARE(BoolVector19(true), BoolVector19(0xff, 0xff, 0x07));
}

void BoolVectorTest::constructOneElement() {
    typedef BoolVector<1> BoolVector1;

    BoolVector1 a = 0x01;
    CORRADE_COMPARE(a, BoolVector1(0x01));
}

void BoolVectorTest::data() {
    BoolVector19 a(0x08, 0x03, 0x04);

    CORRADE_VERIFY(!a[0] && !a[1] && !a[2]);
    CORRADE_VERIFY(a[3]);
    CORRADE_VERIFY(!a[4] && !a[5] && !a[6] && !a[7]);
    CORRADE_VERIFY(a[8]);
    CORRADE_VERIFY(a[9]);
    CORRADE_VERIFY(!a[10] && !a[11] && !a[12] && !a[13] && !a[14] && !a[15] && !a[16] && !a[17]);
    CORRADE_VERIFY(a[18]);

    a.set(15, true);
    CORRADE_VERIFY(a[15]);

    CORRADE_COMPARE(a, BoolVector19(0x08, 0x83, 0x04));
}

void BoolVectorTest::constExpressions() {
    /* Default constructor */
    constexpr BoolVector19 a;
    CORRADE_COMPARE(a, BoolVector19(0x00, 0x00, 0x00));

    /* Value constructor */
    constexpr BoolVector19 b(0xa5, 0x5f, 0x07);
    CORRADE_COMPARE(b, BoolVector19(0xa5, 0x5f, 0x07));

    /* One-value constructor, not constexpr under GCC < 4.7 */
    #ifndef CORRADE_GCC46_COMPATIBILITY
    constexpr BoolVector19 c(true);
    CORRADE_COMPARE(c, BoolVector19(0xff, 0xff, 0x07));
    #endif

    /* Copy constructor */
    constexpr BoolVector19 d(b);
    CORRADE_COMPARE(d, BoolVector19(0xa5, 0x5f, 0x07));

    /* Data access, pointer chasings, i.e. *(b.data()[3]), are not possible */
    constexpr bool e = b[2];
    constexpr UnsignedByte f = *b.data();
    CORRADE_COMPARE(e, true);
    CORRADE_COMPARE(f, 0xa5);
}

void BoolVectorTest::compare() {
    BoolVector19 a(0xa5, 0x5f, 0x07);
    CORRADE_VERIFY(a == a);

    /* Change in full segments */
    BoolVector19 b(0xa3, 0x5f, 0x07);
    BoolVector19 c(0xa5, 0x98, 0x07);
    CORRADE_VERIFY(a != b);
    CORRADE_VERIFY(a != c);

    /* Change in last bit */
    BoolVector19 d(0xa5, 0x5f, 0x06);
    CORRADE_VERIFY(a != d);
}

void BoolVectorTest::compareUndefined() {
    BoolVector19 a(0xa5, 0x5f, 0x07);

    /* Change in unused part of last segment */
    BoolVector19 b(0xa5, 0x5f, 0x0f);
    CORRADE_VERIFY(a == b);

    /* Change in used part of last segment */
    BoolVector19 c(0xa5, 0x5f, 0x03);
    CORRADE_VERIFY(a != c);
}

void BoolVectorTest::all() {
    CORRADE_VERIFY(BoolVector19(0xff, 0xff, 0x07).all());

    /* Last segment - bit in used and unused part */
    CORRADE_VERIFY(BoolVector19(0xff, 0xff, 0x0f).all());
    CORRADE_VERIFY(!BoolVector19(0xff, 0xff, 0x04).all());
}

void BoolVectorTest::none() {
    CORRADE_VERIFY(BoolVector19(0x00, 0x00, 0x00).none());

    /* Last segment - bit in used and unused part */
    CORRADE_VERIFY(BoolVector19(0x00, 0x00, 0x08).none());
    CORRADE_VERIFY(!BoolVector19(0x00, 0x00, 0x04).none());
}

void BoolVectorTest::any() {
    CORRADE_VERIFY(BoolVector19(0x00, 0x01, 0x00).any());

    /* Last segment - bit in used and unused part */
    CORRADE_VERIFY(BoolVector19(0x00, 0x00, 0x04).any());
    CORRADE_VERIFY(!BoolVector19(0x00, 0x00, 0x08).any());
}

void BoolVectorTest::bitInverse() {
    CORRADE_COMPARE(~BoolVector19(0xa5, 0x5f, 0x03), BoolVector19(0x5a, 0xa0, 0x04));
}

void BoolVectorTest::bitAndOrXor() {
    BoolVector19 a(0xa5, 0x5f, 0x03);
    BoolVector19 b(0x37, 0xf3, 0x06);

    CORRADE_COMPARE(a & b, BoolVector19(0x25, 0x53, 0x02));
    CORRADE_COMPARE(a | b, BoolVector19(0xb7, 0xff, 0x07));
    CORRADE_COMPARE(a ^ b, BoolVector19(0x92, 0xac, 0x05));
}

void BoolVectorTest::debug() {
    std::ostringstream o;

    Debug(&o) << BoolVector19(0x25, 0x53, 0x02);

    CORRADE_COMPARE(o.str(), "BoolVector(10100100 11001010 010)\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::BoolVectorTest)
