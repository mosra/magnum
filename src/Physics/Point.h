#ifndef Magnum_Physics_Point_h
#define Magnum_Physics_Point_h
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
 * @brief Class Magnum::Physics::Point, typedef Magnum::Physics::Point2D, Magnum::Physics::Point3D
 */

#include "Math/Vector3.h"
#include "AbstractShape.h"

#include "corradeCompatibility.h"

namespace Magnum { namespace Physics {

/**
@brief %Point

@see Point2D, Point3D
*/
template<UnsignedInt dimensions> class MAGNUM_PHYSICS_EXPORT Point: public AbstractShape<dimensions> {
    public:
        /**
         * @brief Default constructor
         *
         * Creates point at origin.
         */
        inline explicit Point() {}

        /** @brief Constructor */
        inline explicit Point(const typename DimensionTraits<dimensions>::VectorType& position): _position(position), _transformedPosition(position) {}

        inline typename AbstractShape<dimensions>::Type type() const override {
            return AbstractShape<dimensions>::Type::Point;
        }

        void applyTransformationMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) override;

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
