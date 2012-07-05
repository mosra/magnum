/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Tipsify.h"

#include <stack>

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Magnum { namespace MeshTools { namespace Implementation {

void Tipsify::operator()(size_t cacheSize) {
    /* Neighboring triangles for each vertex, per-vertex live triangle count */
    std::vector<unsigned int> liveTriangleCount, neighborPosition, neighbors;
    buildAdjacency(liveTriangleCount, neighborPosition, neighbors);

    /* Global time, per-vertex caching timestamps, per-triangle emmited flag */
    unsigned int time = cacheSize+1;
    std::vector<unsigned int> timestamp(vertexCount);
    std::vector<bool> emitted(indices.size()/3);

    /* Dead-end vertex stack */
    std::stack<unsigned int> deadEndStack;

    /* Output index buffer */
    std::vector<unsigned int> outputIndices;
    outputIndices.reserve(indices.size());

    /* Starting vertex for fanning, cursor */
    unsigned int fanningVertex = 0;
    unsigned int i = 0;
    while(fanningVertex != 0xFFFFFFFFu) {
        /* Array with candidates for next fanning vertex (in 1-ring around
           fanning vertex) */
        std::vector<unsigned int> candidates;

        /* For all neighbors of fanning vertex */
        for(unsigned int ti = neighborPosition[fanningVertex], t = neighbors[ti]; ti != neighborPosition[fanningVertex+1]; t = neighbors[++ti]) {
            /* Continue if already emitted */
            if(emitted[t]) continue;
            emitted[t] = true;

            /* Write all vertices of the triangle to output buffer */
            for(unsigned int vi = 0, v = indices[t*3]; vi != 3; v = indices[++vi+t*3]) {
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
        int candidatePriority = -1;
        for(auto it = candidates.begin(); it != candidates.end(); ++it) {
            unsigned int v = *it;

            /* Skip if it doesn't have any live triangles */
            if(!liveTriangleCount[v]) continue;

            /* Get most fresh candidate which will still be in cache even
               after fanning. Every fanned triangle will generate at most
               two cache misses, thus 2*liveTriangleCount */
            int priority = 0;
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
    std::swap(indices, outputIndices);
}

void Tipsify::buildAdjacency(std::vector<unsigned int>& liveTriangleCount, std::vector<unsigned int>& neighborOffset, std::vector<unsigned int>& neighbors) const {
    /* How many times is each vertex referenced == count of neighboring
       triangles for each vertex */
    liveTriangleCount.clear();
    liveTriangleCount.resize(vertexCount);
    for(unsigned int i = 0; i != indices.size(); ++i)
        ++liveTriangleCount[indices[i]];

    /* Building offset array from counts. Neighbors for i-th vertex will at
       the end be in interval neighbors[neighborOffset[i]] ;
       neighbors[neighborOffset[i+1]]. Currently the values are shifted to
       right, because the next loop will shift them back left. */
    neighborOffset.clear();
    neighborOffset.reserve(vertexCount+1);
    neighborOffset.push_back(0);
    unsigned int sum = 0;
    for(unsigned int i = 0; i != vertexCount; ++i) {
        neighborOffset.push_back(sum);
        sum += liveTriangleCount[i];
    }

    /* Array of neighbors, using (and changing) neighborOffset array for
       positioning */
    neighbors.clear();
    neighbors.resize(sum);
    for(unsigned int i = 0; i != indices.size(); ++i)
        neighbors[neighborOffset[indices[i]+1]++] = i/3;
}

}}}
#endif
