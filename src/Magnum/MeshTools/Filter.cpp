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

#include "Filter.h"

#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Utility/BitAlgorithms.h>

#include "Magnum/MeshTools/Copy.h"
#include "Magnum/Trade/MeshData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_MESHTOOLS_FILTERATTRIBUTES /* so it doesn't warn */
#include "Magnum/MeshTools/FilterAttributes.h"
#endif

namespace Magnum { namespace MeshTools {

Trade::MeshData filterAttributes(Trade::MeshData&& mesh, const Containers::BitArrayView attributesToKeep) {
    CORRADE_ASSERT(attributesToKeep.size() == mesh.attributeCount(),
        "MeshTools::filterAttributes(): expected" << mesh.attributeCount() << "bits but got" << attributesToKeep.size(), (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    /* Copy attributes that aren't filtered away. Not using NoInit in order to
       use the default deleter and have this usable from plugins. */
    Containers::Array<Trade::MeshAttributeData> filtered{ValueInit, attributesToKeep.count()};
    Utility::copyMasked(mesh.attributeData(), attributesToKeep, filtered);

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

    /* If either the index or vertex buffer is owned, transfer the ownership to
       the returned instance, otherwise reference the original. Save vertex
       count first to use for the returned instance because releaseVertexData()
       would clear it. */
    const UnsignedInt vertexCount = mesh.vertexCount();
    if(mesh.indexDataFlags() >= Trade::DataFlag::Owned &&
       mesh.vertexDataFlags() >= Trade::DataFlag::Owned)
        return Trade::MeshData{mesh.primitive(),
            mesh.releaseIndexData(), indices,
            mesh.releaseVertexData(), Utility::move(filtered),
            vertexCount};
    else if(mesh.indexDataFlags() >= Trade::DataFlag::Owned)
        return Trade::MeshData{mesh.primitive(),
            mesh.releaseIndexData(), indices,
            {}, mesh.vertexData(), Utility::move(filtered),
            vertexCount};
    else if(mesh.vertexDataFlags() >= Trade::DataFlag::Owned)
        return Trade::MeshData{mesh.primitive(),
            {}, mesh.indexData(), indices,
            mesh.releaseVertexData(), Utility::move(filtered),
            vertexCount};
    else
        return Trade::MeshData{mesh.primitive(),
            {}, mesh.indexData(), indices,
            {}, mesh.vertexData(), Utility::move(filtered),
            mesh.vertexCount()};
}

Trade::MeshData filterAttributes(const Trade::MeshData& mesh, const Containers::BitArrayView attributesToKeep) {
    return filterAttributes(reference(mesh), attributesToKeep);
}

Trade::MeshData filterOnlyAttributes(Trade::MeshData&& mesh, const Containers::ArrayView<const Trade::MeshAttribute> attributes) {
    Containers::BitArray attributesToKeep{DirectInit, mesh.attributeCount(), false};
    for(const Trade::MeshAttribute attribute: attributes) {
        /* Can't use findAttributeId() because all instances of the attribute
           should be kept, not just the first one */
        for(UnsignedInt i = 0; i != attributesToKeep.size(); ++i) {
            if(mesh.attributeName(i) == attribute)
                attributesToKeep.set(i);
        }
    }
    return filterAttributes(Utility::move(mesh), attributesToKeep);
}

Trade::MeshData filterOnlyAttributes(const Trade::MeshData& mesh, const Containers::ArrayView<const Trade::MeshAttribute> attributes) {
    return filterOnlyAttributes(reference(mesh), attributes);
}

Trade::MeshData filterOnlyAttributes(const Trade::MeshData& mesh, std::initializer_list<Trade::MeshAttribute> attributes) {
    return filterOnlyAttributes(mesh, Containers::arrayView(attributes));
}

Trade::MeshData filterOnlyAttributes(Trade::MeshData&& mesh, std::initializer_list<Trade::MeshAttribute> attributes) {
    return filterOnlyAttributes(Utility::move(mesh), Containers::arrayView(attributes));
}

#ifdef MAGNUM_BUILD_DEPRECATED
Trade::MeshData filterOnlyAttributes(const Trade::MeshData& mesh, const Containers::ArrayView<const UnsignedInt> attributes) {
    Containers::BitArray attributesToKeep{DirectInit, mesh.attributeCount(), false};
    for(const UnsignedInt i: attributes) {
        CORRADE_ASSERT(i < mesh.attributeCount(),
            "MeshTools::filterOnlyAttributes(): index" << i << "out of range for" << mesh.attributeCount() << "attributes",
            (Trade::MeshData{MeshPrimitive{}, 0}));
        attributesToKeep.set(i);
    }

    return filterAttributes(mesh, attributesToKeep);
}

Trade::MeshData filterOnlyAttributes(const Trade::MeshData& mesh, std::initializer_list<UnsignedInt> attributes) {
    CORRADE_IGNORE_DEPRECATED_PUSH
    return filterOnlyAttributes(mesh, Containers::arrayView(attributes));
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

Trade::MeshData filterExceptAttributes(Trade::MeshData&& mesh, const Containers::ArrayView<const Trade::MeshAttribute> attributes) {
    Containers::BitArray attributesToKeep{DirectInit, mesh.attributeCount(), true};
    for(const Trade::MeshAttribute attribute: attributes) {
        /* Can't use findAttributeId() because all instances of the attribute
           should be removed, not just the first one */
        for(UnsignedInt i = 0; i != attributesToKeep.size(); ++i) {
            if(mesh.attributeName(i) == attribute)
                attributesToKeep.reset(i);
        }
    }
    return filterAttributes(Utility::move(mesh), attributesToKeep);
}

Trade::MeshData filterExceptAttributes(const Trade::MeshData& mesh, const Containers::ArrayView<const Trade::MeshAttribute> attributes) {
    return filterExceptAttributes(reference(mesh), attributes);
}

Trade::MeshData filterExceptAttributes(const Trade::MeshData& mesh, std::initializer_list<Trade::MeshAttribute> attributes) {
    return filterExceptAttributes(mesh, Containers::arrayView(attributes));
}

Trade::MeshData filterExceptAttributes(Trade::MeshData&& mesh, std::initializer_list<Trade::MeshAttribute> attributes) {
    return filterExceptAttributes(Utility::move(mesh), Containers::arrayView(attributes));
}

#ifdef MAGNUM_BUILD_DEPRECATED
Trade::MeshData filterExceptAttributes(const Trade::MeshData& mesh, const Containers::ArrayView<const UnsignedInt> attributes) {
    Containers::BitArray attributesToKeep{DirectInit, mesh.attributeCount(), true};
    for(const UnsignedInt i: attributes) {
        CORRADE_ASSERT(i < mesh.attributeCount(),
            "MeshTools::filterExceptAttributes(): index" << i << "out of range for" << mesh.attributeCount() << "attributes",
            (Trade::MeshData{MeshPrimitive{}, 0}));
        attributesToKeep.reset(i);
    }

    return filterAttributes(mesh, attributesToKeep);
}

Trade::MeshData filterExceptAttributes(const Trade::MeshData& mesh, std::initializer_list<UnsignedInt> attributes) {
    CORRADE_IGNORE_DEPRECATED_PUSH
    return filterExceptAttributes(mesh, Containers::arrayView(attributes));
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

}}
