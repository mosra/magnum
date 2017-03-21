#ifndef Magnum_Shapes_Line_h
#define Magnum_Shapes_Line_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Shapes::Line, typedef @ref Magnum::Shapes::Line2D, @ref Magnum::Shapes::Line3D
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Shapes/visibility.h"

namespace Magnum { namespace Shapes {

/**
@brief Infinite line, defined by two points

See @ref shapes for brief introduction.
@see @ref Line2D, @ref Line3D
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
        constexpr /*implicit*/ Line() {}

        /** @brief Constructor */
        constexpr /*implicit*/ Line(const VectorTypeFor<dimensions, Float>& a, const typename DimensionTraits<dimensions, Float>::VectorType& b): _a(a), _b(b) {}

        /** @brief Transformed shape */
        Line<dimensions> transformed(const MatrixTypeFor<dimensions, Float>& matrix) const;

        /** @brief First point */
        constexpr VectorTypeFor<dimensions, Float> a() const {
            return _a;
        }

        /** @brief Set first point */
        void setA(const VectorTypeFor<dimensions, Float>& a) {
            _a = a;
        }

        /** @brief Second point */
        constexpr VectorTypeFor<dimensions, Float> b() const {
            return _b;
        }

        /** @brief Set second point */
        void setB(const VectorTypeFor<dimensions, Float>& b) {
            _b = b;
        }

    private:
        VectorTypeFor<dimensions, Float> _a, _b;
};

/** @brief Infinite two-dimensional line */
typedef Line<2> Line2D;

/** @brief Infinite three-dimensional line */
typedef Line<3> Line3D;

}}

#endif
