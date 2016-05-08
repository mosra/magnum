#ifndef Magnum_Math_Geometry_Distance_h
#define Magnum_Math_Geometry_Distance_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
 * @brief Class @ref Magnum::Math::Geometry::Distance
 */

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Math { namespace Geometry {

/** @brief Functions for computing distances */
class Distance {
    public:
        Distance() = delete;

        /**
         * @brief Distance of line and point in 2D
         * @param a         First point of the line
         * @param b         Second point of the line
         * @param point     Point
         *
         * The distance *d* is computed from point **p** and line defined by **a**
         * and **b** using @ref cross(const Vector2<T>&, const Vector2<T>&) "perp-dot product": @f[
         *      d = \frac{|(\boldsymbol b - \boldsymbol a)_\bot \cdot (\boldsymbol a - \boldsymbol p)|} {|\boldsymbol b - \boldsymbol a|}
         * @f]
         * Source: http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
         * @see @ref linePointSquared(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
         */
        template<class T> static T linePoint(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
            const Vector2<T> bMinusA = b - a;
            return std::abs(cross(bMinusA, a - point))/bMinusA.length();
        }

        /**
         * @brief Distance of line and point in 2D, squared
         * @param a         First point of the line
         * @param b         Second point of the line
         * @param point     Point
         *
         * More efficient than
         * @ref linePoint(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
         * for comparing distance with other values, because it doesn't
         * compute the square root.
         */
        template<class T> static T linePointSquared(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
            const Vector2<T> bMinusA = b - a;
            return Math::pow<2>(cross(bMinusA, a - point))/bMinusA.dot();
        }

        /**
         * @brief Distance of line and point in 3D
         * @param a         First point of the line
         * @param b         Second point of the line
         * @param point     Point
         *
         * The distance *d* is computed from point **p** and line defined by **a**
         * and **b** using @ref cross(const Vector3<T>&, const Vector3<T>&) "cross product": @f[
         *      d = \frac{|(\boldsymbol p - \boldsymbol a) \times (\boldsymbol p - \boldsymbol b)|}
         *      {|\boldsymbol b - \boldsymbol a|}
         * @f]
         * Source: http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
         * @see @ref linePointSquared(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
         */
        template<class T> static T linePoint(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
            return std::sqrt(linePointSquared(a, b, point));
        }

        /**
         * @brief Distance of line and point in 3D, squared
         *
         * More efficient than @ref linePoint(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
         * for comparing distance with other values, because it doesn't
         * compute the square root.
         */
        template<class T> static T linePointSquared(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
            return cross(point - a, point - b).dot()/(b - a).dot();
        }

        /**
         * @brief Dístance of point from line segment in 2D
         * @param a         Starting point of the line
         * @param b         Ending point of the line
         * @param point     Point
         *
         * Returns distance of point from line segment or from its
         * starting/ending point, depending on where the point lies.
         *
         * Determining whether the point lies next to line segment or outside
         * is done using Pythagorean theorem. If the following equation
         * applies, the point **p** lies outside line segment closer to **a**: @f[
         *      |\boldsymbol p - \boldsymbol b|^2 > |\boldsymbol b - \boldsymbol a|^2 + |\boldsymbol p - \boldsymbol a|^2
         * @f]
         * On the other hand, if the following equation applies, the point
         * lies outside line segment closer to **b**: @f[
         *      |\boldsymbol p - \boldsymbol a|^2 > |\boldsymbol b - \boldsymbol a|^2 + |\boldsymbol p - \boldsymbol b|^2
         * @f]
         * The last alternative is when the following equation applies. The
         * point then lies between **a** and **b** and the distance is
         * computed the same way as in @ref linePoint(). @f[
         *      |\boldsymbol b - \boldsymbol a|^2 > |\boldsymbol p - \boldsymbol a|^2 + |\boldsymbol p - \boldsymbol b|^2
         * @f]
         *
         * @see @ref lineSegmentPointSquared()
         */
        template<class T> static T lineSegmentPoint(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point);

        /**
         * @brief Distance of point from line segment in 2D, squared
         *
         * More efficient than @ref lineSegmentPoint() for comparing distance
         * with other values, because it doesn't compute the square root.
         */
        template<class T> static T lineSegmentPointSquared(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point);

        /**
         * @brief Dístance of point from line segment in 3D
         * @param a         Starting point of the line
         * @param b         Ending point of the line
         * @param point     Point
         *
         * Similar to 2D implementation
         * @ref lineSegmentPoint(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&).
         *
         * @see @ref lineSegmentPointSquared(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
         */
        template<class T> static T lineSegmentPoint(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
            return std::sqrt(lineSegmentPointSquared(a, b, point));
        }

        /**
         * @brief Distance of point from line segment in 3D, squared
         *
         * More efficient than
         * @ref lineSegmentPoint(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
         * for comparing distance with other values, because it doesn't compute
         * the square root.
         */
        template<class T> static T lineSegmentPointSquared(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point);
};

template<class T> T Distance::lineSegmentPoint(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
    const Vector2<T> pointMinusA = point - a;
    const Vector2<T> pointMinusB = point - b;
    const Vector2<T> bMinusA = b - a;
    const T pointDistanceA = pointMinusA.dot();
    const T pointDistanceB = pointMinusB.dot();
    const T bDistanceA = bMinusA.dot();

    /* Point is before A */
    if(pointDistanceB > bDistanceA + pointDistanceA)
        return std::sqrt(pointDistanceA);

    /* Point is after B */
    if(pointDistanceA > bDistanceA + pointDistanceB)
        return std::sqrt(pointDistanceB);

    /* Between A and B */
    return std::abs(cross(bMinusA, -pointMinusA))/std::sqrt(bDistanceA);
}

template<class T> T Distance::lineSegmentPointSquared(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
    const Vector2<T> pointMinusA = point - a;
    const Vector2<T> pointMinusB = point - b;
    const Vector2<T> bMinusA = b - a;
    const T pointDistanceA = pointMinusA.dot();
    const T pointDistanceB = pointMinusB.dot();
    const T bDistanceA = bMinusA.dot();

    /* Point is before A */
    if(pointDistanceB > bDistanceA + pointDistanceA)
        return pointDistanceA;

    /* Point is after B */
    if(pointDistanceA > bDistanceA + pointDistanceB)
        return pointDistanceB;

    /* Between A and B */
    return Math::pow<2>(cross(bMinusA, -pointMinusA))/bDistanceA;
}

template<class T> T Distance::lineSegmentPointSquared(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
    const Vector3<T> pointMinusA = point - a;
    const Vector3<T> pointMinusB = point - b;
    const T pointDistanceA = pointMinusA.dot();
    const T pointDistanceB = pointMinusB.dot();
    const T bDistanceA = (b - a).dot();

    /* Point is before A */
    if(pointDistanceB > bDistanceA + pointDistanceA)
        return pointDistanceA;

    /* Point is after B */
    if(pointDistanceA > bDistanceA + pointDistanceB)
        return pointDistanceB;

    /* Between A and B */
    return cross(pointMinusA, pointMinusB).dot()/bDistanceA;
}

}}}

#endif
