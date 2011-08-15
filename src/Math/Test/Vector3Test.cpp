/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Vector3Test.h"

#include <QtTest/QTest>

#include "Vector3.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::Vector3Test)

namespace Magnum { namespace Math { namespace Test {

typedef Math::Vector3<float> Vector3;

void Vector3Test::cross() {
    Vector3 a(1, -1, 1);
    Vector3 b(4, 3, 7);

    QVERIFY(Vector3::cross(a, b) == Vector3(-10, -3, 7));
}

}}}
