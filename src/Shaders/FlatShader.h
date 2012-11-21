#ifndef Magnum_Shaders_FlatShader_h
#define Magnum_Shaders_FlatShader_h
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
 * @brief Class Magnum::Shaders::FlatShader
 */

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "AbstractShaderProgram.h"
#include "Color.h"
#include "DimensionTraits.h"

#include "magnumShadersVisibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Flat shader

Draws whole mesh with one color.
@see FlatShader2D, FlatShader3D
*/
template<std::uint8_t dimensions> class SHADERS_EXPORT FlatShader: public AbstractShaderProgram {
    public:
        /** @brief Vertex position */
        typedef Attribute<0, typename DimensionTraits<dimensions>::PointType> Position;

        FlatShader();

        /**
         * @brief Set transformation and projection matrix
         * @return Pointer to self (for method chaining)
         */
        FlatShader<dimensions>* setTransformationProjection(const typename DimensionTraits<dimensions>::MatrixType& matrix) {
            setUniform(transformationProjectionUniform, matrix);
            return this;
        }

        /**
         * @brief Set color
         * @return Pointer to self (for method chaining)
         */
        FlatShader<dimensions>* setColor(const Color3<GLfloat>& color) {
            setUniform(colorUniform, color);
            return this;
        }

    private:
        GLint transformationProjectionUniform,
            colorUniform;
};

/** @brief 2D flat shader */
typedef FlatShader<2> FlatShader2D;

/** @brief 3D flat shader */
typedef FlatShader<3> FlatShader3D;

}}

#endif
