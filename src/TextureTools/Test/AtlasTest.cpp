/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <sstream>
#include <TestSuite/Tester.h>

#include "Math/Geometry/Rectangle.h"
#include "TextureTools/Atlas.h"

namespace Magnum { namespace TextureTools { namespace Test {

class AtlasTest: public Corrade::TestSuite::Tester {
    public:
        explicit AtlasTest();

        void create();
        void createPadding();
        void createEmpty();
        void createTooSmall();
};

AtlasTest::AtlasTest() {
    addTests(&AtlasTest::create,
             &AtlasTest::createPadding,
             &AtlasTest::createEmpty,
             &AtlasTest::createTooSmall);
}

void AtlasTest::create() {
    std::vector<Rectanglei> atlas = TextureTools::atlas({64, 64}, {
        {12, 18},
        {32, 15},
        {23, 25}
    });

    CORRADE_COMPARE(atlas.size(), 3);
    CORRADE_COMPARE(atlas, (std::vector<Rectanglei>{
        Rectanglei::fromSize({0, 0}, {12, 18}),
        Rectanglei::fromSize({32, 0}, {32, 15}),
        Rectanglei::fromSize({0, 25}, {23, 25})}));
}

void AtlasTest::createPadding() {
    std::vector<Rectanglei> atlas = TextureTools::atlas({64, 64}, {
        {8, 16},
        {28, 13},
        {19, 23}
    }, {2, 1});

    CORRADE_COMPARE(atlas.size(), 3);
    CORRADE_COMPARE(atlas, (std::vector<Rectanglei>{
        Rectanglei::fromSize({2, 1}, {8, 16}),
        Rectanglei::fromSize({34, 1}, {28, 13}),
        Rectanglei::fromSize({2, 26}, {19, 23})}));
}

void AtlasTest::createEmpty() {
    std::vector<Rectanglei> atlas = TextureTools::atlas({}, {});
    CORRADE_VERIFY(atlas.empty());
}

void AtlasTest::createTooSmall() {
    std::ostringstream o;
    Error::setOutput(&o);

    std::vector<Rectanglei> atlas = TextureTools::atlas({64, 32}, {
        {8, 16},
        {21, 13},
        {19, 29}
    }, {2, 1});
    CORRADE_VERIFY(atlas.empty());
    CORRADE_COMPARE(o.str(), "TextureTools::atlas(): requested atlas size Vector(64, 32) is too small to fit 3 Vector(25, 31) textures. Generated atlas will be empty.\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::TextureTools::Test::AtlasTest)
