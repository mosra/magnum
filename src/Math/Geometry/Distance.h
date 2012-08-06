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
            return Vector3<T>::cross(point - a, point - b).dot()/(b - a).dot();
        }

        /**
         * @brief %Dístance of point from line segment
         * @param a         Starting point of the line
         * @param b         Ending point of the line
         * @param point     Point
         *
         * Returns distance of point from line segment or from its
         * starting/ending point, depending on where the point lies.
         *
         * Determining whether the point lies next to line segment or outside
         * is done using Pythagorean theorem. If the following equation
         * applies, the point **p** lies outside line segment closer to **a**:
         * @f[
         *      |\boldsymbol p - \boldsymbol b|^2 > |\boldsymbol b - \boldsymbol a|^2 + |\boldsymbol p - \boldsymbol a|^2
         * @f]
         * On the other hand, if the following equation applies, the point
         * lies outside line segment closer to **b**:
         * @f[
         *      |\boldsymbol p - \boldsymbol a|^2 > |\boldsymbol b - \boldsymbol a|^2 + |\boldsymbol p - \boldsymbol b|^2
         * @f]
         * The last alternative is when the following equation applies. The
         * point then lies between **a** and **b** and the distance is
         * computed the same way as in linePoint().
         * @f[
         *      |\boldsymbol b - \boldsymbol a|^2 > |\boldsymbol p - \boldsymbol a|^2 + |\boldsymbol p - \boldsymbol b|^2
         * @f]
         *
         * @see lineSegmentPointSquared()
         */
        template<class T> inline static T lineSegmentPoint(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
            return sqrt(lineSegmentPointSquared(a, b, point));
        }

        /**
         * @brief %Distance of point from line segment, squared
         *
         * More efficient than lineSegmentPoint() for comparing distance with
         * other values, because it doesn't compute the square root.
         */
        template<class T> static T lineSegmentPointSquared(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
            Vector3<T> pointMinusA = point - a;
            Vector3<T> pointMinusB = point - b;
            T pointDistanceA = pointMinusA.dot();
            T pointDistanceB = pointMinusB.dot();
            T bDistanceA = (b - a).dot();

            /* Point is before A */
            if(pointDistanceB > bDistanceA + pointDistanceA)
                return pointDistanceA;

            /* Point is after B */
            if(pointDistanceA > bDistanceA + pointDistanceB)
                return pointDistanceB;

            /* Between A and B */
            return Vector3<T>::cross(pointMinusA, pointMinusB).dot()/bDistanceA;
        }
};

}}}

#endif
