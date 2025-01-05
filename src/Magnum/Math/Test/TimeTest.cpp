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
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Time.h"

struct Time {
    unsigned secondsSinceEpoch;
};

struct Keyframe {
    float duration;
};

namespace Magnum { namespace Math {

namespace Implementation {

template<> struct NanosecondsConverter<Long, Time> {
    constexpr static Nanoseconds<Long> from(Time other) {
        return Nanoseconds<Long>{Long(other.secondsSinceEpoch)*1000000000ll};
    }
    constexpr static Time to(Nanoseconds<Long> other) {
        return {unsigned(Long(other)/1000000000ll)};
    }
};
template<> struct SecondsConverter<Float, Keyframe> {
    constexpr static Seconds<Float> from(Keyframe other) {
        return Seconds<Float>{other.duration};
    }
    constexpr static Keyframe to(Seconds<Float> other) {
        return {Float(other)};
    }
};

}

namespace Test { namespace {

struct TimeTest: TestSuite::Tester {
    explicit TimeTest();

    void limits();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructCopy();
    void constructFromBase();
    void convert();

    void literals();
    void conversion();
    void nanosecondFloatScaling();

    void debugNanoseconds();
    void debugNanosecondsPacked();
    void debugSeconds();
    void debugSecondsPacked();
};

using Magnum::Nanoseconds;
using Magnum::Seconds;

using namespace Math::Literals;

TimeTest::TimeTest() {
    addTests({&TimeTest::limits,

              &TimeTest::construct,
              &TimeTest::constructDefault,
              &TimeTest::constructNoInit,
              &TimeTest::constructCopy,
              &TimeTest::constructFromBase,
              &TimeTest::convert,

              &TimeTest::literals,
              &TimeTest::conversion,
              &TimeTest::nanosecondFloatScaling,

              &TimeTest::debugNanoseconds,
              &TimeTest::debugNanosecondsPacked,
              &TimeTest::debugSeconds,
              &TimeTest::debugSecondsPacked});
}

void TimeTest::limits() {
    /* There's apparently no way to say -0x8000000000000000ll, so there's also
       no non-error-prone way to verify the values are correct. */

    /* It should be all 64 bits (so 16 nibbles) being set */
    CORRADE_COMPARE(UnsignedLong(Long(Nanoseconds::min()))|
                    UnsignedLong(Long(Nanoseconds::max())),
                   /* 0123456789abcdef */
                    0xffffffffffffffffull);

    /* Assuming signed integer overflow is defined sanely, which it should */
    CORRADE_COMPARE(Nanoseconds::min() - 1_nsec, Nanoseconds::max());
    CORRADE_COMPARE(Nanoseconds::max() + 1_nsec, Nanoseconds::min());

    /* This should also hold */
    CORRADE_COMPARE(Nanoseconds::min() + Nanoseconds::max(), -1_nsec);
    CORRADE_COMPARE(Nanoseconds::max() - Nanoseconds::min(), -1_nsec);
}

void TimeTest::construct() {
    Nanoseconds a{-123456789123456789ll};
    Seconds b{123.45f};
    CORRADE_COMPARE(Long(a), -123456789123456789ll);
    CORRADE_COMPARE(Float(b), 123.45f);

    constexpr Nanoseconds ca{-123456789123456789ll};
    constexpr Seconds cb{123.45f};
    CORRADE_COMPARE(Long(ca), -123456789123456789ll);
    CORRADE_COMPARE(Float(cb), 123.45f);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Long, Nanoseconds>::value);
    CORRADE_VERIFY(!std::is_convertible<Float, Seconds>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Nanoseconds, Long>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Seconds, Float>::value);
}

void TimeTest::constructDefault() {
    Nanoseconds a1;
    Nanoseconds a2{ZeroInit};
    Seconds b1;
    Seconds b2{ZeroInit};
    CORRADE_COMPARE(Long(a1), 0ll);
    CORRADE_COMPARE(Long(a2), 0ll);
    CORRADE_COMPARE(Float(b1), 0.0f);
    CORRADE_COMPARE(Float(b2), 0.0f);

    constexpr Nanoseconds ca1;
    constexpr Nanoseconds ca2{ZeroInit};
    constexpr Seconds cb1;
    constexpr Seconds cb2{ZeroInit};
    CORRADE_COMPARE(Long(ca1), 0ll);
    CORRADE_COMPARE(Long(ca2), 0ll);
    CORRADE_COMPARE(Float(cb1), 0.0f);
    CORRADE_COMPARE(Float(cb2), 0.0f);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Nanoseconds>::value);
    CORRADE_VERIFY(std::is_nothrow_default_constructible<Seconds>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Nanoseconds, ZeroInitT>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Seconds, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Nanoseconds>::value);
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Seconds>::value);
}

void TimeTest::constructNoInit() {
    Nanoseconds a{123456789123456789ll};
    Seconds b{123.45f};
    new(&a) Nanoseconds{Magnum::NoInit};
    new(&b) Seconds{Magnum::NoInit};
    {
        /* Explicitly check we're not on Clang because certain Clang-based IDEs
           inherit __GNUC__ if GCC is used instead of leaving it at 4 like
           Clang itself does */
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601
        /* The warning is reported for both debug and release build */
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
        /* On GCC 13 it's -Wuninitialized now, the compiler is now EVEN MORE
           DEFINITELY RIGHT that I'm doing something wrong */
        #if __GNUC__ >= 13
        #pragma GCC diagnostic ignored "-Wuninitialized"
        #endif
        #ifdef __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        #endif
        CORRADE_COMPARE(Long(a), 123456789123456789ll);
        CORRADE_COMPARE(Float(b), 123.45f);
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601
        #pragma GCC diagnostic pop
        #endif
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<Nanoseconds, Magnum::NoInitT>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Seconds, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Nanoseconds>::value);
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Seconds>::value);
}

void TimeTest::constructCopy() {
    Nanoseconds a{-987654321987654321ll};
    Nanoseconds b{a};
    Seconds c{-543.21f};
    Seconds d{c};
    CORRADE_COMPARE(Long(b), -987654321987654321ll);
    CORRADE_COMPARE(Float(d), -543.21f);

    constexpr Nanoseconds ca{-987654321987654321ll};
    constexpr Nanoseconds cb{ca};
    constexpr Seconds cc{-543.21f};
    constexpr Seconds cd{cc};
    CORRADE_COMPARE(Long(cb), -987654321987654321ll);
    CORRADE_COMPARE(Float(cd), -543.21f);

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Nanoseconds>::value);
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Seconds>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Nanoseconds>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Seconds>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Nanoseconds>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Seconds>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Nanoseconds>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Seconds>::value);
}

void TimeTest::constructFromBase() {
    /* The operation returns Unit instead of the leaf type, so this can work
       only if the base class has a "copy constructor" from the base type */
    Nanoseconds a = 15.0_usec + 3.5_msec;
    Seconds b = Seconds{15.0_msec} + Seconds{3.5_sec};
    CORRADE_COMPARE(a, 3.515_msec);
    /* Comparing as seconds because precision loss involved */
    CORRADE_COMPARE_AS(b, 3.515_sec, Seconds);
}

void TimeTest::convert() {
    /* From external type */
    Time a0{1707678819};
    Keyframe b0{56.72f};
    Nanoseconds a1{a0};
    Seconds b1{b0};
    CORRADE_COMPARE(a1, 1707678819.0_sec);
    CORRADE_COMPARE_AS(b1, 56.72_sec, Seconds);

    constexpr Time ca0{1707678819};
    constexpr Keyframe cb0{56.72f};
    constexpr Nanoseconds ca1{ca0};
    constexpr Seconds cb1{cb0};
    CORRADE_COMPARE(ca1, 1707678819.0_sec);
    CORRADE_COMPARE_AS(cb1, 56.72_sec, Seconds);

    /* To external type */
    Nanoseconds c0 = 1707678819.0_sec;
    Seconds d0 = 56.72_sec;
    Time c1(c0);
    Keyframe d1(d0);
    CORRADE_COMPARE(c1.secondsSinceEpoch, 1707678819);
    CORRADE_COMPARE(d1.duration, 56.72f);

    constexpr Nanoseconds cc0 = 1707678819.0_sec;
    constexpr Seconds cd0 = 56.72_sec;
    constexpr Time cc1(cc0);
    constexpr Keyframe cd1(cd0);
    CORRADE_COMPARE(cc1.secondsSinceEpoch, 1707678819);
    CORRADE_COMPARE(cd1.duration, 56.72f);

    /* It should not be possible to convert in a direction that may result in a
       precision loss, i.e. Seconds with a NanosecondsConverter */
    CORRADE_VERIFY(std::is_constructible<Seconds, Keyframe>::value);
    CORRADE_VERIFY(!std::is_constructible<Seconds, Time>::value);
    CORRADE_VERIFY(std::is_constructible<Nanoseconds, Time>::value);
    CORRADE_VERIFY(!std::is_constructible<Nanoseconds, Keyframe>::value);
    CORRADE_VERIFY(std::is_constructible<Time, Nanoseconds>::value);
    CORRADE_VERIFY(!std::is_constructible<Time, Seconds>::value);
    CORRADE_VERIFY(std::is_constructible<Keyframe, Seconds>::value);
    CORRADE_VERIFY(!std::is_constructible<Keyframe, Nanoseconds>::value);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Keyframe, Seconds>::value);
    CORRADE_VERIFY(!std::is_convertible<Time, Nanoseconds>::value);
    CORRADE_VERIFY(!std::is_convertible<Nanoseconds, Time>::value);
    CORRADE_VERIFY(!std::is_convertible<Seconds, Keyframe>::value);
}

void TimeTest::literals() {
    /* Testing the full precision, 19 digits. Max representable 63-bit value is
             9223372036854775807. */
    auto a = 9087654321987654321_nsec;
    auto b = 9087654321987654.321_usec;
    auto c = 9087654321987.654321_msec;
    auto d = 9087654321.987654321_sec;
    CORRADE_VERIFY(std::is_same<decltype(a), Nanoseconds>::value);
    CORRADE_VERIFY(std::is_same<decltype(b), Nanoseconds>::value);
    CORRADE_VERIFY(std::is_same<decltype(c), Nanoseconds>::value);
    CORRADE_VERIFY(std::is_same<decltype(d), Nanoseconds>::value);
    CORRADE_COMPARE(Long(a), 9087654321987654321ll);
    /* Not sure what Emscripten does here, but it behaves as if long double was
       actually the full precision */
    #if !defined(CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE) || defined(CORRADE_TARGET_EMSCRIPTEN)
    /* 80-bit long double has a 63-bit mantissa, which means this is converted
       without any precision loss. Otherwise the precision is just 52 bits. */
    CORRADE_COMPARE(Long(b), 9087654321987654321ll);
    /* Well, almost. On x86 this conversion has a slight imprecision in the
       lowest bit for the _ms variant, on ARM64 and Emscripten for the _s
       variant. */
    #if (defined(CORRADE_TARGET_ARM) && !defined(CORRADE_TARGET_32BIT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    CORRADE_COMPARE(Long(c), 9087654321987654321ll);
    CORRADE_COMPARE(Long(d), 9087654321987654320ll);
    #else
    CORRADE_COMPARE(Long(c), 9087654321987654320ll);
    CORRADE_COMPARE(Long(d), 9087654321987654321ll);
    #endif
    #else
    CORRADE_COMPARE(Long(b), 9087654321987653632ll);
    CORRADE_COMPARE(Long(c), 9087654321987654656ll);
    CORRADE_COMPARE(Long(d), 9087654321987653632ll);
    #endif

    constexpr auto ca = 9087654321987654321_nsec;
    constexpr auto cb = 9087654321987654.321_usec;
    constexpr auto cc = 9087654321987.654321_msec;
    constexpr auto cd = 9087654321.987654321_sec;
    CORRADE_VERIFY(std::is_same<decltype(ca), const Nanoseconds>::value);
    CORRADE_VERIFY(std::is_same<decltype(cb), const Nanoseconds>::value);
    CORRADE_VERIFY(std::is_same<decltype(cc), const Nanoseconds>::value);
    CORRADE_VERIFY(std::is_same<decltype(cd), const Nanoseconds>::value);
    CORRADE_COMPARE(Long(ca), 9087654321987654321ll);
    #if !defined(CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE) || defined(CORRADE_TARGET_EMSCRIPTEN)
    CORRADE_COMPARE(Long(cb), 9087654321987654321ll);
    #if (defined(CORRADE_TARGET_ARM) && !defined(CORRADE_TARGET_32BIT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    CORRADE_COMPARE(Long(cc), 9087654321987654321ll);
    CORRADE_COMPARE(Long(cd), 9087654321987654320ll);
    #else
    CORRADE_COMPARE(Long(cc), 9087654321987654320ll);
    CORRADE_COMPARE(Long(cd), 9087654321987654321ll);
    #endif
    #else
    CORRADE_COMPARE(Long(cb), 9087654321987653632ll);
    CORRADE_COMPARE(Long(cc), 9087654321987654656ll);
    CORRADE_COMPARE(Long(cd), 9087654321987653632ll);
    #endif
}

void TimeTest::conversion() {
    /* Implicit conversion should be allowed. Again testing (almost) the full
       nanosecond precision, although not much of it is left when converting
       to a 32-bit float. */
    Nanoseconds a = Seconds{-987654321.987654321f};
    Seconds b = 987654321987654321_nsec;
    CORRADE_COMPARE(Long(a), -987654336000000000ll);
    CORRADE_COMPARE(Float(b), 987654336.0f);

    constexpr Nanoseconds ca = Seconds{987654321.987654321f};
    constexpr Seconds cb = -987654321987654321_nsec;
    CORRADE_COMPARE(Long(ca), 987654336000000000ll);
    CORRADE_COMPARE(Float(cb), -987654336.0f);

    CORRADE_VERIFY(std::is_nothrow_constructible<Nanoseconds, Seconds>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Seconds, Nanoseconds>::value);
}

void TimeTest::nanosecondFloatScaling() {
    /* Nanoseconds is an integer type, but multiplying it with float should
       give a reasonable output. The actual logic is in the Unit class, just
       verify that it works from the high level perspective here. */

    CORRADE_COMPARE(1000000000_nsec*1.25, 1250000000_nsec);
    CORRADE_COMPARE(1000000000_nsec*1.25f, 1250000000_nsec);
    CORRADE_COMPARE(1000000000_nsec/0.8, 1250000000_nsec);
    CORRADE_COMPARE(1000000000_nsec/0.8f, 1250000000_nsec);

    /* Compared to above this looks like it should "obviously work", although
       internally both cases are the same, operating on Nanoseconds */
    CORRADE_COMPARE(1.0_sec*1.25, 1.25_sec);
    CORRADE_COMPARE(1.0_sec*1.25f, 1.25_sec);
    CORRADE_COMPARE(1.0_sec/0.8, 1.25_sec);
    CORRADE_COMPARE(1.0_sec/0.8f, 1.25_sec);

    /* This would be nice if it worked, but so far it doesn't, as it's
       calculated as an integer value */
    CORRADE_COMPARE(1.0f/0.018f, 55.5556f);
    {
        CORRADE_EXPECT_FAIL("This doesn't work correctly.");
        CORRADE_COMPARE(1.0_sec/18.0_msec, 55.5556f);
    }
}

void TimeTest::debugNanoseconds() {
    Containers::String out;

    /* Also verify that the second expression compiles (it's the Unit type,
       not Nanoseconds) */
    Debug{&out} << 987654321987654321_nsec << 15.0_sec - 7.5_sec;
    CORRADE_COMPARE(out, "Nanoseconds(987654321987654321) Nanoseconds(7500000000)\n");
}

void TimeTest::debugNanosecondsPacked() {
    Containers::String out;

    /* Second is not packed, the first should not make any flags persistent */
    Debug{&out} << Debug::packed << 15.0_sec << 45.0_sec;
    CORRADE_COMPARE(out, "15000000000 Nanoseconds(45000000000)\n");
}

void TimeTest::debugSeconds() {
    Containers::String out;

    /* Also verify that the second expression compiles (it's the Unit type,
       not Nanoseconds) */
    Debug{&out} << Seconds{123.45_sec} << Seconds{15.0_sec} - Seconds{7.5_sec};
    CORRADE_COMPARE(out, "Seconds(123.45) Seconds(7.5)\n");
}

void TimeTest::debugSecondsPacked() {
    Containers::String out;

    /* Second is not packed, the first should not make any flags persistent */
    Debug{&out} << Debug::packed << Seconds{123.45_sec} << Seconds{45.0_sec};
    CORRADE_COMPARE(out, "123.45 Seconds(45)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::TimeTest)
