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

#include "Magnum/Magnum.h"
#include "Magnum/Trade/SceneData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct SceneDataTest: TestSuite::Tester {
    explicit SceneDataTest();

    void construct();
    void constructCopy();
    void constructMove();
};

SceneDataTest::SceneDataTest() {
    addTests({&SceneDataTest::construct,
              &SceneDataTest::constructCopy,
              &SceneDataTest::constructMove});
}

void SceneDataTest::construct() {
    const int a{};
    const SceneData data{{0, 1, 4}, {2, 5}, &a};

    CORRADE_COMPARE(data.children2D(), (std::vector<UnsignedInt>{0, 1, 4}));
    CORRADE_COMPARE(data.children3D(), (std::vector<UnsignedInt>{2, 5}));
    CORRADE_COMPARE(data.importerState(), &a);
}

void SceneDataTest::constructCopy() {
    CORRADE_VERIFY(!(std::is_constructible<SceneData, const SceneData&>{}));
    CORRADE_VERIFY(!(std::is_assignable<SceneData, const SceneData&>{}));
}

void SceneDataTest::constructMove() {
    const int a{};
    SceneData data{{0, 1, 4}, {2, 5}, &a};

    SceneData b{std::move(data)};

    CORRADE_COMPARE(b.children2D(), (std::vector<UnsignedInt>{0, 1, 4}));
    CORRADE_COMPARE(b.children3D(), (std::vector<UnsignedInt>{2, 5}));
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    SceneData d{{1, 3}, {1, 4, 5}, &c};
    d = std::move(b);

    CORRADE_COMPARE(d.children2D(), (std::vector<UnsignedInt>{0, 1, 4}));
    CORRADE_COMPARE(d.children3D(), (std::vector<UnsignedInt>{2, 5}));
    CORRADE_COMPARE(d.importerState(), &a);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<SceneData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<SceneData>::value);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::SceneDataTest)
