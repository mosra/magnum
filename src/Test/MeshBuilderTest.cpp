/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

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
    MeshBuilder<int> builder;

    int vertexData[] = { 1, 2, 3, 4 };
    GLubyte indices[] = { 0, 1, 2, 1, 2, 3 };
    builder.setData(vertexData, indices, 4, 6);

    set<int*> vertices = builder.vertices();
    QVERIFY(vertices.size() == 4);

    int i = 1;
    for(set<int*>::const_iterator it = vertices.begin(); it != vertices.end(); ++it)
        QVERIFY(**it == i++);

    set<MeshBuilder<int>::Face*> faces = builder.faces();
    QVERIFY(faces.size() == 2);

    set<MeshBuilder<int>::Face*>::const_iterator it = faces.begin();
    QVERIFY(*(*it)->vertices[0] == 1);
    QVERIFY(*(*it)->vertices[1] == 2);
    QVERIFY(*(*it)->vertices[2] == 3);

    it++;
    QVERIFY(*(*it)->vertices[0] == 2);
    QVERIFY(*(*it)->vertices[1] == 3);
    QVERIFY(*(*it)->vertices[2] == 4);
}

void MeshBuilderTest::addFace() {
    MeshBuilder<int> builder;

    int* v1 = new int(1);
    int* v2 = new int(2);
    int* v3 = new int(3);
    int* v4 = new int(4);

    MeshBuilder<int>::Face* f1 = new MeshBuilder<int>::Face(v1, v2, v3);
    builder.addFace(f1);
    MeshBuilder<int>::Face* f2 = new MeshBuilder<int>::Face(v2, v3, v4);
    builder.addFace(f2);

    set<int*> vertices;
    vertices.insert(v1);
    vertices.insert(v2);
    vertices.insert(v3);
    vertices.insert(v4);

    set<MeshBuilder<int>::Face*> faces;
    faces.insert(f1);
    faces.insert(f2);

    QVERIFY(builder.vertices() == vertices);
    QVERIFY(builder.faces() == faces);
}

void MeshBuilderTest::removeFace() {
    MeshBuilder<int> builder;

    int* v1 = new int(1);
    int* v2 = new int(2);
    int* v3 = new int(3);
    int* v4 = new int(4);

    MeshBuilder<int>::Face* f1 = new MeshBuilder<int>::Face(v1, v2, v3);
    builder.addFace(f1);
    MeshBuilder<int>::Face* f2 = new MeshBuilder<int>::Face(v2, v3, v4);
    builder.addFace(f2);
    MeshBuilder<int>::Face* f3 = new MeshBuilder<int>::Face(v2, v3, v1);
    builder.addFace(f3);

    /* Remove second face */
    builder.removeFace(f2);

    set<int*> vertices;
    vertices.insert(v1);
    vertices.insert(v2);
    vertices.insert(v3);

    set<MeshBuilder<int>::Face*> faces;
    faces.insert(f1);
    faces.insert(f3);

    QVERIFY(builder.vertices() == vertices);
    QVERIFY(builder.faces() == faces);

    /* Remove third face (vertices shouldn't change) */
    builder.removeFace(f3);
    faces.erase(f3);
    QVERIFY(builder.vertices() == vertices);
    QVERIFY(builder.faces() == faces);
}

void MeshBuilderTest::cleanMesh() {
    MeshBuilder<int> builder;

    int* v1 = new int(1);
    int* v2 = new int(2);
    int* v3 = new int(1);
    int* v4 = new int(4);

    MeshBuilder<int>::Face* f1 = new MeshBuilder<int>::Face(v1, v2, v3);
    builder.addFace(f1);
    MeshBuilder<int>::Face* f2 = new MeshBuilder<int>::Face(v2, v3, v4);
    builder.addFace(f2);

    builder.cleanMesh();
    int* unique = (f1->vertices[2] == v1) ? v1 : v3;

    set<int*> vertices;
    vertices.insert(unique);
    vertices.insert(v2);
    vertices.insert(v4);

    set<MeshBuilder<int>::Face*> faces;
    faces.insert(f1);
    faces.insert(f2);

    /* Verify cleanup */
    QVERIFY(f1->vertices[2] == unique);
    QVERIFY(f2->vertices[1] == unique);
    QVERIFY(builder.vertices() == vertices);
    QVERIFY(builder.faces() == faces);
}

void MeshBuilderTest::subdivide() {
    MeshBuilder<int> builder;

    int* v1 = new int(0);
    int* v2 = new int(2);
    int* v3 = new int(6);
    int* v4 = new int(8);

    MeshBuilder<int>::Face* f1 = new MeshBuilder<int>::Face(v1, v2, v3);
    builder.addFace(f1);
    MeshBuilder<int>::Face* f2 = new MeshBuilder<int>::Face(v2, v3, v4);
    builder.addFace(f2);

    builder.subdivide(interpolator);

    QVERIFY(builder.vertices().size() == 10);
    QVERIFY(builder.faces().size() == 8);

    /* This also effectively checks the data, as the vertices should be exactly
       0, 1, 2, .. 8 */
    builder.cleanMesh();
    QVERIFY(builder.vertices().size() == 9);
    QVERIFY(builder.faces().size() == 8);
}

}}
