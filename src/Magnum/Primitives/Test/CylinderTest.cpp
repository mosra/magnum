/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/Primitives/Cylinder.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives { namespace Test {

struct CylinderTest: TestSuite::Tester {
    explicit CylinderTest();

    void solidWithoutAnything();
    void solidWithTextureCoordsAndCaps();
    void wireframe();
};

CylinderTest::CylinderTest() {
    addTests({&CylinderTest::solidWithoutAnything,
              &CylinderTest::solidWithTextureCoordsAndCaps,
              &CylinderTest::wireframe});
}

void CylinderTest::solidWithoutAnything() {
    Trade::MeshData3D cylinder = Cylinder::solid(2, 3, 1.5f);

    CORRADE_COMPARE_AS(cylinder.positions(0), (std::vector<Vector3>{
        {0.0f, -1.5f, 1.0f},
        {0.866025f, -1.5f, -0.5f},
        {-0.866025f, -1.5f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 1.5f, 1.0f},
        {0.866025f, 1.5f, -0.5f},
        {-0.866025f, 1.5f, -0.5f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(cylinder.normals(0), (std::vector<Vector3>{
        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f},

        {0.0f, 0.0f, 1.0f},
        {0.866025f, 0.0f, -0.5f},
        {-0.866025f, 0.0f, -0.5f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(cylinder.indices(), (std::vector<UnsignedInt>{
        0, 1, 4, 0, 4, 3, 1, 2, 5, 1, 5, 4, 2, 0, 3, 2, 3, 5,
        3, 4, 7, 3, 7, 6, 4, 5, 8, 4, 8, 7, 5, 3, 6, 5, 6, 8
    }), TestSuite::Compare::Container);
}

void CylinderTest::solidWithTextureCoordsAndCaps() {
    Trade::MeshData3D cylinder = Cylinder::solid(2, 3, 1.5f, Cylinder::Flag::GenerateTextureCoords|Cylinder::Flag::CapEnds);

    /* First and last ring are duplicated because they have different normals */
    CORRADE_COMPARE_AS(cylinder.positions(0), (std::vector<Vector3>{
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
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(cylinder.normals(0), (std::vector<Vector3>{
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
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(cylinder.textureCoords2D(0), (std::vector<Vector2>{
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
    }), TestSuite::Compare::Container);

    /* Faces of the caps and sides do not share any vertices due to different
       normals */
    CORRADE_COMPARE_AS(cylinder.indices(), (std::vector<UnsignedInt>{
         0,  2,  1,  0,  3,  2,  0,  4,  3,
         4,  5,  9,  4,  9,  8,  5,  6, 10,  5, 10,  9,  6,  7, 11,  6, 11, 10,
         8,  9, 13,  8, 13, 12,  9, 10, 14,  9, 14, 13, 10, 11, 15, 10, 15, 14,
        17, 18, 21, 18, 19, 21, 19, 20, 21
    }), TestSuite::Compare::Container);
}

void CylinderTest::wireframe() {
    Trade::MeshData3D cylinder = Cylinder::wireframe(2, 8, 0.5f);

    CORRADE_COMPARE_AS(cylinder.positions(0), (std::vector<Vector3>{
        {0.0f, -0.5f, 1.0f},
        {1.0f, -0.5f, 0.0f},
        {0.0f, -0.5f, -1.0f},
        {-1.0f, -0.5f, 0.0f},
        {0.707107f, -0.5f, 0.707107f},
        {0.707107f, -0.5f, -0.707107f},
        {-0.707107f, -0.5f, -0.707107f},
        {-0.707107f, -0.5f, 0.707107f},

        {0.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f},
        {-1.0f, 0.0f, 0.0f},
        {0.707107f, 0.0f, 0.707107f},
        {0.707107f, 0.0f, -0.707107f},
        {-0.707107f, 0.0f, -0.707107f},
        {-0.707107f, 0.0f, 0.707107f},

        {0.0f, 0.5f, 1.0f},
        {1.0f, 0.5f, 0.0f},
        {0.0f, 0.5f, -1.0f},
        {-1.0f, 0.5f, 0.0f},
        {0.707107f, 0.5f, 0.707107f},
        {0.707107f, 0.5f, -0.707107f},
        {-0.707107f, 0.5f, -0.707107f},
        {-0.707107f, 0.5f, 0.707107f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE(cylinder.normalArrayCount(), 0);

    CORRADE_COMPARE_AS(cylinder.indices(), (std::vector<UnsignedInt>{
        0, 4, 1, 5, 2, 6, 3, 7,
        4, 1, 5, 2, 6, 3, 7, 0,

        0, 8, 1, 9, 2, 10, 3, 11,

        8, 12, 9, 13, 10, 14, 11, 15,
        12, 9, 13, 10, 14, 11, 15, 8,

        8, 16, 9, 17, 10, 18, 11, 19,

        16, 20, 17, 21, 18, 22, 19, 23,
        20, 17, 21, 18, 22, 19, 23, 16
    }), TestSuite::Compare::Container);
}

}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CylinderTest)
