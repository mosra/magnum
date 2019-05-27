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

}}
