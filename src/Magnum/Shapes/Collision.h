#ifndef Magnum_Shapes_Collision_h
#define Magnum_Shapes_Collision_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::Shapes::Collision, typedef @ref Magnum::Shapes::Collision2D, @ref Magnum::Shapes::Collision3D
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace Shapes {

/**
@brief Collision data

Contains information about collision between objects A and B, described by
contact position, separation normal and separation distance.

If the collision occured, contact position is on object B surface, separation
normal is *normalized* vector in which direction should object A be moved to
separate the bodies, separation distance is positive and describes minimal
movement of object A in direction of separation normal after which the contact
position will no longer be colliding with object A.

If the collision not occured, contact position and separation normal is
undefined (i.e., *not* normalized) and separation distance is negative or zero.
@see @ref Collision2D, @ref Collision3D
*/
template<UnsignedInt dimensions> class Collision {
    public:
        /**
         * @brief Default constructor
         *
         * Sets position, normal and separation distance to zero, as if no
         * collision happened.
         */
        /*implicit*/ Collision(): _separationDistance(0.0f) {}

        /**
         * @brief Constructor
         *
         * If separation distance is positive, the separation normal is
         * expected to be normalized.
         */
        explicit Collision(const VectorTypeFor<dimensions, Float>& position, const VectorTypeFor<dimensions, Float>& separationNormal, Float separationDistance) noexcept: _position(position), _separationNormal(separationNormal), _separationDistance(separationDistance) {
            CORRADE_ASSERT(_separationDistance < Math::TypeTraits<Float>::epsilon() || separationNormal.isNormalized(), "Shapes::Collision::Collision: separation normal is not normalized", );
        }

        /**
         * @brief Whether the collision happened
         *
         * Negative or zero separation distance means that no collision
         * happened.
         * @see @ref separationDistance()
         */
        operator bool() const { return _separationDistance > 0.0f; }

        /** @brief Collision position */
        VectorTypeFor<dimensions, Float> position() const {
            return _position;
        }

        /**
         * @brief Separation normal
         *
         * @see @ref separationDistance(), @ref flipped()
         */
        VectorTypeFor<dimensions, Float> separationNormal() const {
            return _separationNormal;
        }

        /**
         * @brief Separation distance
         *
         * @see @ref separationNormal(), @ref operator bool()
         */
        Float separationDistance() const {
            return _separationDistance;
        }

        /**
         * @brief Flipped collision
         *
         * Returns new collision object as if the collision occured between
         * flipped pair of objects, i.e. with flipped separation normal and
         * contact position on surface of object A.
         * @see @ref position(), @ref separationNormal()
         */
        Collision<dimensions> flipped() const {
            return Collision<dimensions>(_position - _separationDistance*_separationNormal, -_separationNormal, _separationDistance);
        }

    private:
        VectorTypeFor<dimensions, Float> _position;
        VectorTypeFor<dimensions, Float> _separationNormal;
        Float _separationDistance;
};

/** @brief Two-dimensional collision data */
typedef Collision<2> Collision2D;

/** @brief Three-dimensional collision data */
typedef Collision<3> Collision3D;

}}

#endif
