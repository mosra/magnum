#ifndef Magnum_Shaders_Flat_h
#define Magnum_Shaders_Flat_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
 * @brief Class @ref Magnum::Shaders::Flat, typedef @ref Magnum::Shaders::Flat2D, @ref Magnum::Shaders::Flat3D
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"
#include "Magnum/Math/Matrix4.h"
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class FlatFlag: UnsignedByte { Textured = 1 << 0 };
    typedef Containers::EnumSet<FlatFlag> FlatFlags;
}

/**
@brief Flat shader

Draws whole mesh with given unshaded color or texture. For colored mesh you
need to provide @ref Position attribute in your triangle mesh and call at least
@ref setTransformationProjectionMatrix() and @ref setColor().

If you want to use texture, you need to provide also @ref TextureCoordinates
attribute. Pass @ref Flag::Textured to constructor and then at render time
don't forget to set also the texture via @ref bindTexture(). The texture is
multipled by the color, which is by default set to fully opaque white if
texturing is enabled.

For coloring the texture based on intensity you can use the @ref Vector shader.

@image html shaders-flat.png

@section Shaders-Flat-usage Example usage

@subsection Shaders-Flat-usage-colored Colored mesh

Common mesh setup:

@snippet MagnumShaders.cpp Flat-usage-colored1

Common rendering setup:

@snippet MagnumShaders.cpp Flat-usage-colored2

@subsection Shaders-Flat-usage-textured Textured mesh

Common mesh setup:

@snippet MagnumShaders.cpp Flat-usage-textured1

Common rendering setup:

@snippet MagnumShaders.cpp Flat-usage-textured2

@see @ref shaders, @ref Flat2D, @ref Flat3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT Flat: public GL::AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute", @ref Vector2 in 2D,
         * @ref Vector3 in 3D.
         */
        typedef typename Generic<dimensions>::Position Position;

        /**
         * @brief 2D texture coordinates
         *
         * @ref shaders-generic "Generic attribute", @ref Vector2. Used only if
         * @ref Flag::Textured is set.
         */
        typedef typename Generic<dimensions>::TextureCoordinates TextureCoordinates;

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            Textured = 1 << 0   /**< The shader uses texture instead of color */
        };

        /**
         * @brief Flags
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;
        #else
        typedef Implementation::FlatFlag Flag;
        typedef Implementation::FlatFlags Flags;
        #endif

        /**
         * @brief Constructor
         * @param flags     Flags
         */
        explicit Flat(Flags flags = {});

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         */
        explicit Flat(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        Flat(const Flat<dimensions>&) = delete;

        /** @brief Move constructor */
        Flat(Flat<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        Flat<dimensions>& operator=(const Flat<dimensions>&) = delete;

        /** @brief Move assignment */
        Flat<dimensions>& operator=(Flat<dimensions>&&) noexcept = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         */
        Flat<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
            setUniform(_transformationProjectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set color
         * @return Reference to self (for method chaining)
         *
         * If @ref Flag::Textured is set, default value is
         * @cpp 0xffffffff_rgbaf @ce and the color will be multiplied with
         * texture.
         * @see @ref bindTexture()
         */
        Flat<dimensions>& setColor(const Color4& color){
            setUniform(_colorUniform, color);
            return *this;
        }

        /**
         * @brief Bind texture
         * @return Reference to self (for method chaining)
         *
         * Has effect only if @ref Flag::Textured is set.
         * @see @ref setColor()
         */
        Flat<dimensions>& bindTexture(GL::Texture2D& texture);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief bindTexture()
         * @deprecated Use @ref bindTexture() instead.
         */
        CORRADE_DEPRECATED("use bindTexture() instead") Flat<dimensions>& setTexture(GL::Texture2D& texture) {
            return bindTexture(texture);
        }
        #endif

    private:
        Flags _flags;
        Int _transformationProjectionMatrixUniform{0},
            _colorUniform{1};
};

/** @brief 2D flat shader */
typedef Flat<2> Flat2D;

/** @brief 3D flat shader */
typedef Flat<3> Flat3D;

CORRADE_ENUMSET_OPERATORS(Implementation::FlatFlags)

}}

#endif
