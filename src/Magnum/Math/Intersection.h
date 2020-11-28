#ifndef Magnum_Math_Intersection_h
#define Magnum_Math_Intersection_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016, 2018 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2020 janos <janos.meny@googlemail.com>

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
 * @brief Namespace @ref Magnum::Math::Intersection
 */

#include "Magnum/Math/Distance.h"
#include "Magnum/Math/Frustum.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Matrix4.h"

namespace Magnum { namespace Math { namespace Intersection {

/**
@brief Intersection of two line segments in 2D
@param p        Starting point of first line segment
@param r        Direction of first line segment
@param q        Starting point of second line segment
@param s        Direction of second line segment

Returns intersection point positions @f$ t @f$, @f$ u @f$ on both lines:

-   @f$ t, u = \mathrm{NaN} @f$ if the lines are collinear
-   @f$ t \in [ 0 ; 1 ] @f$ if the intersection is inside the line segment
    defined by @f$ \boldsymbol{p} @f$ and @f$ \boldsymbol{p} + \boldsymbol{r} @f$
-   @f$ t \notin [ 0 ; 1 ] @f$ if the intersection is outside the line segment
-   @f$ u \in [ 0 ; 1 ] @f$ if the intersection is inside the line segment
    defined by @f$ \boldsymbol{q} @f$ and @f$ \boldsymbol{q} + \boldsymbol{s} @f$
-   @f$ u \notin [ 0 ; 1 ] @f$ if the intersection is outside the line segment
-   @f$ t, u \in \{-\infty, \infty\} @f$ if the intersection doesn't exist (the
    2D lines are parallel)

The two lines intersect if @f$ t @f$ and @f$ u @f$ exist such that: @f[
     \boldsymbol{p} + t \boldsymbol{r} = \boldsymbol{q} + u \boldsymbol{s}
@f]
Crossing both sides with @f$ \boldsymbol{s} @f$, distributing the cross product
and eliminating @f$ \boldsymbol{s} \times \boldsymbol{s} = 0 @f$, then solving
for @f$ t @f$ and similarly for @f$ u @f$: @f[
     \begin{array}{rcl}
         (\boldsymbol{p} + t \boldsymbol{r}) \times \boldsymbol{s} & = & (\boldsymbol{q} + u \boldsymbol{s}) \times \boldsymbol{s} \\
         t (\boldsymbol{r} \times \boldsymbol{s}) & = & (\boldsymbol{q} - \boldsymbol{p}) \times \boldsymbol{s} \\
         t & = & \cfrac{(\boldsymbol{q} - \boldsymbol{p}) \times \boldsymbol{s}}{\boldsymbol{r} \times \boldsymbol{s}} \\
         u & = & \cfrac{(\boldsymbol{q} - \boldsymbol{p}) \times \boldsymbol{r}}{\boldsymbol{r} \times \boldsymbol{s}}
     \end{array}
@f]

See also @ref lineSegmentLine() which calculates only @f$ t @f$, useful if you
don't need to test that the intersection lies inside line segment defined by
@f$ \boldsymbol{q} @f$ and @f$ \boldsymbol{q} + \boldsymbol{s} @f$.

@see @ref isInf(), @ref isNan()
*/
template<class T> inline std::pair<T, T> lineSegmentLineSegment(const Vector2<T>& p, const Vector2<T>& r, const Vector2<T>& q, const Vector2<T>& s) {
    const Vector2<T> qp = q - p;
    const T rs = cross(r, s);
    return {cross(qp, s)/rs, cross(qp, r)/rs};
}

/**
@brief Intersection of line segment and line in 2D
@param p        Starting point of first line segment
@param r        Direction of first line segment
@param q        Starting point of second line
@param s        Direction of second line

Returns intersection point position @f$ t @f$ on the first line:

-   @f$ t = \mathrm{NaN} @f$ if the lines are collinear
-   @f$ t \in [ 0 ; 1 ] @f$ if the intersection is inside the line segment
    defined by @f$ \boldsymbol{p} @f$ and @f$ \boldsymbol{p} + \boldsymbol{r} @f$
-   @f$ t \notin [ 0 ; 1 ] @f$ if the intersection is outside the line segment
-   @f$ t \in \{-\infty, \infty\} @f$ if the intersection doesn't exist (the 2D
    lines are parallel)

Unlike @ref lineSegmentLineSegment() calculates only @f$ t @f$.

@see @ref isInf(), @ref isNan()
*/
template<class T> inline T lineSegmentLine(const Vector2<T>& p, const Vector2<T>& r, const Vector2<T>& q, const Vector2<T>& s) {
    return cross(q - p, s)/cross(r, s);
}

/**
@brief Intersection of a plane and line
@param plane            Plane equation
@param p                Starting point of the line
@param r                Direction of the line

Returns intersection point position @f$ t @f$ on the line:

-   @f$ t = \mathrm{NaN} @f$ if the line lies on the plane
-   @f$ t \in [ 0 ; 1 ] @f$ if the intersection is inside the line segment
    defined by @f$ \boldsymbol{p} @f$ and @f$ \boldsymbol{p} + \boldsymbol{r} @f$
-   @f$ t \notin [ 0 ; 1 ] @f$ if the intersection is outside the line segment
-   @f$ t \in \{-\infty, \infty\} @f$ if the intersection doesn't exist

Using the plane equation @f$ ax + by + cz + d = 0 @f$ with @f$ \boldsymbol{n} @f$
defined as @f$ \boldsymbol{n} = (a, b, c)^T @f$ and a line defined by
@f$ \boldsymbol{p} @f$ and @f$ \boldsymbol{r} @f$, value of @f$ t @f$ is
calculated as: @f[
    t = \cfrac{-d - \boldsymbol{n} \cdot \boldsymbol{p}}{\boldsymbol{n} \cdot \boldsymbol{r}}
@f]

@see @ref planeEquation(), @ref isInf(), @ref isNan()
*/
template<class T> inline T planeLine(const Vector4<T>& plane, const Vector3<T>& p, const Vector3<T>& r) {
    return (-plane.w() - dot(plane.xyz(), p))/dot(plane.xyz(), r);
}

/**
@brief Intersection of a point and a frustum
@param point    Point
@param frustum  Frustum planes with normals pointing outwards
@return @cpp true @ce if the point is on or inside the frustum, @cpp false @ce
    otherwise

Checks for each plane of the frustum whether the point is behind the plane (the
points distance from the plane is negative) using @ref Distance::pointPlaneScaled().
*/
template<class T> bool pointFrustum(const Vector3<T>& point, const Frustum<T>& frustum);

/**
@brief Intersection of a range and a frustum
@param range    Range
@param frustum  Frustum planes with normals pointing outwards
@return @cpp true @ce if the box intersects with the frustum, @cpp false @ce
    otherwise

Uses the "p/n-vertex" approach: First converts the @ref Range3D into a
representation using center and extent which allows using the following
condition for whether the plane is intersecting the box: @f[
     \begin{array}{rcl}
         d & = & \boldsymbol c \cdot \boldsymbol n \\
         r & = & \boldsymbol c \cdot \text{abs}(\boldsymbol n) \\
         d + r & < & -w
     \end{array}
@f]

for plane normal @f$ \boldsymbol n @f$ and determinant @f$ w @f$.

@see @ref aabbFrustum()
*/
template<class T> bool rangeFrustum(const Range3D<T>& range, const Frustum<T>& frustum);

/**
@brief Intersection of a ray with a range.
@param rayOrigin            Origin of the ray
@param inverseRayDirection  Component-wise inverse of the ray direction
@param range                Range
@return @cpp true @ce if the the ray intersects the range, @cpp false @ce
    otherwise
@m_since_latest

Note that you need to pass the inverse ray direction and not the ray direction.
The purpose for this is to reduce the number of times you have to compute
a ray inverse, when doing multiple ray / range intersections (for example
when traversing an AABB tree). The algorithm implemented is a version of the
classical slabs algorithm, see *Listing 1* in
[Majercik et al.](http://jcgt.org/published/0007/03/04/).
*/
template<class T> bool rayRange(const Vector3<T>& rayOrigin, const Vector3<T>& inverseRayDirection, const Range3D<T>& range);

/**
@brief Intersection of an axis-aligned box and a frustum
@param aabbCenter   Center of the AABB
@param aabbExtents  (Half-)extents of the AABB
@param frustum      Frustum planes with normals pointing outwards
@return @cpp true @ce if the box intersects with the frustum, @cpp false @ce
    otherwise

Uses the same method as @ref rangeFrustum(), but does not need to convert to
center/extents representation.
*/
template<class T> bool aabbFrustum(const Vector3<T>& aabbCenter, const Vector3<T>& aabbExtents, const Frustum<T>& frustum);

/**
@brief Intersection of a sphere and a frustum
@param sphereCenter Sphere center
@param sphereRadius Sphere radius
@param frustum      Frustum planes with normals pointing outwards
@return @cpp true @ce if the sphere intersects the frustum, @cpp false @ce
    otherwise

Checks for each plane of the frustum whether the sphere is behind the plane
(the points distance larger than the sphere's radius) using
@ref Distance::pointPlaneScaled().
*/
template<class T> bool sphereFrustum(const Vector3<T>& sphereCenter, T sphereRadius, const Frustum<T>& frustum);

/**
@brief Intersection of a point and a cone
@param point        The point
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param coneAngle    Apex angle of the cone (@f$ 0 < \Theta < \pi @f$)
@return @cpp true @ce if the point is inside the cone, @cpp false @ce otherwise

Precomputes a portion of the intersection equation from @p coneAngle and calls
@ref pointCone(const Vector3&, const Vector3&, const Vector3&, T).
*/
template<class T> bool pointCone(const Vector3<T>& point, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, Rad<T> coneAngle);

/**
@brief Intersection of a point and a cone using precomputed values
@param point        The point
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param tanAngleSqPlusOne Precomputed portion of the cone intersection equation
@return @cpp true @ce if the point is inside the cone, @cpp false @ce
    otherwise

The @p tanAngleSqPlusOne parameter can be precomputed like this:

@snippet MagnumMath.cpp Intersection-tanAngleSqPlusOne
*/
template<class T> bool pointCone(const Vector3<T>& point, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, T tanAngleSqPlusOne);

/**
@brief Intersection of a point and a double cone
@param point        The point
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param coneAngle    Apex angle of the cone (@f$ 0 < \Theta < \pi @f$)
@return @cpp true @ce if the point is inside the double cone, @cpp false @ce
    otherwise

Precomputes a portion of the intersection equation from @p coneAngle and calls
@ref pointDoubleCone(const Vector3&, const Vector3&, const Vector3&, T).
*/
template<class T> bool pointDoubleCone(const Vector3<T>& point, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, Rad<T> coneAngle);

/**
@brief Intersection of a point and a double cone using precomputed values
@param point        The point
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param tanAngleSqPlusOne Precomputed portion of the cone intersection equation
@return @cpp true @ce if the point is inside the double cone, @cpp false @ce
    otherwise

The @p tanAngleSqPlusOne parameter can be precomputed like this:

@snippet MagnumMath.cpp Intersection-tanAngleSqPlusOne
*/
template<class T> bool pointDoubleCone(const Vector3<T>& point, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, T tanAngleSqPlusOne);

/**
@brief Intersection of a sphere and a cone view
@param sphereCenter Center of the sphere
@param sphereRadius Radius of the sphere
@param coneView     View matrix with translation and rotation of the cone
@param coneAngle    Apex angle of the cone (@f$ 0 < \Theta < \pi @f$)
@return @cpp true @ce if the sphere intersects the cone, @cpp false @ce
    otherwise

Precomputes a portion of the intersection equation from @p coneAngle and calls
@ref sphereConeView(const Vector3<T>&, T, const Matrix4<T>&, T, T).
*/
template<class T> bool sphereConeView(const Vector3<T>& sphereCenter, T sphereRadius, const Matrix4<T>& coneView, Rad<T> coneAngle);

/**
@brief Intersection of a sphere and a cone view
@param sphereCenter Sphere center
@param sphereRadius Sphere radius
@param coneView     View matrix with translation and rotation of the cone
@param sinAngle     Precomputed sine of half the cone's opening angle
@param tanAngle     Precomputed tangent of half the cone's opening angle
@return @cpp true @ce if the sphere intersects the cone, @cpp false @ce
    otherwise

Transforms the sphere center into cone space (using the cone view matrix) and
performs sphere-cone intersection with the zero-origin -Z axis-aligned cone.
The @p sinAngle and @p tanAngle can be precomputed like this:

@snippet MagnumMath.cpp Intersection-sinAngle-tanAngle
*/
template<class T> bool sphereConeView(const Vector3<T>& sphereCenter, T sphereRadius, const Matrix4<T>& coneView, T sinAngle, T tanAngle);

/**
@brief Intersection of a sphere and a cone
@param sphereCenter Sphere center
@param sphereRadius Sphere radius
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param coneAngle    Apex angle of the cone (@f$ 0 < \Theta < \pi @f$)
@return @cpp true @ce if the sphere intersects with the cone, @cpp false @ce
    otherwise

Precomputes a portion of the intersection equation from @p coneAngle and calls
@ref sphereCone(const Vector3<T>& sphereCenter, T, const Vector3<T>&, const Vector3<T>&, T, T).
*/
template<class T> bool sphereCone(const Vector3<T>& sphereCenter, T sphereRadius, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, Rad<T> coneAngle);

/**
@brief Intersection of a sphere and a cone using precomputed values
@param sphereCenter Sphere center
@param sphereRadius Sphere radius
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param sinAngle     Precomputed sine of half the cone's opening angle
@param tanAngleSqPlusOne Precomputed portion of the cone intersection equation
@return @cpp true @ce if the sphere intersects with the cone, @cpp false @ce
    otherwise

Offsets the cone plane by @f$ -r\sin{\frac{\Theta}{2}} \cdot \boldsymbol n @f$
(with @f$ \Theta @f$ being the cone apex angle) which separates two
half-spaces: In front of the plane, in which the sphere cone intersection test
is equivalent to testing the sphere's center against a similarly offset cone
(which is equivalent the cone with surface expanded by @f$ r @f$ in surface
normal direction), and behind the plane, where the test is equivalent to
testing whether the origin of the original cone intersects the sphere. The
@p sinAngle and @p tanAngleSqPlusOne parameters can be precomputed like this:

@snippet MagnumMath.cpp Intersection-sinAngle-tanAngleSqPlusOne
*/
template<class T> bool sphereCone(const Vector3<T>& sphereCenter, T sphereRadius, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, T sinAngle, T tanAngleSqPlusOne);

/**
@brief Intersection of an axis aligned bounding box and a cone
@param aabbCenter   Center of the AABB
@param aabbExtents  (Half-)extents of the AABB
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param coneAngle    Apex angle of the cone (@f$ 0 < \Theta < \pi @f$)
@return @cpp true @ce if the box intersects the cone, @cpp false @ce otherwise

Precomputes a portion of the intersection equation from @p coneAngle and calls
@ref aabbCone(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, T).
*/
template<class T> bool aabbCone(const Vector3<T>& aabbCenter, const Vector3<T>& aabbExtents, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, Rad<T> coneAngle);

/**
@brief Intersection of an axis aligned bounding box and a cone using precomputed values
@param aabbCenter   Center of the AABB
@param aabbExtents  (Half-)extents of the AABB
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param tanAngleSqPlusOne Precomputed portion of the cone intersection equation
@return @cpp true @ce if the box intersects the cone, @cpp false @ce otherwise

On each axis finds the intersection points of the cone's axis with infinite
planes obtained by extending the two faces of the box that are perpendicular to
that axis. The intersection points on the planes perpendicular to axis
@f$ a \in \{ 0, 1, 2 \} @f$ are given by @f[
    \boldsymbol i = \boldsymbol n \cdot \frac{(\boldsymbol c_a - \boldsymbol o_a) \pm \boldsymbol e_a}{\boldsymbol n_a}
@f]

with normal @f$ \boldsymbol n @f$, cone origin @f$ \boldsymbol o @f$, box
center @f$ \boldsymbol c @f$ and box extents @f$ \boldsymbol e @f$. The points
on the faces that are closest to this intersection point are the closest to the
cone's axis and are tested for intersection with the cone using
@ref pointCone(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, const T) "pointCone()". As soon as an intersecting point is found, the function returns
@cpp true @ce. If all points lie outside of the cone, it will return
@cpp false @ce.

The @p tanAngleSqPlusOne parameter can be precomputed like this:

@snippet MagnumMath.cpp Intersection-tanAngleSqPlusOne
*/
template<class T> bool aabbCone(const Vector3<T>& aabbCenter, const Vector3<T>& aabbExtents, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, T tanAngleSqPlusOne);

/**
@brief Intersection of a range and a cone
@param range        Range
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param coneAngle    Apex angle of the cone (@f$ 0 < \Theta < \pi @f$)
@return @cpp true @ce if the range intersects the cone, @cpp false @ce
    otherwise

Precomputes a portion of the intersection equation from @p coneAngle and calls
@ref rangeCone(const Range3D<T>&, const Vector3<T>&, const Vector3<T>&, T).
*/
template<class T> bool rangeCone(const Range3D<T>& range, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const Rad<T> coneAngle);

/**
@brief Intersection of a range and a cone using precomputed values
@param range        Range
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param tanAngleSqPlusOne Precomputed portion of the cone intersection equation
@return @cpp true @ce if the range intersects the cone, @cpp false @ce
    otherwise

Converts the range into center/extents representation and passes it on to
@ref aabbCone(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, T) "aabbCone()".
The @p tanAngleSqPlusOne parameter can be precomputed like this:

@snippet MagnumMath.cpp Intersection-tanAngleSqPlusOne
*/
template<class T> bool rangeCone(const Range3D<T>& range, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const T tanAngleSqPlusOne);

template<class T> bool pointFrustum(const Vector3<T>& point, const Frustum<T>& frustum) {
    for(const Vector4<T>& plane: frustum) {
        /* The point is in front of one of the frustum planes (normals point
           outwards) */
        if(Distance::pointPlaneScaled<T>(point, plane) < T(0))
            return false;
    }

    return true;
}

template<class T> bool rangeFrustum(const Range3D<T>& range, const Frustum<T>& frustum) {
    /* Convert to center/extent, avoiding division by 2 and instead comparing
       to 2*-plane.w() later */
    const Vector3<T> center = range.min() + range.max();
    const Vector3<T> extent = range.max() - range.min();

    for(const Vector4<T>& plane: frustum) {
        const Vector3<T> absPlaneNormal = Math::abs(plane.xyz());

        const Float d = Math::dot(center, plane.xyz());
        const Float r = Math::dot(extent, absPlaneNormal);
        if(d + r < -T(2)*plane.w()) return false;
    }

    return true;
}

template<class T> bool rayRange(const Vector3<T>& rayOrigin, const Vector3<T>& inverseRayDirection, const Range3D<T>& range) {
    const Vector3<T> t0 = (range.min() - rayOrigin)*inverseRayDirection;
    const Vector3<T> t1 = (range.max() - rayOrigin)*inverseRayDirection;
    const std::pair<Vector3<T>, Vector3<T>> tminMax = minmax(t0, t1);

    return tminMax.first.max() <= tminMax.second.min();
}

template<class T> bool aabbFrustum(const Vector3<T>& aabbCenter, const Vector3<T>& aabbExtents, const Frustum<T>& frustum) {
    for(const Vector4<T>& plane: frustum) {
        const Vector3<T> absPlaneNormal = Math::abs(plane.xyz());

        const Float d = Math::dot(aabbCenter, plane.xyz());
        const Float r = Math::dot(aabbExtents, absPlaneNormal);
        if(d + r < -plane.w()) return false;
    }

    return true;
}

template<class T> bool sphereFrustum(const Vector3<T>& sphereCenter, const T sphereRadius, const Frustum<T>& frustum) {
    const T radiusSq = sphereRadius*sphereRadius;

    for(const Vector4<T>& plane: frustum) {
        /* The sphere is in front of one of the frustum planes (normals point
           outwards) */
        if(Distance::pointPlaneScaled<T>(sphereCenter, plane) < -radiusSq)
            return false;
    }

    return true;
}


template<class T> bool pointCone(const Vector3<T>& point, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const Rad<T> coneAngle) {
    const T tanAngleSqPlusOne = Math::pow<2>(Math::tan(coneAngle*T(0.5))) + T(1);
    return pointCone(point, coneOrigin, coneNormal, tanAngleSqPlusOne);
}

template<class T> bool pointCone(const Vector3<T>& point, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const T tanAngleSqPlusOne) {
    const Vector3<T> c = point - coneOrigin;
    const T lenA = dot(c, coneNormal);

    return lenA >= 0 && c.dot() <= lenA*lenA*tanAngleSqPlusOne;
}

template<class T> bool pointDoubleCone(const Vector3<T>& point, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const Rad<T> coneAngle) {
    const T tanAngleSqPlusOne = Math::pow<2>(Math::tan(coneAngle*T(0.5))) + T(1);
    return pointDoubleCone(point, coneOrigin, coneNormal, tanAngleSqPlusOne);
}

template<class T> bool pointDoubleCone(const Vector3<T>& point, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const T tanAngleSqPlusOne) {
    const Vector3<T> c = point - coneOrigin;
    const T lenA = dot(c, coneNormal);

    return c.dot() <= lenA*lenA*tanAngleSqPlusOne;
}

template<class T> bool sphereConeView(const Vector3<T>& sphereCenter, const T sphereRadius, const Matrix4<T>& coneView, const Rad<T> coneAngle) {
    const Rad<T> halfAngle = coneAngle*T(0.5);
    const T sinAngle = Math::sin(halfAngle);
    const T tanAngle = Math::tan(halfAngle);

    return sphereConeView(sphereCenter, sphereRadius, coneView, sinAngle, tanAngle);
}

template<class T> bool sphereConeView(const Vector3<T>& sphereCenter, const T sphereRadius, const Matrix4<T>& coneView, const T sinAngle, const T tanAngle) {
    CORRADE_ASSERT(coneView.isRigidTransformation(),
        "Math::Intersection::sphereConeView(): coneView does not represent a rigid transformation:" << Corrade::Utility::Debug::newline << coneView, false);

    /* Transform the sphere so that we can test against Z axis aligned origin
       cone instead */
    const Vector3<T> center = coneView.transformPoint(sphereCenter);

    /* Test against plane which determines whether to test against shifted cone
       or center-sphere */
    if (-center.z() > -sphereRadius*sinAngle) {
        /* Point - axis aligned cone test, shifted so that the cone's surface
           is extended by the radius of the sphere */
        const T coneRadius = tanAngle*(center.z() - sphereRadius/sinAngle);
        return center.xy().dot() <= coneRadius*coneRadius;
    } else {
        /* Simple sphere point check */
        return center.dot() <= sphereRadius*sphereRadius;
    }

    return false;
}

template<class T> bool sphereCone(
    const Vector3<T>& sphereCenter, const T sphereRadius,
    const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const Rad<T> coneAngle)
{
    const Rad<T> halfAngle = coneAngle*T(0.5);
    const T sinAngle = Math::sin(halfAngle);
    const T tanAngleSqPlusOne = T(1) + Math::pow<T>(Math::tan<T>(halfAngle), T(2));

    return sphereCone(sphereCenter, sphereRadius, coneOrigin, coneNormal, sinAngle, tanAngleSqPlusOne);
}

template<class T> bool sphereCone(
    const Vector3<T>& sphereCenter, const T sphereRadius,
    const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal,
    const T sinAngle, const T tanAngleSqPlusOne)
{
    const Vector3<T> diff = sphereCenter - coneOrigin;

    /* Point - cone test */
    if(Math::dot(diff - sphereRadius*sinAngle*coneNormal, coneNormal) > T(0)) {
        const Vector3<T> c = sinAngle*diff + coneNormal*sphereRadius;
        const T lenA = Math::dot(c, coneNormal);

        return c.dot() <= lenA*lenA*tanAngleSqPlusOne;

    /* Simple sphere point check */
    } else return diff.dot() <= sphereRadius*sphereRadius;
}

template<class T> bool aabbCone(
    const Vector3<T>& aabbCenter, const Vector3<T>& aabbExtents,
    const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const Rad<T> coneAngle)
{
    const T tanAngleSqPlusOne = Math::pow<T>(Math::tan<T>(coneAngle*T(0.5)), T(2)) + T(1);
    return aabbCone(aabbCenter, aabbExtents, coneOrigin, coneNormal, tanAngleSqPlusOne);
}

template<class T> bool aabbCone(
    const Vector3<T>& aabbCenter, const Vector3<T>& aabbExtents,
    const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const T tanAngleSqPlusOne)
{
    const Vector3<T> c = aabbCenter - coneOrigin;

    for(const Int axis: {0, 1, 2}) {
        const Int z = axis;
        const Int x = (axis + 1) % 3;
        const Int y = (axis + 2) % 3;
        if(coneNormal[z] != T(0)) {
            const Float t0 = ((c[z] - aabbExtents[z])/coneNormal[z]);
            const Float t1 = ((c[z] + aabbExtents[z])/coneNormal[z]);

            const Vector3<T> i0 = coneNormal*t0;
            const Vector3<T> i1 = coneNormal*t1;

            for(const auto& i : {i0, i1}) {
                Vector3<T> closestPoint = i;

                if(i[x] - c[x] > aabbExtents[x]) {
                    closestPoint[x] = c[x] + aabbExtents[x];
                } else if(i[x] - c[x] < -aabbExtents[x]) {
                    closestPoint[x] = c[x] - aabbExtents[x];
                } /* Else: normal intersects within x bounds */

                if(i[y] - c[y] > aabbExtents[y]) {
                    closestPoint[y] = c[y] + aabbExtents[y];
                } else if(i[y] - c[y] < -aabbExtents[y]) {
                    closestPoint[y] = c[y] - aabbExtents[y];
                } /* Else: normal intersects within Y bounds */

                /* Found a point in cone and aabb */
                if(pointCone<T>(closestPoint, {}, coneNormal, tanAngleSqPlusOne))
                    return true;
            }
        } /* Else: normal will intersect one of the other planes */
    }

    return false;
}

template<class T> bool rangeCone(const Range3D<T>& range, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const Rad<T> coneAngle) {
    const T tanAngleSqPlusOne = Math::pow<2>(Math::tan(coneAngle*T(0.5))) + T(1);
    return rangeCone(range, coneOrigin, coneNormal, tanAngleSqPlusOne);
}

template<class T> bool rangeCone(const Range3D<T>& range, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const T tanAngleSqPlusOne) {
    const Vector3<T> center = (range.min() + range.max())*T(0.5);
    const Vector3<T> extents = (range.max() - range.min())*T(0.5);
    return aabbCone(center, extents, coneOrigin, coneNormal, tanAngleSqPlusOne);
}

}}}

#endif
