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
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/Duplicate.h"

namespace Magnum { namespace MeshTools { namespace Test { namespace {

struct DuplicateTest: TestSuite::Tester {
    explicit DuplicateTest();

    void duplicate();
    void duplicateOutOfBounds();
    void duplicateStl();

    void duplicateInto();
    void duplicateIntoWrongSize();
};

DuplicateTest::DuplicateTest() {
    addTests({&DuplicateTest::duplicate,
              &DuplicateTest::duplicateOutOfBounds,
              &DuplicateTest::duplicateStl,

              &DuplicateTest::duplicateInto,
              &DuplicateTest::duplicateIntoWrongSize});
}

void DuplicateTest::duplicate() {
    constexpr UnsignedByte indices[]{1, 1, 0, 3, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};

    CORRADE_COMPARE_AS((MeshTools::duplicate<UnsignedByte, Int>(indices, data)),
        (Containers::Array<Int>{Containers::InPlaceInit, {
            35, 35, -7, -18, 12, 12
        }}), TestSuite::Compare::Container);
}

void DuplicateTest::duplicateOutOfBounds() {
    constexpr UnsignedByte indices[]{1, 1, 0, 4, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};

    std::ostringstream out;
    Error redirectError{&out};

    MeshTools::duplicate<UnsignedByte, Int>(indices, data);
    CORRADE_COMPARE(out.str(),
        "MeshTools::duplicateInto(): index 4 out of bounds for 4 elements\n");
}

void DuplicateTest::duplicateStl() {
    CORRADE_COMPARE(MeshTools::duplicate({1, 1, 0, 3, 2, 2}, std::vector<int>{-7, 35, 12, -18}),
        (std::vector<Int>{35, 35, -7, -18, 12, 12}));
}

void DuplicateTest::duplicateInto() {
    constexpr UnsignedByte indices[]{1, 1, 0, 3, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};
    Int output[6];

    MeshTools::duplicateInto<UnsignedByte, Int>(indices, data, output);
    CORRADE_COMPARE_AS(Containers::arrayView<const Int>(output),
        (Containers::Array<Int>{Containers::InPlaceInit, {
            35, 35, -7, -18, 12, 12
        }}), TestSuite::Compare::Container);
}

void DuplicateTest::duplicateIntoWrongSize() {
    constexpr UnsignedByte indices[]{1, 1, 0, 3, 2, 2};
    constexpr Int data[]{-7, 35, 12, -18};
    Int output[5];

    std::ostringstream out;
    Error redirectError{&out};

    MeshTools::duplicateInto<UnsignedByte, Int>(indices, data, output);
    CORRADE_COMPARE(out.str(),
        "MeshTools::duplicateInto(): bad output size, expected 6 but got 5\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::DuplicateTest)
