#ifndef Magnum_Vk_Memory_h
#define Magnum_Vk_Memory_h
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

/** @file
 * @brief Enum @ref Magnum::Vk::MemoryFlag, enum set @ref Magnum::Vk::MemoryFlags
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Memory type flag
@m_since_latest

Wraps a @type_vk_keyword{MemoryPropertyFlagBits}.
@see @ref MemoryFlags, @ref DeviceProperties::memoryFlags()
@m_enum_values_as_keywords
*/
enum class MemoryFlag: UnsignedInt {
    /**
     * Device local. Always corresponds to a heap with
     * @ref MemoryHeapFlag::DeviceLocal.
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      This memory is the most efficient for device access.
     */
    DeviceLocal = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,

    /** Memory that can be mapped for host access */
    HostVisible = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,

    /** Memory with coherent access on the host */
    HostCoherent = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,

    /**
     * Memory that is cached on the host. Host memory accesses to uncached
     * memory are slower than to cached memory, however uncached memory is
     * always @ref MemoryFlag::HostCoherent.
     */
    HostCached = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,

    /**
     * Lazily allocated memory. Allows only device access (i.e., there's no
     * memory that has both this and @ref MemoryFlag::HostVisible set).
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      The device is allowed (but not required) to allocate the memory
     *      as-needed and thus is useful for example for temporary framebuffer
     *      attachments --- certain tiled architectures might not even need to
     *      allocate the memory in that case.
     */
    LazilyAllocated = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,

    /** @todo Protected, VK 1.1 */
};

/**
@debugoperatorclassenum{DeviceProperties,MemoryFlag}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, MemoryFlag value);

/**
@brief Memory type flags
@m_since_latest

@see @ref DeviceProperties::memoryFlags()
*/
typedef Containers::EnumSet<MemoryFlag> MemoryFlags;

CORRADE_ENUMSET_OPERATORS(MemoryFlags)

/**
@debugoperatorclassenum{DeviceProperties,MemoryFlags}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, MemoryFlags value);

}}

#endif
