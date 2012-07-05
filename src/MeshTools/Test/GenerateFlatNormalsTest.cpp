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

#include "GenerateFlatNormalsTest.h"

#include <sstream>
#include <QtTest/QTest>
#include "MeshTools/GenerateFlatNormals.h"

QTEST_APPLESS_MAIN(Magnum::MeshTools::Test::GenerateFlatNormalsTest)

using namespace std;

namespace Magnum { namespace MeshTools { namespace Test {

void GenerateFlatNormalsTest::wrongIndexCount() {
    stringstream ss;
    Error::setOutput(&ss);
    vector<unsigned int> indices;
    vector<Vector3> normals;
    tie(indices, normals) = MeshTools::generateFlatNormals({
        0, 1
    }, {});

    QVERIFY(indices.size() == 0);
    QVERIFY(normals.size() == 0);
    QVERIFY(ss.str() == "MeshTools::generateFlatNormals(): index count is not divisible by 3!\n");
}

void GenerateFlatNormalsTest::generate() {
    /* Two vertices connected by one edge, each winded in another direction */
    vector<unsigned int> indices;
    vector<Vector3> normals;
    vector<Vector4> vertices{
        {-1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f}
    };
    tie(indices, normals) = MeshTools::generateFlatNormals({
        0, 1, 2,
        1, 2, 3
    }, vertices);


    QVERIFY((indices == vector<unsigned int>{
        0, 0, 0,
        1, 1, 1
    }));
    QVERIFY((normals == vector<Vector3>{
        Vector3::zAxis(),
        -Vector3::zAxis()
    }));
}

}}}
