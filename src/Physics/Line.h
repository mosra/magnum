#ifndef Magnum_Physics_Line_h
#define Magnum_Physics_Line_h
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
 * @brief Class Magnum::Physics::Line, typedef Magnum::Physics::Line2D, Magnum::Physics::Line3D
 */

#include "Math/Vector3.h"
#include "AbstractShape.h"

#include "magnumCompatibility.h"

namespace Magnum { namespace Physics {

/**
@brief Infinite line, defined by two points

@see Line2D, Line3D
@todo collision detection of two Line2D
*/
template<std::uint8_t dimensions> class MAGNUM_PHYSICS_EXPORT Line: public AbstractShape<dimensions> {
    public:
        /** @brief Constructor */
        inline Line(const typename DimensionTraits<dimensions>::VectorType& a, const typename DimensionTraits<dimensions>::VectorType& b): _a(a), _transformedA(a), _b(b), _transformedB(b) {}

        inline typename AbstractShape<dimensions>::Type type() const override {
            return AbstractShape<dimensions>::Type::Line;
        }

        void applyTransformation(const typename DimensionTraits<dimensions>::MatrixType& transformation) override;

        /** @brief First point */
        inline typename DimensionTraits<dimensions>::VectorType a() const {
            return _a;
        }

        /** @brief Second point */
        inline typename DimensionTraits<dimensions>::VectorType b() const {
            return _a;
        }

        /** @brief Set first point */
        inline void setA(const typename DimensionTraits<dimensions>::VectorType& a) {
            _a = a;
        }

        /** @brief Set second point */
        inline void setB(const typename DimensionTraits<dimensions>::VectorType& b) {
            _b = b;
        }

        /** @brief Transformed first point */
        inline typename DimensionTraits<dimensions>::VectorType transformedA() const {
            return _transformedA;
        }

        /** @brief Transformed second point */
        inline typename DimensionTraits<dimensions>::VectorType transformedB() const {
            return _transformedB;
        }

    private:
        typename DimensionTraits<dimensions>::VectorType _a, _transformedA,
            _b, _transformedB;
};

/** @brief Infinite two-dimensional line */
typedef Line<2> Line2D;

/** @brief Infinite three-dimensional line */
typedef Line<3> Line3D;

}}

#endif
