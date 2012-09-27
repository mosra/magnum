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

namespace Magnum { namespace Physics {

template<size_t> class Point;
template<size_t> class Sphere;

/**
@brief %Capsule defined by cylinder start and end point and radius

Unlike other elements the capsule doesn't support asymmetric scaling. When
applying transformation, the scale factor is averaged from all axes.
@see Capsule2D, Capsule3D
*/
template<size_t dimensions> class PHYSICS_EXPORT Capsule: public AbstractShape<dimensions> {
    public:
        /** @brief Constructor */
        inline Capsule(const typename AbstractShape<dimensions>::VectorType& a, const typename AbstractShape<dimensions>::VectorType& b, float radius): _a(a), _transformedA(a), _b(b), _transformedB(b), _radius(radius), _transformedRadius(radius) {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        void applyTransformation(const typename AbstractShape<dimensions>::MatrixType& transformation);
        #else
        void applyTransformation(const MatrixType& transformation);
        #endif

        bool collides(const AbstractShape<dimensions>* other) const;

        /** @brief Start point */
        inline typename AbstractShape<dimensions>::VectorType a() const {
            return _a;
        }

        /** @brief End point */
        inline typename AbstractShape<dimensions>::VectorType b() const {
            return _a;
        }

        /** @brief Set start point */
        inline void setA(const typename AbstractShape<dimensions>::VectorType& a) {
            _a = a;
        }

        /** @brief Set end point */
        inline void setB(const typename AbstractShape<dimensions>::VectorType& b) {
            _b = b;
        }

        /** @brief Radius */
        inline float radius() const { return _radius; }

        /** @brief Set radius */
        inline void setRadius(float radius) { _radius = radius; }

        /** @brief Transformed first point */
        inline typename AbstractShape<dimensions>::VectorType transformedA() const {
            return _transformedA;
        }

        /** @brief Transformed second point */
        inline typename AbstractShape<dimensions>::VectorType transformedB() const {
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

    protected:
        inline typename AbstractShape<dimensions>::Type type() const {
            return AbstractShape<dimensions>::Type::Capsule;
        }

    private:
        typename AbstractShape<dimensions>::VectorType _a, _transformedA,
            _b, _transformedB;
        float _radius, _transformedRadius;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
extern template class PHYSICS_EXPORT Capsule<2>;
extern template class PHYSICS_EXPORT Capsule<3>;
#endif

/** @brief Two-dimensional capsule */
typedef Capsule<2> Capsule2D;

/** @brief Three-dimensional capsule */
typedef Capsule<3> Capsule3D;

/** @collisionoperator{Point,Capsule} */
template<size_t dimensions> inline bool operator%(const Point<dimensions>& a, const Capsule<dimensions>& b) { return b % a; }

/** @collisionoperator{Sphere,Capsule} */
template<size_t dimensions> inline bool operator%(const Sphere<dimensions>& a, const Capsule<dimensions>& b) { return b % a; }

}}

#endif
