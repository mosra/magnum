#ifndef Magnum_MeshTools_Subdivide_h
#define Magnum_MeshTools_Subdivide_h
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
 * @brief Function Magnum::MeshTools::subdivide()
 */

#include <vector>
#include "Utility/Debug.h"

namespace Magnum { namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

template<class Vertex, class Interpolator> class Subdivide {
    public:
        inline Subdivide(std::vector<unsigned int>& indices, std::vector<Vertex>& vertices): indices(indices), vertices(vertices) {}

        void operator()(Interpolator interpolator) {
            if(indices.size()%3 != 0) {
                Corrade::Utility::Error() << "MeshTools::subdivide(): index count is not divisible by 3!";
                return;
            }

            size_t indexCount = indices.size();
            indices.reserve(indices.size()*4);

            /* Subdivide each face to four new */
            for(size_t i = 0; i != indexCount; i += 3) {
                /* Interpolate each side */
                unsigned int newVertices[3];
                for(int j = 0; j != 3; ++j)
                    newVertices[j] = addVertex(interpolator(vertices[indices[i+j]], vertices[indices[i+(j+1)%3]]));

                /*
                 * Add three new faces (0, 1, 3) and update original (2)
                 *
                 *                orig 0
                 *                /   \
                 *               /  0  \
                 *              /       \
                 *          new 0 ----- new 2
                 *          /   \       /  \
                 *         /  1  \  2  / 3  \
                 *        /       \   /      \
                 *   orig 1 ----- new 1 ---- orig 2
                 */
                addFace(indices[i], newVertices[0], newVertices[2]);
                addFace(newVertices[0], indices[i+1], newVertices[1]);
                addFace(newVertices[2], newVertices[1], indices[i+2]);
                for(size_t j = 0; j != 3; ++j)
                    indices[i+j] = newVertices[j];
            }
        }

    private:
        std::vector<unsigned int>& indices;
        std::vector<Vertex>& vertices;

        unsigned int addVertex(const Vertex& v) {
            vertices.push_back(v);
            return vertices.size()-1;
        }

        void addFace(unsigned int first, unsigned int second, unsigned int third) {
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(third);
        }
};

}
#endif

/**
@brief %Subdivide the mesh
@tparam Vertex          Vertex data type
@tparam Interpolator    See `interpolator` function parameter
@param indices          Index array to operate on
@param vertices         Vertex array to operate on
@param interpolator     Functor or function pointer which interpolates
    two adjacent vertices: `Vertex interpolator(Vertex a, Vertex b)`

Goes through all triangle faces and subdivides them into four new. Cleaning
duplicate vertices in the mesh is up to user.
*/
template<class Vertex, class Interpolator> inline void subdivide(std::vector<unsigned int>& indices, std::vector<Vertex>& vertices, Interpolator interpolator) {
    Implementation::Subdivide<Vertex, Interpolator>(indices, vertices)(interpolator);
}

}}

#endif
