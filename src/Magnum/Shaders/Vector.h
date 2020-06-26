#ifndef Magnum_Shaders_Vector_h
#define Magnum_Shaders_Vector_h
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
 * @brief Class @ref Magnum::Shaders::Vector, typedef @ref Magnum::Shaders::Vector2D, @ref Magnum::Shaders::Vector3D
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/Shaders/AbstractVector.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class VectorFlag: UnsignedByte {
        TextureTransformation = 1 << 0
    };
    typedef Containers::EnumSet<VectorFlag> VectorFlags;
}

/**
@brief Vector shader

Renders vector art in plain grayscale form. See also @ref DistanceFieldVector
for more advanced effects. For rendering an unchanged texture you can use the
@ref Flat shader. You need to provide the @ref Position and
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

@section Shaders-Vector-usage Example usage

Common mesh setup:

@snippet MagnumShaders.cpp Vector-usage1

Common rendering setup:

@snippet MagnumShaders.cpp Vector-usage2

@see @ref shaders, @ref Vector2D, @ref Vector3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT Vector: public AbstractVector<dimensions> {
    public:
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
        typedef Implementation::VectorFlag Flag;
        typedef Implementation::VectorFlags Flags;
        #endif

        /**
         * @brief Constructor
         * @param flags     Flags
         */
        explicit Vector(Flags flags = {});

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
        explicit Vector(NoCreateT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : AbstractVector<dimensions>{NoCreate}
            #endif
            {}

        /** @brief Copying is not allowed */
        Vector(const Vector<dimensions>&) = delete;

        /** @brief Move constructor */
        Vector(Vector<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        Vector<dimensions>& operator=(const Vector<dimensions>&) = delete;

        /** @brief Move assignment */
        Vector<dimensions>& operator=(Vector<dimensions>&&) noexcept = default;

        /**
         * @brief Flags
         * @m_since{2020,06}
         */
        Flags flags() const { return _flags; }

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Default is an identity matrix.
         */
        Vector<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @brief Set texture coordinate transformation matrix
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the shader was created with
         * @ref Flag::TextureTransformation enabled. Initial value is an
         * identity matrix.
         */
        Vector<dimensions>& setTextureMatrix(const Matrix3& matrix);

        /**
         * @brief Set background color
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp 0x00000000_rgbaf @ce.
         * @see @ref setColor()
         */
        Vector<dimensions>& setBackgroundColor(const Color4& color);

        /**
         * @brief Set fill color
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp 0xffffffff_rgbaf @ce.
         * @see @ref setBackgroundColor()
         */
        Vector<dimensions>& setColor(const Color4& color);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Overloads to remove WTF-factor from method chaining order */
        Vector<dimensions>& bindVectorTexture(GL::Texture2D& texture) {
            AbstractVector<dimensions>::bindVectorTexture(texture);
            return *this;
        }
        #endif

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

/** @brief Two-dimensional vector shader */
typedef Vector<2> Vector2D;

/** @brief Three-dimensional vector shader */
typedef Vector<3> Vector3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{Vector,Vector::Flag} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, Vector<dimensions>::Flag value);

/** @debugoperatorclassenum{Vector,Vector::Flags} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, Vector<dimensions>::Flags value);
#else
namespace Implementation {
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, VectorFlag value);
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, VectorFlags value);
    CORRADE_ENUMSET_OPERATORS(VectorFlags)
}
#endif

}}

#endif
