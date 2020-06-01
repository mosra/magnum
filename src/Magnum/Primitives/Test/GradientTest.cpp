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
#include "Magnum/Math/Color.h"
#include "Magnum/Primitives/Gradient.h"
#include "Magnum/Trade/MeshData.h"
#include "Magnum/Primitives/Square.h"
#include "Magnum/Primitives/Plane.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct GradientTest: TestSuite::Tester {
    explicit GradientTest();

    void gradient2D();
    void gradient2DHorizontal();
    void gradient2DVertical();

    void gradient3D();
    void gradient3DHorizontal();
    void gradient3DVertical();
};

GradientTest::GradientTest() {
    addTests({&GradientTest::gradient2D,
              &GradientTest::gradient2DHorizontal,
              &GradientTest::gradient2DVertical,

              &GradientTest::gradient3D,
              &GradientTest::gradient3DHorizontal,
              &GradientTest::gradient3DVertical});
}

using namespace Magnum::Math::Literals;

void GradientTest::gradient2D() {
    /* The corners sould have 0.2, 0.4, 0.6, 0.8 blends */
    Trade::MeshData gradient = Primitives::gradient2D(
        {-1.0f, 2.0f}, {0.2f, 0.6f, 1.0f},
        {1.0f, -2.0f}, {0.4f, 1.0f, 0.0f});

    CORRADE_COMPARE(gradient.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!gradient.isIndexed());
    CORRADE_COMPARE(gradient.attributeCount(), 2);

    /* Positions should be the same as for a square */
    Trade::MeshData square = Primitives::squareSolid();
    CORRADE_COMPARE(gradient.primitive(), square.primitive());
    CORRADE_COMPARE_AS(
        gradient.attribute<Vector2>(Trade::MeshAttribute::Position),
        square.attribute<Vector2>(Trade::MeshAttribute::Position),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(gradient.attribute<Vector2>(Trade::MeshAttribute::Position), Containers::arrayView<Vector2>({
        { 1.0f, -1.0f}, /* Bottom right */
        { 1.0f,  1.0f}, /* Top right */
        {-1.0f, -1.0f}, /* Bottom left */
        {-1.0f,  1.0f}  /* Top left */
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(gradient.attribute<Color4>(Trade::MeshAttribute::Color), Containers::arrayView<Color4>({
        {0.36f, 0.92f, 0.2f}, /* 80% */
        {0.28f, 0.76f, 0.6f}, /* 40% */
        {0.32f, 0.84f, 0.4f}, /* 60% */
        {0.24f, 0.68f, 0.8f}  /* 20% */
    }), TestSuite::Compare::Container);
}

void GradientTest::gradient2DHorizontal() {
    Trade::MeshData gradient = Primitives::gradient2DHorizontal(0xfabcde_srgbf, 0xdeab09_srgbf);

    CORRADE_COMPARE_AS(gradient.attribute<Color4>(Trade::MeshAttribute::Color), Containers::arrayView<Color4>({
        0xdeab09_srgbf,
        0xdeab09_srgbf,
        0xfabcde_srgbf,
        0xfabcde_srgbf
    }), TestSuite::Compare::Container);
}

void GradientTest::gradient2DVertical() {
    Trade::MeshData gradient = Primitives::gradient2DVertical(0xfabcde_srgbf, 0xdeab09_srgbf);

    CORRADE_COMPARE_AS(gradient.attribute<Color4>(Trade::MeshAttribute::Color), Containers::arrayView<Color4>({
        0xfabcde_srgbf,
        0xdeab09_srgbf,
        0xfabcde_srgbf,
        0xdeab09_srgbf
    }), TestSuite::Compare::Container);
}

void GradientTest::gradient3D() {
    /* The corners sould have 0.2, 0.4, 0.6, 0.8 blends */
    Trade::MeshData gradient = Primitives::gradient3D(
        {-1.0f, 2.0f, -1.5f}, {0.2f, 0.6f, 1.0f},
        {1.0f, -2.0f, -1.5f}, {0.4f, 1.0f, 0.0f});

    CORRADE_COMPARE(gradient.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!gradient.isIndexed());
    CORRADE_COMPARE(gradient.attributeCount(), 3);

    /* Positions should be the same as for a plane */
    Trade::MeshData plane = Primitives::planeSolid();
    CORRADE_COMPARE(gradient.primitive(), plane.primitive());
    CORRADE_COMPARE_AS(
        gradient.attribute<Vector3>(Trade::MeshAttribute::Position),
        plane.attribute<Vector3>(Trade::MeshAttribute::Position),
        TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(gradient.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        { 1.0f, -1.0f, 0.0f}, /* Bottom right */
        { 1.0f,  1.0f, 0.0f}, /* Top right */
        {-1.0f, -1.0f, 0.0f}, /* Bottom left */
        {-1.0f,  1.0f, 0.0f}  /* Top left */
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(gradient.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(gradient.attribute<Color4>(Trade::MeshAttribute::Color), Containers::arrayView<Color4>({
        {0.36f, 0.92f, 0.2f}, /* 80% */
        {0.28f, 0.76f, 0.6f}, /* 40% */
        {0.32f, 0.84f, 0.4f}, /* 60% */
        {0.24f, 0.68f, 0.8f}  /* 20% */
    }), TestSuite::Compare::Container);
}

void GradientTest::gradient3DHorizontal() {
    Trade::MeshData gradient = Primitives::gradient3DHorizontal(0xfabcde_srgbf, 0xdeab09_srgbf);

    CORRADE_COMPARE_AS(gradient.attribute<Color4>(Trade::MeshAttribute::Color), Containers::arrayView<Color4>({
        0xdeab09_srgbf,
        0xdeab09_srgbf,
        0xfabcde_srgbf,
        0xfabcde_srgbf
    }), TestSuite::Compare::Container);
}

void GradientTest::gradient3DVertical() {
    Trade::MeshData gradient = Primitives::gradient3DVertical(0xfabcde_srgbf, 0xdeab09_srgbf);

    CORRADE_COMPARE_AS(gradient.attribute<Color4>(Trade::MeshAttribute::Color), Containers::arrayView<Color4>({
        0xfabcde_srgbf,
        0xdeab09_srgbf,
        0xfabcde_srgbf,
        0xdeab09_srgbf
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::GradientTest)
