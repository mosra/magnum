/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <string>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/TypeTraits.h"
#include "Magnum/Math/Constants.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct TypeTraitsTest: Corrade::TestSuite::Tester {
    explicit TypeTraitsTest();

    void sizeOfLongDouble();
    void name();

    template<class T> void equalsIntegral();
    template<class T> void equalsFloatingPoint0();
    template<class T> void equalsFloatingPoint1();
    template<class T> void equalsFloatingPointLarge();
    template<class T> void equalsFloatingPointInfinity();
    template<class T> void equalsFloatingPointNaN();

    template<class T> void equalsZeroIntegral();
    template<class T> void equalsZeroFloatingPoint();
    template<class T> void equalsZeroFloatingPointSmall();
    template<class T> void equalsZeroFloatingPointLarge();
};

enum: std::size_t { EqualsZeroDataCount = 3 };

struct {
    const char* name;
    Float a, aStep;
    Double b, bStep;
    long double c, cStep;

    Float get(Float) const { return a; }
    Float getStep(Float) const { return aStep; }
    Double get(Double) const { return b; }
    Double getStep(Double) const { return bStep; }
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    long double get(long double) const { return c; }
    long double getStep(long double) const { return cStep; }
    #endif
} EqualsZeroData[EqualsZeroDataCount] = {
    {"", -3.141592653589793f, 5.0e-5f, -3.141592653589793, 5.0e-14, -3.141592653589793l,
        #if !defined(_MSC_VER) && (!defined(CORRADE_TARGET_ANDROID) || __LP64__)
        5.0e-17l
        #else
        5.0e-14
        #endif
    },
    {"small", 1.0e-6f, 5.0e-6f, -1.0e-15, 5.0e-15, 1.0e-18l,
        #if !defined(_MSC_VER) && (!defined(CORRADE_TARGET_ANDROID) || __LP64__)
        5.0e-18l
        #else
        5.0e-15
        #endif
    },
    {"large", 12345.0f, 0.2f, 12345678901234.0, 0.2,
        #if !defined(_MSC_VER) && (!defined(CORRADE_TARGET_ANDROID) || __LP64__)
        -12345678901234567.0l,
        #else
        -12345678901234.0,
        #endif
        0.2l},
};

TypeTraitsTest::TypeTraitsTest() {
    addTests({&TypeTraitsTest::sizeOfLongDouble,
              &TypeTraitsTest::name,

              &TypeTraitsTest::equalsIntegral<UnsignedByte>,
              &TypeTraitsTest::equalsIntegral<Byte>,
              &TypeTraitsTest::equalsIntegral<UnsignedShort>,
              &TypeTraitsTest::equalsIntegral<Short>,
              &TypeTraitsTest::equalsIntegral<UnsignedInt>,
              &TypeTraitsTest::equalsIntegral<Int>,
              #ifndef CORRADE_TARGET_EMSCRIPTEN
              &TypeTraitsTest::equalsIntegral<UnsignedLong>,
              &TypeTraitsTest::equalsIntegral<Long>,
              #endif

              &TypeTraitsTest::equalsFloatingPoint0<Float>,
              &TypeTraitsTest::equalsFloatingPoint0<Double>,
              #ifndef CORRADE_TARGET_EMSCRIPTEN
              &TypeTraitsTest::equalsFloatingPoint0<long double>,
              #endif
              &TypeTraitsTest::equalsFloatingPoint1<Float>,
              &TypeTraitsTest::equalsFloatingPoint1<Double>,
              #ifndef CORRADE_TARGET_EMSCRIPTEN
              &TypeTraitsTest::equalsFloatingPoint1<long double>,
              #endif
              &TypeTraitsTest::equalsFloatingPointLarge<Float>,
              &TypeTraitsTest::equalsFloatingPointLarge<Double>,
              #ifndef CORRADE_TARGET_EMSCRIPTEN
              &TypeTraitsTest::equalsFloatingPointLarge<long double>,
              #endif
              &TypeTraitsTest::equalsFloatingPointInfinity<Float>,
              &TypeTraitsTest::equalsFloatingPointInfinity<Double>,
              &TypeTraitsTest::equalsFloatingPointNaN<Float>,
              &TypeTraitsTest::equalsFloatingPointNaN<Double>,

              &TypeTraitsTest::equalsZeroIntegral<UnsignedByte>,
              &TypeTraitsTest::equalsZeroIntegral<Byte>,
              &TypeTraitsTest::equalsZeroIntegral<UnsignedShort>,
              &TypeTraitsTest::equalsZeroIntegral<Short>,
              &TypeTraitsTest::equalsZeroIntegral<UnsignedInt>,
              &TypeTraitsTest::equalsZeroIntegral<Int>,
              #ifndef CORRADE_TARGET_EMSCRIPTEN
              &TypeTraitsTest::equalsZeroIntegral<UnsignedLong>,
              &TypeTraitsTest::equalsZeroIntegral<Long>,
              #endif
              });

    addInstancedTests<TypeTraitsTest>({
        &TypeTraitsTest::equalsZeroFloatingPoint<Float>,
        &TypeTraitsTest::equalsZeroFloatingPoint<Double>,
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        &TypeTraitsTest::equalsZeroFloatingPoint<long double>
        #endif
        }, EqualsZeroDataCount);
}

void TypeTraitsTest::sizeOfLongDouble() {
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_SKIP("Not defined in Emscripten.");
    #else
    #if defined(_MSC_VER) || (defined(CORRADE_TARGET_ANDROID) && !__LP64__)
    CORRADE_COMPARE(sizeof(long double), 8);
    CORRADE_EXPECT_FAIL("long double is equivalent to double on MSVC and 32-bit Android.");
    #endif

    /* It's 80 bit, but has to be aligned somehow, so 128 bits / 16 bytes */
    CORRADE_COMPARE(sizeof(long double), 16);
    #endif
}

void TypeTraitsTest::name() {
    CORRADE_COMPARE(TypeTraits<UnsignedShort>::name(), std::string{"UnsignedShort"});
    CORRADE_COMPARE(TypeTraits<Float>::name(), std::string{"Float"});
}

template<class T> void TypeTraitsTest::equalsIntegral() {
    setTestCaseName(std::string{"equalsIntegral<"} + TypeTraits<T>::name() + ">");

    CORRADE_VERIFY(TypeTraits<T>::equals(T(1), T(1)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(1), T(-1)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(1), T(1)+TypeTraits<T>::epsilon()));
}

template<class T> void TypeTraitsTest::equalsFloatingPoint0() {
    setTestCaseName(std::string{"equalsFloatingPoint0<"} + TypeTraits<T>::name() + ">");

    CORRADE_VERIFY(TypeTraits<T>::equals(T(0)+TypeTraits<T>::epsilon()/T(2), T(0)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(0)+TypeTraits<T>::epsilon()*T(2), T(0)));
}

template<class T> void TypeTraitsTest::equalsFloatingPoint1() {
    setTestCaseName(std::string{"equalsFloatingPoint1<"} + TypeTraits<T>::name() + ">");

    CORRADE_VERIFY(TypeTraits<T>::equals(T(1)+TypeTraits<T>::epsilon()/T(2), T(1)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(1)+TypeTraits<T>::epsilon()*T(3), T(1)));
}

template<class T> void TypeTraitsTest::equalsFloatingPointLarge() {
    setTestCaseName(std::string{"equalsFloatingPointLarge<"} + TypeTraits<T>::name() + ">");

    CORRADE_VERIFY(TypeTraits<T>::equals(T(25)+TypeTraits<T>::epsilon()*T(2), T(25)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(25)+TypeTraits<T>::epsilon()*T(75), T(25)));
}

template<class T> void TypeTraitsTest::equalsFloatingPointInfinity() {
    setTestCaseName(std::string{"equalsFloatingPointInfinity<"} + TypeTraits<T>::name() + ">");

    CORRADE_VERIFY(TypeTraits<T>::equals(Constants<T>::inf(),
                                         Constants<T>::inf()));
}

template<class T> void TypeTraitsTest::equalsFloatingPointNaN() {
    setTestCaseName(std::string{"equalsFloatingPointNaN<"} + TypeTraits<T>::name() + ">");

    CORRADE_VERIFY(!TypeTraits<T>::equals(Constants<T>::nan(),
                                          Constants<T>::nan()));
}

/* Argh! Why there is no standard std::abs() for unsigned types? */
template<class T, class U = typename std::enable_if<std::is_unsigned<T>::value>::type> T abs(T value) {
    return value;
}
template<class T, class U = T, class V = typename std::enable_if<!std::is_unsigned<T>::value>::type> T abs(T value) {
    return std::abs(value);
}

template<class T> void TypeTraitsTest::equalsZeroIntegral() {
    setTestCaseName(std::string{"equalsZeroIntegral<"} + TypeTraits<T>::name() + ">");

    const T a = T(-123);
    const T magnitude = abs(a);

    CORRADE_VERIFY(TypeTraits<T>::equals(a, a));
    CORRADE_VERIFY(TypeTraits<T>::equalsZero(a - a, magnitude));
    CORRADE_VERIFY(!TypeTraits<T>::equalsZero(a - a + TypeTraits<T>::epsilon(), magnitude));
}

template<class T> void TypeTraitsTest::equalsZeroFloatingPoint() {
    setTestCaseName(std::string{"equalsZeroFloatingPoint<"} + TypeTraits<T>::name() + ">");
    setTestCaseDescription(EqualsZeroData[testCaseInstanceId()].name);

    const T a = EqualsZeroData[testCaseInstanceId()].get(T{});
    const T step = EqualsZeroData[testCaseInstanceId()].getStep(T{});
    const T magnitude = abs(a);

    CORRADE_VERIFY(TypeTraits<T>::equals(a + step/T(2.0), a));
    CORRADE_VERIFY(TypeTraits<T>::equalsZero(a + step/T(2.0) - a, magnitude));

    CORRADE_VERIFY(!TypeTraits<T>::equals(a - step*T(2.0), a));
    CORRADE_VERIFY(!TypeTraits<T>::equalsZero(a - step*T(2.0) - a, magnitude));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::TypeTraitsTest)
