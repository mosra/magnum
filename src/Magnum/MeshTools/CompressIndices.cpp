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

#include "CompressIndices.h"

#include <cstring>
#include <algorithm>
#include <Corrade/Containers/Array.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Buffer.h"

namespace Magnum { namespace MeshTools {

namespace {

template<class> constexpr Mesh::IndexType indexType();
template<> constexpr Mesh::IndexType indexType<UnsignedByte>() { return Mesh::IndexType::UnsignedByte; }
template<> constexpr Mesh::IndexType indexType<UnsignedShort>() { return Mesh::IndexType::UnsignedShort; }
template<> constexpr Mesh::IndexType indexType<UnsignedInt>() { return Mesh::IndexType::UnsignedInt; }

template<class T> inline std::pair<Containers::Array<char>, Mesh::IndexType> compress(const std::vector<UnsignedInt>& indices) {
    Containers::Array<char> buffer(indices.size()*sizeof(T));
    for(std::size_t i = 0; i != indices.size(); ++i) {
        T index = static_cast<T>(indices[i]);
        std::memcpy(buffer.begin()+i*sizeof(T), &index, sizeof(T));
    }

    return {std::move(buffer), indexType<T>()};
}

}

std::tuple<Containers::Array<char>, Mesh::IndexType, UnsignedInt, UnsignedInt> compressIndices(const std::vector<UnsignedInt>& indices) {
    /** @todo Performance hint when range can be represented by smaller value? */
    auto minmax = std::minmax_element(indices.begin(), indices.end());
    std::pair<Containers::Array<char>, Mesh::IndexType> typeData;
    switch(Math::log(256, *minmax.second)) {
        case 0:
            typeData = compress<UnsignedByte>(indices);
            break;
        case 1:
            typeData = compress<UnsignedShort>(indices);
            break;
        case 2:
        case 3:
            typeData = compress<UnsignedInt>(indices);
            break;

        default:
            CORRADE_ASSERT(false, "MeshTools::compressIndices(): no type able to index" << *minmax.second << "elements.", {});
    }

    return std::make_tuple(std::move(typeData.first), typeData.second, *minmax.first, *minmax.second);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void compressIndices(Mesh& mesh, Buffer& buffer, BufferUsage usage, const std::vector<UnsignedInt>& indices) {
    Containers::Array<char> data;
    Mesh::IndexType type;
    UnsignedInt start, end;
    std::tie(data, type, start, end) = compressIndices(indices);

    buffer.setData(data, usage);

    mesh.setCount(indices.size())
        .setIndexBuffer(buffer, 0, type, start, end);
}
#endif

}}
