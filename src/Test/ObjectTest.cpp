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

#include "ObjectTest.h"
#include "Scene.h"

#include <QtTest/QTest>

QTEST_APPLESS_MAIN(Magnum::Test::ObjectTest)

namespace Magnum { namespace Test {

void ObjectTest::parenting() {
    Object root;

    Object* childOne = new Object(&root);
    Object* childTwo = new Object(&root);

    QVERIFY(childOne->parent() == &root);
    QVERIFY(root.children().size() == 2);

    /* A object cannot be parent of itself */
    childOne->setParent(childOne);
    QVERIFY(childOne->parent() == &root);

    /* In fact, cyclic dependencies are not allowed at all */
    root.setParent(childTwo);
    QVERIFY(root.parent() == 0);

    /* Reparent to another */
    childTwo->setParent(childOne);
    QVERIFY(root.children().size() == 1 && *root.children().begin() == childOne);
    QVERIFY(childOne->children().size() == 1 && *childOne->children().begin() == childTwo);

    /* Delete child */
    delete childTwo;
    QVERIFY(childOne->children().size() == 0);
}

void ObjectTest::scene() {
    Scene scene;

    Object* childOne = new Object(&scene);
    Object* childTwo = new Object(childOne);

    Object* orphan = new Object;
    Object* childOfOrphan = new Object(orphan);

    QVERIFY(childTwo->scene() == &scene);
    QVERIFY(childOfOrphan->scene() == 0);
}

void ObjectTest::dirty() {
    Scene scene;

    Object* childOne = new Object(&scene);
    Object* childTwo = new Object(childOne);
    Object* childThree = new Object(childTwo);

    /* Object is dirty at the beginning */
    QVERIFY(childOne->isDirty());

    /* Clean the object and all its parents */
    childThree->setClean();
    QVERIFY(!childThree->isDirty());
    QVERIFY(!childTwo->isDirty());
    QVERIFY(!childOne->isDirty());
    QVERIFY(!scene.isDirty());

    /* Mark object and all its children as dirty */
    childTwo->setDirty();
    QVERIFY(childTwo->isDirty());
    QVERIFY(childThree->isDirty());

    /* Set camera, makes everything dirty except path from camera to scene */
    Camera* camera = new Camera(&scene);
    scene.setCamera(camera);
    QVERIFY(childOne->isDirty());
    QVERIFY(!camera->isDirty());
    QVERIFY(!scene.isDirty());

    /* Clean up and try to move the camera -> makes all dirty (except path
       from camera to scene) */
    childThree->setClean();
    QVERIFY(!scene.isDirty());
    camera->translate(0, 0, 1);
    QVERIFY(childOne->isDirty());
}

}}
