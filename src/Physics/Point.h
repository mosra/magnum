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
 * @brief Class Magnum::Physics::Point
 */

#include "AbstractShape.h"

namespace Magnum { namespace Physics {

/** @brief %Point */
class Point: public AbstractShape {
    public:
        /** @brief Constructor */
        inline Point(const Vector3& position): _position(position), _transformedPosition(position) {}

        inline void applyTransformation(const Matrix4& transformation) {
            _transformedPosition = (transformation*Vector4(_position)).xyz();
        }

        /** @brief Position */
        inline Vector3 position() const { return _position; }

        /** @brief Set position */
        inline void setPosition(const Vector3& position) {
            _position = position;
        }

        /** @brief Transformed position */
        inline Vector3 transformedPosition() const {
            return _transformedPosition;
        }

    protected:
        inline Type type() const { return Type::Point; }

    private:
        Vector3 _position, _transformedPosition;
};

}}

#endif
