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
 * @brief Class Magnum::Physics::Sphere, typedef Magnum::Physics::Sphere2D, Magnum::Physics::Sphere3D
 */

#include "Math/Vector3.h"
#include "AbstractShape.h"
#include "Physics.h"

#include "magnumCompatibility.h"

namespace Magnum { namespace Physics {

/**
@brief %Sphere defined by position and radius

Unlike other elements the sphere doesn't support asymmetric scaling. When
applying transformation, the scale factor is averaged from all axes.
@see Sphere2D, Sphere3D
*/
template<std::uint8_t dimensions> class PHYSICS_EXPORT Sphere: public AbstractShape<dimensions> {
    public:
        /** @brief Constructor */
        inline Sphere(const typename DimensionTraits<dimensions, GLfloat>::VectorType& position, float radius): _position(position), _transformedPosition(position), _radius(radius), _transformedRadius(radius) {}

        inline typename AbstractShape<dimensions>::Type type() const override {
            return AbstractShape<dimensions>::Type::Sphere;
        }

        void applyTransformation(const typename DimensionTraits<dimensions, GLfloat>::MatrixType& transformation) override;

        bool collides(const AbstractShape<dimensions>* other) const override;

        /** @brief Position */
        inline typename DimensionTraits<dimensions, GLfloat>::VectorType position() const {
            return _position;
        }

        /** @brief Set position */
        inline void setPosition(const typename DimensionTraits<dimensions, GLfloat>::VectorType& position) {
            _position = position;
        }

        /** @brief Radius */
        inline float radius() const { return _radius; }

        /** @brief Set radius */
        inline void setRadius(float radius) { _radius = radius; }

        /** @brief Transformed position */
        inline typename DimensionTraits<dimensions, GLfloat>::VectorType transformedPosition() const {
            return _transformedPosition;
        }

        /** @brief Transformed radius */
        inline float transformedRadius() const {
            return _transformedRadius;
        }

        /** @brief Collision with point */
        bool operator%(const Point<dimensions>& other) const;

        /** @brief Collision with line */
        bool operator%(const Line<dimensions>& other) const;

        /** @brief Collision with line segment */
        bool operator%(const LineSegment<dimensions>& other) const;

        /** @brief Collision with sphere */
        bool operator%(const Sphere<dimensions>& other) const;

    private:
        Math::Vector<dimensions, GLfloat> _position,
            _transformedPosition;
        float _radius, _transformedRadius;
};

/** @brief Two-dimensional sphere */
typedef Sphere<2> Sphere2D;

/** @brief Three-dimensional sphere */
typedef Sphere<3> Sphere3D;

/** @collisionoperator{Point,Sphere} */
template<std::uint8_t dimensions> inline bool operator%(const Point<dimensions>& a, const Sphere<dimensions>& b) { return b % a; }

/** @collisionoperator{Line,Sphere} */
template<std::uint8_t dimensions> inline bool operator%(const Line<dimensions>& a, const Sphere<dimensions>& b) { return b % a; }

/** @collisionoperator{LineSegment,Sphere} */
template<std::uint8_t dimensions> inline bool operator%(const LineSegment<dimensions>& a, const Sphere<dimensions>& b) { return b % a; }

}}

#endif
