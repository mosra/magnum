#ifndef Magnum_MeshTools_GenerateNormals_h
#define Magnum_MeshTools_GenerateNormals_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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
 * @brief Function @ref Magnum::MeshTools::generateFlatNormals(), @ref Magnum::MeshTools::generateFlatNormalsInto(), @ref Magnum::MeshTools::generateSmoothNormals(), @ref Magnum::MeshTools::generateSmoothNormalsInto()
 */

#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/visibility.h"
#include "Magnum/MeshTools/InterleaveFlags.h"

#include "Magnum/Trade/Trade.h"

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

@snippet MeshTools.cpp generateFlatNormals

@see @ref generateFlatNormalsInto(), @ref generateSmoothNormals(),
    @ref generateFlatNormals(const Trade::MeshData&, UnsignedInt, Int, InterleaveFlags),
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

@snippet MeshTools-stl.cpp generateFlatNormalsInto

@see @ref generateSmoothNormalsInto()
*/
MAGNUM_MESHTOOLS_EXPORT void generateFlatNormalsInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals);

/**
@brief Generate flat normals in a mesh data
@m_since_latest

Expects that the mesh contains a three-dimensional
@ref Trade::MeshAttribute::Position with index @p id (and in morph target
@p morphTargetId if not @cpp -1 @ce) and that the attribute does not have an
implementation-specific format. If @p id is non-zero expects that there's also
at least @p id @ref Trade::MeshAttribute::Normal attributes in the same morph
target. If there is a normal attribute matching the positions, it's expected to
not have an implementation-specific format either, and it gets overwritten with
the generated normals. If there is not, a new normal attribute with @p id and
@p morphTargetId is added to contain the generated normals.

The positions are converted to @ref VertexFormat::Vector3 in order to perform
the calculation if not already. If there is a normal attribute with @p id and
@p morphTargetId, it's also converted to @ref VertexFormat::Vector3 if not
already. In case a conversion needs to happen, *all* attributes are expected to not have an implementation-specific format. The data layouting is done by
@ref interleavedLayout() with the @p flags parameter propagated to it, see its
documentation for detailed behavior description. Other attributes, position and
normal attributes other than @p id or with different @p morphTargetId are
passed through untouched. Then,
@ref generateFlatNormalsInto(const Containers::StridedArrayView1D<const Vector3>&, const Containers::StridedArrayView1D<Vector3>&)
is called on the corresponding position and normal attribute.

The mesh is expected to *not* be indexed as the generated normals may be
different for each vertex and thus cannot be shared. If you want to generate
flat normals for an indexed mesh, call @ref duplicate(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>)
on it first.

Currently the implementation expects that @p mesh is
@ref MeshPrimitive::Triangles and vertex count is divisible by 3. If you have
triangle strips or fans, convert them to indexed triangles with
@ref generateIndices() first, and then @ref duplicate(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>)
to get rid of the index buffer.

See also @ref generateFlatNormals(Trade::MeshData&&, UnsignedInt, Int, InterleaveFlags)
for a potentially more efficient operation instead of always performing a full
copy. Unlike other @ref MeshTools APIs there is no `*InPlace` variant as in the
common case the input mesh doesn't yet contain a normal attribute, thus the
operation cannot be performed in-place.
@see @ref generateSmoothNormals(const Trade::MeshData&, UnsignedInt, Int, InterleaveFlags),
    @ref Trade::MeshData::attributeCount(MeshAttribute, Int) const,
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt, Int) const,
    @ref isVertexFormatImplementationSpecific(),
    @ref Trade::MeshData::isIndexed(), @ref Trade::MeshData::primitive(),
    @ref Trade::MeshData::vertexCount()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData generateFlatNormals(const Trade::MeshData& mesh, UnsignedInt id = 0, Int morphTargetId = -1, InterleaveFlags flags = {});

/**
@brief Generate flat normals in a mesh data
@m_since_latest

Compared to @ref generateFlatNormals(const Trade::MeshData&, UnsignedInt, Int, InterleaveFlags)
this function can transfer ownership of @p mesh vertex buffer (in case it's
owned and has expected attribute formats) to the returned instance instead of
making a copy of it. Attribute data are copied always.
@see @ref Trade::MeshData::indexDataFlags(),
    @ref Trade::MeshData::vertexDataFlags()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData generateFlatNormals(Trade::MeshData&& mesh, UnsignedInt id = 0, Int morphTargetId = -1, InterleaveFlags flags = {});

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
contribute to calculated vertex normals. Normals for @p positions that aren't
referenced by any @p indices are left uninitialized.

Implementation is based on the article
[Weighted Vertex Normals](http://www.bytehazard.com/articles/vertnorm.html) by
Martijn Buijs.
@see @ref generateSmoothNormalsInto(), @ref generateFlatNormals(),
    @ref generateSmoothNormals(const Trade::MeshData&, UnsignedInt, Int, InterleaveFlags)
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
as @p positions. Normals for @p positions that aren't referenced by any
@p indices are left untouched. Note that even with the output array this
function isn't fully allocation-free --- it still does one internal allocation
for adjacent face calculation.

Useful when you need to interface for example with STL containers --- in that
case @cpp #include @ce @ref Corrade/Containers/ArrayViewStl.h to get implicit
conversions:

@snippet MeshTools-stl.cpp generateSmoothNormalsInto

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

/**
@brief Generate smooth normals in a mesh data
@m_since_latest

Expects that the mesh contains a three-dimensional
@ref Trade::MeshAttribute::Position with index @p id (and in morph target
@p morphTargetId if not @cpp -1 @ce) and that the attribute does not have an
implementation-specific format. If @p id is non-zero expects that there's also
at least @p id @ref Trade::MeshAttribute::Normal attributes in the same morph
target. If there is a normal attribute matching the positions, it's expected to
not have an implementation-specific format either, and it gets overwritten with
the generated normals. If there is not, a new normal attribute with @p id and
@p morphTargetId is added to contain the generated normals.

The positions are converted to @ref VertexFormat::Vector3 in order to perform
the calculation if not already. If there is a normal attribute with @p id and
@p morphTargetId, it's also converted to @ref VertexFormat::Vector3 if not
already. In case a conversion needs to happen, *all* attributes are expected to not have an implementation-specific format. The data layouting is done by
@ref interleavedLayout() with the @p flags parameter propagated to it, see its
documentation for detailed behavior description. Other attributes, position and
normal attributes other than @p id or with different @p morphTargetId are
passed through untouched. Then,
@ref generateFlatNormalsInto(const Containers::StridedArrayView1D<const Vector3>&, const Containers::StridedArrayView1D<Vector3>&)
is called on the corresponding position and normal attribute.

The mesh is expected to be indexed as generating smooth normals needs triangle
adjacency information to average them over neighboring faces. Without an index
buffer you can either generate flat normals with
@ref generateFlatNormals(const Trade::MeshData&, UnsignedInt, Int, InterleaveFlags)
or first use @ref removeDuplicates(const Trade::MeshData&) to produce an index
buffer by merging duplicate vertices and then call this function.

Currently the implementation expects that @p mesh is
@ref MeshPrimitive::Triangles and index count is divisible by 3. If you have
triangle strips or fans, convert them to indexed triangles with
@ref generateIndices() first.

See also @ref generateSmoothNormals(Trade::MeshData&&, UnsignedInt, Int, InterleaveFlags)
for a potentially more efficient operation instead of always performing a full
copy. Unlike other @ref MeshTools APIs there is no `*InPlace` variant as in the
common case the input mesh doesn't yet contain a normal attribute, thus the
operation cannot be performed in-place.
@see @ref Trade::MeshData::attributeCount(MeshAttribute, Int) const,
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt, Int) const,
    @ref isVertexFormatImplementationSpecific(),
    @ref Trade::MeshData::isIndexed(), @ref Trade::MeshData::primitive(),
    @ref Trade::MeshData::indexCount()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData generateSmoothNormals(const Trade::MeshData& mesh, UnsignedInt id = 0, Int morphTargetId = -1, InterleaveFlags flags = {});

/**
@brief Generate smooth normals in a mesh data
@m_since_latest

Compared to @ref generateSmoothNormals(const Trade::MeshData&, UnsignedInt, Int, InterleaveFlags)
this function can transfer ownership of @p mesh vertex and index buffers (in
case they're owned and attributes have expected formats) to the returned
instance instead of making a copy of them. Attribute data are copied always.
@see @ref Trade::MeshData::indexDataFlags(),
    @ref Trade::MeshData::vertexDataFlags()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData generateSmoothNormals(Trade::MeshData&& mesh, UnsignedInt id = 0, Int morphTargetId = -1, InterleaveFlags flags = {});

}}

#endif
