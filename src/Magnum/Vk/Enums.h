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

#ifdef MAGNUM_BUILD_DEPRECATED
/** @file
 * @brief Function @ref Magnum::Vk::hasVkPrimitiveTopology(), @ref Magnum::Vk::vkPrimitiveTopology(), @ref Magnum::Vk::hasVkIndexType(), @ref Magnum::Vk::vkIndexType(), @ref Magnum::Vk::hasVkFormat(), @ref Magnum::Vk::vkFormat(), @ref Magnum::Vk::vkFilter(), @ref Magnum::Vk::vkSamplerMipmapMode(), @ref Magnum::Vk::hasVkSamplerAddressMode(), @ref Magnum::Vk::vkSamplerAddressMode()
 * @m_deprecated_since_latest All functionality in this header has been
 *      deprecated and moved elsewhere. Use headers corresponding to the
 *      suggested replacement APIs instead.
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/Array.h" /* That's fine, we're deprecated too */
#include "Magnum/Math/Vector3.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

#include <Corrade/Utility/Macros.h>

#ifndef _MAGNUM_NO_DEPRECATED_VK_ENUMS
CORRADE_DEPRECATED_FILE("use headers corresponding to the suggested replacement APIs instead")
#endif

namespace Magnum { namespace Vk {

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

/**
 * @brief @copybrief samplerFilter()
 * @m_deprecated_since_latest Use @ref samplerFilter() instead.
 */
CORRADE_DEPRECATED("use samplerFilter() instead") MAGNUM_VK_EXPORT VkFilter vkFilter(Magnum::SamplerFilter filter);

/**
 * @brief @copybrief samplerMipmap()
 * @m_deprecated_since_latest Use @ref samplerMipmap() instead.
 */
CORRADE_DEPRECATED("use samplerMipmap() instead") MAGNUM_VK_EXPORT VkSamplerMipmapMode vkSamplerMipmapMode(Magnum::SamplerMipmap mipmap);

/**
 * @brief Check availability of a generic sampler wrapping mode
 * @m_deprecated_since_latest All generic sampler wrapping modes are available
 *      in Vulkan.
 */
CORRADE_DEPRECATED("all generic sampler wrapping modes are available in Vulkan") MAGNUM_VK_EXPORT bool hasVkSamplerAddressMode(Magnum::SamplerWrapping wrapping);

/**
 * @brief @copybrief samplerWrapping()
 * @m_deprecated_since_latest Use @ref samplerWrapping() instead.
 */
CORRADE_DEPRECATED("use samplerWrapping() instead") MAGNUM_VK_EXPORT VkSamplerAddressMode vkSamplerAddressMode(Magnum::SamplerWrapping wrapping);

/**
 * @brief @copybrief samplerWrapping()
 * @m_deprecated_since_latest Use @ref samplerWrapping() instead.
 */
template<std::size_t dimensions> CORRADE_DEPRECATED("use samplerWrapping() instead") Math::Vector<dimensions, VkSamplerAddressMode> vkSamplerAddressMode(const Math::Vector<dimensions, Magnum::SamplerWrapping>& wrapping) {
    Math::Vector<dimensions, VkSamplerAddressMode> out{NoInit};
    for(std::size_t i = 0; i != dimensions; ++i)
        out[i] = vkSamplerAddressMode(wrapping[i]);
    return out;
}

}}
#else
#error use headers corresponding to the suggested replacement APIs instead
#endif

#endif
