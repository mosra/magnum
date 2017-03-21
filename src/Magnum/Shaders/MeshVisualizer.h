#ifndef Magnum_Shaders_MeshVisualizer_h
#define Magnum_Shaders_MeshVisualizer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix4.h"

#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

/**
@brief Mesh visualization shader

Uses geometry shader to visualize wireframe of 3D meshes. You need to provide
@ref Position attribute in your triangle mesh and call at least
@ref setTransformationProjectionMatrix() to be able to render.

@image html shaders-meshvisualizer.png
@image latex shaders-meshvisualizer.png

## Wireframe visualization

Wireframe visualization is done by enabling @ref Flag::Wireframe. It is done
either using geometry shaders or with help of additional vertex information.

If you have geometry shaders available, you don't need to do anything else.

@requires_gl32 Extension @extension{ARB,geometry_shader4} for wireframe
    rendering using geometry shaders.
@requires_es_extension Extension @extension{EXT,geometry_shader} for
    wireframe rendering using geometry shaders.

If you don't have geometry shaders, you need to set @ref Flag::NoGeometryShader
(it's enabled by default in OpenGL ES 2.0) and use only **non-indexed** triangle
meshes (see @ref MeshTools::duplicate() for possible solution). Additionaly, if
you have OpenGL < 3.1 or OpenGL ES 2.0, you need to provide also
@ref VertexIndex attribute.

@requires_gles30 Extension @extension{OES,standard_derivatives} for
    wireframe rendering without geometry shaders.

If using geometry shaders on OpenGL ES, @extension{NV,shader_noperspective_interpolation}
is optionally used for improving line appearance.

## Example usage

### Wireframe visualization with geometry shader (desktop GL)

Common mesh setup:
@code
struct Vertex {
    Vector3 position;
};
Vertex data[] = { ... };

Buffer vertices;
vertices.setData(data, BufferUsage::StaticDraw);

Mesh mesh;
mesh.addVertexBuffer(vertices, 0, Shaders::MeshVisualizer::Position{});
@endcode

Common rendering setup:
@code
Matrix4 transformationMatrix = Matrix4::translation(Vector3::zAxis(-5.0f));
Matrix4 projectionMatrix = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

Shaders::MeshVisualizer shader{Shaders::MeshVisualizer::Wireframe};
shader.setColor(Color3::fromHSV(216.0_degf, 0.85f, 1.0f))
    .setWireframeColor(Color3{0.95f})
    .setViewportSize(defaultFramebuffer.viewport().size())
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(shader);
@endcode

### Wireframe visualization without geometry shader on older hardware

You need to provide also the @ref VertexIndex attribute. Mesh setup *in
addition to the above*:
@code
constexpr std::size_t vertexCount = std::extent<decltype(data)>::value;
Float vertexIndex[vertexCount];
std::iota(vertexIndex, vertexIndex + vertexCount, 0.0f);

Buffer vertexIndices;
vertexIndices.setData(vertexIndex, BufferUsage::StaticDraw);

mesh.addVertexBuffer(vertexIndices, 0, Shaders::MeshVisualizer::VertexIndex{});
@endcode

Rendering setup:
@code
Matrix4 transformationMatrix, projectionMatrix;

Shaders::MeshVisualizer shader{Shaders::MeshVisualizer::Wireframe|
                               Shaders::MeshVisualizer::NoGeometryShader};
shader.setColor(Color3::fromHSV(216.0_degf, 0.85f, 1.0f))
    .setWireframeColor(Color3{0.95f})
    .setTransformationProjectionMatrix(projectionMatrix*transformationMatrix);

mesh.draw(shader);
@endcode

### Wireframe visualization of indexed meshes without geometry shader

The vertices must be converted to non-indexed array. Mesh setup:
@code
std::vector<UnsignedInt> indices{ ... };
std::vector<Vector3> indexedPositions{ ... };

// De-indexing the position array
Buffer vertices;
vertices.setData(MeshTools::duplicate(indices, indexedPositions), BufferUsage::StaticDraw);

Mesh mesh;
mesh.addVertexBuffer(vertices, 0, Shaders::MeshVisualizer::Position{});
@endcode

Rendering setup the same as above.

@see @ref shaders
@todo Understand and add support wireframe width/smoothness without GS
*/
class MAGNUM_SHADERS_EXPORT MeshVisualizer: public AbstractShaderProgram {
    public:
        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute", @ref Vector3.
         */
        typedef Attribute<0, Vector3> Position;

        /**
         * @brief Vertex index
         *
         * @ref Magnum::Float "Float", used only in OpenGL < 3.1 and OpenGL ES
         * 2.0 if @ref Flag::Wireframe is enabled. This attribute (modulo 3)
         * specifies index of given vertex in triangle, i.e. `0` for first, `1`
         * for second, `2` for third. In OpenGL 3.1, OpenGL ES 3.0 and newer
         * this value is provided by the shader itself, so the attribute is not
         * needed.
         */
        typedef Attribute<3, Float> VertexIndex;

        /**
         * @brief Flag
         *
         * @see @ref Flags, @ref MeshVisualizer()
         */
        enum class Flag: UnsignedByte {
            /**
             * Visualize wireframe. On OpenGL ES 2.0 this also enables
             * @ref Flag::NoGeometryShader.
             */
            #ifndef MAGNUM_TARGET_GLES2
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
        explicit MeshVisualizer(Flags flags = {});

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
        explicit MeshVisualizer(NoCreateT) noexcept: AbstractShaderProgram{NoCreate} {}

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         */
        MeshVisualizer& setTransformationProjectionMatrix(const Matrix4& matrix) {
            setUniform(_transformationProjectionMatrixUniform, matrix);
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
            if(_flags & Flag::Wireframe && !(_flags & Flag::NoGeometryShader))
                setUniform(_viewportSizeUniform, size);
            return *this;
        }

        /**
         * @brief Set base object color
         * @return Reference to self (for method chaining)
         *
         * Initial value is fully opaque white.
         */
        MeshVisualizer& setColor(const Color4& color) {
            setUniform(_colorUniform, color);
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
            if(_flags & Flag::Wireframe) setUniform(_wireframeColorUniform, color);
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
            if(_flags & Flag::Wireframe) setUniform(_wireframeWidthUniform, width);
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
        Flags _flags;
        Int _transformationProjectionMatrixUniform{0},
            _viewportSizeUniform{1},
            _colorUniform{2},
            _wireframeColorUniform{3},
            _wireframeWidthUniform{4},
            _smoothnessUniform{5};
};

CORRADE_ENUMSET_OPERATORS(MeshVisualizer::Flags)

inline MeshVisualizer& MeshVisualizer::setSmoothness(Float smoothness) {
    if(_flags & Flag::Wireframe)
        setUniform(_smoothnessUniform, smoothness);
    return *this;
}

}}

#endif
