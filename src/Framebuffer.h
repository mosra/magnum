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
#include "Texture.h"
#include "Renderbuffer.h"

namespace Magnum {

/**
@brief %Framebuffer

@see DefaultFramebuffer
@requires_gl30 %Extension @extension{EXT,framebuffer_object}
*/
class MAGNUM_EXPORT Framebuffer: public AbstractFramebuffer {
    public:
        /**
         * @brief Constructor
         *
         * Generates new OpenGL framebuffer.
         * @see @fn_gl{GenFramebuffers}
         */
        inline Framebuffer() { glGenFramebuffers(1, &_id); }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL framebuffer.
         * @see @fn_gl{DeleteFramebuffers}
         */
        ~Framebuffer();

        /**
         * @brief Map given color attachments of current framebuffer for drawing
         * @param colorAttachments  Color attachment IDs. If any value is -1,
         *      given output is not used.
         *
         * @see mapForRead(), @fn_gl{BindFramebuffer}, @fn_gl{DrawBuffers}
         * @requires_gles30 %Extension @es_extension2{NV,draw_buffers,GL_NV_draw_buffers}
         */
        void mapForDraw(std::initializer_list<std::int8_t> colorAttachments);

        /**
         * @brief Map given color attachment of current framebuffer for reading
         * @param colorAttachment   Color attachment ID
         *
         * @see mapForDraw(), @fn_gl{BindFramebuffer}, @fn_gl{ReadBuffer}
         * @requires_gles30 %Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
         */
        inline void mapForRead(std::uint8_t colorAttachment) {
            bind(Target::Read);
            /** @todo Get some extension wrangler instead to avoid undeclared glReadBuffer() on ES2 */
            #ifndef MAGNUM_TARGET_GLES2
            glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachment);
            #else
            static_cast<void>(colorAttachment);
            #endif
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
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param renderbuffer      %Renderbuffer
         *
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferRenderbuffer}
         */
        inline void attachRenderbuffer(Target target, DepthStencilAttachment depthStencilAttachment, Renderbuffer* renderbuffer) {
            bind(target);
            glFramebufferRenderbuffer(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), GL_RENDERBUFFER, renderbuffer->id());
        }

        /**
         * @brief Attach renderbuffer to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param renderbuffer      %Renderbuffer
         *
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferRenderbuffer}
         */
        inline void attachRenderbuffer(Target target, std::uint8_t colorAttachment, Renderbuffer* renderbuffer) {
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
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture}
         * @requires_gl Only 2D and 3D textures are available in OpenGL ES.
         */
        inline void attachTexture1D(Target target, DepthStencilAttachment depthStencilAttachment, Texture1D* texture, GLint mipLevel) {
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
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture}
         * @requires_gl Only 2D and 3D textures are available in OpenGL ES.
         */
        inline void attachTexture1D(Target target, std::uint8_t colorAttachment, Texture1D* texture, GLint mipLevel) {
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
         * @see attachCubeMapTexture(), @fn_gl{BindFramebuffer},
         *      @fn_gl{FramebufferTexture}
         */
        inline void attachTexture2D(Target target, DepthStencilAttachment depthStencilAttachment, Texture2D* texture, GLint mipLevel) {
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
         * @see attachCubeMapTexture(), @fn_gl{BindFramebuffer},
         *      @fn_gl{FramebufferTexture}
         */
        inline void attachTexture2D(Target target, std::uint8_t colorAttachment, Texture2D* texture, GLint mipLevel) {
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
         * @see attachTexture2D(), @fn_gl{BindFramebuffer},
         *      @fn_gl{FramebufferTexture}
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
         * @see attachTexture2D(), @fn_gl{BindFramebuffer},
         *      @fn_gl{FramebufferTexture}
         */
        inline void attachCubeMapTexture(Target target, std::uint8_t colorAttachment, CubeMapTexture* texture, CubeMapTexture::Coordinate coordinate, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            bind(target);
            glFramebufferTexture2D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(coordinate), texture->id(), mipLevel);
        }

        /**
         * @brief Attach 3D texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           3D texture
         * @param mipLevel          Mip level
         * @param layer             Layer of 2D image within a 3D texture
         *
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture}
         * @requires_es_extension %Extension @es_extension{OES,texture_3D}
         */
        inline void attachTexture3D(Target target, DepthStencilAttachment depthStencilAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for texture target compatibility */
            bind(target);
            /** @todo Get some extension wrangler for glFramebufferTexture3D() (extension only) */
            #ifndef MAGNUM_TARGET_GLES
            glFramebufferTexture3D(static_cast<GLenum>(target), static_cast<GLenum>(depthStencilAttachment), static_cast<GLenum>(texture->target()), texture->id(), mipLevel, layer);
            #else
            static_cast<void>(depthStencilAttachment);
            static_cast<void>(texture);
            static_cast<void>(mipLevel);
            static_cast<void>(layer);
            #endif
        }

        /**
         * @brief Attach 3D texture to given framebuffer color attachment
         * @param target            %Target
         * @param colorAttachment   Color attachment ID (number between 0 and 15)
         * @param texture           3D texture
         * @param mipLevel          Mip level
         * @param layer             Layer of 2D image within a 3D texture.
         *
         * @see @fn_gl{BindFramebuffer}, @fn_gl{FramebufferTexture}
         * @requires_es_extension %Extension @es_extension{OES,texture_3D}
         */
        inline void attachTexture3D(Target target, std::uint8_t colorAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for texture target compatibility */
            bind(target);
            /** @todo Get some extension wrangler for glFramebufferTexture3D() (extension only) */
            #ifndef MAGNUM_TARGET_GLES
            glFramebufferTexture3D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel, layer);
            #else
            static_cast<void>(colorAttachment);
            static_cast<void>(texture);
            static_cast<void>(mipLevel);
            static_cast<void>(layer);
            #endif
        }
};

}

#endif
