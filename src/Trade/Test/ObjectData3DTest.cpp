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

#include "Trade/ObjectData3D.h"

namespace Magnum { namespace Trade { namespace Test {

class ObjectData3DTest: public Corrade::TestSuite::Tester {
    public:
        explicit ObjectData3DTest();

        void debug();
};

ObjectData3DTest::ObjectData3DTest() {
    addTests({&ObjectData3DTest::debug});
}

void ObjectData3DTest::debug() {
    std::ostringstream o;
    Debug(&o) << ObjectData3D::InstanceType::Light;
    CORRADE_COMPARE(o.str(), "Trade::ObjectData3D::InstanceType::Light\n");
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ObjectData3DTest)
