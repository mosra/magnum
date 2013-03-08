/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <limits>
#include <TestSuite/Tester.h>

#include "Math/MathTypeTraits.h"

namespace Magnum { namespace Math { namespace Test {

class MathTypeTraitsTest: public Corrade::TestSuite::Tester {
    public:
        MathTypeTraitsTest();

        void equalsFloatingPoint();
        void equalsIntegral();

    private:
        template<class T> void _equalsFloatingPoint();
        template<class T> void _equalsIntegral();
};

MathTypeTraitsTest::MathTypeTraitsTest() {
    addTests({&MathTypeTraitsTest::equalsIntegral,
              &MathTypeTraitsTest::equalsFloatingPoint});
}

void MathTypeTraitsTest::equalsIntegral() {
    _equalsIntegral<UnsignedByte>();
    _equalsIntegral<Byte>();
    _equalsIntegral<UnsignedShort>();
    _equalsIntegral<Short>();
    _equalsIntegral<UnsignedInt>();
    _equalsIntegral<Int>();
    _equalsIntegral<UnsignedLong>();
    _equalsIntegral<Long>();
}

void MathTypeTraitsTest::equalsFloatingPoint() {
    _equalsFloatingPoint<Float>();
    _equalsFloatingPoint<Double>();
}

template<class T> void MathTypeTraitsTest::_equalsIntegral() {
    CORRADE_VERIFY(!MathTypeTraits<T>::equals(1, 1+MathTypeTraits<T>::epsilon()));
}

template<class T> void MathTypeTraitsTest::_equalsFloatingPoint() {
    CORRADE_VERIFY(MathTypeTraits<T>::equals(T(1)+MathTypeTraits<T>::epsilon()/T(2), T(1)));
    CORRADE_VERIFY(!MathTypeTraits<T>::equals(T(1)+MathTypeTraits<T>::epsilon()*T(2), T(1)));

    {
        CORRADE_EXPECT_FAIL("Comparing to infinity is broken");
        CORRADE_VERIFY(MathTypeTraits<T>::equals(std::numeric_limits<T>::infinity(),
                                                std::numeric_limits<T>::infinity()));
    }

    CORRADE_VERIFY(!MathTypeTraits<T>::equals(std::numeric_limits<T>::quiet_NaN(),
                                              std::numeric_limits<T>::quiet_NaN()));
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::MathTypeTraitsTest)
