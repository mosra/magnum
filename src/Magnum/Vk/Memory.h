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
 * @brief Class @ref Magnum::Vk::MemoryRequirements, @ref Magnum::Vk::MemoryAllocateInfo, @ref Magnum::Vk::Memory, enum @ref Magnum::Vk::MemoryFlag, enum set @ref Magnum::Vk::MemoryFlags
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/** @relates Memory
@brief Deleter for mapped memory
@m_since_latest

Deleter for the array returned from @ref Memory::map(). Calls
@fn_vk_keyword{UnmapMemory}.
*/
class MemoryMapDeleter;

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

        /** @brief Required memory size */
        UnsignedLong size() const {
            return _requirements.memoryRequirements.size;
        }

        /** @brief Required memory alignment */
        UnsignedLong alignment() const {
            return _requirements.memoryRequirements.alignment;
        }

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

Wraps a @type_vk_keyword{MemoryAllocateInfo}. See @ref Memory for usage
information.
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

/**
@brief Device memory
@m_since_latest

Wraps a @type_vk_keyword{DeviceMemory}.
*/
class MAGNUM_VK_EXPORT Memory {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device    Vulkan device the memory is allocated on
         * @param handle    The @type_vk{DeviceMemory} handle
         * @param size      Memory size
         * @param flags     Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * a memory allocated using a constructor, the Vulkan memory is by
         * default not freed on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static Memory wrap(Device& device, VkDeviceMemory handle, UnsignedLong size, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param device    Vulkan device to allocate the memory on
         * @param info      Memory allocation info
         *
         * @see @fn_vk_keyword{AllocateMemory}
         */
        explicit Memory(Device& device, const MemoryAllocateInfo& info);

        /**
         * @brief Construct without allocating the memory
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Memory(NoCreateT);

        /** @brief Copying is not allowed */
        Memory(const Memory&) = delete;

        /** @brief Move constructor */
        Memory(Memory&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Frees associated @type_vk{DeviceMemory} handle, unless the instance
         * was created using @ref wrap() without @ref HandleFlag::DestroyOnDestruction
         * specified.
         * @see @fn_vk_keyword{FreeMemory}, @ref release()
         */
        ~Memory();

        /** @brief Copying is not allowed */
        Memory& operator=(const Memory&) = delete;

        /** @brief Move assignment */
        Memory& operator=(Memory&& other) noexcept;

        /** @brief Underlying @type_vk{DeviceMemory} handle */
        VkDeviceMemory handle() { return _handle; }
        /** @overload */
        operator VkDeviceMemory() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /** @brief Memory allocation size */
        UnsignedLong size() const { return _size; }

        /**
         * @brief Map a memory range
         * @param offset    Byte offset
         * @param size      Memory size
         *
         * The returned array size is @p size and the deleter performs an
         * unmap. For this operation to work, the memory has to be allocated
         * with @ref MemoryFlag::HostVisible and the @p offset and @p size be
         * in bounds for @ref size().
         * @see @fn_vk_keyword{MapMemory}, @fn_vk{UnmapMemory}
         */
        Containers::Array<char, MemoryMapDeleter> map(UnsignedLong offset, UnsignedLong size);

        /**
         * @brief Map the whole memory
         *
         * Equivalent to calling @ref map(UnsignedLong, UnsignedLong) with
         * @cpp 0 @ce and @ref size().
         */
        Containers::Array<char, MemoryMapDeleter> map();

        /**
         * @brief Map a memory range read-only
         *
         * Like @ref map(UnsignedLong, UnsignedLong) but returning a
         * @cpp const @ce array. Currently Vulkan doesn't have any flags to
         * control read/write access, so apart from a different return type the
         * behavior is equivalent.
         */
        Containers::Array<const char, MemoryMapDeleter> mapRead(UnsignedLong offset, UnsignedLong size);

        /**
         * @brief Map the whole memory read-only
         *
         * Equivalent to calling @ref mapRead(UnsignedLong, UnsignedLong) with
         * @cpp 0 @ce and @ref size().
         */
        Containers::Array<const char, MemoryMapDeleter> mapRead();

        /**
         * @brief Release the underlying Vulkan memory
         *
         * Releases ownership of the Vulkan memory and returns its handle so
         * @fn_vk{FreeMemory} is not called on destruction. The internal state
         * is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkDeviceMemory release();

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkDeviceMemory _handle;
        HandleFlags _flags;
        UnsignedLong _size;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
class MAGNUM_VK_EXPORT MemoryMapDeleter {
    public:
        explicit MemoryMapDeleter(): _unmap{}, _device{}, _memory{} {}
        explicit MemoryMapDeleter(void(*unmap)(VkDevice, VkDeviceMemory), VkDevice device, VkDeviceMemory memory): _unmap{unmap}, _device{device}, _memory{memory} {}
        void operator()(const char*, std::size_t) {
            if(_unmap) _unmap(_device, _memory);
        }

    private:
        void(*_unmap)(VkDevice, VkDeviceMemory);
        VkDevice _device;
        VkDeviceMemory _memory;
};
#endif

}}

#endif
