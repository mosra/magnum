/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/Math/PackingBatch.h"
#include "Magnum/MeshTools/Implementation/GenerateLines.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct CompileLinesTest: TestSuite::Tester {
    explicit CompileLinesTest();

    template<class T> void oneLoop();

    void extraAttributes();
    void zeroVertices();
    void twoVerticesStrip();
    void twoVerticesLoop();
    /* Non-line primitives and absence of position attribute tested in
       CompileLinesGLTest, to verify it's not accessed earlier than the
       assertion */
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

CompileLinesTest::CompileLinesTest() {
    addInstancedTests<CompileLinesTest>({
        &CompileLinesTest::oneLoop<UnsignedInt>,
        &CompileLinesTest::oneLoop<UnsignedShort>,
        &CompileLinesTest::oneLoop<UnsignedByte>},
        Containers::arraySize(OneLoopData));

    addTests({&CompileLinesTest::extraAttributes,
              &CompileLinesTest::zeroVertices,
              &CompileLinesTest::twoVerticesStrip,
              &CompileLinesTest::twoVerticesLoop});
}

template<class T> void CompileLinesTest::oneLoop() {
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

    Trade::MeshData mesh = Implementation::generateLines(lineMesh);
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

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributePreviousPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributeNextPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributeAnnotation));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributePreviousPosition), VertexFormat::Vector2);
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributeNextPosition), VertexFormat::Vector2);
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributeAnnotation), VertexFormat::UnsignedInt);
    if(data.expectedJoins && data.expectedJoinsFirstLast) {
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::MeshAttributePreviousPosition), Containers::arrayView<Vector2>({
            positions[12], positions[12],
                positions[0], positions[0],
            positions[0], positions[0],
                positions[4], positions[4],
            positions[4], positions[4],
                positions[8], positions[8],
            positions[8], positions[8],
                positions[12], positions[12],
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::MeshAttributeNextPosition), Containers::arrayView<Vector2>({
            positions[2], positions[2],
                positions[6], positions[6],
            positions[6], positions[6],
                positions[10], positions[10],
            positions[10], positions[10],
                positions[14], positions[14],
            positions[14], positions[14],
                positions[2], positions[2],
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::MeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
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
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::MeshAttributePreviousPosition), Containers::arrayView<Vector2>({
            {}, {},
                positions[0], positions[0],
            positions[0], positions[0],
                positions[4], positions[4],
            positions[4], positions[4],
                positions[8], positions[8],
            positions[8], positions[8],
                positions[12], positions[12],
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::MeshAttributeNextPosition), Containers::arrayView<Vector2>({
            positions[2], positions[2],
                positions[6], positions[6],
            positions[6], positions[6],
                positions[10], positions[10],
            positions[10], positions[10],
                positions[14], positions[14],
            positions[14], positions[14],
                {}, {}
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::MeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
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
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::MeshAttributePreviousPosition), Containers::arrayView<Vector2>({
            {}, {},
                positions[0], positions[0],
            {}, {},
                positions[4], positions[4],
            {}, {},
                positions[8], positions[8],
            {}, {},
                positions[12], positions[12],
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::MeshAttributeNextPosition), Containers::arrayView<Vector2>({
            positions[2], positions[2],
                {}, {},
            positions[6], positions[6],
                {}, {},
            positions[10], positions[10],
                {}, {},
            positions[14], positions[14],
                {}, {},
        }), TestSuite::Compare::Container);
        CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::MeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
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

void CompileLinesTest::extraAttributes() {
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

    Trade::MeshData mesh = Implementation::generateLines(lineMesh);
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

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributePreviousPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributeNextPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributeAnnotation));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributePreviousPosition), VertexFormat::Vector3b);
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributeNextPosition), VertexFormat::Vector3b);
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributeAnnotation), VertexFormat::UnsignedInt);

    CORRADE_COMPARE_AS(mesh.attribute<Vector3b>(Implementation::MeshAttributePreviousPosition), Containers::arrayView<Vector3b>({
        positions[12], positions[12],
            positions[0], positions[0],
        positions[0], positions[0],
            positions[4], positions[4],
        positions[4], positions[4],
            positions[8], positions[8],
        positions[8], positions[8],
            positions[12], positions[12],
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS(mesh.attribute<Vector3b>(Implementation::MeshAttributeNextPosition), Containers::arrayView<Vector3b>({
        positions[2], positions[2],
            positions[6], positions[6],
        positions[6], positions[6],
            positions[10], positions[10],
        positions[10], positions[10],
            positions[14], positions[14],
        positions[14], positions[14],
            positions[2], positions[2],
    }), TestSuite::Compare::Container);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::MeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
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

void CompileLinesTest::zeroVertices() {
    Trade::MeshData lineMesh{MeshPrimitive::LineLoop, nullptr, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3usNormalized, nullptr}
    }};

    Trade::MeshData mesh = Implementation::generateLines(lineMesh);
    CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::Triangles);
    CORRADE_COMPARE(mesh.attributeCount(), 4);
    CORRADE_COMPARE(mesh.vertexCount(), 0);

    CORRADE_VERIFY(mesh.hasAttribute(Trade::MeshAttribute::Position));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributePreviousPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributePreviousPosition));
    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributeAnnotation));
}

void CompileLinesTest::twoVerticesStrip() {
    Vector2 positionData[]{
        {-1.0f, 0.0f},
        {+1.0f, 0.0f}
    };

    Trade::MeshData lineMesh{MeshPrimitive::LineStrip, {}, positionData, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(positionData)}
    }};

    Trade::MeshData mesh = Implementation::generateLines(lineMesh);
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

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributePreviousPosition));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributePreviousPosition), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::MeshAttributePreviousPosition), Containers::arrayView<Vector2>({
        {}, {},
            positions[0], positions[0],
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributePreviousPosition));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributeNextPosition), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::MeshAttributeNextPosition), Containers::arrayView<Vector2>({
        positions[2], positions[2],
            {}, {},
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributeAnnotation));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributeAnnotation), VertexFormat::UnsignedInt);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::MeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
        Shaders::LineVertexAnnotation::Up|
        Shaders::LineVertexAnnotation::Begin,
            Shaders::LineVertexAnnotation::Begin,
                Shaders::LineVertexAnnotation::Up,
                    {},
    }), TestSuite::Compare::Container);
}

void CompileLinesTest::twoVerticesLoop() {
    Vector2 positionData[]{
        {-1.0f, 0.0f},
        {+1.0f, 0.0f}
    };

    Trade::MeshData lineMesh{MeshPrimitive::LineLoop, {}, positionData, {
        Trade::MeshAttributeData{Trade::MeshAttribute::Position, Containers::stridedArrayView(positionData)}
    }};

    Trade::MeshData mesh = Implementation::generateLines(lineMesh);
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

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributePreviousPosition));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributePreviousPosition), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::MeshAttributePreviousPosition), Containers::arrayView<Vector2>({
        positions[4], positions[4],
            positions[0], positions[0],
        positions[0], positions[0],
            positions[4], positions[4],
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributePreviousPosition));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributeNextPosition), VertexFormat::Vector2);
    CORRADE_COMPARE_AS(mesh.attribute<Vector2>(Implementation::MeshAttributeNextPosition), Containers::arrayView<Vector2>({
        positions[2], positions[2],
            positions[6], positions[6],
        positions[6], positions[6],
            positions[2], positions[2],
    }), TestSuite::Compare::Container);

    CORRADE_VERIFY(mesh.hasAttribute(Implementation::MeshAttributeAnnotation));
    CORRADE_COMPARE(mesh.attributeFormat(Implementation::MeshAttributeAnnotation), VertexFormat::UnsignedInt);
    CORRADE_COMPARE_AS((Containers::arrayCast<1, const Shaders::LineVertexAnnotations>(mesh.attribute(Implementation::MeshAttributeAnnotation))), Containers::arrayView<Shaders::LineVertexAnnotations>({
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

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CompileLinesTest)
