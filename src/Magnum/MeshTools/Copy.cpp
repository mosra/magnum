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

#include "Copy.h"

#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Trade/MeshData.h"
#include "Magnum/MeshTools/Implementation/remapAttributeData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_MESHTOOLS_REFERENCE
#include "Magnum/MeshTools/Reference.h"
#endif

namespace Magnum { namespace MeshTools {

Trade::MeshData reference(const Trade::MeshData& mesh) {
    /* Can't do just Trade::MeshIndexData{data.indices()} as that would discard
       implementation-specific types. And can't do
        Trade::meshIndexData{data.indexType(), view}
       because asking for index type would assert on non-indexed meshes. */
    Trade::MeshIndexData indices;
    if(mesh.isIndexed()) indices = Trade::MeshIndexData{
        mesh.indexType(),
        Containers::StridedArrayView1D<const void>{
            mesh.indexData(),
            mesh.indexData().data() + mesh.indexOffset(),
            mesh.indexCount(),
            mesh.indexStride()}};

    return Trade::MeshData{mesh.primitive(),
        {}, mesh.indexData(), indices,
        {}, mesh.vertexData(), Trade::meshAttributeDataNonOwningArray(mesh.attributeData()),
        mesh.vertexCount()};
}

Trade::MeshData mutableReference(Trade::MeshData& mesh) {
    CORRADE_ASSERT((mesh.indexDataFlags() & Trade::DataFlag::Mutable) && (mesh.vertexDataFlags() & Trade::DataFlag::Mutable),
        "MeshTools::mutableReference(): data not mutable",
        (Trade::MeshData{MeshPrimitive::Points, 0}));

    /* Can't do just Trade::MeshIndexData{data.indices()} as that would discard
       implementation-specific types. And can't do
        Trade::meshIndexData{data.indexType(), view}
       because asking for index type would assert on non-indexed meshes. */
    Trade::MeshIndexData indices;
    if(mesh.isIndexed()) indices = Trade::MeshIndexData{
        mesh.indexType(),
        Containers::StridedArrayView1D<const void>{
            mesh.indexData(),
            mesh.indexData().data() + mesh.indexOffset(),
            mesh.indexCount(),
            mesh.indexStride()}};

    return Trade::MeshData{mesh.primitive(),
        Trade::DataFlag::Mutable, mesh.mutableIndexData(), indices,
        Trade::DataFlag::Mutable, mesh.mutableVertexData(), Trade::meshAttributeDataNonOwningArray(mesh.attributeData()),
        mesh.vertexCount()};
}

Trade::MeshData copy(const Trade::MeshData& mesh) {
    return copy(reference(mesh));
}

Trade::MeshData copy(Trade::MeshData&& mesh) {
    /* Transfer index data if they're owned and mutable. This works without any
       extra effort also for non-indexed meshes. */
    Containers::Array<char> indexData;
    Trade::MeshIndexData indices;
    if(mesh.indexDataFlags() >= (Trade::DataFlag::Owned|Trade::DataFlag::Mutable)) {
        indices = Trade::MeshIndexData{mesh.indices()};
        indexData = mesh.releaseIndexData();

    /* Otherwise copy them, if the mesh is indexed. If not, the
       default-constructed instances are fine. */
    } else if(mesh.isIndexed()) {
        indexData = Containers::Array<char>{NoInit, mesh.indexData().size()};
        indices = Trade::MeshIndexData{
            mesh.indexType(),
            Containers::StridedArrayView1D<const void>{
                indexData,
                indexData.data() + mesh.indexOffset(),
                mesh.indexCount(),
                mesh.indexStride()}};
        Utility::copy(mesh.indexData(), indexData);
    }

    /* If vertex data are already owned, move them to the output. Because
       releasing them will clear vertex count, save that in advance, save also
       original vertex data view for attribute offset calculation */
    const UnsignedInt vertexCount = mesh.vertexCount();
    const Containers::ArrayView<const char> originalVertexData = mesh.vertexData();
    Containers::Array<char> vertexData;
    if(mesh.vertexDataFlags() & Trade::DataFlag::Owned) {
        vertexData = mesh.releaseVertexData();

    /* Otherwise copy them */
    } else {
        vertexData = Containers::Array<char>{NoInit, originalVertexData.size()};
        Utility::copy(originalVertexData, vertexData);
    }

    /* There's no way to know if attribute data are owned until we release
       them and check the deleter, but releasing them makes it impossible to
       use the convenience MeshData APIs, so we have to do the hard way. */
    Containers::Array<Trade::MeshAttributeData> originalAttributeData = mesh.releaseAttributeData();

    /* If the attribute data are owned *and* the vertex data weren't copied,
       we can reuse the original array in its entirety */
    Containers::Array<Trade::MeshAttributeData> attributeData;
    if(!originalAttributeData.deleter() && (mesh.vertexDataFlags() & Trade::DataFlag::Owned)) {
        attributeData = Utility::move(originalAttributeData);

    /* Otherwise we have to allocate a new one and re-route the attributes to
       a potentially different vertex array */
    /** @todo could theoretically also just modify the array in-place if it has
        a default deleter, but would need to pay attention to not copy items
        to themselves and such */
    } else {
        /* Using DefaultInit so the array has a default deleter and isn't
           problematic to use in plugins */
        attributeData = Containers::Array<Trade::MeshAttributeData>{DefaultInit, originalAttributeData.size()};
        for(std::size_t i = 0; i != originalAttributeData.size(); ++i) {
            const Trade::MeshAttributeData& originalAttribute = originalAttributeData[i];

            /* If the attribute is offset-only, copy it directly, yay! */
            if(originalAttribute.isOffsetOnly())
                attributeData[i] = originalAttribute;

            /* Otherwise remap it to the new vertex data */
            else attributeData[i] = Implementation::remapAttributeData(originalAttribute, vertexCount, originalVertexData, vertexData);
        }
    }

    return Trade::MeshData{mesh.primitive(),
        Utility::move(indexData), indices,
        Utility::move(vertexData), Utility::move(attributeData),
        vertexCount};
}

#ifdef MAGNUM_BUILD_DEPRECATED
Trade::MeshData owned(const Trade::MeshData& mesh) {
    return copy(mesh);
}

Trade::MeshData owned(Trade::MeshData&& mesh) {
    return copy(Utility::move(mesh));
}
#endif

}}
