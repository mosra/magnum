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

#include "CylinderTest.h"

#include <TestSuite/Compare/Container.h>

#include "Math/Vector4.h"
#include "Primitives/Cylinder.h"

using namespace std;
using Corrade::TestSuite::Compare::Container;

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CylinderTest)

namespace Magnum { namespace Primitives { namespace Test {

CylinderTest::CylinderTest() {
    addTests(&CylinderTest::withoutAnything,
             &CylinderTest::withTextureCoordsAndCaps);
}

void CylinderTest::withoutAnything() {
    Cylinder cylinder(2, 3, 3.0f);

    CORRADE_COMPARE_AS(*cylinder.positions(0), (vector<Vector4>{
        {0.0f, -1.5f, 1.0f},
        {0.866025f, -1.5f, -0.5f},
        {-0.866025f, -1.5f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 1.5f, 1.0f},
        {0.866025f, 1.5f, -0.5f},
        {-0.866025f, 1.5f, -0.5f}
    }), Container);

    CORRADE_COMPARE_AS(*cylinder.normals(0), (vector<Vector3>{
        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f}
    }), Container);

    CORRADE_COMPARE_AS(*cylinder.indices(), (vector<unsigned int>{
        0, 1, 4, 0, 4, 3, 1, 2, 5, 1, 5, 4, 2, 0, 3, 2, 3, 5,
        3, 4, 7, 3, 7, 6, 4, 5, 8, 4, 8, 7, 5, 3, 6, 5, 6, 8
    }), Container);
}

void CylinderTest::withTextureCoordsAndCaps() {
    Cylinder cylinder(2, 3, 3.0f, Cylinder::Flag::GenerateTextureCoords|Cylinder::Flag::CapEnds);

    CORRADE_COMPARE_AS(*cylinder.positions(0), (vector<Vector4>{
        {0.0f, -1.5f, 0.0f},

        {0.0f, -1.5f, 1.0f},
        {0.866025f, -1.5f, -0.5f},
        {-0.866025f, -1.5f, -0.5f},
        {0.0f, -1.5f, 1.0f},

        {0.0f, -1.5f, 1.0f},
        {0.866025f, -1.5f, -0.5f},
        {-0.866025f, -1.5f, -0.5f},
        {0.0f, -1.5f, 1.0f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},
        {0.0f, 0.0f, 1.0f},

        {0.0f, 1.5f, 1.0f},
        {0.866025f, 1.5f, -0.5f},
        {-0.866025f, 1.5f, -0.5f},
        {0.0f, 1.5f, 1.0f},

        {0.0f, 1.5f, 1.0f},
        {0.866025f, 1.5f, -0.5f},
        {-0.866025f, 1.5f, -0.5f},
        {0.0f, 1.5f, 1.0f},

        {0.0f, 1.5f, 0.0f}
    }), Container);

    CORRADE_COMPARE_AS(*cylinder.normals(0), (vector<Vector3>{
        {0.0f, -1.0f, 0.0f},

        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},
        {0.0f, 0.0f, 1.0f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},
        {0.0f, 0.0f, 1.0f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},
        {0.0f, 0.0f, 1.0f},

        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},

        {0.0f, 1.0f, 0.0f},
    }), Container);

    CORRADE_COMPARE_AS(*cylinder.textureCoords2D(0), (vector<Vector2>{
        {0.5f, 0.0f},

        {0.0f, 0.2f},
        {0.333333f, 0.2f},
        {0.666667f, 0.2f},
        {1.0f, 0.2f},

        {0.0f, 0.2f},
        {0.333333f, 0.2f},
        {0.666667f, 0.2f},
        {1.0f, 0.2f},

        {0.0f, 0.5f},
        {0.333333f, 0.5f},
        {0.666667f, 0.5f},
        {1.0f, 0.5f},

        {0.0f, 0.8f},
        {0.333333f, 0.8f},
        {0.666667f, 0.8f},
        {1.0f, 0.8f},

        {0.0f, 0.8f},
        {0.333333f, 0.8f},
        {0.666667f, 0.8f},
        {1.0f, 0.8f},

        {0.5f, 1.0f}
    }), Container);

    CORRADE_COMPARE_AS(*cylinder.indices(), (vector<unsigned int>{
        0, 2, 1, 0, 3, 2, 0, 4, 3,
        1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 4, 8, 3, 8, 7,
        5, 6, 10, 5, 10, 9, 6, 7, 11, 6, 11, 10, 7, 8, 12, 7,
        12, 11, 17, 18, 21, 18, 19, 21, 19, 20, 21
    }), Container);
}

}}}
