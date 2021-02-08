#ifndef Magnum_Vk_Enums_h
#define Magnum_Vk_Enums_h
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
 * @brief Function @ref Magnum::Vk::hasVkPrimitiveTopology(), @ref Magnum::Vk::vkPrimitiveTopology(), @ref Magnum::Vk::hasVkIndexType(), @ref Magnum::Vk::vkIndexType(), @ref Magnum::Vk::hasVkFormat(), @ref Magnum::Vk::vkFormat(), @ref Magnum::Vk::vkFilter(), @ref Magnum::Vk::vkSamplerMipmapMode(), @ref Magnum::Vk::hasVkSamplerAddressMode(), @ref Magnum::Vk::vkSamplerAddressMode()
 */

#include "Magnum/Array.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Vk {

#ifdef MAGNUM_BUILD_DEPRECATED
/**
 * @brief @copybrief hasMeshPrimitive()
 * @m_deprecated_since_latest Use @ref hasMeshPrimitive() instead.
 */
CORRADE_DEPRECATED("use hasMeshPrimitive() instead") MAGNUM_VK_EXPORT bool hasVkPrimitiveTopology(Magnum::MeshPrimitive primitive);

/**
 * @brief @copybrief meshPrimitive()
 * @m_deprecated_since_latest Use @ref meshPrimitive() instead.
 */
CORRADE_DEPRECATED("use meshPrimitive() instead") MAGNUM_VK_EXPORT VkPrimitiveTopology vkPrimitiveTopology(Magnum::MeshPrimitive primitive);

/**
 * @brief Check availability of a generic index type
 * @m_deprecated_since_latest All generic index types are available in Vulkan.
 */
CORRADE_DEPRECATED("all generic index types are available in Vulkan") MAGNUM_VK_EXPORT bool hasVkIndexType(Magnum::MeshIndexType type);

/**
 * @brief @copybrief meshIndexType()
 * @m_deprecated_since_latest Use @ref meshIndexType() instead.
 */
CORRADE_DEPRECATED("use meshIndexType() instead") MAGNUM_VK_EXPORT VkIndexType vkIndexType(Magnum::MeshIndexType type);

/**
 * @brief @copybrief hasVertexFormat()
 * @m_deprecated_since_latest Use @ref hasVertexFormat() instead.
 */
CORRADE_DEPRECATED("use hasVertexFormat() instead") MAGNUM_VK_EXPORT bool hasVkFormat(Magnum::VertexFormat format);

/**
 * @brief @copybrief hasPixelFormat(Magnum::PixelFormat)
 * @m_deprecated_since_latest Use @ref hasPixelFormat(Magnum::PixelFormat)
 *      instead.
 */
CORRADE_DEPRECATED("use hasPixelFormat() instead") MAGNUM_VK_EXPORT bool hasVkFormat(Magnum::PixelFormat format);

/**
 * @brief @copybrief hasPixelFormat(Magnum::CompressedPixelFormat)
 * @m_deprecated_since_latest Use @ref hasPixelFormat(Magnum::CompressedPixelFormat)
 *      instead.
 */
CORRADE_DEPRECATED("use hasPixelFormat() instead") MAGNUM_VK_EXPORT bool hasVkFormat(Magnum::CompressedPixelFormat format);

/**
 * @brief @copybrief vertexFormat()
 * @m_deprecated_since_latest Use @ref vertexFormat() instead.
 */
CORRADE_DEPRECATED("use vertexFormat() instead") MAGNUM_VK_EXPORT VkFormat vkFormat(Magnum::VertexFormat format);

/**
 * @brief @copybrief pixelFormat(Magnum::PixelFormat)
 * @m_deprecated_since_latest Use @ref pixelFormat(Magnum::PixelFormat)
 *      instead.
 */
CORRADE_DEPRECATED("use pixelFormat() instead") MAGNUM_VK_EXPORT VkFormat vkFormat(Magnum::PixelFormat format);

/**
 * @brief @copybrief pixelFormat(Magnum::CompressedPixelFormat)
 * @m_deprecated_since_latest Use @ref pixelFormat(Magnum::CompressedPixelFormat)
 *      instead.
 */
CORRADE_DEPRECATED("use pixelFormat() instead") MAGNUM_VK_EXPORT VkFormat vkFormat(Magnum::CompressedPixelFormat format);
#endif

/**
@brief Convert generic sampler filter to Vulkan filter

@see @ref vkSamplerMipmapMode(), @ref vkSamplerAddressMode()
*/
MAGNUM_VK_EXPORT VkFilter vkFilter(Magnum::SamplerFilter filter);

/**
@brief Convert generic sampler mipomap mode to Vulkan sampler mipmap mode

Vulkan doesn't support the @ref SamplerMipmap::Base value directly, instead
@val_vk{SAMPLER_MIPMAP_MODE_NEAREST,SamplerMipmapMode} is used and you have to
configure the sampler to use just a single mipmap level.
@see @ref vkFilter(), @ref vkSamplerAddressMode()
*/
MAGNUM_VK_EXPORT VkSamplerMipmapMode vkSamplerMipmapMode(Magnum::SamplerMipmap mipmap);

/**
@brief Check availability of a generic sampler wrapping mode

Returns @cpp false @ce if Vulkan doesn't support such wrapping, @cpp true @ce
otherwise. The @p wrapping value is expected to be valid.

@note Support of some modes depends on presence of a particular Vulkan
    extension. Such check is outside of the scope of this function and you are
    expected to verify extension availability before using such mode.

@see @ref vkSamplerAddressMode(), @ref vkFilter(), @ref vkSamplerMipmapMode()
*/
MAGNUM_VK_EXPORT bool hasVkSamplerAddressMode(Magnum::SamplerWrapping wrapping);

/**
@brief Convert generic sampler filter mode to Vulkan sampler address mode

Not all generic sampler wrapping modes have a Vulkan equivalent and this
function expects that given mode is available. Use @ref hasVkSamplerAddressMode()
to query availability of given mode.
@see @ref vkFilter(), @ref vkSamplerAddressMode()
*/
MAGNUM_VK_EXPORT VkSamplerAddressMode vkSamplerAddressMode(Magnum::SamplerWrapping wrapping);

/** @overload */
template<UnsignedInt dimensions> Array<dimensions, VkSamplerAddressMode> vkSamplerAddressMode(const Array<dimensions, Magnum::SamplerWrapping>& wrapping) {
    Array<dimensions, VkSamplerAddressMode> out; /** @todo NoInit */
    for(std::size_t i = 0; i != dimensions; ++i)
        out[i] = vkSamplerAddressMode(wrapping[i]);
    return out;
}

}}

#endif
