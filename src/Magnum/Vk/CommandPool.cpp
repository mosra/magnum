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

#include "CommandPool.h"

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Handle.h"

namespace Magnum { namespace Vk {

CommandPoolCreateInfo::CommandPoolCreateInfo(const UnsignedInt queueFamilyIndex, const Flags flags): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    _info.flags = VkCommandPoolCreateFlags(flags);
    _info.queueFamilyIndex = queueFamilyIndex;
}

CommandPoolCreateInfo::CommandPoolCreateInfo(NoInitT) noexcept {}

CommandPoolCreateInfo::CommandPoolCreateInfo(const VkCommandPoolCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

CommandPool CommandPool::wrap(Device& device, const VkCommandPool handle, const HandleFlags flags) {
    CommandPool out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;
    return out;
}

CommandPool::CommandPool(Device& device, const CommandPoolCreateInfo& info): _device{&device}, _flags{HandleFlag::DestroyOnDestruction} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateCommandPool(device, info, nullptr, &_handle));
}

CommandPool::CommandPool(NoCreateT) noexcept: _device{}, _handle{} {}

CommandPool::CommandPool(CommandPool&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags} {
    other._handle = {};
}

CommandPool::~CommandPool() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroyCommandPool(*_device, _handle, nullptr);
}

CommandPool& CommandPool::operator=(CommandPool&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    return *this;
}

CommandBuffer CommandPool::allocate(const CommandBufferLevel level) {
    CommandBuffer out{NoCreate};
    out._device = _device;
    out._pool = _handle;
    out._flags = HandleFlag::DestroyOnDestruction;

    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = _handle;
    info.commandBufferCount = 1;
    info.level = VkCommandBufferLevel(level);
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS((**_device).AllocateCommandBuffers(*_device, &info, &out._handle));

    return out;
}

void CommandPool::reset(const CommandPoolResetFlags flags) {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS((**_device).ResetCommandPool(*_device, _handle, VkCommandPoolResetFlags(flags)));
}

VkCommandPool CommandPool::release() {
    const VkCommandPool handle = _handle;
    _handle = {};
    return handle;
}

}}
