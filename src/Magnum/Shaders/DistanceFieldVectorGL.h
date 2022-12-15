#ifndef Magnum_Shaders_DistanceFieldVectorGL_h
#define Magnum_Shaders_DistanceFieldVectorGL_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>
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

/** @file
 * @brief Class @ref Magnum::Shaders::DistanceFieldVectorGL, typedef @ref Magnum::Shaders::DistanceFieldVectorGL2D, @ref Magnum::Shaders::DistanceFieldVectorGL3D
 * @m_since_latest
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/glShaderWrapper.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class DistanceFieldVectorGLFlag: UnsignedByte {
        TextureTransformation = 1 << 0,
        #ifndef MAGNUM_TARGET_GLES2
        UniformBuffers = 1 << 1,
        MultiDraw = UniformBuffers|(1 << 2)
        #endif
    };
    typedef Containers::EnumSet<DistanceFieldVectorGLFlag> DistanceFieldVectorGLFlags;
}

/**
@brief Distance field vector OpenGL shader
@m_since_latest

Renders vector graphics in a form of signed distance field. See
@ref TextureTools::DistanceField for more information and @ref VectorGL for a
simpler variant of this shader. Note that the final rendered outlook will
greatly depend on radius of input distance field and value passed to
@ref setSmoothness(). You need to provide @ref Position and
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

@section Shaders-DistanceFieldVectorGL-usage Example usage

Common mesh setup:

@snippet MagnumShaders-gl.cpp DistanceFieldVectorGL-usage1

Common rendering setup:

@snippet MagnumShaders-gl.cpp DistanceFieldVectorGL-usage2

@section Shaders-DistanceFieldVectorGL-ubo Uniform buffers

See @ref shaders-usage-ubo for a high-level overview that applies to all
shaders. In this particular case, because the shader doesn't need a separate
projection and transformation matrix, a combined one is supplied via a
@ref TransformationProjectionUniform2D / @ref TransformationProjectionUniform3D
buffer bound with @ref bindTransformationProjectionBuffer(). To maximize use of
the limited uniform buffer memory, materials are supplied separately in a
@ref DistanceFieldVectorMaterialUniform buffer bound with
@ref bindMaterialBuffer() and then referenced via
@relativeref{DistanceFieldVectorDrawUniform,materialId} from a
@ref DistanceFieldVectorDrawUniform bound with @ref bindDrawBuffer(); for
optional texture transformation a per-draw @ref TextureTransformationUniform
buffer bound with @ref bindTextureTransformationBuffer() can be supplied as
well. A uniform buffer setup equivalent to the above would look like this:

@snippet MagnumShaders-gl.cpp DistanceFieldVectorGL-ubo

For a multidraw workflow enable @ref Flag::MultiDraw, supply desired material
and draw count via @ref Configuration::setMaterialCount() and
@relativeref{Configuration,setDrawCount()} and specify material references and
texture offsets for every draw. Texture arrays aren't currently supported for
this shader. Besides that, the usage is similar for all shaders, see
@ref shaders-usage-multidraw for an example.

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

@see @ref shaders, @ref DistanceFieldVectorGL2D, @ref DistanceFieldVectorGL3D
@todo Use fragment shader derivations to have proper smoothness in perspective/
    large zoom levels, make it optional as it might have negative performance
    impact
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT DistanceFieldVectorGL: public GL::AbstractShaderProgram {
    public:
        class Configuration;
        class CompileState;

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
         * @see @ref Flags, @ref flags(), @ref Configuration::setFlags()
         */
        enum class Flag: UnsignedByte {
            /**
             * Enable texture coordinate transformation.
             * @see @ref setTextureMatrix()
             * @m_since{2020,06}
             */
            TextureTransformation = 1 << 0,

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Use uniform buffers. Expects that uniform data are supplied via
             * @ref bindTransformationProjectionBuffer(),
             * @ref bindDrawBuffer(), @ref bindTextureTransformationBuffer(),
             * and @ref bindMaterialBuffer() instead of direct uniform setters.
             * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
             * @requires_gles30 Uniform buffers are not available in OpenGL ES
             *      2.0.
             * @requires_webgl20 Uniform buffers are not available in WebGL
             *      1.0.
             * @m_since_latest
             */
            UniformBuffers = 1 << 1,

            /**
             * Enable multidraw functionality. Implies @ref Flag::UniformBuffers
             * and adds the value from @ref setDrawOffset() with the
             * @glsl gl_DrawID @ce builtin, which makes draws submitted via
             * @ref GL::AbstractShaderProgram::draw(const Containers::Iterable<MeshView>&)
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
            MultiDraw = UniformBuffers|(1 << 2)
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
        typedef Implementation::DistanceFieldVectorGLFlag Flag;
        typedef Implementation::DistanceFieldVectorGLFlags Flags;
        #endif

        /**
         * @brief Compile asynchronously
         * @m_since_latest
         *
         * Compared to @ref DistanceFieldVectorGL(const Configuration&) can
         * perform an asynchronous compilation and linking. See
         * @ref shaders-async for more information.
         * @see @ref DistanceFieldVectorGL(CompileState&&)
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        static CompileState compile(const Configuration& configuration = Configuration{});
        #else
        /* Configuration is forward-declared */
        static CompileState compile(const Configuration& configuration);
        static CompileState compile();
        #endif

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
        CORRADE_DEPRECATED("use compile(const Configuration& instead") static CompileState compile(Flags flags, UnsignedInt materialCount, UnsignedInt drawCount);
        #endif
        #endif

        /**
         * @brief Constructor
         * @m_since_latest
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit DistanceFieldVectorGL(const Configuration& configuration = Configuration{});
        #else
        /* Configuration is forward-declared */
        explicit DistanceFieldVectorGL(const Configuration& configuration);
        explicit DistanceFieldVectorGL();
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Constructor
         * @m_deprecated_since_latest Use @ref DistanceFieldVectorGL(const Configuration&)
         *      instead.
         */
        explicit CORRADE_DEPRECATED("use DistanceFieldVectorGL(const Configuration& instead") DistanceFieldVectorGL(Flags flags);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Construct for a multi-draw scenario
         * @m_deprecated_since_latest Use @ref DistanceFieldVectorGL(const Configuration&)
         *      instead.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        explicit CORRADE_DEPRECATED("use DistanceFieldVectorGL(const Configuration& instead") DistanceFieldVectorGL(Flags flags, UnsignedInt materialCount, UnsignedInt drawCount);
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
        explicit DistanceFieldVectorGL(CompileState&& state);

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
        explicit DistanceFieldVectorGL(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        DistanceFieldVectorGL(const DistanceFieldVectorGL<dimensions>&) = delete;

        /** @brief Move constructor */
        DistanceFieldVectorGL(DistanceFieldVectorGL<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        DistanceFieldVectorGL<dimensions>& operator=(const DistanceFieldVectorGL<dimensions>&) = delete;

        /** @brief Move assignment */
        DistanceFieldVectorGL<dimensions>& operator=(DistanceFieldVectorGL<dimensions>&&) noexcept = default;

        /**
         * @brief Flags
         * @m_since{2020,06}
         *
         * @see @ref Configuration::setFlags()
         */
        Flags flags() const { return _flags; }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Material count
         * @m_since_latest
         *
         * Statically defined size of the
         * @ref DistanceFieldVectorMaterialUniform uniform buffer bound with
         * @ref bindMaterialBuffer(). Has use only if @ref Flag::UniformBuffers
         * is set.
         * @see @ref Configuration::setMaterialCount()
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Draw count
         * @m_since_latest
         *
         * Statically defined size of each of the
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D,
         * @ref DistanceFieldVectorDrawUniform and
         * @ref TextureTransformationUniform uniform buffers bound with
         * @ref bindTransformationProjectionBuffer(), @ref bindDrawBuffer() and
         * @ref bindTextureTransformationBuffer(). Has use only if
         * @ref Flag::UniformBuffers is set.
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
         * Initial value is an identity matrix.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TransformationProjectionUniform2D::transformationProjectionMatrix /
         * @ref TransformationProjectionUniform3D::transformationProjectionMatrix
         * and call @ref bindTransformationProjectionBuffer() instead.
         */
        DistanceFieldVectorGL<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

        /**
         * @brief Set texture coordinate transformation matrix
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Expects that the shader was created with
         * @ref Flag::TextureTransformation enabled. Initial value is an
         * identity matrix.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref TextureTransformationUniform::rotationScaling and
         * @ref TextureTransformationUniform::offset and call
         * @ref bindTextureTransformationBuffer() instead.
         */
        DistanceFieldVectorGL<dimensions>& setTextureMatrix(const Matrix3& matrix);

        /**
         * @brief Set fill color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref DistanceFieldVectorMaterialUniform::color and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref setOutlineColor()
         */
        DistanceFieldVectorGL<dimensions>& setColor(const Color4& color);

        /**
         * @brief Set outline color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x00000000_rgbaf @ce and the outline is not
         * drawn --- see @ref setOutlineRange() for more information.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref DistanceFieldVectorMaterialUniform::outlineColor and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref setOutlineRange(), @ref setColor()
         */
        DistanceFieldVectorGL<dimensions>& setOutlineColor(const Color4& color);

        /**
         * @brief Set outline range
         * @return Reference to self (for method chaining)
         *
         * The @p start parameter describes where fill ends and possible
         * outline starts. Initial value is @cpp 0.5f @ce, larger values will
         * make the vector art look thinner, smaller will make it look thicker.
         *
         * The @p end parameter describes where outline ends. If set to a value
         * larger than @p start, the outline is not drawn. Initial value is
         * @cpp 1.0f @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref DistanceFieldVectorMaterialUniform::outlineStart and
         * @ref DistanceFieldVectorMaterialUniform::outlineEnd and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref setOutlineColor()
         */
        DistanceFieldVectorGL<dimensions>& setOutlineRange(Float start, Float end);

        /**
         * @brief Set smoothness radius
         * @return Reference to self (for method chaining)
         *
         * Larger values will make edges look less aliased (but blurry),
         * smaller values will make them look more crisp (but possibly
         * aliased). Initial value is @cpp 0.04f @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref DistanceFieldVectorMaterialUniform::smoothness and call
         * @ref bindMaterialBuffer() instead.
         */
        DistanceFieldVectorGL<dimensions>& setSmoothness(Float value);

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
         * @ref TransformationProjectionUniform3D,
         * @ref DistanceFieldVectorDrawUniform and
         * @ref TextureTransformationUniform buffers bound with
         * @ref bindTransformationProjectionBuffer(), @ref bindDrawBuffer() and
         * @ref bindTextureTransformationBuffer() should be used for current
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
        DistanceFieldVectorGL<dimensions>& setDrawOffset(UnsignedInt offset);

        /**
         * @brief Bind a transformation and projection uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref TransformationProjectionUniform2D /
         * @ref TransformationProjectionUniform3D. At the very least you need
         * to call also @ref bindDrawBuffer() and @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        DistanceFieldVectorGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        DistanceFieldVectorGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a draw uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref DistanceFieldVectorDrawUniform. At the very least you need to
         * call also @ref bindTransformationProjectionBuffer() and
         * @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        DistanceFieldVectorGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        DistanceFieldVectorGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a texture transformation uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that both @ref Flag::UniformBuffers and
         * @ref Flag::TextureTransformation is set. The buffer is expected to
         * contain @ref drawCount() instances of
         * @ref TextureTransformationUniform.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        DistanceFieldVectorGL<dimensions>& bindTextureTransformationBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        DistanceFieldVectorGL<dimensions>& bindTextureTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Bind a material uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref materialCount() instances of
         * @ref DistanceFieldVectorMaterialUniform. At the very least you need
         * to call also @ref bindTransformationProjectionBuffer() and
         * @ref bindDrawBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        DistanceFieldVectorGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        DistanceFieldVectorGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @}
         */
        #endif

        /** @{
         * @name Texture binding
         */

        /**
         * @brief Bind a vector texture
         * @return Reference to self (for method chaining)
         *
         * @see @ref DistanceFieldVectorGL::Flag::TextureTransformation,
         *      @ref VectorGL::Flag::TextureTransformation,
         *      @ref DistanceFieldVectorGL::setTextureMatrix(),
         *      @ref VectorGL::setTextureMatrix()
         */
        DistanceFieldVectorGL<dimensions>& bindVectorTexture(GL::Texture2D& texture);

        /**
         * @}
         */

        MAGNUM_GL_ABSTRACTSHADERPROGRAM_SUBCLASS_DRAW_IMPLEMENTATION(DistanceFieldVectorGL<dimensions>)

    private:
        /* Creates the GL shader program object but does nothing else.
           Internal, used by compile(). */
        explicit DistanceFieldVectorGL(NoInitT);

        Flags _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _materialCount{}, _drawCount{};
        #endif
        Int _transformationProjectionMatrixUniform{0},
            _textureMatrixUniform{1},
            _colorUniform{2},
            _outlineColorUniform{3},
            _outlineRangeUniform{4},
            _smoothnessUniform{5};
        #ifndef MAGNUM_TARGET_GLES2
        /* Used instead of all other uniforms when Flag::UniformBuffers is set,
           so it can alias them */
        Int _drawOffsetUniform{0};
        #endif
};

/**
@brief Configuration
@m_since_latest

@see @ref DistanceFieldVectorGL(const Configuration&),
    @ref compile(const Configuration&)
*/
template<UnsignedInt dimensions> class DistanceFieldVectorGL<dimensions>::Configuration {
    public:
        explicit Configuration() = default;

        /** @brief Flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set flags
         *
         * No flags are set by default.
         * @see @ref DistanceFieldVectorGL::flags()
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Material count
         *
         * @requires_gles30 Not defined on OpenGL ES 2.0 builds.
         * @requires_webgl20 Not defined on WebGL 1.0 builds.
         */
        UnsignedInt materialCount() const { return _materialCount; }

        /**
         * @brief Set material count
         *
         * If @ref Flag::UniformBuffers is set, describes size of a
         * @ref DistanceFieldVectorMaterialUniform buffer bound with
         * @ref bindMaterialBuffer(); as uniform buffers are required to have a
         * statically defined size. The per-draw materials are then specified
         * via @ref DistanceFieldVectorDrawUniform::materialId. Default value
         * is @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setDrawCount(),
         *      @ref DistanceFieldVectorGL::materialCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        Configuration& setMaterialCount(UnsignedInt materialCount) {
            _materialCount = materialCount;
            return *this;
        }

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
         * @ref TransformationProjectionUniform3D /
         * @ref DistanceFieldVectorDrawUniform /
         * @ref TextureTransformationUniform buffer bound with
         * @ref bindTransformationProjectionBuffer(), @ref bindDrawBuffer() and
         * @ref bindTextureTransformationBuffer(); as uniform buffers are
         * required to have a statically defined size. The draw offset is then
         * set via @ref setDrawOffset(). Default value is @cpp 1 @ce.
         *
         * If @ref Flag::UniformBuffers isn't set, this value is ignored.
         * @see @ref setFlags(), @ref setMaterialCount(),
         *      @ref DistanceFieldVectorGL::drawCount()
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        Configuration& setDrawCount(UnsignedInt drawCount) {
            _drawCount = drawCount;
            return *this;
        }
        #endif

    private:
        Flags _flags;
        #ifndef MAGNUM_TARGET_GLES2
        UnsignedInt _materialCount = 1,
            _drawCount = 1;
        #endif
};

/**
@brief Asynchronous compilation state
@m_since_latest

Returned by @ref compile(). See @ref shaders-async for more information.
*/
template<UnsignedInt dimensions> class DistanceFieldVectorGL<dimensions>::CompileState: public DistanceFieldVectorGL<dimensions> {
    /* Everything deliberately private except for the inheritance */
    friend class DistanceFieldVectorGL;

    explicit CompileState(NoCreateT): DistanceFieldVectorGL{NoCreate}, _vert{NoCreate}, _frag{NoCreate} {}

    explicit CompileState(DistanceFieldVectorGL<dimensions>&& shader, GL::Shader&& vert, GL::Shader&& frag, GL::Version version): DistanceFieldVectorGL<dimensions>{std::move(shader)}, _vert{std::move(vert)}, _frag{std::move(frag)}, _version{version} {}

    Implementation::GLShaderWrapper _vert, _frag;
    GL::Version _version;
};

/**
@brief Two-dimensional distance field vector OpenGL shader
@m_since_latest
*/
typedef DistanceFieldVectorGL<2> DistanceFieldVectorGL2D;

/**
@brief Three-dimensional distance field vector OpenGL shader
@m_since_latest
*/
typedef DistanceFieldVectorGL<3> DistanceFieldVectorGL3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{DistanceFieldVectorGL,DistanceFieldVectorGL::Flag} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, DistanceFieldVectorGL<dimensions>::Flag value);

/** @debugoperatorclassenum{DistanceFieldVectorGL,DistanceFieldVectorGL::Flags} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, DistanceFieldVectorGL<dimensions>::Flags value);
#else
namespace Implementation {
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, DistanceFieldVectorGLFlag value);
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, DistanceFieldVectorGLFlags value);
    CORRADE_ENUMSET_OPERATORS(DistanceFieldVectorGLFlags)
}
#endif

}}

#endif
