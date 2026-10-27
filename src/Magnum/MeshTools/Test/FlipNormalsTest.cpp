/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/TestSuite/Compare/String.h>

#include "Magnum/Math/Vector4.h"
#include "Magnum/MeshTools/FlipNormals.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct FlipNormalsTest: TestSuite::Tester {
    explicit FlipNormalsTest();

    void flipFaceWindingWrongIndexCount();
    template<class T> void flipFaceWinding();
    template<class T> void flipFaceWindingErased();
    void flipFaceWindingErasedNonContiguous();
    void flipNormals();

    template<class T> void flipNormalsFaceWinding();
    template<class T> void flipNormalsFaceWindingErased();
    /* Delegates into type-erased flipFaceWinding(), so testing asserts again
       isn't needed */
};

FlipNormalsTest::FlipNormalsTest() {
    addTests({&FlipNormalsTest::flipFaceWindingWrongIndexCount,
              &FlipNormalsTest::flipFaceWinding<UnsignedByte>,
              &FlipNormalsTest::flipFaceWinding<UnsignedShort>,
              &FlipNormalsTest::flipFaceWinding<UnsignedInt>,
              &FlipNormalsTest::flipFaceWindingErased<UnsignedByte>,
              &FlipNormalsTest::flipFaceWindingErased<UnsignedShort>,
              &FlipNormalsTest::flipFaceWindingErased<UnsignedInt>,
              &FlipNormalsTest::flipFaceWindingErased<Vector3us>,
              &FlipNormalsTest::flipFaceWindingErased<Vector4ui>,
              &FlipNormalsTest::flipFaceWindingErasedNonContiguous,
              &FlipNormalsTest::flipNormals,

              &FlipNormalsTest::flipNormalsFaceWinding<UnsignedByte>,
              &FlipNormalsTest::flipNormalsFaceWinding<UnsignedShort>,
              &FlipNormalsTest::flipNormalsFaceWinding<UnsignedInt>,
              &FlipNormalsTest::flipNormalsFaceWindingErased<UnsignedByte>,
              &FlipNormalsTest::flipNormalsFaceWindingErased<UnsignedShort>,
              &FlipNormalsTest::flipNormalsFaceWindingErased<UnsignedInt>});
}

void FlipNormalsTest::flipFaceWindingWrongIndexCount() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Passing an empty view is okay */
    flipFaceWindingInPlace(Containers::ArrayView<UnsignedShort>{});

    UnsignedByte indices[7];
    Vector3b attribute[8];

    Containers::String out;
    Error redirectError{&out};
    flipFaceWindingInPlace(Containers::stridedArrayView(indices));
    flipNormalsInPlace(Containers::stridedArrayView(indices), {});
    flipFaceWindingInPlace(Containers::arrayCast<2, char>(Containers::stridedArrayView(attribute)));
    CORRADE_COMPARE_AS(out,
        "MeshTools::flipFaceWindingInPlace(): expected index count to be divisible by 3 but got 7\n"
        /* flipNormalsInPlace() delegates to flipFaceWindingInPlace() */
        "MeshTools::flipFaceWindingInPlace(): expected index count to be divisible by 3 but got 7\n"
        /* The fallback byte-by-byte flipping code has its own assert */
        "MeshTools::flipFaceWindingInPlace(): expected index count to be divisible by 3 but got 8\n",
        TestSuite::Compare::String);
}

template<class T> void FlipNormalsTest::flipFaceWinding() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{0, 1, 2, 3, 4, 5};
    flipFaceWindingInPlace(indices);

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({0, 2, 1, 3, 5, 4}),
        TestSuite::Compare::Container);
}

template<class> struct IndexTraits;
template<> struct IndexTraits<UnsignedByte> {
    static const char* name() { return "UnsignedByte"; }
    static UnsignedByte multiplier() { return 47; };
};
template<> struct IndexTraits<UnsignedShort> {
    static const char* name() { return "UnsignedShort"; }
    static UnsignedShort multiplier() { return 13103; };
};
template<> struct IndexTraits<UnsignedInt> {
    static const char* name() { return "UnsignedInt"; }
    static UnsignedInt multiplier() { return 858993433; };
};
template<> struct IndexTraits<Vector3us> {
    static const char* name() { return "Vector3us"; }
    static Vector3us multiplier() { return {13103, 13099, 13001}; }
};
template<> struct IndexTraits<Vector4ui> {
    static const char* name() { return "Vector4ui"; }
    static Vector4ui multiplier() { return {858993433, 286331141, 1228889, 12413}; }
};

template<class T> void FlipNormalsTest::flipFaceWindingErased() {
    setTestCaseTemplateName(IndexTraits<T>::name());

    T indices[]{
        T(0*IndexTraits<T>::multiplier()),
        T(1*IndexTraits<T>::multiplier()),
        T(2*IndexTraits<T>::multiplier()),
        T(3*IndexTraits<T>::multiplier()),
        T(4*IndexTraits<T>::multiplier()),
        T(5*IndexTraits<T>::multiplier()),
    };
    flipFaceWindingInPlace(Containers::arrayCast<2, char>(Containers::stridedArrayView(indices)));

    CORRADE_COMPARE_AS(Containers::arrayView(indices), Containers::arrayView({
        T(0*IndexTraits<T>::multiplier()),
        T(2*IndexTraits<T>::multiplier()),
        T(1*IndexTraits<T>::multiplier()),
        T(3*IndexTraits<T>::multiplier()),
        T(5*IndexTraits<T>::multiplier()),
        T(4*IndexTraits<T>::multiplier()),
    }), TestSuite::Compare::Container);
}

void FlipNormalsTest::flipFaceWindingErasedNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char indices[6*4]{};

    Containers::String out;
    Error redirectError{&out};
    flipFaceWindingInPlace(Containers::StridedArrayView2D<char>{indices, {6, 2}, {4, 2}});
    CORRADE_COMPARE(out,
        "MeshTools::flipFaceWindingInPlace(): second index view dimension is not contiguous\n");
}

void FlipNormalsTest::flipNormals() {
    Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    flipNormalsInPlace(normals);

    CORRADE_COMPARE_AS(Containers::arrayView(normals), Containers::arrayView({
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
    CORRADE_COMPARE_AS(Containers::arrayView(normals), Containers::arrayView({
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
    CORRADE_COMPARE_AS(Containers::arrayView(normals), Containers::arrayView({
        -Vector3::xAxis(), -Vector3::yAxis(), -Vector3::zAxis()
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::FlipNormalsTest)
