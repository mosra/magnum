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

#include "GenerateNormals.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Math/Functions.h"
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
    CORRADE_IGNORE_DEPRECATED_PUSH
    normalIndices = MeshTools::duplicate(normalIndices, MeshTools::removeDuplicates(normals));
    CORRADE_IGNORE_DEPRECATED_POP
    return {std::move(normalIndices), std::move(normals)};
}
#endif

namespace {

#if defined(CORRADE_MSVC2019_COMPATIBILITY) && !defined(CORRADE_MSVC2017_COMPATIBILITY)
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

    if(indices.empty()) return;

    /* Gather count of triangles for every vertex. This abuses the output
       storage to avoid extra allocations, zero-initialize it first to avoid
       random memory getting used. */
    Containers::StridedArrayView1D<UnsignedInt> triangleCount =
        Containers::arrayCast<UnsignedInt>(normals);
    for(UnsignedInt& i: triangleCount) i = 0;
    for(const T index: indices) {
        CORRADE_ASSERT(index < positions.size(), "MeshTools::generateSmoothNormalsInto(): index" << index << "out of bounds for" << positions.size() << "elements", );
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

    /* Precalculate cross product and interior angles of each face --- the loop
       below would otherwise calculate it for every vertex, which is at least
       3x as much work */
    Containers::Array<std::pair<Vector3, Math::Vector3<Rad>>> crossAngles{NoInit, indices.size()/3};
    for(std::size_t i = 0; i != crossAngles.size(); ++i) {
        const Vector3 v0 = positions[indices[i*3 + 0]];
        const Vector3 v1 = positions[indices[i*3 + 1]];
        const Vector3 v2 = positions[indices[i*3 + 2]];

        /* Cross product */
        crossAngles[i].first = Math::cross(v2 - v1, v0 - v1);

        /* If any of the vectors is zero, the normalization would result in a
           NaN and the angle calculation will assert. This happens also when
           any of the original positions is NaN. If that's the case, skip the
           rest. Given triangle will then contribute with a zero total angle,
           effectively getting ignored for normal calculation. */
        const Vector3 v10n = (v1 - v0).normalized();
        const Vector3 v20n = (v2 - v0).normalized();
        const Vector3 v21n = (v2 - v1).normalized();
        if(Math::isNan(v10n) || Math::isNan(v20n) || Math::isNan(v21n)) {
            crossAngles[i].second = Math::Vector3<Rad>{Math::ZeroInit};
            continue;
        }

        /* Inner angle at each vertex of the triangle. The last one can be
           calculated as a remainder to 180°. */
        /* This using namespace doesn't work with MSVC2019 with /permissive-
           (it gets lost when instantiating?!), so it's duplicated above */
        using namespace Math::Literals;
        crossAngles[i].second[0] = Math::angle(v10n, v20n);
        crossAngles[i].second[1] = Math::angle(-v10n, v21n);
        crossAngles[i].second[2] = Rad(180.0_degf)
            - crossAngles[i].second[0] - crossAngles[i].second[1];
    }

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
            const std::pair<Vector3, Math::Vector3<Rad>>& crossAngle = crossAngles[triangleIds[t]];

            /* Angle between two sides of the triangle that share vertex `v`.
               The shared vertex can be one of the three. */
            Rad angle;
            if(v == v0i) angle = crossAngle.second[0];
            else if(v == v1i) angle = crossAngle.second[1];
            else if(v == v2i) angle = crossAngle.second[2];
            else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

            /* The normal is cross.normalized(), we need to multiply it it by
               surface area which is cross.length()/2. Since normalization is
               division by length, multiplying it by length again will be a
               no-op. Then, since all normals are divided by 2, it doesn't
               change their ratio for the final normalization so we can omit
               that as well. Finally we need to weight by the angle, and in
               that case only the ratio is important as well, so it doesn't
               matter if degrees or radians. */
            normals[v] += crossAngle.first*Float(angle);
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
    Containers::Array<Vector3> out{Containers::NoInit, positions.size()};
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
    Containers::Array<Vector3> out{Containers::NoInit, positions.size()};
    generateSmoothNormalsInto(indices, positions, out);
    return out;
}

}}
