#ifndef Magnum_MeshTools_CompressIndices_h
#define Magnum_MeshTools_CompressIndices_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Function Magnum::MeshTools::compressIndices()
 */

#include <tuple>

#include "Buffer.h"
#include "Mesh.h"

#include "magnumMeshToolsVisibility.h"

namespace Magnum { namespace MeshTools {

/**
@brief Compress vertex indices
@param indices  Index array
@return Index count, type and compressed index array. Deleting the array is
    user responsibility.

This function takes index array and outputs them compressed to smallest
possible size. For example when your indices have maximum number 463, it's
wasteful to store them in array of 32bit integers, array of 16bit integers is
sufficient. Size of the buffer can be computed from index count and type, as
shown below. Example usage:
@code
std::size_t indexCount;
Mesh::IndexType indexType;
char* data;
std::tie(indexCount, indexType, data) = MeshTools::compressIndices(indices);
std::size_t dataSize = indexCount*Mesh::indexSize(indexType);
// ...
delete[] data;
@endcode

See also compressIndices(Mesh*, Buffer*, Buffer::Usage, const std::vector<UnsignedInt>&),
which writes the compressed data directly into index buffer of given mesh.
*/
std::tuple<std::size_t, Mesh::IndexType, char*> MAGNUM_MESHTOOLS_EXPORT compressIndices(const std::vector<UnsignedInt>& indices);

/**
@brief Compress vertex indices and write them to index buffer
@param mesh     Output mesh
@param buffer   Index buffer
@param usage    Index buffer usage
@param indices  Index array

The same as compressIndices(const std::vector<UnsignedInt>&), but this
function writes the output to given buffer, updates index count and specifies
index buffer with proper index range in the mesh, so you don't have to call
Mesh::setIndexCount() and Mesh::setIndexBuffer() on your own.

@see MeshTools::interleave()
*/
void MAGNUM_MESHTOOLS_EXPORT compressIndices(Mesh* mesh, Buffer* buffer, Buffer::Usage usage, const std::vector<UnsignedInt>& indices);

}}

#endif
