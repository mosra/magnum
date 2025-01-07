#ifndef Magnum_MeshTools_GenerateIndices_h
#define Magnum_MeshTools_GenerateIndices_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Function @ref Magnum::MeshTools::primitiveCount(), @ref Magnum::MeshTools::generateTrivialIndices(), @ref Magnum::MeshTools::generateTrivialIndicesInto(), @ref Magnum::MeshTools::generateLineStripIndices(), @ref Magnum::MeshTools::generateLineStripIndicesInto(), @ref Magnum::MeshTools::generateLineLoopIndices(), @ref Magnum::MeshTools::generateLineLoopIndicesInto(), @ref Magnum::MeshTools::generateTriangleStripIndices(), @ref Magnum::MeshTools::generateTriangleStripIndicesInto(), @ref Magnum::MeshTools::generateTriangleFanIndices(), @ref Magnum::MeshTools::generateTriangleFanIndicesInto(), @ref Magnum::MeshTools::generateIndices()
 * @m_since{2020,06}
 */

#include "Magnum/Magnum.h"
#include "Magnum/MeshTools/visibility.h"
#ifndef MAGNUM_SINGLES_NO_TRADE_DEPENDENCY
#include "Magnum/Trade/Trade.h"
#endif

namespace Magnum { namespace MeshTools {

#ifndef MAGNUM_SINGLES_NO_TRADE_DEPENDENCY
/**
@brief Actual primitive count for given primitive type and element count
@m_since{2020,06}

Returns how many primitives is generated for given @p primitive and
@p elementCount, for example for @ref MeshPrimitive::Triangles returns
@cpp elementCount/3 @ce. Expects that @p primitive is valid, @p elementCount
is either zero or at least @cpp 2 @ce for a line-based primitive and at least
@cpp 3 @ce for a triangle-based primitive, is divisible by @cpp 2 @ce for
@ref MeshPrimitive::Lines and by @cpp 3 @ce for @ref MeshPrimitive::Triangles.
*/
MAGNUM_MESHTOOLS_EXPORT UnsignedInt primitiveCount(MeshPrimitive primitive, UnsignedInt elementCount);
#endif

/**
@brief Create a trivial index buffer
@m_since_latest

Generates a @cpp 0, 1, 2, 3, 4, 5, ... @ce sequence, i.e. what @ref std::iota()
would produce, optionally with @p offset added to each index. Can be used to
turn a non-indexed mesh into indexed.
@see @ref generateTrivialIndicesInto(), @ref generateLineStripIndices(),
    @ref generateLineLoopIndices(), @ref generateTriangleStripIndices(),
    @ref generateTriangleFanIndices(), @ref generateIndices()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTrivialIndices(UnsignedInt vertexCount, UnsignedInt offset = 0);

/**
@brief Create a trivial index buffer into an existing array
@m_since_latest

A variant of @ref generateTrivialIndices() that fills existing memory
instead of allocating a new array.
*/
MAGNUM_MESHTOOLS_EXPORT void generateTrivialIndicesInto(const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for a line strip primitive
@m_since{2020,06}

Generates a @cpp 0, 1, 1, 2, 3, 4, ... @ce sequence, optionally with @p offset
added to each index. Can be used to convert a @ref MeshPrimitive::LineStrip
mesh to @ref MeshPrimitive::Lines. The @p vertexCount is expected to be either
@cpp 0 @ce or at least @cpp 2 @ce. Primitive restart is not supported. If the
mesh is already indexed, use @ref generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, UnsignedInt)
and overloads instead.
@see @ref generateLineStripIndicesInto(), @ref generateLineLoopIndices(),
    @ref generateTriangleStripIndices(), @ref generateTriangleFanIndices(),
    @ref generateTrivialIndices(), @ref generateIndices()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineStripIndices(UnsignedInt vertexCount, UnsignedInt offset = 0);

/**
@brief Create index buffer for an indexed line strip primitive
@m_since_latest

Like @ref generateLineStripIndices(UnsignedInt, UnsignedInt), but merges
@p indices into the generated line strip index buffer.
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, UnsignedInt offset = 0);

/**
@brief Create index buffer for a line strip primitive with a type-erased index buffer
@m_since_latest

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref generateLineStripIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, UnsignedInt)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineStripIndices(const Containers::StridedArrayView2D<const char>& indices, UnsignedInt offset = 0);

/**
@brief Create index buffer for a line strip primitive into an existing array
@m_since{2020,06}

A variant of @ref generateLineStripIndices() that fills existing memory instead
of allocating a new array. The @p vertexCount is expected to be either
@cpp 0 @ce or at least @cpp 2 @ce, the @p output array is expected to have a
size of @cpp 2*(vertexCount - 1) @ce. Primitive restart is not supported. If
the mesh is already indexed, use @ref generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt)
and overloads instead.
*/
MAGNUM_MESHTOOLS_EXPORT void generateLineStripIndicesInto(UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for an indexed line strip primitive into an existing array
@m_since_latest

Like @ref generateLineStripIndicesInto(UnsignedInt, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt),
but merges @p indices into the generated line strip index buffer.
*/
MAGNUM_MESHTOOLS_EXPORT void generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for a line strip primitive with a type-erased index buffer into an existing array
@m_since_latest

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref generateLineStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT void generateLineStripIndicesInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for a line loop primitive
@m_since{2020,06}

Generates a @cpp 0, 1, 1, 2, 3, ..., 0 @ce sequence, optionally with @p offset
added to each index. Can be used to convert a @ref MeshPrimitive::LineLoop mesh
to @ref MeshPrimitive::Lines. The @p vertexCount is expected to be either
@cpp 0 @ce or at least @cpp 2 @ce. Primitive restart is not supported. If the
mesh is already indexed, use @ref generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, UnsignedInt)
and overloads instead.
@see @ref generateLineLoopIndicesInto(), @ref generateLineStripIndices(),
    @ref generateTriangleStripIndices(), @ref generateTriangleFanIndices(),
    @ref generateTrivialIndices(), @ref generateIndices()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineLoopIndices(UnsignedInt vertexCount, UnsignedInt offset = 0);

/**
@brief Create index buffer for an indexed line loop primitive
@m_since_latest

Like @ref generateLineLoopIndices(UnsignedInt, UnsignedInt), but merges
@p indices into the generated line loop index buffer.
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, UnsignedInt offset = 0);

/**
@brief Create index buffer for a line loop primitive with a type-erased index buffer
@m_since_latest

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref generateLineLoopIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, UnsignedInt)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateLineLoopIndices(const Containers::StridedArrayView2D<const char>& indices, UnsignedInt offset = 0);

/**
@brief Create index buffer for a line loop primitive into an existing array
@m_since{2020,06}

A variant of @ref generateLineLoopIndices() that fills existing memory instead
of allocating a new array. The @p vertexCount is expected to be either
@cpp 0 @ce or at least @cpp 2 @ce, the @p output array is expected to have a
size of @cpp 2*vertexCount @ce. Primitive restart is not supported.If
the mesh is already indexed, use @ref generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt)
and overloads instead.
*/
MAGNUM_MESHTOOLS_EXPORT void generateLineLoopIndicesInto(UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for an indexed line loop primitive into an existing array
@m_since_latest

Like @ref generateLineLoopIndicesInto(UnsignedInt, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt),
but merges @p indices into the generated line loop index buffer.
*/
MAGNUM_MESHTOOLS_EXPORT void generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for a line loop primitive with a type-erased index buffer into an existing array
@m_since_latest

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref generateLineLoopIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT void generateLineLoopIndicesInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for a triangle strip primitive
@m_since{2020,06}

Generates a @cpp 0, 1, 2, 2, 1, 3, 2, 3, 4, ... @ce sequence, optionally with
@p offset added to each index. Can be used to convert a
@ref MeshPrimitive::TriangleStrip mesh to @ref MeshPrimitive::Triangles. The
@p vertexCount is expected to be either @cpp 0 @ce or at least @cpp 3 @ce.
Primitive restart is not supported. If the mesh is already indexed, use
@ref generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, UnsignedInt)
and overloads instead.
@see @ref generateTriangleStripIndicesInto(), @ref generateLineStripIndices(),
    @ref generateLineLoopIndices(), @ref generateTriangleFanIndices(),
    @ref generateTrivialIndices(), @ref generateIndices()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleStripIndices(UnsignedInt vertexCount, UnsignedInt offset = 0);

/**
@brief Create index buffer for an indexed triangle strip primitive
@m_since_latest

Like @ref generateTriangleStripIndices(UnsignedInt, UnsignedInt), but merges
@p indices into the generated triangle strip index buffer.
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, UnsignedInt offset = 0);

/**
@brief Create index buffer for a triangle strip primitive with a type-erased index buffer
@m_since_latest

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref generateTriangleStripIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, UnsignedInt)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleStripIndices(const Containers::StridedArrayView2D<const char>& indices, UnsignedInt offset = 0);

/**
@brief Create index buffer for a triangle strip primitive into an existing array
@m_since{2020,06}

A variant of @ref generateTriangleStripIndices() that fills existing memory
instead of allocating a new array. The @p vertexCount is expected to be either
@cpp 0 @ce or at least @cpp 3 @ce, the @p output array is expected to have a
size of @cpp 3*(vertexCount - 2) @ce. Primitive restart is not supported. If
the mesh is already indexed, use @ref generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt)
and overloads instead.
*/
MAGNUM_MESHTOOLS_EXPORT void generateTriangleStripIndicesInto(UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for an indexed triangle strip primitive into an existing array
@m_since_latest

Like @ref generateTriangleStripIndicesInto(UnsignedInt, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt),
but merges @p indices into the generated triangle strip index buffer.
*/
MAGNUM_MESHTOOLS_EXPORT void generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for a triangle strip primitive with a type-erased index buffer into an existing array
@m_since_latest

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref generateTriangleStripIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT void generateTriangleStripIndicesInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for a triangle fan primitive
@m_since{2020,06}

Generates a @cpp 0, 1, 2, 0, 2, 3, 0, 3, 4, ... @ce sequence, optionally with
@p offset added to each index. Can be used to convert a
@ref MeshPrimitive::TriangleFan mesh to @ref MeshPrimitive::Triangles. The
@p vertexCount is expected to be either @cpp 0 @ce or at least @cpp 3 @ce.
Primitive restart is not supported. If the mesh is already indexed, use
@ref generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, UnsignedInt)
and overloads instead.
@see @ref generateTriangleFanIndicesInto(), @ref generateLineStripIndices(),
    @ref generateLineLoopIndices(), @ref generateTriangleStripIndices(),
    @ref generateTrivialIndices(), @ref generateIndices()
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleFanIndices(UnsignedInt vertexCount, UnsignedInt offset = 0);

/**
@brief Create index buffer for an indexed triangle fan primitive
@m_since_latest

Like @ref generateTriangleFanIndices(UnsignedInt, UnsignedInt), but merges
@p indices into the generated triangle fan index buffer.
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, UnsignedInt offset = 0);

/**
@brief Create index buffer for a triangle fan primitive with a type-erased index buffer
@m_since_latest

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref generateTriangleFanIndices(const Containers::StridedArrayView1D<const UnsignedInt>&, UnsignedInt)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateTriangleFanIndices(const Containers::StridedArrayView2D<const char>& indices, UnsignedInt offset = 0);

/**
@brief Create index buffer for a triangle fan primitive into an existing array
@m_since{2020,06}

A variant of @ref generateTriangleFanIndices() that fills existing memory
instead of allocating a new array. The @p vertexCount is expected to be either
@cpp 0 @ce or at least @cpp 3 @ce, the @p output array is expected to have a
size of @cpp 3*(vertexCount - 2) @ce. Primitive restart is not supported. If
the mesh is already indexed, use @ref generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt)
and overloads instead.
*/
MAGNUM_MESHTOOLS_EXPORT void generateTriangleFanIndicesInto(UnsignedInt vertexCount, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for an indexed triangle fan primitive into an existing array
@m_since_latest

Like @ref generateTriangleFanIndicesInto(UnsignedInt, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt),
but merges @p indices into the generated triangle fan index buffer.
*/
MAGNUM_MESHTOOLS_EXPORT void generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
@brief Create index buffer for a triangle fan primitive with a type-erased index buffer into an existing array
@m_since_latest

Expects that the second dimension of @p indices is contiguous and represents
the actual 1/2/4-byte index type. Based on its size then calls one of the
@ref generateTriangleFanIndicesInto(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<UnsignedInt>&, UnsignedInt)
etc. overloads.
*/
MAGNUM_MESHTOOLS_EXPORT void generateTriangleFanIndicesInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

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

Optionally adds @p offset to each index. Size of @p quads is expected to be
divisible by @cpp 4 @ce and all indices being in bounds of the @p positions
view.
@see @ref generateQuadIndicesInto(), \n
    @ref Math::cross(const Vector3<T>&, const Vector3<T>&), \n
    @ref Math::dot(const Vector<size, T>&, const Vector<size, T>&)
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedInt>& quads, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedShort>& quads, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> generateQuadIndices(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedByte>& quads, UnsignedInt offset = 0);

/**
@brief Create a triangle index buffer for quad primitives into an existing array
@m_since_latest

A variant of @ref generateQuadIndices() that fills existing memory instead of
allocating a new array. Size of @p quads is expected to be divisible by
@cpp 4 @ce and @p output should have a size that's @cpp quads.size()*6/4 @ce.
*/
MAGNUM_MESHTOOLS_EXPORT void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedInt>& quads, const Containers::StridedArrayView1D<UnsignedInt>& output, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedShort>& quads, const Containers::StridedArrayView1D<UnsignedShort>& output, UnsignedInt offset = 0);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_MESHTOOLS_EXPORT void generateQuadIndicesInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<const UnsignedByte>& quads, const Containers::StridedArrayView1D<UnsignedByte>& output, UnsignedInt offset = 0);

#ifndef MAGNUM_SINGLES_NO_TRADE_DEPENDENCY
/**
@brief Convert a mesh to a plain indexed one
@m_since{2020,06}

If @p mesh is one of @ref MeshPrimitive::LineStrip,
@ref MeshPrimitive::LineLoop, @ref MeshPrimitive::TriangleStrip or
@ref MeshPrimitive::TriangleFan primitives, calls one of
@ref generateLineStripIndices(), @ref generateLineLoopIndices(),
@ref generateTriangleStripIndices() or @ref generateTriangleFanIndices()
functions or their indexed overloads to generate the index buffer. In that case
expects that the mesh has either @cpp 0 @ce vertices or at least @cpp 2 @ce
vertices for a line-based primitive and @cpp 3 @ce vertices for a
triangle-based primitive. If @p mesh is a different
@relativeref{Magnum,MeshPrimitive}, it's passed through unchanged if already
indexed, and with indices generated using @ref generateTrivialIndices()
otherwise.

If @p mesh is already indexed, the index type is expected to be
non-implementation-specific.

The resulting mesh always has @ref MeshIndexType::UnsignedInt, call
@ref compressIndices(const Trade::MeshData&, MeshIndexType) on the result to
compress it to a smaller type, if desired. This function will unconditionally
make a copy of all vertex data, use @ref generateIndices(Trade::MeshData&&) to
avoid that copy.
@see @ref isMeshIndexTypeImplementationSpecific()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData generateIndices(const Trade::MeshData& mesh);

/**
@brief Convert a mesh to a plain indexed one
@m_since{2020,06}

Compared to @ref generateIndices(const Trade::MeshData&) this function can
transfer ownership of @p mesh vertex buffer (in case it is owned) to the
returned instance instead of making a copy of it, and index buffer as well if
it's owned, doesn't need expanding and is already with
@ref MeshIndexType::UnsignedInt. Attribute data is copied always.
@see @ref Trade::MeshData::vertexDataFlags()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData generateIndices(Trade::MeshData&& mesh);
#endif

}}

#endif
