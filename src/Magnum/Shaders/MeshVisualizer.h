#ifndef Magnum_Shaders_MeshVisualizer_h
#define Magnum_Shaders_MeshVisualizer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
 * @brief Class @ref Magnum::Shaders::MeshVisualizer
 */

#include "Magnum/Math/Matrix4.h"
#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Color.h"

#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Mesh visualization shader

Uses geometry shader to visualize wireframe. You need to provide @ref Position
attribute in your triangle mesh and call at least @ref setTransformationProjectionMatrix()
to be able to render.

## Wireframe visualization

Wireframe visualization is done by enabling @ref Flag::Wireframe. It is done
either using geometry shaders or with help of additional vertex information.

If you have geometry shaders available, you don't need to do anything else.

@requires_gl32 Extension @extension{ARB,geometry_shader4} for wireframe
    rendering using geometry shaders.

If you don't have geometry shaders, you need to set @ref Flag::NoGeometryShader
(it's enabled by default in OpenGL ES) and use only **non-indexed** triangle
meshes (see @ref MeshTools::duplicate() for possible solution). Additionaly, if
you have OpenGL < 3.1 or OpenGL ES 2.0, you need to provide also
@ref VertexIndex attribute.

@requires_es_extension Extension @extension{OES,standard_derivatives} for
    wireframe rendering.

@todo Understand and add support wireframe width/smoothness without GS
*/
class MAGNUM_SHADERS_EXPORT MeshVisualizer: public AbstractShaderProgram {
    public:
        typedef Attribute<0, Vector3> Position; /**< @brief Vertex position */

        /**
         * @brief Vertex index
         *
         * Used only in OpenGL < 3.1 and OpenGL ES 2.0 if @ref Flag::Wireframe
         * is enabled. This attribute specifies index of given vertex in
         * triangle, i.e. `0` for first, `1` for second, `2` for third. In
         * OpenGL 3.1, OpenGL ES 3.0 and newer this value is provided by the
         * shader itself, so the attribute is not needed.
         */
        typedef Attribute<3, Float> VertexIndex;

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref MeshVisualizer()
         */
        enum class Flag: UnsignedByte {
            /**
             * Visualize wireframe. On OpenGL ES this also enables
             * @ref Flag::NoGeometryShader.
             */
            #ifndef MAGNUM_TARGET_GLES
            Wireframe = 1 << 0,
            #else
            Wireframe = (1 << 0) | (1 << 1),
            #endif

            /**
             * Don't use geometry shader for wireframe visualization. If
             * enabled, you might need to provide also @ref VertexIndex
             * attribute in the mesh. In OpenGL ES enabled alongside
             * @ref Flag::Wireframe.
             */
            NoGeometryShader = 1 << 1
        };

        /** @brief Flags */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags     Flags
         */
        explicit MeshVisualizer(Flags flags = Flags());

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         */
        MeshVisualizer& setTransformationProjectionMatrix(const Matrix4& matrix) {
            setUniform(transformationProjectionMatrixUniform, matrix);
            return *this;
        }

        /**
         * @brief Set viewport size
         * @return Reference to self (for method chaining)
         *
         * Has effect only if @ref Flag::Wireframe is enabled and geometry
         * shaders are used.
         */
        MeshVisualizer& setViewportSize(const Vector2& size) {
            if(flags & Flag::Wireframe && !(flags & Flag::NoGeometryShader))
                setUniform(viewportSizeUniform, size);
            return *this;
        }

        /**
         * @brief Set base object color
         * @return Reference to self (for method chaining)
         *
         * Initial value is fully opaque white.
         */
        MeshVisualizer& setColor(const Color4& color) {
            setUniform(colorUniform, color);
            return *this;
        }

        /**
         * @brief Set wireframe color
         * @return Reference to self (for method chaining)
         *
         * Initial value is fully opaque black. Has effect only if
         * @ref Flag::Wireframe is enabled.
         */
        MeshVisualizer& setWireframeColor(const Color4& color) {
            if(flags & Flag::Wireframe) setUniform(wireframeColorUniform, color);
            return *this;
        }

        /**
         * @brief Set wireframe width
         * @return Reference to self (for method chaining)
         *
         * Initial value is `1.0f`. Has effect only if @ref Flag::Wireframe is
         * enabled.
         */
        MeshVisualizer& setWireframeWidth(Float width) {
            if(flags & Flag::Wireframe) setUniform(wireframeWidthUniform, width);
            return *this;
        }

        /**
         * @brief Set line smoothness
         * @return Reference to self (for method chaining)
         *
         * Initial value is `2.0f`. Has effect only if @ref Flag::Wireframe is
         * enabled.
         */
        MeshVisualizer& setSmoothness(Float smoothness);

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

inline MeshVisualizer& MeshVisualizer::setSmoothness(Float smoothness) {
    if(flags & Flag::Wireframe)
        setUniform(smoothnessUniform, smoothness);
    return *this;
}

}}

#endif
