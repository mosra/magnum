#ifndef Magnum_DebugTools_TextureImage_h
#define Magnum_DebugTools_TextureImage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
 * @brief Function @ref Magnum::DebugTools::textureSubImage()
 */

#include "Magnum/Magnum.h"
#include "Magnum/DebugTools/visibility.h"

namespace Magnum { namespace DebugTools {

/**
@brief Read range of given texture mip level to image

Emulates @ref Texture::subImage() "*Texture::subImage()" call on platforms that
don't support it (such as OpenGL ES) by creating a framebuffer object and using
@ref Framebuffer::read(). Note that only @ref PixelFormat and @ref PixelType
values that are marked as framebuffer readable are supported.
*/
MAGNUM_DEBUGTOOLS_EXPORT void textureSubImage(Texture2D& texture, Int level, const Range2Di& range, Image2D& image);

/** @overload */
MAGNUM_DEBUGTOOLS_EXPORT void textureSubImage(CubeMapTexture& texture, CubeMapCoordinate coordinate, Int level, const Range2Di& range, Image2D& image);

/**
@brief Read range of given texture mip level to image

Convenience alternative to the above, example usage:
@code
Image2D image = DebugTools::textureSubImage(texture, 0, rect, {PixelFormat::RGBA, PixelType::UnsignedByte});
@endcode
*/
MAGNUM_DEBUGTOOLS_EXPORT Image2D textureSubImage(Texture2D& texture, Int level, const Range2Di& range, Image2D&& image);

/** @overload */
MAGNUM_DEBUGTOOLS_EXPORT Image2D textureSubImage(CubeMapTexture& texture, CubeMapCoordinate coordinate, Int level, const Range2Di& range, Image2D&& image);

#ifndef MAGNUM_TARGET_GLES2
/**
@brief Read range of given texture mip level to buffer image

Emulates @ref Texture::subImage() "*Texture::subImage()" call on platforms that
don't support it (such as OpenGL ES) by creating a framebuffer object and using
@ref Framebuffer::read(). Note that only @ref PixelFormat and @ref PixelType
values that are marked as framebuffer readable are supported.
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
@requires_webgl20 Pixel buffer objects are not available in WebGL 1.0.
*/
MAGNUM_DEBUGTOOLS_EXPORT void textureSubImage(Texture2D& texture, Int level, const Range2Di& range, BufferImage2D& image, BufferUsage usage);

/** @overload */
MAGNUM_DEBUGTOOLS_EXPORT void textureSubImage(CubeMapTexture& texture, CubeMapCoordinate coordinate, Int level, const Range2Di& range, BufferImage2D& image, BufferUsage usage);

/**
@brief Read range of given texture mip level to buffer image

Convenience alternative to the above, example usage:
@code
BufferImage2D image = DebugTools::textureSubImage(texture, 0, rect, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
@endcode
*/
MAGNUM_DEBUGTOOLS_EXPORT BufferImage2D textureSubImage(Texture2D& texture, Int level, const Range2Di& range, BufferImage2D&& image, BufferUsage usage);

/** @overload */
MAGNUM_DEBUGTOOLS_EXPORT BufferImage2D textureSubImage(CubeMapTexture& texture, CubeMapCoordinate coordinate, Int level, const Range2Di& range, BufferImage2D&& image, BufferUsage usage);
#endif

}}

#endif
