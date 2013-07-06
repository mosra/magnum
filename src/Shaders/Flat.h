#ifndef Magnum_Shaders_Flat_h
#define Magnum_Shaders_Flat_h
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
 * @brief Class Magnum::Shaders::Flat
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
@see Flat2D, Flat3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT Flat: public AbstractShaderProgram {
    public:
        /** @brief Vertex position */
        typedef Attribute<0, typename DimensionTraits<dimensions, Float>::VectorType> Position;

        explicit Flat();

        /**
         * @brief Set transformation and projection matrix
         * @return Pointer to self (for method chaining)
         */
        Flat<dimensions>* setTransformationProjectionMatrix(const typename DimensionTraits<dimensions, Float>::MatrixType& matrix) {
            setUniform(transformationProjectionMatrixUniform, matrix);
            return this;
        }

        /**
         * @brief Set color
         * @return Pointer to self (for method chaining)
         */
        Flat<dimensions>* setColor(const Color4& color) {
            setUniform(colorUniform, color);
            return this;
        }

    private:
        Int transformationProjectionMatrixUniform,
            colorUniform;
};

/** @brief 2D flat shader */
typedef Flat<2> Flat2D;

/** @brief 3D flat shader */
typedef Flat<3> Flat3D;

}}

#endif
