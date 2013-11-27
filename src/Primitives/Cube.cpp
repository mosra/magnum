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

#include "Cube.h"

#include "Math/Vector3.h"
#include "Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

Trade::MeshData3D Cube::solid() {
    return Trade::MeshData3D(Mesh::Primitive::Triangles, {
         0,  1,  2,  0,  2,  3, /* +Z */
         4,  5,  6,  4,  6,  7, /* +X */
         8,  9, 10,  8, 10, 11, /* +Y */
        12, 13, 14, 12, 14, 15, /* -Z */
        16, 17, 18, 16, 18, 19, /* -Y */
        20, 21, 22, 20, 22, 23  /* -X */
    }, {{
        {-1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f}, /* +Z */
        {-1.0f,  1.0f,  1.0f},

        { 1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f}, /* +X */
        { 1.0f,  1.0f,  1.0f},

        {-1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f, -1.0f}, /* +Y */
        {-1.0f,  1.0f, -1.0f},

        { 1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f}, /* -Z */
        { 1.0f,  1.0f, -1.0f},

        {-1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f,  1.0f}, /* -Y */
        {-1.0f, -1.0f,  1.0f},

        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f}, /* -X */
        {-1.0f,  1.0f, -1.0f}
    }}, {{
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f,  1.0f}, /* +Z */
        { 0.0f,  0.0f,  1.0f},

        { 1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f}, /* +X */
        { 1.0f,  0.0f,  0.0f},

        { 0.0f,  1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f}, /* +Y */
        { 0.0f,  1.0f,  0.0f},

        { 0.0f,  0.0f, -1.0f},
        { 0.0f,  0.0f, -1.0f},
        { 0.0f,  0.0f, -1.0f}, /* -Z */
        { 0.0f,  0.0f, -1.0f},

        { 0.0f, -1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f}, /* -Y */
        { 0.0f, -1.0f,  0.0f},

        {-1.0f,  0.0f,  0.0f},
        {-1.0f,  0.0f,  0.0f},
        {-1.0f,  0.0f,  0.0f}, /* -X */
        {-1.0f,  0.0f,  0.0f}
    }}, std::vector<std::vector<Vector2>>{});
}

Trade::MeshData3D Cube::wireframe() {
    return Trade::MeshData3D(Mesh::Primitive::Lines, {
        0, 1, 1, 2, 2, 3, 3, 0, /* +Z */
        4, 5, 5, 6, 6, 7, 7, 4, /* -Z */
        1, 5, 2, 6,             /* +X */
        0, 4, 3, 7              /* -X */
    }, {{
        {-1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f},

        {-1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f}
    }}, std::vector<std::vector<Vector3>>{}, std::vector<std::vector<Vector2>>{});
}

}}
