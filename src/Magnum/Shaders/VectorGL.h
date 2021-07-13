#ifndef Magnum_Shaders_VectorGL_h
#define Magnum_Shaders_VectorGL_h
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
 * @brief Class @ref Magnum::Shaders::VectorGL, typedef @ref Magnum::Shaders::VectorGL2D, @ref Magnum::Shaders::VectorGL3D
 * @m_since_latest
 */

#include "Magnum/DimensionTraits.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Shaders/GenericGL.h"
#include "Magnum/Shaders/visibility.h"

namespace Magnum { namespace Shaders {

namespace Implementation {
    enum class VectorGLFlag: UnsignedByte {
        TextureTransformation = 1 << 0,
        #ifndef MAGNUM_TARGET_GLES2
        UniformBuffers = 1 << 1,
        MultiDraw = UniformBuffers|(1 << 2)
        #endif
    };
    typedef Containers::EnumSet<VectorGLFlag> VectorGLFlags;
}

/**
@brief Vector OpenGL shader
@m_since_latest

Renders vector art in plain grayscale form. See also @ref DistanceFieldVectorGL
for more advanced effects. For rendering an unchanged texture you can use the
@ref FlatGL shader. You need to provide the @ref Position and
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

@section Shaders-VectorGL-usage Example usage

Common mesh setup:

@snippet MagnumShaders-gl.cpp VectorGL-usage1

Common rendering setup:

@snippet MagnumShaders-gl.cpp VectorGL-usage2

@section Shaders-VectorGL-ubo Uniform buffers

See @ref shaders-usage-ubo for a high-level overview that applies to all
shaders. In this particular case, because the shader doesn't need a separate
projection and transformation matrix, a combined one is supplied via a
@ref TransformationProjectionUniform2D / @ref TransformationProjectionUniform3D
buffer. To maximize use of the limited uniform buffer memory, materials are
supplied separately in a @ref VectorMaterialUniform buffer and then referenced
via @relativeref{VectorDrawUniform,materialId} from a @ref VectorDrawUniform;
for optional texture transformation a per-draw
@ref TextureTransformationUniform can be supplied as well. A uniform buffer
setup equivalent to the above would look like this:

@snippet MagnumShaders-gl.cpp VectorGL-ubo

For a multidraw workflow enable @ref Flag::MultiDraw, supply desired material
and draw count in the @ref VectorGL(Flags, UnsignedInt, UnsignedInt)
constructor and specify material references and texture offsets for every draw.
Texture arrays aren't currently supported for this shader. Besides that, the
usage is similar for all shaders, see @ref shaders-usage-multidraw for an
example.

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

@see @ref shaders, @ref VectorGL2D, @ref VectorGL3D
*/
template<UnsignedInt dimensions> class MAGNUM_SHADERS_EXPORT VectorGL: public GL::AbstractShaderProgram {
    public:
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
         * @see @ref Flags, @ref flags()
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
             * @ref bindDrawBuffer(), @ref bindTextureTransformationBuffer()
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
        typedef Implementation::VectorGLFlag Flag;
        typedef Implementation::VectorGLFlags Flags;
        #endif

        /**
         * @brief Constructor
         * @param flags     Flags
         *
         * While this function is meant mainly for the classic uniform
         * scenario (without @ref Flag::UniformBuffers set), it's equivalent to
         * @ref VectorGL(Flags, UnsignedInt, UnsignedInt) with @p materialCount
         * and @p drawCount set to @cpp 1 @ce.
         */
        explicit VectorGL(Flags flags = {});

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Construct for a multi-draw scenario
         * @param flags         Flags
         * @param materialCount Size of a @ref VectorMaterialUniform buffer
         *      bound with @ref bindMaterialBuffer()
         * @param drawCount     Size of a @ref TransformationProjectionUniform2D
         *      / @ref TransformationProjectionUniform3D /
         *      @ref VectorDrawUniform / @ref TextureTransformationUniform
         *      buffer bound with @ref bindTransformationProjectionBuffer(),
         *      @ref bindDrawBuffer() and @ref bindTextureTransformationBuffer()
         *
         * If @p flags contains @ref Flag::UniformBuffers, @p materialCount and
         * @p drawCount describe the uniform buffer sizes as these are required
         * to have a statically defined size. The draw offset is then set via
         * @ref setDrawOffset() and the per-draw materials specified via
         * @ref VectorDrawUniform::materialId.
         *
         * If @p flags don't contain @ref Flag::UniformBuffers, @p drawCount is
         * ignored and the constructor behaves the same as @ref VectorGL(Flags).
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
        explicit VectorGL(Flags flags, UnsignedInt materialCount, UnsignedInt drawCount);
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
        explicit VectorGL(NoCreateT) noexcept: GL::AbstractShaderProgram{NoCreate} {}

        /** @brief Copying is not allowed */
        VectorGL(const VectorGL<dimensions>&) = delete;

        /** @brief Move constructor */
        VectorGL(VectorGL<dimensions>&&) noexcept = default;

        /** @brief Copying is not allowed */
        VectorGL<dimensions>& operator=(const VectorGL<dimensions>&) = delete;

        /** @brief Move assignment */
        VectorGL<dimensions>& operator=(VectorGL<dimensions>&&) noexcept = default;

        /**
         * @brief Flags
         * @m_since{2020,06}
         */
        Flags flags() const { return _flags; }

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Material count
         * @m_since_latest
         *
         * Statically defined size of the @ref VectorMaterialUniform uniform
         * buffer. Has use only if @ref Flag::UniformBuffers is set.
         * @see @ref bindMaterialBuffer()
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
         * @ref TransformationProjectionUniform3D, @ref VectorDrawUniform and
         * @ref TextureTransformationUniform uniform buffers. Has use only if
         * @ref Flag::UniformBuffers is set.
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
        VectorGL<dimensions>& setTransformationProjectionMatrix(const MatrixTypeFor<dimensions, Float>& matrix);

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
        VectorGL<dimensions>& setTextureMatrix(const Matrix3& matrix);

        /**
         * @brief Set background color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0x00000000_rgbaf @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref VectorMaterialUniform::backgroundColor and call
         * @ref bindMaterialBuffer() instead.
         * @see @ref setColor()
         */
        VectorGL<dimensions>& setBackgroundColor(const Color4& color);

        /**
         * @brief Set fill color
         * @return Reference to self (for method chaining)
         *
         * Initial value is @cpp 0xffffffff_rgbaf @ce.
         *
         * Expects that @ref Flag::UniformBuffers is not set, in that case fill
         * @ref VectorMaterialUniform::color and call @ref bindMaterialBuffer()
         * instead.
         * @see @ref setBackgroundColor()
         */
        VectorGL<dimensions>& setColor(const Color4& color);

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
         * @ref VectorDrawUniform and @ref TextureTransformationUniform buffers
         * bound with @ref bindTransformationProjectionBuffer(),
         * @ref bindDrawBuffer() and @ref bindTextureTransformationBuffer()
         * should be used for current draw. Expects that
         * @ref Flag::UniformBuffers is set and @p offset is less than
         * @ref drawCount(). Initial value is @cpp 0 @ce, if @ref drawCount()
         * is @cpp 1 @ce, the function is a no-op as the shader assumes draw
         * offset to be always zero.
         *
         * If @ref Flag::MultiDraw is set, @glsl gl_DrawID @ce is added to this
         * value, which makes each draw submitted via
         * @ref GL::AbstractShaderProgram::draw(Containers::ArrayView<const Containers::Reference<MeshView>>)
         * pick up its own per-draw parameters.
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        VectorGL<dimensions>& setDrawOffset(UnsignedInt offset);

        /**
         * @brief Set a transformation and projection uniform buffer
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
        VectorGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        VectorGL<dimensions>& bindTransformationProjectionBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a draw uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref drawCount() instances of
         * @ref VectorDrawUniform. At the very least you need to call also
         * @ref bindTransformationProjectionBuffer() and
         * @ref bindMaterialBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        VectorGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        VectorGL<dimensions>& bindDrawBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a texture transformation uniform buffer
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
        VectorGL<dimensions>& bindTextureTransformationBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        VectorGL<dimensions>& bindTextureTransformationBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @brief Set a material uniform buffer
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Expects that @ref Flag::UniformBuffers is set. The buffer is
         * expected to contain @ref materialCount() instances of
         * @ref VectorMaterialUniform. At the very least you need to call also
         * @ref bindTransformationProjectionBuffer() and @ref bindDrawBuffer().
         * @requires_gl31 Extension @gl_extension{ARB,uniform_buffer_object}
         * @requires_gles30 Uniform buffers are not available in OpenGL ES 2.0.
         * @requires_webgl20 Uniform buffers are not available in WebGL 1.0.
         */
        VectorGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer);
        /**
         * @overload
         * @m_since_latest
         */
        VectorGL<dimensions>& bindMaterialBuffer(GL::Buffer& buffer, GLintptr offset, GLsizeiptr size);

        /**
         * @}
         */
        #endif

        /** @{
         * @name Texture binding
         */

        /**
         * @brief Bind vector texture
         * @return Reference to self (for method chaining)
         *
         * @see @ref Flag::TextureTransformation, @ref setTextureMatrix()s
         */
        VectorGL<dimensions>& bindVectorTexture(GL::Texture2D& texture);

        /**
         * @}
         */

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        VectorGL<dimensions>& draw(GL::Mesh& mesh) {
            return static_cast<VectorGL<dimensions>&>(GL::AbstractShaderProgram::draw(mesh));
        }
        VectorGL<dimensions>& draw(GL::Mesh&& mesh) {
            return static_cast<VectorGL<dimensions>&>(GL::AbstractShaderProgram::draw(mesh));
        }
        VectorGL<dimensions>& draw(GL::MeshView& mesh) {
            return static_cast<VectorGL<dimensions>&>(GL::AbstractShaderProgram::draw(mesh));
        }
        VectorGL<dimensions>& draw(GL::MeshView&& mesh) {
            return static_cast<VectorGL<dimensions>&>(GL::AbstractShaderProgram::draw(mesh));
        }
        VectorGL<dimensions>& draw(Containers::ArrayView<const Containers::Reference<GL::MeshView>> meshes) {
            return static_cast<VectorGL<dimensions>&>(GL::AbstractShaderProgram::draw(meshes));
        }
        VectorGL<dimensions>& draw(std::initializer_list<Containers::Reference<GL::MeshView>> meshes) {
            return static_cast<VectorGL<dimensions>&>(GL::AbstractShaderProgram::draw(meshes));
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
        UnsignedInt _materialCount{}, _drawCount{};
        #endif
        Int _transformationProjectionMatrixUniform{0},
            _textureMatrixUniform{1},
            _backgroundColorUniform{2},
            _colorUniform{3};
        #ifndef MAGNUM_TARGET_GLES2
        /* Used instead of all other uniforms when Flag::UniformBuffers is set,
           so it can alias them */
        Int _drawOffsetUniform{0};
        #endif
};

/**
@brief Two-dimensional vector OpenGL shader
@m_since_latest
*/
typedef VectorGL<2> VectorGL2D;

/**
@brief Three-dimensional vector OpenGL shader
@m_since_latest
*/
typedef VectorGL<3> VectorGL3D;

#ifdef DOXYGEN_GENERATING_OUTPUT
/** @debugoperatorclassenum{VectorGL,VectorGL::Flag} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, VectorGL<dimensions>::Flag value);

/** @debugoperatorclassenum{VectorGL,VectorGL::Flags} */
template<UnsignedInt dimensions> Debug& operator<<(Debug& debug, VectorGL<dimensions>::Flags value);
#else
namespace Implementation {
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, VectorGLFlag value);
    MAGNUM_SHADERS_EXPORT Debug& operator<<(Debug& debug, VectorGLFlags value);
    CORRADE_ENUMSET_OPERATORS(VectorGLFlags)
}
#endif

}}

#endif
