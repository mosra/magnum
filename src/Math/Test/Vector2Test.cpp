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

#include "Vector2Test.h"

#include <sstream>
#include <QtTest/QTest>

#include "Vector2.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::Vector2Test)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Math::Vector2<float> Vector2;

void Vector2Test::debug() {
    ostringstream o;
    Debug(&o) << Vector2(0.5f, 15.0f);
    QCOMPARE(QString::fromStdString(o.str()), QString("Vector(0.5, 15)\n"));
}

}}}
