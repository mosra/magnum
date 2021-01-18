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

#include "Queue.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Fence.h"

namespace Magnum { namespace Vk {

Queue Queue::wrap(Device& device, VkQueue handle) {
    Queue out{NoCreate};
    out._device = &device;
    out._handle = handle;
    return out;
}

Queue::Queue(NoCreateT): _device{}, _handle{} {}

Queue::Queue(Queue&& other) noexcept: _device{other._device}, _handle{other._handle} {
    other._handle = {};
}

Queue::~Queue() = default;

Queue& Queue::operator=(Queue&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    return *this;
}

void Queue::submit(const Containers::ArrayView<const Containers::Reference<const SubmitInfo>> infos, const VkFence fence) {
    /** @todo use DynamicArray here. I also thought about taking an ArrayView
        of VkSubmitInfo structures directly and thus avoiding this whole hell,
        but that feels kinda inconsistent in the public API (as everywhere else
        we take only the structure wrappers, as opposed to handles which *are*
        taken raw); plus once the SubmitInfo gets more complex it may not be
        so easy to verify we don't use unsupported functionality / backport on
        the raw structure (like done in RenderPassCreateInfo, e.g.) */

    /* If we have just one item, we don't need to allocate. This will become
       obsolete once DynamicArray can handle both cases efficiently */
    if(infos.size() == 1) {
        MAGNUM_VK_INTERNAL_ASSERT_SUCCESS((**_device).QueueSubmit(_handle, 1, *infos[0], fence));
        return;
    }

    Containers::Array<VkSubmitInfo> vkInfos{Containers::NoInit, infos.size()};
    for(std::size_t i = 0; i != infos.size(); ++i)
        vkInfos[i] = *infos[i];

    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS((**_device).QueueSubmit(_handle, vkInfos.size(), vkInfos, fence));
}

void Queue::submit(std::initializer_list<Containers::Reference<const SubmitInfo>> infos, VkFence fence) {
    return submit(Containers::arrayView(infos), fence);
}

Fence Queue::submit(const Containers::ArrayView<const Containers::Reference<const SubmitInfo>> infos) {
    Vk::Fence fence{*_device};
    submit(infos, fence);
    return fence;
}

Fence Queue::submit(const std::initializer_list<Containers::Reference<const SubmitInfo>> infos) {
    return submit(Containers::arrayView(infos));
}

struct SubmitInfo::State {
    Containers::Array<VkCommandBuffer> commandBuffers;
};

SubmitInfo::SubmitInfo(): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
}

SubmitInfo::SubmitInfo(NoInitT) noexcept {}

SubmitInfo::SubmitInfo(const VkSubmitInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

SubmitInfo::SubmitInfo(SubmitInfo&& other) noexcept:
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(other._info),
    _state{std::move(other._state)}
{
    /* Ensure the previous instance doesn't reference state that's now ours */
    /** @todo this is now more like a destructible move, do it more selectively
        and clear only what's really ours and not external? */
    other._info.pNext = nullptr;
    other._info.waitSemaphoreCount = 0;
    other._info.pWaitSemaphores = nullptr;
    other._info.pWaitDstStageMask = nullptr;
    other._info.commandBufferCount = 0;
    other._info.pCommandBuffers = nullptr;
    other._info.signalSemaphoreCount = 0;
    other._info.pSignalSemaphores = nullptr;
}

SubmitInfo::~SubmitInfo() = default;

SubmitInfo& SubmitInfo::operator=(SubmitInfo&& other) noexcept {
    using std::swap;
    swap(other._info, _info);
    swap(other._state, _state);
    return *this;
}

SubmitInfo& SubmitInfo::setCommandBuffers(const Containers::ArrayView<const VkCommandBuffer> buffers) {
    if(!_state) _state.emplace();

    _state->commandBuffers = Containers::Array<VkCommandBuffer>{Containers::NoInit, buffers.size()};
    Utility::copy(buffers, _state->commandBuffers);
    _info.commandBufferCount = _state->commandBuffers.size();
    _info.pCommandBuffers = _state->commandBuffers;
    return *this;
}

SubmitInfo& SubmitInfo::setCommandBuffers(const std::initializer_list<VkCommandBuffer> buffers) {
    return setCommandBuffers(Containers::arrayView(buffers));
}

}}
