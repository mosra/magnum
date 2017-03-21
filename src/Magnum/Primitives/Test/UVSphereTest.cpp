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
#include "Magnum/Primitives/UVSphere.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives { namespace Test {

struct UVSphereTest: TestSuite::Tester {
    explicit UVSphereTest();

    void solidWithoutTextureCoords();
    void solidWithTextureCoords();
    void wireframe();
};

UVSphereTest::UVSphereTest() {
    addTests({&UVSphereTest::solidWithoutTextureCoords,
              &UVSphereTest::solidWithTextureCoords,
              &UVSphereTest::wireframe});
}

void UVSphereTest::solidWithoutTextureCoords() {
    Trade::MeshData3D sphere = UVSphere::solid(3, 3);

    CORRADE_COMPARE_AS(sphere.positions(0), (std::vector<Vector3>{
        {0.0f, -1.0f, 0.0f},

        {0.0f, -0.5f, 0.866025f},
        {0.75f, -0.5f, -0.433013f},
        {-0.75f, -0.5f, -0.433013f},

        {0, 0.5f, 0.866025f},
        {0.75f, 0.5f, -0.433013f},
        {-0.75f, 0.5f, -0.433013f},

        {0.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(sphere.normals(0), (std::vector<Vector3>{
        {0.0f, -1.0f, 0.0f},

        {0.0f, -0.5f, 0.866025f},
        {0.75f, -0.5f, -0.433013f},
        {-0.75f, -0.5f, -0.433013f},

        {0, 0.5f, 0.866025f},
        {0.75f, 0.5f, -0.433013f},
        {-0.75f, 0.5f, -0.433013f},

        {0.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(sphere.indices(), (std::vector<UnsignedInt>{
        0, 2, 1, 0, 3, 2, 0, 1, 3,
        1, 2, 5, 1, 5, 4, 2, 3, 6, 2, 6, 5, 3, 1, 4, 3, 4, 6,
        4, 5, 7, 5, 6, 7, 6, 4, 7
    }), TestSuite::Compare::Container);
}

void UVSphereTest::solidWithTextureCoords() {
    Trade::MeshData3D sphere = UVSphere::solid(3, 3, UVSphere::TextureCoords::Generate);

    CORRADE_COMPARE_AS(sphere.positions(0), (std::vector<Vector3>{
        {0.0f, -1.0f, 0.0f},

        {0.0f, -0.5f, 0.866025f},
        {0.75f, -0.5f, -0.433013f},
        {-0.75f, -0.5f, -0.433013f},
        {0.0f, -0.5f, 0.866025f},

        {0.0f, 0.5f, 0.866025f},
        {0.75f, 0.5f, -0.433013f},
        {-0.75f, 0.5f, -0.433013f},
        {0.0f, 0.5f, 0.866025f},

        {0.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(sphere.textureCoords2D(0), (std::vector<Vector2>{
        {0.5f, 0.0f},

        {0.0f, 0.333333f},
        {0.333333f, 0.333333f},
        {0.666667f, 0.333333f},
        {1.0f, 0.333333f},

        {0.0f, 0.666667f},
        {0.333333f, 0.666667f},
        {0.666667f, 0.666667f},
        {1.0f, 0.666667f},

        {0.5f, 1.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(sphere.indices(), (std::vector<UnsignedInt>{
        0, 2, 1, 0, 3, 2, 0, 4, 3,
        1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 4, 8, 3, 8, 7,
        5, 6, 9, 6, 7, 9, 7, 8, 9
    }), TestSuite::Compare::Container);
}

void UVSphereTest::wireframe() {
    Trade::MeshData3D sphere = UVSphere::wireframe(4, 8);

    CORRADE_COMPARE_AS(sphere.positions(0), (std::vector<Vector3>{
        {0.0f, -1.0f, 0.0f},

        {0.0f, -0.707107f, 0.707107f},
        {0.707107f, -0.707107f, 0.0f},
        {0.0f, -0.707107f, -0.707107f},
        {-0.707107f, -0.707107f, 0.0f},

        {0.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f},
        {-1.0f, 0.0f, 0.0f},
        {0.707107f, 0.0f, 0.707107f},
        {0.707107f, 0.0f, -0.707107f},
        {-0.707107f, 0.0f, -0.707107f},
        {-0.707107f, 0.0f, 0.707107f},

        {0.0f, 0.707107f, 0.707107f},
        {0.707107f, 0.707107f, 0.0f},
        {0.0f, 0.707107f, -0.707107f},
        {-0.707107f, 0.707107f, 0.0f},

        {0.0f, 1.0f, 0.0f}
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE(sphere.normalArrayCount(), 0);

    CORRADE_COMPARE_AS(sphere.indices(), (std::vector<UnsignedInt>{
        0, 1, 0, 2, 0, 3, 0, 4,
        1, 5, 2, 6, 3, 7, 4, 8,

        5, 9, 6, 10, 7, 11, 8, 12,
        9, 6, 10, 7, 11, 8, 12, 5,

        5, 13, 6, 14, 7, 15, 8, 16,
        13, 17, 14, 17, 15, 17, 16, 17
    }), TestSuite::Compare::Container);
}

}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::UVSphereTest)
