#ifndef Magnum_Vk_PixelFormat_h
#define Magnum_Vk_PixelFormat_h
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

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

/** @file
 * @brief Enum @ref Magnum::Vk::PixelFormat, function @ref Magnum::Vk::hasPixelFormat(), @ref Magnum::Vk::pixelFormat()
 * @m_since_latest
 */

namespace Magnum { namespace Vk {

/**
@brief Pixel format
@m_since_latest

Wraps a subset of @type_vk_keyword{Format} that's usable as a pixel format.

Note that while Vulkan exposes almost all imaginable formats, they're not
universally usable in all scenarios and the support differs from device to
device. To make things simpler, Vulkan provides implicit guarantees where
certain formats can be used, see documentation of each value for more
information. Generally, the following rules apply for uncompressed color
formats:

-   All implicit guarantees are only for optimal tiling, not linear
-   RGB formats of all sizes don't have *any* guarantees due to alignment
    issues, and use of two- or four-channel formats is preferred
-   Single-channel formats provide the same guarantees as a corresponding
    two-channel format, possibly supporting more
-   While 8-bit normalized formats have guaranteed support for linear filtering
    and blending, it's not implicitly guaranteed for 16-bit normalized formats
    --- the support is common, but not universal.
-   Signed normalized formats don't have blit destination and color attachment
    support guaranteed as opposed to unsigned normalized formats
-   Signed and unsigned integral formats of the same channel size and count
    provide the exact same guarantees
-   Shader storage support is implicitly guaranteed only for full-width
    (32-bit) formats, for others it depends on
    @ref DeviceFeature::ShaderStorageImageExtendedFormats
-   Atomic operation support is only guaranteed for single-channel full-width
    integral formats

For compressed color formats, sampling, blit source and linear filtering has
guaranteed support with optimal tiling either for all BC formats, all ETC
formats or all ASTC LDR formats, and a full support for a particular family of
these formats is denoted by the @ref DeviceFeature::TextureCompressionBc,
@ref DeviceFeature::TextureCompressionEtc2 and
@ref DeviceFeature::TextureCompressionAstcLdr features.

@m_enum_values_as_keywords

@see @ref Magnum::PixelFormat, @ref hasPixelFormat(), @ref pixelFormat()
*/
enum class PixelFormat: Int {
    /**
     * Red component, normalized unsigned byte.
     *
     * Sampling, blit source/destination, linear filtering and color attachment
     * including blending has guaranteed support with optimal tiling.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    R8Unorm = VK_FORMAT_R8_UNORM,

    /**
     * Red and green component, normalized unsigned byte.
     *
     * Sampling, blit source/destination, linear filtering and color attachment
     * including blending has guaranteed support with optimal tiling.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RG8Unorm = VK_FORMAT_R8G8_UNORM,

    /**
     * RGB, normalized unsigned byte.
     *
     * Compared to @ref PixelFormat::RG8Unorm / @ref PixelFormat::RGBA8Unorm
     * there are no guarantees for this format.
     */
    RGB8Unorm = VK_FORMAT_R8G8B8_UNORM,

    /**
     * RGBA, normalized unsigned byte.
     *
     * Sampling, blit source/destination, linear filtering, image storage,
     * color attachment including blending has guaranteed support with optimal
     * tiling.
     */
    RGBA8Unorm = VK_FORMAT_R8G8B8A8_UNORM,

    /**
     * Red component, normalized signed byte.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling. Unlike @ref PixelFormat::R8Unorm and
     * @ref PixelFormat::R8I, support for blit destination and color attachment
     * isn't guaranteed.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    R8Snorm = VK_FORMAT_R8_SNORM,

    /**
     * Red and green component, normalized signed byte,
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling. Unlike @ref PixelFormat::RG8Unorm and
     * @ref PixelFormat::RG8I, support for blit destination and color
     * attachment isn't guaranteed.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RG8Snorm = VK_FORMAT_R8G8_SNORM,

    /**
     * RGB, normalized signed byte.
     *
     * Compared to @ref PixelFormat::RG8Snorm / @ref PixelFormat::RGBA8Snorm
     * there are no guarantees for this format.
     */
    RGB8Snorm = VK_FORMAT_R8G8B8_SNORM,

    /**
     * RGBA, normalized signed byte.
     *
     * Sampling, blit source, linear filtering and image storage has guaranteed
     * support with optimal tiling. Unlike @ref PixelFormat::RGBA8Unorm and
     * @ref PixelFormat::RGBA8I, support for blit destination and color
     * attachment isn't guaranteed.
     */
    RGBA8Snorm = VK_FORMAT_R8G8B8A8_SNORM,

    /**
     * sRGB-encoded red component, normalized unsigned byte.
     *
     * Compared to @ref PixelFormat::R8Unorm / @ref PixelFormat::RGBA8Srgb
     * there are no guarantees for this format.
     */
    R8Srgb = VK_FORMAT_R8_SRGB,

    /**
     * sRGB-encoded red and green component, normalized unsigned byte.
     *
     * Compared to @ref PixelFormat::RG8Unorm / @ref PixelFormat::RGBA8Srgb
     * there are no guarantees for this format.
     */
    RG8Srgb = VK_FORMAT_R8G8_SRGB,

    /**
     * sRGB, normalized unsigned byte
     *
     * Compared to @ref PixelFormat::RGBA8Srgb there are no guarantees for this
     * format.
     */
    RGB8Srgb = VK_FORMAT_R8G8B8_SRGB,

    /**
     * sRGB + linear alpha, normalized unsigned byte.
     *
     * Sampling, blit source/destination, linear filtering and color attachment
     * including blending has guaranteed support with optimal tiling. Unlike
     * @ref PixelFormat::RGBA8Unorm, support for image storage isn't
     * guaranteed.
     */
    RGBA8Srgb = VK_FORMAT_R8G8B8A8_SRGB,

    /**
     * Red component, integral unsigned byte.
     *
     * Sampling, blit source/destination and color attachment has guaranteed
     * support with optimal tiling. Compared to @ref PixelFormat::R8Unorm,
     * linear filtering and blending isn't supported.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage. Compared to @ref PixelFormat::R32UI, atomic
     *      operation support is not guaranteed.
     */
    R8UI = VK_FORMAT_R8_UINT,

    /**
     * Red and green component, integral unsigned byte.
     *
     * Sampling, blit source/destination and color attachment has guaranteed
     * support with optimal tiling. Compared to @ref PixelFormat::RG8Unorm,
     * linear filtering and blending isn't supported.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RG8UI = VK_FORMAT_R8G8_UINT,

    /**
     * RGB, integral unsigned byte.
     *
     * Compared to @ref PixelFormat::RG8UI / @ref PixelFormat::RGBA8UI there
     * are no guarantees for this format.
     */
    RGB8UI = VK_FORMAT_R8G8B8_UINT,

    /**
     * RGBA, integral unsigned byte.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling. Compared to
     * @ref PixelFormat::RGBA8Unorm, linear filtering and blending isn't
     * supported.
     */
    RGBA8UI = VK_FORMAT_R8G8B8A8_UINT,

    /**
     * Red component, integral signed byte.
     *
     * Sampling, blit source/destination and color attachment has guaranteed
     * support with optimal tiling. Compared to @ref PixelFormat::R8Snorm,
     * linear filtering and blending isn't supported.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage. Compared to @ref PixelFormat::R32I, atomic
     *      operation support is not guaranteed.
     */
    R8I = VK_FORMAT_R8_SINT,

    /**
     * Red and green component, integral signed byte.
     *
     * Sampling, blit source/destination and color attachment has guaranteed
     * support with optimal tiling. Compared to @ref PixelFormat::RG8Snorm,
     * linear filtering and blending isn't supported.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RG8I = VK_FORMAT_R8G8_SINT,

    /**
     * RGB, integral signed byte.
     *
     * Compared to @ref PixelFormat::RG8I / @ref PixelFormat::RGBA8I there are
     * no guarantees for this format.
     */
    RGB8I = VK_FORMAT_R8G8B8_SINT,

    /**
     * RGBA, integral signed byte.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling. Compared to
     * @ref PixelFormat::RGBA8Snorm, linear filtering and blending isn't
     * supported.
     */
    RGBA8I = VK_FORMAT_R8G8B8A8_SINT,

    /**
     * Red component, normalized unsigned short.
     *
     * Compared to @ref PixelFormat::R8Unorm there are no implicit guarantees
     * for this format.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    R16Unorm = VK_FORMAT_R16_UNORM,

    /**
     * Red and green component, normalized unsigned short.
     *
     * Compared to @ref PixelFormat::RG8Unorm there are no implicit guarantees
     * for this format.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RG16Unorm = VK_FORMAT_R16G16_UNORM,

    /**
     * RGB, normalized unsigned short.
     *
     * Compared to @ref PixelFormat::RG16Unorm / @ref PixelFormat::RGBA16Unorm
     * there are no guarantees for this format.
     */
    RGB16Unorm = VK_FORMAT_R16G16B16_UNORM,

    /**
     * RGBA, normalized unsigned short.
     *
     * Compared to @ref PixelFormat::RGBA8Unorm there are no implicit
     * guarantees for this format.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RGBA16Unorm = VK_FORMAT_R16G16B16A16_UNORM,

    /**
     * Red component, normalized signed short.
     *
     * Compared to @ref PixelFormat::R8Unorm there are no implicit guarantees
     * for this format.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    R16Snorm = VK_FORMAT_R16_SNORM,

    /**
     * Red and green component, normalized signed short.
     *
     * Compared to @ref PixelFormat::RG8Snorm there are no implicit guarantees
     * for this format.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RG16Snorm = VK_FORMAT_R16G16_SNORM,

    /**
     * RGB, normalized signed short.
     *
     * Compared to @ref PixelFormat::RG16Snorm / @ref PixelFormat::RGBA16Snorm
     * there are no guarantees for this format.
     */
    RGB16Snorm = VK_FORMAT_R16G16B16_SNORM,

    /**
     * RGBA, normalized signed short.
     *
     * Compared to @ref PixelFormat::RGBA8Snorm there are no implicit
     * guarantees for this format.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RGBA16Snorm = VK_FORMAT_R16G16B16A16_SNORM,

    /**
     * Red component, integral unsigned short.
     *
     * Sampling, blit source/destination and color attachment has guaranteed
     * support with optimal tiling.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage. Compared to @ref PixelFormat::R32UI, atomic
     *      operation support is not guaranteed.
     */
    R16UI = VK_FORMAT_R16_UINT,

    /**
     * Red and green component, integral unsigned short.
     *
     * Sampling, blit source/destination and color attachment has guaranteed
     * support with optimal tiling.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RG16UI = VK_FORMAT_R16G16_UINT,

    /**
     * RGB, integral unsigned short.
     *
     * Compared to @ref PixelFormat::RG16UI / @ref PixelFormat::RGBA16UI there
     * are no guarantees for this format.
     */
    RGB16UI = VK_FORMAT_R16G16B16_UINT,

    /**
     * RGBA, integral unsigned short.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling.
     */
    RGBA16UI = VK_FORMAT_R16G16B16A16_UINT,

    /**
     * Red component, integral signed short.
     *
     * Sampling, blit source/destination and color attachment has guaranteed
     * support with optimal tiling.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage. Compared to @ref PixelFormat::R32I, atomic
     *      operation support is not guaranteed.
     */
    R16I = VK_FORMAT_R16_SINT,

    /**
     * Red and green component, integral signed short.
     *
     * Sampling, blit source/destination and color attachment has guaranteed
     * support with optimal tiling.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RG16I = VK_FORMAT_R16G16_SINT,

    /**
     * RGB, integral signed short.
     *
     * Compared to @ref PixelFormat::RG16I / @ref PixelFormat::RGBA16I there
     * are no guarantees for this format.
     */
    RGB16I = VK_FORMAT_R16G16B16_SINT,

    /**
     * RGBA, integral signed short.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling.
     */
    RGBA16I = VK_FORMAT_R16G16B16A16_SINT,

    /**
     * Red component, integral unsigned int.
     *
     * Sampling, blit source/destination, image storage including atomic
     * operations and color attachment has guaranteed support with optimal
     * tiling.
     */
    R32UI = VK_FORMAT_R32_UINT,

    /**
     * Red and green component, integral unsigned int.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling.
     */
    RG32UI = VK_FORMAT_R32G32_UINT,

    /**
     * RGB, integral unsigned int.
     *
     * Compared to @ref PixelFormat::RG32UI / @ref PixelFormat::RGBA32UI there
     * are no guarantees for this format.
     */
    RGB32UI = VK_FORMAT_R32G32B32_UINT,

    /**
     * RGBA, integral unsigned int.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling.
     */
    RGBA32UI = VK_FORMAT_R32G32B32A32_UINT,

    /**
     * Red component, integral signed int.
     *
     * Sampling, blit source/destination, image storage including atomic
     * operations and color attachment has guaranteed support with optimal
     * tiling.
     */
    R32I = VK_FORMAT_R32_SINT,

    /**
     * Red and green component, integral signed int.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling.
     */
    RG32I = VK_FORMAT_R32G32_SINT,

    /**
     * RGB, integral signed int.
     *
     * Compared to @ref PixelFormat::RG32UI / @ref PixelFormat::RGBA32UI there
     * are no guarantees for this format.
     */
    RGB32I = VK_FORMAT_R32G32B32_SINT,

    /**
     * RGBA, integral signed int.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling.
     */
    RGBA32I = VK_FORMAT_R32G32B32A32_SINT,

    /**
     * Red component, half float.
     *
     * Sampling, blit source/destination, linear filtering and color attachment
     * including blending has guaranteed support with optimal tiling.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    R16F = VK_FORMAT_R16_SFLOAT,

    /**
     * Red and green component, half float.
     *
     * Sampling, blit source/destination, linear filtering and color attachment
     * including blending has guaranteed support with optimal tiling.
     * @requires_vk_feature @ref DeviceFeature::ShaderStorageImageExtendedFormats
     *      for image storage
     */
    RG16F = VK_FORMAT_R16G16_SFLOAT,

    /**
     * RGB, half float.
     *
     * Compared to @ref PixelFormat::RG16F / @ref PixelFormat::RGBA16F there
     * are no guarantees for this format.
     */
    RGB16F = VK_FORMAT_R16G16B16_SFLOAT,

    /**
     * RGBA, half float.
     *
     * Sampling, blit source/destination, image storage, linear filtering and
     * color attachment including blending has guaranteed support with optimal
     * tiling.
     */
    RGBA16F = VK_FORMAT_R16G16B16A16_SFLOAT,

    /**
     * Red component, float.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling. Compared to
     * @ref PixelFormat::R16F, linear filtering and blending support is not
     * guaranteed.
     */
    R32F = VK_FORMAT_R32_SFLOAT,

    /**
     * Red and green component, float.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling. Compared to
     * @ref PixelFormat::RG16F, linear filtering and blending support is not
     * guaranteed.
     */
    RG32F = VK_FORMAT_R32G32_SFLOAT,

    /**
     * RGB, float.
     *
     * Compared to @ref PixelFormat::RG32F / @ref PixelFormat::RGBA32F there
     * are no guarantees for this format.
     */
    RGB32F = VK_FORMAT_R32G32B32_SFLOAT,

    /**
     * RGBA, float.
     *
     * Sampling, blit source/destination, image storage and color attachment
     * has guaranteed support with optimal tiling. Compared to
     * @ref PixelFormat::RGBA16F, linear filtering and blending support is not
     * guaranteed.
     */
    RGBA32F = VK_FORMAT_R32G32B32A32_SFLOAT,

    /**
     * 16-bit unsigned normalized depth.
     *
     * Sampling, blit source and depth attachment has guaranteed support with
     * optimal tiling.
     */
    Depth16Unorm = VK_FORMAT_D16_UNORM,

    /**
     * 24-bit unsigned normalized depth with 8-bit padding.
     *
     * Depth attachment support with optimal tiling is guaranteed for either
     * this or the @ref PixelFormat::Depth32F format
     */
    Depth24Unorm = VK_FORMAT_X8_D24_UNORM_PACK32,

    /**
     * 32-bit float depth.
     *
     * Sampling and blit source has guaranteed support with optimal tiling.
     * Depth attachment support with optimal tiling is guaranteed for either
     * this or the @ref PixelFormat::Depth24Unorm format.
     */
    Depth32F = VK_FORMAT_D32_SFLOAT,

    /**
     * 8-bit unsigned integral stencil.
     *
     * There are no guarantees for this format.
     */
    Stencil8UI = VK_FORMAT_S8_UINT,

    /**
     * 16-bit unsigned normalized depth with 8-bit unsigned integral stencil.
     *
     * There are no guarantees for this format.
     */
    Depth16UnormStencil8UI = VK_FORMAT_D16_UNORM_S8_UINT,

    /**
     * 24-bit unsigned normalized depth with 8-bit unsigned integral stencil.
     *
     * Depth/stencil attachment support with optimal tiling is guaranteed for
     * either this or the @ref PixelFormat::Depth32FStencil8UI format.
     */
    Depth24UnormStencil8UI = VK_FORMAT_D24_UNORM_S8_UINT,

    /**
     * 32-bit float depth with 8-bit unsigned integral stencil.
     *
     * Depth/stencil attachment support with optimal tiling is guaranteed for
     * either this or the @ref PixelFormat::Depth24UnormStencil8UI format.
     */
    Depth32FStencil8UI = VK_FORMAT_D32_SFLOAT_S8_UINT,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC1
     * compressed RGB, normalized unsigned.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc1RGBUnorm = VK_FORMAT_BC1_RGB_UNORM_BLOCK,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC1
     * compressed sRGB, normalized unsigned.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc1RGBSrgb = VK_FORMAT_BC1_RGB_SRGB_BLOCK,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC1
     * compressed RGBA, normalized unsigned.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc1RGBAUnorm = VK_FORMAT_BC1_RGBA_UNORM_BLOCK,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC1
     * compressed sRGB + linear alpha, normalized unsigned.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc1RGBASrgb = VK_FORMAT_BC1_RGBA_SRGB_BLOCK,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC2
     * compressed RGBA, normalized unsigned.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc2RGBAUnorm = VK_FORMAT_BC2_UNORM_BLOCK,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC2
     * compressed sRGB + linear alpha, normalized unsigned.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc2RGBASrgb = VK_FORMAT_BC2_SRGB_BLOCK,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC3
     * compressed RGBA, normalized unsigned.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc3RGBAUnorm = VK_FORMAT_BC3_UNORM_BLOCK,

    /**
     * [S3TC](https://en.wikipedia.org/wiki/S3_Texture_Compression) BC3
     * compressed sRGB + linear alpha, normalized unsigned.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc3RGBASrgb = VK_FORMAT_BC3_SRGB_BLOCK,

    /**
     * [3Dc+](https://en.wikipedia.org/wiki/3Dc#3Dc+) BC4 compressed red
     * component, unsigned normalized.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc4RUnorm = VK_FORMAT_BC4_UNORM_BLOCK,

    /**
     * [3Dc+](https://en.wikipedia.org/wiki/3Dc#3Dc+) BC4 compressed red
     * component, signed normalized.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc4RSnorm = VK_FORMAT_BC4_SNORM_BLOCK,

    /**
     * [3Dc](https://en.wikipedia.org/wiki/3Dc) BC5 compressed red and green
     * component, unsigned normalized.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc5RGUnorm = VK_FORMAT_BC5_UNORM_BLOCK,

    /**
     * [3Dc](https://en.wikipedia.org/wiki/3Dc) BC5 compressed red and green
     * component, signed normalized.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc5RGSnorm = VK_FORMAT_BC5_SNORM_BLOCK,

    /**
     * [BC6H](https://docs.microsoft.com/en-us/windows/win32/direct3d11/bc6h-format)
     * compressed RGB, unsigned float.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc6hRGBUfloat = VK_FORMAT_BC6H_UFLOAT_BLOCK,

    /**
     * [BC6H](https://docs.microsoft.com/en-us/windows/win32/direct3d11/bc6h-format)
     * compressed RGB, signed float.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc6hRGBSfloat = VK_FORMAT_BC6H_SFLOAT_BLOCK,

    /**
     * [BC7](https://docs.microsoft.com/en-us/windows/win32/direct3d11/bc7-format),
     * compressed RGBA, unsigned normalized.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc7RGBAUnorm = VK_FORMAT_BC7_UNORM_BLOCK,

    /**
     * [BC7](https://docs.microsoft.com/en-us/windows/win32/direct3d11/bc7-format),
     * compressed sRGB + linear alpha, unsigned normalized.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionBc
     */
    CompressedBc7RGBASrgb = VK_FORMAT_BC7_SRGB_BLOCK,

    /**
     * [EAC](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed red component, normalized unsigned 11-bit.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionEtc2
     */
    CompressedEacR11Unorm = VK_FORMAT_EAC_R11_UNORM_BLOCK,

    /**
     * [EAC](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed red component, normalized signed 11-bit.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionEtc2
     */
    CompressedEacR11Snorm = VK_FORMAT_EAC_R11_SNORM_BLOCK,

    /**
     * [EAC](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed red and green component, normalized unsigned 11-bit.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionEtc2
     */
    CompressedEacRG11Unorm = VK_FORMAT_EAC_R11G11_UNORM_BLOCK,

    /**
     * [EAC](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed red and green component, normalized signed 11-bit.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionEtc2
     */
    CompressedEacRG11Snorm = VK_FORMAT_EAC_R11G11_SNORM_BLOCK,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed RGB, normalized unsigned byte.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionEtc2
     */
    CompressedEtc2RGB8Unorm = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed sRGB, normalized unsigned byte.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionEtc2
     */
    CompressedEtc2RGB8Srgb = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed RGB, normalized unsigned byte + a single-bit alpha.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionEtc2
     */
    CompressedEtc2RGB8A1Unorm = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed sRGB, normalized unsigned byte + a single-bit alpha.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionEtc2
     */
    CompressedEtc2RGB8A1Srgb = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed RGBA, normalized unsigned byte.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionEtc2
     */
    CompressedEtc2RGBA8Unorm = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,

    /**
     * [ETC2](https://en.wikipedia.org/wiki/Ericsson_Texture_Compression#ETC2_and_EAC)
     * compressed sRGB + linear alpha, normalized unsigned byte.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionEtc2
     */
    CompressedEtc2RGBA8Srgb = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 4x4 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc4x4RGBAUnorm = VK_FORMAT_ASTC_4x4_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 4x4 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc4x4RGBASrgb = VK_FORMAT_ASTC_4x4_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 4x4 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc4x4RGBAF = VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 5x4 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc5x4RGBAUnorm = VK_FORMAT_ASTC_5x4_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 5x4 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc5x4RGBASrgb = VK_FORMAT_ASTC_5x4_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 5x4 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc5x4RGBAF = VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 5x5 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc5x5RGBAUnorm = VK_FORMAT_ASTC_5x5_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 5x5 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc5x5RGBASrgb = VK_FORMAT_ASTC_5x5_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 5x5 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc5x5RGBAF = VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 6x5 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc6x5RGBAUnorm = VK_FORMAT_ASTC_6x5_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 6x5 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc6x5RGBASrgb = VK_FORMAT_ASTC_6x5_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 6x5 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc6x5RGBAF = VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 6x6 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc6x6RGBAUnorm = VK_FORMAT_ASTC_6x6_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 6x6 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc6x6RGBASrgb = VK_FORMAT_ASTC_6x6_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 6x6 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc6x6RGBAF = VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 8x5 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc8x5RGBAUnorm = VK_FORMAT_ASTC_8x5_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 8x5 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc8x5RGBASrgb = VK_FORMAT_ASTC_8x5_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 8x5 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc8x5RGBAF = VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 8x6 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc8x6RGBAUnorm = VK_FORMAT_ASTC_8x6_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 8x6 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc8x6RGBASrgb = VK_FORMAT_ASTC_8x6_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 8x6 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc8x6RGBAF = VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 8x8 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc8x8RGBAUnorm = VK_FORMAT_ASTC_8x8_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 8x8 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc8x8RGBASrgb = VK_FORMAT_ASTC_8x8_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 8x8 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc8x8RGBAF = VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 10x5 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc10x5RGBAUnorm = VK_FORMAT_ASTC_10x5_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 10x5 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc10x5RGBASrgb = VK_FORMAT_ASTC_10x5_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 10x5 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc10x5RGBAF = VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 10x6 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc10x6RGBAUnorm = VK_FORMAT_ASTC_10x6_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 10x6 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc10x6RGBASrgb = VK_FORMAT_ASTC_10x6_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 10x6 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc10x6RGBAF = VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 10x8 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc10x8RGBAUnorm = VK_FORMAT_ASTC_10x8_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 10x8 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc10x8RGBASrgb = VK_FORMAT_ASTC_10x8_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 10x8 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc10x8RGBAF = VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 10x10 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc10x10RGBAUnorm = VK_FORMAT_ASTC_10x10_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 10x10 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc10x10RGBASrgb = VK_FORMAT_ASTC_10x10_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 10x10 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc10x10RGBAF = VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 12x10 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc12x10RGBAUnorm = VK_FORMAT_ASTC_12x10_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 12x10 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc12x10RGBASrgb = VK_FORMAT_ASTC_12x10_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 12x10 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc12x10RGBAF = VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, normalized unsigned with 12x12 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc12x12RGBAUnorm = VK_FORMAT_ASTC_12x12_UNORM_BLOCK,

    /**
     * 2D LDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed sRGB + linear alpha, normalized unsigned with 12x12 blocks.
     *
     * Sampling, blit source and linear filtering has guaranteed support with
     * optimal tiling either for all BC formats, all ETC formats or all ASTC
     * LDR formats.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr
     */
    CompressedAstc12x12RGBASrgb = VK_FORMAT_ASTC_12x12_SRGB_BLOCK,

    /**
     * 2D HDR [ASTC](https://en.wikipedia.org/wiki/Adaptive_Scalable_Texture_Compression)
     * compressed RGBA, float with 12x12 blocks.
     *
     * @requires_vk_feature @ref DeviceFeature::TextureCompressionAstcHdr
     */
    CompressedAstc12x12RGBAF = VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT,

    /* See https://github.com/KhronosGroup/Vulkan-Docs/issues/512#issuecomment-307768667
       for how this maps to actual channels. Ugh. */

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed RGBA, normalized
     * unsigned byte with 2 bits per pixel.
     *
     * @requires_vk_extension Extension @vk_extension{IMG,format_pvrtc}
     */
    CompressedPvrtcRGBA2bppUnorm = VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed sRGB + linear
     * alpha, normalized unsigned byte with 2 bits per pixel.
     *
     * @requires_vk_extension Extension @vk_extension{IMG,format_pvrtc}
     */
    CompressedPvrtcRGBA2bppSrgb = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed RGBA, normalized
     * unsigned byte with 4 bits per pixel.
     *
     * @requires_vk_extension Extension @vk_extension{IMG,format_pvrtc}
     */
    CompressedPvrtcRGBA4bppUnorm = VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,

    /**
     * [PVRTC](https://en.wikipedia.org/wiki/PVRTC) compressed sRGB + linear
     * alpha, normalized unsigned byte with 4 bits per pixel.
     *
     * @requires_vk_extension Extension @vk_extension{IMG,format_pvrtc}
     */
    CompressedPvrtcRGBA4bppSrgb = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,

    /**
     * [PVRTC2](https://en.wikipedia.org/wiki/PVRTC) compressed RGBA,
     * normalized unsigned byte with 2 bits per pixel.
     *
     * @requires_vk_extension Extension @vk_extension{IMG,format_pvrtc}
     */
    CompressedPvrtc2RGBA2bppUnorm = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,

    /**
     * [PVRTC2](https://en.wikipedia.org/wiki/PVRTC) compressed sRGB + linear
     * alpha, normalized unsigned byte with 2 bits per pixel.
     *
     * @requires_vk_extension Extension @vk_extension{IMG,format_pvrtc}
     */
    CompressedPvrtc2RGBA2bppSrgb = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG,

    /**
     * [PVRTC2](https://en.wikipedia.org/wiki/PVRTC) compressed RGBA,
     * normalized unsigned byte with 4 bits per pixel.
     *
     * @requires_vk_extension Extension @vk_extension{IMG,format_pvrtc}
     */
    CompressedPvrtc2RGBA4bppUnorm = VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,

    /**
     * [PVRTC2](https://en.wikipedia.org/wiki/PVRTC) compressed sRGB + linear
     * alpha, normalized unsigned byte with 4 bits per pixel.
     *
     * @requires_vk_extension Extension @vk_extension{IMG,format_pvrtc}
     */
    CompressedPvrtc2RGBA4bppSrgb = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG
};

/**
@debugoperatorenum{PixelFormat}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, PixelFormat value);

/**
@brief Check availability of a generic pixel format
@m_since_latest

Returns @cpp false @ce if Vulkan doesn't support such format, @cpp true @ce
otherwise. Moreover, returns @cpp true @ce also for all formats that are
@ref isPixelFormatImplementationSpecific(). The @p format value is expected to
be valid.

@note Support of some formats depends on presence of a particular Vulkan
    extension. Such check is outside of the scope of this function and you are
    expected to verify extension availability before using such format.

@see @ref pixelFormat(Magnum::PixelFormat)
*/
MAGNUM_VK_EXPORT bool hasPixelFormat(Magnum::PixelFormat format);

/**
@brief Check availability of a generic compressed pixel format
@m_since_latest

Returns @cpp false @ce if Vulkan doesn't support such format, @cpp true @ce
otherwise. Moreover, returns @cpp true @ce also for all formats that are
@ref isCompressedPixelFormatImplementationSpecific(). The @p format value is
expected to be valid.

@note Support of some formats depends on presence of a particular Vulkan
    extension. Such check is outside of the scope of this function and you are
    expected to verify extension availability before using such format.

@see @ref pixelFormat(Magnum::CompressedPixelFormat)
*/
MAGNUM_VK_EXPORT bool hasPixelFormat(Magnum::CompressedPixelFormat format);

/**
@brief Convert a generic pixel format to Vulkan pixel format
@m_since_latest

In case @ref isPixelFormatImplementationSpecific() returns @cpp false @ce for
@p format, maps it to a corresponding Vulkan format. In case
@ref isPixelFormatImplementationSpecific() returns @cpp true @ce, assumes
@p format stores a Vulkan-specific format and returns @ref pixelFormatUnwrap()
cast to @ref PixelFormat.

Not all generic pixel formats have a Vulkan equivalent and this function
expects that given format is available. Use
@ref hasPixelFormat(Magnum::PixelFormat) to query availability of given format.
*/
MAGNUM_VK_EXPORT PixelFormat pixelFormat(Magnum::PixelFormat format);

/**
@brief Convert a generic compressed pixel format to Vulkan pixel format
@m_since_latest

In case @ref isCompressedPixelFormatImplementationSpecific() returns
@cpp false @ce for @p format, maps it to a corresponding Vulkan format. In case
@ref isCompressedPixelFormatImplementationSpecific() returns @cpp true @ce,
assumes @p format stores a Vulkan-specific format and returns
@ref compressedPixelFormatUnwrap() cast to @ref PixelFormat.

Not all generic pixel formats have a Vulkan equivalent and this function
expects that given format is available. Use
@ref hasPixelFormat(Magnum::CompressedPixelFormat) to query availability of given
format.
*/
MAGNUM_VK_EXPORT PixelFormat pixelFormat(Magnum::CompressedPixelFormat format);

}}

#endif
