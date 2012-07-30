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

#include "MathTypeTraitsTest.h"

#include <limits>
#include <QtTest/QTest>

#include "MathTypeTraits.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::MathTypeTraitsTest)

namespace Magnum { namespace Math { namespace Test {

void MathTypeTraitsTest::equalsIntegral() {
    _equalsIntegral<unsigned char>();
    _equalsIntegral<char>();
    _equalsIntegral<unsigned short>();
    _equalsIntegral<short>();
    _equalsIntegral<unsigned int>();
    _equalsIntegral<int>();
    _equalsIntegral<unsigned long int>();
    _equalsIntegral<long int>();
    _equalsIntegral<unsigned long long>();
    _equalsIntegral<long long>();
}

void MathTypeTraitsTest::equalsFloatingPoint() {
    _equalsFloatingPoint<float>();
    _equalsFloatingPoint<double>();
}

template<class T> void MathTypeTraitsTest::_equalsIntegral() {
    QVERIFY(!MathTypeTraits<T>::equals(1, 1+MathTypeTraits<T>::epsilon()));
}

template<class T> void MathTypeTraitsTest::_equalsFloatingPoint() {
    QVERIFY(MathTypeTraits<T>::equals(T(1)+MathTypeTraits<T>::epsilon()/T(2), T(1)));
    QVERIFY(!MathTypeTraits<T>::equals(T(1)+MathTypeTraits<T>::epsilon()*T(2), T(1)));

    QEXPECT_FAIL(0, "Comparing to infinity is broken", Continue);
    QVERIFY(MathTypeTraits<T>::equals(std::numeric_limits<T>::infinity(),
                                  std::numeric_limits<T>::infinity()));
    QVERIFY(!MathTypeTraits<T>::equals(std::numeric_limits<T>::quiet_NaN(),
                                   std::numeric_limits<T>::quiet_NaN()));
}

}}}
