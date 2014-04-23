#ifndef Magnum_MeshTools_CompressIndices_h
#define Magnum_MeshTools_CompressIndices_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
 * @brief Function @ref Magnum::MeshTools::compressIndices()
 */

#include <tuple>

#include "Magnum/Mesh.h"
#include "Magnum/MeshTools/visibility.h"

namespace Magnum { namespace MeshTools {

/**
@brief Compress vertex indices
@param indices  Index array
@return Index range, type and compressed index array

This function takes index array and outputs them compressed to smallest
possible size. For example when your indices have maximum number 463, it's
wasteful to store them in array of 32bit integers, array of 16bit integers is
sufficient.

Example usage:
@code
std::vector<UnsignedInt> indices;

Containers::Array<char> indexData;
Mesh::IndexType indexType;
UnsignedInt indexStart, indexEnd;
std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(indices);

Buffer indexBuffer;
indexBuffer.setData(indexData, BufferUsage::StaticDraw);

Mesh mesh;
mesh.setCount(indices.size())
    .setIndexBuffer(indexBuffer, 0, indexType, indexStart, indexEnd);
@endcode
@todo Extract IndexType out of Mesh class
*/
std::tuple<Containers::Array<char>, Mesh::IndexType, UnsignedInt, UnsignedInt> MAGNUM_MESHTOOLS_EXPORT compressIndices(const std::vector<UnsignedInt>& indices);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Compress vertex indices and write them to index buffer
@param mesh     Output mesh
@param buffer   Index buffer
@param usage    Index buffer usage
@param indices  Index array

@deprecated Use general-purpose
    @ref Magnum::MeshTools::compressIndices(const std::vector<UnsignedInt>&) "compressIndices(const std::vector<UnsignedInt>&)"
    instead.

The same as @ref compressIndices(const std::vector<UnsignedInt>&), but this
function writes the output to given buffer and calls @ref Mesh::setCount() and
@ref Mesh::setIndexBuffer(), thus you don't need to do anything else for mesh
index configuration.
*/
void MAGNUM_MESHTOOLS_EXPORT compressIndices(Mesh& mesh, Buffer& buffer, BufferUsage usage, const std::vector<UnsignedInt>& indices);
#endif

}}

#endif
