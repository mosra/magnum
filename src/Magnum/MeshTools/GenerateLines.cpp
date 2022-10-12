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

#include "GenerateLines.h"

#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace MeshTools {

namespace {

// TODO take line segment count? easier to document, harder to use with a strip/loop
template<class T> inline void generateLineIndicesIntoImplementation(const std::size_t lineStripPointCount, const Containers::StridedArrayView1D<T>& indices) {
    // TODO assert lineStripPointCount > 2 and indices.size() == (lineStripPointCount - 1)*6

    for(std::size_t i = 0, max = lineStripPointCount/2; i != max; ++i) {
        /* For consistency same as the Text library does
           0---2 0---2 5
           |   | |  / /|
           |   | | / / |
           |   | |/ /  |
           1---3 1 3---4 */
        indices[i*6 + 0] = i*4 + 0;
        indices[i*6 + 1] = i*4 + 1;
        indices[i*6 + 2] = i*4 + 2;
        indices[i*6 + 3] = i*4 + 1;
        indices[i*6 + 4] = i*4 + 3;
        indices[i*6 + 5] = i*4 + 2;
    }
}

}

void generateLineIndicesInto(const std::size_t lineStripPointCount, const Containers::StridedArrayView1D<UnsignedInt>& indices) {
    return generateLineIndicesIntoImplementation(lineStripPointCount, indices);
}

void generateLineIndicesInto(const std::size_t lineStripPointCount, const Containers::StridedArrayView1D<UnsignedShort>& indices) {
    return generateLineIndicesIntoImplementation(lineStripPointCount, indices);
}

/* TODO useful for anything?? need to cap like SVG

  0-------+-----+-+
  |      /      |/
  A---------B   /
  |    /   /   /|
  2---/---/-----+
     /   /   /
    /   /   /
   +---C---+ */

namespace {

// TODO take indices?
template<UnsignedInt dimensions> void generateLinesIntoImplementation(const Containers::StridedArrayView1D<const VectorTypeFor<dimensions, Float>>& lineStripPositions, const Containers::StridedArrayView1D<VectorTypeFor<dimensions, Float>>& positions, const Containers::StridedArrayView1D<VectorTypeFor<dimensions, Float>>& directions, const Containers::StridedArrayView1D<VectorTypeFor<dimensions, Float>>& neighborDirections, const bool loop) {
    // TODO assert lineStripPositions.size() > 2 and positions,tangents,normals.size() == (lineStripPositions.size() - 1)*4

    for(std::size_t i = 0, max = lineStripPositions.size() - 1; i != max; ++i) {
        /*
            Illustrative example of a line segment from A to B, with quad
            corners 0, 1, 2 and 3. Each gets either a `+d` or `-d` for the
            segment direction, and `pd` or `nd` for prev/next directions. The
            next segment is from B to C, forming the `nd`, and A is the first
            point of a line strip, so `pd` will be a zero vector.

            TODO this is outdated, all directions should be the same
             <-d-0-----------2-+d->          direction: +---->
                 |          / \
                 A---------B   nd            prevDirection: +
                 |        / \   v
             <-d-1-------3-+d->  \           nextDirection: +
                          \   \    .                         \
                           nd   .   .                         \
                            v   C    .                         v
        */
        const VectorTypeFor<dimensions, Float> a = lineStripPositions[i + 0];
        const VectorTypeFor<dimensions, Float> b = lineStripPositions[i + 1];
        const VectorTypeFor<dimensions, Float> direction = b - a;

        /* Direction to the previous point. If we're at the line start, it's
           a zero vector to indicate that a line cap should be drawn, otherwise
           it's a normalized vector. */
        // TODO normalization clashes with zero-length line segments, thse
        //  would be zero as well, causing caps to appear
        VectorTypeFor<dimensions, Float> prevDirection{NoInit};
        if(i == 0) {
            if(!loop) prevDirection = {};
            else prevDirection = (lineStripPositions[max] - a).normalized();
        } else prevDirection = (lineStripPositions[i - 1] - a).normalized();

        /* Direction to the next point. If we're at the line end, it's a zero
           vector to indicate that a line cap should be drawn, otherwise it's a
           normalized vector. */
        // TODO normalization clashes with zero-length line segments, thse
        //  would be zero as well, causing caps to appear
        VectorTypeFor<dimensions, Float> nextDirection{NoInit};
        if(i + 1 == max) {
            if(!loop) nextDirection = {};
            else nextDirection = (lineStripPositions[0] - a).normalized();
        } else nextDirection = (lineStripPositions[i + 2] - b).normalized();

        /* Save all output for this segment. Since the line width is a shader
           input, the quad actually has no width at all, it's expanded later
           with the help of normal and tangent properties */
        positions[i*4 + 0] = positions[i*4 + 2] = a;
        positions[i*4 + 1] = positions[i*4 + 3] = a;
        neighborDirections[i*4 + 0] = neighborDirections[i*4 + 2] =  prevDirection;
        neighborDirections[i*4 + 1] = neighborDirections[i*4 + 3] =  nextDirection;
        directions[i*4 + 0] = direction;
        directions[i*4 + 1] = direction;
        directions[i*4 + 2] = direction;
        directions[i*4 + 3] = direction;
    }
}

}

void generateLineStripInto(const Containers::StridedArrayView1D<const Vector2>& lineStripPositions, const Containers::StridedArrayView1D<Vector2>& positions, const Containers::StridedArrayView1D<Vector2>& directions, const Containers::StridedArrayView1D<Vector2>& neighborDirections) {
    return generateLinesIntoImplementation<2>(lineStripPositions, positions, directions, neighborDirections, false);
}

void generateLineStripInto(const Containers::StridedArrayView1D<const Vector3>& lineStripPositions, const Containers::StridedArrayView1D<Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& directions, const Containers::StridedArrayView1D<Vector3>& neighborDirections) {
    return generateLinesIntoImplementation<3>(lineStripPositions, positions, directions, neighborDirections, false);
}

void generateLineLoopInto(const Containers::StridedArrayView1D<const Vector2>& lineLoopPositions, const Containers::StridedArrayView1D<Vector2>& positions, const Containers::StridedArrayView1D<Vector2>& directions, const Containers::StridedArrayView1D<Vector2>& neighborDirections) {
    return generateLinesIntoImplementation<2>(lineLoopPositions, positions, directions, neighborDirections, true);
}

void generateLineLoopInto(const Containers::StridedArrayView1D<const Vector3>& lineLoopPositions, const Containers::StridedArrayView1D<Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& directions, const Containers::StridedArrayView1D<Vector3>& neighborDirections) {
    return generateLinesIntoImplementation<3>(lineLoopPositions, positions, directions, neighborDirections, true);
}

// Trade::MeshData generateLineStrip(

}}
