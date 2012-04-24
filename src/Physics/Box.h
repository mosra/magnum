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
 * @brief Class Magnum::Physics::Box
 */

#include "AbstractShape.h"

namespace Magnum { namespace Physics {

/** @brief Unit size box with assigned transformation matrix */
class Box: public AbstractShape {
    public:
        /** @brief Constructor */
        inline constexpr Box(const Matrix4& transformation): _transformation(transformation), _transformedTransformation(transformation) {}

        inline void applyTransformation(const Matrix4& transformation) {
            _transformedTransformation = transformation*_transformation;
        }

        /** @brief Transformation */
        inline constexpr Matrix4 transformation() const { return _transformation; }

        /** @brief Set transformation */
        inline Vector3 setTransformation(const Matrix4& transformation) {
            _transformation = transformation;
        }

        /** @brief Transformed transformation */
        inline constexpr Vector3 transformedTransformation() const {
            return _transformedTransformation;
        }

    protected:
        inline Type type() const { return Type::Box; }

    private:
        Vector3 _transformation, _transformedTransformation;
};

}}

#endif
