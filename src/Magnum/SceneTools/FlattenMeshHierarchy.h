#ifndef Magnum_SceneTools_FlattenMeshHierarchy_h
#define Magnum_SceneTools_FlattenMeshHierarchy_h
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

#ifdef MAGNUM_BUILD_DEPRECATED
/** @file
 * @brief Function @ref Magnum::SceneTools::flattenMeshHierarchy2D(), @ref Magnum::SceneTools::flattenMeshHierarchy2DInto(), @ref Magnum::SceneTools::flattenMeshHierarchy3D(), @ref Magnum::SceneTools::flattenMeshHierarchy3DInto()
 * @m_deprecated_since_latest Use @ref Magnum/SceneTools/Hierarchy.h and the
 *      @relativeref{Magnum,SceneTools::absoluteFieldTransformations2D()} /
 *      @relativeref{Magnum,SceneTools::absoluteFieldTransformations3D()}
 *      functions instead.
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneTools/visibility.h"
#include "Magnum/Trade/Trade.h"

#ifndef _MAGNUM_NO_DEPRECATED_FLATTENMESHHIERARCHY
CORRADE_DEPRECATED_FILE("use Magnum/SceneTools/Hierarchy.h and the SceneTools::absoluteFieldTransformations*D() functions instead")
#endif

namespace Magnum { namespace SceneTools {

/**
@brief Flatten a 2D mesh hierarchy

@m_deprecated_since_latest Use @ref absoluteFieldTransformations2D(const Trade::SceneData&, Trade::SceneField, const Matrix3&)
    with @ref Trade::SceneField::Mesh together with
    @ref Trade::SceneData::meshesMaterialsAsArray() instead.
*/
CORRADE_DEPRECATED("use absoluteFieldTransformations2D() instead") MAGNUM_SCENETOOLS_EXPORT Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix3>> flattenMeshHierarchy2D(const Trade::SceneData& scene, const Matrix3& globalTransformation = {});

/**
@brief Flatten a 2D mesh hierarchy into an existing array
@param[in]  scene           Input scene
@param[out] transformations Where to put the calculated transformations
@param[in]  globalTransformation Global transformation to prepend

@m_deprecated_since_latest Use @ref absoluteFieldTransformations2DInto(const Trade::SceneData&, Trade::SceneField, const Containers::StridedArrayView1D<Matrix3>&, const Matrix3&)
    with @ref Trade::SceneField::Mesh instead.
*/
CORRADE_DEPRECATED("use absoluteFieldTransformations2DInto() instead") MAGNUM_SCENETOOLS_EXPORT void flattenMeshHierarchy2DInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation = {});

/**
@brief Flatten a 3D mesh hierarchy

@m_deprecated_since_latest Use @ref absoluteFieldTransformations3D(const Trade::SceneData&, Trade::SceneField, const Matrix4&)
    with @ref Trade::SceneField::Mesh together with
    @ref Trade::SceneData::meshesMaterialsAsArray() instead.
*/
CORRADE_DEPRECATED("use absoluteFieldTransformations3D() instead") MAGNUM_SCENETOOLS_EXPORT Containers::Array<Containers::Triple<UnsignedInt, Int, Matrix4>> flattenMeshHierarchy3D(const Trade::SceneData& scene, const Matrix4& globalTransformation = {});

/**
@brief Flatten a 3D mesh hierarchy into an existing array
@param[in]  scene           Input scene
@param[out] transformations Where to put the calculated transformations
@param[in]  globalTransformation Global transformation to prepend

@m_deprecated_since_latest Use @ref absoluteFieldTransformations3DInto(const Trade::SceneData&, Trade::SceneField, const Containers::StridedArrayView1D<Matrix4>&, const Matrix4&)
    with @ref Trade::SceneField::Mesh instead.
*/
CORRADE_DEPRECATED("use absoluteFieldTransformations3DInto() instead") MAGNUM_SCENETOOLS_EXPORT void flattenMeshHierarchy3DInto(const Trade::SceneData& scene, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation = {});

}}
#else
#error use Magnum/SceneTools/FlattenTransformationHierarchy.h and the SceneTools::absoluteFieldTransformations*D() functions instead
#endif

#endif
