/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <TestSuite/Tester.h>
#include <TestSuite/Compare/Container.h>

#include "Math/Vector3.h"
#include "Primitives/Cylinder.h"
#include "Trade/MeshData3D.h"

using Corrade::TestSuite::Compare::Container;

namespace Magnum { namespace Primitives { namespace Test {

class CylinderTest: public Corrade::TestSuite::Tester {
    public:
        CylinderTest();

        void withoutAnything();
        void withTextureCoordsAndCaps();
};

CylinderTest::CylinderTest() {
    addTests({&CylinderTest::withoutAnything,
              &CylinderTest::withTextureCoordsAndCaps});
}

void CylinderTest::withoutAnything() {
    Trade::MeshData3D cylinder = Cylinder::solid(2, 3, 3.0f);

    CORRADE_COMPARE_AS(*cylinder.positions(0), (std::vector<Vector3>{
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

    CORRADE_COMPARE_AS(*cylinder.normals(0), (std::vector<Vector3>{
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

    CORRADE_COMPARE_AS(*cylinder.indices(), (std::vector<UnsignedInt>{
        0, 1, 4, 0, 4, 3, 1, 2, 5, 1, 5, 4, 2, 0, 3, 2, 3, 5,
        3, 4, 7, 3, 7, 6, 4, 5, 8, 4, 8, 7, 5, 3, 6, 5, 6, 8
    }), Container);
}

void CylinderTest::withTextureCoordsAndCaps() {
    Trade::MeshData3D cylinder = Cylinder::solid(2, 3, 3.0f, Cylinder::Flag::GenerateTextureCoords|Cylinder::Flag::CapEnds);

    CORRADE_COMPARE_AS(*cylinder.positions(0), (std::vector<Vector3>{
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

    CORRADE_COMPARE_AS(*cylinder.normals(0), (std::vector<Vector3>{
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

    CORRADE_COMPARE_AS(*cylinder.textureCoords2D(0), (std::vector<Vector2>{
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

    CORRADE_COMPARE_AS(*cylinder.indices(), (std::vector<UnsignedInt>{
        0, 2, 1, 0, 3, 2, 0, 4, 3,
        1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 4, 8, 3, 8, 7,
        5, 6, 10, 5, 10, 9, 6, 7, 11, 6, 11, 10, 7, 8, 12, 7,
        12, 11, 17, 18, 21, 18, 19, 21, 19, 20, 21
    }), Container);
}

}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CylinderTest)
