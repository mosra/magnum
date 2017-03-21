#ifndef Magnum_MeshTools_Tipsify_h
#define Magnum_MeshTools_Tipsify_h
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

/** @file
 * @brief Function @ref Magnum::MeshTools::tipsify()
 */

#include <vector>

#include "Magnum/Types.h"
#include "Magnum/MeshTools/visibility.h"

namespace Magnum { namespace MeshTools {

namespace Implementation {

class MAGNUM_MESHTOOLS_EXPORT Tipsify {
    public:
        Tipsify(std::vector<UnsignedInt>& indices, UnsignedInt vertexCount): indices(indices), vertexCount(vertexCount) {}

        void operator()(std::size_t cacheSize);

        /**
         * @brief Build vertex-triangle adjacency
         *
         * Computes count and indices of adjacent triangles for each vertex
         * (used internally).
         * @todo Export only for unit test, hide otherwise
         */
        void buildAdjacency(std::vector<UnsignedInt>& liveTriangleCount, std::vector<UnsignedInt>& neighborOffset, std::vector<UnsignedInt>& neighbors) const;

    private:
        std::vector<UnsignedInt>& indices;
        const UnsignedInt vertexCount;
};

}

/**
@brief Tipsify the mesh
@param[in,out] indices  Indices array to operate on
@param[in] vertexCount  Vertex count
@param[in] cacheSize    Post-transform vertex cache size

Optimizes the mesh for vertex-bound applications by rearranging its index
array for beter usage of post-transform vertex cache. Algorithm used:
*Pedro V. Sander, Diego Nehab, and Joshua Barczak - Fast Triangle Reordering
for Vertex Locality and Reduced Overdraw, SIGGRAPH 2007,
http://gfx.cs.princeton.edu/pubs/Sander_2007_%3ETR/index.php*.
@todo Ability to compute vertex count automatically
*/
inline void tipsify(std::vector<UnsignedInt>& indices, UnsignedInt vertexCount, std::size_t cacheSize) {
    Implementation::Tipsify(indices, vertexCount)(cacheSize);
}

}}

#endif
