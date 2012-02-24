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

#include "TypeTraitsTest.h"

#include <QtTest/QTest>

#include "TypeTraits.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::TypeTraitsTest)

namespace Magnum { namespace Math { namespace Test {

void TypeTraitsTest::equalsIntegral() {
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

void TypeTraitsTest::equalsFloatingPoint() {
    _equalsFloatingPoint<float>();
    _equalsFloatingPoint<double>();
}

template<class T> void TypeTraitsTest::_equalsIntegral() {
    QVERIFY(!TypeTraits<T>::equals(1, 1+TypeTraits<T>::epsilon()));
}

template<class T> void TypeTraitsTest::_equalsFloatingPoint() {
    QVERIFY(TypeTraits<T>::equals(1.0f+TypeTraits<T>::epsilon()/2, 1.0f));
    QVERIFY(!TypeTraits<T>::equals(1.0f+TypeTraits<T>::epsilon()*2, 1.0f));

    QEXPECT_FAIL(0, "Comparing to infinity is broken", Continue);
    QVERIFY(TypeTraits<T>::equals(std::numeric_limits<T>::infinity(),
                                  std::numeric_limits<T>::infinity()));
    QVERIFY(!TypeTraits<T>::equals(std::numeric_limits<T>::quiet_NaN(),
                                   std::numeric_limits<T>::quiet_NaN()));
}

}}}
