/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Vector4Test.h"

#include <QtTest/QTest>

#include "Vector4.h"

QTEST_APPLESS_MAIN(Magnum::Test::Vector4Test)

namespace Magnum { namespace Test {

typedef Magnum::Vector4<float> Vector4;
typedef Magnum::Vector3<float> Vector3;

void Vector4Test::construct() {
    QVERIFY(Vector4() == Vector4(0.0f, 0.0f, 0.0f, 1.0f));
}

void Vector4Test::threeComponent() {
    QVERIFY(Vector4(1.0f, 2.0f, 3.0f, 4.0f).xyz() == Vector3(1.0f, 2.0f, 3.0f));
}

}}
