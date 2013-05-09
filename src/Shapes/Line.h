#ifndef Magnum_Shapes_Line_h
#define Magnum_Shapes_Line_h
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
 * @brief Class Magnum::Shapes::Line, typedef Magnum::Shapes::Line2D, Magnum::Shapes::Line3D
 */

#include "Math/Vector3.h"
#include "DimensionTraits.h"
#include "Shapes/magnumShapesVisibility.h"

namespace Magnum { namespace Shapes {

/**
@brief Infinite line, defined by two points

See @ref shapes for brief introduction.
@see Line2D, Line3D
@todo collision detection of two Line2D
*/
template<UnsignedInt dimensions> class MAGNUM_SHAPES_EXPORT Line {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Dimension count */
        };

        /**
         * @brief Default constructor
         *
         * Creates line with both points at origin.
         */
        inline constexpr /*implicit*/ Line() {}

        /** @brief Constructor */
        inline constexpr /*implicit*/ Line(const typename DimensionTraits<dimensions>::VectorType& a, const typename DimensionTraits<dimensions>::VectorType& b): _a(a), _b(b) {}

        /** @brief Transformed shape */
        Line<dimensions> transformed(const typename DimensionTraits<dimensions>::MatrixType& matrix) const;

        /** @brief First point */
        inline constexpr typename DimensionTraits<dimensions>::VectorType a() const {
            return _a;
        }

        /** @brief Set first point */
        inline void setA(const typename DimensionTraits<dimensions>::VectorType& a) {
            _a = a;
        }

        /** @brief Second point */
        inline constexpr typename DimensionTraits<dimensions>::VectorType b() const {
            return _b;
        }

        /** @brief Set second point */
        inline void setB(const typename DimensionTraits<dimensions>::VectorType& b) {
            _b = b;
        }

    private:
        typename DimensionTraits<dimensions>::VectorType _a, _b;
};

/** @brief Infinite two-dimensional line */
typedef Line<2> Line2D;

/** @brief Infinite three-dimensional line */
typedef Line<3> Line3D;

}}

#endif
