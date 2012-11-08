#ifndef Magnum_Physics_Capsule_h
#define Magnum_Physics_Capsule_h
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
 * @brief Class Magnum::Physics::Capsule, typedef Magnum::Physics::Capsule2D, Magnum::Physics::Capsule3D
 */

#include "Math/Vector3.h"
#include "AbstractShape.h"

#include "magnumCompatibility.h"

namespace Magnum { namespace Physics {

template<std::uint8_t> class Point;
template<std::uint8_t> class Sphere;

/**
@brief %Capsule defined by cylinder start and end point and radius

Unlike other elements the capsule doesn't support asymmetric scaling. When
applying transformation, the scale factor is averaged from all axes.
@see Capsule2D, Capsule3D
*/
template<std::uint8_t dimensions> class PHYSICS_EXPORT Capsule: public AbstractShape<dimensions> {
    public:
        /** @brief Constructor */
        inline Capsule(const typename DimensionTraits<dimensions, GLfloat>::VectorType& a, const typename DimensionTraits<dimensions, GLfloat>::VectorType& b, float radius): _a(a), _transformedA(a), _b(b), _transformedB(b), _radius(radius), _transformedRadius(radius) {}

        inline typename AbstractShape<dimensions>::Type type() const override {
            return AbstractShape<dimensions>::Type::Capsule;
        }

        void applyTransformation(const typename DimensionTraits<dimensions, GLfloat>::MatrixType& transformation) override;

        bool collides(const AbstractShape<dimensions>* other) const override;

        /** @brief Start point */
        inline typename DimensionTraits<dimensions, GLfloat>::VectorType a() const {
            return _a;
        }

        /** @brief End point */
        inline typename DimensionTraits<dimensions, GLfloat>::VectorType b() const {
            return _a;
        }

        /** @brief Set start point */
        inline void setA(const typename DimensionTraits<dimensions, GLfloat>::VectorType& a) {
            _a = a;
        }

        /** @brief Set end point */
        inline void setB(const typename DimensionTraits<dimensions, GLfloat>::VectorType& b) {
            _b = b;
        }

        /** @brief Radius */
        inline float radius() const { return _radius; }

        /** @brief Set radius */
        inline void setRadius(float radius) { _radius = radius; }

        /** @brief Transformed first point */
        inline typename DimensionTraits<dimensions, GLfloat>::VectorType transformedA() const {
            return _transformedA;
        }

        /** @brief Transformed second point */
        inline typename DimensionTraits<dimensions, GLfloat>::VectorType transformedB() const {
            return _transformedB;
        }

        /** @brief Transformed radius */
        inline float transformedRadius() const {
            return _transformedRadius;
        }

        /** @brief Collision with point */
        bool operator%(const Point<dimensions>& other) const;

        /** @brief Collision with sphere */
        bool operator%(const Sphere<dimensions>& other) const;

    private:
        Math::Vector<dimensions, GLfloat> _a, _transformedA,
            _b, _transformedB;
        float _radius, _transformedRadius;
};

/** @brief Two-dimensional capsule */
typedef Capsule<2> Capsule2D;

/** @brief Three-dimensional capsule */
typedef Capsule<3> Capsule3D;

/** @collisionoperator{Point,Capsule} */
template<std::uint8_t dimensions> inline bool operator%(const Point<dimensions>& a, const Capsule<dimensions>& b) { return b % a; }

/** @collisionoperator{Sphere,Capsule} */
template<std::uint8_t dimensions> inline bool operator%(const Sphere<dimensions>& a, const Capsule<dimensions>& b) { return b % a; }

}}

#endif
