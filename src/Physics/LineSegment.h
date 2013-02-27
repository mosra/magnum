#ifndef Magnum_Physics_LineSegment_h
#define Magnum_Physics_LineSegment_h
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
 * @brief Class Magnum::Physics::LineSegment, typedef Magnum::Physics::LineSegment2D, Magnum::Physics::LineSegment3D
 */

#include "Line.h"

namespace Magnum { namespace Physics {

/**
@brief %Line segment, defined by starting and ending point

@see LineSegment2D, LineSegment3D
*/
template<UnsignedInt dimensions> class LineSegment: public Line<dimensions> {
    public:
        /** @brief Constructor */
        inline explicit LineSegment(const typename DimensionTraits<dimensions>::VectorType& a, const typename DimensionTraits<dimensions>::VectorType& b): Line<dimensions>(a, b) {}

        inline typename AbstractShape<dimensions>::Type type() const override {
            return AbstractShape<dimensions>::Type::LineSegment;
        }
};

/** @brief Two-dimensional line segment */
typedef LineSegment<2> LineSegment2D;

/** @brief Three-dimensional line segment */
typedef LineSegment<3> LineSegment3D;

}}

#endif
