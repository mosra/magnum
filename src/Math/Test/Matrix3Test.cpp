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

#include "Matrix3Test.h"

#include <sstream>
#include <QtTest/QTest>

#include "Matrix3.h"

QTEST_APPLESS_MAIN(Magnum::Math::Test::Matrix3Test)

using namespace std;
using namespace Corrade::Utility;

namespace Magnum { namespace Math { namespace Test {

typedef Math::Matrix3<float> Matrix3;

void Matrix3Test::debug() {
    float m[] = {
        3, 5, 8,
        4, 4, 7,
        7, -1, 8,
    };

    ostringstream o;
    Debug(&o) << Matrix3(m);
    QCOMPARE(QString::fromStdString(o.str()), QString("Matrix(3, 4, 7,\n"
                                                      "       5, 4, -1,\n"
                                                      "       8, 7, 8)\n"));
}

}}}
