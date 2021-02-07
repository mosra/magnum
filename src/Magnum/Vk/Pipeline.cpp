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
#include "RasterizationPipelineCreateInfo.h"
#include "ComputePipelineCreateInfo.h"
#include "CommandBuffer.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/BigEnumSet.hpp>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/Integration.h"
#include "Magnum/Vk/MeshLayout.h"
#include "Magnum/Vk/ShaderSet.h"

namespace Magnum { namespace Vk {

struct RasterizationPipelineCreateInfo::State {
    Containers::Array<VkPipelineColorBlendAttachmentState> colorBlendAttachments;

    /* The enum is saved as well to be subsequently available through
       Pipeline::dynamicRasterizationStates() */
    DynamicRasterizationStates dynamicStates;
    Containers::Array<VkDynamicState> dynamicStateList;

    /** @todo make an array once we support multiview */
    VkViewport viewport;
    VkRect2D scissor;
};

RasterizationPipelineCreateInfo::RasterizationPipelineCreateInfo(const ShaderSet& shaderSet, const MeshLayout& meshLayout, const VkPipelineLayout pipelineLayout, const VkRenderPass renderPass, const UnsignedInt subpass, const UnsignedInt subpassColorAttachmentCount, Flags flags): _info{}, _viewportInfo{}, _rasterizationInfo{}, _multisampleInfo{}, _depthStencilInfo{}, _colorBlendInfo{}, _dynamicInfo{}, _state{Containers::InPlaceInit} {
    _info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    _info.flags = VkPipelineCreateFlags(flags);
    _info.stageCount = shaderSet.stages().size();
    _info.pStages = shaderSet.stages();
    _info.pVertexInputState = meshLayout;
    _info.pInputAssemblyState = meshLayout;

    /* pTesselationState is fine to be null */
    /** @todo add it if we get passed a tessellation shader? or should the
        shader wrapper include it? */

    /* Leaving pViewportState null as that gets (but doesn't have to, if
       rasterization is disabled) set by setViewport() */

    _rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    _rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    _rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    _rasterizationInfo.lineWidth = 1.0f;
    _info.pRasterizationState = &_rasterizationInfo;

    _multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    _multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _info.pMultisampleState = &_multisampleInfo;

    _depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    _info.pDepthStencilState = &_depthStencilInfo;

    _state->colorBlendAttachments = Containers::Array<VkPipelineColorBlendAttachmentState>{Containers::ValueInit, subpassColorAttachmentCount};
    for(VkPipelineColorBlendAttachmentState& i: _state->colorBlendAttachments) {
        i.colorWriteMask = VK_COLOR_COMPONENT_R_BIT|
                           VK_COLOR_COMPONENT_G_BIT|
                           VK_COLOR_COMPONENT_B_BIT|
                           VK_COLOR_COMPONENT_A_BIT;
    }

    _colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    _colorBlendInfo.attachmentCount = subpassColorAttachmentCount;
    _colorBlendInfo.pAttachments = _state->colorBlendAttachments;
    _info.pColorBlendState = &_colorBlendInfo;

    /* pDynamicState left null, gets set by setDynamicStates() if needed */

    _info.layout = pipelineLayout;
    _info.renderPass = renderPass;
    _info.subpass = subpass;
}

RasterizationPipelineCreateInfo::RasterizationPipelineCreateInfo(NoInitT) noexcept {}

RasterizationPipelineCreateInfo::RasterizationPipelineCreateInfo(const VkGraphicsPipelineCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info)
{
    /* Copy and reroute all top-level nested structs as well */
    if(info.pViewportState)
        _info.pViewportState = &(_viewportInfo = *info.pViewportState);
    if(info.pRasterizationState)
        _info.pRasterizationState = &(_rasterizationInfo = *info.pRasterizationState);
    if(info.pMultisampleState)
        _info.pMultisampleState = &(_multisampleInfo = *info.pMultisampleState);
    if(info.pDepthStencilState)
        _info.pDepthStencilState = &(_depthStencilInfo = *info.pDepthStencilState);
    if(info.pColorBlendState)
        _info.pColorBlendState = &(_colorBlendInfo = *info.pColorBlendState);
    if(info.pDynamicState)
        _info.pDynamicState = &(_dynamicInfo = *info.pDynamicState);
}

RasterizationPipelineCreateInfo::RasterizationPipelineCreateInfo(RasterizationPipelineCreateInfo&& other) noexcept:
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(other._info),
    _viewportInfo(other._viewportInfo),
    _rasterizationInfo(other._rasterizationInfo),
    _multisampleInfo(other._multisampleInfo),
    _depthStencilInfo(other._depthStencilInfo),
    _colorBlendInfo(other._colorBlendInfo),
    _dynamicInfo(other._dynamicInfo),
    _state{std::move(other._state)}
{
    /* Reroute the pointers */
    if(_info.pViewportState == &other._viewportInfo)
        _info.pViewportState = &_viewportInfo;
    if(_info.pRasterizationState == &other._rasterizationInfo)
        _info.pRasterizationState = &_rasterizationInfo;
    if(_info.pMultisampleState == &other._multisampleInfo)
        _info.pMultisampleState = &_multisampleInfo;
    if(_info.pDepthStencilState == &other._depthStencilInfo)
        _info.pDepthStencilState = &_depthStencilInfo;
    if(_info.pColorBlendState == &other._colorBlendInfo)
        _info.pColorBlendState = &_colorBlendInfo;
    if(_info.pDynamicState == &other._dynamicInfo)
        _info.pDynamicState = &_dynamicInfo;

    /* Ensure the previous instance doesn't reference state that's now ours */
    /** @todo this is now more like a destructible move, do it more selectively
        and clear only what's really ours and not external? */
    other._info.pNext = nullptr;
    other._info.stageCount = 0;
    other._info.pStages = nullptr;
    other._info.pVertexInputState = nullptr;
    other._info.pInputAssemblyState = nullptr;
    other._info.pTessellationState = nullptr;
    other._info.pViewportState = nullptr;
    other._info.pRasterizationState = nullptr;
    other._info.pMultisampleState = nullptr;
    other._info.pDepthStencilState = nullptr;
    other._info.pColorBlendState = nullptr;
    other._info.pDynamicState = nullptr;
}

RasterizationPipelineCreateInfo::~RasterizationPipelineCreateInfo() = default;

RasterizationPipelineCreateInfo& RasterizationPipelineCreateInfo::operator=(RasterizationPipelineCreateInfo&& other) noexcept {
    using std::swap;
    swap(other._info, _info);
    swap(other._viewportInfo, _viewportInfo);
    swap(other._rasterizationInfo, _rasterizationInfo);
    swap(other._multisampleInfo, _multisampleInfo);
    swap(other._depthStencilInfo, _depthStencilInfo);
    swap(other._colorBlendInfo, _colorBlendInfo);
    swap(other._dynamicInfo, _dynamicInfo);
    swap(other._state, _state);

    /* Reroute the pointers */
    if(_info.pViewportState == &other._viewportInfo)
        _info.pViewportState = &_viewportInfo;
    if(_info.pRasterizationState == &other._rasterizationInfo)
        _info.pRasterizationState = &_rasterizationInfo;
    if(_info.pMultisampleState == &other._multisampleInfo)
        _info.pMultisampleState = &_multisampleInfo;
    if(_info.pDepthStencilState == &other._depthStencilInfo)
        _info.pDepthStencilState = &_depthStencilInfo;
    if(_info.pColorBlendState == &other._colorBlendInfo)
        _info.pColorBlendState = &_colorBlendInfo;
    if(_info.pDynamicState == &other._dynamicInfo)
        _info.pDynamicState = &_dynamicInfo;

    /* And other way around as well */
    if(other._info.pViewportState == &_viewportInfo)
        other._info.pViewportState = &other._viewportInfo;
    if(other._info.pRasterizationState == &_rasterizationInfo)
        other._info.pRasterizationState = &other._rasterizationInfo;
    if(other._info.pMultisampleState == &_multisampleInfo)
        other._info.pMultisampleState = &other._multisampleInfo;
    if(other._info.pDepthStencilState == &_depthStencilInfo)
        other._info.pDepthStencilState = &other._depthStencilInfo;
    if(other._info.pColorBlendState == &_colorBlendInfo)
        other._info.pColorBlendState = &other._colorBlendInfo;
    if(other._info.pDynamicState == &_dynamicInfo)
        other._info.pDynamicState = &other._dynamicInfo;

    return *this;
}

RasterizationPipelineCreateInfo& RasterizationPipelineCreateInfo::setViewport(const Range3D& viewport, const Range2Di& scissor) {
    _state->viewport = VkViewport(viewport);
    _state->scissor = VkRect2D(scissor);

    _viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    _viewportInfo.viewportCount = 1;
    _viewportInfo.pViewports = &_state->viewport;
    _viewportInfo.scissorCount = 1;
    _viewportInfo.pScissors = &_state->scissor;
    _info.pViewportState = &_viewportInfo;
    return *this;
}

RasterizationPipelineCreateInfo& RasterizationPipelineCreateInfo::setViewport(const Range3D& viewport) {
    return setViewport(viewport, Range2Di{viewport.xy()});
}

RasterizationPipelineCreateInfo& RasterizationPipelineCreateInfo::setViewport(const Range2D& viewport, const Range2Di& scissor) {
    return setViewport(Range3D{{viewport.min(), 0.0f}, {viewport.max(), 1.0f}}, scissor);
}

RasterizationPipelineCreateInfo& RasterizationPipelineCreateInfo::setViewport(const Range2D& viewport) {
    return setViewport(viewport, Range2Di{viewport});
}

namespace {

constexpr VkDynamicState DynamicRasterizationStateMapping[]{
    #define _c(value, vkValue) VK_DYNAMIC_STATE_ ## vkValue,
    #include "Magnum/Vk/Implementation/dynamicRasterizationStateMapping.hpp"
    #undef _c
};

}

RasterizationPipelineCreateInfo& RasterizationPipelineCreateInfo::setDynamicStates(const DynamicRasterizationStates& states) {
    /* Save the enum so we can store it in the created Pipeline object later */
    _state->dynamicStates = states;

    /* Count the number of states set, allocate for that */
    std::size_t count = 0;
    for(std::size_t i = 0; i != DynamicRasterizationStates::Size; ++i)
        count += Math::popcount(states.data()[i]);
    _state->dynamicStateList = Containers::Array<VkDynamicState>{Containers::NoInit, count};

    std::size_t offset = 0;
    for(std::uint64_t i = 0; i != Containers::arraySize(DynamicRasterizationStateMapping); ++i)
        if(states & DynamicRasterizationState(i))
            _state->dynamicStateList[offset++] = DynamicRasterizationStateMapping[i];
    CORRADE_INTERNAL_ASSERT(offset == count);

    _dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    _dynamicInfo.dynamicStateCount = count;
    _dynamicInfo.pDynamicStates = _state->dynamicStateList;
    _info.pDynamicState = &_dynamicInfo;
    return *this;
}

ComputePipelineCreateInfo::ComputePipelineCreateInfo(const ShaderSet& shaderSet, const VkPipelineLayout pipelineLayout, const Flags flags): _info{} {
    CORRADE_ASSERT(shaderSet.stages().size() == 1,
        "Vk::ComputePipelineCreateInfo: the shader set has to contain exactly one shader, got" << shaderSet.stages().size(), );

    _info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    _info.flags = VkPipelineCreateFlags(flags);
    _info.stage = shaderSet.stages()[0];
    _info.layout = pipelineLayout;
}

ComputePipelineCreateInfo::ComputePipelineCreateInfo(NoInitT) noexcept {}

ComputePipelineCreateInfo::ComputePipelineCreateInfo(const VkComputePipelineCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

Pipeline Pipeline::wrap(Device& device, const PipelineBindPoint bindPoint, const VkPipeline handle, const DynamicRasterizationStates& dynamicStates, const HandleFlags flags) {
    Pipeline out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._bindPoint = bindPoint;
    out._flags = flags;
    out._dynamicStates.rasterization = dynamicStates;
    return out;
}

Pipeline Pipeline::wrap(Device& device, const PipelineBindPoint bindPoint, const VkPipeline handle, const HandleFlags flags) {
    return wrap(device, bindPoint, handle, DynamicRasterizationStates{}, flags);
}

Pipeline::Pipeline(Device& device, const RasterizationPipelineCreateInfo& info):
    _device{&device},
    #ifdef CORRADE_GRACEFUL_ASSERT
    /* Otherwise vkDestroyPipeline() crashes when we hit the assert */
    _handle{},
    #endif
    _bindPoint{PipelineBindPoint::Rasterization},
    _flags{HandleFlag::DestroyOnDestruction},
    _dynamicStates{info._state->dynamicStates}
{
    /* Doesn't check that the viewport is really a dynamic state, but should
       catch most cases without false positives */
    CORRADE_ASSERT(info->pViewportState || info->pRasterizationState->rasterizerDiscardEnable || info->pDynamicState,
        "Vk::Pipeline: if rasterization discard is not enabled, the viewport has to be either dynamic or set via setViewport()", );

    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateGraphicsPipelines(device, {}, 1, info, nullptr, &_handle));
}

Pipeline::Pipeline(Device& device, const ComputePipelineCreateInfo& info): _device{&device}, _bindPoint{PipelineBindPoint::Compute}, _flags{HandleFlag::DestroyOnDestruction}, _dynamicStates{} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateComputePipelines(device, {}, 1, info, nullptr, &_handle));
}

Pipeline::Pipeline(NoCreateT): _device{}, _handle{}, _bindPoint{}, _dynamicStates{} {}

Pipeline::Pipeline(Pipeline&& other) noexcept: _device{other._device}, _handle{other._handle}, _bindPoint{other._bindPoint}, _flags{other._flags},
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _dynamicStates(other._dynamicStates)
{
    other._handle = {};
}

Pipeline::~Pipeline() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroyPipeline(*_device, _handle, nullptr);
}

Pipeline& Pipeline::operator=(Pipeline&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._bindPoint, _bindPoint);
    swap(other._flags, _flags);
    swap(other._dynamicStates, _dynamicStates);
    return *this;
}

DynamicRasterizationStates Pipeline::dynamicRasterizationStates() const {
    CORRADE_ASSERT(_bindPoint == PipelineBindPoint::Rasterization,
        "Vk::Pipeline::dynamicRasterizationStates(): not a rasterization pipeline", {});
    return _dynamicStates.rasterization;
}

VkPipeline Pipeline::release() {
    const VkPipeline handle = _handle;
    _handle = {};
    return handle;
}

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

ImageMemoryBarrier::ImageMemoryBarrier(const Accesses sourceAccesses, const Accesses destinationAccesses, const ImageLayout oldLayout, const ImageLayout newLayout, Image& image, const UnsignedInt layerOffset, const UnsignedInt layerCount, const UnsignedInt levelOffset, const UnsignedInt levelCount): ImageMemoryBarrier{sourceAccesses, destinationAccesses, oldLayout, newLayout, image, imageAspectsFor(image.format()), layerOffset, layerCount, levelOffset, levelCount} {}

ImageMemoryBarrier::ImageMemoryBarrier(NoInitT) noexcept {}

ImageMemoryBarrier::ImageMemoryBarrier(const VkImageMemoryBarrier& barrier):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _barrier(barrier) {}

CommandBuffer& CommandBuffer::bindPipeline(Pipeline& pipeline) {
    /* Save the set of dynamic states for future use */
    if(pipeline.bindPoint() == PipelineBindPoint::Rasterization)
        _dynamicRasterizationStates = pipeline.dynamicRasterizationStates();

    (**_device).CmdBindPipeline(_handle, VkPipelineBindPoint(pipeline.bindPoint()), pipeline);
    return *this;
}

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

CommandBuffer& CommandBuffer::pipelineBarrier(const PipelineStages sourceStages, const PipelineStages destinationStages, const DependencyFlags dependencyFlags) {
    return pipelineBarrier(sourceStages, destinationStages, {}, {}, {}, dependencyFlags);
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

Debug& operator<<(Debug& debug, const PipelineBindPoint value) {
    debug << "Vk::PipelineBindPoint" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::PipelineBindPoint::value: return debug << "::" << Debug::nospace << #value;
        _c(Rasterization)
        _c(RayTracing)
        _c(Compute)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

namespace {

constexpr const char* DynamicRasterizationStateNames[]{
    #define _c(value, vkValue) #value,
    #include "Magnum/Vk/Implementation/dynamicRasterizationStateMapping.hpp"
    #undef _c
};

}

Debug& operator<<(Debug& debug, const DynamicRasterizationState value) {
    debug << "Vk::DynamicRasterizationState" << Debug::nospace;

    if(UnsignedInt(value) < Containers::arraySize(DynamicRasterizationStateNames)) {
        return debug << "::" << Debug::nospace << DynamicRasterizationStateNames[UnsignedInt(value)];
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DynamicRasterizationStates& value) {
    return Containers::bigEnumSetDebugOutput(debug, value, "Vk::DynamicRasterizationStates{}");
}

}}
