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

#include "ObjectShape.h"

#include "Physics/ShapeGroup.h"

namespace Magnum { namespace Physics { namespace Implementation {

template<UnsignedInt dimensions> void ObjectShapeHelper<ShapeGroup<dimensions>>::set(ObjectShape<ShapeGroup<dimensions>>& objectShape, const ShapeGroup<dimensions>& shape) {
    objectShape._transformedShape.shape = objectShape._shape.shape = shape;
}

template<UnsignedInt dimensions> void ObjectShapeHelper<ShapeGroup<dimensions>>::set(ObjectShape<ShapeGroup<dimensions>>& objectShape, ShapeGroup<dimensions>&& shape) {
    objectShape._transformedShape.shape = objectShape._shape.shape = std::move(shape);
}

template<UnsignedInt dimensions> void ObjectShapeHelper<ShapeGroup<dimensions>>::transform(ObjectShape<ShapeGroup<dimensions>>& objectShape, const typename DimensionTraits<dimensions>::MatrixType& absoluteTransformationMatrix) {
    CORRADE_INTERNAL_ASSERT(objectShape._shape.shape.size() == objectShape._transformedShape.shape.size());
    for(std::size_t i = 0; i != objectShape.shape().size(); ++i)
        objectShape._shape.shape._shapes[i]->transform(absoluteTransformationMatrix, objectShape._transformedShape.shape._shapes[i]);
}

template struct MAGNUM_PHYSICS_EXPORT ObjectShapeHelper<ShapeGroup<2>>;
template struct MAGNUM_PHYSICS_EXPORT ObjectShapeHelper<ShapeGroup<3>>;

}}}
