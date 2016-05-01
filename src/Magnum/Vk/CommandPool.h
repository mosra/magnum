#ifndef Magnum_Vk_CommandPool_h
#define Magnum_Vk_CommandPool_h
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
 * @brief Class @ref Magnum::Vk::CommandPool
 */

#include "Magnum/Magnum.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/PhysicalDevice.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

class MAGNUM_VK_EXPORT CommandPool {
    friend class CommandBuffer;

    public:

        /** @brief Copying is not allowed */
        CommandPool(const Context&) = delete;

        /** @brief Move constructor */
        CommandPool(CommandPool&& other);

        CommandPool(Device& device, QueueFamily family): _device{device} {
            const VkCommandPoolCreateInfo cmdPoolInfo = {
                VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr,
                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                _device.physicalDevice().getQueueFamilyIndex(family)};
            vkCreateCommandPool(_device, &cmdPoolInfo, nullptr, &_pool);
        }

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyCommandPool}
         */
        ~CommandPool();

        /** @brief Copying is not allowed */
        CommandPool& operator=(const CommandPool&) = delete;

        /** @brief Move assignment is not allowed */
        CommandPool& operator=(CommandPool&&) = delete;

        VkCommandPool vkCommandPool() {
            return _pool;
        }

        std::unique_ptr<CommandBuffer> allocateCommandBuffer(CommandBuffer::Level level);

    private:
        VkCommandPool _pool;
        Device& _device;

        void freeCommandBuffer(CommandBuffer* buffer) {
            VkCommandBuffer buffers = *buffer;
            vkFreeCommandBuffers(_device.vkDevice(), _pool, 1, &buffers);
        }
};

}}

#endif
