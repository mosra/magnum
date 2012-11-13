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

#include <sstream>

#include "Math/Constants.h"
#include "SceneGraph/MatrixTransformation3D.h"
#include "SceneGraph/Scene.h"

using namespace std;

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::ObjectTest)

namespace Magnum { namespace SceneGraph { namespace Test {

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D<GLfloat>> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D<GLfloat>> Scene3D;

ObjectTest::ObjectTest() {
    addTests(&ObjectTest::parenting,
             &ObjectTest::scene,
             &ObjectTest::absoluteTransformation,
             &ObjectTest::transformations,
             &ObjectTest::setClean);
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

void ObjectTest::scene() {
    Scene3D scene;
    CORRADE_VERIFY(scene.scene() == &scene);

    Object3D* childOne = new Object3D(&scene);
    Object3D* childTwo = new Object3D(childOne);

    Object3D orphan;
    Object3D* childOfOrphan = new Object3D(&orphan);

    CORRADE_VERIFY(childTwo->scene() == &scene);
    CORRADE_VERIFY(childOfOrphan->scene() == nullptr);
}

void ObjectTest::absoluteTransformation() {
    Scene3D s;

    /* Proper transformation composition */
    Object3D o(&s);
    o.translate(Vector3::xAxis(2.0f));
    Object3D o2(&o);
    o2.rotateY(deg(90.0f));
    CORRADE_COMPARE(o2.absoluteTransformation(),
        Matrix4::translation(Vector3::xAxis(2.0f))*Matrix4::rotationY(deg(90.0f)));
    CORRADE_COMPARE(o2.absoluteTransformation(), o2.absoluteTransformationMatrix());

    /* Transformation of root object */
    Object3D o3;
    o3.translate({1.0f, 2.0f, 3.0f});
    CORRADE_COMPARE(o3.absoluteTransformation(), Matrix4::translation({1.0f, 2.0f, 3.0f}));
}

void ObjectTest::transformations() {
    Scene3D s;

    Matrix4 initial = Matrix4::rotationX(deg(90.0f)).inverted();

    /* Scene alone */
    CORRADE_COMPARE(s.transformations({&s}, initial), vector<Matrix4>{initial});

    /* One object */
    Object3D first(&s);
    first.rotateZ(deg(30.0f));
    Object3D second(&first);
    second.scale(Vector3(0.5f));
    CORRADE_COMPARE(s.transformations({&second}, initial), vector<Matrix4>{
        initial*Matrix4::rotationZ(deg(30.0f))*Matrix4::scaling(Vector3(0.5f))
    });

    /* One object and scene */
    CORRADE_COMPARE(s.transformations({&second, &s}, initial), (vector<Matrix4>{
        initial*Matrix4::rotationZ(deg(30.0f))*Matrix4::scaling(Vector3(0.5f)),
        initial
    }));

    /* Two objects with foreign joint */
    Object3D third(&first);
    third.translate(Vector3::xAxis(5.0f));
    CORRADE_COMPARE(s.transformations({&second, &third}, initial), (vector<Matrix4>{
        initial*Matrix4::rotationZ(deg(30.0f))*Matrix4::scaling(Vector3(0.5f)),
        initial*Matrix4::rotationZ(deg(30.0f))*Matrix4::translation(Vector3::xAxis(5.0f)),
    }));

    /* Three objects with joint as one of them */
    CORRADE_COMPARE(s.transformations({&second, &third, &first}, initial), (vector<Matrix4>{
        initial*Matrix4::rotationZ(deg(30.0f))*Matrix4::scaling(Vector3(0.5f)),
        initial*Matrix4::rotationZ(deg(30.0f))*Matrix4::translation(Vector3::xAxis(5.0f)),
        initial*Matrix4::rotationZ(deg(30.0f)),
    }));

    {
        CORRADE_EXPECT_FAIL("Transformations not relative to scene are not yet implemented.");

        /* Transformation relative to another object */
        CORRADE_COMPARE(second.transformations({&third}), vector<Matrix4>{
            Matrix4::scaling(Vector3(0.5f)).inverted()*Matrix4::translation(Vector3::xAxis(5.0f))
        });

        /* Transformation relative to another object, not part of any scene (but should work) */
        Object3D orphanParent1;
        orphanParent1.rotate(deg(31.0f), Vector3(1.0f).normalized());
        Object3D orphanParent(&orphanParent1);
        Object3D orphan1(&orphanParent);
        orphan1.scale(Vector3::xScale(3.0f));
        Object3D orphan2(&orphanParent);
        orphan2.translate(Vector3::zAxis(5.0f));
        CORRADE_COMPARE(orphan1.transformations({&orphan2}), vector<Matrix4>{
            Matrix4::scaling(Vector3::xScale(3.0f)).inverted()*Matrix4::translation(Vector3::zAxis(5.0f))
        });
    }

    ostringstream o;
    Error::setOutput(&o);

    /* Transformation of objects not part of the same scene */
    Object3D orphan;
    CORRADE_COMPARE(s.transformations({&orphan}), vector<Matrix4>());
    CORRADE_COMPARE(o.str(), "SceneGraph::Object::transformations(): the objects are not part of the same tree\n");
}

void ObjectTest::setClean() {
    Scene3D scene;

    class CachingFeature: public AbstractFeature<3, GLfloat> {
        public:
            CachingFeature(AbstractObject<3, GLfloat>* object): AbstractFeature<3, GLfloat>(object) {
                setCachedTransformations(CachedTransformation::Absolute);
            }

            Matrix4 cleanedAbsoluteTransformation;

            void clean(const Matrix4& absoluteTransformation) override {
                cleanedAbsoluteTransformation = absoluteTransformation;
            }
    };

    class CachingInvertedFeature: public AbstractFeature<3, GLfloat> {
        public:
            CachingInvertedFeature(AbstractObject<3, GLfloat>* object): AbstractFeature<3, GLfloat>(object) {
                setCachedTransformations(CachedTransformation::InvertedAbsolute);
            }

            Matrix4 cleanedInvertedAbsoluteTransformation;

            void cleanInverted(const Matrix4& invertedAbsoluteTransformation) override {
                cleanedInvertedAbsoluteTransformation = invertedAbsoluteTransformation;
            }
    };

    class CachingObject: public Object3D, AbstractFeature<3, GLfloat> {
        public:
            inline CachingObject(Object3D* parent = nullptr): Object3D(parent), AbstractFeature<3, GLfloat>(this) {
                setCachedTransformations(CachedTransformation::Absolute);
            }

            Matrix4 cleanedAbsoluteTransformation;

        protected:
            void clean(const Matrix4& absoluteTransformation) override {
                cleanedAbsoluteTransformation = absoluteTransformation;
            }
    };

    CachingObject* childOne = new CachingObject(&scene);
    childOne->scale(Vector3(2.0f));

    CachingObject* childTwo = new CachingObject(childOne);
    childTwo->translate(Vector3::xAxis(1.0f));
    CachingFeature* childTwoFeature = new CachingFeature(childTwo);
    CachingInvertedFeature* childTwoFeature2 = new CachingInvertedFeature(childTwo);

    CachingObject* childThree = new CachingObject(childTwo);
    childThree->rotate(deg(90.0f), Vector3::yAxis());

    /* Object is dirty at the beginning */
    CORRADE_VERIFY(scene.isDirty());
    CORRADE_VERIFY(childOne->isDirty());
    CORRADE_VERIFY(childTwo->isDirty());
    CORRADE_VERIFY(childThree->isDirty());

    /* Clean the object and all its dirty parents (but not children) */
    childTwo->setClean();
    CORRADE_VERIFY(!scene.isDirty());
    CORRADE_VERIFY(!childOne->isDirty());
    CORRADE_VERIFY(!childTwo->isDirty());
    CORRADE_VERIFY(childThree->isDirty());

    /* Verify the right matrices were passed */
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, childOne->absoluteTransformationMatrix());
    CORRADE_COMPARE(childTwo->cleanedAbsoluteTransformation, childTwo->absoluteTransformationMatrix());
    CORRADE_COMPARE(childTwoFeature->cleanedAbsoluteTransformation, childTwo->absoluteTransformationMatrix());
    CORRADE_COMPARE(childTwoFeature2->cleanedInvertedAbsoluteTransformation, childTwo->absoluteTransformationMatrix().inverted());

    /* Mark object and all its children as dirty (but not parents) */
    childTwo->setDirty();
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

    /* Remove object from tree => make it and its children dirty */
    childThree->setClean();
    childTwo->setParent(nullptr);
    CORRADE_VERIFY(childTwo->isDirty());
    CORRADE_VERIFY(!childOne->isDirty());

    /* Add object to tree => make it and its children dirty, don't touch parents */
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
