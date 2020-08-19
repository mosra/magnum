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

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Trade/SkinData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct SkinDataTest: TestSuite::Tester {
    explicit SkinDataTest();

    void construct();
    void constructNonOwned();
    void constructDifferentSize();
    void constructCopy();
    void constructMove();

    void release();
};

SkinDataTest::SkinDataTest() {
    addTests({&SkinDataTest::construct,
              &SkinDataTest::constructNonOwned,
              &SkinDataTest::constructDifferentSize,
              &SkinDataTest::constructCopy,
              &SkinDataTest::constructMove,

              &SkinDataTest::release});
}

void SkinDataTest::construct() {
    int state;
    SkinData3D data{{0, 2, 3}, {
        Matrix4::translation(Vector3::zAxis(0.0f)),
        Matrix4::translation(Vector3::zAxis(2.0f)),
        Matrix4::translation(Vector3::zAxis(4.0f)),
    }, &state};

    CORRADE_COMPARE(data.joints()[1], 2);
    CORRADE_COMPARE(data.inverseBindMatrices()[1], Matrix4::translation(Vector3::zAxis(2.0f)));
    CORRADE_COMPARE(data.importerState(), &state);
}

void SkinDataTest::constructNonOwned() {
    int state;
    const UnsignedInt jointData[]{0, 2, 3};
    const Matrix4 inverseBindMatrixData[]{
        Matrix4::translation(Vector3::zAxis(0.0f)),
        Matrix4::translation(Vector3::zAxis(2.0f)),
        Matrix4::translation(Vector3::zAxis(4.0f))
    };
    SkinData3D data{{}, jointData, {}, inverseBindMatrixData, &state};

    CORRADE_COMPARE(data.joints().size(), 3);
    CORRADE_COMPARE(data.joints().data(), jointData);
    CORRADE_COMPARE(data.inverseBindMatrices().size(), 3);
    CORRADE_COMPARE(data.inverseBindMatrices().data(), inverseBindMatrixData);
    CORRADE_COMPARE(data.importerState(), &state);
}

void SkinDataTest::constructDifferentSize() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    SkinData3D data{{0, 2}, {{}, {}, {}}};
    CORRADE_COMPARE(out.str(), "Trade::SkinData: joint and inverse bind matrix arrays have different size, got 2 and 3\n");
}

void SkinDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<SkinData3D, const SkinData3D&>{}));
    CORRADE_VERIFY(!(std::is_assignable<SkinData3D, const SkinData3D&>{}));
}

void SkinDataTest::constructMove() {
    int state;
    SkinData3D a{{0, 2, 3}, {
        Matrix4::translation(Vector3::zAxis(0.0f)),
        Matrix4::translation(Vector3::zAxis(2.0f)),
        Matrix4::translation(Vector3::zAxis(4.0f)),
    }, &state};

    SkinData3D b = std::move(a);
    CORRADE_COMPARE(b.joints()[1], 2);
    CORRADE_COMPARE(b.inverseBindMatrices()[1], Matrix4::translation(Vector3::zAxis(2.0f)));
    CORRADE_COMPARE(b.importerState(), &state);

    SkinData3D c{{}, {}};
    c = std::move(b);
    CORRADE_COMPARE(c.joints()[1], 2);
    CORRADE_COMPARE(c.inverseBindMatrices()[1], Matrix4::translation(Vector3::zAxis(2.0f)));
    CORRADE_COMPARE(c.importerState(), &state);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<SkinData2D>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<SkinData2D>::value);
}

void SkinDataTest::release() {
    Containers::Array<UnsignedInt> joints{Containers::InPlaceInit, {0, 2, 3}};
    Containers::Array<Matrix3> inverseBindMatrices{Containers::InPlaceInit, {
        Matrix3::translation(Vector2::yAxis(0.0f)),
        Matrix3::translation(Vector2::yAxis(2.0f)),
        Matrix3::translation(Vector2::yAxis(4.0f))
    }};

    const void* jointsPointer = joints;
    const void* inverseBindMatricesPointer = inverseBindMatrices;

    SkinData2D data{std::move(joints), std::move(inverseBindMatrices)};

    Containers::Array<UnsignedInt> releasedJoints = data.releaseJointData();
    CORRADE_COMPARE(data.joints().size(), 0);
    CORRADE_COMPARE(data.inverseBindMatrices().size(), 3);
    CORRADE_COMPARE(releasedJoints.size(), 3);
    CORRADE_COMPARE(releasedJoints, jointsPointer);

    Containers::Array<Matrix3> releasedInverseBindMatrices = data.releaseInverseBindMatrixData();
    CORRADE_COMPARE(data.joints().size(), 0);
    CORRADE_COMPARE(data.inverseBindMatrices().size(), 0);
    CORRADE_COMPARE(releasedInverseBindMatrices.size(), 3);
    CORRADE_COMPARE(releasedInverseBindMatrices, inverseBindMatricesPointer);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::SkinDataTest)
