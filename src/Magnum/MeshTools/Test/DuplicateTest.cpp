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
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct DuplicateTest: TestSuite::Tester {
    explicit DuplicateTest();

    void duplicate();
    void duplicateOutOfBounds();
    void duplicateStl();

    void duplicateInto();
    void duplicateIntoWrongSize();

    template<class T> void duplicateIntoErased();
    void duplicateIntoErasedWrongTypeSize();
    void duplicateIntoErasedNonContiguous();

    template<class T> void duplicateErasedIndicesIntoErased();
    void duplicateErasedIndicesIntoErasedNonContiguous();
    void duplicateErasedIndicesIntoErasedWrongTypeSize();

    template<class T> void duplicateMeshData();
    void duplicateMeshDataNotIndexed();
    void duplicateMeshDataExtra();
    void duplicateMeshDataExtraEmpty();
    void duplicateMeshDataExtraWrongCount();
    void duplicateMeshDataNoAttributes();
};

DuplicateTest::DuplicateTest() {
    addTests({&DuplicateTest::duplicate,
              &DuplicateTest::duplicateOutOfBounds,
              &DuplicateTest::duplicateStl,

              &DuplicateTest::duplicateInto,
              &DuplicateTest::duplicateIntoWrongSize,

              &DuplicateTest::duplicateIntoErased<UnsignedByte>,
              &DuplicateTest::duplicateIntoErased<UnsignedShort>,
              &DuplicateTest::duplicateIntoErased<UnsignedInt>,
              &DuplicateTest::duplicateIntoErasedWrongTypeSize,
              &DuplicateTest::duplicateIntoErasedNonContiguous,

              &DuplicateTest::duplicateErasedIndicesIntoErased<UnsignedByte>,
              &DuplicateTest::duplicateErasedIndicesIntoErased<UnsignedShort>,
              &DuplicateTest::duplicateErasedIndicesIntoErased<UnsignedInt>,
              &DuplicateTest::duplicateErasedIndicesIntoErasedNonContiguous,
              &DuplicateTest::duplicateErasedIndicesIntoErasedWrongTypeSize,

              &DuplicateTest::duplicateMeshData<UnsignedByte>,
              &DuplicateTest::duplicateMeshData<UnsignedShort>,
              &DuplicateTest::duplicateMeshData<UnsignedInt>,
              &DuplicateTest::duplicateMeshDataNotIndexed,
              &DuplicateTest::duplicateMeshDataExtra,
              &DuplicateTest::duplicateMeshDataExtraEmpty,
              &DuplicateTest::duplicateMeshDataExtraWrongCount,
              &DuplicateTest::duplicateMeshDataNoAttributes});
}

void DuplicateTest::duplicate() {
    constexpr UnsignedByte indices[]{1, 1, 0, 3, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};

    CORRADE_COMPARE_AS((MeshTools::duplicate<UnsignedByte, Int>(indices, data)),
        Containers::arrayView<Int>({35, 35, -7, -18, 12, 12}),
        TestSuite::Compare::Container);
}

void DuplicateTest::duplicateOutOfBounds() {
    constexpr UnsignedByte indices[]{1, 1, 0, 4, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};

    std::ostringstream out;
    Error redirectError{&out};

    MeshTools::duplicate<UnsignedByte, Int>(indices, data);
    CORRADE_COMPARE(out.str(),
        "MeshTools::duplicateInto(): index 4 out of bounds for 4 elements\n");
}

void DuplicateTest::duplicateStl() {
    CORRADE_COMPARE(MeshTools::duplicate({1, 1, 0, 3, 2, 2}, std::vector<int>{-7, 35, 12, -18}),
        (std::vector<Int>{35, 35, -7, -18, 12, 12}));
}

void DuplicateTest::duplicateInto() {
    constexpr UnsignedByte indices[]{1, 1, 0, 3, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};
    Int output[6];

    MeshTools::duplicateInto<UnsignedByte, Int>(indices, data, output);
    CORRADE_COMPARE_AS(Containers::arrayView<const Int>(output),
        Containers::arrayView({35, 35, -7, -18, 12, 12}),
        TestSuite::Compare::Container);
}

void DuplicateTest::duplicateIntoWrongSize() {
    constexpr UnsignedByte indices[]{1, 1, 0, 3, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};
    Int output[5];

    std::ostringstream out;
    Error redirectError{&out};

    MeshTools::duplicateInto<UnsignedByte, Int>(indices, data, output);
    CORRADE_COMPARE(out.str(),
        "MeshTools::duplicateInto(): index array and output size don't match, expected 6 but got 5\n");
}

template<class T> void DuplicateTest::duplicateIntoErased() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    constexpr T indices[]{1, 1, 0, 3, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};
    Int output[6];

    MeshTools::duplicateInto(
        Containers::stridedArrayView(indices),
        Containers::arrayCast<2, const char>(Containers::stridedArrayView(data)),
        Containers::arrayCast<2, char>(Containers::stridedArrayView(output)));
    CORRADE_COMPARE_AS(Containers::arrayView<const Int>(output),
        Containers::arrayView({35, 35, -7, -18, 12, 12}),
        TestSuite::Compare::Container);
}

void DuplicateTest::duplicateIntoErasedWrongTypeSize() {
    constexpr UnsignedByte indices[]{1, 1, 0, 3, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};
    Short output[6];

    std::ostringstream out;
    Error redirectError{&out};

    MeshTools::duplicateInto(
        Containers::stridedArrayView(indices),
        Containers::arrayCast<2, const char>(Containers::stridedArrayView(data)),
        Containers::arrayCast<2, char>(Containers::stridedArrayView(output)));
    CORRADE_COMPARE(out.str(),
        "MeshTools::duplicateInto(): input and output type size doesn't match, expected 4 but got 2\n");
}

void DuplicateTest::duplicateIntoErasedNonContiguous() {
    constexpr UnsignedByte indices[]{1, 1, 0, 3, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};
    Short output[6];

    std::ostringstream out;
    Error redirectError{&out};

    MeshTools::duplicateInto(
        Containers::stridedArrayView(indices),
        Containers::arrayCast<2, const char>(Containers::stridedArrayView(data)).every({1, 2}),
        Containers::arrayCast<2, char>(Containers::stridedArrayView(output)));
    CORRADE_COMPARE(out.str(),
        "MeshTools::duplicateInto(): second view dimension is not contiguous\n");
}

template<class T> void DuplicateTest::duplicateErasedIndicesIntoErased() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    constexpr T indices[]{1, 1, 0, 3, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};
    Int output[6];

    MeshTools::duplicateInto(
        Containers::arrayCast<2, const char>(Containers::stridedArrayView(indices)),
        Containers::arrayCast<2, const char>(Containers::stridedArrayView(data)),
        Containers::arrayCast<2, char>(Containers::stridedArrayView(output)));
    CORRADE_COMPARE_AS(Containers::arrayView<const Int>(output),
        Containers::arrayView({35, 35, -7, -18, 12, 12}),
        TestSuite::Compare::Container);
}

void DuplicateTest::duplicateErasedIndicesIntoErasedWrongTypeSize() {
    constexpr char indices[6*3]{};
    constexpr Int data[]{-7, 35, 12, -18};
    Short output[6];

    std::ostringstream out;
    Error redirectError{&out};

    MeshTools::duplicateInto(
        Containers::StridedArrayView2D<const char>{indices, {6, 3}}.every(2),
        Containers::arrayCast<2, const char>(Containers::stridedArrayView(data)),
        Containers::arrayCast<2, char>(Containers::stridedArrayView(output)));
    CORRADE_COMPARE(out.str(),
        "MeshTools::duplicateInto(): expected index type size 1, 2 or 4 but got 3\n");
}

void DuplicateTest::duplicateErasedIndicesIntoErasedNonContiguous() {
    constexpr char indices[3*6]{};
    constexpr Int data[]{-7, 35, 12, -18};
    Short output[6];

    std::ostringstream out;
    Error redirectError{&out};

    MeshTools::duplicateInto(
        Containers::StridedArrayView2D<const char>{indices, {3, 3}, {6, 2}},
        Containers::arrayCast<2, const char>(Containers::stridedArrayView(data)).every({1, 2}),
        Containers::arrayCast<2, char>(Containers::stridedArrayView(output)));
    CORRADE_COMPARE(out.str(),
        "MeshTools::duplicateInto(): second index view dimension is not contiguous\n");
}

template<class T> void DuplicateTest::duplicateMeshData() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{0, 1, 2, 2, 1, 0};
    struct {
        Vector2 positions[3];
        Vector3 normals[3];
    } vertexData{
        {{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}},
        {Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()}
    };
    Trade::MeshData data{MeshPrimitive::TriangleFan,
        {}, indices, Trade::MeshIndexData{indices},
        {}, Containers::arrayView(&vertexData, 1), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,    Containers::arrayView(vertexData.positions)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertexData.normals)}
        }};

    Trade::MeshData duplicated = MeshTools::duplicate(data);
    CORRADE_VERIFY(MeshTools::isInterleaved(duplicated));
    CORRADE_COMPARE(duplicated.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!duplicated.isIndexed());
    CORRADE_COMPARE(duplicated.vertexCount(), 6);
    CORRADE_COMPARE(duplicated.attributeCount(), 2);
    CORRADE_COMPARE_AS(duplicated.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f},
            {1.0f, -0.5f}, {0.87f, 1.1f}, {1.3f, 0.3f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(duplicated.attribute<Vector3>(Trade::MeshAttribute::Normal),
        Containers::arrayView<Vector3>({
            Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis(),
            Vector3::zAxis(), Vector3::yAxis(), Vector3::xAxis()
        }), TestSuite::Compare::Container);
}

void DuplicateTest::duplicateMeshDataNotIndexed() {
    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::duplicate(Trade::MeshData{MeshPrimitive::Points, 0});
    CORRADE_COMPARE(out.str(), "MeshTools::duplicate(): mesh data not indexed\n");
}

void DuplicateTest::duplicateMeshDataExtra() {
    UnsignedByte indices[]{0, 1, 2, 2, 1, 0};
    Vector2 positions[]{{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}};
    Trade::MeshData data{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices},
        {}, positions, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
        }};

    const Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()};
    Trade::MeshData duplicated = MeshTools::duplicate(data, {
        Trade::MeshAttributeData{4},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(normals)}
    });
    CORRADE_VERIFY(MeshTools::isInterleaved(duplicated));
    CORRADE_COMPARE(duplicated.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(!duplicated.isIndexed());
    CORRADE_COMPARE(duplicated.vertexCount(), 6);
    CORRADE_COMPARE(duplicated.attributeCount(), 2);
    CORRADE_COMPARE_AS(duplicated.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f},
            {1.0f, -0.5f}, {0.87f, 1.1f}, {1.3f, 0.3f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(duplicated.attribute<Vector3>(Trade::MeshAttribute::Normal),
        Containers::arrayView<Vector3>({
            Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis(),
            Vector3::zAxis(), Vector3::yAxis(), Vector3::xAxis()
        }), TestSuite::Compare::Container);
}

void DuplicateTest::duplicateMeshDataExtraEmpty() {
    UnsignedByte indices[]{0, 1, 2, 2, 1, 0};
    Vector2 positions[]{{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}};
    Trade::MeshData data{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices},
        {}, positions, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
        }};

    Trade::MeshData duplicated = MeshTools::duplicate(data, {
        Trade::MeshAttributeData{4},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
            VertexFormat::Vector3, nullptr}
    });
    CORRADE_COMPARE(duplicated.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(!duplicated.isIndexed());
    CORRADE_COMPARE(duplicated.vertexCount(), 6);
    CORRADE_COMPARE(duplicated.attributeCount(), 2);
    CORRADE_COMPARE_AS(duplicated.attribute<Vector2>(Trade::MeshAttribute::Position),
        Containers::arrayView<Vector2>({
            {1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f},
            {1.0f, -0.5f}, {0.87f, 1.1f}, {1.3f, 0.3f}
        }), TestSuite::Compare::Container);
    CORRADE_COMPARE(duplicated.attributeStride(Trade::MeshAttribute::Normal), 24);
    CORRADE_COMPARE(duplicated.attributeOffset(Trade::MeshAttribute::Normal), 12);
}

void DuplicateTest::duplicateMeshDataExtraWrongCount() {
    UnsignedByte indices[]{0, 1, 2, 2, 1, 0};
    Vector2 positions[]{{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}};
    Trade::MeshData data{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices},
        {}, positions, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::arrayView(positions)}
        }};
    const Vector3 normals[]{Vector3::xAxis(), Vector3::yAxis()};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::duplicate(data, {
        Trade::MeshAttributeData{10},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(normals)}
    });
    CORRADE_COMPARE(out.str(), "MeshTools::duplicate(): extra attribute 1 expected to have 3 items but got 2\n");
}

void DuplicateTest::duplicateMeshDataNoAttributes() {
    UnsignedByte indices[]{0, 1, 2, 2, 1, 0};
    Trade::MeshData data{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}};

    Trade::MeshData duplicated = MeshTools::duplicate(data, {});
    CORRADE_COMPARE(duplicated.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(!duplicated.isIndexed());
    CORRADE_COMPARE(duplicated.vertexCount(), 6);
    CORRADE_COMPARE(duplicated.attributeCount(), 0);
    CORRADE_VERIFY(!duplicated.vertexData());
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::DuplicateTest)
