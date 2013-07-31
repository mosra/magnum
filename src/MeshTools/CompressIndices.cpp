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

#include "CompressIndices.h"

#include <cstring>
#include <algorithm>

#include "Math/Functions.h"

namespace Magnum { namespace MeshTools {

namespace {

template<class> constexpr Mesh::IndexType indexType();
template<> constexpr Mesh::IndexType indexType<UnsignedByte>() { return Mesh::IndexType::UnsignedByte; }
template<> constexpr Mesh::IndexType indexType<UnsignedShort>() { return Mesh::IndexType::UnsignedShort; }
template<> constexpr Mesh::IndexType indexType<UnsignedInt>() { return Mesh::IndexType::UnsignedInt; }

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
        .setIndexBuffer(buffer, 0, indexType, *minmax.first, *minmax.second);
    buffer->setData(indexCount*Mesh::indexSize(indexType), data, usage);

    delete[] data;
}

}}
