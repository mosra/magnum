/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/DebugTools/Implementation/ForceRendererTransformation.h"

namespace Magnum { namespace DebugTools { namespace Implementation { namespace Test { namespace {

struct ForceRendererTest: TestSuite::Tester {
    explicit ForceRendererTest();

    void zero2D();
    void common2D();

    void zero3D();
    void parallel3D();
    void antiParallel3D();
    void arbitrary3D();
};

ForceRendererTest::ForceRendererTest() {
    addTests({&ForceRendererTest::zero2D,
              &ForceRendererTest::common2D,

              &ForceRendererTest::zero3D,
              &ForceRendererTest::parallel3D,
              &ForceRendererTest::antiParallel3D,
              &ForceRendererTest::arbitrary3D});
}

void ForceRendererTest::zero2D() {
    CORRADE_COMPARE(Implementation::forceRendererTransformation<2>({0.5f, -3.0f}, Vector2()),
                    Matrix3::translation({0.5f, -3.0f})*Matrix3::scaling(Vector2(0.0f)));
}

void ForceRendererTest::common2D() {
    Vector2 force(2.7f, -11.5f);
    Matrix3 m = Implementation::forceRendererTransformation<2>({0.5f, -3.0f}, force);

    /* Translation, right-pointing base vector is the same as force */
    CORRADE_COMPARE(m.translation(), Vector2(0.5f, -3.0f));
    CORRADE_COMPARE(m.right(), force);

    /* All vectors have the same length */
    CORRADE_COMPARE(m.up().length(), force.length());

    /* All vectors are orthogonal */
    CORRADE_COMPARE(Math::dot(m.right(), m.up()), 0.0f);
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

    /* All vectors are orthogonal */
    CORRADE_COMPARE(Math::dot(m.right(), m.up()),       0.0f);
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_COMPARE(Math::dot(m.right(), m.backward()), 0.0f);
    #else
    /* Emscripten -O1 has slightly lower precision. -O2 works fine. */
    CORRADE_COMPARE_AS(Math::abs(Math::dot(m.right(), m.backward())),
        Math::TypeTraits<Float>::epsilon(),
        TestSuite::Compare::Less);
    #endif
    /** @todo This shouldn't be too different */
    CORRADE_COMPARE_AS(Math::abs(Math::dot(m.up(), m.backward())),
        Math::TypeTraits<Float>::epsilon(),
        TestSuite::Compare::Less);
}

}}}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Implementation::Test::ForceRendererTest)
