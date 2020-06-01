#ifndef Magnum_Shaders_AbstractVector_h
#define Magnum_Shaders_AbstractVector_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/Generic.h"

namespace Magnum { namespace Shaders {

/**
@brief Base for vector shaders

See @ref DistanceFieldVector and @ref Vector for more information.
@see @ref shaders, @ref AbstractVector2D, @ref AbstractVector3D
*/
template<UnsignedInt dimensions> class AbstractVector: public GL::AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2" in 2D, @ref Magnum::Vector3 "Vector3"
         * in 3D.
         */
        typedef typename Generic<dimensions>::Position Position;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2".
         */
        typedef typename Generic<dimensions>::TextureCoordinates TextureCoordinates;

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             */
            ColorOutput = Generic<dimensions>::ColorOutput
        };

        /** @brief Copying is not allowed */
        AbstractVector(const AbstractVector<dimensions>&) = delete;

        /** @brief Move constructor */
        AbstractVector(AbstractVector<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        AbstractVector<dimensions>& operator=(const AbstractVector<dimensions>&) = delete;

        /** @brief Move assignment */
        AbstractVector<dimensions>& operator=(AbstractVector<dimensions>&&) noexcept = default;

        /**
         * @brief Bind vector texture
         * @return Reference to self (for method chaining)
         *
         * @see @ref DistanceFieldVector::Flag::TextureTransformation,
         *      @ref Vector::Flag::TextureTransformation,
         *      @ref DistanceFieldVector::setTextureMatrix(),
         *      @ref Vector::setTextureMatrix()
         */
        AbstractVector<dimensions>& bindVectorTexture(GL::Texture2D& texture);

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /* Those textures are quite specific (and likely reused multiple times
           per frame for e.g. text rendering, so put them in a specific slot.
           Older iOS (and iOS WebGL) has only 8 texture units, so can't go
           above that. Unit 7 is used by TextureTools::DistanceField. */
        enum: Int { VectorTextureUnit = 6 };

        explicit AbstractVector(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}
        explicit AbstractVector() = default;
        ~AbstractVector() = default;
};

/** @brief Base for two-dimensional text shaders */
typedef AbstractVector<2> AbstractVector2D;

/** @brief Base for three-dimensional text shader */
typedef AbstractVector<3> AbstractVector3D;

}}

#endif
