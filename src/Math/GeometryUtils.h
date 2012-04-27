#ifndef Magnum_Math_GeometryUtils_h
#define Magnum_Math_GeometryUtils_h
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
 * @brief Class Magnum::Math::GeometryUtils
 */

#include "Vector3.h"

namespace Magnum { namespace Math {

/**
@brief Geometry utils
*/
class GeometryUtils {
    public:
        /**
         * @brief Intersection of a plane and line
         * @param planePosition Plane position
         * @param planeNormal   Plane normal
         * @param a             Starting point of the line
         * @param b             Ending point of the line
         * @return Intersection point position, NaN if the line lies on the
         * plane or infinity if the intersection doesn't exist. Intersection
         * point can be computed from the position with `a+intersection(...)*b`.
         * If returned value is in range @f$ [ 0 ; 1 ] @f$, the intersection
         * is inside the line segment defined by `a` and `b`.
         *
         * First the parameter *f* of parametric equation of the plane
         * is computed from plane normal **n** and plane position:
         * @f[
         *      \begin{pmatrix} n_0 \\ n_1 \\ n_2 \end{pmatrix} \cdot
         *      \begin{pmatrix} x \\ y \\ z \end{pmatrix} - f = 0
         * @f]
         * Using plane normal **n**, parameter *f* and points **a** and **b**,
         * value of *t* is computed and returned.
         * @f[
         *      \begin{array}{rcl}
         *          \Delta \boldsymbol b & = & \boldsymbol b - \boldsymbol a \\
         *          f & = & \boldsymbol n \cdot (\boldsymbol a + \Delta \boldsymbol b \cdot t) \\
         *          \Rightarrow t & = & \cfrac{f - \boldsymbol n \cdot \boldsymbol a}{\boldsymbol n \cdot \Delta \boldsymbol b}
         *      \end{array}
         * @f]
         */
        template<class T> static T intersection(const Vector3<T>& planePosition, const Vector3<T>& planeNormal, const Vector3<T>& a, const Vector3<T>& b) {
            /* Compute f from normal and plane position */
            T f = Vector3<T>::dot(planePosition, planeNormal);

            /* Compute t */
            return (f-Vector3<T>::dot(planeNormal, a)/Vector3<T>::dot(planeNormal, b-a));
        }
};

}}

#endif
