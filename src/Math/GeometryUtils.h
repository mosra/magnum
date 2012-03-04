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

#include "Matrix3.h"

namespace Magnum { namespace Math {

/**
@brief Geometry utils
*/
template<class T> class GeometryUtils {
    public:
        /**
         * @brief Intersection of a plane and line
         * @param plane     Plane defined by three points
         * @param a         Starting point of the line
         * @param b         Ending point of the line
         * @return Value, NaN if the line lies on the plane or infinity if the
         * intersection doesn't exist. Intersection point can be then computed
         * with `a+intersection(...)*b`. If returned value is in range
         * @f$ [ 0 ; 1 ] @f$, the intersection is inside the line segment
         * defined by `a` and `b`.
         *
         * First the parametric equation of the plane is computed,
         * @f$ cx + dy + ez = f @f$. Parameters @f$ (c, d, e) @f$ are cross
         * product of two vectors defining the plane, parameter @f$ f @f$ is
         * computed using @f$ (c, d, e) @f$ and one of points defining the
         * plane.
         * @f[
         *     \begin{array}{lcl}
         *         (g, h, i) & = & plane                                    \\
         *         (c, d, e) & = & (h - g) \times (i - g)                   \\
         *         f & = & (c, d, e) \cdot g
         *     \end{array}
         * @f]
         *
         * Using parametric equation and points @f$ a @f$ and @f$ b @f$, value
         * of @f$ t @f$ is computed and returned.
         * @f[
         *     \begin{array}{lcl}
         *         \Delta b & = & b - a                                     \\
         *         f & = & (c, d, e) \cdot (a + \Delta b \cdot t)           \\
         *         t & = & \frac{f - (c, d, e) \cdot a}
         *                 {(c, d, e) \cdot \Delta b}
         *      \end{array}
         * @f]
         */
        static T intersection(const Matrix3<T>& plane, const Vector3<T>& a, const Vector3<T>& b) {
            /* Cross product of two vectors defining the plane */
            Vector3<T> crossProduct = Vector3<T>::cross(plane.at(1)-plane.at(0), plane.at(2)-plane.at(0));

            /* Compute f with cross product and one of the points defining the
               plane */
            T f = crossProduct*plane.at(0);

            /* Compute t */
            return (f-crossProduct*a)/(crossProduct*(b-a));
        }
};

}}

#endif
