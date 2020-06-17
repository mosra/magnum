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

#include <algorithm>
#include <random>
#include <sstream>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct RemoveDuplicatesTest: TestSuite::Tester {
    explicit RemoveDuplicatesTest();

    /* These test also the InPlace variant */
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

    template<class T> void removeDuplicatesFuzzyInPlaceOneDimension();
    template<class T> void removeDuplicatesFuzzyInPlaceMoreDimensions();
    template<class T> void removeDuplicatesFuzzyInPlaceInto();
    void removeDuplicatesFuzzyInPlaceIntoWrongOutputSize();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void removeDuplicatesFuzzyStl();
    #endif
    template<class IndexType, class T> void removeDuplicatesFuzzyIndexedInPlace();
    void removeDuplicatesFuzzyIndexedInPlaceSmallType();
    void removeDuplicatesFuzzyIndexedInPlaceEmptyIndices();
    void removeDuplicatesFuzzyIndexedInPlaceEmptyIndicesVertices();
    template<class IndexType, class T> void removeDuplicatesFuzzyIndexedInPlaceErased();
    void removeDuplicatesFuzzyIndexedInPlaceErasedNonContiguous();
    void removeDuplicatesFuzzyIndexedInPlaceErasedWrongIndexSize();

    /* this is additionally regression-tested in PrimitivesIcosphereTest */

    void removeDuplicatesMeshData();
    void removeDuplicatesMeshDataAttributeless();

    void removeDuplicatesMeshDataFuzzy();
    void removeDuplicatesMeshDataFuzzyDouble();
    void removeDuplicatesMeshDataFuzzyAttributeless();
    void removeDuplicatesMeshDataFuzzyImplementationSpecific();

    void soakTest();
    void soakTestFuzzy();

    void benchmark();
    void benchmarkFuzzy();
};

const struct {
    const char* name;
    bool indexed;
} RemoveDuplicatesMeshDataData[] {
    {"", false},
    {"indexed", true}
};

const struct {
    const char* name;
    Containers::Array<Trade::MeshAttributeData> attributes;
    Float offset, scale, epsilon;
    UnsignedInt vertexCount;
    bool indexed;
} RemoveDuplicatesMeshDataFuzzyData[] {
    {"position, normal", Containers::array({
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector3, 0, 10, 6*sizeof(Float)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
            VertexFormat::Vector3, 3*sizeof(Float), 10, 6*sizeof(Float)}
    }), 0.0f, 1.0f, Math::TypeTraits<Float>::epsilon(), 7, false},
    {"position, normal, epsilon 0", Containers::array({
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector3, 0, 10, 6*sizeof(Float)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
            VertexFormat::Vector3, 3*sizeof(Float), 10, 6*sizeof(Float)}
        /* Only the bit-exact value gets removed */
    }), 0.0f, 1.0f,
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        0.0f
        #else
        /* Otherwise I get RuntimeError: float unrepresentable in integer range
           on Travis (1.38.44) but not locally (1.38.38) */
        Math::TypeTraits<Float>::epsilon()/10
        #endif
        , 9, false},
    {"position, normal, indexed", Containers::array({
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector3, 0, 10, 6*sizeof(Float)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
            VertexFormat::Vector3, 3*sizeof(Float), 10, 6*sizeof(Float)}
    }), 0.0f, 1.0f, Math::TypeTraits<Float>::epsilon(), 7, true},
    {"custom mat3x2, offset 100",Containers::array({
        Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
            VertexFormat::Matrix3x2, 0, 10, 6*sizeof(Float)}
    }), 100.0f, 1.0f, Math::TypeTraits<Float>::epsilon(), 7, false},
    {"position + custom float[3], offset 100, scale 10, indexed",Containers::array({
        Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
            VertexFormat::Float, 0, 10, 6*sizeof(Float), 3},
        Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            VertexFormat::Vector3, 3*sizeof(Float), 10, 6*sizeof(Float)}
    }), 100.0f, 10.0f, Math::TypeTraits<Float>::epsilon(), 7, true},
    {"normal. bitangent, scale 2", Containers::array({
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
            VertexFormat::Vector3, 0, 10, 6*sizeof(Float)},
        Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent,
            VertexFormat::Vector3, 3*sizeof(Float), 10, 6*sizeof(Float)}
        /* Should still fit into the epsilon as the range is [-1, 1] */
    }), 0.0f, 2.0f, Math::TypeTraits<Float>::epsilon(), 7, false},
    {"color, texcoord, scale 10", Containers::array({
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            VertexFormat::Vector4, 0, 10, 6*sizeof(Float)},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            VertexFormat::Vector2, 4*sizeof(Float), 10, 6*sizeof(Float)}
        /* Should not fit into the epsilon, only the bit-exact value gets
           removed */
    }), 0.0f, 10.0f, Math::TypeTraits<Float>::epsilon(), 9, true},
    {"color, texcoord, scale 10, epsilon *10",Containers::array({
        Trade::MeshAttributeData{Trade::MeshAttribute::Color,
            VertexFormat::Vector4, 0, 10, 6*sizeof(Float)},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            VertexFormat::Vector2, 4*sizeof(Float), 10, 6*sizeof(Float)}
        /* Fit into the epsilon again */
    }), 0.0f, 10.0f, 10.0f*Math::TypeTraits<Float>::epsilon(), 7, false}
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

              &RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceOneDimension<Float>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceOneDimension<Double>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceMoreDimensions<Float>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceMoreDimensions<Double>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceInto<Float>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceInto<Double>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceIntoWrongOutputSize,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &RemoveDuplicatesTest::removeDuplicatesFuzzyStl,
              #endif
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace<UnsignedByte, Float>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace<UnsignedByte, Double>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace<UnsignedShort, Float>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace<UnsignedShort, Double>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace<UnsignedInt, Float>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace<UnsignedInt, Double>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceSmallType,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceEmptyIndices,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceEmptyIndicesVertices,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased<UnsignedByte, Float>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased<UnsignedByte, Double>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased<UnsignedShort, Float>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased<UnsignedShort, Double>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased<UnsignedInt, Float>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased<UnsignedInt, Double>,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErasedNonContiguous,
              &RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErasedWrongIndexSize});

    addInstancedTests({&RemoveDuplicatesTest::removeDuplicatesMeshData},
        Containers::arraySize(RemoveDuplicatesMeshDataData));

    addTests({&RemoveDuplicatesTest::removeDuplicatesMeshDataAttributeless});

    addInstancedTests({&RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzy},
        Containers::arraySize(RemoveDuplicatesMeshDataFuzzyData));

    addTests({&RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyDouble,

              &RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyAttributeless,
              &RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyImplementationSpecific});

    addRepeatedTests({&RemoveDuplicatesTest::soakTest,
                      &RemoveDuplicatesTest::soakTestFuzzy}, 10);

    addBenchmarks({&RemoveDuplicatesTest::benchmark,
                   &RemoveDuplicatesTest::benchmarkFuzzy}, 10);
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

template<class T> void RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceOneDimension() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Numbers with distance <=1 should be merged. In the first iteration item
       2 gets collapsed into item 1, in the second iteration item 3 gets
       collapsed into item 1, reducing to 2 items in total. */
    T data[]{
        T(1.0), /* bucket 0 in 1st iteration, bucket 1 in 2nd */
        T(2.9), /* bucket 2 in 1st iteration, bucket 3 in 2nd */
        T(0.0), /* bucket 0 in 1st iteration, bucket 0 in 2nd */
        T(3.4)  /* bucket 3 in 1st iteration, bucket 3 in 2nd */
    };

    std::pair<Containers::Array<UnsignedInt>, std::size_t> result =
        MeshTools::removeDuplicatesFuzzyInPlace(
            Containers::arrayCast<2, T>(Containers::stridedArrayView(data)),
            T(1.00001));
    CORRADE_COMPARE_AS(Containers::arrayView(result.first),
        Containers::arrayView<UnsignedInt>({0, 1, 0, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(result.second),
        (Containers::arrayView<T>({T(1.0), T(2.9)})),
        TestSuite::Compare::Container);
}

template<class T> void RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceMoreDimensions() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Numbers with distance 1 should be merged, numbers with distance 2 should
       be kept. Testing both even-odd and odd-even sequence to verify that
       half-epsilon translations are applied properly. */
    Math::Vector2<T> data[]{
        {T(1.0), T(0.0)},
        {T(2.0), T(1.0)},
        {T(0.0), T(4.0)},
        {T(1.0), T(5.0)}
    };

    std::pair<Containers::Array<UnsignedInt>, std::size_t> result =
        MeshTools::removeDuplicatesFuzzyInPlace(
            Containers::arrayCast<2, T>(Containers::stridedArrayView(data)),
            T(2.0));
    CORRADE_COMPARE_AS(Containers::arrayView(result.first),
        Containers::arrayView<UnsignedInt>({0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(result.second),
        Containers::arrayView<Math::Vector2<T>>({{T(1.0), T(0.0)}, {T(0.0), T(4.0)}}),
        TestSuite::Compare::Container);
}

template<class T> void RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceInto() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    /* Same as above, but using the Into variant */
    Math::Vector2<T> data[]{
        {T(1.0), T(0.0)},
        {T(2.0), T(1.0)},
        {T(0.0), T(4.0)},
        {T(1.0), T(5.0)}
    };

    Containers::Array<UnsignedInt> indices{Containers::NoInit, Containers::arraySize(data)};
    std::size_t result = MeshTools::removeDuplicatesFuzzyInPlaceInto(
            Containers::arrayCast<2, T>(Containers::stridedArrayView(data)),
            indices, T(2.0));
    CORRADE_COMPARE_AS(indices,
        Containers::arrayView<UnsignedInt>({0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(result),
        Containers::arrayView<Math::Vector2<T>>({{T(1.0), T(0.0)}, {T(0.0), T(4.0)}}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyInPlaceIntoWrongOutputSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Vector2 data[8]{};
    UnsignedInt output[7];

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzyInPlaceInto(
        Containers::arrayCast<2, Float>(Containers::stridedArrayView(data)),
        output);
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicatesFuzzyInPlaceInto(): output index array has 7 elements but expected 8\n");
}

#ifdef MAGNUM_BUILD_DEPRECATED
void RemoveDuplicatesTest::removeDuplicatesFuzzyStl() {
    /* Same but with implicit bloat. HEH HEH */
    std::vector<Vector2> data{
        {1.0f, 0.0f},
        {2.0f, 1.0f},
        {0.0f, 4.0f},
        {1.0f, 5.0f}
    };

    CORRADE_IGNORE_DEPRECATED_PUSH
    const std::vector<UnsignedInt> indices = MeshTools::removeDuplicates(data, 2.0f);
    CORRADE_IGNORE_DEPRECATED_POP
    CORRADE_COMPARE_AS(indices,
        (std::vector<UnsignedInt>{0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(data,
        (std::vector<Vector2>{{1.0f, 0.0f}, {0.0f, 4.0f}}),
        TestSuite::Compare::Container);
}
#endif

template<class IndexType, class T> void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlace() {
    setTestCaseTemplateName(Utility::formatString("{}, {}",
        Math::TypeTraits<IndexType>::name(),
        Math::TypeTraits<T>::name()));

    /* Same as above, but with an explicit index buffer */
    IndexType indices[]{3, 2, 0, 1, 2, 3};
    Math::Vector2<T> data[]{
        {T(1.0), T(0.0)},
        {T(2.0), T(1.0)},
        {T(0.0), T(4.0)},
        {T(1.0), T(5.0)}
    };

    std::size_t count = MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::stridedArrayView(indices),
        Containers::arrayCast<2, T>(Containers::stridedArrayView(data)), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<IndexType>({1, 1, 0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Math::Vector2<T>>({{T(1.0), T(0.0)}, {T(0.0), T(4.0)}}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceSmallType() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::stringstream out;
    Error redirectError{&out};

    UnsignedByte indices[1];
    Vector2 data[256]{};
    MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::stridedArrayView(indices),
        Containers::arrayCast<2, Float>(Containers::stridedArrayView(data)));
    CORRADE_COMPARE(out.str(), "MeshTools::removeDuplicatesFuzzyIndexedInPlace(): a 1-byte index type is too small for 256 vertices\n");
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceEmptyIndices() {
    Vector2 data[]{
        {1.0f, 0.0f},
        {2.0f, 1.0f},
        {0.0f, 4.0f},
        {1.0f, 5.0f}
    };

    std::size_t count = MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::StridedArrayView1D<UnsignedInt>{},
        Containers::arrayCast<2, Float>(Containers::stridedArrayView(data)), 2.0f);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Vector2>({{1.0f, 0.0f}, {0.0f, 4.0f}}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceEmptyIndicesVertices() {
    CORRADE_COMPARE((MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::StridedArrayView1D<UnsignedInt>{},
        Containers::StridedArrayView2D<Float>{})), 0);
}

template<class IndexType, class T> void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErased() {
    setTestCaseTemplateName(Utility::formatString("{}, {}",
        Math::TypeTraits<IndexType>::name(),
        Math::TypeTraits<T>::name()));

    /* Same as above, but with an explicit index buffer */
    IndexType indices[]{3, 2, 0, 1, 2, 3};
    Math::Vector2<T> data[]{
        {T(1.0), T(0.0)},
        {T(2.0), T(1.0)},
        {T(0.0), T(4.0)},
        {T(1.0), T(5.0)}
    };

    std::size_t count = MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::arrayCast<2, char>(Containers::arrayView(indices)),
        Containers::arrayCast<2, T>(Containers::stridedArrayView(data)), 2);
    CORRADE_COMPARE_AS(Containers::arrayView(indices),
        Containers::arrayView<IndexType>({1, 1, 0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(count),
        Containers::arrayView<Math::Vector2<T>>({{T(1.0), T(0.0)}, {T(0.0), T(4.0)}}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErasedNonContiguous() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char indices[6*4]{};
    Vector2 data[1];

    std::stringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::StridedArrayView2D<char>{indices, {6, 2}, {4, 2}},
        Containers::arrayCast<2, Float>(Containers::stridedArrayView(data)));
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicatesFuzzyIndexedInPlace(): second index view dimension is not contiguous\n");
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErasedWrongIndexSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    char indices[6*3]{};
    Vector2 data[1];

    std::stringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::StridedArrayView2D<char>{indices, {6, 3}},
        Containers::arrayCast<2, Float>(Containers::stridedArrayView(data)));
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
                VertexFormat::ShortNormalized,
                Containers::stridedArrayView(vertexData->data), 2},
    }};

    Trade::MeshData unique = MeshTools::removeDuplicates(mesh);
    CORRADE_COMPARE(unique.primitive(), MeshPrimitive::Lines);

    CORRADE_VERIFY(unique.isIndexed());
    if(data.indexed) {
        CORRADE_COMPARE(unique.indexCount(), mesh.indexCount());
        CORRADE_COMPARE(unique.indexType(), MeshIndexType::UnsignedShort);
        CORRADE_COMPARE_AS(unique.indices<UnsignedShort>(),
            Containers::arrayView<UnsignedShort>({1, 2, 5, 7, 1, 6, 4, 1, 5, 0, 3, 2, 3}),
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

void RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzy() {
    auto&& data = RemoveDuplicatesMeshDataFuzzyData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Deliberately not owned and not interleaved to verify that the function
       will handle this */
    struct Vertex {
        Short ints[10][2]{
            {15, 2},
            {15, 2},
            {15, 2},
            {2365, -2},
            {-2, 2365},
            {-2, 2365},
            {2365, -2},
            {37, 0},
            {37, 0},
            {37, 0}
        };
        Math::Vector<6, Float> floats[10]{
            {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
            /* This one gets collapsed to the above */
            {0.0f, 1.0f, 0.0f, 1.0f - Math::TypeTraits<Float>::epsilon()/4, 0.0f, 0.0f},
            /* This one not */
            {0.0f, 1.0f, 0.0f, 1.0f - Math::TypeTraits<Float>::epsilon()*4, 0.0f, 0.0f},
            /* These are bit-equivalent, but not all get collapsed because the
               ints are different */
            {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
            {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
            {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
            {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
             /* Same as above, only at a smaller scale */
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f + Math::TypeTraits<Float>::epsilon()/2, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f + Math::TypeTraits<Float>::epsilon()*2, 0.0f, 0.0f, 0.0f},
        };
        UnsignedByte intsAgain[10]{
            33,
            33,
            33,
            15,
            15,
            15,
            17,
            223,
            223,
            223
        };
        UnsignedInt objectId[10]{ 15, 15, 15, 15, 15, 15, 15, 15, 15, 15 };
    } vertexData[1];

    const UnsignedShort indexData[]{1, 2, 5, 9, 7, 6, 4, 7, 5, 0, 3, 8, 3};

    /* Scale and offset the floats */
    for(Math::Vector<6, Float>& f: vertexData->floats)
        f = f*data.scale + Math::Vector<6, Float>{data.offset};

    /* Create a combined attribute list */
    Containers::Array<Trade::MeshAttributeData> attributes;
    arrayAppend(attributes, Trade::MeshAttributeData{
        Trade::meshAttributeCustom(15), VertexFormat::Short, 0, 10, 4, 2});
    for(const Trade::MeshAttributeData& a: data.attributes)
        arrayAppend(attributes, Trade::MeshAttributeData{
            a.name(), a.format(), offsetof(Vertex, floats) + a.offset({}), 10, a.stride(), a.arraySize()});
    arrayAppend(attributes, Trade::MeshAttributeData{
        Trade::meshAttributeCustom(16), VertexFormat::UnsignedByte, offsetof(Vertex, intsAgain), 10, 1});
    arrayAppend(attributes, Trade::MeshAttributeData{
        Trade::MeshAttribute::ObjectId, VertexFormat::UnsignedInt, offsetof(Vertex, objectId), 10, 4});

    Containers::ArrayView<const void> indexView;
    Trade::MeshIndexData indices;
    if(data.indexed) {
        indexView = indexData;
        indices = Trade::MeshIndexData{indexData};
    }

    Trade::MeshData mesh{MeshPrimitive::Lines,
        {}, indexView, indices,
        {}, vertexData, std::move(attributes)};

    Trade::MeshData unique = MeshTools::removeDuplicatesFuzzy(mesh,
        data.epsilon);
    CORRADE_COMPARE(unique.primitive(), MeshPrimitive::Lines);

    CORRADE_VERIFY(unique.isIndexed());
    if(data.indexed) {
        CORRADE_COMPARE(unique.indexCount(), mesh.indexCount());
        CORRADE_COMPARE(unique.indexType(), MeshIndexType::UnsignedShort);
    } else {
        CORRADE_COMPARE(unique.indexCount(), mesh.vertexCount());
        CORRADE_COMPARE(unique.indexType(), MeshIndexType::UnsignedInt);
    }

    CORRADE_COMPARE(unique.attributeCount(), 3 + data.attributes.size());

    /* Verify that all attributes have expected metadata and are interleaved */
    for(UnsignedInt i = 0; i != unique.attributeCount(); ++i) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(unique.attributeStride(i), 4 + 6*sizeof(float) + 5);
    }

    CORRADE_COMPARE(unique.attributeFormat(0), VertexFormat::Short);
    CORRADE_COMPARE(unique.attributeOffset(0), 0);
    CORRADE_COMPARE(unique.attributeName(0), Trade::meshAttributeCustom(15));
    CORRADE_COMPARE(unique.attributeArraySize(0), 2);

    for(std::size_t i = 0; i != data.attributes.size(); ++i) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(unique.attributeFormat(1 + i), data.attributes[i].format());
        CORRADE_COMPARE(unique.attributeOffset(1 + i), 4 + data.attributes[i].offset({}));
        CORRADE_COMPARE(unique.attributeName(1 + i), data.attributes[i].name());
        CORRADE_COMPARE(unique.attributeArraySize(1 + i), data.attributes[i].arraySize());
    }

    CORRADE_COMPARE(unique.attributeFormat(1 + data.attributes.size()), VertexFormat::UnsignedByte);
    CORRADE_COMPARE(unique.attributeOffset(1 + data.attributes.size()), 4 + 6*sizeof(Float));
    CORRADE_COMPARE(unique.attributeName(1 + data.attributes.size()), Trade::meshAttributeCustom(16));

    CORRADE_COMPARE(unique.attributeFormat(2 + data.attributes.size()), VertexFormat::UnsignedInt);
    CORRADE_COMPARE(unique.attributeOffset(2 + data.attributes.size()), 5 + 6*sizeof(Float));
    CORRADE_COMPARE(unique.attributeName(2 + data.attributes.size()), Trade::MeshAttribute::ObjectId);

    /* The data differ depending on how much is actually removed */
    if(data.vertexCount == 7) {
        if(data.indexed) CORRADE_COMPARE_AS(unique.indices<UnsignedShort>(),
            Containers::arrayView<UnsignedShort>({0, 1, 3, 6, 5, 4, 3, 5, 3, 0, 2, 5, 2}),
            TestSuite::Compare::Container);
        else CORRADE_COMPARE_AS(unique.indices<UnsignedInt>(),
            Containers::arrayView<UnsignedInt>({0, 0, 1, 2, 3, 3, 4, 5, 5, 6}),
            TestSuite::Compare::Container);

        /* Compare the integer data through the attribute API */
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const Vector2s>(unique.attribute<Short[]>(Trade::meshAttributeCustom(15)))),
            Containers::arrayView<Vector2s>({
                {15, 2},
                {15, 2},
                {2365, -2},
                {-2, 2365},
                {2365, -2},
                {37, 0},
                {37, 0}
            }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(unique.attribute<UnsignedByte>(Trade::meshAttributeCustom(16)),
            Containers::arrayView<UnsignedByte>({
                33,
                33,
                15,
                15,
                17,
                223,
                223
            }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(unique.attribute<UnsignedInt>(Trade::MeshAttribute::ObjectId),
            Containers::arrayView<UnsignedInt>({
                15, 15, 15, 15, 15, 15, 15
            }), TestSuite::Compare::Container);

        /* Compare the float/double data as a single block independently of the
           attribute layout */
        Math::Vector<6, Float> expectedFloats[]{
            {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 1.0f - Math::TypeTraits<Float>::epsilon()*4, 0.0f, 0.0f},
            {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
            {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
            {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f + Math::TypeTraits<Float>::epsilon()*2, 0.0f, 0.0f, 0.0f},
        };
        for(Math::Vector<6, Float>& f: expectedFloats)
            f = f*data.scale + Math::Vector<6, Float>{data.offset};
        /** @todo i need some feature like "gimme just the top-level dimension" */
        Containers::StridedArrayView1D<const char> floats{unique.vertexData(),
            unique.vertexData() + unique.attributeOffset(1),
            unique.vertexCount(), unique.attributeStride(1)};
        CORRADE_COMPARE_AS((Containers::arrayCast<const Math::Vector<6, Float>>(floats)),
            Containers::arrayView(expectedFloats),
            TestSuite::Compare::Container);

    } else if(data.vertexCount == 9) {
        if(data.indexed) CORRADE_COMPARE_AS(unique.indices<UnsignedShort>(),
            Containers::arrayView<UnsignedShort>({1, 2, 4, 8, 6, 5, 4, 6, 4, 0, 3, 7, 3}),
            TestSuite::Compare::Container);
        else CORRADE_COMPARE_AS(unique.indices<UnsignedInt>(),
            Containers::arrayView<UnsignedInt>({0, 1, 2, 3, 4, 4, 5, 6, 7, 8}),
            TestSuite::Compare::Container);

        /* Not testing the rest, it's verified well enough in the other cases */
    }
}

void RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyDouble() {
    /* Deliberately not owned and not interleaved to verify that the function
       will handle this */
    const struct Vertex {
        /* Epsilon enlarged a lot to ensure the cell size isn't too small for
           the grid size to fit into 32 bit std::size_t */
        Math::Vector<3, Double> doubles[10]{
            {110.0, 100.0, 100.0},
            /* This one gets collapsed to the above */
            {110.0 - 250000*Math::TypeTraits<Double>::epsilon()/2, 100.0, 100.0},
            /* This one not */
            {110.0 - 250000*Math::TypeTraits<Double>::epsilon()*2, 100.0, 100.0},
            /* These are bit-equivalent, but not all get collapsed because the
               ints are different */
            {100.0, 100.0, 110.0},
            {100.0, 100.0, 110.0},
            {100.0, 100.0, 110.0},
            {100.0, 100.0, 110.0},
             /* Same as above, only at a smaller scale */
            {100.0, 100.0, 100.0},
            {100.0, 100.0, 100.0 + 250000*Math::TypeTraits<Double>::epsilon()/2},
            {100.0, 100.0, 100.0 + 250000*Math::TypeTraits<Double>::epsilon()*2},
        };
        UnsignedByte objectId[10]{
            33,
            33,
            33,
            15,
            16,
            15,
            17,
            223,
            223,
            223
        };
    /* Old Clang complains that "error: default initialization of struct"
       without the {} */
    } vertexData[1]{};

    Trade::MeshData mesh{MeshPrimitive::Points,
        {}, vertexData, {
            Trade::MeshAttributeData{Trade::meshAttributeCustom(10),
                VertexFormat::Double, 0, 10, 3*sizeof(Double)},
            Trade::MeshAttributeData{Trade::meshAttributeCustom(11),
                VertexFormat::Double, sizeof(Double), 10, 3*sizeof(Double), 2},
            Trade::MeshAttributeData{
                Trade::MeshAttribute::ObjectId, VertexFormat::UnsignedByte, offsetof(Vertex, objectId), 10, 1}
        }};

    Trade::MeshData unique = MeshTools::removeDuplicatesFuzzy(mesh,
        Math::TypeTraits<Float>::epsilon(),
        /* Epsilon enlarged a lot to ensure the cell size isn't too small for
           the grid size to fit into 32 bit std::size_t */
        25000*Math::TypeTraits<Double>::epsilon());
    CORRADE_COMPARE(unique.primitive(), MeshPrimitive::Points);

    CORRADE_VERIFY(unique.isIndexed());
    CORRADE_COMPARE(unique.indexCount(), mesh.vertexCount());
    CORRADE_COMPARE(unique.indexType(), MeshIndexType::UnsignedInt);

    CORRADE_COMPARE(unique.attributeCount(), 3);

    /* Verify that all attributes have expected metadata and are interleaved */
    for(UnsignedInt i = 0; i != unique.attributeCount(); ++i) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(unique.attributeStride(i), 3*sizeof(Double) + 1);
    }

    CORRADE_COMPARE(unique.attributeFormat(0), VertexFormat::Double);
    CORRADE_COMPARE(unique.attributeOffset(0), 0);
    CORRADE_COMPARE(unique.attributeName(0), Trade::meshAttributeCustom(10));

    CORRADE_COMPARE(unique.attributeFormat(1), VertexFormat::Double);
    CORRADE_COMPARE(unique.attributeOffset(1), sizeof(Double));
    CORRADE_COMPARE(unique.attributeName(1), Trade::meshAttributeCustom(11));
    CORRADE_COMPARE(unique.attributeArraySize(1), 2);

    CORRADE_COMPARE(unique.attributeFormat(2), VertexFormat::UnsignedByte);
    CORRADE_COMPARE(unique.attributeOffset(2), 3*sizeof(Double));
    CORRADE_COMPARE(unique.attributeName(2), Trade::MeshAttribute::ObjectId);

    CORRADE_COMPARE_AS(unique.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 0, 1, 2, 3, 2, 4, 5, 5, 6}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(unique.attribute<UnsignedByte>(Trade::MeshAttribute::ObjectId),
        Containers::arrayView<UnsignedByte>({
            33,
            33,
            15,
            16,
            17,
            223,
            223
        }), TestSuite::Compare::Container);

    /* Compare the float/double data as a single block independently of the
        attribute layout */
    Math::Vector<3, Double> expectedFloats[]{
        {110.0, 100.0, 100.0},
        {110.0 - 250000*Math::TypeTraits<Double>::epsilon()*2, 100.0, 100.0},
        {100.0, 100.0, 110.0},
        {100.0, 100.0, 110.0},
        {100.0, 100.0, 110.0},
        {100.0, 100.0, 100.0},
        {100.0, 100.0, 100.0 + 250000*Math::TypeTraits<Double>::epsilon()*2},
    };
    /** @todo i need some feature like "gimme just the top-level dimension" */
    Containers::StridedArrayView1D<const char> floats{unique.vertexData(),
        unique.vertexData() + unique.attributeOffset(0),
        unique.vertexCount(), unique.attributeStride(0)};
    CORRADE_COMPARE_AS((Containers::arrayCast<const Math::Vector<3, Double>>(floats)),
        Containers::arrayView(expectedFloats),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyAttributeless() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzy(Trade::MeshData{MeshPrimitive::Points, 10});
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicatesFuzzy(): can't remove duplicates in an attributeless mesh\n");
}

void RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyImplementationSpecific() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    MeshTools::removeDuplicatesFuzzy(Trade::MeshData{MeshPrimitive::Points,
        nullptr, {Trade::MeshAttributeData{Trade::MeshAttribute::Position,
            vertexFormatWrap(0x1234), nullptr}}});
    CORRADE_COMPARE(out.str(),
        "MeshTools::removeDuplicatesFuzzy(): can't remove duplicates in an implementation-specific format 0x1234\n");
}

void RemoveDuplicatesTest::soakTest() {
    /* Array of 100 unique items with 10 duplicates each, randomly shuffled */
    UnsignedInt data[1000];
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
        data[i] = i / 10 + testCaseRepeatId()*909091;
    std::shuffle(std::begin(data), std::end(data), std::minstd_rand{std::random_device{}()});

    CORRADE_COMPARE(MeshTools::removeDuplicatesInPlace(
        Containers::StridedArrayView2D<char>{
            Containers::arrayCast<char>(Containers::arrayView(data)), {1000, 4}}
        ).second, 100);
}

void RemoveDuplicatesTest::soakTestFuzzy() {
    /* Array of 100 unique items with 10 duplicates each, randomly shuffled */
    Float data[1000];
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
        data[i] = Float(Int(i/10) + testCaseRepeatId()*909091);
    std::shuffle(std::begin(data), std::end(data), std::minstd_rand{std::random_device{}()});

    CORRADE_COMPARE(MeshTools::removeDuplicatesFuzzyInPlace(
        Containers::arrayCast<2, Float>(Containers::arrayView(data))).second,
        100);
}

void RemoveDuplicatesTest::benchmark() {
    /* Array of 100 unique items with 100 duplicates each, shuffled */
    Vector3i data[10000];
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
        data[i].x() = i/100;
    std::shuffle(std::begin(data), std::end(data), std::minstd_rand{std::random_device{}()});

    std::size_t count;
    UnsignedInt indices[10000];
    CORRADE_BENCHMARK(1)
        count = MeshTools::removeDuplicatesInPlaceInto(
            Containers::arrayCast<2, char>(Containers::arrayView(data)),
            indices);

    CORRADE_COMPARE(count, 100);
}

void RemoveDuplicatesTest::benchmarkFuzzy() {
    /* Array of 100 unique items with 100 duplicates each, shuffled */
    Vector3 data[10000];
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
        data[i].x() = i/100;
    std::shuffle(std::begin(data), std::end(data), std::minstd_rand{std::random_device{}()});

    std::size_t count;
    UnsignedInt indices[10000];
    CORRADE_BENCHMARK(1)
        count = MeshTools::removeDuplicatesFuzzyInPlaceInto(
            Containers::arrayCast<2, Float>(Containers::arrayView(data)),
            indices);

    CORRADE_COMPARE(count, 100);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::RemoveDuplicatesTest)
