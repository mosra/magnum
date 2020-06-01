#ifndef Magnum_MeshTools_Implementation_Tipsify_h
#define Magnum_MeshTools_Implementation_Tipsify_h
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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Magnum.h"

namespace Magnum { namespace MeshTools { namespace Implementation { namespace {

/* Vertex-triangle adjacency. Computes count and indices of adjacent triangles
   for each vertex (used internally by tipsify()) */
template<class T> void buildAdjacency(const Containers::StridedArrayView1D<const T>& indices, const UnsignedInt vertexCount, Containers::Array<UnsignedInt>& liveTriangleCount, Containers::Array<UnsignedInt>& neighborOffset, Containers::Array<UnsignedInt>& neighbors) {
    /* How many times is each vertex referenced == count of neighboring
       triangles for each vertex */
    liveTriangleCount = Containers::Array<UnsignedInt>{vertexCount};
    for(std::size_t i = 0; i != indices.size(); ++i)
        ++liveTriangleCount[indices[i]];

    /* Building offset array from counts. Neighbors for i-th vertex will at
       the end be in interval neighbors[neighborOffset[i]] ;
       neighbors[neighborOffset[i+1]]. Currently the values are shifted to
       right, because the next loop will shift them back left. */
    neighborOffset = Containers::Array<UnsignedInt>{Containers::NoInit, vertexCount + 1};
    neighborOffset[0] = 0;
    UnsignedInt sum = 0;
    for(std::size_t i = 0; i != vertexCount; ++i) {
        neighborOffset[i + 1] = sum;
        sum += liveTriangleCount[i];
    }

    /* Array of neighbors, using (and changing) neighborOffset array for
       positioning */
    neighbors = Containers::Array<UnsignedInt>{Containers::NoInit, sum};
    for(std::size_t i = 0; i != indices.size(); ++i)
        neighbors[neighborOffset[indices[i]+1]++] = i/3;
}

}}}}

#endif
