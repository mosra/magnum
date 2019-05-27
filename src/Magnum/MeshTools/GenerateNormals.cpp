/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Math/Vector3.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <vector>

#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#endif

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
    Containers::Array<Vector3> out{Containers::NoInit, positions.size()};
    generateFlatNormalsInto(positions, Containers::arrayView(out));
    return out;
}

#ifdef MAGNUM_BUILD_DEPRECATED
/* Original implementation kept verbatim as I can't be bothered rewriting it
   using the new APIs (the original test is kept as well) */
std::pair<std::vector<UnsignedInt>, std::vector<Vector3>> generateFlatNormals(const std::vector<UnsignedInt>& indices, const std::vector<Vector3>& positions) {
    CORRADE_ASSERT(!(indices.size()%3), "MeshTools::generateFlatNormals(): index count is not divisible by 3!", {});

    /* Create normal for every triangle (assuming counterclockwise winding) */
    std::vector<UnsignedInt> normalIndices;
    normalIndices.reserve(indices.size());
    std::vector<Vector3> normals;
    normals.reserve(indices.size()/3);
    for(std::size_t i = 0; i != indices.size(); i += 3) {
        const Vector3 normal = Math::cross(positions[indices[i+2]]-positions[indices[i+1]],
                                           positions[indices[i]]-positions[indices[i+1]]).normalized();

        /* Use the same normal for all three vertices of the face */
        normalIndices.push_back(normals.size());
        normalIndices.push_back(normals.size());
        normalIndices.push_back(normals.size());
        normals.push_back(normal);
    }

    /* Remove duplicate normals and return */
    normalIndices = MeshTools::duplicate(normalIndices, MeshTools::removeDuplicates(normals));
    return {std::move(normalIndices), std::move(normals)};
}
#endif

template<class T> void generateSmoothNormalsInto(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<const Vector3>& positions, const Containers::StridedArrayView1D<Vector3>& normals) {
    CORRADE_ASSERT(indices.size() % 3 == 0,
        "MeshTools::generateSmoothNormalsInto(): index count not divisible by 3", );
    CORRADE_ASSERT(normals.size() == positions.size(),
        "MeshTools::generateSmoothNormalsInto(): bad output size, expected" << positions.size() << "but got" << normals.size(), );

    if(indices.empty()) return;

    /* Gather count of triangles for every vertex. This abuses the output
       storage to avoid extra allocations, zero-initialize it first to avoid
       random memory getting used. */
    Containers::StridedArrayView1D<UnsignedInt> triangleCount =
        Containers::arrayCast<UnsignedInt>(normals);
    for(UnsignedInt& i: triangleCount) i = 0;
    for(const T index: indices) {
        CORRADE_ASSERT(index < positions.size(), "MeshTools::generateSmoothNormals(): index" << index << "out of bounds for" << positions.size() << "elements", );
        ++triangleCount[index];
    }

    /* Turn that into a running offset array:
       triangleOffset[i + 1] - triangleOffset[i] is triangle count for vertex i
       triangleOffset[i] is offset into an triangle ID array for vertex i */
    Containers::Array<UnsignedInt> triangleOffset{Containers::NoInit, positions.size() + 1};
    triangleOffset[0] = 0;
    for(std::size_t i = 0; i != triangleCount.size(); ++i)
        triangleOffset[i + 1] = triangleOffset[i] + triangleCount[i];

    CORRADE_INTERNAL_ASSERT(triangleOffset.back() == indices.size());

    /* Gather triangle IDs for every vertex. For vertex i,
       triangleIds[triangleOffset[i]] until triangleIds[triangleOffset[i + 1]]
       contains IDs of triangles that contain it. */
    Containers::Array<T> triangleIds{Containers::NoInit, indices.size()};
    for(std::size_t i = 0; i != indices.size(); ++i) {
        const T triangleId = i/3;
        const T vertexId = indices[i];

        /* How many triangle IDs is still left to be written, which also means
           the offset where we put the ID. Decrement that for the next run. */
        const std::size_t triangleIdsLeftForVertex = triangleCount[vertexId]--;
        triangleIds[triangleOffset[vertexId + 1] - triangleIdsLeftForVertex] = triangleId;
    }

    /* Now, triangleCount should be all zeros, we don't need it anymore and the
       underlying `normals` array is ready to get filled with real output. */

    /* For every vertex v, calculate normals from all faces it belongs to and
       average them */
    for(std::size_t v = 0; v != positions.size(); ++v) {
        /* normals are an external memory, ensure we accumulate from zero */
        normals[v] = Vector3{Math::ZeroInit};

        /* Go through all triangles sharing this vertex */
        for(std::size_t t = triangleOffset[v]; t != triangleOffset[v + 1]; ++t) {
            const std::size_t baseIndex = triangleIds[t]*3;
            const T v0i = indices[baseIndex + 0];
            const T v1i = indices[baseIndex + 1];
            const T v2i = indices[baseIndex + 2];

            /* Cross product is a vector in direction of the normal with length
               equal to size of the parallelogram */
            const Vector3 cross = Math::cross(positions[v2i] - positions[v1i],
                                              positions[v0i] - positions[v1i]);

            /* Angle between two sides of the triangle that share vertex `v`.
               The shared vertex can be one of the three, so three ways to
               calculate the angle */
            Vector3 a{Math::NoInit}, b{Math::NoInit};
            if(v == v0i) {
                a = positions[v1i] - positions[v0i];
                b = positions[v2i] - positions[v0i];
            } else if(v == v1i) {
                a = positions[v0i] - positions[v1i];
                b = positions[v2i] - positions[v1i];
            } else if(v == v2i) {
                a = positions[v0i] - positions[v2i];
                b = positions[v1i] - positions[v2i];
            } else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

            /* The normal is cross.normalized(), we need to multiply it it by
               surface area which is cross.length()/2. Since normalization is
               division by length, multiplying it by length again will be a
               no-op. Then, since all normals are divided by 2, it doesn't
               change their ratio for the final normalization so we can omit
               that as well. Finally we need to weight by the angle, and in
               that case only the ratio is important as well, so it doesn't
               matter if degrees or radians. */
            normals[v] += cross*Float(Math::angle(a.normalized(), b.normalized()));
        }

        /* Normalize the accumulated direction */
        normals[v] = normals[v].normalized();
    }
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template void generateSmoothNormalsInto<UnsignedByte>(const Containers::StridedArrayView1D<const UnsignedByte>&, const Containers::StridedArrayView1D<const Vector3>&, const Containers::StridedArrayView1D<Vector3>&);
template void generateSmoothNormalsInto<UnsignedShort>(const Containers::StridedArrayView1D<const UnsignedShort>&, const Containers::StridedArrayView1D<const Vector3>&, const Containers::StridedArrayView1D<Vector3>&);
template void generateSmoothNormalsInto<UnsignedInt>(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const Vector3>&, const Containers::StridedArrayView1D<Vector3>&);
#endif

template<class T> Containers::Array<Vector3> generateSmoothNormals(const Containers::StridedArrayView1D<const T>& indices, const Containers::StridedArrayView1D<const Vector3>& positions) {
    Containers::Array<Vector3> out{Containers::NoInit, positions.size()};
    generateSmoothNormalsInto(indices, positions, out);
    return out;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template Containers::Array<Vector3> generateSmoothNormals<UnsignedByte>(const Containers::StridedArrayView1D<const UnsignedByte>&, const Containers::StridedArrayView1D<const Vector3>&);
template Containers::Array<Vector3> generateSmoothNormals<UnsignedShort>(const Containers::StridedArrayView1D<const UnsignedShort>&, const Containers::StridedArrayView1D<const Vector3>&);
template Containers::Array<Vector3> generateSmoothNormals<UnsignedInt>(const Containers::StridedArrayView1D<const UnsignedInt>&, const Containers::StridedArrayView1D<const Vector3>&);
#endif

}}
