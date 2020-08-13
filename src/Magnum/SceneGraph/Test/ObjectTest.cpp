/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include <sstream>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/SceneGraph/MatrixTransformation3D.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct ObjectTest: TestSuite::Tester {
    explicit ObjectTest();

    void addFeature();

    void parenting();
    void addChild();
    void move();
    void scene();
    void setParentKeepTransformation();
    void setParentKeepTransformationInvalid();
    void absoluteTransformation();
    void transformations();
    void transformationsRelative();
    void transformationsOrphan();
    void transformationsDuplicate();
    void setClean();
    void setCleanListHierarchy();
    void setCleanListBulk();

    void rangeBasedForChildren();
    void rangeBasedForFeatures();
};

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class CachingObject: public Object3D, AbstractFeature3D {
    public:
        CachingObject(Object3D* parent = nullptr): Object3D(parent), AbstractFeature3D(*this) {
            setCachedTransformations(CachedTransformation::Absolute);
        }

        Matrix4 cleanedAbsoluteTransformation;

    protected:
        void clean(const Matrix4& absoluteTransformation) override {
            cleanedAbsoluteTransformation = absoluteTransformation;
        }
};

ObjectTest::ObjectTest() {
    addTests({&ObjectTest::addFeature,

              &ObjectTest::parenting,
              &ObjectTest::addChild,
              &ObjectTest::move,
              &ObjectTest::scene,
              &ObjectTest::setParentKeepTransformation,
              &ObjectTest::setParentKeepTransformationInvalid,
              &ObjectTest::absoluteTransformation,
              &ObjectTest::transformations,
              &ObjectTest::transformationsRelative,
              &ObjectTest::transformationsOrphan,
              &ObjectTest::transformationsDuplicate,
              &ObjectTest::setClean,
              &ObjectTest::setCleanListHierarchy,
              &ObjectTest::setCleanListBulk,

              &ObjectTest::rangeBasedForChildren,
              &ObjectTest::rangeBasedForFeatures});
}

void ObjectTest::addFeature() {
    class MyFeature: public AbstractFeature3D {
        public:
            explicit MyFeature(AbstractObject3D& object, Int&, Containers::Pointer<int>&&): AbstractFeature3D{object} {}
    };

    Object3D o;
    CORRADE_VERIFY(o.features().isEmpty());
    /* Test perfect forwarding as well */
    int a = 0;
    MyFeature& f = o.addFeature<MyFeature>(a, Containers::Pointer<int>{});
    CORRADE_VERIFY(!o.features().isEmpty());
    CORRADE_COMPARE(&f.object(), &o);
}

void ObjectTest::parenting() {
    Object3D root;

    Object3D* childOne = new Object3D(&root);
    Object3D* childTwo = new Object3D(&root);

    CORRADE_VERIFY(childOne->parent() == &root);
    CORRADE_VERIFY(childTwo->parent() == &root);
    CORRADE_VERIFY(root.children().first() == childOne);
    CORRADE_VERIFY(root.children().last() == childTwo);
    CORRADE_VERIFY(root.children().first()->nextSibling() == root.children().last());

    /* A object cannot be parent of itself */
    childOne->setParent(childOne);
    CORRADE_VERIFY(childOne->parent() == &root);

    /* In fact, cyclic dependencies are not allowed at all */
    root.setParent(childTwo);
    CORRADE_VERIFY(root.parent() == nullptr);

    /* Reparent to another */
    childTwo->setParent(childOne);
    CORRADE_VERIFY(root.children().first() == childOne && root.children().first()->nextSibling() == nullptr);
    CORRADE_VERIFY(childOne->children().first() == childTwo && childOne->children().first()->nextSibling() == nullptr);

    /* Delete child */
    delete childTwo;
    CORRADE_VERIFY(childOne->children().isEmpty());
}

void ObjectTest::addChild() {
    class MyObject: public Object3D {
        public:
            explicit MyObject(Int&, Containers::Pointer<int>&&, Object3D* parent = nullptr): Object3D{parent} {}
    };

    Object3D o;
    CORRADE_VERIFY(o.children().isEmpty());
    /* Test perfect forwarding as well */
    int a = 0;
    MyObject& p = o.addChild<MyObject>(a, Containers::Pointer<int>{});
    CORRADE_VERIFY(!o.children().isEmpty());
    CORRADE_COMPARE(p.parent(), &o);
}

void ObjectTest::move() {
    Scene3D scene;
    Object3D* a = new Object3D{&scene};
    Object3D* b = new Object3D{&scene};
    Object3D* c = new Object3D{&scene};

    CORRADE_COMPARE(a->nextSibling(), b);
    CORRADE_COMPARE(b->nextSibling(), c);
    CORRADE_COMPARE(c->nextSibling(), nullptr);

    scene.move(*a, c);
    CORRADE_COMPARE(b->nextSibling(), a);
    CORRADE_COMPARE(a->nextSibling(), c);
    CORRADE_COMPARE(c->nextSibling(), nullptr);

    scene.move(*a, nullptr);
    CORRADE_COMPARE(b->nextSibling(), c);
    CORRADE_COMPARE(c->nextSibling(), a);
    CORRADE_COMPARE(a->nextSibling(), nullptr);
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

void ObjectTest::setParentKeepTransformation() {
    Object3D root;
    root.rotateZ(Deg(35.0f));

    Object3D* childOne = new Object3D(&root);
    Object3D* childTwo = new Object3D(&root);

    childOne->translate(Vector3::xAxis(2.0f));
    childTwo->rotateY(Deg(90.0f));

    /* Reparent to another and keep absolute transformation */
    auto transformation = childOne->absoluteTransformation();
    childOne->setParentKeepTransformation(childTwo);
    CORRADE_VERIFY(childOne->parent() == childTwo);
    CORRADE_COMPARE(childOne->absoluteTransformation(), transformation);
}

void ObjectTest::setParentKeepTransformationInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Object3D root;
    root.rotateZ(Deg(35.0f));

    Object3D* child = new Object3D(&root);

    /* Old parent and new parent must share the same scene */
    std::ostringstream o;
    Error redirectError{&o};
    Scene3D scene;
    child->setParentKeepTransformation(&scene);
    CORRADE_COMPARE(o.str(), "SceneGraph::Object::setParentKeepTransformation(): both parents must be in the same scene\n");
}

void ObjectTest::absoluteTransformation() {
    Scene3D s;

    /* Proper transformation composition */
    Object3D o(&s);
    o.translate(Vector3::xAxis(2.0f));
    CORRADE_COMPARE(o.transformation(), Matrix4::translation(Vector3::xAxis(2.0f)));
    CORRADE_COMPARE(o.transformation(), o.transformationMatrix());
    Object3D o2(&o);
    o2.rotateY(Deg(90.0f));
    CORRADE_COMPARE(o2.absoluteTransformation(),
        Matrix4::translation(Vector3::xAxis(2.0f))*Matrix4::rotationY(Deg(90.0f)));
    CORRADE_COMPARE(o2.absoluteTransformation(), o2.absoluteTransformationMatrix());

    /* Transformation of root object */
    Object3D o3;
    o3.translate({1.0f, 2.0f, 3.0f});
    CORRADE_COMPARE(o3.absoluteTransformation(), Matrix4::translation({1.0f, 2.0f, 3.0f}));
}

void ObjectTest::transformations() {
    Scene3D s;

    Matrix4 initial = Matrix4::rotationX(Deg(90.0f)).inverted();

    /* Empty list */
    CORRADE_COMPARE(s.transformations({}, initial), std::vector<Matrix4>());

    /* Scene alone */
    CORRADE_COMPARE(s.transformations({s}, initial), std::vector<Matrix4>{initial});

    /* One object */
    Object3D first(&s);
    first.rotateZ(Deg(30.0f));
    Object3D second(&first);
    second.scale(Vector3(0.5f));
    CORRADE_COMPARE(s.transformations({second}, initial), std::vector<Matrix4>{
        initial*Matrix4::rotationZ(Deg(30.0f))*Matrix4::scaling(Vector3(0.5f))
    });

    /* One object and scene */
    CORRADE_COMPARE(s.transformations({second, s}, initial), (std::vector<Matrix4>{
        initial*Matrix4::rotationZ(Deg(30.0f))*Matrix4::scaling(Vector3(0.5f)),
        initial
    }));

    /* Two objects with foreign joint */
    Object3D third(&first);
    third.translate(Vector3::xAxis(5.0f));
    CORRADE_COMPARE(s.transformations({second, third}, initial), (std::vector<Matrix4>{
        initial*Matrix4::rotationZ(Deg(30.0f))*Matrix4::scaling(Vector3(0.5f)),
        initial*Matrix4::rotationZ(Deg(30.0f))*Matrix4::translation(Vector3::xAxis(5.0f)),
    }));

    /* Three objects with joint as one of them */
    CORRADE_COMPARE(s.transformations({second, third, first}, initial), (std::vector<Matrix4>{
        initial*Matrix4::rotationZ(Deg(30.0f))*Matrix4::scaling(Vector3(0.5f)),
        initial*Matrix4::rotationZ(Deg(30.0f))*Matrix4::translation(Vector3::xAxis(5.0f)),
        initial*Matrix4::rotationZ(Deg(30.0f)),
    }));
}

void ObjectTest::transformationsRelative() {
    CORRADE_SKIP("Transformations not relative to scene are not implemented yet.");

    Scene3D s;
    Object3D first(&s);
    first.rotateZ(Deg(30.0f));
    Object3D second(&first);
    second.scale(Vector3(0.5f));
    Object3D third(&first);
    third.translate(Vector3::xAxis(5.0f));

    /* Transformation relative to another object */
    CORRADE_COMPARE(second.transformations({third}), std::vector<Matrix4>{
        Matrix4::scaling(Vector3(0.5f)).inverted()*Matrix4::translation(Vector3::xAxis(5.0f))
    });

    /* Transformation relative to another object, not part of any scene (but should work) */
    Object3D orphanParent1;
    orphanParent1.rotate(Deg(31.0f), Vector3(1.0f).normalized());
    Object3D orphanParent(&orphanParent1);
    Object3D orphan1(&orphanParent);
    orphan1.scale(Vector3::xScale(3.0f));
    Object3D orphan2(&orphanParent);
    orphan2.translate(Vector3::zAxis(5.0f));
    CORRADE_COMPARE(orphan1.transformations({orphan2}), std::vector<Matrix4>{
        Matrix4::scaling(Vector3::xScale(3.0f)).inverted()*Matrix4::translation(Vector3::zAxis(5.0f))
    });
}

void ObjectTest::transformationsOrphan() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream o;
    Error redirectError{&o};

    /* Transformation of objects not part of the same scene */
    Scene3D s;
    Object3D orphan;
    CORRADE_COMPARE(s.transformations({orphan}), std::vector<Matrix4>());
    CORRADE_COMPARE(o.str(), "SceneGraph::Object::transformations(): the objects are not part of the same tree\n");
}

void ObjectTest::transformationsDuplicate() {
    Scene3D s;
    Object3D first(&s);
    first.rotateZ(Deg(30.0f));
    Object3D second(&first);
    second.scale(Vector3(0.5f));
    Object3D third(&first);
    third.translate(Vector3::xAxis(5.0f));

    Matrix4 firstExpected = Matrix4::rotationZ(Deg(30.0f));
    Matrix4 secondExpected = Matrix4::rotationZ(Deg(30.0f))*Matrix4::scaling(Vector3(0.5f));
    Matrix4 thirdExpected = Matrix4::rotationZ(Deg(30.0f))*Matrix4::translation(Vector3::xAxis(5.0f));
    CORRADE_COMPARE(s.transformations({second, third, second, first, third}), (std::vector<Matrix4>{
        secondExpected, thirdExpected, secondExpected, firstExpected, thirdExpected
    }));
}

void ObjectTest::setClean() {
    Scene3D scene;

    class CachingFeature: public AbstractFeature3D {
        public:
            explicit CachingFeature(AbstractObject3D& object): AbstractFeature3D{object} {
                setCachedTransformations(CachedTransformation::Absolute);
            }

            Matrix4 cleanedAbsoluteTransformation;

            void clean(const Matrix4& absoluteTransformation) override {
                cleanedAbsoluteTransformation = absoluteTransformation;
            }
    };

    class CachingInvertedFeature: public AbstractFeature3D {
        public:
            explicit CachingInvertedFeature(AbstractObject3D& object): AbstractFeature3D{object} {
                setCachedTransformations(CachedTransformation::InvertedAbsolute);
            }

            Matrix4 cleanedInvertedAbsoluteTransformation;

            void cleanInverted(const Matrix4& invertedAbsoluteTransformation) override {
                cleanedInvertedAbsoluteTransformation = invertedAbsoluteTransformation;
            }
    };

    CachingObject* childOne = new CachingObject(&scene);
    childOne->scale(Vector3(2.0f));

    CachingObject* childTwo = new CachingObject(childOne);
    childTwo->translate(Vector3::xAxis(1.0f));
    CachingFeature* childTwoFeature = new CachingFeature(*childTwo);
    CachingInvertedFeature* childTwoFeature2 = new CachingInvertedFeature(*childTwo);

    CachingObject* childThree = new CachingObject(childTwo);
    childThree->rotate(Deg(90.0f), Vector3::yAxis());

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
    childOne->cleanedAbsoluteTransformation = Matrix4{Math::ZeroInit};
    CORRADE_VERIFY(!childOne->isDirty());
    childOne->setClean();
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, Matrix4{Math::ZeroInit});

    /* If any object in the hierarchy is already clean, it shouldn't clean it again */
    CORRADE_VERIFY(!childOne->isDirty());
    CORRADE_VERIFY(childTwo->isDirty());
    childTwo->setClean();
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, Matrix4{Math::ZeroInit});

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

void ObjectTest::setCleanListHierarchy() {
    Scene3D scene;

    class CachingFeature: public AbstractFeature3D {
        public:
            explicit CachingFeature(AbstractObject3D& object): AbstractFeature3D{object} {
                setCachedTransformations(CachedTransformation::Absolute);
            }

            Matrix4 cleanedAbsoluteTransformation;

            void clean(const Matrix4& absoluteTransformation) override {
                cleanedAbsoluteTransformation = absoluteTransformation;
            }
    };

    CachingObject* childOne = new CachingObject(&scene);
    childOne->scale(Vector3(2.0f));

    CachingObject* childTwo = new CachingObject(childOne);
    childTwo->translate(Vector3::xAxis(1.0f));
    CachingFeature* childTwoFeature = new CachingFeature(*childTwo);

    CachingObject* childThree = new CachingObject(childTwo);
    childThree->rotate(Deg(90.0f), Vector3::yAxis());

    /* Clean the object and all its dirty parents (but not children) */
    Scene3D::setClean({*childTwo});
    CORRADE_VERIFY(!scene.isDirty());
    CORRADE_VERIFY(!childOne->isDirty());
    CORRADE_VERIFY(!childTwo->isDirty());
    CORRADE_VERIFY(childThree->isDirty());

    /* Verify the right matrices were passed */
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, childOne->absoluteTransformationMatrix());
    CORRADE_COMPARE(childTwo->cleanedAbsoluteTransformation, childTwo->absoluteTransformationMatrix());
    CORRADE_COMPARE(childTwoFeature->cleanedAbsoluteTransformation, childTwo->absoluteTransformationMatrix());

    /* If the object itself is already clean, it shouldn't clean it again */
    childOne->cleanedAbsoluteTransformation = Matrix4{Math::ZeroInit};
    CORRADE_VERIFY(!childOne->isDirty());
    Scene3D::setClean({*childOne});
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, Matrix4{Math::ZeroInit});

    /* If any object in the hierarchy is already clean, it shouldn't clean it again */
    CORRADE_VERIFY(!childOne->isDirty());
    childTwo->setDirty();
    Scene3D::setClean({*childTwo});
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, Matrix4{Math::ZeroInit});
}

void ObjectTest::setCleanListBulk() {
    /* Verify it doesn't crash when passed empty list */
    Object3D::setClean({});

    Scene3D scene;
    Object3D a(&scene);
    Object3D b(&scene);
    b.setClean();
    Object3D c(&scene);
    c.translate(Vector3::zAxis(3.0f));
    CachingObject d(&c);
    d.scale(Vector3(-2.0f));
    Object3D e(&scene);

    /* All objects should be cleaned */
    CORRADE_VERIFY(a.isDirty());
    CORRADE_VERIFY(!b.isDirty());
    CORRADE_VERIFY(c.isDirty());
    CORRADE_VERIFY(d.isDirty());
    CORRADE_VERIFY(e.isDirty());
    Object3D::setClean({a, b, c, d, e});
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(!b.isDirty());
    CORRADE_VERIFY(!c.isDirty());
    CORRADE_VERIFY(!d.isDirty());
    CORRADE_VERIFY(!e.isDirty());

    /* Verify that right transformation was passed */
    CORRADE_COMPARE(d.cleanedAbsoluteTransformation, Matrix4::translation(Vector3::zAxis(3.0f))*Matrix4::scaling(Vector3(-2.0f)));
}

void ObjectTest::rangeBasedForChildren() {
    Scene3D scene;
    Object3D a(&scene);
    Object3D b(&scene);
    Object3D c(&scene);

    std::vector<Object3D*> objects;
    for(auto&& i: scene.children()) objects.push_back(&i);
    CORRADE_COMPARE(objects, (std::vector<Object3D*>{&a, &b, &c}));
}

void ObjectTest::rangeBasedForFeatures() {
    struct Feature: AbstractFeature3D {
        explicit Feature(AbstractObject3D& object): AbstractFeature3D{object} {}
    };

    Object3D object;
    Feature a(object);
    Feature b(object);
    Feature c(object);

    std::vector<AbstractFeature3D*> features;
    for(auto&& i: object.features()) features.push_back(&i);
    CORRADE_COMPARE(features, (std::vector<AbstractFeature3D*>{&a, &b, &c}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::ObjectTest)
