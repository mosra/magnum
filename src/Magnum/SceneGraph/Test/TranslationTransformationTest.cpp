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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/SceneGraph/Object.hpp"
#include "Magnum/SceneGraph/TranslationTransformation.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph { namespace Test { namespace {

typedef Object<TranslationTransformation2D> Object2D;
typedef Scene<TranslationTransformation2D> Scene2D;

struct TranslationTransformationTest: TestSuite::Tester {
    explicit TranslationTransformationTest();

    void fromMatrix();
    void fromMatrixInvalid();
    void toMatrix();
    void compose();
    void inverted();

    void setTransformation();
    void resetTransformation();
    void transform();
    void translate();

    void integral();
};

TranslationTransformationTest::TranslationTransformationTest() {
    addTests({&TranslationTransformationTest::fromMatrix,
              &TranslationTransformationTest::fromMatrixInvalid,
              &TranslationTransformationTest::toMatrix,
              &TranslationTransformationTest::compose,
              &TranslationTransformationTest::inverted,

              &TranslationTransformationTest::setTransformation,
              &TranslationTransformationTest::resetTransformation,
              &TranslationTransformationTest::transform,
              &TranslationTransformationTest::translate,

              &TranslationTransformationTest::integral});
}

void TranslationTransformationTest::fromMatrix() {
    const Vector2 v(1.0f, -0.3f);
    CORRADE_COMPARE(Implementation::Transformation<TranslationTransformation2D>::fromMatrix(Matrix3::translation(v)), v);
}

void TranslationTransformationTest::fromMatrixInvalid() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream o;
    Error redirectError{&o};
    Implementation::Transformation<TranslationTransformation2D>::fromMatrix(Matrix3::scaling(Vector2(4.0f)));
    CORRADE_COMPARE(o.str(), "SceneGraph::TranslationTransformation: the matrix doesn't represent pure translation\n");
}

void TranslationTransformationTest::toMatrix() {
    const Vector2 v(1.0f, -0.3f);
    CORRADE_COMPARE(Implementation::Transformation<TranslationTransformation2D>::toMatrix(v), Matrix3::translation(v));
}

void TranslationTransformationTest::compose() {
    const Vector2 parent(-0.5f, 2.0f);
    const Vector2 child(1.0f, -0.3f);
    CORRADE_COMPARE(Implementation::Transformation<TranslationTransformation2D>::compose(parent, child), Vector2(0.5f, 1.7f));
}

void TranslationTransformationTest::inverted() {
    const Vector2 v(1.0f, -0.3f);
    CORRADE_COMPARE(Implementation::Transformation<TranslationTransformation2D>::inverted(v), Vector2(-1.0f, 0.3f));
}

void TranslationTransformationTest::setTransformation() {
    Object2D o;

    /* Dirty after setting transformation */
    o.setClean();
    CORRADE_VERIFY(!o.isDirty());
    o.setTransformation({1.0f, -0.3f});
    CORRADE_VERIFY(o.isDirty());
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({1.0f, -0.3f}));

    /* Scene cannot be transformed */
    Scene2D s;
    s.setClean();
    s.setTransformation({1.0f, -0.3f});
    CORRADE_VERIFY(!s.isDirty());
    CORRADE_COMPARE(s.transformationMatrix(), Matrix3());
}

void TranslationTransformationTest::resetTransformation() {
    Object2D o;
    o.setTransformation({1.0f, -0.3f});
    CORRADE_VERIFY(o.transformationMatrix() != Matrix3());
    o.resetTransformation();
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3());
}

void TranslationTransformationTest::transform() {
    Object2D o;
    o.setTransformation({1.0f, -0.3f})
        .transform({-0.5f, 2.0f});
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({0.5f, 1.7f}));
}

void TranslationTransformationTest::translate() {
    Object2D o;
    o.setTransformation({1.0f, -0.3f})
        .translate({-0.5f, 2.0f});
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({1.0f, -0.3f})*Matrix3::translation({-0.5f, 2.0f}));
}

void TranslationTransformationTest::integral() {
    typedef Object<BasicTranslationTransformation2D<Float, Short>> Object2Di;

    Object2Di o;
    o.translate({3, -7});
    CORRADE_COMPARE(o.transformationMatrix(), Matrix3::translation({3.0f, -7.0f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::SceneGraph::Test::TranslationTransformationTest)
