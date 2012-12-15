#ifndef Magnum_AbstractFramebuffer_h
#define Magnum_AbstractFramebuffer_h
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
 * @brief Class Magnum::AbstractFramebuffer
 */

#include <Containers/EnumSet.h>

#include "Math/Vector2.h"
#include "AbstractImage.h"
#include "Buffer.h"

namespace Magnum {

/**
@brief Base for default and named framebuffers

See DefaultFramebuffer and Framebuffer for more information.
@todo @extension{ARB,viewport_array}
*/
class MAGNUM_EXPORT AbstractFramebuffer {
    AbstractFramebuffer(const AbstractFramebuffer& other) = delete;
    AbstractFramebuffer(AbstractFramebuffer&& other) = delete;
    AbstractFramebuffer& operator=(const AbstractFramebuffer& other) = delete;
    AbstractFramebuffer& operator=(AbstractFramebuffer&& other) = delete;

    public:
        /**
         * @brief Mask for clearing
         *
         * @see ClearMask
         */
        enum class Clear: GLbitfield {
            Color = GL_COLOR_BUFFER_BIT,    /**< Color */
            Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth value */
            Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil value */
        };

        /**
         * @brief Mask for clearing
         *
         * @see clear()
         */
        typedef Corrade::Containers::EnumSet<Clear, GLbitfield,
            GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> ClearMask;

        /**
         * @brief Mask for blitting
         *
         * @see BlitMask
         * @requires_gl30 %Extension @extension{EXT,framebuffer_object}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit}
         */
        enum class Blit: GLbitfield {
            ColorBuffer = GL_COLOR_BUFFER_BIT,    /**< Color buffer */
            DepthBuffer = GL_DEPTH_BUFFER_BIT,    /**< Depth buffer */
            StencilBuffer = GL_STENCIL_BUFFER_BIT /**< Stencil buffer */
        };

        /**
         * @brief Mask for blitting
         *
         * @see blit()
         * @requires_gl30 %Extension @extension{EXT,framebuffer_object}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit}
         */
        typedef Corrade::Containers::EnumSet<Blit, GLbitfield,
            GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> BlitMask;

        /**
         * @brief Blit filtering
         *
         * @see blit()
         */
        enum class BlitFilter: GLenum {
            NearestNeighbor = GL_NEAREST,   /**< Nearest neighbor filtering */
            LinearInterpolation = GL_LINEAR /**< Linear interpolation filtering */
        };

        /**
         * @brief Target for binding framebuffer
         *
         * @see DefaultFramebuffer::bind(), Framebuffer::bind()
         * @requires_gl30 %Extension @extension{EXT,framebuffer_object}
         */
        enum class Target: GLenum {
            /**
             * For reading only.
             * @requires_gl30 %Extension @extension{EXT,framebuffer_blit}
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
             * @requires_gl30 %Extension @extension{EXT,framebuffer_blit}
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

        /**
         * @brief Copy block of pixels
         * @param source            Source framebuffer
         * @param destination       Destination framebuffer
         * @param sourceBottomLeft  Bottom left coordinates of source rectangle
         * @param sourceTopRight    Top right coordinates of source rectangle
         * @param destinationBottomLeft Bottom left coordinates of destination rectangle
         * @param destinationTopRight Top right coordinates of destination
         *      rectangle
         * @param mask              Which buffers to perform blit operation on
         * @param filter            Interpolation filter
         *
         * Binds @p source framebuffer to @ref Target "Target::Read" and
         * @p destination framebuffer to @ref Target "Target::Draw" and
         * performs blitting operation. See DefaultFramebuffer::mapForRead(),
         * Framebuffer::mapForRead(), DefaultFramebuffer::mapForDraw() and
         * Framebuffer::mapForDraw() for specifying particular buffers for
         * blitting operation.
         * @see @fn_gl{BlitFramebuffer}
         * @requires_gl30 %Extension @extension{EXT,framebuffer_blit}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit}
         */
        inline static void blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Vector2i& sourceBottomLeft, const Vector2i& sourceTopRight, const Vector2i& destinationBottomLeft, const Vector2i& destinationTopRight, BlitMask mask, BlitFilter filter) {
            source.bind(AbstractFramebuffer::Target::Read);
            destination.bind(AbstractFramebuffer::Target::Draw);
            /** @todo Get some extension wrangler instead to avoid undeclared glBlitFramebuffer() on ES2 */
            #ifndef MAGNUM_TARGET_GLES2
            glBlitFramebuffer(sourceBottomLeft.x(), sourceBottomLeft.y(), sourceTopRight.x(), sourceTopRight.y(), destinationBottomLeft.x(), destinationBottomLeft.y(), destinationTopRight.x(), destinationTopRight.y(), static_cast<GLbitfield>(mask), static_cast<GLenum>(filter));
            #else
            static_cast<void>(sourceBottomLeft);
            static_cast<void>(sourceTopRight);
            static_cast<void>(destinationBottomLeft);
            static_cast<void>(destinationTopRight);
            static_cast<void>(mask);
            static_cast<void>(filter);
            #endif
        }

        /**
         * @brief Copy block of pixels
         * @param source            Source framebuffer
         * @param destination       Destination framebuffer
         * @param bottomLeft        Bottom left coordinates of source and
         *      destination rectangle
         * @param topRight          Top right coordinates of source and
         *      destination rectangle
         * @param mask              Which buffers to perform blit operation on
         *
         * Convenience alternative to above function when source rectangle is
         * the same as destination rectangle. As the image is copied
         * pixel-by-pixel, no interpolation is needed and thus
         * @ref BlitFilter "BlitFilter::NearestNeighbor" filtering is used by
         * default.
         * @see @fn_gl{BlitFramebuffer}
         * @requires_gl30 %Extension @extension{EXT,framebuffer_blit}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit}
         */
        inline static void blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Vector2i& bottomLeft, const Vector2i& topRight, BlitMask mask) {
            blit(source, destination, bottomLeft, topRight, bottomLeft, topRight, mask, BlitFilter::NearestNeighbor);
        }

        AbstractFramebuffer() = default;
        virtual ~AbstractFramebuffer() = 0;

        /**
         * @brief Bind framebuffer
         *
         * @see DefaultFramebuffer::mapForRead(), Framebuffer::mapForRead(),
         *      DefaultFramebuffer::mapForDraw(), Framebuffer::mapForDraw(),
         *      @fn_gl{BindFramebuffer}
         */
        inline void bind(Target target) {
            glBindFramebuffer(static_cast<GLenum>(target), _id);
        }

        /**
         * @brief Set viewport size
         *
         * Call when window size changes.
         * @see @fn_gl{Viewport}
         */
        inline void setViewport(const Vector2i& position, const Vector2i& size) {
            bind(Target::ReadDraw);
            glViewport(position.x(), position.y(), size.x(), size.y());
        }

        /**
         * @brief Clear specified buffers in framebuffer
         * @param mask              Which buffers to clear
         *
         * @see Renderer::setClearColor(), Renderer::setClearDepth(),
         *      Renderer::setClearStencil(), @fn_gl{Clear}
         */
        inline void clear(ClearMask mask) {
            bind(Target::ReadDraw);
            glClear(static_cast<GLbitfield>(mask));
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
         */
        void read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, Image2D* image);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Read block of pixels from framebuffer to buffer image
         * @param offset            Offset in the framebuffer
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param image             %Buffer image where to put the data
         * @param usage             %Buffer usage
         *
         * @see Buffer::bind(Target), @fn_gl{ReadPixels}
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
         */
        void read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, BufferImage2D* image, Buffer::Usage usage);
        #endif

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        GLuint _id;
    #endif
};

inline AbstractFramebuffer::~AbstractFramebuffer() {}

CORRADE_ENUMSET_OPERATORS(AbstractFramebuffer::ClearMask)
#ifndef MAGNUM_TARGET_GLES
CORRADE_ENUMSET_OPERATORS(AbstractFramebuffer::BlitMask)
#endif

}

#endif
