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

framebuffer.attachTexture2D(0, &color);
framebuffer.attachTexture2D(1, &normal);
framebuffer.attachRenderbuffer(Framebuffer::DepthStencilAttachment::DepthStencil, &depthStencil);
@endcode

Then you need to map outputs of your shader to color attachments in the
framebuffer:
@code
framebuffer.mapForDraw({{MyShader::ColorOutput, 0},
                        {MyShader::NormalOutput, 1}});
@endcode

The actual @ref Platform::GlutApplication::drawEvent() "drawEvent()" might
look like this. First you clear all buffers you need, perform drawing to
off-screen framebuffer, then bind the default and render the textures on
screen:
@code
void drawEvent() {
    defaultFramebuffer.clear(AbstractFramebuffer::Clear::Color)
    framebuffer.clear(AbstractFramebuffer::Clear::Color|AbstractFramebuffer::Clear::Depth|AbstractFramebuffer::Clear::Stencil);

    framebuffer.bind(AbstractFramebuffer::Target::Draw);
    // ...

    defaultFramebuffer.bind(AbstractFramebuffer::Target::Draw);
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

@requires_gl30 %Extension @extension{EXT,framebuffer_object}
*/
class MAGNUM_EXPORT Framebuffer: public AbstractFramebuffer {
    friend class Context;

    public:
        /**
         * @brief Constructor
         *
         * Generates new OpenGL framebuffer.
         * @see @fn_gl{GenFramebuffers}
         */
        explicit Framebuffer(const Vector2i& viewportPosition, const Vector2i& viewportSize);

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL framebuffer.
         * @see @fn_gl{DeleteFramebuffers}
         */
        ~Framebuffer();

        /**
         * @brief Map shader output to color attachment
         *
         * @p attachments is list of shader outputs mapped to framebuffer
         * color attachment IDs. Shader outputs which are not listed are not
         * used, you can achieve the same by passing `-1` as color attachment
         * ID. Example usage:
         * @code
         * framebuffer.mapForDraw({{MyShader::ColorOutput, 0},
         *                         {MyShader::NormalOutput, 1}});
         * @endcode
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see mapForRead(), @fn_gl{BindFramebuffer}, @fn_gl{DrawBuffers} or
         *      @fn_gl_extension{FramebufferDrawBuffers,EXT,direct_state_access}
         * @requires_gles30 %Extension @es_extension2{NV,draw_buffers,GL_NV_draw_buffers}
         */
        void mapForDraw(std::initializer_list<std::pair<GLuint, std::int8_t>> attachments);

        /**
         * @brief Map shader output to color attachment
         * @param attachment        Color attachment ID
         *
         * Similar to above function, can be used in cases when shader has
         * only one (unnamed) output.
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see mapForRead(), @fn_gl{BindFramebuffer}, @fn_gl{DrawBuffer} or
         *      @fn_gl_extension{FramebufferDrawBuffer,EXT,direct_state_access}
         * @requires_gles30 %Extension @es_extension2{NV,draw_buffers,GL_NV_draw_buffers}
         */
        inline void mapForDraw(std::int8_t attachment) {
            (this->*drawBufferImplementation)(static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + attachment));
        }

        /**
         * @brief Map given color attachment for reading
         * @param attachment        Color attachment ID
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see mapForDraw(), @fn_gl{BindFramebuffer}, @fn_gl{ReadBuffer} or
         *      @fn_gl_extension{FramebufferReadBuffer,EXT,direct_state_access}
         * @requires_gles30 %Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
         */
        inline void mapForRead(std::uint8_t attachment) {
            (this->*readBufferImplementation)(GL_COLOR_ATTACHMENT0 + attachment);
        }

        /**
         * @brief Attachment for depth/stencil part of fragment shader output
         *
         * @see attachRenderbuffer(), attachTexture1D(), attachTexture2D(),
         *      attachCubeMapTexture(), attachTexture3D()
         */
        enum class DepthStencilAttachment: GLenum {
            Depth = GL_DEPTH_ATTACHMENT,    /**< Depth output only. */

            Stencil = GL_STENCIL_ATTACHMENT /**< Stencil output only. */

            #ifndef MAGNUM_TARGET_GLES2
            ,
            /**
             * Both depth and stencil output.
             * @requires_gles30 Combined depth and stencil attachment is not
             *      available in OpenGL ES 2.0.
             */
            DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT
            #endif
        };

        /**
         * @brief Attach renderbuffer to given framebuffer depth/stencil attachment
         * @param depthStencilAttachment Depth/stencil attachment
         * @param renderbuffer      %Renderbuffer
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferRenderbuffer} or
         *      @fn_gl_extension{NamedFramebufferRenderbuffer,EXT,direct_state_access}
         */
        inline void attachRenderbuffer(DepthStencilAttachment depthStencilAttachment, Renderbuffer* renderbuffer) {
            (this->*renderbufferImplementation)(GLenum(depthStencilAttachment), renderbuffer);
        }

        /**
         * @brief Attach renderbuffer to given framebuffer color attachment
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param renderbuffer      %Renderbuffer
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferRenderbuffer} or
         *      @fn_gl_extension{NamedFramebufferRenderbuffer,EXT,direct_state_access}
         */
        inline void attachRenderbuffer(std::uint8_t colorAttachment, Renderbuffer* renderbuffer) {
            (this->*renderbufferImplementation)(GL_COLOR_ATTACHMENT0 + colorAttachment, renderbuffer);
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Attach 1D texture to given framebuffer depth/stencil attachment
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           1D texture
         * @param mipLevel          Mip level
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture} or
         *      @fn_gl_extension{NamedFramebufferTexture1D,EXT,direct_state_access}
         * @requires_gl Only 2D and 3D textures are available in OpenGL ES.
         */
        inline void attachTexture1D(DepthStencilAttachment depthStencilAttachment, Texture1D* texture, GLint mipLevel) {
            (this->*texture1DImplementation)(GLenum(depthStencilAttachment), texture, mipLevel);
        }

        /**
         * @brief Attach 1D texture to given framebuffer color attachment
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           1D texture
         * @param mipLevel          Mip level
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture} or
         *      @fn_gl_extension{NamedFramebufferTexture1D,EXT,direct_state_access}
         * @requires_gl Only 2D and 3D textures are available in OpenGL ES.
         */
        inline void attachTexture1D(std::uint8_t colorAttachment, Texture1D* texture, GLint mipLevel) {
            (this->*texture1DImplementation)(GL_COLOR_ATTACHMENT0 + colorAttachment, texture, mipLevel);
        }
        #endif

        /**
         * @brief Attach 2D texture to given framebuffer depth/stencil attachment
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           2D texture
         * @param mipLevel          Mip level. For rectangle textures it
         *      should be always 0.
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see attachCubeMapTexture(), @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture}
         *      or @fn_gl_extension{NamedFramebufferTexture2D,EXT,direct_state_access}
         */
        void attachTexture2D(DepthStencilAttachment depthStencilAttachment, Texture2D* texture, GLint mipLevel);

        /**
         * @brief Attach 2D texture to given framebuffer color attachment
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           2D texture
         * @param mipLevel          Mip level. For rectangle textures it
         *      should be always 0.
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see attachCubeMapTexture(), @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture}
         *      or @fn_gl_extension{NamedFramebufferTexture2D,EXT,direct_state_access}
         */
        void attachTexture2D(std::uint8_t colorAttachment, Texture2D* texture, GLint mipLevel);

        /**
         * @brief Attach cube map texture to given framebuffer depth/stencil attachment
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           Cube map texture
         * @param coordinate        Cube map coordinate
         * @param mipLevel          Mip level
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see attachTexture2D(), @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture}
         *      or @fn_gl_extension{NamedFramebufferTexture2D,EXT,direct_state_access}
         */
        inline void attachCubeMapTexture(DepthStencilAttachment depthStencilAttachment, CubeMapTexture* texture, CubeMapTexture::Coordinate coordinate, GLint mipLevel) {
            (this->*texture2DImplementation)(GLenum(depthStencilAttachment), GLenum(coordinate), texture->id(), mipLevel);
        }

        /**
         * @brief Attach cube map texture to given framebuffer color attachment
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           Cube map texture
         * @param coordinate        Cube map coordinate
         * @param mipLevel          Mip level
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see attachTexture2D(), @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture}
         *      or @fn_gl_extension{NamedFramebufferTexture2D,EXT,direct_state_access}
         */
        inline void attachCubeMapTexture(std::uint8_t colorAttachment, CubeMapTexture* texture, CubeMapTexture::Coordinate coordinate, GLint mipLevel) {
            (this->*texture2DImplementation)(GL_COLOR_ATTACHMENT0 + colorAttachment, GLenum(coordinate), texture->id(), mipLevel);
        }

        /**
         * @brief Attach 3D texture to given framebuffer depth/stencil attachment
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           3D texture
         * @param mipLevel          Mip level
         * @param layer             Layer of 2D image within a 3D texture
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture} or
         *      @fn_gl_extension{NamedFramebufferTexture3D,EXT,direct_state_access}
         * @requires_es_extension %Extension @es_extension{OES,texture_3D}
         */
        inline void attachTexture3D(DepthStencilAttachment depthStencilAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for texture target compatibility */
            (this->*texture3DImplementation)(GLenum(depthStencilAttachment), texture, mipLevel, layer);
        }

        /**
         * @brief Attach 3D texture to given framebuffer color attachment
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           3D texture
         * @param mipLevel          Mip level
         * @param layer             Layer of 2D image within a 3D texture.
         *
         * If @extension{EXT,direct_state_access} is not available and the
         * framebufferbuffer is not currently bound, it is bound before the
         * operation.
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture} or
         *      @fn_gl_extension{NamedFramebufferTexture3D,EXT,direct_state_access}
         * @requires_es_extension %Extension @es_extension{OES,texture_3D}
         */
        inline void attachTexture3D(std::uint8_t colorAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for texture target compatibility */
            (this->*texture3DImplementation)(GL_COLOR_ATTACHMENT0 + colorAttachment, texture, mipLevel, layer);
        }

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

        typedef void(Framebuffer::*RenderbufferImplementation)(GLenum, Renderbuffer*);
        void MAGNUM_LOCAL renderbufferImplementationDefault(GLenum attachment, Renderbuffer* renderbuffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL renderbufferImplementationDSA(GLenum attachment, Renderbuffer* renderbuffer);
        #endif
        static RenderbufferImplementation renderbufferImplementation;

        #ifndef MAGNUM_TARGET_GLES
        typedef void(Framebuffer::*Texture1DImplementation)(GLenum, Texture1D*, GLint);
        void MAGNUM_LOCAL texture1DImplementationDefault(GLenum attachment, Texture1D* texture, GLint mipLevel);
        void MAGNUM_LOCAL texture1DImplementationDSA(GLenum attachment, Texture1D* texture, GLint mipLevel);
        static Texture1DImplementation texture1DImplementation;
        #endif

        typedef void(Framebuffer::*Texture2DImplementation)(GLenum, GLenum, GLuint, GLint);
        void MAGNUM_LOCAL texture2DImplementationDefault(GLenum attachment, GLenum textureTarget, GLuint textureId, GLint mipLevel);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL texture2DImplementationDSA(GLenum attachment, GLenum textureTarget, GLuint textureId, GLint mipLevel);
        #endif
        static MAGNUM_LOCAL Texture2DImplementation texture2DImplementation;

        typedef void(Framebuffer::*Texture3DImplementation)(GLenum, Texture3D*, GLint, GLint);
        void MAGNUM_LOCAL texture3DImplementationDefault(GLenum attachment, Texture3D* texture, GLint mipLevel, GLint layer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL texture3DImplementationDSA(GLenum attachment, Texture3D* texture, GLint mipLevel, GLint layer);
        #endif
        static Texture3DImplementation texture3DImplementation;
};

}

#endif
