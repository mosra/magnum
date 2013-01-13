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
#include <Utility/Configuration.h>

#include "IndexedMesh.h"

using namespace Corrade::Utility;

namespace Magnum { namespace Test {

class IndexedMeshTest: public Corrade::TestSuite::Tester {
    public:
        IndexedMeshTest();

        void debug();
        void configuration();
};

IndexedMeshTest::IndexedMeshTest() {
    addTests(&IndexedMeshTest::debug,
             &IndexedMeshTest::configuration);
}

void IndexedMeshTest::debug() {
    std::ostringstream o;
    Debug(&o) << IndexedMesh::IndexType::UnsignedShort;
    CORRADE_COMPARE(o.str(), "IndexedMesh::IndexType::UnsignedShort\n");
}

void IndexedMeshTest::configuration() {
    Configuration c;

    c.setValue("type", IndexedMesh::IndexType::UnsignedByte);
    CORRADE_COMPARE(c.value<std::string>("type"), "UnsignedByte");
    CORRADE_COMPARE(c.value<IndexedMesh::IndexType>("type"), IndexedMesh::IndexType::UnsignedByte);
}

}}

CORRADE_TEST_MAIN(Magnum::Test::IndexedMeshTest)
