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

#include "Axis.h"

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives {

using namespace Math::Literals;

Trade::MeshData2D axis2D() {
    return Trade::MeshData2D{MeshPrimitive::Lines,
        {0, 1,
         1, 2, /* X axis */
         1, 3,

         4, 5,
         5, 6, /* Y axis */
         5, 7},
        {{{ 0.0f,  0.0f},
          { 1.0f,  0.0f}, /* X axis */
          { 0.9f,  0.1f},
          { 0.9f, -0.1f},

          { 0.0f,  0.0f},
          { 0.0f,  1.0f}, /* Y axis */
          { 0.1f,  0.9f},
          {-0.1f,  0.9f}}}, {},
        {{0xff0000_rgbf,
          0xff0000_rgbf, /* X axis */
          0xff0000_rgbf,
          0xff0000_rgbf,

          0x00ff00_rgbf,
          0x00ff00_rgbf, /* Y axis */
          0x00ff00_rgbf,
          0x00ff00_rgbf}}};
}

Trade::MeshData3D axis3D() {
    return Trade::MeshData3D{MeshPrimitive::Lines,
        {0, 1,
         1, 2,  /* X axis */
         1, 3,

         4, 5,
         5, 6,  /* Y axis */
         5, 7,

         8, 9,
         9, 10, /* Z axis */
         9, 11},
        {{{ 0.0f,  0.0f,  0.0f},
          { 1.0f,  0.0f,  0.0f}, /* X axis */
          { 0.9f,  0.1f,  0.0f},
          { 0.9f, -0.1f,  0.0f},

          { 0.0f,  0.0f,  0.0f},
          { 0.0f,  1.0f,  0.0f}, /* Y axis */
          { 0.1f,  0.9f,  0.0f},
          {-0.1f,  0.9f,  0.0f},

          { 0.0f,  0.0f,  0.0f},
          { 0.0f,  0.0f,  1.0f}, /* Z axis */
          { 0.1f,  0.0f,  0.9f},
          {-0.1f,  0.0f,  0.9f}}}, {}, {},
        {{0xff0000_rgbf,
          0xff0000_rgbf, /* X axis */
          0xff0000_rgbf,
          0xff0000_rgbf,

          0x00ff00_rgbf,
          0x00ff00_rgbf, /* Y axis */
          0x00ff00_rgbf,
          0x00ff00_rgbf,

          0x0000ff_rgbf,
          0x0000ff_rgbf, /* Z axis */
          0x0000ff_rgbf,
          0x0000ff_rgbf}}};
}

}}
