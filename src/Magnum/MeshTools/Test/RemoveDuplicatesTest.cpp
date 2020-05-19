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

#include "Magnum/Math/Vector2.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct RemoveDuplicatesTest: TestSuite::Tester {
    explicit RemoveDuplicatesTest();

    void removeDuplicates();
    void removeDuplicatesNonContiguous();
    void removeDuplicatesIntoWrongOutputSize();
    template<class T> void removeDuplicatesIndexedInPlace();
    void removeDuplicatesIndexedInPlaceSmallType();
    void removeDuplicatesIndexedInPlaceEmptyIndices();
    void removeDuplicatesIndexedInPlaceEmptyIndicesVertices();
    template<class T> void removeDuplicatesIndexedInPlaceErased();
    void removeDuplicatesIndexedInPlaceErasedNonContiguous();
    void removeDuplicatesIndexedInPlaceErasedWrongIndexSize();

    void removeDuplicatesFuzzyInPlace();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void removeDuplicatesFuzzyStl();
    #endif
    template<class T> void removeDuplicatesFuzzyIndexedInPlace();
    void removeDuplicatesFuzzyIndexedInPlaceSmallType();
    void removeDuplicatesFuzzyIndexedInPlaceEmptyIndices();
    void removeDuplicatesFuzzyIndexedInPlaceEmptyIndicesVertices();
    template<class T> void removeDuplicatesFuzzyIndexedInPlaceErased();
    void removeDuplicatesFuzzyIndexedInPlaceErasedNonContiguous();
    void removeDuplicatesFuzzyIndexedInPlaceErasedWrongIndexSize();

    /* this is additionally regression-tested in PrimitivesIcosphereTest */

    void removeDuplicatesMeshData();
    void removeDuplicatesMeshDataAttributeless();
};

const struct {
    const char* name;
    bool indexed;
} RemoveDuplicatesMeshDataData[] {
    {"", false},
    {"indexed", true}
};

RemoveDuplicatesTest::RemoveDuplicatesTest() {
    addTests({&RemoveDuplicatesTest::removeDuplicates,
              &RemoveDuplicatesTest::removeDuplicatesNonContiguous,
              &RemoveDuplicatesTest::removeDuplicatesIntoWrongOutputSize,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlace<UnsignedByte>,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlace<UnsignedShort>,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlace<UnsignedInt>,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceSmallType,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceEmptyIndices,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceEmptyIndicesVertices,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceErased<UnsignedByte>,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceErased<UnsignedShort>,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceErased<UnsignedInt>,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceErasedNonContiguous,
              &RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceErasedWrongIndexSize,

              &RemoveDuplicatesTest::removeDuplicatesFuzzyInPlace,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &RemoveDuplicatesTest::removeDuplicatesFuzzyStl,
              #endif
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace<UnsignedByte>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace<UnsignedShort>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace<UnsignedInt>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceSmallType,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceEmptyIndices,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceEmptyIndicesVertices,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased<UnsignedByte>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased<UnsignedShort>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased<UnsignedInt>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErasedNonContiguous,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErasedWrongIndexSize});

    addInstancedTests({&RemoveDuplicatesTest::removeDuplicatesMeshData},
        Containers::arraySize(RemoveDuplicatesMeshDataData));

    addTests({&RemoveDuplicatesTest::removeDuplicatesMeshDataAttributeless});
}

void RemoveDuplicatesTest::removeDuplicates() {
    Int data[]{-15, 32, 24, -15, 15, 7541, 24, 32};

    std::pair<Containers::Array<UnsignedInt>, std::size_t> result =
        MeshTools::removeDuplicates(Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE_AS(Containers::arrayView(result.first),
        Containers::arrayView<UnsignedInt>({0, 1, 2, 0, 4, 5, 2, 1}),
        TestSuite::Compare::Container);

    std::pair<Containers::Array<UnsignedInt>, std::size_t> resultInPlace =
        MeshTools::removeDuplicatesInPlace(Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE_AS(Containers::arrayView(resultInPlace.first),
        Containers::arrayView<UnsignedInt>({0, 1, 2, 0, 3, 4, 2, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(resultInPlace.second),
        Containers::arrayView<Int>({-15, 32, 24, 15, 7541}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Int data[8]{};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicates(Containers::arrayCast<2, const char>(Containers::arrayView(data)).every({1, 2}));
    MeshTools::removeDuplicatesInPlace(Containers::arrayCast<2, char>(Containers::arrayView(data)).every({1, 2}));
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicatesInto(): second data view dimension is not contiguous\n"
        "MeshTools::removeDuplicatesInPlaceInto(): second data view dimension is not contiguous\n");
}

void RemoveDuplicatesTest::removeDuplicatesIntoWrongOutputSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Int data[8]{};
    UnsignedInt output[7];

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesInto(
        Containers::arrayCast<2, const char>(Containers::arrayView(data)),
        output);
    MeshTools::removeDuplicatesInPlaceInto(
        Containers::arrayCast<2, char>(Containers::arrayView(data)),
        output);
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicatesInto(): output index array has 7 elements but expected 8\n"
        "MeshTools::removeDuplicatesInPlaceInto(): output index array has 7 elements but expected 8\n");
}

template<class T> void RemoveDuplicatesTest::removeDuplicatesIndexedInPlace() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{3, 2, 0, 1, 7, 6, 4, 2, 5, 0};
    Int data[]{-15, 32, 24, -15, 15, 7541, 24, 32};
    std::size_t count = MeshTools::removeDuplicatesIndexedInPlace(indices,
        Containers::arrayCast<2, char>(Containers::arrayView(data)));

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({0, 2, 0, 1, 1, 2, 3, 2, 4, 0}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Int>({-15, 32, 24, 15, 7541}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceSmallType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream out;
    Error redirectError{&out};

    UnsignedByte indices[1];
    Vector2i data[256]{};
    MeshTools::removeDuplicatesIndexedInPlace(
        Containers::stridedArrayView(indices),
        Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE(out.str(), "MeshTools::removeDuplicatesIndexedInPlace(): a 1-byte index type is too small for 256 vertices\n");
}

void RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceEmptyIndices() {
    Int data[]{-15, 32, 24, -15, 15, 7541, 24, 32};

    std::size_t count = MeshTools::removeDuplicatesIndexedInPlace(
        Containers::StridedArrayView1D<UnsignedInt>{},
        Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Int>({-15, 32, 24, 15, 7541}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceEmptyIndicesVertices() {
    CORRADE_COMPARE(MeshTools::removeDuplicatesIndexedInPlace(
        Containers::StridedArrayView1D<UnsignedInt>{}, {}), 0);
}

template<class T> void RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceErased() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    T indices[]{3, 2, 0, 1, 7, 6, 4, 2, 5, 0};
    Int data[]{-15, 32, 24, -15, 15, 7541, 24, 32};
    std::size_t count = MeshTools::removeDuplicatesIndexedInPlace(
        Containers::arrayCast<2, char>(Containers::arrayView(indices)),
        Containers::arrayCast<2, char>(Containers::arrayView(data)));

    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({0, 2, 0, 1, 1, 2, 3, 2, 4, 0}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Int>({-15, 32, 24, 15, 7541}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceErasedNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char indices[6*4]{};
    Int data[1]{};

    std::stringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesIndexedInPlace(
        Containers::StridedArrayView2D<char>{indices, {6, 2}, {4, 2}},
        Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicatesIndexedInPlace(): second index view dimension is not contiguous\n");
}

void RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceErasedWrongIndexSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char indices[6*3]{};
    Int data[1]{};

    std::stringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesIndexedInPlace(
        Containers::StridedArrayView2D<char>{indices, {6, 3}},
        Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicatesIndexedInPlace(): expected index type size 1, 2 or 4 but got 3\n");
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyInPlace() {
    /* Numbers with distance 1 should be merged, numbers with distance 2 should
       be kept. Testing both even-odd and odd-even sequence to verify that
       half-epsilon translations are applied properly. */
    Vector2i data[]{
        {1, 0},
        {2, 1},
        {0, 4},
        {1, 5}
    };

    std::pair<Containers::Array<UnsignedInt>, std::size_t> result = MeshTools::removeDuplicatesFuzzyInPlace(Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(result.first),
        Containers::arrayView<UnsignedInt>({0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(result.second),
        Containers::arrayView<Vector2i>({{1, 0}, {0, 4}}),
        TestSuite::Compare::Container);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void RemoveDuplicatesTest::removeDuplicatesFuzzyStl() {
    /* Same but with implicit bloat. HEH HEH */
    std::vector<Vector2i> data{
        {1, 0},
        {2, 1},
        {0, 4},
        {1, 5}
    };

    CORRADE_IGNORE_DEPRECATED_PUSH
    const std::vector<UnsignedInt> indices = MeshTools::removeDuplicates(data, 2);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE_AS(indices,
        (std::vector<UnsignedInt>{0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data,
        (std::vector<Vector2i>{{1, 0}, {0, 4}}),
        TestSuite::Compare::Container);
}
#endif

template<class T> void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Same as above, but with an explicit index buffer */
    T indices[]{3, 2, 0, 1, 2, 3};
    Vector2i data[]{
        {1, 0},
        {2, 1},
        {0, 4},
        {1, 5}
    };

    std::size_t count = MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::stridedArrayView(indices),
        Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({1, 1, 0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Vector2i>({{1, 0}, {0, 4}}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceSmallType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream out;
    Error redirectError{&out};

    UnsignedByte indices[1];
    Vector2i data[256]{};
    MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::stridedArrayView(indices),
        Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE(out.str(), "MeshTools::removeDuplicatesFuzzyIndexedInPlace(): a 1-byte index type is too small for 256 vertices\n");
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceEmptyIndices() {
    Vector2i data[]{
        {1, 0},
        {2, 1},
        {0, 4},
        {1, 5}
    };

    std::size_t count = MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::StridedArrayView1D<UnsignedInt>{},
        Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Vector2i>({{1, 0}, {0, 4}}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceEmptyIndicesVertices() {
    CORRADE_COMPARE((MeshTools::removeDuplicatesFuzzyIndexedInPlace<UnsignedInt, Vector2i>({}, {}, 2)), 0);
}

template<class T> void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Same as above, but with an explicit index buffer */
    T indices[]{3, 2, 0, 1, 2, 3};
    Vector2i data[]{
        {1, 0},
        {2, 1},
        {0, 4},
        {1, 5}
    };

    std::size_t count = MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::arrayCast<2, char>(Containers::arrayView(indices)),
        Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<T>({1, 1, 0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Vector2i>({{1, 0}, {0, 4}}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErasedNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char indices[6*4]{};
    Vector2i data[1];

    std::stringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::StridedArrayView2D<char>{indices, {6, 2}, {4, 2}},
        Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicatesFuzzyIndexedInPlace(): second index view dimension is not contiguous\n");
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErasedWrongIndexSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char indices[6*3]{};
    Vector2i data[1];

    std::stringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::StridedArrayView2D<char>{indices, {6, 3}},
        Containers::stridedArrayView(data), 2);
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicatesFuzzyIndexedInPlace(): expected index type size 1, 2 or 4 but got 3\n");
}

void RemoveDuplicatesTest::removeDuplicatesMeshData() {
    auto&& data = RemoveDuplicatesMeshDataData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Deliberately not owned and not interleaved to verify that the function
       will handle this */
    struct Vertex {
        Vector2 positions[10]{
            {1.0f, 2.0f},
            {3.0f, 4.0f},
            {5.0f, 6.0f},
            {7.0f, 8.0f},
            {7.0f, 8.0f},
            {7.0f, 8.0f},
            {9.0f, 10.0f},
            {3.0f, 4.0f},
            {5.0f, 6.0f},
            {5.0f, 6.0f}
        };
        Short data[10][2]{
            {-15, 2},
            {2, 32},
            {24, 2},
            {-15, 2},
            {15, 2},
            {2, 7541},
            {24, 2},
            {2, 32},
            {24, 2},
            {15, 2}
        };
    } vertexData[1];

    const UnsignedShort indexData[]{1, 2, 5, 9, 7, 6, 4, 7, 5, 0, 3, 8, 3};

    Containers::ArrayView<const void> indexView;
    Trade::MeshIndexData indices;
    if(data.indexed) {
        indexView = indexData;
        indices = Trade::MeshIndexData{indexData};
    }

    Trade::MeshData mesh{MeshPrimitive::Lines,
        {}, indexView, indices,
        {}, vertexData, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::arrayView(vertexData->positions)},
            Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
                VertexFormat::ShortNormalized, 2,
                Containers::stridedArrayView(vertexData->data)},
    }};

    Trade::MeshData unique = MeshTools::removeDuplicates(mesh);
    CORRADE_COMPARE(unique.primitive(), MeshPrimitive::Lines);

    CORRADE_VERIFY(unique.isIndexed());
    if(data.indexed) {
        CORRADE_COMPARE(unique.indexCount(), mesh.indexCount());
        {
            CORRADE_EXPECT_FAIL("The function currently doesn't preserve the index type.");
            CORRADE_COMPARE(unique.indexType(), MeshIndexType::UnsignedShort);
        }
        CORRADE_COMPARE(unique.indexType(), MeshIndexType::UnsignedInt);
        CORRADE_COMPARE_AS(unique.indices<UnsignedInt>(),
            Containers::arrayView<UnsignedInt>({1, 2, 5, 7, 1, 6, 4, 1, 5, 0, 3, 2, 3}),
            TestSuite::Compare::Container);
    } else {
        CORRADE_COMPARE(unique.indexCount(), mesh.vertexCount());
        CORRADE_COMPARE(unique.indexType(), MeshIndexType::UnsignedInt);
        CORRADE_COMPARE_AS(unique.indices<UnsignedInt>(),
            Containers::arrayView<UnsignedInt>({0, 1, 2, 3, 4, 5, 6, 1, 2, 7}),
            TestSuite::Compare::Container);
    }

    CORRADE_COMPARE(unique.vertexCount(), 8);
    CORRADE_COMPARE(unique.attributeCount(), 2);
    CORRADE_COMPARE(unique.attributeName(0), Trade::MeshAttribute::Position);
    CORRADE_COMPARE(unique.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(unique.attribute<Vector2>(0),
        Containers::arrayView<Vector2>({
            {1.0f, 2.0f},
            {3.0f, 4.0f},
            {5.0f, 6.0f},
            {7.0f, 8.0f},
            {7.0f, 8.0f},
            {7.0f, 8.0f},
            {9.0f, 10.0f},
            {5.0f, 6.0f}
        }),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(unique.attributeName(1), Trade::meshAttributeCustom(42));
    CORRADE_COMPARE(unique.attributeFormat(1), VertexFormat::ShortNormalized);
    CORRADE_COMPARE(unique.attributeArraySize(1), 2);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Vector2s>(unique.attribute<Short[]>(1))),
        Containers::arrayView<Vector2s>({
            {-15, 2},
            {2, 32},
            {24, 2},
            {-15, 2},
            {15, 2},
            {2, 7541},
            {24, 2},
            {15, 2}
        }),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesMeshDataAttributeless() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicates(Trade::MeshData{MeshPrimitive::Points, 10});
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicates(): can't remove duplicates in an attributeless mesh\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::RemoveDuplicatesTest)
