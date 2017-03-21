#ifndef Magnum_Shapes_LineSegment_h
#define Magnum_Shapes_LineSegment_h
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
 * @brief Class @ref Magnum::Shapes::LineSegment, typedef @ref Magnum::Shapes::LineSegment2D, @ref Magnum::Shapes::LineSegment3D
 */

#include "Magnum/Shapes/Line.h"

namespace Magnum { namespace Shapes {

/**
@brief Line segment, defined by starting and ending point

See @ref shapes for brief introduction.
@see @ref LineSegment2D, @ref LineSegment3D
*/
template<UnsignedInt dimensions> class LineSegment: public Line<dimensions> {
    public:
        /**
         * @brief Default constructor
         *
         * Creates line segment with both points at origin.
         */
        constexpr /*implicit*/ LineSegment() {}

        /** @brief Constructor */
        constexpr /*implicit*/ LineSegment(const VectorTypeFor<dimensions, Float>& a, const VectorTypeFor<dimensions, Float>& b): Line<dimensions>(a, b) {}

        /** @brief Transformed shape */
        LineSegment<dimensions> transformed(const MatrixTypeFor<dimensions, Float>& matrix) const {
            return Line<dimensions>::transformed(matrix);
        }

    private:
        constexpr LineSegment(const Line<dimensions>& line): Line<dimensions>(line) {}
};

/** @brief Two-dimensional line segment */
typedef LineSegment<2> LineSegment2D;

/** @brief Three-dimensional line segment */
typedef LineSegment<3> LineSegment3D;

}}

#endif
