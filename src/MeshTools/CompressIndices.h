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

#include <cstring>
#include <vector>
#include <tuple>
#include <algorithm>

#include "TypeTraits.h"
#include "SizeTraits.h"
#include "IndexedMesh.h"

namespace Magnum { namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

class CompressIndices {
    public:
        CompressIndices(const std::vector<unsigned int>& indices): indices(indices) {}

        inline std::tuple<size_t, Type, char*> operator()() const {
            #ifndef CORRADE_GCC44_COMPATIBILITY
            return SizeBasedCall<Compressor>(*std::max_element(indices.begin(), indices.end()))(indices);
            #else
            return SizeBasedCall<std::tuple<size_t, Type, char*>, Compressor>(*std::max_element(indices.begin(), indices.end()))(indices);
            #endif
        }

        void operator()(IndexedMesh* mesh, Buffer::Usage usage) const {
            size_t indexCount;
            Type indexType;
            char* data;
            std::tie(indexCount, indexType, data) = operator()();

            mesh->setIndexType(indexType);
            mesh->setIndexCount(indices.size());
            mesh->indexBuffer()->setData(indexCount*TypeInfo::sizeOf(indexType), data, usage);

            delete[] data;
        }

    private:
        struct Compressor {
            template<class IndexType> static std::tuple<size_t, Type, char*> run(const std::vector<unsigned int>& indices) {
                /* Create smallest possible version of index buffer */
                char* buffer = new char[indices.size()*sizeof(IndexType)];
                for(size_t i = 0; i != indices.size(); ++i) {
                    IndexType index = indices[i];
                    memcpy(buffer+i*sizeof(IndexType), reinterpret_cast<const char*>(&index), sizeof(IndexType));
                }

                return std::make_tuple(indices.size(), TypeTraits<IndexType>::indexType(), buffer);
            }
        };

        const std::vector<unsigned int>& indices;
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
wasteful to store them in array of `unsigned int`s, array of `unsigned short`s
is sufficient. Size of the buffer can be computed from index count and type,
as shown below. Example usage:
@code
size_t indexCount;
Type indexType;
char* data;
std::tie(indexCount, indexType, data) = MeshTools::compressIndices(indices);
size_t dataSize = indexCount*TypeInfo::sizeOf(indexType);
// ...
delete[] data;
@endcode

See also compressIndices(IndexedMesh*, Buffer::Usage, const std::vector<unsigned int>&),
which writes the compressed data directly into index buffer of given mesh.
*/
inline std::tuple<size_t, Type, char*> compressIndices(const std::vector<unsigned int>& indices) {
    return Implementation::CompressIndices{indices}();
}

/**
@brief Compress vertex indices and write them to index buffer
@param mesh     Output mesh
@param usage    Index buffer usage
@param indices  Index array

The same as compressIndices(const std::vector<unsigned int>&), but this
function writes the output to mesh's index buffer and updates index count and
type in the mesh accordingly.
*/
inline void compressIndices(IndexedMesh* mesh, Buffer::Usage usage, const std::vector<unsigned int>& indices) {
    return Implementation::CompressIndices{indices}(mesh, usage);
}

}}

#endif
