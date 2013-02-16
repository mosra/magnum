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

#include "Math/Constants.h"
#include "Math/DualQuaternion.h"

namespace Magnum { namespace Math { namespace Test {

class DualQuaternionTest: public Corrade::TestSuite::Tester {
    public:
        explicit DualQuaternionTest();

        void construct();
        void constructDefault();

        void norm();

        void quaternionConjugated();
        void dualConjugated();
        void conjugated();
        void inverted();

        void rotation();
        void translation();

        void debug();
};

typedef Math::Dual<float> Dual;
typedef Math::DualQuaternion<float> DualQuaternion;
typedef Math::Quaternion<float> Quaternion;
typedef Math::Vector3<float> Vector3;

DualQuaternionTest::DualQuaternionTest() {
    addTests(&DualQuaternionTest::construct,
             &DualQuaternionTest::constructDefault,

             &DualQuaternionTest::norm,

             &DualQuaternionTest::quaternionConjugated,
             &DualQuaternionTest::dualConjugated,
             &DualQuaternionTest::conjugated,
             &DualQuaternionTest::inverted,

             &DualQuaternionTest::rotation,
             &DualQuaternionTest::translation,

             &DualQuaternionTest::debug);
}

void DualQuaternionTest::construct() {
    DualQuaternion q({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f});
    CORRADE_COMPARE(q.real(), Quaternion({1.0f, 2.0f, 3.0f}, -4.0f));
    CORRADE_COMPARE(q.dual(), Quaternion({0.5f, -3.1f, 3.3f}, 2.0f));
}

void DualQuaternionTest::constructDefault() {
    CORRADE_COMPARE(DualQuaternion(), DualQuaternion({{0.0f, 0.0f, 0.0f}, 1.0f}, {{0.0f, 0.0f, 0.0f}, 0.0f}));
}

void DualQuaternionTest::norm() {
    CORRADE_COMPARE(DualQuaternion().norm(), 1.0f);

    DualQuaternion a({{ 1.0f,  2.0f,  3.0f}, -4.0f}, {{  0.5f, -3.0f,  3.0f},  2.0f});
    CORRADE_COMPARE(a.norm(), Dual(5.477226f, -0.821584f));
}

void DualQuaternionTest::quaternionConjugated() {
    DualQuaternion a({{ 1.0f,  2.0f,  3.0f}, -4.0f}, {{ 0.5f, -3.1f,  3.3f}, 2.0f});
    DualQuaternion b({{-1.0f, -2.0f, -3.0f}, -4.0f}, {{-0.5f,  3.1f, -3.3f}, 2.0f});

    CORRADE_COMPARE(a.quaternionConjugated(), b);
}

void DualQuaternionTest::dualConjugated() {
    DualQuaternion a({{1.0f, 2.0f, 3.0f}, -4.0f}, {{ 0.5f, -3.1f,  3.3f},  2.0f});
    DualQuaternion b({{1.0f, 2.0f, 3.0f}, -4.0f}, {{-0.5f,  3.1f, -3.3f}, -2.0f});

    CORRADE_COMPARE(a.dualConjugated(), b);
}

void DualQuaternionTest::conjugated() {
    DualQuaternion a({{ 1.0f,  2.0f,  3.0f}, -4.0f}, {{ 0.5f, -3.1f,  3.3f},  2.0f});
    DualQuaternion b({{-1.0f, -2.0f, -3.0f}, -4.0f}, {{ 0.5f, -3.1f,  3.3f}, -2.0f});

    CORRADE_COMPARE(a.conjugated(), b);
}

void DualQuaternionTest::inverted() {
    DualQuaternion a({{ 1.0f,  2.0f,  3.0f}, -4.0f}, {{ 2.5f, -3.1f,  3.3f},  2.0f});
    DualQuaternion b({{-0.033333f, -0.066667f, -0.1f}, -0.133333f}, {{-0.087333f, 0.095333f, -0.122f}, 0.050667f});

    CORRADE_COMPARE(a*a.inverted(), DualQuaternion());
    CORRADE_COMPARE(a.inverted(), b);
}

void DualQuaternionTest::rotation() {
    std::ostringstream o;
    Error::setOutput(&o);

    float angle = deg(120.0f);
    Vector3 axis(1.0f/Constants<float>::sqrt3());

    CORRADE_COMPARE(DualQuaternion::rotation(angle, axis*2.0f), DualQuaternion());
    CORRADE_COMPARE(o.str(), "Math::Quaternion::rotation(): axis must be normalized\n");

    DualQuaternion q = DualQuaternion::rotation(angle, axis);
    CORRADE_COMPARE(q, DualQuaternion({Vector3(0.5f, 0.5f, 0.5f), 0.5f}, {{}, 0.0f}));
    CORRADE_COMPARE(q.rotationAngle(), angle);
    CORRADE_COMPARE(q.rotationAxis(), axis);
}

void DualQuaternionTest::translation() {
    Vector3 vec(1.0f, -3.5f, 0.5f);
    DualQuaternion q = DualQuaternion::translation(vec);
    CORRADE_COMPARE(q, DualQuaternion({}, {{0.5f, -1.75f, 0.25f}, 0.0f}));
    CORRADE_COMPARE(q.translation(), vec);
}

void DualQuaternionTest::debug() {
    std::ostringstream o;

    Debug(&o) << DualQuaternion({{1.0f, 2.0f, 3.0f}, -4.0f}, {{0.5f, -3.1f, 3.3f}, 2.0f});
    CORRADE_COMPARE(o.str(), "DualQuaternion({{1, 2, 3}, -4}, {{0.5, -3.1, 3.3}, 2})\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::DualQuaternionTest)
