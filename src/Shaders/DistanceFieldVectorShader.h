#ifndef Magnum_Shaders_DistanceFieldVectorShader_h
#define Magnum_Shaders_DistanceFieldVectorShader_h
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
 * @brief Class Magnum::Shaders::DistanceFieldVectorShader, typedef Magnum::Shaders::DistanceFieldVectorShader2D, Magnum::Shaders::DistanceFieldVectorShader3D
 */

#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "AbstractVectorShader.h"

#include "magnumShadersVisibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Distance field vector shader

Renders vector art in form of signed distance field. See TextureTools::distanceField()
for more information. Note that the final rendered outlook will greatly depend
on radius of input distance field and value passed to setSmoothness().
@see DistanceFieldVectorShader2D, DistanceFieldVectorShader3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT DistanceFieldVectorShader: public AbstractVectorShader<dimensions> {
    public:
        DistanceFieldVectorShader();

        /** @brief Set transformation and projection matrix */
        inline DistanceFieldVectorShader* setTransformationProjectionMatrix(const typename DimensionTraits<dimensions>::MatrixType& matrix) {
            AbstractShaderProgram::setUniform(transformationProjectionMatrixUniform, matrix);
            return this;
        }

        /**
         * @brief Set fill color
         * @return Pointer to self (for method chaining)
         *
         * @see setOutlineColor()
         */
        inline DistanceFieldVectorShader* setColor(const Color4<>& color) {
            AbstractShaderProgram::setUniform(colorUniform, color);
            return this;
        }

        /**
         * @brief Set outline color
         * @return Pointer to self (for method chaining)
         *
         * @see setOutlineRange(), setColor()
         */
        inline DistanceFieldVectorShader* setOutlineColor(const Color4<>& color) {
            AbstractShaderProgram::setUniform(outlineColorUniform, color);
            return this;
        }

        /**
         * @brief Set outline range
         * @return Pointer to self (for method chaining)
         *
         * Parameter @p start describes where fill ends and possible outline
         * starts. Initial value is `0.5f`, smaller values will make the vector
         * art look thinner, larger will make it look thicker.
         *
         * Parameter @p end describes where outline ends. If set to value
         * smaller than @p start the outline is not drawn. Initial value is
         * `0.0f`.
         *
         * @see setOutlineColor()
         */
        inline DistanceFieldVectorShader* setOutlineRange(Float start, Float end) {
            AbstractShaderProgram::setUniform(outlineRangeUniform, Vector2(start, end));
            return this;
        }

        /**
         * @brief Set smoothness radius
         * @return Pointer to self (for method chaining)
         *
         * Larger values will make edges look less aliased (but blurry), smaller
         * values will make them look more crisp (but possibly aliased). Initial
         * value is `0.04f`.
         */
        inline DistanceFieldVectorShader* setSmoothness(Float value) {
            AbstractShaderProgram::setUniform(smoothnessUniform, value);
            return this;
        }

    private:
        Int transformationProjectionMatrixUniform,
            colorUniform,
            outlineColorUniform,
            outlineRangeUniform,
            smoothnessUniform;
};

/** @brief Two-dimensional distance field vector shader */
typedef DistanceFieldVectorShader<2> DistanceFieldVectorShader2D;

/** @brief Three-dimensional distance field vector shader */
typedef DistanceFieldVectorShader<3> DistanceFieldVectorShader3D;

}}

#endif
