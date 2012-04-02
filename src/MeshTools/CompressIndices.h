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
 * @brief Class Magnum::MeshTools::CompressIndices, function Magnum::MeshTools::compressIndices()
 */

#include <cstring>
#include <vector>
#include <algorithm>

#include "TypeTraits.h"
#include "SizeTraits.h"
#include "IndexedMesh.h"

namespace Magnum { namespace MeshTools {

/**
@brief Index compressor implementation

See compressIndices() for full documentation.
*/
class CompressIndices {
    public:
        /**
         * @brief Compressed index array
         *
         * Size of the data buffer can be computed as follows:
         * @code
         * Result result;
         * size_t size = result.indexCount*TypeInfo::sizeOf(result.indexType);
         * @endcode
         */
        struct Result {
            size_t indexCount;  /**< @brief Index count */
            Type indexType;     /**< @brief Index data type */
            char* data;         /**< @brief Data buffer */
        };

        /**
         * @brief Constructor
         *
         * See compressIndices() for full documentation.
         */
        CompressIndices(const std::vector<unsigned int>& indices): indices(indices) {}

        /**
         * @brief Functor
         *
         * See compressIndices(const std::vector<unsigned int>&) for full
         * documentation.
         */
        inline Result operator()() const {
            return SizeBasedCall<Compressor>(*std::max_element(indices.begin(), indices.end()))(indices);
        }

        /**
         * @brief Functor
         *
         * See compressIndices(IndexedMesh*, Buffer::Usage, const
         * std::vector<unsigned int>&) for full documentation.
         */
        void operator()(IndexedMesh* mesh, Buffer::Usage usage) const {
            Result compressed = operator()();

            mesh->setIndexType(compressed.indexType);
            mesh->setIndexCount(indices.size());
            mesh->indexBuffer()->setData(compressed.indexCount*TypeInfo::sizeOf(compressed.indexType), compressed.data, usage);

            delete[] compressed.data;
        }

    private:
        struct Compressor {
            template<class IndexType> static Result run(const std::vector<unsigned int>& indices) {
                /* Create smallest possible version of index buffer */
                char* buffer = new char[indices.size()*sizeof(IndexType)];
                for(size_t i = 0; i != indices.size(); ++i) {
                    IndexType index = indices[i];
                    memcpy(buffer+i*sizeof(IndexType), reinterpret_cast<const char*>(&index), sizeof(IndexType));
                }

                return Result{indices.size(), TypeTraits<IndexType>::indexType(), buffer};
            }
        };

        const std::vector<unsigned int>& indices;
};

/**
@brief Compress vertex indices
@param indices  Index array
@return Compressed index array. Deleting the buffer is user's responsibility.

This function takes index array and outputs them compressed to smallest
possible size. For example when your indices have maximum number 463, it's
wasteful to store them in array of `unsigned int`s, array of
`unsigned short`s is sufficient.

This is convenience function supplementing direct usage of CompressIndices
class, instead of
@code
MeshTools::CompressIndices{indices}();
@endcode
you can just write
@code
MeshTools::compressIndices(indices);
@endcode
*/
inline CompressIndices::Result compressIndices(const std::vector<unsigned int>& indices) {
    return CompressIndices{indices}();
}

/**
@brief Compress vertex indices and write them to index buffer
@param indices  Index array
@param mesh     Output mesh
@param usage    Index buffer usage

The same as compressIndices(const std::vector<unsigned int>&), but this
function writes the output to mesh's index buffer and updates index count and
type in the mesh accordingly.

This is convenience function supplementing direct usage of CompressIndices
class, instead of
@code
MeshTools::CompressIndices{indices}(mesh, usage);
@endcode
you can just write
@code
MeshTools::compressIndices(mesh, usage, indices);
@endcode
*/
inline void compressIndices(IndexedMesh* mesh, Buffer::Usage usage, const std::vector<unsigned int>& indices) {
    return CompressIndices{indices}(mesh, usage);
}

}}

#endif
