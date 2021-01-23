#ifndef Magnum_Vk_Buffer_h
#define Magnum_Vk_Buffer_h
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
 * @brief Class @ref Magnum::Vk::Buffer
 * @m_since_latest
 */

#include <initializer_list>
#include <Corrade/Containers/ArrayTuple.h>
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
@brief Buffer
@m_since_latest

Wraps a @type_vk_keyword{Buffer} and its memory.

@section Vk-Buffer-creation Buffer creation

Pass a @ref BufferCreateInfo with desired usage and size to the @ref Buffer
constructor together with specifying @ref MemoryFlags for the allocation.

@snippet MagnumVk.cpp Buffer-creation

@attention At this point, a dedicated allocation is used, subsequently
    accessible through @ref dedicatedMemory(). This behavior may change in the
    future.

@subsection Vk-Buffer-creation-custom-allocation Custom memory allocation

Using @ref Buffer(Device&, const BufferCreateInfo&, NoAllocateT), the buffer
will be created without any memory bound. Buffer memory requirements can be
then queried using @ref memoryRequirements() and an allocated memory bound with
@ref bindMemory(). See @ref Memory for further details about memory allocation.

@snippet MagnumVk.cpp Buffer-creation-custom-allocation

Using @ref bindDedicatedMemory() instead of @ref bindMemory() will transfer
ownership of the @ref Memory to the buffer instance, making it subsequently
available through @ref dedicatedMemory(). This matches current behavior of the
@ref Buffer(Device&, const BufferCreateInfo&, MemoryFlags) constructor shown
above, except that you have more control over choosing and allocating the
memory.

@section Vk-Buffer-usage Buffer usage

@subsection Vk-Buffer-usage-fill Clearing / filling buffer data

The following snippet shows zero-filling the whole buffer using
@ref CommandBuffer::fillBuffer():

@snippet MagnumVk.cpp Buffer-usage-fill

@subsection Vk-Buffer-usage-copy Copying buffer data

Most common buffer copy operation is uploading vertex data from a host-visible
to device-local memory. This is the preferred workflow for static data over
using a host-visible memory directly, since it usually isn't the fastest for
device access.

The copy is done using @ref CommandBuffer::copyBuffer(). In most cases you'll
want to combine it with a @ref CommandBuffer::pipelineBarrier(PipelineStages, PipelineStages, Containers::ArrayView<const BufferMemoryBarrier>, DependencyFlags) "pipelineBarrier()"
after to make the memory visible for subsequent operations. The following
snippet shows populating a device-local vertex buffer from host-visible memory:

@snippet MagnumVk.cpp Buffer-usage-copy

It's also possible to copy data between buffers and images, see
@ref Vk-Image-usage-copy for examples.

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

        MAGNUM_VK_LOCAL static VkResult bindMemoryImplementationDefault(Device& device, UnsignedInt count, const VkBindBufferMemoryInfo* infos);
        MAGNUM_VK_LOCAL static VkResult bindMemoryImplementationKHR(Device& device, UnsignedInt count, const VkBindBufferMemoryInfo* infos);
        MAGNUM_VK_LOCAL static VkResult bindMemoryImplementation11(Device& device, UnsignedInt count, const VkBindBufferMemoryInfo* infos);

        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkBuffer _handle;
        HandleFlags _flags;
        Memory _dedicatedMemory;
};

/**
@brief Buffer copy region
@m_since_latest

Wraps a @type_vk_keyword{BufferCopy2KHR}. This class is subsequently passed to
a @ref CopyBufferInfo and then used in @ref CommandBuffer::copyBuffer(). See
@ref Vk-Buffer-usage-copy for usage information and examples.

@section Vk-BufferCopy-compatibility Compatibility with VkBufferCopy

While the class operates on the @type_vk{BufferCopy2KHR} structure that's
provided by the @vk_extension{KHR,copy_commands2} extension, conversion from
and to @type_vk{BufferCopy} is provided to some extent --- you can create a
@ref BufferCopy from it, call various methods on the instance and then get a
@type_vk{BufferCopy} back again using @ref vkBufferCopy().

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{BufferCopy2KHR} fields and then perform the conversion instead of
editing the resulting @type_vk{BufferCopy}, as additional safety checks may be
done during the conversion to ensure no information is lost.

@see @ref ImageCopy, @ref BufferImageCopy
*/
class MAGNUM_VK_EXPORT BufferCopy {
    public:
        /**
         * @brief Constructor
         * @param sourceOffset      Source buffer offset in bytes
         * @param destinationOffset Destination buffer offset in bytes
         * @param size              Size to copy in bytes
         *
         * The following @type_vk{BufferCopy2KHR} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `srcOffset` to @p sourceOffset
         * -    `dstOffset` to @p destinationOffset
         * -    `size`
         */
        /*implicit*/ BufferCopy(UnsignedLong sourceOffset, UnsignedLong destinationOffset, UnsignedLong size);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit BufferCopy(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit BufferCopy(const VkBufferCopy2KHR& copy);

        /**
         * @brief Construct from a @type_vk{BufferCopy}
         *
         * Compared to the above, fills the common subset of
         * @type_vk{BufferCopy2KHR}, sets `sType` and zero-fills `pNext`.
         * @see @ref vkBufferCopy()
         */
        explicit BufferCopy(const VkBufferCopy& copy);

        /**
         * @brief Corresponding @type_vk{BufferCopy} structure
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support the @vk_extension{KHR,copy_commands2} extensions. See
         * @ref Vk-BufferCopy-compatibility for more information.
         * @see @ref BufferCopy(const VkBufferCopy&),
         *      @ref CopyBufferInfo::vkBufferCopies(),
         *      @ref CopyImageInfo::vkImageCopies(),
         *      @ref ImageCopy::vkImageCopy(),
         *      @ref CopyImageToBufferInfo::vkBufferImageCopies(),
         *      @ref CopyBufferToImageInfo::vkBufferImageCopies(),
         *      @ref BufferImageCopy::vkBufferImageCopy()
         */
        VkBufferCopy vkBufferCopy() const;

        /** @brief Underlying @type_vk{BufferCopy2KHR} structure */
        VkBufferCopy2KHR& operator*() { return _copy; }
        /** @overload */
        const VkBufferCopy2KHR& operator*() const { return _copy; }
        /** @overload */
        VkBufferCopy2KHR* operator->() { return &_copy; }
        /** @overload */
        const VkBufferCopy2KHR* operator->() const { return &_copy; }
        /** @overload */
        operator const VkBufferCopy2KHR*() const { return &_copy; }

        /**
         * @overload
         *
         * The class is implicitly convertible to a reference in addition to
         * a pointer because the type is commonly used in arrays as well, which
         * would be annoying to do with a pointer conversion.
         */
        operator const VkBufferCopy2KHR&() const { return _copy; }

    private:
        VkBufferCopy2KHR _copy;
};

/**
@brief Buffer copy command
@m_since_latest

Wraps a @type_vk_keyword{CopyBufferInfo2KHR}. This class is subsequently used
in @ref CommandBuffer::copyBuffer(). See @ref Vk-Buffer-usage-copy for more
information.

@section Vk-CopyBufferInfo-compatibility Compatibility with vkCmdCopyBuffer()

While the class operates on the @type_vk{CopyBufferInfo2KHR} structure that's
provided by the @vk_extension{KHR,copy_commands2} extension, conversion from
and to the set of parameters accepted by @fn_vk{CmdCopyBuffer} is provided to
some extent --- you can create @ref BufferCopy instances out of
@type_vk{BufferCopy} structures, pass them together with the rest to
@ref CopyBufferInfo and then get a @type_vk{BufferCopy} list back again using
@ref vkBufferCopies().

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{CopyBufferInfo2KHR} fields and then perform the conversion instead of
editing the resulting @type_vk{BufferCopy} list, as additional safety checks
may be done during the conversion to ensure no information is lost.

@see @ref CopyImageInfo, @ref CopyBufferToImageInfo, @ref CopyImageToBufferInfo
*/
class MAGNUM_VK_EXPORT CopyBufferInfo {
    public:
        /**
         * @brief Constructor
         * @param source        Source @ref Buffer or a raw Vulkan buffer
         *      handle. Expected to have been created with
         *      @ref BufferUsage::TransferSource.
         * @param destination   Destination @ref Buffer or a raw Vulkan buffer
         *      handle. Expected to have been created with
         *      @ref BufferUsage::TransferDestination.
         * @param regions       Regions to copy. There has to be at least one.
         */
        /*implicit*/ CopyBufferInfo(VkBuffer source, VkBuffer destination, Containers::ArrayView<const BufferCopy> regions);
        /** @overload */
        /*implicit*/ CopyBufferInfo(VkBuffer source, VkBuffer destination, std::initializer_list<BufferCopy> regions);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit CopyBufferInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit CopyBufferInfo(const VkCopyBufferInfo2KHR& info);

        /**
         * @brief Corresponding @type_vk{BufferCopy} structures
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support the @vk_extension{KHR,copy_commands2} extension. See
         * @ref Vk-CopyBufferInfo-compatibility for more information.
         * @see @ref BufferImageCopy::vkBufferImageCopy(),
         *      @ref BufferCopy::vkBufferCopy(),
         *      @ref CopyImageInfo::vkImageCopies(),
         *      @ref ImageCopy::vkImageCopy(),
         *      @ref CopyImageToBufferInfo::vkBufferImageCopies(),
         *      @ref BufferImageCopy::vkBufferImageCopy()
         */
        Containers::Array<VkBufferCopy> vkBufferCopies() const;

        /** @brief Underlying @type_vk{CopyBufferInfo2KHR} structure */
        VkCopyBufferInfo2KHR& operator*() { return _info; }
        /** @overload */
        const VkCopyBufferInfo2KHR& operator*() const { return _info; }
        /** @overload */
        VkCopyBufferInfo2KHR* operator->() { return &_info; }
        /** @overload */
        const VkCopyBufferInfo2KHR* operator->() const { return &_info; }
        /** @overload */
        operator const VkCopyBufferInfo2KHR*() const { return &_info; }

    private:
        VkCopyBufferInfo2KHR _info;
        Containers::ArrayTuple _data;
};

}}

#endif
