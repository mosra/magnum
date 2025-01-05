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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/MeshTools/GenerateLines.h"
#include "Magnum/Shaders/Line.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct GenerateLinesTest: TestSuite::Tester {
    explicit GenerateLinesTest();

    template<class T> void oneLoop();

    void extraAttributes();
    void zeroVertices();
    void twoVerticesStrip();
    void twoVerticesLoop();

    void notLines();
    void noAttributes();
    void noPositionAttribute();
};

using namespace Math::Literals;

const struct {
    const char* name;
    MeshPrimitive primitive;
    Containers::Array<Vector2> positions;
    Containers::Array<UnsignedInt> indices;
    bool expectedJoins;
    bool expectedJoinsFirstLast;
} OneLoopData[]{
    {"loose segments", MeshPrimitive::Lines, {InPlaceInit, {
        {-1.0f, -1.0f}, {+1.0f, -1.0f},
        {+1.0f, -1.0f}, {+1.0f, +1.0f},
        {+1.0f, +1.0f}, {-1.0f, +1.0f},
        {-1.0f, +1.0f}, {-1.0f, -1.0f}
    }}, nullptr, false, false},
    {"loose indexed segments", MeshPrimitive::Lines, {InPlaceInit, {
        {-1.0f, -1.0f},
        {+1.0f, -1.0f},
        {+1.0f, +1.0f},
        {-1.0f, +1.0f},
    }}, {InPlaceInit, {
        0, 1, 1, 2, 2, 3, 3, 0
    }}, false, false},
    /** @todo indexed segments that get connected */
    {"loop", MeshPrimitive::LineLoop, {InPlaceInit, {
        {-1.0f, -1.0f},
        {+1.0f, -1.0f},
        {+1.0f, +1.0f},
        {-1.0f, +1.0f},
    }}, nullptr, true, true},
    {"indexed loop", MeshPrimitive::LineLoop, {InPlaceInit, {
        {-1.0f, -1.0f},
        {-1.0f, +1.0f},
        {+1.0f, -1.0f},
        {+1.0f, +1.0f},
    }}, {InPlaceInit, {
        0, 2, 3, 1
    }}, true, true},
    {"strip", MeshPrimitive::LineStrip, {InPlaceInit, {
        {-1.0f, -1.0f},
        {+1.0f, -1.0f},
        {+1.0f, +1.0f},
        {-1.0f, +1.0f},
        {-1.0f, -1.0f},
    }}, nullptr, true, false},
    {"indexed strip", MeshPrimitive::LineStrip, {InPlaceInit, {
        {-1.0f, -1.0f},
        {-1.0f, +1.0f},
        {+1.0f, -1.0f},
        {+1.0f, +1.0f},
    }}, {InPlaceInit, {
        0, 2, 3, 1, 0
    }}, true, false},
    /** @todo closed (indexed) strip, once arbitrary index buffer looping is supported */
};

GenerateLinesTest::GenerateLinesTest() {
    addInstancedTests<GenerateLinesTest>({
        &GenerateLinesTest::oneLoop<UnsignedInt>,
        &GenerateLinesTest::oneLoop<UnsignedShort>,
        &GenerateLinesTest::oneLoop<UnsignedByte>},
        Containers::arraySize(OneLoopData));

    addTests({&GenerateLinesTest::extraAttributes,
              &GenerateLinesTest::zeroVertices,
              &GenerateLinesTest::twoVerticesStrip,
              &GenerateLinesTest::twoVerticesLoop,

              &GenerateLinesTest::notLines,
              &GenerateLinesTest::noAttributes,
              &GenerateLinesTest::noPositionAttribute});
}

template<class T> void GenerateLinesTest::oneLoop() {
    auto&& data = OneLoopData[testCaseInstanceId()];
    setTestCaseTemplateName(Math::TypeTraits<T>::name());
    setTestCaseDescription(data.name);

    Containers::Array<T> indices{NoInit, data.indices.size()};
    Math::castInto(stridedArrayView(data.indices), stridedArrayView(indices));

    Trade::MeshData lineMesh{data.primitive,
        {}, indices, indices ? Trade::MeshIndexData{indices} : Trade::MeshIndexData{},
        {}, data.positions, {
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, stridedArrayView(data.positions)}
        }};

    Trade::MeshData mesh = generateLines(lineMesh);
    CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(mesh.attributeCount(), 4);

    CORRADE_VERIFY(mesh.isIndexed());
    CORRADE_COMPARE(mesh.indexType(), MeshIndexType::UnsignedInt);
    if(data.expectedJoins && data.expectedJoinsFirstLast)
        CORRADE_COMPARE_AS(mesh.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
            2, 0, 1, 1, 3, 2,
            2, 3, 4, 4, 3, 5, /* join */
            6, 4, 5, 5, 7, 6,
            6, 7, 8, 8, 7, 9, /* join */
            10, 8, 9, 9, 11, 10,
            10, 11, 12, 12, 11, 13, /* join */
            14, 12, 13, 13, 15, 14,
            14, 15, 0, 0, 15, 1, /* join */
        }), TestSuite::Compare::Container);
    else if(data.expectedJoins)
        CORRADE_COMPARE_AS(mesh.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
            2, 0, 1, 1, 3, 2,
            2, 3, 4, 4, 3, 5, /* join */
            6, 4, 5, 5, 7, 6,
            6, 7, 8, 8, 7, 9, /* join */
            10, 8, 9, 9, 11, 10,
            10, 11, 12, 12, 11, 13, /* join */
            14, 12, 13, 13, 15, 14,
        }), TestSuite::Compare::Container);
    else
        CORRADE_COMPARE_AS(mesh.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
            2, 0, 1, 1, 3, 2,
            6, 4, 5, 5, 7, 6,
            10, 8, 9, 9, 11, 10,
            14, 12, 13, 13, 15, 14,
        }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Trade::MeshAttribute::Position));
    CORRADE_COMPARE(mesh.attributeFormat(Trade::MeshAttribute::Position), VertexFormat::Vector2);
    Containers::StridedArrayView1D<const Vector2> positions = mesh.attribute<Vector2>(Trade::MeshAttribute::Position);
    CORRADE_COMPARE_AS(positions, Containers::arrayView<Vector2>({
        {-1.0f, -1.0f}, {-1.0f, -1.0f},
            {+1.0f, -1.0f}, {+1.0f, -1.0f},
        {+1.0f, -1.0f}, {+1.0f, -1.0f},
            {+1.0f, +1.0f}, {+1.0f, +1.0f},
        {+1.0f, +1.0f}, {+1.0f, +1.0f},
            {-1.0f, +1.0f}, {-1.0f, +1.0f},
        {-1.0f, +1.0f}, {-1.0f, +1.0f},
            {-1.0f, -1.0f}, {-1.0f, -1.0f}
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributePreviousPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributeNextPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributeAnnotation));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributePreviousPosition), VertexFormat::Vector2);
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributeNextPosition), VertexFormat::Vector2);
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributeAnnotation), VertexFormat::UnsignedInt);
    if(data.expectedJoins && data.expectedJoinsFirstLast) {
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::LineMeshAttributePreviousPosition), Containers::arrayView<Vector2>({
            positions[12], positions[12],
                positions[0], positions[0],
            positions[0], positions[0],
                positions[4], positions[4],
            positions[4], positions[4],
                positions[8], positions[8],
            positions[8], positions[8],
                positions[12], positions[12],
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::LineMeshAttributeNextPosition), Containers::arrayView<Vector2>({
            positions[2], positions[2],
                positions[6], positions[6],
            positions[6], positions[6],
                positions[10], positions[10],
            positions[10], positions[10],
                positions[14], positions[14],
            positions[14], positions[14],
                positions[2], positions[2],
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::LineMeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Begin|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Up|
                    Shaders::LineVertexAnnotation::Join,
                        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Begin|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Up|
                    Shaders::LineVertexAnnotation::Join,
                        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Begin|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Up|
                    Shaders::LineVertexAnnotation::Join,
                        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Begin|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Up|
                    Shaders::LineVertexAnnotation::Join,
                        Shaders::LineVertexAnnotation::Join,
        }), TestSuite::Compare::Container);
    } else if(data.expectedJoins) {
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::LineMeshAttributePreviousPosition), Containers::arrayView<Vector2>({
            {}, {},
                positions[0], positions[0],
            positions[0], positions[0],
                positions[4], positions[4],
            positions[4], positions[4],
                positions[8], positions[8],
            positions[8], positions[8],
                positions[12], positions[12],
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::LineMeshAttributeNextPosition), Containers::arrayView<Vector2>({
            positions[2], positions[2],
                positions[6], positions[6],
            positions[6], positions[6],
                positions[10], positions[10],
            positions[10], positions[10],
                positions[14], positions[14],
            positions[14], positions[14],
                {}, {}
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::LineMeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin,
                Shaders::LineVertexAnnotation::Begin,
                    Shaders::LineVertexAnnotation::Up|
                    Shaders::LineVertexAnnotation::Join,
                        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Begin|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Up|
                    Shaders::LineVertexAnnotation::Join,
                        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Begin|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Up|
                    Shaders::LineVertexAnnotation::Join,
                        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Begin|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Up,
                        {},
        }), TestSuite::Compare::Container);
    } else {
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::LineMeshAttributePreviousPosition), Containers::arrayView<Vector2>({
            {}, {},
                positions[0], positions[0],
            {}, {},
                positions[4], positions[4],
            {}, {},
                positions[8], positions[8],
            {}, {},
                positions[12], positions[12],
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::LineMeshAttributeNextPosition), Containers::arrayView<Vector2>({
            positions[2], positions[2],
                {}, {},
            positions[6], positions[6],
                {}, {},
            positions[10], positions[10],
                {}, {},
            positions[14], positions[14],
                {}, {},
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::LineMeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin,
                Shaders::LineVertexAnnotation::Begin,
                    Shaders::LineVertexAnnotation::Up,
                        {},
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin,
                Shaders::LineVertexAnnotation::Begin,
                    Shaders::LineVertexAnnotation::Up,
                        {},
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin,
                Shaders::LineVertexAnnotation::Begin,
                    Shaders::LineVertexAnnotation::Up,
                        {},
            Shaders::LineVertexAnnotation::Up|
            Shaders::LineVertexAnnotation::Begin,
                Shaders::LineVertexAnnotation::Begin,
                    Shaders::LineVertexAnnotation::Up,
                        {},
        }), TestSuite::Compare::Container);
    }
}

void GenerateLinesTest::extraAttributes() {
    const struct Vertex {
        Color3ub color;
        Vector3b position;
        UnsignedShort objectId;
    } vertexData[]{
        {0xdcdcdc_rgb, {-1, -1, 0}, 156},
        {0xdcdcdc_rgb, {+1, -1, 1}, 223},
        {0x2f83cc_rgb, {+1, +1, 0}, 999},
        {0x2f83cc_rgb, {-1, +1, 1}, 768}
    };
    auto vertices = Containers::stridedArrayView(vertexData);

    Trade::MeshData lineMesh{MeshPrimitive::LineLoop,
        {}, vertexData, {
            /* Having position not first to catch accidental use of first
               attribute as position */
            Trade::MeshAttributeData{Trade::MeshAttribute::Color, vertices.slice(&Vertex::color)},
            Trade::MeshAttributeData{Trade::MeshAttribute::Position, vertices.slice(&Vertex::position)},
            Trade::MeshAttributeData{Trade::MeshAttribute::ObjectId, vertices.slice(&Vertex::objectId)},
        }};

    Trade::MeshData mesh = generateLines(lineMesh);
    CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(mesh.attributeCount(), 6);

    CORRADE_VERIFY(mesh.isIndexed());
    CORRADE_COMPARE(mesh.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(mesh.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        2, 0, 1, 1, 3, 2,
        2, 3, 4, 4, 3, 5, /* join */
        6, 4, 5, 5, 7, 6,
        6, 7, 8, 8, 7, 9, /* join */
        10, 8, 9, 9, 11, 10,
        10, 11, 12, 12, 11, 13, /* join */
        14, 12, 13, 13, 15, 14,
        14, 15, 0, 0, 15, 1, /* join */
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Trade::MeshAttribute::Position));
    CORRADE_COMPARE(mesh.attributeFormat(Trade::MeshAttribute::Position), VertexFormat::Vector3b);
    Containers::StridedArrayView1D<const Vector3b> positions = mesh.attribute<Vector3b>(Trade::MeshAttribute::Position);
    CORRADE_COMPARE_AS(positions, Containers::arrayView<Vector3b>({
        {-1, -1, 0}, {-1, -1, 0},
            {+1, -1, 1}, {+1, -1, 1},
        {+1, -1, 1}, {+1, -1, 1},
            {+1, +1, 0}, {+1, +1, 0},
        {+1, +1, 0}, {+1, +1, 0},
            {-1, +1, 1}, {-1, +1, 1},
        {-1, +1, 1}, {-1, +1, 1},
            {-1, -1, 0}, {-1, -1, 0}
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Trade::MeshAttribute::Color));
    CORRADE_COMPARE(mesh.attributeFormat(Trade::MeshAttribute::Color), VertexFormat::Vector3ubNormalized);
    CORRADE_COMPARE_AS(mesh.attribute<Color3ub>(Trade::MeshAttribute::Color), Containers::arrayView<Color3ub>({
        0xdcdcdc_rgb, 0xdcdcdc_rgb,
            0xdcdcdc_rgb, 0xdcdcdc_rgb,
        0xdcdcdc_rgb, 0xdcdcdc_rgb,
            0x2f83cc_rgb, 0x2f83cc_rgb,
        0x2f83cc_rgb, 0x2f83cc_rgb,
            0x2f83cc_rgb, 0x2f83cc_rgb,
        0x2f83cc_rgb, 0x2f83cc_rgb,
            0xdcdcdc_rgb, 0xdcdcdc_rgb,
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Trade::MeshAttribute::ObjectId));
    CORRADE_COMPARE(mesh.attributeFormat(Trade::MeshAttribute::ObjectId), VertexFormat::UnsignedShort);
    CORRADE_COMPARE_AS(mesh.attribute<UnsignedShort>(Trade::MeshAttribute::ObjectId), Containers::arrayView<UnsignedShort>({
        156, 156,
            223, 223,
        223, 223,
            999, 999,
        999, 999,
            768, 768,
        768, 768,
            156, 156,
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributePreviousPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributeNextPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributeAnnotation));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributePreviousPosition), VertexFormat::Vector3b);
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributeNextPosition), VertexFormat::Vector3b);
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributeAnnotation), VertexFormat::UnsignedInt);

    CORRADE_COMPARE_AS(mesh.attribute<Vector3b>(Implementation::LineMeshAttributePreviousPosition), Containers::arrayView<Vector3b>({
        positions[12], positions[12],
            positions[0], positions[0],
        positions[0], positions[0],
            positions[4], positions[4],
        positions[4], positions[4],
            positions[8], positions[8],
        positions[8], positions[8],
            positions[12], positions[12],
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.attribute<Vector3b>(Implementation::LineMeshAttributeNextPosition), Containers::arrayView<Vector3b>({
        positions[2], positions[2],
            positions[6], positions[6],
        positions[6], positions[6],
            positions[10], positions[10],
        positions[10], positions[10],
            positions[14], positions[14],
        positions[14], positions[14],
            positions[2], positions[2],
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::LineMeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
        Shaders::LineVertexAnnotation::Up|
        Shaders::LineVertexAnnotation::Begin|
        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Up|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Join,
        Shaders::LineVertexAnnotation::Up|
        Shaders::LineVertexAnnotation::Begin|
        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Up|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Join,
        Shaders::LineVertexAnnotation::Up|
        Shaders::LineVertexAnnotation::Begin|
        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Up|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Join,
        Shaders::LineVertexAnnotation::Up|
        Shaders::LineVertexAnnotation::Begin|
        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Up|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Join,
    }), TestSuite::Compare::Container);
}

void GenerateLinesTest::zeroVertices() {
    Trade::MeshData lineMesh{MeshPrimitive::LineLoop, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3usNormalized, nullptr}
    }};

    Trade::MeshData mesh = generateLines(lineMesh);
    CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(mesh.attributeCount(), 4);
    CORRADE_COMPARE(mesh.vertexCount(), 0);

    CORRADE_VERIFY(mesh.hasAttribute(Trade::MeshAttribute::Position));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributePreviousPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributePreviousPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributeAnnotation));
}

void GenerateLinesTest::twoVerticesStrip() {
    Vector2 positionData[]{
        {-1.0f, 0.0f},
        {+1.0f, 0.0f}
    };

    Trade::MeshData lineMesh{MeshPrimitive::LineStrip, {}, positionData, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(positionData)}
    }};

    Trade::MeshData mesh = generateLines(lineMesh);
    CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(mesh.attributeCount(), 4);

    CORRADE_VERIFY(mesh.isIndexed());
    CORRADE_COMPARE(mesh.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(mesh.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        2, 0, 1, 1, 3, 2,
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Trade::MeshAttribute::Position));
    CORRADE_COMPARE(mesh.attributeFormat(Trade::MeshAttribute::Position), VertexFormat::Vector2);
    Containers::StridedArrayView1D<const Vector2> positions = mesh.attribute<Vector2>(Trade::MeshAttribute::Position);
    CORRADE_COMPARE_AS(positions, Containers::arrayView<Vector2>({
        {-1.0f, 0.0f}, {-1.0f, 0.0f},
            {+1.0f, 0.0f}, {+1.0f, 0.0f},
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributePreviousPosition));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributePreviousPosition), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::LineMeshAttributePreviousPosition), Containers::arrayView<Vector2>({
        {}, {},
            positions[0], positions[0],
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributePreviousPosition));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributeNextPosition), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::LineMeshAttributeNextPosition), Containers::arrayView<Vector2>({
        positions[2], positions[2],
            {}, {},
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributeAnnotation));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributeAnnotation), VertexFormat::UnsignedInt);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::LineMeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
        Shaders::LineVertexAnnotation::Up|
        Shaders::LineVertexAnnotation::Begin,
            Shaders::LineVertexAnnotation::Begin,
                Shaders::LineVertexAnnotation::Up,
                    {},
    }), TestSuite::Compare::Container);
}

void GenerateLinesTest::twoVerticesLoop() {
    Vector2 positionData[]{
        {-1.0f, 0.0f},
        {+1.0f, 0.0f}
    };

    Trade::MeshData lineMesh{MeshPrimitive::LineLoop, {}, positionData, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(positionData)}
    }};

    Trade::MeshData mesh = generateLines(lineMesh);
    CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(mesh.attributeCount(), 4);

    CORRADE_VERIFY(mesh.isIndexed());
    CORRADE_COMPARE(mesh.indexType(), MeshIndexType::UnsignedInt);
    CORRADE_COMPARE_AS(mesh.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        2, 0, 1, 1, 3, 2,
        2, 3, 4, 4, 3, 5, /* join */
        6, 4, 5, 5, 7, 6,
        6, 7, 0, 0, 7, 1, /* join */
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Trade::MeshAttribute::Position));
    CORRADE_COMPARE(mesh.attributeFormat(Trade::MeshAttribute::Position), VertexFormat::Vector2);
    Containers::StridedArrayView1D<const Vector2> positions = mesh.attribute<Vector2>(Trade::MeshAttribute::Position);
    CORRADE_COMPARE_AS(positions, Containers::arrayView<Vector2>({
        {-1.0f, 0.0f}, {-1.0f, 0.0f},
            {+1.0f, 0.0f}, {+1.0f, 0.0f},
        {+1.0f, 0.0f}, {+1.0f, 0.0f},
            {-1.0f, 0.0f}, {-1.0f, 0.0f},
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributePreviousPosition));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributePreviousPosition), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::LineMeshAttributePreviousPosition), Containers::arrayView<Vector2>({
        positions[4], positions[4],
            positions[0], positions[0],
        positions[0], positions[0],
            positions[4], positions[4],
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributePreviousPosition));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributeNextPosition), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::LineMeshAttributeNextPosition), Containers::arrayView<Vector2>({
        positions[2], positions[2],
            positions[6], positions[6],
        positions[6], positions[6],
            positions[2], positions[2],
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::LineMeshAttributeAnnotation));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::LineMeshAttributeAnnotation), VertexFormat::UnsignedInt);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::LineMeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
        Shaders::LineVertexAnnotation::Up|
        Shaders::LineVertexAnnotation::Begin|
        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Up|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Join,
        Shaders::LineVertexAnnotation::Up|
        Shaders::LineVertexAnnotation::Begin|
        Shaders::LineVertexAnnotation::Join,
            Shaders::LineVertexAnnotation::Begin|
            Shaders::LineVertexAnnotation::Join,
                Shaders::LineVertexAnnotation::Up|
                Shaders::LineVertexAnnotation::Join,
                    Shaders::LineVertexAnnotation::Join,
    }), TestSuite::Compare::Container);
}

void GenerateLinesTest::notLines() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3 positions[3]{};

    Containers::String out;
    Error redirectError{&out};
    generateLines(Trade::MeshData{MeshPrimitive::TriangleFan, {}, positions, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(positions)}
    }});
    CORRADE_COMPARE(out, "MeshTools::generateLines(): expected a line primitive, got MeshPrimitive::TriangleFan\n");
}

void GenerateLinesTest::noAttributes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    generateLines(Trade::MeshData{MeshPrimitive::Lines, 12});
    CORRADE_COMPARE(out, "MeshTools::generateLines(): the mesh has no positions\n");
}

void GenerateLinesTest::noPositionAttribute() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Vector3 colors[2]{};

    Containers::String out;
    Error redirectError{&out};
    generateLines(Trade::MeshData{MeshPrimitive::Lines, {}, colors, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Color, Containers::stridedArrayView(colors)}
    }});
    CORRADE_COMPARE(out, "MeshTools::generateLines(): the mesh has no positions\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::GenerateLinesTest)
