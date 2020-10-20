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

#include <cstring>
#include <algorithm>
#include <sstream>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>
#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Corrade/Utility/Tweakable.h>
#endif

#include "Magnum/Math/Half.h"
#include "Magnum/Math/Packing.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/StrictWeakOrdering.h"
#include "Magnum/Math/PackingBatch.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct HalfTest: Corrade::TestSuite::Tester {
    explicit HalfTest();

    void unpack();
    void pack();
    void repack();

    void unpack1k();
    void unpack1kNaive();
    void unpack1kTable();
    void pack1k();
    void pack1kNaive();
    void pack1kTable();

    void constructDefault();
    void constructValue();
    void constructData();
    void constructNoInit();
    void constructCopy();

    void compare();
    void compareNaN();

    void promotion();
    void negation();

    void strictWeakOrdering();

    void literal();
    void debug();
    #if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    void tweakable();
    void tweakableError();
    #endif
};

typedef Math::Constants<Float> Constants;

using namespace Literals;

#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
const struct {
    const char* name;
    const char* data;
    Half result;
} TweakableData[] {
    {"fixed", "35.0_h", 35.0_h},
    {"no zero before", ".5_h",  0.5_h},
    {"no zero after", "35._h", 35.0_h},
    {"exponential positive", "3.5e+1_h", 3.5e+1_h},
    {"exponential negative", "350.0e-1_h", 350.0e-1_h},
    {"positive", "+35.0_h", +35.0_h},
    {"negative", "-35.0_h", -35.0_h}
};

constexpr struct {
    const char* name;
    const char* data;
    Corrade::Utility::TweakableState state;
    const char* error;
} TweakableErrorData[] {
    {"empty", "", Corrade::Utility::TweakableState::Recompile,
        "Utility::TweakableParser:  is not a half literal\n"},
    {"integral", "42_h", Corrade::Utility::TweakableState::Recompile,
        "Utility::TweakableParser: 42_h is not a half literal\n"},
    {"garbage after", "42.b_h", Corrade::Utility::TweakableState::Recompile,
        "Utility::TweakableParser: unexpected characters b_h after a half literal\n"},
    {"different suffix", "42.0u", Corrade::Utility::TweakableState::Recompile, /* not for double */
        "Utility::TweakableParser: 42.0u has an unexpected suffix, expected _h\n"}
};
#endif

HalfTest::HalfTest() {
    addTests({&HalfTest::unpack,
              &HalfTest::pack});

    addRepeatedTests({&HalfTest::repack}, 65536);

    addBenchmarks({
        &HalfTest::unpack1k,
        &HalfTest::unpack1kNaive,
        &HalfTest::unpack1kTable,
        &HalfTest::pack1k,
        &HalfTest::pack1kNaive,
        &HalfTest::pack1kTable}, 100);

    addTests({&HalfTest::constructDefault,
              &HalfTest::constructValue,
              &HalfTest::constructData,
              &HalfTest::constructNoInit,
              &HalfTest::constructCopy,

              &HalfTest::compare});

    addRepeatedTests({&HalfTest::compareNaN}, 65536);

    addTests({&HalfTest::promotion,
              &HalfTest::negation,

              &HalfTest::strictWeakOrdering,

              &HalfTest::literal,
              &HalfTest::debug});

    #if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    addInstancedTests({&HalfTest::tweakable},
                      Corrade::Containers::arraySize(TweakableData));

    addInstancedTests({&HalfTest::tweakableError},
                      Corrade::Containers::arraySize(TweakableErrorData));
    #endif
}

union FloatBits {
    UnsignedInt u;
    Float f;
    struct {
        UnsignedInt mantissa:23;
        UnsignedInt exponent:8;
        UnsignedInt sign:1;
    } bits;
};

union HalfBits {
    UnsignedShort u;
    struct {
        UnsignedShort mantissa:10;
        UnsignedShort exponent:5;
        UnsignedShort sign:1;
    } bits;
};

/* float_to_half_full() from https://gist.github.com/rygorous/2156668,
   originally from ISPC */
UnsignedShort packNaive(Float value) {
    FloatBits f;
    f.f = value;
    HalfBits o{};

    /* Signed zero/denormal (which will underflow) */
    if(f.bits.exponent == 0) {
        o.bits.exponent = 0;

    /* Inf or NaN (all exponent bits set): NaN->qNaN and Inf->Inf */
    } else if(f.bits.exponent == 255) {
        o.bits.exponent = 31;
        o.bits.mantissa = f.bits.mantissa ? 0x200 : 0;

    /* Normalized number */
    } else {
        /* Exponent unbias the single, then bias the halfp */
        Int newexp = f.bits.exponent - 127 + 15;

        /* Overflow, return signed infinity */
        if(newexp >= 31) {
            o.bits.exponent = 31;

        /* Underflow */
        } else if(newexp <= 0) {
            /* Mantissa might be non-zero */
            if((14 - newexp) <= 24) {
                /* Hidden 1 bit */
                UnsignedInt mant = f.bits.mantissa | 0x800000;
                o.bits.mantissa = mant >> (14 - newexp);

                /* Check for rounding */
                if((mant >> (13 - newexp)) & 1) {
                    /* Round, might overflow into exp bit, but this is OK */
                    o.u++;
                }
            }
        } else {
            o.bits.exponent = newexp;
            o.bits.mantissa = f.bits.mantissa >> 13;

            /* Check for rounding */
            if(f.bits.mantissa & 0x1000) {
                /* Round, might overflow to inf, this is OK */
                o.u++;
            }
        }
    }

    o.bits.sign = f.bits.sign;
    return o.u;
}

/* half_to_float_full() from https://gist.github.com/rygorous/2144712,
   originally from ISPC */
Float unpackNaive(UnsignedShort value) {
    HalfBits h{value};
    FloatBits o{};

    /* (Signed) zero */
    if(h.bits.exponent == 0 && h.bits.mantissa == 0) {
        o.bits.sign = h.bits.sign;

    } else {
        /* Denormal (will convert to normalized) */
        if(h.bits.exponent == 0) {
            /* Adjust mantissa so it's normalized (and keep track of exp
               adjust) */
            Int e = -1;
            UnsignedInt m = h.bits.mantissa;
            do {
                e++;
                m <<= 1;
            } while((m & 0x400) == 0);

            o.bits.mantissa = (m & 0x3ff) << 13;
            o.bits.exponent = 127 - 15 - e;
            o.bits.sign = h.bits.sign;

        /* Inf/NaN */
        } else if(h.bits.exponent == 0x1f) {
            /* Note: it's safe to treat both with the same code path by just
               truncating lower Mantissa bits in NaNs (this is valid). */
            o.bits.mantissa = h.bits.mantissa << 13;
            o.bits.exponent = 255;
            o.bits.sign = h.bits.sign;

        /* Normalized number */
        } else {
            o.bits.mantissa = h.bits.mantissa << 13;
            o.bits.exponent = 127 - 15 + h.bits.exponent;
            o.bits.sign = h.bits.sign;
        }
    }

    return o.f;
}

UnsignedShort packTable(const Float value) {
    UnsignedShort out;
    packHalfInto(
        Corrade::Containers::StridedArrayView2D<const Float>{{&value, 1}, {1, 1}},
        Corrade::Containers::StridedArrayView2D<UnsignedShort>{{&out, 1}, {1, 1}});
    return out;
}

Float unpackTable(const UnsignedShort value) {
    Float out;
    unpackHalfInto(
        Corrade::Containers::StridedArrayView2D<const UnsignedShort>{{&value, 1}, {1, 1}},
        Corrade::Containers::StridedArrayView2D<Float>{{&out, 1}, {1, 1}});
    return out;
}

void HalfTest::unpack() {
    CORRADE_COMPARE(Math::unpackHalf(0x0000), 0.0f);

    /* 0b0011110000000000 */
    CORRADE_COMPARE(Math::unpackHalf(0x3c00), 1.0f);

    /* 0b0100000000000000 */
    CORRADE_COMPARE(Math::unpackHalf(0x4000), 2.0f);

    /* 0b0100001000000000 */
    CORRADE_COMPARE(Math::unpackHalf(0x4200), 3.0f);

    CORRADE_COMPARE(unpackNaive(0x0000), 0.0f);
    CORRADE_COMPARE(unpackNaive(0x3c00), 1.0f);
    CORRADE_COMPARE(unpackNaive(0x4000), 2.0f);
    CORRADE_COMPARE(unpackNaive(0x4200), 3.0f);

    CORRADE_COMPARE(unpackTable(0x0000), 0.0f);
    CORRADE_COMPARE(unpackTable(0x3c00), 1.0f);
    CORRADE_COMPARE(unpackTable(0x4000), 2.0f);
    CORRADE_COMPARE(unpackTable(0x4200), 3.0f);

    /* Normals, denormals, specials */
    CORRADE_COMPARE(Math::unpackHalf(0x8dc2), -0.000351f);
    CORRADE_COMPARE(Math::unpackHalf(0x57bc), 123.75f);
    CORRADE_COMPARE(Math::unpackHalf(0xfe00), -Constants::nan());
    CORRADE_COMPARE(Math::unpackHalf(0x7e00), +Constants::nan());
    CORRADE_COMPARE(Math::unpackHalf(0xfc00), -Constants::inf());
    CORRADE_COMPARE(Math::unpackHalf(0x7c00), +Constants::inf());

    CORRADE_COMPARE(unpackNaive(0x8dc2), -0.000351f);
    CORRADE_COMPARE(unpackNaive(0x57bc), 123.75f);
    CORRADE_COMPARE(unpackNaive(0xfe00), -Constants::nan());
    CORRADE_COMPARE(unpackNaive(0x7e00), +Constants::nan());
    CORRADE_COMPARE(unpackNaive(0xfc00), -Constants::inf());
    CORRADE_COMPARE(unpackNaive(0x7c00), +Constants::inf());

    CORRADE_COMPARE(unpackTable(0x8dc2), -0.000351f);
    CORRADE_COMPARE(unpackTable(0x57bc), 123.75f);
    CORRADE_COMPARE(unpackTable(0xfe00), -Constants::nan());
    CORRADE_COMPARE(unpackTable(0x7e00), +Constants::nan());
    CORRADE_COMPARE(unpackTable(0xfc00), -Constants::inf());
    CORRADE_COMPARE(unpackTable(0x7c00), +Constants::inf());

    /* Vector */
    CORRADE_COMPARE(Math::unpackHalf(Math::Vector3<UnsignedShort>{0x0000, 0x4200, 0x3c00}),
        (Math::Vector3<Float>{0.0f, 3.0f, 1.0f}));
}

void HalfTest::pack() {
    CORRADE_COMPARE(Math::packHalf(0.0f), 0x0000);

    /* 0b0011110000000000 */
    CORRADE_COMPARE(Math::packHalf(1.0f), 0x3c00);

    /* 0b0100000000000000 */
    CORRADE_COMPARE(Math::packHalf(2.0f), 0x4000);

    /* 0b0100001000000000 */
    CORRADE_COMPARE(Math::packHalf(3.0f), 0x4200);

    CORRADE_COMPARE(packNaive(0.0f), 0x0000);
    CORRADE_COMPARE(packNaive(1.0f), 0x3c00);
    CORRADE_COMPARE(packNaive(2.0f), 0x4000);
    CORRADE_COMPARE(packNaive(3.0f), 0x4200);

    CORRADE_COMPARE(packTable(0.0f), 0x0000);
    CORRADE_COMPARE(packTable(1.0f), 0x3c00);
    CORRADE_COMPARE(packTable(2.0f), 0x4000);
    CORRADE_COMPARE(packTable(3.0f), 0x4200);

    /* Rounding */
    CORRADE_COMPARE(Math::unpackHalf(Math::packHalf(-1024.01f)), -1024.0f);
    CORRADE_COMPARE(Math::unpackHalf(Math::packHalf(-1024.50f)), -1025.0f);
    CORRADE_COMPARE(Math::unpackHalf(Math::packHalf(-1024.99f)), -1025.0f);
    CORRADE_COMPARE(Math::unpackHalf(Math::packHalf(+1024.01f)), +1024.0f);
    CORRADE_COMPARE(Math::unpackHalf(Math::packHalf(+1024.50f)), +1025.0f);
    CORRADE_COMPARE(Math::unpackHalf(Math::packHalf(+1024.99f)), +1025.0f);

    /* Don't care about rounding behavior of the others */

    /* Normals, denormals, specials */
    CORRADE_COMPARE(Math::packHalf(-0.000351512f), 0x8dc2);
    CORRADE_COMPARE(Math::packHalf(123.7567f), 0x57bc);
    /* Emscripten doesn't differentiate NaNs and treats their sign slightly
       differently on different optimization levels. On MSVC they are somehow
       flipped around, so I'm testing w/o the sign. */
    CORRADE_COMPARE(Math::packHalf(-Constants::nan()) & ~0x8000, 0x7e00);
    CORRADE_COMPARE(Math::packHalf(+Constants::nan()) & ~0x8000, 0x7e00);
    CORRADE_COMPARE(Math::packHalf(-Constants::inf()), 0xfc00);
    CORRADE_COMPARE(Math::packHalf(+Constants::inf()), 0x7c00);

    CORRADE_COMPARE(packNaive(-0.000351512f), 0x8dc2);
    CORRADE_COMPARE(packNaive(123.7567f), 0x57bc);
    /* Emscripten doesn't differentiate NaNs and treats their sign slightly
       differently on different optimization levels. On MSVC they are somehow
       flipped around, so I'm testing w/o the sign. */
    CORRADE_COMPARE(packNaive(-Constants::nan()) & ~0x8000, 0x7e00);
    CORRADE_COMPARE(packNaive(+Constants::nan()) & ~0x8000, 0x7e00);
    CORRADE_COMPARE(packNaive(-Constants::inf()), 0xfc00);
    CORRADE_COMPARE(packNaive(+Constants::inf()), 0x7c00);

    CORRADE_COMPARE(packTable(-0.000351512f), 0x8dc2);
    CORRADE_COMPARE(packTable(123.7567f), 0x57bc);
    /* Emscripten doesn't differentiate NaNs and treats their sign slightly
       differently on different optimization levels. On MSVC they are somehow
       flipped around, so I'm testing w/o the sign. */
    CORRADE_COMPARE(packTable(-Constants::nan()) & ~0x8000, 0x7e00);
    CORRADE_COMPARE(packTable(+Constants::nan()) & ~0x8000, 0x7e00);
    CORRADE_COMPARE(packTable(-Constants::inf()), 0xfc00);
    CORRADE_COMPARE(packTable(+Constants::inf()), 0x7c00);

    /* Vector */
    CORRADE_COMPARE(Math::packHalf(Math::Vector3<Float>{0.0f, 3.0f, 1.0f}),
        (Math::Vector3<UnsignedShort>{0x0000, 0x4200, 0x3c00}));
}

void HalfTest::repack() {
    UnsignedShort in = testCaseRepeatId();
    Float result = Math::unpackHalf(in);
    Float resultNaive = unpackNaive(in);
    Float resultTable = unpackTable(in);

    /* NaNs don't rountrip, but that's okay */
    if(result != result) {
        CORRADE_VERIFY(result != result);
        CORRADE_VERIFY(resultNaive != resultNaive);
        CORRADE_VERIFY(resultTable != resultTable);

    /* Otherwise verify that all algos give the same results */
    } else {
        CORRADE_COMPARE(result, resultTable);
        CORRADE_COMPARE(result, resultNaive);

        CORRADE_COMPARE(Math::packHalf(result), in);
        CORRADE_COMPARE(packTable(result), in);
        CORRADE_COMPARE(packNaive(result), in);
    }
}

void HalfTest::pack1k() {
    UnsignedInt out = 0;
    CORRADE_BENCHMARK(100)
        for(std::uint_fast16_t i = 0; i != 1000; ++i)
            out += Math::packHalf(Float(i)*65);

    /* To avoid optimizing things out */
    CORRADE_VERIFY(out);
}

void HalfTest::pack1kNaive() {
    UnsignedInt out = 0;
    CORRADE_BENCHMARK(100)
        for(std::uint_fast16_t i = 0; i != 1000; ++i)
            out += packNaive(Float(i)*65);

    /* To avoid optimizing things out */
    CORRADE_VERIFY(out);
}

void HalfTest::pack1kTable() {
    Float src[1000];
    UnsignedShort dst[1000];
    for(std::uint_fast16_t i = 0; i != 1000; ++i)
        src[i] = i*65;

    CORRADE_BENCHMARK(100) {
        packHalfInto(Corrade::Containers::StridedArrayView2D<Float>{src, {1, 1000}},
            Corrade::Containers::StridedArrayView2D<UnsignedShort>{dst, {1, 1000}});
    }
}

void HalfTest::unpack1k() {
    Float out = 0.0f;
    CORRADE_BENCHMARK(100)
        for(std::uint_fast16_t i = 0; i != 1000; ++i)
            out += Math::unpackHalf(i*65);

    /* To avoid optimizing things out */
    CORRADE_VERIFY(out);
}

void HalfTest::unpack1kNaive() {
    Float out = 0.0f;
    CORRADE_BENCHMARK(100)
        for(std::uint_fast16_t i = 0; i != 1000; ++i)
            out += unpackNaive(i*65);

    /* To avoid optimizing things out */
    CORRADE_VERIFY(out);
}

void HalfTest::unpack1kTable() {
    UnsignedShort src[1000];
    Float dst[1000];
    for(std::uint_fast16_t i = 0; i != 1000; ++i)
        src[i] = i*65;

    CORRADE_BENCHMARK(100) {
        unpackHalfInto(Corrade::Containers::StridedArrayView2D<UnsignedShort>{src, {1, 1000}},
            Corrade::Containers::StridedArrayView2D<Float>{dst, {1, 1000}});
    }
}

void HalfTest::constructDefault() {
    constexpr Half a;
    CORRADE_COMPARE(Float(a), 0.0f);
    CORRADE_COMPARE(UnsignedShort(a), 0);
    CORRADE_COMPARE(a.data(), 0);

    constexpr Half b{ZeroInit};
    CORRADE_COMPARE(Float(b), 0.0f);
    CORRADE_COMPARE(UnsignedShort(b), 0);
    CORRADE_COMPARE(b.data(), 0);

    CORRADE_VERIFY((std::is_nothrow_default_constructible<Half>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Half, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Half>::value));
}

void HalfTest::constructValue() {
    Half a{3.5f};
    Half b{3.5};
    CORRADE_COMPARE(Float(a), 3.5f);
    CORRADE_COMPARE(Float(b), 3.5f);
    CORRADE_COMPARE(UnsignedShort(a), 0x4300);
    CORRADE_COMPARE(UnsignedShort(b), 0x4300);
    CORRADE_COMPARE(a.data(), 0x4300);
    CORRADE_COMPARE(b.data(), 0x4300);

    CORRADE_VERIFY((std::is_nothrow_constructible<Half, Float>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Half, Double>::value));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Half>::value));
    CORRADE_VERIFY(!(std::is_convertible<Double, Half>::value));
}

void HalfTest::constructData() {
    constexpr Half a{UnsignedShort(0x4300)};
    CORRADE_COMPARE(Float(a), 3.5f);
    CORRADE_COMPARE(UnsignedShort(a), 0x4300);

    CORRADE_VERIFY((std::is_nothrow_constructible<Half, UnsignedShort>::value));

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<UnsignedShort, Half>::value));
}

void HalfTest::constructNoInit() {
    Half a{3.5f};
    new(&a) Half{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601 && __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        CORRADE_COMPARE(a, Half{3.5f});
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Half, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Half>::value));
}

void HalfTest::constructCopy() {
    constexpr Half a{UnsignedShort(0x4300)};
    constexpr Half b{a};

    CORRADE_COMPARE(b, Half{3.5f});

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Half>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Half>::value);
}

void HalfTest::compare() {
    constexpr Half a{UnsignedShort(0x4300)};
    constexpr Half b{UnsignedShort(0x4301)};

    CORRADE_VERIFY(a == a);
    CORRADE_VERIFY(a != b);
}

void HalfTest::compareNaN() {
    Half a{UnsignedShort(testCaseRepeatId())};
    Float fa(a);

    /* If the values are NaNs as Float, they should also not compare as Half.
       Interestingly enough, writing Float(a) != Float(a) in expected confused
       the optimizer in Emscripten on higher optimization levels and it was
       always saying `true` for values >= 32769 (which is *not* right). */
    CORRADE_COMPARE(a != a, fa != fa);
}

void HalfTest::promotion() {
    constexpr Half a{UnsignedShort(0x4300)};
    constexpr Half b = +a;

    CORRADE_COMPARE(b, a);
}

void HalfTest::negation() {
    constexpr Half a{UnsignedShort(0x4300)};
    constexpr Half b = -a;

    CORRADE_COMPARE(b, Half{-3.5f});
    CORRADE_COMPARE(-b, a);
}

void HalfTest::strictWeakOrdering() {
    StrictWeakOrdering o;
    constexpr Half a{UnsignedShort(0x4300)};
    constexpr Half b{UnsignedShort(0x5100)};

    CORRADE_VERIFY( o(a, b));
    CORRADE_VERIFY(!o(b, a));
    CORRADE_VERIFY(!o(a, a));
}

void HalfTest::literal() {
    Half a = 3.5_h;
    CORRADE_COMPARE(a, Half{UnsignedShort(0x4300)});
    CORRADE_COMPARE(a, Half{3.5f});
}

void HalfTest::debug() {
    std::ostringstream out;

    Debug{&out} << -36.41_h << Half{Constants::inf()}
        << Math::Vector3<Half>{3.14159_h, -1.4142_h, 1.618_h};
    #ifdef _MSC_VER
    CORRADE_COMPARE(out.str(), "-36.41 inf Vector(3.141, -1.414, 1.618)\n");
    #else
    CORRADE_COMPARE(out.str(), "-36.41 inf Vector(3.141, -1.414, 1.618)\n");
    #endif
}

#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
void HalfTest::tweakable() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    Corrade::Utility::TweakableState state;
    Half result;
    std::tie(state, result) = Corrade::Utility::TweakableParser<Half>::parse(data.data);
    CORRADE_COMPARE(state, Corrade::Utility::TweakableState::Success);
    CORRADE_COMPARE(result, data.result);
}

void HalfTest::tweakableError() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    std::ostringstream out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Corrade::Utility::TweakableState state = Corrade::Utility::TweakableParser<Half>::parse(data.data).first;
    CORRADE_COMPARE(out.str(), data.error);
    CORRADE_COMPARE(state, data.state);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::HalfTest)
