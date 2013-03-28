#ifndef Magnum_Renderer_h
#define Magnum_Renderer_h
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
 * @brief Class Magnum::Renderer
 */

#include <Containers/EnumSet.h>

#include "AbstractImage.h"
#include "Buffer.h"
#include "CubeMapTexture.h"
#include "Color.h"
#include "Renderbuffer.h"

namespace Magnum {

/** @nosubgrouping
@brief %Renderer

Access to global renderer configuration.
@todo @extension{ARB,viewport_array}
*/
class MAGNUM_EXPORT Renderer {
    public:
        Renderer() = delete;

        /**
         * @brief Affected polygon facing for culling, stencil operations and masks
         *
         * @see setFaceCullingMode(),
         *      setStencilFunction(PolygonFacing, StencilFunction, Int, UnsignedInt),
         *      setStencilOperation(PolygonFacing, StencilOperation, StencilOperation, StencilOperation),
         *      setStencilMask(PolygonFacing, UnsignedInt)
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
         * If not specified otherwise, all features are disabled by default.
         * @see setFeature()
         */
        enum class Feature: GLenum {
            /**
             * Blending
             * @see setBlendEquation(), setBlendFunction(), setBlendColor()
             */
            Blending = GL_BLEND,

            DepthTest = GL_DEPTH_TEST,      /**< Depth test */
            Dithering = GL_DITHER,          /**< Dithering (enabled by default) */

            FaceCulling = GL_CULL_FACE,     /**< Back face culling */

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Logical operation
             * @see setLogicOperation()
             * @requires_gl Logical operations on framebuffer are not
             *      available in OpenGL ES.
             */
            LogicOperation = GL_COLOR_LOGIC_OP,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Multisampling (enabled by default)
             * @requires_gl Always enabled in OpenGL ES.
             */
            Multisampling = GL_MULTISAMPLE,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Depth clamping. If enabled, ignores near and far clipping plane.
             * @requires_gl32 %Extension @extension{ARB,depth_clamp}
             * @requires_gl Depth clamping is not available in OpenGL ES.
             */
            DepthClamp = GL_DEPTH_CLAMP,
            #endif

            /**
             * Scissor test
             * @see setScissor()
             */
            ScissorTest = GL_SCISSOR_TEST,

            StencilTest = GL_STENCIL_TEST   /**< Stencil test */
        };

        /**
         * @brief Set feature
         *
         * @see @fn_gl{Enable}/@fn_gl{Disable}
         */
        inline static void setFeature(Feature feature, bool enabled) {
            enabled ? glEnable(static_cast<GLenum>(feature)) : glDisable(static_cast<GLenum>(feature));
        }

        /**
         * @brief Which polygon facing to cull
         *
         * Initial value is `PolygonFacing::Back`. If set to both front and
         * back, only points and lines are drawn.
         * @attention You have to also enable face culling with setFeature().
         * @see @fn_gl{CullFace}
         */
        inline static void setFaceCullingMode(PolygonFacing mode) {
            glCullFace(static_cast<GLenum>(mode));
        }

        /*@}*/

        /** @{ @name Clearing values */

        /**
         * @brief Set clear color
         *
         * Initial value is `{0.0f, 0.0f, 0.0f, 1.0f}`.
         * @see @fn_gl{ClearColor}
         */
        inline static void setClearColor(const Color4<>& color) {
            glClearColor(color.r(), color.g(), color.b(), color.a());
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set clear depth
         *
         * Initial value is `1.0`.
         * @see @fn_gl{ClearDepth}
         * @requires_gl See setClearDepth(Float), which is available in OpenGL ES.
         */
        inline static void setClearDepth(Double depth) { glClearDepth(depth); }
        #endif

        /**
         * @overload
         *
         * @see @fn_gl{ClearDepth}
         * @requires_gl41 %Extension @extension{ARB,ES2_compatibility}
         * @todo Call double version if the extension is not available
         */
        inline static void setClearDepth(Float depth) { glClearDepthf(depth); }

        /**
         * @brief Set clear stencil
         *
         * Initial value is `0`.
         * @see @fn_gl{ClearStencil}
         */
        inline static void setClearStencil(Int stencil) { glClearStencil(stencil); }

        /*@}*/

        /** @{ @name Scissor operations */

        /**
         * @brief Set scissor rectangle
         * @param bottomLeft    Bottom left corner. Initial value is `(0, 0)`.
         * @param size          Scissor rectangle size. Initial value is
         *      size of the window when the context is first attached to a
         *      window.
         *
         * @attention You have to enable scissoring with setFeature() first.
         * @see @fn_gl{Scissor}
         */
        inline static void setScissor(const Vector2i& bottomLeft, const Vector2i& size) {
            glScissor(bottomLeft.x(), bottomLeft.y(), size.x(), size.y());
        }

        /*@}*/

        /** @{ @name Stencil operations */

        /**
         * @brief Stencil function
         *
         * @see setStencilFunction()
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
         * @see setStencilOperation()
         */
        enum class StencilOperation: GLenum {
            Keep = GL_KEEP, /**< Keep the current value. */
            Zero = GL_ZERO, /**< Set the stencil buffer value to `0`. */

            /**
             * Set the stencil value to reference value specified by
             * setStencilFunction().
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
         *      `StencilFunction::Always`.
         * @param referenceValue    Reference value. Initial value is `0`.
         * @param mask              Mask for both reference and buffer value.
         *      Initial value is all `1`s.
         *
         * @attention You have to enable stencil test with setFeature() first.
         * @see setStencilFunction(StencilFunction, Int, UnsignedInt),
         *      @fn_gl{StencilFuncSeparate}
         */
        inline static void setStencilFunction(PolygonFacing facing, StencilFunction function, Int referenceValue, UnsignedInt mask) {
            glStencilFuncSeparate(static_cast<GLenum>(facing), static_cast<GLenum>(function), referenceValue, mask);
        }

        /**
         * @brief Set stencil function
         *
         * The same as setStencilFunction(PolygonFacing, StencilFunction, Int, UnsignedInt)
         * with `facing` set to `PolygonFacing::FrontAndBack`.
         * @see @fn_gl{StencilFunc}
         */
        inline static void setStencilFunction(StencilFunction function, Int referenceValue, UnsignedInt mask) {
            glStencilFunc(static_cast<GLenum>(function), referenceValue, mask);
        }

        /**
         * @brief Set stencil operation
         * @param facing            Affected polygon facing
         * @param stencilFail       Action when stencil test fails
         * @param depthFail         Action when stencil test passes, but depth
         *      test fails
         * @param depthPass         Action when both stencil and depth test
         *      pass
         *
         * Initial value for all fields is `StencilOperation::Keep`.
         * @attention You have to enable stencil test with setFeature() first.
         * @see setStencilOperation(StencilOperation, StencilOperation, StencilOperation),
         *      @fn_gl{StencilOpSeparate}
         */
        inline static void setStencilOperation(PolygonFacing facing, StencilOperation stencilFail, StencilOperation depthFail, StencilOperation depthPass) {
            glStencilOpSeparate(static_cast<GLenum>(facing), static_cast<GLenum>(stencilFail), static_cast<GLenum>(depthFail), static_cast<GLenum>(depthPass));
        }

        /**
         * @brief Set stencil operation
         *
         * The same as setStencilOperation(PolygonFacing, StencilOperation, StencilOperation, StencilOperation)
         * with `facing` set to `PolygonFacing::FrontAndBack`.
         * @see @fn_gl{StencilOp}
         */
        inline static void setStencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation depthPass) {
            glStencilOp(static_cast<GLenum>(stencilFail), static_cast<GLenum>(depthFail), static_cast<GLenum>(depthPass));
        }

        /*@}*/

        /** @{ @name Depth testing */

        /**
         * @brief Depth function
         *
         * @see setDepthFunction()
         */
        typedef StencilFunction DepthFunction;

        /**
         * @brief Set depth function
         *
         * Initial value is `DepthFunction::Less`.
         * @attention You have to enable depth test with setFeature() first.
         * @see @fn_gl{DepthFunc}
         */
        inline static void setDepthFunction(DepthFunction function) {
            glDepthFunc(static_cast<GLenum>(function));
        }

        /*@}*/

        /** @{ @name Masking writes */

        /**
         * @brief Mask color writes
         *
         * Set to `false` to disallow writing to given color channel. Initial
         * values are all `true`.
         * @see @fn_gl{ColorMask}
         * @todo Masking only given draw buffer
         */
        inline static void setColorMask(GLboolean allowRed, GLboolean allowGreen, GLboolean allowBlue, GLboolean allowAlpha) {
            glColorMask(allowRed, allowGreen, allowBlue, allowAlpha);
        }

        /**
         * @brief Mask depth writes
         *
         * Set to `false` to disallow writing to depth buffer. Initial value
         * is `true`.
         * @see @fn_gl{DepthMask}
         */
        inline static void setDepthMask(GLboolean allow) {
            glDepthMask(allow);
        }

        /**
         * @brief Mask stencil writes
         *
         * Set given bit to `0` to disallow writing stencil value for given
         * faces to it. Initial value is all `1`s.
         * @see setStencilMask(UnsignedInt), @fn_gl{StencilMaskSeparate}
         */
        inline static void setStencilMask(PolygonFacing facing, UnsignedInt allowBits) {
            glStencilMaskSeparate(static_cast<GLenum>(facing), allowBits);
        }

        /**
         * @brief Mask stencil writes
         *
         * The same as setStencilMask(PolygonFacing, UnsignedInt) with `facing` set
         * to `PolygonFacing::FrontAndBack`.
         * @see @fn_gl{StencilMask}
         */
        inline static void setStencilMask(UnsignedInt allowBits) {
            glStencilMask(allowBits);
        }

        /*@}*/

        /**
         * @{ @name Blending
         *
         * You have to enable blending with setFeature() first.
         * @todo Blending for given draw buffer
         */

        /**
         * @brief Blend equation
         *
         * @see setBlendEquation()
         */
        enum class BlendEquation: GLenum {
            Add = GL_FUNC_ADD,                          /**< `source + destination` */
            Subtract = GL_FUNC_SUBTRACT,                /**< `source - destination` */
            ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT  /**< `destination - source` */

            #ifndef MAGNUM_TARGET_GLES2
            ,
            /**
             * `min(source, destination)`
             * @requires_gles30 %Extension @es_extension2{EXT,blend_minmax,blend_minmax}
             */
            Min = GL_MIN,

            /**
             * `max(source, destination)`
             * @requires_gles30 %Extension @es_extension2{EXT,blend_minmax,blend_minmax}
             */
            Max = GL_MAX
            #endif
        };

        /**
         * @brief Blend function
         *
         * @see setBlendFunction()
         */
        enum class BlendFunction: GLenum {
            /** Zero (@f$ RGB = (0.0, 0.0, 0.0); A = 0.0 @f$) */
            Zero = GL_ZERO,

            /** One (@f$ RGB = (1.0, 1.0, 1.0); A = 1.0 @f$) */
            One = GL_ONE,

            /**
             * Constant color (@f$ RGB = (R_c, G_c, B_c); A = A_c @f$)
             *
             * @see setBlendColor()
             */
            ConstantColor = GL_CONSTANT_COLOR,

            /**
             * One minus constant color (@f$ RGB = (1.0 - R_c, 1.0 - G_c, 1.0 - B_c); A = 1.0 - A_c @f$)
             *
             * @see setBlendColor()
             */
            OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,

            /**
             * Constant alpha (@f$ RGB = (A_c, A_c, A_c); A = A_c @f$)
             *
             * @see setBlendColor()
             */
            ConstantAlpha = GL_CONSTANT_ALPHA,

            /**
             * One minus constant alpha (@f$ RGB = (1.0 - A_c, 1.0 - A_c, 1.0 - A_c); A = 1.0 - A_c @f$)
             *
             * @see setBlendColor()
             */
            OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,

            /** Source color (@f$ RGB = (R_{s0}, G_{s0}, B_{s0}); A = A_{s0} @f$) */
            SourceColor = GL_SRC_COLOR,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Second source color (@f$ RGB = (R_{s1}, G_{s1}, B_{s1}); A = A_{s1} @f$)
             *
             * @see AbstractShaderProgram::bindFragmentDataLocationIndexed()
             * @requires_gl33 %Extension @extension{ARB,blend_func_extended}
             * @requires_gl Multiple blending inputs are not available in
             *      OpenGL ES.
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
             * @see AbstractShaderProgram::bindFragmentDataLocationIndexed()
             * @requires_gl33 %Extension @extension{ARB,blend_func_extended}
             * @requires_gl Multiple blending inputs are not available in
             *      OpenGL ES.
             */
            OneMinusSecondSourceColor = GL_ONE_MINUS_SRC1_COLOR,
            #endif

            /** Source alpha (@f$ RGB = (A_{s0}, A_{s0}, A_{s0}); A = A_{s0} @f$) */
            SourceAlpha = GL_SRC_ALPHA,

            /**
             * Saturate source alpha (@f$ RGB = (f, f, f); A = 1.0; f = min(A_s, 1.0 - A_d) @f$)
             *
             * Can be used only in source parameter of setBlendFunction().
             */
            SourceAlphaSaturate = GL_SRC_ALPHA_SATURATE,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Second source alpha (@f$ RGB = (A_{s1}, A_{s1}, A_{s1}); A = A_{s1} @f$)
             *
             * @see AbstractShaderProgram::bindFragmentDataLocationIndexed()
             * @requires_gl33 %Extension @extension{ARB,blend_func_extended}
             * @requires_gl Multiple blending inputs are not available in
             *      OpenGL ES.
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
             * @see AbstractShaderProgram::bindFragmentDataLocationIndexed()
             * @requires_gl33 %Extension @extension{ARB,blend_func_extended}
             * @requires_gl Multiple blending inputs are not available in
             *      OpenGL ES.
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
         * (framebuffer). Initial value is `BlendEquation::Add`.
         * @attention You have to enable blending with setFeature() first.
         * @see setBlendEquation(BlendEquation, BlendEquation),
         *      @fn_gl{BlendEquation}
         */
        inline static void setBlendEquation(BlendEquation equation) {
            glBlendEquation(static_cast<GLenum>(equation));
        }

        /**
         * @brief Set blend equation separately for RGB and alpha components
         *
         * See setBlendEquation(BlendEquation) for more information.
         * @attention You have to enable blending with setFeature() first.
         * @see @fn_gl{BlendEquationSeparate}
         */
        inline static void setBlendEquation(BlendEquation rgb, BlendEquation alpha) {
            glBlendEquationSeparate(static_cast<GLenum>(rgb), static_cast<GLenum>(alpha));
        }

        /**
         * @brief Set blend function
         * @param source        How the source blending factor is computed
         *      from pixel value. Initial value is `BlendFunction::One`.
         * @param destination   How the destination blending factor is
         *      computed from framebuffer. Initial value is
         *      `BlendFunction::Zero`.
         *
         * @attention You have to enable blending with setFeature() first.
         * @see setBlendFunction(BlendFunction, BlendFunction, BlendFunction, BlendFunction),
         *      @fn_gl{BlendFunc}
         */
        inline static void setBlendFunction(BlendFunction source, BlendFunction destination) {
            glBlendFunc(static_cast<GLenum>(source), static_cast<GLenum>(destination));
        }

        /**
         * @brief Set blend function separately for RGB and alpha components
         *
         * See setBlendFunction(BlendFunction, BlendFunction) for more information.
         * @attention You have to enable blending with setFeature() first.
         * @see @fn_gl{BlendFuncSeparate}
         */
        inline static void setBlendFunction(BlendFunction sourceRgb, BlendFunction destinationRgb, BlendFunction sourceAlpha, BlendFunction destinationAlpha) {
            glBlendFuncSeparate(static_cast<GLenum>(sourceRgb), static_cast<GLenum>(destinationRgb), static_cast<GLenum>(sourceAlpha), static_cast<GLenum>(destinationAlpha));
        }

        /**
         * @brief Set blend color
         *
         * Sets constant color used in setBlendFunction() by
         * `BlendFunction::ConstantColor`,
         * `BlendFunction::OneMinusConstantColor`,
         * `BlendFunction::ConstantAlpha` and
         * `BlendFunction::OneMinusConstantAlpha`.
         * @attention You have to enable blending with setFeature() first.
         * @see @fn_gl{BlendColor}
         */
        inline static void setBlendColor(const Color4<>& color) {
            glBlendColor(color.r(), color.g(), color.b(), color.a());
        }

        /*@}*/

        #ifndef MAGNUM_TARGET_GLES
        /** @{ @name Logical operation */

        /**
         * @brief Logical operation
         *
         * @see setLogicOperation()
         * @requires_gl Logical operations on framebuffer are not available in
         *      OpenGL ES.
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
         * @attention You have to enable logical operation with setFeature() first.
         * @see @fn_gl{LogicOp}
         * @requires_gl Logical operations on framebuffer are not available in
         *      OpenGL ES.
         */
        inline static void setLogicOperation(LogicOperation operation) {
            glLogicOp(static_cast<GLenum>(operation));
        }

        /*@}*/
        #endif

        /** @{ @name Renderer management */

        /**
         * @brief Flush the pipeline
         *
         * @see finish(), @fn_gl{Flush}
         */
        inline static void flush() {
            glFlush();
        }

        /**
         * @brief Finish the pipeline
         *
         * Blocks until all commands in the pipeline are finished.
         * @see flush(), @fn_gl{Finish}
         */
        inline static void finish() {
            glFinish();
        }

        /*@}*/
};

}

#endif
