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

#include "Duplicate.h"

#include <cstring>

namespace Magnum { namespace MeshTools {

namespace {

template<class T> inline void duplicateIntoImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView2D<char>& out) {
    CORRADE_ASSERT(out.size()[0] == indices.size(),
        "MeshTools::duplicateInto(): index array and output size don't match, expected" << indices.size() << "but got" << out.size()[0], );
    CORRADE_ASSERT(data.isContiguous<1>() && out.isContiguous<1>(),
        "MeshTools::duplicateInto(): second view dimension is not contiguous", );
    CORRADE_ASSERT(data.size()[1] == out.size()[1],
        "MeshTools::duplicateInto(): input and output type size doesn't match, expected" << data.size()[1] << "but got" << out.size()[1], );
    const std::size_t size = data.size()[1];
    for(std::size_t i = 0; i != indices.size(); ++i) {
        const std::size_t index = indices[i];
        CORRADE_ASSERT(index < data.size()[0], "MeshTools::duplicateInto(): index" << index << "out of bounds for" << data.size()[0] << "elements", );
        std::memcpy(out[i].data(), data[index].data(), size);
    }
}

}

/* If not done this way but with templates instead, C++ wouldn't be able to
   figure out on its own which overload to use when indices are not already a
   strided arrray view */
void duplicateInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView2D<char>& out) {
    duplicateIntoImplementation(indices, data, out);
}
void duplicateInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView2D<char>& out) {
    duplicateIntoImplementation(indices, data, out);
}
void duplicateInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView2D<char>& out) {
    duplicateIntoImplementation(indices, data, out);
}

}}
