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

#include "CleanTest.h"

#include <QtTest/QTest>

#include "MeshTools/Clean.h"

QTEST_APPLESS_MAIN(Magnum::MeshTools::Test::CleanTest)

using namespace std;

namespace Magnum { namespace MeshTools { namespace Test {

void CleanTest::cleanMesh() {
    MeshBuilder<Vector1> builder;
    builder.addVertex(1);
    builder.addVertex(2);
    builder.addVertex(1);
    builder.addVertex(4);
    builder.addFace(0, 1, 2);
    builder.addFace(1, 2, 3);

    MeshTools::clean<Vector1, 1>(builder, 1);

    /* Verify cleanup */
    QVERIFY((builder.vertices() == vector<Vector1>{1, 2, 4}));
    QVERIFY((builder.indices() == vector<unsigned int>{0, 1, 0, 1, 0, 2}));
}

}}}
