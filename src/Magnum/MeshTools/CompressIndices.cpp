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

template<class T> inline Containers::Array<char> compress(const std::vector<UnsignedInt>& indices) {
    Containers::Array<char> buffer(indices.size()*sizeof(T));
    for(std::size_t i = 0; i != indices.size(); ++i) {
        T index = static_cast<T>(indices[i]);
        std::memcpy(buffer.begin()+i*sizeof(T), &index, sizeof(T));
    }

    return buffer;
}

}

std::tuple<Containers::Array<char>, MeshIndexType, UnsignedInt, UnsignedInt> compressIndices(const std::vector<UnsignedInt>& indices) {
    /** @todo Performance hint when range can be represented by smaller value? */
    const auto minmax = Math::minmax<UnsignedInt>(indices);
    Containers::Array<char> data;
    MeshIndexType type;
    switch(Math::log(256, minmax.second)) {
        case 0:
            data = compress<UnsignedByte>(indices);
            type = MeshIndexType::UnsignedByte;
            break;
        case 1:
            data = compress<UnsignedShort>(indices);
            type = MeshIndexType::UnsignedShort;
            break;
        case 2:
        case 3:
            data = compress<UnsignedInt>(indices);
            type = MeshIndexType::UnsignedInt;
            break;

        default:
            CORRADE_ASSERT(false, "MeshTools::compressIndices(): no type able to index" << minmax.second << "elements.", {}); /* LCOV_EXCL_LINE */
    }

    return std::make_tuple(std::move(data), type, minmax.first, minmax.second);
}

template<class T> Containers::Array<T> compressIndicesAs(const std::vector<UnsignedInt>& indices) {
    #if !defined(CORRADE_NO_ASSERT) || defined(CORRADE_GRACEFUL_ASSERT)
    const auto max = Math::max<UnsignedInt>(indices);
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
