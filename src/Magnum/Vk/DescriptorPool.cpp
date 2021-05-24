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
#include <Corrade/Containers/Optional.h>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/DescriptorSet.h"
#include "Magnum/Vk/DescriptorType.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Result.h"

namespace Magnum { namespace Vk {

DescriptorPoolCreateInfo::DescriptorPoolCreateInfo(const UnsignedInt maxSets, const Containers::ArrayView<const std::pair<DescriptorType, UnsignedInt>> poolSizes, const Flags flags): _info{} {
    CORRADE_ASSERT(maxSets,
        "Vk::DescriptorPoolCreateInfo: there has to be at least one set", );
    /* On certain compilers, {} (empty initializer list) gets converted to an
       arrayview that's not null, interesting. Explicitly using .empty() to
       ensure the assert gets properly fired. */
    CORRADE_ASSERT(!poolSizes.empty(),
        "Vk::DescriptorPoolCreateInfo: there has to be at least one pool", );

    Containers::ArrayView<VkDescriptorPoolSize> poolSizesCopy;
    _data = Containers::ArrayTuple{
        {NoInit, poolSizes.size(), poolSizesCopy}
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

DescriptorPool::DescriptorPool(Device& device, const DescriptorPoolCreateInfo& info): _device{&device}, _flags{HandleFlag::DestroyOnDestruction}, _freeAllocatedSets{!!(info->flags & VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateDescriptorPool(device, info, nullptr, &_handle));
}

DescriptorPool::DescriptorPool(NoCreateT): _device{}, _handle{} {}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags}, _freeAllocatedSets{other._freeAllocatedSets} {
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
    swap(other._freeAllocatedSets, _freeAllocatedSets);
    return *this;
}

std::pair<Result, DescriptorSet> DescriptorPool::allocateInternal(const VkDescriptorSetLayout layout) {
    DescriptorSet set{NoCreate};
    set._device = _device;
    set._pool = _handle;
    set._flags = _freeAllocatedSets ? HandleFlag::DestroyOnDestruction : HandleFlags{};

    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = _handle;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &layout;
    /* VK_ERROR_OUT_OF_POOL_MEMORY is only available since VK_KHR_maintenance1
       and it's not really clear what was supposed to happen before that. At
       the very least, running the allocateFail() test using

        ./VkDescriptorPoolVkTest --magnum-vulkan-version 1.0 --magnum-enable-layers VK_LAYER_KHRONOS_validation

       without VK_KHR_maintenance1 enabled, the validation layer complains that
       I'm allocating from a pool that doesn't have enough free items, which
       implies it used to be a user error and thus the driver is free to do
       *anything*, including random crashes, as noted on
       https://community.khronos.org/t/descriptor-pool-able-to-allocate-even-though-pool-should-be-empty/7330/6

       From practical testing, even the oldest Vulkan driver I have (ARM Mali
       on Huawei P9, Vulkan 1.0.66) seems to return VK_ERROR_OUT_OF_POOL_MEMORY
       no matter whether the extension is enabled or not. So I'll assume all
       contemporary drivers in early 2021 do this, there's nothing I can do
       otherwise. */
    const Result result = MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR((**_device).AllocateDescriptorSets(*_device, &info, &set._handle), Result::ErrorOutOfPoolMemory, Result::ErrorFragmentedPool);
    return {result, std::move(set)};
}

DescriptorSet DescriptorPool::allocate(const VkDescriptorSetLayout layout) {
    std::pair<Result, DescriptorSet> out = allocateInternal(layout);
    CORRADE_ASSERT(out.first == Result::Success,
        "Vk::DescriptorPool::allocate(): allocation failed with" << out.first, std::move(out.second));
    return std::move(out.second);
}

Containers::Optional<DescriptorSet> DescriptorPool::tryAllocate(const VkDescriptorSetLayout layout) {
    std::pair<Result, DescriptorSet> out = allocateInternal(layout);
    if(out.first != Result::Success) return {};
    return std::move(out.second);
}

std::pair<Result, DescriptorSet> DescriptorPool::allocateInternal(const VkDescriptorSetLayout layout, const UnsignedInt variableDescriptorCount) {
    DescriptorSet set{NoCreate};
    set._device = _device;
    set._pool = _handle;
    set._flags = _freeAllocatedSets ? HandleFlag::DestroyOnDestruction : HandleFlags{};

    VkDescriptorSetVariableDescriptorCountAllocateInfo variableInfo{};
    variableInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
    variableInfo.descriptorSetCount = 1;
    variableInfo.pDescriptorCounts = &variableDescriptorCount;

    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.pNext = &variableInfo;
    info.descriptorPool = _handle;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &layout;
    /* See the not about VK_ERROR_OUT_OF_POOL_MEMORY and VK_KHR_maintenance1
       in the other allocateInternal() implementation above. */
    const Result result = MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR((**_device).AllocateDescriptorSets(*_device, &info, &set._handle), Result::ErrorOutOfPoolMemory, Result::ErrorFragmentedPool);
    return {result, std::move(set)};
}

DescriptorSet DescriptorPool::allocate(const VkDescriptorSetLayout layout, const UnsignedInt variableDescriptorCount) {
    std::pair<Result, DescriptorSet> out = allocateInternal(layout, variableDescriptorCount);
    CORRADE_ASSERT(out.first == Result::Success,
        "Vk::DescriptorPool::allocate(): allocation failed with" << out.first, std::move(out.second));
    return std::move(out.second);
}

Containers::Optional<DescriptorSet> DescriptorPool::tryAllocate(const VkDescriptorSetLayout layout, const UnsignedInt variableDescriptorCount) {
    std::pair<Result, DescriptorSet> out = allocateInternal(layout, variableDescriptorCount);
    if(out.first != Result::Success) return {};
    return std::move(out.second);
}

void DescriptorPool::reset() {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS((**_device).ResetDescriptorPool(*_device, _handle, 0));
}

VkDescriptorPool DescriptorPool::release() {
    const VkDescriptorPool handle = _handle;
    _handle = {};
    return handle;
}

}}
