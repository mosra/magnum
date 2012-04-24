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

    /* Reparent object => make it dirty */
    childThree->setClean();
    childOne->setParent(nullptr);
    childOne->setParent(&scene);
    QVERIFY(childOne->isDirty());
    QVERIFY(childTwo->isDirty());
    QVERIFY(childThree->isDirty());

    /* Set object transformation => make it dirty */
    childThree->setClean();
    childTwo->setTransformation({});
    QVERIFY(childTwo->isDirty());
    QVERIFY(childThree->isDirty());
}

}}
