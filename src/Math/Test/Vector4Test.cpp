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

#include <sstream>
#include <TestSuite/Tester.h>
#include <Utility/Configuration.h>

#include "Math/Vector4.h"

namespace Magnum { namespace Math { namespace Test {

class Vector4Test: public Corrade::TestSuite::Tester {
    public:
        Vector4Test();

        void access();
        void construct();
        void threeComponent();
        void twoComponent();

        void debug();
        void configuration();
};

typedef Math::Vector4<float> Vector4;
typedef Math::Vector3<float> Vector3;
typedef Math::Vector2<float> Vector2;

Vector4Test::Vector4Test() {
    addTests(&Vector4Test::construct,
             &Vector4Test::access,
             &Vector4Test::threeComponent,
             &Vector4Test::twoComponent,
             &Vector4Test::debug,
             &Vector4Test::configuration);
}

void Vector4Test::construct() {
    CORRADE_COMPARE(Vector4(), Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    CORRADE_COMPARE(Vector4(1, 2, 3, 4), (Vector<4, float>(1.0f, 2.0f, 3.0f, 4.0f)));
    CORRADE_COMPARE(Vector4(Vector<3, float>(1.0f, 2.0f, 3.0f), 4), (Vector<4, float>(1.0f, 2.0f, 3.0f, 4.0f)));
}

void Vector4Test::access() {
    Vector4 vec(1.0f, -2.0f, 5.0f, 0.5f);
    const Vector4 cvec(1.0f, -2.0f, 5.0f, 0.5f);

    CORRADE_COMPARE(vec.x(), 1.0f);
    CORRADE_COMPARE(vec.y(), -2.0f);
    CORRADE_COMPARE(vec.z(), 5.0f);
    CORRADE_COMPARE(vec.w(), 0.5f);
    CORRADE_COMPARE(cvec.x(), 1.0f);
    CORRADE_COMPARE(cvec.y(), -2.0f);
    CORRADE_COMPARE(cvec.z(), 5.0f);
    CORRADE_COMPARE(cvec.w(), 0.5f);
}

void Vector4Test::threeComponent() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).xyz(), Vector3(1.0f, 2.0f, 3.0f));
}

void Vector4Test::twoComponent() {
    CORRADE_COMPARE(Vector4(1.0f, 2.0f, 3.0f, 4.0f).xy(), Vector2(1.0f, 2.0f));
}

void Vector4Test::debug() {
    std::ostringstream o;
    Debug(&o) << Vector4(0.5f, 15.0f, 1.0f, 1.0f);
    CORRADE_COMPARE(o.str(), "Vector(0.5, 15, 1, 1)\n");
}

void Vector4Test::configuration() {
    Corrade::Utility::Configuration c;

    Vector4 vec(3.0f, 3.125f, 9.0f, 9.55f);
    std::string value("3 3.125 9 9.55");

    c.setValue("vector", vec);
    CORRADE_COMPARE(c.value("vector"), value);
    CORRADE_COMPARE(c.value<Vector4>("vector"), vec);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Vector4Test)
