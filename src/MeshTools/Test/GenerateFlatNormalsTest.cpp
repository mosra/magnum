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

#include "Math/Vector3.h"
#include "MeshTools/GenerateFlatNormals.h"

namespace Magnum { namespace MeshTools { namespace Test {

class GenerateFlatNormalsTest: public Corrade::TestSuite::Tester {
    public:
        GenerateFlatNormalsTest();

        void wrongIndexCount();
        void generate();
};

GenerateFlatNormalsTest::GenerateFlatNormalsTest() {
    addTests(&GenerateFlatNormalsTest::wrongIndexCount,
             &GenerateFlatNormalsTest::generate);
}

void GenerateFlatNormalsTest::wrongIndexCount() {
    std::stringstream ss;
    Error::setOutput(&ss);
    std::vector<UnsignedInt> indices;
    std::vector<Vector3> normals;
    std::tie(indices, normals) = MeshTools::generateFlatNormals({
        0, 1
    }, {});

    CORRADE_COMPARE(indices.size(), 0);
    CORRADE_COMPARE(normals.size(), 0);
    CORRADE_COMPARE(ss.str(), "MeshTools::generateFlatNormals(): index count is not divisible by 3!\n");
}

void GenerateFlatNormalsTest::generate() {
    /* Two vertices connected by one edge, each winded in another direction */
    std::vector<UnsignedInt> indices;
    std::vector<Vector3> normals;
    std::tie(indices, normals) = MeshTools::generateFlatNormals({
        0, 1, 2,
        1, 2, 3
    }, {
        {-1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f}
    });

    CORRADE_COMPARE(indices, (std::vector<UnsignedInt>{
        0, 0, 0,
        1, 1, 1
    }));
    CORRADE_COMPARE(normals, (std::vector<Vector3>{
        Vector3::zAxis(),
        -Vector3::zAxis()
    }));
}

}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::GenerateFlatNormalsTest)
