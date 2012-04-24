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
 * @brief Class Magnum::Physics::Capsule
 */

#include "AbstractShape.h"

namespace Magnum { namespace Physics {

/**
@brief %Capsule defined by cylinder start and end point and radius

Unlike other elements the capsule doesn't support asymmetric scaling. When
applying transformation, the scale factor is averaged from all axes.
*/
class PHYSICS_EXPORT Capsule: public AbstractShape {
    public:
        /** @brief Constructor */
        inline constexpr Capsule(const Vector3& a, const Vector3& b, float radius): _a(a), _transformedA(a), _b(b), _transformedB(b), _radius(radius), _transformedRadius(radius) {}

        void applyTransformation(const Matrix4& transformation);

        inline Vector3 a() const { return _a; }         /**< @brief Start point */
        inline Vector3 b() const { return _a; }         /**< @brief End point */

        inline void setA(const Vector3& a) { _a = a; }  /**< @brief Set start point */
        inline void setB(const Vector3& b) { _b = b; }  /**< @brief Set end point */

        /** @brief Radius */
        inline float radius() const { return _radius; }

        /** @brief Set radius */
        inline void setRadius(float radius) { _radius = radius; }

        /** @brief Transformed first point */
        inline Vector3 transformedA() const { return _transformedA; }

        /** @brief Transformed second point */
        inline Vector3 transformedB() const { return _transformedB; }

        /** @brief Transformed radius */
        inline float transformedRadius() const {
            return _transformedRadius;
        }

    protected:
        inline Type type() const { return Type::Capsule; }

    private:
        Vector3 _a, _transformedA,
            _b, _transformedB;
        float _radius, _transformedRadius;
};

}}

#endif
