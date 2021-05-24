#ifndef Magnum_Shaders_AbstractVectorGL_h
#define Magnum_Shaders_AbstractVectorGL_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Shaders::AbstractVectorGL, typedef @ref Magnum::Shaders::AbstractVectorGL2D, @ref Magnum::Shaders::AbstractVectorGL3D
 * @m_since_latest
 */

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"

namespace Magnum { namespace Shaders {

/**
@brief Base for vector OpenGL shaders
@m_since_latest

See @ref DistanceFieldVectorGL and @ref VectorGL for more information.
@see @ref shaders, @ref AbstractVectorGL2D, @ref AbstractVectorGL3D
*/
template<UnsignedInt dimensions> class AbstractVectorGL: public GL::AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2" in 2D, @ref Magnum::Vector3 "Vector3"
         * in 3D.
         */
        typedef typename GenericGL<dimensions>::Position Position;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2".
         */
        typedef typename GenericGL<dimensions>::TextureCoordinates TextureCoordinates;

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             */
            ColorOutput = GenericGL<dimensions>::ColorOutput
        };

        /** @brief Copying is not allowed */
        AbstractVectorGL(const AbstractVectorGL<dimensions>&) = delete;

        /** @brief Move constructor */
        AbstractVectorGL(AbstractVectorGL<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        AbstractVectorGL<dimensions>& operator=(const AbstractVectorGL<dimensions>&) = delete;

        /** @brief Move assignment */
        AbstractVectorGL<dimensions>& operator=(AbstractVectorGL<dimensions>&&) noexcept = default;

        /** @{
         * @name Texture binding
         */

        /**
         * @brief Bind vector texture
         * @return Reference to self (for method chaining)
         *
         * @see @ref DistanceFieldVectorGL::Flag::TextureTransformation,
         *      @ref VectorGL::Flag::TextureTransformation,
         *      @ref DistanceFieldVectorGL::setTextureMatrix(),
         *      @ref VectorGL::setTextureMatrix()
         */
        AbstractVectorGL<dimensions>& bindVectorTexture(GL::Texture2D& texture);

        /**
         * @}
         */

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

        explicit AbstractVectorGL(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}
        explicit AbstractVectorGL() = default;
        ~AbstractVectorGL() = default;
};

/**
@brief Base for two-dimensional vector OpenGL shaders
@m_since_latest
*/
typedef AbstractVectorGL<2> AbstractVectorGL2D;

/**
@brief Base for three-dimensional vector OpenGL shader
@m_since_latest
*/
typedef AbstractVectorGL<3> AbstractVectorGL3D;

}}

#endif
