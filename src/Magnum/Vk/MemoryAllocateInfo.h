#ifndef Magnum_Vk_MemoryAllocateInfo_h
#define Magnum_Vk_MemoryAllocateInfo_h
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
 * @brief Class @ref Magnum::Vk::MemoryRequirements, @ref Magnum::Vk::MemoryAllocateInfo
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Device memory requirements
@m_since_latest

Wraps a @type_vk_keyword{MemoryRequirements2}. Not constructible directly,
returned from @ref Image::memoryRequirements() and
@ref Buffer::memoryRequirements().
@see @ref DeviceProperties::pickMemory()
*/
class MAGNUM_VK_EXPORT MemoryRequirements {
    public:
        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit MemoryRequirements(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit MemoryRequirements(const VkMemoryRequirements2& requirements);

        /** @brief Underlying @type_vk{MemoryRequirements} structure */
        VkMemoryRequirements2& requirements() { return _requirements; }
        /** @overload */
        const VkMemoryRequirements2& requirements() const { return _requirements; }
        /** @overload */
        operator VkMemoryRequirements2&() { return _requirements; }
        /** @overload */
        operator const VkMemoryRequirements2&() const { return _requirements; }
        /** @overload */
        VkMemoryRequirements2* operator->() { return &_requirements; }
        /** @overload */
        const VkMemoryRequirements2* operator->() const { return &_requirements; }

        /**
         * @brief Required memory size
         *
         * @see @ref alignedSize()
         */
        UnsignedLong size() const {
            return _requirements.memoryRequirements.size;
        }

        /**
         * @brief Required memory alignment
         *
         * @see @ref alignedSize()
         */
        UnsignedLong alignment() const {
            return _requirements.memoryRequirements.alignment;
        }

        /**
         * @brief Required memory size rounded up for given alignment
         *
         * Pads @ref size() with given alignment requirements. For example, a
         * 13765-byte buffer aligned to 4 kB would be 16384 bytes. See the
         * @ref Memory class for more information and example usage.
         *
         * The alignment is expected to be non-zero.
         */
        UnsignedLong alignedSize(UnsignedLong alignment) const;

        /** @brief Bits indicating which memory  */
        UnsignedInt memories() const {
            return _requirements.memoryRequirements.memoryTypeBits;
        }

    private:
        friend Buffer;
        friend Image;

        explicit MemoryRequirements();

        VkMemoryRequirements2 _requirements;
};

/**
@brief Memory allocation info
@m_since_latest

Wraps a @type_vk_keyword{MemoryAllocateInfo}. See
@ref Vk-Memory-allocation "Memory allocation" for usage information.
*/
class MAGNUM_VK_EXPORT MemoryAllocateInfo {
    public:
        /** @todo Flags, in VkMemoryAllocateFlagsInfo (1.1) */

        /**
         * @brief Constructor
         * @param size      Allocation size in bytes
         * @param memory    Memory index, smaller than
         *      @ref DeviceProperties::memoryCount()
         *
         * The following @type_vk{MemoryAllocateInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `allocationSize` to @p size
         * -    `memoryTypeIndex` to @p memory
         *
         * @see @ref DeviceProperties::pickMemory()
         */
        explicit MemoryAllocateInfo(UnsignedLong size, UnsignedInt memory);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit MemoryAllocateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit MemoryAllocateInfo(const VkMemoryAllocateInfo& info);

        /** @brief Underlying @type_vk{MemoryAllocateInfo} structure */
        VkMemoryAllocateInfo& operator*() { return _info; }
        /** @overload */
        const VkMemoryAllocateInfo& operator*() const { return _info; }
        /** @overload */
        VkMemoryAllocateInfo* operator->() { return &_info; }
        /** @overload */
        const VkMemoryAllocateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkMemoryAllocateInfo*() const { return &_info; }

    private:
        VkMemoryAllocateInfo _info;
};

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/Memory.h"

#endif
