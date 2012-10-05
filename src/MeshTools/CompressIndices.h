#ifndef Magnum_MeshTools_CompressIndices_h
#define Magnum_MeshTools_CompressIndices_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Function Magnum::MeshTools::compressIndices()
 */

#include <tuple>

#include "Buffer.h"
#include "TypeTraits.h"

#include "magnumMeshToolsVisibility.h"

namespace Magnum {

class IndexedMesh;

namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

class MESHTOOLS_EXPORT CompressIndices {
    public:
        CompressIndices(const std::vector<std::uint32_t>& indices): indices(indices) {}

        std::tuple<std::size_t, Type, char*> operator()() const;

        void operator()(IndexedMesh* mesh, Buffer::Usage usage) const;

    private:
        struct Compressor {
            template<class IndexType> static std::tuple<std::size_t, Type, char*> run(const std::vector<std::uint32_t>& indices);
        };

        const std::vector<std::uint32_t>& indices;
};

}
#endif

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
Type indexType;
char* data;
std::tie(indexCount, indexType, data) = MeshTools::compressIndices(indices);
std::size_t dataSize = indexCount*TypeInfo::sizeOf(indexType);
// ...
delete[] data;
@endcode

See also compressIndices(IndexedMesh*, Buffer::Usage, const std::vector<std::uint32_t>&),
which writes the compressed data directly into index buffer of given mesh.
*/
inline std::tuple<std::size_t, Type, char*> compressIndices(const std::vector<std::uint32_t>& indices) {
    return Implementation::CompressIndices{indices}();
}

/**
@brief Compress vertex indices and write them to index buffer
@param mesh     Output mesh
@param usage    Index buffer usage
@param indices  Index array

The same as compressIndices(const std::vector<std::uint32_t>&), but this
function writes the output to mesh's index buffer and updates index count and
type in the mesh accordingly, so you don't have to call Mesh::setIndexCount()
and Mesh::setIndexType() on your own.

@see MeshTools::interleave()
*/
inline void compressIndices(IndexedMesh* mesh, Buffer::Usage usage, const std::vector<std::uint32_t>& indices) {
    return Implementation::CompressIndices{indices}(mesh, usage);
}

}}

#endif
