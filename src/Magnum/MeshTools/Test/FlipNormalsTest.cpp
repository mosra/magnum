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
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/FlipNormals.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct FlipNormalsTest: TestSuite::Tester {
    explicit FlipNormalsTest();

    void wrongIndexCount();
    template<class T> void flipFaceWinding();
    template<class T> void flipFaceWindingErased();
    void flipFaceWindingErasedNonContiguous();
    void flipFaceWindingErasedWrongIndexSize();
    void flipNormals();

    template<class T> void flipNormalsFaceWinding();
    template<class T> void flipNormalsFaceWindingErased();
    /* Delegates into type-erased flipFaceWinding(), so testing asserts again
       isn't needed */
};

FlipNormalsTest::FlipNormalsTest() {
    addTests({&FlipNormalsTest::wrongIndexCount,
              &FlipNormalsTest::flipFaceWinding<UnsignedByte>,
              &FlipNormalsTest::flipFaceWinding<UnsignedShort>,
              &FlipNormalsTest::flipFaceWinding<UnsignedInt>,
              &FlipNormalsTest::flipFaceWindingErased<UnsignedByte>,
              &FlipNormalsTest::flipFaceWindingErased<UnsignedShort>,
              &FlipNormalsTest::flipFaceWindingErased<UnsignedInt>,
              &FlipNormalsTest::flipFaceWindingErasedNonContiguous,
              &FlipNormalsTest::flipFaceWindingErasedWrongIndexSize,
              &FlipNormalsTest::flipNormals,

              &FlipNormalsTest::flipNormalsFaceWinding<UnsignedByte>,
              &FlipNormalsTest::flipNormalsFaceWinding<UnsignedShort>,
              &FlipNormalsTest::flipNormalsFaceWinding<UnsignedInt>,
              &FlipNormalsTest::flipNormalsFaceWindingErased<UnsignedByte>,
              &FlipNormalsTest::flipNormalsFaceWindingErased<UnsignedShort>,
              &FlipNormalsTest::flipNormalsFaceWindingErased<UnsignedInt>});
}

void FlipNormalsTest::wrongIndexCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream ss;
    Error redirectError{&ss};

    UnsignedByte indices[2];
    flipFaceWindingInPlace(Containers::stridedArrayView(indices));

    CORRADE_COMPARE(ss.str(), "MeshTools::flipNormals(): index count is not divisible by 3!\n");
}

template<class T> void FlipNormalsTest::flipFaceWinding() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{0, 1, 2, 3, 4, 5};
    flipFaceWindingInPlace(indices);

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({0, 2, 1, 3, 5, 4}),
        TestSuite::Compare::Container);
}

template<class T> void FlipNormalsTest::flipFaceWindingErased() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{0, 1, 2, 3, 4, 5};
    flipFaceWindingInPlace(Containers::arrayCast<2, char>(Containers::stridedArrayView(indices)));

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({0, 2, 1, 3, 5, 4}),
        TestSuite::Compare::Container);
}

void FlipNormalsTest::flipFaceWindingErasedNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char indices[6*4]{};

    std::stringstream out;
    Error redirectError{&out};
    flipFaceWindingInPlace(Containers::StridedArrayView2D<char>{indices, {6, 2}, {4, 2}});
    CORRADE_COMPARE(out.str(),
        "MeshTools::flipFaceWindingInPlace(): second index view dimension is not contiguous\n");
}

void FlipNormalsTest::flipFaceWindingErasedWrongIndexSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char indices[6*3]{};

    std::stringstream out;
    Error redirectError{&out};
    flipFaceWindingInPlace(Containers::StridedArrayView2D<char>{indices, {6, 3}});
    CORRADE_COMPARE(out.str(),
        "MeshTools::flipFaceWindingInPlace(): expected index type size 1, 2 or 4 but got 3\n");
}

void FlipNormalsTest::flipNormals() {
    Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    flipNormalsInPlace(normals);

    CORRADE_COMPARE_AS(Containers::arrayView(normals),
        Containers::arrayView<Vector3>({
            -Vector3::xAxis(), -Vector3::yAxis(), -Vector3::zAxis()
        }), TestSuite::Compare::Container);
}

template<class T> void FlipNormalsTest::flipNormalsFaceWinding() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{0, 1, 2, 3, 4, 5};
    Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    flipNormalsInPlace(indices, normals);

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({0, 2, 1, 3, 5, 4}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(normals),
        Containers::arrayView<Vector3>({
            -Vector3::xAxis(), -Vector3::yAxis(), -Vector3::zAxis()
        }), TestSuite::Compare::Container);
}

template<class T> void FlipNormalsTest::flipNormalsFaceWindingErased() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{0, 1, 2, 3, 4, 5};
    Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    flipNormalsInPlace(
        Containers::arrayCast<2, char>(Containers::stridedArrayView(indices)),
        normals);

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({0, 2, 1, 3, 5, 4}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(normals),
        Containers::arrayView<Vector3>({
            -Vector3::xAxis(), -Vector3::yAxis(), -Vector3::zAxis()
        }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::FlipNormalsTest)
