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

#include "shapeImplementation.h"

#include <Corrade/Utility/Debug.h>

namespace Magnum { namespace Shapes { namespace Implementation {

Debug& operator<<(Debug& debug, ShapeDimensionTraits<2>::Type value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _val(value) case ShapeDimensionTraits<2>::Type::value: return debug << "Shapes::Shape2D::Type::" #value;
        _val(Point)
        _val(Line)
        _val(LineSegment)
        _val(Sphere)
        _val(InvertedSphere)
        _val(Capsule)
        _val(Cylinder)
        _val(AxisAlignedBox)
        _val(Box)
        _val(Composition)
        #undef _val
        /* LCOV_EXCL_STOP */
    }

    return debug << "Shapes::Shape2D::Type(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, ShapeDimensionTraits<3>::Type value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _val(value) case ShapeDimensionTraits<3>::Type::value: return debug << "Shapes::Shape3D::Type::" #value;
        _val(Point)
        _val(Line)
        _val(LineSegment)
        _val(Sphere)
        _val(InvertedSphere)
        _val(Capsule)
        _val(Cylinder)
        _val(AxisAlignedBox)
        _val(Box)
        _val(Plane)
        _val(Composition)
        #undef _val
        /* LCOV_EXCL_STOP */
    }

    return debug << "Shapes::Shape3D::Type(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

template<UnsignedInt dimensions> AbstractShape<dimensions>::~AbstractShape() = default;
template<UnsignedInt dimensions> AbstractShape<dimensions>::AbstractShape() = default;

template struct AbstractShape<2>;
template struct AbstractShape<3>;

}}}
