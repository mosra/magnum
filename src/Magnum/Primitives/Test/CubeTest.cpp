/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/String.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/FunctionsBatch.h" /* minmax() */
#include "Magnum/Math/Vector4.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct CubeTest: TestSuite::Tester {
    explicit CubeTest();

    void solid();
    template<CubeFlag flag = CubeFlag{}> void solidTextureCoordinates();
    void solidInvalid();
    void solidStrip();
    void solidStripGlsl();
    void wireframe();
};

const struct {
    const char* name;
    Containers::Optional<CubeFlags> flags;
} SolidData[]{
    {"", {}},
    {"explicit empty flags", CubeFlags{}}
};

enum class CubeEdge {
    /* 0 is reserved */

    /* Horizontal edges */
    BottomBack = 1, /* {0, -1, +1} */
    BottomFront,    /* {0, -1, -1} */
    TopBack,        /* {0, +1, +1} */
    TopFront,       /* {0, +1, -1} */

    /* Vertical edges */
    BackLeft,       /* {-1, 0, +1} */
    BackRight,      /* {+1, 0, +1} */
    FrontLeft,      /* {-1, 0, -1} */
    FrontRight,     /* {+1, 0, -1} */

    /* "Depth" edges */
    BottomLeft,     /* {-1, -1, 0} */
    BottomRight,    /* {+1, -1, 0} */
    TopLeft,        /* {-1, +1, 0} */
    TopRight,       /* {+1, +1, 0} */
};

Debug& operator<<(Debug& out, CubeEdge edge) {
    switch(edge) {
        #define _c(value) case CubeEdge::value: return out << #value;
        _c(BottomBack)
        _c(BottomFront)
        _c(TopBack)
        _c(TopFront)
        _c(BackLeft)
        _c(BackRight)
        _c(FrontLeft)
        _c(FrontRight)
        _c(BottomLeft)
        _c(BottomRight)
        _c(TopLeft)
        _c(TopRight)
        #undef _c
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE();
}

const struct {
    const char* name;
    CubeFlags flags;
    /* +X, -X, +Y, -Y, +Z, -Z (same order as GL::CubeMapCoordinate) */
    Vector2 expectedCenters[6];
    /* Cases where less than 12 edges are shared have the rest zero-filled */
    CubeEdge expectedSharedEdges[12];
} SolidTextureCoordinatesData[]{
    {"all same", CubeFlag::TextureCoordinatesAllSame, {
        {0.5f, 0.5f},
        {0.5f, 0.5f},
        {0.5f, 0.5f},
        {0.5f, 0.5f},
        {0.5f, 0.5f},
        {0.5f, 0.5f}
    }, {
        /* (No shared edges in this case) */
    }},
    /* +----+----+----+ 1.0
       | +X | +Y | +Z | 0.75
       +----+----+----+ 0.5
       | -X | -Y | -Z | 0.25
       +----+----+----+ 0.0
     0.0 0.333  0.667 1.0
       0.167  0.5 0.833         */
    {"+ up, - down", CubeFlag::TextureCoordinatesPositiveUpNegativeDown, {
        {0.16667f, 0.75f}, /* +X */
        {0.16667f, 0.25f}, /* -X */
        {0.5f,     0.75f}, /* +Y */
        {0.5f,     0.25f}, /* -Y */
        {0.83333f, 0.75f}, /* +Z */
        {0.83333f, 0.25f}  /* -Z */
    }, {
        /* (*Deliberately* no shared edges in this case either. They could be
           but it'd mean some faces would be rotated, which is just weird. */
    }},
    /* +-----+                   1.0
       |  +Y |                   0.833
       +-tl--+-----+-----+-----+ 0.667
       |  -X bl +Z br +X fr -Z | 0.5
       +-bl--+-----+-----+-----+ 0.333
       |  -Y |                   0.167
       +-----+                   0.0
      0.0   0.25  0.5   0.75  1.0
        0.125 0.375 0.625 0.875   */
    {"-X up, -X down", CubeFlag::TextureCoordinatesNegativeXUpNegativeXDown, {
        {0.625f, 0.5f},     /* +X */
        {0.125f, 0.5f},     /* -X */
        {0.125f, 0.83333f}, /* +Y */
        {0.125f, 0.16667f}, /* -Y */
        {0.375f, 0.5f},     /* +Z */
        {0.875f, 0.5f}      /* -Z */
    }, {
        CubeEdge::TopLeft,
        CubeEdge::BottomLeft,
        CubeEdge::BackLeft,
        CubeEdge::BackRight,
        CubeEdge::FrontRight,
    }},
    /* +-----+
       |  +Y |
       +-tl--+-----+-----+-----+
       |  -X bl +Z br +X fr -Z |
       +-----+-bb--+-----+-----+
             |  -Y |
             +-----+
            0.25  0.5
              0.375               */
    {"-X up, +Z down", CubeFlag::TextureCoordinatesNegativeXUpPositiveZDown, {
        {0.625f, 0.5f},     /* +X */
        {0.125f, 0.5f},     /* -X */
        {0.125f, 0.83333f}, /* +Y */
        {0.375f, 0.16667f}, /* -Y */
        {0.375f, 0.5f},     /* +Z */
        {0.875f, 0.5f}      /* -Z */
    }, {
        CubeEdge::TopLeft,
        CubeEdge::BackLeft,
        CubeEdge::BottomBack,
        CubeEdge::BackRight,
        CubeEdge::FrontRight,
    }},
    /* +-----+
       |  +Y |
       +-tl--+-----+-----+-----+
       |  -X bl +Z br +X fr -Z |
       +-----+-----+-br--+-----+
                   |  -Y |
                   +-----+
                  0.5   0.75
                    0.625         */
    {"-X up, +X down", CubeFlag::TextureCoordinatesNegativeXUpPositiveXDown, {
        {0.625f, 0.5f},     /* +X */
        {0.125f, 0.5f},     /* -X */
        {0.125f, 0.83333f}, /* +Y */
        {0.625f, 0.16667f}, /* -Y */
        {0.375f, 0.5f},     /* +Z */
        {0.875f, 0.5f}      /* -Z */
    }, {
        CubeEdge::TopLeft,
        CubeEdge::BackLeft,
        CubeEdge::BackRight,
        CubeEdge::BottomRight,
        CubeEdge::FrontRight,
    }},
    /* +-----+
       |  +Y |
       +-tl--+-----+-----+-----+
       |  -X bl +Z br +X fr -Z |
       +-----+-----+-----+-bf--+
                         |  -Y |
                         +-----+
                        0.75  1.0
                          0.875   */
    {"-X up, -Z down", CubeFlag::TextureCoordinatesNegativeXUpNegativeZDown, {
        {0.625f, 0.5f},     /* +X */
        {0.125f, 0.5f},     /* -X */
        {0.125f, 0.83333f}, /* +Y */
        {0.875f, 0.16667f}, /* -Y */
        {0.375f, 0.5f},     /* +Z */
        {0.875f, 0.5f}      /* -Z */
    }, {
        CubeEdge::TopLeft,
        CubeEdge::BackLeft,
        CubeEdge::BackRight,
        CubeEdge::FrontRight,
        CubeEdge::BottomFront,
    }},
    /*       +-----+
             |  +Y |
       +-----+-tb--+-----+-----+
       |  -X bl +Z br +X fr -Z |
       +-----+-bb--+-----+-----+
             |  -Y |
             +-----+
            0.25  0.5
              0.375               */
    {"+Z up, +Z down", CubeFlag::TextureCoordinatesPositiveZUpPositiveZDown, {
        {0.625f, 0.5f},     /* +X */
        {0.125f, 0.5f},     /* -X */
        {0.375f, 0.83333f}, /* +Y */
        {0.375f, 0.16667f}, /* -Y */
        {0.375f, 0.5f},     /* +Z */
        {0.875f, 0.5f}      /* -Z */
    }, {
        CubeEdge::TopBack,
        CubeEdge::BackLeft,
        CubeEdge::BackRight,
        CubeEdge::BottomBack,
        CubeEdge::FrontRight,
    }},
    /*       +-----+
             |  +Y |
       +-----+-tb--+-----+-----+
       |  -X bl +Z br +X fr -Z |
       +-----+-bb--+-br--+-----+
                   |  -Y |
                   +-----+
                  0.5  0.75
                    0.625         */
    {"+Z up, +X down", CubeFlag::TextureCoordinatesPositiveZUpPositiveXDown, {
        {0.625f, 0.5f},     /* +X */
        {0.125f, 0.5f},     /* -X */
        {0.375f, 0.83333f}, /* +Y */
        {0.625f, 0.16667f}, /* -Y */
        {0.375f, 0.5f},     /* +Z */
        {0.875f, 0.5f}      /* -Z */
    }, {
        CubeEdge::TopBack,
        CubeEdge::BackLeft,
        CubeEdge::BackRight,
        CubeEdge::FrontRight,
        CubeEdge::BottomRight,
    }},
};


CubeTest::CubeTest() {
    addInstancedTests({&CubeTest::solid},
        Containers::arraySize(SolidData));

    addInstancedTests<CubeTest>({
        &CubeTest::solidTextureCoordinates,
        &CubeTest::solidTextureCoordinates<CubeFlag::Tangents>
    }, Containers::arraySize(SolidTextureCoordinatesData));

    addTests({&CubeTest::solidInvalid,
              &CubeTest::solidStrip,
              &CubeTest::solidStripGlsl,
              &CubeTest::wireframe});
}

void CubeTest::solid() {
    auto&& data = SolidData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Trade::MeshData cube = data.flags ?
        Primitives::cubeSolid(*data.flags) :
        Primitives::cubeSolid();

    CORRADE_COMPARE(cube.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(cube.isIndexed());
    CORRADE_COMPARE(cube.indexCount(), 36);
    CORRADE_COMPARE(cube.vertexCount(), 24);
    CORRADE_COMPARE(cube.attributeCount(), 2);
    CORRADE_COMPARE(cube.indices<UnsignedShort>()[17], 11);
    CORRADE_COMPARE(cube.attribute<Vector3>(Trade::MeshAttribute::Position)[4],
        (Vector3{1.0f, -1.0f, 1.0f}));
    CORRADE_COMPARE(cube.attribute<Vector3>(Trade::MeshAttribute::Normal)[6],
        (Vector3{1.0f, 0.0f, 0.0f}));
}

template<class T> T sampleQuad(const T& a, const T& b, const T& c, const T& d, Vector2 t) {
    /* Assuming the vertex order is the following, which means the second lerp
       has to be in a flipped direction in order to give expected result.
       3--2
       |  |
       0--1 */
    return Math::lerp(Math::lerp(a, b, t[0]),
                      Math::lerp(d, c, t[0]), t[1]);
}

template<CubeFlag flag> void CubeTest::solidTextureCoordinates() {
    auto&& data = SolidTextureCoordinatesData[testCaseInstanceId()];
    setTestCaseDescription(data.name);
    setTestCaseTemplateName(flag == CubeFlag::Tangents ? "CubeFlag::Tangents" : "");

    Trade::MeshData cube = Primitives::cubeSolid(data.flags|flag);
    Containers::StridedArrayView1D<const Vector3> positions = cube.attribute<Vector3>(Trade::MeshAttribute::Position);
    Containers::StridedArrayView1D<const Vector2> textureCoordinates = cube.attribute<Vector2>(Trade::MeshAttribute::TextureCoordinates);

    /* Same as in solid(), to verify basic sanity */
    CORRADE_COMPARE(cube.primitive(), MeshPrimitive::Triangles);
    CORRADE_VERIFY(cube.isIndexed());
    CORRADE_COMPARE(cube.indexCount(), 36);
    CORRADE_COMPARE(cube.vertexCount(), 24);
    CORRADE_COMPARE(cube.attributeCount(), 3 + (flag == CubeFlag::Tangents ? 1 : 0));
    CORRADE_COMPARE(cube.indices<UnsignedShort>()[17], 11);
    CORRADE_COMPARE(positions[4], (Vector3{1.0f, -1.0f, 1.0f}));
    CORRADE_COMPARE(cube.attribute<Vector3>(Trade::MeshAttribute::Normal)[6], (Vector3{1.0f, 0.0f, 0.0f}));

    /* Discover which groups of vertices correspond to which faces, in order
       matching SolidTextureCoordinatesData::expectedCenters, so +X, -X, +Y,
       -Y, +Z, -Z. This could be done just once but who cares, it's just a
       test. It could also be hardcoded but that'll make the test tied too much
       to the particular data, making it more likely that the test passes with
       the data actually being completely wrong. */
    Vector3 faceCenters[6]{
        {+1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {0.0f, +1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 0.0f, +1.0f},
        {0.0f, 0.0f, -1.0f},
    };
    UnsignedInt faceVertexOffsets[6];
    for(UnsignedInt face = 0; face != 6; ++face) {
        CORRADE_ITERATION(face);

        Vector3 center = sampleQuad(positions[face*4 + 0],
                                    positions[face*4 + 1],
                                    positions[face*4 + 2],
                                    positions[face*4 + 3], {0.5f, 0.5f});
        UnsignedInt candidate = 0;
        for(; candidate != Containers::arraySize(faceCenters); ++candidate) {
            if(center == faceCenters[candidate]) {
                faceVertexOffsets[candidate] = face*4;
                break;
            }
        }
        CORRADE_VERIFY(candidate != Containers::arraySize(faceCenters));
    }

    /* Discover which groups of vertices correspond to which edges, in order
       matching the CubeEdge enum above. Same as above, this could be done just
       once, or hardcoded, etc., but it's not. */
    Vector3 edgeCenters[12] {
        {0.0f, -1.0f, +1.0f}, /* BottomBack */
        {0.0f, -1.0f, -1.0f}, /* BottomFront */
        {0.0f, +1.0f, +1.0f}, /* TopBack */
        {0.0f, +1.0f, -1.0f}, /* TopFront */

        {-1.0f, 0.0f, +1.0f}, /* BackLeft */
        {+1.0f, 0.0f, +1.0f}, /* BackRight */
        {-1.0f, 0.0f, -1.0f}, /* FrontLeft */
        {+1.0f, 0.0f, -1.0f}, /* FrontRight */

        {-1.0f, -1.0f, 0.0f}, /* BottomLeft */
        {+1.0f, -1.0f, 0.0f}, /* BottomRight */
        {-1.0f, +1.0f, 0.0f}, /* TopLeft */
        {+1.0f, +1.0f, 0.0f}, /* TopRight */
    };
    /* Each of 12 edges is shared by exactly 2 faces */
    Vector2ui edgeVertices[12][2];
    for(UnsignedInt face = 0; face != 6; ++face) {
        CORRADE_ITERATION(face);

        /* Four edges of the quad. Assuming ordering like below, if that
           wouldn't be the case, the CORRADE_VERIFY after would fail.
            3--2
            |  |
            0--1 */
        for(Vector2ui edge: {
            Vector2ui{face*4 + 0, face*4 + 1},
            Vector2ui{face*4 + 1, face*4 + 2},
            Vector2ui{face*4 + 2, face*4 + 3},
            Vector2ui{face*4 + 3, face*4 + 0}
        }) {
            Vector3 center = Math::lerp(positions[edge[0]],
                                        positions[edge[1]], 0.5f);

            UnsignedInt candidate = 0;
            for(; candidate != Containers::arraySize(edgeCenters); ++candidate) {
                if(center == edgeCenters[candidate]) {
                    if(edgeVertices[candidate][0].isZero())
                        edgeVertices[candidate][0] = edge;
                    else if(edgeVertices[candidate][1].isZero())
                        edgeVertices[candidate][1] = edge;
                    else CORRADE_FAIL("Too many shared edges.");
                    break;
                }
            }
            CORRADE_ITERATION(edge);
            CORRADE_VERIFY(candidate != Containers::arraySize(edgeCenters));
        }
    }
    /* At this point, if neither the above CORRADE_VERIFY() nor the
       CORRADE_FAIL() fire, for each of the 6 faces the 4 edges were assigned,
       filling all 24 array entries */

    /* For each face verify that the sampled texture coordinates at the center
       match the expectation */
    for(UnsignedInt face = 0; face != 6; ++face) {
        UnsignedInt vertexOffset = faceVertexOffsets[face];
        CORRADE_ITERATION("face" << face << "at offset" << vertexOffset);
        Vector2 center = sampleQuad(
            textureCoordinates[vertexOffset + 0],
            textureCoordinates[vertexOffset + 1],
            textureCoordinates[vertexOffset + 2],
            textureCoordinates[vertexOffset + 3], {0.5f, 0.5f});
        CORRADE_COMPARE(center, data.expectedCenters[face]);
    }

    /* Verify that the expected shared edges indeed have the same texture
       coordinates for both faces */
    for(CubeEdge edge: data.expectedSharedEdges) {
        /* When we reach an edge that's zero it's the end of the list */
        if(edge == CubeEdge{})
            break;

        /* Sanity check -- the two edges should be filled and have contents
           that aren't the same */
        const Vector2ui(&vertices)[2] = edgeVertices[UnsignedInt(edge) - 1];
        CORRADE_ITERATION(edge << "edge with vertices" << Debug::packed << vertices[0] << "and" << Debug::packed << vertices[1]);
        CORRADE_VERIFY(!vertices[0].isZero() &&
                       !vertices[1].isZero());
        CORRADE_VERIFY(vertices[0] != vertices[1] &&
                       vertices[0] != vertices[1].flipped());

        /* The edge should match in one or the other direction */
        Vector2 a0 = textureCoordinates[vertices[0][0]];
        Vector2 a1 = textureCoordinates[vertices[0][1]];
        Vector2 b0 = textureCoordinates[vertices[1][0]];
        Vector2 b1 = textureCoordinates[vertices[1][1]];
        CORRADE_VERIFY((a0 == b0 && a1 == b1) ||
                       (a0 == b1 && a1 == b0));
    }

    /* The texture coordinates should always span the whole [0, 0] to [1, 1]
       range. That may mean the faces won't be square if using a square
       texture, but in practice the texture would have a size matching the
       texture coordinate layout, so e.g. with a 4:3 ratio for the
       NegativeXUpNegativeXDown variant. */
    CORRADE_COMPARE(Math::minmax(textureCoordinates), Containers::pair(Vector2{0.0f}, Vector2{1.0f}));

    /* If tangents are enabled, check their properties also */
    if(flag == CubeFlag::Tangents) {
        Containers::StridedArrayView1D<const Vector3> normals = cube.attribute<Vector3>(Trade::MeshAttribute::Normal);
        Containers::StridedArrayView1D<const Vector4> tangents = cube.attribute<Vector4>(Trade::MeshAttribute::Tangent);

        /* Normals and tangents should be the same for all vertices in a face,
           and perpendicular in all cases */
        for(UnsignedInt face = 0; face != 6; ++face) {
            CORRADE_ITERATION(face);
            CORRADE_COMPARE(Math::dot(normals[face*4], tangents[face*4].xyz()), 0.0f);
            CORRADE_COMPARE(normals[face*4].dot(), 1.0f);
            CORRADE_COMPARE(tangents[face*4].xyz().dot(), 1.0f);
            CORRADE_COMPARE(Math::abs(tangents[face*4].w()), 1.0f);
            for(UnsignedInt vertex = 1; vertex != 4; ++vertex) {
                CORRADE_ITERATION(vertex);
                CORRADE_COMPARE(normals[face*4 + vertex], normals[face*4]);
                CORRADE_COMPARE(tangents[face*4 + vertex], tangents[face*4]);
            }
        }

        /* For each face, sample in a position off center on X and Y */
        for(UnsignedInt face = 0; face != 6; ++face) {
            CORRADE_ITERATION(face);

            Vector3 center = sampleQuad(
                positions[face*4 + 0], positions[face*4 + 1],
                positions[face*4 + 2], positions[face*4 + 3], {0.5f, 0.5f});
            Vector2 centerTexture = sampleQuad(
                textureCoordinates[face*4 + 0], textureCoordinates[face*4 + 1],
                textureCoordinates[face*4 + 2], textureCoordinates[face*4 + 3],
                {0.5f, 0.5f});
            Vector3 tangent = tangents[face*4].xyz();
            Vector3 bitangent = Math::cross(normals[face*4],
                tangents[face*4].xyz())*tangents[face*4].w();

            Vector3 offset[]{
                sampleQuad(positions[face*4 + 0], positions[face*4 + 1],
                           positions[face*4 + 2], positions[face*4 + 3],
                           {0.75f, 0.5f}),
                sampleQuad(positions[face*4 + 0], positions[face*4 + 1],
                           positions[face*4 + 2], positions[face*4 + 3],
                           {0.5f, 0.75f})
            };
            Vector2 offsetTexture[]{
                sampleQuad(textureCoordinates[face*4 + 0], textureCoordinates[face*4 + 1],
                           textureCoordinates[face*4 + 2], textureCoordinates[face*4 + 3],
                           {0.75f, 0.5f}),
                sampleQuad(textureCoordinates[face*4 + 0], textureCoordinates[face*4 + 1],
                           textureCoordinates[face*4 + 2], textureCoordinates[face*4 + 3],
                           {0.5f, 0.75f})
            };

            for(Int i: {0, 1}) {
                CORRADE_ITERATION(i);

                /* If the shift is in direction of tangent, texture coordinates
                   should be the same in Y and different with a matching sign
                   in X */
                if(Math::notEqual(Math::dot(offset[i] - center, tangent), 0.0f)) {
                    Vector2 delta = offsetTexture[i] - centerTexture;
                    CORRADE_COMPARE(delta.y(), 0.0f);
                    CORRADE_COMPARE(Math::sign(delta.x()), Math::sign(Math::dot(offset[i] - center, tangent)));

                /* If the shift is in direction of bitangent, texture
                   coordinates should be the same in X and different with a
                   matching sign in Y */
                } else if(Math::notEqual(Math::dot(offset[i] - center, bitangent), 0.0f)) {
                    Vector2 delta = offsetTexture[i] - centerTexture;
                    CORRADE_COMPARE(delta.x(), 0.0f);
                    CORRADE_COMPARE(Math::sign(delta.y()), Math::sign(Math::dot(offset[i] - center, bitangent)));

                } else CORRADE_FAIL(Debug::packed << offset[i] - center << "is parallel to neither" << Debug::packed << tangent << "nor" << Debug::packed << bitangent);
            }
        }
    }
}

void CubeTest::solidInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    Primitives::cubeSolid(CubeFlag::Tangents);
    Primitives::cubeSolid(CubeFlag(UnsignedInt(CubeFlag::TextureCoordinatesPositiveZUpPositiveXDown) + 2));
    CORRADE_COMPARE_AS(out,
        "Primitives::cubeSolid(): a texture coordinate option has to be picked if tangents are enabled\n"
        "Primitives::cubeSolid(): unrecognized texture coordinate option 0x12\n",
        TestSuite::Compare::String);
}

void CubeTest::solidStrip() {
    Trade::MeshData cube = Primitives::cubeSolidStrip();

    CORRADE_COMPARE(cube.primitive(), MeshPrimitive::TriangleStrip);
    CORRADE_VERIFY(!cube.isIndexed());
    CORRADE_COMPARE(cube.vertexCount(), 14);
    CORRADE_COMPARE(cube.attributeCount(), 1);
    CORRADE_COMPARE(cube.attribute<Vector3>(Trade::MeshAttribute::Position)[4],
        (Vector3{-1.0f, -1.0f, -1.0f}));
}

void CubeTest::solidStripGlsl() {
    Trade::MeshData cube = Primitives::cubeSolidStrip();

    /* Yes, really. */
    auto solidStripVertex = [](UnsignedInt gl_VertexID) {
        typedef Vector3 vec3;
        #define mix Math::lerp
        #include "data.glsl"
        #undef mix
        return corner;
    };

    auto vertices = cube.attribute<Vector3>(Trade::MeshAttribute::Position);
    for(UnsignedInt i = 0; i != cube.vertexCount(); ++i) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(solidStripVertex(i), vertices[i]);
    }
}

void CubeTest::wireframe() {
    Trade::MeshData cube = Primitives::cubeWireframe();

    CORRADE_COMPARE(cube.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(cube.isIndexed());
    CORRADE_COMPARE(cube.indexCount(), 24);
    CORRADE_COMPARE(cube.vertexCount(), 8);
    CORRADE_COMPARE(cube.attributeCount(), 1);
    CORRADE_COMPARE(cube.indices<UnsignedShort>()[5], 3);
    CORRADE_COMPARE(cube.attribute<Vector3>(Trade::MeshAttribute::Position)[5],
        (Vector3{1.0f, -1.0f, -1.0f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::CubeTest)
