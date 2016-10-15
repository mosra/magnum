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

#include "Image.h"
#include "Magnum/Vk/Buffer.h"
#include "Magnum/Vk/Command.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPool.h"
#include "Magnum/Vk/DeviceMemory.h"
#include "Magnum/Vk/Pipeline.h"
#include "Magnum/Vk/Queue.h"

namespace Magnum { namespace Vk {

Image::~Image() {
    if(_flags >= ObjectFlag::DeleteOnDestruction) {
        vkDestroyImage(*_device, _image, nullptr);
    }
}

std::unique_ptr<DeviceMemory> Image::allocateDeviceMemory(Vk::MemoryProperty memProperty) {
    VkMemoryRequirements memReqs = getMemoryRequirements();
    UnsignedInt memoryTypeIndex = _device->physicalDevice().getMemoryType(memReqs.memoryTypeBits, memProperty);

    std::unique_ptr<DeviceMemory> memory(new Vk::DeviceMemory{*_device, memReqs.size, memoryTypeIndex});
    bindImageMemory(*memory);

    return std::unique_ptr<DeviceMemory>{};
}

Image& Image::update(Queue& queue, CommandPool& pool, const void* sourceData, UnsignedLong size, UnsignedLong destOffset) {
    // TODO: Do not use staging if the memory bound to this buffer is host visible.

    /* command buffer needs to be destroyed before memory */
    std::unique_ptr<Vk::CommandBuffer> copyToDeviceCmds = pool.allocateCommandBuffer(Vk::CommandBuffer::Level::Primary);

    /* plane vertices */
    Buffer stagingBuffer{*_device, UnsignedInt(size), Vk::BufferUsage::TransferSrc};
    std::unique_ptr<DeviceMemory> stagingMemory = stagingBuffer.allocateDeviceMemory(Vk::MemoryProperty::HostVisible);

    Containers::ArrayView<char> data = stagingMemory->map(destOffset, size);
    std::memcpy(data, sourceData, stagingBuffer.size());
    stagingMemory->unmap();

    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 1;
    ImageMemoryBarrier transferBarrier{*this, ImageLayout::Undefined, ImageLayout::TransferDstOptimal, range, {}, {Access::TransferWrite}};
    ImageMemoryBarrier endBarrier{*this, ImageLayout::TransferDstOptimal, ImageLayout::ShaderReadOnlyOptimal, range, {Access::TransferWrite}, {Access::ShaderRead}};

    *copyToDeviceCmds << Vk::Cmd::begin()
                      << Vk::Cmd::pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::TopOfPipe, {}, {}, {transferBarrier})
                      << stagingBuffer.cmdFullCopyTo(*this)
                      << Vk::Cmd::pipelineBarrier(PipelineStage::TopOfPipe, PipelineStage::TopOfPipe, {}, {}, {endBarrier})
                      << Vk::Cmd::end();
    queue.submit(*copyToDeviceCmds).waitIdle();

    return *this;
}

}}
