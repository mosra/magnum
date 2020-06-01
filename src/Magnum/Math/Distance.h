#ifndef Magnum_Math_Distance_h
#define Magnum_Math_Distance_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Namespace @ref Magnum::Math::Distance
 */

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Vector4.h"

namespace Magnum { namespace Math { namespace Distance {

/**
@brief Distance of line and point in 2D, squared
@param a        First point of the line
@param b        Second point of the line
@param point    Point

More efficient than @ref linePoint(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
for comparing distance with other values, because it doesn't calculate the
square root.
*/
template<class T> inline T linePointSquared(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
    const Vector2<T> bMinusA = b - a;
    return Math::pow<2>(cross(bMinusA, a - point))/bMinusA.dot();
}

/**
@brief Distance of line and point in 2D
@param a        First point of the line
@param b        Second point of the line
@param point    Point

The distance @f$ d @f$ is calculated from point @f$ \boldsymbol{p} @f$ and line
defined by @f$ \boldsymbol{a} @f$ and @f$ \boldsymbol{b} @f$ using
@ref cross(const Vector2<T>&, const Vector2<T>&) "perp-dot product": @f[
    d = \frac{|(\boldsymbol b - \boldsymbol a)_\bot \cdot (\boldsymbol a - \boldsymbol p)|}{|\boldsymbol b - \boldsymbol a|}
@f]
Source: http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
@see @ref linePointSquared(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&)
*/
template<class T> inline T linePoint(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
    const Vector2<T> bMinusA = b - a;
    return std::abs(cross(bMinusA, a - point))/bMinusA.length();
}

/**
@brief Distance of line and point in 3D, squared

More efficient than @ref linePoint(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
for comparing distance with other values, because it doesn't calculate the
square root.
*/
template<class T> inline T linePointSquared(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
    return cross(point - a, point - b).dot()/(b - a).dot();
}

/**
@brief Distance of line and point in 3D
@param a        First point of the line
@param b        Second point of the line
@param point    Point

The distance @f$ d @f$ is calculated from point @f$ \boldsymbol{p} @f$ and line
defined by @f$ \boldsymbol{a} @f$ and @f$ \boldsymbol{b} @f$ using
@ref cross(const Vector3<T>&, const Vector3<T>&) "cross product": @f[
    d = \frac{|(\boldsymbol p - \boldsymbol a) \times (\boldsymbol p - \boldsymbol b)|}{|\boldsymbol b - \boldsymbol a|}
@f]
Source: http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
@see @ref linePointSquared(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
*/
template<class T> inline T linePoint(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
    return std::sqrt(linePointSquared(a, b, point));
}

/**
@brief Distance of point from line segment in 2D, squared

More efficient than @ref lineSegmentPoint() for comparing distance with other
values, because it doesn't calculate the square root.
*/
template<class T> T lineSegmentPointSquared(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point);

/**
@brief Dístance of point from line segment in 2D
@param a        Starting point of the line
@param b        Ending point of the line
@param point    Point

Returns distance of point from line segment or from its starting/ending point,
depending on where the point lies.

Determining whether the point lies next to line segment or outside is done
using Pythagorean theorem. If the following equation applies, the point
@f$ \boldsymbol{p} @f$ lies outside line segment closer to @f$ \boldsymbol{a} @f$: @f[
    |\boldsymbol p - \boldsymbol b|^2 > |\boldsymbol b - \boldsymbol a|^2 + |\boldsymbol p - \boldsymbol a|^2
@f]
On the other hand, if the following equation applies, the point lies outside
line segment closer to @f$ \boldsymbol{b} @f$: @f[
    |\boldsymbol p - \boldsymbol a|^2 > |\boldsymbol b - \boldsymbol a|^2 + |\boldsymbol p - \boldsymbol b|^2
@f]
The last alternative is when the following equation applies. The point then
lies between @f$ \boldsymbol{a} @f$ and @f$ \boldsymbol{b} @f$ and the distance
is calculated the same way as in @ref linePoint(). @f[
    |\boldsymbol b - \boldsymbol a|^2 > |\boldsymbol p - \boldsymbol a|^2 + |\boldsymbol p - \boldsymbol b|^2
@f]

@see @ref lineSegmentPointSquared()
*/
template<class T> T lineSegmentPoint(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point);

/**
@brief Distance of point from line segment in 3D, squared

More efficient than @ref lineSegmentPoint(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
for comparing distance with other values, because it doesn't calculate the
square root.
*/
template<class T> T lineSegmentPointSquared(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point);

/**
@brief Dístance of point from line segment in 3D
@param a         Starting point of the line
@param b         Ending point of the line
@param point     Point

Similar to 2D implementation @ref lineSegmentPoint(const Vector2<T>&, const Vector2<T>&, const Vector2<T>&).
@see @ref lineSegmentPointSquared(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&)
*/
template<class T> inline T lineSegmentPoint(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
    return std::sqrt(lineSegmentPointSquared(a, b, point));
}

/**
@brief Distance of point from plane, scaled by the length of the planes normal

The distance @f$ d @f$ is calculated from point @f$ \boldsymbol{p} @f$ and
plane with normal @f$ \boldsymbol{n} @f$ and @f$ w @f$ using: @f[
    d = \boldsymbol{p} \cdot \boldsymbol{n} + w
@f]
The distance is negative if the point lies behind the plane.

More efficient than @ref pointPlane() when merely the sign of the distance is
of interest, for example when testing on which half space of the plane the
point lies.
@see @ref planeEquation(), @ref pointPlaneNormalized()
*/
template<class T> inline T pointPlaneScaled(const Vector3<T>& point, const Vector4<T>& plane) {
    return dot(plane.xyz(), point) + plane.w();
}

/**
@brief Distance of point from plane

The distance @f$ d @f$ is calculated from point @f$ \boldsymbol{p} @f$ and
plane with normal @f$ \boldsymbol{n} @f$ and @f$ w @f$ using: @f[
    d = \frac{\boldsymbol{p} \cdot \boldsymbol{n} + w}{\left| \boldsymbol{n} \right|}
@f]
The distance is negative if the point lies behind the plane.

In cases where the planes normal is a unit vector, @ref pointPlaneNormalized()
is more efficient. If merely the sign of the distance is of interest,
@ref pointPlaneScaled() is more efficient.
@see @ref planeEquation()
*/
template<class T> inline T pointPlane(const Vector3<T>& point, const Vector4<T>& plane) {
    return pointPlaneScaled<T>(point, plane)/plane.xyz().length();
}

/**
@brief Distance of point from plane with normalized normal

The distance @f$ d @f$ is calculated from point @f$ \boldsymbol{p} @f$ and plane
with normal @f$ \boldsymbol{n} @f$ and @f$ w @f$ using: @f[
    d = \boldsymbol{p} \cdot \boldsymbol{n} + w
@f]
The distance is negative if the point lies behind the plane. Expects that
@p plane normal is normalized.

More efficient than @ref pointPlane() in cases where the plane's normal is
normalized. Equivalent to @ref pointPlaneScaled() but with assertion added on
top.
@see @ref planeEquation()
*/
template<class T> inline T pointPlaneNormalized(const Vector3<T>& point, const Vector4<T>& plane) {
    CORRADE_ASSERT(plane.xyz().isNormalized(),
        "Math::Distance::pointPlaneNormalized(): plane normal" << plane.xyz() << "is not normalized", {});
    return pointPlaneScaled<T>(point, plane);
}

template<class T> T lineSegmentPoint(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
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

template<class T> T lineSegmentPointSquared(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& point) {
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

template<class T> T lineSegmentPointSquared(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& point) {
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
