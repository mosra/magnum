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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Math/Frustum.h"

namespace Magnum { namespace Math { namespace Test {

struct FrustumTest: Corrade::TestSuite::Tester {
    explicit FrustumTest();

    void construct();
    void constructFromMatrix();

    void compare();

    void debug();
};

typedef Math::Vector4<Float> Vector4;
typedef Math::Matrix4<Float> Matrix4;
typedef Math::Frustum<Float> Frustum;

FrustumTest::FrustumTest() {
    addTests({&FrustumTest::construct,
              &FrustumTest::constructFromMatrix,

              &FrustumTest::compare,

              &FrustumTest::debug});
}

void FrustumTest::construct() {
    Vector4 planes[6]{
        {-1.0f,  0.0f,  0.0f, 1.0f},
        { 1.0f,  0.0f,  0.0f, 1.0f},
        { 0.0f, -1.0f,  0.0f, 1.0f},
        { 0.0f,  1.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, -1.0f, 1.0f},
        { 0.0f,  0.0f,  1.0f, 1.0f}};

    Frustum frustum{
        planes[0], planes[1],
        planes[2], planes[3],
        planes[4], planes[5]};

    CORRADE_COMPARE_AS(frustum.planes(), Corrade::Containers::ArrayView<const Vector4>(planes),
                       Corrade::TestSuite::Compare::Container);
}

void FrustumTest::constructFromMatrix() {
    using namespace Magnum::Math::Literals;

    Frustum expected{
        { 1.0f,  0.0f, -1.0f, 0.0f},
        {-1.0f,  0.0f, -1.0f, 0.0f},
        { 0.0f,  1.0f, -1.0f, 0.0f},
        { 0.0f, -1.0f, -1.0f, 0.0f},
        { 0.0f,  0.0f, -2.22222f, -2.22222f},
        { 0.0f,  0.0f,  0.22222f,  2.22222f}};

    const Frustum frustum = Frustum::fromMatrix(
            Matrix4::perspectiveProjection(90.0_degf, 1.0f, 1.0f, 10.0f));

    CORRADE_COMPARE(frustum, expected);
}

void FrustumTest::compare() {
    Frustum a{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};

    Frustum b{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f + Math::TypeTraits<Float>::epsilon()/2.0f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};

    Frustum c{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f + Math::TypeTraits<Float>::epsilon()*2.0f},
        { 7.0f,  8.0f,  9.0f, 0.6f}};

    CORRADE_VERIFY(a == a);
    CORRADE_VERIFY(a == b);
    CORRADE_VERIFY(a != c);
}

void FrustumTest::debug() {
    Frustum frustum{
        {-1.0f,  2.0f, -3.0f, 0.1f},
        { 1.0f, -2.0f,  3.0f, 0.2f},
        {-4.0f,  5.0f, -6.0f, 0.3f},
        { 4.0f, -5.0f,  6.0f, 0.4f},
        {-7.0f,  8.0f, -9.0f, 0.5f},
        { 7.0f, -8.0f,  9.0f, 0.6f}};

    std::ostringstream out;
    Debug{&out} << frustum;
    CORRADE_COMPARE(out.str(), "Frustum({-1, 2, -3, 0.1},\n"
                               "        {1, -2, 3, 0.2},\n"
                               "        {-4, 5, -6, 0.3},\n"
                               "        {4, -5, 6, 0.4},\n"
                               "        {-7, 8, -9, 0.5},\n"
                               "        {7, -8, 9, 0.6})\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::FrustumTest)
