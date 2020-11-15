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

#include "CommandBuffer.h"

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Handle.h"

namespace Magnum { namespace Vk {

CommandBuffer CommandBuffer::wrap(Device& device, const VkCommandPool pool, const VkCommandBuffer handle, const HandleFlags flags) {
    CommandBuffer out{NoCreate};
    out._device = &device;
    out._pool = pool;
    out._handle = handle;
    out._flags = flags;
    return out;
}

CommandBuffer::CommandBuffer(NoCreateT) noexcept: _device{}, _pool{}, _handle{} {}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept: _device{other._device}, _pool{other._pool}, _handle{other._handle}, _flags{other._flags} {
    other._handle = nullptr;
}

CommandBuffer::~CommandBuffer() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).FreeCommandBuffers(*_device, _pool, 1, &_handle);
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._pool, _pool);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    return *this;
}

void CommandBuffer::reset(const CommandBufferResetFlags flags) {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS((**_device).ResetCommandBuffer(_handle, VkCommandBufferResetFlags(flags)));
}

VkCommandBuffer CommandBuffer::release() {
    const VkCommandBuffer handle = _handle;
    _handle = nullptr;
    return handle;
}

}}
