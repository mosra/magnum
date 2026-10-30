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

#include "FlipNormals.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>
#endif
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/Copy.h"
#include "Magnum/MeshTools/Filter.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

void flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>& normals) {
    for(Vector3& normal: normals)
        normal = -normal;
}

#ifdef MAGNUM_BUILD_DEPRECATED
void flipNormals(std::vector<Vector3>& normals) {
    flipNormalsInPlace(normals);
}
#endif

namespace {

template<class T> inline void flipFaceWindingInPlaceImplementation(const Containers::StridedArrayView1D<T>& indices) {
    CORRADE_ASSERT(indices.size() % 3 == 0,
        "MeshTools::flipFaceWindingInPlace(): expected index count to be divisible by 3 but got" << indices.size(), );

    for(std::size_t i = 0; i != indices.size(); i += 3)
        Utility::swap(indices[i + 1], indices[i + 2]);
}

}

void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices) {
    flipFaceWindingInPlaceImplementation(indices);
}

void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices) {
    flipFaceWindingInPlaceImplementation(indices);
}

void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices) {
    flipFaceWindingInPlaceImplementation(indices);
}

void flipFaceWindingInPlace(const Containers::StridedArrayView2D<char>& indices) {
    CORRADE_ASSERT(indices.isContiguous<1>(),
        "MeshTools::flipFaceWindingInPlace(): second index view dimension is not contiguous", );

    const std::size_t typeSize = indices.size()[1];
    if(typeSize == 4)
        return flipFaceWindingInPlaceImplementation(Containers::arrayCast<1, UnsignedInt>(indices));
    if(typeSize == 2)
        return flipFaceWindingInPlaceImplementation(Containers::arrayCast<1, UnsignedShort>(indices));
    if(typeSize == 1)
        return flipFaceWindingInPlaceImplementation(Containers::arrayCast<1, UnsignedByte>(indices));

    const std::size_t indexCount = indices.size()[0];
    CORRADE_ASSERT(indexCount % 3 == 0,
        "MeshTools::flipFaceWindingInPlace(): expected index count to be divisible by 3 but got" << indexCount, );

    /* Especially on debug builds this isn't really the peak achievable
       performance (could take advantage of the contiguous second dimension,
       operate on raw pointers...) but is better than not being able to do this
       at all */
    for(std::size_t i = 0; i != indexCount; i += 3)
        for(std::size_t j = 0; j != typeSize; ++j)
            Utility::swap(indices[{i + 1, j}], indices[{i + 2, j}]);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void flipFaceWinding(std::vector<UnsignedInt>& indices) {
    flipFaceWindingInPlace(indices);
}
#endif

Trade::MeshData flipFaceWinding(Trade::MeshData&& mesh) {
    /* Make a copy of the mesh, transferring ownership of owned data if there
       are any */
    Trade::MeshData out = copy(Utility::move(mesh));

    /* Delegate to the in-place implementation (which also handles all
       assertions) and return */
    flipFaceWindingInPlace(out);
    return out;
}

Trade::MeshData flipFaceWinding(const Trade::MeshData& mesh) {
    /* Pass through to the && overload, which then decides whether to reuse
       anything based on the DataFlags */
    return flipFaceWinding(reference(mesh));
}

void flipFaceWindingInPlace(Trade::MeshData& mesh) {
    CORRADE_ASSERT(mesh.primitive() == MeshPrimitive::Triangles,
        "MeshTools::flipFaceWindingInPlace(): expected a" << MeshPrimitive::Triangles << "mesh but got" << mesh.primitive(), );

    /* For an indexed mesh we flip winding in the index array */
    if(mesh.isIndexed()) {
        CORRADE_ASSERT(mesh.indexCount() % 3 == 0,
            "MeshTools::flipFaceWindingInPlace(): expected index count divisible by 3 but got" << mesh.indexCount(), );
        CORRADE_ASSERT(mesh.indexDataFlags() >= Trade::DataFlag::Mutable,
            "MeshTools::flipFaceWindingInPlace(): index data not mutable", );
        CORRADE_ASSERT(!isMeshIndexTypeImplementationSpecific(mesh.indexType()),
            "MeshTools::flipFaceWindingInPlace(): mesh has an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(mesh.indexType()),
            );
        flipFaceWindingInPlace(mesh.mutableIndices());

    /* Otherwise we flip attributes one by one */
    } else {
        CORRADE_ASSERT(mesh.vertexCount() % 3 == 0,
            "MeshTools::flipFaceWindingInPlace(): expected vertex count divisible by 3 but got" << mesh.vertexCount(), );
        CORRADE_ASSERT(mesh.vertexDataFlags() >= Trade::DataFlag::Mutable,
            "MeshTools::flipFaceWindingInPlace(): vertex data not mutable", );
        for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i) {
            CORRADE_ASSERT(!isVertexFormatImplementationSpecific(mesh.attributeFormat(i)),
                "MeshTools::flipFaceWindingInPlace(): attribute" << i << "has an implementation-specific format" << Debug::hex << vertexFormatUnwrap(mesh.attributeFormat(i)), );
            /** @todo if the mesh is interleaved, it could be faster to flip
                the whole interleaved stride at once */
            flipFaceWindingInPlace(mesh.mutableAttribute(i));
        }
    }
}

#ifdef MAGNUM_BUILD_DEPRECATED
void flipNormals(std::vector<UnsignedInt>& indices, std::vector<Vector3>& normals) {
    flipNormalsInPlace(indices, normals);
}
#endif

Trade::MeshData flipNormals(Trade::MeshData&& mesh, const InterleaveFlags flags) {
    /* First check if there are any normal attributes that need format
       expansion. Not checking for implementation-specific formats here --- if
       there aren't any normals and the mesh is indexed we wouldn't need to
       touch the vertex data at all, so impl-spec formats don't matter there */
    bool needFormatExpansion = false;
    for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i) {
        if(mesh.attributeName(i) == Trade::MeshAttribute::Normal &&
           mesh.attributeFormat(i) != VertexFormat::Vector3) {
            needFormatExpansion = true;
            break;
        }
    }

    /* Here we need to do the same check as flipFaceWindingInPlace() does as we
       could otherwise blow up early inside interleave() with a rather cryptic
       message */
    CORRADE_ASSERT(!mesh.isIndexed() || !isMeshIndexTypeImplementationSpecific(mesh.indexType()),
        "MeshTools::flipNormals(): mesh has an implementation-specific index type" << Debug::hex << meshIndexTypeUnwrap(mesh.indexType()),
        (Trade::MeshData{MeshPrimitive::Points, 0}));
    /* Compared to flipFaceWindingInPlace(), the attributes have to be checked
       not just if the mesh is not indexed but also if we need format expansion
       as it'd again blow up inside interleave() otherwise. This means we also
       don't need to additionally check that the normal attributes don't have
       an implementation-specific format in order to expand them. */
    #ifndef CORRADE_NO_ASSERT
    if(!mesh.isIndexed() || needFormatExpansion) {
        for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i) {
            CORRADE_ASSERT(!isVertexFormatImplementationSpecific(mesh.attributeFormat(i)),
                "MeshTools::flipNormals(): attribute" << i << "has an implementation-specific format" << Debug::hex << vertexFormatUnwrap(mesh.attributeFormat(i)),
                (Trade::MeshData{MeshPrimitive::Points, 0}));
        }
    }
    #endif

    /* If need format expansion, first copy original attributes to a mutable
       array so we can update the normal attribute formats. Not using
       Utility::copy() here as the view returned by attributeData() might have
       offset-only attributes which interleave() doesn't want. */
    Trade::MeshData out{MeshPrimitive::Points, 0}; /* placeholder */
    if(needFormatExpansion) {
        Containers::Array<Trade::MeshAttributeData> attributes{ValueInit, mesh.attributeCount()};
        for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i)
            attributes[i] = mesh.attributeData(i);

        /* Look for normal attributes. If any of them isn't in a desired
           format, replace it with an empty placeholder that we'll unpack the
           data into. */
        for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i) {
            if(mesh.attributeName(i) == Trade::MeshAttribute::Normal &&
               mesh.attributeFormat(i) != VertexFormat::Vector3)
                attributes[i] = Trade::MeshAttributeData{Trade::MeshAttribute::Normal, VertexFormat::Vector3, nullptr, 0, mesh.attributeMorphTargetId(i)};
        }

        /* Create the output mesh, making more room for the full formats,
           transferring ownership of owned index data if there are */
        /** @todo isn't there some less silly way to take just the indices from
            the mesh?! */
        out = interleave(filterOnlyAttributes(Utility::move(mesh), Containers::ArrayView<const Trade::MeshAttribute>{}), attributes, flags);

        /* Go through all attributes again and unpack normal attributes that
           aren't in the desired format */
        for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i) {
            if(mesh.attributeName(i) == Trade::MeshAttribute::Normal &&
               mesh.attributeFormat(i) != VertexFormat::Vector3)
                mesh.normalsInto(out.mutableAttribute<Vector3>(i), mesh.attributeId(i), mesh.attributeMorphTargetId(i));
        }

    /* Otherwise we can do just a simple copy, transferring ownership of owned
       data if there are any */
    } else out = copy(Utility::move(mesh));

    /* Delegate to the in-place implementation and return */
    flipNormalsInPlace(out);
    return out;
}

Trade::MeshData flipNormals(const Trade::MeshData& mesh, const InterleaveFlags flags) {
    /* Pass through to the && overload, which then decides whether to reuse
       anything based on the DataFlags */
    return flipNormals(reference(mesh), flags);
}

void flipNormalsInPlace(Trade::MeshData& mesh) {
    /* Flip all normal attributes if there are any. Iterating over all
       attributes because the `mesh.attribute(MeshAttribute::Normal)` is for a
       concrete morph target ID, not all. */
    for(UnsignedInt i = 0; i != mesh.attributeCount(); ++i) {
        if(mesh.attributeName(i) != Trade::MeshAttribute::Normal)
            continue;

        /* This assert makes sense only for the first found normal attribute
           but (1) meshes with multiple normal attributes are rare so doing the
           same check multiple times is fine, and (2) the code is simpler this
           way */
        CORRADE_ASSERT(mesh.vertexDataFlags() >= Trade::DataFlag::Mutable,
            "MeshTools::flipNormalsInPlace(): vertex data not mutable", );
        CORRADE_ASSERT(mesh.attributeFormat(i) == VertexFormat::Vector3,
            "MeshTools::flipNormalsInPlace(): expected" << VertexFormat::Vector3 << "normals but got" << mesh.attributeFormat(i) << "for attribute" << i, );
        flipNormalsInPlace(mesh.mutableAttribute<Vector3>(i));
    }

    /* Flip face winding as well. The assertions done there might or might not
       overlap with what we need to check here so the order doesn't matter, but
       this way we get out asserts first which is less confusing for the
       user. */
    flipFaceWindingInPlace(mesh);
}

}}
