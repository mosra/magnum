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

#include "Transform.h"

#include <Corrade/Containers/Optional.h>

#include "Magnum/MeshTools/FilterAttributes.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

Trade::MeshData transform2D(const Trade::MeshData& data, const Matrix3& transformation, const UnsignedInt id, const InterleaveFlags flags) {
    const Containers::Optional<UnsignedInt> positionAttributeId = data.findAttributeId(Trade::MeshAttribute::Position, id);
    CORRADE_ASSERT(positionAttributeId,
        "MeshTools::transform2D(): the mesh has no positions with index" << id,
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));
    const VertexFormat positionAttributeFormat = data.attributeFormat(*positionAttributeId);
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(positionAttributeFormat),
        "MeshTools::transform2D(): positions have an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(positionAttributeFormat)),
        (Trade::MeshData{MeshPrimitive::Points, 0}));
    CORRADE_ASSERT(vertexFormatComponentCount(positionAttributeFormat) == 2,
        "MeshTools::transform2D(): expected 2D positions but got" << positionAttributeFormat,
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));

    /* Copy original attributes to a mutable array so we can update the
       position attribute format, if needed. Not using Utility::copy() here as
       the view returned by attributeData() might have offset-only attributes
       which interleave() doesn't want. */
    Containers::Array<Trade::MeshAttributeData> attributes{data.attributeCount()};
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i)
        attributes[i] = data.attributeData(i);

    /* If the position attribute isn't in a desired format, replace it with an
       empty placeholder that we'll unpack the data into */
    if(positionAttributeFormat != VertexFormat::Vector2)
        attributes[*positionAttributeId] = Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector2, nullptr};

    /* Create the output mesh, making more room for the full formats if
       necessary */
    Trade::MeshData out = interleave(filterOnlyAttributes(data, Containers::ArrayView<const UnsignedInt>{}), attributes, flags);

    /* If the position attribute wasn't in a desired format, unpack it */
    if(positionAttributeFormat != VertexFormat::Vector2)
        data.positions2DInto(out.mutableAttribute<Vector2>(*positionAttributeId), id);

    /* Delegate to the in-place implementation and return */
    transform2DInPlace(out, transformation, id);
    return out;
}

Trade::MeshData transform2D(Trade::MeshData&& data, const Matrix3& transformation, const UnsignedInt id, const InterleaveFlags flags) {
    /* Perform the operation in-place, if we can transfer the ownership and
       have positions in the right format already. Explicitly checking for
       presence of the position attribute so we don't need to duplicate the
       assert here again. */
    if((data.indexDataFlags() & Trade::DataFlag::Owned) &&
       (data.vertexDataFlags() & Trade::DataFlag::Owned) &&
       data.attributeCount(Trade::MeshAttribute::Position) > id &&
       data.attributeFormat(Trade::MeshAttribute::Position, id) == VertexFormat::Vector2)
    {
        transform2DInPlace(data, transformation, id);
        return std::move(data);
    }

    /* Otherwise delegate to the function that does all the copying and format
       expansion */
    return transform2D(data, transformation, id, flags);
}

void transform2DInPlace(Trade::MeshData& data, const Matrix3& transformation, const UnsignedInt id) {
    CORRADE_ASSERT(data.vertexDataFlags() & Trade::DataFlag::Mutable,
        "MeshTools::transform2DInPlace(): vertex data not mutable", );
    const Containers::Optional<UnsignedInt> positionAttributeId = data.findAttributeId(Trade::MeshAttribute::Position, id);
    CORRADE_ASSERT(positionAttributeId,
        "MeshTools::transform2DInPlace(): the mesh has no positions with index" << id, );
    CORRADE_ASSERT(data.attributeFormat(*positionAttributeId) == VertexFormat::Vector2,
        "MeshTools::transform2DInPlace(): expected" << VertexFormat::Vector2 << "positions but got" << data.attributeFormat(*positionAttributeId), );

    /** @todo this needs a proper batch implementation */
    for(Vector2& position: data.mutableAttribute<Vector2>(*positionAttributeId))
        position = transformation.transformPoint(position);
}

Trade::MeshData transform3D(const Trade::MeshData& data, const Matrix4& transformation, const UnsignedInt id, const InterleaveFlags flags) {
    const Containers::Optional<UnsignedInt> positionAttributeId = data.findAttributeId(Trade::MeshAttribute::Position, id);
    CORRADE_ASSERT(positionAttributeId,
        "MeshTools::transform3D(): the mesh has no positions with index" << id,
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));
    const VertexFormat positionAttributeFormat = data.attributeFormat(*positionAttributeId);
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(positionAttributeFormat),
        "MeshTools::transform3D(): positions have an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(positionAttributeFormat)),
        (Trade::MeshData{MeshPrimitive::Points, 0}));
    CORRADE_ASSERT(vertexFormatComponentCount(positionAttributeFormat) == 3,
        "MeshTools::transform3D(): expected 3D positions but got" << positionAttributeFormat,
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));
    const Containers::Optional<UnsignedInt> tangentAttributeId = data.findAttributeId(Trade::MeshAttribute::Tangent, id);
    const Containers::Optional<UnsignedInt> bitangentAttributeId = data.findAttributeId(Trade::MeshAttribute::Bitangent, id);
    const Containers::Optional<UnsignedInt> normalAttributeId = data.findAttributeId(Trade::MeshAttribute::Normal, id);

    /* Copy original attributes to a mutable array so we can update the
       position attribute format, if needed. Not using Utility::copy() here as
       the view returned by attributeData() might have offset-only attributes
       which interleave() doesn't want. */
    Containers::Array<Trade::MeshAttributeData> attributes{data.attributeCount()};
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i)
        attributes[i] = data.attributeData(i);

    /* If the position/TBN attributes aren't in a desired format, replace them
       with an empty placeholder that we'll unpack the data into */
    if(positionAttributeFormat != VertexFormat::Vector3)
        attributes[*positionAttributeId] = Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr};
    VertexFormat tangentAttributeFormat{};
    VertexFormat desiredTangentVertexFormat{};
    if(tangentAttributeId) {
        tangentAttributeFormat = data.attributeFormat(*tangentAttributeId);
        CORRADE_ASSERT(!isVertexFormatImplementationSpecific(tangentAttributeFormat),
        "MeshTools::transform3D(): tangents have an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(tangentAttributeFormat)),
            (Trade::MeshData{MeshPrimitive::Points, 0}));
        desiredTangentVertexFormat = vertexFormatComponentCount(data.attributeFormat(*tangentAttributeId)) == 4 ?
        VertexFormat::Vector4 : VertexFormat::Vector3;
        if(tangentAttributeFormat != desiredTangentVertexFormat)
            attributes[*tangentAttributeId] = Trade::MeshAttributeData{Trade::MeshAttribute::Tangent, desiredTangentVertexFormat, nullptr};
    }
    VertexFormat bitangentAttributeFormat{};
    if(bitangentAttributeId) {
        bitangentAttributeFormat = data.attributeFormat(*bitangentAttributeId);
        CORRADE_ASSERT(!isVertexFormatImplementationSpecific(bitangentAttributeFormat),
        "MeshTools::transform3D(): bitangents have an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(bitangentAttributeFormat)),
            (Trade::MeshData{MeshPrimitive::Points, 0}));
        if(bitangentAttributeFormat != VertexFormat::Vector3)
            attributes[*bitangentAttributeId] = Trade::MeshAttributeData{Trade::MeshAttribute::Bitangent, VertexFormat::Vector3, nullptr};
    }
    VertexFormat normalAttributeFormat{};
    if(normalAttributeId) {
        normalAttributeFormat = data.attributeFormat(*normalAttributeId);
        CORRADE_ASSERT(!isVertexFormatImplementationSpecific(normalAttributeFormat),
        "MeshTools::transform3D(): normals have an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(normalAttributeFormat)),
            (Trade::MeshData{MeshPrimitive::Points, 0}));
        if(normalAttributeFormat != VertexFormat::Vector3)
            attributes[*normalAttributeId] = Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr};
    }

    /* Create the output mesh, making more room for the full formats if
       necessary */
    Trade::MeshData out = interleave(filterOnlyAttributes(data, Containers::ArrayView<const UnsignedInt>{}), attributes, flags);

    /* If the position/TBN attributes weren't in a desired format, unpack them */
    if(positionAttributeFormat != VertexFormat::Vector3)
        data.positions3DInto(out.mutableAttribute<Vector3>(*positionAttributeId), id);
    if(tangentAttributeId && tangentAttributeFormat != desiredTangentVertexFormat) {
        if(desiredTangentVertexFormat == VertexFormat::Vector4) {
            data.tangentsInto(out.mutableAttribute<Vector4>(*tangentAttributeId).slice(&Vector4::xyz), id);
            data.bitangentSignsInto(out.mutableAttribute<Vector4>(*tangentAttributeId).slice(&Vector4::w), id);
        } else {
            data.tangentsInto(out.mutableAttribute<Vector3>(*tangentAttributeId), id);
        }
    }
    if(bitangentAttributeId && bitangentAttributeFormat != VertexFormat::Vector3)
        data.bitangentsInto(out.mutableAttribute<Vector3>(*bitangentAttributeId), id);
    if(normalAttributeId && normalAttributeFormat != VertexFormat::Vector3)
        data.normalsInto(out.mutableAttribute<Vector3>(*normalAttributeId), id);

    /* Delegate to the in-place implementation and return */
    transform3DInPlace(out, transformation, id);
    return out;
}

Trade::MeshData transform3D(Trade::MeshData&& data, const Matrix4& transformation, const UnsignedInt id, const InterleaveFlags flags) {
    /* Perform the operation in-place, if we can transfer the ownership and
       have positions in the right format already. Explicitly checking for
       presence of the position attribute so we don't need to duplicate the
       assert here again. */
    const Containers::Optional<UnsignedInt> positionAttributeId = data.findAttributeId(Trade::MeshAttribute::Position, id);
    const Containers::Optional<UnsignedInt> tangentAttributeId = data.findAttributeId(Trade::MeshAttribute::Tangent, id);
    const Containers::Optional<UnsignedInt> bitangentAttributeId = data.findAttributeId(Trade::MeshAttribute::Bitangent, id);
    const Containers::Optional<UnsignedInt> normalAttributeId = data.findAttributeId(Trade::MeshAttribute::Normal, id);
    if((data.indexDataFlags() & Trade::DataFlag::Owned) &&
       (data.vertexDataFlags() & Trade::DataFlag::Owned) &&
       positionAttributeId &&
       data.attributeFormat(*positionAttributeId) == VertexFormat::Vector3 &&
       (!tangentAttributeId || data.attributeFormat(*tangentAttributeId) == VertexFormat::Vector3 || data.attributeFormat(*tangentAttributeId) == VertexFormat::Vector4) &&
       (!bitangentAttributeId || data.attributeFormat(*bitangentAttributeId) == VertexFormat::Vector3) &&
       (!normalAttributeId || data.attributeFormat(*normalAttributeId) == VertexFormat::Vector3))
    {
        transform3DInPlace(data, transformation, id);
        return std::move(data);
    }

    /* Otherwise delegate to the function that does all the copying and format
       expansion */
    return transform3D(data, transformation, id, flags);
}

void transform3DInPlace(Trade::MeshData& data, const Matrix4& transformation, const UnsignedInt id) {
    CORRADE_ASSERT(data.vertexDataFlags() & Trade::DataFlag::Mutable,
        "MeshTools::transform3DInPlace(): vertex data not mutable", );
    const Containers::Optional<UnsignedInt> positionAttributeId = data.findAttributeId(Trade::MeshAttribute::Position, id);
    CORRADE_ASSERT(positionAttributeId,
        "MeshTools::transform3DInPlace(): the mesh has no positions with index" << id, );
    CORRADE_ASSERT(data.attributeFormat(*positionAttributeId) == VertexFormat::Vector3,
        "MeshTools::transform3DInPlace(): expected" << VertexFormat::Vector3 << "positions but got" << data.attributeFormat(*positionAttributeId), );
    const Containers::Optional<UnsignedInt> tangentAttributeId = data.findAttributeId(Trade::MeshAttribute::Tangent, id);
    const VertexFormat tangentAttributeFormat = tangentAttributeId ? data.attributeFormat(*tangentAttributeId) : VertexFormat{};
    CORRADE_ASSERT(!tangentAttributeId || tangentAttributeFormat == VertexFormat::Vector3 || tangentAttributeFormat == VertexFormat::Vector4,
        "MeshTools::transform3DInPlace(): expected" << VertexFormat::Vector3 << "or" << VertexFormat::Vector4 << "tangents but got" << data.attributeFormat(*tangentAttributeId), );
    const Containers::Optional<UnsignedInt> bitangentAttributeId = data.findAttributeId(Trade::MeshAttribute::Bitangent, id);
    CORRADE_ASSERT(!bitangentAttributeId || data.attributeFormat(*bitangentAttributeId) == VertexFormat::Vector3,
        "MeshTools::transform3DInPlace(): expected" << VertexFormat::Vector3 << "bitangents but got" << data.attributeFormat(*bitangentAttributeId), );
    const Containers::Optional<UnsignedInt> normalAttributeId = data.findAttributeId(Trade::MeshAttribute::Normal, id);
    CORRADE_ASSERT(!normalAttributeId || data.attributeFormat(*normalAttributeId) == VertexFormat::Vector3,
        "MeshTools::transform3DInPlace(): expected" << VertexFormat::Vector3 << "normals but got" << data.attributeFormat(*normalAttributeId), );

    /** @todo this needs a proper batch implementation */
    for(Vector3& position: data.mutableAttribute<Vector3>(*positionAttributeId))
        position = transformation.transformPoint(position);

    /* If no other attributes are present, nothing to do */
    if(!tangentAttributeId && !bitangentAttributeId && !normalAttributeId)
        return;

    const Matrix3x3 normalMatrix = transformation.normalMatrix();
    if(tangentAttributeId) {
    /** @todo this needs a proper batch implementation */
        if(tangentAttributeFormat == VertexFormat::Vector3)
            for(Vector3& tangent: data.mutableAttribute<Vector3>(*tangentAttributeId))
                tangent = normalMatrix*tangent;
        else for(Vector4& tangent: data.mutableAttribute<Vector4>(*tangentAttributeId)) {
            tangent.xyz() = normalMatrix*tangent.xyz();
            /** @todo figure out the fourth component, probably has to get
                flipped when the scale changes handedness? */
        }
    }
    /** @todo this needs a proper batch implementation */
    if(bitangentAttributeId) for(Vector3& bitangent: data.mutableAttribute<Vector3>(*bitangentAttributeId))
        bitangent = normalMatrix*bitangent;
    /** @todo this needs a proper batch implementation */
    if(normalAttributeId) for(Vector3& normal: data.mutableAttribute<Vector3>(*normalAttributeId))
        normal = normalMatrix*normal;
}

Trade::MeshData transformTextureCoordinates2D(const Trade::MeshData& data, const Matrix3& transformation, const UnsignedInt id, const InterleaveFlags flags) {
    const Containers::Optional<UnsignedInt> textureCoordinateAttributeId = data.findAttributeId(Trade::MeshAttribute::TextureCoordinates, id);
    CORRADE_ASSERT(textureCoordinateAttributeId,
        "MeshTools::transformTextureCoordinates2D(): the mesh has no texture coordinates with index" << id,
        (Trade::MeshData{MeshPrimitive::Triangles, 0}));
    const VertexFormat textureCoordinateAttributeFormat = data.attributeFormat(*textureCoordinateAttributeId);
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(textureCoordinateAttributeFormat),
        "MeshTools::transformTextureCoordinates2D(): texture coordinates have an implementation-specific format" << reinterpret_cast<void*>(vertexFormatUnwrap(textureCoordinateAttributeFormat)),
        (Trade::MeshData{MeshPrimitive::Points, 0}));

    /* Copy original attributes to a mutable array so we can update the
       position attribute format, if needed. Not using Utility::copy() here as
       the view returned by attributeData() might have offset-only attributes
       which interleave() doesn't want. */
    Containers::Array<Trade::MeshAttributeData> attributes{data.attributeCount()};
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i)
        attributes[i] = data.attributeData(i);

    /* If the position attribute isn't in a desired format, replace it with an
       empty placeholder that we'll unpack the data into */
    if(textureCoordinateAttributeFormat != VertexFormat::Vector2)
        attributes[*textureCoordinateAttributeId] = Trade::MeshAttributeData{Trade::MeshAttribute::TextureCoordinates, VertexFormat::Vector2, nullptr};

    /* Create the output mesh, making more room for the full formats if
       necessary */
    Trade::MeshData out = interleave(filterOnlyAttributes(data, Containers::ArrayView<const UnsignedInt>{}), attributes, flags);

    /* If the position attribute wasn't in a desired format, unpack it */
    if(data.attributeFormat(*textureCoordinateAttributeId) != VertexFormat::Vector2)
        data.textureCoordinates2DInto(out.mutableAttribute<Vector2>(*textureCoordinateAttributeId), id);

    /* Delegate to the in-place implementation and return */
    transformTextureCoordinates2DInPlace(out, transformation, id);
    return out;
}

Trade::MeshData transformTextureCoordinates2D(Trade::MeshData&& data, const Matrix3& transformation, const UnsignedInt id, const InterleaveFlags flags) {
    /* Perform the operation in-place, if we can transfer the ownership and
       have positions in the right format already. Explicitly checking for
       presence of the position attribute so we don't need to duplicate the
       assert here again. */
    if((data.indexDataFlags() & Trade::DataFlag::Owned) &&
       (data.vertexDataFlags() & Trade::DataFlag::Owned) &&
       data.attributeCount(Trade::MeshAttribute::TextureCoordinates) > id &&
       data.attributeFormat(Trade::MeshAttribute::TextureCoordinates, id) == VertexFormat::Vector2)
    {
        transformTextureCoordinates2DInPlace(data, transformation, id);
        return std::move(data);
    }

    /* Otherwise delegate to the function that does all the copying and format
       expansion */
    return transformTextureCoordinates2D(data, transformation, id, flags);
}

void transformTextureCoordinates2DInPlace(Trade::MeshData& data, const Matrix3& transformation, const UnsignedInt id) {
    CORRADE_ASSERT(data.vertexDataFlags() & Trade::DataFlag::Mutable,
        "MeshTools::transformTextureCoordinates2DInPlace(): vertex data not mutable", );
    const Containers::Optional<UnsignedInt> textureCoordinateAttributeId = data.findAttributeId(Trade::MeshAttribute::TextureCoordinates, id);
    CORRADE_ASSERT(textureCoordinateAttributeId,
        "MeshTools::transformTextureCoordinates2DInPlace(): the mesh has no texture coordinates with index" << id, );
    CORRADE_ASSERT(data.attributeFormat(*textureCoordinateAttributeId) == VertexFormat::Vector2,
        "MeshTools::transformTextureCoordinates2DInPlace(): expected" << VertexFormat::Vector2 << "texture coordinates but got" << data.attributeFormat(*textureCoordinateAttributeId), );

    /** @todo this needs a proper batch implementation */
    for(Vector2& position: data.mutableAttribute<Vector2>(*textureCoordinateAttributeId))
        position = transformation.transformPoint(position);
}

}}
