#ifndef Magnum_Shaders_AbstractVector_h
#define Magnum_Shaders_AbstractVector_h
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
 * @brief Class @ref Magnum::Shaders::AbstractVector, typedef @ref Magnum::Shaders::AbstractVector2D, @ref Magnum::Shaders::AbstractVector3D
 */

#include "Magnum/Shaders/Generic.h"

namespace Magnum { namespace Shaders {

/**
@brief Base for vector shaders

@see @ref AbstractVector2D, @ref AbstractVector3D
*/
template<UnsignedInt dimensions> class AbstractVector: public AbstractShaderProgram {
    public:
        /** @brief Vertex position */
        typedef typename Generic<dimensions>::Position Position;

        /** @brief Texture coordinates */
        typedef typename Generic<dimensions>::TextureCoordinates TextureCoordinates;

        #ifdef MAGNUM_BUILD_DEPRECATED
        enum: Int {
            /**
             * Vector texture binding unit
             * @deprecated Use @ref Magnum::Shaders::AbstractVector::setVectorTexture() "setVectorTexture()" instead.
             */
            VectorTextureLayer = 16
        };
        #endif

        /**
         * @brief Set vector texture
         * @return Reference to self (for method chaining)
         */
        AbstractVector<dimensions>& setVectorTexture(Texture2D& texture);

    protected:
        explicit AbstractVector();
        ~AbstractVector();

        #ifndef MAGNUM_BUILD_DEPRECATED
        enum: Int { VectorTextureLayer = 16 };
        #endif
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
