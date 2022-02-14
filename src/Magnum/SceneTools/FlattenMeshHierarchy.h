#ifndef Magnum_SceneTools_FlattenMeshHierarchy_h
#define Magnum_SceneTools_FlattenMeshHierarchy_h
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

/** @file
 * @brief Function @ref Magnum::SceneTools::flattenMeshHierarchy2D(), @ref Magnum::SceneTools::flattenMeshHierarchy3D()
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/SceneTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace SceneTools {

/**
@brief Flatten a 2D mesh hierarchy
@m_since_latest

For all @ref Trade::SceneField::Mesh entries that are a part of a hierarchy
returns a triple of mesh ID, @ref Trade::SceneField::MeshMaterial and its
absolute transformation in the scene with @p globalTransformation prepended.
The @ref Trade::SceneField::Parent field is expected to be contained in the
scene, having no cycles or duplicates, and the scene is expected to be 2D. If
@ref Trade::SceneField::Mesh is not present or is empty, returns an empty
array. You can then use @ref MeshTools::transform2D() to apply the
transformations to actual meshes:

@snippet MagnumSceneTools.cpp flattenMeshHierarchy2D-transformations

The operation is done in an @f$ \mathcal{O}(m + n) @f$ execution time and
memory complexity, with @f$ m @f$ being size of the @ref Trade::SceneField::Mesh
field and @f$ n @f$ being @ref Trade::SceneData::mappingBound(). The function
calls @ref orderClusterParents() internally.
@see @ref Trade::SceneData::hasField(), @ref Trade::SceneData::is2D(),
    @ref MeshTools::concatenate()
*/
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix3>> flattenMeshHierarchy2D(const Trade::SceneData& scene, const Matrix3& globalTransformation);

/** @overload
@m_since_latest

Same as above with @p globalTransformation set to an identity matrix.
*/
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix3>> flattenMeshHierarchy2D(const Trade::SceneData& scene);

/**
@brief Flatten a 3D mesh hierarchy
@m_since_latest

For all @ref Trade::SceneField::Mesh entries that are a part of a hierarchy
returns a triple of mesh ID, @ref Trade::SceneField::MeshMaterial and its
absolute transformation in the scene with @p globalTransformation prepended.
The @ref Trade::SceneField::Parent field is expected to be contained in the
scene, having no cycles or duplicates, and the scene is expected to be 3D. If
@ref Trade::SceneField::Mesh is not present or is empty, returns an empty
array. You can then use @ref MeshTools::transform3D() to apply the
transformations to actual meshes:

@snippet MagnumSceneTools.cpp flattenMeshHierarchy3D-transformations

The operation is done in an @f$ \mathcal{O}(m + n) @f$ execution time and
memory complexity, with @f$ m @f$ being size of the @ref Trade::SceneField::Mesh
field and @f$ n @f$ being @ref Trade::SceneData::mappingBound(). The function
calls @ref orderClusterParents() internally.
@see @ref Trade::SceneData::hasField(), @ref Trade::SceneData::is3D(),
    @ref MeshTools::concatenate()
*/
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix4>> flattenMeshHierarchy3D(const Trade::SceneData& scene, const Matrix4& globalTransformation);

/** @overload
@m_since_latest

Same as above with @p globalTransformation set to an identity matrix.
*/
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix4>> flattenMeshHierarchy3D(const Trade::SceneData& scene);

}}

#endif
