#ifndef Magnum_DebugTools_TextureImage_h
#define Magnum_DebugTools_TextureImage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/Magnum.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/GL/GL.h"

#ifdef MAGNUM_TARGET_GL
namespace Magnum { namespace DebugTools {

/**
@brief Read range of given texture mip level to image

Emulates @ref GL::Texture2D::subImage() call on platforms that don't support it
(such as OpenGL ES) by creating a framebuffer object and using
@ref GL::Framebuffer::read(). On desktop OpenGL, if @gl_extension{ARB,get_texture_sub_image}
is available, it's just an alias to @ref GL::Texture2D::subImage().

Note that only @ref GL::PixelFormat and @ref GL::PixelType values that are
marked as framebuffer readable are supported; their generic
@ref Magnum::PixelFormat "PixelFormat" counterparts are supported as well. In
addition, on OpenGL ES 3.0, images with @ref GL::PixelType::Float are supported
--- they are reinterpreted as @ref GL::PixelType::UnsignedInt using an
additional shader and the @glsl floatBitsToUint() @ce GLSL function and then
reinterpreted back to @ref GL::PixelType::Float when read to client memory.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT void textureSubImage(GL::Texture2D& texture, Int level, const Range2Di& range, Image2D& image);

/**
@brief Read range of given texture mip level to image

Convenience alternative to the above, example usage:

@snippet MagnumDebugTools-gl.cpp textureSubImage-2D-rvalue

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT Image2D textureSubImage(GL::Texture2D& texture, Int level, const Range2Di& range, Image2D&& image);

/**
@brief Read range of given cube map texture coordinate mip level to image

Emulates @ref GL::CubeMapTexture::subImage() call on platforms that don't
support it (such as OpenGL ES) by creating a framebuffer object and using
@ref GL::Framebuffer::read().

Note that only @ref GL::PixelFormat and @ref GL::PixelType values that are
marked as framebuffer readable are supported; their generic
@ref Magnum::PixelFormat "PixelFormat" counterparts are supported as well.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT void textureSubImage(GL::CubeMapTexture& texture, GL::CubeMapCoordinate coordinate, Int level, const Range2Di& range, Image2D& image);

/**
@brief Read range of given cube map texture coordinate mip level to image

Convenience alternative to the above, example usage:

@snippet MagnumDebugTools-gl.cpp textureSubImage-cubemap-rvalue

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT Image2D textureSubImage(GL::CubeMapTexture& texture, GL::CubeMapCoordinate coordinate, Int level, const Range2Di& range, Image2D&& image);

#ifndef MAGNUM_TARGET_GLES2
/**
@brief Read range of given texture mip level to buffer image

Emulates @ref GL::Texture2D::subImage() call on platforms that don't support it
(such as OpenGL ES) by creating a framebuffer object and using
@ref GL::Framebuffer::read(). On desktop OpenGL, if
@gl_extension{ARB,get_texture_sub_image} is available, it's just an alias to
@ref GL::Texture2D::subImage().

Note that only @ref GL::PixelFormat and @ref GL::PixelType values that are
marked as framebuffer readable are supported; their generic
@ref Magnum::PixelFormat "PixelFormat" counterparts are supported as well.
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
@requires_webgl20 Pixel buffer objects are not available in WebGL 1.0.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT void textureSubImage(GL::Texture2D& texture, Int level, const Range2Di& range, GL::BufferImage2D& image, GL::BufferUsage usage);

/**
@brief Read range of given texture mip level to buffer image

Convenience alternative to the above, example usage:

@snippet MagnumDebugTools-gl.cpp textureSubImage-2D-rvalue-buffer

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT GL::BufferImage2D textureSubImage(GL::Texture2D& texture, Int level, const Range2Di& range, GL::BufferImage2D&& image, GL::BufferUsage usage);

/**
@brief Read range of given cube map texture coordinate mip level to buffer image

Emulates @ref GL::CubeMapTexture::subImage() call on platforms that don't
support it (such as OpenGL ES) by creating a framebuffer object and using
@ref GL::Framebuffer::read().

Note that only @ref GL::PixelFormat and @ref GL::PixelType values that are
marked as framebuffer readable are supported; their generic
@ref Magnum::PixelFormat "PixelFormat" counterparts are supported as well.
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
@requires_webgl20 Pixel buffer objects are not available in WebGL 1.0.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT void textureSubImage(GL::CubeMapTexture& texture, GL::CubeMapCoordinate coordinate, Int level, const Range2Di& range, GL::BufferImage2D& image, GL::BufferUsage usage);

/**
@brief Read range of given cube map texture coordinate mip level to buffer image

Convenience alternative to the above, example usage:

@snippet MagnumDebugTools-gl.cpp textureSubImage-cubemap-rvalue-buffer

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL "TARGET_GL" enabled (done by default). See
    @ref building-features for more information.
*/
MAGNUM_DEBUGTOOLS_EXPORT GL::BufferImage2D textureSubImage(GL::CubeMapTexture& texture, GL::CubeMapCoordinate coordinate, Int level, const Range2Di& range, GL::BufferImage2D&& image, GL::BufferUsage usage);
#endif
#else
#error this header is available only in the OpenGL build
#endif

}}

#endif
