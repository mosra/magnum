#ifndef Magnum_Math_Geometry_Distance_h
#define Magnum_Math_Geometry_Distance_h
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
 * @brief Class Magnum::Math::Geometry::Distance
 */

#include "Math/Vector3.h"

namespace Magnum { namespace Math { namespace Geometry {

/** @brief Functions for computing distances */
class Distance {
    public:
        /**
         * @brief %Distance of line and point
         * @param a         First point of the line
         * @param b         Second point of the line
         * @param point     Point
         *
         * The distance *d* is computed from point **p** and line defined by **a**
         * and **b** using @ref Vector3::cross() "cross product":
         * @f[
         *      d = \frac{|(\boldsymbol p - \boldsymbol a) \times (\boldsymbol p - \boldsymbol b)|}
         *      {|\boldsymbol b - \boldsymbol a|}
         * @f]
         *
         * @see linePointSquared()
         */
        template<class T> inline static T linePoint(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
            return sqrt(linePointSquared(a, b, point));
        }

        /**
         * @brief %Distance of line and point, squared
         *
         * More efficient than linePoint() for comparing distance with other
         * values, because it doesn't compute the square root.
         */
        template<class T> static T linePointSquared(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
            return Vector3<T>::cross(point - a, point - b).lengthSquared()/(b - a).lengthSquared();
        }
};

}}}

#endif
