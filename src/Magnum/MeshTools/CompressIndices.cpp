/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayViewStl.h>

#include "Magnum/Math/FunctionsBatch.h"

namespace Magnum { namespace MeshTools {

namespace {

template<class T, class U> inline Containers::Array<char> compress(const Containers::StridedArrayView1D<const U>& indices) {
    /* Can't use Utility::copy() here because we're copying from a larger type
       to a smaller one */
    Containers::Array<char> buffer(indices.size()*sizeof(T));
    for(std::size_t i = 0; i != indices.size(); ++i) {
        T index = static_cast<T>(indices[i]);
        std::memcpy(buffer.begin()+i*sizeof(T), &index, sizeof(T));
    }

    return buffer;
}

template<class T> std::pair<Containers::Array<char>, MeshIndexType> compressIndicesImplementation(const Containers::StridedArrayView1D<const T>& indices, const MeshIndexType atLeast) {
    const T max = Math::max(indices);
    Containers::Array<char> out;
    MeshIndexType type;
    const UnsignedInt log = Math::log(256, max);

    /* If it fits into 8 bytes and 8 bytes are allowed, pack into 8 */
    if(log == 0 && atLeast == MeshIndexType::UnsignedByte) {
        out = compress<UnsignedByte>(indices);
        type = MeshIndexType::UnsignedByte;

    /* Otherwise, if it fits into either 8 or 16 bytes and we allow either 8 or
       16, pack into 16 */
    } else if(log <= 1 && atLeast != MeshIndexType::UnsignedInt) {
        out = compress<UnsignedShort>(indices);
        type = MeshIndexType::UnsignedShort;

    /* Otherwise pack into 32 */
    } else {
        out = compress<UnsignedInt>(indices);
        type = MeshIndexType::UnsignedInt;
    }

    return {std::move(out), type};
}

}

std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const MeshIndexType atLeast) {
    return compressIndicesImplementation(indices, atLeast);
}

std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const MeshIndexType atLeast) {
    return compressIndicesImplementation(indices, atLeast);
}

std::pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const MeshIndexType atLeast) {
    return compressIndicesImplementation(indices, atLeast);
}

#ifdef MAGNUM_BUILD_DEPRECATED
std::tuple<Containers::Array<char>, MeshIndexType, UnsignedInt, UnsignedInt> compressIndices(const std::vector<UnsignedInt>& indices) {
    /** @todo Performance hint when range can be represented by smaller value? */
    const auto minmax = Math::minmax(indices);
    Containers::Array<char> data;
    MeshIndexType type;
    std::tie(data, type) = compressIndices(indices, MeshIndexType::UnsignedByte);
    return std::make_tuple(std::move(data), type, minmax.first, minmax.second);
}
#endif

template<class T> Containers::Array<T> compressIndicesAs(const std::vector<UnsignedInt>& indices) {
    #if !defined(CORRADE_NO_ASSERT) || defined(CORRADE_GRACEFUL_ASSERT)
    const auto max = Math::max(indices);
    CORRADE_ASSERT(Math::log(256, max) < sizeof(T), "MeshTools::compressIndicesAs(): type too small to represent value" << max, {});
    #endif

    Containers::Array<T> buffer(indices.size());
    for(std::size_t i = 0; i != indices.size(); ++i)
        buffer[i] = T(indices[i]);

    return buffer;
}

template Containers::Array<UnsignedByte> compressIndicesAs(const std::vector<UnsignedInt>&);
template Containers::Array<UnsignedShort> compressIndicesAs(const std::vector<UnsignedInt>&);
template Containers::Array<UnsignedInt> compressIndicesAs(const std::vector<UnsignedInt>&);

}}
