/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
#include <TestSuite/Tester.h>

#include "Math/TypeTraits.h"

namespace Magnum { namespace Math { namespace Test {

class TypeTraitsTest: public Corrade::TestSuite::Tester {
    public:
        TypeTraitsTest();

        void equalsFloatingPoint();
        void equalsIntegral();

    private:
        template<class T> void _equalsFloatingPoint();
        template<class T> void _equalsIntegral();
};

TypeTraitsTest::TypeTraitsTest() {
    addTests<TypeTraitsTest>({&TypeTraitsTest::equalsIntegral,
              &TypeTraitsTest::equalsFloatingPoint});
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

void TypeTraitsTest::equalsFloatingPoint() {
    _equalsFloatingPoint<Float>();
    #ifndef MAGNUM_TARGET_GLES
    _equalsFloatingPoint<Double>();
    #endif
}

template<class T> void TypeTraitsTest::_equalsIntegral() {
    CORRADE_VERIFY(!TypeTraits<T>::equals(1, 1+TypeTraits<T>::epsilon()));
}

template<class T> void TypeTraitsTest::_equalsFloatingPoint() {
    CORRADE_VERIFY(TypeTraits<T>::equals(T(1)+TypeTraits<T>::epsilon()/T(2), T(1)));
    CORRADE_VERIFY(!TypeTraits<T>::equals(T(1)+TypeTraits<T>::epsilon()*T(2), T(1)));

    {
        CORRADE_EXPECT_FAIL("Comparing to infinity is broken");
        CORRADE_VERIFY(TypeTraits<T>::equals(std::numeric_limits<T>::infinity(),
                                                std::numeric_limits<T>::infinity()));
    }

    CORRADE_VERIFY(!TypeTraits<T>::equals(std::numeric_limits<T>::quiet_NaN(),
                                              std::numeric_limits<T>::quiet_NaN()));
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::TypeTraitsTest)
