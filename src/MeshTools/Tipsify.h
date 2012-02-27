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
 * @brief Class Magnum::MeshTools::Tipsify, function Magnum::MeshTools::tipsify()
 */

#include <cstddef>
#include <vector>

#include "utilities.h"

namespace Magnum { namespace MeshTools {

/**
@brief %Mesh tipsifier implementation

See tipsify() for full documentation.
*/
class MESHTOOLS_EXPORT Tipsify {
    public:
        /**
         * @brief Constructor
         *
         * See tipsify() for full documentation.
         */
        inline Tipsify(std::vector<unsigned int>& indices, unsigned int vertexCount): indices(indices), vertexCount(vertexCount) {}

        /**
         * @brief Functor
         *
         * See tipsify() for full documentation.
         */
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

/**
@brief %Tipsify the mesh
@param indices      Indices array to operate on
@param vertexCount  Vertex count
@param cacheSize    Post-transform vertex cache size

Optimizes the mesh for vertex-bound applications by rearranging its index
array for beter usage of post-transform vertex cache. Algorithm used:
<em>Pedro V. Sander, Diego Nehab, and Joshua Barczak,
<a href="http://gfx.cs.princeton.edu/pubs/Sander_2007_%3ETR/index.php">Fast
Triangle Reordering for Vertex Locality and Reduced Overdraw</a>, SIGGRAPH
2007</em>.

This is convenience function supplementing direct usage of Tipsify class,
instead of
@code
MeshTools::Tipsify(indices, vertexCount)(cacheSize);
@endcode
you can just write
@code
MeshTools::tipsify(indices, vertexCount, cacheSize);
@endcode
*/
inline void tipsify(std::vector<unsigned int>& indices, unsigned int vertexCount, size_t cacheSize) {
    Tipsify(indices, vertexCount)(cacheSize);
}

}}

#endif
