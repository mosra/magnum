#ifndef Magnum_Shapes_Point_h
#define Magnum_Shapes_Point_h
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
@brief Class @ref Magnum::Shapes::Point, typedef @ref Magnum::Shapes::Point2D, @ref Magnum::Shapes::Point3D

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
#include "Magnum/Shapes/visibility.h"

/* File-level deprecation warning issued from Shapes.h */

namespace Magnum { namespace Shapes {

CORRADE_IGNORE_DEPRECATED_PUSH
/**
@brief Point

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.

See @ref shapes for brief introduction.
@see @ref Point2D, @ref Point3D
*/
template<UnsignedInt dimensions> class CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") MAGNUM_SHAPES_EXPORT Point {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Dimension count */
        };

        /**
         * @brief Default constructor
         *
         * Creates point at origin.
         */
        constexpr /*implicit*/ Point() {}

        /** @brief Constructor */
        constexpr /*implicit*/ Point(const VectorTypeFor<dimensions, Float>& position): _position(position) {}

        /** @brief Transformed shape */
        Point<dimensions> transformed(const MatrixTypeFor<dimensions, Float>& matrix) const;

        /** @brief Position */
        constexpr VectorTypeFor<dimensions, Float> position() const {
            return _position;
        }

        /** @brief Set position */
        void setPosition(const VectorTypeFor<dimensions, Float>& position) {
            _position = position;
        }

    private:
        VectorTypeFor<dimensions, Float> _position;
};

/**
@brief Two-dimensional point

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Point<2> Point2D;

/**
@brief Three-dimensional point

@deprecated The @ref Shapes library is a failed design experiment and is
    scheduled for removal in a future release. Related geometry algorithms were
    moved to @ref Math::Distance and @ref Math::Intersection; if you need a
    full-fledged physics library, please have look at [Bullet](https://bulletphysics.org),
    which has Magnum integration in @ref BulletIntegration, or at
    [Box2D](https://box2d.org/), which has a @ref examples-box2d "Magnum example"
    as well.
*/
typedef CORRADE_DEPRECATED("scheduled for removal, see the docs for alternatives") Point<3> Point3D;
CORRADE_IGNORE_DEPRECATED_POP

}}

#endif
