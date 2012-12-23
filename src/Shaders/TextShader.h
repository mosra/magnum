#ifndef Magnum_Shaders_TextShader_h
#define Magnum_Shaders_TextShader_h
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
 * @brief Class Magnum::Shaders::TextShader
 */

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "AbstractTextShader.h"

#include "magnumShadersVisibility.h"

namespace Magnum { namespace Shaders {

/**
@brief %Text shader

@see TextShader2D, TextShader3D
*/
template<std::uint8_t dimensions> class MAGNUM_SHADERS_EXPORT TextShader: public AbstractTextShader<dimensions> {
    public:
        TextShader();

        /** @brief Set transformation and projection matrix */
        inline TextShader* setTransformationProjectionMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) {
            AbstractTextShader<dimensions>::setUniform(transformationProjectionMatrixUniform, matrix);
            return this;
        }

        /** @brief Set text color */
        inline TextShader* setColor(const Color3<>& color) {
            AbstractTextShader<dimensions>::setUniform(colorUniform, color);
            return this;
        }

    private:
        GLint transformationProjectionMatrixUniform = 0,
            colorUniform = 1;
};

/** @brief Two-dimensional text shader */
typedef TextShader<2> TextShader2D;

/** @brief Three-dimensional text shader */
typedef TextShader<3> TextShader3D;

}}

#endif
