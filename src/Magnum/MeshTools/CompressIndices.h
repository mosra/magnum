#ifndef Magnum_MeshTools_CompressIndices_h
#define Magnum_MeshTools_CompressIndices_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

@see @ref compressIndicesAs()
@todo Extract IndexType out of Mesh class
*/
std::tuple<Containers::Array<char>, Mesh::IndexType, UnsignedInt, UnsignedInt> MAGNUM_MESHTOOLS_EXPORT compressIndices(const std::vector<UnsignedInt>& indices);

/**
@brief Compress vertex indices as given type

The type can be either @ref Magnum::UnsignedByte "UnsignedByte",
@ref Magnum::UnsignedShort "UnsignedShort" or @ref Magnum::UnsignedInt "UnsignedInt".
Values in the index array are expected to be representable with given type.

Example usage:
@code
std::vector<UnsignedInt> indices;
Containers::Array<UnsignedShort> indexData = MeshTools::compressIndicesAs<UnsignedShort>(indices);
@endcode

@see @ref compressIndices()
*/
template<class T> MAGNUM_MESHTOOLS_EXPORT Containers::Array<T> compressIndicesAs(const std::vector<UnsignedInt>& indices);

#if defined(CORRADE_TARGET_WINDOWS) && !defined(__MINGW32__)
extern template MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedByte> compressIndicesAs<UnsignedByte>(const std::vector<UnsignedInt>& indices);
extern template MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedShort> compressIndicesAs<UnsignedShort>(const std::vector<UnsignedInt>& indices);
extern template MAGNUM_MESHTOOLS_EXPORT Containers::Array<UnsignedInt> compressIndicesAs<UnsignedInt>(const std::vector<UnsignedInt>& indices);
#endif

}}

#endif
