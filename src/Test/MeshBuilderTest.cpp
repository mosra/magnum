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

#include "MeshBuilderTest.h"

#include <QtTest/QTest>
#include <QtCore/QDebug>

#include "MeshBuilder.h"

QTEST_APPLESS_MAIN(Magnum::Test::MeshBuilderTest)

using namespace std;

namespace Magnum { namespace Test {

void MeshBuilderTest::setData() {
    MeshBuilder<Vector1> builder;

    int vertexData[] = { 1, 2, 3, 4 };
    GLubyte indices[] = { 0, 1, 2, 1, 2, 3 };
    builder.setData(reinterpret_cast<Vector1*>(vertexData), indices, 4, 6);

    vector<MeshBuilder<Vector1>::Face> faces = builder.faces();
    QVERIFY(faces.size() == 2);

    vector<MeshBuilder<Vector1>::Face>::const_iterator it = faces.begin();
    QVERIFY(builder.vertices()[it->vertices[0]] == 1);
    QVERIFY(builder.vertices()[it->vertices[1]] == 2);
    QVERIFY(builder.vertices()[it->vertices[2]] == 3);

    it++;
    QVERIFY(builder.vertices()[it->vertices[0]] == 2);
    QVERIFY(builder.vertices()[it->vertices[1]] == 3);
    QVERIFY(builder.vertices()[it->vertices[2]] == 4);

    vector<Vector1> vertices = builder.vertices();
    QVERIFY(vertices.size() == 4);

    int i = 0;
    for(auto it = vertices.begin(); it != vertices.end(); ++it)
        QVERIFY(*it == ++i);
}

void MeshBuilderTest::addFace() {
    Vector1 v1(1);
    Vector1 v2(2);
    Vector1 v3(3);
    Vector1 v4(4);
    MeshBuilder<Vector1>::Face f1(0, 1, 2);
    MeshBuilder<Vector1>::Face f2(1, 2, 3);

    MeshBuilder<Vector1> builder;
    builder.addVertex(v1);
    builder.addVertex(v2);
    builder.addVertex(v3);
    builder.addVertex(v4);
    builder.addFace(f1);
    builder.addFace(f2);

    vector<Vector1> vertices;
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    QVERIFY(builder.vertices() == vertices);

    vector<MeshBuilder<Vector1>::Face> faces;
    faces.push_back(f1);
    faces.push_back(f2);
    QVERIFY(builder.faces() == faces);
}

void MeshBuilderTest::cleanMesh() {
    Vector1 v1(1);
    Vector1 v2(2);
    Vector1 v3(1);
    Vector1 v4(4);
    MeshBuilder<Vector1>::Face f1(0, 1, 2);
    MeshBuilder<Vector1>::Face f2(1, 2, 3);

    MeshBuilder<Vector1> builder;
    builder.addVertex(v1);
    builder.addVertex(v2);
    builder.addVertex(v3);
    builder.addVertex(v4);
    builder.addFace(f1);
    builder.addFace(f2);

    builder.cleanMesh(1);

    /* Verify cleanup */
    vector<Vector1> vertices;
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v4);
    QVERIFY(builder.vertices() == vertices);

    vector<MeshBuilder<Vector1>::Face> faces;
    faces.push_back(f1);
    faces.push_back(f2);
    QVERIFY(builder.faces()[0].vertices[0] == 0);
    QVERIFY(builder.faces()[0].vertices[1] == 1);
    QVERIFY(builder.faces()[0].vertices[2] == 0);
    QVERIFY(builder.faces()[1].vertices[0] == 1);
    QVERIFY(builder.faces()[1].vertices[1] == 0);
    QVERIFY(builder.faces()[1].vertices[2] == 2);
    QVERIFY(f1.vertices[2] == f2.vertices[1]);
}

void MeshBuilderTest::subdivide() {
    Vector1 v1(0);
    Vector1 v2(2);
    Vector1 v3(6);
    Vector1 v4(8);
    MeshBuilder<Vector1>::Face f1(0, 1, 2);
    MeshBuilder<Vector1>::Face f2(1, 2, 3);

    MeshBuilder<Vector1> builder;
    builder.addVertex(v1);
    builder.addVertex(v2);
    builder.addVertex(v3);
    builder.addVertex(v4);
    builder.addFace(f1);
    builder.addFace(f2);

    builder.subdivide(interpolator);
    QVERIFY(builder.faces().size() == 8);

    QVERIFY(builder.vertices().size() == 10);
    QVERIFY(builder.vertices()[0] == Vector1(0));
    QVERIFY(builder.vertices()[1] == Vector1(2));
    QVERIFY(builder.vertices()[2] == Vector1(6));
    QVERIFY(builder.vertices()[3] == Vector1(8));
    QVERIFY(builder.vertices()[4] == Vector1(1));
    QVERIFY(builder.vertices()[5] == Vector1(4));
    QVERIFY(builder.vertices()[6] == Vector1(3));
    QVERIFY(builder.vertices()[7] == Vector1(4));
    QVERIFY(builder.vertices()[8] == Vector1(7));
    QVERIFY(builder.vertices()[9] == Vector1(5));

    QVERIFY(builder.faces()[0].vertices[0] == 4);
    QVERIFY(builder.faces()[0].vertices[1] == 5);
    QVERIFY(builder.faces()[0].vertices[2] == 6);
    QVERIFY(builder.faces()[1].vertices[0] == 7);
    QVERIFY(builder.faces()[1].vertices[1] == 8);
    QVERIFY(builder.faces()[1].vertices[2] == 9);
    QVERIFY(builder.faces()[2].vertices[0] == 0);
    QVERIFY(builder.faces()[2].vertices[1] == 4);
    QVERIFY(builder.faces()[2].vertices[2] == 6);
    QVERIFY(builder.faces()[3].vertices[0] == 4);
    QVERIFY(builder.faces()[3].vertices[1] == 1);
    QVERIFY(builder.faces()[3].vertices[2] == 5);
    QVERIFY(builder.faces()[4].vertices[0] == 6);
    QVERIFY(builder.faces()[4].vertices[1] == 5);
    QVERIFY(builder.faces()[4].vertices[2] == 2);
    QVERIFY(builder.faces()[5].vertices[0] == 1);
    QVERIFY(builder.faces()[5].vertices[1] == 7);
    QVERIFY(builder.faces()[5].vertices[2] == 9);
    QVERIFY(builder.faces()[6].vertices[0] == 7);
    QVERIFY(builder.faces()[6].vertices[1] == 2);
    QVERIFY(builder.faces()[6].vertices[2] == 8);
    QVERIFY(builder.faces()[7].vertices[0] == 9);
    QVERIFY(builder.faces()[7].vertices[1] == 8);
    QVERIFY(builder.faces()[7].vertices[2] == 3);

    builder.cleanMesh(1);

    /* Vertices 0, 1, 2, 3, 4, 5, 6, 7, 8 */
    QVERIFY(builder.vertices().size() == 9);
}

}}
