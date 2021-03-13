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

#include "DescriptorPool.h"
#include "DescriptorPoolCreateInfo.h"

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/DescriptorType.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Result.h"

namespace Magnum { namespace Vk {

DescriptorPoolCreateInfo::DescriptorPoolCreateInfo(const UnsignedInt maxSets, const Containers::ArrayView<const std::pair<DescriptorType, UnsignedInt>> poolSizes, const Flags flags): _info{} {
    CORRADE_ASSERT(maxSets,
        "Vk::DescriptorPoolCreateInfo: there has to be at least one set", );
    CORRADE_ASSERT(poolSizes,
        "Vk::DescriptorPoolCreateInfo: there has to be at least one pool", );

    Containers::ArrayView<VkDescriptorPoolSize> poolSizesCopy;
    _data = Containers::ArrayTuple{
        {Containers::NoInit, poolSizes.size(), poolSizesCopy}
    };
    for(std::size_t i = 0; i != poolSizes.size(); ++i) {
        CORRADE_ASSERT(poolSizes[i].second,
            "Vk::DescriptorPoolCreateInfo: pool" << i << "of" << poolSizes[i].first << "has no descriptors", );
        poolSizesCopy[i].type = VkDescriptorType(poolSizes[i].first);
        poolSizesCopy[i].descriptorCount = poolSizes[i].second;
    }

    _info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    _info.flags = VkDescriptorPoolCreateFlags(flags);
    _info.maxSets = maxSets;
    _info.poolSizeCount = poolSizesCopy.size();
    _info.pPoolSizes = poolSizesCopy;
}

DescriptorPoolCreateInfo::DescriptorPoolCreateInfo(const UnsignedInt maxSets, const std::initializer_list<std::pair<DescriptorType, UnsignedInt>> poolSizes, const Flags flags): DescriptorPoolCreateInfo{maxSets, Containers::arrayView(poolSizes), flags} {}

DescriptorPoolCreateInfo::DescriptorPoolCreateInfo(NoInitT) noexcept {}

DescriptorPoolCreateInfo::DescriptorPoolCreateInfo(const VkDescriptorPoolCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

DescriptorPoolCreateInfo::DescriptorPoolCreateInfo(DescriptorPoolCreateInfo&& other) noexcept:
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(other._info),
    _data{std::move(other._data)}
{
    /* Ensure the previous instance doesn't reference state that's now ours */
    /** @todo this is now more like a destructible move, do it more selectively
        and clear only what's really ours and not external? */
    other._info.pNext = nullptr;
    other._info.poolSizeCount = 0;
    other._info.pPoolSizes = nullptr;
}

DescriptorPoolCreateInfo::~DescriptorPoolCreateInfo() = default;

DescriptorPoolCreateInfo& DescriptorPoolCreateInfo::operator=(DescriptorPoolCreateInfo&& other) noexcept {
    using std::swap;
    swap(other._info, _info);
    swap(other._data, _data);
    return *this;
}

DescriptorPool DescriptorPool::wrap(Device& device, const VkDescriptorPool handle, const HandleFlags flags) {
    DescriptorPool out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;
    return out;
}

DescriptorPool::DescriptorPool(Device& device, const DescriptorPoolCreateInfo& info): _device{&device}, _flags{HandleFlag::DestroyOnDestruction} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateDescriptorPool(device, info, nullptr, &_handle));
}

DescriptorPool::DescriptorPool(NoCreateT): _device{}, _handle{} {}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags} {
    other._handle = {};
}

DescriptorPool::~DescriptorPool() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroyDescriptorPool(*_device, _handle, nullptr);
}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    return *this;
}

VkDescriptorPool DescriptorPool::release() {
    const VkDescriptorPool handle = _handle;
    _handle = {};
    return handle;
}

}}
