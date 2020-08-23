#ifndef Magnum_MeshTools_GenerateNormals_h
#define Magnum_MeshTools_GenerateNormals_h
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
 * @brief Function @ref Magnum::MeshTools::generateFlatNormals(), @ref Magnum::MeshTools::generateFlatNormalsInto(), @ref Magnum::MeshTools::generateSmoothNormals(), @ref Magnum::MeshTools::generateSmoothNormalsInto()
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
@m_since{2019,10}

All vertices in each triangle face get the same normal vector. Expects that the
position count is divisible by 3. If you need to generate flat normals for an
indexed mesh, @ref duplicate() the vertices first, after the operation you
might want to remove the duplicates again using @ref removeDuplicatesInPlace().
Example usage:

@snippet MagnumMeshTools.cpp generateFlatNormals

@see @ref generateFlatNormalsInto(), @ref generateSmoothNormals(),
    @ref MeshTools::CompileFlag::GenerateFlatNormals,
    @ref Math::cross(const Vector3<T>&, const Vector3<T>&)
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<Vector3> generateFlatNormals(const Containers::StridedArrayView1D<const Vector3>& positions);

/**
@brief Generate flat normals into an existing array
@param[in]  positions   Triangle vertex positions
@param[out] normals     Where to put the generated normals
@m_since{2019,10}

A variant of @ref generateFlatNormals() that fills existing memory instead of
allocating a new array. The @p normals array is expected to have the same size
as @p positions.

Useful when you need to interface for example with STL containers --- in that
case @cpp #include @ce @ref Corrade/Containers/ArrayViewStl.h to get implicit
conversions:

@snippet MagnumMeshTools-stl.cpp generateFlatNormalsInto

@see @ref generateSmoothNormalsInto()
*/
MAGNUM_MESHTOOLS_EXPORT void generateFlatNormalsInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Generate flat normals
@param indices      Triangle face indices
@param positions    Triangle vertex positions
@return Normal indices and vectors

All vertices in each triangle face get the same normal vector. Removes
duplicates before returning. Expects that the position count is divisible by 3.

@m_deprecated_since{2019,10} This will generate index buffer that's different
    from the input @p indices array, so you'll need to recombine them using
    @ref combineIndexedArrays() in order to have a single index array for both
    vertices and normals. Because this makes the usage more complex than
    strictly necessary, this function is deprecated in favor of
    @ref generateFlatNormals(const Containers::StridedArrayView1D<const Vector3>&).
*/
CORRADE_DEPRECATED("use generateFlatNormals(const Containers::StridedArrayView1D<const Vector3>&) instead") std::pair<std::vector<UnsignedInt>, std::vector<Vector3>> MAGNUM_MESHTOOLS_EXPORT generateFlatNormals(const std::vector<UnsignedInt>& indices, const std::vector<Vector3>& positions);
#endif

/**
@brief Generate smooth normals
@param indices      Triangle face indices
@param positions    Triangle vertex positions
@return Per-vertex normals
@m_since{2019,10}

Uses the @p indices array to discover adjacent triangles and then for each
vertex position calculates a normal averaged from all triangles that share it.
The normal is weighted according to adjacent triangle area and angle at given
vertex; hard edges are preserved where adjacent triangles don't share vertices.
Triangles with zero area or triangles containing invalid positions (NaNs) don't
contribute to calculated vertex normals.

Implementation is based on the article
[Weighted Vertex Normals](http://www.bytehazard.com/articles/vertnorm.html) by
Martijn Buijs.
@see @ref generateSmoothNormalsInto(), @ref generateFlatNormals(),
    @ref MeshTools::CompileFlag::GenerateSmoothNormals
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<Vector3> generateSmoothNormals(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<const Vector3>& positions);

/**
 * @overload
 * @m_since{2019,10}
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<Vector3> generateSmoothNormals(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<const Vector3>& positions);

/**
 * @overload
 * @m_since{2019,10}
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<Vector3> generateSmoothNormals(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<const Vector3>& positions);

/**
@brief Generate smooth normals using a type-erased index array
@m_since{2020,06}

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref generateSmoothNormals(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const Vector3>&)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<Vector3> generateSmoothNormals(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<const Vector3>& positions);

/**
@brief Generate smooth normals into an existing array
@param[in] indices      Triangle face indices
@param[in] positions    Triangle vertex positions
@param[out] normals     Where to put the generated normals
@m_since{2019,10}

A variant of @ref generateSmoothNormals() that fills existing memory instead of
allocating a new array. The @p normals array is expected to have the same size
as @p positions. Note that even with the output array this function isn't fully
allocation-free --- it still allocates three additional internal arrays for
adjacent face calculation.

Useful when you need to interface for example with STL containers --- in that
case @cpp #include @ce @ref Corrade/Containers/ArrayViewStl.h to get implicit
conversions:

@snippet MagnumMeshTools-stl.cpp generateSmoothNormalsInto

@see @ref generateFlatNormalsInto()
*/
MAGNUM_MESHTOOLS_EXPORT void generateSmoothNormalsInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals);

/**
 * @overload
 * @m_since{2019,10}
 */
MAGNUM_MESHTOOLS_EXPORT void generateSmoothNormalsInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals);

/**
 * @overload
 * @m_since{2019,10}
 */
MAGNUM_MESHTOOLS_EXPORT void generateSmoothNormalsInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals);

/**
@brief Generate smooth normals into an existing array using a type-erased index array
@m_since{2020,06}

Expects that @p normals has the same size as @p positions and that the second
dimension of @p indices is contiguous and represents the actual 1/2/4-byte
index type. Based on its size then calls one of the
@ref generateSmoothNormalsInto(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const Vector3>&, const Containers::StridedArrayView1D<Vector3>&)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT void generateSmoothNormalsInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals);

}}

#endif
