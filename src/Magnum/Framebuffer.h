#ifndef Magnum_Framebuffer_h
#define Magnum_Framebuffer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "Magnum/AbstractFramebuffer.h"
#include "Magnum/CubeMapTexture.h"

#ifdef _X11_XLIB_H_ /* Xlib.h, I hate you sincerely */
#undef Status
#endif

namespace Magnum {

/**
@brief %Framebuffer

Unlike @ref DefaultFramebuffer, which is used for on-screen rendering, this
class is used for off-screen rendering, usable either in windowless
applications, texture generation or for various post-processing effects.

@section Framebuffer-usage Example usage

See @ref DefaultFramebuffer-usage "DefaultFramebuffer documentation" for
introduction. Imagine you have shader with multiple outputs (e.g. for deferred
rendering). You want to render them off-screen to textures and then use the
textures for actual on-screen rendering. First you need to create the
framebuffer with the same viewport as default framebuffer and attach textures
and renderbuffers to desired outputs:
@code
Framebuffer framebuffer(defaultFramebuffer.viewportPosition(), defaultFramebuffer.viewportSize());
Texture2D color, normal;
Renderbuffer depthStencil;

// configure the textures and allocate texture memory...

framebuffer.attachTexture2D(Framebuffer::ColorAttachment(0), color);
framebuffer.attachTexture2D(Framebuffer::ColorAttachment(1), normal);
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

    framebuffer.bind(FramebufferTarget::Draw);
    // ...

    defaultFramebuffer.bind(Framebuffer::Target::Draw);
    // ...
}
@endcode

@section Framebuffer-performance-optimization Performance optimizations

See also @ref AbstractFramebuffer-performance-optimization "relevant section in AbstractFramebuffer".

If extension @extension{EXT,direct_state_access} is available, functions
@ref mapForDraw(), @ref mapForRead(), @ref attachRenderbuffer(),
@ref attachTexture1D(), @ref attachTexture2D(), @ref attachCubeMapTexture() and
@ref attachTexture3D() use DSA to avoid unnecessary calls to @fn_gl{BindFramebuffer}.
See their respective documentation for more information.

@requires_gl30 %Extension @extension{ARB,framebuffer_object}
@todo `MAX_COLOR_ATTACHMENTS`
*/
class MAGNUM_EXPORT Framebuffer: public AbstractFramebuffer, public AbstractObject {
    friend struct Implementation::FramebufferState;

    public:
        /**
         * @brief Color attachment
         *
         * @see @ref BufferAttachment, @ref attachRenderbuffer(),
         *      @ref attachTexture1D(), @ref attachTexture2D(),
         *      @ref attachCubeMapTexture(), @ref attachTexture3D()
         */
        class ColorAttachment {
            friend class Framebuffer;

            public:
                /**
                 * @brief Constructor
                 * @param id        Color attachment ID
                 *
                 * @requires_gles30 %Extension @es_extension{NV,fbo_color_attachments}
                 *      is required for @p id greater than 0 in OpenGL ES 2.0
                 */
                constexpr explicit ColorAttachment(UnsignedInt id): attachment(GL_COLOR_ATTACHMENT0 + id) {}

                #ifndef DOXYGEN_GENERATING_OUTPUT
                #ifndef CORRADE_GCC44_COMPATIBILITY
                constexpr explicit operator GLenum() const { return attachment; }
                #else
                constexpr operator GLenum() const { return attachment; }
                #endif
                #endif

            private:
                GLenum attachment;
        };

        /**
         * @brief Draw attachment
         *
         * @see @ref mapForDraw()
         */
        class DrawAttachment {
            public:
                /** @brief No attachment */
                static const DrawAttachment None;

                /** @brief Color attachment */
                constexpr /*implicit*/ DrawAttachment(Framebuffer::ColorAttachment attachment): attachment(GLenum(attachment)) {}

                #ifndef DOXYGEN_GENERATING_OUTPUT
                #ifndef CORRADE_GCC44_COMPATIBILITY
                constexpr explicit operator GLenum() const { return attachment; }
                #else
                constexpr operator GLenum() const { return attachment; }
                #endif
                #endif

            private:
                constexpr explicit DrawAttachment(GLenum attachment): attachment(attachment) {}

                GLenum attachment;
        };

        /**
         * @brief %Buffer attachment
         *
         * @see @ref attachRenderbuffer(), @ref attachTexture1D(),
         *      @ref attachTexture2D(), @ref attachCubeMapTexture(),
         *      @ref attachTexture3D()
         */
        class MAGNUM_EXPORT BufferAttachment {
            public:
                /** @brief Depth buffer */
                static const BufferAttachment Depth;

                /** @brief Stencil buffer */
                static const BufferAttachment Stencil;

                #ifndef MAGNUM_TARGET_GLES2
                /**
                 * @brief Both depth and stencil buffer
                 *
                 * @requires_gles30 Combined depth and stencil attachment is
                 *      not available in OpenGL ES 2.0. Attach the same object
                 *      to both @ref Magnum::Framebuffer::BufferAttachment::Depth
                 *      "BufferAttachment::Depth" and @ref Magnum::Framebuffer::BufferAttachment::Stencil
                 *      "BufferAttachment::Stencil" instead.
                 * @todo Support this in ES2 (bind to both depth and stencil internally)
                 */
                static const BufferAttachment DepthStencil;
                #endif

                /** @brief Color buffer */
                constexpr /*implicit*/ BufferAttachment(Framebuffer::ColorAttachment attachment): attachment(GLenum(attachment)) {}

                #ifndef DOXYGEN_GENERATING_OUTPUT
                #ifndef CORRADE_GCC44_COMPATIBILITY
                constexpr explicit operator GLenum() const { return attachment; }
                #else
                constexpr operator GLenum() const { return attachment; }
                #endif
                #endif

            private:
                constexpr explicit BufferAttachment(GLenum attachment): attachment(attachment) {}

                GLenum attachment;
        };

        /**
         * @brief Invalidation attachment
         *
         * @see @ref invalidate()
         * @requires_gl43 %Extension @extension{ARB,invalidate_subdata}
         * @requires_gles30 %Extension @es_extension{EXT,discard_framebuffer}
         *      in OpenGL ES 2.0
         */
        class InvalidationAttachment {
            public:
                /** @brief Invalidate depth buffer */
                static const InvalidationAttachment Depth;

                /** @brief Invalidate stencil buffer */
                static const InvalidationAttachment Stencil;

                /** @brief Invalidate color buffer */
                constexpr /*implicit*/ InvalidationAttachment(Framebuffer::ColorAttachment attachment): attachment(GLenum(attachment)) {}

                #ifndef DOXYGEN_GENERATING_OUTPUT
                #ifndef CORRADE_GCC44_COMPATIBILITY
                constexpr explicit operator GLenum() const { return attachment; }
                #else
                constexpr operator GLenum() const { return attachment; }
                #endif
                #endif

            private:
                constexpr explicit InvalidationAttachment(GLenum attachment): attachment(attachment) {}

                GLenum attachment;
        };

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
             * @requires_gl Not available in OpenGL ES.
             */
            IncompleteDrawBuffer = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,

            /**
             * No object attached to read color attachment point
             * @requires_gl Not available in OpenGL ES.
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
             * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_multisample},
             *      @es_extension{APPLE,framebuffer_multisample},
             *      @es_extension{EXT,multisampled_render_to_texture} or
             *      @es_extension{NV,framebuffer_multisample} in OpenGL ES 2.0
             */
            #ifndef MAGNUM_TARGET_GLES2
            IncompleteMultisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
            #else
            IncompleteMultisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_APPLE,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Mismatched layered color attachments
             * @requires_gl Geometry shaders are not available in OpenGL ES.
             */
            IncompleteLayerTargets = GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
            #endif
        };

        /** @todo `GL_MAX_FRAMEBUFFER_WIDTH` etc. when @extension{ARB,framebuffer_no_attachments} is done */

        /**
         * @brief Max supported color attachment count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If ES extension @extension{NV,fbo_color_attachments}
         * is not available, returns `0`.
         * @see @ref mapForDraw(), @fn_gl{Get} with @def_gl{MAX_COLOR_ATTACHMENTS}
         */
        static Int maxColorAttachments();

        /**
         * @brief Constructor
         *
         * Generates new OpenGL framebuffer.
         * @see @ref setViewport(), @fn_gl{GenFramebuffers}
         */
        explicit Framebuffer(const Range2Di& viewport);

        /** @brief Copying is not allowed */
        Framebuffer(const Framebuffer&) = delete;

        /** @brief Move constructor */
        Framebuffer(Framebuffer&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL framebuffer.
         * @see @fn_gl{DeleteFramebuffers}
         */
        ~Framebuffer();

        /** @brief Copying is not allowed */
        Framebuffer& operator=(const Framebuffer&) = delete;

        /** @brief Move assignment */
        Framebuffer& operator=(Framebuffer&& other) noexcept;

        /** @brief OpenGL framebuffer ID */
        GLuint id() const { return _id; }

        /**
         * @brief %Framebuffer label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} nor @extension2{EXT,debug_label} desktop or ES
         * extension is available, this function returns empty string.
         * @see @fn_gl{GetObjectLabel} or
         *      @fn_gl_extension2{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{FRAMEBUFFER}
         */
        std::string label() const;

        /**
         * @brief Set framebuffer label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 is not supported and neither
         * @extension{KHR,debug} nor @extension2{EXT,debug_label} desktop or ES
         * extension is available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl{ObjectLabel} or
         *      @fn_gl_extension2{LabelObject,EXT,debug_label} with
         *      @def_gl{FRAMEBUFFER}
         */
        Framebuffer& setLabel(const std::string& label);

        /**
         * @brief Check framebuffer status
         * @param target    Target for which check the status
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{CheckFramebufferStatus} or
         *      @fn_gl_extension{CheckNamedFramebufferStatus,EXT,direct_state_access}
         */
        Status checkStatus(FramebufferTarget target);

        /**
         * @brief Map shader output to attachments
         * @return Reference to self (for method chaining)
         *
         * @p attachments is list of shader outputs mapped to framebuffer
         * color attachment IDs. %Shader outputs which are not listed are not
         * used, you can achieve the same by passing @ref Framebuffer::DrawAttachment::None
         * as color attachment ID. Example usage:
         * @code
         * framebuffer.mapForDraw({{MyShader::ColorOutput, Framebuffer::ColorAttachment(0)},
         *                         {MyShader::NormalOutput, Framebuffer::DrawAttachment::None}});
         * @endcode
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @ref maxDrawBuffers(), @ref maxDualSourceDrawBuffers(),
         *      @ref maxColorAttachments(), @ref mapForRead(),
         *      @fn_gl{BindFramebuffer}, @fn_gl{DrawBuffers} or
         *      @fn_gl_extension{FramebufferDrawBuffers,EXT,direct_state_access}
         * @requires_gles30 %Extension @es_extension2{NV,draw_buffers,GL_NV_draw_buffers}
         *      in OpenGL ES 2.0
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
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @ref maxColorAttachments(), @ref mapForRead(),
         *      @fn_gl{BindFramebuffer}, @fn_gl{DrawBuffer} or
         *      @fn_gl_extension{FramebufferDrawBuffer,EXT,direct_state_access},
         *      @fn_gl{DrawBuffers} in OpenGL ES 3.0
         * @requires_gles30 %Extension @es_extension2{NV,draw_buffers,GL_NV_draw_buffers}
         *      in OpenGL ES 2.0
         */
        Framebuffer& mapForDraw(DrawAttachment attachment);

        /**
         * @brief Map given color attachment for reading
         * @param attachment        Color attachment
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @ref mapForDraw(), @fn_gl{BindFramebuffer}, @fn_gl{ReadBuffer}
         *      or @fn_gl_extension{FramebufferReadBuffer,EXT,direct_state_access}
         * @requires_gles30 %Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
         *      in OpenGL ES 2.0
         */
        Framebuffer& mapForRead(ColorAttachment attachment);

        /**
         * @brief Invalidate framebuffer
         * @param attachments       Attachments to invalidate
         *
         * The framebuffer is bound to some target before the operation, if
         * not already.
         * @see @fn_gl{InvalidateFramebuffer} or @fn_gles_extension{DiscardFramebuffer,EXT,discard_framebuffer}
         *      on OpenGL ES 2.0
         * @requires_gl43 %Extension @extension{ARB,invalidate_subdata}. Use
         *      @ref Magnum::Framebuffer::clear() "clear()" instead where the
         *      extension is not supported.
         * @requires_gles30 %Extension @es_extension{EXT,discard_framebuffer}
         *      in OpenGL ES 2.0. Use @ref Magnum::Framebuffer::clear() "clear()"
         *      instead where the extension is not supported.
         */
        void invalidate(std::initializer_list<InvalidationAttachment> attachments);

        /**
         * @brief Invalidate framebuffer rectangle
         * @param attachments       Attachments to invalidate
         * @param rectangle         %Rectangle to invalidate
         *
         * The framebuffer is bound to some target before the operation, if
         * not already.
         * @see @fn_gl{InvalidateSubFramebuffer} or @fn_gles_extension{DiscardSubFramebuffer,EXT,discard_framebuffer}
         *      on OpenGL ES 2.0
         * @requires_gl43 %Extension @extension{ARB,invalidate_subdata}. Use
         *      @ref Magnum::Framebuffer::clear() "clear()" instead where the
         *      extension is not supported.
         * @requires_gles30 %Extension @es_extension{EXT,discard_framebuffer}
         *      in OpenGL ES 2.0. Use @ref Magnum::Framebuffer::clear() "clear()"
         *      instead where the extension is not supported.
         */
        void invalidate(std::initializer_list<InvalidationAttachment> attachments, const Range2Di& rectangle);

        /**
         * @brief Attach renderbuffer to given buffer
         * @param attachment        %Buffer attachment
         * @param renderbuffer      %Renderbuffer
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferRenderbuffer} or
         *      @fn_gl_extension{NamedFramebufferRenderbuffer,EXT,direct_state_access}
         */
        Framebuffer& attachRenderbuffer(BufferAttachment attachment, Renderbuffer& renderbuffer);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Attach texture to given buffer
         * @param attachment        %Buffer attachment
         * @param texture           %Texture
         * @param level             Mip level
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @ref attachCubeMapTexture(), @fn_gl{BindFramebuffer},
         *      @fn_gl2{FramebufferTexture1D,FramebufferTexture} or
         *      @fn_gl_extension{NamedFramebufferTexture1D,EXT,direct_state_access}
         * @requires_gl Only 2D and 3D textures are available in OpenGL ES.
         */
        Framebuffer& attachTexture(BufferAttachment attachment, Texture1D& texture, Int level);
        #endif

        /**
         * @brief Attach texture to given buffer
         * @param attachment        %Buffer attachment
         * @param texture           %Texture
         * @param level             Mip level
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @ref attachCubeMapTexture(), @fn_gl{BindFramebuffer},
         *      @fn_gl2{FramebufferTexture2D,FramebufferTexture} or
         *      @fn_gl_extension{NamedFramebufferTexture2D,EXT,direct_state_access}
         */
        Framebuffer& attachTexture(BufferAttachment attachment, Texture2D& texture, Int level);

        #ifndef MAGNUM_TARGET_GLES
        /** @overload
         * @requires_gl31 %Extension @extension{ARB,texture_rectangle}
         * @requires_gl Rectangle textures are not available in OpenGL ES.
         */
        Framebuffer& attachTexture(BufferAttachment attachment, RectangleTexture& texture, Int level);

        /** @overload
         * @requires_gl32 %Extension @extension{ARB,texture_multisample}
         * @requires_gl Multisample textures are not available in OpenGL ES.
         */
        Framebuffer& attachTexture(BufferAttachment attachment, MultisampleTexture2D& texture, Int level);
        #endif

        /**
         * @brief Attach cube map texture to given buffer
         * @param attachment        %Buffer attachment
         * @param texture           %Texture
         * @param coordinate        Cube map coordinate
         * @param level             Mip level
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @ref attachTexture2D(), @fn_gl{BindFramebuffer},
         *      @fn_gl2{FramebufferTexture2D,FramebufferTexture} or
         *      @fn_gl_extension{NamedFramebufferTexture2D,EXT,direct_state_access}
         */
        Framebuffer& attachCubeMapTexture(BufferAttachment attachment, CubeMapTexture& texture, CubeMapTexture::Coordinate coordinate, Int level);

        /**
         * @brief Attach texture layer to given buffer
         * @param attachment        %Buffer attachment
         * @param texture           %Texture
         * @param level             Mip level
         * @param layer             Layer
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl2{FramebufferTextureLayer,FramebufferTexture}
         *      or @fn_gl_extension{NamedFramebufferTextureLayer,EXT,direct_state_access},
         *      @fn_gles_extension{FramebufferTexture3D,OES,texture_3D} in OpenGL ES 2.0
         * @requires_gles30 %Extension @es_extension{OES,texture_3D} in OpenGL
         *      ES 2.0
         */
        Framebuffer& attachTextureLayer(BufferAttachment attachment, Texture3D& texture, Int level, Int layer);

        #ifndef MAGNUM_TARGET_GLES
        /** @overload
         * @requires_gl30 %Extension @extension{EXT,texture_array}
         * @requires_gl Only 2D array textures are available in OpenGL ES.
         */
        Framebuffer& attachTextureLayer(BufferAttachment attachment, Texture1DArray& texture, Int level, Int layer);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /** @overload
         * @requires_gl30 %Extension @extension{EXT,texture_array}
         * @requires_gles30 %Array textures are not available in OpenGL ES 2.0
         */
        Framebuffer& attachTextureLayer(BufferAttachment attachment, Texture2DArray& texture, Int level, Int layer);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /** @overload
         * @requires_gl40 %Extension @extension{ARB,texture_cube_map_array}
         * @requires_gl Cube map texture arrays are not available in OpenGL ES.
         */
        Framebuffer& attachTextureLayer(BufferAttachment attachment, CubeMapTextureArray& texture, Int level, Int layer);

        /** @overload
         * @requires_gl32 %Extension @extension{ARB,texture_multisample}
         * @requires_gl Multisample textures are not available in OpenGL ES.
         */
        Framebuffer& attachTextureLayer(BufferAttachment attachment, MultisampleTexture2DArray& texture, Int level, Int layer);
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        #ifndef MAGNUM_TARGET_GLES
        /**
         * @copybrief attachTexture()
         * @deprecated Use one of @ref Magnum::Framebuffer::attachTexture() "attachTexture()" overloads instead.
         */
        Framebuffer& attachTexture1D(BufferAttachment attachment, Texture1D& texture, Int level) {
            return attachTexture(attachment, texture, level);
        }
        #endif

        /**
         * @copybrief attachTexture()
         * @deprecated Use one of @ref Magnum::Framebuffer::attachTexture() "attachTexture()" overloads instead.
         */
        Framebuffer& attachTexture2D(BufferAttachment attachment, Texture2D& texture, Int level);

        /**
         * @copybrief attachTextureLayer()
         * @deprecated Use one of @ref Magnum::Framebuffer::attachTextureLayer() "attachTextureLayer()" overloads instead.
         */
        Framebuffer& attachTexture3D(BufferAttachment attachment, Texture3D& texture, Int level, Int layer) {
            return attachTextureLayer(attachment, texture, level, layer);
        }
        #endif

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Framebuffer& setViewport(const Range2Di& rectangle) {
            AbstractFramebuffer::setViewport(rectangle);
            return *this;
        }
        #endif

    private:
        void MAGNUM_LOCAL renderbufferImplementationDefault(BufferAttachment attachment, Renderbuffer& renderbuffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL renderbufferImplementationDSA(BufferAttachment attachment, Renderbuffer& renderbuffer);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL texture1DImplementationDefault(BufferAttachment attachment, GLuint textureId, GLint level);
        void MAGNUM_LOCAL texture1DImplementationDSA(BufferAttachment attachment, GLuint textureId, GLint level);
        #endif

        void MAGNUM_LOCAL texture2DImplementationDefault(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint level);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL texture2DImplementationDSA(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint level);
        #endif

        void MAGNUM_LOCAL textureLayerImplementationDefault(BufferAttachment attachment, GLuint textureId, GLint level, GLint layer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL textureLayerImplementationDSA(BufferAttachment attachment, GLuint textureId, GLint level, GLint layer);
        #endif
};

/** @debugoperatorclassenum{Magnum::Framebuffer,Magnum::Framebuffer::Status} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Framebuffer::Status value);

inline Framebuffer::Framebuffer(Framebuffer&& other) noexcept {
    _id = other._id;
    _viewport = other._viewport;
    other._id = 0;
    other._viewport = {};
}

inline Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    std::swap(_id, other._id);
    std::swap(_viewport, other._viewport);
    return *this;
}

}

#endif
