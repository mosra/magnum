#ifndef Magnum_DebugTools_TextureImage_h
#define Magnum_DebugTools_TextureImage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#ifdef MAGNUM_TARGET_GL
/** @file
 * @brief Function @ref Magnum::DebugTools::textureSubImage()
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/Magnum.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/GL/GL.h"

namespace Magnum { namespace DebugTools {

/**
@brief Read a range of given texture mip level to an image

Emulates @ref GL::Texture2D::subImage() on OpenGL ES and WebGL platforms by
creating a framebuffer object and using @ref GL::Framebuffer::read(). On
desktop OpenGL, if @gl_extension{ARB,get_texture_sub_image} is available, it's
just an alias to @ref GL::Texture2D::subImage().

The function expects that @p texture has a @ref GL::TextureFormat that's
framebuffer-readable and that the @ref GL::PixelFormat and @ref GL::PixelType
combination or the generic @relativeref{Magnum,PixelFormat} is compatible with
it. On OpenGL ES 3.0 and 3.1, @ref GL::PixelType::Float isn't guaranteed to be
supported for reading so such images are reinterpreted as
@ref GL::PixelType::UnsignedInt using an additional shader and the
@glsl floatBitsToUint() @ce GLSL function and then reinterpreted back to
@ref GL::PixelType::Float when read to client memory. On OpenGL ES 3.2 and on
desktop GL, @ref GL::PixelType::Float is guaranteed to be supported. No similar
treatment is done for any other types.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT void textureSubImage(GL::Texture2D& texture, Int level, const Range2Di& range, Image2D& image);

/**
@brief Read a range of given texture mip level to an image

Convenience alternative to the above, example usage:

@snippet DebugTools-gl.cpp textureSubImage-2D-rvalue

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT Image2D textureSubImage(GL::Texture2D& texture, Int level, const Range2Di& range, Image2D&& image);

/**
@brief Read a range of given cube map texture coordinate mip level to an image
@m_since_latest

Emulates @ref GL::CubeMapTexture::subImage() call on OpenGL ES and WebGL
platforms by creating a framebuffer object and using
@ref GL::Framebuffer::read().

The function expects that @p texture has a @ref GL::TextureFormat that's
framebuffer-readable and that the @ref GL::PixelFormat and @ref GL::PixelType
combination or the generic @relativeref{Magnum,PixelFormat} is compatible with
it.
@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT void textureSubImage(GL::CubeMapTexture& texture, GL::CubeMapCoordinate coordinate, Int level, const Range2Di& range, Image2D& image);

/**
@brief Read range of given cube map texture coordinate mip level to image

Convenience alternative to the above, example usage:

@snippet DebugTools-gl.cpp textureSubImage-cubemap-rvalue

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT Image2D textureSubImage(GL::CubeMapTexture& texture, GL::CubeMapCoordinate coordinate, Int level, const Range2Di& range, Image2D&& image);

#if defined(MAGNUM_BUILD_DEPRECATED) && !defined(MAGNUM_TARGET_GLES2)
/**
@brief Read a range of given texture mip level to a buffer image
@m_deprecated_since_latest As these APIs are mainly meant to be used for
    testing and verification, there's little point in having a variant that
    puts the data into a pixel buffer by creating a temporary framebuffer. Use
    @ref textureSubImage(GL::Texture2D&, Int, const Range2Di&, Image2D&)
    instead or populate the buffer using
    @ref GL::Framebuffer::read(const Range2Di&, GL::BufferImage2D&, GL::BufferUsage)
    on a non-temporary framebuffer.

Emulates @ref GL::Texture2D::subImage() call on OpenGL ES and WebGL platforms
by creating a framebuffer object and using @ref GL::Framebuffer::read(). On
desktop OpenGL, if @gl_extension{ARB,get_texture_sub_image} is available, it's
just an alias to @ref GL::Texture2D::subImage().

The function expects that @p texture has a @ref GL::TextureFormat that's
framebuffer-readable and that the @ref GL::PixelFormat and @ref GL::PixelType
combination or the generic @relativeref{Magnum,PixelFormat} is compatible with
it.
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
@requires_webgl20 Pixel buffer objects are not available in WebGL 1.0.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT CORRADE_DEPRECATED("use textureSubImage(GL::Texture2D&, Int, const Range2Di&, Image2D&) instead") void textureSubImage(GL::Texture2D& texture, Int level, const Range2Di& range, GL::BufferImage2D& image, GL::BufferUsage usage);

/**
@brief Read a range of given texture mip level to a buffer image
@m_deprecated_since_latest As these APIs are mainly meant to be used for
    testing and verification, there's little point in having a variant that
    puts the data into a buffer image by creating a temporary framebuffer. Use
    @ref textureSubImage(GL::Texture2D&, Int, const Range2Di&, Image2D&&)
    instead or populate the buffer image directly using
    @ref GL::Framebuffer::read(const Range2Di&, GL::BufferImage2D&&, GL::BufferUsage)
    on a non-temporary framebuffer.

Convenience alternative to the above, example usage:

@snippet DebugTools-gl.cpp textureSubImage-2D-rvalue-buffer

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT CORRADE_DEPRECATED("use textureSubImage(GL::Texture2D&, Int, const Range2Di&, Image2D&&) instead") GL::BufferImage2D textureSubImage(GL::Texture2D& texture, Int level, const Range2Di& range, GL::BufferImage2D&& image, GL::BufferUsage usage);

/**
@brief Read a range of given cube map texture coordinate mip level to a buffer image
@m_deprecated_since_latest As these APIs are mainly meant to be used for
    testing and verification, there's little point in having a variant that
    puts the data into a buffer image by creating a temporary framebuffer. Use
    @ref textureSubImage(GL::CubeMapTexture&, GL::CubeMapCoordinate, Int, const Range2Di&, Image2D&)
    instead or populate the buffer image directly using
    @ref GL::Framebuffer::read(const Range2Di&, GL::BufferImage2D&, GL::BufferUsage)
    on a non-temporary framebuffer.

Emulates @ref GL::CubeMapTexture::subImage() call on OpenGL ES and WebGL
platforms by creating a framebuffer object and using
@ref GL::Framebuffer::read().

The function expects that @p texture has a @ref GL::TextureFormat that's
framebuffer-readable and that the @ref GL::PixelFormat and @ref GL::PixelType
combination or the generic @relativeref{Magnum,PixelFormat} is compatible with
it.
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
@requires_webgl20 Pixel buffer objects are not available in WebGL 1.0.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT CORRADE_DEPRECATED("use textureSubImage(GL::CubeMapTexture2D&, GL::CubeMapCoordinate, Int, const Range2Di&, Image2D&) instead") void textureSubImage(GL::CubeMapTexture& texture, GL::CubeMapCoordinate coordinate, Int level, const Range2Di& range, GL::BufferImage2D& image, GL::BufferUsage usage);

/**
@brief Read a range of given cube map texture coordinate mip level to a buffer image
@m_deprecated_since_latest As these APIs are mainly meant to be used for
    testing and verification, there's little point in having a variant that
    puts the data into a buffer image by creating a temporary framebuffer. Use
    @ref textureSubImage(GL::CubeMapTexture&, GL::CubeMapCoordinate, Int, const Range2Di&, Image2D&)
    instead or populate the buffer image directly using
    @ref GL::Framebuffer::read(const Range2Di&, GL::BufferImage2D&, GL::BufferUsage)
    on a non-temporary framebuffer.

Convenience alternative to the above, example usage:

@snippet DebugTools-gl.cpp textureSubImage-cubemap-rvalue-buffer

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT CORRADE_DEPRECATED("use textureSubImage(GL::CubeMapTexture2D&, GL::CubeMapCoordinate, Int, const Range2Di&, Image2D&&) instead") GL::BufferImage2D textureSubImage(GL::CubeMapTexture& texture, GL::CubeMapCoordinate coordinate, Int level, const Range2Di& range, GL::BufferImage2D&& image, GL::BufferUsage usage);
#endif
#else
#error this header is available only in the OpenGL build
#endif

}}

#endif
