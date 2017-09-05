#ifndef Magnum_Framebuffer_h
#define Magnum_Framebuffer_h
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
 * @brief Class @ref Magnum::Framebuffer
 */

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/AbstractFramebuffer.h"
#include "Magnum/Tags.h"

#ifdef _X11_XLIB_H_ /* Xlib.h, I hate you sincerely */
#undef Status
#endif

namespace Magnum {

/**
@brief Framebuffer

Unlike @ref DefaultFramebuffer, which is used for on-screen rendering, this
class is used for off-screen rendering, usable either in windowless
applications, texture generation or for various post-processing effects.

@anchor Framebuffer-usage
## Example usage

See @ref DefaultFramebuffer-usage "DefaultFramebuffer documentation" for
introduction. Imagine you have shader with multiple outputs (e.g. for deferred
rendering). You want to render them off-screen to textures and then use the
textures for actual on-screen rendering. First you need to create the
framebuffer with the same viewport as default framebuffer and attach textures
and renderbuffers to desired outputs:
@code
Framebuffer framebuffer({defaultFramebuffer.viewportPosition(), defaultFramebuffer.viewportSize()});
Texture2D color, normal;
Renderbuffer depthStencil;

// configure the textures and allocate texture memory...

framebuffer.attachTexture(Framebuffer::ColorAttachment(0), color);
framebuffer.attachTexture(Framebuffer::ColorAttachment(1), normal);
framebuffer.attachRenderbuffer(Framebuffer::BufferAttachment::DepthStencil, depthStencil);
@endcode

Then you need to map outputs of your shader to color attachments in the
framebuffer:
@code
framebuffer.mapForDraw({{MyShader::ColorOutput, Framebuffer::ColorAttachment(0)},
                        {MyShader::NormalOutput, Framebuffer::ColorAttachment(1)}});
@endcode

The actual @ref Platform::Sdl2Application::drawEvent() "drawEvent()" might look
like this. First you clear all buffers you need, perform drawing to off-screen
framebuffer, then bind the default and render the textures on screen:
@code
void drawEvent() {
    defaultFramebuffer.clear(FramebufferClear::Color)
    framebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth|FramebufferClear::Stencil);

    framebuffer.bind();
    // ...

    defaultFramebuffer.bind();
    // ...
}
@endcode

## Performance optimizations

See also @ref AbstractFramebuffer-performance-optimization "relevant section in AbstractFramebuffer".

If either @extension{ARB,direct_state_access} (part of OpenGL 4.5) or
@extension{EXT,direct_state_access} desktop extension is available, functions
@ref checkStatus(), @ref mapForDraw(), @ref mapForRead(), @ref invalidate(),
@ref attachRenderbuffer(), @ref attachTexture(), @ref attachCubeMapTexture(),
@ref attachTextureLayer() and @ref detach() use DSA to avoid unnecessary calls
to @fn_gl{BindFramebuffer}. See their respective documentation for more
information.

@requires_gl30 Extension @extension{ARB,framebuffer_object}
*/
class MAGNUM_EXPORT Framebuffer: public AbstractFramebuffer, public AbstractObject {
    friend Implementation::FramebufferState;

    public:
        /**
         * @brief Color attachment
         *
         * @see @ref mapForDraw(), @ref attachRenderbuffer(),
         *      @ref attachTexture(), @ref attachCubeMapTexture(),
         *      @ref attachTextureLayer()
         * @requires_gles30 Extension @extension{EXT,draw_buffers} or
         *      @extension{NV,draw_buffers} for @ref mapForDraw() and
         *      extension @extension{NV,fbo_color_attachments} for `attach*()`
         *      in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{WEBGL,draw_buffers} in
         *      WebGL 1.0.
         */
        class ColorAttachment {
            friend Framebuffer;

            public:
                /**
                 * @brief Constructor
                 * @param id        Color attachment ID
                 */
                constexpr explicit ColorAttachment(UnsignedInt id): attachment(GL_COLOR_ATTACHMENT0 + id) {}

                #ifndef DOXYGEN_GENERATING_OUTPUT
                constexpr explicit operator GLenum() const { return attachment; }
                #endif

            private:
                GLenum attachment;
        };

        /**
         * @brief Draw attachment
         *
         * @see @ref mapForDraw()
         * @requires_gles30 Extension @extension{EXT,draw_buffers} or
         *      @extension{NV,draw_buffers} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{WEBGL,draw_buffers} in
         *      WebGL 1.0.
         */
        class MAGNUM_EXPORT DrawAttachment {
            public:
                /** @brief No attachment */
                static const DrawAttachment None;

                /** @brief Color attachment */
                constexpr /*implicit*/ DrawAttachment(Framebuffer::ColorAttachment attachment): attachment(GLenum(attachment)) {}

                #ifndef DOXYGEN_GENERATING_OUTPUT
                constexpr explicit operator GLenum() const { return attachment; }
                #endif

            private:
                constexpr explicit DrawAttachment(GLenum attachment): attachment(attachment) {}

                GLenum attachment;
        };

        /**
         * @brief Buffer attachment
         *
         * @see @ref attachRenderbuffer(), @ref attachTexture(),
         *      @ref attachCubeMapTexture(), @ref attachTextureLayer()
         * @requires_gles30 Extension @extension{EXT,draw_buffers} or
         *      @extension{NV,fbo_color_attachments} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{WEBGL,draw_buffers} in
         *      WebGL 1.0.
         */
        class MAGNUM_EXPORT BufferAttachment {
            public:
                /** @brief Depth buffer */
                static const BufferAttachment Depth;

                /** @brief Stencil buffer */
                static const BufferAttachment Stencil;

                #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
                /**
                 * @brief Both depth and stencil buffer
                 *
                 * @requires_gles30 Combined depth and stencil attachment is
                 *      not available in OpenGL ES 2.0. Attach the same object
                 *      to both @ref BufferAttachment::Depth and
                 *      @ref BufferAttachment::Stencil instead.
                 * @todo Support this in ES2 (bind to both depth and stencil internally)
                 */
                static const BufferAttachment DepthStencil;
                #endif

                /** @brief Color buffer */
                constexpr /*implicit*/ BufferAttachment(Framebuffer::ColorAttachment attachment): attachment(GLenum(attachment)) {}

                #ifndef DOXYGEN_GENERATING_OUTPUT
                constexpr explicit operator GLenum() const { return attachment; }
                #endif

            private:
                constexpr explicit BufferAttachment(GLenum attachment): attachment(attachment) {}

                GLenum attachment;
        };

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Invalidation attachment
         *
         * @see @ref invalidate()
         * @requires_gl43 Extension @extension{ARB,invalidate_subdata}
         * @requires_gles30 Extension @extension{EXT,discard_framebuffer} in
         *      OpenGL ES 2.0.
         * @requires_webgl20 Framebuffer invalidation is not available in WebGL
         *      1.0.
         */
        class MAGNUM_EXPORT InvalidationAttachment {
            public:
                /** @brief Invalidate depth buffer */
                static const InvalidationAttachment Depth;

                /** @brief Invalidate stencil buffer */
                static const InvalidationAttachment Stencil;

                /** @brief Invalidate color buffer */
                constexpr /*implicit*/ InvalidationAttachment(Framebuffer::ColorAttachment attachment): attachment(GLenum(attachment)) {}

                #ifndef DOXYGEN_GENERATING_OUTPUT
                constexpr explicit operator GLenum() const { return attachment; }
                #endif

            private:
                constexpr explicit InvalidationAttachment(GLenum attachment): attachment(attachment) {}

                GLenum attachment;
        };
        #endif

        /**
         * @brief Status
         *
         * @see @ref checkStatus()
         */
        enum class Status: GLenum {
            /** The framebuffer is complete */
            Complete = GL_FRAMEBUFFER_COMPLETE,

            /** Any of the attachment points are incomplete */
            IncompleteAttachment = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,

            /** The framebuffer does not have at least one image attached to it */
            IncompleteMissingAttachment = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,

            #ifndef MAGNUM_TARGET_GLES
            /** @todo Why exactly this is not needed? */
            /**
             * No object attached to any draw color attachment points
             * @requires_gl Not available in OpenGL ES or WebGL.
             */
            IncompleteDrawBuffer = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,

            /**
             * No object attached to read color attachment point
             * @requires_gl Not available in OpenGL ES or WebGL.
             */
            IncompleteReadBuffer = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
            #endif

            /**
             * Combination of internal formats of the attached images violates
             * an implementation-dependent set of restrictions.
             */
            Unsupported = GL_FRAMEBUFFER_UNSUPPORTED,

            /**
             * Sample count or locations are not the same for all attached
             * images.
             * @requires_gles30 Extension @extension{ANGLE,framebuffer_multisample},
             *      @extension{APPLE,framebuffer_multisample},
             *      @extension{EXT,multisampled_render_to_texture} or
             *      @extension{NV,framebuffer_multisample} in OpenGL ES 2.0.
             * @requires_webgl20 Multisample framebuffers are not available in
             *      WebGL 1.0.
             */
            #ifndef MAGNUM_TARGET_GLES2
            IncompleteMultisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
            #elif !defined(MAGNUM_TARGET_WEBGL)
            IncompleteMultisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_APPLE,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Mismatched layered color attachments
             * @requires_gl Geometry shaders are not available in OpenGL ES or
             *      WebGL.
             */
            IncompleteLayerTargets = GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
            #endif
        };

        /** @todo `GL_MAX_FRAMEBUFFER_WIDTH` etc. when @extension{ARB,framebuffer_no_attachments} is done */

        /**
         * @brief Max supported color attachment count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{EXT,draw_buffers} nor
         * @extension{NV,fbo_color_attachments} extension is available in
         * OpenGL ES 2.0 and @webgl_extension{WEBGL,draw_buffers} is not
         * available in WebGL 1.0, returns `0`.
         * @see @ref mapForDraw(), @fn_gl{Get} with @def_gl{MAX_COLOR_ATTACHMENTS}
         */
        static Int maxColorAttachments();

        /**
         * @brief Wrap existing OpenGL framebuffer object
         * @param id            OpenGL framebuffer ID
         * @param viewport      Viewport to use with this framebuffer
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL framebuffer
         * object. Unlike framebuffer created using constructor, the OpenGL
         * object is by default not deleted on destruction, use @p flags for
         * different behavior.
         * @see @ref release()
         */
        static Framebuffer wrap(GLuint id, const Range2Di& viewport, ObjectFlags flags = {}) {
            return Framebuffer{id, viewport, flags};
        }

        /**
         * @brief Constructor
         *
         * Generates new OpenGL framebuffer object. If @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the framebuffer is created on
         * first use.
         * @see @ref Framebuffer(NoCreateT), @ref wrap(), @ref setViewport(),
         *      @fn_gl{CreateFramebuffers}, eventually @fn_gl{GenFramebuffers}
         */
        explicit Framebuffer(const Range2Di& viewport);

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * @see @ref Framebuffer(const Range2Di&), @ref wrap()
         */
        explicit Framebuffer(NoCreateT) noexcept { _id = 0; }

        /** @brief Copying is not allowed */
        Framebuffer(const Framebuffer&) = delete;

        /** @brief Move constructor */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        inline Framebuffer(Framebuffer&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL framebuffer object.
         * @see @ref wrap(), @ref release(), @fn_gl{DeleteFramebuffers}
         */
        ~Framebuffer();

        /** @brief Copying is not allowed */
        Framebuffer& operator=(const Framebuffer&) = delete;

        /** @brief Move assignment */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        inline Framebuffer& operator=(Framebuffer&& other) noexcept;

        /** @brief OpenGL framebuffer ID */
        GLuint id() const { return _id; }

        /**
         * @brief Release OpenGL object
         *
         * Releases ownership of OpenGL framebuffer object and returns its ID
         * so it is not deleted on destruction. The internal state is then
         * equivalent to moved-from state.
         * @see @ref wrap()
         */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        inline GLuint release();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Framebuffer label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function returns empty string.
         * @see @fn_gl{GetObjectLabel} or
         *      @fn_gl_extension{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{FRAMEBUFFER}
         * @requires_gles Debug output is not available in WebGL.
         */
        std::string label();

        /**
         * @brief Set framebuffer label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} (covered also by @extension{ANDROID,extension_pack_es31a})
         * nor @extension{EXT,debug_label} desktop or ES extension is
         * available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} or
         *      @fn_gl_extension{LabelObject,EXT,debug_label} with
         *      @def_gl{FRAMEBUFFER}
         * @requires_gles Debug output is not available in WebGL.
         */
        Framebuffer& setLabel(const std::string& label) {
            return setLabelInternal({label.data(), label.size()});
        }

        /** @overload */
        template<std::size_t size> Framebuffer& setLabel(const char(&label)[size]) {
            return setLabelInternal({label, size - 1});
        }
        #endif

        /**
         * @brief Check framebuffer status
         * @param target    Target for which check the status
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the framebuffer is bound before the operation (if not
         * already).
         *
         * The @p target parameter is ignored on OpenGL ES 2.0 if none of
         * @extension{APPLE,framebuffer_multisample}, @extension{ANGLE,framebuffer_blit}
         * or @extension{NV,framebuffer_blit} is available and also on WebGL
         * 1.0.
         * @see @fn_gl2{CheckNamedFramebufferStatus,CheckFramebufferStatus},
         *      @fn_gl_extension{CheckNamedFramebufferStatus,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{CheckFramebufferStatus}
         */
        Status checkStatus(FramebufferTarget target);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Clear color buffer to specified value
         * @param attachment    Color attachment index
         * @param color         Value to clear with
         * @return Reference to self (for method chaining)
         *
         * @see @ref clear(), @fn_gl{ClearNamedFramebuffer}, eventually
         *      @fn_gl{BindFramebuffer}, then @fn_gl{ClearBuffer}
         * @requires_gl30 Direct framebuffer clearing is not available in
         *      OpenGL 2.1.
         * @requires_gles30 Direct framebuffer clearing is not available in
         *      OpenGL ES 2.0 or WebGL 1.0.
         */
        Framebuffer& clearColor(Int attachment, const Color4& color);

        /** @overload */
        Framebuffer& clearColor(Int attachment, const Vector4i& color);

        /** @overload */
        Framebuffer& clearColor(Int attachment, const Vector4ui& color);
        #endif

        /**
         * @brief Map shader output to attachments
         * @return Reference to self (for method chaining)
         *
         * @p attachments is list of shader outputs mapped to framebuffer
         * color attachment IDs. Shader outputs which are not listed are not
         * used, you can achieve the same by passing @ref Framebuffer::DrawAttachment::None
         * as color attachment ID. Example usage:
         * @code
         * framebuffer.mapForDraw({{MyShader::ColorOutput, Framebuffer::ColorAttachment(0)},
         *                         {MyShader::NormalOutput, Framebuffer::DrawAttachment::None}});
         * @endcode
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref maxDrawBuffers(), @ref maxDualSourceDrawBuffers(),
         *      @ref maxColorAttachments(), @ref mapForRead(),
         *      @fn_gl2{NamedFramebufferDrawBuffers,DrawBuffers},
         *      @fn_gl_extension{FramebufferDrawBuffers,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{DrawBuffers}
         * @requires_gles30 Extension @extension{EXT,draw_buffers} or
         *      @extension{NV,draw_buffers} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{WEBGL,draw_buffers} in
         *      WebGL 1.0.
         */
        Framebuffer& mapForDraw(std::initializer_list<std::pair<UnsignedInt, DrawAttachment>> attachments);

        /**
         * @brief Map shader output to attachment
         * @param attachment        Draw attachment
         * @return Reference to self (for method chaining)
         *
         * Similar to above function, can be used in cases when shader has
         * only one (unnamed) output.
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref maxColorAttachments(), @ref mapForRead(),
         *      @fn_gl2{NamedFramebufferDrawBuffer,DrawBuffer},
         *      @fn_gl_extension{FramebufferDrawBuffer,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{DrawBuffer} (or
         *      @fn_gl{DrawBuffers} in OpenGL ES)
         * @requires_gles30 Extension @extension{EXT,draw_buffers} or
         *      @extension{NV,draw_buffers} in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{WEBGL,draw_buffers} in
         *      WebGL 1.0.
         */
        Framebuffer& mapForDraw(DrawAttachment attachment);

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Map given color attachment for reading
         * @param attachment        Color attachment
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref mapForDraw(), @fn_gl2{NamedFramebufferReadBuffer,ReadBuffer},
         *      @fn_gl_extension{FramebufferReadBuffer,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{ReadBuffer}
         * @requires_gles30 Extension @extension2{NV,read_buffer,GL_NV_read_buffer}
         *      in OpenGL ES 2.0.
         * @requires_webgl20 Framebuffer read mapping is not available in WebGL
         *      1.0.
         */
        Framebuffer& mapForRead(ColorAttachment attachment);

        /**
         * @brief Invalidate framebuffer
         * @param attachments       Attachments to invalidate
         *
         * If extension @extension{ARB,invalidate_subdata} (part of OpenGL
         * 4.3), extension @extension{EXT,discard_framebuffer} in OpenGL ES
         * 2.0 or OpenGL ES 3.0 is not available, this function does nothing.
         * If @extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @fn_gl2{InvalidateNamedFramebufferData,InvalidateFramebuffer},
         *      eventually @fn_gl{InvalidateFramebuffer} or
         *      @fn_gl_extension{DiscardFramebuffer,EXT,discard_framebuffer}
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
         * If extension @extension{ARB,invalidate_subdata} (part of OpenGL
         * 4.3) is not available, this function does nothing. If
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref invalidate(std::initializer_list<InvalidationAttachment>),
         *      @fn_gl2{InvalidateNamedFramebufferSubData,InvalidateSubFramebuffer},
         *      eventually @fn_gl{InvalidateSubFramebuffer}
         * @requires_gles30 Use @ref invalidate(std::initializer_list<InvalidationAttachment>)
         *      in OpenGL ES 2.0 instead.
         * @requires_webgl20 Framebuffer invalidation is not available in WebGL
         *      1.0.
         */
        void invalidate(std::initializer_list<InvalidationAttachment> attachments, const Range2Di& rectangle);
        #endif

        /**
         * @brief Attach renderbuffer to given buffer
         * @param attachment        Buffer attachment
         * @param renderbuffer      Renderbuffer
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref detach(), @fn_gl2{NamedFramebufferRenderbuffer,FramebufferRenderbuffer},
         *      @fn_gl_extension{NamedFramebufferRenderbuffer,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{FramebufferRenderbuffer}
         */
        Framebuffer& attachRenderbuffer(BufferAttachment attachment, Renderbuffer& renderbuffer);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Attach texture to given buffer
         * @param attachment        Buffer attachment
         * @param texture           Texture
         * @param level             Mip level
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref detach(), @ref attachCubeMapTexture(),
         *      @fn_gl2{NamedFramebufferTexture,FramebufferTexture},
         *      @fn_gl_extension{NamedFramebufferTexture1D,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and
         *      @fn_gl2{FramebufferTexture1D,FramebufferTexture}
         * @requires_gl Only 2D and 3D textures are available in OpenGL ES and
         *      WebGL.
         */
        Framebuffer& attachTexture(BufferAttachment attachment, Texture1D& texture, Int level);
        #endif

        /**
         * @brief Attach texture to given buffer
         * @param attachment        Buffer attachment
         * @param texture           Texture
         * @param level             Mip level
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref detach(), @ref attachCubeMapTexture(),
         *      @fn_gl2{NamedFramebufferTexture,FramebufferTexture},
         *      @fn_gl_extension{NamedFramebufferTexture2D,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and
         *      @fn_gl2{FramebufferTexture2D,FramebufferTexture}
         * @requires_gles30 Extension @extension{OES,fbo_render_mipmap} to
         *      render to @p level different than `0` in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{OES,fbo_render_mipmap}
         *      to render to @p level different than `0` in WebGL 1.0.
         */
        Framebuffer& attachTexture(BufferAttachment attachment, Texture2D& texture, Int level);

        #ifndef MAGNUM_TARGET_GLES
        /** @overload
         * @requires_gl31 Extension @extension{ARB,texture_rectangle}
         * @requires_gl Rectangle textures are not available in OpenGL ES and
         *      WebGL.
         */
        Framebuffer& attachTexture(BufferAttachment attachment, RectangleTexture& texture);
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /** @overload
         * @requires_gl32 Extension @extension{ARB,texture_multisample}
         * @requires_gles31 Multisample textures are not available in OpenGL ES
         *      3.0 and older.
         * @requires_gles Multisample textures are not available in WebGL.
         */
        Framebuffer& attachTexture(BufferAttachment attachment, MultisampleTexture2D& texture);
        #endif

        /**
         * @brief Attach cube map texture to given buffer
         * @param attachment        Buffer attachment
         * @param texture           Texture
         * @param coordinate        Cube map coordinate
         * @param level             Mip level
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref detach(), @ref attachTexture(),
         *      @fn_gl2{NamedFramebufferTextureLayer,FramebufferTextureLayer},
         *      @fn_gl_extension{NamedFramebufferTexture2D,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl2{FramebufferTexture2D,FramebufferTexture}
         * @requires_gles30 Extension @extension{OES,fbo_render_mipmap} to
         *      render to @p level different than `0` in OpenGL ES 2.0.
         * @requires_webgl20 Extension @webgl_extension{OES,fbo_render_mipmap}
         *      to render to @p level different than `0` in WebGL 1.0.
         */
        Framebuffer& attachCubeMapTexture(BufferAttachment attachment, CubeMapTexture& texture, CubeMapCoordinate coordinate, Int level);

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Attach texture layer to given buffer
         * @param attachment        Buffer attachment
         * @param texture           Texture
         * @param level             Mip level
         * @param layer             Layer
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref detach(), @fn_gl2{NamedFramebufferTextureLayer,FramebufferTextureLayer},
         *      @fn_gl_extension{NamedFramebufferTextureLayer,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl2{FramebufferTextureLayer,FramebufferTexture}
         *      or @fn_gl_extension{FramebufferTexture3D,OES,texture_3D} in
         *      OpenGL ES 2.0
         * @requires_gles30 Extension @extension{OES,texture_3D} in OpenGL
         *      ES 2.0.
         * @requires_gles30 Extension @extension{OES,fbo_render_mipmap} to
         *      render to @p level different than `0` in OpenGL ES 2.0.
         * @requires_webgl20 Only 2D textures are available in WebGL 1.0.
         */
        Framebuffer& attachTextureLayer(BufferAttachment attachment, Texture3D& texture, Int level, Int layer);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /** @overload
         * @requires_gl30 Extension @extension{EXT,texture_array}
         * @requires_gl Only 2D array textures are available in OpenGL ES and
         *      WebGL.
         */
        Framebuffer& attachTextureLayer(BufferAttachment attachment, Texture1DArray& texture, Int level, Int layer);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gl30 Extension @extension{EXT,texture_array}
         * @requires_gles30 Array textures are not available in OpenGL ES 2.0.
         * @requires_webgl20 Array textures are not available in WebGL 1.0.
         */
        Framebuffer& attachTextureLayer(BufferAttachment attachment, Texture2DArray& texture, Int level, Int layer);
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /** @overload
         * @requires_gl40 Extension @extension{ARB,texture_cube_map_array}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,texture_cube_map_array}
         * @requires_gles Cube map texture arrays are not available in WebGL.
         */
        Framebuffer& attachTextureLayer(BufferAttachment attachment, CubeMapTextureArray& texture, Int level, Int layer);

        /** @overload
         * @requires_gl32 Extension @extension{ARB,texture_multisample}
         * @requires_gles30 Multisample 2D array textures are not defined in
         *      OpenGL ES 2.0.
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{OES,texture_storage_multisample_2d_array}
         * @requires_gles Multisample textures are not available in WebGL.
         */
        Framebuffer& attachTextureLayer(BufferAttachment attachment, MultisampleTexture2DArray& texture, Int layer);
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Attach layered cube map texture to given buffer
         * @param attachment        Buffer attachment
         * @param texture           Texture
         * @param level             Mip level
         * @return Reference to self (for method chaining)
         *
         * Attaches whole texture with all layers addressable using `gl_Layer`
         * in geometry shader. If neither @extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) nor @extension{EXT,direct_state_access} desktop
         * extension is available, the framebuffer is bound before the
         * operation (if not already).
         * @see @ref detach(), @ref attachTexture(),
         *      @fn_gl2{NamedFramebufferTexture,FramebufferTexture},
         *      @fn_gl_extension{NamedFramebufferTexture,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{FramebufferTexture}
         * @requires_gl32 Extension @extension{ARB,geometry_shader4}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,geometry_shader}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        Framebuffer& attachLayeredTexture(BufferAttachment attachment, Texture3D& texture, Int level);

        #ifndef MAGNUM_TARGET_GLES
        /** @overload
         * @requires_gl32 Extension @extension{ARB,geometry_shader4}
         * @requires_gl Only 2D array textures are available in OpenGL ES and
         *      WebGL.
         */
        Framebuffer& attachLayeredTexture(BufferAttachment attachment, Texture1DArray& texture, Int level);
        #endif

        /** @overload
         * @requires_gl32 Extension @extension{ARB,geometry_shader4}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,geometry_shader}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        Framebuffer& attachLayeredTexture(BufferAttachment attachment, Texture2DArray& texture, Int level);

        /**
         * @overload
         * @requires_gl32 Extension @extension{ARB,geometry_shader4}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,geometry_shader}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        Framebuffer& attachLayeredTexture(BufferAttachment attachment, CubeMapTexture& texture, Int level);

        /** @overload
         * @requires_gl40 Extension @extension{ARB,texture_cube_map_array}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,geometry_shader} and
         *      @extension{EXT,texture_cube_map_array}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        Framebuffer& attachLayeredTexture(BufferAttachment attachment, CubeMapTextureArray& texture, Int level);

        /** @overload
         * @requires_gl32 Extension @extension{ARB,geometry_shader4} and
         *      @extension{ARB,texture_multisample}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,geometry_shader} and
         *      @extension{OES,texture_storage_multisample_2d_array}
         * @requires_gles Geometry shaders are not available in WebGL.
         */
        Framebuffer& attachLayeredTexture(BufferAttachment attachment, MultisampleTexture2DArray& texture);
        #endif

        /**
         * @brief Detach any texture or renderbuffer bound to given buffer
         * @param attachment        Buffer attachment
         * @return Reference to self (for method chaining)
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the framebuffer is bound before the operation (if not
         * already).
         * @see @ref attachRenderbuffer(), @ref attachTexture(),
         *      @ref attachCubeMapTexture(), @ref attachTextureLayer(),
         *      @fn_gl2{NamedFramebufferRenderbuffer,FramebufferRenderbuffer},
         *      @fn_gl_extension{NamedFramebufferRenderbuffer,EXT,direct_state_access},
         *      eventually @fn_gl{BindFramebuffer} and @fn_gl{FramebufferRenderbuffer}
         */
        Framebuffer& detach(BufferAttachment attachment);

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Framebuffer& setViewport(const Range2Di& rectangle) {
            AbstractFramebuffer::setViewport(rectangle);
            return *this;
        }
        Framebuffer& clear(FramebufferClearMask mask) {
            AbstractFramebuffer::clear(mask);
            return *this;
        }
        #ifndef MAGNUM_TARGET_GLES2
        Framebuffer& clearDepth(Float depth) {
            AbstractFramebuffer::clearDepth(depth);
            return *this;
        }
        Framebuffer& clearStencil(Int stencil) {
            AbstractFramebuffer::clearStencil(stencil);
            return *this;
        }
        Framebuffer& clearDepthStencil(Float depth, Int stencil) {
            AbstractFramebuffer::clearDepthStencil(depth, stencil);
            return *this;
        }
        #endif
        #endif

    private:
        explicit Framebuffer(GLuint id, const Range2Di& viewport, ObjectFlags flags) noexcept: AbstractFramebuffer{id, viewport, flags} {}

        void MAGNUM_LOCAL createImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL createImplementationDSA();
        #endif

        #ifndef MAGNUM_TARGET_WEBGL
        Framebuffer& setLabelInternal(Containers::ArrayView<const char> label);
        #endif

        void MAGNUM_LOCAL renderbufferImplementationDefault(BufferAttachment attachment, GLuint renderbufferId);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL renderbufferImplementationDSA(BufferAttachment attachment, GLuint renderbufferId);
        void MAGNUM_LOCAL renderbufferImplementationDSAEXT(BufferAttachment attachment, GLuint renderbufferId);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL texture1DImplementationDefault(BufferAttachment attachment, GLuint textureId, GLint level);
        void MAGNUM_LOCAL texture1DImplementationDSAEXT(BufferAttachment attachment, GLuint textureId, GLint level);
        #endif

        void MAGNUM_LOCAL texture2DImplementationDefault(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint level);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL texture2DImplementationDSA(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint level);
        void MAGNUM_LOCAL texture2DImplementationDSAEXT(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint level);
        void MAGNUM_LOCAL textureCubeMapImplementationDSA(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint level);
        #endif

        #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
        void MAGNUM_LOCAL textureImplementationDefault(BufferAttachment attachment, GLuint textureId, GLint level);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL textureImplementationDSA(BufferAttachment attachment, GLuint textureId, GLint level);
        void MAGNUM_LOCAL textureImplementationDSAEXT(BufferAttachment attachment, GLuint textureId, GLint level);
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        void MAGNUM_LOCAL textureLayerImplementationDefault(BufferAttachment attachment, GLuint textureId, GLint level, GLint layer);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL textureLayerImplementationDSA(BufferAttachment attachment, GLuint textureId, GLint level, GLint layer);
        void MAGNUM_LOCAL textureLayerImplementationDSAEXT(BufferAttachment attachment, GLuint textureId, GLint level, GLint layer);
        #endif
};

/** @debugoperatorclassenum{Magnum::Framebuffer,Magnum::Framebuffer::Status} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Framebuffer::Status value);

inline Framebuffer::Framebuffer(Framebuffer&& other) noexcept {
    _id = other._id;
    _viewport = other._viewport;
    _flags = other._flags;
    other._id = 0;
    other._viewport = {};
}

inline Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    using std::swap;
    swap(_id, other._id);
    swap(_viewport, other._viewport);
    swap(_flags, other._flags);
    return *this;
}

inline GLuint Framebuffer::release() {
    const GLuint id = _id;
    _id = 0;
    return id;
}

}

#endif
