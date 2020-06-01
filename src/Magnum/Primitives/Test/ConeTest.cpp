/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/Math/Vector4.h"
#include "Magnum/Primitives/Cone.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct ConeTest: TestSuite::Tester {
    explicit ConeTest();

    void solidWithoutAnything();
    void solidWithCaps();
    void solidWithTextureCoordinatesOrTangents();
    void solidWithTextureCoordinatesOrTangentsAndCaps();
    void wireframe();
};

constexpr struct {
    const char* name;
    ConeFlags flags;
} TextureCoordinatesOrTangentsData[] {
    {"texture coordinates", ConeFlag::TextureCoordinates},
    {"tangents", ConeFlag::Tangents},
    {"both", ConeFlag::TextureCoordinates|ConeFlag::Tangents}
};

ConeTest::ConeTest() {
    addTests({&ConeTest::solidWithoutAnything,
              &ConeTest::solidWithCaps});

    addInstancedTests({
        &ConeTest::solidWithTextureCoordinatesOrTangents,
        &ConeTest::solidWithTextureCoordinatesOrTangentsAndCaps},
        Containers::arraySize(TextureCoordinatesOrTangentsData));

    addTests({&ConeTest::wireframe});
}

void ConeTest::solidWithoutAnything() {
    Trade::MeshData cone = coneSolid(2, 3, 1.0f);

    CORRADE_COMPARE(cone.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(cone.isIndexed());
    CORRADE_COMPARE(cone.attributeCount(), 2);

    CORRADE_COMPARE_AS(cone.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 1.0f},        /* 0 */
        {0.866025f, -1.0f, -0.5f},  /* 1 */
        {-0.866025f, -1.0f, -0.5f}, /* 2 */

        {0.0f, 0.0f, 0.5f},         /* 3 */
        {0.433013f, 0.0f, -0.25f},  /* 4 */
        {-0.433013f, 0.0f, -0.25f}, /* 5 */

        {0.0f, 1.0f, 0.0f},         /* 6 */
        {0.0f, 1.0f, 0.0f},         /* 7 */
        {0.0f, 1.0f, 0.0f}          /* 8 */
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(cone.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        {0.0f, 0.447214f, 0.894427f},           /* 0 */
        {0.774597f, 0.447214f, -0.447214f},     /* 1 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 2 */

        {0.0f, 0.447214f, 0.894427f},           /* 3 */
        {0.774597f, 0.447214f, -0.447214f},     /* 4 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 5 */

        {0.0f, 0.447214f, 0.894427f},           /* 6 */
        {0.774597f, 0.447214f, -0.447214f},     /* 7 */
        {-0.774597f, 0.447214f, -0.447214f}     /* 8 */
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(cone.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 1, 4, 0, 4, 3, 1, 2, 5, 1, 5, 4, 2, 0, 3, 2, 3, 5,
        3, 4, 7, 3, 7, 6, 4, 5, 8, 4, 8, 7, 5, 3, 6, 5, 6, 8
    }), TestSuite::Compare::Container);
}

void ConeTest::solidWithCaps() {
    Trade::MeshData cone = coneSolid(2, 3, 1.0f, ConeFlag::CapEnd);

    CORRADE_COMPARE(cone.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(cone.isIndexed());
    CORRADE_COMPARE(cone.attributeCount(), 2);

    /* Bottom ring duplicated because it has different normals, first vertex of
       each ring duplicated because it has different texture coordinates */
    CORRADE_COMPARE_AS(cone.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 0.0f},        /* 0 */

        {0.0f, -1.0f, 1.0f},        /* 1 */
        {0.866025f, -1.0f, -0.5f},  /* 2 */
        {-0.866025f, -1.0f, -0.5f}, /* 3 */

        {0.0f, -1.0f, 1.0f},        /* 4 */
        {0.866025f, -1.0f, -0.5f},  /* 5 */
        {-0.866025f, -1.0f, -0.5f}, /* 6 */

        {0.0f, 0.0f, 0.5f},         /* 7 */
        {0.433013f, 0.0f, -0.25f},  /* 8 */
        {-0.433013f, 0.0f, -0.25f}, /* 9 */

        {0.0f, 1.0f, 0.0f},         /* 10 */
        {0.0f, 1.0f, 0.0f},         /* 11 */
        {0.0f, 1.0f, 0.0f},         /* 12 */
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(cone.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 0.0f},                    /* 0 */

        {0.0f, -1.0f, 0.0f},                    /* 1 */
        {0.0f, -1.0f, 0.0f},                    /* 2 */
        {0.0f, -1.0f, 0.0f},                    /* 3 */

        {0.0f, 0.447214f, 0.894427f},           /* 4 */
        {0.774597f, 0.447214f, -0.447214f},     /* 5 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 6 */

        {0.0f, 0.447214f, 0.894427f},           /* 7 */
        {0.774597f, 0.447214f, -0.447214f},     /* 8 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 9 */

        {0.0f, 0.447214f, 0.894427f},           /* 10 */
        {0.774597f, 0.447214f, -0.447214f},     /* 11 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 12 */
    }), TestSuite::Compare::Container);

    /* Faces of the caps and sides do not share any vertices due to different
       normals */
    CORRADE_COMPARE_AS(cone.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
         0,  2,  1,  0,  3,  2,  0,  1,  3,
         4,  5,  8,  4,  8,  7,  5,  6,  9,  5,  9,  8,  6,  4,  7,  6,  7,  9,
         7,  8, 11,  7, 11, 10,  8,  9, 12,  8, 12, 11,  9,  7, 10,  9, 10, 12
    }), TestSuite::Compare::Container);
}

void ConeTest::solidWithTextureCoordinatesOrTangents() {
    auto&& data = TextureCoordinatesOrTangentsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::MeshData cone = coneSolid(2, 3, 1.0f, data.flags);

    CORRADE_COMPARE(cone.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(cone.isIndexed());

    /* Bottom ring duplicated because it has different normals, first vertex of
       each ring duplicated because it has different texture coordinates */
    CORRADE_COMPARE_AS(cone.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 1.0f},        /* 0 */
        {0.866025f, -1.0f, -0.5f},  /* 1 */
        {-0.866025f, -1.0f, -0.5f}, /* 2 */
        {0.0f, -1.0f, 1.0f},        /* 3 */

        {0.0f, 0.0f, 0.5f},         /* 4 */
        {0.433013f, 0.0f, -0.25f},  /* 5 */
        {-0.433013f, 0.0f, -0.25f}, /* 6 */
        {0.0f, 0.0f, 0.5f},         /* 7 */

        {0.0f, 1.0f, 0.0f},         /* 8 */
        {0.0f, 1.0f, 0.0f},         /* 9 */
        {0.0f, 1.0f, 0.0f},         /* 10 */
        {0.0f, 1.0f, 0.0f}          /* 11 */
    }), TestSuite::Compare::Container);

    if(data.flags & ConeFlag::Tangents) {
        CORRADE_COMPARE_AS(cone.attribute<Vector4>(Trade::MeshAttribute::Tangent), Containers::arrayView<Vector4>({
            {1.0f, 0.0f, 0.0f, 1.0f},           /* 0 */
            {-0.5f, 0.0f, -0.866025f, 1.0f},    /* 1 */
            {-0.5f, 0.0f, 0.866025f, 1.0f},     /* 2 */
            {1.0f, 0.0f, 0.0f, 1.0f},           /* 3 */

            {1.0f, 0.0f, 0.0f, 1.0f},           /* 4 */
            {-0.5f, 0.0f, -0.866025f, 1.0f},    /* 5 */
            {-0.5f, 0.0f, 0.866025f, 1.0f},     /* 6 */
            {1.0f, 0.0f, 0.0f, 1.0f},           /* 7 */

            {1.0f, 0.0f, 0.0f, 1.0f},           /* 8 */
            {-0.5f, 0.0f, -0.866025f, 1.0f},    /* 9 */
            {-0.5f, 0.0f, 0.866025f, 1.0f},     /* 10 */
            {1.0f, 0.0f, 0.0f, 1.0f}            /* 11 */
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!cone.hasAttribute(Trade::MeshAttribute::Tangent));

    CORRADE_COMPARE_AS(cone.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        {0.0f, 0.447214f, 0.894427f},           /* 0 */
        {0.774597f, 0.447214f, -0.447214f},     /* 1 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 2 */
        {0.0f, 0.447214f, 0.894427f},           /* 3 */

        {0.0f, 0.447214f, 0.894427f},           /* 4 */
        {0.774597f, 0.447214f, -0.447214f},     /* 5 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 6 */
        {0.0f, 0.447214f, 0.894427f},           /* 7 */

        {0.0f, 0.447214f, 0.894427f},           /* 8 */
        {0.774597f, 0.447214f, -0.447214f},     /* 9 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 10 */
        {0.0f, 0.447214f, 0.894427f}            /* 11 */
    }), TestSuite::Compare::Container);

    if(data.flags & ConeFlag::TextureCoordinates) {
        CORRADE_COMPARE_AS(cone.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView<Vector2>({
            {0.0f, 0.0f},       /* 0 */
            {0.333333f, 0.0f},  /* 1 */
            {0.666667f, 0.0f},  /* 2 */
            {1.0f, 0.0f},       /* 3 */

            {0.0f, 0.5f},       /* 4 */
            {0.333333f, 0.5f},  /* 5 */
            {0.666667f, 0.5f},  /* 6 */
            {1.0f, 0.5f},       /* 7 */

            {0.0f, 1.0f},       /* 8 */
            {0.333333f, 1.0f},  /* 9 */
            {0.666667f, 1.0f},  /* 10 */
            {1.0f, 1.0f},       /* 11 */
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!cone.hasAttribute(Trade::MeshAttribute::TextureCoordinates));

    if(data.flags & ConeFlag::Tangents) {
        auto tangents = cone.attribute<Vector4>(Trade::MeshAttribute::Tangent);
        auto normals = cone.attribute<Vector3>(Trade::MeshAttribute::Normal);
        for(std::size_t i = 0; i != tangents.size(); ++i) {
            CORRADE_ITERATION(i);
            CORRADE_ITERATION(tangents[i]);
            CORRADE_ITERATION(normals[i]);
            CORRADE_VERIFY(tangents[i].xyz().isNormalized());
            CORRADE_VERIFY(normals[i].isNormalized());
            CORRADE_COMPARE(Math::dot(tangents[i].xyz(), normals[i]), 0.0f);
        }
    }

    /* Each ring has an extra vertex for texture coords */
    CORRADE_COMPARE_AS(cone.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
         0,  1,  5,  0,  5,  4,  1,  2,  6,  1,  6,  5,  2,  3,  7,  2,  7,  6,
         4,  5,  9,  4,  9,  8,  5,  6, 10,  5, 10,  9,  6,  7, 11,  6, 11, 10
    }), TestSuite::Compare::Container);
}

void ConeTest::solidWithTextureCoordinatesOrTangentsAndCaps() {
    auto&& data = TextureCoordinatesOrTangentsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::MeshData cone = coneSolid(2, 3, 1.0f, data.flags|ConeFlag::CapEnd);

    CORRADE_COMPARE(cone.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(cone.isIndexed());

    /* Bottom ring duplicated because it has different normals, first vertex of
       each ring duplicated because it has different texture coordinates */
    CORRADE_COMPARE_AS(cone.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 0.0f},        /* 0 */

        {0.0f, -1.0f, 1.0f},        /* 1 */
        {0.866025f, -1.0f, -0.5f},  /* 2 */
        {-0.866025f, -1.0f, -0.5f}, /* 3 */
        {0.0f, -1.0f, 1.0f},        /* 4 */

        {0.0f, -1.0f, 1.0f},        /* 5 */
        {0.866025f, -1.0f, -0.5f},  /* 6 */
        {-0.866025f, -1.0f, -0.5f}, /* 7 */
        {0.0f, -1.0f, 1.0f},        /* 8 */

        {0.0f, 0.0f, 0.5f},         /* 9 */
        {0.433013f, 0.0f, -0.25f},  /* 10 */
        {-0.433013f, 0.0f, -0.25f}, /* 11 */
        {0.0f, 0.0f, 0.5f},         /* 12 */

        {0.0f, 1.0f, 0.0f},         /* 13 */
        {0.0f, 1.0f, 0.0f},         /* 14 */
        {0.0f, 1.0f, 0.0f},         /* 15 */
        {0.0f, 1.0f, 0.0f}          /* 16 */
    }), TestSuite::Compare::Container);

    if(data.flags & ConeFlag::Tangents) {
        CORRADE_COMPARE_AS(cone.attribute<Vector4>(Trade::MeshAttribute::Tangent), Containers::arrayView<Vector4>({
            {-1.0f, 0.0f, 0.0f, 1.0f},          /* 0 */

            {1.0f, 0.0f, 0.0f, 1.0f},           /* 1 */
            {-0.5f, 0.0f, -0.866025f, 1.0f},    /* 2 */
            {-0.5f, 0.0f, 0.866025f, 1.0f},     /* 3 */
            {1.0f, 0.0f, 0.0f, 1.0f},           /* 4 */

            {1.0f, 0.0f, 0.0f, 1.0f},           /* 5 */
            {-0.5f, 0.0f, -0.866025f, 1.0f},    /* 6 */
            {-0.5f, 0.0f, 0.866025f, 1.0f},     /* 7 */
            {1.0f, 0.0f, 0.0f, 1.0f},           /* 8 */

            {1.0f, 0.0f, 0.0f, 1.0f},           /* 9 */
            {-0.5f, 0.0f, -0.866025f, 1.0f},    /* 10 */
            {-0.5f, 0.0f, 0.866025f, 1.0f},     /* 11 */
            {1.0f, 0.0f, 0.0f, 1.0f},           /* 12 */

            {1.0f, 0.0f, 0.0f, 1.0f},           /* 13 */
            {-0.5f, 0.0f, -0.866025f, 1.0f},    /* 14 */
            {-0.5f, 0.0f, 0.866025f, 1.0f},     /* 15 */
            {1.0f, 0.0f, 0.0f, 1.0f}            /* 16 */
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!cone.hasAttribute(Trade::MeshAttribute::Tangent));

    CORRADE_COMPARE_AS(cone.attribute<Vector3>(Trade::MeshAttribute::Normal), Containers::arrayView<Vector3>({
        {0.0f, -1.0f, 0.0f},                    /* 0 */

        {0.0f, -1.0f, 0.0f},                    /* 1 */
        {0.0f, -1.0f, 0.0f},                    /* 2 */
        {0.0f, -1.0f, 0.0f},                    /* 3 */
        {0.0f, -1.0f, 0.0f},                    /* 4 */

        {0.0f, 0.447214f, 0.894427f},           /* 5 */
        {0.774597f, 0.447214f, -0.447214f},     /* 6 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 7 */
        {0.0f, 0.447214f, 0.894427f},           /* 8 */

        {0.0f, 0.447214f, 0.894427f},           /* 9 */
        {0.774597f, 0.447214f, -0.447214f},     /* 10 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 11 */
        {0.0f, 0.447214f, 0.894427f},           /* 12 */

        {0.0f, 0.447214f, 0.894427f},           /* 13 */
        {0.774597f, 0.447214f, -0.447214f},     /* 14 */
        {-0.774597f, 0.447214f, -0.447214f},    /* 15 */
        {0.0f, 0.447214f, 0.894427f}            /* 16 */
    }), TestSuite::Compare::Container);

    if(data.flags & ConeFlag::TextureCoordinates) {
        CORRADE_COMPARE_AS(cone.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates), Containers::arrayView<Vector2>({
            {0.5f, 0.0f},           /* 0 */

            {0.0f, 0.333333f},      /* 1 */
            {0.333333f, 0.333333f}, /* 2 */
            {0.666667f, 0.333333f}, /* 3 */
            {1.0f, 0.333333f},      /* 4 */

            {0.0f, 0.333333f},      /* 5 */
            {0.333333f, 0.333333f}, /* 6 */
            {0.666667f, 0.333333f}, /* 7 */
            {1.0f, 0.333333f},      /* 8 */

            {0.0f, 0.666667f},      /* 9 */
            {0.333333f, 0.666667f}, /* 10 */
            {0.666667f, 0.666667f}, /* 11 */
            {1.0f, 0.666667f},      /* 12 */

            {0.0f, 1.0f},           /* 13 */
            {0.333333f, 1.0f},      /* 14 */
            {0.666667f, 1.0f},      /* 15 */
            {1.0f, 1.0f},           /* 16 */
        }), TestSuite::Compare::Container);
    } else CORRADE_VERIFY(!cone.hasAttribute(Trade::MeshAttribute::TextureCoordinates));

    if(data.flags & ConeFlag::Tangents) {
        auto tangents = cone.attribute<Vector4>(Trade::MeshAttribute::Tangent);
        auto normals = cone.attribute<Vector3>(Trade::MeshAttribute::Normal);
        for(std::size_t i = 0; i != tangents.size(); ++i) {
            CORRADE_ITERATION(i);
            CORRADE_ITERATION(tangents[i]);
            CORRADE_ITERATION(normals[i]);
            CORRADE_VERIFY(tangents[i].xyz().isNormalized());
            CORRADE_VERIFY(normals[i].isNormalized());
            CORRADE_COMPARE(Math::dot(tangents[i].xyz(), normals[i]), 0.0f);
        }
    }

    /* Faces of the caps and sides do not share any vertices due to different
       normals, each ring has an extra vertex for texture coords */
    CORRADE_COMPARE_AS(cone.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
         0,  2,  1,  0,  3,  2,  0,  4,  3,
         5,  6, 10,  5, 10,  9,  6,  7, 11,  6, 11, 10,  7,  8, 12,  7, 12, 11,
         9, 10, 14,  9, 14, 13, 10, 11, 15, 10, 15, 14, 11, 12, 16, 11, 16, 15
    }), TestSuite::Compare::Container);
}

void ConeTest::wireframe() {
    Trade::MeshData cone = coneWireframe(8, 1.5f);

    CORRADE_COMPARE(cone.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(cone.isIndexed());
    CORRADE_COMPARE(cone.attributeCount(), 1);

    CORRADE_COMPARE_AS(cone.attribute<Vector3>(Trade::MeshAttribute::Position), Containers::arrayView<Vector3>({
        {0.0f, -1.5f, 1.0f},                /* 0 */
        {1.0f, -1.5f, 0.0f},                /* 1 */
        {0.0f, -1.5f, -1.0f},               /* 2 */
        {-1.0f, -1.5f, 0.0f},               /* 3 */
        {0.707107f, -1.5f, 0.707107f},      /* 4 */
        {0.707107f, -1.5f, -0.707107f},     /* 5 */
        {-0.707107f, -1.5f, -0.707107f},    /* 6 */
        {-0.707107f, -1.5f, 0.707107f},     /* 7 */

        {0.0f, 1.5f, 0.0f}                  /* 8 */
    }), TestSuite::Compare::Container);

    CORRADE_COMPARE_AS(cone.indices<UnsignedInt>(), Containers::arrayView<UnsignedInt>({
        0, 4, 1, 5, 2, 6, 3, 7,
        4, 1, 5, 2, 6, 3, 7, 0,

        0, 8, 1, 8, 2, 8, 3, 8
    }), TestSuite::Compare::Container);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::ConeTest)
