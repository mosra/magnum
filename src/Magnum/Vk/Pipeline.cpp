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

#include "Pipeline.h"
#include "CommandBuffer.h"

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Vk/Device.h"

namespace Magnum { namespace Vk {

MemoryBarrier::MemoryBarrier(const Accesses sourceAccesses, const Accesses destinationAccesses): _barrier{} {
    _barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    _barrier.srcAccessMask = VkAccessFlags(sourceAccesses);
    _barrier.dstAccessMask = VkAccessFlags(destinationAccesses);
}

MemoryBarrier::MemoryBarrier(NoInitT) noexcept {}

MemoryBarrier::MemoryBarrier(const VkMemoryBarrier& barrier):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _barrier(barrier) {}

BufferMemoryBarrier::BufferMemoryBarrier(const Accesses sourceAccesses, const Accesses destinationAccesses, const VkBuffer buffer, const UnsignedLong offset, const UnsignedLong size): _barrier{} {
    _barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    _barrier.srcAccessMask = VkAccessFlags(sourceAccesses);
    _barrier.dstAccessMask = VkAccessFlags(destinationAccesses);
    _barrier.buffer = buffer;
    _barrier.offset = offset;
    _barrier.size = size;
}

BufferMemoryBarrier::BufferMemoryBarrier(NoInitT) noexcept {}

BufferMemoryBarrier::BufferMemoryBarrier(const VkBufferMemoryBarrier& barrier):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _barrier(barrier) {}

ImageMemoryBarrier::ImageMemoryBarrier(const Accesses sourceAccesses, const Accesses destinationAccesses, const ImageLayout oldLayout, const ImageLayout newLayout, const VkImage image, const ImageAspects aspects, const UnsignedInt layerOffset, const UnsignedInt layerCount, const UnsignedInt levelOffset, const UnsignedInt levelCount): _barrier{} {
    _barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    _barrier.srcAccessMask = VkAccessFlags(sourceAccesses);
    _barrier.dstAccessMask = VkAccessFlags(destinationAccesses);
    _barrier.oldLayout = VkImageLayout(oldLayout);
    _barrier.newLayout = VkImageLayout(newLayout);
    _barrier.image = image;
    _barrier.subresourceRange.aspectMask = VkImageAspectFlags(aspects);
    _barrier.subresourceRange.baseMipLevel = levelOffset;
    _barrier.subresourceRange.levelCount = levelCount;
    _barrier.subresourceRange.baseArrayLayer = layerOffset;
    _barrier.subresourceRange.layerCount = layerCount;
}

ImageMemoryBarrier::ImageMemoryBarrier(NoInitT) noexcept {}

ImageMemoryBarrier::ImageMemoryBarrier(const VkImageMemoryBarrier& barrier):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _barrier(barrier) {}

CommandBuffer& CommandBuffer::pipelineBarrier(const PipelineStages sourceStages, const PipelineStages destinationStages, const Containers::ArrayView<const MemoryBarrier> memoryBarriers, const Containers::ArrayView<const BufferMemoryBarrier> bufferMemoryBarriers, const Containers::ArrayView<const ImageMemoryBarrier> imageMemoryBarriers, const DependencyFlags dependencyFlags) {
    /* Once these grow (VkSampleLocationsInfoEXT?), they will need to be
       linearized into a separate array first */
    static_assert(
        sizeof(MemoryBarrier) == sizeof(VkMemoryBarrier) &&
        sizeof(BufferMemoryBarrier) == sizeof(VkBufferMemoryBarrier) &&
        sizeof(ImageMemoryBarrier) == sizeof(VkImageMemoryBarrier),
        "");
    (**_device).CmdPipelineBarrier(_handle, VkPipelineStageFlags(sourceStages), VkPipelineStageFlags(destinationStages), VkDependencyFlags(dependencyFlags), memoryBarriers.size(), memoryBarriers[0], bufferMemoryBarriers.size(), bufferMemoryBarriers[0], imageMemoryBarriers.size(), imageMemoryBarriers[0]);
    return *this;
}

CommandBuffer& CommandBuffer::pipelineBarrier(const PipelineStages sourceStages, const PipelineStages destinationStages, const std::initializer_list<MemoryBarrier> memoryBarriers, const std::initializer_list<BufferMemoryBarrier> bufferMemoryBarriers, const std::initializer_list<ImageMemoryBarrier> imageMemoryBarriers, const DependencyFlags dependencyFlags) {
    return pipelineBarrier(sourceStages, destinationStages, Containers::arrayView(memoryBarriers), Containers::arrayView(bufferMemoryBarriers), Containers::arrayView(imageMemoryBarriers), dependencyFlags);
}

CommandBuffer& CommandBuffer::pipelineBarrier(const PipelineStages sourceStages, const PipelineStages destinationStages, const Containers::ArrayView<const MemoryBarrier> memoryBarriers, const DependencyFlags dependencyFlags) {
    return pipelineBarrier(sourceStages, destinationStages, memoryBarriers, {}, {}, dependencyFlags);
}

CommandBuffer& CommandBuffer::pipelineBarrier(const PipelineStages sourceStages, const PipelineStages destinationStages, const std::initializer_list<MemoryBarrier> memoryBarriers, const DependencyFlags dependencyFlags) {
    return pipelineBarrier(sourceStages, destinationStages, Containers::arrayView(memoryBarriers), dependencyFlags);
}

CommandBuffer& CommandBuffer::pipelineBarrier(const PipelineStages sourceStages, const PipelineStages destinationStages, const Containers::ArrayView<const BufferMemoryBarrier> bufferMemoryBarriers, const DependencyFlags dependencyFlags) {
    return pipelineBarrier(sourceStages, destinationStages, {}, bufferMemoryBarriers, {}, dependencyFlags);
}

CommandBuffer& CommandBuffer::pipelineBarrier(const PipelineStages sourceStages, const PipelineStages destinationStages, const std::initializer_list<BufferMemoryBarrier> bufferMemoryBarriers, const DependencyFlags dependencyFlags) {
    return pipelineBarrier(sourceStages, destinationStages, Containers::arrayView(bufferMemoryBarriers), dependencyFlags);
}

CommandBuffer& CommandBuffer::pipelineBarrier(const PipelineStages sourceStages, const PipelineStages destinationStages, const Containers::ArrayView<const ImageMemoryBarrier> imageMemoryBarriers, const DependencyFlags dependencyFlags) {
    return pipelineBarrier(sourceStages, destinationStages, {}, {}, imageMemoryBarriers, dependencyFlags);
}

CommandBuffer& CommandBuffer::pipelineBarrier(const PipelineStages sourceStages, const PipelineStages destinationStages, const std::initializer_list<ImageMemoryBarrier> imageMemoryBarriers, const DependencyFlags dependencyFlags) {
    return pipelineBarrier(sourceStages, destinationStages, Containers::arrayView(imageMemoryBarriers), dependencyFlags);
}

}}
