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
#include <Corrade/Containers/Reference.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/MeshTools/Combine.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct CombineTest: TestSuite::Tester {
    explicit CombineTest();

    void combineIndexedAttributes();
    void combineIndexedAttributesIndicesOnly();

    void combineIndexedAttributesNoMeshes();
    void combineIndexedAttributesNotIndexed();
    void combineIndexedAttributesDifferentPrimitive();
    void combineIndexedAttributesDifferentIndexCount();
};

CombineTest::CombineTest() {
    addTests({&CombineTest::combineIndexedAttributes,
              &CombineTest::combineIndexedAttributesIndicesOnly,

              &CombineTest::combineIndexedAttributesNoMeshes,
              &CombineTest::combineIndexedAttributesNotIndexed,
              &CombineTest::combineIndexedAttributesDifferentPrimitive,
              &CombineTest::combineIndexedAttributesDifferentIndexCount});
}

void CombineTest::combineIndexedAttributes() {
    const UnsignedInt indicesA[]{2, 1, 2, 0};
    const Int dataA[]{2, 1, 0};
    const UnsignedByte indicesB[]{3, 4, 3, 2};
    const Short dataB[]{4, 3, 2, 1, 0};
    const UnsignedShort indicesC[]{7, 6, 7, 5};
    const Float dataC[]{0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
    Trade::MeshData a{MeshPrimitive::LineLoop,
        {}, indicesA, Trade::MeshIndexData{indicesA},
        {}, dataA, {Trade::MeshAttributeData{
            Trade::meshAttributeCustom(2), Containers::arrayView(dataA)}}};
    Trade::MeshData b{MeshPrimitive::LineLoop,
        {}, indicesB, Trade::MeshIndexData{indicesB},
        {}, dataB, {Trade::MeshAttributeData{
            Trade::meshAttributeCustom(17), Containers::arrayView(dataB)}}};
    Trade::MeshData c{MeshPrimitive::LineLoop,
        {}, indicesC, Trade::MeshIndexData{indicesC},
        {}, dataC, {Trade::MeshAttributeData{
            Trade::meshAttributeCustom(22), Containers::arrayView(dataC)}}};

    Trade::MeshData result = MeshTools::combineIndexedAttributes({a, b, c});
    CORRADE_COMPARE(result.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(result.isIndexed());
    CORRADE_COMPARE(result.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(result.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 0, 2}),
        TestSuite::Compare::Container);

    CORRADE_COMPARE(result.attributeCount(), 3);
    CORRADE_COMPARE(result.attributeName(0), Trade::meshAttributeCustom(2));
    CORRADE_COMPARE(result.attributeFormat(0), VertexFormat::Int);
    CORRADE_COMPARE_AS(result.attribute<Int>(0),
        Containers::arrayView<Int>({0, 1, 2}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(result.attributeName(1), Trade::meshAttributeCustom(17));
    CORRADE_COMPARE(result.attributeFormat(1), VertexFormat::Short);
    CORRADE_COMPARE_AS(result.attribute<Short>(1),
        Containers::arrayView<Short>({1, 0, 2}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(result.attributeName(2), Trade::meshAttributeCustom(22));
    CORRADE_COMPARE(result.attributeFormat(2), VertexFormat::Float);
    CORRADE_COMPARE_AS(result.attribute<Float>(2),
        Containers::arrayView<Float>({7.0f, 6.0f, 5.0f}),
        TestSuite::Compare::Container);
}

void CombineTest::combineIndexedAttributesIndicesOnly() {
    const UnsignedInt indicesA[]{2, 1, 2};
    const UnsignedShort indicesB[]{3, 4, 3};
    const UnsignedByte indicesC[]{7, 6, 7};
    Trade::MeshData a{MeshPrimitive::LineLoop, {}, indicesA,
        Trade::MeshIndexData{indicesA}};
    Trade::MeshData b{MeshPrimitive::LineLoop, {}, indicesB,
        Trade::MeshIndexData{indicesB}};
    Trade::MeshData c{MeshPrimitive::LineLoop, {}, indicesC,
        Trade::MeshIndexData{indicesC}};

    Trade::MeshData result = MeshTools::combineIndexedAttributes({a, b, c});
    CORRADE_COMPARE(result.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(result.isIndexed());
    CORRADE_COMPARE(result.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(result.indices<UnsignedInt>(),
        Containers::arrayView<UnsignedInt>({0, 1, 0}),
        TestSuite::Compare::Container);
    CORRADE_COMPARE(result.attributeCount(), 0);
    CORRADE_COMPARE(result.vertexCount(), 0);
}

void CombineTest::combineIndexedAttributesNoMeshes() {
    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::combineIndexedAttributes({});
    CORRADE_COMPARE(out.str(), "MeshTools::combineIndexedAttributes(): no meshes passed\n");
}

void CombineTest::combineIndexedAttributesNotIndexed() {
    const UnsignedShort indices[5]{};
    Trade::MeshData a{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}};
    Trade::MeshData b{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}};
    Trade::MeshData c{MeshPrimitive::Lines, 5};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::combineIndexedAttributes({a, b, c});
    CORRADE_COMPARE(out.str(), "MeshTools::combineIndexedAttributes(): data 2 is not indexed\n");
}

void CombineTest::combineIndexedAttributesDifferentPrimitive() {
    const UnsignedShort indices[5]{};
    Trade::MeshData a{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}};
    Trade::MeshData b{MeshPrimitive::Points,
        {}, indices, Trade::MeshIndexData{indices}};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::combineIndexedAttributes({a, b});
    CORRADE_COMPARE(out.str(), "MeshTools::combineIndexedAttributes(): data 1 is MeshPrimitive::Points but expected MeshPrimitive::Lines\n");
}

void CombineTest::combineIndexedAttributesDifferentIndexCount() {
    const UnsignedShort indices[5]{};
    Trade::MeshData a{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}};
    Trade::MeshData b{MeshPrimitive::Lines,
        {}, indices, Trade::MeshIndexData{indices}};
    Trade::MeshData c{MeshPrimitive::Lines,
        {}, indices,
        Trade::MeshIndexData{Containers::arrayView(indices).prefix(4)}};

    std::ostringstream out;
    Error redirectError{&out};
    MeshTools::combineIndexedAttributes({a, b, c});
    CORRADE_COMPARE(out.str(), "MeshTools::combineIndexedAttributes(): data 2 has 4 indices but expected 5\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CombineTest)
