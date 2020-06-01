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

#include <array>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/Transform.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct TransformTest: TestSuite::Tester {
    explicit TransformTest();

    void transformVectors2D();
    void transformVectors3D();

    void transformPoints2D();
    void transformPoints3D();
};

TransformTest::TransformTest() {
    addTests({&TransformTest::transformVectors2D,
              &TransformTest::transformVectors3D,

              &TransformTest::transformPoints2D,
              &TransformTest::transformPoints3D});
}

constexpr static std::array<Vector2, 2> points2D{{
    {-3.0f,   4.0f},
    { 2.5f, -15.0f}
}};

constexpr static std::array<Vector2, 2> points2DRotated{{
    {-4.0f, -3.0f},
    {15.0f,  2.5f}
}};

constexpr static std::array<Vector2, 2> points2DRotatedTranslated{{
    {-4.0f, -4.0f},
    {15.0f,  1.5f}
}};

constexpr static std::array<Vector3, 2> points3D{{
    {-3.0f,   4.0f, 34.0f},
    { 2.5f, -15.0f,  1.5f}
}};

constexpr static std::array<Vector3, 2> points3DRotated{{
    {-4.0f, -3.0f, 34.0f},
    {15.0f,  2.5f,  1.5f}
}};

constexpr static std::array<Vector3, 2> points3DRotatedTranslated{{
    {-4.0f, -4.0f, 34.0f},
    {15.0f,  1.5f,  1.5f}
}};

void TransformTest::transformVectors2D() {
    auto matrix = MeshTools::transformVectors(Matrix3::rotation(Deg(90.0f)), points2D);
    auto complex = MeshTools::transformVectors(Complex::rotation(Deg(90.0f)), points2D);

    CORRADE_COMPARE(matrix, points2DRotated);
    CORRADE_COMPARE(complex, points2DRotated);
}

void TransformTest::transformVectors3D() {
    auto matrix = MeshTools::transformVectors(Matrix4::rotationZ(Deg(90.0f)), points3D);
    auto quaternion = MeshTools::transformVectors(Quaternion::rotation(Deg(90.0f), Vector3::zAxis()), points3D);

    CORRADE_COMPARE(matrix, points3DRotated);
    CORRADE_COMPARE(quaternion, points3DRotated);
}

void TransformTest::transformPoints2D() {
    auto matrix = MeshTools::transformPoints(
        Matrix3::translation(Vector2::yAxis(-1.0f))*Matrix3::rotation(Deg(90.0f)), points2D);
    auto complex = MeshTools::transformPoints(
        DualComplex::translation(Vector2::yAxis(-1.0f))*DualComplex::rotation(Deg(90.0f)), points2D);

    CORRADE_COMPARE(matrix, points2DRotatedTranslated);
    CORRADE_COMPARE(complex, points2DRotatedTranslated);
}

void TransformTest::transformPoints3D() {
    auto matrix = MeshTools::transformPoints(
        Matrix4::translation(Vector3::yAxis(-1.0f))*Matrix4::rotationZ(Deg(90.0f)), points3D);
    auto quaternion = MeshTools::transformPoints(
        DualQuaternion::translation(Vector3::yAxis(-1.0f))*DualQuaternion::rotation(Deg(90.0f), Vector3::zAxis()), points3D);

    CORRADE_COMPARE(matrix, points3DRotatedTranslated);
    CORRADE_COMPARE(quaternion, points3DRotatedTranslated);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::TransformTest)
