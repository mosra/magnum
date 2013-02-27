#ifndef Magnum_Physics_Box_h
#define Magnum_Physics_Box_h
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
 * @brief Class Magnum::Physics::Box, typedef Magnum::Physics::Box2D, Magnum::Physics::Box3D
 */

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "AbstractShape.h"

#include "corradeCompatibility.h"

namespace Magnum { namespace Physics {

/**
@brief Unit-size box with assigned transformation matrix

@todo Use quat + position + size instead?
@see Box2D, Box3D
@todo Assert for skew
*/
template<UnsignedInt dimensions> class MAGNUM_PHYSICS_EXPORT Box: public AbstractShape<dimensions> {
    public:
        /** @brief Constructor */
        inline explicit Box(const typename DimensionTraits<dimensions>::MatrixType& transformation): _transformation(transformation), _transformedTransformation(transformation) {}

        inline typename AbstractShape<dimensions>::Type type() const override {
            return AbstractShape<dimensions>::Type::Box;
        }

        void applyTransformationMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) override;

        /** @brief Transformation */
        inline typename DimensionTraits<dimensions>::MatrixType transformation() const {
            return _transformation;
        }

        /** @brief Set transformation */
        inline void setTransformation(const typename DimensionTraits<dimensions>::MatrixType& transformation) {
            _transformation = transformation;
        }

        /** @brief Transformed transformation */
        inline typename DimensionTraits<dimensions>::MatrixType transformedTransformation() const {
            return _transformedTransformation;
        }

    private:
        typename DimensionTraits<dimensions>::MatrixType _transformation,
            _transformedTransformation;
};

/** @brief Two-dimensional box */
typedef Box<2> Box2D;

/** @brief Three-dimensional box */
typedef Box<3> Box3D;

}}

#endif
