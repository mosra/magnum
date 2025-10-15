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

#include "Cube.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives {

namespace {

/* not 8-bit because GPUs (and Vulkan) don't like it nowadays */
constexpr UnsignedShort IndicesSolid[]{
    /* 3--2
       | /|
       |/ |
       0--1 */
     0,  1,  2,  0,  2,  3, /* +Z */
     4,  5,  6,  4,  6,  7, /* +X */
     8,  9, 10,  8, 10, 11, /* +Y */
    12, 13, 14, 12, 14, 15, /* -Z */
    16, 17, 18, 16, 18, 19, /* -Y */
    20, 21, 22, 20, 22, 23  /* -X */
};
constexpr struct VertexSolid {
    Vector3 position;
    Vector3 normal;
} VerticesSolid[]{
    /*    11----10        23 14----15
         /  +Y  /  6      /| |      |
        8------9 / |    22 | |  -Z  |
       3------2 7  |    |-X| |      |
       |      | |+X|    | 20 13----12
       |  +Z  | |  5    | / 16----17
       |      | | /     21 /  -Y  /
       0------1 4         19----18    */

    /* 0, +Z */
    {{-1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},
    {{ 1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},
    {{ 1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},
    {{-1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}},
    /* 4, +X */
    {{ 1.0f, -1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}},
    {{ 1.0f, -1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f}},
    /* 8, +Y */
    {{-1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}},
    {{-1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f}},
    /* 12, -Z */
    {{ 1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
    {{-1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
    {{-1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
    {{ 1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}},
    /* 16, -Y */
    {{-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}},
    {{ 1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f}},
    {{ 1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}},
    {{-1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f}},
    /* 20, -X */
    {{-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}},
    {{-1.0f, -1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}},
    {{-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}},
    {{-1.0f,  1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}}
};
constexpr Trade::MeshAttributeData AttributesSolid[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesSolid, &VerticesSolid[0].position,
            Containers::arraySize(VerticesSolid), sizeof(VertexSolid))},
    Trade::MeshAttributeData{Trade::MeshAttribute::Normal,
        Containers::stridedArrayView(VerticesSolid, &VerticesSolid[0].normal,
            Containers::arraySize(VerticesSolid), sizeof(VertexSolid))}
};

}

Trade::MeshData cubeSolid() {
    return Trade::MeshData{MeshPrimitive::Triangles,
        Trade::DataFlag::Global, IndicesSolid, Trade::MeshIndexData{IndicesSolid},
        Trade::DataFlag::Global, VerticesSolid, Trade::meshAttributeDataNonOwningArray(AttributesSolid)};
}

namespace {

/* GCC 4.8 dies with "error: array must be initialized with a brace-enclosed
   initializer" if it's `constexpr Vector2 TextureCoordinates[][24]`. Turning
   the second dimension into a struct seems to work around the problem. */
/** @todo drop once GCC 4.8 is gone */
constexpr struct {
    Vector2 v[24];
} TextureCoordinates[]{
    /* All same
       3--2
       |  |
       0--1 */
    {{/* 0, +Z */
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
      /* 4, +X */
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
      /* 8, +Y */
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
      /* 12, -Z */
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
      /* 16, -Y */
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f},
      /* 20, -X */
      {0.0f, 0.0f},
      {1.0f, 0.0f},
      {1.0f, 1.0f},
      {0.0f, 1.0f}}},
    /* Positive up, negative down
       +----+----3----2 1.0
       | +X | +Y | +Z |
       +----+----0----1 0.5
       | -X | -Y | -Z |
       +----+----+----+ 0.0
     0.0 0.333  0.667 1.0  */
    {{/* 0, +Z */
      {2.0f/3.0f, 0.5f},
      {3.0f/3.0f, 0.5f},
      {3.0f/3.0f, 1.0f},
      {2.0f/3.0f, 1.0f},
      /* 4, +X */
      {0.0f/3.0f, 0.5f},
      {1.0f/3.0f, 0.5f},
      {1.0f/3.0f, 1.0f},
      {0.0f/3.0f, 1.0f},
      /* 8, +Y */
      {1.0f/3.0f, 0.5f},
      {2.0f/3.0f, 0.5f},
      {2.0f/3.0f, 1.0f},
      {1.0f/3.0f, 1.0f},
      /* 12, -Z */
      {2.0f/3.0f, 0.0f},
      {3.0f/3.0f, 0.0f},
      {3.0f/3.0f, 0.5f},
      {2.0f/3.0f, 0.5f},
      /* 16, -Y */
      {1.0f/3.0f, 0.0f},
      {2.0f/3.0f, 0.0f},
      {2.0f/3.0f, 0.5f},
      {1.0f/3.0f, 0.5f},
      /* 20, -X */
      {0.0f/3.0f, 0.0f},
      {1.0f/3.0f, 0.0f},
      {1.0f/3.0f, 0.5f},
      {0.0f/3.0f, 0.5f}}},
    /* -X up, -X down
       +----+                1.0
       | +Y |
       A----C----E----G----+ 0.667
       | -X | +Z | +X | -Z |
       B----D----F----H----+ 0.333
       | -Y |
       +----+                0.0
      0.0  0.25 0.5  0.75 1.0   */
    {{/* 0, +Z */
      {0.25f, 1.0f/3.0f},  /* D */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.50f, 2.0f/3.0f},  /* E */
      {0.25f, 2.0f/3.0f},  /* C */
      /* 4, +X */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.75f, 1.0f/3.0f},  /* H */
      {0.75f, 2.0f/3.0f},  /* G */
      {0.50f, 2.0f/3.0f},  /* E */
      /* 8, +Y */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.25f, 3.0f/3.0f},
      {0.00f, 3.0f/3.0f},
      {0.00f, 2.0f/3.0f},  /* A */
      /* 12, -Z */
      {0.75f, 1.0f/3.0f},  /* H */
      {1.00f, 1.0f/3.0f},
      {1.00f, 2.0f/3.0f},
      {0.75f, 2.0f/3.0f},  /* G */
      /* 16, -Y */
      {0.00f, 1.0f/3.0f},  /* B */
      {0.00f, 0.0f/3.0f},
      {0.25f, 0.0f/3.0f},
      {0.25f, 1.0f/3.0f},  /* D */
      /* 20, -X */
      {0.00f, 1.0f/3.0f},  /* B */
      {0.25f, 1.0f/3.0f},  /* D */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.00f, 2.0f/3.0f}}},/* A */
    /* -X up, +Z down
       +----+
       | +Y |
       A----C----E----G----+
       | -X | +Z | +X | -Z |
       +----D----F----H----+
            | -Y |
            +----+
      0.0  0.25 0.5 */
    {{/* 0, +Z */
      {0.25f, 1.0f/3.0f},  /* D */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.50f, 2.0f/3.0f},  /* E */
      {0.25f, 2.0f/3.0f},  /* C */
      /* 4, +X */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.75f, 1.0f/3.0f},  /* H */
      {0.75f, 2.0f/3.0f},  /* G */
      {0.50f, 2.0f/3.0f},  /* E */
      /* 8, +Y */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.25f, 3.0f/3.0f},
      {0.00f, 3.0f/3.0f},
      {0.00f, 2.0f/3.0f},  /* A */
      /* 12, -Z */
      {0.75f, 1.0f/3.0f},  /* H */
      {1.00f, 1.0f/3.0f},
      {1.00f, 2.0f/3.0f},
      {0.75f, 2.0f/3.0f},  /* G */
      /* 16, -Y */
      {0.25f, 0.0f/3.0f},
      {0.50f, 0.0f/3.0f},
      {0.50f, 1.0f/3.0f},  /* F */
      {0.25f, 1.0f/3.0f},  /* D */
      /* 20, -X */
      {0.00f, 1.0f/3.0f},
      {0.25f, 1.0f/3.0f},  /* D */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.00f, 2.0f/3.0f}}},/* A */
    /* -X up, +X down
       +----+
       | +Y |
       A----C----E----G----+
       | -X | +Z | +X | -Z |
       +----D----F----H----+
                 | -Y |
                 +----+
      0.0       0.5  0.75 */
    {{/* 0, +Z */
      {0.25f, 1.0f/3.0f},  /* D */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.50f, 2.0f/3.0f},  /* E */
      {0.25f, 2.0f/3.0f},  /* C */
      /* 4, +X */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.75f, 1.0f/3.0f},  /* H */
      {0.75f, 2.0f/3.0f},  /* G */
      {0.50f, 2.0f/3.0f},  /* E */
      /* 8, +Y */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.25f, 3.0f/3.0f},
      {0.00f, 3.0f/3.0f},
      {0.00f, 2.0f/3.0f},  /* A */
      /* 12, -Z */
      {0.75f, 1.0f/3.0f},  /* H */
      {1.00f, 1.0f/3.0f},
      {1.00f, 2.0f/3.0f},
      {0.75f, 2.0f/3.0f},  /* G */
      /* 16, -Y */
      {0.75f, 0.0f/3.0f},
      {0.75f, 1.0f/3.0f},  /* H */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.50f, 0.0f/3.0f},
      /* 20, -X */
      {0.00f, 1.0f/3.0f},
      {0.25f, 1.0f/3.0f},  /* D */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.00f, 2.0f/3.0f}}},/* A */
    /* -X up, -Z down
       +----+
       | +Y |
       A----C----E----G----+
       | -X | +Z | +X | -Z |
       +----D----F----H----B
                      | -Y |
                      +----+
       0.0           0.75 1.0 */
    {{/* 0, +Z */
      {0.25f, 1.0f/3.0f},  /* D */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.50f, 2.0f/3.0f},  /* E */
      {0.25f, 2.0f/3.0f},  /* C */
      /* 4, +X */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.75f, 1.0f/3.0f},  /* H */
      {0.75f, 2.0f/3.0f},  /* G */
      {0.50f, 2.0f/3.0f},  /* E */
      /* 8, +Y */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.25f, 3.0f/3.0f},
      {0.00f, 3.0f/3.0f},
      {0.00f, 2.0f/3.0f},  /* A */
      /* 12, -Z */
      {0.75f, 1.0f/3.0f},  /* H */
      {1.00f, 1.0f/3.0f},  /* B */
      {1.00f, 2.0f/3.0f},
      {0.75f, 2.0f/3.0f},  /* G */
      /* 16, -Y */
      {1.00f, 1.0f/3.0f},  /* B */
      {0.75f, 1.0f/3.0f},  /* H */
      {0.75f, 0.0f/3.0f},
      {1.00f, 0.0f/3.0f},
      /* 20, -X */
      {0.00f, 1.0f/3.0f},
      {0.25f, 1.0f/3.0f},  /* D */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.00f, 2.0f/3.0f}}},/* A */
    /* +Z up, +Z down
            +----+
            | +Y |
       +----C----E----G----+
       | -X | +Z | +X | -Z |
       +----D----F----H----+
            | -Y |
            +----+
           0.25 0.5 */
    {{/* 0, +Z */
      {0.25f, 1.0f/3.0f},  /* D */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.50f, 2.0f/3.0f},  /* E */
      {0.25f, 2.0f/3.0f},  /* C */
      /* 4, +X */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.75f, 1.0f/3.0f},  /* H */
      {0.75f, 2.0f/3.0f},  /* G */
      {0.50f, 2.0f/3.0f},  /* E */
      /* 8, +Y */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.50f, 2.0f/3.0f},  /* E */
      {0.50f, 3.0f/3.0f},
      {0.25f, 3.0f/3.0f},
      /* 12, -Z */
      {0.75f, 1.0f/3.0f},  /* H */
      {1.00f, 1.0f/3.0f},
      {1.00f, 2.0f/3.0f},
      {0.75f, 2.0f/3.0f},  /* G */
      /* 16, -Y */
      {0.25f, 0.0f/3.0f},
      {0.50f, 0.0f/3.0f},
      {0.50f, 1.0f/3.0f},  /* F */
      {0.25f, 1.0f/3.0f},  /* D */
      /* 20, -X */
      {0.00f, 1.0f/3.0f},
      {0.25f, 1.0f/3.0f},  /* D */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.00f, 2.0f/3.0f}}},
    /* +Z up, +X down
            +----+
            | +Y |
       +----C----E----G----+
       | -X | +Z | +X | -Z |
       +----D----F----H----+
                 | -Y |
                 +----+
                0.5  0.75 */
    {{/* 0, +Z */
      {0.25f, 1.0f/3.0f},  /* D */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.50f, 2.0f/3.0f},  /* E */
      {0.25f, 2.0f/3.0f},  /* C */
      /* 4, +X */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.75f, 1.0f/3.0f},  /* H */
      {0.75f, 2.0f/3.0f},  /* G */
      {0.50f, 2.0f/3.0f},  /* E */
      /* 8, +Y */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.50f, 2.0f/3.0f},  /* E */
      {0.50f, 3.0f/3.0f},
      {0.25f, 3.0f/3.0f},
      /* 12, -Z */
      {0.75f, 1.0f/3.0f},  /* H */
      {1.00f, 1.0f/3.0f},
      {1.00f, 2.0f/3.0f},
      {0.75f, 2.0f/3.0f},  /* G */
      /* 16, -Y */
      {0.75f, 0.0f/3.0f},
      {0.75f, 1.0f/3.0f},  /* H */
      {0.50f, 1.0f/3.0f},  /* F */
      {0.50f, 0.0f/3.0f},
      /* 20, -X */
      {0.00f, 1.0f/3.0f},
      {0.25f, 1.0f/3.0f},  /* D */
      {0.25f, 2.0f/3.0f},  /* C */
      {0.00f, 2.0f/3.0f}}},
};

/* The tangent is the same for all four vertices in each face so it's just 6
   instead of 24. GCC 4.8 dies with "error: array must be initialized with a
   brace-enclosed initializer" if it's `constexpr Vector4 Tangents[][6]`.
   Turning the second dimension into a struct seems to work around the
   problem. */
/** @todo drop once GCC 4.8 is gone */
constexpr struct {
    Vector4 v[6];
} Tangents[8]{
    /* All same. Well, tangents are *not* all same in this case. */
    {{{ 1.0f,  0.0f,  0.0f, 1.0f},  /* +Z */
      { 0.0f,  0.0f, -1.0f, 1.0f},  /* +X */
      { 1.0f,  0.0f,  0.0f, 1.0f},  /* +Y */
      {-1.0f,  0.0f,  0.0f, 1.0f},  /* -Z */
      { 1.0f,  0.0f,  0.0f, 1.0f},  /* -Y */
      { 0.0f,  0.0f,  1.0f, 1.0f}}},/* -X */
    /* Positive up, negative down */
    {{{ 1.0f,  0.0f,  0.0f, 1.0f},  /* +Z */
      { 0.0f,  0.0f, -1.0f, 1.0f},  /* +X */
      { 1.0f,  0.0f,  0.0f, 1.0f},  /* +Y */
      {-1.0f,  0.0f,  0.0f, 1.0f},  /* -Z */
      { 1.0f,  0.0f,  0.0f, 1.0f},  /* -Y */
      { 0.0f,  0.0f,  1.0f, 1.0f}}},/* -X */
    /* -X up, -X down */
    {{{ 1.0f,  0.0f,  0.0f, 1.0f},  /* +Z */
      { 0.0f,  0.0f, -1.0f, 1.0f},  /* +X */
      { 0.0f,  0.0f,  1.0f, 1.0f},  /* +Y */
      {-1.0f,  0.0f,  0.0f, 1.0f},  /* -Z */
      { 0.0f,  0.0f,  1.0f, 1.0f},  /* -Y */
      { 0.0f,  0.0f,  1.0f, 1.0f}}},/* -X */
    /* -X up, +Z down */
    {{{ 1.0f,  0.0f,  0.0f, 1.0f},  /* +Z */
      { 0.0f,  0.0f, -1.0f, 1.0f},  /* +X */
      { 0.0f,  0.0f,  1.0f, 1.0f},  /* +Y */
      {-1.0f,  0.0f,  0.0f, 1.0f},  /* -Z */
      { 1.0f,  0.0f,  0.0f, 1.0f},  /* -Y */
      { 0.0f,  0.0f,  1.0f, 1.0f}}},/* -X */
    /* -X up, +X down */
    {{{ 1.0f,  0.0f,  0.0f, 1.0f},  /* +Z */
      { 0.0f,  0.0f, -1.0f, 1.0f},  /* +X */
      { 0.0f,  0.0f,  1.0f, 1.0f},  /* +Y */
      {-1.0f,  0.0f,  0.0f, 1.0f},  /* -Z */
      { 0.0f,  0.0f, -1.0f, 1.0f},  /* -Y */
      { 0.0f,  0.0f,  1.0f, 1.0f}}},/* -X */
    /* -X up, -Z down */
    {{{ 1.0f,  0.0f,  0.0f, 1.0f},  /* +Z */
      { 0.0f,  0.0f, -1.0f, 1.0f},  /* +X */
      { 0.0f,  0.0f,  1.0f, 1.0f},  /* +Y */
      {-1.0f,  0.0f,  0.0f, 1.0f},  /* -Z */
      {-1.0f,  0.0f,  0.0f, 1.0f},  /* -Y */
      { 0.0f,  0.0f,  1.0f, 1.0f}}},/* -X */
    /* +Z up, +Z down */
    {{{ 1.0f,  0.0f,  0.0f, 1.0f},  /* +Z */
      { 0.0f,  0.0f, -1.0f, 1.0f},  /* +X */
      { 1.0f,  0.0f,  0.0f, 1.0f},  /* +Y */
      {-1.0f,  0.0f,  0.0f, 1.0f},  /* -Z */
      { 1.0f,  0.0f,  0.0f, 1.0f},  /* -Y */
      { 0.0f,  0.0f,  1.0f, 1.0f}}},/* -X */
    /* +Z up, +X down */
    {{{ 1.0f,  0.0f,  0.0f, 1.0f},  /* +Z */
      { 0.0f,  0.0f, -1.0f, 1.0f},  /* +X */
      { 1.0f,  0.0f,  0.0f, 1.0f},  /* +Y */
      {-1.0f,  0.0f,  0.0f, 1.0f},  /* -Z */
      { 0.0f,  0.0f, -1.0f, 1.0f},  /* -Y */
      { 0.0f,  0.0f,  1.0f, 1.0f}}},/* -X */
};

}

Trade::MeshData cubeSolid(const CubeFlags flags) {
    const UnsignedInt textureCoordinateVariant = UnsignedByte(flags) >> 1;
    CORRADE_ASSERT(textureCoordinateVariant <= Containers::arraySize(TextureCoordinates),
        /* Since the enum isn't really a bitflag, implementing a debug printer
           for it doesn't really make sense anyway. But be at least a bit
           helpful and print the assumed TextureCoordinates flag value w/o the
           other bits. */
        "Primitives::cubeSolid(): unrecognized texture coordinate option" << Debug::hex << UnsignedByte(flags & ~CubeFlag::Tangents), (Trade::MeshData{MeshPrimitive::Triangles, 0}));
    CORRADE_ASSERT(!(flags & CubeFlag::Tangents) || textureCoordinateVariant,
        "Primitives::cubeSolid(): a texture coordinate option has to be picked if tangents are enabled", (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    /* Return the compile-time data if nothing extra is requested */
    if(!flags)
        return cubeSolid();

    /* Calculate attribute count and vertex size */
    std::size_t stride = sizeof(Vector3) + sizeof(Vector3);
    std::size_t attributeCount = 2;
    if(flags & CubeFlag::Tangents) {
        stride += sizeof(Vector4);
        ++attributeCount;
    }
    if(textureCoordinateVariant) {
        stride += sizeof(Vector2);
        ++attributeCount;
    }

    /* Set up the layout */
    Containers::Array<char> vertexData{NoInit, 24*stride};
    Containers::Array<Trade::MeshAttributeData> attributeData{attributeCount};
    std::size_t attributeIndex = 0;
    std::size_t attributeOffset = 0;

    Containers::StridedArrayView1D<Vector3> positions{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + attributeOffset),
        24, std::ptrdiff_t(stride)};
    attributeData[attributeIndex++] = Trade::MeshAttributeData{
        Trade::MeshAttribute::Position, positions};
    attributeOffset += sizeof(Vector3);

    Containers::StridedArrayView1D<Vector3> normals{vertexData,
        reinterpret_cast<Vector3*>(vertexData.data() + sizeof(Vector3)),
        24, std::ptrdiff_t(stride)};
    attributeData[attributeIndex++] = Trade::MeshAttributeData{
        Trade::MeshAttribute::Normal, normals};
    attributeOffset += sizeof(Vector3);

    Containers::StridedArrayView1D<Vector4> tangents;
    if(flags & CubeFlag::Tangents) {
        tangents = Containers::StridedArrayView1D<Vector4>{vertexData,
            reinterpret_cast<Vector4*>(vertexData.data() + attributeOffset),
            24, std::ptrdiff_t(stride)};
        attributeData[attributeIndex++] = Trade::MeshAttributeData{
            Trade::MeshAttribute::Tangent, tangents};
        attributeOffset += sizeof(Vector4);
    }

    Containers::StridedArrayView1D<Vector2> textureCoordinates;
    if(textureCoordinateVariant) {
            textureCoordinates = Containers::StridedArrayView1D<Vector2>{vertexData,
            reinterpret_cast<Vector2*>(vertexData.data() + attributeOffset),
            24, std::ptrdiff_t(stride)};
        attributeData[attributeIndex++] = Trade::MeshAttributeData{
            Trade::MeshAttribute::TextureCoordinates, textureCoordinates};
        attributeOffset += sizeof(Vector2);
    }

    CORRADE_INTERNAL_ASSERT(attributeIndex == attributeCount);
    CORRADE_INTERNAL_ASSERT(attributeOffset == stride);

    /* Fill the data */
    for(std::size_t i = 0; i != 24; ++i) {
        positions[i] = VerticesSolid[i].position;
        normals[i] = VerticesSolid[i].normal;
    }
    if(textureCoordinateVariant) {
        for(std::size_t i = 0; i != 24; ++i)
            textureCoordinates[i] = TextureCoordinates[textureCoordinateVariant - 1].v[i];
    }
    if(flags & CubeFlag::Tangents) {
        for(std::size_t i = 0; i != 6; ++i)
            for(std::size_t j = 0; j != 4; ++j)
                tangents[i*4 + j] = Tangents[textureCoordinateVariant - 1].v[i];
    }

    return Trade::MeshData{MeshPrimitive::Triangles,
        Trade::DataFlag::Global, IndicesSolid, Trade::MeshIndexData{IndicesSolid},
        Utility::move(vertexData), Utility::move(attributeData)};
}

namespace {

/* Can't be just an array of Vector3 because MSVC 2015 is special. See
   Crosshair.cpp for details. */
constexpr struct VertexSolidStrip {
    Vector3 position;
} VerticesSolidStrip[]{
    /* Sources:
        https://twitter.com/Donzanoid/status/436843034966507520
        https://www.asmcommunity.net/forums/topic/6284/#post-45209
        https://gist.github.com/cdwfs/2cab675b333632d940cf

        0---2---3---1
        |E /|\ A|H /|
        | / | \ | / |
        |/ D|B \|/ I|
        4---7---6---5
            |C /|
            | / |
            |/ J|
            4---5
            |\ K|
            | \ |
            |L \|
            0---1
            |\ G|
            | \ |
            |F \|
            2---3
    */
    {{ 1.0f,  1.0f,  1.0f}}, /* 3 */
    {{-1.0f,  1.0f,  1.0f}}, /* 2 */
    {{ 1.0f, -1.0f,  1.0f}}, /* 6 */
    {{-1.0f, -1.0f,  1.0f}}, /* 7 */
    {{-1.0f, -1.0f, -1.0f}}, /* 4 */
    {{-1.0f,  1.0f,  1.0f}}, /* 2 */
    {{-1.0f,  1.0f, -1.0f}}, /* 0 */
    {{ 1.0f,  1.0f,  1.0f}}, /* 3 */
    {{ 1.0f,  1.0f, -1.0f}}, /* 1 */
    {{ 1.0f, -1.0f,  1.0f}}, /* 6 */
    {{ 1.0f, -1.0f, -1.0f}}, /* 5 */
    {{-1.0f, -1.0f, -1.0f}}, /* 4 */
    {{ 1.0f,  1.0f, -1.0f}}, /* 1 */
    {{-1.0f,  1.0f, -1.0f}}  /* 0 */
};
constexpr Trade::MeshAttributeData AttributesSolidStrip[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesSolidStrip, &VerticesSolidStrip[0].position,
            Containers::arraySize(VerticesSolidStrip), sizeof(VertexSolidStrip))}
};

}

Trade::MeshData cubeSolidStrip() {
    return Trade::MeshData{MeshPrimitive::TriangleStrip,
        Trade::DataFlag::Global, VerticesSolidStrip, Trade::meshAttributeDataNonOwningArray(AttributesSolidStrip)};
}

namespace {

/* not 8-bit because GPUs (and Vulkan) don't like it nowadays */
constexpr UnsignedShort IndicesWireframe[]{
    0, 1, 1, 2, 2, 3, 3, 0, /* +Z */
    4, 5, 5, 6, 6, 7, 7, 4, /* -Z */
    1, 5, 2, 6,             /* +X */
    0, 4, 3, 7              /* -X */
};
/* Can't be just an array of Vector3 because MSVC 2015 is special. See
   Crosshair.cpp for details. */
constexpr struct VertexWireframe {
    Vector3 position;
} VerticesWireframe[]{
    {{-1.0f, -1.0f,  1.0f}},
    {{ 1.0f, -1.0f,  1.0f}},
    {{ 1.0f,  1.0f,  1.0f}},
    {{-1.0f,  1.0f,  1.0f}},

    {{-1.0f, -1.0f, -1.0f}},
    {{ 1.0f, -1.0f, -1.0f}},
    {{ 1.0f,  1.0f, -1.0f}},
    {{-1.0f,  1.0f, -1.0f}}
};
constexpr Trade::MeshAttributeData AttributesWireframe[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position,
        Containers::stridedArrayView(VerticesWireframe, &VerticesWireframe[0].position,
            Containers::arraySize(VerticesWireframe), sizeof(VertexWireframe))}
};

}

Trade::MeshData cubeWireframe() {
    return Trade::MeshData{MeshPrimitive::Lines,
        Trade::DataFlag::Global, IndicesWireframe, Trade::MeshIndexData{IndicesWireframe},
        Trade::DataFlag::Global, VerticesWireframe, Trade::meshAttributeDataNonOwningArray(AttributesWireframe)};
}

}}
