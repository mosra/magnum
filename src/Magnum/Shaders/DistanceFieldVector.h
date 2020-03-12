#ifndef Magnum_Shaders_DistanceFieldVector_h
#define Magnum_Shaders_DistanceFieldVector_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Class @ref Magnum::Shaders::DistanceFieldVector, typedef @ref Magnum::Shaders::DistanceFieldVector2D, @ref Magnum::Shaders::DistanceFieldVector3D
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/Shaders/AbstractVector.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

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
        explicit DistanceFieldVector();

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
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Initial value is an identity matrix.
         */
        DistanceFieldVector& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @brief Set fill color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce.
         * @see @ref setOutlineColor()
         */
        DistanceFieldVector& setColor(const Color4& color);

        /**
         * @brief Set outline color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x00000000_rgbaf @ce and the outline is not
         * drawn --- see @ref setOutlineRange() for more information.
         * @see @ref setOutlineRange(), @ref setColor()
         */
        DistanceFieldVector& setOutlineColor(const Color4& color);

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
        DistanceFieldVector& setOutlineRange(Float start, Float end);

        /**
         * @brief Set smoothness radius
         * @return Reference to self (for method chaining)
         *
         * Larger values will make edges look less aliased (but blurry),
         * smaller values will make them look more crisp (but possibly
         * aliased). Initial value is @cpp 0.04f @ce.
         */
        DistanceFieldVector& setSmoothness(Float value);

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Overloads to remove WTF-factor from method chaining order */
        DistanceFieldVector<dimensions>& bindVectorTexture(GL::Texture2D& texture) {
            AbstractVector<dimensions>::bindVectorTexture(texture);
            return *this;
        }
        #ifdef MAGNUM_BUILD_DEPRECATED
        CORRADE_DEPRECATED("use bindVectorTexture() instead") DistanceFieldVector<dimensions>& setVectorTexture(GL::Texture2D& texture) {
            return bindVectorTexture(texture);
        }
        #endif
        #endif

    private:
        /* Prevent accidentally calling irrelevant functions */
        #ifndef MAGNUM_TARGET_GLES
        using GL::AbstractShaderProgram::drawTransformFeedback;
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        using GL::AbstractShaderProgram::dispatchCompute;
        #endif

        Int _transformationProjectionMatrixUniform{0},
            _colorUniform{1},
            _outlineColorUniform{2},
            _outlineRangeUniform{3},
            _smoothnessUniform{4};
};

/** @brief Two-dimensional distance field vector shader */
typedef DistanceFieldVector<2> DistanceFieldVector2D;

/** @brief Three-dimensional distance field vector shader */
typedef DistanceFieldVector<3> DistanceFieldVector3D;

}}

#endif
