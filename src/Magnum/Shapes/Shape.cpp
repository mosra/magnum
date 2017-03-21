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

#include "Shape.h"

#include "Magnum/Shapes/Composition.h"

namespace Magnum { namespace Shapes { namespace Implementation {

template<UnsignedInt dimensions> void ShapeHelper<Composition<dimensions>>::set(Shapes::Shape<Composition<dimensions>>& shape, const Composition<dimensions>& composition) {
    shape._transformedShape.shape = shape._shape.shape = composition;
}

template<UnsignedInt dimensions> void ShapeHelper<Composition<dimensions>>::set(Shapes::Shape<Composition<dimensions>>& shape, Composition<dimensions>&& composition) {
    shape._transformedShape.shape = shape._shape.shape = std::move(composition);
}

template<UnsignedInt dimensions> void ShapeHelper<Composition<dimensions>>::transform(Shapes::Shape<Composition<dimensions>>& shape, const MatrixTypeFor<dimensions, Float>& absoluteTransformationMatrix) {
    CORRADE_INTERNAL_ASSERT(shape._shape.shape.size() == shape._transformedShape.shape.size());
    for(std::size_t i = 0; i != shape.shape().size(); ++i)
        shape._shape.shape._shapes[i]->transform(absoluteTransformationMatrix, shape._transformedShape.shape._shapes[i]);
}

template struct MAGNUM_SHAPES_EXPORT ShapeHelper<Composition<2>>;
template struct MAGNUM_SHAPES_EXPORT ShapeHelper<Composition<3>>;

}}}
