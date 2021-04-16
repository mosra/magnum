#ifndef Magnum_Shaders_VertexColorGL_h
#define Magnum_Shaders_VertexColorGL_h
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
 * @brief Class @ref Magnum::Shaders::VertexColorGL
 * @m_since_latest
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Vertex color OpenGL shader
@m_since_latest

Draws a vertex-colored mesh. You need to provide @ref Position and @ref Color3
/ @ref Color4 attributes in your triangle mesh. By default, the shader renders
the mesh in an identity transformation. Use
@ref setTransformationProjectionMatrix() to configure the shader.

@image html shaders-vertexcolor.png width=256px

This shader is equivalent to @ref FlatGL with @ref FlatGL::Flag::VertexColor
enabled; the 3D version of this shader is equivalent to @ref PhongGL with
@ref PhongGL::Flag::VertexColor enabled. In both cases this implementation is
much simpler and thus likely also faster.

Alpha / transparency is supported by the shader implicitly, but to have it
working on the framebuffer, you need to enable
@ref GL::Renderer::Feature::Blending and set up the blending function. See
@ref GL::Renderer::setBlendFunction() for details.

@section Shaders-VertexColorGL-example Example usage

Common mesh setup. The shader accepts either three- or four-component color
attribute, use either @ref Color3 or @ref Color4 to specify which one you use.

@snippet MagnumShaders-gl.cpp VertexColorGL-usage1

Common rendering setup:

@snippet MagnumShaders-gl.cpp VertexColorGL-usage2

@see @ref shaders, @ref VertexColorGL2D, @ref VertexColorGL3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT VertexColorGL: public GL::AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @ref Magnum::Vector2 "Vector2" in 2D @ref Magnum::Vector3 "Vector3"
         * in 3D.
         */
        typedef typename GenericGL<dimensions>::Position Position;

        /**
         * @brief Three-component vertex color
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Color3. Use
         * either this or the @ref Color4 attribute.
         */
        typedef typename GenericGL<dimensions>::Color3 Color3;

        /**
         * @brief Four-component vertex color
         *
         * @ref shaders-generic "Generic attribute", @ref Magnum::Color4. Use
         * either this or the @ref Color3 attribute.
         */
        typedef typename GenericGL<dimensions>::Color4 Color4;

        enum: UnsignedInt {
            /**
             * Color shader output. @ref shaders-generic "Generic output",
             * present always. Expects three- or four-component floating-point
             * or normalized buffer attachment.
             */
            ColorOutput = GenericGL<dimensions>::ColorOutput
        };

        explicit VertexColorGL();

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
        explicit VertexColorGL(NoCreateT) noexcept: AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        VertexColorGL(const VertexColorGL<dimensions>&) = delete;

        /** @brief Move constructor */
        VertexColorGL(VertexColorGL<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        VertexColorGL<dimensions>& operator=(const VertexColorGL<dimensions>&) = delete;

        /** @brief Move assignment */
        VertexColorGL<dimensions>& operator=(VertexColorGL<dimensions>&&) noexcept = default;

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Default is an identity matrix.
         */
        VertexColorGL<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

    private:
        /* Prevent accidentally calling irrelevant functions */
        #ifndef MAGNUM_TARGET_GLES
        using GL::AbstractShaderProgram::drawTransformFeedback;
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        using GL::AbstractShaderProgram::dispatchCompute;
        #endif

        Int _transformationProjectionMatrixUniform{0};
};

/**
@brief 2D vertex color OpenGL shader
@m_since_latest
*/
typedef VertexColorGL<2> VertexColorGL2D;

/**
@brief 3D vertex color OpenGL shader
@m_since_latest
*/
typedef VertexColorGL<3> VertexColorGL3D;

}}

#endif
