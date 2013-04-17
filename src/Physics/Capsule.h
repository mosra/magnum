#ifndef Magnum_Physics_Capsule_h
#define Magnum_Physics_Capsule_h
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
 * @brief Class Magnum::Physics::Capsule, typedef Magnum::Physics::Capsule2D, Magnum::Physics::Capsule3D
 */

#include "Math/Vector3.h"
#include "AbstractShape.h"
#include "Physics.h"

#include "corradeCompatibility.h"

namespace Magnum { namespace Physics {

/**
@brief %Capsule defined by cylinder start and end point and radius

Unlike other elements the capsule doesn't support asymmetric scaling. When
applying transformation, the scale factor is averaged from all axes.
@see Capsule2D, Capsule3D
@todo Assert for asymmetric scaling
*/
template<UnsignedInt dimensions> class MAGNUM_PHYSICS_EXPORT Capsule: public AbstractShape<dimensions> {
    public:
        /**
         * @brief Constructor
         *
         * Creates zero-sized capsule at origin.
         */
        inline explicit Capsule(): _radius(0.0f), _transformedRadius(0.0f) {}

        /** @brief Constructor */
        inline explicit Capsule(const typename DimensionTraits<dimensions>::VectorType& a, const typename DimensionTraits<dimensions>::VectorType& b, Float radius): _a(a), _transformedA(a), _b(b), _transformedB(b), _radius(radius), _transformedRadius(radius) {}

        inline typename AbstractShape<dimensions>::Type type() const override {
            return AbstractShape<dimensions>::Type::Capsule;
        }

        void applyTransformationMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) override;

        bool collides(const AbstractShape<dimensions>* other) const override;

        /** @brief Start point */
        inline typename DimensionTraits<dimensions>::VectorType a() const {
            return _a;
        }

        /** @brief End point */
        inline typename DimensionTraits<dimensions>::VectorType b() const {
            return _b;
        }

        /** @brief Set start point */
        inline void setA(const typename DimensionTraits<dimensions>::VectorType& a) {
            _a = a;
        }

        /** @brief Set end point */
        inline void setB(const typename DimensionTraits<dimensions>::VectorType& b) {
            _b = b;
        }

        /** @brief Radius */
        inline Float radius() const { return _radius; }

        /** @brief Set radius */
        inline void setRadius(Float radius) { _radius = radius; }

        /** @brief Transformed first point */
        inline typename DimensionTraits<dimensions>::VectorType transformedA() const {
            return _transformedA;
        }

        /** @brief Transformed second point */
        inline typename DimensionTraits<dimensions>::VectorType transformedB() const {
            return _transformedB;
        }

        /** @brief Transformed radius */
        inline Float transformedRadius() const {
            return _transformedRadius;
        }

        /** @brief Collision with point */
        bool operator%(const Point<dimensions>& other) const;

        /** @brief Collision with sphere */
        bool operator%(const Sphere<dimensions>& other) const;

    private:
        typename DimensionTraits<dimensions>::VectorType _a, _transformedA,
            _b, _transformedB;
        Float _radius, _transformedRadius;
};

/** @brief Two-dimensional capsule */
typedef Capsule<2> Capsule2D;

/** @brief Three-dimensional capsule */
typedef Capsule<3> Capsule3D;

/** @collisionoperator{Point,Capsule} */
template<UnsignedInt dimensions> inline bool operator%(const Point<dimensions>& a, const Capsule<dimensions>& b) { return b % a; }

/** @collisionoperator{Sphere,Capsule} */
template<UnsignedInt dimensions> inline bool operator%(const Sphere<dimensions>& a, const Capsule<dimensions>& b) { return b % a; }

}}

#endif
