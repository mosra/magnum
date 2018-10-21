#ifndef Magnum_PixelFormat_h
#define Magnum_PixelFormat_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
 * @brief Enum @ref Magnum::PixelFormat, @ref Magnum::CompressedPixelFormat, function @ref Magnum::pixelSize(), @ref Magnum::isPixelFormatImplementationSpecific(), @ref Magnum::pixelFormatWrap(), @ref Magnum::pixelFormatUnwrap(), @ref Magnum::isCompressedPixelFormatImplementationSpecific(), @ref Magnum::compressedPixelFormatWrap(), @ref Magnum::compressedPixelFormatUnwrap()
 */

#include <Corrade/Utility/Assert.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
#include "Magnum/GL/PixelFormat.h"
#endif

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

In case of Vulkan, corresponds to @type_vk_keyword{Format} and is convertible
to it using @ref Vk::vkFormat(Magnum::PixelFormat). See documentation of each
value for more information about the mapping. Note that not every format may be
available, use @ref Vk::hasVkFormat(Magnum::PixelFormat) to check for its
presence.
@see @ref pixelSize(), @ref CompressedPixelFormat, @ref Image, @ref ImageView
*/
enum class PixelFormat: UnsignedInt {
    /**
     * Red component, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::R8 /
     * @def_vk_keyword{FORMAT_R8_UNORM,Format}.
     */
    R8Unorm,

    /**
     * Red and green component, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RG8 /
     * @def_vk_keyword{FORMAT_R8G8_UNORM,Format}.
     */
    RG8Unorm,

    /**
     * RGB, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RGB8 /
     * @def_vk_keyword{FORMAT_R8G8B8_UNORM,Format}.
     */
    RGB8Unorm,

    /**
     * RGBA, normalized unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RGBA8 /
     * @def_vk_keyword{FORMAT_R8G8B8A8_UNORM,Format}.
     */
    RGBA8Unorm,

    /**
     * Red component, normalized signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::R8Snorm /
     * @def_vk_keyword{FORMAT_R8_SNORM,Format}.
     */
    R8Snorm,

    /**
     * Red and green component, normalized signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RG8Snorm /
     * @def_vk_keyword{FORMAT_R8G8_SNORM,Format}.
     */
    RG8Snorm,

    /**
     * RGB, normalized signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RGB8Snorm /
     * @def_vk_keyword{FORMAT_R8G8B8_SNORM,Format}.
     */
    RGB8Snorm,

    /**
     * RGBA, normalized signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RGBA8Snorm /
     * @def_vk_keyword{FORMAT_R8G8B8A8_SNORM,Format}.
     */
    RGBA8Snorm,

    /**
     * Red component, integral unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::R8UI /
     * @def_vk_keyword{FORMAT_R8_UINT,Format}.
     */
    R8UI,

    /**
     * Red and green component, integral unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RG8UI /
     * @def_vk_keyword{FORMAT_R8G8_UINT,Format}.
     */
    RG8UI,

    /**
     * RGB, integral unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RGB8UI /
     * @def_vk_keyword{FORMAT_R8G8B8_UINT,Format}.
     */
    RGB8UI,

    /**
     * RGBA, integral unsigned byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::UnsignedByte, @ref GL::TextureFormat::RGBA8UI /
     * @def_vk_keyword{FORMAT_R8G8B8A8_UINT,Format}.
     */
    RGBA8UI,

    /**
     * Red component, integral signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::R8I /
     * @def_vk_keyword{FORMAT_R8_SINT,Format}.
     */
    R8I,

    /**
     * Red and green component, integral signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RG8I /
     * @def_vk_keyword{FORMAT_R8G8_SINT,Format}.
     */
    RG8I,

    /**
     * RGB, integral signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RGB8I /
     * @def_vk_keyword{FORMAT_R8G8B8_SINT,Format}.
     */
    RGB8I,

    /**
     * RGBA, integral signed byte.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::Byte, @ref GL::TextureFormat::RGBA8I /
     * @def_vk_keyword{FORMAT_R8G8B8A8_SINT,Format}.
     */
    RGBA8I,

    /**
     * Red component, normalized unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::R16 /
     * @def_vk_keyword{FORMAT_R16_UNORM,Format}.
     */
    R16Unorm,

    /**
     * Red and green component, normalized unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RG16 /
     * @def_vk_keyword{FORMAT_R16G16_UNORM,Format}.
     */
    RG16Unorm,

    /**
     * RGB, normalized unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RGB16 /
     * @def_vk_keyword{FORMAT_R16G16B16_UNORM,Format}.
     */
    RGB16Unorm,

    /**
     * RGBA, normalized unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RGBA16 /
     * @def_vk_keyword{FORMAT_R16G16B16A16_UNORM,Format}.
     */
    RGBA16Unorm,

    /**
     * Red component, normalized signed short.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::R16Snorm /
     * @def_vk_keyword{FORMAT_R16_SNORM,Format}.
     */
    R16Snorm,

    /**
     * Red and green component, normalized signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RG16Snorm /
     * @def_vk_keyword{FORMAT_R16G16_SNORM,Format}.
     */
    RG16Snorm,

    /**
     * RGB, normalized signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RGB16Snorm /
     * @def_vk_keyword{FORMAT_R16G16B16_SNORM,Format}.
     */
    RGB16Snorm,

    /**
     * RGBA, normalized signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RGBA16Snorm /
     * @def_vk_keyword{FORMAT_R16G16B16A16_SNORM,Format}.
     */
    RGBA16Snorm,

    /**
     * Red component, integral unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::R16UI /
     * @def_vk_keyword{FORMAT_R16_UINT,Format}.
     */
    R16UI,

    /**
     * Red and green component, integral unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RG16UI /
     * @def_vk_keyword{FORMAT_R16G16_UINT,Format}.
     */
    RG16UI,

    /**
     * RGB, integral unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RGB16UI /
     * @def_vk_keyword{FORMAT_R16G16B16_UINT,Format}.
     */
    RGB16UI,

    /**
     * RGBA, integral unsigned short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::UnsignedShort, @ref GL::TextureFormat::RGBA16UI /
     * @def_vk_keyword{FORMAT_R16G16B16A16_UINT,Format}.
     */
    RGBA16UI,

    /**
     * Red component, integral signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::R16I /
     * @def_vk_keyword{FORMAT_R16_SINT,Format}.
     */
    R16I,

    /**
     * Red and green component, integral signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RG16I /
     * @def_vk_keyword{FORMAT_R16G16_SINT,Format}.
     */
    RG16I,

    /**
     * RGB, integral signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RGB16I /
     * @def_vk_keyword{FORMAT_R16G16B16_SINT,Format}.
     */
    RGB16I,

    /**
     * RGBA, integral signed short.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::Short, @ref GL::TextureFormat::RGBA16I /
     * @def_vk_keyword{FORMAT_R16G16B16A16_SINT,Format}.
     */
    RGBA16I,

    /**
     * Red component, integral unsigned int.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::UnsignedInt, @ref GL::TextureFormat::R32UI /
     * @def_vk_keyword{FORMAT_R32_UINT,Format}.
     */
    R32UI,

    /**
     * Red and green component, integral unsigned int.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::UnsignedInt, @ref GL::TextureFormat::RG32UI /
     * @def_vk_keyword{FORMAT_R32G32_UINT,Format}.
     */
    RG32UI,

    /**
     * RGB, integral unsigned int.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::UnsignedInt, @ref GL::TextureFormat::RGB32UI /
     * @def_vk_keyword{FORMAT_R32G32B32_UINT,Format}.
     */
    RGB32UI,

    /**
     * RGBA, integral unsigned int.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::UnsignedInt, @ref GL::TextureFormat::RGBA32UI /
     * @def_vk_keyword{FORMAT_R32G32B32A32_UINT,Format}.
     */
    RGBA32UI,

    /**
     * Red component, integral signed int.
     *
     * Corresponds to @ref GL::PixelFormat::RedInteger and
     * @ref GL::PixelType::Int, @ref GL::TextureFormat::R32I /
     * @def_vk_keyword{FORMAT_R32_SINT,Format}.
     */
    R32I,

    /**
     * Red and green component, integral signed int.
     *
     * Corresponds to @ref GL::PixelFormat::RGInteger and
     * @ref GL::PixelType::Int, @ref GL::TextureFormat::RG32I /
     * @def_vk_keyword{FORMAT_R32G32_SINT,Format}.
     */
    RG32I,

    /**
     * RGB, integral signed int.
     *
     * Corresponds to @ref GL::PixelFormat::RGBInteger and
     * @ref GL::PixelType::Int, @ref GL::TextureFormat::RGB32I /
     * @def_vk_keyword{FORMAT_R32G32B32_SINT,Format}.
     */
    RGB32I,

    /**
     * RGBA, integral signed int.
     *
     * Corresponds to @ref GL::PixelFormat::RGBAInteger and
     * @ref GL::PixelType::Int, @ref GL::TextureFormat::RGBA32I /
     * @def_vk_keyword{FORMAT_R32G32B32A32_SINT,Format}.
     */
    RGBA32I,

    /**
     * Red component, half float.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::HalfFloat, @ref GL::TextureFormat::R16F /
     * @def_vk_keyword{FORMAT_R16_SFLOAT,Format}.
     * @see @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     */
    R16F,

    /**
     * Red and green component, half float.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::HalfFloat, @ref GL::TextureFormat::RG16F /
     * @def_vk_keyword{FORMAT_R16G16_SFLOAT,Format}.
     * @see @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     */
    RG16F,

    /**
     * RGB, half float.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::HalfFloat, @ref GL::TextureFormat::RGB16F /
     * @def_vk_keyword{FORMAT_R16G16B16_SFLOAT,Format}.
     * @see @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     */
    RGB16F,

    /**
     * RGBA, half float.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::HalfFloat, @ref GL::TextureFormat::RGBA16F /
     * @def_vk_keyword{FORMAT_R16G16B16A16_SFLOAT,Format}.
     * @see @ref Half, @ref Math::packHalf(), @ref Math::unpackHalf()
     */
    RGBA16F,

    /**
     * Red component, half float.
     *
     * Corresponds to @ref GL::PixelFormat::Red and
     * @ref GL::PixelType::Float, @ref GL::TextureFormat::R32F /
     * @def_vk_keyword{FORMAT_R32_SFLOAT,Format}.
     */
    R32F,

    /**
     * Red and green component, half float.
     *
     * Corresponds to @ref GL::PixelFormat::RG and
     * @ref GL::PixelType::Float, @ref GL::TextureFormat::RG32F /
     * @def_vk_keyword{FORMAT_R32G32_SFLOAT,Format}.
     */
    RG32F,

    /**
     * RGB, half float.
     *
     * Corresponds to @ref GL::PixelFormat::RGB and
     * @ref GL::PixelType::Float, @ref GL::TextureFormat::RGB32F /
     * @def_vk_keyword{FORMAT_R32G32B32_SFLOAT,Format}.
     */
    RGB32F,

    /**
     * RGBA, half float.
     *
     * Corresponds to @ref GL::PixelFormat::RGBA and
     * @ref GL::PixelType::Float, @ref GL::TextureFormat::RGBA32F /
     * @def_vk_keyword{FORMAT_R32G32B32A32_SFLOAT,Format}.
     */
    RGBA32F,

    #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Floating-point red channel.
     * @deprecated Use @ref GL::PixelFormat::Red or any of the complete
     *      single-channel formats instead.
     */
    Red CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::Red or any of the complete single-channel formats instead") = UnsignedInt(GL::PixelFormat::Red),
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Floating-point green channel.
     * @deprecated Use @ref GL::PixelFormat::Green or any of the complete
     *      single-channel formats instead.
     */
    Green CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::Green or any of the complete single-channel formats instead") = UnsignedInt(GL::PixelFormat::Green),

    /**
     * Floating-point blue channel.
     * @deprecated Use @ref GL::PixelFormat::Blue or any of the complete
     *      single-channel formats instead.
     */
    Blue CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::Blue or any of the complete single-channel formats instead") = UnsignedInt(GL::PixelFormat::Blue),
    #endif

    #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * Floating-point luminance channel. The value is used for all RGB
     * channels.
     * @deprecated Use @ref GL::PixelFormat::Luminance or any of the complete
     *      single-channel formats instead.
     */
    Luminance CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::Luminance or any of the complete single-channel formats instead") = UnsignedInt(GL::PixelFormat::Luminance),
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /**
     * Floating-point red and green channel.
     * @deprecated Use @ref GL::PixelFormat::RG or any of the complete
     *      two-channel formats instead.
     */
    RG CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::RG or any of the complete two-channel formats instead") = UnsignedInt(GL::PixelFormat::RG),
    #endif

    #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * Floating-point luminance and alpha channel. First value is used for all
     * RGB channels, second value is used for alpha channel.
     * @deprecated Use @ref GL::PixelFormat::LuminanceAlpha or any of the
     *      complete two-channel formats instead.
     */
    LuminanceAlpha CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::LuminanceAlpha or any of the complete two-channel formats instead") = UnsignedInt(GL::PixelFormat::LuminanceAlpha),
    #endif

    /**
     * Floating-point RGB.
     * @deprecated Use @ref GL::PixelFormat::RGB or any of the complete
     *      three-channel formats instead.
     */
    RGB CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::RGB or any of the complete three-channel formats instead") = UnsignedInt(GL::PixelFormat::RGB),

    /**
     * Floating-point RGBA.
     * @deprecated Use @ref GL::PixelFormat::RGBA or any of the complete
     *      four-channel formats instead.
     */
    RGBA CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::RGBA or any of the complete four-channel formats instead") = UnsignedInt(GL::PixelFormat::RGBA),

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Floating-point BGR.
     * @deprecated Use @ref GL::PixelFormat::BGR or any of the complete
     *      three-channel formats instead.
     */
    BGR CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::BGR or any of the complete three-channel formats instead") = UnsignedInt(GL::PixelFormat::BGR),
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Floating-point BGRA.
     * @deprecated Use @ref GL::PixelFormat::BGRA or any of the complete
     *      four-channel formats instead.
     */
    BGRA CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::BGRA or any of the complete four-channel formats instead") = UnsignedInt(GL::PixelFormat::BGRA),
    #endif

    #if defined(MAGNUM_TARGET_GLES2) || defined(DOXYGEN_GENERATING_OUTPUT)
    /**
     * Floating-point sRGB.
     * @deprecated Use @ref GL::PixelFormat::SRGB or any of the complete
     *      three-channel formats instead.
     */
    SRGB CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::SRGB or any of the complete three-channel formats instead") = UnsignedInt(GL::PixelFormat::SRGB),

    /**
     * Floating-point sRGB + alpha.
     * @deprecated Use @ref GL::PixelFormat::SRGBAlpha or any of the complete
     *      four-channel formats instead.
     */
    SRGBAlpha CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::SRGBAlpha or any of the complete four-channel formats instead") = UnsignedInt(GL::PixelFormat::SRGBAlpha),
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * Integer red channel.
     * @deprecated Use @ref GL::PixelFormat::RedInteger or any of the complete
     *      single-channel formats instead.
     */
    RedInteger CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::RedInteger or any of the complete single-channel formats instead") = UnsignedInt(GL::PixelFormat::RedInteger),

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Integer green channel.
     * @deprecated Use @ref GL::PixelFormat::GreenInteger or any of the
     *      complete single-channel formats instead.
     */
    GreenInteger CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::GreenInteger or any of the complete single-channel formats instead") = UnsignedInt(GL::PixelFormat::GreenInteger),

    /**
     * Integer blue channel.
     * @deprecated Use @ref GL::PixelFormat::BlueInteger or any of the complete
     *      single-channel formats instead.
     */
    BlueInteger CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::BlueInteger or any of the complete single-channel formats instead") = UnsignedInt(GL::PixelFormat::BlueInteger),
    #endif

    /**
     * Integer red and green channel.
     * @deprecated Use @ref GL::PixelFormat::RGInteger or any of the complete
     *      two-channel formats instead.
     */
    RGInteger CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::RGInteger or any of the complete two-channel formats instead") = UnsignedInt(GL::PixelFormat::RGInteger),

    /**
     * Integer RGB.
     * @deprecated Use @ref GL::PixelFormat::RGBInteger or any of the complete
     *      three-channel formats instead.
     */
    RGBInteger CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::RGBInteger or any of the complete three-channel formats instead") = UnsignedInt(GL::PixelFormat::RGBInteger),

    /**
     * Integer RGBA.
     * @deprecated Use @ref GL::PixelFormat::RGBAInteger or any of the complete
     *      four-channel formats instead.
     */
    RGBAInteger CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::RGBAInteger or any of the complete four-channel formats instead") = UnsignedInt(GL::PixelFormat::RGBAInteger),

    #ifndef MAGNUM_TARGET_GLES
    /**
     * Integer BGR.
     * @deprecated Use @ref GL::PixelFormat::BGRInteger or any of the complete
     *      three-channel formats instead.
     */
    BGRInteger CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::BGRInteger or any of the complete three-channel formats instead") = UnsignedInt(GL::PixelFormat::BGRInteger),

    /**
     * Integer BGRA.
     * @deprecated Use @ref GL::PixelFormat::BGRAInteger or any of the complete
     *      four-channel formats instead.
     */
    BGRAInteger CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::BGRAInteger or any of the complete four-channel formats instead") = UnsignedInt(GL::PixelFormat::BGRAInteger),
    #endif
    #endif

    /**
     * Depth component.
     * @deprecated Use @ref GL::PixelFormat::DepthComponent or any of the
     *      complete depth formats instead.
     */
    DepthComponent CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::DepthComponent or any of the complete depth formats instead") = UnsignedInt(GL::PixelFormat::DepthComponent),

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * Stencil index.
     * @deprecated Use @ref GL::PixelFormat::StencilIndex or any of the
     *      complete stencil formats instead.
     */
    StencilIndex CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::StencilIndex or any of the complete stencil formats instead") = UnsignedInt(GL::PixelFormat::StencilIndex),
    #endif

    /**
     * Depth and stencil.
     * @deprecated Use @ref GL::PixelFormat::DepthStencil or any of the
     *      complete depth+stencil formats instead.
     */
    DepthStencil CORRADE_DEPRECATED_ENUM("use GL::PixelFormat::DepthStencil or any of the complete depth+stencil formats instead") = UnsignedInt(GL::PixelFormat::DepthStencil)
    #endif
};

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
/** @brief @copybrief GL::PixelType
 * @deprecated Use @ref GL::PixelType instead.
 */
typedef CORRADE_DEPRECATED("use GL::PixelType instead") GL::PixelType PixelType;
#endif

/**
@brief Pixel size

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
        "pixelFormatWrap(): implementation-specific value already wrapped or too large"),
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
        "pixelFormatUnwrap(): format doesn't contain a wrapped implementation-specific value"),
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

In case of Vulkan, corresponds to @type_vk_keyword{Format} and is convertible
to it using @ref Vk::vkFormat(Magnum::CompressedPixelFormat). See documentation
of each value for more information about the mapping. Note that not every
format may be available, use @ref Vk::hasVkFormat(Magnum::CompressedPixelFormat)
to check for its presence.
@see @ref PixelFormat, @ref CompressedImage, @ref CompressedImageView
*/
enum class CompressedPixelFormat: UnsignedInt {
    /**
     * S3TC BC1 compressed RGB (DXT1).
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBS3tcDxt1,
     * @ref GL::TextureFormat::RGBS3tcDxt1 /
     * @def_vk_keyword{FORMAT_BC1_RGB_UNORM_BLOCK,Format}.
     */
    Bc1RGBUnorm,

    /**
     * S3TC BC1 compressed RGBA (DXT1).
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAS3tcDxt1,
     * @ref GL::TextureFormat::RGBAS3tcDxt1 /
     * @def_vk_keyword{FORMAT_BC1_RGBA_UNORM_BLOCK,Format}.
     */
    Bc1RGBAUnorm,

    /**
     * S3TC BC2 compressed RGBA (DXT3).
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAS3tcDxt3,
     * @ref GL::TextureFormat::RGBAS3tcDxt3 /
     * @def_vk_keyword{FORMAT_BC2_UNORM_BLOCK,Format}.
     */
    Bc2RGBAUnorm,

    /**
     * S3TC BC3 compressed RGBA (DXT5).
     *
     * Corresponds to @ref GL::CompressedPixelFormat::RGBAS3tcDxt5,
     * @ref GL::TextureFormat::RGBAS3tcDxt5 /
     * @def_vk_keyword{FORMAT_BC3_UNORM_BLOCK,Format}.
     */
    Bc3RGBAUnorm,

    #if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
    #ifndef MAGNUM_TARGET_GLES
    /**
     * Compressed red channel, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::Red instead.
     */
    Red CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::Red instead") = UnsignedInt(GL::CompressedPixelFormat::Red),

    /**
     * Compressed red and green channel, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::RG instead.
     */
    RG CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RG instead") = UnsignedInt(GL::CompressedPixelFormat::RG),

    /**
     * Compressed RGB, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGB instead.
     */
    RGB CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGB instead") = UnsignedInt(GL::CompressedPixelFormat::RGB),

    /**
     * Compressed RGBA, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBA instead.
     */
    RGBA CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBA instead") = UnsignedInt(GL::CompressedPixelFormat::RGBA),

    /**
     * RGTC compressed red channel, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::RedRgtc1 instead.
     */
    RedRgtc1 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RedRgtc1 instead") = UnsignedInt(GL::CompressedPixelFormat::RedRgtc1),

    /**
     * RGTC compressed red and green channel, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGRgtc2 instead.
     */
    RGRgtc2 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGRgtc2 instead") = UnsignedInt(GL::CompressedPixelFormat::RGRgtc2),

    /**
     * RGTC compressed red channel, normalized signed.
     * @deprecated Use @ref GL::CompressedPixelFormat::SignedRedRgtc1 instead.
     */
    SignedRedRgtc1 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SignedRedRgtc1 instead") = UnsignedInt(GL::CompressedPixelFormat::SignedRedRgtc1),

    /**
     * RGTC compressed red and green channel, normalized signed.
     * @deprecated Use @ref GL::CompressedPixelFormat::SignedRGRgtc2 instead.
     */
    SignedRGRgtc2 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SignedRGRgtc2 instead") = UnsignedInt(GL::CompressedPixelFormat::SignedRGRgtc2),

    /**
     * BPTC compressed RGB, unsigned float.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBBptcUnsignedFloat instead.
     */
    RGBBptcUnsignedFloat CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBBptcUnsignedFloat instead") = UnsignedInt(GL::CompressedPixelFormat::RGBBptcUnsignedFloat),

    /**
     * BPTC compressed RGB, signed float.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBBptcSignedFloat instead.
     */
    RGBBptcSignedFloat CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBBptcSignedFloat instead") = UnsignedInt(GL::CompressedPixelFormat::RGBBptcSignedFloat),

    /**
     * BPTC compressed RGBA, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBABptcUnorm instead.
     */
    RGBABptcUnorm CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBABptcUnorm instead") = UnsignedInt(GL::CompressedPixelFormat::RGBABptcUnorm),

    /**
     * BPTC compressed sRGBA, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGBAlphaBptcUnorm instead.
     */
    SRGBAlphaBptcUnorm CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGBAlphaBptcUnorm instead") = UnsignedInt(GL::CompressedPixelFormat::SRGBAlphaBptcUnorm),
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    /**
     * ETC2 compressed RGB, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGB8Etc2 instead.
     */
    RGB8Etc2 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGB8Etc2 instead") = UnsignedInt(GL::CompressedPixelFormat::RGB8Etc2),

    /**
     * ETC2 compressed sRGB, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Etc2 instead.
     */
    SRGB8Etc2 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Etc2 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Etc2),

    /**
     * ETC2 compressed RGB with punchthrough (single-bit) alpha, normalized
     * unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGB8PunchthroughAlpha1Etc2 instead.
     */
    RGB8PunchthroughAlpha1Etc2 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGB8PunchthroughAlpha1Etc2 instead") = UnsignedInt(GL::CompressedPixelFormat::RGB8PunchthroughAlpha1Etc2),

    /**
     * ETC2 compressed sRGB with punchthrough (single-bit) alpha, normalized
     * unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8PunchthroughAlpha1Etc2 instead.
     */
    SRGB8PunchthroughAlpha1Etc2 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8PunchthroughAlpha1Etc2 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8PunchthroughAlpha1Etc2),

    /**
     * ETC2/EAC compressed RGBA, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBA8Etc2Eac instead.
     */
    RGBA8Etc2Eac CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBA8Etc2Eac instead") = UnsignedInt(GL::CompressedPixelFormat::RGBA8Etc2Eac),

    /**
     * ETC2/EAC compressed sRGB with alpha, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Etc2Eac instead.
     */
    SRGB8Alpha8Etc2Eac CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Etc2Eac instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Etc2Eac),

    /**
     * EAC compressed red channel, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::R11Eac instead.
     */
    R11Eac CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::R11Eac instead") = UnsignedInt(GL::CompressedPixelFormat::R11Eac),

    /**
     * EAC compressed red channel, normalized signed.
     * @deprecated Use @ref GL::CompressedPixelFormat::SignedR11Eac instead.
     */
    SignedR11Eac CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SignedR11Eac instead") = UnsignedInt(GL::CompressedPixelFormat::SignedR11Eac),

    /**
     * EAC compressed red and green channel, normalized unsigned.
     * @deprecated Use @ref GL::CompressedPixelFormat::RG11Eac instead.
     */
    RG11Eac CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RG11Eac instead") = UnsignedInt(GL::CompressedPixelFormat::RG11Eac),

    /**
     * EAC compressed red and green channel, normalized signed.
     * @deprecated Use @ref GL::CompressedPixelFormat::SignedRG11Eac instead.
     */
    SignedRG11Eac CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SignedRG11Eac instead") = UnsignedInt(GL::CompressedPixelFormat::SignedRG11Eac),
    #endif

    /**
     * S3TC DXT1 compressed RGB.
     * @deprecated Use @ref CompressedPixelFormat::Bc1RGBUnorm or
     *      @ref GL::CompressedPixelFormat::RGBS3tcDxt1 instead.
     */
    RGBS3tcDxt1 CORRADE_DEPRECATED_ENUM("use CompressedPixelFormat::Bc1RGBUnorm or GL::CompressedPixelFormat::RGBS3tcDxt1 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBS3tcDxt1),

    /**
     * S3TC DXT1 compressed RGBA.
     * @deprecated Use @ref CompressedPixelFormat::Bc1RGBAUnorm or
     *      @ref GL::CompressedPixelFormat::RGBAS3tcDxt1 instead.
     */
    RGBAS3tcDxt1 CORRADE_DEPRECATED_ENUM("use CompressedPixelFormat::Bc1RGBAUnorm or GL::CompressedPixelFormat::RGBAS3tcDxt1 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAS3tcDxt1),

    /**
     * S3TC DXT3 compressed RGBA.
     * @deprecated Use @ref CompressedPixelFormat::Bc2RGBAUnorm or
     *      @ref GL::CompressedPixelFormat::RGBAS3tcDxt3 instead.
     */
    RGBAS3tcDxt3 CORRADE_DEPRECATED_ENUM("use CompressedPixelFormat::Bc2RGBAUnorm or GL::CompressedPixelFormat::RGBAS3tcDxt3 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAS3tcDxt3),

    /**
     * S3TC DXT5 compressed RGBA.
     * @deprecated Use @ref CompressedPixelFormat::Bc3RGBAUnorm or
     *      @ref GL::CompressedPixelFormat::RGBAS3tcDxt5 instead.
     */
    RGBAS3tcDxt5 CORRADE_DEPRECATED_ENUM("use CompressedPixelFormat::Bc3RGBAUnorm or GL::CompressedPixelFormat::RGBAS3tcDxt5 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAS3tcDxt5),

    #ifndef MAGNUM_TARGET_WEBGL
    /**
     * ASTC compressed RGBA with 4x4 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc4x4 instead.
     */
    RGBAAstc4x4 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc4x4 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc4x4),

    /**
     * ASTC compressed sRGB with alpha with 4x4 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc4x4 instead.
     */
    SRGB8Alpha8Astc4x4 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc4x4 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc4x4),

    /**
     * ASTC compressed RGBA with 5x4 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc5x4 instead.
     */
    RGBAAstc5x4 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc5x4 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc5x4),

    /**
     * ASTC compressed sRGB with alpha with 5x4 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc5x4 instead.
     */
    SRGB8Alpha8Astc5x4 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc5x4 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc5x4),

    /**
     * ASTC compressed RGBA with 5x5 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc5x5 instead.
     */
    RGBAAstc5x5 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc5x5 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc5x5),

    /**
     * ASTC compressed sRGB with alpha with 5x5 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc5x5 instead.
     */
    SRGB8Alpha8Astc5x5 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc5x5 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc5x5),

    /**
     * ASTC compressed RGBA with 6x5 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc6x5 instead.
     */
    RGBAAstc6x5 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc6x5 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc6x5),

    /**
     * ASTC compressed sRGB with alpha with 6x5 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc6x5 instead.
     */
    SRGB8Alpha8Astc6x5 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc6x5 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc6x5),

    /**
     * ASTC compressed RGBA with 6x6 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc6x6 instead.
     */
    RGBAAstc6x6 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc6x6 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc6x6),

    /**
     * ASTC compressed sRGB with alpha with 6x6 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc6x6 instead.
     */
    SRGB8Alpha8Astc6x6 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc6x6 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc6x6),

    /**
     * ASTC compressed RGBA with 8x5 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc8x5 instead.
     */
    RGBAAstc8x5 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc8x5 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc8x5),

    /**
     * ASTC compressed sRGB with alpha with 8x5 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc8x5 instead.
     */
    SRGB8Alpha8Astc8x5 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc8x5 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc8x5),

    /**
     * ASTC compressed RGBA with 8x6 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc8x6 instead.
     */
    RGBAAstc8x6 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc8x6 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc8x6),

    /**
     * ASTC compressed sRGB with alpha with 8x6 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc8x6 instead.
     */
    SRGB8Alpha8Astc8x6 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc8x6 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc8x6),

    /**
     * ASTC compressed RGBA with 8x8 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc8x8 instead.
     */
    RGBAAstc8x8 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc8x8 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc8x8),

    /**
     * ASTC compressed sRGB with alpha with 8x8 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc8x8 instead.
     */
    SRGB8Alpha8Astc8x8 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc8x8 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc8x8),

    /**
     * ASTC compressed RGBA with 10x5 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc10x5 instead.
     */
    RGBAAstc10x5 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc10x5 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc10x5),

    /**
     * ASTC compressed sRGB with alpha with 10x5 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc10x5 instead.
     */
    SRGB8Alpha8Astc10x5 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc10x5 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc10x5),

    /**
     * ASTC compressed RGBA with 10x6 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc10x6 instead.
     */
    RGBAAstc10x6 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc10x6 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc10x6),

    /**
     * ASTC compressed sRGB with alpha with 10x6 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc10x6 instead.
     */
    SRGB8Alpha8Astc10x6 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc10x6 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc10x6),

    /**
     * ASTC compressed RGBA with 10x8 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc10x8 instead.
     */
    RGBAAstc10x8 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc10x8 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc10x8),

    /**
     * ASTC compressed sRGB with alpha with 10x8 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc10x8 instead.
     */
    SRGB8Alpha8Astc10x8 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc10x8 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc10x8),

    /**
     * ASTC compressed RGBA with 10x10 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc10x10 instead.
     */
    RGBAAstc10x10 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc10x10 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc10x10),

    /**
     * ASTC compressed sRGB with alpha with 10x10 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc10x10 instead.
     */
    SRGB8Alpha8Astc10x10 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc10x10 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc10x10),

    /**
     * ASTC compressed RGBA with 12x10 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc12x10 instead.
     */
    RGBAAstc12x10 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc12x10 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc12x10),

    /**
     * ASTC compressed sRGB with alpha with 12x10 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc12x10 instead.
     */
    SRGB8Alpha8Astc12x10 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc12x10 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc12x10),

    /**
     * ASTC compressed RGBA with 12x12 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::RGBAAstc12x12 instead.
     */
    RGBAAstc12x12 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::RGBAAstc12x12 instead") = UnsignedInt(GL::CompressedPixelFormat::RGBAAstc12x12),

    /**
     * ASTC compressed sRGB with alpha with 12x12 blocks.
     * @deprecated Use @ref GL::CompressedPixelFormat::SRGB8Alpha8Astc12x12 instead.
     */
    SRGB8Alpha8Astc12x12 CORRADE_DEPRECATED_ENUM("use GL::CompressedPixelFormat::SRGB8Alpha8Astc12x12 instead") = UnsignedInt(GL::CompressedPixelFormat::SRGB8Alpha8Astc12x12)
    #endif
    #endif
};

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
        "compressedPixelFormatWrap(): implementation-specific value already wrapped or too large"),
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
        "compressedPixelFormatUnwrap(): format doesn't contain a wrapped implementation-specific value"),
        T(UnsignedInt(format) & ~(1u << 31));
}

namespace Implementation {

#if defined(MAGNUM_BUILD_DEPRECATED) && defined(MAGNUM_TARGET_GL)
template<class T> inline typename std::enable_if<!std::is_same<T, PixelFormat>::value, UnsignedInt>::type wrapPixelFormatIfNotGLSpecific(T format) {
    return UnsignedInt(format);
}
inline PixelFormat wrapPixelFormatIfNotGLSpecific(PixelFormat format) {
    return format;
}
#endif

}

}

#endif
