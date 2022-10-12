#ifndef Magnum_MeshTools_GenerateLines_h
#define Magnum_MeshTools_GenerateLines_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MeshTools {

void generateLineIndicesInto(std::size_t lineStripPointCount, const Containers::StridedArrayView1D<UnsignedInt>& indices);

void generateLineIndicesInto(std::size_t lineStripPointCount, const Containers::StridedArrayView1D<UnsignedShort>& indices);

void generateLineStripInto(const Containers::StridedArrayView1D<const Vector2>& lineStripPositions, const Containers::StridedArrayView1D<Vector2>& positions, const Containers::StridedArrayView1D<Vector2>& directions, const Containers::StridedArrayView1D<Vector2>& neighborDirections);

void generateLineStripInto(const Containers::StridedArrayView1D<const Vector3>& lineStripPositions, const Containers::StridedArrayView1D<Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& directions, const Containers::StridedArrayView1D<Vector3>& neighborDirections);

void generateLineLoopInto(const Containers::StridedArrayView1D<const Vector2>& lineStripPositions, const Containers::StridedArrayView1D<Vector2>& positions, const Containers::StridedArrayView1D<Vector2>& directions, const Containers::StridedArrayView1D<Vector2>& neighborDirections);

void generateLineLoopInto(const Containers::StridedArrayView1D<const Vector3>& lineStripPositions, const Containers::StridedArrayView1D<Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& directions, const Containers::StridedArrayView1D<Vector3>& neighborDirections);

// Trade::MeshData generateLineStrip(const Containers::StridedArrayView1D<const Vector2>& lineStripPositions);
//
// Trade::MeshData generateLineLoop(const Containers::StridedArrayView1D<const Vector2>& lineStripPositions);

// TODO take indexed line segments, probably? can't work with strips
// TODO *fuck* can't work with that either, need to generate triangles from scratch

}}

#endif
