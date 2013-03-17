#ifndef Magnum_Shaders_VertexColorShader_h
#define Magnum_Shaders_VertexColorShader_h
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
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT VertexColorShader: public AbstractShaderProgram {
    public:
        /** @brief Vertex position */
        typedef Attribute<0, typename DimensionTraits<dimensions>::VectorType> Position;

        /** @brief Vertex color */
        typedef Attribute<1, Color3<>> Color;

        explicit VertexColorShader();

        /**
         * @brief Set transformation and projection matrix
         * @return Pointer to self (for method chaining)
         */
        inline VertexColorShader<dimensions>* setTransformationProjectionMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) {
            setUniform(transformationProjectionMatrixUniform, matrix);
            return this;
        }

    private:
        Int transformationProjectionMatrixUniform;
};

/** @brief 2D vertex color shader */
typedef VertexColorShader<2> VertexColorShader2D;

/** @brief 3D vertex color shader */
typedef VertexColorShader<3> VertexColorShader3D;

}}

#endif
