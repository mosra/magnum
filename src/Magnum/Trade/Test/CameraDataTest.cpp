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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Trade/CameraData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct CameraDataTest: TestSuite::Tester {
    explicit CameraDataTest();

    void construct3DFoV();
    void construct3DSize();

    void construct2D();
    void construct2DFoV();
    void construct2DNearFar();

    void constructCopy();
    void constructMove();

    void fovNonPerspective();

    void debugType();
};

CameraDataTest::CameraDataTest() {
    addTests({&CameraDataTest::construct3DFoV,
              &CameraDataTest::construct3DSize,

              &CameraDataTest::construct2D,
              &CameraDataTest::construct2DFoV,
              &CameraDataTest::construct2DNearFar,

              &CameraDataTest::constructCopy,
              &CameraDataTest::constructMove,

              &CameraDataTest::fovNonPerspective,

              &CameraDataTest::debugType});
}

using namespace Math::Literals;

void CameraDataTest::construct3DFoV() {
    const int a{};
    CameraData data{CameraType::Perspective3D, 25.0_degf, 4.0f/3.0f, 0.1f, 1000.0f, &a};

    CORRADE_COMPARE(data.type(), CameraType::Perspective3D);
    CORRADE_COMPARE(data.size(), (Vector2{0.0443389f, 0.0332542f}));
    CORRADE_COMPARE(data.fov(), 25.0_degf);
    CORRADE_COMPARE(data.aspectRatio(), 1.333333f);
    CORRADE_COMPARE(data.near(), 0.1f);
    CORRADE_COMPARE(data.far(), 1000.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void CameraDataTest::construct3DSize() {
    const int a{};
    CameraData data{CameraType::Orthographic3D, {0.03f, 0.04f}, 0.01f, 1000.0f, &a};

    CORRADE_COMPARE(data.type(), CameraType::Orthographic3D);
    CORRADE_COMPARE(data.size(), (Vector2{0.03f, 0.04f}));
    CORRADE_COMPARE(data.aspectRatio(), 0.75f);
    CORRADE_COMPARE(data.near(), 0.01f);
    CORRADE_COMPARE(data.far(), 1000.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void CameraDataTest::construct2D() {
    const int a{};
    CameraData data{CameraType::Orthographic2D, {4.0f, 2.0f}, {}, {}, &a};

    CORRADE_COMPARE(data.type(), CameraType::Orthographic2D);
    CORRADE_COMPARE(data.size(), (Vector2{4.0f, 2.0f}));
    CORRADE_COMPARE(data.aspectRatio(), 2.0f);
    CORRADE_COMPARE(data.near(), 0.0f);
    CORRADE_COMPARE(data.far(), 0.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void CameraDataTest::construct2DFoV() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    CameraData{CameraType::Orthographic2D, 25.0_degf, 1.0f, 0.001f, 1000.0f};

    CORRADE_COMPARE(out.str(), "Trade::CameraData: only perspective cameras can have FoV specified\n");
}

void CameraDataTest::construct2DNearFar() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    CameraData{CameraType::Orthographic2D, {3.0f, 4.0f}, 0.001f, 1000.0f};

    CORRADE_COMPARE(out.str(), "Trade::CameraData: 2D cameras can't be specified with near and far clipping planes\n");
}

void CameraDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<CameraData, const CameraData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<CameraData, const CameraData&>{}));
}

void CameraDataTest::constructMove() {
    const int a{};
    CameraData data{CameraType::Perspective3D, 25.0_degf, 2.35f, 1.0f, 1000.0f, &a};

    CameraData b{std::move(data)};
    CORRADE_COMPARE(b.type(), CameraType::Perspective3D);
    CORRADE_COMPARE(b.size(), (Vector2{0.443389f, 0.188676f}));
    CORRADE_COMPARE(b.fov(), 25.0_degf);
    CORRADE_COMPARE(b.aspectRatio(), 2.35f);
    CORRADE_COMPARE(b.near(), 1.0f);
    CORRADE_COMPARE(b.far(), 1000.0f);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    CameraData d{CameraType::Orthographic3D, {2.0f, 1.0f}, 0.5f, 10.0f, &c};
    d = std::move(b);
    CORRADE_COMPARE(b.type(), CameraType::Perspective3D);
    CORRADE_COMPARE(b.size(), (Vector2{0.443389f, 0.188676f}));
    CORRADE_COMPARE(b.fov(), 25.0_degf);
    CORRADE_COMPARE(b.aspectRatio(), 2.35f);
    CORRADE_COMPARE(d.near(), 1.0f);
    CORRADE_COMPARE(d.far(), 1000.0f);
    CORRADE_COMPARE(d.importerState(), &a);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<CameraData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<CameraData>::value);
}

void CameraDataTest::fovNonPerspective() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};

    CameraData a{CameraType::Orthographic2D, {3.0f, 4.0f}, {}, {}};
    a.fov();

    CORRADE_COMPARE(out.str(), "Trade::CameraData::fov(): the camera is not perspective\n");
}

void CameraDataTest::debugType() {
    std::ostringstream out;

    Debug{&out} << CameraType::Orthographic3D << CameraType(0xde);
    CORRADE_COMPARE(out.str(), "Trade::CameraType::Orthographic3D Trade::CameraType(0xde)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::CameraDataTest)
