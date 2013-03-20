#ifndef Magnum_AbstractFramebuffer_h
#define Magnum_AbstractFramebuffer_h
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
 * @brief Class Magnum::AbstractFramebuffer
 */

#include <Containers/EnumSet.h>

#include "Math/Geometry/Rectangle.h"
#include "AbstractImage.h"
#include "Buffer.h"

namespace Magnum {

/**
@brief Base for default and named framebuffers

See DefaultFramebuffer and Framebuffer for more information.

@section AbstractFramebuffer-performance-optimization Performance optimizations

The engine tracks currently bound framebuffer and current viewport to avoid
unnecessary calls to @fn_gl{BindFramebuffer} and @fn_gl{Viewport} when
switching framebuffers.

@todo @extension{ARB,viewport_array}
*/
class MAGNUM_EXPORT AbstractFramebuffer {
    friend class Context;

    AbstractFramebuffer(const AbstractFramebuffer&) = delete;
    AbstractFramebuffer(AbstractFramebuffer&&) = delete;
    AbstractFramebuffer& operator=(const AbstractFramebuffer&) = delete;
    AbstractFramebuffer& operator=(AbstractFramebuffer&&) = delete;

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
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit} or
         *      @es_extension{NV,framebuffer_blit}
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
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit} or
         *      @es_extension{NV,framebuffer_blit}
         */
        typedef Corrade::Containers::EnumSet<Blit, GLbitfield,
            GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> BlitMask;

        /**
         * @brief Blit filtering
         *
         * @see blit()
         */
        enum class BlitFilter: GLenum {
            Nearest = GL_NEAREST,   /**< Nearest neighbor filtering */
            Linear = GL_LINEAR      /**< Linear interpolation filtering */
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
             * @requires_gles30 %Extension @es_extension{APPLE,framebuffer_multisample},
             *      @es_extension{ANGLE,framebuffer_blit} or @es_extension{NV,framebuffer_blit}
             */
            #ifndef MAGNUM_TARGET_GLES2
            Read = GL_READ_FRAMEBUFFER,
            #else
            Read = GL_READ_FRAMEBUFFER_APPLE,
            #endif

            /**
             * For drawing only.
             * @requires_gl30 %Extension @extension{EXT,framebuffer_blit}
             * @requires_gles30 %Extension @es_extension{APPLE,framebuffer_multisample},
             *      @es_extension{ANGLE,framebuffer_blit} or @es_extension{NV,framebuffer_blit}
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
         * @param sourceRectangle   Source rectangle
         * @param destinationRectangle Destination rectangle
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
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit} or
         *      @es_extension{NV,framebuffer_blit}
         */
        static void blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Rectanglei& sourceRectangle, const Rectanglei& destinationRectangle, BlitMask mask, BlitFilter filter);

        /**
         * @brief Copy block of pixels
         * @param source            Source framebuffer
         * @param destination       Destination framebuffer
         * @param rectangle         Source and destination rectangle
         * @param mask              Which buffers to perform blit operation on
         *
         * Convenience alternative to above function when source rectangle is
         * the same as destination rectangle. As the image is copied
         * pixel-by-pixel, no interpolation is needed and thus
         * @ref BlitFilter "BlitFilter::Nearest" filtering is used by default.
         * @see @fn_gl{BlitFramebuffer}
         * @requires_gl30 %Extension @extension{EXT,framebuffer_blit}
         * @requires_gles30 %Extension @es_extension{ANGLE,framebuffer_blit} or
         *      @es_extension{NV,framebuffer_blit}
         */
        inline static void blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Rectanglei& rectangle, BlitMask mask) {
            blit(source, destination, rectangle, rectangle, mask, BlitFilter::Nearest);
        }

        explicit AbstractFramebuffer() = default;
        virtual ~AbstractFramebuffer() = 0;

        /**
         * @brief Bind framebuffer for rendering
         *
         * Binds the framebuffer and updates viewport to saved dimensions.
         * @see setViewport(), DefaultFramebuffer::mapForRead(),
         *      Framebuffer::mapForRead(), DefaultFramebuffer::mapForDraw(),
         *      Framebuffer::mapForDraw(), @fn_gl{BindFramebuffer},
         *      @fn_gl{Viewport}
         */
        void bind(Target target);

        /** @brief Viewport rectangle */
        inline Rectanglei viewport() const { return _viewport; }

        /**
         * @brief Set viewport
         * @return Pointer to self (for method chaining)
         *
         * Saves the viewport to be used at later time in bind(). If the
         * framebuffer is currently bound, updates the viewport to given
         * rectangle.
         * @see @fn_gl{Viewport}
         */
        AbstractFramebuffer* setViewport(const Rectanglei& rectangle);

        /**
         * @brief Clear specified buffers in framebuffer
         * @param mask              Which buffers to clear
         *
         * To improve performance you can also use
         * DefaultFramebuffer::invalidate() / Framebuffer::invalidate() instead
         * of clearing given buffer if you will not use it anymore or fully
         * overwrite it later.
         * @see Renderer::setClearColor(), Renderer::setClearDepth(),
         *      Renderer::setClearStencil(), @fn_gl{BindFramebuffer},
         *      @fn_gl{Clear}
         */
        void clear(ClearMask mask);

        /**
         * @brief Read block of pixels from framebuffer to image
         * @param offset            Offset in the framebuffer
         * @param size              %Image size
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param image             %Image where to put the data
         *
         * @see @fn_gl{BindFramebuffer}, @fn_gl{ReadPixels}
         * @todo Read size, format & type from image?
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
         * @see @fn_gl{BindFramebuffer}, @fn_gl{ReadPixels}
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
         * @todo Read size, format & type from image?
         */
        void read(const Vector2i& offset, const Vector2i& size, AbstractImage::Format format, AbstractImage::Type type, BufferImage2D* image, Buffer::Usage usage);
        #endif

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        void MAGNUM_LOCAL bindInternal(Target target);
        Target MAGNUM_LOCAL bindInternal();
        void MAGNUM_LOCAL setViewportInternal();

        static MAGNUM_LOCAL Target readTarget;
        static MAGNUM_LOCAL Target drawTarget;

        typedef void(AbstractFramebuffer::*DrawBuffersImplementation)(GLsizei, const GLenum*);
        static MAGNUM_LOCAL DrawBuffersImplementation drawBuffersImplementation;

        typedef void(AbstractFramebuffer::*DrawBufferImplementation)(GLenum);
        static DrawBufferImplementation drawBufferImplementation;

        typedef void(AbstractFramebuffer::*ReadBufferImplementation)(GLenum);
        static ReadBufferImplementation readBufferImplementation;

        void MAGNUM_LOCAL invalidateImplementation(GLsizei count, GLenum* attachments);
        void MAGNUM_LOCAL invalidateImplementation(GLsizei count, GLenum* attachments, const Rectanglei& rectangle);

        GLuint _id;
        Rectanglei _viewport;
    #endif

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);

        void MAGNUM_LOCAL drawBuffersImplementationDefault(GLsizei count, const GLenum* buffers);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL drawBuffersImplementationDSA(GLsizei count, const GLenum* buffers);
        #endif

        void MAGNUM_LOCAL drawBufferImplementationDefault(GLenum buffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL drawBufferImplementationDSA(GLenum buffer);
        #endif

        void MAGNUM_LOCAL readBufferImplementationDefault(GLenum buffer);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL readBufferImplementationDSA(GLenum buffer);
        #endif
};

inline AbstractFramebuffer::~AbstractFramebuffer() {}

CORRADE_ENUMSET_OPERATORS(AbstractFramebuffer::ClearMask)
CORRADE_ENUMSET_OPERATORS(AbstractFramebuffer::BlitMask)

}

#endif
