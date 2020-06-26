#ifndef Magnum_MeshTools_Concatenate_h
#define Magnum_MeshTools_Concatenate_h
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

/** @file
 * @brief Function @ref Magnum::MeshTools::concatenate(), @ref Magnum::MeshTools::concatenateInto()
 * @m_since{2020,06}
 */

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Reference.h>

#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

namespace Implementation {
    MAGNUM_MESHTOOLS_EXPORT std::pair<UnsignedInt, UnsignedInt> concatenateIndexVertexCount(Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> meshes);
    MAGNUM_MESHTOOLS_EXPORT Trade::MeshData concatenate(Containers::Array<char>&& indexData, UnsignedInt vertexCount, Containers::Array<char>&& vertexData, Containers::Array<Trade::MeshAttributeData>&& attributeData, Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> meshes, const char* assertPrefix);
}

/**
@brief Concatenate meshes together
@m_since{2020,06}

The returned mesh contains vertices from all meshes concatenated together. If
any mesh is indexed, the resulting mesh is indexed as well, with indices
adjusted for vertex offsets of particular meshes. The behavior is undefined if
any mesh has indices out of bounds for its particular vertex count. Meshes with
@ref MeshPrimitive::LineStrip, @ref MeshPrimitive::LineLoop,
@ref MeshPrimitive::TriangleStrip and @ref MeshPrimitive::TriangleFan can't be
concatenated --- use @ref generateIndices() to turn them into
@ref MeshPrimitive::Lines or @ref MeshPrimitive::Triangles first. The @p meshes
array is expected to have at least one item.

All attributes from the first mesh are taken; for each following mesh
attributes present in the first are copied, superfluous attributes ignored and
missing attributes zeroed out. Matching attributes are expected to have the
same type, all meshes are expected to have the same primitive. The vertex data
are concatenated in the same order as passed, with no duplicate removal.
Returned instance vertex and index data flags always have both
@ref Trade::DataFlag::Owned and @ref Trade::DataFlag::Mutable to guarante
mutable access to particular parts of the concatenated mesh --- for example for
applying transformations.

If an index buffer is needed, @ref MeshIndexType::UnsignedInt is always used.
Call @ref compressIndices(const Trade::MeshData&, MeshIndexType) on the result
to compress it to a smaller type, if desired.
@see @ref concatenateInto()
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData concatenate(Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> meshes);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData concatenate(std::initializer_list<Containers::Reference<const Trade::MeshData>> meshes);

/**
@brief Concatenate a list of meshes into a pre-existing destination, enlarging it if necessary
@tparam Allocator           Allocator to use
@param[in,out] destination  Destination mesh from which the output arrays as
    well as desired attribute layout is taken
@param[in] meshes           Meshes to concatenate
@m_since{2020,06}

Compared to @ref concatenate(Containers::ArrayView<const Containers::Reference<const Trade::MeshData>>)
this function resizes existing index and vertex buffers in @p destination using
@ref Containers::arrayResize() and given @p allocator, and reuses its
atttribute data array instead of always allocating new ones. Only the attribute
layout from @p destination is used, all vertex/index data are taken from
@p meshes. Expects that @p meshes contains at least one item.
*/
template<template<class> class Allocator = Containers::ArrayAllocator> void concatenateInto(Trade::MeshData& destination, const Containers::ArrayView<const Containers::Reference<const Trade::MeshData>> meshes) {
    CORRADE_ASSERT(!meshes.empty(),
        "MeshTools::concatenateInto(): no meshes passed", );

    std::pair<UnsignedInt, UnsignedInt> indexVertexCount = Implementation::concatenateIndexVertexCount(meshes);

    Containers::Array<char> indexData;
    if(indexVertexCount.first) {
        indexData = destination.releaseIndexData();
        /* Everything is overwritten here so we don't need to zero-out the
           memory */
        Containers::arrayResize<Allocator>(indexData, Containers::NoInit, indexVertexCount.first*sizeof(UnsignedInt));
    }

    Containers::Array<Trade::MeshAttributeData> attributeData = Implementation::interleavedLayout(std::move(destination), {});
    Containers::Array<char> vertexData;
    if(!attributeData.empty() && indexVertexCount.second) {
        const UnsignedInt attributeStride = attributeData[0].stride();
        vertexData = destination.releaseVertexData();
        /* Resize to 0 and then to the desired size to zero-out whatever was
           there, otherwise attributes that are not present in `meshes` would
           be garbage */
        Containers::arrayResize<Allocator>(vertexData, 0);
        Containers::arrayResize<Allocator>(vertexData, Containers::ValueInit, attributeStride*indexVertexCount.second);
    }

    destination = Implementation::concatenate(std::move(indexData), indexVertexCount.second, std::move(vertexData), std::move(attributeData), meshes, "MeshTools::concatenateInto():");
}

/**
 * @overload
 * @m_since{2020,06}
 */
template<template<class> class Allocator = Containers::ArrayAllocator> void concatenateInto(Trade::MeshData& destination, const std::initializer_list<Containers::Reference<const Trade::MeshData>> meshes) {
    concatenateInto<Allocator>(destination, Containers::arrayView(meshes));
}

}}

#endif
