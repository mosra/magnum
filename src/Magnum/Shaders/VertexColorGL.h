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

namespace Implementation {
    enum class VertexColorGLFlag: UnsignedByte {
        #ifndef MAGNUM_TARGET_GLES2
        UniformBuffers = 1 << 0,
        MultiDraw = UniformBuffers|(1 << 1)
        #endif
    };
    typedef Containers::EnumSet<VertexColorGLFlag> VertexColorGLFlags;
}

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

@section Shaders-VertexColorGL-ubo Uniform buffers

See @ref shaders-usage-ubo for a high-level overview that applies to all
shaders. In this particular case, because the shader doesn't need a separate
projection and transformation matrix, a combined one is supplied via a
@ref TransformationProjectionUniform2D / @ref TransformationProjectionUniform3D
buffer. This is also the only buffer supplied, as there are no other draw
parameters. A uniform buffer setup equivalent to the above would look like
this:

@snippet MagnumShaders-gl.cpp VectorGL-ubo

For a multidraw workflow enable @ref Flag::MultiDraw and supply desired draw
count in the @ref VertexColorGL(Flags, UnsignedInt) constructor. The usage is
similar for all shaders, see @ref shaders-usage-multidraw for an example.

@requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object} for uniform
    buffers.
@requires_gl46 Extension @gl_extension{ARB,shader_draw_parameters} for
    multidraw.
@requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
@requires_webgl20 Uniform buffers are not available in WebGL 1.0.
@requires_es_extension Extension @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
    (unlisted) for multidraw.
@requires_webgl_extension Extension @webgl_extension{ANGLE,multi_draw} for
    multidraw.

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

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Flag
         * @m_since{2020,06}
         *
         * @see @ref Flags, @ref flags()
         */
        enum class Flag: UnsignedByte {
            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Use uniform buffers. Expects that uniform data are supplied via
             * @ref bindTransformationProjectionBuffer() instead of direct
             * uniform setters.
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             * @m_since_latest
             */
            UniformBuffers = 1 << 0,

            /**
             * Enable multidraw functionality. Implies @ref Flag::UniformBuffers
             * and adds the value from @ref setDrawOffset() with the
             * @glsl gl_DrawID @ce builtin, which makes draws submitted via
             * @ref GL::AbstractShaderProgram::draw(Containers::ArrayView<const Containers::Reference<MeshView>>)
             * pick up per-draw parameters directly, without having to rebind
             * the uniform buffers or specify @ref setDrawOffset() before each
             * draw. In a non-multidraw scenario, @glsl gl_DrawID @ce is
             * @cpp 0 @ce, which means a shader with this flag enabled can be
             * used for regular draws as well.
             * @requires_gl46 Extension @gl_extension{ARB,uniform_buffer_object}
             *      and @gl_extension{ARB,shader_draw_parameters}
             * @requires_es_extension OpenGL ES 3.0 and extension
             *      @m_class{m-doc-external} [ANGLE_multi_draw](https://chromium.googlesource.com/angle/angle/+/master/extensions/ANGLE_multi_draw.txt)
             *      (unlisted). While the extension alone needs only OpenGL ES
             *      2.0, the shader implementation relies on uniform buffers,
             *      which require OpenGL ES 3.0.
             * @requires_webgl_extension WebGL 2.0 and extension
             *      @webgl_extension{ANGLE,multi_draw}. While the extension
             *      alone needs only WebGL 1.0, the shader implementation
             *      relies on uniform buffers, which require WebGL 2.0.
             * @m_since_latest
             */
            MultiDraw = UniformBuffers|(1 << 1)
            #endif
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
        typedef Implementation::VertexColorGLFlag Flag;
        typedef Implementation::VertexColorGLFlags Flags;
        #endif

        /**
         * @brief Constructor
         * @param flags     Flags
         *
         * While this function is meant mainly for the classic uniform
         * scenario (without @ref Flag::UniformBuffers set), it's equivalent to
         * @ref VertexColorGL(Flags, UnsignedInt) with @p drawCount set to
         * @cpp 1 @ce.
         */
        explicit VertexColorGL(Flags flags = {});

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Construct for a multi-draw scenario
         * @param flags         Flags
         * @param drawCount     Size of a @ref TransformationProjectionUniform2D
         *      / @ref TransformationProjectionUniform3D buffer bound with
         *      @ref bindTransformationProjectionBuffer()
         *
         * If @p flags contains @ref Flag::UniformBuffers, @p drawCount
         * describes the uniform buffer sizes as these are required to have a
         * statically defined size. The draw offset is then set via
         * @ref setDrawOffset().
         *
         * If @p flags don't contain @ref Flag::UniformBuffers, @p drawCount is
         * ignored and the constructor behaves the same as
         * @ref VertexColorGL(Flags).
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        /** @todo this constructor will eventually need to have also joint
            count, per-vertex weight count, view count for multiview and clip
            plane count ... and putting them in arbitrary order next to each
            other is too error-prone, so it needs some other solution
            (accepting pairs of parameter type and value like in GL context
            creation, e.g., which will probably need a new enum as reusing Flag
            for this might be too confusing) */
        explicit VertexColorGL(Flags flags, UnsignedInt drawCount);
        #endif

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

        /** @brief Flags */
        Flags flags() const { return _flags; }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Draw count
         * @m_since_latest
         *
         * Statically defined size of each of the
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D uniform buffers. Has use only
         * if @ref Flag::UniformBuffers is set.
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt drawCount() const { return _drawCount; }
        #endif

        /** @{
         * @name Uniform setters
         *
         * Used only if @ref Flag::UniformBuffers is not set.
         */

        /**
         * @brief Set transformation and projection matrix
         * @return Reference to self (for method chaining)
         *
         * Default is an identity matrix.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationProjectionUniform2D::transformationProjectionMatrix /
         * @ref TransformationProjectionUniform3D::transformationProjectionMatrix
         * and call @ref bindTransformationProjectionBuffer() instead.
         */
        VertexColorGL<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @}
         */

        #ifndef MAGNUM_TARGET_GLES2
        /** @{
         * @name Uniform buffer binding and related uniform setters
         *
         * Used if @ref Flag::UniformBuffers is set.
         */

        /**
         * @brief Set a draw offset
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Specifies which item in the @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D buffers bound with
         * @ref bindTransformationProjectionBuffer() should be used for current
         * draw. Expects that @ref Flag::UniformBuffers is set and @p offset is
         * less than @ref drawCount(). Initial value is @cpp 0 @ce, if
         * @ref drawCount() is @cpp 1 @ce, the function is a no-op as the
         * shader assumes draw offset to be always zero.
         *
         * If @ref Flag::MultiDraw is set, @glsl gl_DrawID @ce is added to this
         * value, which makes each draw submitted via
         * @ref GL::AbstractShaderProgram::draw(Containers::ArrayView<const Containers::Reference<MeshView>>)
         * pick up its own per-draw parameters.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        VertexColorGL<dimensions>& setDrawOffset(UnsignedInt offset);

        /**
         * @brief Set a transformation and projection uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        VertexColorGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        VertexColorGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @}
         */
        #endif

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        VertexColorGL<dimensions>& draw(GL::Mesh& mesh) {
            return static_cast<VertexColorGL<dimensions>&>(GL::AbstractShaderProgram::draw(mesh));
        }
        VertexColorGL<dimensions>& draw(GL::Mesh&& mesh) {
            return static_cast<VertexColorGL<dimensions>&>(GL::AbstractShaderProgram::draw(mesh));
        }
        VertexColorGL<dimensions>& draw(GL::MeshView& mesh) {
            return static_cast<VertexColorGL<dimensions>&>(GL::AbstractShaderProgram::draw(mesh));
        }
        VertexColorGL<dimensions>& draw(GL::MeshView&& mesh) {
            return static_cast<VertexColorGL<dimensions>&>(GL::AbstractShaderProgram::draw(mesh));
        }
        VertexColorGL<dimensions>& draw(Containers::ArrayView<const Containers::Reference<GL::MeshView>> meshes) {
            return static_cast<VertexColorGL<dimensions>&>(GL::AbstractShaderProgram::draw(meshes));
        }
        VertexColorGL<dimensions>& draw(std::initializer_list<Containers::Reference<GL::MeshView>> meshes) {
            return static_cast<VertexColorGL<dimensions>&>(GL::AbstractShaderProgram::draw(meshes));
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
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _drawCount{};
        #endif
        Int _transformationProjectionMatrixUniform{0};
        #ifndef MAGNUM_TARGET_GLES2
        /* Used instead of all other uniforms when Flag::UniformBuffers is set,
           so it can alias them */
        Int _drawOffsetUniform{0};
        #endif
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

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{VertexColorGL,VertexColorGL::Flag} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, VertexColorGL<dimensions>::Flag value);

/** @debugoperatorclassenum{VertexColorGL,VertexColorGL::Flags} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, VertexColorGL<dimensions>::Flags value);
#else
namespace Implementation {
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, VertexColorGLFlag value);
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, VertexColorGLFlags value);
    CORRADE_ENUMSET_OPERATORS(VertexColorGLFlags)
}
#endif

}}

#endif
