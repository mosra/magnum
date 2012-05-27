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

#include "FlipNormalsTest.h"

#include <sstream>
#include <QtTest/QTest>
#include "MeshTools/FlipNormals.h"

QTEST_APPLESS_MAIN(Magnum::MeshTools::Test::FlipNormalsTest)

using namespace std;

namespace Magnum { namespace MeshTools { namespace Test {

void FlipNormalsTest::wrongIndexCount() {
    stringstream ss;
    Error::setOutput(&ss);

    vector<unsigned int> indices{0, 1};
    MeshTools::flipFaceWinding(indices);

    QVERIFY(ss.str() == "MeshTools::flipNormals(): index count is not divisible by 3!\n");
}

void FlipNormalsTest::flipFaceWinding() {
    vector<unsigned int> indices{0, 1, 2,
                                 3, 4, 5};
    MeshTools::flipFaceWinding(indices);

    QVERIFY((indices == vector<unsigned int>{0, 2, 1,
                                             3, 5, 4}));
}

void FlipNormalsTest::flipNormals() {
    vector<Vector3> normals{Vector3::xAxis(),
                            Vector3::yAxis(),
                            Vector3::zAxis()};
    MeshTools::flipNormals(normals);

    QVERIFY((normals == vector<Vector3>{-Vector3::xAxis(),
                                        -Vector3::yAxis(),
                                        -Vector3::zAxis()}));
}

}}}
