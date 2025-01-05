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

#include <new>
#include <Corrade/Containers/ArrayView.h> /* arraySize() */
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/BitVector.h"
#include "Magnum/Math/StrictWeakOrdering.h"

struct BVec3 {
    bool x, y, z;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct BitVectorConverter<3, BVec3> {
    constexpr static BitVector<3> from(const BVec3& other) {
        return (other.x << 0)|(other.y << 1)|(other.z << 2);
    }

    constexpr static BVec3 to(const BitVector<3>& other) {
        return {other[0], other[1], other[2]};
    }
};

}

namespace Test { namespace {

struct BitVectorTest: TestSuite::Tester {
    explicit BitVectorTest();

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

static_assert(BitVector<15>::DataSize == 2, "Improper DataSize");
static_assert(BitVector<16>::DataSize == 2, "Improper DataSize");
static_assert(BitVector<17>::DataSize == 3, "Improper DataSize");

typedef Math::BitVector<19> BitVector19;

BitVectorTest::BitVectorTest() {
    addTests({&BitVectorTest::construct,
              &BitVectorTest::constructDefault,
              &BitVectorTest::constructNoInit,
              &BitVectorTest::constructOneValue,
              &BitVectorTest::constructOneElement,
              &BitVectorTest::constructCopy,
              &BitVectorTest::convert,

              &BitVectorTest::data,

              &BitVectorTest::compare,
              &BitVectorTest::compareUndefined,
              &BitVectorTest::convertBool,
              &BitVectorTest::all,
              &BitVectorTest::none,
              &BitVectorTest::any,

              &BitVectorTest::bitInverse,
              &BitVectorTest::bitAndOrXor,
              &BitVectorTest::booleanOperationEquivalents,

              &BitVectorTest::strictWeakOrdering,

              &BitVectorTest::debug});
}

void BitVectorTest::construct() {
    constexpr BitVector19 a = {0xa5, 0x5f, 0x07};
    CORRADE_COMPARE(a, BitVector19(0xa5, 0x5f, 0x07));

    CORRADE_VERIFY(std::is_nothrow_constructible<BitVector19, UnsignedByte, UnsignedByte, UnsignedByte>::value);
}

void BitVectorTest::constructDefault() {
    constexpr BitVector19 a;
    constexpr BitVector19 b{ZeroInit};
    CORRADE_COMPARE(a, BitVector19(0x00, 0x00, 0x00));
    CORRADE_COMPARE(b, BitVector19(0x00, 0x00, 0x00));

    CORRADE_VERIFY(std::is_nothrow_default_constructible<BitVector19>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<BitVector19, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, BitVector19>::value);
}

void BitVectorTest::constructNoInit() {
    BitVector19 a{0xa5, 0x5f, 0x07};
    new(&a) BitVector19{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, BitVector19(0xa5, 0x5f, 0x07));
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<BitVector19, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, BitVector19>::value);
}

void BitVectorTest::constructOneValue() {
    constexpr BitVector19 a(false);
    CORRADE_COMPARE(a, BitVector19(0x00, 0x00, 0x00));

    constexpr BitVector19 b(true);
    CORRADE_COMPARE(b, BitVector19(0xff, 0xff, 0x07));

    CORRADE_VERIFY(!std::is_convertible<bool, BitVector19>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<BitVector19, bool>::value);
}

void BitVectorTest::constructOneElement() {
    typedef BitVector<1> BitVector1;

    constexpr BitVector1 a = 0x01;
    CORRADE_COMPARE(a, BitVector1(0x01));

    CORRADE_VERIFY(std::is_nothrow_constructible<BitVector1, UnsignedByte>::value);
}

void BitVectorTest::constructCopy() {
    constexpr BitVector19 a = {0xa5, 0x5f, 0x07};
    constexpr BitVector19 b(a);
    CORRADE_COMPARE(b, BitVector19(0xa5, 0x5f, 0x07));

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<BitVector19>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<BitVector19>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<BitVector19>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<BitVector19>::value);
}

void BitVectorTest::convert() {
    constexpr BVec3 a{false, true, true};
    constexpr BitVector<3> b{0x6};

    constexpr BitVector<3> c{a};
    CORRADE_COMPARE(c, b);

    constexpr BVec3 d(b);
    CORRADE_COMPARE(d.x, a.x);
    CORRADE_COMPARE(d.y, a.y);
    CORRADE_COMPARE(d.z, a.z);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<BVec3, BitVector<3>>::value);
    CORRADE_VERIFY(!std::is_convertible<BitVector<3>, BVec3>::value);
}

void BitVectorTest::data() {
    /* 0b00001000, 0b00000011, 0b100 */
    constexpr BitVector19 ca(0x08, 0x03, 0x04);

    CORRADE_VERIFY(!ca[0] && !ca[1] && !ca[2]);
    CORRADE_VERIFY(ca[3]);
    CORRADE_VERIFY(!ca[4] && !ca[5] && !ca[6] && !ca[7]);
    CORRADE_VERIFY(ca[8]);
    CORRADE_VERIFY(ca[9]);
    CORRADE_VERIFY(!ca[10] && !ca[11] && !ca[12] && !ca[13] && !ca[14] && !ca[15] && !ca[16] && !ca[17]);
    CORRADE_VERIFY(ca[18]);

    constexpr bool b = ca[9];
    CORRADE_COMPARE(b, true);

    BitVector19 a1(0x08, 0x03, 0x04);
    BitVector19 a2(0x08, 0x03, 0x04);
    a1.set(15);
    a2.set(15, true);
    CORRADE_VERIFY(a1[15]);
    CORRADE_VERIFY(a2[15]);
    CORRADE_COMPARE(a1, BitVector19(0x08, 0x83, 0x04));
    CORRADE_COMPARE(a2, BitVector19(0x08, 0x83, 0x04));
    a1.reset(15);
    a2.set(15, false);
    CORRADE_VERIFY(!a1[15]);
    CORRADE_VERIFY(!a2[15]);
    CORRADE_COMPARE(a1, BitVector19(0x08, 0x03, 0x04));
    CORRADE_COMPARE(a2, BitVector19(0x08, 0x03, 0x04));

    #ifndef CORRADE_MSVC2015_COMPATIBILITY /* Apparently dereferencing pointer is verboten */
    constexpr
    #endif
    UnsignedByte c = *ca.data();
    CORRADE_COMPARE(a1.data()[1], 0x03);
    CORRADE_COMPARE(c, 0x08);

    /* It actually returns an array */
    CORRADE_COMPARE(Containers::arraySize(a1.data()), 3);
    CORRADE_COMPARE(Containers::arraySize(ca.data()), 3);
}

void BitVectorTest::compare() {
    BitVector19 a(0xa5, 0x5f, 0x07);
    CORRADE_VERIFY(a == a);

    /* Change in full segments */
    BitVector19 b(0xa3, 0x5f, 0x07);
    BitVector19 c(0xa5, 0x98, 0x07);
    CORRADE_VERIFY(a != b);
    CORRADE_VERIFY(a != c);

    /* Change in last bit */
    BitVector19 d(0xa5, 0x5f, 0x06);
    CORRADE_VERIFY(a != d);
}

void BitVectorTest::compareUndefined() {
    BitVector19 a(0xa5, 0x5f, 0x07);

    /* Change in unused part of last segment */
    BitVector19 b(0xa5, 0x5f, 0x0f);
    CORRADE_VERIFY(a == b);

    /* Change in used part of last segment */
    BitVector19 c(0xa5, 0x5f, 0x03);
    CORRADE_VERIFY(a != c);
}

void BitVectorTest::convertBool() {
    CORRADE_VERIFY(BitVector19(0xff, 0xff, 0x07));
    CORRADE_VERIFY(!bool(BitVector19(0xff, 0xff, 0x04)));
    CORRADE_VERIFY(!bool(BitVector19(0x00, 0x00, 0x00)));
    CORRADE_VERIFY(BitVector19(0xff, 0xff, 0xff));

    /* Using ! before and after bool conversion will produce a different
       result -- first is equivalent to !a.all(), while second is (~a).all() */
    CORRADE_COMPARE(!bool(BitVector19(0xff, 0xff, 0x04)), true);
    CORRADE_COMPARE(bool(!BitVector19(0xff, 0xff, 0x04)), false);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<BitVector19, bool>::value);
}

void BitVectorTest::all() {
    CORRADE_VERIFY(BitVector19(0xff, 0xff, 0x07).all());

    /* Last segment - bit in used and unused part */
    CORRADE_VERIFY(BitVector19(0xff, 0xff, 0x0f).all());
    CORRADE_VERIFY(!BitVector19(0xff, 0xff, 0x04).all());
}

void BitVectorTest::none() {
    CORRADE_VERIFY(BitVector19(0x00, 0x00, 0x00).none());

    /* Last segment - bit in used and unused part */
    CORRADE_VERIFY(BitVector19(0x00, 0x00, 0x08).none());
    CORRADE_VERIFY(!BitVector19(0x00, 0x00, 0x04).none());
}

void BitVectorTest::any() {
    CORRADE_VERIFY(BitVector19(0x00, 0x01, 0x00).any());

    /* Last segment - bit in used and unused part */
    CORRADE_VERIFY(BitVector19(0x00, 0x00, 0x04).any());
    CORRADE_VERIFY(!BitVector19(0x00, 0x00, 0x08).any());
}

void BitVectorTest::bitInverse() {
    CORRADE_COMPARE(~BitVector19(0xa5, 0x5f, 0x03), BitVector19(0x5a, 0xa0, 0x04));
    CORRADE_COMPARE(!BitVector19(0xa5, 0x5f, 0x03), BitVector19(0x5a, 0xa0, 0x04));
}

void BitVectorTest::bitAndOrXor() {
    BitVector19 a(0xa5, 0x5f, 0x03);
    BitVector19 b(0x37, 0xf3, 0x06);

    CORRADE_COMPARE(a & b, BitVector19(0x25, 0x53, 0x02));
    CORRADE_COMPARE(a && b, BitVector19(0x25, 0x53, 0x02));

    CORRADE_COMPARE(a | b, BitVector19(0xb7, 0xff, 0x07));
    CORRADE_COMPARE(a || b, BitVector19(0xb7, 0xff, 0x07));

    CORRADE_COMPARE(a ^ b, BitVector19(0x92, 0xac, 0x05));
}

void BitVectorTest::booleanOperationEquivalents() {
    Math::BitVector<2> a{0x3};
    Math::BitVector<2> b{0x2};

    CORRADE_COMPARE(!(a || b), !a && !b);
    CORRADE_COMPARE(!(a || b), ~(a | b));
    CORRADE_COMPARE(!a && !b, ~a & ~b);
}

void BitVectorTest::strictWeakOrdering() {
    BitVector<11> a, b, c;

    a.set(0);
    a.set(1);

    c.set(7);

    b.set(8);

    StrictWeakOrdering o;
    CORRADE_VERIFY( o(b, a));
    CORRADE_VERIFY(!o(a, b));
    CORRADE_VERIFY(!o(c, b));
    CORRADE_VERIFY( o(a, c));
    CORRADE_VERIFY(!o(c, a));

    CORRADE_VERIFY(!o(a, a));

    /* Check uninitialized padding reads */
    a.set(8);
    a.set(10);
    b = a;
    a.data()[1] |= 0x08;
    b.data()[1] |= 0x20;
    a.data()[1] |= 0x40;
    b.data()[1] |= 0x80;

    CORRADE_VERIFY(!o(a, b));
    CORRADE_VERIFY(!o(b, a));
}

void BitVectorTest::debug() {
    Containers::String out;

    /* 0b00100101 0b01010011 0b010 */
    Debug{&out} << BitVector19(0x25, 0x53, 0x02);

    CORRADE_COMPARE(out, "BitVector(0b00100101, 0b01010011, 0b010)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::BitVectorTest)
