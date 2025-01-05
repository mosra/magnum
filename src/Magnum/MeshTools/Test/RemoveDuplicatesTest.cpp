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

#include <algorithm> /* std::shuffle() */
#include <random> /* random device for std::shuffle() */
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

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
    void removeDuplicatesMeshDataPaddedAttributes();
    void removeDuplicatesMeshDataAttributeless();
    void removeDuplicatesMeshDataImplementationSpecificIndexType();
    void removeDuplicatesMeshDataImplementationSpecificVertexFormat();

    void removeDuplicatesMeshDataFuzzy();
    void removeDuplicatesMeshDataFuzzyDouble();
    void removeDuplicatesMeshDataFuzzyAttributeless();
    void removeDuplicatesMeshDataFuzzyImplementationSpecificIndexType();
    void removeDuplicatesMeshDataFuzzyImplementationSpecificVertexFormat();

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
    /* The attributes are expected to be offsets into the `floats` array in the
       test, which consists of 10 times 6 floats, so count should be always 10
       and size be always 6*sizeof(Float). And all six floats in the array need
       to be covered by some attribute as the output stride is expected to be
       the same. */
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
    }), 0.0f, 10.0f, 10.0f*Math::TypeTraits<Float>::epsilon(), 7, false},
    /* Nothing special here, just verifying these (array) attributes get
       handled properly */
    {"jointId[3] + weight[3], texcoord", Containers::array({
        Trade::MeshAttributeData{Trade::MeshAttribute::JointIds,
            /* It's a float, but interpreting as UnsignedInt. The first 2
               floats out of 6 are bit-exact so integer comparison should work
               as well; the second 2 are then with the epsilon.  */
            VertexFormat::UnsignedInt, 0, 10, 6*sizeof(Float), 2},
        Trade::MeshAttributeData{Trade::MeshAttribute::Weights,
            VertexFormat::Float, 2*sizeof(Float), 10, 6*sizeof(Float), 2},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            VertexFormat::Vector2, 4*sizeof(Float), 10, 6*sizeof(Float)}
    }), 0.0f, 1.0f, Math::TypeTraits<Float>::epsilon(), 7, false},
    {"jointId[3] + weight[3], epsilon 0", Containers::array({
        Trade::MeshAttributeData{Trade::MeshAttribute::JointIds,
            /* It's a float, but interpreting as UnsignedInt. The first 2
               floats out of 6 are bit-exact so integer comparison should work
               as well; the second 2 are then with the epsilon.  */
            VertexFormat::UnsignedInt, 0, 10, 6*sizeof(Float), 2},
        Trade::MeshAttributeData{Trade::MeshAttribute::Weights,
            VertexFormat::Float, 2*sizeof(Float), 10, 6*sizeof(Float), 2},
        Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates,
            VertexFormat::Vector2, 4*sizeof(Float), 10, 6*sizeof(Float)}
    }), 0.0f, 1.0f, 0.0f, 9, false},
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

    addTests({&RemoveDuplicatesTest::removeDuplicatesMeshDataPaddedAttributes});

    addTests({&RemoveDuplicatesTest::removeDuplicatesMeshDataAttributeless,
              &RemoveDuplicatesTest::removeDuplicatesMeshDataImplementationSpecificIndexType,
              &RemoveDuplicatesTest::removeDuplicatesMeshDataImplementationSpecificVertexFormat});

    addInstancedTests({&RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzy},
        Containers::arraySize(RemoveDuplicatesMeshDataFuzzyData));

    addTests({&RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyDouble,

              &RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyAttributeless,
              &RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyImplementationSpecificIndexType,
              &RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyImplementationSpecificVertexFormat});

    addRepeatedTests({&RemoveDuplicatesTest::soakTest,
                      &RemoveDuplicatesTest::soakTestFuzzy}, 10);

    addBenchmarks({&RemoveDuplicatesTest::benchmark,
                   &RemoveDuplicatesTest::benchmarkFuzzy}, 10);
}

void RemoveDuplicatesTest::removeDuplicates() {
    Int data[]{-15, 32, 24, -15, 15, 7541, 24, 32};

    Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> result =
        MeshTools::removeDuplicates(Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE_AS(Containers::arrayView(result.first()),
        Containers::arrayView<UnsignedInt>({0, 1, 2, 0, 4, 5, 2, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(result.second(), 5);

    Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> resultInPlace =
        MeshTools::removeDuplicatesInPlace(Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE_AS(Containers::arrayView(resultInPlace.first()),
        Containers::arrayView<UnsignedInt>({0, 1, 2, 0, 3, 4, 2, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(resultInPlace.second()),
        Containers::arrayView<Int>({-15, 32, 24, 15, 7541}),
        TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesNonContiguous() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Int data[8]{};

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicates(Containers::arrayCast<2, const char>(Containers::arrayView(data)).every({1, 2}));
    MeshTools::removeDuplicatesInPlace(Containers::arrayCast<2, char>(Containers::arrayView(data)).every({1, 2}));
    CORRADE_COMPARE(out,
        "MeshTools::removeDuplicatesInto(): second data view dimension is not contiguous\n"
        "MeshTools::removeDuplicatesInPlaceInto(): second data view dimension is not contiguous\n");
}

void RemoveDuplicatesTest::removeDuplicatesIntoWrongOutputSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Int data[8]{};
    UnsignedInt output[7];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesInto(
        Containers::arrayCast<2, const char>(Containers::arrayView(data)),
        output);
    MeshTools::removeDuplicatesInPlaceInto(
        Containers::arrayCast<2, char>(Containers::arrayView(data)),
        output);
    CORRADE_COMPARE(out,
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
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    UnsignedByte indices[1];
    Vector2i data[256]{};
    MeshTools::removeDuplicatesIndexedInPlace(
        Containers::stridedArrayView(indices),
        Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE(out, "MeshTools::removeDuplicatesIndexedInPlace(): a 1-byte index type is too small for 256 vertices\n");
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
    CORRADE_SKIP_IF_NO_ASSERT();

    char indices[6*4]{};
    Int data[1]{};

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesIndexedInPlace(
        Containers::StridedArrayView2D<char>{indices, {6, 2}, {4, 2}},
        Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE(out,
        "MeshTools::removeDuplicatesIndexedInPlace(): second index view dimension is not contiguous\n");
}

void RemoveDuplicatesTest::removeDuplicatesIndexedInPlaceErasedWrongIndexSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char indices[6*3]{};
    Int data[1]{};

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesIndexedInPlace(
        Containers::StridedArrayView2D<char>{indices, {6, 3}},
        Containers::arrayCast<2, char>(Containers::arrayView(data)));
    CORRADE_COMPARE(out,
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

    Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> result =
        MeshTools::removeDuplicatesFuzzyInPlace(
            Containers::arrayCast<2, T>(Containers::stridedArrayView(data)),
            T(1.00001));
    CORRADE_COMPARE_AS(Containers::arrayView(result.first()),
        Containers::arrayView<UnsignedInt>({0, 1, 0, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(result.second()),
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

    Containers::Pair<Containers::Array<UnsignedInt>, std::size_t> result =
        MeshTools::removeDuplicatesFuzzyInPlace(
            Containers::arrayCast<2, T>(Containers::stridedArrayView(data)),
            T(2.0));
    CORRADE_COMPARE_AS(Containers::arrayView(result.first()),
        Containers::arrayView<UnsignedInt>({0, 0, 1, 1}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(Containers::arrayView(data).prefix(result.second()),
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

    Containers::Array<UnsignedInt> indices{NoInit, Containers::arraySize(data)};
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
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector2 data[8]{};
    UnsignedInt output[7];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzyInPlaceInto(
        Containers::arrayCast<2, Float>(Containers::stridedArrayView(data)),
        output);
    CORRADE_COMPARE(out,
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
    setTestCaseTemplateName({Math::TypeTraits<IndexType>::name(), Math::TypeTraits<T>::name()});

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
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    UnsignedByte indices[1];
    Vector2 data[256]{};
    MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::stridedArrayView(indices),
        Containers::arrayCast<2, Float>(Containers::stridedArrayView(data)));
    CORRADE_COMPARE(out, "MeshTools::removeDuplicatesFuzzyIndexedInPlace(): a 1-byte index type is too small for 256 vertices\n");
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
    setTestCaseTemplateName({Math::TypeTraits<IndexType>::name(), Math::TypeTraits<T>::name()});

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
    CORRADE_SKIP_IF_NO_ASSERT();

    char indices[6*4]{};
    Vector2 data[1];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::StridedArrayView2D<char>{indices, {6, 2}, {4, 2}},
        Containers::arrayCast<2, Float>(Containers::stridedArrayView(data)));
    CORRADE_COMPARE(out,
        "MeshTools::removeDuplicatesFuzzyIndexedInPlace(): second index view dimension is not contiguous\n");
}

void RemoveDuplicatesTest::removeDuplicatesFuzzyIndexedInPlaceErasedWrongIndexSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char indices[6*3]{};
    Vector2 data[1];

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzyIndexedInPlace(
        Containers::StridedArrayView2D<char>{indices, {6, 3}},
        Containers::arrayCast<2, Float>(Containers::stridedArrayView(data)));
    CORRADE_COMPARE(out,
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

void RemoveDuplicatesTest::removeDuplicatesMeshDataPaddedAttributes() {
    /* Same as the non-indexed case in removeDuplicatesMeshData() except that
       the vertex data is interleaved with deliberate padding at the front and
       the end of each vertex to test that the output attribute offsets and
       strides are recalculated appropriately -- and not the original attribute
       stride/offset just being passed through.

       Additionally, there's explicitly unique padding data for each vertex
       which should be ignored by the algorithm -- otherwise no duplicates
       would get removed.

       The array aspect also doesn't need to be tested here again, so it's just
       Vector2s instead of Short[2]. */
    struct PaddedVertex {
        /* Usually it was enough to just move the struct out of the function to
           make MSVC 2015 recognize the anonymous padding bitfields, but in
           this case that somehow leads to an ICE. So doing it the long way,
           naming all fields and then providing a constructor that initializes
           just some, *deliberately* leaving the others uninitialized to match
           the behavior with anonymous bitfields. */
        #ifdef CORRADE_MSVC2015_COMPATIBILITY
        PaddedVertex(const Vector2& position, Int padding, const Vector2s& data): position{position}, padding{padding}, data{data} {}
        Int padding0;
        Int padding1;
        Int padding2;
        Int padding3;
        Vector2 position;
        Int padding;
        Vector2s data;
        Int padding4;
        #else
        Int:32;
        Int:32;
        Int:32;
        Int:32;
        Vector2 position;
        Int padding;
        Vector2s data;
        Int:32;
        #endif
    } vertices[]{
        {{1.0f, 2.0f},  0, {-15, 2}},
        {{3.0f, 4.0f},  1, {2, 32}},
        {{5.0f, 6.0f},  2, {24, 2}},
        {{7.0f, 8.0f},  3, {-15, 2}},
        {{7.0f, 8.0f},  4, {15, 2}},
        {{7.0f, 8.0f},  5, {2, 7541}},
        {{9.0f, 10.0f}, 6, {24, 2}},
        {{3.0f, 4.0f},  7, {2, 32}},
        {{5.0f, 6.0f},  8, {24, 2}},
        {{5.0f, 6.0f},  9, {15, 2}}
    };

    Trade::MeshData mesh{MeshPrimitive::Lines,
        {}, vertices, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position,
                Containers::stridedArrayView(vertices).slice(&PaddedVertex::position)},
            Trade::MeshAttributeData{Trade::meshAttributeCustom(42),
                Containers::stridedArrayView(vertices).slice(&PaddedVertex::data)},
    }};

    Trade::MeshData unique = MeshTools::removeDuplicates(mesh);
    CORRADE_COMPARE(unique.vertexCount(), 8);
    CORRADE_COMPARE(unique.attributeCount(), 2);

    /* Same as in removeDuplicatesMeshData() */
    CORRADE_COMPARE(unique.attributeFormat(0), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(unique.attribute<Vector2>(0), Containers::arrayView<Vector2>({
        {1.0f, 2.0f},
        {3.0f, 4.0f},
        {5.0f, 6.0f},
        {7.0f, 8.0f},
        {7.0f, 8.0f},
        {7.0f, 8.0f},
        {9.0f, 10.0f},
        {5.0f, 6.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE(unique.attributeFormat(1), VertexFormat::Vector2s);
    CORRADE_COMPARE_AS(unique.attribute<Vector2s>(1), Containers::arrayView<Vector2s>({
        {-15, 2},
        {2, 32},
        {24, 2},
        {-15, 2},
        {15, 2},
        {2, 7541},
        {24, 2},
        {15, 2}
    }), TestSuite::Compare::Container);
}

void RemoveDuplicatesTest::removeDuplicatesMeshDataAttributeless() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicates(Trade::MeshData{MeshPrimitive::Points, 10});
    CORRADE_COMPARE(out,
        "MeshTools::removeDuplicates(): can't remove duplicates in an attributeless mesh\n");
}

void RemoveDuplicatesTest::removeDuplicatesMeshDataImplementationSpecificIndexType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicates(Trade::MeshData{MeshPrimitive::Points,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<const void>{}},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr}
        }});
    CORRADE_COMPARE(out,
        "MeshTools::removeDuplicates(): mesh has an implementation-specific index type 0xcaca\n");
}

void RemoveDuplicatesTest::removeDuplicatesMeshDataImplementationSpecificVertexFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicates(Trade::MeshData{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertexFormatWrap(0xcaca), nullptr}
    }});
    CORRADE_COMPARE(out,
        "MeshTools::removeDuplicates(): attribute 1 has an implementation-specific format 0xcaca\n");
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
        {}, vertexData, Utility::move(attributes)};

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
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzy(Trade::MeshData{MeshPrimitive::Points, 10});
    CORRADE_COMPARE(out,
        "MeshTools::removeDuplicatesFuzzy(): can't remove duplicates in an attributeless mesh\n");
}

void RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyImplementationSpecificIndexType() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzy(Trade::MeshData{MeshPrimitive::Points,
        nullptr, Trade::MeshIndexData{meshIndexTypeWrap(0xcaca), Containers::StridedArrayView1D<const void>{}},
        nullptr, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr}
        }});
    CORRADE_COMPARE(out,
        "MeshTools::removeDuplicatesFuzzy(): mesh has an implementation-specific index type 0xcaca\n");
}

void RemoveDuplicatesTest::removeDuplicatesMeshDataFuzzyImplementationSpecificVertexFormat() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    MeshTools::removeDuplicatesFuzzy(Trade::MeshData{MeshPrimitive::Points, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr},
        Trade::MeshAttributeData{Trade::MeshAttribute::Normal, vertexFormatWrap(0xcaca), nullptr}
    }});
    CORRADE_COMPARE(out,
        "MeshTools::removeDuplicatesFuzzy(): attribute 1 has an implementation-specific format 0xcaca\n");
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
        ).second(), 100);
}

void RemoveDuplicatesTest::soakTestFuzzy() {
    /* Array of 100 unique items with 10 duplicates each, randomly shuffled */
    Float data[1000];
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
        data[i] = Float(Int(i/10) + testCaseRepeatId()*909091);
    std::shuffle(std::begin(data), std::end(data), std::minstd_rand{std::random_device{}()});

    CORRADE_COMPARE(MeshTools::removeDuplicatesFuzzyInPlace(
        Containers::arrayCast<2, Float>(Containers::arrayView(data))).second(),
        100);
}

void RemoveDuplicatesTest::benchmark() {
    /* Array of 100 unique items with 100 duplicates each, shuffled */
    Vector3i data[10000];
    for(std::size_t i = 0; i != Containers::arraySize(data); ++i)
        data[i].x() = i/100;
    std::shuffle(std::begin(data), std::end(data), std::minstd_rand{std::random_device{}()});

    /* GCC 12 LOUDLY warns that this variable might be uninitialized, which
       is stupid, suppress that */
    std::size_t count = 0;
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

    /* GCC 12 LOUDLY warns that this variable might be uninitialized, which
       is stupid, suppress that */
    std::size_t count = 0;
    UnsignedInt indices[10000];
    CORRADE_BENCHMARK(1)
        count = MeshTools::removeDuplicatesFuzzyInPlaceInto(
            Containers::arrayCast<2, Float>(Containers::arrayView(data)),
            indices);

    CORRADE_COMPARE(count, 100);
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::RemoveDuplicatesTest)
