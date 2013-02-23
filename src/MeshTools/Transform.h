#ifndef Magnum_MeshTools_Transform_h
#define Magnum_MeshTools_Transform_h
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
 * @brief Function Magnum::MeshTools::transformVectorsInPlace(), Magnum::MeshTools::transformVectors(), Magnum::MeshTools::transformPointsInPlace(), Magnum::MeshTools::transformPoints()
 */

#include "Math/Complex.h"
#include "Math/DualQuaternion.h"
#include "Math/Matrix3.h"

namespace Magnum { namespace MeshTools {

/**
@brief Transform vectors in-place using given transformation

Usable for one-time mesh transformations that would otherwise negatively affect
dependent objects, such as (uneven) scaling. Accepts any forward-iterable type
with compatible vector type as @p vectors. Expects that @ref Math::Quaternion "Quaternion"
or @ref Math::Complex "Complex" is normalized, no further requirements are for
transformation matrices.

Unlike in transformPointsInPlace(), the transformation does not involve
translation.

Example usage:
@code
std::vector<Vector3> vectors;
auto transformation = Quaternion::rotation(35.0_degf, Vector3::yAxis());
MeshTools::transformVectorsInPlace(rotation, vectors);
@endcode

@see transformVectors(), Matrix3::transformVector(), Matrix4::transformVector(),
    Complex::transformVectorNormalized(), Quaternion::transformVectorNormalized()
@todo GPU transform feedback implementation (otherwise this is only bad joke)
*/
template<class T, class U> void transformVectorsInPlace(const Math::Quaternion<T>& normalizedQuaternion, U& vectors) {
    for(auto& vector: vectors) vector = normalizedQuaternion.transformVectorNormalized(vector);
}

/** @overload */
template<class T, class U> void transformVectorsInPlace(const Math::Complex<T>& normalizedComplex, U& vectors) {
    for(auto& vector: vectors) vector = normalizedComplex.transformVectorNormalized(vector);
}

/** @overload */
template<class T, class U> void transformVectorsInPlace(const Math::Matrix3<T>& matrix, U& vectors) {
    for(auto& vector: vectors) vector = matrix.transformVector(vector);
}

/** @overload */
template<class T, class U> void transformVectorsInPlace(const Math::Matrix4<T>& matrix, U& vectors) {
    for(auto& vector: vectors) vector = matrix.transformVector(vector);
}

/**
@brief Transform vectors using given transformation

Returns transformed vectors instead of modifying them in-place. See
transformVectorsInPlace() for more information.
*/
template<class T, class U> U transformVectors(const T& transformation, U vectors) {
    U result(std::move(vectors));
    transformVectorsInPlace(transformation, result);
    return result;
}

/**
@brief Transform points in-place using given transformation

Usable for one-time mesh transformations that would otherwise negatively affect
dependent objects, such as (uneven) scaling. Accepts any forward-iterable type
with compatible vector type as @p vectors. Expects that
@ref Math::DualQuaternion "DualQuaternion" is normalized, no further
requirements are for transformation matrices.

Unlike in transformVectorsInPlace(), the transformation also involves
translation.

Example usage:
@code
std::vector<Vector3> points;
auto transformation = DualQuaternion::rotation(35.0_degf, Vector3::yAxis())*
    DualQuaternion::translation({0.5f, -1.0f, 3.0f});
MeshTools::transformPointsInPlace(rotation, points);
@endcode

@see transformPoints(), Matrix3::transformPoint(), Matrix4::transformPoint(),
    DualQuaternion::transformPointNormalized()
*/
template<class T, class U> void transformPointsInPlace(const Math::DualQuaternion<T>& normalizedDualQuaternion, U& points) {
    for(auto& point: points) point = normalizedDualQuaternion.transformPointNormalized(point);
}

/** @overload */
template<class T, class U> void transformPointsInPlace(const Math::Matrix3<T>& matrix, U& points) {
    for(auto& point: points) point = matrix.transformPoint(point);
}

/** @overload */
template<class T, class U> void transformPointsInPlace(const Math::Matrix4<T>& matrix, U& points) {
    for(auto& point: points) point = matrix.transformPoint(point);
}

/**
@brief Transform points using given transformation

Returns transformed points instead of modifying them in-place. See
transformPointsInPlace() for more information.
*/
template<class T, class U> U transformPoints(const T& transformation, U vectors) {
    U result(std::move(vectors));
    transformPointsInPlace(transformation, result);
    return result;
}

}}

#endif
