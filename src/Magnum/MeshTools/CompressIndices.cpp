/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/MeshTools/Copy.h"
#include "Magnum/MeshTools/Implementation/remapAttributeData.h"
#include "Magnum/Trade/MeshData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <tuple>
#include <Corrade/Containers/ArrayViewStl.h>
#endif

namespace Magnum { namespace MeshTools {

namespace {

template<class T, class U> inline Containers::Array<char> compress(const Containers::StridedArrayView1D<const U>& indices, Long offset) {
    /* Can't use Math::castInto() here because we're subtracting an offset in
       addition */
    Containers::Array<char> buffer{NoInit, indices.size()*sizeof(T)};
    const Containers::ArrayView<T> view = Containers::arrayCast<T>(buffer);
    for(std::size_t i = 0; i != indices.size(); ++i)
        view[i] = indices[i] - offset;

    return buffer;
}

template<class T> Containers::Pair<Containers::Array<char>, MeshIndexType> compressIndicesImplementation(const Containers::StridedArrayView1D<const T>& indices, const MeshIndexType atLeast, const Long offset) {
    CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(atLeast),
        "MeshTools::compressIndices(): can't compress to an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(atLeast),
        (Containers::Pair<Containers::Array<char>, MeshIndexType>{nullptr, MeshIndexType::UnsignedInt}));

    const UnsignedInt max = Math::max(indices) - offset;
    Containers::Array<char> out;
    MeshIndexType type;
    const UnsignedInt log = Math::log(256, max);

    /* If it fits into 8 bytes and 8 bytes are allowed, pack into 8 */
    if(log == 0 && atLeast == MeshIndexType::UnsignedByte) {
        out = compress<UnsignedByte>(indices, offset);
        type = MeshIndexType::UnsignedByte;

    /* Otherwise, if it fits into either 8 or 16 bytes and we allow either 8 or
       16, pack into 16 */
    } else if(log <= 1 && atLeast != MeshIndexType::UnsignedInt) {
        out = compress<UnsignedShort>(indices, offset);
        type = MeshIndexType::UnsignedShort;

    /* Otherwise pack into 32 */
    } else {
        out = compress<UnsignedInt>(indices, offset);
        type = MeshIndexType::UnsignedInt;
    }

    return {Utility::move(out), type};
}

}

Containers::Pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const MeshIndexType atLeast, const Long offset) {
    return compressIndicesImplementation(indices, atLeast, offset);
}

Containers::Pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const MeshIndexType atLeast, const Long offset) {
    return compressIndicesImplementation(indices, atLeast, offset);
}

Containers::Pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const MeshIndexType atLeast, const Long offset) {
    return compressIndicesImplementation(indices, atLeast, offset);
}

Containers::Pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Long offset) {
    return compressIndicesImplementation(indices, MeshIndexType::UnsignedShort, offset);
}

Containers::Pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Long offset) {
    return compressIndicesImplementation(indices, MeshIndexType::UnsignedShort, offset);
}

Containers::Pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Long offset) {
    return compressIndicesImplementation(indices, MeshIndexType::UnsignedShort, offset);
}

Containers::Pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView2D<const char>& indices, const MeshIndexType atLeast, const Long offset) {
    CORRADE_ASSERT(indices.isContiguous<1>(), "MeshTools::compressIndices(): second view dimension is not contiguous", {});
    if(indices.size()[1] == 4)
        return compressIndicesImplementation(Containers::arrayCast<1, const UnsignedInt>(indices), atLeast, offset);
    else if(indices.size()[1] == 2)
        return compressIndicesImplementation(Containers::arrayCast<1, const UnsignedShort>(indices), atLeast, offset);
    else {
        CORRADE_ASSERT(indices.size()[1] == 1, "MeshTools::compressIndices(): expected index type size 1, 2 or 4 but got" << indices.size()[1], {});
        return compressIndicesImplementation(Containers::arrayCast<1, const UnsignedByte>(indices), atLeast, offset);
    }
}

Containers::Pair<Containers::Array<char>, MeshIndexType> compressIndices(const Containers::StridedArrayView2D<const char>& indices, const Long offset) {
    return compressIndices(indices, MeshIndexType::UnsignedShort, offset);
}

Trade::MeshData compressIndices(Trade::MeshData&& mesh, MeshIndexType atLeast) {
    CORRADE_ASSERT(mesh.isIndexed(), "MeshTools::compressIndices(): mesh data not indexed", (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    /* Transfer vertex data as-is, as those don't need any changes. Release if
       possible. */
    const Containers::ArrayView<const char> originalVertexData = mesh.vertexData();
    Containers::Array<char> vertexData;
    const UnsignedInt vertexCount = mesh.vertexCount();
    if(mesh.vertexDataFlags() & Trade::DataFlag::Owned)
        vertexData = mesh.releaseVertexData();
    else {
        vertexData = Containers::Array<char>{NoInit, mesh.vertexData().size()};
        Utility::copy(mesh.vertexData(), vertexData);
    }

    /* Compress the indices */
    UnsignedInt offset;
    Containers::Pair<Containers::Array<char>, MeshIndexType> result;
    if(mesh.indexType() == MeshIndexType::UnsignedInt) {
        auto indices = mesh.indices<UnsignedInt>();
        offset = Math::min(indices);
        result = compressIndicesImplementation<UnsignedInt>(indices, atLeast, offset);
    } else if(mesh.indexType() == MeshIndexType::UnsignedShort) {
        auto indices = mesh.indices<UnsignedShort>();
        offset = Math::min(indices);
        result = compressIndicesImplementation<UnsignedShort>(indices, atLeast, offset);
    } else {
        CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(mesh.indexType()),
            "MeshTools::compressIndices(): mesh has an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(mesh.indexType()),
            (Trade::MeshData{MeshPrimitive{}, 0}));
        CORRADE_INTERNAL_ASSERT(mesh.indexType() == MeshIndexType::UnsignedByte);
        auto indices = mesh.indices<UnsignedByte>();
        offset = Math::min(indices);
        result = compressIndicesImplementation<UnsignedByte>(indices, atLeast, offset);
    }

    /* Recreate the attribute array with each attribute being shifted by the
       offset calculated above */
    const UnsignedInt newVertexCount = vertexCount - offset;
    Containers::Array<Trade::MeshAttributeData> attributeData{mesh.attributeCount()};
    for(UnsignedInt i = 0, max = attributeData.size(); i != max; ++i) {
        attributeData[i] = Implementation::remapAttributeData(mesh.attributeData(i), newVertexCount, originalVertexData, vertexData.exceptPrefix(offset*mesh.attributeStride(i)));
    }

    Trade::MeshIndexData indices{result.second(), result.first()};
    return Trade::MeshData{mesh.primitive(), Utility::move(result.first()), indices,
        Utility::move(vertexData), Utility::move(attributeData), newVertexCount};
}

Trade::MeshData compressIndices(const Trade::MeshData& mesh, MeshIndexType atLeast) {
    /* Pass through to the && overload, which then decides whether to reuse
       anything based on the DataFlags */
    return compressIndices(reference(mesh), atLeast);
}

#ifdef MAGNUM_BUILD_DEPRECATED
std::tuple<Containers::Array<char>, MeshIndexType, UnsignedInt, UnsignedInt> compressIndices(const std::vector<UnsignedInt>& indices) {
    const auto minmax = Math::minmax(indices);
    Containers::Pair<Containers::Array<char>, MeshIndexType> dataType = compressIndices(indices, MeshIndexType::UnsignedByte);
    return std::make_tuple(Utility::move(dataType.first()), dataType.second(), minmax.first(), minmax.second());
}

template<class T> Containers::Array<T> compressIndicesAs(const std::vector<UnsignedInt>& indices) {
    #ifndef CORRADE_NO_ASSERT
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
#endif

}}
