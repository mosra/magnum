#ifndef Magnum_Physics_Implementation_ShapeShader_h
#define Magnum_Physics_Implementation_ShapeShader_h
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

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "AbstractShaderProgram.h"
#include "Color.h"
#include "DimensionTraits.h"

namespace Magnum { namespace Physics { namespace Implementation {

template<std::uint8_t dimensions> class ShapeShader: public AbstractShaderProgram {
    public:
        typedef Attribute<0, typename DimensionTraits<dimensions, GLfloat>::PointType> Position;

        ShapeShader();

        ShapeShader<dimensions>* setTransformationProjection(const typename DimensionTraits<dimensions, GLfloat>::MatrixType& matrix) {
            setUniform(transformationProjectionUniform, matrix);
            return this;
        }

        ShapeShader<dimensions>* setColor(const Color3<GLfloat>& color) {
            setUniform(colorUniform, color);
            return this;
        }

    private:
        GLint transformationProjectionUniform,
            colorUniform;
};

extern template class ShapeShader<2>;
extern template class ShapeShader<3>;

}}}

#endif
