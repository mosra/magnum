/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

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

void duplicateInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView2D<const char>& data, const Containers::StridedArrayView2D<char>& out) {
    CORRADE_ASSERT(indices.isContiguous<1>(), "MeshTools::duplicateInto(): second index view dimension is not contiguous", );
    if(indices.size()[1] == 4)
        return duplicateIntoImplementation(Containers::arrayCast<1, const UnsignedInt>(indices), data, out);
    else if(indices.size()[1] == 2)
        return duplicateIntoImplementation(Containers::arrayCast<1, const UnsignedShort>(indices), data, out);
    else {
        CORRADE_ASSERT(indices.size()[1] == 1, "MeshTools::duplicateInto(): expected index type size 1, 2 or 4 but got" << indices.size()[1], );
        return duplicateIntoImplementation(Containers::arrayCast<1, const UnsignedByte>(indices), data, out);
    }
}

Trade::MeshData duplicate(const Trade::MeshData& data, const Containers::ArrayView<const Trade::MeshAttributeData> extra) {
    CORRADE_ASSERT(data.isIndexed(), "MeshTools::duplicate(): mesh data not indexed", (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    /* Calculate the layout */
    Trade::MeshData layout = interleavedLayout(data, data.indexCount(), extra);

    /* Copy existing attributes to new locations */
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i)
        duplicateInto(data.indices(), data.attribute(i), layout.mutableAttribute(i));

    /* Mix in the extra attributes */
    UnsignedInt attributeIndex = data.attributeCount();
    for(UnsignedInt i = 0; i != extra.size(); ++i) {
        /* Padding, ignore */
        if(extra[i].format() == VertexFormat{}) continue;

        /* Asserting here even though data() has another assert since that one
           would be too confusing in this context */
        CORRADE_ASSERT(!extra[i].isOffsetOnly(),
            "MeshTools::duplicate(): extra attribute" << i << "is offset-only, which is not supported",
            (Trade::MeshData{MeshPrimitive::Triangles, 0}));

        /* Copy the attribute in, if it is non-empty, otherwise keep the
           memory uninitialized */
        if(extra[i].data()) {
            CORRADE_ASSERT(extra[i].data().size() == data.vertexCount(),
                "MeshTools::duplicate(): extra attribute" << i << "expected to have" << data.vertexCount() << "items but got" << extra[i].data().size(),
                (Trade::MeshData{MeshPrimitive::Triangles, 0}));
            const Containers::StridedArrayView2D<const char> attributeData =
                Containers::arrayCast<2, const char>(extra[i].data(),
                vertexFormatSize(extra[i].format())*Math::max(extra[i].arraySize(), UnsignedShort{1}));
            duplicateInto(data.indices(), attributeData, layout.mutableAttribute(attributeIndex));
        }

        ++attributeIndex;
    }

    return layout;
}

Trade::MeshData duplicate(const Trade::MeshData& data, std::initializer_list<Trade::MeshAttributeData> extra) {
    return duplicate(data, Containers::arrayView(extra));
}

}}
