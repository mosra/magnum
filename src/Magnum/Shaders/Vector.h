#ifndef Magnum_Shaders_Vector_h
#define Magnum_Shaders_Vector_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

/** @file
 * @brief Class @ref Magnum::Shaders::Vector, typedef @ref Magnum::Shaders::Vector2D, @ref Magnum::Shaders::Vector3D
 */

#include "Magnum/Color.h"
#include "Magnum/DimensionTraits.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/AbstractVector.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief %Vector shader

Renders vector art in plain grayscale form. See also @ref DistanceFieldVector
for more advanced effects. For rendering unchanged texture you can use the
@ref Flat shader.
@see @ref Vector2D, @ref Vector3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT Vector: public AbstractVector<dimensions> {
    public:
        Vector();

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         */
        Vector& setTransformationProjectionMatrix(const typename DimensionTraits<dimensions, Float>::MatrixType& matrix) {
            AbstractShaderProgram::setUniform(transformationProjectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set background color
         * @return Reference to self (for method chaining)
         *
         * Default is transparent black.
         * @see @ref setColor()
         */
        Vector& setBackgroundColor(const Color4& color) {
            AbstractShaderProgram::setUniform(backgroundColorUniform, color);
            return *this;
        }

        /**
         * @brief Set fill color
         * @return Reference to self (for method chaining)
         *
         * @see @ref setBackgroundColor()
         */
        Vector& setColor(const Color4& color) {
            AbstractShaderProgram::setUniform(colorUniform, color);
            return *this;
        }

    private:
        Int transformationProjectionMatrixUniform,
            backgroundColorUniform,
            colorUniform;
};

/** @brief Two-dimensional vector shader */
typedef Vector<2> Vector2D;

/** @brief Three-dimensional vector shader */
typedef Vector<3> Vector3D;

}}

#endif
