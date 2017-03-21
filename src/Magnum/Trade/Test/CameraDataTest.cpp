/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Trade/CameraData.h"

namespace Magnum { namespace Trade { namespace Test {

struct CameraDataTest: TestSuite::Tester {
    explicit CameraDataTest();

    void construct();
    void constructDefaults();
    void constructCopy();
    void constructMove();
};

namespace {

using namespace Math::Literals;

enum: std::size_t { ConstructDefaultsDataCount = 3 };

struct {
    const char* name;
    Rad fov, expectedFov;
    Float near, expectedNear;
    Float far, expectedFar;
} ConstructDefaultsData[ConstructDefaultsDataCount]{
    {"fov", Rad{Constants::nan()}, 35.0_degf, 0.5f, 0.5f, 120.0f, 120.0f},
    {"near", 25.0_degf, 25.0_degf, Constants::nan(), 0.01f, 120.0f, 120.0f},
    {"far", 25.0_degf, 25.0_degf, 0.5f, 0.5f, Constants::nan(), 100.0f}
};

}

CameraDataTest::CameraDataTest() {
    addTests({&CameraDataTest::construct});

    addInstancedTests({&CameraDataTest::constructDefaults}, ConstructDefaultsDataCount);

    addTests({&CameraDataTest::constructCopy,
              &CameraDataTest::constructMove});
}

void CameraDataTest::construct() {
    const int a{};
    CameraData data{25.0_degf, 0.001f, 1000.0f, &a};

    CORRADE_COMPARE(data.fov(), 25.0_degf);
    CORRADE_COMPARE(data.near(), 0.001f);
    CORRADE_COMPARE(data.far(), 1000.0f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void CameraDataTest::constructDefaults() {
    setTestCaseDescription(ConstructDefaultsData[testCaseInstanceId()].name);

    const int a{};
    CameraData data{
        ConstructDefaultsData[testCaseInstanceId()].fov,
        ConstructDefaultsData[testCaseInstanceId()].near,
        ConstructDefaultsData[testCaseInstanceId()].far,
        &a};

    CORRADE_COMPARE(data.fov(), ConstructDefaultsData[testCaseInstanceId()].expectedFov);
    CORRADE_COMPARE(data.near(), ConstructDefaultsData[testCaseInstanceId()].expectedNear);
    CORRADE_COMPARE(data.far(), ConstructDefaultsData[testCaseInstanceId()].expectedFar);
    CORRADE_COMPARE(data.importerState(), &a);
}

void CameraDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<CameraData, const CameraData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<CameraData, const CameraData&>{}));
}

void CameraDataTest::constructMove() {
    const int a{};
    CameraData data{25.0_degf, 0.001f, 1000.0f, &a};

    CameraData b{std::move(data)};
    CORRADE_COMPARE(b.fov(), 25.0_degf);
    CORRADE_COMPARE(b.near(), 0.001f);
    CORRADE_COMPARE(b.far(), 1000.0f);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    CameraData d{75.0_degf, 0.5f, 10.0f, &c};
    d = std::move(b);
    CORRADE_COMPARE(d.fov(), 25.0_degf);
    CORRADE_COMPARE(d.near(), 0.001f);
    CORRADE_COMPARE(d.far(), 1000.0f);
    CORRADE_COMPARE(d.importerState(), &a);
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::CameraDataTest)
