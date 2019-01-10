#ifndef Magnum_Shapes_Plane_h
#define Magnum_Shapes_Plane_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
@brief Class @ref Magnum::Shapes::Plane

@deprecated The @ref Magnum::Shapes library is a failed design experiment and
    is scheduled for removal in a future release. Related geometry algorithms
    were moved to @ref Magnum::Math::Distance and @ref Magnum::Math::Intersection;
    if you need a full-fledged physics library, please have look at
    [Bullet](https://bulletphysics.org), which has Magnum integration in
    @ref Magnum::BulletIntegration, or at [Box2D](https://box2d.org/), which
    has a @ref examples-box2d "Magnum example" as well.
*/

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Shapes/Shapes.h"
#include "Magnum/Shapes/visibility.h"

/* File-level deprecation warning issued from Shapes.h */

namespace Magnum { namespace Shapes {

CORRADE_IGNORE_DEPRECATED_PUSH
/**
@brief Infinite plane, defined by position and normal (3D only)

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

Unlike other elements the plane expects uniform scaling. See @ref shapes for
brief introduction.
*/
class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") MAGNUM_SHAPES_EXPORT Plane {
    public:
        enum: UnsignedInt {
            Dimensions = 3 /**< Dimension count */
        };

        /**
         * @brief Default constructor
         *
         * Creates plane with zero-sized normal at origin.
         */
        constexpr /*implicit*/ Plane() {}

        /** @brief Constructor */
        constexpr /*implicit*/ Plane(const Vector3& position, const Vector3& normal): _position(position), _normal(normal) {}

        /** @brief Transformed shape */
        Plane transformed(const Matrix4& matrix) const;

        /** @brief Position */
        constexpr Vector3 position() const { return _position; }

        /** @brief Set position */
        void setPosition(const Vector3& position) {
            _position = position;
        }

        /** @brief Normal */
        constexpr Vector3 normal() const { return _normal; }

        /** @brief Set normal */
        void setNormal(const Vector3& normal) {
            _normal = normal;
        }

        /** @brief Collision occurence with line */
        bool operator%(const Line3D& other) const;

        /** @brief Collision occurence with line segment */
        bool operator%(const LineSegment3D& other) const;

    private:
        Vector3 _position, _normal;
};

/** @relatesalso Line
@brief Collision occurence of @ref Line and @ref Plane

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

@see @ref Plane::operator%(const Line3D&) const
*/
inline CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") bool operator%(const Line3D& a, const Plane& b) { return b % a; }

/** @relatesalso LineSegment
@brief Collision occurence of @ref LineSegment and @ref Plane

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

@see @ref Plane::operator%(const LineSegment3D&) const
*/
inline CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") bool operator%(const LineSegment3D& a, const Plane& b) { return b % a; }
CORRADE_IGNORE_DEPRECATED_POP

}}

#endif
