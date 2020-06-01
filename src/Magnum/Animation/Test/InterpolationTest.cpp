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

#include "Magnum/Animation/Easing.h"
#include "Magnum/Animation/Interpolation.h"
#include "Magnum/Math/Complex.h"
#include "Magnum/Math/CubicHermite.h"
#include "Magnum/Math/DualQuaternion.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Packing.h"

namespace Magnum { namespace Animation { namespace Test { namespace {

struct InterpolationTest: TestSuite::Tester {
    explicit InterpolationTest();

    void interpolatorFor();
    void interpolatorForInvalid();
    void interpolatorForBool();
    void interpolatorForBoolInvalid();
    void interpolatorForBoolVector();
    void interpolatorForBoolVectorInvalid();
    void interpolatorForComplex();
    void interpolatorForComplexInvalid();
    void interpolatorForQuaternion();
    void interpolatorForQuaternionInvalid();
    void interpolatorForDualQuaternion();
    void interpolatorForDualQuaternionInvalid();
    void interpolatorForCubicHermiteScalar();
    void interpolatorForCubicHermiteScalarInvalid();
    void interpolatorForCubicHermiteVector();
    void interpolatorForCubicHermiteVectorInvalid();
    void interpolatorForCubicHermiteComplex();
    void interpolatorForCubicHermiteComplexInvalid();
    void interpolatorForCubicHermiteQuaternion();
    void interpolatorForCubicHermiteQuaternionInvalid();

    void interpolate();
    void interpolateStrict();
    void interpolateSingleKeyframe();
    void interpolateNoKeyframe();

    void interpolateHint();
    void interpolateStrictHint();

    void interpolateDifferentResultType();
    void interpolateStrictDifferentResultType();

    void interpolateError();
    void interpolateStrictError();

    void interpolateIntegerKey();
    void interpolateStrictIntegerKey();

    void ease();
    void easeClamped();
    void unpack();
    void unpackEase();
    void unpackEaseClamped();

    void debugInterpolation();
    void debugExtrapolation();
};

using namespace Math::Literals;

const struct {
    const char* name;
    Extrapolation extrapolationBefore;
    Extrapolation extrapolationAfter;
    Float time;
    Float expectedValue, expectedValueStrict;
    std::size_t expectedHint;
} Data[] {
    {"before default-constructed",
        Extrapolation::DefaultConstructed, Extrapolation::Extrapolated,
        -1.0f, 0.0f, 4.0f, 0},
    {"before constant",
        Extrapolation::Constant, Extrapolation::Extrapolated,
        -1.0f, 3.0f, 4.0f, 0},
    {"before extrapolated",
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed,
        -1.0f, 4.0f, 4.0f, 0},
    {"during first",
        Extrapolation::DefaultConstructed, Extrapolation::DefaultConstructed,
        1.5f, 1.5f, 1.5f, 0},
    {"during second",
        Extrapolation::DefaultConstructed, Extrapolation::DefaultConstructed,
        4.75f, 1.0f, 1.0f, 2},
    {"after default-constructed",
        Extrapolation::Extrapolated, Extrapolation::DefaultConstructed,
        6.0f, 0.0f, -1.5f, 2},
    {"after constant",
        Extrapolation::Extrapolated, Extrapolation::Constant,
        6.0f, 0.5f, -1.5f, 2},
    {"after extrapolated",
        Extrapolation::DefaultConstructed, Extrapolation::Extrapolated,
        6.0f, -1.5f, -1.5f, 2}
};

const struct {
    const char* name;
    Extrapolation extrapolation;
    Float time;
    Float expectedValue;
} SingleKeyframeData[] {
    {"before default-constructed",
        Extrapolation::DefaultConstructed, -1.0f, 0.0f},
    {"before constant",
        Extrapolation::Constant, -1.0f, 3.0f},
    {"before extrapolated",
        Extrapolation::Extrapolated, -1.0f, 3.0f},
    {"at",
        Extrapolation::DefaultConstructed, 0.0f, 3.0f},
    {"after default-constructed",
        Extrapolation::DefaultConstructed, 1.0f, 0.0f},
    {"after constant",
        Extrapolation::Constant, 1.0f, 3.0f},
    {"after extrapolated",
        Extrapolation::Extrapolated, 1.0f, 3.0f}
};

const struct {
    const char* name;
    std::size_t hint;
} HintData[] {
    {"before", 1},
    {"at", 2},
    {"after", 3},
    {"out of bounds", 405780454}
};

InterpolationTest::InterpolationTest() {
    addTests({&InterpolationTest::interpolatorFor,
              &InterpolationTest::interpolatorForInvalid,
              &InterpolationTest::interpolatorForBool,
              &InterpolationTest::interpolatorForBoolInvalid,
              &InterpolationTest::interpolatorForBoolVector,
              &InterpolationTest::interpolatorForBoolVectorInvalid,
              &InterpolationTest::interpolatorForComplex,
              &InterpolationTest::interpolatorForComplexInvalid,
              &InterpolationTest::interpolatorForQuaternion,
              &InterpolationTest::interpolatorForQuaternionInvalid,
              &InterpolationTest::interpolatorForDualQuaternion,
              &InterpolationTest::interpolatorForDualQuaternionInvalid,
              &InterpolationTest::interpolatorForCubicHermiteScalar,
              &InterpolationTest::interpolatorForCubicHermiteScalarInvalid,
              &InterpolationTest::interpolatorForCubicHermiteVector,
              &InterpolationTest::interpolatorForCubicHermiteVectorInvalid,
              &InterpolationTest::interpolatorForCubicHermiteComplex,
              &InterpolationTest::interpolatorForCubicHermiteComplexInvalid,
              &InterpolationTest::interpolatorForCubicHermiteQuaternion,
              &InterpolationTest::interpolatorForCubicHermiteQuaternionInvalid});

    addInstancedTests({&InterpolationTest::interpolate,
                       &InterpolationTest::interpolateStrict},
                       Containers::arraySize(Data));

    addInstancedTests({&InterpolationTest::interpolateSingleKeyframe},
                       Containers::arraySize(SingleKeyframeData));

    addTests({&InterpolationTest::interpolateNoKeyframe});

    addInstancedTests({&InterpolationTest::interpolateHint,
                       &InterpolationTest::interpolateStrictHint},
                       Containers::arraySize(HintData));

    addTests({&InterpolationTest::interpolateDifferentResultType,
              &InterpolationTest::interpolateStrictDifferentResultType,

              &InterpolationTest::interpolateError,
              &InterpolationTest::interpolateStrictError,

              &InterpolationTest::interpolateIntegerKey,
              &InterpolationTest::interpolateStrictIntegerKey,

              &InterpolationTest::ease,
              &InterpolationTest::easeClamped,
              &InterpolationTest::unpack,
              &InterpolationTest::unpackEase,
              &InterpolationTest::unpackEaseClamped,

              &InterpolationTest::debugInterpolation,
              &InterpolationTest::debugExtrapolation});
}

void InterpolationTest::interpolatorFor() {
    CORRADE_COMPARE(Animation::interpolatorFor<Vector2>(Interpolation::Constant)(
        Vector2{0.3f, 0.5f}, Vector2{-0.3f, -1.5f}, 0.5f), (Vector2{0.3f, 0.5f}));
    CORRADE_COMPARE(Animation::interpolatorFor<Vector2>(Interpolation::Linear)(
        Vector2{0.3f, 0.5f}, Vector2{-0.3f, -1.5f}, 0.5f), (Vector2{0.0f, -0.5f}));
}

void InterpolationTest::interpolatorForInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Animation::interpolatorFor<Float>(Interpolation::Spline);
    Animation::interpolatorFor<Float>(Interpolation(0xde));

    CORRADE_COMPARE(out.str(),
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Spline\n"
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation(0xde)\n");
}

void InterpolationTest::interpolatorForBool() {
    CORRADE_COMPARE(Animation::interpolatorFor<bool>(Interpolation::Constant)(
        true, false, 0.5f), true);
    CORRADE_COMPARE(Animation::interpolatorFor<bool>(Interpolation::Linear)(
        true, false, 0.5f), true);
}

void InterpolationTest::interpolatorForBoolInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Animation::interpolatorFor<bool>(Interpolation::Custom);
    Animation::interpolatorFor<bool>(Interpolation(0xde));

    CORRADE_COMPARE(out.str(),
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Custom\n"
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation(0xde)\n");
}

void InterpolationTest::interpolatorForBoolVector() {
    CORRADE_COMPARE(Animation::interpolatorFor<Math::BoolVector<4>>(Interpolation::Constant)(
        Math::BoolVector<4>{0xa}, Math::BoolVector<4>{0x5}, 0.5f), (Math::BoolVector<4>{0xa}));
    CORRADE_COMPARE(Animation::interpolatorFor<Math::BoolVector<4>>(Interpolation::Linear)(
        Math::BoolVector<4>{0xa}, Math::BoolVector<4>{0x5}, 0.5f), (Math::BoolVector<4>{0xa}));
}

void InterpolationTest::interpolatorForBoolVectorInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Animation::interpolatorFor<Math::BoolVector<4>>(Interpolation::Custom);
    Animation::interpolatorFor<Math::BoolVector<4>>(Interpolation(0xde));

    CORRADE_COMPARE(out.str(),
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Custom\n"
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation(0xde)\n");
}

void InterpolationTest::interpolatorForComplex() {
    CORRADE_COMPARE(Animation::interpolatorFor<Complex>(Interpolation::Constant)(
        Complex::rotation(25.0_degf),
        Complex::rotation(75.0_degf), 0.5f),
        Complex::rotation(25.0_degf));
    CORRADE_COMPARE(Animation::interpolatorFor<Complex>(Interpolation::Linear)(
        Complex::rotation(25.0_degf),
        Complex::rotation(75.0_degf), 0.5f),
        Complex::rotation(50.0_degf));
}

void InterpolationTest::interpolatorForComplexInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Animation::interpolatorFor<Complex>(Interpolation::Custom);
    Animation::interpolatorFor<Complex>(Interpolation(0xde));

    CORRADE_COMPARE(out.str(),
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Custom\n"
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation(0xde)\n");
}

void InterpolationTest::interpolatorForQuaternion() {
    CORRADE_COMPARE(Animation::interpolatorFor<Quaternion>(Interpolation::Constant)(
        Quaternion::rotation(25.0_degf, Vector3::xAxis()),
        Quaternion::rotation(75.0_degf, Vector3::xAxis()), 0.5f),
        Quaternion::rotation(25.0_degf, Vector3::xAxis()));
    CORRADE_COMPARE(Animation::interpolatorFor<Quaternion>(Interpolation::Linear)(
        Quaternion::rotation(25.0_degf, Vector3::xAxis()),
        Quaternion::rotation(75.0_degf, Vector3::xAxis()), 0.5f),
        Quaternion::rotation(50.0_degf, Vector3::xAxis()));
}

void InterpolationTest::interpolatorForQuaternionInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Animation::interpolatorFor<Quaternion>(Interpolation::Spline);
    Animation::interpolatorFor<Quaternion>(Interpolation(0xde));

    CORRADE_COMPARE(out.str(),
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Spline\n"
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation(0xde)\n");
}

void InterpolationTest::interpolatorForDualQuaternion() {
    CORRADE_COMPARE(Animation::interpolatorFor<DualQuaternion>(Interpolation::Constant)(
        DualQuaternion::translation(Vector3::xAxis(2.5f)),
        DualQuaternion::translation(Vector3::xAxis(7.5f)), 0.5f),
        DualQuaternion::translation(Vector3::xAxis(2.5f)));
    CORRADE_COMPARE(Animation::interpolatorFor<DualQuaternion>(Interpolation::Linear)(
        DualQuaternion::translation(Vector3::xAxis(2.5f)),
        DualQuaternion::translation(Vector3::xAxis(7.5f)), 0.5f),
        DualQuaternion::translation(Vector3::xAxis(5.0f)));
}

void InterpolationTest::interpolatorForDualQuaternionInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Animation::interpolatorFor<DualQuaternion>(Interpolation::Custom);
    Animation::interpolatorFor<DualQuaternion>(Interpolation(0xde));

    CORRADE_COMPARE(out.str(),
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Custom\n"
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation(0xde)\n");
}

void InterpolationTest::interpolatorForCubicHermiteScalar() {
    CubicHermite1D a{2.0f, 3.0f, -1.0f};
    CubicHermite1D b{5.0f, -2.0f, 1.5f};
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermite1D>(Interpolation::Constant)(a, b, 0.8f), 3.0f);
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermite1D>(Interpolation::Linear)(a, b, 0.8f), -1.0f);
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermite1D>(Interpolation::Spline)(a, b, 0.8f), -2.152f);
}

void InterpolationTest::interpolatorForCubicHermiteScalarInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Animation::interpolatorFor<CubicHermite1D>(Interpolation::Custom);
    Animation::interpolatorFor<CubicHermite1D>(Interpolation(0xde));

    CORRADE_COMPARE(out.str(),
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Custom\n"
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation(0xde)\n");
}

void InterpolationTest::interpolatorForCubicHermiteVector() {
    CubicHermite2D a{{2.0f, 1.5f}, {3.0f, 0.1f}, {-1.0f, 0.0f}};
    CubicHermite2D b{{5.0f, 0.3f}, {-2.0f, 1.1f}, {1.5f, 0.3f}};
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermite2D>(Interpolation::Constant)(a, b, 0.8f), (Vector2{3.0f, 0.1f}));
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermite2D>(Interpolation::Linear)(a, b, 0.8f), (Vector2{-1.0f, 0.9f}));
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermite2D>(Interpolation::Spline)(a, b, 0.8f), (Vector2{-2.152f, 0.9576f}));
}

void InterpolationTest::interpolatorForCubicHermiteVectorInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Animation::interpolatorFor<CubicHermite2D>(Interpolation::Custom);
    Animation::interpolatorFor<CubicHermite2D>(Interpolation(0xde));

    CORRADE_COMPARE(out.str(),
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Custom\n"
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation(0xde)\n");
}

void InterpolationTest::interpolatorForCubicHermiteComplex() {
    CubicHermiteComplex a{{2.0f, 1.5f}, {0.999445f, 0.0333148f}, {-1.0f, 0.0f}};
    CubicHermiteComplex b{{5.0f, 0.3f}, {-0.876216f, 0.481919f}, {1.5f, 0.3f}};
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermiteComplex>(Interpolation::Constant)(a, b, 0.8f), (Complex{0.999445f, 0.0333148f}));
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermiteComplex>(Interpolation::Linear)(a, b, 0.8f), (Complex{-0.78747f, 0.616353f}));
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermiteComplex>(Interpolation::Spline)(a, b, 0.8f), (Complex{-0.95958f, 0.281435f}));
}

void InterpolationTest::interpolatorForCubicHermiteComplexInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Animation::interpolatorFor<CubicHermiteComplex>(Interpolation::Custom);
    Animation::interpolatorFor<CubicHermiteComplex>(Interpolation(0xde));

    CORRADE_COMPARE(out.str(),
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Custom\n"
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation(0xde)\n");
}

void InterpolationTest::interpolatorForCubicHermiteQuaternion() {
    CubicHermiteQuaternion a{
        {{2.0f, 1.5f, 0.3f}, 1.1f},
        {{0.780076f, 0.0260025f, 0.598059f}, 0.182018f},
        {{-1.0f, 0.0f, 0.3f}, 0.4f}};
    CubicHermiteQuaternion b{
        {{5.0f, 0.3f, 1.1f}, 0.5f},
        {{-0.711568f, 0.391362f, 0.355784f}, 0.462519f},
        {{1.5f, 0.3f, 17.0f}, -7.0f}};
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermiteQuaternion>(Interpolation::Constant)(a, b, 0.8f), (Quaternion{{0.780076f, 0.0260025f, 0.598059f}, 0.182018f}));
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermiteQuaternion>(Interpolation::Linear)(a, b, 0.8f), (Quaternion{{-0.533196f, 0.410685f, 0.521583f}, 0.524396f}));
    CORRADE_COMPARE(Animation::interpolatorFor<CubicHermiteQuaternion>(Interpolation::Spline)(a, b, 0.8f), (Quaternion{{-0.911408f, 0.23368f, 0.185318f}, 0.283524f}));
}

void InterpolationTest::interpolatorForCubicHermiteQuaternionInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Animation::interpolatorFor<CubicHermiteQuaternion>(Interpolation::Custom);
    Animation::interpolatorFor<CubicHermiteQuaternion>(Interpolation(0xde));

    CORRADE_COMPARE(out.str(),
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation::Custom\n"
        "Animation::interpolatorFor(): can't deduce interpolator function for Animation::Interpolation(0xde)\n");
}

constexpr Float Keys[]{0.0f, 2.0f, 4.0f, 5.0f};
constexpr Float Values[]{3.0f, 1.0f, 2.5f, 0.5f};

void InterpolationTest::interpolate() {
    const auto& data = Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    std::size_t hint{};
    CORRADE_COMPARE((Animation::interpolate<Float, Float>(
        Keys, Values, data.extrapolationBefore, data.extrapolationAfter,
        Math::lerp, data.time, hint)), data.expectedValue);
    CORRADE_COMPARE(hint, data.expectedHint);
}

void InterpolationTest::interpolateStrict() {
    const auto& data = Data[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    std::size_t hint{};
    CORRADE_COMPARE((Animation::interpolateStrict<Float, Float>(
        Keys, Values, Math::lerp, data.time, hint)), data.expectedValueStrict);
    CORRADE_COMPARE(hint, data.expectedHint);
}

void InterpolationTest::interpolateSingleKeyframe() {
    const auto& data = SingleKeyframeData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    std::size_t hint{};
    CORRADE_COMPARE((Animation::interpolate<Float, Float>(
        Containers::arrayView(Keys).prefix(1),
        Containers::arrayView(Values).prefix(1),
        data.extrapolation, data.extrapolation,
        Math::lerp, data.time, hint)), data.expectedValue);
    CORRADE_COMPARE(hint, 0);
}

void InterpolationTest::interpolateNoKeyframe() {
    std::size_t hint{};
    CORRADE_COMPARE((Animation::interpolate<Float, Float>(
        nullptr, nullptr, Extrapolation::Extrapolated,
        Extrapolation::Extrapolated, Math::lerp, 3.5f, hint)), Float{});
    CORRADE_COMPARE(hint, 0);
}

void InterpolationTest::interpolateHint() {
    const auto& data = HintData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    std::size_t hint = data.hint;
    CORRADE_COMPARE((Animation::interpolate<Float, Float>(
        Keys, Values, Extrapolation::Extrapolated, Extrapolation::Extrapolated,
        Math::lerp, 4.75f, hint)), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

void InterpolationTest::interpolateStrictHint() {
    const auto& data = HintData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    std::size_t hint = data.hint;
    CORRADE_COMPARE((Animation::interpolateStrict<Float, Float>(
        Keys, Values, Math::lerp, 4.75f, hint)), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

using namespace Math::Literals;

const Half HalfValues[]{3.0_h, 1.0_h, 2.5_h, 0.5_h};

Float lerpHalf(const Half& a, const Half& b, Float t) {
    return Math::lerp(Float(a), Float(b), t);
}

void InterpolationTest::interpolateDifferentResultType() {
    std::size_t hint{};
    CORRADE_COMPARE((Animation::interpolate<Float, Half, Float>(
        Keys, HalfValues, Extrapolation::Extrapolated, Extrapolation::Extrapolated, lerpHalf, 4.75f, hint)), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

void InterpolationTest::interpolateStrictDifferentResultType() {
    std::size_t hint{};
    CORRADE_COMPARE((Animation::interpolateStrict<Float, Half, Float>(
        Keys, HalfValues, lerpHalf, 4.75f, hint)), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

constexpr Int IntegerKeys[]{0, 48, 96, 120};

void InterpolationTest::interpolateIntegerKey() {
    std::size_t hint{};
    CORRADE_COMPARE((Animation::interpolate<Int, Float>(
        IntegerKeys, Values, Extrapolation::Extrapolated, Extrapolation::Extrapolated, Math::lerp, 114, hint)), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

void InterpolationTest::interpolateStrictIntegerKey() {
    std::size_t hint{};
    CORRADE_COMPARE((Animation::interpolateStrict<Int, Float>(
        IntegerKeys, Values, Math::lerp, 114, hint)), 1.0f);
    CORRADE_COMPARE(hint, 2);
}

void InterpolationTest::interpolateError() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    {
        std::size_t hint{};
        Animation::interpolate<Float, Float>(Keys, nullptr, Extrapolation::Extrapolated, Extrapolation::Extrapolated, Math::lerp, 0.0f, hint);
    }

    CORRADE_COMPARE(out.str(),
        "Animation::interpolate(): keys and values don't have the same size\n");
}

void InterpolationTest::interpolateStrictError() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    {
        std::size_t hint{};
        Animation::interpolateStrict<Float, Float>(
            Containers::arrayView(Keys).prefix(1),
            Containers::arrayView(Values).prefix(1),
            Math::lerp, 0.0f, hint);
    } {
        std::size_t hint{};
        Animation::interpolateStrict<Float, Float>(
            Containers::arrayView(Keys).prefix(3), Values,
            Math::lerp, 0.0f, hint);
    }

    CORRADE_COMPARE(out.str(),
        "Animation::interpolateStrict(): at least two keyframes required\n"
        "Animation::interpolateStrict(): keys and values don't have the same size\n");
}

void InterpolationTest::ease() {
    auto lerpQuadratic = Animation::ease<Float, Math::lerp, Easing::quadraticIn>();

    CORRADE_COMPARE(Math::lerp(0.5f, 0.95f, Easing::quadraticIn(0.3f)), 0.5405f);
    CORRADE_COMPARE(lerpQuadratic(0.5f, 0.95f, 0.3f), 0.5405f);
}

void InterpolationTest::easeClamped() {
    auto lerpBackInClamped = Animation::easeClamped<Float, Math::lerp, Easing::backIn>();

    /* Verify it doesn't return garbage outside the range */
    CORRADE_COMPARE(lerpBackInClamped(0.5f, 0.95f, -0.3f), 0.5f);
    CORRADE_COMPARE(lerpBackInClamped(0.5f, 0.95f, 1.3f), 0.95f);

    /* Verify it doesn't clamp the easer output (should be less than 0.5) */
    CORRADE_COMPARE(Math::lerp(0.5f, 0.95f, Easing::backIn(0.3f)), 0.402933f);
    CORRADE_COMPARE(lerpBackInClamped(0.5f, 0.95f, 0.3f), 0.402933f);
}

void InterpolationTest::unpack() {
    auto lerpPacked = Animation::unpack<UnsignedShort, Float, Math::lerp, Math::unpack<Float>>();

    CORRADE_COMPARE(Math::lerp(Math::unpack<Float, UnsignedShort>(32767), Math::unpack<Float, UnsignedShort>(62258), 0.3f), 0.634994f);
    CORRADE_COMPARE(lerpPacked(32767, 62258, 0.3f), 0.634994f);
}

void InterpolationTest::unpackEase() {
    auto lerpPackedQuadratic = Animation::unpackEase<UnsignedShort, Float, Math::lerp, Math::unpack<Float>, Easing::quadraticIn>();

    /* Some minor imprecision compared to ease() due to lossy packing */
    CORRADE_COMPARE(Math::lerp(Math::unpack<Float, UnsignedShort>(32767), Math::unpack<Float, UnsignedShort>(62258), Easing::quadraticIn(0.3f)), 0.540493f);
    CORRADE_COMPARE(lerpPackedQuadratic(32767, 62258, 0.3f), 0.540493f);
}

void InterpolationTest::unpackEaseClamped() {
    auto lerpPackedBackInClamped = Animation::unpackEaseClamped<UnsignedShort, Float, Math::lerp, Math::unpack<Float>, Easing::backIn>();

    /* Some minor imprecision compared to easeClamped() due to lossy packing */

    /* Verify it doesn't return garbage outside the range */
    CORRADE_COMPARE(lerpPackedBackInClamped(32767, 62258, -0.3f), 0.499992f);
    CORRADE_COMPARE(lerpPackedBackInClamped(32767, 62258, 1.3f), 0.949996f);

    /* Verify it doesn't clamp the easer output (should be less than 0.5) */
    CORRADE_COMPARE(Math::lerp(Math::unpack<Float, UnsignedShort>(32767), Math::unpack<Float, UnsignedShort>(62258), Easing::backIn(0.3f)), 0.402924f);
    CORRADE_COMPARE(lerpPackedBackInClamped(32767, 62258, 0.3f), 0.402924f);
}

void InterpolationTest::debugInterpolation() {
    std::ostringstream out;

    Debug{&out} << Interpolation::Custom << Interpolation(0xde);
    CORRADE_COMPARE(out.str(), "Animation::Interpolation::Custom Animation::Interpolation(0xde)\n");
}

void InterpolationTest::debugExtrapolation() {
    std::ostringstream out;

    Debug{&out} << Extrapolation::DefaultConstructed << Extrapolation(0xde);
    CORRADE_COMPARE(out.str(), "Animation::Extrapolation::DefaultConstructed Animation::Extrapolation(0xde)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Animation::Test::InterpolationTest)
