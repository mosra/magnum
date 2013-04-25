#ifndef Magnum_Physics_Plane_h
#define Magnum_Physics_Plane_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Physics::Plane
 */

#include "Math/Vector3.h"
#include "Magnum.h"
#include "Physics/Physics.h"
#include "Physics/magnumPhysicsVisibility.h"

namespace Magnum { namespace Physics {

/** @brief Infinite plane, defined by position and normal (3D only) */
class MAGNUM_PHYSICS_EXPORT Plane {
    public:
        enum: UnsignedInt {
            Dimensions = 3 /**< Dimension count */
        };

        /**
         * @brief Default constructor
         *
         * Creates plane with zero-sized normal at origin.
         */
        inline constexpr /*implicit*/ Plane() {}

        /** @brief Constructor */
        inline constexpr /*implicit*/ Plane(const Vector3& position, const Vector3& normal): _position(position), _normal(normal) {}

        /** @brief Transformed shape */
        Plane transformed(const Matrix4& matrix) const;

        /** @brief Position */
        inline constexpr Vector3 position() const { return _position; }

        /** @brief Set position */
        inline void setPosition(const Vector3& position) {
            _position = position;
        }

        /** @brief Normal */
        inline constexpr Vector3 normal() const { return _normal; }

        /** @brief Set normal */
        inline void setNormal(const Vector3& normal) {
            _normal = normal;
        }

        /** @brief Collision with line */
        bool operator%(const Line3D& other) const;

        /** @brief Collision with line segment */
        bool operator%(const LineSegment3D& other) const;

    private:
        Vector3 _position, _normal;
};

/** @collisionoperator{Line,Plane} */
inline bool operator%(const Line3D& a, const Plane& b) { return b % a; }

/** @collisionoperator{LineSegment,Plane} */
inline bool operator%(const LineSegment3D& a, const Plane& b) { return b % a; }


}}

#endif
