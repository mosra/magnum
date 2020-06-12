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

#include "Magnum/Math/BoolVector.h"
#include "Magnum/Math/StrictWeakOrdering.h"

struct BVec3 {
    bool x, y, z;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct BoolVectorConverter<3, BVec3> {
    constexpr static BoolVector<3> from(const BVec3& other) {
        return (other.x << 0)|(other.y << 1)|(other.z << 2);
    }

    constexpr static BVec3 to(const BoolVector<3>& other) {
        return {other[0], other[1], other[2]};
    }
};

}

namespace Test { namespace {

struct BoolVectorTest: Corrade::TestSuite::Tester {
    explicit BoolVectorTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructOneValue();
    void constructOneElement();
    void constructCopy();
    void convert();

    void data();

    void compare();
    void compareUndefined();
    void convertBool();
    void all();
    void none();
    void any();

    void bitInverse();
    void bitAndOrXor();
    void booleanOperationEquivalents();

    void strictWeakOrdering();

    void debug();
};

static_assert(BoolVector<15>::DataSize == 2, "Improper DataSize");
static_assert(BoolVector<16>::DataSize == 2, "Improper DataSize");
static_assert(BoolVector<17>::DataSize == 3, "Improper DataSize");

typedef Math::BoolVector<19> BoolVector19;

BoolVectorTest::BoolVectorTest() {
    addTests({&BoolVectorTest::construct,
              &BoolVectorTest::constructDefault,
              &BoolVectorTest::constructNoInit,
              &BoolVectorTest::constructOneValue,
              &BoolVectorTest::constructOneElement,
              &BoolVectorTest::constructCopy,
              &BoolVectorTest::convert,

              &BoolVectorTest::data,

              &BoolVectorTest::compare,
              &BoolVectorTest::compareUndefined,
              &BoolVectorTest::convertBool,
              &BoolVectorTest::all,
              &BoolVectorTest::none,
              &BoolVectorTest::any,

              &BoolVectorTest::bitInverse,
              &BoolVectorTest::bitAndOrXor,
              &BoolVectorTest::booleanOperationEquivalents,

              &BoolVectorTest::strictWeakOrdering,

              &BoolVectorTest::debug});
}

void BoolVectorTest::construct() {
    constexpr BoolVector19 a = {0xa5, 0x5f, 0x07};
    CORRADE_COMPARE(a, BoolVector19(0xa5, 0x5f, 0x07));

    CORRADE_VERIFY((std::is_nothrow_constructible<BoolVector19, UnsignedByte, UnsignedByte, UnsignedByte>::value));
}

void BoolVectorTest::constructDefault() {
    constexpr BoolVector19 a;
    constexpr BoolVector19 b{ZeroInit};
    CORRADE_COMPARE(a, BoolVector19(0x00, 0x00, 0x00));
    CORRADE_COMPARE(b, BoolVector19(0x00, 0x00, 0x00));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<BoolVector19>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<BoolVector19, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, BoolVector19>::value));
}

void BoolVectorTest::constructNoInit() {
    BoolVector19 a{0xa5, 0x5f, 0x07};
    new(&a) BoolVector19{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, BoolVector19(0xa5, 0x5f, 0x07));
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<BoolVector19, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, BoolVector19>::value));
}

void BoolVectorTest::constructOneValue() {
    constexpr BoolVector19 a(false);
    CORRADE_COMPARE(a, BoolVector19(0x00, 0x00, 0x00));

    constexpr BoolVector19 b(true);
    CORRADE_COMPARE(b, BoolVector19(0xff, 0xff, 0x07));

    CORRADE_VERIFY(!(std::is_convertible<bool, BoolVector19>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<BoolVector19, bool>::value));
}

void BoolVectorTest::constructOneElement() {
    typedef BoolVector<1> BoolVector1;

    constexpr BoolVector1 a = 0x01;
    CORRADE_COMPARE(a, BoolVector1(0x01));

    CORRADE_VERIFY((std::is_nothrow_constructible<BoolVector1, UnsignedByte>::value));
}

void BoolVectorTest::constructCopy() {
    constexpr BoolVector19 a = {0xa5, 0x5f, 0x07};
    constexpr BoolVector19 b(a);
    CORRADE_COMPARE(b, BoolVector19(0xa5, 0x5f, 0x07));

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<BoolVector19>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<BoolVector19>::value);
}

void BoolVectorTest::convert() {
    constexpr BVec3 a{false, true, true};
    constexpr BoolVector<3> b{0x6};

    constexpr BoolVector<3> c{a};
    CORRADE_COMPARE(c, b);

    constexpr BVec3 d(b);
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<BVec3, BoolVector<3>>::value));
    CORRADE_VERIFY(!(std::is_convertible<BoolVector<3>, BVec3>::value));
}

void BoolVectorTest::data() {
    /* 0b00001000, 0b00000011, 0b100 */
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

    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Apparently dereferencing pointer is verboten */
    constexpr
    #endif
    UnsignedByte c = *a.data();
    CORRADE_COMPARE(c, 0x08);

    BoolVector19 d(0x08, 0x03, 0x04);
    d.set(15, true);
    CORRADE_VERIFY(d[15]);
    CORRADE_COMPARE(d, BoolVector19(0x08, 0x83, 0x04));
    d.set(15, false);
    CORRADE_VERIFY(!d[15]);
    CORRADE_COMPARE(d, BoolVector19(0x08, 0x03, 0x04));
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

void BoolVectorTest::convertBool() {
    CORRADE_VERIFY(BoolVector19(0xff, 0xff, 0x07));
    CORRADE_VERIFY(!bool(BoolVector19(0xff, 0xff, 0x04)));
    CORRADE_VERIFY(!bool(BoolVector19(0x00, 0x00, 0x00)));
    CORRADE_VERIFY(BoolVector19(0xff, 0xff, 0xff));

    /* Using ! before and after bool conversion will produce a different
       result -- first is equivalent to !a.all(), while second is (~a).all() */
    CORRADE_COMPARE(!bool(BoolVector19(0xff, 0xff, 0x04)), true);
    CORRADE_COMPARE(bool(!BoolVector19(0xff, 0xff, 0x04)), false);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<BoolVector19, bool>::value));
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
    CORRADE_COMPARE(!BoolVector19(0xa5, 0x5f, 0x03), BoolVector19(0x5a, 0xa0, 0x04));
}

void BoolVectorTest::bitAndOrXor() {
    BoolVector19 a(0xa5, 0x5f, 0x03);
    BoolVector19 b(0x37, 0xf3, 0x06);

    CORRADE_COMPARE(a & b, BoolVector19(0x25, 0x53, 0x02));
    CORRADE_COMPARE(a && b, BoolVector19(0x25, 0x53, 0x02));

    CORRADE_COMPARE(a | b, BoolVector19(0xb7, 0xff, 0x07));
    CORRADE_COMPARE(a || b, BoolVector19(0xb7, 0xff, 0x07));

    CORRADE_COMPARE(a ^ b, BoolVector19(0x92, 0xac, 0x05));
}

void BoolVectorTest::booleanOperationEquivalents() {
    Math::BoolVector<2> a{0x3};
    Math::BoolVector<2> b{0x2};

    CORRADE_COMPARE(!(a || b), !a && !b);
    CORRADE_COMPARE(!(a || b), ~(a | b));
    CORRADE_COMPARE(!a && !b, ~a & ~b);
}

void BoolVectorTest::strictWeakOrdering() {
    BoolVector<11> a, b, c;

    a.set(0, true);
    a.set(1, true);

    c.set(7, true);

    b.set(8, true);

    StrictWeakOrdering o;
    CORRADE_VERIFY( o(b, a));
    CORRADE_VERIFY(!o(a, b));
    CORRADE_VERIFY(!o(c, b));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));

    CORRADE_VERIFY(!o(a, a));

    /* Check uninitialized padding reads */
    a.set(8, true);
    a.set(10, true);
    b = a;
    a.data()[1] |= 0x08;
    b.data()[1] |= 0x20;
    a.data()[1] |= 0x40;
    b.data()[1] |= 0x80;

    CORRADE_VERIFY(!o(a, b));
    CORRADE_VERIFY(!o(b, a));
}

void BoolVectorTest::debug() {
    std::ostringstream o;

    /* 0b00100101 0b01010011 0b010 */
    Debug(&o) << BoolVector19(0x25, 0x53, 0x02);

    CORRADE_COMPARE(o.str(), "BoolVector(0b00100101, 0b01010011, 0b010)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::BoolVectorTest)
