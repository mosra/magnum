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

#include "Buffer.h"

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Implementation/DeviceState.h"

namespace Magnum { namespace Vk {

BufferCreateInfo::BufferCreateInfo(const BufferUsages usages, const UnsignedLong size, const Flags flags): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    _info.flags = VkBufferCreateFlags(flags);
    _info.size = size;
    _info.usage = VkBufferUsageFlags(usages);
    /* _info.sharingMode is implicitly VK_SHARING_MODE_EXCLUSIVE;
       _info.queueFamilyIndexCount and _info.pQueueFamilyIndices should be
       filled only for VK_SHARING_MODE_CONCURRENT */
}

BufferCreateInfo::BufferCreateInfo(NoInitT) noexcept {}

BufferCreateInfo::BufferCreateInfo(const VkBufferCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

Buffer Buffer::wrap(Device& device, const VkBuffer handle, const HandleFlags flags) {
    Buffer out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;
    return out;
}

Buffer::Buffer(Device& device, const BufferCreateInfo& info, NoAllocateT): _device{&device}, _flags{HandleFlag::DestroyOnDestruction}, _dedicatedMemory{NoCreate} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateBuffer(device, info, nullptr, &_handle));
}

Buffer::Buffer(Device& device, const BufferCreateInfo& info, const MemoryFlags memoryFlags): Buffer{device, info, NoAllocate} {
    const MemoryRequirements requirements = memoryRequirements();
    bindDedicatedMemory(Memory{device, MemoryAllocateInfo{
        requirements.size(),
        device.properties().pickMemory(memoryFlags, requirements.memories())
    }});
}

Buffer::Buffer(NoCreateT): _device{}, _handle{}, _dedicatedMemory{NoCreate} {}

Buffer::Buffer(Buffer&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags}, _dedicatedMemory{std::move(other._dedicatedMemory)} {
    other._handle = {};
}

Buffer::~Buffer() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroyBuffer(*_device, _handle, nullptr);
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    swap(other._dedicatedMemory, _dedicatedMemory);
    return *this;
}

MemoryRequirements Buffer::memoryRequirements() const {
    MemoryRequirements requirements;
    VkBufferMemoryRequirementsInfo2 info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
    info.buffer = _handle;
    _device->state().getBufferMemoryRequirementsImplementation(*_device, info, requirements);
    return requirements;
}

void Buffer::bindMemory(Memory& memory, const UnsignedLong offset) {
    VkBindBufferMemoryInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO;
    info.buffer = _handle;
    info.memory = memory;
    info.memoryOffset = offset;
    _device->state().bindBufferMemoryImplementation(*_device, 1, &info);
}

void Buffer::bindDedicatedMemory(Memory&& memory) {
    bindMemory(memory, 0);
    _dedicatedMemory = std::move(memory);
}

bool Buffer::hasDedicatedMemory() const {
    /* Sigh. Though better than needing to have `const handle()` overloads
       returning `const VkDeviceMemory_T*` */
    return const_cast<Buffer&>(*this)._dedicatedMemory.handle();
}

Memory& Buffer::dedicatedMemory() {
    CORRADE_ASSERT(_dedicatedMemory.handle(),
        "Vk::Buffer::dedicatedMemory(): buffer doesn't have a dedicated memory", _dedicatedMemory);
    return _dedicatedMemory;
}

VkBuffer Buffer::release() {
    const VkBuffer handle = _handle;
    _handle = {};
    return handle;
}

void Buffer::getMemoryRequirementsImplementationDefault(Device& device, const VkBufferMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements) {
    device->GetBufferMemoryRequirements(device, info.buffer, &requirements.memoryRequirements);
}

void Buffer::getMemoryRequirementsImplementationKHR(Device& device, const VkBufferMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements) {
    device->GetBufferMemoryRequirements2KHR(device, &info, &requirements);
}

void Buffer::getMemoryRequirementsImplementation11(Device& device, const VkBufferMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements) {
    device->GetBufferMemoryRequirements2(device, &info, &requirements);
}

void Buffer::bindMemoryImplementationDefault(Device& device, UnsignedInt count, const VkBindBufferMemoryInfo* const infos) {
    for(std::size_t i = 0; i != count; ++i)
        device->BindBufferMemory(device, infos[i].buffer, infos[i].memory, infos[i].memoryOffset);
}

void Buffer::bindMemoryImplementationKHR(Device& device, UnsignedInt count, const VkBindBufferMemoryInfo* const infos) {
    device->BindBufferMemory2KHR(device, count, infos);
}

void Buffer::bindMemoryImplementation11(Device& device, UnsignedInt count, const VkBindBufferMemoryInfo* const infos) {
    device->BindBufferMemory2(device, count, infos);
}

}}
