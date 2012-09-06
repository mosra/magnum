#ifndef Magnum_Physics_Sphere_h
#define Magnum_Physics_Sphere_h
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
 * @brief Class Magnum::Physics::Sphere
 */

#include "Math/Vector3.h"
#include "AbstractShape.h"

namespace Magnum { namespace Physics {

class Line;
class LineSegment;
class Point;

/**
@brief %Sphere defined by position and radius

Unlike other elements the sphere doesn't support asymmetric scaling. When
applying transformation, the scale factor is averaged from all axes.
*/
class PHYSICS_EXPORT Sphere: public AbstractShape {
    public:
        /** @brief Constructor */
        inline Sphere(const Vector3& position, float radius): _position(position), _transformedPosition(position), _radius(radius), _transformedRadius(radius) {}

        void applyTransformation(const Matrix4& transformation);

        bool collides(const AbstractShape* other) const;

        /** @brief Position */
        inline Vector3 position() const { return _position; }

        /** @brief Set position */
        inline void setPosition(const Vector3& position) { _position = position; }

        /** @brief Radius */
        inline float radius() const { return _radius; }

        /** @brief Set radius */
        inline void setRadius(float radius) { _radius = radius; }

        /** @brief Transformed position */
        inline Vector3 transformedPosition() const {
            return _transformedPosition;
        }

        /** @brief Transformed radius */
        inline float transformedRadius() const {
            return _transformedRadius;
        }

        /** @brief Collision with point */
        bool operator%(const Point& other) const;

        /** @brief Collision with line */
        bool operator%(const Line& other) const;

        /** @brief Collision with line segment */
        bool operator%(const LineSegment& other) const;

        /** @brief Collision with sphere */
        bool operator%(const Sphere& other) const;

    protected:
        inline Type type() const { return Type::Sphere; }

    private:
        Vector3 _position, _transformedPosition;
        float _radius, _transformedRadius;
};

/** @collisionoperator{Point,Sphere} */
inline bool operator%(const Point& a, const Sphere& b) { return b % a; }

/** @collisionoperator{Line,Sphere} */
inline bool operator%(const Line& a, const Sphere& b) { return b % a; }

/** @collisionoperator{LineSegment,Sphere} */
inline bool operator%(const LineSegment& a, const Sphere& b) { return b % a; }

}}

#endif
