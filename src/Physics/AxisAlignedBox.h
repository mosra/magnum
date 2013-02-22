#ifndef Magnum_Physics_AxisAlignedBox_h
#define Magnum_Physics_AxisAlignedBox_h
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
 * @brief Class Magnum::Physics::AxisAlignedBox, typedef Magnum::Physics::AxisAlignedBox2D, Magnum::Physics.:AxisAlignedBox3D
 */

#include "Math/Vector3.h"
#include "Physics/AbstractShape.h"
#include "Physics/Physics.h"

#include "corradeCompatibility.h"

namespace Magnum { namespace Physics {

/**
@brief Axis-aligned box

@see AxisAlignedBox2D, AxisAlignedBox3D
@todo Assert for rotation
*/
template<std::uint8_t dimensions> class MAGNUM_PHYSICS_EXPORT AxisAlignedBox: public AbstractShape<dimensions> {
    public:
        /** @brief Constructor */
        inline explicit AxisAlignedBox(const typename DimensionTraits<dimensions>::VectorType& min, const typename DimensionTraits<dimensions>::VectorType& max): _min(min), _max(max), _transformedMin(min), _transformedMax(max) {}

        inline typename AbstractShape<dimensions>::Type type() const override {
            return AbstractShape<dimensions>::Type::AxisAlignedBox;
        }

        void applyTransformationMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) override;

        bool collides(const AbstractShape<dimensions>* other) const override;

        /** @brief Minimal coordinates */
        inline typename DimensionTraits<dimensions>::VectorType min() const {
            return _min;
        }

        /** @brief Set minimal coordinates */
        inline void setMin(const typename DimensionTraits<dimensions>::VectorType& min) {
            _min = min;
        }

        /** @brief Maximal coordinates */
        inline typename DimensionTraits<dimensions>::VectorType max() const { return _max; }

        /** @brief Set maximal coordinates */
        inline void setMax(const typename DimensionTraits<dimensions>::VectorType& max) {
            _max = max;
        }

        /** @brief Transformed minimal coordinates */
        inline typename DimensionTraits<dimensions>::VectorType transformedMin() const {
            return _transformedMin;
        }

        /** @brief Transformed maximal coordinates */
        inline typename DimensionTraits<dimensions>::VectorType transformedMax() const {
            return _transformedMax;
        }

        /** @brief Collision with point */
        bool operator%(const Point<dimensions>& other) const;

    private:
        typename DimensionTraits<dimensions>::VectorType _min, _max,
            _transformedMin, _transformedMax;
};

/** @brief Two-dimensional axis-aligned box */
typedef AxisAlignedBox<2> AxisAlignedBox2D;

/** @brief Three-dimensional axis-aligned box */
typedef AxisAlignedBox<3> AxisAlignedBox3D;

/** @collisionoperator{Point,AxisAlignedBox} */
template<std::uint8_t dimensions> inline bool operator%(const Point<dimensions>& a, const AxisAlignedBox<dimensions>& b) { return b % a; }

}}

#endif
