#ifndef Magnum_Physics_Point_h
#define Magnum_Physics_Point_h
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
 * @brief Class Magnum::Physics::Point, typedef Magnum::Physics::Point2D, Magnum::Physics::Point3D
 */

#include "Math/Vector3.h"
#include "AbstractShape.h"

#include "magnumCompatibility.h"

namespace Magnum { namespace Physics {

/**
@brief %Point

@see Point2D, Point3D
*/
template<std::uint8_t dimensions> class PHYSICS_EXPORT Point: public AbstractShape<dimensions> {
    public:
        /** @brief Constructor */
        inline Point(const typename DimensionTraits<dimensions>::VectorType& position): _position(position), _transformedPosition(position) {}

        inline typename AbstractShape<dimensions>::Type type() const override {
            return AbstractShape<dimensions>::Type::Point;
        }

        void applyTransformation(const typename DimensionTraits<dimensions>::MatrixType& transformation) override;

        /** @brief Position */
        inline typename DimensionTraits<dimensions>::VectorType position() const {
            return _position;
        }

        /** @brief Set position */
        inline void setPosition(const typename DimensionTraits<dimensions>::VectorType& position) {
            _position = position;
        }

        /** @brief Transformed position */
        inline typename DimensionTraits<dimensions>::VectorType transformedPosition() const {
            return _transformedPosition;
        }

    private:
        typename DimensionTraits<dimensions>::VectorType _position, _transformedPosition;
};

/** @brief Two-dimensional point */
typedef Point<2> Point2D;

/** @brief Three-dimensional point */
typedef Point<3> Point3D;

}}

#endif
