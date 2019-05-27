#ifndef Magnum_MeshTools_GenerateNormals_h
#define Magnum_MeshTools_GenerateNormals_h
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
 * @brief Function @ref Magnum::MeshTools::generateFlatNormals(), @ref Magnum::MeshTools::generateFlatNormalsInto()
 */

#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/StlForwardVector.h>
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace MeshTools {

/**
@brief Generate flat normals
@param positions    Triangle vertex positions
@return Per-vertex normals

All vertices in each triangle face get the same normal vector. Expects that the
position count is divisible by 3. If you need to generate flat normals for an
indexed mesh, @ref duplicate() the vertices first, after the operation you
might want to remove the duplicates again using @ref removeDuplicates().
Example usage:

@snippet MagnumMeshTools.cpp generateFlatNormals

@see @ref generateFlatNormalsInto()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<Vector3> generateFlatNormals(const Containers::StridedArrayView1D<const Vector3>& positions);

/**
@brief Generate flat normals into an existing array
@param[in]  positions   Triangle vertex positions
@param[out] normals     Where to put the generated normals

A variant of @ref generateFlatNormals() that fills existing memory instead of
allocating a new array.
*/
MAGNUM_MESHTOOLS_EXPORT void generateFlatNormalsInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Generate flat normals
@param indices      Array of triangle face indices
@param positions    Array of vertex positions
@return Normal indices and vectors

All vertices in each triangle face get the same normal vector. Removes
duplicates before returning.

@attention The function requires the mesh to have triangle faces, thus index
    count must be divisible by 3.

@deprecated This will generate index buffer that's different from the input
    @p indices array, so you'll need to recombine them using
    @ref combineIndexedArrays() in order to have a single index array for both
    vertices and normals. Because this makes the usage more complex than
    strictly neccessary, this function is deprecated in favor of
    @ref generateFlatNormals(const Containers::StridedArrayView1D<const Vector3>&).
*/
CORRADE_DEPRECATED("use generateFlatNormals(const Containers::StridedArrayView1D<const Vector3>&) instead") std::pair<std::vector<UnsignedInt>, std::vector<Vector3>> MAGNUM_MESHTOOLS_EXPORT generateFlatNormals(const std::vector<UnsignedInt>& indices, const std::vector<Vector3>& positions);
#endif

}}

#endif
