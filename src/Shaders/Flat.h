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
#include "Color.h"
#include "DimensionTraits.h"
#include "Shaders/Generic.h"

#include "magnumShadersVisibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class FlatFlag: UnsignedByte { Textured = 1 << 0 };
    typedef Containers::EnumSet<FlatFlag, UnsignedByte> FlatFlags;
}

/**
@brief %Flat shader

Draws whole mesh with given unshaded color or texture. For colored mesh you
need to provide @ref Position attribute in your triangle mesh and call at least
@ref setTransformationProjectionMatrix() and @ref setColor().

If you want to use texture instead of color, you need to provide also
@ref TextureCoordinates attribute. Pass @ref Flag::Textured to constructor and
then at render time bind the texture to its respective layer instead of calling
@ref setColor(). Example:
@code
Shaders::Flat2D shader(Shaders::Flat2D::Flag::Textured);

// ...

myTexture.bind(Shaders::Flat2D::TextureLayer);
@endcode

@see @ref Flat2D, @ref Flat3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT Flat: public AbstractShaderProgram {
    public:
        /** @brief Vertex position */
        typedef typename Generic<dimensions>::Position Position;

        /**
         * @brief Texture coordinates
         *
         * Used only if @ref Flag::Textured is set.
         */
        typedef typename Generic<dimensions>::TextureCoordinates TextureCoordinates;

        enum: Int {
            /** Layer for color texture. Used only if @ref Flag::Textured is set. */
            TextureLayer = 0
        };

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Shader flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            Textured = 1 << 0   /**< The shader uses texture instead of color */
        };

        /**
         * @brief Shader flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag, UnsignedByte> Flags;
        #else
        typedef Implementation::FlatFlag Flag;
        typedef Implementation::FlatFlags Flags;
        #endif

        /**
         * @brief Constructor
         * @param flags     Shader flags
         */
        explicit Flat(Flags flags = Flags());

        /** @brief Shader flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         */
        Flat<dimensions>& setTransformationProjectionMatrix(const typename DimensionTraits<dimensions, Float>::MatrixType& matrix) {
            setUniform(transformationProjectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set color
         * @return Reference to self (for method chaining)
         *
         * Has no effect if @ref Flag::Textured is set.
         */
        Flat<dimensions>& setColor(const Color4& color);

    private:
        Int transformationProjectionMatrixUniform,
            colorUniform;

        Flags _flags;
};

/** @brief 2D flat shader */
typedef Flat<2> Flat2D;

/** @brief 3D flat shader */
typedef Flat<3> Flat3D;

CORRADE_ENUMSET_OPERATORS(Implementation::FlatFlags)

template<UnsignedInt dimensions> inline Flat<dimensions>& Flat<dimensions>::setColor(const Color4& color) {
    if(!(_flags & Flag::Textured)) setUniform(colorUniform, color);
    return *this;
}

}}

#endif
