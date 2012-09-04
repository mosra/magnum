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

/* Less precision */
#define FLOAT_EQUALITY_PRECISION 1.0e-5

#include <TestSuite/Compare/Container.h>

#include "CapsuleTest.h"

#include "Primitives/Capsule.h"

using namespace std;
using Corrade::TestSuite::Compare::Container;

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CapsuleTest)

namespace Magnum { namespace Primitives { namespace Test {

CapsuleTest::CapsuleTest() {
    addTests(&CapsuleTest::withoutTextureCoords,
             &CapsuleTest::withTextureCoords);
}

void CapsuleTest::withoutTextureCoords() {
    Capsule capsule(2, 3, 1.0f);

    CORRADE_COMPARE_AS(*capsule.positions(0), (vector<Vector4>{
        Vector4(0.0f, -1.5f, 0.0f),

        Vector4(0.0f, -1.20711f, 0.707107f),
        Vector4(0.612372f, -1.20711f, -0.353553f),
        Vector4(-0.612373f, -1.20711f, -0.353553f),

        Vector4(0.0f, -0.5f, 1.0f),
        Vector4(0.866025f, -0.5f, -0.5f),
        Vector4(-0.866025f, -0.5f, -0.5f),

        Vector4(0.0f, 0.5f, 1.0f),
        Vector4(0.866025f, 0.5f, -0.5f),
        Vector4(-0.866025f, 0.5f, -0.5f),

        Vector4(0.0f, 1.20711f, 0.707107f),
        Vector4(0.612372f, 1.20711f, -0.353553f),
        Vector4(-0.612372f, 1.20711f, -0.353553f),

        Vector4(0.0f, 1.5f, 0.0f)
    }), Container);

    CORRADE_COMPARE_AS(*capsule.normals(0), (vector<Vector3>{
        Vector3(0.0f, -1.0f, 0.0f),

        Vector3(0.0f, -0.707107f, 0.707107f),
        Vector3(0.612372f, -0.707107f, -0.353553f),
        Vector3(-0.612373f, -0.707107f, -0.353553f),

        Vector3(0.0f, 0.0f, 1.0f),
        Vector3(0.866025f, 0.0f, -0.5f),
        Vector3(-0.866025f, 0.0f, -0.5f),

        Vector3(0.0f, 0.0f, 1.0f),
        Vector3(0.866025f, 0.0f, -0.5f),
        Vector3(-0.866025f, 0.0f, -0.5f),

        Vector3(0.0f, 0.707107f, 0.707107f),
        Vector3(0.612372f, 0.707107f, -0.353553f),
        Vector3(-0.612372f, 0.707107f, -0.353553f),

        Vector3(0.0f, 1.0f, 0.0f)
    }), Container);

    CORRADE_COMPARE_AS(*capsule.indices(), (vector<unsigned int>{
        0, 2, 1, 0, 3, 2, 0, 1, 3,
        1, 2, 5, 1, 5, 4, 2, 3, 6, 2, 6, 5, 3, 1, 4, 3, 4, 6,
        4, 5, 8, 4, 8, 7, 5, 6, 9, 5, 9, 8, 6, 4, 7, 6, 7, 9,
        7, 8, 11, 7, 11, 10, 8, 9, 12, 8, 12, 11, 9, 7, 10, 9, 10, 12,
        10, 11, 13, 11, 12, 13, 12, 10, 13
    }), Container);
}

void CapsuleTest::withTextureCoords() {
    Capsule capsule(2, 3, 1.0f, Capsule::TextureCoords::Generate);

    CORRADE_COMPARE_AS(*capsule.positions(0), (vector<Vector4>{
        Vector4(0.0f, -1.5f, 0.0f),

        Vector4(0.0f, -1.20711f, 0.707107f),
        Vector4(0.612372f, -1.20711f, -0.353553f),
        Vector4(-0.612373f, -1.20711f, -0.353553f),
        Vector4(0.0f, -1.20711f, 0.707107f),

        Vector4(0.0f, -0.5f, 1.0f),
        Vector4(0.866025f, -0.5f, -0.5f),
        Vector4(-0.866025f, -0.5f, -0.5f),
        Vector4(0.0f, -0.5f, 1.0f),

        Vector4(0.0f, 0.5f, 1.0f),
        Vector4(0.866025f, 0.5f, -0.5f),
        Vector4(-0.866025f, 0.5f, -0.5f),
        Vector4(0.0f, 0.5f, 1.0f),

        Vector4(0.0f, 1.20711f, 0.707107f),
        Vector4(0.612372f, 1.20711f, -0.353553f),
        Vector4(-0.612372f, 1.20711f, -0.353553f),
        Vector4(0.0f, 1.20711f, 0.707107f),

        Vector4(0.0f, 1.5f, 0.0f)
    }), Container);

    CORRADE_COMPARE_AS(*capsule.textureCoords2D(0), (vector<Vector2>{
        Vector2(0.5f, 0.0f),

        Vector2(0.0f, 0.166667f),
        Vector2(0.333333f, 0.166667f),
        Vector2(0.666667f, 0.166667f),
        Vector2(1.0f, 0.166667f),

        Vector2(0.0f, 0.333333f),
        Vector2(0.333333f, 0.333333f),
        Vector2(0.666667f, 0.333333f),
        Vector2(1.0f, 0.333333f),

        Vector2(0.0f, 0.666667f),
        Vector2(0.333333f, 0.666667f),
        Vector2(0.666667f, 0.666667f),
        Vector2(1.0f, 0.666667f),

        Vector2(0.0f, 0.833333f),
        Vector2(0.333333f, 0.833333f),
        Vector2(0.666667f, 0.833333f),
        Vector2(1.0f, 0.833333f),

        Vector2(0.5f, 1.0f)
    }), Container);

    CORRADE_COMPARE_AS(*capsule.indices(), (vector<unsigned int>{
        0, 2, 1, 0, 3, 2, 0, 4, 3,
        1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 4, 8, 3, 8, 7,
        5, 6, 10, 5, 10, 9, 6, 7, 11, 6, 11, 10, 7, 8, 12, 7, 12, 11,
        9, 10, 14, 9, 14, 13, 10, 11, 15, 10, 15, 14, 11, 12, 16, 11, 16, 15,
        13, 14, 17, 14, 15, 17, 15, 16, 17
    }), Container);
}

}}}
