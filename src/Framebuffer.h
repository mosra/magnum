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
         * @brief Set viewport size
         *
         * Call when window size changes.
         * @see @fn_gl{Viewport}
         */
        inline static void setViewport(const Vector2i& position, const Vector2i& size) {
            glViewport(position.x(), position.y(), size.x(), size.y());
        }

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
         * @see clear(), Renderer::setClearColor(), Renderer::setClearDepth(),
         *      Renderer::setClearStencil(), @fn_gl{Clear}
         * @todo Clearing only given draw buffer
         */
        inline static void clear(ClearMask mask) { glClear(static_cast<GLbitfield>(mask)); }

        /** @{ @name Framebuffer creation and binding */

        /**
         * @brief %Framebuffer target
         *
         * @see bind(), bindDefault()
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        enum class Target: GLenum {
            /**
             * For reading only.
             * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
             * @requires_gles30 %Extension @es_extension{APPLE,framebuffer_multisample}
             *      or @es_extension{ANGLE,framebuffer_blit}
             */
            #ifndef MAGNUM_TARGET_GLES2
            Read = GL_READ_FRAMEBUFFER,
            #else
            Read = GL_READ_FRAMEBUFFER_APPLE,
            #endif

            /**
             * For drawing only.
             * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
             * @requires_gles30 %Extension @es_extension{APPLE,framebuffer_multisample}
             *      or @es_extension{ANGLE,framebuffer_blit}
             */
            #ifndef MAGNUM_TARGET_GLES2
            Draw = GL_DRAW_FRAMEBUFFER,
            #else
            Draw = GL_DRAW_FRAMEBUFFER_APPLE,
            #endif

            ReadDraw = GL_FRAMEBUFFER       /**< For both reading and drawing. */
        };

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Draw attachment for default framebuffer
         *
         * @see mapDefaultForDraw()
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_gles30 Draw attachments for default framebuffer are
         *      available only in OpenGL ES 3.0.
         */
        enum class DefaultDrawAttachment: GLenum {
            /** Don't use the output. */
            None = GL_NONE,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Write output to back left framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackLeft = GL_BACK_LEFT,

            /**
             * Write output to back right framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackRight = GL_BACK_RIGHT,

            /**
             * Write output to front left framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontLeft = GL_FRONT_LEFT,

            /**
             * Write output to front right framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontRight = GL_FRONT_RIGHT,
            #endif

            /**
             * Write output to back framebuffer.
             *
             * On desktop OpenGL, this is equal to
             * @ref Magnum::Framebuffer::DefaultDrawAttachment "DefaultDrawAttachment::BackLeft".
             */
            #ifdef MAGNUM_TARGET_GLES
            Back = GL_BACK,
            #else
            Back = GL_BACK_LEFT,
            #endif

            /**
             * Write output to front framebuffer.
             *
             * On desktop OpenGL, this is equal to
             * @ref Magnum::Framebuffer::DefaultDrawAttachment "DefaultDrawAttachment::FrontLeft".
             */
            #ifdef MAGNUM_TARGET_GLES
            Front = GL_FRONT
            #else
            Front = GL_FRONT_LEFT
            #endif
        };
        #endif

        /**
         * @brief Read attachment for default framebuffer
         *
         * @see mapDefaultForRead()
         * @requires_gl30 %Extension @extension{EXT,framebuffer_object}
         * @requires_gles30 %Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
         */
        enum class DefaultReadAttachment: GLenum {
            /** Don't read from any framebuffer */
            None = GL_NONE,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Read from back left framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackLeft = GL_BACK_LEFT,

            /**
             * Read from back right framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackRight = GL_BACK_RIGHT,

            /**
             * Read from front left framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontLeft = GL_FRONT_LEFT,

            /**
             * Read from front right framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontRight = GL_FRONT_RIGHT,

            /**
             * Read from left framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            Left = GL_LEFT,

            /**
             * Read from right framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            Right = GL_RIGHT,
            #endif

            /** Read from back framebuffer. */
            Back = GL_BACK,

            /**
             * Read from front framebuffer.
             * @requires_es_extension %Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
             */
            Front = GL_FRONT

            #ifndef MAGNUM_TARGET_GLES
            ,

            /**
             * Read from front and back framebuffer.
             * @requires_gl In OpenGL ES you must specify either
             *      @ref Magnum::Framebuffer::DefaultReadAttachment "DefaultReadAttachment::Front"
             *      or @ref Magnum::Framebuffer::DefaultReadAttachment "DefaultReadAttachment::Back".
             */
            FrontAndBack = GL_FRONT_AND_BACK
            #endif
        };

        /**
         * @brief Constructor
         *
         * Generates new OpenGL framebuffer.
         * @see @fn_gl{GenFramebuffers}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline Framebuffer() { glGenFramebuffers(1, &_id); }

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL framebuffer.
         * @see @fn_gl{DeleteFramebuffers}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        inline ~Framebuffer() { glDeleteFramebuffers(1, &_id); }

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
            glBindFramebuffer(static_cast<GLenum>(target), _id);
        }

        #ifndef MAGNUM_TARGET_GLES2
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
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_gles30 Draw attachments for default framebuffer are
         *      available only in OpenGL ES 3.0.
         */
        static void mapDefaultForDraw(std::initializer_list<DefaultDrawAttachment> attachments);
        #endif

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
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_gles30 %Extension @es_extension2{NV,draw_buffers,GL_NV_draw_buffers}
         */
        void mapForDraw(std::initializer_list<std::int8_t> colorAttachments);

        /**
         * @brief Map given attachment of default framebuffer for reading
         * @param attachment        Default attachment
         *
         * Each used attachment should have either renderbuffer or texture
         * attached to work properly.
         * @see mapForRead(), mapDefaultForDraw(), bindDefault(), @fn_gl{ReadBuffer}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_gles30 %Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
         */
        inline static void mapDefaultForRead(DefaultReadAttachment attachment) {
            bindDefault(Target::Read);
            /** @todo Get some extension wrangler instead to avoid undeclared glReadBuffer() on ES2 */
            #ifndef MAGNUM_TARGET_GLES2
            glReadBuffer(static_cast<GLenum>(attachment));
            #else
            static_cast<void>(attachment);
            #endif
        }

        /**
         * @brief Map given color attachment of current framebuffer for reading
         * @param colorAttachment   Color attachment ID
         *
         * The color attachment should have either renderbuffer or texture
         * attached for reading to work properly.
         * @see mapDefaultForRead(), mapForDraw(), bind(), @fn_gl{ReadBuffer}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
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
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_gl Only 2D and 3D textures are available in OpenGL ES.
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
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_gl Only 2D and 3D textures are available in OpenGL ES.
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

        /**
         * @brief Attach 3D texture to given framebuffer depth/stencil attachment
         * @param target            %Target
         * @param depthStencilAttachment Depth/stencil attachment
         * @param texture           3D texture
         * @param mipLevel          Mip level
         * @param layer             Layer of 2D image within a 3D texture
         *
         * @see bind(), @fn_gl{FramebufferTexture}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_es_extension %Extension @es_extension{OES,texture_3D}
         */
        inline void attachTexture3D(Target target, DepthStencilAttachment depthStencilAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for internal format compatibility */
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
         * @see bind(), @fn_gl{FramebufferTexture}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_es_extension %Extension @es_extension{OES,texture_3D}
         */
        inline void attachTexture3D(Target target, std::uint8_t colorAttachment, Texture3D* texture, GLint mipLevel, GLint layer) {
            /** @todo Check for internal format compatibility */
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

        /*@}*/

        /** @{ @name Framebuffer blitting and reading */

        /**
         * @brief Output mask for blitting
         *
         * Specifies which data are copied when performing blit operation
         * using blit().
         * @see BlitMask
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit}
         */
        enum class Blit: GLbitfield {
            Color = GL_COLOR_BUFFER_BIT,    /**< Color */
            Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth value */
            Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil value */
        };

        /**
         * @brief Output mask for blitting
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit}
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
         * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit}
         */
        inline static void blit(const Vector2i& bottomLeft, const Vector2i& topRight, const Vector2i& destinationBottomLeft, const Vector2i& destinationTopRight, BlitMask blitMask, AbstractTexture::Filter filter) {
            /** @todo Get some extension wrangler instead to avoid undeclared glBlitFramebuffer() on ES2 */
            #ifndef MAGNUM_TARGET_GLES2
            glBlitFramebuffer(bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), destinationBottomLeft.x(), destinationBottomLeft.y(), destinationTopRight.x(), destinationTopRight.y(), static_cast<GLbitfield>(blitMask), static_cast<GLenum>(filter));
            #else
            static_cast<void>(bottomLeft);
            static_cast<void>(topRight);
            static_cast<void>(destinationBottomLeft);
            static_cast<void>(destinationTopRight);
            static_cast<void>(blitMask);
            static_cast<void>(filter);
            #endif
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
         * @requires_gl30 Extension @extension{EXT,framebuffer_blit}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit}
         */
        inline static void blit(const Vector2i& bottomLeft, const Vector2i& topRight, BlitMask blitMask) {
            /** @todo Get some extension wrangler instead to avoid undeclared glBlitFramebuffer() on ES2 */
            #ifndef MAGNUM_TARGET_GLES2
            glBlitFramebuffer(bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), bottomLeft.x(), bottomLeft.y(), topRight.x(), topRight.y(), static_cast<GLbitfield>(blitMask), static_cast<GLenum>(AbstractTexture::Filter::NearestNeighbor));
            #else
            static_cast<void>(bottomLeft);
            static_cast<void>(topRight);
            static_cast<void>(blitMask);
            #endif
        }

        /**
         * @brief Read block of pixels from framebuffer to image
         * @param offset            Offset in the framebuffer
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param image             %Image where to put the data
         *
         * @see @fn_gl{ReadPixels}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         */
        static void read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, Image2D* image);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Read block of pixels from framebuffer to buffered image
         * @param offset            Offset in the framebuffer
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param image             Buffered image where to put the data
         * @param usage             %Buffer usage
         *
         * @see Buffer::bind(Target), @fn_gl{ReadPixels}
         * @requires_gl30 Extension @extension{EXT,framebuffer_object}
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
         */
        static void read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, BufferImage2D* image, Buffer::Usage usage);
        #endif

        /*@}*/

    private:
        GLuint _id;
};

CORRADE_ENUMSET_OPERATORS(Framebuffer::ClearMask)
#ifndef MAGNUM_TARGET_GLES
CORRADE_ENUMSET_OPERATORS(Framebuffer::BlitMask)
#endif

}

#endif
