#ifndef Magnum_MeshTools_Concatenate_h
#define Magnum_MeshTools_Concatenate_h
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

/** @file
 * @brief Function @ref Magnum::MeshTools::concatenate(), @ref Magnum::MeshTools::concatenateInto()
 * @m_since{2020,06}
 */

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/Pair.h>

#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

namespace Implementation {
    MAGNUM_MESHTOOLS_EXPORT Containers::Pair<UnsignedInt, UnsignedInt> concatenateIndexVertexCount(const Containers::Iterable<const Trade::MeshData>& meshes);
    MAGNUM_MESHTOOLS_EXPORT Trade::MeshData concatenate(Containers::Array<char>&& indexData, UnsignedInt vertexCount, Containers::Array<char>&& vertexData, Containers::Array<Trade::MeshAttributeData>&& attributeData, const Containers::Iterable<const Trade::MeshData>& meshes, const char* assertPrefix);
}

/**
@brief Concatenate meshes together
@param meshes           Meshes to concatenate
@param flags            Flags to pass to @ref interleavedLayout()
@m_since{2020,06}

Returns a mesh that contains index and vertex data from all input meshes
concatenated together. Usage example:

@snippet MeshTools.cpp concatenate

Relative order of passed meshes is preserved in the resulting index and vertex
data, meaning you can directly calculate their offsets for example if it's
desirable to render or modify them separately. If any mesh is indexed, the
resulting mesh is indexed as well, with indices adjusted for vertex offsets of
particular meshes.

@snippet MeshTools.cpp concatenate-offsets

The indices, if present, are expected to not have an implementation-specific
index type. The behavior is undefined if any mesh has indices out of range for
its particular vertex count. Meshes with @ref MeshPrimitive::LineStrip,
@ref MeshPrimitive::LineLoop, @ref MeshPrimitive::TriangleStrip and
@ref MeshPrimitive::TriangleFan can't be concatenated --- use
@ref generateIndices() to turn them into @ref MeshPrimitive::Lines or
@ref MeshPrimitive::Triangles first. The @p meshes array is expected to have at
least one item.

All attributes from the first mesh are taken, expected to not have an
implementation-specific format. For each following mesh attributes present in
the first are copied, superfluous attributes ignored and missing attributes
zeroed out. Matching attributes are expected to have the same type, all meshes
are expected to have the same primitive. In case of array attributes,
attributes in subsequent meshes are expected to be arrays as well and have the
same or smaller array size. Unused components at the end are zeroed out. The
vertex data are concatenated in the same order as passed, with no duplicate
removal. Returned instance vertex and index data flags always have both
@ref Trade::DataFlag::Owned and @ref Trade::DataFlag::Mutable to guarante
mutable access to particular parts of the concatenated mesh --- for example for
applying transformations.

The data layouting is done by @ref interleavedLayout() with the @p flags
parameter propagated to it, see its documentation for detailed behavior
description.

If an index buffer is needed, @ref MeshIndexType::UnsignedInt is always used.
Call @ref compressIndices(const Trade::MeshData&, MeshIndexType) on the result
to compress it to a smaller type, if desired.
@see @ref concatenateInto(), @ref isMeshIndexTypeImplementationSpecific(),
    @ref isVertexFormatImplementationSpecific(),
    @ref SceneTools::flattenMeshHierarchy2D(),
    @ref SceneTools::flattenMeshHierarchy3D(), @ref meshtools-concatenate
*/
MAGNUM_MESHTOOLS_EXPORT Trade::MeshData concatenate(const Containers::Iterable<const Trade::MeshData>& meshes, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes);

/**
@brief Concatenate a list of meshes into a pre-existing destination, enlarging it if necessary
@tparam Allocator           Allocator to use
@param[in,out] destination  Destination mesh from which the output arrays as
    well as desired attribute layout is taken
@param[in] meshes           Meshes to concatenate
@param[in] flags            Flags to pass to @ref interleavedLayout()
@m_since{2020,06}

Compared to @ref concatenate(const Containers::Iterable<const Trade::MeshData>&, InterleaveFlags)
this function resizes existing index and vertex buffers in @p destination using
@ref Containers::arrayResize() and given @p allocator, and reuses its
atttribute data array instead of always allocating new ones. Only the attribute
layout from @p destination is used, all vertex/index data are taken from
@p meshes. Expects that @p meshes contains at least one item.
*/
template<template<class> class Allocator = Containers::ArrayAllocator> void concatenateInto(Trade::MeshData& destination, const Containers::Iterable<const Trade::MeshData>& meshes, InterleaveFlags flags = InterleaveFlag::PreserveInterleavedAttributes) {
    CORRADE_ASSERT(!meshes.isEmpty(),
        "MeshTools::concatenateInto(): no meshes passed", );
    #ifndef CORRADE_NO_ASSERT
    for(std::size_t i = 0; i != destination.attributeCount(); ++i) {
        const VertexFormat format = destination.attributeFormat(i);
        CORRADE_ASSERT(!isVertexFormatImplementationSpecific(format),
            "MeshTools::concatenateInto(): attribute" << i << "of the destination mesh has an implementation-specific format" << Debug::hex << vertexFormatUnwrap(format), );
    }
    #endif

    const Containers::Pair<UnsignedInt, UnsignedInt> indexVertexCount = Implementation::concatenateIndexVertexCount(meshes);

    Containers::Array<char> indexData;
    if(indexVertexCount.first()) {
        indexData = destination.releaseIndexData();
        /* Everything is overwritten here so we don't need to zero-out the
           memory */
        Containers::arrayResize<Allocator>(indexData, NoInit, indexVertexCount.first()*sizeof(UnsignedInt));
    }

    Containers::Array<Trade::MeshAttributeData> attributeData = Implementation::interleavedLayout(Utility::move(destination), {}, flags);
    Containers::Array<char> vertexData;
    if(!attributeData.isEmpty() && indexVertexCount.second()) {
        const UnsignedInt attributeStride = attributeData[0].stride();
        vertexData = destination.releaseVertexData();
        /* Resize to 0 and then to the desired size to zero-out whatever was
           there, otherwise attributes that are not present in `meshes` would
           be garbage */
        Containers::arrayResize<Allocator>(vertexData, 0);
        /* A cast to std::size_t is needed in order to allow sizes over 4 GB on
           64-bit */
        Containers::arrayResize<Allocator>(vertexData, ValueInit, attributeStride*std::size_t(indexVertexCount.second()));
    }

    destination = Implementation::concatenate(Utility::move(indexData), indexVertexCount.second(), Utility::move(vertexData), Utility::move(attributeData), meshes, "MeshTools::concatenateInto():");
}

}}

#endif
