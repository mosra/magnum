#ifndef Magnum_MeshTools_Subdivide_h
#define Magnum_MeshTools_Subdivide_h
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

/** @file
 * @brief Function @ref Magnum::MeshTools::subdivide(), @ref Magnum::MeshTools::subdivideInPlace()
 */

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Assert.h>

#include "Magnum/Magnum.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#endif

namespace Magnum { namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
template<class IndexType, class Vertex, class Interpolator> void subdivideInPlace(const Containers::StridedArrayView1D<IndexType>& indices, const Containers::StridedArrayView1D<Vertex>& vertices, Interpolator interpolator);
#endif

/**
@brief Subdivide a mesh
@tparam Vertex          Vertex data type
@tparam Interpolator    See the @p interpolator function parameter
@param[in,out] indices  Index array to operate on
@param[in,out] vertices Vertex array to operate on
@param interpolator     Functor or function pointer which interpolates
    two adjacent vertices: @cpp Vertex interpolator(Vertex a, Vertex b) @ce
@m_since{2020,06}

Goes through all triangle faces and subdivides them into four new, enlarging
the @p indices and @p vertices arrays as appropriate. Removing duplicate
vertices in the mesh is up to the user.
@see @ref subdivideInPlace(), @ref removeDuplicatesInPlace()
*/
template<class IndexType, class Vertex, class Interpolator> void subdivide(Containers::Array<IndexType>& indices, Containers::Array<Vertex>& vertices, Interpolator interpolator) {
    CORRADE_ASSERT(!(indices.size()%3), "MeshTools::subdivide(): index count is not divisible by 3", );

    arrayResize(vertices, Containers::NoInit, vertices.size() + indices.size());
    arrayResize(indices, Containers::NoInit, indices.size()*4);
    subdivideInPlace(Containers::stridedArrayView(indices), Containers::stridedArrayView(vertices), interpolator);
}

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Subdivide a mesh
@m_deprecated_since{2020,06} Use @ref subdivide(Containers::Array<IndexType>&, Containers::Array<Vertex>&vertices, Interpolator)
    or @ref subdivideInPlace() instead.
*/
template<class Vertex, class Interpolator> CORRADE_DEPRECATED("use subdivide(Containers::Array<IndexType>&, Containers::Array<Vertex>&vertices, Interpolator) or subdivideInPlace() instead") void subdivide(std::vector<UnsignedInt>& indices, std::vector<Vertex>& vertices, Interpolator interpolator) {
    CORRADE_ASSERT(!(indices.size()%3), "MeshTools::subdivide(): index count is not divisible by 3", );

    vertices.resize(vertices.size() + indices.size());
    indices.resize(indices.size()*4);
    subdivideInPlace(Containers::stridedArrayView(indices), Containers::stridedArrayView(vertices), interpolator);
}
#endif

/**
@brief Subdivide a mesh in-place
@tparam Vertex          Vertex data type
@tparam Interpolator    See the @p interpolator function parameter
@param[in,out] indices  Index array to operate on
@param[in,out] vertices Vertex array to operate on
@param interpolator     Functor or function pointer which interpolates
    two adjacent vertices: @cpp Vertex interpolator(Vertex a, Vertex b) @ce
@m_since{2020,06}

Assuming the original mesh has @f$ i @f$ indices and @f$ v @f$ vertices,
expects the @p indices array to have a size of @f$ 4i @f$ (as every triangle
face would be divided into four new), with the original indices being in the
first quarter, and the @p vertices array to have a size of @f$ v + i @f$ (as
every original triangle face will get three new vertices). Removing duplicate
vertices in the mesh is up to the user.

Generally, for @f$ k @f$ subsequent subdivisions, the resulting index and
vertex array sizes @f$ i' @f$ and @f$ v' @f$ will be as following. To subdivide
the mesh multiple times in-place, pass correctly sized prefix of the arrays to
each step. @f[
    \begin{array}{rcl}
        i' & = & 4^k i \\
        v' & = & v + \frac{1}{3}(i' - i)
    \end{array}
@f]

@see @ref subdivide(), @ref removeDuplicatesInPlace()
*/
template<class IndexType, class Vertex, class Interpolator> void subdivideInPlace(const Containers::StridedArrayView1D<IndexType>& indices, const Containers::StridedArrayView1D<Vertex>& vertices, Interpolator interpolator) {
    CORRADE_ASSERT(!(indices.size()%12), "MeshTools::subdivideInPlace(): can't divide" << indices.size() << "indices to four parts with each having triangle faces", );
    /* Somehow ~IndexType{} doesn't work for < 4byte types, as the result is
       int(-1) instead of the type I want */
    CORRADE_ASSERT(vertices.size() <= IndexType(-1), "MeshTools::subdivideInPlace(): a" << sizeof(IndexType) << Debug::nospace << "-byte index type is too small for" << vertices.size() << "vertices", );

    /* Subdivide each face to four new */
    const std::size_t indexCount = indices.size()/4;
    std::size_t indexOffset = indexCount;
    std::size_t vertexOffset = vertices.size() - indexCount;
    for(std::size_t i = 0; i != indexCount; i += 3) {
        /* Interpolate each side */
        IndexType newVertices[3];
        for(int j = 0; j != 3; ++j) {
            newVertices[j] = vertexOffset;
            vertices[vertexOffset++] = interpolator(vertices[indices[i+j]], vertices[indices[i+(j+1)%3]]);
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
        indices[indexOffset++] = indices[i];
        indices[indexOffset++] = newVertices[0];
        indices[indexOffset++] = newVertices[2];

        indices[indexOffset++] = newVertices[0];
        indices[indexOffset++] = indices[i+1];
        indices[indexOffset++] = newVertices[1];

        indices[indexOffset++] = newVertices[2];
        indices[indexOffset++] = newVertices[1];
        indices[indexOffset++] = indices[i+2];
        for(std::size_t j = 0; j != 3; ++j)
            indices[i+j] = newVertices[j];
    }
}

/**
 * @overload
 * @m_since{2020,06}
 */
template<class IndexType, class Vertex, class Interpolator> void subdivideInPlace(const Containers::ArrayView<IndexType>& indices, const Containers::StridedArrayView1D<Vertex>& vertices, Interpolator interpolator) {
    subdivideInPlace(Containers::stridedArrayView(indices), vertices, interpolator);
}

}}

#endif
