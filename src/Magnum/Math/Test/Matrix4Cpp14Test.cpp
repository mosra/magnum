/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include "Magnum/Math/Matrix4.h"

namespace Magnum { namespace Math { namespace Test { namespace {

struct Matrix4Cpp14Test: TestSuite::Tester {
    explicit Matrix4Cpp14Test();

    void orthographicProjectionConstexpr();
    void orthographicProjectionOffCenterConstexpr();
    void perspectiveProjectionConstexpr();
    void perspectiveProjectionInfiniteFarConstexpr();
    void perspectiveProjectionOffCenterConstexpr();
    void perspectiveProjectionOffCenterInfiniteFarConstexpr();
    /* The FoV variant of perspectiveProjection() uses trig functions that are
       only constexpr since C++26, don't want to bother with that yet */
};

/* What's a typedef and not a using differs from the typedefs in root Magnum
   namespace, or is not present there at all */
using Magnum::Constants;
using Magnum::Matrix4;
using Magnum::Vector3;

Matrix4Cpp14Test::Matrix4Cpp14Test() {
    addTests({&Matrix4Cpp14Test::orthographicProjectionConstexpr,
              &Matrix4Cpp14Test::orthographicProjectionOffCenterConstexpr,
              &Matrix4Cpp14Test::perspectiveProjectionConstexpr,
              &Matrix4Cpp14Test::perspectiveProjectionInfiniteFarConstexpr,
              &Matrix4Cpp14Test::perspectiveProjectionOffCenterConstexpr,
              &Matrix4Cpp14Test::perspectiveProjectionOffCenterInfiniteFarConstexpr});
}

void Matrix4Cpp14Test::orthographicProjectionConstexpr() {
    /* Like Matrix4Test::orthographicProjection(), but testing usability in a
       constexpr context */

    constexpr Matrix4 actual = Matrix4::orthographicProjection({5.0f, 4.0f}, 1.0f, 9.0f);
    CORRADE_COMPARE(actual, (Matrix4{
        {0.4f, 0.0f,   0.0f, 0.0f},
        {0.0f, 0.5f,   0.0f, 0.0f},
        {0.0f, 0.0f, -0.25f, 0.0f},
        {0.0f, 0.0f, -1.25f, 1.0f}}));

    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -1.0f}), (Vector3{0.0f, 0.0f, -1.0f}));
    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -9.0f}), (Vector3{0.0f, 0.0f, +1.0f}));
}

void Matrix4Cpp14Test::orthographicProjectionOffCenterConstexpr() {
    /* Like Matrix4Test::orthographicProjectionOffCenter(), but testing
       usability in a constexpr context */

    constexpr Matrix4 actual = Matrix4::orthographicProjection({-3.5f, -2.5f}, {1.5f, 1.5f}, 1.0f, 9.0f);
    CORRADE_COMPARE(actual, (Matrix4{
        {0.4f,  0.0f,   0.0f, 0.0f},
        {0.0f,  0.5f,   0.0f, 0.0f},
        {0.0f,  0.0f, -0.25f, 0.0f},
        {0.4f, 0.25f, -1.25f, 1.0f}}));

    CORRADE_COMPARE(actual.transformPoint({1.5f, 1.5f, -1.0f}), (Vector3{1.0f, 1.0f, -1.0f}));
    CORRADE_COMPARE(actual.transformPoint({-3.5f, -2.5f, -9.0f}), (Vector3{-1.0f, -1.0f, +1.0f}));
}

void Matrix4Cpp14Test::perspectiveProjectionConstexpr() {
    /* Like Matrix4Test::perspectiveProjection(), but testing usability in a
       constexpr context */

    Matrix4 expected{{4.0f,      0.0f,         0.0f,  0.0f},
                     {0.0f, 7.111111f,         0.0f,  0.0f},
                     {0.0f,      0.0f,  -1.9411764f, -1.0f},
                     {0.0f,      0.0f, -94.1176452f,  0.0f}};
    constexpr Matrix4 actual = Matrix4::perspectiveProjection({16.0f, 9.0f}, 32.0f, 100.0f);
    CORRADE_COMPARE(actual, expected);

    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -32.0f}), Vector3(0.0f, 0.0f, -1.0f));
    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -100.0f}), Vector3(0.0f, 0.0f, +1.0f));

    CORRADE_COMPARE(Matrix4::perspectiveProjection({-8.0f, -4.5f}, {8.0f, 4.5f}, 32.0f, 100.0f), expected);
}

void Matrix4Cpp14Test::perspectiveProjectionInfiniteFarConstexpr() {
    /* Like Matrix4Test::perspectiveProjectionInfiniteFar(), but testing
       usability in a constexpr context */

    Matrix4 expected{{4.0f,      0.0f,   0.0f,  0.0f},
                     {0.0f, 7.111111f,   0.0f,  0.0f},
                     {0.0f,      0.0f,  -1.0f, -1.0f},
                     {0.0f,      0.0f, -64.0f,  0.0f}};
    constexpr Matrix4 actual = Matrix4::perspectiveProjection({16.0f, 9.0f}, 32.0f, Constants::inf());
    CORRADE_COMPARE(actual, expected);

    CORRADE_COMPARE(actual.transformPoint({0.0f, 0.0f, -32.0f}), Vector3(0.0f, 0.0f, -1.0f));
    CORRADE_COMPARE(actual.transformVector({0.0f, 0.0f, -1.0f}), Vector3(0.0f, 0.0f, +1.0f));

    CORRADE_COMPARE(Matrix4::perspectiveProjection({-8.0f, -4.5f}, {8.0f, 4.5f}, 32.0f, Constants::inf()), expected);
}

void Matrix4Cpp14Test::perspectiveProjectionOffCenterConstexpr() {
    /* Like Matrix4Test::perspectiveProjectionOffCenter(), but testing
       usability in a constexpr context */

    constexpr Matrix4 projection = Matrix4::perspectiveProjection({-9.0f, -5.0f}, {7.0f, 4.0f}, 32.0f, 100.0f);
    CORRADE_COMPARE(projection, (Matrix4{
        {   4.0f,        0.0f,         0.0f,  0.0f},
        {   0.0f,   7.111111f,         0.0f,  0.0f},
        {-0.125f, -0.1111111f,  -1.9411764f, -1.0f},
        {   0.0f,        0.0f, -94.1176452f,  0.0f}}));

    CORRADE_COMPARE(projection.transformPoint({7.0f, 4.0f, -32.0f}), Vector3(1.0f, 1.0f, -1.0f));
    CORRADE_COMPARE(projection.transformPoint({0.0f, 0.0f, -100.0f}), Vector3(0.125f, 0.1111111f, +1.0f));
}

void Matrix4Cpp14Test::perspectiveProjectionOffCenterInfiniteFarConstexpr() {
    /* Like Matrix4Test::perspectiveProjectionOffCenterInfiniteFar(), but
       testing usability in a constexpr context */

    constexpr Matrix4 projection = Matrix4::perspectiveProjection({-9.0f, -5.0f}, {7.0f, 4.0f}, 32.0f, Constants::inf());
    CORRADE_COMPARE(projection, (Matrix4{
        {   4.0f,        0.0f,   0.0f,  0.0f},
        {   0.0f,   7.111111f,   0.0f,  0.0f},
        {-0.125f, -0.1111111f,  -1.0f, -1.0f},
        {   0.0f,        0.0f, -64.0f,  0.0f}}));

    CORRADE_COMPARE(projection.transformPoint({-9.0f, -5.0f, -32.0f}), Vector3(-1.0f, -1.0f, -1.0f));
    CORRADE_COMPARE(projection.transformVector({0.0f, 0.0f, -1.0f}), Vector3(0.125f, 0.1111111f, +1.0f));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Math::Test::Matrix4Cpp14Test)
