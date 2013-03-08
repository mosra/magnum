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
#include "MeshTools/FlipNormals.h"

namespace Magnum { namespace MeshTools { namespace Test {

class FlipNormalsTest: public Corrade::TestSuite::Tester {
    public:
        FlipNormalsTest();

        void wrongIndexCount();
        void flipFaceWinding();
        void flipNormals();
};

FlipNormalsTest::FlipNormalsTest() {
    addTests({&FlipNormalsTest::wrongIndexCount,
              &FlipNormalsTest::flipFaceWinding,
              &FlipNormalsTest::flipNormals});
}

void FlipNormalsTest::wrongIndexCount() {
    std::stringstream ss;
    Error::setOutput(&ss);

    std::vector<UnsignedInt> indices{0, 1};
    MeshTools::flipFaceWinding(indices);

    CORRADE_COMPARE(ss.str(), "MeshTools::flipNormals(): index count is not divisible by 3!\n");
}

void FlipNormalsTest::flipFaceWinding() {
    std::vector<UnsignedInt> indices{0, 1, 2,
                                       3, 4, 5};
    MeshTools::flipFaceWinding(indices);

    CORRADE_COMPARE(indices, (std::vector<UnsignedInt>{0, 2, 1,
                                                         3, 5, 4}));
}

void FlipNormalsTest::flipNormals() {
    std::vector<Vector3> normals{Vector3::xAxis(),
                                 Vector3::yAxis(),
                                 Vector3::zAxis()};
    MeshTools::flipNormals(normals);

    CORRADE_COMPARE(normals, (std::vector<Vector3>{-Vector3::xAxis(),
                                                   -Vector3::yAxis(),
                                                   -Vector3::zAxis()}));
}

}}}

CORRADE_TEST_MAIN(Magnum::MeshTools::Test::FlipNormalsTest)
