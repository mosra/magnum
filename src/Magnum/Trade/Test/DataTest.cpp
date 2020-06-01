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

#include "Magnum/Trade/Data.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct DataTest: TestSuite::Tester {
    explicit DataTest();

    void debugDataFlag();
    void debugDataFlags();
};

DataTest::DataTest() {
    addTests({&DataTest::debugDataFlag,
              &DataTest::debugDataFlags});
}

void DataTest::debugDataFlag() {
    std::ostringstream out;

    Debug{&out} << DataFlag::Owned << DataFlag(0xf0);
    CORRADE_COMPARE(out.str(), "Trade::DataFlag::Owned Trade::DataFlag(0xf0)\n");
}

void DataTest::debugDataFlags() {
    std::ostringstream out;

    Debug{&out} << (DataFlag::Owned|DataFlag::Mutable) << DataFlags{};
    CORRADE_COMPARE(out.str(), "Trade::DataFlag::Owned|Trade::DataFlag::Mutable Trade::DataFlags{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::DataTest)
