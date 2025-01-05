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
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/TweakableParser.h>
#endif

#include "Magnum/Math/Angle.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct AngleTest: TestSuite::Tester {
    explicit AngleTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructConversion();
    void constructCopy();
    void constructFromBase();

    void literals();
    void conversion();

    void debugDeg();
    void debugDegPacked();
    void debugRad();
    void debugRadPacked();
    #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    template<class T> void tweakable();
    template<class T> void tweakableError();
    #endif
};

using Magnum::Deg;
using Magnum::Rad;
using Magnum::Degd;
using Magnum::Radd;

using namespace Literals;

#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
constexpr struct {
    const char* name;
    const char* data;
    float result;
} TweakableData[] {
    {"fixed", "35.0_{}", 35.0f},
    {"no zero before", ".5_{}",  0.5f},
    {"no zero after", "35._{}", 35.0f},
    {"positive", "+35.0_{}", 35.0f},
    {"negative", "-35.0_{}", -35.0}
};

constexpr struct {
    const char* name;
    const char* data;
    Utility::TweakableState state;
    const char* error;
} TweakableErrorData[] {
    {"empty", "", Utility::TweakableState::Recompile,
        "Utility::TweakableParser:  is not an angle literal\n"},
    {"integral", "42_{}", Utility::TweakableState::Recompile,
        "Utility::TweakableParser: 42_{} is not an angle literal\n"},
    {"garbage after", "42.b_{}", Utility::TweakableState::Recompile,
        "Utility::TweakableParser: unexpected characters b_{} after an angle literal\n"},
    {"different suffix", "42.0u", Utility::TweakableState::Recompile, /* not for double */
        "Utility::TweakableParser: 42.0u has an unexpected suffix, expected _{}\n"}
};

template<class> struct TweakableTraits;
template<> struct TweakableTraits<Deg> {
    static const char* name() { return "Deg"; }
    static const char* literal() { return "degf"; }
};
template<> struct TweakableTraits<Unit<Math::Deg, Float>> {
    static const char* name() { return "Unit<Deg, Float>"; }
    static const char* literal() { return "degf"; }
};
template<> struct TweakableTraits<Degd> {
    static const char* name() { return "Degd"; }
    static const char* literal() { return "deg"; }
};
template<> struct TweakableTraits<Unit<Math::Deg, Double>> {
    static const char* name() { return "Unit<Deg, Double>"; }
    static const char* literal() { return "deg"; }
};
template<> struct TweakableTraits<Rad> {
    static const char* name() { return "Rad"; }
    static const char* literal() { return "radf"; }
};
template<> struct TweakableTraits<Unit<Math::Rad, Float>> {
    static const char* name() { return "Unit<Rad, Float>"; }
    static const char* literal() { return "radf"; }
};
template<> struct TweakableTraits<Radd> {
    static const char* name() { return "Radd"; }
    static const char* literal() { return "rad"; }
};
template<> struct TweakableTraits<Unit<Math::Rad, Double>> {
    static const char* name() { return "Unit<Rad, Double>"; }
    static const char* literal() { return "rad"; }
};
#endif

AngleTest::AngleTest() {
    addTests({&AngleTest::construct,
              &AngleTest::constructDefault,
              &AngleTest::constructNoInit,
              &AngleTest::constructConversion,
              &AngleTest::constructCopy,
              &AngleTest::constructFromBase,

              &AngleTest::literals,
              &AngleTest::conversion,

              &AngleTest::debugDeg,
              &AngleTest::debugDegPacked,
              &AngleTest::debugRad,
              &AngleTest::debugRadPacked});

    #if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    addInstancedTests<AngleTest>({
        &AngleTest::tweakable<Deg>,
        &AngleTest::tweakable<Unit<Math::Deg, Float>>,
        &AngleTest::tweakable<Degd>,
        &AngleTest::tweakable<Unit<Math::Deg, Double>>,
        &AngleTest::tweakable<Rad>,
        &AngleTest::tweakable<Unit<Math::Rad, Float>>,
        &AngleTest::tweakable<Radd>,
        &AngleTest::tweakable<Unit<Math::Rad, Double>>},
        Containers::arraySize(TweakableData));

    addInstancedTests<AngleTest>({
        &AngleTest::tweakableError<Deg>,
        &AngleTest::tweakableError<Unit<Math::Deg, Float>>,
        &AngleTest::tweakableError<Degd>,
        &AngleTest::tweakableError<Unit<Math::Deg, Double>>,
        &AngleTest::tweakableError<Rad>,
        &AngleTest::tweakableError<Unit<Math::Rad, Float>>,
        &AngleTest::tweakableError<Radd>,
        &AngleTest::tweakableError<Unit<Math::Rad, Double>>},
        Containers::arraySize(TweakableErrorData));
    #endif
}

void AngleTest::construct() {
    Deg a{25.0f};
    Radd b{3.14};
    CORRADE_COMPARE(Float(a), 25.0f);
    CORRADE_COMPARE(Double(b), 3.14);

    constexpr Deg ca{25.0f};
    constexpr Radd cb(3.14);
    CORRADE_COMPARE(Float(ca), 25.0f);
    CORRADE_COMPARE(Double(cb), 3.14);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Float, Rad>::value);
    CORRADE_VERIFY(!std::is_convertible<Double, Degd>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Deg, Float>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Rad, Float>::value);
}

void AngleTest::constructDefault() {
    Deg a1;
    Deg a2{ZeroInit};
    Radd b1;
    Radd b2{ZeroInit};
    CORRADE_COMPARE(Float(a1), 0.0f);
    CORRADE_COMPARE(Float(a2), 0.0f);
    CORRADE_COMPARE(Double(b1), 0.0);
    CORRADE_COMPARE(Double(b2), 0.0);

    constexpr Deg ca1;
    constexpr Deg ca2{ZeroInit};
    constexpr Radd cb1;
    constexpr Radd cb2{ZeroInit};
    CORRADE_COMPARE(Float(ca1), 0.0f);
    CORRADE_COMPARE(Float(ca2), 0.0f);
    CORRADE_COMPARE(Double(cb1), 0.0);
    CORRADE_COMPARE(Double(cb2), 0.0);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Deg>::value);
    CORRADE_VERIFY(std::is_nothrow_default_constructible<Rad>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Deg, ZeroInitT>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Rad, ZeroInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Deg>::value);
    CORRADE_VERIFY(!std::is_convertible<ZeroInitT, Rad>::value);
}

void AngleTest::constructNoInit() {
    Deg a{25.0f};
    Rad b{3.14f};
    new(&a) Deg{Magnum::NoInit};
    new(&b) Rad{Magnum::NoInit};
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
        CORRADE_COMPARE(Float(a), 25.0f);
        CORRADE_COMPARE(Float(b), 3.14f);
        #if defined(CORRADE_TARGET_GCC) && !defined(CORRADE_TARGET_CLANG) && __GNUC__*100 + __GNUC_MINOR__ >= 601
        #pragma GCC diagnostic pop
        #endif
    }

    CORRADE_VERIFY(std::is_nothrow_constructible<Deg, Magnum::NoInitT>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Rad, Magnum::NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Deg>::value);
    CORRADE_VERIFY(!std::is_convertible<Magnum::NoInitT, Rad>::value);
}

void AngleTest::constructConversion() {
    Deg a{25.0f};
    Radd b{3.14};
    Rad c{b};
    Degd d{a};
    CORRADE_COMPARE(Float(c), 3.14f);
    CORRADE_COMPARE(Double(d), 25.0);

    constexpr Deg ca{25.0f};
    constexpr Radd cb{3.14};
    constexpr Rad cc{cb};
    constexpr Degd cd{ca};
    CORRADE_COMPARE(Float(cc), 3.14f);
    CORRADE_COMPARE(Double(cd), 25.0);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!std::is_convertible<Degd, Deg>::value);
    CORRADE_VERIFY(!std::is_convertible<Rad, Radd>::value);

    CORRADE_VERIFY(std::is_nothrow_constructible<Deg, Degd>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Radd, Rad>::value);
}

void AngleTest::constructCopy() {
    constexpr Deg a(25.0);
    constexpr Radd b(3.14);

    constexpr Deg c(a);
    CORRADE_COMPARE(c, a);
    constexpr Radd d(b);
    CORRADE_COMPARE(d, b);

    #ifndef CORRADE_NO_STD_IS_TRIVIALLY_TRAITS
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Deg>::value);
    CORRADE_VERIFY(std::is_trivially_copy_constructible<Rad>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Deg>::value);
    CORRADE_VERIFY(std::is_trivially_copy_assignable<Rad>::value);
    #endif
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Deg>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Rad>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Deg>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Rad>::value);
}

void AngleTest::constructFromBase() {
    /* The operation returns Unit instead of the leaf type, so this can work
       only if the base class has a "copy constructor" from the base type */
    Deg a = 35.0_degf + 0.15_degf;
    Radd b = 1.0_rad + 0.25_rad;
    CORRADE_COMPARE(a, 35.15_degf);
    CORRADE_COMPARE(b, 1.25_rad);
}

void AngleTest::literals() {
    constexpr auto a = 25.0_deg;
    CORRADE_VERIFY(std::is_same<decltype(a), const Degd>::value);
    CORRADE_COMPARE(Double(a), 25.0);
    constexpr auto b = 25.0_degf;
    CORRADE_VERIFY(std::is_same<decltype(b), const Deg>::value);
    CORRADE_COMPARE(Float(b), 25.0f);

    constexpr auto m = 3.14_rad;
    CORRADE_VERIFY(std::is_same<decltype(m), const Radd>::value);
    CORRADE_COMPARE(Double(m), 3.14);
    constexpr auto n = 3.14_radf;
    CORRADE_VERIFY(std::is_same<decltype(n), const Rad>::value);
    CORRADE_COMPARE(Float(n), 3.14f);
}

void AngleTest::conversion() {
    /* Implicit conversion should be allowed */
    constexpr Deg a = 1.57079633_radf;
    CORRADE_COMPARE(Float(a), 90.0f);

    constexpr Rad b = 90.0_degf;
    CORRADE_COMPARE(Float(b), 1.57079633f);

    CORRADE_VERIFY(std::is_nothrow_constructible<Deg, Rad>::value);
    CORRADE_VERIFY(std::is_nothrow_constructible<Radd, Degd>::value);
}

void AngleTest::debugDeg() {
    Containers::String out;

    Debug{&out} << 90.0_degf;
    CORRADE_COMPARE(out, "Deg(90)\n");

    /* Verify that this compiles */
    out = {};
    Debug{&out} << 56.0_degf - 34.0_degf;
    CORRADE_COMPARE(out, "Deg(22)\n");
}

void AngleTest::debugDegPacked() {
    Containers::String out;

    /* Second is not packed, the first should not make any flags persistent */
    Debug{&out} << Debug::packed << 90.0_degf << 45.0_degf;
    CORRADE_COMPARE(out, "90 Deg(45)\n");
}

void AngleTest::debugRad() {
    Containers::String out;

    Debug{&out} << 1.5708_radf;
    CORRADE_COMPARE(out, "Rad(1.5708)\n");

    /* Verify that this compiles */
    out = {};
    Debug{&out} << 1.5708_radf - 3.1416_radf;
    CORRADE_COMPARE(out, "Rad(-1.5708)\n");
}

void AngleTest::debugRadPacked() {
    Containers::String out;

    /* Second is not packed, the first should not make any flags persistent */
    Debug{&out} << Debug::packed << 1.5708_radf << 3.1416_radf;
    CORRADE_COMPARE(out, "1.5708 Rad(3.1416)\n");
}

#if defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
template<class T> void AngleTest::tweakable() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseTemplateName(TweakableTraits<T>::name());
    setTestCaseDescription(data.name);
    auto result = Utility::TweakableParser<T>::parse(Utility::format(data.data, TweakableTraits<T>::literal()));
    CORRADE_COMPARE(result.first(), Utility::TweakableState::Success);
    CORRADE_COMPARE(result.second(), T(typename T::Type(data.result)));
}

template<class T> void AngleTest::tweakableError() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseTemplateName(TweakableTraits<T>::name());
    setTestCaseDescription(data.name);

    Containers::String out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Utility::TweakableState state = Utility::TweakableParser<T>::parse(Utility::format(data.data, TweakableTraits<T>::literal())).first();
    CORRADE_COMPARE(out, Utility::format(data.error, TweakableTraits<T>::literal()));
    CORRADE_COMPARE(state, data.state);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::AngleTest)
