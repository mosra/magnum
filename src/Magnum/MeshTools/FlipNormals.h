#ifndef Magnum_MeshTools_FlipNormals_h
#define Magnum_MeshTools_FlipNormals_h
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
 * @brief Function @ref Magnum::MeshTools::flipFaceWindingInPlace(), @ref Magnum::MeshTools::flipFaceWinding(), @ref Magnum::MeshTools::flipNormalsInPlace(), @ref Magnum::MeshTools::flipNormals()
 */

#include <Corrade/Containers/Containers.h>

#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/InterleaveFlags.h"
#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/ArrayViewStl.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/DeprecationMacros.h>
#include <Corrade/Utility/StlForwardVector.h>
#endif

namespace Magnum { namespace MeshTools {

/**
@brief Flip mesh normals in-place
@param[in,out] normals  Normal array to operate on

Flips normal vectors to point in the opposite direction. Note that for face
culling to work properly, face winding may need to be flipped as well --- use
@ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
and overloads to perform both operations at once. See
@ref flipFaceWindingInPlace() for detailed explanation of face winding
direction and its effect on rendering.
@see @ref flipNormalsInPlace(Trade::MeshData&)
*/
MAGNUM_MESHTOOLS_EXPORT void flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>& normals);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@copybrief flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>&)
@m_deprecated_since{2020,06} Use @ref flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>&)
    instead.
*/
CORRADE_DEPRECATED("use flipNormalsInPlace() instead") MAGNUM_MESHTOOLS_EXPORT void flipNormals(std::vector<Vector3>& normals);
#endif

/**
@brief Flip mesh face winding in-place
@param[in,out] indices  Index array to operate on
@m_since{2020,06}

Flips order of indices in each triangle, turning `a, b, c` into `a, c, b` for
each. Expects a triangle mesh, thus size of the @p indices view has to be
divisible by 3.

Face winding direction --- either counterclockwise or clockwise --- is used to distinguish front and back faces of a mesh when doing back face culling, such
as with @ref GL::Renderer::Feature::FaceCulling, and when rendering
double-sided meshes with e.g. @ref Shaders::PhongGL::Flag::DoubleSided. For
correct rendering the winding direction is expected to match normal direction.

If the direction doesn't match, which can happen for example if a negative
scaling is applied on the mesh, or if the mesh data were produced with
expectation that front faces have clockwise winding but the renderer expects
counterclockwise, the winding can be flipped either directly in the mesh by
calling this function or by setting appropriate renderer state such as
@ref GL::Renderer::setFrontFace().

Most commonly you'd want to flip both normals and face winding, for example if
wanting to look at a cube from inside instead of from outside, which can be
done with @ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&).

See also @ref SceneTools::absoluteTransformationScalingSigns3D() which can be
used to collect transformations with negative scaling for a complete scene
hierarchy.
@see @ref flipFaceWindingInPlace(Trade::MeshData&)
*/
MAGNUM_MESHTOOLS_EXPORT void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices);

/**
@brief Flip mesh face winding in-place on a type-erased index array
@m_since{2020,06}

Flips order of indices in each triangle, turning `a, b, c` into `a, c, b` for
each. Expects a triangle mesh, thus size of the @p indices view has to be
divisible by 3, and the second dimension is expected to be contiguous.

Can be used not just with index arrays but with vertex
attribute arrays of arbitrary types as well in cases of a non-indexed mesh. For
1/2/4-byte types delegates into one of the
@ref flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
etc. overloads, for other sizes falls back to byte-by-byte swapping. For
example:

@snippet MeshTools.cpp flipFaceWindingInPlace-attributes

@see @ref flipFaceWindingInPlace(Trade::MeshData&)
*/
MAGNUM_MESHTOOLS_EXPORT void flipFaceWindingInPlace(const Containers::StridedArrayView2D<char>& indices);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
@m_deprecated_since{2020,06} Use @ref flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
    instead.
*/
CORRADE_DEPRECATED("use flipFaceWindingInPlace() instead") MAGNUM_MESHTOOLS_EXPORT void flipFaceWinding(std::vector<UnsignedInt>& indices);
#endif

/**
@brief Flip mesh face winding in-place
@m_since_latest

If the @p mesh is indexed, expects that it has mutable index data with the
indices not being in an implementation-specific format and index count is
divisible by 3. Then calls @ref flipFaceWindingInPlace(const Containers::StridedArrayView2D<char>&)
on the index array and leaves vertex and attribute data untouched.

If the @p mesh is not indexed, instead expects that it has mutable vertex data
with no attributes having an implementation-specific format and vertex count is divisible by 3. Then calls @ref flipFaceWindingInPlace(const Containers::StridedArrayView2D<char>&)
on all attribute arrays.

Currently the implementation expects that @p mesh is
@ref MeshPrimitive::Triangles. If you have triangle strips or fans, convert
them to indexed triangles with @ref generateIndices() first.

See documentation of @ref flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
for details about what all is affected by a mesh face winding direction. Use
@ref flipNormalsInPlace(Trade::MeshData&) to flip both normals and face
winding.
@see @ref Trade::MeshData::isIndexed(),
    @ref Trade::MeshData::indexDataFlags(),
    @ref Trade::MeshData::vertexDataFlags(),
    @ref Trade::MeshData::indexType() const,
    @ref isMeshIndexTypeImplementationSpecific(),
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt, Int) const,
    @ref isVertexFormatImplementationSpecific()
*/
MAGNUM_MESHTOOLS_EXPORT void flipFaceWindingInPlace(Trade::MeshData& mesh);

/**
@brief Flip mesh face winding
@m_since_latest

If the @p mesh is indexed, expects that the indices are not in an
implementation-specific format and index count is divisible by 3. Then makes a
copy of the index array, calls @ref flipFaceWindingInPlace(const Containers::StridedArrayView2D<char>&)
on it, and copies vertex and attribute data untouched.

If the @p mesh is not indexed, instead expects that no attributes have an
implementation-specific format and vertex count is divisible by 3. Then makes a
copy of the vertex array and calls @ref flipFaceWindingInPlace(const Containers::StridedArrayView2D<char>&)
on all attribute arrays. The attribute data are copied untouched.

Currently the implementation expects that @p mesh is
@ref MeshPrimitive::Triangles. If you have triangle strips or fans, convert
them to indexed triangles with @ref generateIndices() first.

See documentation of @ref flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
for details about what all is affected by a mesh face winding direction. See
also @ref flipFaceWinding(Trade::MeshData&&) for a potentially more efficient
operation instead of always performing a full copy, you can also do an in-place
transformation using @ref flipFaceWindingInPlace(Trade::MeshData&). Use
@ref flipNormals(const Trade::MeshData&, InterleaveFlags) to flip both normals
and face winding.
@see @ref Trade::MeshData::isIndexed(),
    @ref Trade::MeshData::indexType() const,
    @ref isMeshIndexTypeImplementationSpecific(),
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt, Int) const,
    @ref isVertexFormatImplementationSpecific()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData flipFaceWinding(const Trade::MeshData& mesh);

/**
@brief Flip mesh face winding
@m_since_latest

Compared to @ref flipFaceWinding(const Trade::MeshData&) this function can
transfer ownership of @p mesh vertex and index buffers (in case they're owned)
to the returned instance instead of making a copy of them. Attribute data are
copied always.
@see @ref Trade::MeshData::indexDataFlags(),
    @ref Trade::MeshData::vertexDataFlags()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData flipFaceWinding(Trade::MeshData&& mesh);

/**
@brief Flip mesh normals and face winding in-place
@param[in,out] indices  Index array to operate on
@param[in,out] normals  Normal array to operate on
@m_since{2020,06}

Same as calling both @ref flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>&)
and @ref flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>&)
at the same time. See their documentation for more information.
*/
inline void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

/**
 * @overload
 * @m_since{2020,06}
 */
inline void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

/**
 * @overload
 * @m_since{2020,06}
 */
inline void flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

/**
@brief Flip mesh normals and face winding in-place on a type-erased index array
@m_since{2020,06}

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
etc. overloads.
*/
inline void flipNormalsInPlace(const Containers::StridedArrayView2D<char>& indices, const Containers::StridedArrayView1D<Vector3>& normals) {
    flipFaceWindingInPlace(indices);
    flipNormalsInPlace(normals);
}

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
@m_deprecated_since{2020,06} Use @ref flipNormalsInPlace(const Containers::StridedArrayView1D<UnsignedInt>&, const Containers::StridedArrayView1D<Vector3>&)
    instead.
*/
CORRADE_DEPRECATED("use flipNormalsInPlace() instead") MAGNUM_MESHTOOLS_EXPORT void flipNormals(std::vector<UnsignedInt>& indices, std::vector<Vector3>& normals);
#endif

/**
@brief Flip mesh normals and face winding in-place
@m_since_latest

If the @p mesh contains @ref Trade::MeshAttribute::Normal attributes, expects
that the vertex data are mutable, with all normals having
@ref VertexFormat::Vector3 --- if you can't guarantee that, use
@ref flipNormals(const Trade::MeshData&, InterleaveFlags) instead. If the mesh
doesn't contain any normals, only face winding is flipped.

If the @p mesh is indexed, expects that it has mutable index data with the
indices not being in an implementation-specific type and index count is
divisible by 3. If the @p mesh is not indexed, instead expects that it has
mutable vertex data, *all* attributes are not in an implementation-specific
format and vertex count is divisible by 3.

Currently the implementation expects that @p mesh is
@ref MeshPrimitive::Triangles. If you have triangle strips or fans, convert
them to indexed triangles with @ref generateIndices() first.

Delegates to @ref flipFaceWindingInPlace(Trade::MeshData&) and then for all
@ref Trade::MeshAttribute::Normal attributes calls @ref flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>&).
Use @ref flipFaceWindingInPlace(Trade::MeshData&) for flipping just face
winding without affecting normals.
@see @ref Trade::MeshData::isIndexed(),
    @ref Trade::MeshData::indexDataFlags(),
    @ref Trade::MeshData::vertexDataFlags(),
    @ref Trade::MeshData::indexType() const,
    @ref isMeshIndexTypeImplementationSpecific(),
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt, Int) const,
    @ref isVertexFormatImplementationSpecific()
*/
MAGNUM_MESHTOOLS_EXPORT void flipNormalsInPlace(Trade::MeshData& mesh);

/**
@brief Flip mesh normals and face winding
@m_since_latest

If the @p mesh is indexed, expects that the indices are not in an
implementation-specific type and index count is divisible by 3. If the @p mesh
is not indexed, instead expects that *all* attributes are not in an
implementation-specific format and vertex count is divisible by 3.

If the mesh contains @ref Trade::MeshAttribute::Normal attributes, they're all
converted to @ref VertexFormat::Vector3 if not already. In case a conversion
needs to happen, *all* attributes are expected to not have an
implementation-specific format. The data layouting is done by
@ref interleavedLayout() with the @p flags parameter propagated to it, see its
documentation for detailed behavior description.

Currently the implementation expects that @p mesh is
@ref MeshPrimitive::Triangles. If you have triangle strips or fans, convert
them to indexed triangles with @ref generateIndices() first.

See also @ref flipNormals(Trade::MeshData&&, InterleaveFlags) for a potentially
more efficient operation instead of always performing a full copy, you can also
do an in-place transformation using @ref flipNormalsInPlace(Trade::MeshData&).
Use @ref flipFaceWinding(const Trade::MeshData&) for flipping just face winding
without affecting normals.
@see @ref Trade::MeshData::isIndexed(),
    @ref Trade::MeshData::indexType() const,
    @ref isMeshIndexTypeImplementationSpecific(),
    @ref Trade::MeshData::attributeFormat(MeshAttribute, UnsignedInt, Int) const,
    @ref isVertexFormatImplementationSpecific()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData flipNormals(const Trade::MeshData& mesh, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Flip mesh normals and face winding
@m_since_latest

Compared to @ref flipNormals(const Trade::MeshData&, InterleaveFlags) this
function can transfer ownership of @p mesh vertex and index buffers (in case
they're owned and all @ref Trade::MeshAttribute::Normal attributes are
@ref VertexFormat::Vector3) to the returned instance instead of making a copy
of them. Attribute data are copied always.
@see @ref Trade::MeshData::indexDataFlags(),
    @ref Trade::MeshData::vertexDataFlags()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData flipNormals(Trade::MeshData&& mesh, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

}}

#endif
