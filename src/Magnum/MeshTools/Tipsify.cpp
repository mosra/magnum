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

#include "Tipsify.h"

#include <stack>

namespace Magnum { namespace MeshTools { namespace Implementation {

void Tipsify::operator()(std::size_t cacheSize) {
    /* Neighboring triangles for each vertex, per-vertex live triangle count */
    std::vector<UnsignedInt> liveTriangleCount, neighborPosition, neighbors;
    buildAdjacency(liveTriangleCount, neighborPosition, neighbors);

    /* Global time, per-vertex caching timestamps, per-triangle emmited flag */
    UnsignedInt time = cacheSize+1;
    std::vector<UnsignedInt> timestamp(vertexCount);
    std::vector<bool> emitted(indices.size()/3);

    /* Dead-end vertex stack */
    std::stack<UnsignedInt> deadEndStack;

    /* Output index buffer */
    std::vector<UnsignedInt> outputIndices;
    outputIndices.reserve(indices.size());

    /* Starting vertex for fanning, cursor */
    UnsignedInt fanningVertex = 0;
    UnsignedInt i = 0;
    while(fanningVertex != 0xFFFFFFFFu) {
        /* Array with candidates for next fanning vertex (in 1-ring around
           fanning vertex) */
        std::vector<UnsignedInt> candidates;

        /* For all neighbors of fanning vertex */
        for(UnsignedInt ti = neighborPosition[fanningVertex], t = neighbors[ti]; ti != neighborPosition[fanningVertex+1]; t = neighbors[++ti]) {
            /* Continue if already emitted */
            if(emitted[t]) continue;
            emitted[t] = true;

            /* Write all vertices of the triangle to output buffer */
            for(UnsignedInt vi = 0; vi != 3; ++vi) {
                const UnsignedInt v = indices[vi + t*3];

                outputIndices.push_back(v);

                /* Add to dead end stack and candidates array */
                /** @todo Limit size of dead end stack to cache size */
                deadEndStack.push(v);
                candidates.push_back(v);

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
                unsigned int d = deadEndStack.top();
                deadEndStack.pop();

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
    using std::swap;
    swap(indices, outputIndices);
}

void Tipsify::buildAdjacency(std::vector<UnsignedInt>& liveTriangleCount, std::vector<UnsignedInt>& neighborOffset, std::vector<UnsignedInt>& neighbors) const {
    /* How many times is each vertex referenced == count of neighboring
       triangles for each vertex */
    liveTriangleCount.clear();
    liveTriangleCount.resize(vertexCount);
    for(std::size_t i = 0; i != indices.size(); ++i)
        ++liveTriangleCount[indices[i]];

    /* Building offset array from counts. Neighbors for i-th vertex will at
       the end be in interval neighbors[neighborOffset[i]] ;
       neighbors[neighborOffset[i+1]]. Currently the values are shifted to
       right, because the next loop will shift them back left. */
    neighborOffset.clear();
    neighborOffset.reserve(vertexCount+1);
    neighborOffset.push_back(0);
    UnsignedInt sum = 0;
    for(std::size_t i = 0; i != vertexCount; ++i) {
        neighborOffset.push_back(sum);
        sum += liveTriangleCount[i];
    }

    /* Array of neighbors, using (and changing) neighborOffset array for
       positioning */
    neighbors.clear();
    neighbors.resize(sum);
    for(std::size_t i = 0; i != indices.size(); ++i)
        neighbors[neighborOffset[indices[i]+1]++] = i/3;
}

}}}
