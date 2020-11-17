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
#include <sstream>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>
#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Corrade/Containers/StringStl.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/TweakableParser.h>
#endif

#include "Magnum/Math/Angle.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct AngleTest: Corrade::TestSuite::Tester {
    explicit AngleTest();

    void construct();
    void constructDefault();
    void constructNoInit();
    void constructConversion();
    void constructCopy();

    void literals();
    void conversion();

    void debugDeg();
    void debugRad();
    #if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    template<class T> void tweakable();
    template<class T> void tweakableError();
    #endif
};

typedef Math::Deg<Float> Deg;
typedef Math::Rad<Float> Rad;
typedef Math::Deg<Double> Degd;
typedef Math::Rad<Double> Radd;

using namespace Literals;

#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
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
    Corrade::Utility::TweakableState state;
    const char* error;
} TweakableErrorData[] {
    {"empty", "", Corrade::Utility::TweakableState::Recompile,
        "Utility::TweakableParser:  is not an angle literal\n"},
    {"integral", "42_{}", Corrade::Utility::TweakableState::Recompile,
        "Utility::TweakableParser: 42_{} is not an angle literal\n"},
    {"garbage after", "42.b_{}", Corrade::Utility::TweakableState::Recompile,
        "Utility::TweakableParser: unexpected characters b_{} after an angle literal\n"},
    {"different suffix", "42.0u", Corrade::Utility::TweakableState::Recompile, /* not for double */
        "Utility::TweakableParser: 42.0u has an unexpected suffix, expected _{}\n"}
};

template<class> struct TweakableTraits;
template<> struct TweakableTraits<Deg> {
    static const char* name() { return "Deg"; }
    static const char* literal() { return "degf"; }
};
template<> struct TweakableTraits<Degd> {
    static const char* name() { return "Degd"; }
    static const char* literal() { return "deg"; }
};
template<> struct TweakableTraits<Rad> {
    static const char* name() { return "Rad"; }
    static const char* literal() { return "radf"; }
};
template<> struct TweakableTraits<Radd> {
    static const char* name() { return "Radd"; }
    static const char* literal() { return "rad"; }
};
#endif

AngleTest::AngleTest() {
    addTests({&AngleTest::construct,
              &AngleTest::constructDefault,
              &AngleTest::constructNoInit,
              &AngleTest::constructConversion,
              &AngleTest::constructCopy,

              &AngleTest::literals,
              &AngleTest::conversion,

              &AngleTest::debugDeg,
              &AngleTest::debugRad});

    #if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
    addInstancedTests<AngleTest>({
        &AngleTest::tweakable<Deg>,
        &AngleTest::tweakable<Degd>,
        &AngleTest::tweakable<Rad>,
        &AngleTest::tweakable<Radd>},
        Corrade::Containers::arraySize(TweakableData));

    addInstancedTests<AngleTest>({
        &AngleTest::tweakableError<Deg>,
        &AngleTest::tweakableError<Degd>,
        &AngleTest::tweakableError<Rad>,
        &AngleTest::tweakableError<Radd>},
        Corrade::Containers::arraySize(TweakableErrorData));
    #endif
}

void AngleTest::construct() {
    constexpr Deg b(25.0);
    CORRADE_COMPARE(Float(b), 25.0f);
    constexpr Radd n(3.14);
    CORRADE_COMPARE(Double(n), 3.14);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Float, Rad>::value));
    CORRADE_VERIFY(!(std::is_convertible<Double, Degd>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Deg, Float>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Rad, Float>::value));
}

void AngleTest::constructDefault() {
    constexpr Deg m1;
    constexpr Deg m2{ZeroInit};
    CORRADE_COMPARE(Float(m1), 0.0f);
    CORRADE_COMPARE(Float(m2), 0.0f);
    constexpr Radd a1;
    constexpr Radd a2{ZeroInit};
    CORRADE_COMPARE(Double(a1), 0.0);
    CORRADE_COMPARE(Double(a2), 0.0);

    CORRADE_VERIFY(std::is_nothrow_default_constructible<Deg>::value);
    CORRADE_VERIFY(std::is_nothrow_default_constructible<Rad>::value);
    CORRADE_VERIFY((std::is_nothrow_constructible<Deg, ZeroInitT>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Rad, ZeroInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Deg>::value));
    CORRADE_VERIFY(!(std::is_convertible<ZeroInitT, Rad>::value));
}

void AngleTest::constructNoInit() {
    Deg a{25.0f};
    Rad b{3.14f};
    new(&a) Deg{Magnum::NoInit};
    new(&b) Rad{Magnum::NoInit};
    {
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601
        /* The warning is reported for both debug and release build */
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
        #ifdef __OPTIMIZE__
        CORRADE_EXPECT_FAIL("GCC 6.1+ misoptimizes and overwrites the value.");
        #endif
        #endif
        CORRADE_COMPARE(Float(a), 25.0f);
        CORRADE_COMPARE(Float(b), 3.14f);
        #if defined(__GNUC__) && __GNUC__*100 + __GNUC_MINOR__ >= 601
        #pragma GCC diagnostic pop
        #endif
    }

    CORRADE_VERIFY((std::is_nothrow_constructible<Deg, Magnum::NoInitT>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Rad, Magnum::NoInitT>::value));

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Deg>::value));
    CORRADE_VERIFY(!(std::is_convertible<Magnum::NoInitT, Rad>::value));
}

void AngleTest::constructConversion() {
    constexpr Deg a(25.0);
    constexpr Radd b(3.14);

    constexpr Rad c(b);
    CORRADE_COMPARE(Float(c), 3.14f);
    constexpr Degd d(a);
    CORRADE_COMPARE(Double(d), 25.0);

    /* Implicit conversion is not allowed */
    CORRADE_VERIFY(!(std::is_convertible<Degd, Deg>::value));
    CORRADE_VERIFY(!(std::is_convertible<Rad, Radd>::value));

    CORRADE_VERIFY((std::is_nothrow_constructible<Deg, Degd>::value));
    CORRADE_VERIFY((std::is_nothrow_constructible<Radd, Rad>::value));
}

void AngleTest::constructCopy() {
    constexpr Deg a(25.0);
    constexpr Radd b(3.14);

    constexpr Deg c(a);
    CORRADE_COMPARE(c, a);
    constexpr Radd d(b);
    CORRADE_COMPARE(d, b);

    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Deg>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_constructible<Rad>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Deg>::value);
    CORRADE_VERIFY(std::is_nothrow_copy_assignable<Rad>::value);
}

void AngleTest::literals() {
    constexpr auto a = 25.0_deg;
    CORRADE_VERIFY((std::is_same<decltype(a), const Degd>::value));
    CORRADE_COMPARE(Double(a), 25.0);
    constexpr auto b = 25.0_degf;
    CORRADE_VERIFY((std::is_same<decltype(b), const Deg>::value));
    CORRADE_COMPARE(Float(b), 25.0f);

    constexpr auto m = 3.14_rad;
    CORRADE_VERIFY((std::is_same<decltype(m), const Radd>::value));
    CORRADE_COMPARE(Double(m), 3.14);
    constexpr auto n = 3.14_radf;
    CORRADE_VERIFY((std::is_same<decltype(n), const Rad>::value));
    CORRADE_COMPARE(Float(n), 3.14f);
}

void AngleTest::conversion() {
    /* Implicit conversion should be allowed */
    constexpr Deg a = Rad(1.57079633f);
    CORRADE_COMPARE(Float(a), 90.0f);

    constexpr Rad b = 90.0_degf;
    CORRADE_COMPARE(Float(b), 1.57079633f);
}

void AngleTest::debugDeg() {
    std::ostringstream o;

    Debug(&o) << 90.0_degf;
    CORRADE_COMPARE(o.str(), "Deg(90)\n");

    /* Verify that this compiles */
    o.str({});
    Debug(&o) << 56.0_degf - 34.0_degf;
    CORRADE_COMPARE(o.str(), "Deg(22)\n");
}

void AngleTest::debugRad() {
    std::ostringstream o;

    Debug(&o) << 1.5708_radf;
    CORRADE_COMPARE(o.str(), "Rad(1.5708)\n");

    /* Verify that this compiles */
    o.str({});
    Debug(&o) << 1.5708_radf - 3.1416_radf;
    CORRADE_COMPARE(o.str(), "Rad(-1.5708)\n");
}

#if defined(DOXYGEN_GENERATING_OUTPUT) || defined(CORRADE_TARGET_UNIX) || (defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)) || defined(CORRADE_TARGET_EMSCRIPTEN)
template<class T> void AngleTest::tweakable() {
    auto&& data = TweakableData[testCaseInstanceId()];
    setTestCaseTemplateName(TweakableTraits<T>::name());
    setTestCaseDescription(data.name);
    std::string input = Corrade::Utility::formatString(data.data, TweakableTraits<T>::literal());
    Corrade::Utility::TweakableState state;
    T result;
    std::tie(state, result) = Corrade::Utility::TweakableParser<T>::parse(input);
    CORRADE_COMPARE(state, Corrade::Utility::TweakableState::Success);
    CORRADE_COMPARE(result, T(typename T::Type(data.result)));
}

template<class T> void AngleTest::tweakableError() {
    auto&& data = TweakableErrorData[testCaseInstanceId()];
    setTestCaseTemplateName(TweakableTraits<T>::name());
    setTestCaseDescription(data.name);
    std::string input = Corrade::Utility::formatString(data.data, TweakableTraits<T>::literal());

    std::ostringstream out;
    Warning redirectWarning{&out};
    Error redirectError{&out};
    Corrade::Utility::TweakableState state = Corrade::Utility::TweakableParser<T>::parse(input).first;
    CORRADE_COMPARE(out.str(), Corrade::Utility::formatString(data.error, TweakableTraits<T>::literal()));
    CORRADE_COMPARE(state, data.state);
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::AngleTest)
