/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/SceneGraph/DualComplexTransformation.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

typedef Object<DualComplexTransformation> Object2D;
typedef Scene<DualComplexTransformation> Scene2D;

struct DualComplexTransformationTest: TestSuite::Tester {
    explicit DualComplexTransformationTest();

    void fromMatrix();
    void toMatrix();
    void compose();
    void inverted();

    void setTransformation();
    void resetTransformation();
    void transform();
    void translate();
    void rotate();
    void normalizeRotation();
};

DualComplexTransformationTest::DualComplexTransformationTest() {
    addTests({&DualComplexTransformationTest::fromMatrix,
              &DualComplexTransformationTest::toMatrix,
              &DualComplexTransformationTest::compose,
              &DualComplexTransformationTest::inverted,

              &DualComplexTransformationTest::setTransformation,
              &DualComplexTransformationTest::resetTransformation,
              &DualComplexTransformationTest::transform,
              &DualComplexTransformationTest::translate,
              &DualComplexTransformationTest::rotate,
              &DualComplexTransformationTest::normalizeRotation});
}

void DualComplexTransformationTest::fromMatrix() {
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    DualComplex c = DualComplex::rotation(Deg(17.0f))*DualComplex::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<DualComplexTransformation>::fromMatrix(m), c);
}

void DualComplexTransformationTest::toMatrix() {
    DualComplex c = DualComplex::rotation(Deg(17.0f))*DualComplex::translation({1.0f, -0.3f});
    Matrix3 m = Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<DualComplexTransformation>::toMatrix(c), m);
}

void DualComplexTransformationTest::compose() {
    DualComplex parent = DualComplex::rotation(Deg(17.0f));
    DualComplex child = DualComplex::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<DualComplexTransformation>::compose(parent, child), parent*child);
}

void DualComplexTransformationTest::inverted() {
    DualComplex c = DualComplex::rotation(Deg(17.0f))*DualComplex::translation({1.0f, -0.3f});
    CORRADE_COMPARE(Implementation::Transformation<DualComplexTransformation>::inverted(c)*c, DualComplex());
}

void DualComplexTransformationTest::setTransformation() {
    Object2D o;

    /* Can't transform with non-rigid transformation */
    std::ostringstream out;
    Error redirectError{&out};
    o.setTransformation(DualComplex({1.0f, 2.0f}, {}));
    CORRADE_COMPARE(out.str(), "SceneGraph::DualComplexTransformation::setTransformation(): the dual complex number is not normalized\n");

    /* Dirty after setting transformation */
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation(DualComplex::rotation(Deg(17.0f)));
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f)));

    /* Scene cannot be transformed */
    Scene2D s;
    s.setClean();
    s.setTransformation(DualComplex::rotation(Deg(17.0f)));
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Matrix3());
}

void DualComplexTransformationTest::resetTransformation() {
    Object2D o;
    o.setTransformation(DualComplex::rotation(Deg(17.0f)));
    CORRADE_VERIFY(o.transformationMatrix() != Matrix3());
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3());
}

void DualComplexTransformationTest::transform() {
    {
        /* Can't transform with non-rigid transformation */
        Object2D o;
        std::ostringstream out;
        Error redirectError{&out};
        o.transform(DualComplex({1.0f, 2.0f}, {}));
        CORRADE_COMPARE(out.str(), "SceneGraph::DualComplexTransformation::transform(): the dual complex number is not normalized\n");
    } {
        Object2D o;
        o.setTransformation(DualComplex::rotation(Deg(17.0f)));
        o.transform(DualComplex::translation({1.0f, -0.3f}));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({1.0f, -0.3f})*Matrix3::rotation(Deg(17.0f)));
    } {
        Object2D o;
        o.setTransformation(DualComplex::rotation(Deg(17.0f)));
        o.transformLocal(DualComplex::translation({1.0f, -0.3f}));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f}));
    }
}

void DualComplexTransformationTest::translate() {
    {
        Object2D o;
        o.setTransformation(DualComplex::rotation(Deg(17.0f)));
        o.translate({1.0f, -0.3f});
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({1.0f, -0.3f})*Matrix3::rotation(Deg(17.0f)));
    } {
        Object2D o;
        o.setTransformation(DualComplex::rotation(Deg(17.0f)));
        o.translateLocal({1.0f, -0.3f});
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f}));
    }
}

void DualComplexTransformationTest::rotate() {
    {
        Object2D o;
        o.setTransformation(DualComplex::translation({1.0f, -0.3f}));
        o.rotate(Deg(17.0f));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f))*Matrix3::translation({1.0f, -0.3f}));
    } {
        Object2D o;
        o.setTransformation(DualComplex::translation({1.0f, -0.3f}));
        o.rotateLocal(Deg(17.0f));
        CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({1.0f, -0.3f})*Matrix3::rotation(Deg(17.0f)));
    }
}

void DualComplexTransformationTest::normalizeRotation() {
    Object2D o;
    o.setTransformation(DualComplex::rotation(Deg(17.0f)));
    o.normalizeRotation();
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::rotation(Deg(17.0f)));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::DualComplexTransformationTest)
