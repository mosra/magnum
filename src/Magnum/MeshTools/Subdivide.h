#ifndef Magnum_MeshTools_Subdivide_h
#define Magnum_MeshTools_Subdivide_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

/** @file
 * @brief Function @ref Magnum::MeshTools::subdivide()
 */

#include <vector>
#include <Corrade/Utility/Assert.h>

namespace Magnum { namespace MeshTools {

/**
@brief Subdivide the mesh
@tparam Vertex          Vertex data type
@tparam Interpolator    See `interpolator` function parameter
@param[in,out] indices  Index array to operate on
@param[in,out] vertices Vertex array to operate on
@param interpolator     Functor or function pointer which interpolates
    two adjacent vertices: `Vertex interpolator(Vertex a, Vertex b)`

Goes through all triangle faces and subdivides them into four new. Removing
duplicate vertices in the mesh is up to user.
*/
template<class Vertex, class Interpolator> void subdivide(std::vector<UnsignedInt>& indices, std::vector<Vertex>& vertices, Interpolator interpolator) {
    CORRADE_ASSERT(!(indices.size()%3), "MeshTools::subdivide(): index count is not divisible by 3!", );

    /* For each triangle it'll generate three new vertices and three more
       triangles */
    indices.reserve(indices.size()*4);
    vertices.reserve(vertices.size() + indices.size());

    /* Subdivide each face to four new */
    std::size_t indexCount = indices.size();
    for(std::size_t i = 0; i != indexCount; i += 3) {
        /* Interpolate each side */
        UnsignedInt newVertices[3];
        for(int j = 0; j != 3; ++j) {
            newVertices[j] = vertices.size(); vertices.push_back(interpolator(vertices[indices[i+j]], vertices[indices[i+(j+1)%3]]));
        }

        /*
            Add three new faces (0, 1, 3) and update original (2)

                          orig 0
                          /   \
                         /  0  \
                        /       \
                    new 0 ----- new 2
                    /   \       /  \
                   /  1  \  2  / 3  \
                  /       \   /      \
             orig 1 ----- new 1 ---- orig 2
        */
        indices.insert(indices.end(), {
            indices[i], newVertices[0], newVertices[2],
            newVertices[0], indices[i+1], newVertices[1],
            newVertices[2], newVertices[1], indices[i+2]
        });
        for(std::size_t j = 0; j != 3; ++j)
            indices[i+j] = newVertices[j];
    }
}

}}

#endif
