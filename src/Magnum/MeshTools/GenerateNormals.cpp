/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include "GenerateNormals.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Triple.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/Copy.h"
#include "Magnum/MeshTools/Filter.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

void generateFlatNormalsInto(const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals) {
    CORRADE_ASSERT(positions.size() % 3 == 0,
        "MeshTools::generateFlatNormalsInto(): position count not divisible by 3", );
    CORRADE_ASSERT(normals.size() == positions.size(),
        "MeshTools::generateFlatNormalsInto(): bad output size, expected" << positions.size() << "but got" << normals.size(), );

    for(std::size_t i = 0; i != positions.size(); i += 3)
        normals[i] = normals[i + 1] = normals[i + 2] = Math::cross(
            positions[i + 2] - positions[i + 1],
            positions[i] - positions[i+1]).normalized();
}

Containers::Array<Vector3> generateFlatNormals(const Containers::StridedArrayView1D<const Vector3>& positions) {
    Containers::Array<Vector3> out{NoInit, positions.size()};
    generateFlatNormalsInto(positions, Containers::arrayView(out));
    return out;
}

namespace {

/* Common code for generateFlatNormals() and generateSmoothNormals() that preps
   the attributes to be in expected formats. Position and normal attribute ID
   returned as the second and third value. */
Containers::Triple<Trade::MeshData, UnsignedInt, UnsignedInt> generateNormalsInternal(
    #ifndef CORRADE_NO_ASSERT
    const char* const messagePrefix,
    #endif
    Trade::MeshData&& mesh, const UnsignedInt id, const Int morphTargetId, const InterleaveFlags flags
) {
    CORRADE_ASSERT(mesh.primitive() == MeshPrimitive::Triangles,
        messagePrefix << "expected a" << MeshPrimitive::Triangles << "mesh but got" << mesh.primitive(),
        Containers::triple(Trade::MeshData{MeshPrimitive::Points, 0}, 0u, 0u));

    const Containers::Optional<UnsignedInt> positionAttributeId = mesh.findAttributeId(Trade::MeshAttribute::Position, id, morphTargetId);
    #ifndef CORRADE_NO_ASSERT
    if(morphTargetId == -1) CORRADE_ASSERT(positionAttributeId,
        messagePrefix << "the mesh has no positions with index" << id,
        Containers::triple(Trade::MeshData{MeshPrimitive::Points, 0}, 0u, 0u));
    else CORRADE_ASSERT(positionAttributeId,
        messagePrefix << "the mesh has no positions with index" << id << "in morph target" << morphTargetId,
        Containers::triple(Trade::MeshData{MeshPrimitive::Points, 0}, 0u, 0u));
    #endif
    const VertexFormat positionAttributeFormat = mesh.attributeFormat(*positionAttributeId);
    CORRADE_ASSERT(!isVertexFormatImplementationSpecific(positionAttributeFormat),
        messagePrefix << "positions have an implementation-specific format" << Debug::hex << vertexFormatUnwrap(positionAttributeFormat),
        Containers::triple(Trade::MeshData{MeshPrimitive::Points, 0}, 0u, 0u));
    CORRADE_ASSERT(vertexFormatComponentCount(positionAttributeFormat) == 3,
        messagePrefix << "expected 3D positions but got" << positionAttributeFormat,
        Containers::triple(Trade::MeshData{MeshPrimitive::Points, 0}, 0u, 0u));

    /*mutable*/ Containers::Optional<UnsignedInt> normalAttributeId = mesh.findAttributeId(Trade::MeshAttribute::Normal, id, morphTargetId);
    #ifndef CORRADE_NO_ASSERT
    if(!normalAttributeId) {
        const UnsignedInt normalAttributeCount = mesh.attributeCount(Trade::MeshAttribute::Normal, morphTargetId);
        if(morphTargetId == -1) CORRADE_ASSERT(normalAttributeCount == id,
            messagePrefix << "expected at least" << id << "normal attributes but got" << normalAttributeCount,
            Containers::triple(Trade::MeshData{MeshPrimitive::Points, 0}, 0u, 0u));
        else CORRADE_ASSERT(normalAttributeCount == id,
            messagePrefix << "expected at least" << id << "normal attributes in morph target" << morphTargetId << "but got" << normalAttributeCount,
            Containers::triple(Trade::MeshData{MeshPrimitive::Points, 0}, 0u, 0u));
    }
    #endif
    const VertexFormat normalAttributeFormat = normalAttributeId ? mesh.attributeFormat(*normalAttributeId) : VertexFormat{};
    CORRADE_ASSERT(!normalAttributeId || !isVertexFormatImplementationSpecific(normalAttributeFormat),
        messagePrefix << "normals have an implementation-specific format" << Debug::hex << vertexFormatUnwrap(normalAttributeFormat),
        Containers::triple(Trade::MeshData{MeshPrimitive::Points, 0}, 0u, 0u));

    /* If needing format expansion or adding a new normal attribute, first copy
       original attributes to a mutable array so we can update it. Not using
       Utility::copy() here as the view returned by attributeData() might have
       offset-only attributes which interleave() doesn't want.

       Here we also need to preserve the original attribute order so it doesn't
       happen that e.g. normals with ID 0 become normals with ID 2 because the
       expanded attribute got put at the end after two other normals. Thus we
       indeed have to copy the whole attribute array and update parts of it
       instead of passing a mask to filterAttributes() and then appending only
       the new or modified attributes with interleave(). Which also means
       implementation-specific vertex formats cannot be preserved (as is
       asserted below), because e.g. a normal attribute that needs expansion
       can be right after an implementation-specific attribute and we don't
       know how large the implementation-specific format is to preserve it. The
       InterleaveFlag::PreserveInterleavedAttributes flag *could* help but only
       if we no longer preserved the order (so, passing a bitmask to
       filterAttributes() instead of discarding all) and added the updated
       normals at the end using interleave() (which is wrong), keeping their
       original locations as unused wasted space (which is even more wrong). */
    Trade::MeshData out{MeshPrimitive::Points, 0}; /* placeholder */
    /** @todo technically we wouldn't need to produce a mesh with positions
        expanded, could do that into a temporary array, but in that case we're
        likely expanding or adding normals as well so we wouldn't save a copy
        anyway */
    if(positionAttributeFormat != VertexFormat::Vector3 ||
       !normalAttributeId ||
       normalAttributeFormat != VertexFormat::Vector3
    ) {
        /* Add one extra item for the normal attribute if it's not in the
           original mesh. Not using arrayAppend() as that'd produce a growable
           array that we'd have to turn back to non-growable after to make this
           utility usable from plugins. */
        Containers::Array<Trade::MeshAttributeData> attributes{ValueInit, mesh.attributeCount() + (normalAttributeId ? 0u : 1u)};
        for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i) {
            CORRADE_ASSERT(!isVertexFormatImplementationSpecific(mesh.attributeFormat(i)),
                messagePrefix << "attribute" << i << "has an implementation-specific format" << Debug::hex << vertexFormatUnwrap(mesh.attributeFormat(i)),
                Containers::triple(Trade::MeshData{MeshPrimitive::Points, 0}, 0u, 0u));
            attributes[i] = mesh.attributeData(i);
        }

        /* If the position attribute isn't in a desired format, replace it with
           an empty placeholder that we'll unpack the data into */
        if(positionAttributeFormat != VertexFormat::Vector3)
            attributes[*positionAttributeId] = Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3, nullptr, 0, morphTargetId};

        /* If the normal attribute isn't there or it's in an undesirable
           format, add an empty placeholder for it also. The original data, if
           any, get discarded. */
        if(!normalAttributeId || normalAttributeFormat != VertexFormat::Vector3) {
            /* If the attribute wasn't there, it's now the last item in the
               attributes array */
            if(!normalAttributeId)
                normalAttributeId = attributes.size() - 1;
            attributes[*normalAttributeId] = Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr, 0, morphTargetId};
        }

        /* Create the output mesh, making more room for the full formats or new
           attributes. In case of flat normal generation the original mesh is
           expected to not be indexed here we'd be transferring just the mesh
           primitive and nothing else. */
        /** @todo isn't there some less silly way to take everything except the
            vertex data from the mesh?! */
        out = interleave(filterOnlyAttributes(Utility::move(mesh), Containers::ArrayView<const Trade::MeshAttribute>{}), attributes, flags);

        /* Unpack the positions if not in desired format */
        if(positionAttributeFormat != VertexFormat::Vector3)
            mesh.positions3DInto(out.mutableAttribute<Vector3>(*positionAttributeId), id, morphTargetId);

    /* Otherwise we can do just a simple copy, transferring ownership of owned
       data if there are any */
    } else out = copy(Utility::move(mesh));

    return {Utility::move(out), *positionAttributeId, *normalAttributeId};
}

}

Trade::MeshData generateFlatNormals(Trade::MeshData&& mesh, const UnsignedInt id, const Int morphTargetId, const InterleaveFlags flags) {
    CORRADE_ASSERT(!mesh.isIndexed(),
        "MeshTools::generateFlatNormals(): expected a non-indexed mesh",
        (Trade::MeshData{MeshPrimitive::Points, 0}));
    CORRADE_ASSERT(mesh.vertexCount() % 3 == 0,
        "MeshTools::generateFlatNormals(): expected vertex count divisible by 3 but got" << mesh.vertexCount(),
        (Trade::MeshData{MeshPrimitive::Points, 0}));

    Containers::Triple<Trade::MeshData, UnsignedInt, UnsignedInt> out = generateNormalsInternal(
        #ifndef CORRADE_NO_ASSERT
        "MeshTools::generateFlatNormals():",
        #endif
        Utility::move(mesh), id, morphTargetId, flags);
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* If the helper asserted, a placeholder MeshData with a clearly wrong
       primitive is returned. Bail in that case to not assert further below. */
    if(out.first().primitive() == MeshPrimitive::Points)
        return Utility::move(out).first();
    #endif

    /* Delegate to the low-level implementation and return */
    generateFlatNormalsInto(out.first().attribute<Vector3>(out.second()), out.first().mutableAttribute<Vector3>(out.third()));
    return Utility::move(out).first();
}

Trade::MeshData generateFlatNormals(const Trade::MeshData& mesh, const UnsignedInt id, const Int morphTargetId, const InterleaveFlags flags) {
    /* Pass through to the && overload, which then decides whether to reuse
       anything based on the DataFlags */
    return generateFlatNormals(reference(mesh), id, morphTargetId, flags);
}

namespace {

#if defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_MSVC_COMPATIBILITY) && _MSC_VER >= 1920 && _MSC_VER < 1930
/* When using /permissive- with MSVC2019, using namespace inside the function
   below FOR SOME REASON gets lost when instantiating the template. That's
   stupid but what can we do -- the only way to work around that is to move it
   outside the function. */
using namespace Math::Literals;
#endif

template<class T> inline void generateSmoothNormalsIntoImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals) {
    CORRADE_ASSERT(indices.size() % 3 == 0,
        "MeshTools::generateSmoothNormalsInto(): index count not divisible by 3", );
    CORRADE_ASSERT(normals.size() == positions.size(),
        "MeshTools::generateSmoothNormalsInto(): bad output size, expected" << positions.size() << "but got" << normals.size(), );

    /* Single allocation for all temporary state */
    Containers::ArrayView<UnsignedInt> triangleOffset;
    Containers::ArrayView<T> triangleIds;
    Containers::ArrayView<Containers::Pair<Vector3, Math::Vector3<Rad>>> crossAngles;
    Containers::ArrayTuple storage{
        {ValueInit, positions.size() + 1, triangleOffset},
        {NoInit, indices.size(), triangleIds},
        {NoInit, indices.size()/3, crossAngles},
    };

    /* Gather count of triangles for every vertex. Offset the indices by one
       so `triangleOffset[0]` stays 0. */
    for(const T index: indices) {
        CORRADE_ASSERT(index < positions.size(), "MeshTools::generateSmoothNormalsInto(): index" << index << "out of range for" << positions.size() << "elements", );
        ++triangleOffset[index + 1];
    }

    /* Turn the counts into a running offset array where
       `[triangleOffset[i + 1], triangleOffset[i + 2])` is the range of
       triangle IDs for vertex `i`. In other words, `triangleOffset[0]` and
       `triangleOffset[1]` (if there is any triangle at all) are now both 0. */
    UnsignedInt prevOffset = triangleOffset[0];
    for(std::size_t i = 1; i != triangleOffset.size(); ++i) {
        const UnsignedInt nextOffset = triangleOffset[i] + prevOffset;
        triangleOffset[i] = prevOffset;
        prevOffset = nextOffset;
    }
    CORRADE_INTERNAL_ASSERT(triangleOffset[0] == 0 && (triangleOffset.size() == 1 || triangleOffset[1] == 0));

    /* Gather triangle IDs for every vertex, shifting the offset array by one
       element left in the process. In other words, then `[triangleOffset[i], triangleOffset[i + 1])` is the range of triangle IDs for vertex `i`,
       with `triangleOffset[0]` being 0. */
    for(std::size_t i = 0; i != indices.size(); ++i)
        triangleIds[triangleOffset[indices[i] + 1]++] = i/3;
    CORRADE_INTERNAL_ASSERT(triangleOffset[0] == 0 && triangleOffset.back() == indices.size());

    /* Precalculate cross product and interior angles of each face --- the loop
       below would otherwise calculate it for every vertex, which is at least
       3x as much work */
    for(std::size_t i = 0; i != crossAngles.size(); ++i) {
        const Vector3 v0 = positions[indices[i*3 + 0]];
        const Vector3 v1 = positions[indices[i*3 + 1]];
        const Vector3 v2 = positions[indices[i*3 + 2]];

        /* Cross product */
        crossAngles[i].first() = Math::cross(v2 - v1, v0 - v1);

        /* If any of the vectors is zero, the normalization would result in a
           NaN and the angle calculation will assert. This happens also when
           any of the original positions is NaN. If that's the case, skip the
           rest. Given triangle will then contribute with a zero total angle,
           effectively getting ignored for normal calculation. */
        const Vector3 v10n = (v1 - v0).normalized();
        const Vector3 v20n = (v2 - v0).normalized();
        const Vector3 v21n = (v2 - v1).normalized();
        if(Math::isNan(v10n) || Math::isNan(v20n) || Math::isNan(v21n)) {
            crossAngles[i].second() = Math::Vector3<Rad>{Math::ZeroInit};
            continue;
        }

        /* Inner angle at each vertex of the triangle. The last one can be
           calculated as a remainder to 180°. */
        /* This using namespace doesn't work with MSVC2019 with /permissive-
           (it gets lost when instantiating?!), so it's duplicated above */
        using namespace Math::Literals;
        crossAngles[i].second()[0] = Math::angle(v10n, v20n);
        crossAngles[i].second()[1] = Math::angle(-v10n, v21n);
        crossAngles[i].second()[2] = Rad(180.0_degf)
            - crossAngles[i].second()[0] - crossAngles[i].second()[1];
    }

    /* For every vertex v, calculate normals from all faces it belongs to and
       average them */
    for(std::size_t v = 0; v != positions.size(); ++v) {
        /* If given vertex isn't used by any triangle, skip it altogether,
           leaving the normal data untouched */
        std::size_t t = triangleOffset[v];
        const std::size_t end = triangleOffset[v + 1];
        if(t == end)
            continue;

        /* Otherwise ensure we accumulate from zero */
        normals[v] = Vector3{Math::ZeroInit};

        /* Go through all triangles sharing this vertex */
        for(; t != end; ++t) {
            const std::size_t baseIndex = triangleIds[t]*3;
            const T v0i = indices[baseIndex + 0];
            const T v1i = indices[baseIndex + 1];
            const T v2i = indices[baseIndex + 2];

            /* Cross product is a vector in direction of the normal with length
               equal to size of the parallelogram */
            const Containers::Pair<Vector3, Math::Vector3<Rad>>& crossAngle = crossAngles[triangleIds[t]];

            /* Angle between two sides of the triangle that share vertex `v`.
               The shared vertex can be one of the three. */
            Rad angle;
            if(v == v0i)
                angle = crossAngle.second()[0];
            else if(v == v1i)
                angle = crossAngle.second()[1];
            else if(v == v2i)
                angle = crossAngle.second()[2];
            else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

            /* The normal is cross.normalized(), we need to multiply it it by
               surface area which is cross.length()/2. Since normalization is
               division by length, multiplying it by length again will be a
               no-op. Then, since all normals are divided by 2, it doesn't
               change their ratio for the final normalization so we can omit
               that as well. Finally we need to weight by the angle, and in
               that case only the ratio is important as well, so it doesn't
               matter if degrees or radians. */
            normals[v] += crossAngle.first()*Float(angle);
        }

        /* Normalize the accumulated direction */
        normals[v] = normals[v].normalized();
    }
}

}

/* If not done this way but with templates instead, C++ wouldn't be able to
   figure out on its own which overload to use when indices are not already a
   strided arrray view */
void generateSmoothNormalsInto(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals) {
    generateSmoothNormalsIntoImplementation(indices, positions, normals);
}
void generateSmoothNormalsInto(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals) {
    generateSmoothNormalsIntoImplementation(indices, positions, normals);
}
void generateSmoothNormalsInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals) {
    generateSmoothNormalsIntoImplementation(indices, positions, normals);
}

void generateSmoothNormalsInto(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals) {
    CORRADE_ASSERT(indices.isContiguous<1>(), "MeshTools::generateSmoothNormalsInto(): second index view dimension is not contiguous", );
    if(indices.size()[1] == 4)
        return generateSmoothNormalsIntoImplementation(Containers::arrayCast<1, const UnsignedInt>(indices), positions, normals);
    else if(indices.size()[1] == 2)
        return generateSmoothNormalsIntoImplementation(Containers::arrayCast<1, const UnsignedShort>(indices), positions, normals);
    else {
        CORRADE_ASSERT(indices.size()[1] == 1, "MeshTools::generateSmoothNormalsInto(): expected index type size 1, 2 or 4 but got" << indices.size()[1], );
        return generateSmoothNormalsIntoImplementation(Containers::arrayCast<1, const UnsignedByte>(indices), positions, normals);
    }
}

namespace {

template<class T> inline Containers::Array<Vector3> generateSmoothNormalsImplementation(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<const Vector3>& positions) {
    Containers::Array<Vector3> out{NoInit, positions.size()};
    generateSmoothNormalsInto(indices, positions, out);
    return out;
}

}

/* If not done this way but with templates instead, C++ wouldn't be able to
   figure out on its own which overload to use when indices are not already a
   strided arrray view */
Containers::Array<Vector3> generateSmoothNormals(const Containers::StridedArrayView1D<const UnsignedByte>& indices, const Containers::StridedArrayView1D<const Vector3>& positions) {
    return generateSmoothNormalsImplementation(indices, positions);
}
Containers::Array<Vector3> generateSmoothNormals(const Containers::StridedArrayView1D<const UnsignedShort>& indices, const Containers::StridedArrayView1D<const Vector3>& positions) {
    return generateSmoothNormalsImplementation(indices, positions);
}
Containers::Array<Vector3> generateSmoothNormals(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::StridedArrayView1D<const Vector3>& positions) {
    return generateSmoothNormalsImplementation(indices, positions);
}

Containers::Array<Vector3> generateSmoothNormals(const Containers::StridedArrayView2D<const char>& indices, const Containers::StridedArrayView1D<const Vector3>& positions) {
    Containers::Array<Vector3> out{NoInit, positions.size()};
    generateSmoothNormalsInto(indices, positions, out);
    return out;
}

Trade::MeshData generateSmoothNormals(Trade::MeshData&& mesh, const UnsignedInt id, const Int morphTargetId, const InterleaveFlags flags) {
    CORRADE_ASSERT(mesh.isIndexed(),
        "MeshTools::generateSmoothNormals(): expected an indexed mesh",
        (Trade::MeshData{MeshPrimitive::Points, 0}));
    CORRADE_ASSERT(mesh.indexCount() % 3 == 0,
        "MeshTools::generateSmoothNormals(): expected index count divisible by 3 but got" << mesh.indexCount(),
        (Trade::MeshData{MeshPrimitive::Points, 0}));
    CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(mesh.indexType()),
        "MeshTools::generateSmoothNormals(): mesh has an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(mesh.indexType()),
        (Trade::MeshData{MeshPrimitive::Points, 0}));

    Containers::Triple<Trade::MeshData, UnsignedInt, UnsignedInt> out = generateNormalsInternal(
        #ifndef CORRADE_NO_ASSERT
        "MeshTools::generateSmoothNormals():",
        #endif
        Utility::move(mesh), id, morphTargetId, flags);
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* If the helper asserted, a placeholder MeshData with a clearly wrong
       primitive is returned. Bail in that case to not assert further below. */
    if(out.first().primitive() == MeshPrimitive::Points)
        return Utility::move(out).first();
    #endif

    /* Delegate to the low-level implementation and return */
    generateSmoothNormalsInto(out.first().indices(), out.first().attribute<Vector3>(out.second()), out.first().mutableAttribute<Vector3>(out.third()));
    return Utility::move(out).first();
}

Trade::MeshData generateSmoothNormals(const Trade::MeshData& mesh, const UnsignedInt id, const Int morphTargetId, const InterleaveFlags flags) {
    /* Pass through to the && overload, which then decides whether to reuse
       anything based on the DataFlags */
    return generateSmoothNormals(reference(mesh), id, morphTargetId, flags);
}

}}
