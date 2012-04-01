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

#include "CubeMapTexture.h"
#include "Renderbuffer.h"

namespace Magnum {

/**
@brief Framebuffer
*/
class MAGNUM_EXPORT Framebuffer {
    Framebuffer(const Framebuffer& other) = delete;
    Framebuffer(Framebuffer&& other) = delete;
    Framebuffer& operator=(const Framebuffer& other) = delete;
    Framebuffer& operator=(Framebuffer&& other) = delete;

    public:
        /** @brief %Framebuffer target */
        enum class Target: GLenum {
            Read = GL_READ_FRAMEBUFFER,     /**< For reading only. */
            Draw = GL_DRAW_FRAMEBUFFER,     /**< For drawing only. */
            ReadDraw = GL_FRAMEBUFFER       /**< For both reading and drawing. */
        };

        /** @brief Draw attachment for default framebuffer */
        enum class DefaultDrawAttachment: GLenum {
            None = GL_NONE,                 /**< Don't use the output. */
            BackLeft = GL_BACK_LEFT,        /**< Write output to back left framebuffer. */
            BackRight = GL_BACK_RIGHT,      /**< Write output to back right framebuffer. */
            FrontLeft = GL_FRONT_LEFT,      /**< Write output to front left framebuffer. */
            FrontRight = GL_FRONT_RIGHT     /**< Write output to front right framebuffer. */
        };

        /** @brief Read attachment for default framebuffer */
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

        /** @brief Attachment for depth/stencil part of fragment shader output */
        enum class DepthStencilAttachment: GLenum {
            Depth = GL_DEPTH_ATTACHMENT,    /**< Depth output only. */
            Stencil = GL_STENCIL_ATTACHMENT, /**< Stencil output only. */
            DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT /**< Both depth and stencil output. */
        };

        /**
         * @brief Output mask for blitting
         *
         * Specifies which data are copied when performing blit operation
         * using blit().
         */
        enum class BlitMask: GLbitfield {
            Color = GL_COLOR_BUFFER_BIT,    /**< Color only. */
            Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth value only. */
            Stencil = GL_STENCIL_BUFFER_BIT, /**< Stencil value only. */

            /** Color and depth value. */
            ColorDepth = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,

            /** Color and stencil value. */
            ColorStencil = GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT,

            /** Depth and stencil value */
            DepthStencil = GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT,

            /** Color, depth and stencil value. */
            ColorDepthStencil = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT
        };

        /**
         * @brief Bind default framebuffer to given target
         * @param target     %Target
         */
        inline static void bindDefault(Target target) {
            glBindFramebuffer(static_cast<GLenum>(target), 0);
        }

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
         * @see mapForDraw(), mapDefaultForRead()
         */
        static void mapDefaultForDraw(std::initializer_list<DefaultDrawAttachment> attachments);

        /**
         * @brief Map given attachment of default framebuffer for reading
         * @param attachment        Default attachment
         *
         * Each used attachment should have either renderbuffer or texture
         * attached to work properly.
         * @see mapForRead(), mapDefaultForDraw()
         */
        inline static void mapDefaultForRead(DefaultReadAttachment attachment) {
            bindDefault(Target::Read);
            glReadBuffer(static_cast<GLenum>(attachment));
        }

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
         */
        inline static void blit(const Math::Vector2<GLint>& bottomLeft, const Math::Vector2<GLint>& topRight, BlitMask blitMask) {
            glBlitFramebuffer(bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), static_cast<GLbitfield>(blitMask), static_cast<GLenum>(AbstractTexture::Filter::NearestNeighbor));
        }

        /**
         * @brief Constructor
         *
         * Generates new OpenGL framebuffer.
         */
        inline Framebuffer() { glGenFramebuffers(1, &framebuffer); }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL framebuffer.
         */
        inline ~Framebuffer() { glDeleteFramebuffers(1, &framebuffer); }

        /** @brief Bind framebuffer */
        inline void bind(Target target) {
            glBindFramebuffer(static_cast<GLenum>(target), framebuffer);
        }

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
         * @see mapDefaultForDraw(), mapForRead()
         */
        void mapForDraw(std::initializer_list<int> colorAttachments);

        /**
         * @brief Map given color attachment of current framebuffer for reading
         * @param colorAttachment   Color attachment ID
         *
         * The color attachment should have either renderbuffer or texture
         * attached for reading to work properly.
         * @see mapDefaultForRead(), mapForDraw()
         */
        inline void mapForRead(unsigned int colorAttachment) {
            bind(Target::Read);
            glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachment);
        }

        /**
         * @brief Attach renderbuffer to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param renderbuffer      Renderbuffer
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
         * @param renderbuffer      Renderbuffer
         */
        inline void attachRenderbuffer(Target target, unsigned int colorAttachment, Renderbuffer* renderbuffer) {
            /** @todo Check for internal format compatibility */
            bind(target);
            glFramebufferRenderbuffer(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, GL_RENDERBUFFER, renderbuffer->id());
        }

        /**
         * @brief Attach 1D texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           1D texture
         * @param mipLevel          Mip level
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
         */
        inline void attachTexture1D(Target target, unsigned int colorAttachment, Texture1D* texture, GLint mipLevel) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture1D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel);
        }

        /**
         * @brief Attach 2D texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           2D texture
         * @param mipLevel          Mip level. For rectangle textures it
         *      should be always 0.
         *
         * @see attachCubeMapTexture()
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
         * @see attachCubeMapTexture()
         */
        inline void attachTexture2D(Target target, unsigned int colorAttachment, Texture2D* texture, GLint mipLevel) {
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
         * @see attachTexture2D()
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
         * @see attachTexture2D()
         */
        inline void attachCubeMapTexture(Target target, unsigned int colorAttachment, CubeMapTexture* texture, CubeMapTexture::Coordinate coordinate, GLint mipLevel) {
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
         */
        inline void attachTexture3D(Target target, unsigned int colorAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for internal format compatibility */
            /** @todo Check for texture target compatibility */
            bind(target);
            glFramebufferTexture3D(static_cast<GLenum>(target), GL_COLOR_ATTACHMENT0 + colorAttachment, static_cast<GLenum>(texture->target()), texture->id(), mipLevel, layer);
        }

    private:
        GLuint framebuffer;
};

}

#endif
