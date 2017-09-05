#ifndef Magnum_AbstractFramebuffer_h
#define Magnum_AbstractFramebuffer_h
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
 * @brief Class @ref Magnum::AbstractFramebuffer, enum @ref Magnum::FramebufferClear, @ref Magnum::FramebufferBlit, @ref Magnum::FramebufferBlitFilter, @ref Magnum::FramebufferTarget, enum set @ref Magnum::FramebufferClearMask
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Utility/Macros.h>

#include "Magnum/AbstractObject.h"
#include "Magnum/Math/Range.h"

namespace Magnum {

/**
@brief Mask for framebuffer clearing

@see @ref AbstractFramebuffer, @ref FramebufferClearMask
*/
enum class FramebufferClear: GLbitfield {
    Color = GL_COLOR_BUFFER_BIT,    /**< Color buffer */
    Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth buffer */
    Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil buffer */
};

/**
@brief Mask for clearing

@see @ref AbstractFramebuffer::clear()
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
typedef Containers::EnumSet<FramebufferClear,
    GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> FramebufferClearMask;
#else
typedef Containers::EnumSet<FramebufferClear> FramebufferClearMask;
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
/**
@brief Mask for framebuffer blitting

@see @ref AbstractFramebuffer, @ref FramebufferBlitMask
@requires_gl30 Extension @extension{ARB,framebuffer_object}
@requires_gles30 Extension @extension{ANGLE,framebuffer_blit} or
    @extension{NV,framebuffer_blit} in OpenGL ES 2.0.
@requires_webgl20 Framebuffer blit is not available in WebGL 1.0.
*/
enum class FramebufferBlit: GLbitfield {
    Color = GL_COLOR_BUFFER_BIT,    /**< Color buffer */
    Depth = GL_DEPTH_BUFFER_BIT,    /**< Depth buffer */
    Stencil = GL_STENCIL_BUFFER_BIT /**< Stencil buffer */
};

/**
@brief Mask for framebuffer blitting

@see @ref AbstractFramebuffer::blit()
@requires_gl30 Extension @extension{ARB,framebuffer_object}
@requires_gles30 Extension @extension{ANGLE,framebuffer_blit} or
    @extension{NV,framebuffer_blit} in OpenGL ES 2.0.
@requires_webgl20 Framebuffer blit is not available in WebGL 1.0.
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
typedef Containers::EnumSet<FramebufferBlit,
    GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT> FramebufferBlitMask;
#else
typedef Containers::EnumSet<FramebufferBlit> FramebufferBlitMask;
#endif

/**
@brief Framebuffer blit filtering

@see @ref AbstractFramebuffer::blit()
@requires_gl30 Extension @extension{ARB,framebuffer_object}
@requires_gles30 Extension @extension{ANGLE,framebuffer_blit} or
    @extension{NV,framebuffer_blit} in OpenGL ES 2.0.
@requires_webgl20 Framebuffer blit is not available in WebGL 1.0.
*/
enum class FramebufferBlitFilter: GLenum {
    Nearest = GL_NEAREST,   /**< Nearest neighbor filtering */
    Linear = GL_LINEAR      /**< Linear interpolation filtering */
};
#endif

/**
@brief Framebuffer target

@see @ref DefaultFramebuffer::checkStatus(), @ref Framebuffer::checkStatus()
@requires_gl30 Extension @extension{ARB,framebuffer_object}
*/
enum class FramebufferTarget: GLenum {
    /** Frambebuffer reading target */
    #ifndef MAGNUM_TARGET_GLES2
    Read = GL_READ_FRAMEBUFFER,
    #elif !defined(MAGNUM_TARGET_WEBGL)
    Read = GL_READ_FRAMEBUFFER_APPLE,
    #else
    Read,
    #endif

    /** Framebuffer drawing target */
    #ifndef MAGNUM_TARGET_GLES2
    Draw = GL_DRAW_FRAMEBUFFER,
    #elif !defined(MAGNUM_TARGET_WEBGL)
    Draw = GL_DRAW_FRAMEBUFFER_APPLE,
    #else
    Draw,
    #endif

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * Framebuffer drawing target
     * @deprecated Use @ref FramebufferTarget::Draw instead.
     */
    ReadDraw CORRADE_DEPRECATED_ENUM("use FramebufferTarget::Draw instead") =
        #ifndef MAGNUM_TARGET_GLES2
        GL_DRAW_FRAMEBUFFER
        #elif !defined(MAGNUM_TARGET_WEBGL)
        GL_DRAW_FRAMEBUFFER_APPLE
        #else
        1
        #endif
    #endif
};

namespace Implementation { struct FramebufferState; }

/**
@brief Base for default and named framebuffers

See @ref DefaultFramebuffer and @ref Framebuffer for more information.

@anchor AbstractFramebuffer-performance-optimization
## Performance optimizations and security

The engine tracks currently bound framebuffer and current viewport to avoid
unnecessary calls to @fn_gl{BindFramebuffer} and @fn_gl{Viewport} when
switching framebuffers. Framebuffer limits and implementation-defined values
(such as @ref maxViewportSize()) are cached, so repeated queries don't result
in repeated @fn_gl{Get} calls. See also @ref Context::resetState() and
@ref Context::State::Framebuffers.

If extension @extension{ARB,direct_state_access} (part of OpenGL 4.5) is
available, @ref blit(), @ref clearDepth(), @ref clearStencil() and
@ref clearDepthStencil() functions use DSA to avoid unnecessary call to
@fn_gl{BindFramebuffer}. See their documentation for more information.

If @extension{ARB,robustness} is available, @ref read() operations are
protected from buffer overflow.
*/
class MAGNUM_EXPORT AbstractFramebuffer {
    friend Implementation::FramebufferState;

    public:
        /** @todo `GL_IMPLEMENTATION_COLOR_READ_FORMAT`, `GL_IMPLEMENTATION_COLOR_READ_TYPE`, seems to be depending on currently bound FB (aargh) (@extension{ARB,ES2_compatibility}). */

        /**
         * @brief Max supported viewport size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref setViewport(), @fn_gl{Get} with @def_gl{MAX_VIEWPORT_DIMS}
         */
        static Vector2i maxViewportSize();

        /**
         * @brief Max supported draw buffer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither @extension{EXT,draw_buffers} nor
         * @extension{NV,draw_buffers} is available in OpenGL ES 2.0 and
         * @webgl_extension{WEBGL,draw_buffers} is not available in WebGL 1.0,
         * returns `0`.
         * @see @ref DefaultFramebuffer::mapForDraw(), @ref Framebuffer::mapForDraw(),
         *      @fn_gl{Get} with @def_gl{MAX_DRAW_BUFFERS}
         */
        static Int maxDrawBuffers();

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Max supported dual-source draw buffer count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If extension @extension{ARB,blend_func_extended} (part
         * of OpenGL 3.3) is not available, returns `0`.
         * @see @ref DefaultFramebuffer::mapForDraw(), @ref Framebuffer::mapForDraw(),
         *      @fn_gl{Get} with @def_gl{MAX_DUAL_SOURCE_DRAW_BUFFERS}
         * @requires_gl Multiple blending inputs are not available in
         *      OpenGL ES or WebGL.
         */
        static Int maxDualSourceDrawBuffers();
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Copy block of pixels
         * @param source            Source framebuffer
         * @param destination       Destination framebuffer
         * @param sourceRectangle   Source rectangle
         * @param destinationRectangle Destination rectangle
         * @param mask              Which buffers to perform blit operation on
         * @param filter            Interpolation filter
         *
         * See @ref DefaultFramebuffer::mapForRead(), @ref Framebuffer::mapForRead(),
         * @ref DefaultFramebuffer::mapForDraw() and @ref Framebuffer::mapForDraw()
         * for specifying particular buffers for blitting operation. If
         * @extension{ARB,direct_state_access} (part of OpenGL 4.5) is not
         * available, @p source framebuffer is bound to @ref FramebufferTarget::Read
         * and @p destination framebuffer to @ref FramebufferTarget::Draw
         * before the operation (if not already).
         * @see @fn_gl2{BlitNamedFramebuffer,BlitFramebuffer}, eventually
         *      @fn_gl{BlitFramebuffer}
         * @requires_gles30 Extension @extension{ANGLE,framebuffer_blit} or
         *      @extension{NV,framebuffer_blit} in OpenGL ES 2.0.
         * @requires_webgl20 Framebuffer blit is not available in WebGL 1.0.
         */
        static void blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);

        /**
         * @brief Copy block of pixels
         *
         * Convenience alternative to the above function when source rectangle
         * is the same as destination rectangle. As the image is copied
         * pixel-by-pixel, no interpolation is needed and thus
         * @ref FramebufferBlitFilter::Nearest filtering is used by default.
         */
        static void blit(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& rectangle, FramebufferBlitMask mask) {
            blit(source, destination, rectangle, rectangle, mask, FramebufferBlitFilter::Nearest);
        }
        #endif

        /**
         * @brief Bind framebuffer for drawing
         *
         * Binds the framebuffer for drawing and updates viewport to saved
         * dimensions.
         * @see @ref setViewport(), @ref DefaultFramebuffer::mapForRead(),
         *      @ref Framebuffer::mapForRead(), @ref DefaultFramebuffer::mapForDraw(),
         *      @ref Framebuffer::mapForDraw(), @fn_gl{BindFramebuffer},
         *      @fn_gl{Viewport}
         */
        void bind();

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief bind()
         * @deprecated Use parameter-less @ref bind() instead.
         */
        CORRADE_DEPRECATED("use parameter-less bind() instead") void bind(FramebufferTarget) {
            bind();
        }
        #endif

        /** @brief Viewport rectangle */
        Range2Di viewport() const { return _viewport; }

        /**
         * @brief Set viewport
         * @return Reference to self (for method chaining)
         *
         * Saves the viewport to be used at later time in @ref bind(). If the
         * framebuffer is currently bound, updates the viewport to given
         * rectangle. Initial value in @ref DefaultFramebuffer is set to cover
         * whole window, in @ref Framebuffer the initial value is specified in
         * constructor.
         * @see @ref maxViewportSize(), @fn_gl{Viewport}
         */
        AbstractFramebuffer& setViewport(const Range2Di& rectangle);

        /**
         * @brief Clear specified buffers in framebuffer
         * @param mask              Which buffers to clear
         * @return Reference to self (for method chaining)
         *
         * To improve performance you can also use
         * @ref DefaultFramebuffer::invalidate() / @ref Framebuffer::invalidate()
         * instead of clearing given buffer if you will not use it anymore or
         * fully overwrite it later.
         * @see @ref Renderer::setClearColor(), @ref Renderer::setClearDepth(),
         *      @ref Renderer::setClearStencil(), @fn_gl{BindFramebuffer},
         *      @fn_gl{Clear}
         * @deprecated_gl Prefer to use @ref Framebuffer::clearColor() "*Framebuffer::clearColor()"
         *      / @ref clearDepth() / @ref clearDepthStencil() instead of
         *      @ref Renderer::setClearColor() / @ref Renderer::setClearDepth()
         *      / @ref Renderer::setClearStencil() and @ref clear() as it leads
         *      to less state changes.
         */
        AbstractFramebuffer& clear(FramebufferClearMask mask);

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Clear depth buffer to specified value
         * @param depth         Value to clear with
         * @return Reference to self (for method chaining)
         *
         * @see @ref clear(), @fn_gl{ClearNamedFramebuffer}, eventually
         *      @fn_gl{BindFramebuffer}, then @fn_gl{ClearBuffer}
         * @requires_gl30 Direct framebuffer clearing is not available in
         *      OpenGL 2.1.
         * @requires_gles30 Direct framebuffer clearing is not available in
         *      OpenGL ES 2.0 or WebGL 1.0.
         */
        AbstractFramebuffer& clearDepth(Float depth);

        /**
         * @brief Clear stencil buffer to specified value
         * @param stencil       Value to clear with
         * @return Reference to self (for method chaining)
         *
         * @see @ref clear(), @fn_gl{ClearNamedFramebuffer}, eventually
         *      @fn_gl{BindFramebuffer}, then @fn_gl{ClearBuffer}
         * @requires_gl30 Direct framebuffer clearing is not available in
         *      OpenGL 2.1.
         * @requires_gles30 Direct framebuffer clearing is not available in
         *      OpenGL ES 2.0 or WebGL 1.0.
         */
        AbstractFramebuffer& clearStencil(Int stencil);

        /**
         * @brief Clear depth and stencil buffer to specified value
         * @param depth         Depth value to clear with
         * @param stencil       Stencil value to clear with
         * @return Reference to self (for method chaining)
         *
         * @see @ref clear(), @fn_gl{ClearNamedFramebuffer}, eventually
         *      @fn_gl{BindFramebuffer}, then @fn_gl{ClearBuffer}
         * @requires_gl30 Direct framebuffer clearing is not available in
         *      OpenGL 2.1.
         * @requires_gles30 Direct framebuffer clearing is not available in
         *      OpenGL ES 2.0 or WebGL 1.0.
         */
        AbstractFramebuffer& clearDepthStencil(Float depth, Int stencil);
        #endif

        /**
         * @brief Read block of pixels from framebuffer to image
         * @param rectangle         Framebuffer rectangle to read
         * @param image             Image where to put the data
         *
         * Image parameters like format and type of pixel data are taken from
         * given image. The storage is not reallocated if it is large enough to
         * contain the new data.
         *
         * If @extension{ARB,robustness} is available, the operation is
         * protected from buffer overflow.
         * @see @fn_gl{BindFramebuffer}, then @fn_gl{PixelStore} and
         *      @fn_gl{ReadPixels} or @fn_gl_extension{ReadnPixels,ARB,robustness}
         */
        void read(const Range2Di& rectangle, Image2D& image);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * Image2D image = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA, PixelType::UnsignedByte});
         * @endcode
         */
        Image2D read(const Range2Di& rectangle, Image2D&& image);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief read(const Range2Di&, Image2D&)
         * @deprecated Use @ref read(const Range2Di&, Image2D&) instead.
         */
        CORRADE_DEPRECATED("use read(const Range2Di&, Image2D& instead) instead") void read(const Vector2i& offset, const Vector2i& size, Image2D& image) {
            read({offset, size}, image);
        }
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Read block of pixels from framebuffer to buffer image
         * @param rectangle         Framebuffer rectangle to read
         * @param image             Buffer image where to put the data
         * @param usage             Buffer usage
         *
         * See @ref read(const Vector2i&, const Vector2i&, Image2D&) for more
         * information. The storage is not reallocated if it is large enough to
         * contain the new data, which means that @p usage might get ignored.
         * @requires_gles30 Pixel buffer objects are not available in OpenGL ES
         *      2.0.
         * @requires_webgl20 Pixel buffer objects are not available in WebGL
         *      1.0.
         * @todo Make it more flexible (usable with
         *      @extension{ARB,buffer_storage}, avoiding relocations...)
         */
        void read(const Range2Di& rectangle, BufferImage2D& image, BufferUsage usage);

        /** @overload
         *
         * Convenience alternative to the above, example usage:
         * @code
         * BufferImage2D image = framebuffer.read(framebuffer.viewport(), {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
         * @endcode
         */
        BufferImage2D read(const Range2Di& rectangle, BufferImage2D&& image, BufferUsage usage);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief read(const Range2Di&, BufferImage2D&, BufferUsage)
         * @deprecated Use @ref read(const Range2Di&, BufferImage2D&, BufferUsage)
         *      instead.
         */
        CORRADE_DEPRECATED("use read(const Range2Di&, BufferImage2D&, BufferUsage) instead") void read(const Vector2i& offset, const Vector2i& size, BufferImage2D& image, BufferUsage usage) {
            read({offset, size}, image, usage);
        }
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Copy block of pixels from framebuffer to 1D texture image
         * @param rectangle         Framebuffer rectangle to copy. Height must
         *      be `1`.
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param internalFormat    Texture internal format
         *
         * On platforms that support it prefer to use @ref Texture1D::setStorage()
         * and @ref copySubImage() instead, as it avoids unnecessary
         * reallocations and has better performance characteristics. This call
         * also has no equivalent in @extension{ARB,direct_state_access}, thus
         * the texture needs to be bound to some texture unit before the
         * operation.
         * @see @ref Texture1D::maxSize(), @fn_gl{BindFramebuffer}, then
         *      @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexImage1D}
         * @requires_gl 1D textures are not available in OpenGL ES or WebGL.
         * @deprecated_gl Prefer to use @ref Texture1D::setStorage() and
         *      @ref copySubImage() instead.
         */
        void copyImage(const Range2Di& rectangle, Texture1D& texture, Int level, TextureFormat internalFormat);
        #endif

        /**
         * @brief Copy block of pixels from framebuffer to 2D texture image
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param internalFormat    Texture internal format
         *
         * On platforms that support it prefer to use @ref Texture2D::setStorage()
         * and @ref copySubImage() instead, as it avoids unnecessary
         * reallocations and has better performance characteristics. This call
         * also has no equivalent in @extension{ARB,direct_state_access}, thus
         * the texture needs to be bound to some texture unit before the
         * operation.
         * @see @ref Texture2D::maxSize(), @fn_gl{BindFramebuffer}, then
         *      @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexImage2D}
         * @deprecated_gl Prefer to use @ref Texture2D::setStorage() and
         *      @ref copySubImage() instead.
         */
        void copyImage(const Range2Di& rectangle, Texture2D& texture, Int level, TextureFormat internalFormat);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Copy block of pixels from framebuffer to rectangle texture
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param internalFormat    Texture internal format
         *
         * On platforms that support it prefer to use @ref RectangleTexture::setStorage()
         * and @ref copySubImage() instead, as it avoids unnecessary
         * reallocations and has better performance characteristics. This call
         * also has no equivalent in @extension{ARB,direct_state_access}, thus
         * the texture needs to be bound to some texture unit before the
         * operation.
         * @see @ref Texture2D::maxSize(), @fn_gl{BindFramebuffer}, then
         *      @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexImage2D}
         * @requires_gl31 Extension @extension{ARB,texture_rectangle}
         * @requires_gl Rectangle textures are not available in OpenGL ES and
         *      WebGL.
         * @deprecated_gl Prefer to use @ref RectangleTexture::setStorage() and
         *      @ref copySubImage() instead.
         */
        void copyImage(const Range2Di& rectangle, RectangleTexture& texture, TextureFormat internalFormat);
        #endif

        /**
         * @brief Copy block of pixels from framebuffer to cube map texture image
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param coordinate        Cube map coordinate
         * @param internalFormat    Texture internal format
         *
         * On platforms that support it prefer to use @ref CubeMapTexture::setStorage()
         * and @ref copySubImage() instead, as it avoids unnecessary
         * reallocations and has better performance characteristics. This call
         * also has no equivalent in @extension{ARB,direct_state_access}, thus
         * the texture needs to be bound to some texture unit before the
         * operation.
         * @see @ref Texture2D::maxSize(), @fn_gl{BindFramebuffer}, then
         *      @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexImage2D}
         * @deprecated_gl Prefer to use @ref CubeMapTexture::setStorage() and
         *      @ref copySubImage() instead.
         */
        void copyImage(const Range2Di& rectangle, CubeMapTexture& texture, CubeMapCoordinate coordinate, Int level, TextureFormat internalFormat);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Copy block of pixels from framebuffer to 1D texture array image
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param internalFormat    Texture internal format
         *
         * On platforms that support it prefer to use @ref Texture2D::setStorage()
         * and @ref copySubImage() instead, as it avoids unnecessary
         * reallocations and has better performance characteristics. This call
         * also has no equivalent in @extension{ARB,direct_state_access}, thus
         * the texture needs to be bound to some texture unit before the
         * operation.
         * @see @ref Texture2D::maxSize(), @fn_gl{BindFramebuffer}, then
         *      @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexImage2D}
         * @requires_gl 1D array textures are not available in OpenGL ES or
         *      WebGL, only 2D ones.
         * @deprecated_gl Prefer to use @ref Texture1DArray::setStorage() and
         *      @ref copySubImage() instead.
         */
        void copyImage(const Range2Di& rectangle, Texture1DArray& texture, Int level, TextureFormat internalFormat);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Copy block of pixels from framebuffer to 1D texture subimage
         * @param rectangle         Framebuffer rectangle to copy. Height must
         *      be `1`.
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param offset            Offset inside the texture
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref Texture1D::setStorage(), @fn_gl{BindFramebuffer}, then
         *      @fn_gl2{CopyTextureSubImage1D,CopyTexSubImage1D},
         *      @fn_gl_extension{CopyTextureSubImage1D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexSubImage1D}
         * @requires_gl 1D textures are not available in OpenGL ES or WebGL.
         */
        void copySubImage(const Range2Di& rectangle, Texture1D& texture, Int level, Int offset);
        #endif

        /**
         * @brief Copy block of pixels from framebuffer to 2D texture subimage
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param offset            Offset inside the texture
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref Texture2D::setStorage(), @fn_gl{BindFramebuffer}, then
         *      @fn_gl2{CopyTextureSubImage2D,CopyTexSubImage2D},
         *      @fn_gl_extension{CopyTextureSubImage2D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexSubImage2D}
         */
        void copySubImage(const Range2Di& rectangle, Texture2D& texture, Int level, const Vector2i& offset);

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Copy block of pixels from framebuffer to rectangle texture subimage
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param offset            Offset inside the texture
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref RectangleTexture::setStorage(), @fn_gl{BindFramebuffer},
         *      then @fn_gl2{CopyTextureSubImage2D,CopyTexSubImage2D},
         *      @fn_gl_extension{CopyTextureSubImage2D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexSubImage2D}
         * @requires_gl Rectangle textures are not available in OpenGL ES and
         *      WebGL.
         */
        void copySubImage(const Range2Di& rectangle, RectangleTexture& texture, const Vector2i& offset);
        #endif

        /**
         * @brief Copy block of pixels from framebuffer to cube map texture subimage
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param offset            Offset inside the texture
         *
         * Z coordinate of the offset is equivalent to number of texture face,
         * i.e. +X is `0` and so on, in order of (+X, -X, +Y, -Y, +Z, -Z). If
         * neither @extension{ARB,direct_state_access} (part of OpenGL 4.5) nor
         * @extension{EXT,direct_state_access} desktop extension is available,
         * the texture is bound before the operation (if not already).
         * @see @ref CubeMapTexture::setStorage(), @fn_gl{BindFramebuffer},
         *      then @fn_gl2{CopyTextureSubImage3D,CopyTexSubImage3D},
         *      @fn_gl_extension{CopyTextureSubImage2D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexSubImage2D}
         */
        void copySubImage(const Range2Di& rectangle, CubeMapTexture& texture, Int level, const Vector3i& offset);

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Copy block of pixels from framebuffer to 3D texture subimage
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param offset            Offset inside the texture
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref Texture3D::setStorage(), @fn_gl{BindFramebuffer}, then
         *      @fn_gl2{CopyTextureSubImage3D,CopyTexSubImage3D},
         *      @fn_gl_extension{CopyTextureSubImage3D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexSubImage3D}
         * @requires_gles30 Extension @extension{OES,texture_3D} in OpenGL
         *      ES 2.0.
         * @requires_webgl20 Only 2D textures are available in WebGL 1.0.
         */
        void copySubImage(const Range2Di& rectangle, Texture3D& texture, Int level, const Vector3i& offset);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Copy block of pixels from framebuffer to 1D texture array subimage
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param offset            Offset inside the texture
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref Texture1DArray::setStorage(), @fn_gl{BindFramebuffer},
         *      then @fn_gl2{CopyTextureSubImage2D,CopyTexSubImage2D},
         *      @fn_gl_extension{CopyTextureSubImage2D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexSubImage2D}
         * @requires_gl 1D array textures are not available in OpenGL ES or
         *      WebGL, only 2D ones.
         */
        void copySubImage(const Range2Di& rectangle, Texture1DArray& texture, Int level, const Vector2i& offset);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Copy block of pixels from framebuffer to 2D texture array subimage
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param offset            Offset inside the texture
         *
         * If neither @extension{ARB,direct_state_access} (part of OpenGL 4.5)
         * nor @extension{EXT,direct_state_access} desktop extension is
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref Texture2DArray::setStorage(), @fn_gl{BindFramebuffer},
         *      then @fn_gl2{CopyTextureSubImage3D,CopyTexSubImage3D},
         *      @fn_gl_extension{CopyTextureSubImage3D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexSubImage3D}
         * @requires_gl30 Extension @extension{EXT,texture_array}
         * @requires_gles30 Array textures are not available in OpenGL ES 2.0.
         * @requires_webgl20 Array textures are not available in WebGL 1.0.
         */
        void copySubImage(const Range2Di& rectangle, Texture2DArray& texture, Int level, const Vector3i& offset);
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        /**
         * @brief Copy block of pixels from framebuffer to cube map texture array subimage
         * @param rectangle         Framebuffer rectangle to copy
         * @param texture           Texture where to put the data
         * @param level             Texture mip level
         * @param offset            Offset inside the texture
         *
         * Z coordinate of the offset is equivalent to layer * 6 + number of
         * texture face, i.e. +X is `0` and so on, in order of (+X, -X, +Y, -Y,
         * +Z, -Z). If neither @extension{ARB,direct_state_access} (part of
         * OpenGL 4.5) nor @extension{EXT,direct_state_access} desktop
         * available, the texture is bound before the operation (if not
         * already).
         * @see @ref CubeMapTextureArray::setStorage(), @fn_gl{BindFramebuffer},
         *      then @fn_gl2{CopyTextureSubImage3D,CopyTexSubImage3D},
         *      @fn_gl_extension{CopyTextureSubImage3D,EXT,direct_state_access},
         *      eventually @fn_gl{ActiveTexture}, @fn_gl{BindTexture} and
         *      @fn_gl{CopyTexSubImage3D}
         * @requires_gl40 Extension @extension{ARB,texture_cube_map_array}
         * @requires_gles30 Not defined in OpenGL ES 2.0.
         * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
         *      @extension{EXT,texture_cube_map_array}
         * @requires_gles Cube map texture arrays are not available in WebGL.
         */
        void copySubImage(const Range2Di& rectangle, CubeMapTextureArray& texture, Int level, const Vector3i& offset);
        #endif

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        explicit AbstractFramebuffer(): _flags{ObjectFlag::DeleteOnDestruction} {}
        explicit AbstractFramebuffer(GLuint id, const Range2Di& viewport, ObjectFlags flags) noexcept: _id{id}, _viewport{viewport}, _flags{flags} {}

        ~AbstractFramebuffer() = default;

        void MAGNUM_LOCAL createIfNotAlready();

        void MAGNUM_LOCAL bindInternal(FramebufferTarget target);
        FramebufferTarget MAGNUM_LOCAL bindInternal();
        void MAGNUM_LOCAL setViewportInternal();

        GLuint _id;
        Range2Di _viewport;
        ObjectFlags _flags;

    private:
        #ifndef MAGNUM_TARGET_GLES2
        static void MAGNUM_LOCAL blitImplementationDefault(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL blitImplementationDSA(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);
        #endif
        #elif !defined(MAGNUM_TARGET_WEBGL)
        static void MAGNUM_LOCAL blitImplementationANGLE(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);
        static void MAGNUM_LOCAL blitImplementationNV(AbstractFramebuffer& source, AbstractFramebuffer& destination, const Range2Di& sourceRectangle, const Range2Di& destinationRectangle, FramebufferBlitMask mask, FramebufferBlitFilter filter);
        #endif

        void MAGNUM_LOCAL bindImplementationDefault(FramebufferTarget target);
        FramebufferTarget MAGNUM_LOCAL bindImplementationDefault();
        #ifdef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL bindImplementationSingle(FramebufferTarget);
        FramebufferTarget MAGNUM_LOCAL bindImplementationSingle();
        #endif

        GLenum MAGNUM_LOCAL checkStatusImplementationDefault(FramebufferTarget target);
        #ifdef MAGNUM_TARGET_GLES2
        GLenum MAGNUM_LOCAL checkStatusImplementationSingle(FramebufferTarget);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        GLenum MAGNUM_LOCAL checkStatusImplementationDSA(FramebufferTarget target);
        GLenum MAGNUM_LOCAL checkStatusImplementationDSAEXT(FramebufferTarget target);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL clearImplementationDefault(GLenum buffer, GLint drawbuffer, const GLint* value);
        void MAGNUM_LOCAL clearImplementationDefault(GLenum buffer, GLint drawbuffer, const GLuint* value);
        void MAGNUM_LOCAL clearImplementationDefault(GLenum buffer, GLint drawbuffer, const GLfloat* value);
        void MAGNUM_LOCAL clearImplementationDefault(GLenum buffer, GLfloat depth, GLint stencil);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL clearImplementationDSA(GLenum buffer, GLint drawbuffer, const GLint* value);
        void MAGNUM_LOCAL clearImplementationDSA(GLenum buffer, GLint drawbuffer, const GLuint* value);
        void MAGNUM_LOCAL clearImplementationDSA(GLenum buffer, GLint drawbuffer, const GLfloat* value);
        void MAGNUM_LOCAL clearImplementationDSA(GLenum buffer, GLfloat depth, GLint stencil);
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL drawBuffersImplementationDefault(GLsizei count, const GLenum* buffers);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL drawBuffersImplementationDSA(GLsizei count, const GLenum* buffers);
        void MAGNUM_LOCAL drawBuffersImplementationDSAEXT(GLsizei count, const GLenum* buffers);
        #endif
        #else
        void MAGNUM_LOCAL drawBuffersImplementationEXT(GLsizei count, const GLenum* buffers);
        #ifndef MAGNUM_TARGET_WEBGL
        void MAGNUM_LOCAL drawBuffersImplementationNV(GLsizei count, const GLenum* buffers);
        #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL drawBufferImplementationDefault(GLenum buffer);
        void MAGNUM_LOCAL drawBufferImplementationDSA(GLenum buffer);
        void MAGNUM_LOCAL drawBufferImplementationDSAEXT(GLenum buffer);
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        void MAGNUM_LOCAL readBufferImplementationDefault(GLenum buffer);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL readBufferImplementationDSA(GLenum buffer);
        void MAGNUM_LOCAL readBufferImplementationDSAEXT(GLenum buffer);
        #endif

        static void MAGNUM_LOCAL readImplementationDefault(const Range2Di& rectangle, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data);
        #ifndef MAGNUM_TARGET_WEBGL
        static void MAGNUM_LOCAL readImplementationRobustness(const Range2Di& rectangle, PixelFormat format, PixelType type, std::size_t dataSize, GLvoid* data);
        #endif

        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL copySub1DImplementationDefault(const Range2Di& rectangle, AbstractTexture& texture, Int level, Int offset);
        static void MAGNUM_LOCAL copySub1DImplementationDSA(const Range2Di& rectangle, AbstractTexture& texture, Int level, Int offset);
        static void MAGNUM_LOCAL copySub1DImplementationDSAEXT(const Range2Di& rectangle, AbstractTexture& texture, Int level, Int offset);
        #endif

        static void MAGNUM_LOCAL copySub2DImplementationDefault(const Range2Di& rectangle, AbstractTexture& texture, GLenum textureTarget, Int level, const Vector2i& offset);
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL copySub2DImplementationDSA(const Range2Di& rectangle, AbstractTexture& texture, GLenum textureTarget, Int level, const Vector2i& offset);
        static void MAGNUM_LOCAL copySubCubeMapImplementationDSA(const Range2Di& rectangle, AbstractTexture& texture, GLenum textureTarget, Int level, const Vector2i& offset);
        static void MAGNUM_LOCAL copySub2DImplementationDSAEXT(const Range2Di& rectangle, AbstractTexture& texture, GLenum textureTarget, Int level, const Vector2i& offset);
        #endif

        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        static void MAGNUM_LOCAL copySub3DImplementationDefault(const Range2Di& rectangle, AbstractTexture& texture, Int level, const Vector3i& offset);
        #endif
        #ifndef MAGNUM_TARGET_GLES
        static void MAGNUM_LOCAL copySub3DImplementationDSA(const Range2Di& rectangle, AbstractTexture& texture, Int level, const Vector3i& offset);
        static void MAGNUM_LOCAL copySub3DImplementationDSAEXT(const Range2Di& rectangle, AbstractTexture& texture, Int level, const Vector3i& offset);
        #endif

        void MAGNUM_LOCAL invalidateImplementationNoOp(GLsizei, const GLenum*);
        void MAGNUM_LOCAL invalidateImplementationDefault(GLsizei count, const GLenum* attachments);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateImplementationDSA(GLsizei count, const GLenum* attachments);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_LOCAL invalidateImplementationNoOp(GLsizei, const GLenum*, const Range2Di&);
        void MAGNUM_LOCAL invalidateImplementationDefault(GLsizei count, const GLenum* attachments, const Range2Di& rectangle);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_LOCAL invalidateImplementationDSA(GLsizei count, const GLenum* attachments, const Range2Di& rectangle);
        #endif
        #endif
};

CORRADE_ENUMSET_OPERATORS(FramebufferClearMask)
#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
CORRADE_ENUMSET_OPERATORS(FramebufferBlitMask)
#endif

}

#endif
