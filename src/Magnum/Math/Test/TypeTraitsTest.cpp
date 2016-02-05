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

    void equalsIntegral();
    void equalsFloatingPoint0();
    void equalsFloatingPoint1();
    void equalsFloatingPointLarge();
    void equalsFloatingPointInfinity();
    void equalsFloatingPointNaN();

    private:
        template<class> void _equalsIntegral();
        template<class> void _equalsFloatingPoint0();
        template<class> void _equalsFloatingPoint1();
        template<class> void _equalsFloatingPointLarge();
        template<class> void _equalsFloatingPointInfinity();
        template<class> void _equalsFloatingPointNaN();
};

TypeTraitsTest::TypeTraitsTest() {
    addTests({&TypeTraitsTest::equalsIntegral,
              &TypeTraitsTest::equalsFloatingPoint0,
              &TypeTraitsTest::equalsFloatingPoint1,
              &TypeTraitsTest::equalsFloatingPointLarge,
              &TypeTraitsTest::equalsFloatingPointInfinity,
              &TypeTraitsTest::equalsFloatingPointNaN});
}

void TypeTraitsTest::equalsIntegral() {
    _equalsIntegral<UnsignedByte>();
    _equalsIntegral<Byte>();
    _equalsIntegral<UnsignedShort>();
    _equalsIntegral<Short>();
    _equalsIntegral<UnsignedInt>();
    _equalsIntegral<Int>();
    _equalsIntegral<UnsignedLong>();
    _equalsIntegral<Long>();
}

template<class T> void TypeTraitsTest::_equalsIntegral() {
    CORRADE_VERIFY(!TypeTraits<T>::equals(1, 1+TypeTraits<T>::epsilon()));
}

void TypeTraitsTest::equalsFloatingPoint0() {
    _equalsFloatingPoint0<Float>();
    #ifndef MAGNUM_TARGET_GLES
    _equalsFloatingPoint0<Double>();
    #endif
}

template<class T> void TypeTraitsTest::_equalsFloatingPoint0() {
    CORRADE_VERIFY(TypeTraits<T>::equals(T(0)+TypeTraits<T>::epsilon()/T(2), T(0)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(0)+TypeTraits<T>::epsilon()*T(2), T(0)));
}

void TypeTraitsTest::equalsFloatingPoint1() {
    _equalsFloatingPoint1<Float>();
    #ifndef MAGNUM_TARGET_GLES
    _equalsFloatingPoint1<Double>();
    #endif
}

template<class T> void TypeTraitsTest::_equalsFloatingPoint1() {
    CORRADE_VERIFY(TypeTraits<T>::equals(T(1)+TypeTraits<T>::epsilon()/T(2), T(1)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(1)+TypeTraits<T>::epsilon()*T(3), T(1)));
}

void TypeTraitsTest::equalsFloatingPointLarge() {
    _equalsFloatingPointLarge<Float>();
    #ifndef MAGNUM_TARGET_GLES
    _equalsFloatingPointLarge<Double>();
    #endif
}

template<class T> void TypeTraitsTest::_equalsFloatingPointLarge() {
    CORRADE_VERIFY(TypeTraits<T>::equals(T(25)+TypeTraits<T>::epsilon()*T(2), T(25)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(25)+TypeTraits<T>::epsilon()*T(75), T(25)));
}

void TypeTraitsTest::equalsFloatingPointInfinity() {
    _equalsFloatingPointInfinity<Float>();
    #ifndef MAGNUM_TARGET_GLES
    _equalsFloatingPointInfinity<Double>();
    #endif
}

template<class T> void TypeTraitsTest::_equalsFloatingPointInfinity() {
    CORRADE_VERIFY(TypeTraits<T>::equals(Constants<T>::inf(),
                                         Constants<T>::inf()));
}

void TypeTraitsTest::equalsFloatingPointNaN() {
    _equalsFloatingPointNaN<Float>();
    #ifndef MAGNUM_TARGET_GLES
    _equalsFloatingPointNaN<Double>();
    #endif
}

template<class T> void TypeTraitsTest::_equalsFloatingPointNaN() {
    CORRADE_VERIFY(!TypeTraits<T>::equals(Constants<T>::nan(),
                                          Constants<T>::nan()));
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::TypeTraitsTest)
