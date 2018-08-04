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
    enum class FlatFlag: UnsignedByte {
        Textured = 1 << 0,
        AlphaMask = 1 << 1
    };
    typedef Containers::EnumSet<FlatFlag> FlatFlags;
}

/**
@brief Flat shader

Draws the whole mesh with given color or texture. For a colored mesh you need
to provide the @ref Position attribute in your triangle mesh. By default, the
shader renders the mesh with a white color in an identity transformation.
Use @ref setTransformationProjectionMatrix(), @ref setColor() and others to
configure the shader.

If you want to use a texture, you need to provide also @ref TextureCoordinates
attribute. Pass @ref Flag::Textured to the constructor and then at render time
don't forget to bind also the texture via @ref bindTexture(). The texture is
multipled by the color, which is by default set to @cpp 0xffffffff_rgbaf @ce.

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

@subsection Shaders-Flat-usage-alpha Alpha blending and masking

Enable @ref Flag::AlphaMask and tune @ref setAlphaMask() for simple
binary alpha-masked drawing that doesn't require depth sorting or blending
enabled. Note that this feature is implemented using the GLSL @glsl discard @ce
operation which is known to have considerable performance impact on some
platforms. With proper depth sorting and blending you'll usually get much
better performance and output quality.

@see @ref shaders, @ref Flat2D, @ref Flat3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT Flat: public GL::AbstractShaderProgram {
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
         * @ref Magnum::Vector2 "Vector2". Used only if @ref Flag::Textured is
         * set.
         */
        typedef typename Generic<dimensions>::TextureCoordinates TextureCoordinates;

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            /**
             * Multiply color with a texture.
             * @see @ref setColor(), @ref setTexture()
             */
            Textured = 1 << 0,

            /**
             * Enable alpha masking. If the combined fragment color has an
             * alpha less than the value specified with @ref setAlphaMask(),
             * given fragment is discarded.
             *
             * This uses the @glsl discard @ce operation which is known to have
             * considerable performance impact on some platforms. While useful
             * for cheap alpha masking that doesn't require depth sorting,
             * with proper depth sorting and blending you'll usually get much
             * better performance and output quality.
             */
            AlphaMask = 1 << 1
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
         *
         * Initial value is an identity matrix.
         */
        Flat<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix) {
            setUniform(_transformationProjectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set color
         * @return Reference to self (for method chaining)
         *
         * If @ref Flag::Textured is set, initial value is
         * @cpp 0xffffffff_rgbaf @ce and the color will be multiplied with
         * texture.
         * @see @ref bindTexture()
         */
        Flat<dimensions>& setColor(const Color4& color){
            setUniform(_colorUniform, color);
            return *this;
        }

        /**
         * @brief Bind a color texture
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::Textured
         * enabled.
         * @see @ref setColor()
         */
        Flat<dimensions>& bindTexture(GL::Texture2D& texture);

        /**
         * @brief Set alpha mask value
         * @return Reference to self (for method chaining)
         *
         * Expects that the shader was created with @ref Flag::AlphaMask
         * enabled. Fragments with alpha values smaller than the mask value
         * will be discarded. Initial value is @cpp 0.5f @ce. See the flag
         * documentation for further information.
         */
        Flat<dimensions>& setAlphaMask(Float mask);

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
            _colorUniform{1},
            _alphaMaskUniform{2};
};

/** @brief 2D flat shader */
typedef Flat<2> Flat2D;

/** @brief 3D flat shader */
typedef Flat<3> Flat3D;

CORRADE_ENUMSET_OPERATORS(Implementation::FlatFlags)

}}

#endif
