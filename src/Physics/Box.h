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

namespace Magnum { namespace Physics {

/**
@brief Unit-size box with assigned transformation matrix

@see Box2D, Box3D
*/
template<size_t dimensions> class PHYSICS_EXPORT Box: public AbstractShape<dimensions> {
    public:
        /** @brief Constructor */
        inline Box(const typename AbstractShape<dimensions>::MatrixType& transformation): _transformation(transformation), _transformedTransformation(transformation) {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        void applyTransformation(const typename AbstractShape<dimensions>::MatrixType& transformation);
        #else
        void applyTransformation(const MatrixType& transformation);
        #endif

        /** @brief Transformation */
        inline typename AbstractShape<dimensions>::MatrixType transformation() const {
            return _transformation;
        }

        /** @brief Set transformation */
        inline void setTransformation(const typename AbstractShape<dimensions>::MatrixType& transformation) {
            _transformation = transformation;
        }

        /** @brief Transformed transformation */
        inline typename AbstractShape<dimensions>::MatrixType transformedTransformation() const {
            return _transformedTransformation;
        }

    protected:
        inline typename AbstractShape<dimensions>::Type type() const {
            return AbstractShape<dimensions>::Type::Box;
        }

    private:
        typename AbstractShape<dimensions>::MatrixType _transformation,
            _transformedTransformation;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
extern template class PHYSICS_EXPORT Box<2>;
extern template class PHYSICS_EXPORT Box<3>;
#endif

/** @brief Two-dimensional box */
typedef Box<2> Box2D;

/** @brief Three-dimensional box */
typedef Box<3> Box3D;

}}

#endif
