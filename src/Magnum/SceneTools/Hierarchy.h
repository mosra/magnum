#ifndef Magnum_SceneTools_Hierarchy_h
#define Magnum_SceneTools_Hierarchy_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

/** @file
 * @brief Function @ref Magnum::SceneTools::parentsBreadthFirst(), @ref Magnum::SceneTools::parentsBreadthFirstInto(), @ref Magnum::SceneTools::childrenDepthFirst(), @ref Magnum::SceneTools::childrenDepthFirstInto(), @ref Magnum::SceneTools::absoluteFieldTransformations2D(), @ref Magnum::SceneTools::absoluteFieldTransformations2DInto(), @ref Magnum::SceneTools::absoluteFieldTransformations3D(), @ref Magnum::SceneTools::absoluteFieldTransformations3DInto()
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/SceneTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace SceneTools {

/**
@brief Retrieve parents in a breadth-first order
@m_since_latest

Extracts the @ref Trade::SceneField::Parent field mapping and data from
@p scene and converts it to match the following rules:

-   a parent object reference appears always before any of its children
-   the array is clustered so children sharing the same parent are together

This form is useful primarily for calculating absolute object transformations,
for example:

@snippet SceneTools.cpp parentsBreadthFirst-transformations

The operation is done in an @f$ \mathcal{O}(n) @f$ execution time and memory
complexity, with @f$ n @f$ being @ref Trade::SceneData::mappingBound(). The
@ref Trade::SceneField::Parent field is expected to be contained in the scene,
having no cycles (i.e., every node listed just once) and not being sparse
(i.e., every node listed in the field reachable from the root).

@experimental

@see @ref Trade::SceneData::hasField(), @ref childrenDepthFirst()
*/
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Containers::Pair<UnsignedInt, Int>> parentsBreadthFirst(const Trade::SceneData& scene);

/**
@brief Retrieve parents in a breadth-first order into a pre-allocated view
@m_since_latest

Like @ref parentsBreadthFirst(), but puts the result into @p mappingDestination
and @p parentDestination instead of allocating a new array. Expect that both
views have a size equal to size of the @ref Trade::SceneField::Parent view in
@p scene.

@experimental

@see @ref Trade::SceneData::fieldSize(SceneField) const,
    @ref childrenDepthFirstInto()
*/
MAGNUM_SCENETOOLS_EXPORT void parentsBreadthFirstInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<Int>& parentDestination);

/**
@brief Retrieve children in a depth-first order
@m_since_latest

Extracts the @ref Trade::SceneField::Parent field mapping and data from
@p scene and converts it to a list of (object index, children count) pairs
such that:

-   children of given object are directly following the object itself
-   the count includes direct children as well as nested children, next object
    in the same level, if exists, is thus after another `count` items

Objects in particular level keep the same order as they had in the
@ref Trade::SceneField::Parent field. Size of the returned list is equal to the
@ref Trade::SceneField::Parent field size. Implicitly, the whole returned list
contains (nested) children of the root, which implies that the first returned
object is the first top-level object (i.e., with a parent equal to
@cpp -1 @ce), and size of the list is the count of all objects.

This form is useful primarily for marking and extracting subtrees, for example:

@snippet SceneTools.cpp childrenDepthFirst-extract-tree

The operation is done in an @f$ \mathcal{O}(n) @f$ execution time and memory
complexity, with @f$ n @f$ being @ref Trade::SceneData::mappingBound(). The
@ref Trade::SceneField::Parent field is expected to be contained in the scene,
having no cycles (i.e., every node listed just once) and not being sparse
(i.e., every node listed in the field reachable from the root).

@experimental

@see @ref Trade::SceneData::hasField(), @ref parentsBreadthFirst()
*/
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Containers::Pair<UnsignedInt, UnsignedInt>> childrenDepthFirst(const Trade::SceneData& scene);

/**
@brief Retrieve children in a depth-first order into a pre-allocated view
@m_since_latest

Like @ref childrenDepthFirst(), but puts the result into @p mappingDestination
and @p childCountDestination instead of allocating a new array. Expect that
both views have a size equal to size of the @ref Trade::SceneField::Parent view
in @p scene.

@experimental

@see @ref Trade::SceneData::fieldSize(SceneField) const,
    @ref parentsBreadthFirstInto()
*/
MAGNUM_SCENETOOLS_EXPORT void childrenDepthFirstInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<UnsignedInt>& mappingDestination, const Containers::StridedArrayView1D<UnsignedInt>& childCountDestination);

/**
@brief Calculate absolute 2D transformations for given field
@m_since_latest

For all entries of given field in @p scene returns an absolute transformation
of the object they're attached to in the scene with @p globalTransformation
prepended. The @ref Trade::SceneField::Parent field is expected to be contained
in the scene, having no cycles or duplicates, the scene is expected to be 2D
and @p fieldId is expected to be less than @ref Trade::SceneData::fieldCount().
If the field is empty, the function returns an empty array.

The operation is done in an @f$ \mathcal{O}(m + n) @f$ execution time and
memory complexity, with @f$ m @f$ being size of @p fieldId and @f$ n @f$ being
@ref Trade::SceneData::mappingBound(). The function calls
@ref parentsBreadthFirst() internally.

The returned data are in the same order as object mapping entries in
@p fieldId. Fields attached to objects without a @ref Trade::SceneField::Parent
or to objects in loose hierarchy subtrees will have their transformation set to
an unspecified value.

This function can be used for example to flatten a mesh hierarchy, bake
the transformations to actual meshes and then concatenate them together into a
single mesh:

@snippet SceneTools.cpp absoluteFieldTransformations2D-mesh-concatenate

@experimental

@see @ref absoluteFieldTransformations2D(const Trade::SceneData&, UnsignedInt, const Matrix3&),
    @ref absoluteFieldTransformations2DInto(),
    @ref absoluteFieldTransformations3D(), @ref Trade::SceneData::hasField(),
    @ref Trade::SceneData::is2D()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, UnsignedInt fieldId, const Matrix3& globalTransformation = {});
#else
/* To avoid including Matrix3 */
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, UnsignedInt fieldId, const Matrix3& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, UnsignedInt fieldId);
#endif

/**
@brief Calculate absolute 2D transformations for given named field
@m_since_latest

Translates @p field to a field ID using @ref Trade::SceneData::fieldId() and
delegates to @ref absoluteFieldTransformations2D(const Trade::SceneData&, UnsignedInt, const Matrix3&).
The @p field is expected to exist in @p scene.
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, Trade::SceneField field, const Matrix3& globalTransformation = {});
#else
/* To avoid including Matrix3 */
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, Trade::SceneField field, const Matrix3& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix3> absoluteFieldTransformations2D(const Trade::SceneData& scene, Trade::SceneField field);
#endif

/**
@brief Calculate absolute 2D transformations for given field into an existing array
@param[in]  scene           Input scene
@param[in]  fieldId         Field to calculate the transformations for
@param[out] transformations Where to put the calculated transformations
@param[in]  globalTransformation Global transformation to prepend
@m_since_latest

A variant of @ref absoluteFieldTransformations2D(const Trade::SceneData&, UnsignedInt, const Matrix3&)
that fills existing memory instead of allocating a new array. The
@p transformations array is expected to have the same size as the @p fieldId.
@see @ref Trade::SceneData::fieldSize()
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation = {});
#else
/* To avoid including Matrix3 */
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations);
#endif

/**
@brief Calculate absolute 2D transformations for given named field into an existing array
@m_since_latest

Translates @p field to a field ID using @ref Trade::SceneData::fieldId() and
delegates to @ref absoluteFieldTransformations2DInto(const Trade::SceneData&, UnsignedInt, const Containers::StridedArrayView1D<Matrix3>&, const Matrix3&)
The @p field is expected to exist in @p scene.
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation = {});
#else
/* To avoid including Matrix3 */
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations2DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations);
#endif

/**
@brief Calculate absolute 3D transformations for given field
@m_since_latest

For all entries of given field in @p scene returns an absolute transformation
of the object they're attached to in the scene with @p globalTransformation
prepended. The @ref Trade::SceneField::Parent field is expected to be contained
in the scene, having no cycles or duplicates, the scene is expected to be 3D
and @p fieldId is expected to be less than @ref Trade::SceneData::fieldCount().
If the field is empty, the function returns an empty array.

The operation is done in an @f$ \mathcal{O}(m + n) @f$ execution time and
memory complexity, with @f$ m @f$ being size of @p fieldId and @f$ n @f$ being
@ref Trade::SceneData::mappingBound(). The function calls
@ref parentsBreadthFirst() internally.

The returned data are in the same order as object mapping entries in
@p fieldId. Fields attached to objects without a @ref Trade::SceneField::Parent
or to objects in loose hierarchy subtrees will have their transformation set to
an unspecified value.

This function can be used for example to flatten a mesh hierarchy, bake
the transformations to actual meshes and then concatenate them together into a
single mesh:

@snippet SceneTools.cpp absoluteFieldTransformations3D-mesh-concatenate

@experimental

@see @ref absoluteFieldTransformations3D(const Trade::SceneData&, UnsignedInt, const Matrix4&),
    @ref absoluteFieldTransformations3DInto(),
    @ref absoluteFieldTransformations2D(), @ref Trade::SceneData::hasField(),
    @ref Trade::SceneData::is3D()
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, UnsignedInt fieldId, const Matrix4& globalTransformation = {});
#else
/* To avoid including Matrix4 */
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, UnsignedInt fieldId, const Matrix4& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, UnsignedInt fieldId);
#endif

/**
@brief Calculate absolute 3D transformations for given named field
@m_since_latest

Translates @p field to a field ID using @ref Trade::SceneData::fieldId() and
delegates to @ref absoluteFieldTransformations3D(const Trade::SceneData&, UnsignedInt, const Matrix4&).
The @p field is expected to exist in @p scene.
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, Trade::SceneField field, const Matrix4& globalTransformation = {});
#else
/* To avoid including Matrix4 */
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, Trade::SceneField field, const Matrix4& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT Containers::Array<Matrix4> absoluteFieldTransformations3D(const Trade::SceneData& scene, Trade::SceneField field);
#endif

/**
@brief Calculate absolute 3D transformations for given field into an existing array
@param[in]  scene           Input scene
@param[in]  fieldId         Field to calculate the transformations for
@param[out] transformations Where to put the calculated transformations
@param[in]  globalTransformation Global transformation to prepend
@m_since_latest

A variant of @ref absoluteFieldTransformations3D(const Trade::SceneData&, UnsignedInt, const Matrix4&)
that fills existing memory instead of allocating a new array. The
@p transformations array is expected to have the same size as the @p fieldId.
@see @ref Trade::SceneData::fieldSize()
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation = {});
#else
/* To avoid including Matrix4 */
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations);
#endif

/**
@brief Calculate absolute 3D transformations for given named field into an existing array
@m_since_latest

Translates @p field to a field ID using @ref Trade::SceneData::fieldId() and
delegates to @ref absoluteFieldTransformations3DInto(const Trade::SceneData&, UnsignedInt, const Containers::StridedArrayView1D<Matrix4>&, const Matrix4&)
The @p field is expected to exist in @p scene.
@experimental
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation = {});
#else
/* To avoid including Matrix4 */
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation);
MAGNUM_SCENETOOLS_EXPORT void absoluteFieldTransformations3DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations);
#endif

}}

#endif
