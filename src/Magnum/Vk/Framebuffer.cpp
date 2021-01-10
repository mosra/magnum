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

#include "Framebuffer.h"
#include "FramebufferCreateInfo.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Reference.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/ImageView.h"

namespace Magnum { namespace Vk {

struct FramebufferCreateInfo::State {
    Containers::Array<VkImageView> attachments;
};

FramebufferCreateInfo::FramebufferCreateInfo(const VkRenderPass renderPass, const Containers::ArrayView<const Containers::Reference<ImageView>> attachments, const Vector3i& size, const Flags flags): _info{}, _state{Containers::InPlaceInit} {
    _info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    _info.flags = VkFramebufferCreateFlags(flags);
    _info.renderPass = renderPass;
    _info.width = size.x();
    _info.height = size.y();
    _info.layers = size.z();

    /* Allocate an array with VkImageView handles */
    /** @todo combine the state and array allocations into one (could use
        ArrayTuple, however that one includes ArrayView so not much better than
        including Array directly in the header which I don't want to do) */
    _state->attachments = Containers::Array<VkImageView>{Containers::NoInit, attachments.size()};
    for(std::size_t i = 0; i != attachments.size(); ++i)
        _state->attachments[i] = *attachments[i];
    _info.attachmentCount = _state->attachments.size();
    _info.pAttachments = _state->attachments;
}

FramebufferCreateInfo::FramebufferCreateInfo(const VkRenderPass renderPass, const std::initializer_list<Containers::Reference<ImageView>> attachments, const Vector3i& size, const Flags flags): FramebufferCreateInfo{renderPass, Containers::arrayView(attachments), size, flags} {}

FramebufferCreateInfo::FramebufferCreateInfo(const VkRenderPass renderPass, const Containers::ArrayView<const Containers::Reference<ImageView>> attachments, const Vector2i& size, const Flags flags): FramebufferCreateInfo{renderPass, attachments, {size, 1}, flags} {}

FramebufferCreateInfo::FramebufferCreateInfo(const VkRenderPass renderPass, const std::initializer_list<Containers::Reference<ImageView>> attachments, const Vector2i& size, const Flags flags): FramebufferCreateInfo{renderPass, Containers::arrayView(attachments), size, flags} {}

FramebufferCreateInfo::FramebufferCreateInfo(NoInitT) noexcept {}

FramebufferCreateInfo::FramebufferCreateInfo(const VkFramebufferCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

FramebufferCreateInfo::FramebufferCreateInfo(FramebufferCreateInfo&& other) noexcept:
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(other._info),
    _state{std::move(other._state)}
{
    /* Ensure the previous instance doesn't reference state that's now ours */
    other._info.attachmentCount = 0;
    other._info.pAttachments = nullptr;
}

FramebufferCreateInfo::~FramebufferCreateInfo() = default;

FramebufferCreateInfo& FramebufferCreateInfo::operator=(FramebufferCreateInfo&& other) noexcept {
    using std::swap;
    swap(other._info, _info);
    swap(other._state, _state);
    return *this;
}

Framebuffer Framebuffer::wrap(Device& device, const VkFramebuffer handle, const Vector3i& size, const HandleFlags flags) {
    Framebuffer out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;

    /* See the _size member for more information */
    CORRADE_INTERNAL_ASSERT((size <= Vector3i{0xffff}).all());
    out._size[0] = size.x();
    out._size[1] = size.y();
    out._size[2] = size.z();

    return out;
}

Framebuffer::Framebuffer(Device& device, const FramebufferCreateInfo& info): _device{&device}, _flags{HandleFlag::DestroyOnDestruction} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateFramebuffer(device, info, nullptr, &_handle));

    /* See the _size member for more information */
    CORRADE_INTERNAL_ASSERT(info->width <= 0xffff && info->height <= 0xffff && info->layers <= 0xffff);
    _size[0] = info->width;
    _size[1] = info->height;
    _size[2] = info->layers;
}

Framebuffer::Framebuffer(NoCreateT): _device{}, _handle{}, _size{} {}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags}, _size{other._size[0], other._size[1], other._size[2]} {
    other._handle = {};
}

Framebuffer::~Framebuffer() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroyFramebuffer(*_device, _handle, nullptr);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    swap(other._size, _size);
    return *this;
}

Vector3i Framebuffer::size() const {
    return {_size[0], _size[1], _size[2]};
}

VkFramebuffer Framebuffer::release() {
    const VkFramebuffer handle = _handle;
    _handle = {};
    return handle;
}

}}
