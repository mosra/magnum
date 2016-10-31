#ifndef Magnum_Vk_DeviceMemory_h
#define Magnum_Vk_DeviceMemory_h
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
 * @brief Class @ref Magnum::Vk::DeviceMemory
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Buffer.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

class MAGNUM_VK_EXPORT DeviceMemory {
    public:

        DeviceMemory(Device& device, UnsignedLong size, UnsignedInt memoryTypeIndex):
                     _device{device}
        {
            _memAlloc = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                nullptr,
                size,
                memoryTypeIndex
            };

            VkResult err = vkAllocateMemory(_device, &_memAlloc, nullptr, &_deviceMemory);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        DeviceMemory(Device& device, UnsignedLong size, UnsignedInt memoryTypeIndex, Vk::Buffer& buffer):
                     _device{device}
        {
            VkDedicatedAllocationMemoryAllocateInfoNV dedicatedInfo{
                VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_MEMORY_ALLOCATE_INFO_NV,
                    nullptr, VK_NULL_HANDLE, buffer
            };
            _memAlloc = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                &dedicatedInfo,
                size,
                memoryTypeIndex
            };

            VkResult err = vkAllocateMemory(_device, &_memAlloc, nullptr, &_deviceMemory);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        DeviceMemory(Device& device, UnsignedLong size, UnsignedInt memoryTypeIndex, Vk::Image& image):
                     _device{device}
        {
            VkDedicatedAllocationMemoryAllocateInfoNV dedicatedInfo{
                VK_STRUCTURE_TYPE_DEDICATED_ALLOCATION_MEMORY_ALLOCATE_INFO_NV,
                    nullptr, image, VK_NULL_HANDLE
            };
            _memAlloc = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                &dedicatedInfo,
                size,
                memoryTypeIndex
            };

            VkResult err = vkAllocateMemory(_device, &_memAlloc, nullptr, &_deviceMemory);
            MAGNUM_VK_ASSERT_ERROR(err);
        }
        /** @brief Copying is not allowed */
        DeviceMemory(const DeviceMemory&) = delete;

        /** @brief Move constructor */
        DeviceMemory(DeviceMemory&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyDeviceMemory}
         */
        ~DeviceMemory();

        /** @brief Copying is not allowed */
        DeviceMemory& operator=(const DeviceMemory&) = delete;

        /** @brief Move assignment is not allowed */
        DeviceMemory& operator=(DeviceMemory&&) = delete;

        operator VkDeviceMemory() const {
            return _deviceMemory;
        }

        Containers::ArrayView<char> map() {
            return map(0, _memAlloc.allocationSize);
        }

        Containers::ArrayView<char> map(VkDeviceSize offset, VkDeviceSize size) {
            // TODO: Memory map flags (== 0)
            void* data;
            VkResult err = vkMapMemory(_device, _deviceMemory, offset, size, 0, &data);
            MAGNUM_VK_ASSERT_ERROR(err);

            return Containers::ArrayView<char>(static_cast<char*>(data), size);
        }

        DeviceMemory& unmap() {
            vkUnmapMemory(_device, _deviceMemory);
            return *this;
        }

    private:
        Device& _device;
        VkDeviceMemory _deviceMemory;
        VkMemoryAllocateInfo _memAlloc;
};

}}

#endif
