#ifndef Magnum_Vk_Buffer_h
#define Magnum_Vk_Buffer_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/DeviceMemory.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

enum class BufferUsage: UnsignedInt {
    TransferSrc = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    TransferDst = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    UniformTexelBuffer = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
    StorageTexelBuffer = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
    UniformBuffer = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    StorageBuffer = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    IndexBuffer = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VertexBuffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    IndirectBuffer = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
};

typedef Containers::EnumSet<BufferUsage> BufferUsageFlags;

CORRADE_ENUMSET_OPERATORS(BufferUsageFlags)

class MAGNUM_VK_EXPORT Buffer {
    public:

        Buffer(Device& device, UnsignedInt size, BufferUsageFlags usage):
            _device{&device},
            _size{size}
        {
            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.pNext = nullptr;
            bufferInfo.size = size;
            bufferInfo.usage = VkBufferUsageFlags(usage);

            VkResult err = vkCreateBuffer(*_device, &bufferInfo, nullptr, &_buffer);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        Buffer(NoCreateT): _device{nullptr} {
        }

        /** @brief Copying is not allowed */
        Buffer(const Buffer&) = delete;

        /** @brief Move constructor */
        Buffer(Buffer&& other) {
            _device = other._device;
            other._device = nullptr;
            std::swap(_buffer, other._buffer);
            std::swap(_size, other._size);
        }

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyBuffer}
         */
        ~Buffer();

        /** @brief Copying is not allowed */
        Buffer& operator=(const Buffer&) = delete;

        /** @brief Move assignment is not allowed */
        Buffer& operator=(Buffer&& other) {
            std::swap(_device, other._device);
            std::swap(_buffer, other._buffer);
            std::swap(_size, other._size);

            return *this;
        }

        operator VkBuffer() const {
            return _buffer;
        }

        VkMemoryRequirements getMemoryRequirements() const {
            VkMemoryRequirements memReqs;
            vkGetBufferMemoryRequirements(*_device, _buffer, &memReqs);

            return memReqs;
        }

        UnsignedInt size() const {
            return _size;
        }

        Buffer& bindBufferMemory(const DeviceMemory& deviceMemory, UnsignedLong offset=0) {
            VkResult err = vkBindBufferMemory(*_device, _buffer, deviceMemory, offset);
            MAGNUM_VK_ASSERT_ERROR(err);

            return *this;
        }

        auto cmdCopyTo(Buffer& dest, std::initializer_list<VkBufferCopy> regions) {
            const VkBuffer source = _buffer;
            return [source, &dest, &regions](VkCommandBuffer cmdBuffer){
                vkCmdCopyBuffer(cmdBuffer, source, dest, UnsignedInt(regions.size()), std::vector<VkBufferCopy>(regions).data());
            };
        }

        auto cmdCopyTo(Image& dest, std::initializer_list<VkBufferImageCopy> regions) {
            const VkBuffer source = _buffer;
            return [source, &dest, &regions](VkCommandBuffer cmdBuffer){
                vkCmdCopyBufferToImage(cmdBuffer, source, dest, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, UnsignedInt(regions.size()), std::vector<VkBufferImageCopy>(regions).data());
            };
        }

        auto cmdFullCopyTo(Buffer& dest) {
            const VkBuffer source = _buffer;
            const UnsignedInt size = _size;
            return [source, &dest, size](VkCommandBuffer cmdBuffer){
                VkBufferCopy bufferCopy = {0, 0, size};
                vkCmdCopyBuffer(cmdBuffer, source, dest, 1, &bufferCopy);
            };
        }

        auto cmdFullCopyTo(Image& dest) {
            const VkBuffer source = _buffer;
            return [source, &dest](VkCommandBuffer cmdBuffer){
                VkBufferImageCopy copy{};
                copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                copy.imageSubresource.mipLevel = 0;
                copy.imageSubresource.baseArrayLayer = 0;
                copy.imageSubresource.layerCount = 1;
                copy.imageExtent = VkExtent3D(dest.extent());
                vkCmdCopyBufferToImage(cmdBuffer, source, dest, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
            };
        }

        VkDescriptorBufferInfo getDescriptor() {
            VkDescriptorBufferInfo descriptor;
            descriptor.buffer = _buffer;
            descriptor.offset = 0;
            descriptor.range = _size;

            return descriptor;
        }

        /**
         * @brief Update contents of device memory.
         * @return Reference to self (for method chaining)
         *
         * Will involve mapping and unmapping memory and possibly creating intermediate
         * staging buffers if the memory is not visible by the host.
         *
         * At the end of the method, the queue will be waited for so that temporary buffers
         * can be cleaned up.
         */
        Buffer& update(Queue& queue, CommandPool& pool, const void* sourceData, UnsignedLong size, UnsignedLong destOffset=0);

        /**
         * @brief Allocate memory matching requirements of this buffer and bind it
         * @param memProperty property for the allocated memory
         * @return the allocated and bound memory
         */
        std::unique_ptr<DeviceMemory> allocateDeviceMemory(Vk::MemoryProperty memProperty);

    private:
        Device* _device;
        VkBuffer _buffer;
        UnsignedInt _size;
};

}}

#endif
