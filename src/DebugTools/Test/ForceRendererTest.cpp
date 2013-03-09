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

#include <TestSuite/Tester.h>

#include "DebugTools/Implementation/ForceRendererTransformation.h"

namespace Magnum { namespace DebugTools { namespace Implementation { namespace Test {

class ForceRendererTest: public Corrade::TestSuite::Tester {
    public:
        explicit ForceRendererTest();

        void zero2D();
        void parallel2D();
        void antiParallel2D();
        void arbitrary2D();

        void zero3D();
        void parallel3D();
        void antiParallel3D();
        void arbitrary3D();
};

ForceRendererTest::ForceRendererTest() {
    addTests({&ForceRendererTest::zero2D,
              &ForceRendererTest::parallel2D,
              &ForceRendererTest::antiParallel2D,
              &ForceRendererTest::arbitrary2D,

              &ForceRendererTest::zero3D,
              &ForceRendererTest::parallel3D,
              &ForceRendererTest::antiParallel3D,
              &ForceRendererTest::arbitrary3D});
}

void ForceRendererTest::zero2D() {
    CORRADE_COMPARE(Implementation::forceRendererTransformation<2>({0.5f, -3.0f}, Vector2()),
                    Matrix3::translation({0.5f, -3.0f})*Matrix3::scaling(Vector2(0.0f)));
}

void ForceRendererTest::parallel2D() {
    CORRADE_COMPARE(Implementation::forceRendererTransformation<2>({0.5f, -3.0f}, Vector2::xAxis(2.5f)),
                    Matrix3::translation({0.5f, -3.0f})*Matrix3::scaling(Vector2(2.5f)));
}

void ForceRendererTest::antiParallel2D() {
    CORRADE_COMPARE(Implementation::forceRendererTransformation<2>({0.5f, -3.0f}, Vector2::xAxis(-2.5f)),
                    Matrix3::translation({0.5f, -3.0f})*Matrix3::scaling(Vector2(-2.5f)));
}

void ForceRendererTest::arbitrary2D() {
    Vector2 force(2.7f, -11.5f);
    Matrix3 m = Implementation::forceRendererTransformation<2>({0.5f, -3.0f}, force);

    /* Translation, right-pointing base vector is the same as force */
    CORRADE_COMPARE(m.translation(), Vector2(0.5f, -3.0f));
    CORRADE_COMPARE(m.right(), force);

    /* All vectors have the same length */
    CORRADE_COMPARE(m.up().length(), force.length());

    /* All vectors are parallel */
    CORRADE_COMPARE(Vector2::dot(m.right(), m.up()), 0.0f);
}

void ForceRendererTest::zero3D() {
    CORRADE_COMPARE(Implementation::forceRendererTransformation<3>({0.5f, -3.0f, 1.0f}, Vector3()),
                    Matrix4::translation({0.5f, -3.0f, 1.0f})*Matrix4::scaling(Vector3(0.0f)));
}

void ForceRendererTest::parallel3D() {
    CORRADE_COMPARE(Implementation::forceRendererTransformation<3>({0.5f, -3.0f, 1.0f}, Vector3::xAxis(2.5f)),
                    Matrix4::translation({0.5f, -3.0f, 1.0f})*Matrix4::scaling(Vector3(2.5f)));
}

void ForceRendererTest::antiParallel3D() {
    CORRADE_COMPARE(Implementation::forceRendererTransformation<3>({0.5f, -3.0f, 1.0f}, Vector3::xAxis(-2.5f)),
                    Matrix4::translation({0.5f, -3.0f, 1.0f})*Matrix4::scaling({-2.5f, 2.5f, 2.5f}));
}

void ForceRendererTest::arbitrary3D() {
    Vector3 force(3.7f, -5.7f, -11.5f);
    Matrix4 m = Implementation::forceRendererTransformation<3>({0.5f, -3.0f, 1.0f}, force);

    /* Translation, right-pointing base vector is the same as force */
    CORRADE_COMPARE(m.translation(), Vector3(0.5f, -3.0f, 1.0f));
    CORRADE_COMPARE(m.right(), force);

    /* All vectors have the same length */
    CORRADE_COMPARE(m.up().length(), force.length());
    CORRADE_COMPARE(m.backward().length(), force.length());

    /* All vectors are parallel */
    CORRADE_COMPARE(Vector3::dot(m.right(), m.up()),       0.0f);
    CORRADE_COMPARE(Vector3::dot(m.right(), m.backward()), 0.0f);
    CORRADE_COMPARE(Vector3::dot(m.up(),    m.backward()), 0.0f);
}

}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Implementation::Test::ForceRendererTest)
