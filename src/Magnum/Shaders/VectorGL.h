#ifndef Magnum_Shaders_VectorGL_h
#define Magnum_Shaders_VectorGL_h
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
 * @brief Class @ref Magnum::Shaders::VectorGL, typedef @ref Magnum::Shaders::VectorGL2D, @ref Magnum::Shaders::VectorGL3D
 * @m_since_latest
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class VectorGLFlag: UnsignedByte {
        TextureTransformation = 1 << 0
    };
    typedef Containers::EnumSet<VectorGLFlag> VectorGLFlags;
}

/**
@brief Vector OpenGL shader
@m_since_latest

Renders vector art in plain grayscale form. See also @ref DistanceFieldVectorGL
for more advanced effects. For rendering an unchanged texture you can use the
@ref FlatGL shader. You need to provide the @ref Position and
@ref TextureCoordinates attributes in your triangle mesh and call at least
@ref bindVectorTexture(). By default, the shader renders the texture with a
white color in an identity transformation. Use
@ref setTransformationProjectionMatrix(), @ref setColor() and others to
configure the shader.

@image html shaders-vector.png width=256px

Alpha / transparency is supported by the shader implicitly, but to have it
working on the framebuffer, you need to enable
@ref GL::Renderer::Feature::Blending and set up the blending function. See
@ref GL::Renderer::setBlendFunction() for details.

@section Shaders-VectorGL-usage Example usage

Common mesh setup:

@snippet MagnumShaders-gl.cpp VectorGL-usage1

Common rendering setup:

@snippet MagnumShaders-gl.cpp VectorGL-usage2

@see @ref shaders, @ref VectorGL2D, @ref VectorGL3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT VectorGL: public GL::AbstractShaderProgram {
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

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Flag
         * @m_since{2020,06}
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            /**
             * Enable texture coordinate transformation.
             * @see @ref setTextureMatrix()
             * @m_since{2020,06}
             */
            TextureTransformation = 1 << 0
        };

        /**
         * @brief Flags
         * @m_since{2020,06}
         *
         * @see @ref flags()
         */
        typedef Containers::EnumSet<Flag> Flags;
        #else
        /* Done this way to be prepared for possible future diversion of 2D
           and 3D flags (e.g. introducing 3D-specific features) */
        typedef Implementation::VectorGLFlag Flag;
        typedef Implementation::VectorGLFlags Flags;
        #endif

        /**
         * @brief Constructor
         * @param flags     Flags
         */
        explicit VectorGL(Flags flags = {});

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to a moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         */
        explicit VectorGL(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        VectorGL(const VectorGL<dimensions>&) = delete;

        /** @brief Move constructor */
        VectorGL(VectorGL<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        VectorGL<dimensions>& operator=(const VectorGL<dimensions>&) = delete;

        /** @brief Move assignment */
        VectorGL<dimensions>& operator=(VectorGL<dimensions>&&) noexcept = default;

        /**
         * @brief Flags
         * @m_since{2020,06}
         */
        Flags flags() const { return _flags; }

        /** @{
         * @name Uniform setters
         */

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix.
         */
        VectorGL<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @brief Set texture coordinate transformation matrix
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the shader was created with
         * @ref Flag::TextureTransformation enabled. Initial value is an
         * identity matrix.
         */
        VectorGL<dimensions>& setTextureMatrix(const Matrix3& matrix);

        /**
         * @brief Set background color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x00000000_rgbaf @ce.
         * @see @ref setColor()
         */
        VectorGL<dimensions>& setBackgroundColor(const Color4& color);

        /**
         * @brief Set fill color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce.
         * @see @ref setBackgroundColor()
         */
        VectorGL<dimensions>& setColor(const Color4& color);

        /**
         * @}
         */

        /** @{
         * @name Texture binding
         */

        /**
         * @brief Bind vector texture
         * @return Reference to self (for method chaining)
         *
         * @see @ref Flag::TextureTransformation, @ref setTextureMatrix()s
         */
        VectorGL<dimensions>& bindVectorTexture(GL::Texture2D& texture);

        /**
         * @}
         */

    private:
        /* Prevent accidentally calling irrelevant functions */
        #ifndef MAGNUM_TARGET_GLES
        using GL::AbstractShaderProgram::drawTransformFeedback;
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        using GL::AbstractShaderProgram::dispatchCompute;
        #endif

        Flags _flags;
        Int _transformationProjectionMatrixUniform{0},
            _textureMatrixUniform{1},
            _backgroundColorUniform{2},
            _colorUniform{3};
};

/**
@brief Two-dimensional vector OpenGL shader
@m_since_latest
*/
typedef VectorGL<2> VectorGL2D;

/**
@brief Three-dimensional vector OpenGL shader
@m_since_latest
*/
typedef VectorGL<3> VectorGL3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{VectorGL,VectorGL::Flag} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, VectorGL<dimensions>::Flag value);

/** @debugoperatorclassenum{VectorGL,VectorGL::Flags} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, VectorGL<dimensions>::Flags value);
#else
namespace Implementation {
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, VectorGLFlag value);
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, VectorGLFlags value);
    CORRADE_ENUMSET_OPERATORS(VectorGLFlags)
}
#endif

}}

#endif
