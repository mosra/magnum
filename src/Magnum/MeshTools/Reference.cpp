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

#include "Reference.h"

#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

Trade::MeshData reference(const Trade::MeshData& data) {
    return Trade::MeshData{data.primitive(),
        {}, data.indexData(), Trade::MeshIndexData{data.indices()},
        {}, data.vertexData(), Trade::meshAttributeDataNonOwningArray(data.attributeData()),
        data.vertexCount()};
}

Trade::MeshData mutableReference(Trade::MeshData& data) {
    CORRADE_ASSERT((data.indexDataFlags() & Trade::DataFlag::Mutable) && (data.vertexDataFlags() & Trade::DataFlag::Mutable),
        "MeshTools::mutableReference(): data not mutable",
        (Trade::MeshData{MeshPrimitive::Points, 0}));

    return Trade::MeshData{data.primitive(),
        Trade::DataFlag::Mutable, data.mutableIndexData(), Trade::MeshIndexData{data.indices()},
        Trade::DataFlag::Mutable, data.mutableVertexData(), Trade::meshAttributeDataNonOwningArray(data.attributeData()),
        data.vertexCount()};
}

Trade::MeshData owned(const Trade::MeshData& data) {
    return owned(reference(data));
}

Trade::MeshData owned(Trade::MeshData&& data) {
    /** @todo copy only the actually used range instead of the whole thing? */

    /* If index data are already owned, move them to the output. This works
       without any extra effort also for non-indexed meshes. */
    Containers::Array<char> indexData;
    Trade::MeshIndexData indices;
    if(data.indexDataFlags() & Trade::DataFlag::Owned) {
        indices = Trade::MeshIndexData{data.indices()};
        indexData = data.releaseIndexData();

    /* Otherwise copy them, if the mesh is indexed. If not, the
       default-constructed instances are fine. */
    } else if(data.isIndexed()) {
        indexData = Containers::Array<char>{Containers::NoInit, data.indexData().size()};
        indices = Trade::MeshIndexData{data.indexType(), indexData.slice(data.indexOffset(), data.indexOffset() + data.indexCount()*meshIndexTypeSize(data.indexType()))};
        Utility::copy(data.indexData(), indexData);
    }

    /* If vertex data are already owned, move them to the output. Because
       releasing them will clear vertex count, save that in advance, save also
       original vertex data view for attribute offset calculation */
    const UnsignedInt vertexCount = data.vertexCount();
    const Containers::ArrayView<const char> originalVertexData = data.vertexData();
    Containers::Array<char> vertexData;
    if(data.vertexDataFlags() & Trade::DataFlag::Owned) {
        vertexData = data.releaseVertexData();

    /* Otherwise copy them */
    } else {
        vertexData = Containers::Array<char>{Containers::NoInit, originalVertexData.size()};
        Utility::copy(originalVertexData, vertexData);
    }

    /* There's no way to know if attribute data are owned until we release
       them and check the deleter, but releasing them makes it impossible to
       use the convenience MeshData APIs, so we have to do the hard way. */
    Containers::Array<Trade::MeshAttributeData> originalAttributeData = data.releaseAttributeData();

    /* If the attribute data are owned *and* the vertex data weren't copied,
       we can reuse the original array in its entirety */
    Containers::Array<Trade::MeshAttributeData> attributeData;
    if(!originalAttributeData.deleter() && (data.vertexDataFlags() & Trade::DataFlag::Owned)) {
        attributeData = std::move(originalAttributeData);

    /* Otherwise we have to allocate a new one and re-route the attributes to
       a potentially different vertex array */
    } else {
        attributeData = Containers::Array<Trade::MeshAttributeData>{originalAttributeData.size()};
        for(std::size_t i = 0; i != originalAttributeData.size(); ++i) {
            attributeData[i] = Trade::MeshAttributeData{
                originalAttributeData[i].name(),
                originalAttributeData[i].format(),
                Containers::StridedArrayView1D<const void>{
                    vertexData,
                    vertexData.data() + originalAttributeData[i].offset(originalVertexData),
                    vertexCount,
                    originalAttributeData[i].stride()},
                originalAttributeData[i].arraySize()};
        }
    }

    return Trade::MeshData{data.primitive(),
        std::move(indexData), indices,
        std::move(vertexData), std::move(attributeData),
        vertexCount};
}

}}
