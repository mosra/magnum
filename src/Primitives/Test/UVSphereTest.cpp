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

#include "UVSphereTest.h"

#include <QtTest/QTest>

#include "Primitives/UVSphere.h"

using namespace std;

QTEST_APPLESS_MAIN(Magnum::Primitives::Test::UVSphereTest)

namespace Magnum { namespace Primitives { namespace Test {

void UVSphereTest::withoutTextureCoords() {
    UVSphere sphere(3, 3);

    QVERIFY((*sphere.vertices(0) == vector<Vector4>{
        Vector4(0.0f, -1.0f, 0.0f),

        Vector4(0.0f, -0.5f, 0.866025f),
        Vector4(0.75f, -0.5f, -0.433013f),
        Vector4(-0.75f, -0.5f, -0.433013f),

        Vector4(0, 0.5f, 0.866025f),
        Vector4(0.75f, 0.5f, -0.433013f),
        Vector4(-0.75f, 0.5f, -0.433013f),

        Vector4(0.0f, 1.0f, 0.0f)
    }));

    QVERIFY((*sphere.normals(0) == vector<Vector3>{
        Vector3(0.0f, -1.0f, 0.0f),

        Vector3(0.0f, -0.5f, 0.866025f),
        Vector3(0.75f, -0.5f, -0.433013f),
        Vector3(-0.75f, -0.5f, -0.433013f),

        Vector3(0, 0.5f, 0.866025f),
        Vector3(0.75f, 0.5f, -0.433013f),
        Vector3(-0.75f, 0.5f, -0.433013f),

        Vector3(0.0f, 1.0f, 0.0f)
    }));

    QVERIFY((*sphere.indices() == vector<unsigned int>{
        0, 2, 1, 0, 3, 2, 0, 1, 3,
        1, 2, 5, 1, 5, 4, 2, 3, 6, 2, 6, 5, 3, 1, 4, 3, 4, 6,
        4, 5, 7, 5, 6, 7, 6, 4, 7
    }));
}

void UVSphereTest::withTextureCoords() {
    UVSphere sphere(3, 3, UVSphere::TextureCoords::Generate);

    QVERIFY((*sphere.vertices(0) == vector<Vector4>{
        Vector4(0.0f, -1.0f, 0.0f),

        Vector4(0.0f, -0.5f, 0.866025f),
        Vector4(0.75f, -0.5f, -0.433013f),
        Vector4(-0.75f, -0.5f, -0.433013f),
        Vector4(0.0f, -0.5f, 0.866025f),

        Vector4(0.0f, 0.5f, 0.866025f),
        Vector4(0.75f, 0.5f, -0.433013f),
        Vector4(-0.75f, 0.5f, -0.433013f),
        Vector4(0.0f, 0.5f, 0.866025f),

        Vector4(0.0f, 1.0f, 0.0f)
    }));

    QVERIFY((*sphere.textureCoords2D(0) == vector<Vector2>{
        Vector2(0.5f, 0.0f),

        Vector2(0.0f, 0.333333f),
        Vector2(0.333333f, 0.333333f),
        Vector2(0.666667f, 0.333333f),
        Vector2(1.0f, 0.333333f),

        Vector2(0.0f, 0.666667f),
        Vector2(0.333333f, 0.666667f),
        Vector2(0.666667f, 0.666667f),
        Vector2(1.0f, 0.666667f),

        Vector2(0.5f, 1.0f)
    }));

    QVERIFY((*sphere.indices() == vector<unsigned int>{
        0, 2, 1, 0, 3, 2, 0, 4, 3,
        1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 4, 8, 3, 8, 7,
        5, 6, 9, 6, 7, 9, 7, 8, 9
    }));
}

}}}
