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
 * @brief Class Magnum::MeshTools::Tipsify
 */

#include "AbstractTool.h"

namespace Magnum { namespace MeshTools {

/**
@brief %Mesh tipsifier implementation

See tipsify() for full documentation.
*/
class Tipsify: public AbstractIndexTool {
    public:
        /** @copydoc AbstractIndexTool::AbstractIndexTool(MeshBuilder<Vertex>&) */
        template<class Vertex> inline Tipsify(MeshBuilder<Vertex>& builder): AbstractIndexTool(builder) {}

        /** @copydoc AbstractIndexTool::AbstractIndexTool(std::vector<unsigned int>&, unsigned int) */
        inline Tipsify(std::vector<unsigned int>& indices, unsigned int vertexCount): AbstractIndexTool(indices, vertexCount) {}

        /**
         * @brief Functor
         *
         * See tipsify() for full documentation.
         */
        void run(size_t cacheSize);

        /**
         * @brief Build vertex-triangle adjacency
         *
         * Computes count and indices of adjacent triangles for each vertex
         * (used internally).
         */
        void buildAdjacency(std::vector<unsigned int>& liveTriangleCount, std::vector<unsigned int>& neighborOffset, std::vector<unsigned int>& neighbors) const;
};

/**
@brief %Tipsify the mesh
@tparam Vertex      Vertex data type (the same as in MeshBuilder)
@param builder      %Mesh builder to operate on
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
MeshBuilder<T> builder;
MeshTools::Tipsify(builder).run(cacheSize);
@endcode
you can just write
@code
MeshTools::tipsify(builder, cacheSize);
@endcode
*/
template<class Vertex> inline void tipsify(MeshBuilder<Vertex>& builder, size_t cacheSize) {
    Tipsify(builder).run(cacheSize);
}

/**
@brief %Tipsify the mesh
@param indices      Indices array to operate on
@param vertexCount  Vertex count
@param cacheSize    Post-transform vertex cache size

See tipsify(MeshBuilder<Vertex>&, size_t) for more information.
*/
inline void tipsify(std::vector<unsigned int>& indices, unsigned int vertexCount, size_t cacheSize) {
    Tipsify(indices, vertexCount).run(cacheSize);
}

}}

#endif
