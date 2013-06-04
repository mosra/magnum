/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <TestSuite/Tester.h>

#include "Shapes/ShapeGroup.h"
#include "Shapes/Shape.h"
#include "Shapes/Point.h"
#include "Shapes/Composition.h"
#include "Shapes/Sphere.h"
#include "SceneGraph/MatrixTransformation2D.h"
#include "SceneGraph/MatrixTransformation3D.h"
#include "SceneGraph/Scene.h"

namespace Magnum { namespace Shapes { namespace Test {

class ShapeTest: public TestSuite::Tester {
    public:
        ShapeTest();

        void clean();
        void firstCollision();
        void shapeGroup();
};

typedef SceneGraph::Scene<SceneGraph::MatrixTransformation2D<>> Scene2D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation2D<>> Object2D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D<>> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D<>> Object3D;

ShapeTest::ShapeTest() {
    addTests({&ShapeTest::clean,
              &ShapeTest::firstCollision,
              &ShapeTest::shapeGroup});
}

void ShapeTest::clean() {
    Scene3D scene;
    ShapeGroup3D shapes;

    Object3D a(&scene);
    auto shape = new Shapes::Shape<Shapes::Point3D>(&a, {{1.0f, -2.0f, 3.0f}}, &shapes);
    a.scale(Vector3(-2.0f));

    Object3D b(&scene);
    new Shapes::Shape<Shapes::Point3D>(&b, &shapes);

    /* Everything is dirty at the beginning */
    CORRADE_VERIFY(shapes.isDirty());
    CORRADE_VERIFY(a.isDirty());
    CORRADE_VERIFY(b.isDirty());

    /* Cleaning object will not clean anything other */
    a.setClean();
    CORRADE_VERIFY(shapes.isDirty());
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(b.isDirty());

    /* Verify that the feature was actually cleaned */
    CORRADE_COMPARE(shape->transformedShape().position(),
        Vector3(-2.0f, 4.0f, -6.0f));

    /* Setting group clean will clean whole group */
    a.setDirty();
    shapes.setClean();
    CORRADE_VERIFY(!shapes.isDirty());
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(!b.isDirty());

    /* Setting object dirty will set also the group, but not other objects */
    b.setDirty();
    CORRADE_VERIFY(shapes.isDirty());
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(b.isDirty());
}

void ShapeTest::firstCollision() {
    Scene3D scene;
    ShapeGroup3D shapes;

    Object3D a(&scene);
    auto aShape = new Shape<Shapes::Sphere3D>(&a, {{1.0f, -2.0f, 3.0f}, 1.5f}, &shapes);

    Object3D b(&scene);
    auto bShape = new Shape<Shapes::Point3D>(&b, {{3.0f, -2.0f, 3.0f}}, &shapes);

    Object3D c(&scene);
    new Shape<Shapes::Composition3D>(&c, &shapes);

    /* No collisions initially */
    CORRADE_VERIFY(!shapes.firstCollision(aShape));
    CORRADE_VERIFY(!shapes.firstCollision(bShape));
    CORRADE_VERIFY(!shapes.isDirty());

    /* Move point into sphere */
    b.translate(Vector3::xAxis(-1.0f));

    /* Collision */
    CORRADE_VERIFY(shapes.isDirty());
    CORRADE_VERIFY(shapes.firstCollision(aShape) == bShape);
    CORRADE_VERIFY(shapes.firstCollision(bShape) == aShape);
    CORRADE_VERIFY(!shapes.isDirty());
}

void ShapeTest::shapeGroup() {
    Scene2D scene;
    ShapeGroup2D shapes;

    /* Verify construction */
    Object2D a(&scene);
    auto shape = new Shape<Shapes::Composition2D>(&a, Shapes::Sphere2D({}, 0.5f) || Shapes::Point2D({0.25f, -1.0f}));
    CORRADE_COMPARE(shape->transformedShape().size(), 2);

    /* Verify the original shape is updated */
    const auto& point = shape->transformedShape().get<Shapes::Point2D>(1);
    a.translate(Vector2::xAxis(5.0f));
    a.setClean();
    CORRADE_COMPARE(point.position(), Vector2(5.25f, -1.0f));
}

}}}

CORRADE_TEST_MAIN(Magnum::Shapes::Test::ShapeTest)
