#ifndef Magnum_Shapes_Point_h
#define Magnum_Shapes_Point_h
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
 * @brief Class Magnum::Shapes::Point, typedef Magnum::Shapes::Point2D, Magnum::Shapes::Point3D
 */

#include "Math/Vector3.h"
#include "DimensionTraits.h"
#include "Shapes/magnumShapesVisibility.h"

namespace Magnum { namespace Shapes {

/**
@brief %Point

See @ref shapes for brief introduction.
@see Point2D, Point3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHAPES_EXPORT Point {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Dimension count */
        };

        /**
         * @brief Default constructor
         *
         * Creates point at origin.
         */
        inline constexpr /*implicit*/ Point() {}

        /** @brief Constructor */
        inline constexpr /*implicit*/ Point(const typename DimensionTraits<dimensions>::VectorType& position): _position(position) {}

        /** @brief Transformed shape */
        Point<dimensions> transformed(const typename DimensionTraits<dimensions>::MatrixType& matrix) const;

        /** @brief Position */
        inline constexpr typename DimensionTraits<dimensions>::VectorType position() const {
            return _position;
        }

        /** @brief Set position */
        inline void setPosition(const typename DimensionTraits<dimensions>::VectorType& position) {
            _position = position;
        }

    private:
        typename DimensionTraits<dimensions>::VectorType _position;
};

/** @brief Two-dimensional point */
typedef Point<2> Point2D;

/** @brief Three-dimensional point */
typedef Point<3> Point3D;

}}

#endif
