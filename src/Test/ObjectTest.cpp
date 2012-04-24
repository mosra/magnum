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

#include "ObjectTest.h"
#include "Scene.h"
#include "Camera.h"

#include <sstream>
#include <QtTest/QTest>

using namespace std;
using namespace Corrade::Utility;

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
    QVERIFY(root.parent() == nullptr);

    /* Reparent to another */
    childTwo->setParent(childOne);
    QVERIFY(root.children().size() == 1 && *root.children().begin() == childOne);
    QVERIFY(childOne->children().size() == 1 && *childOne->children().begin() == childTwo);

    /* Delete child */
    delete childTwo;
    QVERIFY(childOne->children().size() == 0);
}

void ObjectTest::transformation() {
    Object o;
    Object o2;

    o.setTransformation(Matrix4::translation(Vector3::xAxis(1.0f)));
    o2.translate(Vector3::xAxis(1.0f));
    o.multiplyTransformation(Matrix4::rotation(deg(35.0f), Vector3::zAxis()));
    o2.rotate(deg(35.0f), Vector3::zAxis());

    QVERIFY(o.transformation() == Matrix4::rotation(deg(35.0f), Vector3::zAxis())*
        Matrix4::translation(Vector3::xAxis(1.0f)));
    QVERIFY(o2.transformation() == o.transformation());

    o.multiplyTransformation(Matrix4::scaling(Vector3(2.0f)), Object::Transformation::Local);
    o2.scale(Vector3(2.0f), Object::Transformation::Local);
    QVERIFY(o.transformation() == Matrix4::rotation(deg(35.0f), Vector3::zAxis())*
        Matrix4::translation(Vector3::xAxis(1.0f))*
        Matrix4::scaling(Vector3(2.0f)));
    QVERIFY(o2.transformation() == o.transformation());
}

void ObjectTest::absoluteTransformationWrongCamera() {
    stringstream ss;
    Error::setOutput(&ss);

    Scene s;
    Object o(&s);
    o.translate(Vector3::yAxis());
    Camera c;
    QVERIFY(o.absoluteTransformation(&c) == Matrix4::translation(Vector3::yAxis()));
    QVERIFY(ss.str() == "Object::absoluteTransformation(): the camera is not part of the same scene as object!\n");

    ss.str("");
    Object o2;
    o2.translate(Vector3::xAxis());
    QVERIFY(o2.absoluteTransformation(&c) == Matrix4::translation(Vector3::xAxis()));
    QVERIFY(ss.str() == "Object::absoluteTransformation(): the object is not part of camera scene!\n");
}

void ObjectTest::absoluteTransformation() {
    Scene s;
    Camera c(&s);
    c.translate(Vector3::zAxis(2.0f));
    QVERIFY(s.absoluteTransformation() == Matrix4());
    QVERIFY(c.absoluteTransformation(&c) == Matrix4());

    Object o(&s);
    o.scale(Vector3(2.0f));
    Object o2(&o);
    o.rotate(deg(90.0f), Vector3::yAxis());
    QVERIFY((o2.absoluteTransformation() ==
        Matrix4::scaling(Vector3(2.0f))*Matrix4::rotation(deg(90.0f), Vector3::yAxis())));
    QVERIFY((o2.absoluteTransformation(&c) ==
        (Matrix4::translation(Vector3::zAxis(2.0f)).inverted())*Matrix4::scaling(Vector3(2.0f))*Matrix4::rotation(deg(90.0f), Vector3::yAxis())));

    Object o3;
    o3.translate({1.0f, 2.0f, 3.0f});
    QVERIFY(o3.absoluteTransformation() == Matrix4::translation({1.0f, 2.0f, 3.0f}));
}

void ObjectTest::scene() {
    Scene scene;

    Object* childOne = new Object(&scene);
    Object* childTwo = new Object(childOne);

    Object orphan;
    Object* childOfOrphan = new Object(&orphan);

    QVERIFY(childTwo->scene() == &scene);
    QVERIFY(childOfOrphan->scene() == nullptr);
}

void ObjectTest::dirty() {
    Scene scene;

    CleaningObject* childOne = new CleaningObject(&scene);
    childOne->scale(Vector3(2.0f));
    CleaningObject* childTwo = new CleaningObject(childOne);
    childTwo->translate(Vector3::xAxis(1.0f));
    CleaningObject* childThree = new CleaningObject(childTwo);
    childThree->rotate(deg(90.0f), Vector3::yAxis());

    /* Object is dirty at the beginning */
    QVERIFY(scene.isDirty());
    QVERIFY(childOne->isDirty());

    /* Clean the object and all its dirty parents (but not children) */
    childOne->setClean();
    QVERIFY(childOne->cleanedAbsoluteTransformation == childOne->absoluteTransformation());
    QVERIFY(!scene.isDirty());
    QVERIFY(!childOne->isDirty());
    QVERIFY(childTwo->isDirty());
    QVERIFY(childThree->isDirty());

    /* If the object itself is already clean, it shouldn't clean it again */
    childOne->cleanedAbsoluteTransformation = Matrix4(Matrix4::Zero);
    childOne->setClean();
    QVERIFY(childOne->cleanedAbsoluteTransformation == Matrix4(Matrix4::Zero));

    /* If any object in the hierarchy is already clean, it shouldn't clean it again */
    childTwo->setClean();
    QVERIFY(childOne->cleanedAbsoluteTransformation == Matrix4(Matrix4::Zero));
    QVERIFY(childTwo->cleanedAbsoluteTransformation == childTwo->absoluteTransformation());
    QVERIFY(!childOne->isDirty());
    QVERIFY(!childTwo->isDirty());
    QVERIFY(childThree->isDirty());

    /* Mark object and all its children as dirty (but not parents) */
    childTwo->setDirty();
    QVERIFY(!scene.isDirty());
    QVERIFY(!childOne->isDirty());
    QVERIFY(childTwo->isDirty());
    QVERIFY(childThree->isDirty());

    /* Reparent object => make it and its children dirty (but not parents) */
    childThree->setClean();
    QVERIFY(childThree->cleanedAbsoluteTransformation == childThree->absoluteTransformation());
    childTwo->setParent(nullptr);
    QVERIFY(childTwo->isDirty());
    QVERIFY(!childOne->isDirty());
    childTwo->setParent(&scene);
    QVERIFY(!scene.isDirty());
    QVERIFY(childTwo->isDirty());
    QVERIFY(childThree->isDirty());

    /* Set object transformation => make it and its children dirty (but not parents) */
    childThree->setClean();
    childTwo->setTransformation(Matrix4::translation(Vector3::xAxis(1.0f)));
    QVERIFY(!scene.isDirty());
    QVERIFY(childTwo->isDirty());
    QVERIFY(childThree->isDirty());
}

}}
