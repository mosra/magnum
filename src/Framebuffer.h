#ifndef Magnum_Framebuffer_h
#define Magnum_Framebuffer_h
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
 * @brief Class Magnum::Framebuffer
 */

#include "AbstractFramebuffer.h"
#include "CubeMapTexture.h"

namespace Magnum {

/**
@brief %Framebuffer

Unlike DefaultFramebuffer, which is used for on-screen rendering, this class
is used for off-screen rendering, usable either in windowless applications,
texture generation or for various post-processing effects.

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

The actual @ref Platform::GlutApplication::drawEvent() "drawEvent()" might
look like this. First you clear all buffers you need, perform drawing to
off-screen framebuffer, then bind the default and render the textures on
screen:
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
mapForDraw(), mapForRead(), attachRenderbuffer(), attachTexture1D(),
attachTexture2D(), attachCubeMapTexture() and attachTexture3D() use DSA
to avoid unnecessary calls to @fn_gl{BindFramebuffer}. See their respective
documentation for more information.

@requires_gl30 %Extension @extension{ARB,framebuffer_object}
*/
class MAGNUM_EXPORT Framebuffer: public AbstractFramebuffer {
    friend class Context;

    public:
        /**
         * @brief Color attachment
         *
         * @see Attachment, attachRenderbuffer(), attachTexture1D(),
         *      attachTexture2D(), attachCubeMapTexture(), attachTexture3D()
         */
        class ColorAttachment {
            friend class Framebuffer;

            public:
                /**
                 * @brief Constructor
                 * @param id        Color attachment id
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
         * @see mapForDraw()
         */
        class DrawAttachment {
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
         * @brief %Buffer attachment
         *
         * @see attachRenderbuffer(), attachTexture1D(), attachTexture2D(),
         *      attachCubeMapTexture(), attachTexture3D()
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
                 *      not available in OpenGL ES 2.0.
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

        /**
         * @brief Invalidation attachment
         *
         * @see invalidate()
         * @requires_gl43 %Extension @extension{ARB,invalidate_subdata}
         * @requires_gles30 %Extension @es_extension{EXT,discard_framebuffer}
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
                constexpr explicit operator GLenum() const { return attachment; }
                #endif

            private:
                constexpr explicit InvalidationAttachment(GLenum attachment): attachment(attachment) {}

                GLenum attachment;
        };

        /**
         * @brief Status
         *
         * @see checkStatus()
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
             *      @es_extension{NV,framebuffer_multisample}
             */
            #ifndef MAGNUM_TARGET_GLES2
            IncompleteMultisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
            #else
            IncompleteMultisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_APPLE,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /** @todo Why exactly this is not needed? */
            /**
             * Mismatched layered color attachments
             * @requires_gl Not available in OpenGL ES.
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
         * @see setViewport(), @fn_gl{GenFramebuffers}
         */
        explicit Framebuffer(const Range2Di& viewport);

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL framebuffer.
         * @see @fn_gl{DeleteFramebuffers}
         */
        ~Framebuffer();

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
        Status checkStatus(FramebufferTarget target) {
            return Status((this->*checkStatusImplementation)(target));
        }

        /**
         * @brief Map shader output to attachments
         * @return Reference to self (for method chaining)
         *
         * @p attachments is list of shader outputs mapped to framebuffer
         * color attachment IDs. %Shader outputs which are not listed are not
         * used, you can achieve the same by passing Framebuffer::DrawAttachment::None
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
         */
        Framebuffer& mapForDraw(DrawAttachment attachment) {
            (this->*drawBufferImplementation)(GLenum(attachment));
            return *this;
        }

        /**
         * @brief Invalidate framebuffer
         * @param attachments       Attachments to invalidate
         *
         * The framebuffer is bound to some target before the operation, if
         * not already.
         * @see @fn_gl{InvalidateFramebuffer} or @fn_gles_extension{DiscardFramebuffer,EXT,discard_framebuffer}
         *      on OpenGL ES 2.0
         * @requires_gl43 %Extension @extension{ARB,invalidate_subdata}. Use
         *      clear() instead where the extension is not supported.
         * @requires_gles30 %Extension @es_extension{EXT,discard_framebuffer}.
         *      Use clear() instead where the extension is not supported.
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
         *      clear() instead where the extension is not supported.
         * @requires_gles30 %Extension @es_extension{EXT,discard_framebuffer}.
         *      Use clear() instead where the extension is not supported.
         */
        void invalidate(std::initializer_list<InvalidationAttachment> attachments, const Range2Di& rectangle);

        /**
         * @brief Map given color attachment for reading
         * @param attachment        Color attachment
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see mapForDraw(), @fn_gl{BindFramebuffer}, @fn_gl{ReadBuffer} or
         *      @fn_gl_extension{FramebufferReadBuffer,EXT,direct_state_access}
         * @requires_gles30 %Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
         */
        Framebuffer& mapForRead(ColorAttachment attachment) {
            (this->*readBufferImplementation)(GLenum(attachment));
            return *this;
        }

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
        Framebuffer& attachRenderbuffer(BufferAttachment attachment, Renderbuffer& renderbuffer) {
            (this->*renderbufferImplementation)(attachment, renderbuffer);
            return *this;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Attach 1D texture to given buffer
         * @param attachment        %Buffer attachment
         * @param texture           1D texture
         * @param level             Mip level
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture} or
         *      @fn_gl_extension{NamedFramebufferTexture1D,EXT,direct_state_access}
         * @requires_gl Only 2D and 3D textures are available in OpenGL ES.
         */
        Framebuffer& attachTexture1D(BufferAttachment attachment, Texture1D& texture, Int level) {
            (this->*texture1DImplementation)(attachment, texture, level);
            return *this;
        }
        #endif

        /**
         * @brief Attach 2D texture to given buffer
         * @param attachment        %Buffer attachment
         * @param texture           2D texture
         * @param level             Mip level
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see attachCubeMapTexture(), @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture}
         *      or @fn_gl_extension{NamedFramebufferTexture2D,EXT,direct_state_access}
         */
        Framebuffer& attachTexture2D(BufferAttachment attachment, Texture2D& texture, Int level);

        /**
         * @brief Attach cube map texture to given buffer
         * @param attachment        %Buffer attachment
         * @param texture           Cube map texture
         * @param coordinate        Cube map coordinate
         * @param level             Mip level
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see attachTexture2D(), @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture}
         *      or @fn_gl_extension{NamedFramebufferTexture2D,EXT,direct_state_access}
         */
        Framebuffer& attachCubeMapTexture(BufferAttachment attachment, CubeMapTexture& texture, CubeMapTexture::Coordinate coordinate, Int level) {
            (this->*texture2DImplementation)(attachment, GLenum(coordinate), texture.id(), level);
            return *this;
        }

        /**
         * @brief Attach 3D texture to given buffer
         * @param attachment        %Buffer attachment
         * @param texture           3D texture
         * @param level             Mip level
         * @param layer             Layer of 2D image within a 3D texture
         * @return Reference to self (for method chaining)
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture} or
         *      @fn_gl_extension{NamedFramebufferTexture3D,EXT,direct_state_access}
         * @requires_es_extension %Extension @es_extension{OES,texture_3D}
         */
        Framebuffer& attachTexture3D(BufferAttachment attachment, Texture3D& texture, Int level, Int layer) {
            /** @todo Check for texture target compatibility */
            (this->*texture3DImplementation)(attachment, texture, level, layer);
            return *this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        Framebuffer& setViewport(const Range2Di& rectangle) {
            AbstractFramebuffer::setViewport(rectangle);
            return *this;
        }
        #endif

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context& context);

        typedef void(Framebuffer::*RenderbufferImplementation)(BufferAttachment, Renderbuffer&);
        void MAGNUM_LOCAL renderbufferImplementationDefault(BufferAttachment attachment, Renderbuffer& renderbuffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL renderbufferImplementationDSA(BufferAttachment attachment, Renderbuffer& renderbuffer);
        #endif
        static RenderbufferImplementation renderbufferImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(Framebuffer::*Texture1DImplementation)(BufferAttachment, Texture1D&, GLint);
        void MAGNUM_LOCAL texture1DImplementationDefault(BufferAttachment attachment, Texture1D& texture, GLint level);
        void MAGNUM_LOCAL texture1DImplementationDSA(BufferAttachment attachment, Texture1D& texture, GLint level);
        static Texture1DImplementation texture1DImplementation;
        #endif

        typedef void(Framebuffer::*Texture2DImplementation)(BufferAttachment, GLenum, GLuint, GLint);
        void MAGNUM_LOCAL texture2DImplementationDefault(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint level);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL texture2DImplementationDSA(BufferAttachment attachment, GLenum textureTarget, GLuint textureId, GLint level);
        #endif
        static MAGNUM_LOCAL Texture2DImplementation texture2DImplementation;

        typedef void(Framebuffer::*Texture3DImplementation)(BufferAttachment, Texture3D&, GLint, GLint);
        void MAGNUM_LOCAL texture3DImplementationDefault(BufferAttachment attachment, Texture3D& texture, GLint level, GLint layer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL texture3DImplementationDSA(BufferAttachment attachment, Texture3D& texture, GLint level, GLint layer);
        #endif
        static Texture3DImplementation texture3DImplementation;
};

/** @debugoperator{DefaultFramebuffer} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Framebuffer::Status value);

}

#endif
