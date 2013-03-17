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

#include "AbstractShape.h"

#include <Utility/Debug.h>

namespace Magnum { namespace Physics {

template<UnsignedInt dimensions> bool AbstractShape<dimensions>::collides(const AbstractShape* other) const {
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
