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
#include "SceneGraph/Camera.h"
#include "SceneGraph/Scene.h"

#include <sstream>

using namespace std;

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::ObjectTest)

namespace Magnum { namespace SceneGraph { namespace Test {

ObjectTest::ObjectTest() {
    addTests(&ObjectTest::parenting,
             &ObjectTest::transformation,
             &ObjectTest::absoluteTransformationWrongCamera,
             &ObjectTest::absoluteTransformation,
             &ObjectTest::scene,
             &ObjectTest::dirty);
}

void ObjectTest::parenting() {
    Object3D root;

    Object3D* childOne = new Object3D(&root);
    Object3D* childTwo = new Object3D(&root);

    CORRADE_VERIFY(childOne->parent() == &root);
    CORRADE_VERIFY(childTwo->parent() == &root);
    CORRADE_VERIFY(root.firstChild() == childOne);
    CORRADE_VERIFY(root.lastChild() == childTwo);
    CORRADE_VERIFY(root.firstChild()->nextSibling() == root.lastChild());

    /* A object cannot be parent of itself */
    childOne->setParent(childOne);
    CORRADE_VERIFY(childOne->parent() == &root);

    /* In fact, cyclic dependencies are not allowed at all */
    root.setParent(childTwo);
    CORRADE_VERIFY(root.parent() == nullptr);

    /* Reparent to another */
    childTwo->setParent(childOne);
    CORRADE_VERIFY(root.firstChild() == childOne && root.firstChild()->nextSibling() == nullptr);
    CORRADE_VERIFY(childOne->firstChild() == childTwo && childOne->firstChild()->nextSibling() == nullptr);

    /* Delete child */
    delete childTwo;
    CORRADE_VERIFY(!childOne->hasChildren());
}

void ObjectTest::transformation() {
    Object3D o;
    Object3D o2;

    o.setTransformation(Matrix4::translation(Vector3::xAxis(1.0f)));
    o2.translate(Vector3::xAxis(1.0f));
    o.multiplyTransformation(Matrix4::rotation(deg(35.0f), Vector3::zAxis()));
    o2.rotate(deg(35.0f), Vector3::zAxis());

    CORRADE_COMPARE(o.transformation(), Matrix4::rotation(deg(35.0f), Vector3::zAxis())*
        Matrix4::translation(Vector3::xAxis(1.0f)));
    CORRADE_COMPARE(o2.transformation(), o.transformation());

    o.multiplyTransformation(Matrix4::scaling(Vector3(2.0f)), Object3D::Transformation::Local);
    o2.scale(Vector3(2.0f), Object3D::Transformation::Local);
    CORRADE_COMPARE(o.transformation(), Matrix4::rotation(deg(35.0f), Vector3::zAxis())*
        Matrix4::translation(Vector3::xAxis(1.0f))*
        Matrix4::scaling(Vector3(2.0f)));
    CORRADE_COMPARE(o2.transformation(), o.transformation());
}

void ObjectTest::absoluteTransformationWrongCamera() {
    stringstream ss;
    Error::setOutput(&ss);

    Scene3D s;
    Object3D o(&s);
    o.translate(Vector3::yAxis());
    Camera3D c;
    CORRADE_COMPARE(o.absoluteTransformation(&c), Matrix4::translation(Vector3::yAxis()));
    CORRADE_COMPARE(ss.str(), "Object::absoluteTransformation(): the camera is not part of the same scene as object!\n");

    ss.str("");
    Object3D o2;
    o2.translate(Vector3::xAxis());
    CORRADE_COMPARE(o2.absoluteTransformation(&c), Matrix4::translation(Vector3::xAxis()));
    CORRADE_COMPARE(ss.str(), "Object::absoluteTransformation(): the object is not part of camera scene!\n");
}

void ObjectTest::absoluteTransformation() {
    Scene3D s;
    Camera3D c(&s);
    c.translate(Vector3::zAxis(2.0f));
    CORRADE_COMPARE(s.absoluteTransformation(), Matrix4());
    CORRADE_COMPARE(c.absoluteTransformation(&c), Matrix4());

    Object3D o(&s);
    o.scale(Vector3(2.0f));
    Object3D o2(&o);
    o.rotate(deg(90.0f), Vector3::yAxis());
    CORRADE_COMPARE(o2.absoluteTransformation(),
        Matrix4::scaling(Vector3(2.0f))*Matrix4::rotation(deg(90.0f), Vector3::yAxis()));
    CORRADE_COMPARE(o2.absoluteTransformation(&c),
        (Matrix4::translation(Vector3::zAxis(2.0f)).inverted())*Matrix4::scaling(Vector3(2.0f))*Matrix4::rotation(deg(90.0f), Vector3::yAxis()));

    Object3D o3;
    o3.translate({1.0f, 2.0f, 3.0f});
    CORRADE_COMPARE(o3.absoluteTransformation(), Matrix4::translation({1.0f, 2.0f, 3.0f}));
}

void ObjectTest::scene() {
    Scene3D scene;

    Object3D* childOne = new Object3D(&scene);
    Object3D* childTwo = new Object3D(childOne);

    Object3D orphan;
    Object3D* childOfOrphan = new Object3D(&orphan);

    CORRADE_VERIFY(childTwo->scene() == &scene);
    CORRADE_VERIFY(childOfOrphan->scene() == nullptr);
}

void ObjectTest::dirty() {
    Scene3D scene;

    CleaningObject* childOne = new CleaningObject(&scene);
    childOne->scale(Vector3(2.0f));
    CleaningObject* childTwo = new CleaningObject(childOne);
    childTwo->translate(Vector3::xAxis(1.0f));
    CleaningObject* childThree = new CleaningObject(childTwo);
    childThree->rotate(deg(90.0f), Vector3::yAxis());

    /* Object is dirty at the beginning */
    CORRADE_VERIFY(scene.isDirty());
    CORRADE_VERIFY(childOne->isDirty());

    /* Clean the object and all its dirty parents (but not children) */
    childOne->setClean();
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, childOne->absoluteTransformation());
    CORRADE_VERIFY(!scene.isDirty());
    CORRADE_VERIFY(!childOne->isDirty());
    CORRADE_VERIFY(childTwo->isDirty());
    CORRADE_VERIFY(childThree->isDirty());

    /* If the object itself is already clean, it shouldn't clean it again */
    childOne->cleanedAbsoluteTransformation = Matrix4(Matrix4::Zero);
    childOne->setClean();
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, Matrix4(Matrix4::Zero));

    /* If any object in the hierarchy is already clean, it shouldn't clean it again */
    childTwo->setClean();
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, Matrix4(Matrix4::Zero));
    CORRADE_COMPARE(childTwo->cleanedAbsoluteTransformation, childTwo->absoluteTransformation());
    CORRADE_VERIFY(!childOne->isDirty());
    CORRADE_VERIFY(!childTwo->isDirty());
    CORRADE_VERIFY(childThree->isDirty());

    /* Mark object and all its children as dirty (but not parents) */
    childTwo->setDirty();
    CORRADE_VERIFY(!scene.isDirty());
    CORRADE_VERIFY(!childOne->isDirty());
    CORRADE_VERIFY(childTwo->isDirty());
    CORRADE_VERIFY(childThree->isDirty());

    /* Reparent object => make it and its children dirty (but not parents) */
    childThree->setClean();
    CORRADE_COMPARE(childThree->cleanedAbsoluteTransformation, childThree->absoluteTransformation());
    childTwo->setParent(nullptr);
    CORRADE_VERIFY(childTwo->isDirty());
    CORRADE_VERIFY(!childOne->isDirty());
    childTwo->setParent(&scene);
    CORRADE_VERIFY(!scene.isDirty());
    CORRADE_VERIFY(childTwo->isDirty());
    CORRADE_VERIFY(childThree->isDirty());

    /* Set object transformation => make it and its children dirty (but not parents) */
    childThree->setClean();
    childTwo->setTransformation(Matrix4::translation(Vector3::xAxis(1.0f)));
    CORRADE_VERIFY(!scene.isDirty());
    CORRADE_VERIFY(childTwo->isDirty());
    CORRADE_VERIFY(childThree->isDirty());
}

}}}
