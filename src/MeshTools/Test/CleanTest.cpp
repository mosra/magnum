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

#include "CleanTest.h"

#include "MeshTools/Clean.h"

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::CleanTest)

using namespace std;

namespace Magnum { namespace MeshTools { namespace Test {

CleanTest::CleanTest() {
    addTests(&CleanTest::cleanMesh);
}

void CleanTest::cleanMesh() {
    vector<Vector1> positions{1, 2, 1, 4};
    vector<uint32_t> indices{0, 1, 2, 1, 2, 3};
    MeshTools::clean(indices, positions);

    /* Verify cleanup */
    CORRADE_VERIFY(positions == (vector<Vector1>{1, 2, 4}));
    CORRADE_COMPARE(indices, (vector<uint32_t>{0, 1, 0, 1, 0, 2}));
}

}}}
