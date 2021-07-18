#ifndef Magnum_PixelFormat_h
#define Magnum_PixelFormat_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Enum @ref Magnum::PixelFormat, @ref Magnum::CompressedPixelFormat, function @ref Magnum::pixelSize(), @ref Magnum::isPixelFormatImplementationSpecific(), @ref Magnum::pixelFormatWrap(), @ref Magnum::pixelFormatUnwrap(), @ref Magnum::isCompressedPixelFormatImplementationSpecific(), @ref Magnum::compressedPixelFormatWrap(), @ref Magnum::compressedPixelFormatUnwrap()
 */

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/StlForwardString.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief Format of pixel data

Can act also as a wrapper for implementation-specific pixel format values using
@ref pixelFormatWrap() and @ref pixelFormatUnwrap(). Distinction between
generic and implementation-specific formats can be done using
@ref isPixelFormatImplementationSpecific().

In case of OpenGL, corresponds to @ref GL::PixelFormat and @ref GL::PixelType
and is convertible to them using @ref GL::pixelFormat() and
@ref GL::pixelType(). See documentation of each value for more information
about the mapping. Note that not every format is available on all targets, use
@ref GL::hasPixelFormat() to check for its presence.

In case of Vulkan, corresponds to @ref Vk::PixelFormat and is convertible
to it using @ref Vk::pixelFormat(Magnum::PixelFormat). See documentation of
each value for more information about the mapping. Note that not every format
may be available, use @ref Vk::hasPixelFormat(Magnum::PixelFormat) to check for
its presence.

For D3D, corresponds to @m_class{m-doc-external} [DXGI_FORMAT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
and import is provided by the @ref Trade::DdsImporter "DdsImporter" plugin; for
Metal, corresponds to @m_class{m-doc-external} [MTLPixelFormat](https://developer.apple.com/documentation/metal/mtlpixelformat?language=objc).
See documentation of each value for more information about the mapping.
@see @ref pixelSize(), @ref CompressedPixelFormat, @ref Image, @ref ImageView,
    @ref VertexFormat
*/
enum class PixelFormat: UnsignedInt {
    /* Zero reserved for an invalid format (but not being a named value) */

    /**
     * Red component, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::R8;
     * @ref Vk::PixelFormat::R8Unorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR8Unorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr8unorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R8_UNORM MTLPixelFormatR8Unorm}
     */
    R8Unorm = 1,

    /**
     * Red and green component, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RG8;
     * @ref Vk::PixelFormat::RG8Unorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8G8_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG8Unorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg8unorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R8G8_UNORM MTLPixelFormatRG8Unorm}
     */
    RG8Unorm,

    /**
     * RGB, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RGB8 or
     * @ref Vk::PixelFormat::RGB8Unorm. No 24-bit D3D or Metal equivalent.
     */
    RGB8Unorm,

    /**
     * RGBA, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RGBA8;
     * @ref Vk::PixelFormat::RGBA8Unorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8G8B8A8_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA8Unorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba8unorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R8G8B8A8_UNORM MTLPixelFormatRGBA8Unorm}
     */
    RGBA8Unorm,

    /**
     * Red component, normalized signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::R8Snorm;
     * @ref Vk::PixelFormat::R8Snorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8_SNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR8Snorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr8Snorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R8_SNORM MTLPixelFormatR8Snorm}
     */
    R8Snorm,

    /**
     * Red and green component, normalized signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RG8Snorm;
     * @ref Vk::PixelFormat::RG8Snorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8G8_SNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG8Snorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg8snorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R8G8_SNORM MTLPixelFormatRG8Snorm}
     */
    RG8Snorm,

    /**
     * RGB, normalized signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RGB8Snorm;
     * @ref Vk::PixelFormat::RGB8Snorm. No 24-bit D3D or Metal equivalent.
     */
    RGB8Snorm,

    /**
     * RGBA, normalized signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RGBA8Snorm;
     * @ref Vk::PixelFormat::RGBA8Snorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8G8B8A8_SNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA8Snorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba8snorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R8G8B8A8_SNORM MTLPixelFormatRGBA8Snorm}
     */
    RGBA8Snorm,

    /**
     * sRGB-encoded red component, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::SR8;
     * @ref Vk::PixelFormat::R8Srgb or
     * @m_class{m-doc-external} [MTLPixelFormatR8Unorm_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr8unorm_srgb?language=objc). No D3D equivalent.
     * @m_keywords{MTLPixelFormatR8Unorm_sRGB}
     * @m_since{2019,10}
     */
    R8Srgb,

    /**
     * sRGB-encoded red and green component, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::SRG8;
     * @ref Vk::PixelFormat::RG8Srgb or
     * @m_class{m-doc-external} [MTLPixelFormatRG8Unorm_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg8unorm_srgb?language=objc). No D3D equivalent.
     * @m_keywords{MTLPixelFormatRG8Unorm_sRGB}
     * @m_since{2019,10}
     */
    RG8Srgb,

    /**
     * sRGB, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::SRGB8 or
     * @ref Vk::PixelFormat::RGB8Srgb. No 24-bit D3D or Metal equivalent.
     * @m_since{2019,10}
     */
    RGB8Srgb,

    /**
     * sRGB + linear alpha, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::SRGB8Alpha8;
     * @ref Vk::PixelFormat::RGBA8Srgb;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8G8B8A8_UNORM_SRGB](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA8Unorm_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba8unorm_srgb?language=objc).
     * @m_keywords{DXGI_FORMAT_R8G8B8A8_UNORM_SRGB MTLPixelFormatRGBA8Unorm_sRGB}
     * @m_since{2019,10}
     */
    RGBA8Srgb,

    /**
     * Red component, integral unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::R8UI;
     * @ref Vk::PixelFormat::R8UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR8Uint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr8uint?language=objc).
     * @m_keywords{DXGI_FORMAT_R8_UINT MTLPixelFormatR8Uint}
     */
    R8UI,

    /**
     * Red and green component, integral unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RG8UI;
     * @ref Vk::PixelFormat::RG8UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8G8_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG8Uint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg8uint?language=objc).
     * @m_keywords{DXGI_FORMAT_R8G8_UINT MTLPixelFormatRG8Uint}
     */
    RG8UI,

    /**
     * RGB, integral unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RGB8UI or
     * @ref Vk::PixelFormat::RGB8UI. No 24-bit D3D or Metal equivalent.
     */
    RGB8UI,

    /**
     * RGBA, integral unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RGBA8UI;
     * @ref Vk::PixelFormat::RGBA8UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8G8B8A8_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA8Uint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba8uint?language=objc).
     * @m_keywords{DXGI_FORMAT_R8G8B8A8_UINT MTLPixelFormatRGBA8Uint}
     */
    RGBA8UI,

    /**
     * Red component, integral signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::R8I;
     * @ref Vk::PixelFormat::R8I;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8_SINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR8Sint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr8sint?language=objc).
     * @m_keywords{DXGI_FORMAT_R8_SINT MTLPixelFormatR8Sint}
     */
    R8I,

    /**
     * Red and green component, integral signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RG8I;
     * @ref Vk::PixelFormat::RG8I;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8G8_SINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG8Sint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg8sint?language=objc).
     * @m_keywords{DXGI_FORMAT_R8G8_SINT MTLPixelFormatRG8Sint}
     */
    RG8I,

    /**
     * RGB, integral signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RGB8I;
     * @ref Vk::PixelFormat::RGB8I. No 24-bit D3D or Metal
     * equivalent.
     */
    RGB8I,

    /**
     * RGBA, integral signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RGBA8I;
     * @ref Vk::PixelFormat::RGBA8I;
     * @m_class{m-doc-external} [DXGI_FORMAT_R8G8B8A8_SINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA8Sint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba8sint?language=objc).
     * @m_keywords{DXGI_FORMAT_R8G8B8A8_SINT MTLPixelFormatRGBA8Sint}
     */
    RGBA8I,

    /**
     * Red component, normalized unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::R16;
     * @ref Vk::PixelFormat::R16Unorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR16Unorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr16unorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R16_UNORM MTLPixelFormatR16Unorm}
     */
    R16Unorm,

    /**
     * Red and green component, normalized unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RG16;
     * @ref Vk::PixelFormat::RG16Unorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16G16_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG16Unorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg16unorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R16G16_UNORM MTLPixelFormatRG16Unorm}
     */
    RG16Unorm,

    /**
     * RGB, normalized unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RGB16 or
     * @ref Vk::PixelFormat::RGB16Unorm. No 48-bit D3D or Metal equivalent.
     */
    RGB16Unorm,

    /**
     * RGBA, normalized unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RGBA16;
     * @ref Vk::PixelFormat::RGBA16Unorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16G16B16A16_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA16Unorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba16unorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R16G16B16A16_UNORM MTLPixelFormatRGBA16Unorm}
     */
    RGBA16Unorm,

    /**
     * Red component, normalized signed short.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::R16Snorm;
     * @ref Vk::PixelFormat::R16Snorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16_SNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR16Snorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr16snorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R16_SNORM MTLPixelFormatR16Snorm}
     */
    R16Snorm,

    /**
     * Red and green component, normalized signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RG16Snorm;
     * @ref Vk::PixelFormat::RG16Snorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16G16_SNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG16Snorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg16snorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R16G16_SNORM MTLPixelFormatRG16Snorm}
     */
    RG16Snorm,

    /**
     * RGB, normalized signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RGB16Snorm;
     * @ref Vk::PixelFormat::RGB16Snorm. No 48-bit D3D or Metal equivalent.
     */
    RGB16Snorm,

    /**
     * RGBA, normalized signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RGBA16Snorm;
     * @ref Vk::PixelFormat::RGBA16Snorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16G16B16A16_SNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA16Snorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba16snorm?language=objc).
     * @m_keywords{DXGI_FORMAT_R16G16B16A16_SNORM MTLPixelFormatRGBA16Snorm}
     */
    RGBA16Snorm,

    /**
     * Red component, integral unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::R16UI;
     * @ref Vk::PixelFormat::R16UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR16Uint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr16uint?language=objc).
     * @m_keywords{DXGI_FORMAT_R16_UINT MTLPixelFormatR16Uint}
     */
    R16UI,

    /**
     * Red and green component, integral unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RG16UI;
     * @ref Vk::PixelFormat::RG16UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16G16_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG16Uint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg16uint?language=objc).
     * @m_keywords{DXGI_FORMAT_R16G16_UINT MTLPixelFormatRG16Uint}
     */
    RG16UI,

    /**
     * RGB, integral unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RGB16UI;
     * @ref Vk::PixelFormat::RGB16UI. No 48-bit D3D or Metal equivalent.
     */
    RGB16UI,

    /**
     * RGBA, integral unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RGBA16UI;
     * @ref Vk::PixelFormat::RGBA16UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16G16B16A16_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA16Uint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba16uint?language=objc).
     * @m_keywords{DXGI_FORMAT_R16G16B16A16_UINT MTLPixelFormatRGBA16Uint}
     */
    RGBA16UI,

    /**
     * Red component, integral signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::R16I;
     * @ref Vk::PixelFormat::R16I;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16_SINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR16Sint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr16sint?language=objc).
     * @m_keywords{DXGI_FORMAT_R16_SINT MTLPixelFormatR16Sint}
     */
    R16I,

    /**
     * Red and green component, integral signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RG16I;
     * @ref Vk::PixelFormat::RG16I;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16G16_SINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG16Sint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg16sint?language=objc).
     * @m_keywords{DXGI_FORMAT_R16G16_SINT MTLPixelFormatRG16Sint}
     */
    RG16I,

    /**
     * RGB, integral signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RGB16I;
     * @ref Vk::PixelFormat::RGB16I. No 48-bit D3D or Metal equivalent.
     */
    RGB16I,

    /**
     * RGBA, integral signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RGBA16I;
     * @ref Vk::PixelFormat::RGBA16I;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16G16B16A16_SINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA16Sint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba16sint?language=objc).
     * @m_keywords{DXGI_FORMAT_R16G16B16A16_SINT MTLPixelFormatRGBA16Sint}
     */
    RGBA16I,

    /**
     * Red component, integral unsigned int.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::UnsignedInt, @ref GL::TextureFormat::R32UI;
     * @ref Vk::PixelFormat::R32UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_R32_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR32Uint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr32uint?language=objc).
     * @m_keywords{DXGI_FORMAT_R32_UINT MTLPixelFormatR32Uint}
     */
    R32UI,

    /**
     * Red and green component, integral unsigned int.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::UnsignedInt, @ref GL::TextureFormat::RG32UI;
     * @ref Vk::PixelFormat::RG32UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_R32G32_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG32Uint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg32uint?language=objc).
     * @m_keywords{DXGI_FORMAT_R32G32_UINT MTLPixelFormatRG32Uint}
     */
    RG32UI,

    /**
     * RGB, integral unsigned int.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::UnsignedInt, @ref GL::TextureFormat::RGB32UI;
     * @ref Vk::PixelFormat::RGB32UI or
     * @m_class{m-doc-external} [DXGI_FORMAT_R32G32B32_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format).
     * No 96-bit Metal equivalent.
     * @m_keywords{DXGI_FORMAT_R32G32B32_UINT}
     */
    RGB32UI,

    /**
     * RGBA, integral unsigned int.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::UnsignedInt, @ref GL::TextureFormat::RGBA32UI;
     * @ref Vk::PixelFormat::RGBA32UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_R32G32B32A32_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA32Uint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba32uint?language=objc).
     * @m_keywords{DXGI_FORMAT_R32G32B32A32_UINT MTLPixelFormatRGBA32Uint}
     */
    RGBA32UI,

    /**
     * Red component, integral signed int.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::Int, @ref GL::TextureFormat::R32I;
     * @ref Vk::PixelFormat::R32I;
     * @m_class{m-doc-external} [DXGI_FORMAT_R32_SINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR32Sint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr32sint?language=objc).
     * @m_keywords{DXGI_FORMAT_R32_SINT MTLPixelFormatR32Sint}
     */
    R32I,

    /**
     * Red and green component, integral signed int.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::Int, @ref GL::TextureFormat::RG32I;
     * @ref Vk::PixelFormat::RG32I;
     * @m_class{m-doc-external} [DXGI_FORMAT_R32G32_SINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG32Sint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg32sint?language=objc).
     * @m_keywords{DXGI_FORMAT_R32G32_SINT MTLPixelFormatRG32Sint}
     */
    RG32I,

    /**
     * RGB, integral signed int.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::Int, @ref GL::TextureFormat::RGB32I;
     * @ref Vk::PixelFormat::RGB32I or
     * @m_class{m-doc-external} [DXGI_FORMAT_R32G32B32_SINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format).
     * No 96-bit Metal equivalent.
     * @m_keywords{DXGI_FORMAT_R32G32B32_SINT}
     */
    RGB32I,

    /**
     * RGBA, integral signed int.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::Int, @ref GL::TextureFormat::RGBA32I;
     * @ref Vk::PixelFormat::RGBA32I;
     * @m_class{m-doc-external} [DXGI_FORMAT_R32G32B32A32_SINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA32Sint](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba32sint?language=objc).
     * @m_keywords{DXGI_FORMAT_R32G32B32A32_SINT MTLPixelFormatRGBA32Sint}
     */
    RGBA32I,

    /**
     * Red component, half float.
     *
     * Corresponds to @ref GL::PixelFormat::Red and @ref GL::PixelType::Half,
     * @ref GL::TextureFormat::R16F; @ref Vk::PixelFormat::R16F;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16_FLOAT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR16Float](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr16float?language=objc).
     * @see @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     * @m_keywords{DXGI_FORMAT_R16_FLOAT MTLPixelFormatR16Float}
     */
    R16F,

    /**
     * Red and green component, half float.
     *
     * Corresponds to @ref GL::PixelFormat::RG and @ref GL::PixelType::Half,
     * @ref GL::TextureFormat::RG16F; @ref Vk::PixelFormat::RG16F;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16G16_FLOAT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG16Float](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg16float?language=objc).
     * @see @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     * @m_keywords{DXGI_FORMAT_R16G16_FLOAT MTLPixelFormatRG16Float}
     */
    RG16F,

    /**
     * RGB, half float.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and @ref GL::PixelType::Half,
     * @ref GL::TextureFormat::RGB16F; @ref Vk::PixelFormat::RGB16F. No 48-bit
     * D3D or Metal equivalent.
     * @see @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     */
    RGB16F,

    /**
     * RGBA, half float.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and @ref GL::PixelType::Half,
     * @ref GL::TextureFormat::RGBA16F; @ref Vk::PixelFormat::RGBA16F;
     * @m_class{m-doc-external} [DXGI_FORMAT_R16G16B16A16_FLOAT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA16Float](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba16float?language=objc).
     * @see @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     * @m_keywords{DXGI_FORMAT_R16G16B16A16_FLOAT MTLPixelFormatRGBA16Float}
     */
    RGBA16F,

    /**
     * Red component, float.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::Float, @ref GL::TextureFormat::R32F;
     * @ref Vk::PixelFormat::R32F;
     * @m_class{m-doc-external} [DXGI_FORMAT_R32_FLOAT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatR32Float](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatr32float?language=objc).
     * @m_keywords{DXGI_FORMAT_R32_FLOAT MTLPixelFormatR32Float}
     */
    R32F,

    /**
     * Red and green component, float.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::Float, @ref GL::TextureFormat::RG32F;
     * @ref Vk::PixelFormat::RG32F;
     * @m_class{m-doc-external} [DXGI_FORMAT_R32G32_FLOAT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRG32Float](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrg32float?language=objc).
     * @m_keywords{DXGI_FORMAT_R32G32_FLOAT MTLPixelFormatRG32Float}
     */
    RG32F,

    /**
     * RGB, float.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::Float, @ref GL::TextureFormat::RGB32F;
     * @ref Vk::PixelFormat::RGB32F or
     * @m_class{m-doc-external} [DXGI_FORMAT_R32G32B32_FLOAT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format).
     * No 96-bit Metal equivalent.
     * @m_keywords{DXGI_FORMAT_R32G32B32_UINT}
     */
    RGB32F,

    /**
     * RGBA, float.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::Float, @ref GL::TextureFormat::RGBA32F;
     * @ref Vk::PixelFormat::RGBA32F;
     * @m_class{m-doc-external} [DXGI_FORMAT_R32G32B32A32_FLOAT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatRGBA32Float](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatrgba32float?language=objc).
     * @m_keywords{DXGI_FORMAT_R32G32B32A32_FLOAT MTLPixelFormatRGBA32Float}
     */
    RGBA32F,

    /**
     * 16-bit unsigned normalized depth.
     *
     * Corresponds to @ref GL::PixelFormat::DepthComponent and
     * @ref GL::PixelType::UnsignedShort,
     * @ref GL::TextureFormat::DepthComponent16;
     * @ref Vk::PixelFormat::Depth16Unorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_D16_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatDepth16Unorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatdepth16unorm?language=objc).
     * @m_keywords{DXGI_FORMAT_D16_UNORM MTLPixelFormatDepth16Unorm}
     * @m_since_latest
     */
    Depth16Unorm,

    /**
     * 24-bit unsigned normalized depth. Data layout consistent with the
     * [KTX2 specification](http://github.khronos.org/KTX-Specification/#_depth_and_stencil_formats)
     * --- aligned to 32 bits with the 8 most significant bits unused.
     *
     * Corresponds to @ref GL::PixelFormat::DepthComponent and
     * @ref GL::PixelType::UnsignedInt,
     * @ref GL::TextureFormat::DepthComponent24;
     * @ref Vk::PixelFormat::Depth24Unorm. No direct 24-bit D3D or Metal
     * equivalent.
     * @m_since_latest
     * @todo wait, how GL interprets this? does it discard the top 8 bits? or
     *      should I use UnsignedInt248 instead?
     */
    Depth24Unorm,

    /**
     * 32-bit float depth.
     *
     * Corresponds to @ref GL::PixelFormat::DepthComponent and
     * @ref GL::PixelType::Float, @ref GL::TextureFormat::DepthComponent32F;
     * @ref Vk::PixelFormat::Depth32F;
     * @m_class{m-doc-external} [DXGI_FORMAT_D32_FLOAT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatDepth32Float](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatdepth32float?language=objc).
     * @m_keywords{DXGI_FORMAT_D32_FLOAT MTLPixelFormatDepth32Float}
     * @m_since_latest
     */
    Depth32F,

    /**
     * 8-bit unsigned integral stencil.
     *
     * Corresponds to @ref GL::PixelFormat::StencilIndex and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::StencilIndex8;
     * @ref Vk::PixelFormat::Stencil8UI
     * or @m_class{m-doc-external} [MTLPixelFormatStencil8](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatstencil8?language=objc). No direct 8-bit D3D equivalent.
     * @m_keywords{MTLPixelFormatStencil8}
     * @m_since_latest
     */
    Stencil8UI,

    /**
     * 16-bit unsigned normalized depth with 8-bit unsigned integral stencil.
     * Data layout consistent with the [KTX2 specification](http://github.khronos.org/KTX-Specification/#_depth_and_stencil_formats)
     * --- aligned to 32 bits with depth in the 16 least significant bits and
     * the 8 most significant bits unused.
     *
     * Corresponds to @ref Vk::PixelFormat::Depth16UnormStencil8UI. No direct
     * 32-bit GL, D3D or Metal equivalent.
     * @m_since_latest
     */
    Depth16UnormStencil8UI,

    /**
     * 24-bit unsigned normalized depth with 8-bit unsigned integral stencil.
     * Data layout consistent with the [KTX2 specification](http://github.khronos.org/KTX-Specification/#_depth_and_stencil_formats)
     * --- packed to 32 bits with depth in the 24 least significant bits
     * and stencil being in the 8 most significant bits.
     *
     * Corresponds to @ref GL::PixelFormat::DepthStencil and
     * @ref GL::PixelType::UnsignedInt248,
     * @ref GL::TextureFormat::Depth24Stencil8;
     * @ref Vk::PixelFormat::Depth24UnormStencil8UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_D24_UNORM_S8_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatDepth24Unorm_Stencil8](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatdepth24unorm_stencil8?language=objc).
     * @m_keywords{DXGI_FORMAT_D24_UNORM_S8_UINT MTLPixelFormatDepth24Unorm_Stencil8}
     * @m_since_latest
     */
    Depth24UnormStencil8UI,

    /**
     * 32-bit float depth with 8-bit unsigned integral stencil. Data layout
     * consistent with the [KTX2 specification](http://github.khronos.org/KTX-Specification/#_depth_and_stencil_formats)
     * --- packed to 64 bits with the depth in the 32 least significant
     * bits and the 24 most significant bits unused.
     *
     * Corresponds to @ref GL::PixelFormat::DepthStencil and
     * @ref GL::PixelType::Float32UnsignedInt248Rev,
     * @ref GL::TextureFormat::Depth32FStencil8;
     * @ref Vk::PixelFormat::Depth32FStencil8UI;
     * @m_class{m-doc-external} [DXGI_FORMAT_D32_FLOAT_S8X24_UINT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatDepth32Float_Stencil8](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatdepth32float_stencil8?language=objc).
     * @m_keywords{DXGI_FORMAT_D32_FLOAT_S8X24_UINT MTLPixelFormatDepth32Float_Stencil8}
     * @m_since_latest
     */
    Depth32FStencil8UI
};

/**
@brief Size of a pixel

Expects that the pixel format is *not* implementation-specific.
@see @ref isPixelFormatImplementationSpecific(), @ref GL::pixelSize()
*/
MAGNUM_EXPORT UnsignedInt pixelSize(PixelFormat format);

/** @debugoperatorenum{PixelFormat} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, PixelFormat value);

/**
@brief Whether a @ref PixelFormat value wraps an implementation-specific identifier

Returns @cpp true @ce if value of @p format has its highest bit set, @cpp false @ce
otherwise. Use @ref pixelFormatWrap() and @ref pixelFormatUnwrap() to
wrap/unwrap an implementation-specific indentifier to/from @ref PixelFormat.
@see @ref isCompressedPixelFormatImplementationSpecific()
*/
constexpr bool isPixelFormatImplementationSpecific(PixelFormat format) {
    return UnsignedInt(format) & (1u << 31);
}

/**
@brief Wrap an implementation-specific pixel format identifier in @ref PixelFormat

Sets the highest bit on @p format to mark it as implementation-specific.
Expects that @p format fits into the remaining bits. Use @ref pixelFormatUnwrap()
for the inverse operation.
@see @ref isPixelFormatImplementationSpecific(), @ref compressedPixelFormatWrap()
*/
template<class T> constexpr PixelFormat pixelFormatWrap(T implementationSpecific) {
    static_assert(sizeof(T) <= 4,
        "format types larger than 32bits are not supported");
    return CORRADE_CONSTEXPR_ASSERT(!(UnsignedInt(implementationSpecific) & (1u << 31)),
        "pixelFormatWrap(): implementation-specific value" << reinterpret_cast<void*>(implementationSpecific) << "already wrapped or too large"),
        PixelFormat((1u << 31)|UnsignedInt(implementationSpecific));
}

/**
@brief Unwrap an implementation-specific pixel format identifier from @ref PixelFormat

Unsets the highest bit from @p format to extract the implementation-specific
value. Expects that @p format has it set. Use @ref pixelFormatWrap() for the
inverse operation.
@see @ref isPixelFormatImplementationSpecific(), @ref compressedPixelFormatUnwrap()
*/
template<class T = UnsignedInt> constexpr T pixelFormatUnwrap(PixelFormat format) {
    return CORRADE_CONSTEXPR_ASSERT(UnsignedInt(format) & (1u << 31),
        "pixelFormatUnwrap():" << format << "isn't a wrapped implementation-specific value"),
        T(UnsignedInt(format) & ~(1u << 31));
}

/**
@brief Format of compressed pixel data

Can act also as a wrapper for implementation-specific pixel format values using
@ref compressedPixelFormatWrap() and @ref compressedPixelFormatUnwrap().
Distinction between generic and implementation-specific formats can be done
using @ref isCompressedPixelFormatImplementationSpecific().

In case of OpenGL, corresponds to @ref GL::CompressedPixelFormat and is
convertible to it using @ref GL::compressedPixelFormat(). See documentation of
each value for more information about the mapping. Note that not every format
is available on all targets, use @ref GL::hasCompressedPixelFormat() to check
for its presence.

In case of Vulkan, corresponds to @ref Vk::PixelFormat and is convertible to it
using @ref Vk::pixelFormat(Magnum::CompressedPixelFormat). See documentation
of each value for more information about the mapping. Note that not every
format may be available, use @ref Vk::hasPixelFormat(Magnum::CompressedPixelFormat)
to check for its presence.

For D3D, corresponds to @m_class{m-doc-external} [DXGI_FORMAT](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
and import is provided by the @ref Trade::DdsImporter "DdsImporter" plugin; for
Metal, corresponds to @m_class{m-doc-external} [MTLPixelFormat](https://developer.apple.com/documentation/metal/mtlpixelformat?language=objc).
See documentation of each value for more information about the mapping.
@see @ref compressedBlockSize(), @ref compressedBlockDataSize(),
    @ref PixelFormat, @ref CompressedImage, @ref CompressedImageView
*/
enum class CompressedPixelFormat: UnsignedInt {
    /* Zero reserved for an invalid format (but not being a named value) */

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC1
     * compressed RGB, normalized unsigned (DXT1). 64bit 4x4 blocks with a
     * 1-bit alpha component that's ignored.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBS3tcDxt1,
     * @ref GL::TextureFormat::CompressedRGBS3tcDxt1 or
     * @ref Vk::PixelFormat::CompressedBc1RGBUnorm. No D3D or Metal
     * equivalent.
     */
    Bc1RGBUnorm = 1,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC1
     * compressed sRGB, normalized unsigned (DXT1). 64bit 4x4 blocks with a
     * 1-bit alpha component that's ignored.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGBS3tcDxt1,
     * @ref GL::TextureFormat::CompressedSRGBS3tcDxt1 or
     * @ref Vk::PixelFormat::CompressedBc1RGBSrgb. No D3D or Metal
     * equivalent.
     * @m_since{2019,10}
     */
    Bc1RGBSrgb,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC1
     * compressed RGBA, normalized unsigned (DXT1). 64bit 4x4 blocks with a
     * 1-bit alpha component.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAS3tcDxt1,
     * @ref GL::TextureFormat::CompressedRGBAS3tcDxt1;
     * @ref Vk::PixelFormat::CompressedBc1RGBAUnorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC1_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC1_RGBA](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc1_rgba?language=objc).
     * @m_keywords{DXGI_FORMAT_BC1_UNORM MTLPixelFormatBC1_RGBA}
     */
    Bc1RGBAUnorm,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC1
     * compressed sRGB + linear alpha, normalized unsigned (DXT1). 64bit 4x4
     * blocks with a 1-bit alpha component.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGBAlphaS3tcDxt1,
     * @ref GL::TextureFormat::CompressedSRGBAlphaS3tcDxt1;
     * @ref Vk::PixelFormat::CompressedBc1RGBASrgb;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC1_UNORM_SRGB](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC1_RGBA_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc1_rgba_srgb?language=objc).
     * @m_keywords{DXGI_FORMAT_BC1_UNORM_SRGB MTLPixelFormatBC1_RGBA_sRGB}
     * @m_since{2019,10}
     */
    Bc1RGBASrgb,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC2
     * compressed RGBA, normalized unsigned (DXT3). A 128-bit 4x4 block,
     * consisting of a 64-bit explicit alpha block followed by a 64-bit color
     * block encoded the same way as BC1.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAS3tcDxt3,
     * @ref GL::TextureFormat::CompressedRGBAS3tcDxt3;
     * @ref Vk::PixelFormat::CompressedBc2RGBAUnorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC2_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC2_RGBA](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc2_rgba?language=objc).
     * @m_keywords{DXGI_FORMAT_BC2_UNORM MTLPixelFormatBC2_RGBA}
     */
    Bc2RGBAUnorm,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC2
     * compressed sRGB + linear alpha, normalized unsigned (DXT3). A 128-bit
     * 4x4 block, consisting of a 64-bit explicit alpha block followed by a
     * 64-bit color block encoded the same way as BC1.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGBAlphaS3tcDxt3,
     * @ref GL::TextureFormat::CompressedSRGBAlphaS3tcDxt3;
     * @ref Vk::PixelFormat::CompressedBc2RGBASrgb;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC2_UNORM_SRGB](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC2_RGBA_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc2_rgba_srgb?language=objc).
     * @m_keywords{DXGI_FORMAT_BC2_UNORM_SRGB MTLPixelFormatBC2_RGBA_sRGB}
     * @m_since{2019,10}
     */
    Bc2RGBASrgb,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC3
     * compressed RGBA, normalized unsigned (DXT5). A 128-bit 4x4 block,
     * consisting of a 64-bit interpolated alpha block followed by a 64-bit
     * color block encoded the same way as BC1.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAS3tcDxt5,
     * @ref GL::TextureFormat::CompressedRGBAS3tcDxt5;
     * @ref Vk::PixelFormat::CompressedBc3RGBAUnorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC3_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC3_RGBA](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc3_rgba?language=objc).
     * @m_keywords{DXGI_FORMAT_BC3_UNORM MTLPixelFormatBC3_RGBA}
     */
    Bc3RGBAUnorm,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC3
     * compressed sRGB + linear alpha, normalized unsigned (DXT5). A 128-bit
     * 4x4 block,  consisting of a 64-bit interpolated alpha block followed by
     * a 64-bit color block encoded the same way as BC1.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGBAlphaS3tcDxt5,
     * @ref GL::TextureFormat::CompressedSRGBAlphaS3tcDxt5;
     * @ref Vk::PixelFormat::CompressedBc3RGBASrgb;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC3_UNORM_SRGB](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC3_RGBA_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc3_rgba_srgb?language=objc).
     * @m_keywords{DXGI_FORMAT_BC3_UNORM_SRGB MTLPixelFormatBC3_RGBA_sRGB}
     * @m_since{2019,10}
     */
    Bc3RGBASrgb,

    /**
     * [3Dc+](https://en.wikipedia.org/wiki/3Dc#3Dc+) BC4 compressed red
     * component, unsigned normalized. Also known as RGTC1 or LATC1. A 64-bit
     * 4x4 block encoded the same way as the alpha block of BC3.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RedRgtc1,
     * @ref GL::TextureFormat::CompressedRedRgtc1;
     * @ref Vk::PixelFormat::CompressedBc4RUnorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC4_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC4_RUnorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc4_runorm?language=objc).
     * @m_keywords{DXGI_FORMAT_BC4_UNORM MTLPixelFormatBC4_RUnorm}
     * @m_since{2019,10}
     */
    Bc4RUnorm,

    /**
     * [3Dc+](https://en.wikipedia.org/wiki/3Dc#3Dc+) BC4 compressed red
     * component, signed normalized. Also known as RGTC1 or LATC1. A 64-bit 4x4
     * block encoded the same way as the alpha block of BC3.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SignedRedRgtc1,
     * @ref GL::TextureFormat::CompressedSignedRedRgtc1;
     * @ref Vk::PixelFormat::CompressedBc4RSnorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC4_SNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC4_RSnorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc4_rsnorm?language=objc).
     * @m_keywords{DXGI_FORMAT_BC4_SNORM MTLPixelFormatBC4_RSnorm}
     * @m_since{2019,10}
     */
    Bc4RSnorm,

    /**
     * [3Dc](https://en.wikipedia.org/wiki/3Dc) BC5 compressed red and green
     * component, unsigned normalized. Also known as RGTC2 or LATC2. A 128-bit
     * 4x4 block consisting of two 64bit blocks for each channel encoded the
     * same way as the alpha block of BC3.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGRgtc2,
     * @ref GL::TextureFormat::CompressedRGRgtc2;
     * @ref Vk::PixelFormat::CompressedBc5RGUnorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC5_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC5_RGUnorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc5_rgunorm?language=objc).
     * @m_keywords{DXGI_FORMAT_BC5_UNORM MTLPixelFormatBC5_RGUnorm}
     * @m_since{2019,10}
     */
    Bc5RGUnorm,

    /**
     * [3Dc](https://en.wikipedia.org/wiki/3Dc) BC5 compressed red and green
     * component, signed normalized. Also known as RGTC2 or LATC2. A 128-bit
     * 4x4 block consisting of two 64bit blocks for each channel encoded the
     * same way as the alpha block of BC3.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SignedRGRgtc2,
     * @ref GL::TextureFormat::CompressedSignedRGRgtc2;
     * @ref Vk::PixelFormat::CompressedBc5RGSnorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC5_SNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC5_RGSnorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc5_rgsnorm?language=objc).
     * @m_keywords{DXGI_FORMAT_BC5_SNORM MTLPixelFormatBC5_RGSnorm}
     * @m_since{2019,10}
     */
    Bc5RGSnorm,

    /**
     * [BC6H](https://docs.microsoft.com/en-us/windows/win32/direct3d11/bc6h-format)
     * compressed RGB, unsigned float. Also known as BPTC.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBBptcUnsignedFloat,
     * @ref GL::TextureFormat::CompressedRGBBptcUnsignedFloat;
     * @ref Vk::PixelFormat::CompressedBc6hRGBUfloat;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC6H_UF16](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC6H_RGBUfloat](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc6h_rgbufloat?language=objc).
     * @m_keywords{DXGI_FORMAT_BC6H_UF16 MTLPixelFormatBC6H_RGBUfloat}
     * @m_since{2019,10}
     */
    Bc6hRGBUfloat,

    /**
     * [BC6H](https://docs.microsoft.com/en-us/windows/win32/direct3d11/bc6h-format)
     * compressed RGB, signed float. Also known as BPTC.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBBptcSignedFloat,
     * @ref GL::TextureFormat::CompressedRGBBptcSignedFloat;
     * @ref Vk::PixelFormat::CompressedBc6hRGBSfloat;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC6H_SF16](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC6H_RGBFloat](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc6h_rgbfloat?language=objc).
     * @m_keywords{DXGI_FORMAT_BC6H_UF16 MTLPixelFormatBC6H_RGBFloat}
     * @m_since{2019,10}
     */
    Bc6hRGBSfloat,

    /**
     * [BC7](https://docs.microsoft.com/en-us/windows/win32/direct3d11/bc7-format),
     * compressed RGBA, unsigned normalized. Also known as BPTC.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBABptcUnorm,
     * @ref GL::TextureFormat::CompressedRGBABptcUnorm;
     * @ref Vk::PixelFormat::CompressedBc7RGBAUnorm;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC7_UNORM](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC7_RGBAUnorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc7_rgbaunorm?language=objc).
     * @m_keywords{DXGI_FORMAT_BC7_UNORM MTLPixelFormatBC7_RGBAUnorm}
     * @m_since{2019,10}
     */
    Bc7RGBAUnorm,

    /**
     * [BC7](https://docs.microsoft.com/en-us/windows/win32/direct3d11/bc7-format),
     * compressed sRGB + linear alpha, unsigned normalized. Also known as BPTC.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGBAlphaBptcUnorm,
     * @ref GL::TextureFormat::CompressedSRGBAlphaBptcUnorm;
     * @ref Vk::PixelFormat::CompressedBc7RGBASrgb;
     * @m_class{m-doc-external} [DXGI_FORMAT_BC7_UNORM_SRGB](https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format)
     * or @m_class{m-doc-external} [MTLPixelFormatBC7_RGBAUnorm_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatbc7_rgbaunorm_srgb?language=objc).
     * @m_keywords{DXGI_FORMAT_BC7_UNORM_SRGB MTLPixelFormatBC7_RGBAUnorm_sRGB}
     * @m_since{2019,10}
     */
    Bc7RGBASrgb,

    /**
     * [EAC](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed red component, normalized unsigned 11-bit.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::R11Eac,
     * @ref GL::TextureFormat::CompressedR11Eac;
     * @ref Vk::PixelFormat::CompressedEacR11Unorm or
     * @m_class{m-doc-external} [MTLPixelFormatEAC_R11Unorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformateac_r11unorm?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatEAC_R11Unorm}
     * @m_since{2019,10}
     */
    EacR11Unorm,

    /**
     * [EAC](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed red component, normalized signed 11-bit.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SignedR11Eac,
     * @ref GL::TextureFormat::CompressedSignedR11Eac;
     * @ref Vk::PixelFormat::CompressedEacR11Snorm or
     * @m_class{m-doc-external} [MTLPixelFormatEAC_R11Snorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformateac_r11snorm?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatEAC_R11Unorm}
     * @m_since{2019,10}
     */
    EacR11Snorm,

    /**
     * [EAC](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed red and green component, normalized unsigned 11-bit.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RG11Eac,
     * @ref GL::TextureFormat::CompressedRG11Eac;
     * @ref Vk::PixelFormat::CompressedEacRG11Unorm or
     * @m_class{m-doc-external} [MTLPixelFormatEAC_RG11Unorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformateac_rg11unorm?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatEAC_R11Unorm}
     * @m_since{2019,10}
     */
    EacRG11Unorm,

    /**
     * [EAC](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed red and green component, normalized signed 11-bit.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SignedRG11Eac,
     * @ref GL::TextureFormat::CompressedSignedRG11Eac;
     * @ref Vk::PixelFormat::CompressedEacRG11Snorm or
     * @m_class{m-doc-external} [MTLPixelFormatEAC_RG11Snorm](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformateac_rg11snorm?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatEAC_RG11Snorm}
     * @m_since{2019,10}
     */
    EacRG11Snorm,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed RGB, normalized unsigned byte.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGB8Etc2,
     * @ref GL::TextureFormat::CompressedRGB8Etc2;
     * @ref Vk::PixelFormat::CompressedEtc2RGB8Unorm or
     * @m_class{m-doc-external} [MTLPixelFormatETC2_RGB8](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatetc2_rgb8?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatETC2_RGB8}
     * @m_since{2019,10}
     */
    Etc2RGB8Unorm,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed sRGB, normalized unsigned byte.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Etc2,
     * @ref GL::TextureFormat::CompressedSRGB8Etc2;
     * @ref Vk::PixelFormat::CompressedEtc2RGB8Srgb or
     * @m_class{m-doc-external} [MTLPixelFormatETC2_RGB8_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatetc2_rgb8_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatETC2_RGB8_sRGB}
     * @m_since{2019,10}
     */
    Etc2RGB8Srgb,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed RGB, normalized unsigned byte + a single-bit alpha.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGB8PunchthroughAlpha1Etc2,
     * @ref GL::TextureFormat::CompressedRGB8PunchthroughAlpha1Etc2;
     * @ref Vk::PixelFormat::CompressedEtc2RGB8A1Unorm or
     * @m_class{m-doc-external} [MTLPixelFormatETC2_RGB8A1](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatetc2_rgb8a1?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatETC2_RGB8A1}
     * @m_since{2019,10}
     */
    Etc2RGB8A1Unorm,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed sRGB, normalized unsigned byte + a single-bit alpha.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8PunchthroughAlpha1Etc2,
     * @ref GL::TextureFormat::CompressedSRGB8PunchthroughAlpha1Etc2;
     * @ref Vk::PixelFormat::CompressedEtc2RGB8A1Srgb or
     * @m_class{m-doc-external} [MTLPixelFormatETC2_RGB8A1_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatetc2_rgb8a1_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatETC2_RGB8A1_sRGB}
     * @m_since{2019,10}
     */
    Etc2RGB8A1Srgb,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed RGBA, normalized unsigned byte (EAC).
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBA8Etc2Eac,
     * @ref GL::TextureFormat::CompressedRGBA8Etc2Eac;
     * @ref Vk::PixelFormat::CompressedEtc2RGBA8Unorm or
     * @m_class{m-doc-external} [MTLPixelFormatEAC_RGBA8](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformateac_rgba8?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatEAC_RGBA8}
     * @m_since{2019,10}
     */
    Etc2RGBA8Unorm,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed sRGB + linear alpha, normalized unsigned byte (EAC).
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Etc2Eac,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Etc2Eac;
     * @ref Vk::PixelFormat::CompressedEtc2RGBA8Srgb or
     * @m_class{m-doc-external} [MTLPixelFormatEAC_RGBA8_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformateac_rgba8_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatEAC_RGBA8_sRGB}
     * @m_since{2019,10}
     */
    Etc2RGBA8Srgb,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 4x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc4x4,
     * @ref GL::TextureFormat::CompressedRGBAAstc4x4;
     * @ref Vk::PixelFormat::CompressedAstc4x4RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_4x4_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_4x4_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_4x4_LDR}
     * @m_since{2019,10}
     */
    Astc4x4RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 4x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc4x4,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc4x4;
     * @ref Vk::PixelFormat::CompressedAstc4x4RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_4x4_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_4x4_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_4x4_sRGB}
     * @m_since{2019,10}
     */
    Astc4x4RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 4x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc4x4,
     * @ref GL::TextureFormat::CompressedRGBAAstc4x4;
     * @ref Vk::PixelFormat::CompressedAstc4x4RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_4x4_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_4x4_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_4x4_HDR}
     * @m_since{2019,10}
     */
    Astc4x4RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 5x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc5x4,
     * @ref GL::TextureFormat::CompressedRGBAAstc5x4;
     * @ref Vk::PixelFormat::CompressedAstc5x4RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_5x4_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_5x4_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_5x4_LDR}
     * @m_since{2019,10}
     */
    Astc5x4RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 5x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc5x4,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc5x4;
     * @ref Vk::PixelFormat::CompressedAstc5x4RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_5x4_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_5x4_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_5x4_sRGB}
     * @m_since{2019,10}
     */
    Astc5x4RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 5x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc5x4,
     * @ref GL::TextureFormat::CompressedRGBAAstc5x4;
     * @ref Vk::PixelFormat::CompressedAstc5x4RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_5x4_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_5x4_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_5x4_HDR}
     * @m_since{2019,10}
     */
    Astc5x4RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 5x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc5x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc5x5;
     * @ref Vk::PixelFormat::CompressedAstc5x5RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_5x5_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_5x5_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_5x5_LDR}
     * @m_since{2019,10}
     */
    Astc5x5RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 5x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc5x5,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc5x5;
     * @ref Vk::PixelFormat::CompressedAstc5x5RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_5x5_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_5x5_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_5x5_sRGB}
     * @m_since{2019,10}
     */
    Astc5x5RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 5x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc5x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc5x5;
     * @ref Vk::PixelFormat::CompressedAstc5x5RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_5x5_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_5x5_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_5x5_HDR}
     * @m_since{2019,10}
     */
    Astc5x5RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 6x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc6x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc6x5;
     * @ref Vk::PixelFormat::CompressedAstc6x5RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_6x5_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_6x5_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_6x5_LDR}
     * @m_since{2019,10}
     */
    Astc6x5RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 6x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc6x5,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc6x5;
     * @ref Vk::PixelFormat::CompressedAstc6x5RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_6x5_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_6x5_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_6x5_sRGB}
     * @m_since{2019,10}
     */
    Astc6x5RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 6x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc6x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc6x5;
     * @ref Vk::PixelFormat::CompressedAstc6x5RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_6x5_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_6x5_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_6x5_HDR}
     * @m_since{2019,10}
     */
    Astc6x5RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 6x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc6x6,
     * @ref GL::TextureFormat::CompressedRGBAAstc6x6;
     * @ref Vk::PixelFormat::CompressedAstc6x6RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_6x6_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_6x6_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_6x6_LDR}
     * @m_since{2019,10}
     */
    Astc6x6RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 6x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc6x6,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc6x6;
     * @ref Vk::PixelFormat::CompressedAstc6x6RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_6x6_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_6x6_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_6x6_sRGB}
     * @m_since{2019,10}
     */
    Astc6x6RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 6x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc6x6,
     * @ref GL::TextureFormat::CompressedRGBAAstc6x6;
     * @ref Vk::PixelFormat::CompressedAstc6x6RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_6x6_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_6x6_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_6x6_HDR}
     * @m_since{2019,10}
     */
    Astc6x6RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 8x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc8x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc8x5;
     * @ref Vk::PixelFormat::CompressedAstc8x5RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_8x5_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_8x5_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_8x5_LDR}
     * @m_since{2019,10}
     */
    Astc8x5RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 8x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc8x5,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc8x5;
     * @ref Vk::PixelFormat::CompressedAstc8x5RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_8x5_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_8x5_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_8x5_sRGB}
     * @m_since{2019,10}
     */
    Astc8x5RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 8x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc8x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc8x5;
     * @ref Vk::PixelFormat::CompressedAstc8x5RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_8x5_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_8x5_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_8x5_HDR}
     * @m_since{2019,10}
     */
    Astc8x5RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 8x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc8x6,
     * @ref GL::TextureFormat::CompressedRGBAAstc8x6;
     * @ref Vk::PixelFormat::CompressedAstc8x6RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_8x6_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_8x6_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_8x6_LDR}
     * @m_since{2019,10}
     */
    Astc8x6RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 8x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc8x6,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc8x6;
     * @ref Vk::PixelFormat::CompressedAstc8x6RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_8x6_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_8x6_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_8x6_sRGB}
     * @m_since{2019,10}
     */
    Astc8x6RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 8x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc8x6,
     * @ref GL::TextureFormat::CompressedRGBAAstc8x6;
     * @ref Vk::PixelFormat::CompressedAstc8x6RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_8x6_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_8x6_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_8x6_HDR}
     * @m_since{2019,10}
     */
    Astc8x6RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 8x8 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc8x8,
     * @ref GL::TextureFormat::CompressedRGBAAstc8x8;
     * @ref Vk::PixelFormat::CompressedAstc8x8RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_8x8_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_8x8_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_8x8_LDR}
     * @m_since{2019,10}
     */
    Astc8x8RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 8x8 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc8x8,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc8x8;
     * @ref Vk::PixelFormat::CompressedAstc8x8RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_8x8_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_8x8_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_8x8_sRGB}
     * @m_since{2019,10}
     */
    Astc8x8RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 8x8 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc8x8,
     * @ref GL::TextureFormat::CompressedRGBAAstc8x8;
     * @ref Vk::PixelFormat::CompressedAstc8x8RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_8x8_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_8x8_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_8x8_HDR}
     * @m_since{2019,10}
     */
    Astc8x8RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 10x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc10x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc10x5;
     * @ref Vk::PixelFormat::CompressedAstc10x5RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x5_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x5_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x5_LDR}
     * @m_since{2019,10}
     */
    Astc10x5RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 10x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc10x5,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc10x5;
     * @ref Vk::PixelFormat::CompressedAstc10x5RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x5_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x5_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x5_sRGB}
     * @m_since{2019,10}
     */
    Astc10x5RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 10x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc10x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc10x5;
     * @ref Vk::PixelFormat::CompressedAstc10x5RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x5_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x5_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x5_HDR}
     * @m_since{2019,10}
     */
    Astc10x5RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 10x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc10x6,
     * @ref GL::TextureFormat::CompressedRGBAAstc10x6;
     * @ref Vk::PixelFormat::CompressedAstc10x6RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x6_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x6_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x6_LDR}
     * @m_since{2019,10}
     */
    Astc10x6RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 10x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc10x6,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc10x6;
     * @ref Vk::PixelFormat::CompressedAstc10x6RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x6_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x6_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x6_sRGB}
     * @m_since{2019,10}
     */
    Astc10x6RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 10x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc10x6,
     * @ref GL::TextureFormat::CompressedRGBAAstc10x6;
     * @ref Vk::PixelFormat::CompressedAstc10x6RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x6_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x6_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x6_HDR}
     * @m_since{2019,10}
     */
    Astc10x6RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 10x8 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc10x8,
     * @ref GL::TextureFormat::CompressedRGBAAstc10x8;
     * @ref Vk::PixelFormat::CompressedAstc10x8RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x8_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x8_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x8_LDR}
     * @m_since{2019,10}
     */
    Astc10x8RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 10x8 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc10x8,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc10x8;
     * @ref Vk::PixelFormat::CompressedAstc10x8RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x8_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x8_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x8_sRGB}
     * @m_since{2019,10}
     */
    Astc10x8RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 10x8 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc10x8,
     * @ref GL::TextureFormat::CompressedRGBAAstc10x8;
     * @ref Vk::PixelFormat::CompressedAstc10x8RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x8_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x8_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x8_HDR}
     * @m_since{2019,10}
     */
    Astc10x8RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 10x10 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc10x10,
     * @ref GL::TextureFormat::CompressedRGBAAstc10x10;
     * @ref Vk::PixelFormat::CompressedAstc10x10RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x10_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x10_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x10_LDR}
     * @m_since{2019,10}
     */
    Astc10x10RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 10x10 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc10x10,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc10x10;
     * @ref Vk::PixelFormat::CompressedAstc10x10RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x10_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x10_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x10_sRGB}
     * @m_since{2019,10}
     */
    Astc10x10RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 10x10 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc10x10,
     * @ref GL::TextureFormat::CompressedRGBAAstc10x10;
     * @ref Vk::PixelFormat::CompressedAstc10x10RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_10x10_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_10x10_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_10x10_HDR}
     * @m_since{2019,10}
     */
    Astc10x10RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 12x10 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc12x10,
     * @ref GL::TextureFormat::CompressedRGBAAstc12x10;
     * @ref Vk::PixelFormat::CompressedAstc12x10RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_12x10_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_12x10_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_12x10_LDR}
     * @m_since{2019,10}
     */
    Astc12x10RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 12x10 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc12x10,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc12x10;
     * @ref Vk::PixelFormat::CompressedAstc12x10RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_12x10_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_12x10_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_12x10_sRGB}
     * @m_since{2019,10}
     */
    Astc12x10RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 12x10 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc12x10,
     * @ref GL::TextureFormat::CompressedRGBAAstc12x10;
     * @ref Vk::PixelFormat::CompressedAstc12x10RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_12x10_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_12x10_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_12x10_HDR}
     * @m_since{2019,10}
     */
    Astc12x10RGBAF,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 12x12 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc12x12,
     * @ref GL::TextureFormat::CompressedRGBAAstc12x12;
     * @ref Vk::PixelFormat::CompressedAstc12x12RGBAUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_12x12_LDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_12x12_ldr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_12x12_LDR}
     * @m_since{2019,10}
     */
    Astc12x12RGBAUnorm,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 12x12 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc12x12,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc12x12;
     * @ref Vk::PixelFormat::CompressedAstc12x12RGBASrgb or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_12x12_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_12x12_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_12x12_sRGB}
     * @m_since{2019,10}
     */
    Astc12x12RGBASrgb,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 12x12 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc12x12,
     * @ref GL::TextureFormat::CompressedRGBAAstc12x12;
     * @ref Vk::PixelFormat::CompressedAstc12x12RGBAF or
     * @m_class{m-doc-external} [MTLPixelFormatASTC_12x12_HDR](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatastc_12x12_hdr?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatASTC_12x12_HDR}
     * @m_since{2019,10}
     */
    Astc12x12RGBAF,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 3x3x3 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc3x3x3,
     * @ref GL::TextureFormat::CompressedRGBAAstc3x3x3;
     * and @val_vk_keyword{FORMAT_ASTC_3x3x3_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc3x3x3RGBAUnorm,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 3x3x3 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc3x3x3,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc3x3x3;
     * and @val_vk_keyword{FORMAT_ASTC_3x3x3_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc3x3x3RGBASrgb,

    /**
     * 3D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 3x3x3 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc3x3x3,
     * @ref GL::TextureFormat::CompressedRGBAAstc3x3x3;
     * and @val_vk_keyword{FORMAT_ASTC_3x3x3_SFLOAT_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc3x3x3RGBAF,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 4x3x3 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc4x3x3,
     * @ref GL::TextureFormat::CompressedRGBAAstc4x3x3;
     * and @val_vk_keyword{FORMAT_ASTC_4x3x3_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc4x3x3RGBAUnorm,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 4x3x3 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc4x3x3,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc4x3x3;
     * and @val_vk_keyword{FORMAT_ASTC_4x3x3_SRGB_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc4x3x3RGBASrgb,

    /**
     * 3D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 4x3x3 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc4x3x3,
     * @ref GL::TextureFormat::CompressedRGBAAstc4x3x3;
     * and @val_vk_keyword{FORMAT_ASTC_4x3x3_SFLOAT_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc4x3x3RGBAF,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 4x4x3 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc4x4x3,
     * @ref GL::TextureFormat::CompressedRGBAAstc4x4x3;
     * and @val_vk_keyword{FORMAT_ASTC_4x4x3_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc4x4x3RGBAUnorm,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 4x4x3 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc4x4x3,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc4x4x3;
     * and @val_vk_keyword{FORMAT_ASTC_4x4x3_SRGB_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc4x4x3RGBASrgb,

    /**
     * 3D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 4x4x3 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc4x4x3,
     * @ref GL::TextureFormat::CompressedRGBAAstc4x4x3;
     * and @val_vk_keyword{FORMAT_ASTC_4x4x3_SFLOAT_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc4x4x3RGBAF,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 4x4x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc4x4x4,
     * @ref GL::TextureFormat::CompressedRGBAAstc4x4x4;
     * and @val_vk_keyword{FORMAT_ASTC_4x4x4_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc4x4x4RGBAUnorm,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 4x4x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc4x4x4,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc4x4x4;
     * and @val_vk_keyword{FORMAT_ASTC_4x4x4_SRGB_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc4x4x4RGBASrgb,

    /**
     * 3D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 4x4x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc4x4x4,
     * @ref GL::TextureFormat::CompressedRGBAAstc4x4x4;
     * and @val_vk_keyword{FORMAT_ASTC_4x4x4_SFLOAT_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc4x4x4RGBAF,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 5x4x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc5x4x4,
     * @ref GL::TextureFormat::CompressedRGBAAstc5x4x4;
     * and @val_vk_keyword{FORMAT_ASTC_5x4x4_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc5x4x4RGBAUnorm,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 5x4x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc5x4x4,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc5x4x4;
     * and @val_vk_keyword{FORMAT_ASTC_5x4x4_SRGB_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc5x4x4RGBASrgb,

    /**
     * 3D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 5x4x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc5x4x4,
     * @ref GL::TextureFormat::CompressedRGBAAstc5x4x4;
     * and @val_vk_keyword{FORMAT_ASTC_5x4x4_SFLOAT_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc5x4x4RGBAF,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 5x5x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc5x5x4,
     * @ref GL::TextureFormat::CompressedRGBAAstc5x5x4;
     * and @val_vk_keyword{FORMAT_ASTC_5x5x4_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc5x5x4RGBAUnorm,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 5x5x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc5x5x4,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc5x5x4;
     * and @val_vk_keyword{FORMAT_ASTC_5x5x4_SRGB_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc5x5x4RGBASrgb,

    /**
     * 3D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 5x5x4 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc5x5x4,
     * @ref GL::TextureFormat::CompressedRGBAAstc5x5x4;
     * and @val_vk_keyword{FORMAT_ASTC_5x5x4_SFLOAT_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc5x5x4RGBAF,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 5x5x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc5x5x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc5x5x5;
     * and @val_vk_keyword{FORMAT_ASTC_5x5x5_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc5x5x5RGBAUnorm,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 5x5x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc5x5x5,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc5x5x5;
     * and @val_vk_keyword{FORMAT_ASTC_5x5x5_SRGB_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc5x5x5RGBASrgb,

    /**
     * 3D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 5x5x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc5x5x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc5x5x5;
     * and @val_vk_keyword{FORMAT_ASTC_5x5x5_SFLOAT_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc5x5x5RGBAF,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 6x5x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc6x5x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc6x5x5;
     * and @val_vk_keyword{FORMAT_ASTC_6x5x5_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc6x5x5RGBAUnorm,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 6x5x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc6x5x5,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc6x5x5;
     * and @val_vk_keyword{FORMAT_ASTC_6x5x5_SRGB_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc6x5x5RGBASrgb,

    /**
     * 3D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 6x5x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc6x5x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc6x5x5;
     * and @val_vk_keyword{FORMAT_ASTC_6x5x5_SFLOAT_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc6x5x5RGBAF,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 6x6x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc6x6x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc6x6x5;
     * and @val_vk_keyword{FORMAT_ASTC_6x6x5_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc6x6x5RGBAUnorm,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 6x6x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc6x6x5,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc6x6x5;
     * and @val_vk_keyword{FORMAT_ASTC_6x6x5_SRGB_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc6x6x5RGBASrgb,

    /**
     * 3D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 6x6x5 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc6x6x5,
     * @ref GL::TextureFormat::CompressedRGBAAstc6x6x5;
     * and @val_vk_keyword{FORMAT_ASTC_6x6x5_SFLOAT_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc6x6x5RGBAF,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 6x6x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc6x6x6,
     * @ref GL::TextureFormat::CompressedRGBAAstc6x6x6;
     * and @val_vk_keyword{FORMAT_ASTC_6x6x6_UNORM_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc6x6x6RGBAUnorm,

    /**
     * 3D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 6x6x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc6x6x6,
     * @ref GL::TextureFormat::CompressedSRGB8Alpha8Astc6x6x6;
     * and @val_vk_keyword{FORMAT_ASTC_6x6x6_SRGB_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc6x6x6RGBASrgb,

    /**
     * 3D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 6x6x6 blocks.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAAstc6x6x6,
     * @ref GL::TextureFormat::CompressedRGBAAstc6x6x6;
     * and @val_vk_keyword{FORMAT_ASTC_6x6x6_SFLOAT_BLOCK_EXT,Format} (not a
     * part of the spec yet, [only defined in KTX](https://github.com/KhronosGroup/KTX-Specification/pull/97/files)).
     * No equivalent in Metal or D3D.
     * @m_since{2019,10}
     */
    Astc6x6x6RGBAF,

    /* See https://github.com/KhronosGroup/Vulkan-Docs/issues/512#issuecomment-307768667
       for Vulkan mapping. Ugh. */

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed RGB, normalized
     * unsigned byte with 2 bits per pixel.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBPvrtc2bppV1,
     * @ref GL::TextureFormat::CompressedRGBPvrtc2bppV1;
     * @ref Vk::PixelFormat::CompressedPvrtcRGBA2bppUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatPVRTC_RGB_2BPP](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatpvrtc_rgb_2bpp?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatPVRTC_RGB_2BPP}
     * @m_since{2019,10}
     */
    PvrtcRGB2bppUnorm,

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed sRGB, normalized
     * unsigned byte with 2 bits per pixel.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGBPvrtc2bppV1,
     * @ref GL::TextureFormat::CompressedSRGBPvrtc2bppV1;
     * @ref Vk::PixelFormat::CompressedPvrtcRGBA2bppSrgb or
     * @m_class{m-doc-external} [MTLPixelFormatPVRTC_RGB_2BPP_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatpvrtc_rgb_2bpp_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatPVRTC_RGB_2BPP_sRGB}
     * @m_since{2019,10}
     */
    PvrtcRGB2bppSrgb,

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed RGBA, normalized
     * unsigned byte with 2 bits per pixel.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAPvrtc2bppV1,
     * @ref GL::TextureFormat::CompressedRGBAPvrtc2bppV1;
     * @ref Vk::PixelFormat::CompressedPvrtcRGBA2bppUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatPVRTC_RGBA_2BPP](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatpvrtc_rgba_2bpp?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatPVRTC_RGBA_2BPP}
     * @m_since{2019,10}
     */
    PvrtcRGBA2bppUnorm,

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed sRGB + linear
     * alpha, normalized unsigned byte with 2 bits per pixel.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGBAlphaPvrtc2bppV1,
     * @ref GL::TextureFormat::CompressedSRGBAlphaPvrtc2bppV1;
     * @ref Vk::PixelFormat::CompressedPvrtcRGBA2bppSrgb or
     * @m_class{m-doc-external} [MTLPixelFormatPVRTC_RGBA_2BPP_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatpvrtc_rgba_2bpp_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatPVRTC_RGBA_2BPP_sRGB}
     * @m_since{2019,10}
     */
    PvrtcRGBA2bppSrgb,

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed RGB, normalized
     * unsigned byte with 4 bits per pixel.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBPvrtc4bppV1,
     * @ref GL::TextureFormat::CompressedRGBPvrtc4bppV1;
     * @ref Vk::PixelFormat::CompressedPvrtcRGBA4bppUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatPVRTC_RGB_4BPP](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatpvrtc_rgb_4bpp?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatPVRTC_RGB_4BPP}
     * @m_since{2019,10}
     */
    PvrtcRGB4bppUnorm,

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed sRGB, normalized
     * unsigned byte with 4 bits per pixel.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGBPvrtc4bppV1,
     * @ref GL::TextureFormat::CompressedSRGBPvrtc4bppV1;
     * @ref Vk::PixelFormat::CompressedPvrtcRGBA4bppSrgb or
     * @m_class{m-doc-external} [MTLPixelFormatPVRTC_RGB_4BPP_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatpvrtc_rgb_4bpp_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatPVRTC_RGB_4BPP_sRGB}
     * @m_since{2019,10}
     */
    PvrtcRGB4bppSrgb,

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed RGBA, normalized
     * unsigned byte with 4 bits per pixel.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAPvrtc4bppV1,
     * @ref GL::TextureFormat::CompressedRGBAPvrtc4bppV1;
     * @ref Vk::PixelFormat::CompressedPvrtcRGBA4bppUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatPVRTC_RGBA_4BPP](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatpvrtc_rgba_4bpp?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatPVRTC_RGBA_4BPP}
     * @m_since{2019,10}
     */
    PvrtcRGBA4bppUnorm,

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed sRGB + linear
     * alpha, normalized unsigned byte with 4 bits per pixel.
     *
     * Corresponds to @ref GL::CompressedPixelFormat::SRGBAlphaPvrtc4bppV1,
     * @ref GL::TextureFormat::CompressedSRGBAlphaPvrtc4bppV1;
     * @ref Vk::PixelFormat::CompressedPvrtcRGBA4bppUnorm or
     * @m_class{m-doc-external} [MTLPixelFormatPVRTC_RGBA_4BPP_sRGB](https://developer.apple.com/documentation/metal/mtlpixelformat/mtlpixelformatpvrtc_rgba_4bpp_srgb?language=objc).
     * No equivalent in D3D.
     * @m_keywords{MTLPixelFormatPVRTC_RGBA_4BPP_sRGB}
     * @m_since{2019,10}
     */
    PvrtcRGBA4bppSrgb

    /* PVRTC2 variants not listed as PVRTC is mainly on Apple hardware but
       Metal doesn't support it and it doesn't have a WebGL equiv either. */
};

/**
@brief Compressed block size
@m_since{2019,10}

For 2D formats the Z dimension is always 1. Expects that the pixel format is
* *not* implementation-specific.
@see @ref compressedBlockDataSize(),
    @ref isCompressedPixelFormatImplementationSpecific()
*/
MAGNUM_EXPORT Vector3i compressedBlockSize(CompressedPixelFormat format);

/**
@brief Compressed block data size
@m_since{2019,10}

Byte size of each compressed block. Expects that the pixel format is *not*
implementation-specific.
@see @ref compressedBlockSize(),
    @ref isCompressedPixelFormatImplementationSpecific()
*/
MAGNUM_EXPORT UnsignedInt compressedBlockDataSize(CompressedPixelFormat format);

/** @debugoperatorenum{CompressedPixelFormat} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, CompressedPixelFormat value);

/**
@brief Whether a @ref CompressedPixelFormat value wraps an implementation-specific identifier

Returns @cpp true @ce if value of @p format has its highest bit set, @cpp false @ce
otherwise. Use @ref compressedPixelFormatWrap() and @ref compressedPixelFormatUnwrap()
to wrap/unwrap an implementation-specific indentifier to/from @ref CompressedPixelFormat.
@see @ref isPixelFormatImplementationSpecific()
*/
constexpr bool isCompressedPixelFormatImplementationSpecific(CompressedPixelFormat format) {
    return UnsignedInt(format) & (1u << 31);
}

/**
@brief Wrap an implementation-specific pixel format identifier in a @ref CompressedPixelFormat

Sets the highest bit on @p format to mark it as implementation-specific.
Expects that @p format fits into the remaining bits. Use @ref compressedPixelFormatUnwrap()
for the inverse operation.
@see @ref isCompressedPixelFormatImplementationSpecific(), @ref pixelFormatWrap()
*/
template<class T> constexpr CompressedPixelFormat compressedPixelFormatWrap(T implementationSpecific) {
    static_assert(sizeof(T) <= 4,
        "format types larger than 32bits are not supported");
    return CORRADE_CONSTEXPR_ASSERT(!(UnsignedInt(implementationSpecific) & (1u << 31)),
        "compressedPixelFormatWrap(): implementation-specific value" << reinterpret_cast<void*>(implementationSpecific) << "already wrapped or too large"),
        CompressedPixelFormat((1u << 31)|UnsignedInt(implementationSpecific));
}

/**
@brief Unwrap an implementation-specific pixel format identifier from a @ref CompressedPixelFormat

Unsets the highest bit from @p format to extract the implementation-specific
value. Expects that @p format has it set. Use @ref compressedPixelFormatWrap() for the
inverse operation.
@see @ref isCompressedPixelFormatImplementationSpecific(), @ref pixelFormatUnwrap()
*/
template<class T = UnsignedInt> constexpr T compressedPixelFormatUnwrap(CompressedPixelFormat format) {
    return CORRADE_CONSTEXPR_ASSERT(UnsignedInt(format) & (1u << 31),
        "compressedPixelFormatUnwrap():" << format << "isn't a wrapped implementation-specific value"),
        T(UnsignedInt(format) & ~(1u << 31));
}

}

namespace Corrade { namespace Utility {

/**
@configurationvalue{Magnum::PixelFormat}
@m_since{2020,06}
*/
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::PixelFormat> {
    ConfigurationValue() = delete;

    /**
     * @brief Writes enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::PixelFormat value, ConfigurationValueFlags);

    /**
     * @brief Reads enum value as string
     *
     * If the value is invalid, returns a zero (invalid) format.
     */
    static Magnum::PixelFormat fromString(const std::string& stringValue, ConfigurationValueFlags);
};

/**
@configurationvalue{Magnum::CompressedPixelFormat}
@m_since{2020,06}
*/
template<> struct MAGNUM_EXPORT ConfigurationValue<Magnum::CompressedPixelFormat> {
    ConfigurationValue() = delete;

    /**
     * @brief Write enum value as string
     *
     * If the value is invalid, returns empty string.
     */
    static std::string toString(Magnum::CompressedPixelFormat value, ConfigurationValueFlags);

    /**
     * @brief Read enum value as string
     *
     * If the value is invalid, returns a zero (invalid) format.
     */
    static Magnum::CompressedPixelFormat fromString(const std::string& stringValue, ConfigurationValueFlags);
};

}}

#endif
