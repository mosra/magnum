#ifndef Magnum_SceneTools_FlattenTransformationHierarchy_h
#define Magnum_SceneTools_FlattenTransformationHierarchy_h
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
 * @brief Function @ref Magnum::SceneTools::flattenTransformationHierarchy2D(), @ref Magnum::SceneTools::flattenTransformationHierarchy2DInto(), @ref Magnum::SceneTools::flattenTransformationHierarchy3D(), @ref Magnum::SceneTools::flattenTransformationHierarchy3DInto()
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/SceneTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace SceneTools {

/**
@brief Flatten a 2D transformation hierarchy for given field
@m_since_latest

For all entries of given @p field in @p scene returns an absolute
transformation of the object they're attached to in the scene with
@p globalTransformation prepended. The @ref Trade::SceneField::Parent field is
expected to be contained in the scene, having no cycles or duplicates, and the
scene is expected to be 2D. The @p field is expected to be present, if it's
empty, the function returns an empty array.

The operation is done in an @f$ \mathcal{O}(m + n) @f$ execution time and
memory complexity, with @f$ m @f$ being size of @p field and @f$ n @f$ being
@ref Trade::SceneData::mappingBound(). The function calls
@ref orderClusterParents() internally.

The returned data are in the same order as object mapping entries in @p field.
Fields attached to objects without a @ref Trade::SceneField::Parent or to
objects in loose hierarchy subtrees will have their transformation set to an
unspecified value.

This function can be used for example to flatten a mesh hierarchy, bake
the transformations to actual meshes and then concatenate them together into a
single mesh:

@snippet MagnumSceneTools.cpp flattenTransformationHierarchy2D-mesh-concatenate

@experimental

@see @ref flattenTransformationHierarchy2D(const Trade::SceneData&, UnsignedInt, const Matrix3&),
    @ref flattenTransformationHierarchy2DInto(),
    @ref flattenTransformationHierarchy3D(), @ref Trade::SceneData::hasField(),
    @ref Trade::SceneData::is2D()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> flattenTransformationHierarchy2D(const Trade::SceneData& scene, Trade::SceneField field, const Matrix3& globalTransformation = {});
#else
/* To avoid including Matrix3 */
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> flattenTransformationHierarchy2D(const Trade::SceneData& scene, Trade::SceneField field, const Matrix3& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> flattenTransformationHierarchy2D(const Trade::SceneData& scene, Trade::SceneField field);
#endif

/**
@brief Flatten a 2D transformation hierarchy for given field ID
@m_since_latest

A variant of @ref flattenTransformationHierarchy2D(const Trade::SceneData&, Trade::SceneField, const Matrix3&)
that takes a field ID instead of name. Useful for example in combination with
@ref Trade::SceneData::findFieldId() to avoid a double lookup if it isn't clear
if a field exists at all. The @p fieldId is expected to be smaller than
@ref Trade::SceneData::fieldCount().
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> flattenTransformationHierarchy2D(const Trade::SceneData& scene, UnsignedInt fieldId, const Matrix3& globalTransformation = {});
#else
/* To avoid including Matrix3 */
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> flattenTransformationHierarchy2D(const Trade::SceneData& scene, UnsignedInt fieldId, const Matrix3& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> flattenTransformationHierarchy2D(const Trade::SceneData& scene, UnsignedInt fieldId);
#endif

/**
@brief Flatten a 2D transformation hierarchy for given field into an existing array
@param[in]  scene           Input scene
@param[in]  field           Field to calculate the transformations for
@param[out] transformations Where to put the calculated transformations
@param[in]  globalTransformation Global transformation to prepend
@m_since_latest

A variant of @ref flattenTransformationHierarchy2D(const Trade::SceneData&, Trade::SceneField, const Matrix3&)
that fills existing memory instead of allocating a new array. The
@p transformations array is expected to have the same size as the @p field.
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy2DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation = {});
#else
/* To avoid including Matrix3 */
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy2DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy2DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations);
#endif

/**
@brief Flatten a 2D transformation hierarchy for given field ID into an existing array
@m_since_latest

A variant of @ref flattenTransformationHierarchy2DInto(const Trade::SceneData&, Trade::SceneField, const Containers::StridedArrayView1D<Matrix3>&, const Matrix3&)
that takes a field ID instead of name. Useful for example in combination with
@ref Trade::SceneData::findFieldId() to avoid a double lookup if it isn't clear
if a field exists at all. The @p fieldId is expected to be smaller than
@ref Trade::SceneData::fieldCount().
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy2DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation = {});
#else
/* To avoid including Matrix3 */
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy2DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy2DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations);
#endif

/**
@brief Flatten a 3D transformation hierarchy for given field
@m_since_latest

For all entries of given @p field in @p scene returns an absolute
transformation of the object they're attached to in the scene with
@p globalTransformation prepended. The @ref Trade::SceneField::Parent field is
expected to be contained in the scene, having no cycles or duplicates, and the
scene is expected to be 3D. The @p field is expected to be present, if it's
empty, the function returns an empty array.

The operation is done in an @f$ \mathcal{O}(m + n) @f$ execution time and
memory complexity, with @f$ m @f$ being size of @p field and @f$ n @f$ being
@ref Trade::SceneData::mappingBound(). The function calls
@ref orderClusterParents() internally.

The returned data are in the same order as object mapping entries in @p field.
Fields attached to objects without a @ref Trade::SceneField::Parent or to
objects in loose hierarchy subtrees will have their transformation set to an
unspecified value.

This function can be used for example to flatten a mesh hierarchy, bake
the transformations to actual meshes and then concatenate them together into a
single mesh:

@snippet MagnumSceneTools.cpp flattenTransformationHierarchy3D-mesh-concatenate

@experimental

@see @ref flattenTransformationHierarchy3D(const Trade::SceneData&, UnsignedInt, const Matrix4&),
    @ref flattenTransformationHierarchy3DInto(),
    @ref flattenTransformationHierarchy2D(), @ref Trade::SceneData::hasField(),
    @ref Trade::SceneData::is3D()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> flattenTransformationHierarchy3D(const Trade::SceneData& scene, Trade::SceneField field, const Matrix4& globalTransformation = {});
#else
/* To avoid including Matrix4 */
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> flattenTransformationHierarchy3D(const Trade::SceneData& scene, Trade::SceneField field, const Matrix4& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> flattenTransformationHierarchy3D(const Trade::SceneData& scene, Trade::SceneField field);
#endif

/**
@brief Flatten a 3D transformation hierarchy for given field ID
@m_since_latest

A variant of @ref flattenTransformationHierarchy3D(const Trade::SceneData&, Trade::SceneField, const Matrix4&)
that takes a field ID instead of name. Useful for example in combination with
@ref Trade::SceneData::findFieldId() to avoid a double lookup if it isn't clear
if a field exists at all. The @p fieldId is expected to be smaller than
@ref Trade::SceneData::fieldCount().
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> flattenTransformationHierarchy3D(const Trade::SceneData& scene, UnsignedInt fieldId, const Matrix4& globalTransformation = {});
#else
/* To avoid including Matrix34 */
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> flattenTransformationHierarchy3D(const Trade::SceneData& scene, UnsignedInt fieldId, const Matrix4& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> flattenTransformationHierarchy3D(const Trade::SceneData& scene, UnsignedInt fieldId);
#endif

/**
@brief Flatten a 3D transformation hierarchy for given field into an existing array
@param[in]  scene           Input scene
@param[in]  field           Field to calculate the transformations for
@param[out] transformations Where to put the calculated transformations
@param[in]  globalTransformation Global transformation to prepend
@m_since_latest

A variant of @ref flattenTransformationHierarchy3D(const Trade::SceneData&, Trade::SceneField, const Matrix4&)
that fills existing memory instead of allocating a new array. The
@p transformations array is expected to have the same size as the @p field.
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy3DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation = {});
#else
/* To avoid including Matrix4 */
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy3DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy3DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations);
#endif

/**
@brief Flatten a 3D transformation hierarchy for given field into an existing array
@m_since_latest

A variant of @ref flattenTransformationHierarchy3DInto(const Trade::SceneData&, Trade::SceneField, const Containers::StridedArrayView1D<Matrix4>&, const Matrix4&)
that takes a field ID instead of name. Useful for example in combination with
@ref Trade::SceneData::findFieldId() to avoid a double lookup if it isn't clear
if a field exists at all. The @p fieldId is expected to be smaller than
@ref Trade::SceneData::fieldCount().
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy3DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation = {});
#else
/* To avoid including Matrix4 */
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy3DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT void flattenTransformationHierarchy3DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations);
#endif

}}

#endif
