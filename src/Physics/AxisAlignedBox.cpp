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

#include "AxisAlignedBox.h"

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Physics/Point.h"

namespace Magnum { namespace Physics {

template<UnsignedInt dimensions> void AxisAlignedBox<dimensions>::applyTransformationMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) {
    _transformedMin = matrix.transformPoint(_min);
    _transformedMax = matrix.transformPoint(_max);
}

template<UnsignedInt dimensions> bool AxisAlignedBox<dimensions>::collides(const AbstractShape<dimensions>* other) const {
    if(other->type() == AbstractShape<dimensions>::Type::Point)
        return *this % *static_cast<const Point<dimensions>*>(other);

    return AbstractShape<dimensions>::collides(other);
}

template<UnsignedInt dimensions> bool AxisAlignedBox<dimensions>::operator%(const Point<dimensions>& other) const {
    return (other.transformedPosition() >= _transformedMin).all() &&
           (other.transformedPosition() < _transformedMax).all();
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_PHYSICS_EXPORT AxisAlignedBox<2>;
template class MAGNUM_PHYSICS_EXPORT AxisAlignedBox<3>;
#endif

}}
