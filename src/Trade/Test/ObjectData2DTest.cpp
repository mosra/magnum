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

#include "Trade/ObjectData2D.h"

namespace Magnum { namespace Trade { namespace Test {

class ObjectData2DTest: public Corrade::TestSuite::Tester {
    public:
        explicit ObjectData2DTest();

        void debug();
};

ObjectData2DTest::ObjectData2DTest() {
    addTests({&ObjectData2DTest::debug});
}

void ObjectData2DTest::debug() {
    std::ostringstream o;
    Debug(&o) << ObjectData2D::InstanceType::Empty;
    CORRADE_COMPARE(o.str(), "Trade::ObjectData2D::InstanceType::Empty\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ObjectData2DTest)
