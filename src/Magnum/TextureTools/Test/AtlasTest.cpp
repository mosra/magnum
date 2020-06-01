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

#include "Magnum/Math/Range.h"
#include "Magnum/TextureTools/Atlas.h"

namespace Magnum { namespace TextureTools { namespace Test { namespace {

struct AtlasTest: TestSuite::Tester {
    explicit AtlasTest();

    void create();
    void createPadding();
    void createEmpty();
    void createTooSmall();
};

AtlasTest::AtlasTest() {
    addTests({&AtlasTest::create,
              &AtlasTest::createPadding,
              &AtlasTest::createEmpty,
              &AtlasTest::createTooSmall});
}

void AtlasTest::create() {
    std::vector<Range2Di> atlas = TextureTools::atlas({64, 64}, {
        {12, 18},
        {32, 15},
        {23, 25}
    });

    CORRADE_COMPARE(atlas.size(), 3);
    CORRADE_COMPARE(atlas, (std::vector<Range2Di>{
        Range2Di::fromSize({0, 0}, {12, 18}),
        Range2Di::fromSize({32, 0}, {32, 15}),
        Range2Di::fromSize({0, 25}, {23, 25})}));
}

void AtlasTest::createPadding() {
    std::vector<Range2Di> atlas = TextureTools::atlas({64, 64}, {
        {8, 16},
        {28, 13},
        {19, 23}
    }, {2, 1});

    CORRADE_COMPARE(atlas.size(), 3);
    CORRADE_COMPARE(atlas, (std::vector<Range2Di>{
        Range2Di::fromSize({2, 1}, {8, 16}),
        Range2Di::fromSize({34, 1}, {28, 13}),
        Range2Di::fromSize({2, 26}, {19, 23})}));
}

void AtlasTest::createEmpty() {
    std::vector<Range2Di> atlas = TextureTools::atlas({}, {});
    CORRADE_VERIFY(atlas.empty());
}

void AtlasTest::createTooSmall() {
    std::ostringstream o;
    Error redirectError{&o};

    std::vector<Range2Di> atlas = TextureTools::atlas({64, 32}, {
        {8, 16},
        {21, 13},
        {19, 29}
    }, {2, 1});
    CORRADE_VERIFY(atlas.empty());
    CORRADE_COMPARE(o.str(), "TextureTools::atlas(): requested atlas size Vector(64, 32) is too small to fit 3 Vector(25, 31) textures. Generated atlas will be empty.\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::TextureTools::Test::AtlasTest)
