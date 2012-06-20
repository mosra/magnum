#ifndef Magnum_MeshTools_Clean_h
#define Magnum_MeshTools_Clean_h
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
 * @brief Function Magnum::MeshTools::clean()
 */

#include <unordered_map>
#include <limits>

#include "Utility/MurmurHash2.h"
#include "TypeTraits.h"

namespace Magnum { namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

template<class Vertex, size_t vertexSize = Vertex::Size> class Clean {
    public:
        inline Clean(std::vector<unsigned int>& indices, std::vector<Vertex>& vertices): indices(indices), vertices(vertices) {}

        void operator()(typename Vertex::Type epsilon = TypeTraits<typename Vertex::Type>::epsilon()) {
            if(indices.empty()) return;

            /* Get mesh bounds */
            Vertex min, max;
            for(size_t i = 0; i != Vertex::Size; ++i) {
                min[i] = std::numeric_limits<typename Vertex::Type>::max();
                max[i] = std::numeric_limits<typename Vertex::Type>::min();
            }
            for(auto it = vertices.cbegin(); it != vertices.cend(); ++it)
                for(size_t i = 0; i != vertexSize; ++i)
                    if((*it)[i] < min[i])
                        min[i] = (*it)[i];
                    else if((*it)[i] > max[i])
                        max[i] = (*it)[i];

            /* Make epsilon so large that size_t can index all vertices inside
               mesh bounds. */
            Vertex size = max-min;
            for(size_t i = 0; i != Vertex::Size; ++i)
                if(static_cast<typename Vertex::Type>(size[i]/std::numeric_limits<size_t>::max()) > epsilon)
                    epsilon = static_cast<typename Vertex::Type>(size[i]/std::numeric_limits<size_t>::max());

            /* First go with original vertex coordinates, then move them by
               epsilon/2 in each direction. */
            Vertex moved;
            for(size_t moving = 0; moving <= vertexSize; ++moving) {

                /* Under each index is pointer to face which contains given vertex
                   and index of vertex in the face. */
                std::unordered_map<Math::Vector<vertexSize, size_t>, HashedVertex, IndexHash> table;

                /* Reserve space for all vertices */
                table.reserve(vertices.size());

                /* Go through all faces' vertices */
                for(auto it = indices.begin(); it != indices.end(); ++it) {
                    /* Index of a vertex in vertexSize-dimensional table */
                    size_t index[vertexSize];
                    for(size_t ii = 0; ii != vertexSize; ++ii)
                        index[ii] = (vertices[*it][ii]+moved[ii]-min[ii])/epsilon;

                    /* Try inserting the vertex into table, if it already
                       exists, change vertex pointer of the face to already
                       existing vertex */
                    HashedVertex v(*it, table.size());
                    auto result = table.insert(std::pair<Math::Vector<vertexSize, size_t>, HashedVertex>(Math::Vector<vertexSize, size_t>::from(index), v));
                    *it = result.first->second.newIndex;
                }

                /* Shrink vertices array */
                std::vector<Vertex> newVertices(table.size());
                for(auto it = table.cbegin(); it != table.cend(); ++it)
                    newVertices[it->second.newIndex] = vertices[it->second.oldIndex];
                std::swap(newVertices, vertices);

                /* Move vertex coordinates by epsilon/2 in next direction */
                if(moving != Vertex::Size) {
                    moved = Vertex();
                    moved[moving] = epsilon/2;
                }
            }
        }

    private:
        class IndexHash {
            public:
                inline size_t operator()(const Math::Vector<vertexSize, size_t>& data) const {
                    return *reinterpret_cast<const size_t*>(Corrade::Utility::MurmurHash2()(reinterpret_cast<const char*>(&data), sizeof(data)).byteArray());
                }
        };

        struct HashedVertex {
            unsigned int oldIndex, newIndex;

            HashedVertex(unsigned int oldIndex, unsigned int newIndex): oldIndex(oldIndex), newIndex(newIndex) {}
        };

        std::vector<unsigned int>& indices;
        std::vector<Vertex>& vertices;
};

}
#endif

/** @ingroup mesh
@brief %Clean the mesh
@tparam Vertex          Vertex data type
@tparam vertexSize      How many initial vertex fields are important (for
    example, when dealing with perspective in 3D space, only first three
    fields of otherwise 4D vertex are important)
@param[in,out] indices  Index array to operate on
@param[in,out] vertices Vertex array to operate on
@param[in] epsilon      Epsilon value, vertices nearer than this distance will
    be melt together.

Removes duplicate vertices from the mesh.

@todo Different (no cycle) implementation for integral vertices
@todo Interpolate vertices, not collapse them to first in the cell
@todo Ability to specify other attributes for interpolation
*/
template<class Vertex, size_t vertexSize = Vertex::Size> inline void clean(std::vector<unsigned int>& indices, std::vector<Vertex>& vertices, typename Vertex::Type epsilon = TypeTraits<typename Vertex::Type>::epsilon()) {
    Implementation::Clean<Vertex, vertexSize>(indices, vertices)(epsilon);
}

}}

#endif
