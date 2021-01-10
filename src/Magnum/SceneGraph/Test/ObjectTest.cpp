/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/SceneGraph/AbstractFeature.hpp"
#include "Magnum/SceneGraph/MatrixTransformation3D.hpp"
#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

struct ObjectTest: TestSuite::Tester {
    explicit ObjectTest();

    template<class T> void addFeature();

    template<class T> void parenting();
    template<class T> void addChild();
    template<class T> void move();
    template<class T> void scene();
    template<class T> void setParentKeepTransformation();
    template<class T> void setParentKeepTransformationInvalid();
    template<class T> void absoluteTransformation();
    template<class T> void transformations();
    template<class T> void transformationsRelative();
    template<class T> void transformationsOrphan();
    template<class T> void transformationsDuplicate();
    template<class T> void setClean();
    template<class T> void setCleanListHierarchy();
    template<class T> void setCleanListBulk();

    template<class T> void rangeBasedForChildren();
    template<class T> void rangeBasedForFeatures();
};

ObjectTest::ObjectTest() {
    addTests<ObjectTest>({
        &ObjectTest::addFeature<Float>,
        &ObjectTest::addFeature<Double>,

        &ObjectTest::parenting<Float>,
        &ObjectTest::parenting<Double>,
        &ObjectTest::addChild<Float>,
        &ObjectTest::addChild<Double>,
        &ObjectTest::move<Float>,
        &ObjectTest::move<Double>,
        &ObjectTest::scene<Float>,
        &ObjectTest::scene<Double>,
        &ObjectTest::setParentKeepTransformation<Float>,
        &ObjectTest::setParentKeepTransformation<Double>,
        &ObjectTest::setParentKeepTransformationInvalid<Float>,
        &ObjectTest::setParentKeepTransformationInvalid<Double>,
        &ObjectTest::absoluteTransformation<Float>,
        &ObjectTest::absoluteTransformation<Double>,
        &ObjectTest::transformations<Float>,
        &ObjectTest::transformations<Double>,
        &ObjectTest::transformationsRelative<Float>,
        &ObjectTest::transformationsRelative<Double>,
        &ObjectTest::transformationsOrphan<Float>,
        &ObjectTest::transformationsOrphan<Double>,
        &ObjectTest::transformationsDuplicate<Float>,
        &ObjectTest::transformationsDuplicate<Double>,
        &ObjectTest::setClean<Float>,
        &ObjectTest::setClean<Double>,
        &ObjectTest::setCleanListHierarchy<Float>,
        &ObjectTest::setCleanListHierarchy<Double>,
        &ObjectTest::setCleanListBulk<Float>,
        &ObjectTest::setCleanListBulk<Double>,

        &ObjectTest::rangeBasedForChildren<Float>,
        &ObjectTest::rangeBasedForChildren<Double>,
        &ObjectTest::rangeBasedForFeatures<Float>,
        &ObjectTest::rangeBasedForFeatures<Double>});
}

template<class T> using Object3D = SceneGraph::Object<SceneGraph::BasicMatrixTransformation3D<T>>;
template<class T> using Scene3D = SceneGraph::Scene<SceneGraph::BasicMatrixTransformation3D<T>>;

template<class T> class CachingObject: public Object3D<T>, AbstractBasicFeature3D<T> {
    public:
        CachingObject(Object3D<T>* parent = nullptr): Object3D<T>(parent), AbstractBasicFeature3D<T>{*this} {
            this->setCachedTransformations(CachedTransformation::Absolute);
        }

        Math::Matrix4<T> cleanedAbsoluteTransformation;

    protected:
        void clean(const Math::Matrix4<T>& absoluteTransformation) override {
            cleanedAbsoluteTransformation = absoluteTransformation;
        }
};

template<class T> void ObjectTest::addFeature() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    class MyFeature: public AbstractBasicFeature3D<T> {
        public:
            explicit MyFeature(AbstractBasicObject3D<T>& object, Int&, Containers::Pointer<int>&&): AbstractBasicFeature3D<T>{object} {}
    };

    Object3D<T> o;
    CORRADE_VERIFY(o.features().isEmpty());
    /* Test perfect forwarding as well */
    int a = 0;
    MyFeature& f = o.template addFeature<MyFeature>(a, Containers::Pointer<int>{});
    CORRADE_VERIFY(!o.features().isEmpty());
    CORRADE_COMPARE(&f.object(), &o);
}

template<class T> void ObjectTest::parenting() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> root;

    Object3D<T>* childOne = new Object3D<T>(&root);
    Object3D<T>* childTwo = new Object3D<T>(&root);

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

template<class T> void ObjectTest::addChild() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    class MyObject: public Object3D<T> {
        public:
            explicit MyObject(Int&, Containers::Pointer<int>&&, Object3D<T>* parent = nullptr): Object3D<T>{parent} {}
    };

    Object3D<T> o;
    CORRADE_VERIFY(o.children().isEmpty());
    /* Test perfect forwarding as well */
    int a = 0;
    MyObject& p = o.template addChild<MyObject>(a, Containers::Pointer<int>{});
    CORRADE_VERIFY(!o.children().isEmpty());
    CORRADE_COMPARE(p.parent(), &o);
}

template<class T> void ObjectTest::move() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Scene3D<T> scene;
    Object3D<T>* a = new Object3D<T>{&scene};
    Object3D<T>* b = new Object3D<T>{&scene};
    Object3D<T>* c = new Object3D<T>{&scene};

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

template<class T> void ObjectTest::scene() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Scene3D<T> scene;
    CORRADE_VERIFY(scene.scene() == &scene);

    Object3D<T>* childOne = new Object3D<T>(&scene);
    Object3D<T>* childTwo = new Object3D<T>(childOne);

    Object3D<T> orphan;
    Object3D<T>* childOfOrphan = new Object3D<T>(&orphan);

    CORRADE_VERIFY(childTwo->scene() == &scene);
    CORRADE_VERIFY(childOfOrphan->scene() == nullptr);
}

template<class T> void ObjectTest::setParentKeepTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Object3D<T> root;
    root.rotateZ(Math::Deg<T>{T(35.0)});

    Object3D<T>* childOne = new Object3D<T>(&root);
    Object3D<T>* childTwo = new Object3D<T>(&root);

    childOne->translate(Math::Vector3<T>::xAxis(T(2.0)));
    childTwo->rotateY(Math::Deg<T>{90.0});

    /* Reparent to another and keep absolute transformation */
    auto transformation = childOne->absoluteTransformation();
    childOne->setParentKeepTransformation(childTwo);
    CORRADE_VERIFY(childOne->parent() == childTwo);
    CORRADE_COMPARE(childOne->absoluteTransformation(), transformation);
}

template<class T> void ObjectTest::setParentKeepTransformationInvalid() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Object3D<T> root;
    root.rotateZ(Math::Deg<T>{35.0});

    Object3D<T>* child = new Object3D<T>(&root);

    /* Old parent and new parent must share the same scene */
    std::ostringstream o;
    Error redirectError{&o};
    Scene3D<T> scene;
    child->setParentKeepTransformation(&scene);
    CORRADE_COMPARE(o.str(), "SceneGraph::Object::setParentKeepTransformation(): both parents must be in the same scene\n");
}

template<class T> void ObjectTest::absoluteTransformation() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Scene3D<T> s;

    /* Proper transformation composition */
    Object3D<T> o(&s);
    o.translate(Math::Vector3<T>::xAxis(T(2.0)));
    CORRADE_COMPARE(o.transformation(), Math::Matrix4<T>::translation(Math::Vector3<T>::xAxis(T(2.0))));
    CORRADE_COMPARE(o.transformation(), o.transformationMatrix());
    Object3D<T> o2(&o);
    o2.rotateY(Math::Deg<T>{90.0});
    CORRADE_COMPARE(o2.absoluteTransformation(),
        Math::Matrix4<T>::translation(Math::Vector3<T>::xAxis(T(2.0)))*Math::Matrix4<T>::rotationY(Math::Deg<T>{90.0}));
    CORRADE_COMPARE(o2.absoluteTransformation(), o2.absoluteTransformationMatrix());

    /* Transformation of root object */
    Object3D<T> o3;
    o3.translate({T(1.0), T(2.0), T(3.0)});
    CORRADE_COMPARE(o3.absoluteTransformation(), Math::Matrix4<T>::translation({T(1.0), T(2.0), T(3.0)}));
}

template<class T> void ObjectTest::transformations() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Scene3D<T> s;

    Math::Matrix4<T> initial = Math::Matrix4<T>::rotationX(Math::Deg<T>{90.0}).inverted();

    /* Empty list */
    CORRADE_COMPARE(s.transformations({}, initial), std::vector<Math::Matrix4<T>>{});

    /* Scene alone */
    CORRADE_COMPARE(s.transformations({s}, initial), std::vector<Math::Matrix4<T>>{initial});

    /* One object */
    Object3D<T> first(&s);
    first.rotateZ(Math::Deg<T>{30.0});
    Object3D<T> second(&first);
    second.scale(Math::Vector3<T>(T(0.5)));
    CORRADE_COMPARE(s.transformations({second}, initial), std::vector<Math::Matrix4<T>>{
        initial*Math::Matrix4<T>::rotationZ(Math::Deg<T>{30.0})*Math::Matrix4<T>::scaling(Math::Vector3<T>(T(0.5)))
    });

    /* One object and scene */
    CORRADE_COMPARE(s.transformations({second, s}, initial), (std::vector<Math::Matrix4<T>>{
        initial*Math::Matrix4<T>::rotationZ(Math::Deg<T>{30.0})*Math::Matrix4<T>::scaling(Math::Vector3<T>(T(0.5))),
        initial
    }));

    /* Two objects with foreign joint */
    Object3D<T> third(&first);
    third.translate(Math::Vector3<T>::xAxis(T(5.0)));
    CORRADE_COMPARE(s.transformations({second, third}, initial), (std::vector<Math::Matrix4<T>>{
        initial*Math::Matrix4<T>::rotationZ(Math::Deg<T>{30.0})*Math::Matrix4<T>::scaling(Math::Vector3<T>(T(0.5))),
        initial*Math::Matrix4<T>::rotationZ(Math::Deg<T>{30.0})*Math::Matrix4<T>::translation(Math::Vector3<T>::xAxis(T(5.0))),
    }));

    /* Three objects with joint as one of them */
    CORRADE_COMPARE(s.transformations({second, third, first}, initial), (std::vector<Math::Matrix4<T>>{
        initial*Math::Matrix4<T>::rotationZ(Math::Deg<T>{30.0})*Math::Matrix4<T>::scaling(Math::Vector3<T>(T(0.5))),
        initial*Math::Matrix4<T>::rotationZ(Math::Deg<T>{30.0})*Math::Matrix4<T>::translation(Math::Vector3<T>::xAxis(T(5.0))),
        initial*Math::Matrix4<T>::rotationZ(Math::Deg<T>{30.0}),
    }));
}

template<class T> void ObjectTest::transformationsRelative() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    CORRADE_SKIP("Transformations not relative to scene are not implemented yet.");

    Scene3D<T> s;
    Object3D<T> first(&s);
    first.rotateZ(Math::Deg<T>{30.0});
    Object3D<T> second(&first);
    second.scale(Math::Vector3<T>(T(0.5)));
    Object3D<T> third(&first);
    third.translate(Math::Vector3<T>::xAxis(T(5.0)));

    /* Transformation relative to another object */
    CORRADE_COMPARE(second.transformations({third}), std::vector<Math::Matrix4<T>>{
        Math::Matrix4<T>::scaling(Math::Vector3<T>(T(0.5))).inverted()*Math::Matrix4<T>::translation(Math::Vector3<T>::xAxis(T(5.0)))
    });

    /* Transformation relative to another object, not part of any scene (but should work) */
    Object3D<T> orphanParent1;
    orphanParent1.rotate(Math::Deg<T>{31.0}, Math::Vector3<T>(T(1.0)).normalized());
    Object3D<T> orphanParent(&orphanParent1);
    Object3D<T> orphan1(&orphanParent);
    orphan1.scale(Math::Vector3<T>::xScale(T(3.0)));
    Object3D<T> orphan2(&orphanParent);
    orphan2.translate(Math::Vector3<T>::zAxis(T(5.0)));
    CORRADE_COMPARE(orphan1.transformations({orphan2}), std::vector<Math::Matrix4<T>>{
        Math::Matrix4<T>::scaling(Math::Vector3<T>::xScale(T(3.0))).inverted()*Math::Matrix4<T>::translation(Math::Vector3<T>::zAxis(T(5.0)))
    });
}

template<class T> void ObjectTest::transformationsOrphan() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream o;
    Error redirectError{&o};

    /* Transformation of objects not part of the same scene */
    Scene3D<T> s;
    Object3D<T> orphan;
    CORRADE_COMPARE(s.transformations({orphan}), std::vector<Math::Matrix4<T>>{});
    CORRADE_COMPARE(o.str(), "SceneGraph::Object::transformations(): the objects are not part of the same tree\n");
}

template<class T> void ObjectTest::transformationsDuplicate() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Scene3D<T> s;
    Object3D<T> first(&s);
    first.rotateZ(Math::Deg<T>{T(30.0)});
    Object3D<T> second(&first);
    second.scale(Math::Vector3<T>(T(0.5)));
    Object3D<T> third(&first);
    third.translate(Math::Vector3<T>::xAxis(T(5.0)));

    Math::Matrix4<T> firstExpected = Math::Matrix4<T>::rotationZ(Math::Deg<T>{30.0});
    Math::Matrix4<T> secondExpected = Math::Matrix4<T>::rotationZ(Math::Deg<T>{30.0})*Math::Matrix4<T>::scaling(Math::Vector3<T>(T(0.5)));
    Math::Matrix4<T> thirdExpected = Math::Matrix4<T>::rotationZ(Math::Deg<T>{30.0})*Math::Matrix4<T>::translation(Math::Vector3<T>::xAxis(T(5.0)));
    CORRADE_COMPARE(s.transformations({second, third, second, first, third}), (std::vector<Math::Matrix4<T>>{
        secondExpected, thirdExpected, secondExpected, firstExpected, thirdExpected
    }));
}

template<class T> void ObjectTest::setClean() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Scene3D<T> scene;

    class CachingFeature: public AbstractBasicFeature3D<T> {
        public:
            explicit CachingFeature(AbstractBasicObject3D<T>& object): AbstractBasicFeature3D<T>{object} {
                this->setCachedTransformations(CachedTransformation::Absolute);
            }

            Math::Matrix4<T> cleanedAbsoluteTransformation;

            void clean(const Math::Matrix4<T>& absoluteTransformation) override {
                cleanedAbsoluteTransformation = absoluteTransformation;
            }
    };

    class CachingInvertedFeature: public AbstractBasicFeature3D<T> {
        public:
            explicit CachingInvertedFeature(AbstractBasicObject3D<T>& object): AbstractBasicFeature3D<T>{object} {
                this->setCachedTransformations(CachedTransformation::InvertedAbsolute);
            }

            Math::Matrix4<T> cleanedInvertedAbsoluteTransformation;

            void cleanInverted(const Math::Matrix4<T>& invertedAbsoluteTransformation) override {
                cleanedInvertedAbsoluteTransformation = invertedAbsoluteTransformation;
            }
    };

    CachingObject<T>* childOne = new CachingObject<T>{&scene};
    childOne->scale(Math::Vector3<T>(T(2.0)));

    CachingObject<T>* childTwo = new CachingObject<T>{childOne};
    childTwo->translate(Math::Vector3<T>::xAxis(T(1.0)));
    CachingFeature* childTwoFeature = new CachingFeature(*childTwo);
    CachingInvertedFeature* childTwoFeature2 = new CachingInvertedFeature(*childTwo);

    CachingObject<T>* childThree = new CachingObject<T>{childTwo};
    childThree->rotate(Math::Deg<T>{90.0}, Math::Vector3<T>::yAxis());

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
    childOne->cleanedAbsoluteTransformation = Math::Matrix4<T>{Math::ZeroInit};
    CORRADE_VERIFY(!childOne->isDirty());
    childOne->setClean();
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, Math::Matrix4<T>{Math::ZeroInit});

    /* If any object in the hierarchy is already clean, it shouldn't clean it again */
    CORRADE_VERIFY(!childOne->isDirty());
    CORRADE_VERIFY(childTwo->isDirty());
    childTwo->setClean();
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, Math::Matrix4<T>{Math::ZeroInit});

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
    childTwo->setTransformation(Math::Matrix4<T>::translation(Math::Vector3<T>::xAxis(T(1.0))));
    CORRADE_VERIFY(!scene.isDirty());
    CORRADE_VERIFY(childTwo->isDirty());
    CORRADE_VERIFY(childThree->isDirty());
}

template<class T> void ObjectTest::setCleanListHierarchy() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Scene3D<T> scene;

    class CachingFeature: public AbstractBasicFeature3D<T> {
        public:
            explicit CachingFeature(AbstractBasicObject3D<T>& object): AbstractBasicFeature3D<T>{object} {
                this->setCachedTransformations(CachedTransformation::Absolute);
            }

            Math::Matrix4<T> cleanedAbsoluteTransformation;

            void clean(const Math::Matrix4<T>& absoluteTransformation) override {
                cleanedAbsoluteTransformation = absoluteTransformation;
            }
    };

    CachingObject<T>* childOne = new CachingObject<T>{&scene};
    childOne->scale(Math::Vector3<T>(T(2.0)));

    CachingObject<T>* childTwo = new CachingObject<T>{childOne};
    childTwo->translate(Math::Vector3<T>::xAxis(T(1.0)));
    CachingFeature* childTwoFeature = new CachingFeature(*childTwo);

    CachingObject<T>* childThree = new CachingObject<T>{childTwo};
    childThree->rotate(Math::Deg<T>{90.0}, Math::Vector3<T>::yAxis());

    /* Clean the object and all its dirty parents (but not children) */
    Scene3D<T>::setClean({*childTwo});
    CORRADE_VERIFY(!scene.isDirty());
    CORRADE_VERIFY(!childOne->isDirty());
    CORRADE_VERIFY(!childTwo->isDirty());
    CORRADE_VERIFY(childThree->isDirty());

    /* Verify the right matrices were passed */
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, childOne->absoluteTransformationMatrix());
    CORRADE_COMPARE(childTwo->cleanedAbsoluteTransformation, childTwo->absoluteTransformationMatrix());
    CORRADE_COMPARE(childTwoFeature->cleanedAbsoluteTransformation, childTwo->absoluteTransformationMatrix());

    /* If the object itself is already clean, it shouldn't clean it again */
    childOne->cleanedAbsoluteTransformation = Math::Matrix4<T>{Math::ZeroInit};
    CORRADE_VERIFY(!childOne->isDirty());
    Scene3D<T>::setClean({*childOne});
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, Math::Matrix4<T>{Math::ZeroInit});

    /* If any object in the hierarchy is already clean, it shouldn't clean it again */
    CORRADE_VERIFY(!childOne->isDirty());
    childTwo->setDirty();
    Scene3D<T>::setClean({*childTwo});
    CORRADE_COMPARE(childOne->cleanedAbsoluteTransformation, Math::Matrix4<T>{Math::ZeroInit});
}

template<class T> void ObjectTest::setCleanListBulk() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Verify it doesn't crash when passed empty list */
    Object3D<T>::setClean({});

    Scene3D<T> scene;
    Object3D<T> a(&scene);
    Object3D<T> b(&scene);
    b.setClean();
    Object3D<T> c(&scene);
    c.translate(Math::Vector3<T>::zAxis(T(3.0)));
    CachingObject<T> d(&c);
    d.scale(Math::Vector3<T>(T(-2.0)));
    Object3D<T> e(&scene);

    /* All objects should be cleaned */
    CORRADE_VERIFY(a.isDirty());
    CORRADE_VERIFY(!b.isDirty());
    CORRADE_VERIFY(c.isDirty());
    CORRADE_VERIFY(d.isDirty());
    CORRADE_VERIFY(e.isDirty());
    Object3D<T>::setClean({a, b, c, d, e});
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(!b.isDirty());
    CORRADE_VERIFY(!c.isDirty());
    CORRADE_VERIFY(!d.isDirty());
    CORRADE_VERIFY(!e.isDirty());

    /* Verify that right transformation was passed */
    CORRADE_COMPARE(d.cleanedAbsoluteTransformation, Math::Matrix4<T>::translation(Math::Vector3<T>::zAxis(T(3.0)))*Math::Matrix4<T>::scaling(Math::Vector3<T>(T(-2.0))));
}

template<class T> void ObjectTest::rangeBasedForChildren() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    Scene3D<T> scene;
    Object3D<T> a{&scene};
    Object3D<T> b{&scene};
    Object3D<T> c{&scene};

    std::vector<Object3D<T>*> objects;
    for(auto&& i: scene.children()) objects.push_back(&i);
    CORRADE_COMPARE(objects, (std::vector<Object3D<T>*>{&a, &b, &c}));
}

template<class T> void ObjectTest::rangeBasedForFeatures() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    struct Feature: AbstractBasicFeature3D<T> {
        explicit Feature(AbstractBasicObject3D<T>& object): AbstractBasicFeature3D<T>{object} {}
    };

    Object3D<T> object;
    Feature a{object};
    Feature b{object};
    Feature c{object};

    std::vector<AbstractBasicFeature3D<T>*> features;
    for(auto&& i: object.features()) features.push_back(&i);
    CORRADE_COMPARE(features, (std::vector<AbstractBasicFeature3D<T>*>{&a, &b, &c}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::ObjectTest)
