#ifndef Magnum_Shaders_AbstractVector_h
#define Magnum_Shaders_AbstractVector_h
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
 * @brief Class Magnum::Shaders::AbstractVector, typedef Magnum::Shaders::AbstractVector2D, Magnum::Shaders::AbstractVector3D
 */

#include "AbstractShaderProgram.h"
#include "Color.h"
#include "DimensionTraits.h"

namespace Magnum { namespace Shaders {

/**
@brief Base for vector shaders

@see AbstractVector2D, AbstractVector3D
*/
template<UnsignedInt dimensions> class AbstractVector: public AbstractShaderProgram {
    public:
        /** @brief Vertex position */
        typedef Attribute<0, typename DimensionTraits<dimensions, Float>::VectorType> Position;

        /** @brief Texture coordinates */
        typedef Attribute<1, Vector2> TextureCoordinates;

        enum: Int {
            VectorTextureLayer = 16 /**< Layer for vector texture */
        };

    protected:
        explicit AbstractVector();
        ~AbstractVector();
};

/** @brief Base for two-dimensional text shaders */
typedef AbstractVector<2> AbstractVector2D;

/** @brief Base for three-dimensional text shader */
typedef AbstractVector<3> AbstractVector3D;

/* GCC 4.5 doesn't like it defaulted in class body */
template<UnsignedInt dimensions> AbstractVector<dimensions>::AbstractVector() = default;
template<UnsignedInt dimensions> AbstractVector<dimensions>::~AbstractVector() = default;

}}

#endif
