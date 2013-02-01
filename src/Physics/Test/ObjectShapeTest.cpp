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

#include <TestSuite/Tester.h>

#include "Physics/ObjectShapeGroup.h"
#include "Physics/ObjectShape.h"
#include "Physics/Point.h"
#include "Physics/Sphere.h"
#include "SceneGraph/MatrixTransformation3D.h"
#include "SceneGraph/Scene.h"

namespace Magnum { namespace Physics { namespace Test {

class ObjectShapeTest: public Corrade::TestSuite::Tester {
    public:
        ObjectShapeTest();

        void clean();
        void firstCollision();
};

typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D<>> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D<>> Object3D;

ObjectShapeTest::ObjectShapeTest() {
    addTests(&ObjectShapeTest::clean,
             &ObjectShapeTest::firstCollision);
}

void ObjectShapeTest::clean() {
    Scene3D scene;
    ObjectShapeGroup3D group;

    Object3D a(&scene);
    ObjectShape3D* shape = new ObjectShape3D(&a, &group);
    shape->setShape(new Physics::Point3D({1.0f, -2.0f, 3.0f}));
    a.scale(Vector3(-2.0f));

    Object3D b(&scene);
    new ObjectShape3D(&b, &group);

    /* Everything is dirty at the beginning */
    CORRADE_VERIFY(group.isDirty());
    CORRADE_VERIFY(a.isDirty());
    CORRADE_VERIFY(b.isDirty());

    /* Cleaning object will not clean anything other */
    a.setClean();
    CORRADE_VERIFY(group.isDirty());
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(b.isDirty());

    /* Verify that the feature was actually cleaned */
    CORRADE_COMPARE(static_cast<const Physics::Point3D*>(shape->shape())->transformedPosition(),
        Vector3(-2.0f, 4.0f, -6.0f));

    /* Setting group clean will clean whole group */
    a.setDirty();
    group.setClean();
    CORRADE_VERIFY(!group.isDirty());
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(!b.isDirty());

    /* Setting object dirty will set also the group, but not other objects */
    b.setDirty();
    CORRADE_VERIFY(group.isDirty());
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(b.isDirty());
}

void ObjectShapeTest::firstCollision() {
    Scene3D scene;
    ObjectShapeGroup3D group;

    Object3D a(&scene);
    ObjectShape3D* aShape = new ObjectShape3D(&a, &group);
    aShape->setShape(Physics::Sphere3D({1.0f, -2.0f, 3.0f}, 1.5f));

    Object3D b(&scene);
    ObjectShape3D* bShape = new ObjectShape3D(&b, &group);
    bShape->setShape(Physics::Point3D({3.0f, -2.0f, 3.0f}));

    Object3D c(&scene);
    ObjectShape3D* cShape = new ObjectShape3D(&c, &group);

    /* No-op if the object has no shape */
    CORRADE_VERIFY(group.isDirty());
    CORRADE_VERIFY(!group.firstCollision(cShape));
    CORRADE_VERIFY(group.isDirty());

    /* No collisions initially */
    CORRADE_VERIFY(!group.firstCollision(aShape));
    CORRADE_VERIFY(!group.firstCollision(bShape));
    CORRADE_VERIFY(!group.isDirty());

    /* Move point into sphere */
    b.translate(Vector3::xAxis(-1.0f));

    /* Collision */
    CORRADE_VERIFY(group.isDirty());
    CORRADE_VERIFY(group.firstCollision(aShape) == bShape);
    CORRADE_VERIFY(group.firstCollision(bShape) == aShape);
    CORRADE_VERIFY(!group.isDirty());
}

}}}

CORRADE_TEST_MAIN(Magnum::Physics::Test::ObjectShapeTest)
