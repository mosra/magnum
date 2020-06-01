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

#include <limits>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Packing.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct PackingTest: Corrade::TestSuite::Tester {
    explicit PackingTest();

    void bitMax();

    void unpackUnsigned();
    void unpackSigned();
    void packUnsigned();
    void packSigned();
    void reunpackUnsigned();
    void reunpackSigned();
    void unpackTypeDeduction();

    void pack8bitRoundtrip();
    void pack16bitRoundtrip();

    /* Half (un)pack functions are tested and benchmarked in HalfTest.cpp,
       because there's involved comparison and benchmarks to ground truth */
};

using namespace Literals;

typedef Math::Rad<Float> Rad;
typedef Math::Vector3<Float> Vector3;
typedef Math::Vector3<UnsignedByte> Vector3ub;
typedef Math::Vector3<Byte> Vector3b;

PackingTest::PackingTest() {
    addTests({&PackingTest::bitMax,

              &PackingTest::unpackUnsigned,
              &PackingTest::unpackSigned,
              &PackingTest::packUnsigned,
              &PackingTest::packSigned,
              &PackingTest::reunpackUnsigned,
              &PackingTest::reunpackSigned,
              &PackingTest::unpackTypeDeduction});

    addRepeatedTests({&PackingTest::pack8bitRoundtrip}, 256);
    addRepeatedTests({&PackingTest::pack16bitRoundtrip}, 65536);
}

void PackingTest::bitMax() {
    CORRADE_COMPARE(Implementation::bitMax<UnsignedByte>(), 0xff);
    CORRADE_COMPARE(Implementation::bitMax<Byte>(), 0x7f);
    CORRADE_COMPARE(Implementation::bitMax<UnsignedShort>(), 0xffff);
    CORRADE_COMPARE(Implementation::bitMax<Short>(), 0x7fff);
    CORRADE_COMPARE(Implementation::bitMax<UnsignedInt>(), 0xffffffff);
    CORRADE_COMPARE(Implementation::bitMax<Int>(), 0x7fffffff);
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_COMPARE(Implementation::bitMax<UnsignedLong>(), 0xffffffffffffffffull);
    CORRADE_COMPARE(Implementation::bitMax<Long>(), 0x7fffffffffffffffll);
    #endif

    CORRADE_COMPARE((Implementation::bitMax<UnsignedShort, 14>()), 16383);
    CORRADE_COMPARE((Implementation::bitMax<UnsignedInt, 14>()), 16383);
    CORRADE_COMPARE((Implementation::bitMax<Short, 14>()), 8191);
    CORRADE_COMPARE((Implementation::bitMax<Int, 14>()), 8191);
}

void PackingTest::unpackUnsigned() {
    CORRADE_COMPARE((Math::unpack<Float, UnsignedByte>(0)), 0.0f);
    CORRADE_COMPARE((Math::unpack<Float, UnsignedByte>(149)), 0.584314f);
    CORRADE_COMPARE((Math::unpack<Float, UnsignedByte>(255)), 1.0f);

    CORRADE_COMPARE((Math::unpack<Double, UnsignedInt>(0)), 0.0);
    CORRADE_COMPARE((Math::unpack<Double, UnsignedInt>(std::numeric_limits<UnsignedInt>::max())), 1.0);

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_COMPARE((Math::unpack<long double, UnsignedLong>(0)), 0.0);
    CORRADE_COMPARE((Math::unpack<long double, UnsignedLong>(std::numeric_limits<UnsignedLong>::max())), 1.0);
    #endif

    CORRADE_COMPARE((Math::unpack<Float, UnsignedShort>(0)), 0.0f);
    CORRADE_COMPARE((Math::unpack<Float, UnsignedShort>(std::numeric_limits<UnsignedShort>::max())), 1.0f);

    CORRADE_COMPARE((Math::unpack<Float, UnsignedShort>(8192)), 0.125002f);
    CORRADE_COMPARE((Math::unpack<Float, UnsignedShort>(49152)), 0.750011f);

    /* Bits */
    CORRADE_COMPARE((Math::unpack<Float, UnsignedShort>(8191)), 0.124987f);
    CORRADE_COMPARE((Math::unpack<Float, UnsignedShort, 14>(8191)), 0.499969f);
    CORRADE_COMPARE((Math::unpack<Float, 14>(8191u)), 0.499969f);
    CORRADE_COMPARE((Math::unpack<Float, 14>(8191)), 1.0f);

    /* Vector overloads */
    CORRADE_COMPARE(Math::unpack<Vector3>(Vector3ub(0, 127, 255)), Vector3(0.0f, 0.498039f, 1.0f));
    CORRADE_COMPARE((Math::unpack<Vector3, 6>(Vector3ub(0, 31, 63))), Vector3(0.0f, 0.492063f, 1.0f));

    /* Wrapped types */
    CORRADE_COMPARE((Math::unpack<Rad, UnsignedShort>(8191)), 0.124987_radf);
    CORRADE_COMPARE((Math::unpack<Rad, 14>(8191u)), 0.499969_radf);
}

void PackingTest::unpackSigned() {
    CORRADE_COMPARE((Math::unpack<Float, Byte>(127)), 1.0f);
    CORRADE_COMPARE((Math::unpack<Float, Byte>(37)), 0.291339f);
    CORRADE_COMPARE((Math::unpack<Float, Byte>(0)), 0.0f);
    CORRADE_COMPARE((Math::unpack<Float, Byte>(-72)), -0.566929f);
    CORRADE_COMPARE((Math::unpack<Float, Byte>(-128)), -1.0f);

    CORRADE_COMPARE((Math::unpack<Float, Short>(std::numeric_limits<Short>::min())), -1.0f);
    CORRADE_COMPARE((Math::unpack<Float, Short>(0)), 0.0f);
    CORRADE_COMPARE((Math::unpack<Float, Short>(std::numeric_limits<Short>::max())), 1.0f);

    CORRADE_COMPARE((Math::unpack<Double, Int>(std::numeric_limits<Int>::min())), -1.0);
    CORRADE_COMPARE((Math::unpack<Double, Int>(0)), 0.0);
    CORRADE_COMPARE((Math::unpack<Double, Int>(std::numeric_limits<Int>::max())), 1.0);

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_COMPARE((Math::unpack<long double, Long>(std::numeric_limits<Long>::min())), -1.0);
    CORRADE_COMPARE((Math::unpack<long double, Long>(0)), 0.0);
    CORRADE_COMPARE((Math::unpack<long double, Long>(std::numeric_limits<Long>::max())), 1.0);
    #endif

    CORRADE_COMPARE((Math::unpack<Float, Short>(16384)), 0.500015f);
    CORRADE_COMPARE((Math::unpack<Float, Short>(-16384)), -0.500015f);

    /* Bits */
    CORRADE_COMPARE((Math::unpack<Float, Short>(8191)), 0.249977f);
    CORRADE_COMPARE((Math::unpack<Float, 14>(8191)), 1.0f);

    /* Vector overloads */
    CORRADE_COMPARE(Math::unpack<Vector3>(Vector3b(0, -127, 64)), Vector3(0.0f, -1.0f, 0.503937f));
    CORRADE_COMPARE((Math::unpack<Vector3, 6>(Vector3b(0, -31, 16))), Vector3(0.0f, -1.0f, 0.516129f));

    /* Wrapped types */
    CORRADE_COMPARE((Math::unpack<Rad, Short>(8191)), 0.249977_radf);
    CORRADE_COMPARE((Math::unpack<Rad, 14>(8191)), 1.0_radf);
}

void PackingTest::packUnsigned() {
    /* Close extremes should work too */
    CORRADE_COMPARE(Math::pack<UnsignedByte>(0.0f), 0);
    CORRADE_COMPARE(Math::pack<UnsignedByte>(0.0000001f), 0);
    CORRADE_COMPARE(Math::pack<UnsignedByte>(0.4357f), 111);
    CORRADE_COMPARE(Math::pack<UnsignedByte>(0.5f), 128);
    CORRADE_COMPARE(Math::pack<UnsignedByte>(1.0f), 255);
    CORRADE_COMPARE(Math::pack<UnsignedByte>(0.9999999f), 255);

    CORRADE_COMPARE(Math::pack<UnsignedShort>(0.0f), 0);
    CORRADE_COMPARE(Math::pack<UnsignedShort>(0.000001f), 0);
    CORRADE_COMPARE(Math::pack<UnsignedShort>(0.4357f), 28554);
    CORRADE_COMPARE(Math::pack<UnsignedShort>(0.5f), 32768);
    CORRADE_COMPARE(Math::pack<UnsignedShort>(1.0f), 65535);
    CORRADE_COMPARE(Math::pack<UnsignedShort>(0.999999f), 65535);

    CORRADE_COMPARE(Math::pack<UnsignedInt>(0.0), 0);
    CORRADE_COMPARE(Math::pack<UnsignedInt>(1.0), std::numeric_limits<UnsignedInt>::max());

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_COMPARE(Math::pack<UnsignedLong>(0.0l), 0);
    {
        #ifdef _MSC_VER
        CORRADE_EXPECT_FAIL("Long double (de)normalization is broken on MSVC.");
        #endif
        CORRADE_COMPARE(Math::pack<UnsignedLong>(1.0l), std::numeric_limits<UnsignedLong>::max());
    }
    #endif

    CORRADE_COMPARE(Math::pack<UnsignedShort>(0.33f), 21627);
    CORRADE_COMPARE(Math::pack<UnsignedShort>(0.66f), 43253);

    /* Bits */
    CORRADE_COMPARE((Math::pack<UnsignedShort>(0.5f)), 32768);
    CORRADE_COMPARE((Math::pack<UnsignedShort, 14>(0.5f)), 8192);

    /* Vector overloads */
    CORRADE_COMPARE(Math::pack<Vector3ub>(Vector3(0.0f, 0.5f, 1.0f)), Vector3ub(0, 128, 255));
    CORRADE_COMPARE((Math::pack<Vector3ub, 6>(Vector3(0.0f, 0.5f, 1.0f))), Vector3ub(0, 32, 63));

    /* Wrapped types */
    CORRADE_COMPARE((Math::pack<UnsignedShort>(0.5_degf)), 32768);
    CORRADE_COMPARE((Math::pack<UnsignedShort, 14>(0.5_degf)), 8192);
}

void PackingTest::packSigned() {
    CORRADE_COMPARE(Math::pack<Byte>(-1.0f), -127);
    CORRADE_COMPARE(Math::pack<Byte>(-0.732f), -93);
    CORRADE_COMPARE(Math::pack<Byte>(0.0f), 0);
    CORRADE_COMPARE(Math::pack<Byte>(0.1357f), 17);
    CORRADE_COMPARE(Math::pack<Byte>(1.0f), 127);

    CORRADE_COMPARE(Math::pack<Short>(-1.0f), std::numeric_limits<Short>::min()+1);
    CORRADE_COMPARE(Math::pack<Short>(0.0f), 0);
    CORRADE_COMPARE(Math::pack<Short>(1.0f), std::numeric_limits<Short>::max());

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(Math::pack<Int>(-1.0), std::numeric_limits<Int>::min()+1);
    CORRADE_COMPARE(Math::pack<Int>(0.0), 0);
    CORRADE_COMPARE(Math::pack<Int>(1.0), std::numeric_limits<Int>::max());

    {
        #ifdef _MSC_VER
        CORRADE_EXPECT_FAIL("Long double (de)normalization is broken on MSVC.");
        #endif
        CORRADE_COMPARE(Math::pack<Long>(-1.0l), std::numeric_limits<Long>::min()+1);
    }
    CORRADE_COMPARE(Math::pack<Long>(0.0l), 0);
    {
        #ifdef _MSC_VER
        CORRADE_EXPECT_FAIL("Long double (de)normalization is broken on MSVC.");
        #endif
        CORRADE_COMPARE(Math::pack<Long>(1.0l), std::numeric_limits<Long>::max());
    }
    #endif

    CORRADE_COMPARE(Math::pack<Short>(-0.33f), -10813);
    CORRADE_COMPARE(Math::pack<Short>(0.66f), 21626);

    /* Bits */
    CORRADE_COMPARE((Math::pack<Short>(-0.5f)), -16384);
    CORRADE_COMPARE((Math::pack<Short, 14>(-0.5f)), -4096);

    /* Vector overloads */
    CORRADE_COMPARE(Math::pack<Vector3b>(Vector3(0.0f, -1.0f, 0.5f)), Vector3b(0, -127, 64));
    CORRADE_COMPARE((Math::pack<Vector3b, 6>(Vector3(0.0f, -1.0f, 0.5f))), Vector3b(0, -31, 16));

    /* Wrapped types */
    CORRADE_COMPARE((Math::pack<Short>(-0.5_degf)), -16384);
    CORRADE_COMPARE((Math::pack<Short, 14>(-0.5_degf)), -4096);
}

void PackingTest::reunpackUnsigned() {
    CORRADE_COMPARE(Math::unpack<Float>(Math::pack<UnsignedByte>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::unpack<Float>(Math::pack<UnsignedByte>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::unpack<Float>(Math::pack<UnsignedShort>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::unpack<Float>(Math::pack<UnsignedShort>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::unpack<Double>(Math::pack<UnsignedInt>(0.0)), 0.0);
    CORRADE_COMPARE(Math::unpack<Double>(Math::pack<UnsignedInt>(1.0)), 1.0);

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_COMPARE(Math::unpack<long double>(Math::pack<UnsignedLong>(0.0l)), 0.0l);
    {
        #ifdef _MSC_VER
        CORRADE_EXPECT_FAIL("Long double (de)normalization is broken on MSVC.");
        #endif
        CORRADE_COMPARE(Math::unpack<long double>(Math::pack<UnsignedLong>(1.0l)), 1.0l);
    }
    #endif
}

void PackingTest::reunpackSigned() {
    CORRADE_COMPARE(Math::unpack<Float>(Math::pack<Byte>(-1.0f)), -1.0f);
    CORRADE_COMPARE(Math::unpack<Float>(Math::pack<Byte>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::unpack<Float>(Math::pack<Byte>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::unpack<Float>(Math::pack<Short>(-1.0f)), -1.0f);
    CORRADE_COMPARE(Math::unpack<Float>(Math::pack<Short>(0.0f)), 0.0f);
    CORRADE_COMPARE(Math::unpack<Float>(Math::pack<Short>(1.0f)), 1.0f);

    CORRADE_COMPARE(Math::unpack<Double>(Math::pack<Int>(-1.0)), -1.0);
    CORRADE_COMPARE(Math::unpack<Double>(Math::pack<Int>(0.0)), 0.0);
    CORRADE_COMPARE(Math::unpack<Double>(Math::pack<Int>(1.0)), 1.0);

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_COMPARE(Math::unpack<long double>(Math::pack<Long>(-1.0l)), -1.0l);
    CORRADE_COMPARE(Math::unpack<long double>(Math::pack<Long>(0.0l)), 0.0l);
    {
        #if defined(_MSC_VER) || (defined(CORRADE_TARGET_ANDROID) && !__LP64__)
        CORRADE_EXPECT_FAIL("Long double (de)normalization is broken on MSVC and 32-bit Android.");
        #endif
        CORRADE_COMPARE(Math::unpack<long double>(Math::pack<Long>(1.0l)), 1.0l);
    }
    #endif
}

void PackingTest::unpackTypeDeduction() {
    if(std::is_signed<char>::value)
        CORRADE_COMPARE(Math::unpack<Float>('\x7F'), 1.0f);
    else {
        /* Raspberry Pi `char` is unsigned (huh) */
        CORRADE_VERIFY(std::is_unsigned<char>::value);
        CORRADE_COMPARE(Math::unpack<Float>('\x7F'), 0.498039f);
    }
    CORRADE_COMPARE((Math::unpack<Float, Byte>('\x7F')), 1.0f);
}

void PackingTest::pack8bitRoundtrip() {
    CORRADE_COMPARE(Math::pack<UnsignedByte>(Math::unpack<Float, UnsignedByte>(testCaseRepeatId())), testCaseRepeatId());
}

void PackingTest::pack16bitRoundtrip() {
    CORRADE_COMPARE(Math::pack<UnsignedShort>(Math::unpack<Float, UnsignedShort>(testCaseRepeatId())), testCaseRepeatId());
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::PackingTest)
