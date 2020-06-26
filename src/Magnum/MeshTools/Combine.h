#ifndef Magnum_MeshTools_Combine_h
#define Magnum_MeshTools_Combine_h
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
 * @brief Function @ref Magnum::MeshTools::combineIndexedAttributes(), @ref Magnum::MeshTools::combineFaceAttributes()
 * @m_since{2020,06}
 */

#include <initializer_list>

#include "Magnum/MeshTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace MeshTools {

/**
@brief Combine differently indexed attributes into a single mesh
@m_since{2020,06}

Assuming each @p data contains only unique vertex data, creates an indexed mesh
that contains all attributes from @p data combined, with duplicate vertices
removed. For example, when you have a position and a normal array, each indexed
with separate indices like this:

@code{.cpp}
{pA, pB, pC, pD, pE, pF}        // positions
{nA, nB, nC, nD, nE, nF, nG}    // normals

{0, 2, 5, 0, 0, 1, 3, 2, 2}     // position indices
{1, 3, 4, 1, 4, 6, 1, 3, 1}     // normal indices
@endcode

Then the first triangle in the mesh is defined as
@cb{.cpp} {pA, nB}, {pC, nD}, {pF, nE} @ce. When combined together using this
function, the resulting mesh stays the same but there's just one index array,
indexing both positions and normals:

@code{.cpp}
{{pA, nB}, {pC, nD}, {pF, nE}, {pA, nE}, {pB, nG}, {pD, nB}, {pC, nB}}
                                // unique pairs of positions and normals

{0, 1, 2, 0, 3, 4, 5, 1, 6}     // unified indices
@endcode

The function preserves all attribute data including repeated or custom
attributes. The resulting mesh is interleaved, with all attributes packed
tightly together. If you need to add specific padding for alignment
preservation, pass the result to @ref interleave() and specify the paddings
between attributes manually. Similarly, for simplicity the resulting mesh has
always @ref MeshIndexType::UnsignedInt --- use @ref compressIndices(const Trade::MeshData&, MeshIndexType)
if you want to have it compressed to a smaller type.

Vertex data unreferenced by the index buffers are discarded. This means the
function can be also called with just a single argument to compact a mesh with
a sparse index buffer.

Expects that @p data is non-empty and all data have the same primitive and
index count. All inputs have to be indexed, although the particular
@ref MeshIndexType doesn't matter. For non-indexed attributes combining can be
done much more efficiently using @ref duplicate(const Trade::MeshData&, Containers::ArrayView<const Trade::MeshAttributeData>),
alternatively you can turn a non-indexed attribute to an indexed one first
using @ref removeDuplicatesInPlace() and then call this function.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData combineIndexedAttributes(const Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> data);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData combineIndexedAttributes(std::initializer_list<Containers::Reference<const Trade::MeshData>> data);

/**
@brief Combine per-face attributes into an existing mesh
@m_since{2020,06}

The resulting mesh will have all per-face attributes turned into per-vertex
attributes, leaving only unique combinations and adjusting the index buffer
accordingly. The resulting mesh has the same amount of indices, but likely
more vertices.

Expects that @p mesh is indexed @ref MeshPrimitive::Triangles and
@p faceAttributes is @ref MeshPrimitive::Faces, with face element count of the
latter corresponding to triangle count of the former. If @p faceAttributes is
indexed, it's assumed to have the data unique; if it's not indexed, it's first
made unique using @ref removeDuplicates() and in that case it's expected to
be interleaved.
@see @ref isInterleaved()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData combineFaceAttributes(const Trade::MeshData& mesh, const Trade::MeshData& faceAttributes);

/**
@overload
@m_since{2020,06}

Same as above with @p faceAttributes wrapped in a @ref Trade::MeshData with
@ref MeshPrimitive::Faces and no index buffer. Same as in the above case,
@p faceAttributes is expected to be interleaved. Note that offset-only
@ref Trade::MeshAttributeData instances are not supported in the
@p faceAttributes array.
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData combineFaceAttributes(const Trade::MeshData& mesh, Containers::ArrayView<const Trade::MeshAttributeData> faceAttributes);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData combineFaceAttributes(const Trade::MeshData& mesh, std::initializer_list<Trade::MeshAttributeData> faceAttributes);

}}

#endif
