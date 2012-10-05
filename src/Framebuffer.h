#ifndef Magnum_Framebuffer_h
#define Magnum_Framebuffer_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Framebuffer
 */

#include <Containers/EnumSet.h>

#include "AbstractImage.h"
#include "Buffer.h"
#include "CubeMapTexture.h"
#include "Color.h"
#include "Renderbuffer.h"

namespace Magnum {

template<std::uint8_t> class BufferedImage;
template<std::uint8_t> class Image;

typedef BufferedImage<1> BufferedImage1D;
typedef BufferedImage<2> BufferedImage2D;
typedef BufferedImage<3> BufferedImage3D;
typedef Image<1> Image1D;
typedef Image<2> Image2D;
typedef Image<3> Image3D;

/** @nosubgrouping
@brief %Framebuffer

Provides operations with framebuffers (configuring, clearing, blitting...) and
creation and attaching of named framebuffers.
@todo @extension{ARB,viewport_array}
*/
class MAGNUM_EXPORT Framebuffer {
    Framebuffer(const Framebuffer& other) = delete;
    Framebuffer(Framebuffer&& other) = delete;
    Framebuffer& operator=(const Framebuffer& other) = delete;
    Framebuffer& operator=(Framebuffer&& other) = delete;

    public:
        /**
         * @brief Affected polygon facing for culling, stencil operations and masks
         *
         * @see setFaceCullingMode(),
         *      setStencilFunction(PolygonFacing, StencilFunction, GLint, GLuint),
         *      setStencilOperation(PolygonFacing, StencilOperation, StencilOperation, StencilOperation),
         *      setStencilMask(PolygonFacing, GLuint)
         */
        enum class PolygonFacing: GLenum {
            Front = GL_FRONT,                   /**< Front-facing polygons */
            Back = GL_BACK,                     /**< Back-facing polygons */
            FrontAndBack = GL_FRONT_AND_BACK    /**< Front- and back-facing polygons */
        };

        /** @{ @name Framebuffer features */

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

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Logical operation
             * @see setLogicOperation()
             * @requires_gl Logic operations on framebuffer are in desktop OpenGL only.
             */
            LogicOperation = GL_COLOR_LOGIC_OP,

            /**
             * Depth clamping. If enabled, ignores near and far clipping plane.
             * @requires_gl
             * @requires_gl32 Extension @extension{ARB,depth_clamp}
             */
            DepthClamp = GL_DEPTH_CLAMP,
            #endif

            /**
             * Scissor test
             * @see setScissor()
             */
            ScissorTest = GL_SCISSOR_TEST,
            DepthTest = GL_DEPTH_TEST,      /**< Depth test */
            StencilTest = GL_STENCIL_TEST,  /**< Stencil test */
            Dithering = GL_DITHER,          /**< Dithering (enabled by default) */
            FaceCulling = GL_CULL_FACE      /**< Back face culling */
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

        /**
         * @brief Set viewport size
         *
         * Call when window size changes.
         * @see @fn_gl{Viewport}
         */
        inline static void setViewport(const Math::Vector2<GLint>& position, const Math::Vector2<GLsizei>& size) {
            glViewport(position.x(), position.y(), size.x(), size.y());
        }

        /*@}*/

        /** @{ @name Clearing the framebuffer */

        /**
         * @brief Mask for clearing
         *
         * @see ClearMask, clear(), clear(ClearMask)
         */
        enum class Clear: GLbitfield {
            Color = GL_COLOR_BUFFER_BIT,    /**< Color */
            Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth value */
            Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil value */
        };

        /** @brief Mask for clearing */
        typedef Corrade::Containers::EnumSet<Clear, GLbitfield,
            GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> ClearMask;

        /**
         * @brief Clear specified buffers in framebuffer
         *
         * @see clear(), setClearColor(), setClearDepth(), setClearStencil(),
         *      @fn_gl{Clear}
         * @todo Clearing only given draw buffer
         */
        inline static void clear(ClearMask mask) { glClear(static_cast<GLbitfield>(mask)); }

        /**
         * @brief Set clear color
         *
         * Initial value is `{0.0f, 0.0f, 0.0f, 1.0f}`.
         * @see @fn_gl{ClearColor}
         */
        inline static void setClearColor(const Color4<GLfloat>& color) {
            glClearColor(color.r(), color.g(), color.b(), color.a());
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Set clear depth
         *
         * Initial value is `1.0`.
         * @see @fn_gl{ClearDepth}
         * @requires_gl See setClearDepth(GLfloat), which is supported in OpenGL ES.
         */
        inline static void setClearDepth(GLdouble depth) { glClearDepth(depth); }
        #endif

        /**
         * @overload
         *
         * @see @fn_gl{ClearDepth}
         * @requires_gl41 Extension @extension{ARB,ES2_compatibility}
         */
        inline static void setClearDepth(GLfloat depth) { glClearDepthf(depth); }

        /**
         * @brief Set clear stencil
         *
         * Initial value is `0`.
         * @see @fn_gl{ClearStencil}
         */
        inline static void setClearStencil(GLint stencil) { glClearStencil(stencil); }

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
        inline static void setScissor(const Math::Vector2<GLint>& bottomLeft, const Math::Vector2<GLsizei>& size) {
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
         * @see setStencilFunction(StencilFunction, GLint, GLuint),
         *      @fn_gl{StencilFuncSeparate}
         */
        inline static void setStencilFunction(PolygonFacing facing, StencilFunction function, GLint referenceValue, GLuint mask) {
            glStencilFuncSeparate(static_cast<GLenum>(facing), static_cast<GLenum>(function), referenceValue, mask);
        }

        /**
         * @brief Set stencil function
         *
         * The same as setStencilFunction(PolygonFacing, StencilFunction, GLint, GLuint)
         * with `facing` set to `PolygonFacing::FrontAndBack`.
         * @see @fn_gl{StencilFunc}
         */
        inline static void setStencilFunction(StencilFunction function, GLint referenceValue, GLuint mask) {
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
         * @see setStencilMask(GLuint), @fn_gl{StencilMaskSeparate}
         */
        inline static void setStencilMask(PolygonFacing facing, GLuint allowBits) {
            glStencilMaskSeparate(static_cast<GLenum>(facing), allowBits);
        }

        /**
         * @brief Mask stencil writes
         *
         * The same as setStencilMask(PolygonFacing, GLuint) with `facing` set
         * to `PolygonFacing::FrontAndBack`.
         * @see @fn_gl{StencilMask}
         */
        inline static void setStencilMask(GLuint allowBits) {
            glStencilMask(allowBits);
        }

        /*@}*/

        /** @{ @name Blending
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

            #ifndef MAGNUM_TARGET_GLES
            /** @todo Enable for ES3 when the headers are available */
            ,

            /**
             * `min(source, destination)`
             * @requires_gles30 Extension @es_extension{EXT,blend_minmax}
             */
            Min = GL_MIN,

            /**
             * `max(source, destination)`
             * @requires_gles30 Extension @es_extension{EXT,blend_minmax}
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
             * @requires_gl
             * @requires_gl33 Extension @extension{ARB,blend_func_extended}
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
             * @requires_gl
             * @requires_gl33 Extension @extension{ARB,blend_func_extended}
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
             * @requires_gl
             * @requires_gl33 Extension @extension{ARB,blend_func_extended}
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
             * @requires_gl
             * @requires_gl33 Extension @extension{ARB,blend_func_extended}
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
        inline static void setBlendColor(const Color4<GLfloat>& color) {
            glBlendColor(color.r(), color.g(), color.b(), color.a());
        }

        /*@}*/

        #ifndef MAGNUM_TARGET_GLES
        /** @{ @name Logical operation */

        /**
         * @brief Logical operation
         *
         * @see setLogicOperation()
         * @requires_gl
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
         * @requires_gl Logic operations on framebuffer are in desktop OpenGL only.
         */
        inline static void setLogicOperation(LogicOperation operation) {
            glLogicOp(static_cast<GLenum>(operation));
        }

        /*@}*/
        #endif

        /** @{ @name Framebuffer creation and binding */

        /**
         * @brief %Framebuffer target
         *
         * @see bind(), bindDefault()
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class Target: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * For reading only.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
             */
            Read = GL_READ_FRAMEBUFFER,

            /**
             * For drawing only.
             * @requires_gl
             * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
             */
            Draw = GL_DRAW_FRAMEBUFFER,
            #endif

            ReadDraw = GL_FRAMEBUFFER       /**< For both reading and drawing. */
        };

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Draw attachment for default framebuffer
         *
         * @see mapDefaultForDraw()
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class DefaultDrawAttachment: GLenum {
            None = GL_NONE,                 /**< Don't use the output. */
            BackLeft = GL_BACK_LEFT,        /**< Write output to back left framebuffer. */
            BackRight = GL_BACK_RIGHT,      /**< Write output to back right framebuffer. */
            FrontLeft = GL_FRONT_LEFT,      /**< Write output to front left framebuffer. */
            FrontRight = GL_FRONT_RIGHT     /**< Write output to front right framebuffer. */
        };

        /**
         * @brief Read attachment for default framebuffer
         *
         * @see mapDefaultForRead()
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class DefaultReadAttachment: GLenum {
            FrontLeft = GL_FRONT_LEFT,      /**< Read from front left framebuffer. */
            FrontRight = GL_FRONT_RIGHT,    /**< Read from front right framebuffer. */
            BackLeft = GL_BACK_LEFT,        /**< Read from back left framebuffer. */
            BackRight = GL_BACK_RIGHT,      /**< Read from back right framebuffer. */
            Left = GL_LEFT,                 /**< Read from left framebuffers. */
            Right = GL_RIGHT,               /**< Read from right framebuffers. */
            Front = GL_FRONT,               /**< Read from front framebuffers. */
            Back = GL_BACK,                 /**< Read from back framebuffers. */
            FrontAndBack = GL_FRONT_AND_BACK /**< Read from front and back framebuffers. */
        };
        #endif

        /**
         * @brief Constructor
         *
         * Generates new OpenGL framebuffer.
         * @see @fn_gl{GenFramebuffers}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline Framebuffer() { glGenFramebuffers(1, &framebuffer); }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL framebuffer.
         * @see @fn_gl{DeleteFramebuffers}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline ~Framebuffer() { glDeleteFramebuffers(1, &framebuffer); }

        /**
         * @brief Bind default framebuffer to given target
         * @param target     %Target
         *
         * @see @fn_gl{BindFramebuffer}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline static void bindDefault(Target target) {
            glBindFramebuffer(static_cast<GLenum>(target), 0);
        }

        /**
         * @brief Bind framebuffer
         *
         * @see @fn_gl{BindFramebuffer}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void bind(Target target) {
            glBindFramebuffer(static_cast<GLenum>(target), framebuffer);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Map given attachments of default framebuffer for drawing
         * @param attachments       Default attachments. If any value is
         *      DefaultAttachment::None, given output is not used.
         *
         * If used for mapping output of fragment shader, the order must be as
         * specified by the shader (see AbstractShaderProgram documentation).
         * If used for blit(), the order is not important. Each used attachment
         * should have either renderbuffer or texture attached for writing to
         * work properly.
         * @see mapForDraw(), mapDefaultForRead(), bindDefault(), @fn_gl{DrawBuffers}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        static void mapDefaultForDraw(std::initializer_list<DefaultDrawAttachment> attachments);

        /**
         * @brief Map given color attachments of current framebuffer for drawing
         * @param colorAttachments  Color attachment IDs. If any value is -1,
         *      given output is not used.
         *
         * If used for mapping output of fragment shader, the order must be as
         * specified by the shader (see AbstractShaderProgram documentation).
         * If used for blit(), the order is not important. Each used attachment
         * should have either renderbuffer or texture attached for writing to
         * work properly.
         * @see mapDefaultForDraw(), mapForRead(), bind(), @fn_gl{DrawBuffers}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        void mapForDraw(std::initializer_list<std::int8_t> colorAttachments);

        /**
         * @brief Map given attachment of default framebuffer for reading
         * @param attachment        Default attachment
         *
         * Each used attachment should have either renderbuffer or texture
         * attached to work properly.
         * @see mapForRead(), mapDefaultForDraw(), bindDefault(), @fn_gl{ReadBuffer}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline static void mapDefaultForRead(DefaultReadAttachment attachment) {
            bindDefault(Target::Read);
            glReadBuffer(static_cast<GLenum>(attachment));
        }

        /**
         * @brief Map given color attachment of current framebuffer for reading
         * @param colorAttachment   Color attachment ID
         *
         * The color attachment should have either renderbuffer or texture
         * attached for reading to work properly.
         * @see mapDefaultForRead(), mapForDraw(), bind(), @fn_gl{ReadBuffer}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void mapForRead(std::uint8_t colorAttachment) {
            bind(Target::Read);
            glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachment);
        }
        #endif

        /*@}*/

        /** @{ @name Attaching textures and renderbuffers */

        /**
         * @brief Attachment for depth/stencil part of fragment shader output
         *
         * @see attachRenderbuffer(Target, DepthStencilAttachment, Renderbuffer*),
         *      attachTexture1D(Target, DepthStencilAttachment, Texture1D*, GLint),
         *      attachTexture2D(Target, DepthStencilAttachment, Texture2D*, GLint),
         *      attachCubeMapTexture(Target, DepthStencilAttachment, CubeMapTexture*, CubeMapTexture::Coordinate, GLint),
         *      attachTexture3D(Target, DepthStencilAttachment, Texture3D*, GLint, GLint)
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class DepthStencilAttachment: GLenum {
            Depth = GL_DEPTH_ATTACHMENT,    /**< Depth output only. */

            Stencil = GL_STENCIL_ATTACHMENT /**< Stencil output only. */

            #ifndef MAGNUM_TARGET_GLES
            ,
            /**
             * Both depth and stencil output.
             */
            DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT
            #endif
        };

        /**
         * @brief Attach renderbuffer to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param renderbuffer      %Renderbuffer
         *
         * @see bind(), @fn_gl{FramebufferRenderbuffer}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachRenderbuffer(Target target, DepthStencilAttachment depthStencilAttachment, Renderbuffer* renderbuffer) {
            /** @todo Check for internal format compatibility */
            bind(target);
            glFramebufferRenderbuffer(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), GL_RENDERBUFFER, renderbuffer->id());
        }

        /**
         * @brief Attach renderbuffer to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param renderbuffer      %Renderbuffer
         *
         * @see bind(), @fn_gl{FramebufferRenderbuffer}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachRenderbuffer(Target target, std::uint8_t colorAttachment, Renderbuffer* renderbuffer) {
            /** @todo Check for internal format compatibility */
            bind(target);
            glFramebufferRenderbuffer(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, GL_RENDERBUFFER, renderbuffer->id());
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Attach 1D texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           1D texture
         * @param mipLevel          Mip level
         *
         * @see bind(), @fn_gl{FramebufferTexture}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture1D(Target target, DepthStencilAttachment depthStencilAttachment, Texture1D* texture, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture1D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
        }

        /**
         * @brief Attach 1D texture to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           1D texture
         * @param mipLevel          Mip level
         *
         * @see bind(), @fn_gl{FramebufferTexture}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture1D(Target target, std::uint8_t colorAttachment, Texture1D* texture, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture1D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
        }
        #endif

        /**
         * @brief Attach 2D texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           2D texture
         * @param mipLevel          Mip level. For rectangle textures it
         *      should be always 0.
         *
         * @see attachCubeMapTexture(), bind(), @fn_gl{FramebufferTexture}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture2D(Target target, DepthStencilAttachment depthStencilAttachment, Texture2D* texture, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
        }

        /**
         * @brief Attach 2D texture to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           2D texture
         * @param mipLevel          Mip level. For rectangle textures it
         *      should be always 0.
         *
         * @see attachCubeMapTexture(), bind(), @fn_gl{FramebufferTexture}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture2D(Target target, std::uint8_t colorAttachment, Texture2D* texture, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture2D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
        }

        /**
         * @brief Attach cube map texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           Cube map texture
         * @param coordinate        Cube map coordinate
         * @param mipLevel          Mip level
         *
         * @see attachTexture2D(), bind(), @fn_gl{FramebufferTexture}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachCubeMapTexture(Target target, DepthStencilAttachment depthStencilAttachment, CubeMapTexture* texture, CubeMapTexture::Coordinate coordinate, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            bind(target);
            glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(coordinate), texture->id(), mipLevel);
        }

        /**
         * @brief Attach cube map texture to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           Cube map texture
         * @param coordinate        Cube map coordinate
         * @param mipLevel          Mip level
         *
         * @see attachTexture2D(), bind(), @fn_gl{FramebufferTexture}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachCubeMapTexture(Target target, std::uint8_t colorAttachment, CubeMapTexture* texture, CubeMapTexture::Coordinate coordinate, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            bind(target);
            glFramebufferTexture2D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(coordinate), texture->id(), mipLevel);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Attach 3D texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           3D texture
         * @param mipLevel          Mip level
         * @param layer             Layer of 2D image within a 3D texture
         *
         * @see bind(), @fn_gl{FramebufferTexture}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture3D(Target target, DepthStencilAttachment depthStencilAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture3D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(texture->target()), texture->id(), mipLevel, layer);
        }

        /**
         * @brief Attach 3D texture to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           3D texture
         * @param mipLevel          Mip level
         * @param layer             Layer of 2D image within a 3D texture.
         *
         * @see bind(), @fn_gl{FramebufferTexture}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline void attachTexture3D(Target target, std::uint8_t colorAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture3D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel, layer);
        }
        #endif

        /*@}*/

        /** @{ @name Framebuffer blitting and reading */

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Output mask for blitting
         *
         * Specifies which data are copied when performing blit operation
         * using blit().
         * @see BlitMask
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class Blit: GLbitfield {
            Color = GL_COLOR_BUFFER_BIT,    /**< Color */
            Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth value */
            Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil value */
        };

        /**
         * @brief Output mask for blitting
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        typedef Corrade::Containers::EnumSet<Blit, GLbitfield,
            GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> BlitMask;

        /**
         * @brief Copy block of pixels from read to draw framebuffer
         * @param bottomLeft        Bottom left coordinates of source rectangle
         * @param topRight          Top right coordinates of source rectangle
         * @param destinationBottomLeft Bottom left coordinates of destination rectangle
         * @param destinationTopRight Top right coordinates of destination
         *      rectangle
         * @param blitMask          Blit mask
         * @param filter            Interpolation applied if the image is
         *      stretched
         *
         * See mapForRead() / mapDefaultForRead() and mapForDraw() /
         * mapDefaultForDraw() for binding particular framebuffer for reading
         * and drawing. If multiple attachments are specified in mapForDraw()
         * / mapDefaultForDraw(), the data are written to each of them.
         * @see @fn_gl{BlitFramebuffer}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
         */
        inline static void blit(const Math::Vector2<GLint>& bottomLeft, const Math::Vector2<GLint>& topRight, const Math::Vector2<GLint>& destinationBottomLeft, const Math::Vector2<GLint>& destinationTopRight, BlitMask blitMask, AbstractTexture::Filter filter) {
            glBlitFramebuffer(bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), destinationBottomLeft.x(), destinationBottomLeft.y(), destinationTopRight.x(), destinationTopRight.y(), static_cast<GLbitfield>(blitMask), static_cast<GLenum>(filter));
        }

        /**
         * @brief Copy block of pixels from read to draw framebuffer
         * @param bottomLeft        Bottom left coordinates of source and
         *      destination rectangle
         * @param topRight          Top right coordinates of source and
         *      destination rectangle
         * @param blitMask          Blit mask
         *
         * Convenience function when source rectangle is the same as
         * destination rectangle. As the image is copied pixel-by-pixel,
         * no interpolation is needed and thus
         * AbstractTexture::Filter::NearestNeighbor filtering is used by
         * default.
         * @see @fn_gl{BlitFramebuffer}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
         */
        inline static void blit(const Math::Vector2<GLint>& bottomLeft, const Math::Vector2<GLint>& topRight, BlitMask blitMask) {
            glBlitFramebuffer(bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), static_cast<GLbitfield>(blitMask), static_cast<GLenum>(AbstractTexture::Filter::NearestNeighbor));
        }
        #endif

        /**
         * @brief Read block of pixels from framebuffer to image
         * @param offset            Offset in the framebuffer
         * @param size              %Image size
         * @param components        Color components
         * @param type              Data type
         * @param image             %Image where to put the data
         *
         * @see @fn_gl{ReadPixels}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        static void read(const Math::Vector2<GLint>& offset, const Math::Vector2<GLsizei>& size, AbstractImage::Components components, AbstractImage::ComponentType type, Image2D* image);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Read block of pixels from framebuffer to buffered image
         * @param offset            Offset in the framebuffer
         * @param size              %Image size
         * @param components        Color components
         * @param type              Data type
         * @param image             Buffered image where to put the data
         * @param usage             %Buffer usage
         *
         * @see Buffer::bind(Target), @fn_gl{ReadPixels}
         * @requires_gl
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        static void read(const Math::Vector2<GLint>& offset, const Math::Vector2<GLsizei>& size, AbstractImage::Components components, AbstractImage::ComponentType type, BufferedImage2D* image, Buffer::Usage usage);
        #endif

        /*@}*/

    private:
        GLuint framebuffer;
};

CORRADE_ENUMSET_OPERATORS(Framebuffer::ClearMask)
#ifndef MAGNUM_TARGET_GLES
CORRADE_ENUMSET_OPERATORS(Framebuffer::BlitMask)
#endif

}

#endif
