/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "FilterAttributes.h"

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/GrowableArray.h>

#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

namespace {

bool hasAttribute(const Containers::ArrayView<const Trade::MeshAttribute> attributes, const Trade::MeshAttribute attribute) {
    for(const Trade::MeshAttribute i: attributes)
        if(i == attribute) return true;
    return false;
}

/** @todo drop this insanity in favor of a BitArray */
bool hasAttribute(const Containers::ArrayView<const UnsignedInt> attributes, const UnsignedInt attribute) {
    for(const UnsignedInt i: attributes)
        if(i == attribute) return true;
    return false;
}

}

Trade::MeshData filterOnlyAttributes(const Trade::MeshData& data, const Containers::ArrayView<const Trade::MeshAttribute> attributes) {
    /* Not asserting here for existence of attributes since that'd be another
       O(n^2) operation */
    /** @todo but that's not consistent with the ID-based variant, or maybe do
        this via a BitArray in the second loop and check that all attributes
        got referenced afterwards? */

    /* Pick just attributes from the list */
    Containers::Array<Trade::MeshAttributeData> filtered;
    arrayReserve(filtered, data.attributeCount());
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i) {
        if(hasAttribute(attributes, data.attributeName(i)))
            arrayAppend(filtered, data.attributeData(i));
    }

    /* Convert back to a default deleter to make this usable in plugins */
    arrayShrink(filtered, DefaultInit);

    /* Can't do just Trade::MeshIndexData{data.indices()} as that would discard
       implementation-specific types. And can't do
        Trade::meshIndexData{data.indexType(), view}
       because asking for index type would assert on non-indexed meshes. */
    Trade::MeshIndexData indices;
    if(data.isIndexed()) indices = Trade::MeshIndexData{
        data.indexType(),
        Containers::StridedArrayView1D<const void>{
            data.indexData(),
            data.indexData().data() + data.indexOffset(),
            data.indexCount(),
            data.indexStride()}};

    return Trade::MeshData{data.primitive(),
        {}, data.indexData(), indices,
        {}, data.vertexData(), std::move(filtered),
        data.vertexCount()};
}

Trade::MeshData filterOnlyAttributes(const Trade::MeshData& data, std::initializer_list<Trade::MeshAttribute> attributes) {
    return filterOnlyAttributes(data, Containers::arrayView(attributes));
}

Trade::MeshData filterOnlyAttributes(const Trade::MeshData& data, const Containers::ArrayView<const UnsignedInt> attributes) {
    #ifndef CORRADE_NO_ASSERT
    for(const UnsignedInt i: attributes) CORRADE_ASSERT(i < data.attributeCount(),
        "MeshTools::filterOnlyAttributes(): index" << i << "out of range for" << data.attributeCount() << "attributes",
        (Trade::MeshData{MeshPrimitive{}, 0}));
    #endif

    /* Pick just attributes from the list */
    Containers::Array<Trade::MeshAttributeData> filtered;
    arrayReserve(filtered, data.attributeCount());
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i) {
        if(hasAttribute(attributes, i))
            arrayAppend(filtered, data.attributeData(i));
    }

    /* Convert back to a default deleter to make this usable in plugins */
    arrayShrink(filtered, DefaultInit);

    /* Can't do just Trade::MeshIndexData{data.indices()} as that would discard
       implementation-specific types. And can't do
        Trade::meshIndexData{data.indexType(), view}
       because asking for index type would assert on non-indexed meshes. */
    Trade::MeshIndexData indices;
    if(data.isIndexed()) indices = Trade::MeshIndexData{
        data.indexType(),
        Containers::StridedArrayView1D<const void>{
            data.indexData(),
            data.indexData().data() + data.indexOffset(),
            data.indexCount(),
            data.indexStride()}};

    return Trade::MeshData{data.primitive(),
        {}, data.indexData(), indices,
        {}, data.vertexData(), std::move(filtered),
        data.vertexCount()};
}

Trade::MeshData filterOnlyAttributes(const Trade::MeshData& data, std::initializer_list<UnsignedInt> attributes) {
    return filterOnlyAttributes(data, Containers::arrayView(attributes));
}

Trade::MeshData filterExceptAttributes(const Trade::MeshData& data, const Containers::ArrayView<const Trade::MeshAttribute> attributes) {
    /* Not asserting here for existence of attributes since that'd be another
       O(n^2) operation */
    /** @todo but that's not consistent with the ID-based variant, or maybe do
        this via a BitArray in the second loop and check that all attributes
        got referenced afterwards? */

    /* Pick just attributes from the list */
    Containers::Array<Trade::MeshAttributeData> filtered;
    arrayReserve(filtered, data.attributeCount());
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i) {
        if(!hasAttribute(attributes, data.attributeName(i)))
            arrayAppend(filtered, data.attributeData(i));
    }

    /* Convert back to a default deleter to make this usable in plugins */
    arrayShrink(filtered, DefaultInit);

    /* Can't do just Trade::MeshIndexData{data.indices()} as that would discard
       implementation-specific types. And can't do
        Trade::meshIndexData{data.indexType(), view}
       because asking for index type would assert on non-indexed meshes. */
    Trade::MeshIndexData indices;
    if(data.isIndexed()) indices = Trade::MeshIndexData{
        data.indexType(),
        Containers::StridedArrayView1D<const void>{
            data.indexData(),
            data.indexData().data() + data.indexOffset(),
            data.indexCount(),
            data.indexStride()}};

    return Trade::MeshData{data.primitive(),
        {}, data.indexData(), indices,
        {}, data.vertexData(), std::move(filtered),
        data.vertexCount()};
}

Trade::MeshData filterExceptAttributes(const Trade::MeshData& data, std::initializer_list<Trade::MeshAttribute> attributes) {
    return filterExceptAttributes(data, Containers::arrayView(attributes));
}

Trade::MeshData filterExceptAttributes(const Trade::MeshData& data, const Containers::ArrayView<const UnsignedInt> attributes) {
    #ifndef CORRADE_NO_ASSERT
    for(const UnsignedInt i: attributes) CORRADE_ASSERT(i < data.attributeCount(),
        "MeshTools::filterExceptAttributes(): index" << i << "out of range for" << data.attributeCount() << "attributes",
        (Trade::MeshData{MeshPrimitive{}, 0}));
    #endif

    /* Pick just attributes from the list */
    Containers::Array<Trade::MeshAttributeData> filtered;
    arrayReserve(filtered, data.attributeCount());
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i) {
        if(!hasAttribute(attributes, i))
            arrayAppend(filtered, data.attributeData(i));
    }

    /* Convert back to a default deleter to make this usable in plugins */
    arrayShrink(filtered, DefaultInit);

    /* Can't do just Trade::MeshIndexData{data.indices()} as that would discard
       implementation-specific types. And can't do
        Trade::meshIndexData{data.indexType(), view}
       because asking for index type would assert on non-indexed meshes. */
    Trade::MeshIndexData indices;
    if(data.isIndexed()) indices = Trade::MeshIndexData{
        data.indexType(),
        Containers::StridedArrayView1D<const void>{
            data.indexData(),
            data.indexData().data() + data.indexOffset(),
            data.indexCount(),
            data.indexStride()}};

    return Trade::MeshData{data.primitive(),
        {}, data.indexData(), indices,
        {}, data.vertexData(), std::move(filtered),
        data.vertexCount()};
}

Trade::MeshData filterExceptAttributes(const Trade::MeshData& data, std::initializer_list<UnsignedInt> attributes) {
    return filterExceptAttributes(data, Containers::arrayView(attributes));
}

}}
