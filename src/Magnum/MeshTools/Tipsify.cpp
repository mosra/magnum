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

#include "Tipsify.h"

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/MeshTools/Implementation/Tipsify.h"

namespace Magnum { namespace MeshTools {

namespace {

template<class T> void tipsifyInPlaceImplementation(const Containers::StridedArrayView1D<T>& indices, const UnsignedInt vertexCount, const std::size_t cacheSize) {
    /* Neighboring triangles for each vertex, per-vertex live triangle count */
    Containers::Array<UnsignedInt> liveTriangleCount, neighborOffset, neighbors;
    Implementation::buildAdjacency<T>(indices, vertexCount, liveTriangleCount, neighborOffset, neighbors);

    /* Global time, per-vertex caching timestamps, per-triangle emmited flag */
    UnsignedInt time = cacheSize+1;
    Containers::Array<UnsignedInt> timestamp{vertexCount};
    /** @todo Have some bitset/staticbitset class for this */
    Containers::Array<bool> emitted{indices.size()/3};

    /* Dead-end vertex stack */
    Containers::Array<UnsignedInt> deadEndStack;

    /* Output index buffer */
    Containers::Array<T> outputIndices{Containers::NoInit, indices.size()};
    std::size_t outputIndex = 0;

    /* Array with candidates for next fanning vertex (in 1-ring around
       fanning vertex) */
    Containers::Array<UnsignedInt> candidates;

    /* Starting vertex for fanning, cursor */
    UnsignedInt fanningVertex = 0;
    UnsignedInt i = 0;
    while(fanningVertex != 0xFFFFFFFFu) {
        /* Reset the candidates for this vertex */
        arrayResize(candidates, 0);

        /* For all neighbors of fanning vertex */
        for(UnsignedInt ti = neighborOffset[fanningVertex]; ti != neighborOffset[fanningVertex+1]; ++ti) {
            const UnsignedInt t = neighbors[ti];

            /* Continue if already emitted */
            if(emitted[t]) continue;
            emitted[t] = true;

            /* Write all vertices of the triangle to output buffer */
            for(UnsignedInt vi = 0; vi != 3; ++vi) {
                const UnsignedInt v = indices[vi + t*3];

                outputIndices[outputIndex++] = v;

                /* Add to dead end stack and candidates array */
                /** @todo Limit size of dead end stack to cache size */
                arrayAppend(deadEndStack, v);
                arrayAppend(candidates, v);

                /* Decrease live triangle count */
                --liveTriangleCount[v];

                /* If not in cache, set timestamp */
                if(time-timestamp[v] > cacheSize)
                    timestamp[v] = time++;
            }
        }

        /* Get next fanning vertex */
        fanningVertex = 0xFFFFFFFFu;

        /* Go through candidates in 1-ring around fanning vertex */
        Int candidatePriority = -1;
        for(UnsignedInt v: candidates) {
            /* Skip if it doesn't have any live triangles */
            if(!liveTriangleCount[v]) continue;

            /* Get most fresh candidate which will still be in cache even
               after fanning. Every fanned triangle will generate at most
               two cache misses, thus 2*liveTriangleCount */
            Int priority = 0;
            if(time-timestamp[v]+2*liveTriangleCount[v] <= cacheSize)
                priority = time-timestamp[v];
            if(priority > candidatePriority) {
                fanningVertex = v;
                candidatePriority = priority;
            }
        }

        /* On dead-end */
        if(fanningVertex == 0xFFFFFFFFu) {
            /* Find vertex with live triangles in dead-end stack */
            while(!deadEndStack.empty()) {
                UnsignedInt d = deadEndStack.back();
                arrayRemoveSuffix(deadEndStack);

                if(!liveTriangleCount[d]) continue;
                fanningVertex = d;
                break;
            }

            /* If not found, find next artbitrary vertex with live
               triangles */
            while(++i < vertexCount) {
                if(!liveTriangleCount[i]) continue;

                fanningVertex = i;
                break;
            }
        }
    }

    /* Swap original index buffer with optimized */
    Utility::copy(outputIndices, indices);
}

}

void tipsifyInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const UnsignedInt vertexCount, const std::size_t cacheSize) {
    tipsifyInPlaceImplementation(indices, vertexCount, cacheSize);
}

void tipsifyInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const UnsignedInt vertexCount, const std::size_t cacheSize) {
    tipsifyInPlaceImplementation(indices, vertexCount, cacheSize);
}

void tipsifyInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const UnsignedInt vertexCount, const std::size_t cacheSize) {
    tipsifyInPlaceImplementation(indices, vertexCount, cacheSize);
}

}}
