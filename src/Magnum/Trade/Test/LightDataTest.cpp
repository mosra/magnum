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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Trade/LightData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct LightDataTest: TestSuite::Tester {
    explicit LightDataTest();

    void construct();
    void constructCopy();
    void constructMove();

    void debugType();
};

LightDataTest::LightDataTest() {
    addTests({&LightDataTest::construct,
              &LightDataTest::constructCopy,
              &LightDataTest::constructMove,

              &LightDataTest::debugType});
}

void LightDataTest::construct() {
    using namespace Math::Literals;
    const int a{};
    LightData data{LightData::Type::Infinite, 0xccff33_rgbf, 0.8f, &a};

    CORRADE_COMPARE(data.type(), LightData::Type::Infinite);
    CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
    CORRADE_COMPARE(data.intensity(), 0.8f);
    CORRADE_COMPARE(data.importerState(), &a);
}

void LightDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<LightData, const LightData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<LightData, const LightData&>{}));
}

void LightDataTest::constructMove() {
    using namespace Math::Literals;
    const int a{};
    LightData data{LightData::Type::Infinite, 0xccff33_rgbf, 0.8f, &a};

    CORRADE_COMPARE(data.type(), LightData::Type::Infinite);
    CORRADE_COMPARE(data.color(), 0xccff33_rgbf);
    CORRADE_COMPARE(data.intensity(), 0.8f);
    CORRADE_COMPARE(data.importerState(), &a);

    LightData b{std::move(data)};
    CORRADE_COMPARE(b.type(), LightData::Type::Infinite);
    CORRADE_COMPARE(b.color(), 0xccff33_rgbf);
    CORRADE_COMPARE(b.intensity(), 0.8f);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    LightData d{LightData::Type::Point, 0xdead00_rgbf, 1.6f, &c};
    d = std::move(b);
    CORRADE_COMPARE(d.type(), LightData::Type::Infinite);
    CORRADE_COMPARE(d.color(), 0xccff33_rgbf);
    CORRADE_COMPARE(d.intensity(), 0.8f);
    CORRADE_COMPARE(d.importerState(), &a);
}

void LightDataTest::debugType() {
    std::ostringstream out;

    Debug(&out) << LightData::Type::Spot << LightData::Type(0xbe);
    CORRADE_COMPARE(out.str(), "Trade::LightData::Type::Spot Trade::LightData::Type(0xbe)\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::LightDataTest)
