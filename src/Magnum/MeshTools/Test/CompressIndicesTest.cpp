/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/Endianness.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/CompressIndices.h"
#include "Magnum/Trade/MeshData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <tuple>
#include <vector>
#endif

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct CompressIndicesTest: TestSuite::Tester {
    explicit CompressIndicesTest();

    template<class T> void compressUnsignedByte();
    template<class T> void compressUnsignedShort();
    template<class T> void compressUnsignedInt();
    void compressUnsignedByteInflateToShort();
    void compressOffset();
    template<class T> void compressOffsetNegative();
    /* No compressErased(), as that's tested in the templates above */
    void compressErasedNonContiguous();
    void compressErasedWrongIndexSize();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void compressDeprecated();
    #endif

    template<class T> void compressMeshData();
    void compressMeshDataMove();
    void compressMeshDataNonIndexed();
    void compressMeshDataImplementationSpecificIndexType();
    void compressMeshDataImplementationSpecificAtLeastIndexType();

    #ifdef MAGNUM_BUILD_DEPRECATED
    void compressAsShort();
    #endif
};

CompressIndicesTest::CompressIndicesTest() {
    addTests({&CompressIndicesTest::compressUnsignedByte<UnsignedByte>,
              &CompressIndicesTest::compressUnsignedByte<UnsignedShort>,
              &CompressIndicesTest::compressUnsignedByte<UnsignedInt>,
              &CompressIndicesTest::compressUnsignedShort<UnsignedShort>,
              &CompressIndicesTest::compressUnsignedShort<UnsignedInt>,
              &CompressIndicesTest::compressUnsignedInt<UnsignedInt>,
              &CompressIndicesTest::compressUnsignedByteInflateToShort,
              &CompressIndicesTest::compressOffset,
              &CompressIndicesTest::compressOffsetNegative<UnsignedByte>,
              &CompressIndicesTest::compressOffsetNegative<UnsignedShort>,
              &CompressIndicesTest::compressOffsetNegative<UnsignedInt>,
              &CompressIndicesTest::compressErasedNonContiguous,
              &CompressIndicesTest::compressErasedWrongIndexSize,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &CompressIndicesTest::compressDeprecated,
              #endif

              &CompressIndicesTest::compressMeshData<UnsignedByte>,
              &CompressIndicesTest::compressMeshData<UnsignedShort>,
              &CompressIndicesTest::compressMeshData<UnsignedInt>,
              &CompressIndicesTest::compressMeshDataMove,
              &CompressIndicesTest::compressMeshDataNonIndexed,
              &CompressIndicesTest::compressMeshDataImplementationSpecificIndexType,
              &CompressIndicesTest::compressMeshDataImplementationSpecificAtLeastIndexType,

              #ifdef MAGNUM_BUILD_DEPRECATED
              &CompressIndicesTest::compressAsShort
              #endif
              });
}

template<class T> void CompressIndicesTest::compressUnsignedByte() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const T indices[]{1, 2, 3, 0, 4};
    /* By default it has 16-byte type as minimum, override */
    Containers::Pair<Containers::Array<char>, MeshIndexType> out =
        compressIndices(indices, MeshIndexType::UnsignedByte);

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedByte);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(out.first()),
        Containers::arrayView<UnsignedByte>({1, 2, 3, 0, 4}),
        TestSuite::Compare::Container);

    /* Test the type-erased variant as well */
    out = compressIndices(Containers::arrayCast<2, const char>(Containers::stridedArrayView(indices)), MeshIndexType::UnsignedByte);

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedByte);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(out.first()),
        Containers::arrayView<UnsignedByte>({1, 2, 3, 0, 4}),
        TestSuite::Compare::Container);
}

template<class T> void CompressIndicesTest::compressUnsignedShort() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const T indices[]{1, 256, 0, 5};
    Containers::Pair<Containers::Array<char>, MeshIndexType> out = compressIndices(indices);

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(out.first()),
        Containers::arrayView<UnsignedShort>({1, 256, 0, 5}),
        TestSuite::Compare::Container);

    /* Test the type-erased variant as well */
    out = compressIndices(Containers::arrayCast<2, const char>(Containers::stridedArrayView(indices)));

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(out.first()),
        Containers::arrayView<UnsignedShort>({1, 256, 0, 5}),
        TestSuite::Compare::Container);
}

template<class T> void CompressIndicesTest::compressUnsignedInt() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const T indices[]{65536, 3, 2};
    Containers::Pair<Containers::Array<char>, MeshIndexType> out = compressIndices(indices);

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedInt>(out.first()),
        Containers::arrayView<UnsignedInt>({65536, 3, 2}),
        TestSuite::Compare::Container);

    /* Test the type-erased variant as well */
    out = compressIndices(Containers::arrayCast<2, const char>(Containers::stridedArrayView(indices)));

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedInt>(out.first()),
        Containers::arrayView<UnsignedInt>({65536, 3, 2}),
        TestSuite::Compare::Container);
}

void CompressIndicesTest::compressUnsignedByteInflateToShort() {
    const UnsignedByte indices[]{1, 2, 3, 0, 4};
    /* That's the default */
    Containers::Pair<Containers::Array<char>, MeshIndexType> out = compressIndices(indices);

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(out.first()),
        Containers::arrayView<UnsignedShort>({1, 2, 3, 0, 4}),
        TestSuite::Compare::Container);
}

void CompressIndicesTest::compressOffset() {
    const UnsignedInt indices[]{75000 + 1, 75000 + 256, 75000 + 0, 75000 + 5};
    Containers::Pair<Containers::Array<char>, MeshIndexType> out = compressIndices(indices, 75000);

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(out.first()),
        Containers::arrayView<UnsignedShort>({1, 256, 0, 5}),
        TestSuite::Compare::Container);

    /* Test the type-erased variant as well */
    out = compressIndices(Containers::arrayCast<2, const char>(Containers::stridedArrayView(indices)), 75000);

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(out.first()),
        Containers::arrayView<UnsignedShort>({1, 256, 0, 5}),
        TestSuite::Compare::Container);
}

template<class T> void CompressIndicesTest::compressOffsetNegative() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    const T indices[]{1, 255, 0, 5};
    Containers::Pair<Containers::Array<char>, MeshIndexType> out = compressIndices(indices, -75000);

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedInt>(out.first()),
        Containers::arrayView<UnsignedInt>({75000 + 1, 75000 + 255, 75000 + 0, 75000 + 5}),
        TestSuite::Compare::Container);

    /* Test the type-erased variant as well */
    out = compressIndices(Containers::arrayCast<2, const char>(Containers::stridedArrayView(indices)), -75000);

    CORRADE_COMPARE(out.second(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedInt>(out.first()),
        Containers::arrayView<UnsignedInt>({75000 + 1, 75000 + 255, 75000 + 0, 75000 + 5}),
        TestSuite::Compare::Container);
}

void CompressIndicesTest::compressErasedNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const char indices[6*4]{};

    Containers::String out;
    Error redirectError{&out};
    compressIndices(Containers::StridedArrayView2D<const char>{indices, {6, 2}, {4, 2}});
    CORRADE_COMPARE(out,
        "MeshTools::compressIndices(): second view dimension is not contiguous\n");
}

void CompressIndicesTest::compressErasedWrongIndexSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const char indices[6*3]{};

    Containers::String out;
    Error redirectError{&out};
    compressIndices(Containers::StridedArrayView2D<const char>{indices, {6, 3}});
    CORRADE_COMPARE(out,
        "MeshTools::compressIndices(): expected index type size 1, 2 or 4 but got 3\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void CompressIndicesTest::compressDeprecated() {
    Containers::Array<char> data;
    MeshIndexType type;
    UnsignedInt start, end;
    CORRADE_IGNORE_DEPRECATED_PUSH
    std::tie(data, type, start, end) = compressIndices(
        std::vector<UnsignedInt>{1, 256, 0, 5});
    CORRADE_IGNORE_DEPRECATED_POP

    CORRADE_COMPARE(start, 0);
    CORRADE_COMPARE(end, 256);
    CORRADE_COMPARE(type, MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedShort>(data),
        Containers::arrayView<UnsignedShort>({1, 256, 0, 5}),
        TestSuite::Compare::Container);
}
#endif

template<class T> void CompressIndicesTest::compressMeshData() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    struct {
        Float data[103][2];
        Vector3 normals[103];
    } vertexData{};
    vertexData.data[100][0] = 1.3f;
    vertexData.data[100][1] = 0.3f;
    vertexData.data[101][0] = 0.87f;
    vertexData.data[101][1] = 1.1f;
    vertexData.data[102][0] = 1.0f;
    vertexData.data[102][1] = -0.5f;
    vertexData.normals[100] = Vector3::xAxis();
    vertexData.normals[101] = Vector3::yAxis();
    vertexData.normals[102] = Vector3::zAxis();

    T indices[] = {102, 101, 100, 101, 102};
    Trade::MeshData data{MeshPrimitive::TriangleFan,
        {}, indices, Trade::MeshIndexData{indices},
        {}, Containers::arrayView(&vertexData, 1), {
            Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
                /* Array attribute to verify it's correctly propagated */
                VertexFormat::Float, Containers::arrayView(vertexData.data), 2},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, Containers::arrayView(vertexData.normals)}
        }};
    CORRADE_COMPARE(data.vertexCount(), 103);
    CORRADE_COMPARE(data.attributeOffset(0), 0);
    CORRADE_COMPARE(data.attributeOffset(1), 103*sizeof(Vector2));

    Trade::MeshData compressed = compressIndices(data);
    CORRADE_COMPARE(compressed.indexCount(), 5);
    CORRADE_COMPARE(compressed.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(compressed.indices<UnsignedShort>(),
        Containers::arrayView<UnsignedShort>({2, 1, 0, 1, 2}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(compressed.vertexCount(), 3);

    CORRADE_COMPARE(compressed.attributeName(0), Trade::meshAttributeCustom(42));
    CORRADE_COMPARE(compressed.attributeFormat(0), VertexFormat::Float);
    CORRADE_COMPARE(compressed.attributeArraySize(0), 2);
    CORRADE_COMPARE(compressed.attributeOffset(0), 100*sizeof(Vector2));
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Vector2>(compressed.attribute<Float[]>(0))),
        Containers::arrayView<Vector2>({{1.3f, 0.3f}, {0.87f, 1.1f}, {1.0f, -0.5f}}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(compressed.attributeName(1), Trade::MeshAttribute::Normal);
    CORRADE_COMPARE(compressed.attributeFormat(1), VertexFormat::Vector3);
    CORRADE_COMPARE(compressed.attributeOffset(1), 103*sizeof(Vector2) + 100*sizeof(Vector3));
    CORRADE_COMPARE_AS(compressed.attribute<Vector3>(1),
        Containers::arrayView<Vector3>({Vector3::xAxis(), Vector3::yAxis(), Vector3::zAxis()}),
        TestSuite::Compare::Container);
}

void CompressIndicesTest::compressMeshDataMove() {
    Containers::Array<char> vertexData{103*24};
    Containers::StridedArrayView1D<Vector2> positionView{vertexData,
        reinterpret_cast<Vector2*>(vertexData.data()), 103, 8};
    Containers::StridedArrayView1D<Vector3> normalView{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + 103*sizeof(Vector2)), 103, 12};
    UnsignedInt indices[] = {102, 101, 100, 101, 102};
    Trade::MeshData data{MeshPrimitive::TriangleFan,
        {}, indices, Trade::MeshIndexData{indices},
        Utility::move(vertexData), {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, positionView},
            Trade::MeshAttributeData{Trade::MeshAttribute::Normal, normalView}
        }};
    CORRADE_COMPARE(data.vertexCount(), 103);
    CORRADE_COMPARE(data.attributeOffset(0), 0);
    CORRADE_COMPARE(data.attributeOffset(1), 103*sizeof(Vector2));

    Trade::MeshData compressed = compressIndices(Utility::move(data));
    CORRADE_COMPARE(compressed.indexCount(), 5);
    CORRADE_COMPARE(compressed.indexType(), MeshIndexType::UnsignedShort);
    CORRADE_COMPARE_AS(compressed.indices<UnsignedShort>(),
        Containers::arrayView<UnsignedShort>({2, 1, 0, 1, 2}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(compressed.vertexCount(), 3);
    CORRADE_COMPARE(compressed.attributeOffset(0), 100*sizeof(Vector2));
    CORRADE_COMPARE(compressed.attributeOffset(1), 103*sizeof(Vector2) + 100*sizeof(Vector3));
    /* The vertex data should be moved, not copied */
    CORRADE_VERIFY(compressed.vertexData().data() == positionView.data());
}

void CompressIndicesTest::compressMeshDataNonIndexed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::TriangleFan, 5};

    /* Test both r-value and l-value overload */
    Containers::String out;
    Error redirectError{&out};
    compressIndices(mesh);
    compressIndices(Utility::move(mesh));
    CORRADE_COMPARE(out,
        "MeshTools::compressIndices(): mesh data not indexed\n"
        "MeshTools::compressIndices(): mesh data not indexed\n");
}

void CompressIndicesTest::compressMeshDataImplementationSpecificIndexType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Trade::MeshData mesh{MeshPrimitive::Points,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<const void>{}}, 1};

    /* Test both r-value and l-value overload */
    Containers::String out;
    Error redirectError{&out};
    compressIndices(mesh);
    compressIndices(Utility::move(mesh));
    CORRADE_COMPARE(out,
        "MeshTools::compressIndices(): mesh has an implementation-specific index type 0xcaca\n"
        "MeshTools::compressIndices(): mesh has an implementation-specific index type 0xcaca\n");
}

void CompressIndicesTest::compressMeshDataImplementationSpecificAtLeastIndexType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    UnsignedInt indices[5]{};
    Trade::MeshData mesh{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{indices}, 1};

    Containers::String out;
    Error redirectError{&out};
    compressIndices(mesh, meshIndexTypeWrap(0xcaca));
    CORRADE_COMPARE(out,
        "MeshTools::compressIndices(): can't compress to an implementation-specific index type 0xcaca\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void CompressIndicesTest::compressAsShort() {
    CORRADE_SKIP_IF_NO_ASSERT();

    CORRADE_IGNORE_DEPRECATED_PUSH
    CORRADE_COMPARE_AS(compressIndicesAs<UnsignedShort>({123, 456}),
        Containers::arrayView<UnsignedShort>({123, 456}),
        TestSuite::Compare::Container);

    Containers::String out;
    Error redirectError{&out};
    compressIndicesAs<UnsignedShort>({65536});
    CORRADE_COMPARE(out, "MeshTools::compressIndicesAs(): type too small to represent value 65536\n");
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CompressIndicesTest)
