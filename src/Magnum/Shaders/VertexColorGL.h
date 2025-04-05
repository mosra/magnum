#ifndef Magnum_Shaders_VertexColorGL_h
#define Magnum_Shaders_VertexColorGL_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>

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

#ifdef MAGNUM_TARGET_GL
/** @file
 * @brief Class @ref Magnum::Shaders::VertexColorGL, typedef @ref Magnum::Shaders::VertexColorGL2D, @ref Magnum::Shaders::VertexColorGL3D
 * @m_since_latest
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <Corrade/Utility/Move.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/glShaderWrapper.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class VertexColorGLFlag: UnsignedByte {
        #ifndef MAGNUM_TARGET_GLES2
        UniformBuffers = 1 << 0,
        #ifndef MAGNUM_TARGET_WEBGL
        ShaderStorageBuffers = UniformBuffers|(1 << 2),
        #endif
        MultiDraw = UniformBuffers|(1 << 1)
        #endif
    };
    typedef Containers::EnumSet<VertexColorGLFlag> VertexColorGLFlags;
    CORRADE_ENUMSET_OPERATORS(VertexColorGLFlags)
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
enabled or to @ref PhongGL with zero lights and @ref PhongGL::Flag::VertexColor
enabled. However, the implementation is much simpler and thus likely also
faster.

Alpha / transparency is supported by the shader implicitly, but to have it
working on the framebuffer, you need to enable
@ref GL::Renderer::Feature::Blending and set up the blending function. See
@ref GL::Renderer::setBlendFunction() for details.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@section Shaders-VertexColorGL-example Example usage

Common mesh setup. The shader accepts either three- or four-component color
attribute, use either @ref Color3 or @ref Color4 to specify which one you use.

@snippet Shaders-gl.cpp VertexColorGL-usage1

Common rendering setup:

@snippet Shaders-gl.cpp VertexColorGL-usage2

@section Shaders-VertexColorGL-ubo Uniform buffers

See @ref shaders-usage-ubo for a high-level overview that applies to all
shaders. In this particular case, because the shader doesn't need a separate
projection and transformation matrix, a combined one is supplied via a
@ref TransformationProjectionUniform2D / @ref TransformationProjectionUniform3D
buffer bound with @ref bindTransformationProjectionBuffer(). This is also the
only buffer supplied, as there are no other draw parameters. A uniform buffer
setup equivalent to the above would look like this:

@snippet Shaders-gl.cpp VectorGL-ubo

For a multidraw workflow enable @ref Flag::MultiDraw and supply desired draw
count via @ref Configuration::setDrawCount(). The usage is similar for all
shaders, see @ref shaders-usage-multidraw for an example.

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
        class Configuration;
        class CompileState;

        /**
         * @brief Vertex position
         *
         * @ref shaders-generic "Generic attribute",
         * @relativeref{Magnum,Vector2} in 2D, @relativeref{Magnum,Vector3} in
         * 3D.
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
         * @see @ref Flags, @ref flags(), @ref Configuration::setFlags()
         */
        enum class Flag: UnsignedByte {
            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Use uniform buffers. Expects that uniform data are supplied via
             * @ref bindTransformationProjectionBuffer() instead of direct
             * uniform setters.
             * @see @ref Flag::ShaderStorageBuffers
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             * @m_since_latest
             */
            UniformBuffers = 1 << 0,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Use shader storage buffers. Superset of functionality provided
             * by @ref Flag::UniformBuffers, compared to it doesn't have any
             * size limits on @relativeref{Configuration,setDrawCount()} in
             * exchange for potentially more costly access and narrower
             * platform support.
             * @requires_gl43 Extension @gl_extension{ARB,shader_storage_buffer_object}
             * @requires_gles31 Shader storage buffers are not available in
             *      OpenGL ES 3.0 and older.
             * @requires_gles Shader storage buffers are not available in
             *      WebGL.
             * @m_since_latest
             */
            ShaderStorageBuffers = UniformBuffers|(1 << 2),
            #endif

            /**
             * Enable multidraw functionality. Implies @ref Flag::UniformBuffers
             * and adds the value from @ref setDrawOffset() with the
             * @glsl gl_DrawID @ce builtin, which makes draws submitted via
             * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
             * and related APIs pick up per-draw parameters directly, without
             * having to rebind the uniform buffers or specify
             * @ref setDrawOffset() before each draw. In a non-multidraw
             * scenario, @glsl gl_DrawID @ce is @cpp 0 @ce, which means a
             * shader with this flag enabled can be used for regular draws as
             * well.
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
         * @see @ref flags(), @ref Configuration::setFlags()
         */
        typedef Containers::EnumSet<Flag> Flags;
        #else
        /* Done this way to be prepared for possible future diversion of 2D
           and 3D flags (e.g. introducing 3D-specific features) */
        typedef Implementation::VertexColorGLFlag Flag;
        typedef Implementation::VertexColorGLFlags Flags;
        #endif

        /**
         * @brief Compile asynchronously
         * @m_since_latest
         *
         * Compared to @ref VertexColorGL(const Configuration&) can perform an
         * asynchronous compilation and linking. See @ref shaders-async for
         * more information.
         * @see @ref VertexColorGL(CompileState&&)
         */
        /* Compared to the non-templated shaders like PhongGL or
           MeshVisualizerGL2D using a forward declaration is fine here. Huh. */
        static CompileState compile(const Configuration& configuration = Configuration{});

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Compile asynchronously
         * @m_deprecated_since_latest Use @ref compile(const Configuration&)
         *      instead.
         */
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Compile for a multi-draw scenario asynchronously
         * @m_deprecated_since_latest Use @ref compile(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags, UnsignedInt drawCount);
        #endif
        #endif

        /**
         * @brief Constructor
         * @m_since_latest
         */
        /* Compared to the non-templated shaders like PhongGL or
           MeshVisualizerGL2D using a forward declaration is fine here. Huh. */
        explicit VertexColorGL(const Configuration& configuration = Configuration{});

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since_latest Use @ref VertexColorGL(const Configuration&)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use VertexColorGL(const Configuration& instead") VertexColorGL(Flags flags);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Construct for a multi-draw scenario
         * @m_deprecated_since_latest Use @ref VertexColorGL(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        explicit CORRADE_DEPRECATED("use VertexColorGL(const Configuration& instead") VertexColorGL(Flags flags, UnsignedInt drawCount);
        #endif
        #endif

        /**
         * @brief Finalize an asynchronous compilation
         * @m_since_latest
         *
         * Takes an asynchronous compilation state returned by @ref compile()
         * and forms a ready-to-use shader object. See @ref shaders-async for
         * more information.
         */
        explicit VertexColorGL(CompileState&& state);

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
         * @brief Flags
         *
         * @see @ref Configuration::setFlags()
         */
        Flags flags() const { return _flags; }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Draw count
         * @m_since_latest
         *
         * Statically defined size of each of the
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D uniform buffers bound with
         * @ref bindTransformationProjectionBuffer(). Has use only if
         * @ref Flag::UniformBuffers is set and @ref Flag::ShaderStorageBuffers
         * is not set.
         * @see @ref Configuration::setDrawCount()
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
         * @name Uniform / shader storage buffer binding and related uniform setters
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
         * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
         * pick up its own per-draw parameters.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        VertexColorGL<dimensions>& setDrawOffset(UnsignedInt offset);

        /**
         * @brief Bind a transformation and projection uniform / shader storage buffer
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

        MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION(VertexColorGL<dimensions>)

    private:
        /* Creates the GL shader program object but does nothing else.
           Internal, used by compile(). */
        explicit VertexColorGL(NoInitT);

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
@brief Configuration
@m_since_latest

@see @ref VertexColorGL(const Configuration&),
    @ref compile(const Configuration&)
*/
template<UnsignedInt dimensions> class VertexColorGL<dimensions>::Configuration {
    public:
        explicit Configuration() = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set flags
         *
         * No flags are set by default.
         * @see @ref VertexColorGL::flags()
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Draw count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt drawCount() const { return _drawCount; }

        /**
         * @brief Set draw count
         *
         * If @ref Flag::UniformBuffers is set, describes size of a
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D buffer bound with
         * @ref bindTransformationProjectionBuffer(). Uniform buffers have a
         * statically defined size and
         * @cpp count*sizeof(TransformationProjectionUniform2D) @ce /,
         * @cpp count*sizeof(TransformationProjectionUniform3D) @ce has to be
         * within @ref GL::AbstractShaderProgram::maxUniformBlockSize(), if
         * @ref Flag::ShaderStorageBuffers is set as well, the buffers are
         * unbounded and @p count is ignored. The draw offset is set via
         * @ref setDrawOffset(). Default value is @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref VertexColorGL::drawCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        Configuration& setDrawCount(UnsignedInt count) {
            _drawCount = count;
            return *this;
        }
        #endif

    private:
        Flags _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _drawCount = 1;
        #endif
};

/**
@brief Asynchronous compilation state
@m_since_latest

Returned by @ref compile(). See @ref shaders-async for more information.
*/
template<UnsignedInt dimensions> class VertexColorGL<dimensions>::CompileState: public VertexColorGL<dimensions> {
    /* Everything deliberately private except for the inheritance */
    friend class VertexColorGL;

    explicit CompileState(NoCreateT): VertexColorGL{NoCreate}, _vert{NoCreate}, _frag{NoCreate} {}

    explicit CompileState(VertexColorGL<dimensions>&& shader, GL::Shader&& vert, GL::Shader&& frag
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , GL::Version version
        #endif
    ): VertexColorGL<dimensions>{Utility::move(shader)}, _vert{Utility::move(vert)}, _frag{Utility::move(frag)}
        #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        , _version{version}
        #endif
        {}

    Implementation::GLShaderWrapper _vert, _frag;
    #if !defined(MAGNUM_TARGET_GLES) || (!defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
    GL::Version _version;
    #endif
};

/**
@brief 2D vertex color OpenGL shader
@m_since_latest

@note This typedef is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
typedef VertexColorGL<2> VertexColorGL2D;

/**
@brief 3D vertex color OpenGL shader
@m_since_latest

@note This typedef is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
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
}
#endif

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
