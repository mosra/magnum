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
 * @brief Class Magnum::Physics::LineSegment
 */

#include "Line.h"

namespace Magnum { namespace Physics {

/** @brief %Line segment, defined by starting and ending point */
class LineSegment: public Line {
    public:
        /** @brief Constructor */
        inline LineSegment(const Vector3& a, const Vector3& b): Line(a, b) {}

    protected:
        inline Type type() const { return Type::LineSegment; }
};

}}

#endif
