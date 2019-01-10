#ifndef Magnum_Shapes_Sphere_h
#define Magnum_Shapes_Sphere_h
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
@brief Class @ref Magnum::Shapes::Sphere, typedef @ref Magnum::Shapes::Sphere2D, @ref Magnum::Shapes::Sphere3D

@deprecated The @ref Magnum::Shapes library is a failed design experiment and
    is scheduled for removal in a future release. Related geometry algorithms
    were moved to @ref Magnum::Math::Distance and @ref Magnum::Math::Intersection;
    if you need a full-fledged physics library, please have look at
    [Bullet](https://bulletphysics.org), which has Magnum integration in
    @ref Magnum::BulletIntegration, or at [Box2D](https://box2d.org/), which
    has a @ref examples-box2d "Magnum example" as well.
*/

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Shapes/Shapes.h"
#include "Magnum/Shapes/Collision.h"
#include "Magnum/Shapes/visibility.h"

/* File-level deprecation warning issued from Shapes.h */

namespace Magnum { namespace Shapes {

CORRADE_IGNORE_DEPRECATED_PUSH
/**
@brief Sphere defined by position and radius

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

Unlike other elements the sphere expects uniform scaling. See @ref shapes for
brief introduction.
@see @ref Sphere2D, @ref Sphere3D
@todo Store the radius as squared value to avoid sqrt/pow? Will complicate
    collision detection with another sphere.
*/
template<UnsignedInt dimensions> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") MAGNUM_SHAPES_EXPORT Sphere {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Dimension count */
        };

        /**
         * @brief Default constructor
         *
         * Creates zero-sized sphere at origin.
         */
        constexpr /*implicit*/ Sphere(): _radius(0.0f) {}

        /** @brief Constructor */
        constexpr /*implicit*/ Sphere(const VectorTypeFor<dimensions, Float>& position, Float radius): _position(position), _radius(radius) {}

        /** @brief Transformed shape */
        Sphere<dimensions> transformed(const MatrixTypeFor<dimensions, Float>& matrix) const;

        /** @brief Position */
        constexpr VectorTypeFor<dimensions, Float> position() const {
            return _position;
        }

        /** @brief Set position */
        void setPosition(const VectorTypeFor<dimensions, Float>& position) {
            _position = position;
        }

        /** @brief Radius */
        constexpr Float radius() const { return _radius; }

        /** @brief Set radius */
        void setRadius(Float radius) { _radius = radius; }

        /** @brief Collision occurence with point */
        bool operator%(const Point<dimensions>& other) const;

        /** @brief Collision with point */
        Collision<dimensions> operator/(const Point<dimensions>& other) const;

        /** @brief Collision occurence with line */
        bool operator%(const Line<dimensions>& other) const;

        /** @brief Collision occurence with line segment */
        bool operator%(const LineSegment<dimensions>& other) const;

        /** @brief Collision occurence with sphere */
        bool operator%(const Sphere<dimensions>& other) const;

        /** @brief Collision with sphere */
        Collision<dimensions> operator/(const Sphere<dimensions>& other) const;

    private:
        VectorTypeFor<dimensions, Float> _position;
        Float _radius;
};

/**
@brief Two-dimensional sphere

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Sphere<2> Sphere2D;

/**
@brief Three-dimensional sphere

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Sphere<3> Sphere3D;

/**
@brief Inverted sphere defined by position and radius

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

Inverted version of @ref Sphere, detecting collisions on the outside, not on
the inside. See @ref shapes for brief introduction.
@see @ref InvertedSphere2D, @ref InvertedSphere3D
*/
template<UnsignedInt dimensions> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") MAGNUM_SHAPES_EXPORT InvertedSphere:
    #ifdef DOXYGEN_GENERATING_OUTPUT
    public Sphere<dimensions>
    #else
    private Sphere<dimensions>
    #endif
{
    public:
        /**
         * @brief Default constructor
         *
         * Creates zero-sized sphere at origin.
         */
        constexpr /*implicit*/ InvertedSphere() = default;

        /** @brief Constructor */
        constexpr /*implicit*/ InvertedSphere(const VectorTypeFor<dimensions, Float>& position, Float radius): Sphere<dimensions>(position, radius) {}

        using Sphere<dimensions>::Dimensions;

        /** @brief Transformed shape */
        InvertedSphere<dimensions> transformed(const MatrixTypeFor<dimensions, Float>& matrix) const {
            return Sphere<dimensions>::transformed(matrix);
        }

        using Sphere<dimensions>::position;
        using Sphere<dimensions>::setPosition;
        using Sphere<dimensions>::radius;
        using Sphere<dimensions>::setRadius;

        /** @brief Collision occurence with point */
        bool operator%(const Point<dimensions>& other) const;

        /** @brief Collision with point */
        Collision<dimensions> operator/(const Point<dimensions>& other) const;

        /** @brief Collision occurence with sphere */
        bool operator%(const Sphere<dimensions>& other) const;

        /** @brief Collision with sphere */
        Collision<dimensions> operator/(const Sphere<dimensions>& other) const;

    private:
        constexpr /*implicit*/ InvertedSphere(const Sphere<dimensions>& other): Sphere<dimensions>(other) {}
};

/**
@brief Inverted two-dimensional sphere

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") InvertedSphere<2> InvertedSphere2D;

/**
@brief Inverted three-dimensional sphere

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") InvertedSphere<3> InvertedSphere3D;

/**
@collisionoccurenceoperator{Point,Sphere}

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
template<UnsignedInt dimensions> inline CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") bool operator%(const Point<dimensions>& a, const Sphere<dimensions>& b) { return b % a; }

/**
@collisionoccurenceoperator{Point,InvertedSphere}

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
template<UnsignedInt dimensions> inline CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") bool operator%(const Point<dimensions>& a, const InvertedSphere<dimensions>& b) { return b % a; }

/**
@collisionoperator{Point,Sphere}

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
template<UnsignedInt dimensions> inline CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Collision<dimensions> operator/(const Point<dimensions>& a, const Sphere<dimensions>& b) { return (b/a).flipped(); }

/**
@collisionoperator{Point,InvertedSphere}

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
template<UnsignedInt dimensions> inline CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Collision<dimensions> operator/(const Point<dimensions>& a, const InvertedSphere<dimensions>& b) { return (b/a).flipped(); }

/**
@collisionoccurenceoperator{Line,Sphere}

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
template<UnsignedInt dimensions> inline CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") bool operator%(const Line<dimensions>& a, const Sphere<dimensions>& b) { return b % a; }

/**
@collisionoccurenceoperator{LineSegment,Sphere}

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
template<UnsignedInt dimensions> inline CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") bool operator%(const LineSegment<dimensions>& a, const Sphere<dimensions>& b) { return b % a; }

/**
@collisionoccurenceoperator{Sphere,InvertedSphere}

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
template<UnsignedInt dimensions> inline CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") bool operator%(const Sphere<dimensions>& a, const InvertedSphere<dimensions>& b) { return b % a; }

/**
@collisionoperator{Sphere,InvertedSphere}

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
template<UnsignedInt dimensions> inline CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Collision<dimensions> operator/(const Sphere<dimensions>& a, const InvertedSphere<dimensions>& b) { return (b/a).flipped(); }
CORRADE_IGNORE_DEPRECATED_POP

}}

#endif
