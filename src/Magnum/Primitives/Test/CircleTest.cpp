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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Primitives/Circle.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct CircleTest: TestSuite::Tester {
    explicit CircleTest();

    void solid2D();
    void solid3D();

    void wireframe2D();
    void wireframe3D();
};

constexpr struct {
    const char* name;
    Circle2DFlags flags;
} Solid2DData[] {
    {"", Circle2DFlags{}},
    {"texture coordinates", Circle2DFlag::TextureCoordinates}
};

constexpr struct {
    const char* name;
    Circle3DFlags flags;
} Solid3DData[] {
    {"", Circle3DFlags{}},
    {"texture coordinates", Circle3DFlag::TextureCoordinates},
    {"tangents", Circle3DFlag::Tangents},
    {"both", Circle3DFlag::TextureCoordinates|Circle3DFlag::Tangents}
};

CircleTest::CircleTest() {
    addInstancedTests({&CircleTest::solid2D},
        Containers::arraySize(Solid2DData));

    addInstancedTests({&CircleTest::solid3D},
        Containers::arraySize(Solid3DData));

    addTests({&CircleTest::wireframe2D,
              &CircleTest::wireframe3D});
}

void CircleTest::solid2D() {
    auto&& data = Solid2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::MeshData circle = Primitives::circle2DSolid(8, data.flags);

    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE_AS(circle.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView<Vector2>({
        { 0.0f,  0.0f},
        { 1.0f,  0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        { 0.0f,  1.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        {-1.0f,  0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 0.0f, -1.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 1.0f,  0.0f}
    }), TestSuite::Compare::Container);

    if(data.flags & Circle2DFlag::TextureCoordinates) {
        CORRADE_COMPARE_AS(circle.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView<Vector2>({
            {0.5f, 0.5f},
            {1.0f, 0.5f}, {0.5f + Constants::sqrt2()/4.0f, 0.5f + Constants::sqrt2()/4.0f},
            {0.5f, 1.0f}, {0.5f - Constants::sqrt2()/4.0f, 0.5f + Constants::sqrt2()/4.0f},
            {0.0f, 0.5f}, {0.5f - Constants::sqrt2()/4.0f, 0.5f - Constants::sqrt2()/4.0f},
            {0.5f, 0.0f}, {0.5f + Constants::sqrt2()/4.0f, 0.5f - Constants::sqrt2()/4.0f},
            {1.0f, 0.5f}
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!circle.hasAttribute(Trade::MeshAttribute::TextureCoordinates));
}

void CircleTest::solid3D() {
    auto&& data = Solid3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::MeshData circle = Primitives::circle3DSolid(8, data.flags);

    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::TriangleFan);
    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE_AS(circle.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        { 0.0f,  0.0f, 0.0f},
        { 1.0f,  0.0f, 0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f,  1.0f, 0.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        {-1.0f,  0.0f, 0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f, -1.0f, 0.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 1.0f,  0.0f, 0.0f}
    }), TestSuite::Compare::Container);

    if(data.flags & Circle3DFlag::Tangents) {
        CORRADE_COMPARE_AS(circle.attribute<Vector4>(Trade::MeshAttribute::Tangent), Containers::arrayView<Vector4>({
            {1.0f,  0.0f, 0.0f, 1.0f},
            {1.0f,  0.0f, 0.0f, 1.0f},
            {1.0f,  0.0f, 0.0f, 1.0f},
            {1.0f,  0.0f, 0.0f, 1.0f},
            {1.0f,  0.0f, 0.0f, 1.0f},
            {1.0f,  0.0f, 0.0f, 1.0f},
            {1.0f,  0.0f, 0.0f, 1.0f},
            {1.0f,  0.0f, 0.0f, 1.0f},
            {1.0f,  0.0f, 0.0f, 1.0f},
            {1.0f,  0.0f, 0.0f, 1.0f}
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!circle.hasAttribute(Trade::MeshAttribute::Tangent));

    CORRADE_COMPARE_AS(circle.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f},
        { 0.0f,  0.0f, 1.0f}
    }), TestSuite::Compare::Container);

    if(data.flags & Circle3DFlag::TextureCoordinates) {
        CORRADE_COMPARE_AS(circle.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView<Vector2>({
            {0.5f, 0.5f},
            {1.0f, 0.5f}, {0.5f + Constants::sqrt2()/4.0f, 0.5f + Constants::sqrt2()/4.0f},
            {0.5f, 1.0f}, {0.5f - Constants::sqrt2()/4.0f, 0.5f + Constants::sqrt2()/4.0f},
            {0.0f, 0.5f}, {0.5f - Constants::sqrt2()/4.0f, 0.5f - Constants::sqrt2()/4.0f},
            {0.5f, 0.0f}, {0.5f + Constants::sqrt2()/4.0f, 0.5f - Constants::sqrt2()/4.0f},
            {1.0f, 0.5f}
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!circle.hasAttribute(Trade::MeshAttribute::TextureCoordinates));

    if(data.flags & Circle3DFlag::Tangents) {
        auto tangents = circle.attribute<Vector4>(Trade::MeshAttribute::Tangent);
        auto normals = circle.attribute<Vector3>(Trade::MeshAttribute::Normal);
        for(std::size_t i = 0; i != tangents.size(); ++i) {
            CORRADE_ITERATION(i);
            CORRADE_ITERATION(tangents[i]);
            CORRADE_ITERATION(normals[i]);
            CORRADE_VERIFY(tangents[i].xyz().isNormalized());
            CORRADE_VERIFY(normals[i].isNormalized());
            CORRADE_COMPARE(Math::dot(tangents[i].xyz(), normals[i]), 0.0f);
        }
    }
}

void CircleTest::wireframe2D() {
    Trade::MeshData circle = Primitives::circle2DWireframe(8);

    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.attributeCount(), 1);
    CORRADE_COMPARE_AS(circle.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView<Vector2>({
        { 1.0f,  0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        { 0.0f,  1.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f},
        {-1.0f,  0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f},
        { 0.0f, -1.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f}
    }), TestSuite::Compare::Container);
}

void CircleTest::wireframe3D() {
    Trade::MeshData circle = Primitives::circle3DWireframe(8);

    CORRADE_COMPARE(circle.primitive(), MeshPrimitive::LineLoop);
    CORRADE_VERIFY(!circle.isIndexed());
    CORRADE_COMPARE(circle.attributeCount(), 1);
    CORRADE_COMPARE_AS(circle.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        { 1.0f,  0.0f, 0.0f}, { Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f,  1.0f, 0.0f}, {-Constants::sqrt2()/2.0f,  Constants::sqrt2()/2.0f, 0.0f},
        {-1.0f,  0.0f, 0.0f}, {-Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f},
        { 0.0f, -1.0f, 0.0f}, { Constants::sqrt2()/2.0f, -Constants::sqrt2()/2.0f, 0.0f}
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CircleTest)
