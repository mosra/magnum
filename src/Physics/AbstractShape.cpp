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

#include "AbstractShape.h"

#include <Utility/Debug.h>

namespace Magnum { namespace Physics {

template<std::uint8_t dimensions> bool AbstractShape<dimensions>::collides(const AbstractShape* other) const {
    /* Operate only with simpler types than this */
    if(static_cast<int>(other->type()) > static_cast<int>(type()))
        return other->collides(this);

    return false;
}

template class AbstractShape<2>;
template class AbstractShape<3>;

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

Debug operator<<(Debug debug, ShapeDimensionTraits<2>::Type value) {
    switch(value) {
        #define _val(value) case AbstractShape2D::Type::value: return debug << "AbstractShape2D::Type::" #value;
        _val(Point)
        _val(Line)
        _val(LineSegment)
        _val(Sphere)
        _val(Capsule)
        _val(AxisAlignedBox)
        _val(Box)
        _val(ShapeGroup)
        #undef _val
    }

    return debug << "AbstractShape2D::Type::(unknown)";
}

Debug operator<<(Debug debug, ShapeDimensionTraits<3>::Type value) {
    switch(value) {
        #define _val(value) case AbstractShape3D::Type::value: return debug << "AbstractShape3D::Type::" #value;
        _val(Point)
        _val(Line)
        _val(LineSegment)
        _val(Sphere)
        _val(Capsule)
        _val(AxisAlignedBox)
        _val(Box)
        _val(ShapeGroup)
        _val(Plane)
        #undef _val
    }

    return debug << "AbstractShape2D::Type::(unknown)";
}

}
#endif

}}
