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
#include "AbstractShape.h"

namespace Magnum { namespace Physics {

/**
@brief Axis-aligned box

@see AxisAlignedBox2D, AxisAlignedBox3D
*/
template<size_t dimensions> class PHYSICS_EXPORT AxisAlignedBox: public AbstractShape<dimensions> {
    public:
        /** @brief Constructor */
        inline AxisAlignedBox(const typename AbstractShape<dimensions>::VectorType& position, const typename AbstractShape<dimensions>::VectorType& size): _position(position), _transformedPosition(position), _size(size), _transformedSize(size) {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        void applyTransformation(const typename AbstractShape<dimensions>::MatrixType& transformation);
        #else
        void applyTransformation(const MatrixType& transformation);
        #endif

        /** @brief Position */
        inline typename AbstractShape<dimensions>::VectorType position() const {
            return _position;
        }

        /** @brief Set position */
        inline void setPosition(const typename AbstractShape<dimensions>::VectorType& position) {
            _position = position;
        }

        /** @brief Size */
        inline typename AbstractShape<dimensions>::VectorType size() const { return _size; }

        /** @brief Set size */
        inline void setSize(const typename AbstractShape<dimensions>::VectorType& size) {
            _size = size;
        }

        /** @brief Transformed position */
        inline typename AbstractShape<dimensions>::VectorType transformedPosition() const {
            return _transformedPosition;
        }

        /** @brief Transformed size */
        inline typename AbstractShape<dimensions>::VectorType transformedSize() const {
            return _transformedSize;
        }

    protected:
        inline typename AbstractShape<dimensions>::Type type() const {
            return AbstractShape<dimensions>::Type::AxisAlignedBox;
        }

    private:
        typename AbstractShape<dimensions>::VectorType _position, _transformedPosition,
            _size, _transformedSize;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
extern template class PHYSICS_EXPORT AxisAlignedBox<2>;
extern template class PHYSICS_EXPORT AxisAlignedBox<3>;
#endif

/** @brief Two-dimensional axis-aligned box */
typedef AxisAlignedBox<2> AxisAlignedBox2D;

/** @brief Three-dimensional axis-aligned box */
typedef AxisAlignedBox<3> AxisAlignedBox3D;

}}

#endif
