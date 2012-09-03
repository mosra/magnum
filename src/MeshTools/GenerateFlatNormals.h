#ifndef Magnum_MeshTools_GenerateFlatNormals_h
#define Magnum_MeshTools_GenerateFlatNormals_h
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

/** @file
 * @brief Function Magnum::MeshTools::generateFlatNormals()
 */

#include <tuple>

#include "Magnum.h"
#include "magnumMeshToolsVisibility.h"

namespace Magnum { namespace MeshTools {

/**
@brief Generate flat normals
@param indices      Array of triangle face indexes
@param positions    Array of vertex positions
@return Normal indices and vectors

For each face generates one normal vector, removes duplicates before
returning. Example usage:
@code
std::vector<unsigned int> vertexIndices;
std::vector<Vector4> positions;

std::vector<unsigned int> normalIndices;
std::vector<Vector3> normals;
std::tie(normalIndices, normals) = MeshTools::generateFlatNormals(vertexIndices, positions);
@endcode
You can then use combineIndexedArrays() to combine normal and vertex array to
use the same indices.

@attention Index count must be divisible by 3, otherwise zero length result
    is generated.
*/
std::tuple<std::vector<unsigned int>, std::vector<Vector3>> MESHTOOLS_EXPORT generateFlatNormals(const std::vector<unsigned int>& indices, const std::vector<Vector4>& positions);

}}

#endif
