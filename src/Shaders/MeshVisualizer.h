#ifndef Magnum_Shaders_MeshVisualizer_h
#define Magnum_Shaders_MeshVisualizer_h
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
 * @brief Class Magnum::Shaders::MeshVisualizer
 */

#include "Math/Matrix4.h"
#include "AbstractShaderProgram.h"
#include "Color.h"

#include "Shaders/magnumShadersVisibility.h"

namespace Magnum { namespace Shaders {

/**
@brief %Mesh visualization shader

Uses geometry shader to visualize wireframe. You need to call at least
setTransformationProjectionMatrix() (and setViewportSize(), if
@ref Flag "Flag::Wireframe" is enabled) to be able to render.
@requires_gl32 %Extension @extension{ARB,geometry_shader4} if
    @ref Magnum::Shaders::MeshVisualizer::Flag "Flag::Wireframe" is enabled.
*/
class MAGNUM_SHADERS_EXPORT MeshVisualizer: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position; /**< @brief Vertex position */

        /**
         * @brief %Flag
         *
         * @see Flags, MeshVisualizer()
         */
        enum class Flag: UnsignedByte {
            Wireframe = 1 << 0      /**< Visualize wireframe */
        };

        /** @brief %Flags */
        typedef Corrade::Containers::EnumSet<Flag, UnsignedByte> Flags;

        /**
         * @brief Constructor
         * @param flags     %Flags
         */
        explicit MeshVisualizer(Flags flags);

        /**
         * @brief Set transformation and projection matrix
         * @return Pointer to self (for method chaining)
         */
        inline MeshVisualizer* setTransformationProjectionMatrix(const Matrix4& matrix) {
            setUniform(transformationProjectionMatrixUniform, matrix);
            return this;
        }

        /**
         * @brief Set viewport size
         * @return Pointer to self (for method chaining)
         *
         * Has effect only if @ref Flag "Flag::Wireframe" is enabled.
         */
        inline MeshVisualizer* setViewportSize(const Vector2& size) {
            setUniform(viewportSizeUniform, size);
            return this;
        }

        /**
         * @brief Set base object color
         * @return Pointer to self (for method chaining)
         *
         * Initial value is fully opaque white.
         */
        inline MeshVisualizer* setColor(const Color4<>& color) {
            setUniform(colorUniform, color);
            return this;
        }

        /**
         * @brief Set wireframe color
         * @return Pointer to self (for method chaining)
         *
         * Initial value is fully opaque black. Has effect only if
         * @ref Flag "Flag::Wireframe" is enabled.
         */
        inline MeshVisualizer* setWireframeColor(const Color4<>& color) {
            if(flags & Flag::Wireframe) setUniform(wireframeColorUniform, color);
            return this;
        }

        /**
         * @brief Set wireframe width
         * @return Pointer to self (for method chaining)
         *
         * Initial value is `1.0f`. Has effect only if
         * @ref Flag "Flag::Wireframe" is enabled.
         */
        inline MeshVisualizer* setWireframeWidth(Float width) {
            if(flags & Flag::Wireframe) setUniform(wireframeWidthUniform, width);
            return this;
        }

        /**
         * @brief Set line smoothness
         * @return Pointer to self (for method chaining)
         *
         * Initial value is `2.0f`. Has effect only if
         * @ref Flag "Flag::Wireframe" is enabled.
         */
        MeshVisualizer* setSmoothness(Float smoothness);

    private:
        Flags flags;
        Int transformationProjectionMatrixUniform,
            viewportSizeUniform,
            colorUniform,
            wireframeColorUniform,
            wireframeWidthUniform,
            smoothnessUniform;
};

CORRADE_ENUMSET_OPERATORS(MeshVisualizer::Flags)

inline MeshVisualizer* MeshVisualizer::setSmoothness(Float smoothness) {
    if(flags & Flag::Wireframe)
        setUniform(smoothnessUniform, smoothness);
    return this;
}

}}

#endif
