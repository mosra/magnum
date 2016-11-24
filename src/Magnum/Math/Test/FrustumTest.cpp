/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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

#include <cmath>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Math/Frustum.h"

using namespace Corrade;

namespace Magnum { namespace Math { namespace Test {

struct FrustumTest: TestSuite::Tester {
    explicit FrustumTest();

    void construct();
    void constructFromMatrix();
};

typedef Vector4<Float> Vector4;
typedef Matrix4<Float> Matrix4;
typedef Frustum<Float> Frustum;
typedef Deg<Float> Degf;

FrustumTest::FrustumTest() {
    addTests({&FrustumTest::construct,
              &FrustumTest::constructFromMatrix});
}

void FrustumTest::construct() {
    Vector4 planes[6]{
        {-1.0f, 0.0f, 0.0f, 1.0f},
        { 1.0f, 0.0f, 0.0f, 1.0f},
        { 0.0f,-1.0f, 0.0f, 1.0f},
        { 0.0f, 1.0f, 0.0f, 1.0f},
        { 0.0f, 0.0f,-1.0f, 1.0f},
        { 0.0f, 0.0f, 1.0f, 1.0f}};

    Frustum frustum{
        planes[0], planes[1],
        planes[2], planes[3],
        planes[4], planes[5],
    };

    CORRADE_COMPARE_AS(frustum.planes(), Containers::ArrayView<const Vector4>(planes), TestSuite::Compare::Container);
}

void FrustumTest::constructFromMatrix() {
    Vector4 planes[6]{
        { 1.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f, 0.0f,-1.0f, 0.0f},
        { 0.0f, 1.0f,-1.0f, 0.0f},
        { 0.0f,-1.0f,-1.0f, 0.0f},
        { 0.0f, 0.0f,-2.22222f,-2.22222f},
        { 0.0f, 0.0f, 0.22222f, 2.22222f}};

    const Frustum frustum = Frustum::fromMatrix(
            Matrix4::perspectiveProjection(Degf(90.0f), 1.0f, 1.0f, 10.0f));

    CORRADE_COMPARE_AS(frustum.planes(), Containers::ArrayView<const Vector4>(planes), TestSuite::Compare::Container);
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FrustumTest)
