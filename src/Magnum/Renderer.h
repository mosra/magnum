#ifndef Magnum_Renderer_h
#define Magnum_Renderer_h
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
 * @brief Class @ref Magnum::Renderer
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

namespace Implementation { struct RendererState; }

/** @nosubgrouping
@brief Global renderer configuration.

@todo @extension{ARB,viewport_array}
@todo `GL_POINT_SIZE_GRANULARITY`, `GL_POINT_SIZE_RANGE` (?)
@todo `GL_STEREO`, `GL_DOUBLEBUFFER` (?)
@todo `GL_MAX_CLIP_DISTANCES`...
*/
class MAGNUM_EXPORT Renderer {
    friend Context;
    friend Implementation::RendererState;

    public:
        Renderer() = delete;

        /**
         * @brief Affected polygon facing for culling, stencil operations and masks
         *
         * @see @ref setFaceCullingMode(),
         *      @ref setStencilFunction(PolygonFacing, StencilFunction, Int, UnsignedInt),
         *      @ref setStencilOperation(PolygonFacing, StencilOperation, StencilOperation, StencilOperation),
         *      @ref setStencilMask(PolygonFacing, UnsignedInt)
         */
        enum class PolygonFacing: GLenum {
            Front = GL_FRONT,                   /**< Front-facing polygons */
            Back = GL_BACK,                     /**< Back-facing polygons */
            FrontAndBack = GL_FRONT_AND_BACK    /**< Front- and back-facing polygons */
        };

        /** @{ @name Renderer features */

        /**
         * @brief Features
         *
         * All features are disabled by default unless specified otherwise.
         * @see @ref enable(), @ref disable(), @ref setFeature()
         */
        enum class Feature: GLenum {
            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Coherent advanced blending. Enabled by default if desktop/ES
             * extension @extension2{KHR,blend_equation_advanced_coherent,blend_equation_advanced}
             * is available. See @ref blendBarrier() for more information.
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in WebGL.
             */
            BlendAdvancedCoherent = GL_BLEND_ADVANCED_COHERENT_KHR,
            #endif

            /**
             * Blending
             * @see @ref setBlendEquation(), @ref setBlendFunction(),
             *      @ref setBlendColor()
             */
            Blending = GL_BLEND,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Debug output. Disabled by default unless the GL context was
             * created with debug output enabled.
             * @see @ref DebugOutput, @ref Feature::DebugOutputSynchronous,
             *      @ref Platform::Sdl2Application::Configuration::Flag::Debug "Platform::*Application::Configuration::Flag::Debug"
             * @requires_gl43 Extension @extension{KHR,debug}
             * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
             *      @extension2{KHR,debug,debug}
             * @requires_gles Debug output is not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            DebugOutput = GL_DEBUG_OUTPUT,
            #else
            DebugOutput = GL_DEBUG_OUTPUT_KHR,
            #endif

            /**
             * Synchronous debug output. Has effect only if
             * @ref Feature::DebugOutput is enabled.
             * @see @ref DebugMessage
             * @requires_gl43 Extension @extension{KHR,debug}
             * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
             *      @extension2{KHR,debug,debug}
             * @requires_gles Debug output is not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            DebugOutputSynchronous = GL_DEBUG_OUTPUT_SYNCHRONOUS,
            #else
            DebugOutputSynchronous = GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR,
            #endif
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Depth clamping. If enabled, ignores near and far clipping plane.
             * @requires_gl32 Extension @extension{ARB,depth_clamp}
             * @requires_gl Depth clamping is not available in OpenGL ES and
             *      WebGL.
             */
            DepthClamp = GL_DEPTH_CLAMP,
            #endif

            /**
             * Depth test
             * @see @ref setClearDepth(), @ref setDepthFunction(),
             *      @ref setDepthMask()
             */
            DepthTest = GL_DEPTH_TEST,

            Dithering = GL_DITHER,          /**< Dithering. Enabled by default. */

            /**
             * Back face culling
             * @see @ref setFrontFace()
             */
            FaceCulling = GL_CULL_FACE,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Perform sRGB conversion of values written to sRGB framebuffers.
             * Disabled by default on desktop GL, enabled by default on OpenGL
             * ES.
             * @requires_gl30 Extension @extension{ARB,framebuffer_sRGB}
             * @requires_es_extension @extension{EXT,sRGB_write_control}. If
             *      the extension is not present, there's no ability to turn
             *      the sRGB conversion off.
             * @requires_gles WebGL has no ability to turn the sRGB conversion
             *      off.
             */
            #ifndef MAGNUM_TARGET_GLES
            FramebufferSRGB = GL_FRAMEBUFFER_SRGB,
            #else
            FramebufferSRGB = GL_FRAMEBUFFER_SRGB_EXT,
            #endif
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Logical operation
             * @see @ref setLogicOperation()
             * @requires_gl Logical operations on framebuffer are not
             *      available in OpenGL ES and WebGL.
             */
            LogicOperation = GL_COLOR_LOGIC_OP,

            /**
             * Multisampling. Enabled by default. Note that the actual presence
             * of this feature in default framebuffer depends on context
             * configuration, see for example @ref Platform::Sdl2Application::Configuration::setSampleCount().
             * @requires_gl Always enabled in OpenGL ES and WebGL.
             */
            Multisampling = GL_MULTISAMPLE,
            #endif

            /**
             * Offset filled polygons
             * @see @ref Feature::PolygonOffsetLine, @ref Feature::PolygonOffsetPoint,
             *      @ref setPolygonOffset()
             */
            PolygonOffsetFill = GL_POLYGON_OFFSET_FILL,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Offset lines
             * @see @ref Feature::PolygonOffsetFill, @ref Feature::PolygonOffsetPoint,
             *      @ref setPolygonOffset()
             * @requires_es_extension Extension @extension{NV,polygon_offset}
             * @requires_gles Only @ref Feature::PolygonOffsetFill is available
             *      in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            PolygonOffsetLine = GL_POLYGON_OFFSET_LINE,
            #else
            PolygonOffsetLine = GL_POLYGON_OFFSET_LINE_NV,
            #endif

            /**
             * Offset points
             * @see @ref Feature::PolygonOffsetFill, @ref Feature::PolygonOffsetLine,
             *      @ref setPolygonOffset()
             * @requires_es_extension Extension @extension{NV,polygon_offset}
             * @requires_gles Only @ref Feature::PolygonOffsetFill is available
             *      in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            PolygonOffsetPoint = GL_POLYGON_OFFSET_POINT,
            #else
            PolygonOffsetPoint = GL_POLYGON_OFFSET_POINT_NV,
            #endif
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Programmable point size. If enabled, the point size is taken
             * from vertex/geometry shader builtin `gl_PointSize`.
             * @see @ref setPointSize()
             * @requires_gl Always enabled on OpenGL ES and WebGL.
             */
            ProgramPointSize = GL_PROGRAM_POINT_SIZE,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Discard primitives before rasterization.
             * @requires_gl30 Extension @extension{EXT,transform_feedback}
             * @requires_gles30 Transform feedback is not available in OpenGL
             *      ES 2.0.
             * @requires_webgl20 Transform feedback is not available in WebGL
             *      1.0.
             */
            RasterizerDiscard = GL_RASTERIZER_DISCARD,
            #endif

            /**
             * Scissor test
             * @see @ref setScissor()
             */
            ScissorTest = GL_SCISSOR_TEST,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Seamless cube map texture.
             * @see @ref CubeMapTexture, @ref CubeMapTextureArray
             * @requires_gl32 Extension @extension{ARB,seamless_cube_map}
             * @requires_gl Not available in OpenGL ES 2.0 and WebGL 1.0,
             *      always enabled in OpenGL ES 3.0 and WebGL 2.0.
             */
            SeamlessCubeMapTexture = GL_TEXTURE_CUBE_MAP_SEAMLESS,
            #endif

            /**
             * Stencil test
             * @see @ref setClearStencil(), @ref setStencilFunction(),
             *      @ref setStencilOperation(), @ref setStencilMask()
             */
            StencilTest = GL_STENCIL_TEST
        };

        /**
         * @brief Enable feature
         *
         * @see @ref disable(), @ref setFeature(), @fn_gl{Enable}
         */
        static void enable(Feature feature);

        /**
         * @brief Disable feature
         *
         * @see @ref enable(), @ref setFeature(), @fn_gl{Disable}
         */
        static void disable(Feature feature);

        /**
         * @brief Enable or disable feature
         *
         * Convenience equivalent to the following:
         * @code
         * enabled ? Renderer::enable(feature) : Renderer::disable(feature)
         * @endcode
         * Prefer to use @ref enable() and @ref disable() directly to avoid
         * unnecessary branching.
         */
        static void setFeature(Feature feature, bool enabled);

        /**
         * @brief Hint
         *
         * @see @ref setHint()
         * @todo other hints
         */
        enum class Hint: GLenum {
            /**
             * Accuracy of derivative calculation in fragment shader.
             * @requires_gles30 Extension @extension{OES,standard_derivatives}
             *      in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{OES,standard_derivatives}
             *      in WebGL 1.0.
             */
            #ifndef MAGNUM_TARGET_GLES2
            FragmentShaderDerivative = GL_FRAGMENT_SHADER_DERIVATIVE_HINT,
            #else
            FragmentShaderDerivative = GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Quality and performance of texture image compression.
             * @requires_gl On-the-fly texture image compression is not
             *      available in OpenGL ES.
             */
            TextureCompression = GL_TEXTURE_COMPRESSION_HINT
            #endif
        };

        /**
         * @brief Hint mode
         *
         * @see @ref setHint()
         */
        enum class HintMode: GLenum {
            Fastest = GL_FASTEST,   /**< Most efficient option. */
            Nicest = GL_NICEST,     /**< Most correct or highest quality option. */
            DontCare = GL_DONT_CARE /**< No preference. */
        };

        /**
         * @brief Set hint
         *
         * Initial value is @ref HintMode::DontCare for all targets.
         * @see @fn_gl{Hint}
         */
        static void setHint(Hint target, HintMode mode);

        /*@}*/

        /** @{ @name Clearing values */

        /**
         * @brief Set clear color
         *
         * Initial value is `{0.125f, 0.125f, 0.125f, 1.0f}`.
         * @see @ref Framebuffer::clearColor() "*Framebuffer::clearColor()",
         *      @fn_gl{ClearColor}
         * @deprecated_gl Prefer to use @ref Framebuffer::clearColor() "*Framebuffer::clearColor()"
         *      instead of @ref setClearColor() and
         *      @ref AbstractFramebuffer::clear() as it leads to less state
         *      changes.
         */
        static void setClearColor(const Color4& color);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set clear depth
         *
         * Initial value is `1.0`.
         * @see @ref Feature::DepthTest, @ref AbstractFramebuffer::clearDepth(),
         *      @ref AbstractFramebuffer::clearDepthStencil(), @fn_gl{ClearDepth}
         * @requires_gl See @ref setClearDepth(Float), which is available in
         *      OpenGL ES and WebGL.
         * @deprecated_gl Prefer to use @ref AbstractFramebuffer::clearDepth()
         *      / @ref AbstractFramebuffer::clearDepthStencil() instead of
         *      @ref setClearDepth() and @ref AbstractFramebuffer::clear() as
         *      it leads to less state changes.
         */
        static void setClearDepth(Double depth);
        #endif

        /**
         * @overload
         *
         * If OpenGL ES, OpenGL 4.1 or extension @extension{ARB,ES2_compatibility}
         * is not available, this function behaves exactly as
         * @ref setClearDepth(Double).
         * @see @ref Feature::DepthTest, @ref AbstractFramebuffer::clearDepth(),
         *      @ref AbstractFramebuffer::clearDepthStencil(), @fn_gl{ClearDepth}
         * @deprecated_gl Prefer to use @ref AbstractFramebuffer::clearDepth()
         *      / @ref AbstractFramebuffer::clearDepthStencil() instead of
         *      @ref setClearDepth() and @ref AbstractFramebuffer::clear() as
         *      it leads to less state changes.
         */
        static void setClearDepth(Float depth);

        /**
         * @brief Set clear stencil
         *
         * Initial value is `0`.
         * @see @ref Feature::StencilTest, @ref AbstractFramebuffer::clearStencil(),
         *      @ref AbstractFramebuffer::clearDepthStencil(), @fn_gl{ClearStencil}
         * @deprecated_gl Prefer to use @ref AbstractFramebuffer::clearStencil()
         *      / @ref AbstractFramebuffer::clearDepthStencil() instead of
         *      @ref setClearStencil() and @ref AbstractFramebuffer::clear() as
         *      it leads to less state changes.
         */
        static void setClearStencil(Int stencil);

        /*@}*/

        /** @name Polygon drawing settings */

        /**
         * @brief Front facing polygon winding
         *
         * @see @ref setFrontFace()
         */
        enum class FrontFace: GLenum {
            /** @brief Counterclockwise polygons are front facing (default). */
            CounterClockWise = GL_CCW,

            /** @brief Clockwise polygons are front facing. */
            ClockWise = GL_CW
        };

        /**
         * @brief Set front-facing polygon winding
         *
         * Initial value is @ref FrontFace::CounterClockWise.
         * @see @ref setFaceCullingMode(), @fn_gl{FrontFace}
         */
        static void setFrontFace(FrontFace mode);

        /**
         * @brief Which polygon facing to cull
         *
         * Initial value is @ref PolygonFacing::Back. If set to both front and
         * back, only points and lines are drawn.
         * @see @ref Feature::FaceCulling, @ref setFrontFace(),
         *      @fn_gl{CullFace}
         */
        static void setFaceCullingMode(PolygonFacing mode);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Provoking vertex
         *
         * @see @ref setProvokingVertex()
         * @requires_gl32 Extension @extension{ARB,provoking_vertex}. Older
         *      versions behave always like @ref ProvokingVertex::LastVertexConvention.
         * @requires_gl OpenGL ES and WebGL behave always like
         *      @ref ProvokingVertex::LastVertexConvention.
         */
        enum class ProvokingVertex: GLenum {
            /** @brief Use first vertex of each polygon. */
            FirstVertexConvention = GL_FIRST_VERTEX_CONVENTION,

            /** @brief Use last vertex of each polygon (default). */
            LastVertexConvention = GL_LAST_VERTEX_CONVENTION
        };

        /**
         * @brief Set provoking vertex
         *
         * Initial value is @ref ProvokingVertex::LastVertexConvention.
         * @see @fn_gl{ProvokingVertex}
         * @requires_gl32 Extension @extension{ARB,provoking_vertex}. Older
         *      versions behave always like the default.
         * @requires_gl OpenGL ES and WebGL behave always like the default.
         */
        static void setProvokingVertex(ProvokingVertex mode);
        #endif

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Polygon mode
         *
         * @see @ref setPolygonMode()
         * @requires_es_extension Extension @extension{NV,polygon_mode}.
         *      Otherwise behaves always like @ref PolygonMode::Fill. See
         *      @ref Mesh::setPrimitive() for possible workaround.
         * @requires_gles WebGL behaves always like @ref PolygonMode::Fill. See
         *      @ref Mesh::setPrimitive() for possible workaround.
         */
        enum class PolygonMode: GLenum {
            /**
             * Interior of the polygon is filled (default).
             */
            #ifndef MAGNUM_TARGET_GLES
            Fill = GL_FILL,
            #else
            Fill = GL_FILL_NV,
            #endif

            /**
             * Boundary edges are filled. See also @ref setLineWidth().
             */
            #ifndef MAGNUM_TARGET_GLES
            Line = GL_LINE,
            #else
            Line = GL_LINE_NV,
            #endif

            /**
             * Starts of boundary edges are drawn as points. See also
             * @ref setPointSize().
             */
            #ifndef MAGNUM_TARGET_GLES
            Point = GL_POINT
            #else
            Point = GL_POINT_NV
            #endif
        };

        /**
         * @brief Set polygon drawing mode
         *
         * Initial value is @ref PolygonMode::Fill.
         * @see @fn_gl{PolygonMode}
         * @requires_es_extension Extension @extension{NV,polygon_mode}.
         *      Otherwise behaves always like the default. See
         *      @ref Mesh::setPrimitive() for possible workaround.
         * @requires_gles WebGL behaves always like the default. See
         *      @ref Mesh::setPrimitive() for possible workaround.
         */
        static void setPolygonMode(PolygonMode mode);
        #endif

        /**
         * @brief Set polygon offset
         * @param factor    Scale factor
         * @param units     Offset units
         *
         * @see @ref Feature::PolygonOffsetFill, @ref Feature::PolygonOffsetLine,
         *      @ref Feature::PolygonOffsetPoint, @fn_gl{PolygonOffset}
         */
        static void setPolygonOffset(Float factor, Float units);

        /**
         * @brief Set line width
         *
         * Initial value is `1.0f`.
         * @see @fn_gl{LineWidth}
         */
        static void setLineWidth(Float width);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set point size
         *
         * Initial value is `1.0f`.
         * @see @ref Feature::ProgramPointSize, @fn_gl{PointSize}
         * @requires_gl Use `gl_PointSize` builtin vertex shader variable in
         *      OpenGL ES and WebGL instead.
         */
        static void setPointSize(Float size);
        #endif

        /*@}*/

        /** @{ @name Scissor operations */

        /**
         * @brief Set scissor rectangle
         *
         * Initial value is set to cover whole window.
         * @see @ref Feature::ScissorTest, @fn_gl{Scissor}
         */
        static void setScissor(const Range2Di& rectangle);

        /*@}*/

        /** @{ @name Stencil operations */

        /**
         * @brief Stencil function
         *
         * @see @ref setStencilFunction(), @ref DepthFunction
         */
        enum class StencilFunction: GLenum {
            Never = GL_NEVER,           /**< Never pass the test. */
            Always = GL_ALWAYS,         /**< Always pass the test. */
            Less = GL_LESS,             /**< Pass when reference value is less than buffer value. */
            LessOrEqual = GL_LEQUAL,    /**< Pass when reference value is less than or equal to buffer value. */
            Equal = GL_EQUAL,           /**< Pass when reference value is equal to buffer value. */
            NotEqual = GL_NOTEQUAL,     /**< Pass when reference value is not equal to buffer value. */
            GreaterOrEqual = GL_GEQUAL, /**< Pass when reference value is greater than or equal to buffer value. */
            Greater = GL_GREATER        /**< Pass when reference value is greater than buffer value. */
        };

        /**
         * @brief Stencil operation
         *
         * @see @ref setStencilOperation()
         */
        enum class StencilOperation: GLenum {
            Keep = GL_KEEP, /**< Keep the current value. */
            Zero = GL_ZERO, /**< Set the stencil buffer value to `0`. */

            /**
             * Set the stencil value to reference value specified by
             * @ref setStencilFunction().
             */
            Replace = GL_REPLACE,

            /**
             * Increment the current stencil buffer value, clamp to maximum
             * possible value on overflow.
             */
            Increment = GL_INCR,

            /**
             * Increment the current stencil buffer value, wrap to zero on
             * overflow.
             */
            IncrementWrap = GL_INCR_WRAP,

            /**
             * Increment the current stencil buffer value, clamp to minimum
             * possible value on underflow.
             */
            Decrement = GL_DECR,

            /**
             * Decrement the current stencil buffer value, wrap to maximum
             * possible value on underflow.
             */
            DecrementWrap = GL_DECR_WRAP,

            /**
             * Bitwise invert the current stencil buffer value.
             */
            Invert = GL_INVERT
        };

        /**
         * @brief Set stencil function
         * @param facing            Affected polygon facing
         * @param function          Stencil function. Initial value is
         *      @ref StencilFunction::Always.
         * @param referenceValue    Reference value. Initial value is `0`.
         * @param mask              Mask for both reference and buffer value.
         *      Initial value is all `1`s.
         *
         * @see @ref Feature::StencilTest, @ref setStencilFunction(StencilFunction, Int, UnsignedInt),
         *      @ref setStencilOperation(), @fn_gl{StencilFuncSeparate}
         * @requires_gles In WebGL the reference value and mask must be the
         *      same for both front and back polygon facing.
         */
        static void setStencilFunction(PolygonFacing facing, StencilFunction function, Int referenceValue, UnsignedInt mask);

        /**
         * @brief Set stencil function
         *
         * The same as @ref setStencilFunction(PolygonFacing, StencilFunction, Int, UnsignedInt)
         * with @p facing set to @ref PolygonFacing::FrontAndBack.
         * @see @ref Feature::StencilTest, @ref setStencilOperation(),
         *      @fn_gl{StencilFunc}
         */
        static void setStencilFunction(StencilFunction function, Int referenceValue, UnsignedInt mask);

        /**
         * @brief Set stencil operation
         * @param facing            Affected polygon facing
         * @param stencilFail       Action when stencil test fails
         * @param depthFail         Action when stencil test passes, but depth
         *      test fails
         * @param depthPass         Action when both stencil and depth test
         *      pass
         *
         * Initial value for all fields is @ref StencilOperation::Keep.
         * @see @ref Feature::StencilTest, @ref setStencilOperation(StencilOperation, StencilOperation, StencilOperation),
         *      @ref setStencilFunction(), @fn_gl{StencilOpSeparate}
         */
        static void setStencilOperation(PolygonFacing facing, StencilOperation stencilFail, StencilOperation depthFail, StencilOperation depthPass);

        /**
         * @brief Set stencil operation
         *
         * The same as @ref setStencilOperation(PolygonFacing, StencilOperation, StencilOperation, StencilOperation)
         * with @p facing set to @ref PolygonFacing::FrontAndBack.
         * @see @ref Feature::StencilTest, @ref setStencilFunction(),
         *      @fn_gl{StencilOp}
         */
        static void setStencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation depthPass);

        /*@}*/

        /** @{ @name Depth testing */

        /**
         * @brief Depth function
         *
         * @see @ref setDepthFunction()
         */
        typedef StencilFunction DepthFunction;

        /**
         * @brief Set depth function
         *
         * Initial value is @ref DepthFunction::Less.
         * @see @ref Feature::DepthTest, @fn_gl{DepthFunc}
         */
        static void setDepthFunction(DepthFunction function);

        /*@}*/

        /** @{ @name Masking writes */

        /**
         * @brief Mask color writes
         *
         * Set to `false` to disallow writing to given color channel. Initial
         * values are all `true`.
         * @see @ref setDepthMask(), @ref setStencilMask(), @fn_gl{ColorMask}
         * @todo Masking only given draw buffer
         */
        static void setColorMask(GLboolean allowRed, GLboolean allowGreen, GLboolean allowBlue, GLboolean allowAlpha);

        /**
         * @brief Mask depth writes
         *
         * Set to `false` to disallow writing to depth buffer. Initial value
         * is `true`.
         * @see @ref setColorMask(), @ref setStencilMask(), @fn_gl{DepthMask}
         */
        static void setDepthMask(GLboolean allow);

        /**
         * @brief Mask stencil writes
         *
         * Set given bit to `0` to disallow writing stencil value for given
         * faces to it. Initial value is all `1`s.
         *
         * @see @ref setStencilMask(UnsignedInt), @ref setColorMask(),
         *      @ref setDepthMask(), @fn_gl{StencilMaskSeparate}
         * @requires_gles In WebGL the mask must be the same for both front and
         *      back polygon facing.
         */
        static void setStencilMask(PolygonFacing facing, UnsignedInt allowBits);

        /**
         * @brief Mask stencil writes
         *
         * The same as calling @ref setStencilMask(PolygonFacing, UnsignedInt)
         * with `facing` set to @ref PolygonFacing::FrontAndBack.
         * @see @fn_gl{StencilMask}
         */
        static void setStencilMask(UnsignedInt allowBits);

        /*@}*/

        /**
         * @{ @name Blending
         *
         * You have to enable blending with @ref enable() first.
         * @todo Blending for given draw buffer
         */

        /**
         * @brief Blend equation
         *
         * @see @ref setBlendEquation()
         */
        enum class BlendEquation: GLenum {
            Add = GL_FUNC_ADD,                          /**< `source + destination` */
            Subtract = GL_FUNC_SUBTRACT,                /**< `source - destination` */
            ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT, /**< `destination - source` */

            /**
             * `min(source, destination)`
             * @requires_gles30 Extension @extension2{EXT,blend_minmax,blend_minmax}
             *      in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{EXT,blend_minmax}
             *      in WebGL 1.0.
             */
            #ifndef MAGNUM_TARGET_GLES2
            Min = GL_MIN,
            #else
            Min = GL_MIN_EXT,
            #endif

            /**
             * `max(source, destination)`
             * @requires_gles30 Extension @extension2{EXT,blend_minmax,blend_minmax}
             *      in OpenGL ES 2.0.
             * @requires_webgl20 Extension @webgl_extension{EXT,blend_minmax}
             *      in WebGL 1.0.
             */
            #ifndef MAGNUM_TARGET_GLES2
            Max = GL_MAX,
            #else
            Max = GL_MAX_EXT,
            #endif

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Multiply. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            Multiply = GL_MULTIPLY_KHR,

            /**
             * Screen. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            Screen = GL_SCREEN_KHR,

            /**
             * Overlay. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            Overlay = GL_OVERLAY_KHR,

            /**
             * Darken. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            Darken = GL_DARKEN_KHR,

            /**
             * Lighten. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            Lighten = GL_LIGHTEN_KHR,

            /**
             * Color dodge. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            ColorDodge = GL_COLORDODGE_KHR,

            /**
             * Color burn. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            ColorBurn = GL_COLORBURN_KHR,

            /**
             * Hard light. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            HardLight = GL_HARDLIGHT_KHR,

            /**
             * Soft light. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            SoftLight = GL_SOFTLIGHT_KHR,

            /**
             * Difference. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            Difference = GL_DIFFERENCE_KHR,

            /**
             * Exclusion. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            Exclusion = GL_EXCLUSION_KHR,

            /**
             * HSL hue. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            HslHue = GL_HSL_HUE_KHR,

            /**
             * HSL saturation. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            HslSaturation = GL_HSL_SATURATION_KHR,

            /**
             * HSL color. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            HslColor = GL_HSL_COLOR_KHR,

            /**
             * HSL luminosity. Not accepted as separate parameters in
             * @ref setBlendEquation(BlendEquation, BlendEquation), only in
             * @ref setBlendEquation(BlendEquation).
             * @see @ref blendBarrier()
             * @requires_extension Extension @extension{KHR,blend_equation_advanced}
             * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
             * @requires_gles Advanced blend equations are not available in
             *      WebGL.
             */
            HslLuminosity = GL_HSL_LUMINOSITY_KHR
            #endif
        };

        /**
         * @brief Blend function
         *
         * @see @ref setBlendFunction()
         */
        enum class BlendFunction: GLenum {
            /** Zero (@f$ RGB = (0.0, 0.0, 0.0); A = 0.0 @f$) */
            Zero = GL_ZERO,

            /** One (@f$ RGB = (1.0, 1.0, 1.0); A = 1.0 @f$) */
            One = GL_ONE,

            /**
             * Constant color (@f$ RGB = (R_c, G_c, B_c); A = A_c @f$)
             *
             * @see @ref setBlendColor()
             */
            ConstantColor = GL_CONSTANT_COLOR,

            /**
             * One minus constant color (@f$ RGB = (1.0 - R_c, 1.0 - G_c, 1.0 - B_c); A = 1.0 - A_c @f$)
             *
             * @see @ref setBlendColor()
             */
            OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,

            /**
             * Constant alpha (@f$ RGB = (A_c, A_c, A_c); A = A_c @f$)
             *
             * @see @ref setBlendColor()
             */
            ConstantAlpha = GL_CONSTANT_ALPHA,

            /**
             * One minus constant alpha (@f$ RGB = (1.0 - A_c, 1.0 - A_c, 1.0 - A_c); A = 1.0 - A_c @f$)
             *
             * @see @ref setBlendColor()
             */
            OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,

            /** Source color (@f$ RGB = (R_{s0}, G_{s0}, B_{s0}); A = A_{s0} @f$) */
            SourceColor = GL_SRC_COLOR,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Second source color (@f$ RGB = (R_{s1}, G_{s1}, B_{s1}); A = A_{s1} @f$)
             *
             * @see @ref AbstractShaderProgram::bindFragmentDataLocationIndexed()
             * @requires_gl33 Extension @extension{ARB,blend_func_extended}
             * @requires_gl Multiple blending inputs are not available in
             *      OpenGL ES and WebGL.
             */
            SecondSourceColor = GL_SRC1_COLOR,
            #endif

            /**
             * One minus source color (@f$ RGB = (1.0 - R_{s0}, 1.0 - G_{s0}, 1.0 - B_{s0}); A = 1.0 - A_{s0} @f$)
             */
            OneMinusSourceColor = GL_ONE_MINUS_SRC_COLOR,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * One minus second source color (@f$ RGB = (1.0 - R_{s1}, 1.0 - G_{s1}, 1.0 - B_{s1}); A = 1.0 - A_{s1} @f$)
             *
             * @see @ref AbstractShaderProgram::bindFragmentDataLocationIndexed()
             * @requires_gl33 Extension @extension{ARB,blend_func_extended}
             * @requires_gl Multiple blending inputs are not available in
             *      OpenGL ES and WebGL.
             */
            OneMinusSecondSourceColor = GL_ONE_MINUS_SRC1_COLOR,
            #endif

            /** Source alpha (@f$ RGB = (A_{s0}, A_{s0}, A_{s0}); A = A_{s0} @f$) */
            SourceAlpha = GL_SRC_ALPHA,

            /**
             * Saturate source alpha (@f$ RGB = (f, f, f); A = 1.0; f = min(A_s, 1.0 - A_d) @f$)
             *
             * Can be used only in source parameter of @ref setBlendFunction().
             */
            SourceAlphaSaturate = GL_SRC_ALPHA_SATURATE,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Second source alpha (@f$ RGB = (A_{s1}, A_{s1}, A_{s1}); A = A_{s1} @f$)
             *
             * @see @ref AbstractShaderProgram::bindFragmentDataLocationIndexed()
             * @requires_gl33 Extension @extension{ARB,blend_func_extended}
             * @requires_gl Multiple blending inputs are not available in
             *      OpenGL ES and WebGL.
             */
            SecondSourceAlpha = GL_SRC1_ALPHA,
            #endif

            /**
             * One minus source alpha (@f$ RGB = (1.0 - A_{s0}, 1.0 - A_{s0}, 1.0 - A_{s0}); A = 1.0 - A_{s0} @f$)
             */
            OneMinusSourceAlpha = GL_ONE_MINUS_SRC_ALPHA,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * One minus second source alpha (@f$ RGB = (1.0 - A_{s1}, 1.0 - A_{s1}, 1.0 - A_{s1}); A = 1.0 - A_{s1} @f$)
             *
             * @see @ref AbstractShaderProgram::bindFragmentDataLocationIndexed()
             * @requires_gl33 Extension @extension{ARB,blend_func_extended}
             * @requires_gl Multiple blending inputs are not available in
             *      OpenGL ES and WebGL.
             */
            OneMinusSecondSourceAlpha = GL_ONE_MINUS_SRC1_ALPHA,
            #endif

            /** Destination color (@f$ RGB = (R_d, G_d, B_d); A = A_d @f$) */
            DestinationColor = GL_DST_COLOR,

            /**
             * One minus source color (@f$ RGB = (1.0 - R_d, 1.0 - G_d, 1.0 - B_d); A = 1.0 - A_d @f$)
             */
            OneMinusDestinationColor = GL_ONE_MINUS_DST_COLOR,

            /** Destination alpha (@f$ RGB = (A_d, A_d, A_d); A = A_d @f$) */
            DestinationAlpha = GL_DST_ALPHA,

            /**
             * One minus source alpha (@f$ RGB = (1.0 - A_d, 1.0 - A_d, 1.0 - A_d); A = 1.0 - A_d @f$)
             */
            OneMinusDestinationAlpha = GL_ONE_MINUS_DST_ALPHA
        };

        /**
         * @brief Set blend equation
         *
         * How to combine source color (pixel value) with destination color
         * (framebuffer). Initial value is @ref BlendEquation::Add.
         * @see @ref Feature::Blending, @ref setBlendEquation(BlendEquation, BlendEquation),
         *      @ref setBlendFunction(), @ref setBlendColor(),
         *      @fn_gl{BlendEquation}
         */
        static void setBlendEquation(BlendEquation equation);

        /**
         * @brief Set blend equation separately for RGB and alpha components
         *
         * See @ref setBlendEquation(BlendEquation) for more information.
         * @see @ref Feature::Blending, @ref setBlendFunction(),
         *      @ref setBlendColor(), @fn_gl{BlendEquationSeparate}
         */
        static void setBlendEquation(BlendEquation rgb, BlendEquation alpha);

        /**
         * @brief Set blend function
         * @param source        How the source blending factor is computed
         *      from pixel value. Initial value is @ref BlendFunction::One.
         * @param destination   How the destination blending factor is
         *      computed from framebuffer. Initial value is @ref BlendFunction::Zero.
         *
         * @see @ref Feature::Blending, @ref setBlendFunction(BlendFunction, BlendFunction, BlendFunction, BlendFunction),
         *      @ref setBlendEquation(), @ref setBlendColor(),
         *      @fn_gl{BlendFunc}
         * @requires_gles In WebGL, constant color and constant alpha cannot be
         *      used together as source and destination factors.
         */
        static void setBlendFunction(BlendFunction source, BlendFunction destination);

        /**
         * @brief Set blend function separately for RGB and alpha components
         *
         * See @ref setBlendFunction(BlendFunction, BlendFunction) for more
         * information.
         * @see @ref Feature::Blending, @ref setBlendEquation(),
         *      @ref setBlendColor(), @fn_gl{BlendFuncSeparate}
         */
        static void setBlendFunction(BlendFunction sourceRgb, BlendFunction destinationRgb, BlendFunction sourceAlpha, BlendFunction destinationAlpha);

        /**
         * @brief Set blend color
         *
         * Sets constant color used in @ref setBlendFunction() by
         * @ref BlendFunction::ConstantColor, @ref BlendFunction::OneMinusConstantColor,
         * @ref BlendFunction::ConstantAlpha and @ref BlendFunction::OneMinusConstantAlpha.
         * @see @ref Feature::Blending, @ref setBlendEquation(),
         *      @ref setBlendFunction(), @fn_gl{BlendColor}
         */
        static void setBlendColor(const Color4& color);

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Blend barrier
         *
         * Specifies a boundary between passes when using advanced blend
         * equations to ensure that each sample in the framebuffer is not
         * touched more than once.
         *
         * This is needed either when desktop/ES extension
         * @extension2{KHR,blend_equation_advanced_coherent,blend_equation_advanced}
         * is not available or when @extension2{KHR,blend_equation_advanced_coherent,blend_equation_advanced}
         * is available and @ref Feature::BlendAdvancedCoherent is turned off.
         * @see @ref BlendEquation, @fn_gl_extension{BlendBarrier,KHR,blend_equation_advanced}
         * @requires_extension Extension @extension{KHR,blend_equation_advanced}
         * @requires_es_extension Extension @extension2{KHR,blend_equation_advanced,blend_equation_advanced}
         * @requires_gles Advanced blend equations are not available in WebGL.
         */
        static void blendBarrier() { glBlendBarrierKHR(); }
        #endif

        /*@}*/

        #ifndef MAGNUM_TARGET_GLES
        /** @{ @name Logical operation */

        /**
         * @brief Logical operation
         *
         * @see @ref setLogicOperation()
         * @requires_gl Logical operations on framebuffer are not available in
         *      OpenGL ES and WebGL.
         */
        enum class LogicOperation: GLenum {
            Clear = GL_CLEAR,               /**< `0` */
            Set = GL_SET,                   /**< `1` */
            Copy = GL_COPY,                 /**< `source` */
            CopyInverted = GL_COPY_INVERTED,/**< `~source` */
            Noop = GL_NOOP,                 /**< `destination` */
            Invert = GL_INVERT,             /**< `~destination` */
            And = GL_AND,                   /**< `source & destination` */
            AndReverse = GL_AND_REVERSE,    /**< `source & ~destination` */
            AndInverted = GL_AND_INVERTED,  /**< `~source & destination` */
            Nand = GL_NAND,                 /**< `~(source & destination)` */
            Or = GL_OR,                     /**< `source | destination` */
            OrReverse = GL_OR_REVERSE,      /**< `source | ~destination` */
            OrInverted = GL_OR_INVERTED,    /**< `~source | destination` */
            Nor = GL_NOR,                   /**< `~(source | destination)` */
            Xor = GL_XOR,                   /**< `source ^ destination` */
            Equivalence = GL_EQUIV          /**< `~(source ^ destination)` */
        };

        /**
         * @brief Set logical operation
         *
         * @see @ref Feature::LogicOperation, @fn_gl{LogicOp}
         * @requires_gl Logical operations on framebuffer are not available in
         *      OpenGL ES and WebGL.
         */
        static void setLogicOperation(LogicOperation operation);

        /*@}*/
        #endif

        /** @{ @name Renderer synchronization */

        /**
         * @brief Flush the pipeline
         *
         * @see @ref finish(), @fn_gl{Flush}
         */
        static void flush() { glFlush(); }

        /**
         * @brief Finish the pipeline
         *
         * Blocks until all commands in the pipeline are finished.
         * @see @ref flush(), @fn_gl{Finish}
         */
        static void finish() { glFinish(); }

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Memory barrier
         *
         * @see @ref MemoryBarriers, @ref setMemoryBarrier(),
         *      @ref setMemoryBarrierByRegion()
         * @requires_gl42 Extension @extension{ARB,shader_image_load_store}
         * @requires_gles31 Shader image load/store is not available in OpenGL
         *      ES 3.0 and older.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        enum class MemoryBarrier: GLbitfield {
            /** Vertex data */
            VertexAttributeArray = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,

            /** Vertex indices */
            ElementArray = GL_ELEMENT_ARRAY_BARRIER_BIT,

            /** Uniforms */
            Uniform = GL_UNIFORM_BARRIER_BIT,

            /** Texture fetches */
            TextureFetch = GL_TEXTURE_FETCH_BARRIER_BIT,

            /** Shader image access */
            ShaderImageAccess = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,

            /** Indirect command data */
            Command = GL_COMMAND_BARRIER_BIT,

            /** Pixel buffer data */
            PixelBuffer = GL_PIXEL_BUFFER_BARRIER_BIT,

            /** Texture updates */
            TextureUpdate = GL_TEXTURE_UPDATE_BARRIER_BIT,

            /** Buffer updates */
            BufferUpdate = GL_BUFFER_UPDATE_BARRIER_BIT,

            /** Framebuffer operations */
            Framebuffer = GL_FRAMEBUFFER_BARRIER_BIT,

            /** Transform feedback data */
            TransformFeedback = GL_TRANSFORM_FEEDBACK_BARRIER_BIT,

            /** Atomic counters */
            AtomicCounter = GL_ATOMIC_COUNTER_BARRIER_BIT,

            /**
             * Shader storage data
             * @requires_gl43 Extension @extension{ARB,shader_storage_buffer_object}
             * @requires_gles31 Shader storage is not available in OpenGL ES
             *      3.0 and older.
             * @requires_gles Shader storage is not available in WebGL.
             */
            ShaderStorage = GL_ATOMIC_COUNTER_BARRIER_BIT
        };

        /**
         * @brief Memory barriers
         *
         * @see @ref setMemoryBarrier(), @ref setMemoryBarrierByRegion()
         * @requires_gl42 Extension @extension{ARB,shader_image_load_store}
         * @requires_gles31 Shader image load/store is not available in OpenGL
         *      ES 3.0 and older.
         * @requires_gles Shader image load/store is not available in WebGL.
         */
        typedef Containers::EnumSet<MemoryBarrier
            #ifndef DOXYGEN_GENERATING_OUTPUT
            , GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT|GL_ELEMENT_ARRAY_BARRIER_BIT|GL_UNIFORM_BARRIER_BIT|GL_TEXTURE_FETCH_BARRIER_BIT|GL_SHADER_IMAGE_ACCESS_BARRIER_BIT|GL_COMMAND_BARRIER_BIT|GL_PIXEL_BUFFER_BARRIER_BIT|GL_TEXTURE_UPDATE_BARRIER_BIT|GL_BUFFER_UPDATE_BARRIER_BIT|GL_FRAMEBUFFER_BARRIER_BIT|GL_TRANSFORM_FEEDBACK_BARRIER_BIT|GL_ATOMIC_COUNTER_BARRIER_BIT|GL_ATOMIC_COUNTER_BARRIER_BIT
            #endif
            > MemoryBarriers;

        /**
         * @brief Set memory barrier
         *
         * Calling the function ensures that operations on particular data
         * after the barrier will reflect all data modifications before the
         * barrier.
         * @see @ref setMemoryBarrierByRegion(), @fn_gl{MemoryBarrier}
         * @requires_gl42 Extension @extension{ARB,shader_image_load_store}
         * @requires_gles31 Shader load/store is not available in OpenGL ES 3.0 and older.
         * @requires_gles Shader load/store is not available in WebGL.
         */
        static void setMemoryBarrier(MemoryBarriers barriers) {
            glMemoryBarrier(GLbitfield(barriers));
        }

        /**
         * @brief Set memory barrier by region
         *
         * Behaves as @ref setMemoryBarrier(), except that the region is
         * narrowed around area affected by particular fragment shader, thus
         * only the fragment shader-related barries are supported:
         *
         * - @ref MemoryBarrier::AtomicCounter
         * - @ref MemoryBarrier::Framebuffer
         * - @ref MemoryBarrier::ShaderImageAccess
         * - @ref MemoryBarrier::ShaderStorage
         * - @ref MemoryBarrier::TextureFetch
         * - @ref MemoryBarrier::Uniform
         *
         * @see @fn_gl{MemoryBarrierByRegion}
         * @requires_gl45 Extension @extension{ARB,ES3_1_compatibility}
         * @requires_gles31 Shader load/store is not available in OpenGL ES 3.0
         *      and older.
         * @requires_gles Shader load/store is not available in WebGL.
         */
        static void setMemoryBarrierByRegion(MemoryBarriers barriers) {
            glMemoryBarrierByRegion(GLbitfield(barriers));
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set texture barrier
         *
         * Calling the function ensures that texel fetches in drawing
         * operations after the barrier will reflect texel writes before the
         * barrier.
         * @see @fn_gl{TextureBarrier}
         * @requires_gl45 Extension @extension{ARB,texture_barrier}
         * @requires_gl Texture barrier is not available in OpenGL ES or WebGL.
         */
        static void setTextureBarrier() {
            glTextureBarrier();
        }
        #endif

        /*@}*/

        /** @{ @name Renderer management */

        /**
         * @brief Error status
         *
         * @see @ref error()
         */
        enum class Error: GLenum {
            /** No error has been recorded */
            NoError = GL_NO_ERROR,

            /** An unacceptable value specified for enumerated argument */
            InvalidEnum = GL_INVALID_ENUM,

            /** A numeric argument is out of range */
            InvalidValue = GL_INVALID_VALUE,

            /** The specified operation is not allowed in the current state */
            InvalidOperation = GL_INVALID_OPERATION,

            /**
             * The framebuffer object is not complete.
             * @see AbstractFramebuffer::checkStatus()
             * @requires_gl30 Extension @extension{ARB,framebuffer_object}
             */
            InvalidFramebufferOperation = GL_INVALID_FRAMEBUFFER_OPERATION,

            /** There is not enough memory left to execute the command. */
            OutOfMemory = GL_OUT_OF_MEMORY,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Given operation would cause an internal stack to underflow.
             * @see @ref DebugGroup
             * @requires_gl43 Extension @extension{KHR,debug}
             * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
             *      @extension2{KHR,debug,debug}
             * @requires_gles Debug output is not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            StackUnderflow = GL_STACK_UNDERFLOW,
            #else
            StackUnderflow = GL_STACK_UNDERFLOW_KHR,
            #endif

            /**
             * Given operation would cause an internal stack to overflow.
             * @see @ref DebugGroup
             * @requires_gl43 Extension @extension{KHR,debug}
             * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
             *      @extension2{KHR,debug,debug}
             * @requires_gles Debug output is not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES
            StackOverflow = GL_STACK_OVERFLOW
            #else
            StackOverflow = GL_STACK_OVERFLOW_KHR
            #endif
            #endif
        };

        /**
         * @brief Error status
         *
         * Returns error flag, if any set. If there aren't any more error
         * flags, returns @ref Error::NoError. Thus this function should be
         * always called in a loop until it returns @ref Error::NoError.
         * @see @fn_gl{GetError}
         */
        static Error error() { return static_cast<Error>(glGetError()); }

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Graphics reset notification strategy
         *
         * @see @ref resetNotificationStrategy()
         * @requires_gles Graphics reset notification is not available in
         *      WebGL.
         */
        enum class ResetNotificationStrategy: GLint {
            /**
             * No reset notification, thus @ref graphicsResetStatus() will
             * always return @ref GraphicsResetStatus::NoError. However this
             * doesn't mean that the context cannot be lost.
             */
            #ifndef MAGNUM_TARGET_GLES
            NoResetNotification = GL_NO_RESET_NOTIFICATION_ARB,
            #else
            NoResetNotification = GL_NO_RESET_NOTIFICATION_EXT,
            #endif

            /**
             * Graphics reset will result in context loss, cause of the reset
             * can be queried with @ref graphicsResetStatus().
             * @requires_extension Extension @extension{ARB,robustness}
             * @requires_es_extension Extension @extension{EXT,robustness}
             */
            #ifndef MAGNUM_TARGET_GLES
            LoseContextOnReset = GL_LOSE_CONTEXT_ON_RESET_ARB
            #else
            LoseContextOnReset = GL_LOSE_CONTEXT_ON_RESET_EXT
            #endif
        };

        /**
         * @brief Graphics reset notification strategy
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If OpenGL extension @extension{ARB,robustness} or ES
         * extension @extension{EXT,robustness} is not available, this
         * function always returns @ref ResetNotificationStrategy::NoResetNotification.
         *
         * For the reset notification to work, additionally to the extension
         * support the context must be created with
         * @ref Platform::Sdl2Application::Configuration::Flag::RobustAccess "Platform::*Application::Configuration::Flag::RobustAccess"
         * flag.
         *
         * @see @ref graphicsResetStatus(), @fn_gl{Get} with
         *      @def_gl{RESET_NOTIFICATION_STRATEGY_ARB}
         * @requires_gles Graphics reset notification is not available in
         *      WebGL.
         */
        static ResetNotificationStrategy resetNotificationStrategy();

        /**
         * @brief Graphics reset status
         *
         * @see @ref resetNotificationStrategy(), @ref graphicsResetStatus()
         * @requires_gles Graphics reset notification is not available in
         *      WebGL.
         */
        enum class GraphicsResetStatus: GLenum {
            /** No reset occured since last call. */
            NoError = GL_NO_ERROR,

            /**
             * Reset attributable to the current context has been detected.
             * @requires_extension Extension @extension{ARB,robustness}
             * @requires_es_extension Extension @extension{EXT,robustness}
             */
            #ifndef MAGNUM_TARGET_GLES
            GuiltyContextReset = GL_GUILTY_CONTEXT_RESET_ARB,
            #else
            GuiltyContextReset = GL_GUILTY_CONTEXT_RESET_EXT,
            #endif

            /**
             * Reset not attributable to the current context has been detected.
             * @requires_extension Extension @extension{ARB,robustness}
             * @requires_es_extension Extension @extension{EXT,robustness}
             */
            #ifndef MAGNUM_TARGET_GLES
            InnocentContextReset = GL_INNOCENT_CONTEXT_RESET_ARB,
            #else
            InnocentContextReset = GL_INNOCENT_CONTEXT_RESET_EXT,
            #endif

            /**
             * Reset with unknown cause has been detected.
             * @requires_extension Extension @extension{ARB,robustness}
             * @requires_es_extension Extension @extension{EXT,robustness}
             */
            #ifndef MAGNUM_TARGET_GLES
            UnknownContextReset = GL_UNKNOWN_CONTEXT_RESET_ARB
            #else
            UnknownContextReset = GL_UNKNOWN_CONTEXT_RESET_EXT
            #endif
        };

        /**
         * @brief Check graphics reset status
         *
         * Reset causes all context state to be lost. If OpenGL extension
         * @extension{ARB,robustness} or ES extension @extension{EXT,robustness}
         * is not available, this function always returns @ref GraphicsResetStatus::NoError.
         *
         * For the reset notification to work, additionally to the extension
         * support the context must be created with
         * @ref Platform::Sdl2Application::Configuration::Flag::RobustAccess "Platform::*Application::Configuration::Flag::RobustAccess"
         * flag.
         *
         * If the reset occurs, @extension{ARB,robustness_isolation}
         * extension is supported and context is created with
         * @ref Platform::Sdl2Application::Configuration::Flag::ResetIsolation "Platform::*Application::Configuration::Flag::ResetIsolation",
         * advertised support for @extension{ARB,robustness_application_isolation}
         * indicates that no other application on the system will be affected
         * by the graphics reset. Advertised support for
         * @extension{ARB,robustness_share_group_isolation} indicates that no
         * other share group will be affected by the graphics reset.
         * @see @ref resetNotificationStrategy(),
         *      @fn_gl_extension{GetGraphicsResetStatus,ARB,robustness}
         * @requires_gles Graphics reset notification is not available in
         *      WebGL.
         */
        static GraphicsResetStatus graphicsResetStatus();
        #endif

        /*@}*/

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality();

        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL clearDepthfImplementationDefault(GLfloat depth);
        #endif
        static void MAGNUM_LOCAL clearDepthfImplementationES(GLfloat depth);

        #ifndef MAGNUM_TARGET_WEBGL
        static GraphicsResetStatus MAGNUM_LOCAL graphicsResetStatusImplementationDefault();
        static GraphicsResetStatus MAGNUM_LOCAL graphicsResetStatusImplementationRobustness();
        #endif
};

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
CORRADE_ENUMSET_OPERATORS(Renderer::MemoryBarriers)
#endif

/** @debugoperatorclassenum{Magnum::Renderer,Magnum::Renderer::Error} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Renderer::Error value);

#ifndef MAGNUM_TARGET_WEBGL
/** @debugoperatorclassenum{Magnum::Renderer,Magnum::Renderer::ResetNotificationStrategy} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Renderer::ResetNotificationStrategy value);

/** @debugoperatorclassenum{Magnum::Renderer,Magnum::Renderer::GraphicsResetStatus} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Renderer::GraphicsResetStatus value);
#endif

}

#endif
