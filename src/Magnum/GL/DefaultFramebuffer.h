#ifndef Magnum_GL_DefaultFramebuffer_h
#define Magnum_GL_DefaultFramebuffer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::GL::DefaultFramebuffer
 */

#include "Magnum/GL/AbstractFramebuffer.h"

namespace Magnum { namespace GL {

/**
@brief Default framebuffer

Default framebuffer, i.e. the actual screen surface. It is automatically
created when @ref Context is created and it is available through global
variable @ref defaultFramebuffer.

@section GL-DefaultFramebuffer-usage Usage

When you are using only the default framebuffer, the usage is simple. You
must ensure that it is properly resized when window surface is resized. In case
you're using one of the @ref Platform::Sdl2Application "Platform::Application"
classes, pass the new size in your
@ref Platform::Sdl2Application::viewportEvent() "viewportEvent()"
implementation, for example:

@snippet MagnumGL-framebuffer.cpp DefaultFramebuffer-usage-viewport

Next thing you probably want is to clear all used buffers before performing
any drawing. Again, in case you're using one of the
@ref Platform::Sdl2Application "Platform::Application" classes, do it in your
@ref Platform::Sdl2Application::drawEvent() "drawEvent()" implementation, for
example:

@snippet MagnumGL-framebuffer.cpp DefaultFramebuffer-usage-clear

See documentation of particular functions and @ref Framebuffer documentation
for more involved usage, usage of non-default or multiple framebuffers.

@section GL-DefaultFramebuffer-performance-optimizations Performance optimizations

See also @ref GL-AbstractFramebuffer-performance-optimization "relevant section in AbstractFramebuffer".

If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is available,
functions @ref checkStatus(), @ref mapForDraw(), @ref mapForRead() and
@ref invalidate() use DSA to avoid unnecessary calls to @fn_gl{BindFramebuffer}.
See their respective documentation for more information.
*/
class MAGNUM_GL_EXPORT DefaultFramebuffer: public AbstractFramebuffer {
    friend Context;

    public:
        /**
         * @brief Status
         *
         * @see @ref checkStatus()
         * @m_enum_values_as_keywords
         * @requires_gl30 Extension @gl_extension{ARB,framebuffer_object}
         */
        enum class Status: GLenum {
            /** The framebuffer is complete */
            Complete = GL_FRAMEBUFFER_COMPLETE,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * The default framebuffer does not exist.
             * @requires_gles30 Extension @gl_extension{OES,surfaceless_context}
             *      in OpenGL ES 2.0.
             * @requires_gles Surfaceless context is not available in WebGL.
             */
            #ifndef MAGNUM_TARGET_GLES2
            Undefined = GL_FRAMEBUFFER_UNDEFINED
            #else
            Undefined = GL_FRAMEBUFFER_UNDEFINED_OES
            #endif
            #endif
        };

        /**
         * @brief Draw attachment
         *
         * @see @ref mapForDraw()
         * @m_enum_values_as_keywords
         * @requires_gles30 Extension @gl_extension{EXT,draw_buffers} in OpenGL
         *      ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{WEBGL,draw_buffers} in
         *      WebGL 1.0.
         */
        enum class DrawAttachment: GLenum {
            /** Don't use the output. */
            None = GL_NONE,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Write output to front left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES
             *      or WebGL.
             */
            FrontLeft = GL_FRONT_LEFT,

            /**
             * Write output to front right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES
             *      or WebGL.
             */
            FrontRight = GL_FRONT_RIGHT,

            /**
             * Write output to back left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES
             *      or WebGL.
             */
            BackLeft = GL_BACK_LEFT,

            /**
             * Write output to back right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES
             *      or WebGL.
             */
            BackRight = GL_BACK_RIGHT,
            #endif

            /**
             * Write output to back buffer.
             *
             * On desktop OpenGL this is equal to @ref DrawAttachment::BackLeft.
             */
            #ifdef MAGNUM_TARGET_GLES
            Back = GL_BACK
            #else
            Back = GL_BACK_LEFT
            #endif
        };

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Read attachment
         *
         * @see @ref mapForRead()
         * @m_enum_values_as_keywords
         * @requires_gles30 Extension @gl_extension{NV,read_buffer} in OpenGL
         *      ES 2.0.
         * @requires_webgl20 Framebuffer read mapping is not available in WebGL
         *      1.0.
         */
        enum class ReadAttachment: GLenum {
            /** Don't read from any buffer */
            None = GL_NONE,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Read from front left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES or
             *      WebGL.
             */
            FrontLeft = GL_FRONT_LEFT,

            /**
             * Read from front right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES or
             *      WebGL.
             */
            FrontRight = GL_FRONT_RIGHT,

            /**
             * Read from back left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES or
             *      WebGL.
             */
            BackLeft = GL_BACK_LEFT,

            /**
             * Read from back right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES or
             *      WebGL.
             */
            BackRight = GL_BACK_RIGHT,

            /**
             * Read from left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES or
             *      WebGL.
             */
            Left = GL_LEFT,

            /**
             * Read from right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES or
             *      WebGL.
             */
            Right = GL_RIGHT,
            #endif

            /** Read from back buffer. */
            Back = GL_BACK,

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Read from front buffer.
             * @requires_es_extension Extension @gl_extension2{NV,read_buffer_front,NV_read_buffer}
             * @requires_gles Reading from front buffer is not available in
             *      WebGL.
             */
            Front = GL_FRONT,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Read from front and back buffer.
             * @requires_gl In OpenGL ES you must specify either
             *      @ref ReadAttachment::Front or @ref ReadAttachment::Back. In
             *      WebGL there is only @ref ReadAttachment::Back.
             */
            FrontAndBack = GL_FRONT_AND_BACK
            #endif
        };

        /**
         * @brief Invalidation attachment
         *
         * @see @ref invalidate()
         * @m_enum_values_as_keywords
         * @requires_gl43 Extension @gl_extension{ARB,invalidate_subdata}
         * @requires_gles30 Extension @gl_extension{EXT,discard_framebuffer} in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Framebuffer invalidation is not available in WebGL
         *      1.0.
         */
        enum class InvalidationAttachment: GLenum {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Invalidate front left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES
             *      or WebGL.
             */
            FrontLeft = GL_FRONT_LEFT,

            /**
             * Invalidate front right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES
             *      or WebGL.
             */
            FrontRight = GL_FRONT_RIGHT,

            /**
             * Invalidate back left buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES
             *      or WebGL.
             */
            BackLeft = GL_BACK_LEFT,

            /**
             * Invalidate back right buffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES
             *      or WebGL.
             */
            BackRight = GL_BACK_RIGHT,
            #endif

            /** Invalidate color buffer. */
            #ifndef MAGNUM_TARGET_GLES2
            Color = GL_COLOR,
            #else
            Color = GL_COLOR_EXT,
            #endif

            /** Invalidate depth bufer. */
            #ifndef MAGNUM_TARGET_GLES2
            Depth = GL_DEPTH,
            #else
            Depth = GL_DEPTH_EXT,
            #endif

            /** Invalidate stencil buffer. */
            #ifndef MAGNUM_TARGET_GLES2
            Stencil = GL_STENCIL
            #else
            Stencil = GL_STENCIL_EXT
            #endif
        };
        #endif

        /**
         * @brief Constructor
         *
         * Not meant to be constructed on the application side, use the
         * @ref GL::defaultFramebuffer instance directly.
         */
        constexpr explicit DefaultFramebuffer(): AbstractFramebuffer{0, {}, ObjectFlag::Created|ObjectFlag::DeleteOnDestruction} {}

        /** @brief Copying is not allowed */
        DefaultFramebuffer(const DefaultFramebuffer&) = delete;

        /** @brief Moving is not allowed */
        DefaultFramebuffer(DefaultFramebuffer&&) = delete;

        /** @brief Copying is not allowed */
        DefaultFramebuffer& operator=(const DefaultFramebuffer&) = delete;

        /** @brief Moving is not allowed */
        DefaultFramebuffer& operator=(DefaultFramebuffer&& other) = delete;

        /**
         * @brief Check framebuffer status
         * @param target    Target for which to check the status
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the framebuffer is bound before the operation (if not
         * already).
         *
         * The @p target parameter is ignored on OpenGL ES 2.0 if none of
         * @gl_extension{APPLE,framebuffer_multisample}, @gl_extension{ANGLE,framebuffer_blit}
         * or @gl_extension{NV,framebuffer_blit} is available and also on WebGL
         * 1.0.
         * @see @fn_gl2_keyword{CheckNamedFramebufferStatus,CheckFramebufferStatus},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl_keyword{CheckFramebufferStatus}
         * @requires_gl30 Extension @gl_extension{ARB,framebuffer_object}
         */
        Status checkStatus(FramebufferTarget target);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Clear color buffer to specified value
         * @param color         Value to clear with
         * @return Reference to self (for method chaining)
         *
         * @see @ref clear(), @fn_gl_keyword{ClearNamedFramebuffer}, eventually
         *      @fn_gl{BindFramebuffer}, then @fn_gl_keyword{ClearBuffer}
         * @requires_gl30 Direct framebuffer clearing is not available in
         *      OpenGL 2.1.
         * @requires_gles30 Direct framebuffer clearing is not available in
         *      OpenGL ES 2.0 or WebGL 1.0.
         */
        DefaultFramebuffer& clearColor(const Color4& color);

        /** @overload */
        DefaultFramebuffer& clearColor(const Vector4i& color);

        /** @overload */
        DefaultFramebuffer& clearColor(const Vector4ui& color);
        #endif

        /**
         * @brief Map shader outputs to buffer attachment
         * @return Reference to self (for method chaining)
         *
         * @p attachments is list of shader outputs mapped to buffer
         * attachments. Shader outputs which are not listed are not used, you
         * can achieve the same by passing @ref DrawAttachment::None as
         * attachment. Example usage:
         *
         * @snippet MagnumGL.cpp DefaultFramebuffer-usage-map
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref maxDrawBuffers(), @ref maxDualSourceDrawBuffers(),
         *      @ref mapForRead(), @fn_gl2_keyword{NamedFramebufferDrawBuffers,DrawBuffers},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl_keyword{DrawBuffers}
         * @requires_gles30 Extension @gl_extension{EXT,draw_buffers} in OpenGL
         *      ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{WEBGL,draw_buffers} in
         *      WebGL 1.0.
         */
        DefaultFramebuffer& mapForDraw(std::initializer_list<std::pair<UnsignedInt, DrawAttachment>> attachments);

        /**
         * @brief Map shader output to buffer attachment
         * @param attachment        Buffer attachment
         * @return Reference to self (for method chaining)
         *
         * Similar to above function, can be used in cases when shader has
         * only one (unnamed) output.
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref mapForRead(), @fn_gl2_keyword{NamedFramebufferDrawBuffer,DrawBuffer},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl_keyword{DrawBuffer}
         *      or @fn_gl{DrawBuffers} in OpenGL ES 3.0
         * @requires_gles30 Extension @gl_extension{EXT,draw_buffers} in OpenGL
         *      ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{WEBGL,draw_buffers} in
         *      WebGL 1.0.
         */
        DefaultFramebuffer& mapForDraw(DrawAttachment attachment);

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Map given attachment for reading
         * @param attachment        Buffer attachment
         * @return Reference to self (for method chaining)
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * not available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref mapForDraw(), @fn_gl2_keyword{NamedFramebufferReadBuffer,ReadBuffer},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl_keyword{ReadBuffer}
         * @requires_gles30 Extension @gl_extension{NV,read_buffer} in OpenGL
         *      ES 2.0.
         * @requires_webgl20 Framebuffer read mapping is not available in WebGL
         *      1.0.
         */
        DefaultFramebuffer& mapForRead(ReadAttachment attachment);

        /**
         * @brief Invalidate framebuffer
         * @param attachments       Attachments to invalidate
         *
         * If extension @gl_extension{ARB,invalidate_subdata} (part of OpenGL
         * 4.3), extension @gl_extension{EXT,discard_framebuffer} in OpenGL ES
         * 2.0 or OpenGL ES 3.0 is not available, this function does nothing.
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @fn_gl2_keyword{InvalidateNamedFramebufferData,InvalidateFramebuffer},
         *      eventually @fn_gl_keyword{InvalidateFramebuffer} or
         *      @fn_gl_extension_keyword{DiscardFramebuffer,EXT,discard_framebuffer}
         *      on OpenGL ES 2.0
         * @requires_webgl20 Framebuffer invalidation is not available in WebGL
         *      1.0.
         */
        void invalidate(std::initializer_list<InvalidationAttachment> attachments);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Invalidate framebuffer rectangle
         * @param attachments       Attachments to invalidate
         * @param rectangle         Rectangle to invalidate
         *
         * If extension @gl_extension{ARB,invalidate_subdata} (part of OpenGL
         * 4.3) is not available, this function does nothing. If
         * @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref invalidate(std::initializer_list<InvalidationAttachment>),
         *      @fn_gl2_keyword{InvalidateNamedFramebufferSubData,InvalidateSubFramebuffer},
         *      eventually @fn_gl_keyword{InvalidateSubFramebuffer}
         * @requires_gles30 Use @ref invalidate(std::initializer_list<InvalidationAttachment>)
         *      in OpenGL ES 2.0 instead.
         * @requires_webgl20 Framebuffer invalidation is not available in WebGL
         *      1.0.
         */
        void invalidate(std::initializer_list<InvalidationAttachment> attachments, const Range2Di& rectangle);
        #endif

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        DefaultFramebuffer& setViewport(const Range2Di& rectangle) {
            AbstractFramebuffer::setViewport(rectangle);
            return *this;
        }
        DefaultFramebuffer& clear(FramebufferClearMask mask) {
            AbstractFramebuffer::clear(mask);
            return *this;
        }
        #ifndef MAGNUM_TARGET_GLES2
        DefaultFramebuffer& clearDepth(Float depth) {
            AbstractFramebuffer::clearDepth(depth);
            return *this;
        }
        DefaultFramebuffer& clearStencil(Int stencil) {
            AbstractFramebuffer::clearStencil(stencil);
            return *this;
        }
        DefaultFramebuffer& clearDepthStencil(Float depth, Int stencil) {
            AbstractFramebuffer::clearDepthStencil(depth, stencil);
            return *this;
        }
        #endif
        #endif

    private:
        static void MAGNUM_GL_LOCAL initializeContextBasedFunctionality(Context& context);
};

/** @brief Default framebuffer instance */
/* Even though the constructor is constexpr, this variable can't -- all
   framebuffer APIs are non-const since they modify global GL state and besides
   that we also need to modify its private _viewport member quite a lot */
extern DefaultFramebuffer MAGNUM_GL_EXPORT defaultFramebuffer;

/** @debugoperatorclassenum{DefaultFramebuffer,DefaultFramebuffer::Status} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DefaultFramebuffer::Status value);

}}

#endif
