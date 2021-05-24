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

#include "Fence.h"
#include "FenceCreateInfo.h"

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Result.h"

namespace Magnum { namespace Vk {

FenceCreateInfo::FenceCreateInfo(const Flags flags): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    _info.flags = VkFenceCreateFlags(flags);
}

FenceCreateInfo::FenceCreateInfo(NoInitT) noexcept {}

FenceCreateInfo::FenceCreateInfo(const VkFenceCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

Fence Fence::wrap(Device& device, const VkFence handle, const HandleFlags flags) {
    Fence out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;
    return out;
}

Fence::Fence(Device& device, const FenceCreateInfo& info): _device{&device}, _flags{HandleFlag::DestroyOnDestruction} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateFence(device, info, nullptr, &_handle));
}

Fence::Fence(Device& device): Fence{device, FenceCreateInfo{}} {}

Fence::Fence(NoCreateT): _device{}, _handle{} {}

Fence::Fence(Fence&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags} {
    other._handle = {};
}

Fence::~Fence() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroyFence(*_device, _handle, nullptr);
}

Fence& Fence::operator=(Fence&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    return *this;
}

bool Fence::status() {
    return MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR((**_device).GetFenceStatus(*_device, _handle), Result::NotReady) == Result::Success;
}

void Fence::reset() {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS((**_device).ResetFences(*_device, 1, &_handle));
}

bool Fence::wait(const std::chrono::nanoseconds timeout) {
    return MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR((**_device).WaitForFences(*_device, 1, &_handle, true, timeout.count()), Result::Timeout) == Result::Success;
}

void Fence::wait() {
    CORRADE_INTERNAL_ASSERT_OUTPUT(wait(std::chrono::nanoseconds{~std::uint64_t{}}));
}

VkFence Fence::release() {
    const VkFence handle = _handle;
    _handle = {};
    return handle;
}

}}
