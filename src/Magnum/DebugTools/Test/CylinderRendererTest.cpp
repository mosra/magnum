/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "../Implementation/CylinderRendererTransformation.h"

namespace Magnum { namespace DebugTools { namespace Test {

struct CylinderRendererTest: TestSuite::Tester {
    explicit CylinderRendererTest();

    void zeroLength2D();
    void common2D();

    void zeroLength3D();
    void parallel3D();
    void antiParallel3D();
    void common3D();
};

CylinderRendererTest::CylinderRendererTest() {
    addTests({&CylinderRendererTest::zeroLength2D,
              &CylinderRendererTest::common2D,

              &CylinderRendererTest::zeroLength3D,
              &CylinderRendererTest::parallel3D,
              &CylinderRendererTest::antiParallel3D,
              &CylinderRendererTest::common3D});
}

void CylinderRendererTest::zeroLength2D() {
    const Vector2 a(0.5f, 3.0f);
    const Matrix3 transformation = Implementation::cylinderRendererTransformation<2>(a, a, 3.5f);

    CORRADE_COMPARE(transformation.rotationScaling(), Matrix2x2::fromDiagonal({3.5f, 0.0f}));
    CORRADE_COMPARE(transformation.translation(), a);
}

void CylinderRendererTest::common2D() {
    const Vector2 a(0.5f, 3.0f);
    const Vector2 b(7.5f, -1.0f);
    const Matrix3 transformation = Implementation::cylinderRendererTransformation<2>(a, b, 3.5f);

    /* Rotation + scaling, test orthogonality */
    CORRADE_COMPARE(transformation.up(), Vector2(3.5f, -2.0f));
    CORRADE_COMPARE(transformation.right(), Vector2(4.0f, 7.0f).resized(3.5f));
    CORRADE_COMPARE(Math::dot(transformation.up(), transformation.right()), 0.0f);

    CORRADE_COMPARE(transformation.translation(), 0.5f*(a + b));
}

void CylinderRendererTest::zeroLength3D() {
    const Vector3 a(0.5f, 3.0f, 7.0f);
    const Matrix4 transformation = Implementation::cylinderRendererTransformation<3>(a, a, 3.5f);

    CORRADE_COMPARE(transformation.rotationScaling(), Matrix3x3::fromDiagonal({3.5f, 0.0f, 3.5f}));
    CORRADE_COMPARE(transformation.translation(), a);
}

void CylinderRendererTest::parallel3D() {
    const Vector3 a(0.5f, 3.0f, 7.0f);
    const Vector3 b(0.5f, 3.0f, 11.0f);
    const Matrix4 transformation = Implementation::cylinderRendererTransformation<3>(a, b, 3.5f);

    CORRADE_COMPARE(transformation.rotationScaling(),
        (Matrix4::rotationX(Deg(90.0f))*Matrix4::scaling({3.5f, 2.0f, 3.5f})).rotationScaling());

    CORRADE_COMPARE(transformation.translation(), a+Vector3::zAxis(2.0f));
}

void CylinderRendererTest::antiParallel3D() {
    const Vector3 a(0.5f, 3.0f, 7.0f);
    const Vector3 b(0.5f, 3.0f, 3.0f);
    const Matrix4 transformation = Implementation::cylinderRendererTransformation<3>(a, b, 3.5f);

    CORRADE_COMPARE(transformation.rotationScaling(),
        (Matrix4::rotationX(-Deg(90.0f))*Matrix4::scaling({3.5f, 2.0f, 3.5f})).rotationScaling());

    CORRADE_COMPARE(transformation.translation(), a+Vector3::zAxis(-2.0f));
}

void CylinderRendererTest::common3D() {
    const Vector3 a(0.5f, 3.0f, 7.0f);
    const Vector3 b(7.5f, -1.0f, 1.5f);
    const Matrix4 transformation = Implementation::cylinderRendererTransformation<3>(a, b, 3.5f);

    /* Rotation + scaling */
    CORRADE_COMPARE(transformation.up(), Vector3(3.5f, -2.0f, -2.75f));
    CORRADE_COMPARE(transformation.right(), Vector3(-2.0f, -3.5f, 0.0f).resized(3.5f));
    CORRADE_COMPARE(transformation.backward(), Vector3(9.625f, -5.5f, 16.25f).resized(3.5f));

    /* Orthogonality */
    CORRADE_COMPARE(Math::dot(transformation.up(), transformation.right()), 0.0f);
    CORRADE_COMPARE(Math::dot(transformation.up(), transformation.backward()), 0.0f);
    CORRADE_COMPARE(Math::dot(transformation.right(), transformation.backward()), 0.0f);

    CORRADE_COMPARE(transformation.translation(), 0.5f*(a + b));
}

}}}

CORRADE_TEST_MAIN(Magnum::DebugTools::Test::CylinderRendererTest)
