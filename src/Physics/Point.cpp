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

#include "Point.h"

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"

namespace Magnum { namespace Physics {

template<UnsignedInt dimensions> void Point<dimensions>::applyTransformationMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) {
    _transformedPosition = matrix.transformPoint(_position);
}

template class Point<2>;
template class Point<3>;

}}
