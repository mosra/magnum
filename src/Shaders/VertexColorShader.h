#ifndef Magnum_Shaders_VertexColorShader_h
#define Magnum_Shaders_VertexColorShader_h
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
 * @brief Class Magnum::Shaders::VertexColorShader
 */

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "AbstractShaderProgram.h"
#include "Color.h"
#include "DimensionTraits.h"

#include "magnumShadersVisibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Vertex color shader

Draws vertex-colored mesh.
@see VertexColorShader2D, VertexColorShader3D
*/
template<std::uint8_t dimensions> class MAGNUM_SHADERS_EXPORT VertexColorShader: public AbstractShaderProgram {
    public:
        /** @brief Vertex position */
        typedef Attribute<0, typename DimensionTraits<dimensions>::PointType> Position;

        /** @brief Vertex color */
        typedef Attribute<1, Color3<>> Color;

        VertexColorShader();

        /**
         * @brief Set transformation and projection matrix
         * @return Pointer to self (for method chaining)
         */
        inline VertexColorShader<dimensions>* setTransformationProjectionMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) {
            setUniform(transformationProjectionMatrixUniform, matrix);
            return this;
        }

    private:
        GLint transformationProjectionMatrixUniform = 0;
};

/** @brief 2D vertex color shader */
typedef VertexColorShader<2> VertexColorShader2D;

/** @brief 3D vertex color shader */
typedef VertexColorShader<3> VertexColorShader3D;

}}

#endif
