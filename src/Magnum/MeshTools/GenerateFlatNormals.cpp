/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "GenerateFlatNormals.h"

#include "Magnum/Math/Vector3.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"

namespace Magnum { namespace MeshTools {

std::tuple<std::vector<UnsignedInt>, std::vector<Vector3>> generateFlatNormals(const std::vector<UnsignedInt>& indices, const std::vector<Vector3>& positions) {
    CORRADE_ASSERT(!(indices.size()%3), "MeshTools::generateFlatNormals(): index count is not divisible by 3!", (std::tuple<std::vector<UnsignedInt>, std::vector<Vector3>>()));

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
    return std::make_tuple(std::move(normalIndices), std::move(normals));
}

}}
