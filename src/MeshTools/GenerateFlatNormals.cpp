/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "GenerateFlatNormals.h"

#include "Math/Point3D.h"
#include "MeshTools/Clean.h"

namespace Magnum { namespace MeshTools {

std::tuple<std::vector<std::uint32_t>, std::vector<Vector3>> generateFlatNormals(const std::vector<std::uint32_t>& indices, const std::vector<Point3D>& positions) {
    CORRADE_ASSERT(!(indices.size()%3), "MeshTools::generateFlatNormals(): index count is not divisible by 3!", (std::tuple<std::vector<std::uint32_t>, std::vector<Vector3>>()));

    /* Create normal for every triangle (assuming counterclockwise winding) */
    std::vector<std::uint32_t> normalIndices;
    normalIndices.reserve(indices.size());
    std::vector<Vector3> normals;
    normals.reserve(indices.size()/3);
    for(std::size_t i = 0; i != indices.size(); i += 3) {
        Vector3 normal = Vector3::cross(positions[indices[i+2]].xyz()-positions[indices[i+1]].xyz(),
                                        positions[indices[i]].xyz()-positions[indices[i+1]].xyz()).normalized();

        /* Use the same normal for all three vertices of the face */
        normalIndices.push_back(normals.size());
        normalIndices.push_back(normals.size());
        normalIndices.push_back(normals.size());
        normals.push_back(normal);
    }

    /* Clean duplicate normals and return */
    clean(normalIndices, normals);
    return std::make_tuple(normalIndices, normals);
}

}}
