#ifndef Magnum_Vk_Buffer_h
#define Magnum_Vk_Buffer_h
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
 * @brief Class @ref Magnum::Vk::BufferCreateInfo, @ref Magnum::Vk::Buffer, enum @ref Magnum::Vk::BufferUsage, enum set @ref Magnum::Vk::BufferUsages
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Memory.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation { struct DeviceState; }

/**
@brief Buffer usage
@m_since_latest

Wraps a @type_vk_keyword{BufferUsageFlagBits}.
@see @ref BufferUsages, @ref BufferCreateInfo
@m_enum_values_as_keywords
*/
enum class BufferUsage: UnsignedInt {
    /** Source of a transfer command */
    TransferSource = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,

    /** Destination of a transfer command */
    TransferDestination = VK_BUFFER_USAGE_TRANSFER_DST_BIT,

    /** Suitable for creating a uniform texel buffer view */
    UniformTexelBuffer = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,

    /** Suitable for creating a storage texel buffer view */
    StorageTexelBuffer = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,

    /** Suitable for a uniform buffer */
    UniformBuffer = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,

    /** Suitable for a storage buffer */
    StorageBuffer = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,

    /** Suitable for an index buffer */
    IndexBuffer = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,

    /** Suitable for a vertex buffer */
    VertexBuffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,

    /** Suitable for a indirect draw buffer */
    IndirectBuffer = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,

    /** @todo VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 1.2 */
};

/**
@brief Buffer usages
@m_since_latest

Type-safe wrapper for @type_vk_keyword{BufferUsageFlags}.
@see @ref BufferCreateInfo
*/
typedef Containers::EnumSet<BufferUsage> BufferUsages;

CORRADE_ENUMSET_OPERATORS(BufferUsages)

/**
@brief Buffer creation info
@m_since_latest

Wraps a @type_vk_keyword{BufferCreateInfo}. See @ref Buffer for usage
information.
*/
class MAGNUM_VK_EXPORT BufferCreateInfo {
    public:
        /**
         * @brief Buffer creation flag
         *
         * Wraps @type_vk_keyword{BufferCreateFlagBits}.
         * @see @ref Flags, @ref BufferCreateInfo()
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /** @todo sparse binding, protected ... */
        };

        /**
         * @brief Buffer creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{BufferCreateFlags}.
         * @see @ref BufferCreateInfo()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param usages    Desired buffer usage. At least one flag is
         *      required.
         * @param size      Buffer size
         * @param flags     Buffer creation flags
         *
         * The following @type_vk{BufferCreateInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `size`
         * -    `usage` to @p usages
         * -    `sharingMode` to @val_vk{SHARING_MODE_EXCLUSIVE,SharingMode}
         */
        explicit BufferCreateInfo(BufferUsages usages, UnsignedLong size, Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit BufferCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit BufferCreateInfo(const VkBufferCreateInfo& info);

        /** @brief Underlying @type_vk{BufferCreateInfo} structure */
        VkBufferCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkBufferCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkBufferCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkBufferCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkBufferCreateInfo*() const { return &_info; }

    private:
        VkBufferCreateInfo _info;
};

CORRADE_ENUMSET_OPERATORS(BufferCreateInfo::Flags)

/**
@brief Buffer
@m_since_latest

Wraps a @type_vk_keyword{Buffer} and its memory.

@section Vk-Buffer-usage Basic usage

Pass a @ref BufferCreateInfo with desired usage and size to the @ref Buffer
constructor together with specifying @ref MemoryFlags for the allocation.

@snippet MagnumVk.cpp Buffer-usage

@attention At this point, a dedicated allocation is used, subsequently
    accessible through @ref dedicatedMemory(). This behavior may change in the
    future.

@section Vk-Buffer-usage-custom-allocation Custom memory allocation

Using @ref Buffer(Device&, const BufferCreateInfo&, NoAllocateT), the buffer
will be created without any memory bound. Buffer memory requirements can be
then queried using @ref memoryRequirements() and an allocated memory bound with
@ref bindMemory(). See @ref Memory for further details about memory allocation.

@snippet MagnumVk.cpp Buffer-usage-custom-allocation

Using @ref bindDedicatedMemory() instead of @ref bindMemory() will transfer
ownership of the @ref Memory to the buffer instance, making it subsequently
available through @ref dedicatedMemory(). This matches current behavior of the
@ref Buffer(Device&, const BufferCreateInfo&, MemoryFlags) constructor shown
above, except that you have more control over choosing and allocating the
memory.

@see @ref Image
*/
class MAGNUM_VK_EXPORT Buffer {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device    Vulkan device the buffer is created on
         * @param handle    The @type_vk{Buffer} handle
         * @param flags     Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * a buffer created using a constructor, the Vulkan buffer is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static Buffer wrap(Device& device, VkBuffer handle, HandleFlags flags = {});

        /**
         * @brief Construct a buffer without allocating
         * @param device    Vulkan device to create the buffer on
         * @param info      Buffer creation info
         *
         * @see @ref Buffer(Device&, const BufferCreateInfo&, MemoryFlags),
         *      @fn_vk_keyword{CreateBuffer}
         */
        explicit Buffer(Device& device, const BufferCreateInfo& info, NoAllocateT);

        /**
         * @brief Construct a buffer
         * @param device        Vulkan device to create the buffer on
         * @param info          Buffer creation info
         * @param memoryFlags   Memory allocation flags
         *
         * Compared to @ref Buffer(Device&, const BufferCreateInfo&, NoAllocateT)
         * allocates a memory satisfying @p memoryFlags as well.
         *
         * @attention At this point, a dedicated allocation is used,
         *      subsequently accessible through @ref dedicatedMemory(). This
         *      behavior may change in the future.
         */
        explicit Buffer(Device& device, const BufferCreateInfo& info, MemoryFlags memoryFlags);

        /**
         * @brief Construct without creating the buffer
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Buffer(NoCreateT);

        /** @brief Copying is not allowed */
        Buffer(const Buffer&) = delete;

        /** @brief Move constructor */
        Buffer(Buffer&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{Buffer} handle, unless the instance was
         * created using @ref wrap() without
         * @ref HandleFlag::DestroyOnDestruction specified.
         * @see @fn_vk_keyword{DestroyBuffer}, @ref release()
         */
        ~Buffer();

        /** @brief Copying is not allowed */
        Buffer& operator=(const Buffer&) = delete;

        /** @brief Move assignment */
        Buffer& operator=(Buffer&& other) noexcept;

        /** @brief Underlying @type_vk{Buffer} handle */
        VkBuffer handle() { return _handle; }
        /** @overload */
        operator VkBuffer() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Buffer memory requirements
         *
         * @see @ref bindMemory(), @fn_vk_keyword{GetBufferMemoryRequirements2},
         *      @fn_vk_keyword{GetBufferMemoryRequirements}
         */
        MemoryRequirements memoryRequirements() const;

        /**
         * @brief Bind buffer memory
         *
         * Assumes that @p memory type, the amount of @p memory at @p offset
         * and @p offset alignment corresponds to buffer memory requirements.
         * @see @ref memoryRequirements(), @ref bindDedicatedMemory(),
         *      @fn_vk_keyword{BindBufferMemory2},
         *      @fn_vk_keyword{BindBufferMemory}
         */
        void bindMemory(Memory& memory, UnsignedLong offset);

        /**
         * @brief Bind a dedicated buffer memory
         *
         * Equivalent to @ref bindMemory() with @p offset set to @cpp 0 @ce,
         * with the additional effect that @p memory ownership transfers to the
         * buffer and is then available through @ref dedicatedMemory().
         */
        void bindDedicatedMemory(Memory&& memory);

        /**
         * @brief Whether the buffer has a dedicated memory
         *
         * Returns @cpp true @ce if the buffer memory was bound using
         * @ref bindDedicatedMemory(), @cpp false @ce otherwise.
         * @see @ref dedicatedMemory()
         */
        bool hasDedicatedMemory() const;

        /**
         * @brief Dedicated buffer memory
         *
         * Expects that the buffer has a dedicated memory.
         * @see @ref hasDedicatedMemory()
         */
        Memory& dedicatedMemory();

        /**
         * @brief Release the underlying Vulkan buffer
         *
         * Releases ownership of the Vulkan buffer and returns its handle so
         * @fn_vk{DestroyBuffer} is not called on destruction. The internal
         * state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkBuffer release();

    private:
        friend Implementation::DeviceState;

        MAGNUM_VK_LOCAL static void getMemoryRequirementsImplementationDefault(Device& device, const VkBufferMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements);
        MAGNUM_VK_LOCAL static void getMemoryRequirementsImplementationKHR(Device& device, const VkBufferMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements);
        MAGNUM_VK_LOCAL static void getMemoryRequirementsImplementation11(Device& device, const VkBufferMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements);

        MAGNUM_VK_LOCAL static void bindMemoryImplementationDefault(Device& device, UnsignedInt count, const VkBindBufferMemoryInfo* infos);
        MAGNUM_VK_LOCAL static void bindMemoryImplementationKHR(Device& device, UnsignedInt count, const VkBindBufferMemoryInfo* infos);
        MAGNUM_VK_LOCAL static void bindMemoryImplementation11(Device& device, UnsignedInt count, const VkBindBufferMemoryInfo* infos);

        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkBuffer _handle;
        HandleFlags _flags;
        Memory _dedicatedMemory;
};

}}

#endif
