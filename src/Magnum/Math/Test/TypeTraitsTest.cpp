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

#include <string>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Half.h"
#include "Magnum/Math/Math.h"
#include "Magnum/Math/TypeTraits.h"
#include "Magnum/Math/Constants.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct TypeTraitsTest: Corrade::TestSuite::Tester {
    explicit TypeTraitsTest();

    void name();

    void isScalar();
    void isVector();
    void isIntegral();
    void isFloatingPoint();
    void isUnitless();

    void underlyingTypeOf();

    template<class T> void epsilonConsistentWithCorrade();

    template<class T> void equalsIntegral();
    void equalsHalf();
    template<class T> void equalsFloatingPoint0();
    template<class T> void equalsFloatingPoint1();
    template<class T> void equalsFloatingPointLarge();
    template<class T> void equalsFloatingPointInfinity();
    template<class T> void equalsFloatingPointNaN();

    template<class T> void equalsZeroIntegral();
    template<class T> void equalsZeroFloatingPoint();
    template<class T> void equalsZeroFloatingPointSmall();
    template<class T> void equalsZeroFloatingPointLarge();

    void equal();
};

struct {
    const char* name;
    Float a, aStep;
    Double b, bStep;
    long double c, cStep;

    Float get(Float) const { return a; }
    Float getStep(Float) const { return aStep; }
    Double get(Double) const { return b; }
    Double getStep(Double) const { return bStep; }
    long double get(long double) const { return c; }
    long double getStep(long double) const { return cStep; }
} EqualsZeroData[] = {
    {"", -3.141592653589793f, 5.0e-5f, -3.141592653589793, 5.0e-14, -3.141592653589793l,
        #ifndef CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE
        5.0e-17l
        #else
        5.0e-14
        #endif
    },
    {"small", 1.0e-6f, 5.0e-6f, -1.0e-15, 5.0e-15,
        #ifndef CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE
        1.0e-18l, 5.0e-18l
        #else
        1.0e-15l, 5.0e-15l
        #endif
    },
    {"large", 12345.0f, 0.2f, 12345678901234.0, 0.2,
        #ifndef CORRADE_LONG_DOUBLE_SAME_AS_DOUBLE
        -12345678901234567.0l,
        #else
        -12345678901234.0,
        #endif
        0.2l},
};

TypeTraitsTest::TypeTraitsTest() {
    addTests({&TypeTraitsTest::name,

              &TypeTraitsTest::isScalar,
              &TypeTraitsTest::isVector,
              &TypeTraitsTest::isIntegral,
              &TypeTraitsTest::isFloatingPoint,
              &TypeTraitsTest::isUnitless,

              &TypeTraitsTest::underlyingTypeOf,

              &TypeTraitsTest::epsilonConsistentWithCorrade<Float>,
              &TypeTraitsTest::epsilonConsistentWithCorrade<Double>,
              &TypeTraitsTest::epsilonConsistentWithCorrade<long double>,

              &TypeTraitsTest::equalsIntegral<UnsignedByte>,
              &TypeTraitsTest::equalsIntegral<Byte>,
              &TypeTraitsTest::equalsIntegral<UnsignedShort>,
              &TypeTraitsTest::equalsIntegral<Short>,
              &TypeTraitsTest::equalsIntegral<UnsignedInt>,
              &TypeTraitsTest::equalsIntegral<Int>,
              &TypeTraitsTest::equalsIntegral<UnsignedLong>,
              &TypeTraitsTest::equalsIntegral<Long>,

              &TypeTraitsTest::equalsHalf,

              &TypeTraitsTest::equalsFloatingPoint0<Float>,
              &TypeTraitsTest::equalsFloatingPoint0<Double>,
              &TypeTraitsTest::equalsFloatingPoint0<long double>,
              &TypeTraitsTest::equalsFloatingPoint1<Float>,
              &TypeTraitsTest::equalsFloatingPoint1<Double>,
              &TypeTraitsTest::equalsFloatingPoint1<long double>,
              &TypeTraitsTest::equalsFloatingPointLarge<Float>,
              &TypeTraitsTest::equalsFloatingPointLarge<Double>,
              &TypeTraitsTest::equalsFloatingPointLarge<long double>,
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
              &TypeTraitsTest::equalsZeroIntegral<UnsignedLong>,
              &TypeTraitsTest::equalsZeroIntegral<Long>,
              });

    addInstancedTests<TypeTraitsTest>({
        &TypeTraitsTest::equalsZeroFloatingPoint<Float>,
        &TypeTraitsTest::equalsZeroFloatingPoint<Double>,
        &TypeTraitsTest::equalsZeroFloatingPoint<long double>},
        Corrade::Containers::arraySize(EqualsZeroData));

    addTests({&TypeTraitsTest::equal});
}

void TypeTraitsTest::name() {
    CORRADE_COMPARE(TypeTraits<UnsignedShort>::name(), std::string{"UnsignedShort"});
    CORRADE_COMPARE(TypeTraits<Float>::name(), std::string{"Float"});
}

void TypeTraitsTest::isScalar() {
    CORRADE_VERIFY(IsScalar<char>::value);
    CORRADE_VERIFY(IsScalar<UnsignedShort>::value);
    CORRADE_VERIFY(IsScalar<Deg<Float>>::value);
    CORRADE_VERIFY(IsScalar<Half>::value);
    CORRADE_VERIFY((IsScalar<Unit<Rad, Double>>::value));
    CORRADE_VERIFY(!IsScalar<Vector2<Float>>::value);
    CORRADE_VERIFY(!IsVector<Matrix2x3<Float>>::value);
    CORRADE_VERIFY(!IsScalar<Color3<Half>>::value);
    CORRADE_VERIFY(!IsScalar<char*>::value);
    CORRADE_VERIFY(!IsScalar<bool>::value);
}

void TypeTraitsTest::isVector() {
    CORRADE_VERIFY(!IsVector<UnsignedByte>::value);
    CORRADE_VERIFY(!IsVector<Deg<UnsignedByte>>::value);
    CORRADE_VERIFY((IsVector<Vector<2, Deg<Float>>>::value));
    CORRADE_VERIFY(IsVector<Color3<UnsignedByte>>::value);
    CORRADE_VERIFY(IsVector<Color3<Half>>::value);
    CORRADE_VERIFY(!IsVector<Matrix2x3<Float>>::value);
    CORRADE_VERIFY(!IsVector<char*>::value);
}

void TypeTraitsTest::isIntegral() {
    /* These three are usually different types */
    CORRADE_VERIFY(IsIntegral<char>::value);
    CORRADE_VERIFY(IsIntegral<Byte>::value);
    CORRADE_VERIFY(IsIntegral<UnsignedByte>::value);

    CORRADE_VERIFY(IsIntegral<Int>::value);
    CORRADE_VERIFY((IsIntegral<Vector<7, UnsignedInt>>::value));
    CORRADE_VERIFY(IsIntegral<Vector2<Long>>::value);
    CORRADE_VERIFY(!IsIntegral<Half>::value);
    CORRADE_VERIFY(!IsIntegral<Deg<Float>>::value);
    CORRADE_VERIFY(!IsIntegral<char*>::value);
    CORRADE_VERIFY(!IsIntegral<bool>::value);
}

void TypeTraitsTest::isFloatingPoint() {
    CORRADE_VERIFY(!IsFloatingPoint<Int>::value);
    CORRADE_VERIFY(!(IsFloatingPoint<Vector<7, UnsignedInt>>::value));
    CORRADE_VERIFY(IsFloatingPoint<Double>::value);
    CORRADE_VERIFY((IsFloatingPoint<Vector<2, Float>>::value));
    CORRADE_VERIFY(IsFloatingPoint<Vector2<long double>>::value);
    CORRADE_VERIFY(IsFloatingPoint<Deg<Float>>::value);
    CORRADE_VERIFY(IsFloatingPoint<Color4<Half>>::value);
    CORRADE_VERIFY((IsFloatingPoint<Unit<Rad, Float>>::value));
    CORRADE_VERIFY(IsFloatingPoint<Deg<Half>>::value);
    CORRADE_VERIFY(!IsFloatingPoint<char*>::value);
}

void TypeTraitsTest::isUnitless() {
    CORRADE_VERIFY(IsUnitless<Int>::value);
    CORRADE_VERIFY(IsUnitless<Half>::value);
    CORRADE_VERIFY(IsUnitless<Color4<Float>>::value);
    CORRADE_VERIFY(!IsUnitless<Deg<Float>>::value);
    CORRADE_VERIFY(!(IsUnitless<Unit<Rad, Double>>::value));
    CORRADE_VERIFY(!IsUnitless<char*>::value);
}

void TypeTraitsTest::underlyingTypeOf() {
    CORRADE_VERIFY((std::is_same<UnderlyingTypeOf<Int>, Int>::value));

    CORRADE_VERIFY((std::is_same<UnderlyingTypeOf<Deg<Float>>, Float>::value));
    CORRADE_VERIFY((std::is_same<UnderlyingTypeOf<Unit<Rad, Double>>, Double>::value));

    CORRADE_VERIFY((std::is_same<UnderlyingTypeOf<Vector2<UnsignedByte>>, UnsignedByte>::value));
    CORRADE_VERIFY((std::is_same<UnderlyingTypeOf<Color3<Float>>, Float>::value));

    CORRADE_VERIFY((std::is_same<UnderlyingTypeOf<Matrix2x3<Double>>, Double>::value));
    CORRADE_VERIFY((std::is_same<UnderlyingTypeOf<Matrix4<Float>>, Float>::value));
}

template<class T> void TypeTraitsTest::epsilonConsistentWithCorrade() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    /* Using VERIFY because we *don't* want fuzzy comparison in this case. The
       equals*() tests below do further checks against TestSuite. */
    CORRADE_VERIFY(TypeTraits<T>::epsilon() == Corrade::Utility::Implementation::FloatPrecision<T>::epsilon());
}

template<class T> void TypeTraitsTest::equalsIntegral() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_VERIFY(TypeTraits<T>::equals(T(1), T(1)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(1), T(-1)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(1), T(1)+TypeTraits<T>::epsilon()));
}

void TypeTraitsTest::equalsHalf() {
    CORRADE_VERIFY(TypeTraits<Half>::equals(Half(UnsignedShort(0xabcd)),
                                            Half(UnsignedShort(0xabcd))));
    CORRADE_VERIFY(!TypeTraits<Half>::equals(Half(UnsignedShort(0xabcd)),
                                             Half(UnsignedShort(0xabce))));
}

template<class T> void TypeTraitsTest::equalsFloatingPoint0() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_VERIFY(TypeTraits<T>::equals(T(0)+TypeTraits<T>::epsilon()/T(2), T(0)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(0)+TypeTraits<T>::epsilon()*T(2), T(0)));

    /* Ensure we have the same behavior as TestSuite. Done in addition to the
       epsilonConsistentWithCorrade() test above, since that one alone might
       give a false sense of security. */
    CORRADE_COMPARE(Corrade::TestSuite::Implementation::FloatComparator<T>{}(
        T(0)+TypeTraits<T>::epsilon()/T(2), T(0)),
        Corrade::TestSuite::ComparisonStatusFlags{});
    CORRADE_COMPARE(Corrade::TestSuite::Implementation::FloatComparator<T>{}(
        T(0)+TypeTraits<T>::epsilon()*T(2), T(0)),
        Corrade::TestSuite::ComparisonStatusFlag::Failed);
}

template<class T> void TypeTraitsTest::equalsFloatingPoint1() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_VERIFY(TypeTraits<T>::equals(T(1)+TypeTraits<T>::epsilon()/T(2), T(1)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(1)+TypeTraits<T>::epsilon()*T(3), T(1)));

    /* Ensure we have the same behavior as TestSuite. Done in addition to the
       epsilonConsistentWithCorrade() test above, since that one alone might
       give a false sense of security. */
    CORRADE_COMPARE(Corrade::TestSuite::Implementation::FloatComparator<T>{}(
        T(1)+TypeTraits<T>::epsilon()/T(2), T(1)),
        Corrade::TestSuite::ComparisonStatusFlags{});
    CORRADE_COMPARE(Corrade::TestSuite::Implementation::FloatComparator<T>{}(
        T(1)+TypeTraits<T>::epsilon()*T(3), T(1)),
        Corrade::TestSuite::ComparisonStatusFlag::Failed);
}

template<class T> void TypeTraitsTest::equalsFloatingPointLarge() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_VERIFY(TypeTraits<T>::equals(T(25)+TypeTraits<T>::epsilon()*T(2), T(25)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(25)+TypeTraits<T>::epsilon()*T(75), T(25)));

    /* Ensure we have the same behavior as TestSuite. Done in addition to the
       epsilonConsistentWithCorrade() test above, since that one alone might
       give a false sense of security. */
    CORRADE_COMPARE(Corrade::TestSuite::Implementation::FloatComparator<T>{}(
        T(25)+TypeTraits<T>::epsilon()*T(2), T(25)),
        Corrade::TestSuite::ComparisonStatusFlags{});
    CORRADE_COMPARE(Corrade::TestSuite::Implementation::FloatComparator<T>{}(
        T(25)+TypeTraits<T>::epsilon()*T(75), T(25)),
        Corrade::TestSuite::ComparisonStatusFlag::Failed);
}

template<class T> void TypeTraitsTest::equalsFloatingPointInfinity() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_VERIFY(TypeTraits<T>::equals(Constants<T>::inf(),
                                         Constants<T>::inf()));
    CORRADE_VERIFY(!TypeTraits<T>::equals(Constants<T>::inf(),
                                         -Constants<T>::inf()));

    /* Ensure we have the same behavior as TestSuite. Done in addition to the
       epsilonConsistentWithCorrade() test above, since that one alone might
       give a false sense of security. */
    CORRADE_COMPARE(Corrade::TestSuite::Implementation::FloatComparator<T>{}(
        Constants<T>::inf(), Constants<T>::inf()),
        Corrade::TestSuite::ComparisonStatusFlags{});
    CORRADE_COMPARE(Corrade::TestSuite::Implementation::FloatComparator<T>{}(
        Constants<T>::inf(), -Constants<T>::inf()),
        Corrade::TestSuite::ComparisonStatusFlag::Failed);
}

template<class T> void TypeTraitsTest::equalsFloatingPointNaN() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    CORRADE_VERIFY(!TypeTraits<T>::equals(Constants<T>::nan(),
                                          Constants<T>::nan()));

    /* OTOH, TestSuite compares two NaNs as equal -- since that makes more
       sense in the context of tests */
    CORRADE_COMPARE(Corrade::TestSuite::Implementation::FloatComparator<T>{}(
        Constants<T>::nan(), Constants<T>::nan()),
        Corrade::TestSuite::ComparisonStatusFlags{});
}

/* Argh! Why there is no standard std::abs() for unsigned types? */
template<class T, class U = typename std::enable_if<std::is_unsigned<T>::value>::type> T abs(T value) {
    return value;
}
template<class T, class U = T, class V = typename std::enable_if<!std::is_unsigned<T>::value>::type> T abs(T value) {
    return std::abs(value);
}

template<class T> void TypeTraitsTest::equalsZeroIntegral() {
    setTestCaseTemplateName(TypeTraits<T>::name());

    const T a = T(-123);
    const T magnitude = abs(a);

    CORRADE_VERIFY(TypeTraits<T>::equals(a, a));
    CORRADE_VERIFY(TypeTraits<T>::equalsZero(a - a, magnitude));
    CORRADE_VERIFY(!TypeTraits<T>::equalsZero(a - a + TypeTraits<T>::epsilon(), magnitude));
}

template<class T> void TypeTraitsTest::equalsZeroFloatingPoint() {
    setTestCaseTemplateName(TypeTraits<T>::name());
    setTestCaseDescription(EqualsZeroData[testCaseInstanceId()].name);

    const T a = EqualsZeroData[testCaseInstanceId()].get(T{});
    const T step = EqualsZeroData[testCaseInstanceId()].getStep(T{});
    const T magnitude = abs(a);

    CORRADE_VERIFY(TypeTraits<T>::equals(a + step/T(2.0), a));
    CORRADE_VERIFY(TypeTraits<T>::equalsZero(a + step/T(2.0) - a, magnitude));

    CORRADE_VERIFY(!TypeTraits<T>::equals(a - step*T(2.0), a));
    CORRADE_VERIFY(!TypeTraits<T>::equalsZero(a - step*T(2.0) - a, magnitude));
}

void TypeTraitsTest::equal() {
    CORRADE_VERIFY(Math::equal(1, 1));
    CORRADE_VERIFY(!Math::equal(1, -1));
    CORRADE_VERIFY(Math::equal(1.0f + TypeTraits<Float>::epsilon()/2.0f, 1.0f));
    CORRADE_VERIFY(!Math::equal(1.0f + TypeTraits<Float>::epsilon()*3.0f, 1.0f));

    CORRADE_VERIFY(!Math::notEqual(1, 1));
    CORRADE_VERIFY(Math::notEqual(1, -1));
    CORRADE_VERIFY(!Math::notEqual(1.0f + TypeTraits<Float>::epsilon()/2.0f, 1.0f));
    CORRADE_VERIFY(Math::notEqual(1.0f + TypeTraits<Float>::epsilon()*3.0f, 1.0f));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::TypeTraitsTest)
