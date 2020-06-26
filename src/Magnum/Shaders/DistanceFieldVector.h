#ifndef Magnum_Shaders_DistanceFieldVector_h
#define Magnum_Shaders_DistanceFieldVector_h
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
 * @brief Class @ref Magnum::Shaders::DistanceFieldVector, typedef @ref Magnum::Shaders::DistanceFieldVector2D, @ref Magnum::Shaders::DistanceFieldVector3D
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/Shaders/AbstractVector.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class DistanceFieldVectorFlag: UnsignedByte {
        TextureTransformation = 1 << 0
    };
    typedef Containers::EnumSet<DistanceFieldVectorFlag> DistanceFieldVectorFlags;
}

/**
@brief Distance field vector shader

Renders vector graphics in a form of signed distance field. See
@ref TextureTools::DistanceField for more information. Note that the final
rendered outlook will greatly depend on radius of input distance field and
value passed to @ref setSmoothness(). You need to provide @ref Position and
@ref TextureCoordinates attributes in your triangle mesh and call at least
@ref bindVectorTexture(). By default, the shader renders the distance field
texture with a white color in an identity transformation, use
@ref setTransformationProjectionMatrix(), @ref setColor() and others to
configure the shader.

Alpha / transparency is supported by the shader implicitly, but to have it
working on the framebuffer, you need to enable
@ref GL::Renderer::Feature::Blending and set up the blending function. See
@ref GL::Renderer::setBlendFunction() for details.

@image html shaders-distancefieldvector.png width=256px

@section Shaders-DistanceFieldVector-usage Example usage

Common mesh setup:

@snippet MagnumShaders.cpp DistanceFieldVector-usage1

Common rendering setup:

@snippet MagnumShaders.cpp DistanceFieldVector-usage2

@see @ref shaders, @ref DistanceFieldVector2D, @ref DistanceFieldVector3D
@todo Use fragment shader derivations to have proper smoothness in perspective/
    large zoom levels, make it optional as it might have negative performance
    impact
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT DistanceFieldVector: public AbstractVector<dimensions> {
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
        typedef Implementation::DistanceFieldVectorFlag Flag;
        typedef Implementation::DistanceFieldVectorFlags Flags;
        #endif

        /**
         * @brief Constructor
         * @param flags     Flags
         */
        explicit DistanceFieldVector(Flags flags = {});

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
        explicit DistanceFieldVector(NoCreateT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : AbstractVector<dimensions>{NoCreate}
            #endif
            {}

        /** @brief Copying is not allowed */
        DistanceFieldVector(const DistanceFieldVector<dimensions>&) = delete;

        /** @brief Move constructor */
        DistanceFieldVector(DistanceFieldVector<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        DistanceFieldVector<dimensions>& operator=(const DistanceFieldVector<dimensions>&) = delete;

        /** @brief Move assignment */
        DistanceFieldVector<dimensions>& operator=(DistanceFieldVector<dimensions>&&) noexcept = default;

        /**
         * @brief Flags
         * @m_since{2020,06}
         */
        Flags flags() const { return _flags; }

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix.
         */
        DistanceFieldVector<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @brief Set texture coordinate transformation matrix
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the shader was created with
         * @ref Flag::TextureTransformation enabled. Initial value is an
         * identity matrix.
         */
        DistanceFieldVector<dimensions>& setTextureMatrix(const Matrix3& matrix);

        /**
         * @brief Set fill color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce.
         * @see @ref setOutlineColor()
         */
        DistanceFieldVector<dimensions>& setColor(const Color4& color);

        /**
         * @brief Set outline color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x00000000_rgbaf @ce and the outline is not
         * drawn --- see @ref setOutlineRange() for more information.
         * @see @ref setOutlineRange(), @ref setColor()
         */
        DistanceFieldVector<dimensions>& setOutlineColor(const Color4& color);

        /**
         * @brief Set outline range
         * @return Reference to self (for method chaining)
         *
         * The @p start parameter describes where fill ends and possible
         * outline starts. Initial value is @cpp 0.5f @ce, larger values will
         * make the vector art look thinner, smaller will make it look thicker.
         *
         * The @p end parameter describes where outline ends. If set to value
         * larger than @p start the outline is not drawn. Initial value is
         * @cpp 1.0f @ce.
         *
         * @see @ref setOutlineColor()
         */
        DistanceFieldVector<dimensions>& setOutlineRange(Float start, Float end);

        /**
         * @brief Set smoothness radius
         * @return Reference to self (for method chaining)
         *
         * Larger values will make edges look less aliased (but blurry),
         * smaller values will make them look more crisp (but possibly
         * aliased). Initial value is @cpp 0.04f @ce.
         */
        DistanceFieldVector<dimensions>& setSmoothness(Float value);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Overloads to remove WTF-factor from method chaining order */
        DistanceFieldVector<dimensions>& bindVectorTexture(GL::Texture2D& texture) {
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
            _colorUniform{2},
            _outlineColorUniform{3},
            _outlineRangeUniform{4},
            _smoothnessUniform{5};
};

/** @brief Two-dimensional distance field vector shader */
typedef DistanceFieldVector<2> DistanceFieldVector2D;

/** @brief Three-dimensional distance field vector shader */
typedef DistanceFieldVector<3> DistanceFieldVector3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{DistanceFieldVector,DistanceFieldVector::Flag} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, DistanceFieldVector<dimensions>::Flag value);

/** @debugoperatorclassenum{DistanceFieldVector,DistanceFieldVector::Flags} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, DistanceFieldVector<dimensions>::Flags value);
#else
namespace Implementation {
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, DistanceFieldVectorFlag value);
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, DistanceFieldVectorFlags value);
    CORRADE_ENUMSET_OPERATORS(DistanceFieldVectorFlags)
}
#endif

}}

#endif
