/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/TypeTraits.h"
#include "Magnum/Math/Constants.h"

namespace Magnum { namespace Math { namespace Test {

struct TypeTraitsTest: Corrade::TestSuite::Tester {
    explicit TypeTraitsTest();

    template<class T> void equalsIntegral();
    template<class T> void equalsFloatingPoint0();
    template<class T> void equalsFloatingPoint1();
    template<class T> void equalsFloatingPointLarge();
    template<class T> void equalsFloatingPointInfinity();
    template<class T> void equalsFloatingPointNaN();
};

TypeTraitsTest::TypeTraitsTest() {
    addTests<TypeTraitsTest>({&TypeTraitsTest::equalsIntegral<UnsignedByte>,
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
              &TypeTraitsTest::equalsFloatingPoint1<Float>,
              &TypeTraitsTest::equalsFloatingPoint1<Double>,
              &TypeTraitsTest::equalsFloatingPointLarge<Float>,
              &TypeTraitsTest::equalsFloatingPointLarge<Double>,
              &TypeTraitsTest::equalsFloatingPointInfinity<Float>,
              &TypeTraitsTest::equalsFloatingPointInfinity<Double>,
              &TypeTraitsTest::equalsFloatingPointNaN<Float>,
              &TypeTraitsTest::equalsFloatingPointNaN<Double>});
}

template<class T> void TypeTraitsTest::equalsIntegral() {
    CORRADE_VERIFY(!TypeTraits<T>::equals(1, 1+TypeTraits<T>::epsilon()));
}

template<class T> void TypeTraitsTest::equalsFloatingPoint0() {
    CORRADE_VERIFY(TypeTraits<T>::equals(T(0)+TypeTraits<T>::epsilon()/T(2), T(0)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(0)+TypeTraits<T>::epsilon()*T(2), T(0)));
}

template<class T> void TypeTraitsTest::equalsFloatingPoint1() {
    CORRADE_VERIFY(TypeTraits<T>::equals(T(1)+TypeTraits<T>::epsilon()/T(2), T(1)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(1)+TypeTraits<T>::epsilon()*T(3), T(1)));
}

template<class T> void TypeTraitsTest::equalsFloatingPointLarge() {
    CORRADE_VERIFY(TypeTraits<T>::equals(T(25)+TypeTraits<T>::epsilon()*T(2), T(25)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(25)+TypeTraits<T>::epsilon()*T(75), T(25)));
}

template<class T> void TypeTraitsTest::equalsFloatingPointInfinity() {
    CORRADE_VERIFY(TypeTraits<T>::equals(Constants<T>::inf(),
                                         Constants<T>::inf()));
}

template<class T> void TypeTraitsTest::equalsFloatingPointNaN() {
    CORRADE_VERIFY(!TypeTraits<T>::equals(Constants<T>::nan(),
                                          Constants<T>::nan()));
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::TypeTraitsTest)
