#ifndef Magnum_SceneTools_FlattenTransformationHierarchy_h
#define Magnum_SceneTools_FlattenTransformationHierarchy_h
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
 * @brief Function @ref Magnum::SceneTools::flattenTransformationHierarchy2D(), @ref Magnum::SceneTools::flattenTransformationHierarchy2DInto(), @ref Magnum::SceneTools::flattenTransformationHierarchy3D(), @ref Magnum::SceneTools::flattenTransformationHierarchy3DInto()
 * @m_deprecated_since_latest Use @ref Magnum/SceneTools/Hierarchy.h and the
 *      @relativeref{Magnum,SceneTools::absoluteFieldTransformations2D()} /
 *      @relativeref{Magnum,SceneTools::absoluteFieldTransformations3D()}
 *      functions instead.
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Macros.h>

#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/SceneTools/Hierarchy.h"
#include "Magnum/Trade/Trade.h"

CORRADE_DEPRECATED_FILE("use Magnum/SceneTools/Hierarchy.h and the SceneTools::absoluteFieldTransformations*D() functions instead")

namespace Magnum { namespace SceneTools {

/* Made header-only to not have to maintain a deprecated source file */

/**
@brief Flatten a 2D transformation hierarchy for given field
@m_deprecated_since_latest Use @ref absoluteFieldTransformations2D(const Trade::SceneData&, Trade::SceneField, const Matrix3&)
    instead.
*/
inline CORRADE_DEPRECATED("use absoluteFieldTransformations2D() instead") Containers::Array<Matrix3> flattenTransformationHierarchy2D(const Trade::SceneData& scene, Trade::SceneField field, const Matrix3& globalTransformation = {}) {
    return absoluteFieldTransformations2D(scene, field, globalTransformation);
}

/**
@brief Flatten a 2D transformation hierarchy for given field ID
@m_deprecated_since_latest Use @ref absoluteFieldTransformations2D(const Trade::SceneData&, UnsignedInt, const Matrix3&)
    instead.
*/
inline CORRADE_DEPRECATED("use absoluteFieldTransformations2D() instead") Containers::Array<Matrix3> flattenTransformationHierarchy2D(const Trade::SceneData& scene, UnsignedInt fieldId, const Matrix3& globalTransformation = {}) {
    return absoluteFieldTransformations2D(scene, fieldId, globalTransformation);
}

/**
@brief Flatten a 2D transformation hierarchy for given field into an existing array
@m_deprecated_since_latest Use @ref absoluteFieldTransformations2DInto(const Trade::SceneData&, Trade::SceneField, const Containers::StridedArrayView1D<Matrix3>&, const Matrix3&)
    instead.
*/
inline CORRADE_DEPRECATED("use absoluteFieldTransformations2DInto() instead") void flattenTransformationHierarchy2DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation = {}) {
    return absoluteFieldTransformations2DInto(scene, field, transformations, globalTransformation);
}

/**
@brief Flatten a 2D transformation hierarchy for given field ID into an existing array
@m_deprecated_since_latest Use @ref absoluteFieldTransformations2DInto(const Trade::SceneData&, UnsignedInt, const Containers::StridedArrayView1D<Matrix3>&, const Matrix3&)
    instead.
*/
inline CORRADE_DEPRECATED("use absoluteFieldTransformations2DInto() instead") void flattenTransformationHierarchy2DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix3>& transformations, const Matrix3& globalTransformation = {}) {
    return absoluteFieldTransformations2DInto(scene, fieldId, transformations, globalTransformation);
}

/**
@brief Flatten a 3D transformation hierarchy for given field
@m_deprecated_since_latest Use @ref absoluteFieldTransformations3D(const Trade::SceneData&, Trade::SceneField, const Matrix4&)
    instead.
*/
inline CORRADE_DEPRECATED("use absoluteFieldTransformations3D() instead") Containers::Array<Matrix4> flattenTransformationHierarchy3D(const Trade::SceneData& scene, Trade::SceneField field, const Matrix4& globalTransformation = {}) {
    return absoluteFieldTransformations3D(scene, field, globalTransformation);
}

/**
@brief Flatten a 3D transformation hierarchy for given field ID
@m_deprecated_since_latest Use @ref absoluteFieldTransformations3D(const Trade::SceneData&, UnsignedInt, const Matrix4&)
    instead.
*/
inline CORRADE_DEPRECATED("use absoluteFieldTransformations3D() instead") Containers::Array<Matrix4> flattenTransformationHierarchy3D(const Trade::SceneData& scene, UnsignedInt fieldId, const Matrix4& globalTransformation = {}) {
    return absoluteFieldTransformations3D(scene, fieldId, globalTransformation);
}

/**
@brief Flatten a 3D transformation hierarchy for given field into an existing array
@m_deprecated_since_latest Use @ref absoluteFieldTransformations3DInto(const Trade::SceneData&, Trade::SceneField, const Containers::StridedArrayView1D<Matrix4>&, const Matrix4&)
    instead.
*/
inline CORRADE_DEPRECATED("use absoluteFieldTransformations3DInto() instead") void flattenTransformationHierarchy3DInto(const Trade::SceneData& scene, Trade::SceneField field, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation = {}) {
    return absoluteFieldTransformations3DInto(scene, field, transformations, globalTransformation);
}

/**
@brief Flatten a 3D transformation hierarchy for given field ID into an existing array
@m_deprecated_since_latest Use @ref absoluteFieldTransformations3DInto(const Trade::SceneData&, UnsignedInt, const Containers::StridedArrayView1D<Matrix4>&, const Matrix4&)
    instead.
*/
inline CORRADE_DEPRECATED("use absoluteFieldTransformations3DInto() instead") void flattenTransformationHierarchy3DInto(const Trade::SceneData& scene, UnsignedInt fieldId, const Containers::StridedArrayView1D<Matrix4>& transformations, const Matrix4& globalTransformation = {}) {
    return absoluteFieldTransformations3DInto(scene, fieldId, transformations, globalTransformation);
}

}}
#else
#error use Magnum/SceneTools/Hierarchy.h and the SceneTools::absoluteFieldTransformations*D() functions instead
#endif

#endif
