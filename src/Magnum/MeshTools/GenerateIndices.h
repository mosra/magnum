#ifndef Magnum_MeshTools_GenerateIndices_h
#define Magnum_MeshTools_GenerateIndices_h
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
 * @brief Function @ref Magnum::MeshTools::primitiveCount(), @ref Magnum::MeshTools::generateLineStripIndices(), @ref Magnum::MeshTools::generateLineStripIndicesInto(), @ref Magnum::MeshTools::generateLineLoopIndices(), @ref Magnum::MeshTools::generateLineLoopIndicesInto(), @ref Magnum::MeshTools::generateTriangleStripIndices(), @ref Magnum::MeshTools::generateTriangleStripIndicesInto(), @ref Magnum::MeshTools::generateTriangleFanIndices(), @ref Magnum::MeshTools::generateTriangleFanIndicesInto(), @ref Magnum::MeshTools::generateIndices()
 * @m_since{2020,06}
 */

#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MeshTools {

/**
@brief Actual primitive count for given primitive type and element count
@m_since{2020,06}

Returns how many primitives is generated for given @p primitive and
@p elementCount, for example for @ref MeshPrimitive::Triangles returns
@cpp elementCount/3 @ce. Expects that @p primitive is valid, return value is
rounded down if there's not enough elements for given primitive type (so for
14 vertices you get just 4 triangles, for example).
*/
MAGNUM_MESHTOOLS_EXPORT UnsignedInt primitiveCount(MeshPrimitive primitive, UnsignedInt elementCount);

/**
@brief Create index buffer for a line strip primitive
@m_since{2020,06}

Can be used to convert a @ref MeshPrimitive::LineStrip mesh to
@ref MeshPrimitive::Lines. The @p vertexCount is expected to be at least
@cpp 2 @ce. Primitive restart is not supported.
@see @ref generateLineStripIndicesInto(), @ref generateLineLoopIndices(),
    @ref generateTriangleStripIndices(), @ref generateTriangleFanIndices(),
    @ref generateIndices()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineStripIndices(UnsignedInt vertexCount);

/**
@brief Create index buffer for a line strip primitive into an existing array
@m_since{2020,06}

A variant of @ref generateLineStripIndicesInto() that fills existing memory
instead of allocating a new array. The @p vertexCount is expected to be at
least @cpp 2 @ce, the @p indices array is expected to have a size of
@cpp 2*(vertexCount - 1) @ce. Primitive restart is not supported.
*/
MAGNUM_MESHTOOLS_EXPORT void generateLineStripIndicesInto(UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& indices);

/**
@brief Create index buffer for a line loop primitive
@m_since{2020,06}

Can be used to convert a @ref MeshPrimitive::LineLoop mesh to
@ref MeshPrimitive::Lines. The @p vertexCount is expected to be at least
@cpp 2 @ce. Primitive restart is not supported.
@see @ref generateLineLoopIndicesInto(), @ref generateLineStripIndices(),
    @ref generateTriangleStripIndices(), @ref generateTriangleFanIndices(),
    @ref generateIndices()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineLoopIndices(UnsignedInt vertexCount);

/**
@brief Create index buffer for a line loop primitive into an existing array
@m_since{2020,06}

A variant of @ref generateLineLoopIndicesInto() that fills existing memory
instead of allocating a new array. The @p vertexCount is expected to be at
least @cpp 2 @ce, the @p indices array is expected to have a size of
@cpp 2*vertexCount @ce. Primitive restart is not supported.
*/
MAGNUM_MESHTOOLS_EXPORT void generateLineLoopIndicesInto(UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& into);

/**
@brief Create index buffer for a triangle strip primitive
@m_since{2020,06}

Can be used to convert a @ref MeshPrimitive::TriangleStrip mesh to
@ref MeshPrimitive::Triangles. The @p vertexCount is expected to be at least
@cpp 3 @ce. Primitive restart is not supported.
@see @ref generateTriangleStripIndicesInto(), @ref generateLineStripIndices(),
    @ref generateLineLoopIndices(), @ref generateTriangleFanIndices(),
    @ref generateIndices()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleStripIndices(UnsignedInt vertexCount);

/**
@brief Create index buffer for a triangle strip primitive into an existing array
@m_since{2020,06}

A variant of @ref generateTriangleStripIndicesInto() that fills existing memory
instead of allocating a new array. The @p vertexCount is expected to be at
least @cpp 3 @ce, the @p indices array is expected to have a size of
@cpp 3*(vertexCount - 2) @ce. Primitive restart is not supported.
*/
MAGNUM_MESHTOOLS_EXPORT void generateTriangleStripIndicesInto(UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& into);

/**
@brief Create index buffer for a triangle fan primitive
@m_since{2020,06}

Can be used to convert a @ref MeshPrimitive::TriangleFan mesh to
@ref MeshPrimitive::Triangles. The @p vertexCount is expected to be at least
@cpp 3 @ce. Primitive restart is not supported.
@see @ref generateTriangleFanIndicesInto(), @ref generateLineStripIndices(),
    @ref generateLineLoopIndices(), @ref generateTriangleStripIndices(),
    @ref generateIndices()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleFanIndices(UnsignedInt vertexCount);

/**
@brief Create index buffer for a triangle fan primitive into an existing array
@m_since{2020,06}

A variant of @ref generateTriangleFanIndicesInto() that fills existing memory
instead of allocating a new array. The @p vertexCount is expected to be at
least @cpp 3 @ce, the @p indices array is expected to have a size of
@cpp 3*(vertexCount - 2) @ce. Primitive restart is not supported.
*/
MAGNUM_MESHTOOLS_EXPORT void generateTriangleFanIndicesInto(UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& into);

/**
@brief Create a triangle index buffer for quad primitives
@m_since_latest

@htmlinclude triangulate.svg

For each quad `ABCD` gives a pair of triangles that is either `ABC ACD` or
`DAB DBC`, correctly handling cases of non-convex quads and avoiding thin
triangles where possible. Loosely based on [this SO question](https://stackoverflow.com/q/12239876):

1.  If normals of triangles `ABC` and `ACD` point in opposite direction and
    `DAB DBC` not (which is equivalent to points `D` and `B` being on the same
    side of a diagonal `AC` in a two-dimensional case), split as `DAB DBC`
2.  Otherwise, if normals of triangles `DAB` and `DBC` point in opposite
    direction and `ABC ACD` not (which is equivalent to points `A` and `C`
    being on the same side of a diagonal `DB` in a two-dimensional case), split
    as `ABC ACD`
3.  Otherwise the normals either point in the same direction in both cases or
    the quad is non-planar and ambiguous, pick the case where the diagonal is
    shorter

Size of @p quads is expected to be divisible by @cpp 4 @ce and all indices
being in bounds of the @p positions view.
@see @ref generateQuadIndicesInto(), \n
    @ref Math::cross(const Vector3<T>&, const Vector3<T>&), \n
    @ref Math::dot(const Vector<size, T>&, const Vector<size, T>&)
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedInt>& quads);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedShort>& quads);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedByte>& quads);

/**
@brief Create a triangle index buffer for quad primitives into an existing array
@m_since_latest

A variant of @ref generateQuadIndices() that fills existing memory instead of
allocating a new array. Size of @p quads is expected to be divisible by @cpp 4 @ce
and @p into should have a size that's @cpp quads.size()*6/4 @ce.
*/
MAGNUM_MESHTOOLS_EXPORT void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedInt>& quads, const Containers::StridedArrayView1D<UnsignedInt>& into);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedShort>& quads, const Containers::StridedArrayView1D<UnsignedShort>& into);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedByte>& quads, const Containers::StridedArrayView1D<UnsignedByte>& into);

/**
@brief Convert a mesh to plain indexed lines or triangles
@m_since{2020,06}

Expects that @p mesh is not indexed and is one of
@ref MeshPrimitive::LineStrip, @ref MeshPrimitive::LineLoop,
@ref MeshPrimitive::TriangleStrip, @ref MeshPrimitive::TriangleFan primitives,
calling one of @ref generateLineStripIndices(), @ref generateLineLoopIndices(),
@ref generateTriangleStripIndices() or @ref generateTriangleFanIndices()
functions to generate the index buffer. If your mesh is indexed, call
@ref duplicate(const Trade::MeshData& data, Containers::ArrayView<const Trade::MeshAttributeData>)
on it first.

The resulting mesh always has @ref MeshIndexType::UnsignedInt, call
@ref compressIndices(const Trade::MeshData&, MeshIndexType) on the result to
compress it to a smaller type, if desired. This function will unconditionally
make a copy of all vertex data, use @ref generateIndices(Trade::MeshData&&) to
avoid that copy.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData generateIndices(const Trade::MeshData& mesh);

/**
@brief Convert a mesh to plain indexed lines or triangles
@m_since{2020,06}

Compared to @ref generateIndices(const Trade::MeshData&) this function can
transfer ownership of @p data vertex buffer (in case it is owned) to the
returned instance instead of making a copy of it. Attribute data is copied
always.
@see @ref Trade::MeshData::vertexDataFlags()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData generateIndices(Trade::MeshData&& data);

}}

#endif
