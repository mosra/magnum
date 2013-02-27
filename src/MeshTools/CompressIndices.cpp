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

#include "CompressIndices.h"

#include <cstring>
#include <algorithm>

#include "Math/Functions.h"

namespace Magnum { namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace {

template<class> constexpr Mesh::IndexType indexType();
template<> inline constexpr Mesh::IndexType indexType<UnsignedByte>() { return Mesh::IndexType::UnsignedByte; }
template<> inline constexpr Mesh::IndexType indexType<UnsignedShort>() { return Mesh::IndexType::UnsignedShort; }
template<> inline constexpr Mesh::IndexType indexType<UnsignedInt>() { return Mesh::IndexType::UnsignedInt; }

template<class T> inline std::tuple<std::size_t, Mesh::IndexType, char*> compress(const std::vector<UnsignedInt>& indices) {
    char* buffer = new char[indices.size()*sizeof(T)];
    for(std::size_t i = 0; i != indices.size(); ++i) {
        T index = static_cast<T>(indices[i]);
        std::memcpy(buffer+i*sizeof(T), &index, sizeof(T));
    }

    return std::make_tuple(indices.size(), indexType<T>(), buffer);
}

std::tuple<std::size_t, Mesh::IndexType, char*> compressIndicesInternal(const std::vector<UnsignedInt>& indices, UnsignedInt max) {
    switch(Math::log(256, max)) {
        case 0:
            return compress<UnsignedByte>(indices);
        case 1:
            return compress<UnsignedShort>(indices);
        case 2:
        case 3:
            return compress<UnsignedInt>(indices);

        default:
            CORRADE_ASSERT(false, "MeshTools::compressIndices(): no type able to index" << max << "elements.", {});
    }
}

}
#endif

std::tuple<std::size_t, Mesh::IndexType, char*> compressIndices(const std::vector<UnsignedInt>& indices) {
    return compressIndicesInternal(indices, *std::max_element(indices.begin(), indices.end()));
}

void compressIndices(Mesh* mesh, Buffer* buffer, Buffer::Usage usage, const std::vector<UnsignedInt>& indices) {
    auto minmax = std::minmax_element(indices.begin(), indices.end());

    /** @todo Performance hint when range can be represented by smaller value? */

    std::size_t indexCount;
    Mesh::IndexType indexType;
    char* data;
    std::tie(indexCount, indexType, data) = compressIndicesInternal(indices, *minmax.second);

    mesh->setIndexCount(indices.size())
        ->setIndexBuffer(buffer, 0, indexType, *minmax.first, *minmax.second);
    buffer->setData(indexCount*Mesh::indexSize(indexType), data, usage);

    delete[] data;
}

}}
