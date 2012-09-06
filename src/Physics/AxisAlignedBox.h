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
 * @brief Class Magnum::Physics::AxisAlignedBox
 */

#include "Math/Vector3.h"
#include "AbstractShape.h"

namespace Magnum { namespace Physics {

/** @brief Axis aligned box */
class PHYSICS_EXPORT AxisAlignedBox: public AbstractShape {
    public:
        /** @brief Constructor */
        inline AxisAlignedBox(const Vector3& position, const Vector3& size): _position(position), _transformedPosition(position), _size(size), _transformedSize(size) {}

        void applyTransformation(const Matrix4& transformation);

        /** @brief Position */
        inline Vector3 position() const { return _position; }

        /** @brief Set position */
        inline void setPosition(const Vector3& position) {
            _position = position;
        }

        /** @brief Size */
        inline Vector3 size() const { return _size; }

        /** @brief Set size */
        inline void setSize(const Vector3& size) {
            _size = size;
        }

        /** @brief Transformed position */
        inline Vector3 transformedPosition() const {
            return _transformedPosition;
        }

        /** @brief Transformed size */
        inline Vector3 transformedSize() const {
            return _transformedSize;
        }

    protected:
        inline Type type() const { return Type::AxisAlignedBox; }

    private:
        Vector3 _position, _transformedPosition,
            _size, _transformedSize;
};

}}

#endif
