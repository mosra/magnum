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

#include "DescriptorSetLayout.h"
#include "DescriptorSetLayoutCreateInfo.h"

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/AnyReference.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Result.h"

namespace Magnum { namespace Vk {

DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(const UnsignedInt binding, const DescriptorType descriptorType, const UnsignedInt descriptorCount, const ShaderStages stages, Flags flags): _binding{}, _flags{VkDescriptorBindingFlags(flags)} {
    _binding.binding = binding;
    _binding.descriptorType = VkDescriptorType(descriptorType);
    _binding.descriptorCount = descriptorCount;
    _binding.stageFlags = VkShaderStageFlags(stages);
}

DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(const UnsignedInt binding, const DescriptorType descriptorType, const Containers::ArrayView<const VkSampler> immutableSamplers, const ShaderStages stages, Flags flags): _binding{}, _flags{VkDescriptorBindingFlags(flags)} {
    Containers::ArrayView<VkSampler> immutableSamplersCopy;
    _data = Containers::ArrayTuple{
        {NoInit, immutableSamplers.size(), immutableSamplersCopy}
    };
    Utility::copy(immutableSamplers, immutableSamplersCopy);

    _binding.binding = binding;
    _binding.descriptorType = VkDescriptorType(descriptorType);
    _binding.descriptorCount = immutableSamplers.size();
    _binding.pImmutableSamplers = immutableSamplersCopy;
    _binding.stageFlags = VkShaderStageFlags(stages);
}

DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(const UnsignedInt binding, const DescriptorType descriptorType, const std::initializer_list<VkSampler> immutableSamplers, const ShaderStages stages, Flags flags): DescriptorSetLayoutBinding{binding, descriptorType, Containers::arrayView(immutableSamplers), stages, flags} {}

DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(NoInitT) noexcept {}

DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding& binding, const VkDescriptorBindingFlags flags):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _binding(binding),
    _flags{flags} {}

DescriptorSetLayoutBinding::DescriptorSetLayoutBinding(DescriptorSetLayoutBinding&& other) noexcept:
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _binding(other._binding),
    _data{std::move(other._data)},
    _flags{other._flags}
{
    /* Ensure the previous instance doesn't reference state that's now ours */
    /** @todo this is now more like a destructible move, do it more selectively
        and clear only what's really ours and not external? */
    other._binding.pImmutableSamplers = nullptr;
}

DescriptorSetLayoutBinding::~DescriptorSetLayoutBinding() = default;

DescriptorSetLayoutBinding& DescriptorSetLayoutBinding::operator=(DescriptorSetLayoutBinding&& other) noexcept {
    using std::swap;
    swap(other._binding, _binding);
    swap(other._data, _data);
    swap(other._flags, _flags);
    return *this;
}

DescriptorSetLayoutCreateInfo::DescriptorSetLayoutCreateInfo(const Containers::ArrayView<const Containers::AnyReference<const DescriptorSetLayoutBinding>> bindings, const Flags flags): _info{} {
    /* Check the total count of immutable samplers to allocate them all in a
       contiguous memory location. Also check if we have any binding flags. If
       yes, we have to create an additional array and put a structure into the
       pNext chain. */
    std::size_t immutableSamplerCount = 0;
    bool hasBindingFlags = false;
    for(const DescriptorSetLayoutBinding& b: bindings) {
        if(b->pImmutableSamplers) immutableSamplerCount += b->descriptorCount;
        if(b.flags()) hasBindingFlags = true;
    }

    /* Allocate a single block of memory for everything. (I'm still amazed at
       how simple this is and yet how useful.) */
    Containers::ArrayView<VkDescriptorSetLayoutBinding> bindingsCopy;
    Containers::ArrayView<VkDescriptorBindingFlags> bindingFlagsCopy;
    Containers::ArrayView<VkDescriptorSetLayoutBindingFlagsCreateInfo> bindingsCreateInfoView;
    Containers::ArrayView<VkSampler> immutableSamplersCopy;
    _data = Containers::ArrayTuple{
        {NoInit, bindings.size(), bindingsCopy},
        {NoInit, hasBindingFlags ? bindings.size() : 0, bindingFlagsCopy},
        {ValueInit, hasBindingFlags ? 1u : 0u, bindingsCreateInfoView},
        {NoInit, immutableSamplerCount, immutableSamplersCopy}
    };

    /* Copy the binding and then for each manually copy and reroute the
       immutable samplers, if any; copy the flags as well if there are any */
    std::size_t immutableSamplerOffset = 0;
    for(std::size_t i = 0; i != bindings.size(); ++i) {
        const DescriptorSetLayoutBinding& b = bindings[i];
        bindingsCopy[i] = b;
        if(b->pImmutableSamplers) {
            Utility::copy(
                /* Just {} makes MSVC (and clang-cl!!) pick ArrayView<void>
                   for some reason */
                Containers::arrayView(b->pImmutableSamplers, b->descriptorCount),
                immutableSamplersCopy.slice(immutableSamplerOffset, immutableSamplerOffset + b->descriptorCount));
            bindingsCopy[i].pImmutableSamplers = immutableSamplersCopy + immutableSamplerOffset;
            immutableSamplerOffset += b->descriptorCount;
        }
        if(hasBindingFlags) bindingFlagsCopy[i] = b.flags();
    }
    CORRADE_INTERNAL_ASSERT(immutableSamplerOffset == immutableSamplerCount);

    _info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    _info.flags = VkDescriptorSetLayoutCreateFlags(flags);
    _info.bindingCount = bindings.size();
    _info.pBindings = bindingsCopy;
    if(hasBindingFlags) {
        bindingsCreateInfoView[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        bindingsCreateInfoView[0].bindingCount = bindings.size();
        bindingsCreateInfoView[0].pBindingFlags = bindingFlagsCopy;
        _info.pNext = bindingsCreateInfoView;
    }
}

DescriptorSetLayoutCreateInfo::DescriptorSetLayoutCreateInfo(const std::initializer_list<Containers::AnyReference<const DescriptorSetLayoutBinding>> bindings, const Flags flags): DescriptorSetLayoutCreateInfo{Containers::arrayView(bindings), flags} {}

DescriptorSetLayoutCreateInfo::DescriptorSetLayoutCreateInfo(NoInitT) noexcept {}

DescriptorSetLayoutCreateInfo::DescriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

DescriptorSetLayoutCreateInfo::DescriptorSetLayoutCreateInfo(DescriptorSetLayoutCreateInfo&& other) noexcept:
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(other._info),
    _data{std::move(other._data)}
{
    /* Ensure the previous instance doesn't reference state that's now ours */
    /** @todo this is now more like a destructible move, do it more selectively
        and clear only what's really ours and not external? */
    other._info.pNext = nullptr;
    other._info.bindingCount = 0;
    other._info.pBindings = nullptr;
}

DescriptorSetLayoutCreateInfo::~DescriptorSetLayoutCreateInfo() = default;

DescriptorSetLayoutCreateInfo& DescriptorSetLayoutCreateInfo::operator=(DescriptorSetLayoutCreateInfo&& other) noexcept {
    using std::swap;
    swap(other._info, _info);
    swap(other._data, _data);
    return *this;
}

DescriptorSetLayout DescriptorSetLayout::wrap(Device& device, const VkDescriptorSetLayout handle, const HandleFlags flags) {
    DescriptorSetLayout out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;
    return out;
}

DescriptorSetLayout::DescriptorSetLayout(Device& device, const DescriptorSetLayoutCreateInfo& info): _device{&device}, _flags{HandleFlag::DestroyOnDestruction} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateDescriptorSetLayout(device, info, nullptr, &_handle));
}

DescriptorSetLayout::DescriptorSetLayout(NoCreateT): _device{}, _handle{} {}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags} {
    other._handle = {};
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroyDescriptorSetLayout(*_device, _handle, nullptr);
}

DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    return *this;
}

VkDescriptorSetLayout DescriptorSetLayout::release() {
    const VkDescriptorSetLayout handle = _handle;
    _handle = {};
    return handle;
}

}}
