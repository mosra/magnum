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

#include "Capsule.h"

#include "Math/Functions.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Geometry/Distance.h"
#include "Magnum.h"
#include "Shapes/Point.h"
#include "Shapes/Sphere.h"

using namespace Magnum::Math::Geometry;

namespace Magnum { namespace Shapes {

template<UnsignedInt dimensions> Capsule<dimensions> Capsule<dimensions>::transformed(const typename DimensionTraits<dimensions, Float>::MatrixType& matrix) const {
    return Capsule<dimensions>(matrix.transformPoint(_a), matrix.transformPoint(_b),
        (matrix.rotationScaling()*typename DimensionTraits<dimensions, Float>::VectorType(1/Constants::sqrt3())).length()*_radius);
}

template<UnsignedInt dimensions> bool Capsule<dimensions>::operator%(const Point<dimensions>& other) const {
    return Distance::lineSegmentPointSquared(_a, _b, other.position()) <
        Math::pow<2>(_radius);
}

template<UnsignedInt dimensions> bool Capsule<dimensions>::operator%(const Sphere<dimensions>& other) const {
    return Distance::lineSegmentPointSquared(_a, _b, other.position()) <
        Math::pow<2>(_radius+other.radius());
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_SHAPES_EXPORT Capsule<2>;
template class MAGNUM_SHAPES_EXPORT Capsule<3>;
#endif

}}
