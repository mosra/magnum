#ifndef Magnum_Math_Geometry_Intersection_h
#define Magnum_Math_Geometry_Intersection_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016, 2018 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Namespace @ref Magnum::Math::Geometry::Intersection
 */

#include "Magnum/Math/Frustum.h"
#include "Magnum/Math/Geometry/Distance.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Matrix4.h"

namespace Magnum { namespace Math { namespace Geometry { namespace Intersection {

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
     \boldsymbol p + t \boldsymbol r = \boldsymbol q + u \boldsymbol s
@f]
Crossing both sides with @f$ \boldsymbol{s} @f$, distributing the cross product
and eliminating @f$ \boldsymbol s \times \boldsymbol s = 0 @f$, then solving
for @f$ t @f$ and similarly for @f$ u @f$: @f[
     \begin{array}{rcl}
         (\boldsymbol p + t \boldsymbol r) \times s & = & (\boldsymbol q + u \boldsymbol s) \times s \\
         t (\boldsymbol r \times s) & = & (\boldsymbol q - \boldsymbol p) \times s \\
         t & = & \cfrac{(\boldsymbol q - \boldsymbol p) \times s}{\boldsymbol r \times \boldsymbol s} \\
         u & = & \cfrac{(\boldsymbol q - \boldsymbol p) \times r}{\boldsymbol r \times \boldsymbol s}
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
@param planePosition    Plane position
@param planeNormal      Plane normal
@param p                Starting point of the line
@param r                Direction of the line

Returns intersection point position @f$ t @f$ on the line:

-   @f$ t = \mathrm{NaN} @f$ if the line lies on the plane
-   @f$ t \in [ 0 ; 1 ] @f$ if the intersection is inside the line segment
    defined by @f$ \boldsymbol{p} @f$ and @f$ \boldsymbol{p} + \boldsymbol{r} @f$
-   @f$ t \notin [ 0 ; 1 ] @f$ if the intersection is outside the line segment
-   @f$ t \in \{-\infty, \infty\} @f$ if the intersection doesn't exist

First the parameter @f$ f @f$ of parametric equation of the plane is calculated
from plane normal @f$ \boldsymbol{n} @f$ and plane position: @f[
     \begin{pmatrix} n_0 \\ n_1 \\ n_2 \end{pmatrix} \cdot
     \begin{pmatrix} x \\ y \\ z \end{pmatrix} - f = 0
@f]
Using plane normal @f$ \boldsymbol{n} @f$, parameter @f$ f @f$ and line defined
by @f$ \boldsymbol{p} @f$ and @f$ \boldsymbol{r} @f$, value of @f$ t @f$ is
calculated and returned. @f[
     \begin{array}{rcl}
         f & = & \boldsymbol n \cdot (\boldsymbol p + t \boldsymbol r) \\
         \Rightarrow t & = & \cfrac{f - \boldsymbol n \cdot \boldsymbol p}{\boldsymbol n \cdot \boldsymbol r}
     \end{array}
@f]

@see @ref isInf(), @ref isNan()
*/
template<class T> inline T planeLine(const Vector3<T>& planePosition, const Vector3<T>& planeNormal, const Vector3<T>& p, const Vector3<T>& r) {
    const T f = dot(planePosition, planeNormal);
    return (f - dot(planeNormal, p))/dot(planeNormal, r);
}

/**
@brief Intersection of a point and a frustum
@param point    Point
@param frustum  Frustum planes with normals pointing outwards

Returns @cpp true @ce if the point is on or inside the frustum.

Checks for each plane of the frustum whether the point is behind the plane (the
points distance from the plane is negative) using @ref Distance::pointPlaneScaled().
*/
template<class T> bool pointFrustum(const Vector3<T>& point, const Frustum<T>& frustum);

/**
@brief Intersection of a range and a frustum
@param range    Range
@param frustum  Frustum planes with normals pointing outwards

Returns @cpp true @ce if the box intersects with the frustum.

Uses the "p/n-vertex" approach: First converts the @ref Range3D into a representation
using center and extent which allows using the following condition for whether the
plane is intersecting the box: @f[
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

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief rangeFrustum()
@deprecated Use @ref rangeFrustum() instead.
*/
template<class T> CORRADE_DEPRECATED("use rangeFrustum() instead") bool boxFrustum(const Range3D<T>& box, const Frustum<T>& frustum) {
    return rangeFrustum(box, frustum);
}
#endif

/**
@brief Intersection of an axis-aligned box and a frustum
@param box      Axis-aligned box
@param frustum  Frustum planes with normals pointing outwards

Returns @cpp true @ce if the box intersects with the frustum.

Uses the same method as @ref rangeFrustum() "rangeFrustum()", but does not need to
convert to center/extents representation.
*/
template<class T> bool aabbFrustum(const Vector3<T>& center, const Vector3<T>& extents, const Frustum<T>& frustum);

/**
@brief Intersection of a sphere and a frustum
@param center   Sphere center
@param radius   Sphere radius
@param frustum  Frustum planes with normals pointing outwards

Returns @cpp true @ce if the sphere intersects the frustum.

Checks for each plane of the frustum whether the sphere is behind the plane (the
points distance larger than the sphere's radius) using @ref Distance::pointPlaneScaled().
*/
template<class T> bool sphereFrustum(const Vector3<T>& center, T radius, const Frustum<T>& frustum);

/**
@brief Intersection of a point and a cone
@param p        The point
@param origin   Cone origin
@param normal   Cone normal
@param angle    Apex angle of the cone

Returns @cpp true @ce if the point is inside the cone.

Precomputes a portion of the intersection equation from @p angle and calls
@ref pointCone(const Vector3&, const Vector3&, const Vector3&, T)
*/
template<class T> bool pointCone(const Vector3<T>& p, const Vector3<T>& origin, const Vector3<T>& normal, Rad<T> angle);

/**
@brief Intersection of a point and a cone using precomputed values
@param p            The point
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param tanAngleSqPlusOne Precomputed portion of the cone intersection equation:
                         @cpp T(1) + Math::pow(Math::tan(angle/T(2)), T(2)) @ce

Returns @cpp true @ce if the point is inside the cone.
*/
template<class T> bool pointCone(const Vector3<T>& p, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, T tanAngleSqPlusOne);

/**
@brief Intersection of a point and a double cone
@param p            The point
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param coneAngle    Apex angle of the cone

Returns @cpp true @ce if the point is inside the double cone.

Precomputes a portion of the intersection equation from @p angle and calls
@ref pointDoubleCone(const Vector3&, const Vector3&, const Vector3&, T)
*/
template<class T> bool pointDoubleCone(const Vector3<T>& p, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, Rad<T> coneAngle);

/**
@brief Intersection of a point and a double cone using precomputed values
@param p            The point
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param tanAngleSqPlusOne Precomputed portion of the cone intersection equation:
                         @cpp T(1) + Math::pow(Math::tan(angle/T(2)), T(2)) @ce

Returns @cpp true @ce if the point is inside the double cone.

Uses the result of precomputing @f$ x = \tan^2{\theta} + 1 @f$.
*/
template<class T> bool pointDoubleCone(const Vector3<T>& p, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, T tanAngleSqPlusOne);

/**
@brief Intersection of a sphere and a cone view
@param sphereCenter   Center of the sphere
@param radius         Radius of the sphere
@param coneView       View matrix with translation and rotation of the cone
@param coneAngle      Cone opening angle

Returns @cpp true @ce if the sphere intersects the cone.

Transforms the sphere center into cone space (using the cone view matrix) and
performs sphere-cone intersection with the zero-origin -Z axis-aligned cone.

Precomputes a portion of the intersection equation from @p angle and calls
@ref sphereConeView(const Vector3<T>&, T, const Matrix4<T>&, T, T, T)
*/
template<class T> bool sphereConeView(const Vector3<T>& sphereCenter, T radius, const Matrix4<T>& coneView, Rad<T> coneAngle);

/**
@brief Intersection of a sphere and a cone view
@param sphereCenter Sphere center
@param radius   Sphere radius
@param coneView View matrix with translation and rotation of the cone
@param sinAngle Precomputed sine of half the cone's opening angle: @cpp Math::sin(angle/T(2)) @ce
@param cosAngle Precomputed cosine of half the cone's opening angle: @cpp Math::cos(angle/T(2)) @ce
@param tanAngle Precomputed tangens of half the cone's opening angle: @cpp Math::tan(angle/T(2)) @ce

Returns @cpp true @ce if the sphere intersects the cone.
*/
template<class T> bool sphereConeView(const Vector3<T>& sphereCenter, T radius, const Matrix4<T>& coneView, T sinAngle, T cosAngle, T tanAngle);

/**
@brief Intersection of a sphere and a cone
@param sphereCenter   Sphere center
@param radius Sphere  Sphere radius
@param coneOrigin     Cone origin
@param coneNormal     Cone normal
@param coneAngle      Cone opening angle (@f$ 0 < \text{angle} < \pi @f$).

Returns @cpp true @ce if the sphere intersects with the cone.

Offsets the cone plane by @f$ -r\sin{\theta} \cdot \boldsymbol n @f$ (with  @f$ \theta @f$
the cone's half-angle) which separates two half-spaces:
In front of the plane, in which the sphere cone intersection test is equivalent
to testing the sphere's center against a similarly offset cone (which is equivalent
the cone with surface expanded by @f$ r @f$ in surface normal direction), and
behind the plane, where the test is equivalent to testing whether the origin of
the original cone intersects the sphere.

Precomputes a portion of the intersection equation from @p angle and calls
@ref sphereCone(const Vector3<T>& sphereCenter, T, const Vector3<T>&, const Vector3<T>&, T, T)
*/
template<class T> bool sphereCone(const Vector3<T>& sphereCenter, T radius, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, Rad<T> coneAngle);

/**
@brief Intersection of a sphere and a cone using precomputed values
@param sphereCenter Sphere center
@param radius       Sphere radius
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param sinAngle     Precomputed sine of half the cone's opening angle:
                    @cpp Math::sin(angle/T(2)) @ce
@param tanAngleSqPlusOne Precomputed portion of the cone intersection equation:
                         @cpp Math::pow(Math::tan(angle/T(2)), 2) + 1 @ce

Returns @cpp true @ce if the sphere intersects with the cone.
*/
template<class T> bool sphereCone(const Vector3<T>& sphereCenter, T radius, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, T sinAngle, T tanAngleSqPlusOne);

/**
@brief Intersection of an axis aligned bounding box and a cone
@param center      Center of the AABB
@param extents     (Half-)extents of the AABB
@param coneOrigin  Cone origin
@param coneNormal  Cone normal
@param coneAngle   Cone opening angle

Returns @cpp true @ce if the axis aligned bounding box intersects the cone.

On each axis finds the intersection points of the cone's axis with infinite planes
obtained by extending the two faces of the box that are perpendicular to that axis.

The intersection points on the planes perpendicular to axis @f$ a \in {0, 1, 2} @f$
are given by @f[
    \boldsymbol i = \boldsymbol n \cdot \frac{(\boldsymbol c_a - \boldsymbol o_a) \pm \boldsymbol e_a}{\boldsymbol n_a}
@f]

with normal @f$ n @f$, cone origin @f$ o @f$, box center @f$ x @f$ and box extents @f$ e @f$.

The points on the faces that are closest to this intersection point are the closest
to the cone's axis and are tested for intersection with the cone using
@ref pointCone(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, const T) "pointCone()".

As soon as an intersecting point is found, the function returns @cpp true @ce.
If all points lie outside of the cone, it will return @cpp false @ce.

Precomputes a portion of the intersection equation from @p angle and calls
@ref aabbCone(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, T)
*/
template<class T> bool aabbCone(const Vector3<T>& center, const Vector3<T>& extents, const Vector3<T>& origin, const Vector3<T>& normal, Rad<T> angle);

/**
@brief Intersection of an axis aligned bounding box and a cone using precomputed values
@param center Center of the AABB
@param extents (Half-)extents of the AABB
@param origin Cone origin
@param normal Cone normal
@param tanAngleSqPlusOne Precomputed portion of the cone intersection equation:
                         @cpp T(1) + Math::pow(Math::tan(angle/T(2)), T(2)) @ce

Returns @cpp true @ce if the axis aligned bounding box intersects the cone.
*/
template<class T> bool aabbCone(const Vector3<T>& center, const Vector3<T>& extents, const Vector3<T>& origin, const Vector3<T>& normal, T tanAngleSqPlusOne);

/**
@brief Intersection of a range and a cone

@param range        Range
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param coneAngle    Cone opening angle

Returns @cpp true @ce if the range intersects the cone.

Converts the range into center/extents representation and passes it on to
@ref aabbCone(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, T) "aabbCone()".
*/
template<class T> bool rangeCone(const Range3D<T>& range, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const Rad<T> angle);

/**
@brief Intersection of a range and a cone using precomputed values
@param range        Range
@param coneOrigin   Cone origin
@param coneNormal   Cone normal
@param tanAngleSqPlusOne Precomputed portion of the cone intersection equation:
                         @cpp T(1) + Math::pow(Math::tan(angle/T(2)), T(2)) @ce

Returns @cpp true @ce if the range intersects the cone.

Converts the range into center/extents representation and passes it on to
@ref aabbCone(const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, const Vector3<T>&, T) "aabbCone()".
*/
template<class T> bool rangeCone(const Range3D<T>& range, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const T tanAngleSqPlusOne);

template<class T> bool pointFrustum(const Vector3<T>& point, const Frustum<T>& frustum) {
    for(const Vector4<T>& plane: frustum.planes()) {
        /* The point is in front of one of the frustum planes (normals point
           outwards) */
        if(Distance::pointPlaneScaled<T>(point, plane) < T(0))
            return false;
    }

    return true;
}

template<class T> bool rangeFrustum(const Range3D<T>& box, const Frustum<T>& frustum) {
    /* Convert to center/extent, avoiding division by 2 and instead comparing
       to 2*-plane.w() later */
    const Vector3<T> center = box.min() + box.max();
    const Vector3<T> extent = box.max() - box.min();

    for(const Vector4<T>& plane: frustum.planes()) {
        const Vector3<T> absPlaneNormal = Math::abs(plane.xyz());

        const Float d = Math::dot(center, plane.xyz());
        const Float r = Math::dot(extent, absPlaneNormal);
        if(d + r < -T(2)*plane.w()) {
            return false;
        }
    }

    return true;
}

template<class T> bool aabbFrustum(
    const Vector3<T>& center, const Vector3<T>& extents, const Frustum<T>& frustum)
{
    for(const Vector4<T>& plane: frustum.planes()) {
        const Vector3<T> absPlaneNormal = Math::abs(plane.xyz());

        const Float d = Math::dot(center, plane.xyz());
        const Float r = Math::dot(extents, absPlaneNormal);
        if(d + r < -plane.w()) {
            return false;
        }
    }

    return true;
}

template<class T> bool sphereFrustum(const Vector3<T>& center, const T radius, const Frustum<T>& frustum) {
    const T radiusSq = radius*radius;

    for(const Vector4<T>& plane: frustum.planes()) {
        /* The sphere is in front of one of the frustum planes (normals point
           outwards) */
        if(Distance::pointPlaneScaled<T>(center, plane) < -radiusSq)
            return false;
    }

    return true;
}


template<class T> bool pointCone(const Vector3<T>& p, const Vector3<T>& origin, const Vector3<T>& normal, const Rad<T> angle) {
    const T x = T(1) + Math::pow(Math::tan(angle/T(2)), T(2));

    return pointCone(p, origin, normal, x);
}

template<class T> bool pointCone(const Vector3<T>& p, const Vector3<T>& origin, const Vector3<T>& normal, const T tanAngleSqPlusOne) {
    const Vector3<T> c = p - origin;
    const T lenA = dot(c, normal);

    return lenA >= 0 && c.dot() <= lenA*lenA*tanAngleSqPlusOne;
}

template<class T> bool pointDoubleCone(const Vector3<T>& p, const Vector3<T>& origin, const Vector3<T>& normal, const Rad<T> angle) {
    const T x = T(1) + Math::pow(Math::tan(angle/T(2)), T(2));

    return pointDoubleCone(p, origin, normal, x);
}

template<class T> bool pointDoubleCone(const Vector3<T>& p, const Vector3<T>& origin, const Vector3<T>& normal, const T tanAngleSqPlusOne) {
    const Vector3<T> c = p - origin;
    const T lenA = dot(c, normal);

    return c.dot() <= lenA*lenA*tanAngleSqPlusOne;
}

template<class T> bool sphereConeView(const Vector3<T>& sphereCenter, const T radius, const Matrix4<T>& coneView, const Rad<T> angle) {
    const T sinAngle = Math::sin(angle/T(2));
    const T cosAngle = Math::cos(angle/T(2));
    const T tanAngle = Math::tan(angle/T(2));

    return sphereConeView(sphereCenter, radius, coneView, sinAngle, cosAngle, tanAngle);
}

template<class T> bool sphereConeView(
    const Vector3<T>& sphereCenter, const T radius, const Matrix4<T>& coneView,
    const T sinAngle, const T cosAngle, const T tanAngle)
{
    CORRADE_ASSERT(coneView.isRigidTransformation(), "Math::Geometry::Intersection::sphereConeView(): coneView must be rigid", false);

    /* Transform the sphere so that we can test against Z axis aligned origin cone instead */
    const Vector3<T> center = coneView.transformPoint(sphereCenter);

    /* Test against plane which determines whether to test against shifted cone or center-sphere */
    if (-center.z() > -radius*sinAngle) {
        /* Point - axis aligned cone test, shifted so that the cone's surface is extended by the radius of the sphere */
        const T coneRadius = tanAngle*(center.z() - radius/sinAngle);
        return center.xy().dot() <= coneRadius*coneRadius;
    } else {
        /* Simple sphere point check */
        return center.dot() <= radius*radius;
    }

    return false;
}

template<class T> bool sphereCone(
    const Vector3<T>& sCenter, const T radius,
    const Vector3<T>& origin, const Vector3<T>& normal, const Rad<T> angle)
{
    const T sinAngle = Math::sin(angle/T(2));
    const T tanAngleSqPlusOne = T(1) + Math::pow<T>(Math::tan<T>(angle/T(2)), T(2));

    return sphereCone(sCenter, radius, origin, normal, sinAngle, tanAngleSqPlusOne);
}

template<class T> bool sphereCone(
    const Vector3<T>& sCenter, const T radius,
    const Vector3<T>& origin, const Vector3<T>& normal,
    const T sinAngle, const T tanAngleSqPlusOne)
{
    const Vector3<T> diff = sCenter - origin;

    if (dot(diff - radius*sinAngle*normal, normal) > T(0)) {
        /* point - cone test */
        const Vector3<T> c = sinAngle*diff + normal*radius;
        const T lenA = dot(c, normal);

        return c.dot() <= lenA*lenA*(tanAngleSqPlusOne);
    } else {
        /* Simple sphere point check */
        return diff.dot() <= radius*radius;
    }
}

template<class T> bool aabbCone(
    const Vector3<T>& center, const Vector3<T>& extents, const Vector3<T>& origin,
    const Vector3<T>& normal, const Rad<T> angle)
{
    const T x = T(1) + Math::pow<T>(Math::tan<T>(angle/T(2)), T(2));
    return aabbCone(center, extents, origin, normal, x);
}

template<class T> bool aabbCone(
    const Vector3<T>& center, const Vector3<T>& extents,
    const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const T tanAngleSqPlusOne)
{
    const Vector3<T> c = center - coneOrigin;

    for (const Int axis: {0, 1, 2}) {
        const Int z = axis;
        const Int x = (axis + 1) % 3;
        const Int y = (axis + 2) % 3;
        if(coneNormal[z] != T(0)) {
            Float t0 = ((c[z] - extents[z])/coneNormal[z]);
            Float t1 = ((c[z] + extents[z])/coneNormal[z]);

            const Vector3<T> i0 = coneNormal*t0;
            const Vector3<T> i1 = coneNormal*t1;

            for(const auto& i : {i0, i1}) {
                Vector3<T> closestPoint = i;

                if(i[x] - c[x] > extents[x]) {
                    closestPoint[x] = c[x] + extents[x];
                } else if(i[x] - c[x] < -extents[x]) {
                    closestPoint[x] = c[x] - extents[x];
                }
                /* Else: normal intersects within x bounds */

                if(i[y] - c[y] > extents[y]) {
                    closestPoint[y] = c[y] + extents[y];
                } else if(i[y] - c[y] < -extents[y]) {
                    closestPoint[y] = c[y] - extents[y];
                }
                /* Else: normal intersects within Y bounds */

                if (pointCone<T>(closestPoint, {}, coneNormal, tanAngleSqPlusOne)) {
                    /* Found a point in cone and aabb */
                    return true;
                }
            }
        }
        /* else: normal will intersect one of the other planes */
    }

    return false;
}

template<class T> bool rangeCone(const Range3D<T>& range, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const Rad<T> angle) {
    const Vector3<T> center = (range.min() + range.max())/T(2);
    const Vector3<T> extents = (range.max() - range.min())/T(2);
    const T x = T(1) + Math::pow<T>(Math::tan<T>(angle/T(2)), T(2));
    return aabbCone(center, extents, coneOrigin, coneNormal, x);
}

template<class T> bool rangeCone(const Range3D<T>& range, const Vector3<T>& coneOrigin, const Vector3<T>& coneNormal, const T tanAngleSqPlusOne) {
    const Vector3<T> center = (range.min() + range.max())/T(2);
    const Vector3<T> extents = (range.max() - range.min())/T(2);
    return aabbCone(center, extents, coneOrigin, coneNormal, tanAngleSqPlusOne);
}

}}}}

#endif
