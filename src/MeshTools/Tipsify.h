#ifndef Magnum_MeshTools_Tipsify_h
#define Magnum_MeshTools_Tipsify_h
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

/** @file
 * @brief Function Magnum::MeshTools::tipsify()
 */

#include <cstddef>
#include <vector>

#include "magnumMeshToolsVisibility.h"

namespace Magnum { namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

class MESHTOOLS_EXPORT Tipsify {
    public:
        inline Tipsify(std::vector<unsigned int>& indices, unsigned int vertexCount): indices(indices), vertexCount(vertexCount) {}

        void operator()(size_t cacheSize);

        /**
         * @brief Build vertex-triangle adjacency
         *
         * Computes count and indices of adjacent triangles for each vertex
         * (used internally).
         */
        void buildAdjacency(std::vector<unsigned int>& liveTriangleCount, std::vector<unsigned int>& neighborOffset, std::vector<unsigned int>& neighbors) const;

    private:
        std::vector<unsigned int>& indices;
        const unsigned int vertexCount;
};

}
#endif

/** @ingroup mesh
@brief %Tipsify the mesh
@param[in,out] indices  Indices array to operate on
@param[in] vertexCount  Vertex count
@param[in] cacheSize    Post-transform vertex cache size

Optimizes the mesh for vertex-bound applications by rearranging its index
array for beter usage of post-transform vertex cache. Algorithm used:
*Pedro V. Sander, Diego Nehab, and Joshua Barczak - Fast Triangle Reordering
for Vertex Locality and Reduced Overdraw, SIGGRAPH 2007,
http://gfx.cs.princeton.edu/pubs/Sander_2007_%3ETR/index.php*.
*/
inline void tipsify(std::vector<unsigned int>& indices, unsigned int vertexCount, size_t cacheSize) {
    Implementation::Tipsify(indices, vertexCount)(cacheSize);
}

}}

#endif
