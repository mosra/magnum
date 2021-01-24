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

#include "Image.h"
#include "ImageCreateInfo.h"
#include "CommandBuffer.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Array.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Integration.h"
#include "Magnum/Vk/MemoryAllocateInfo.h"
#include "Magnum/Vk/PixelFormat.h"
#include "Magnum/Vk/Implementation/DeviceState.h"

namespace Magnum { namespace Vk {

ImageCreateInfo::ImageCreateInfo(const VkImageType type, const ImageUsages usages, const PixelFormat format, const Vector3i& size, const Int layers, const Int levels, const Int samples, const ImageLayout initialLayout, const Flags flags): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    _info.flags = VkImageCreateFlags(flags);
    _info.imageType = type;
    _info.format = VkFormat(format);
    _info.extent = VkExtent3D(size);
    _info.mipLevels = levels;
    _info.arrayLayers = layers;
    _info.samples = VkSampleCountFlagBits(samples);
    _info.tiling = VK_IMAGE_TILING_OPTIMAL;
    _info.usage = VkImageUsageFlags(usages);
    /* _info.sharingMode is implicitly VK_SHARING_MODE_EXCLUSIVE;
       _info.queueFamilyIndexCount and _info.pQueueFamilyIndices should be
       filled only for VK_SHARING_MODE_CONCURRENT */
    _info.initialLayout = VkImageLayout(initialLayout);
}

ImageCreateInfo::ImageCreateInfo(const VkImageType type, const ImageUsages usages, const Magnum::PixelFormat format, const Vector3i& size, const Int layers, const Int levels, const Int samples, const ImageLayout initialLayout, const Flags flags): ImageCreateInfo{type, usages, pixelFormat(format), size, layers, levels, samples, initialLayout, flags} {}

ImageCreateInfo::ImageCreateInfo(const VkImageType type, const ImageUsages usages, const Magnum::CompressedPixelFormat format, const Vector3i& size, const Int layers, const Int levels, const Int samples, const ImageLayout initialLayout, const Flags flags): ImageCreateInfo{type, usages, pixelFormat(format), size, layers, levels, samples, initialLayout, flags} {}

ImageCreateInfo::ImageCreateInfo(NoInitT) noexcept {}

ImageCreateInfo::ImageCreateInfo(const VkImageCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

Debug& operator<<(Debug& debug, const ImageAspect value) {
    debug << "Vk::ImageAspect" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::ImageAspect::value: return debug << "::" << Debug::nospace << #value;
        _c(Color)
        _c(Depth)
        _c(Stencil)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Flag bits should be in hex, unlike plain values */
    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ImageAspects value) {
    return Containers::enumSetDebugOutput(debug, value, "Vk::ImageAspects{}", {
        Vk::ImageAspect::Color,
        Vk::ImageAspect::Depth,
        Vk::ImageAspect::Stencil});
}

/* Vulkan, it would kill you if 0 was a valid default, right?! ffs */
ImageAspects imageAspectsFor(const PixelFormat format) {
    /** @todo expand somehow to catch any invalid values? */
    CORRADE_ASSERT(Int(format), "Vk::imageAspectsFor(): can't get an aspect for" << format, {});

    if(format == PixelFormat::Depth16UnormStencil8UI ||
       format == PixelFormat::Depth24UnormStencil8UI ||
       format == PixelFormat::Depth32FStencil8UI)
        return ImageAspect::Depth|ImageAspect::Stencil;
    if(format == PixelFormat::Depth16Unorm ||
       format == PixelFormat::Depth24Unorm ||
       format == PixelFormat::Depth32F)
        return ImageAspect::Depth;
    if(format == PixelFormat::Stencil8UI)
        return ImageAspect::Stencil;

    /** @todo planar formats */

    return ImageAspect::Color;
}

ImageAspects imageAspectsFor(const Magnum::PixelFormat format) {
    return imageAspectsFor(pixelFormat(format));
}

Image Image::wrap(Device& device, const VkImage handle, const PixelFormat format, const HandleFlags flags) {
    Image out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;
    out._format = format;
    return out;
}

Image Image::wrap(Device& device, const VkImage handle, const Magnum::PixelFormat format, const HandleFlags flags) {
    return wrap(device, handle, pixelFormat(format), flags);
}

Image Image::wrap(Device& device, const VkImage handle, const Magnum::CompressedPixelFormat format, const HandleFlags flags) {
    return wrap(device, handle, pixelFormat(format), flags);
}

Image::Image(Device& device, const ImageCreateInfo& info, NoAllocateT): _device{&device}, _flags{HandleFlag::DestroyOnDestruction}, _format{PixelFormat(info->format)}, _dedicatedMemory{NoCreate} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateImage(device, info, nullptr, &_handle));
}

Image::Image(Device& device, const ImageCreateInfo& info, const MemoryFlags memoryFlags): Image{device, info, NoAllocate} {
    const MemoryRequirements requirements = memoryRequirements();
    bindDedicatedMemory(Memory{device, MemoryAllocateInfo{
        requirements.size(),
        device.properties().pickMemory(memoryFlags, requirements.memories())
    }});
}

Image::Image(NoCreateT): _device{}, _handle{}, _format{}, _dedicatedMemory{NoCreate} {}

Image::Image(Image&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags}, _format{other._format}, _dedicatedMemory{std::move(other._dedicatedMemory)} {
    other._handle = {};
}

Image::~Image() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroyImage(*_device, _handle, nullptr);
}

Image& Image::operator=(Image&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    swap(other._format, _format);
    swap(other._dedicatedMemory, _dedicatedMemory);
    return *this;
}

MemoryRequirements Image::memoryRequirements() const {
    MemoryRequirements requirements;
    VkImageMemoryRequirementsInfo2 info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
    info.image = _handle;
    _device->state().getImageMemoryRequirementsImplementation(*_device, info, requirements);
    return requirements;
}

void Image::bindMemory(Memory& memory, const UnsignedLong offset) {
    VkBindImageMemoryInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
    info.image = _handle;
    info.memory = memory;
    info.memoryOffset = offset;
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(_device->state().bindImageMemoryImplementation(*_device, 1, &info));
}

void Image::bindDedicatedMemory(Memory&& memory) {
    bindMemory(memory, 0);
    _dedicatedMemory = std::move(memory);
}

bool Image::hasDedicatedMemory() const {
    /* Sigh. Though better than needing to have `const handle()` overloads
       returning `const VkDeviceMemory_T*` */
    return const_cast<Image&>(*this)._dedicatedMemory.handle();
}

Memory& Image::dedicatedMemory() {
    CORRADE_ASSERT(_dedicatedMemory.handle(),
        "Vk::Image::dedicatedMemory(): image doesn't have a dedicated memory", _dedicatedMemory);
    return _dedicatedMemory;
}

VkImage Image::release() {
    const VkImage handle = _handle;
    _handle = {};
    return handle;
}

void Image::getMemoryRequirementsImplementationDefault(Device& device, const VkImageMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements) {
    return device->GetImageMemoryRequirements(device, info.image, &requirements.memoryRequirements);
}

void Image::getMemoryRequirementsImplementationKHR(Device& device, const VkImageMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements) {
    return device->GetImageMemoryRequirements2KHR(device, &info, &requirements);
}

void Image::getMemoryRequirementsImplementation11(Device& device, const VkImageMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements) {
    return device->GetImageMemoryRequirements2(device, &info, &requirements);
}

VkResult Image::bindMemoryImplementationDefault(Device& device, UnsignedInt count, const VkBindImageMemoryInfo* const infos) {
    for(std::size_t i = 0; i != count; ++i)
        if(VkResult result = device->BindImageMemory(device, infos[i].image, infos[i].memory, infos[i].memoryOffset)) return result;
    return VK_SUCCESS;
}

VkResult Image::bindMemoryImplementationKHR(Device& device, UnsignedInt count, const VkBindImageMemoryInfo* const infos) {
    return device->BindImageMemory2KHR(device, count, infos);
}

VkResult Image::bindMemoryImplementation11(Device& device, UnsignedInt count, const VkBindImageMemoryInfo* const infos) {
    return device->BindImageMemory2(device, count, infos);
}

ImageCopy::ImageCopy(const ImageAspects aspects, const Int sourceLevel, const Int sourceLayerOffset, const Int sourceLayerCount, const Vector3i& sourceOffset, const Int destinationLevel, const Int destinationLayerOffset, const Int destinationLayerCount, const Vector3i& destinationOffset, const Vector3i& size): _copy{} {
    _copy.sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2_KHR;
    _copy.srcSubresource.aspectMask = VkImageAspectFlags(aspects);
    _copy.srcSubresource.mipLevel = sourceLevel;
    _copy.srcSubresource.baseArrayLayer = sourceLayerOffset;
    _copy.srcSubresource.layerCount = sourceLayerCount;
    _copy.srcOffset = VkOffset3D(sourceOffset);
    _copy.dstSubresource.aspectMask = VkImageAspectFlags(aspects);
    _copy.dstSubresource.mipLevel = destinationLevel;
    _copy.dstSubresource.baseArrayLayer = destinationLayerOffset;
    _copy.dstSubresource.layerCount = destinationLayerCount;
    _copy.dstOffset = VkOffset3D(destinationOffset);
    _copy.extent = VkExtent3D(size);
}

ImageCopy::ImageCopy(NoInitT) noexcept {}

ImageCopy::ImageCopy(const VkImageCopy2KHR& copy):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _copy(copy) {}

ImageCopy::ImageCopy(const VkImageCopy& copy): _copy{
    VK_STRUCTURE_TYPE_IMAGE_COPY_2_KHR,
    nullptr,
    copy.srcSubresource,
    copy.srcOffset,
    copy.dstSubresource,
    copy.dstOffset,
    copy.extent
} {}

namespace {

/* Used by CopyImageInfo::vkCopyImageInfo() as well */
VkImageCopy vkImageCopy(const VkImageCopy2KHR& copy) {
    CORRADE_ASSERT(!copy.pNext,
        "Vk::ImageCopy: disallowing conversion to VkImageCopy with non-empty pNext to prevent information loss", {});
    return {
        copy.srcSubresource,
        copy.srcOffset,
        copy.dstSubresource,
        copy.dstOffset,
        copy.extent
    };
}

}

VkImageCopy ImageCopy::vkImageCopy() const {
    return Vk::vkImageCopy(_copy);
}

CopyImageInfo::CopyImageInfo(const VkImage source, const ImageLayout sourceLayout, const VkImage destination, const ImageLayout destinationLayout, const Containers::ArrayView<const ImageCopy> regions): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2_KHR;
    _info.srcImage = source;
    _info.srcImageLayout = VkImageLayout(sourceLayout);
    _info.dstImage = destination;
    _info.dstImageLayout = VkImageLayout(destinationLayout);

    /* Vulkan 1.2.166 doesn't allow anything in VkImageCopy2KHR::pNext yet
       so there's no point in storing the original ImageCopy wrapper */
    static_assert(sizeof(VkImageCopy2KHR) == sizeof(ImageCopy),
        "expecting ImageCopy to have no extra members referenced from pNext");
    Containers::ArrayView<VkImageCopy2KHR> vkImageCopies2;
    _data = Containers::ArrayTuple{
        {NoInit, regions.size(), vkImageCopies2}
    };

    for(std::size_t i = 0; i != regions.size(); ++i) {
        /* Can't use {} with GCC 4.8 here because it tries to initialize the
           first member instead of doing a copy */
        new(vkImageCopies2 + i) VkImageCopy2KHR(ImageCopy{regions[i]});
    }

    _info.regionCount = regions.size();
    _info.pRegions = vkImageCopies2;
}

CopyImageInfo::CopyImageInfo(const VkImage source, const ImageLayout sourceLayout, const VkImage destination, const ImageLayout destinationLayout, const std::initializer_list<ImageCopy> regions): CopyImageInfo{source, sourceLayout, destination, destinationLayout, Containers::arrayView(regions)} {}

CopyImageInfo::CopyImageInfo(NoInitT) noexcept {}

CopyImageInfo::CopyImageInfo(const VkCopyImageInfo2KHR& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

Containers::Array<VkImageCopy> CopyImageInfo::vkImageCopies() const {
    Containers::Array<VkImageCopy> out{NoInit, _info.regionCount};
    for(std::size_t i = 0; i != _info.regionCount; ++i)
        new(out + i) VkImageCopy(vkImageCopy(_info.pRegions[i]));
    return out;
}

BufferImageCopy::BufferImageCopy(const UnsignedLong bufferOffset, const UnsignedInt bufferRowLength, const UnsignedInt bufferImageHeight, const ImageAspect imageAspect, const Int imageLevel, const Int imageLayerOffset, const Int imageLayerCount, const Range3Di& imageRange): _copy{} {
    _copy.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR;
    _copy.bufferOffset = bufferOffset;
    _copy.bufferRowLength = bufferRowLength;
    _copy.bufferImageHeight = bufferImageHeight;
    _copy.imageSubresource.aspectMask = VkImageAspectFlags(imageAspect);
    _copy.imageSubresource.mipLevel = imageLevel;
    _copy.imageSubresource.baseArrayLayer = imageLayerOffset;
    _copy.imageSubresource.layerCount = imageLayerCount;
    _copy.imageOffset = VkOffset3D(imageRange.min());
    _copy.imageExtent = VkExtent3D(imageRange.size());
}

BufferImageCopy::BufferImageCopy(NoInitT) noexcept {}

BufferImageCopy::BufferImageCopy(const VkBufferImageCopy2KHR& copy):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _copy(copy) {}

BufferImageCopy::BufferImageCopy(const VkBufferImageCopy& copy): _copy{
    VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2_KHR,
    nullptr,
    copy.bufferOffset,
    copy.bufferRowLength,
    copy.bufferImageHeight,
    copy.imageSubresource,
    copy.imageOffset,
    copy.imageExtent
} {}

namespace {

/* Used by CopyImageToBufferInfo::vkImageCopies() and
   CopyImageToBufferInfo::vkImageCopies() as well */
VkBufferImageCopy vkBufferImageCopy(const VkBufferImageCopy2KHR& copy) {
    CORRADE_ASSERT(!copy.pNext,
        "Vk::BufferImageCopy: disallowing conversion to VkBufferImageCopy with non-empty pNext to prevent information loss", {});
    return {
        copy.bufferOffset,
        copy.bufferRowLength,
        copy.bufferImageHeight,
        copy.imageSubresource,
        copy.imageOffset,
        copy.imageExtent
    };
}

}

VkBufferImageCopy BufferImageCopy::vkBufferImageCopy() const {
    return Vk::vkBufferImageCopy(_copy);
}

BufferImageCopy1D::BufferImageCopy1D(const UnsignedLong bufferOffset, const ImageAspect aspect, const Int level, const Range1Di& range): BufferImageCopy{bufferOffset, 0, 0, aspect, level, 0, 1, {{range.min(), 0, 0}, {range.max(), 1, 1}}} {}

BufferImageCopy2D::BufferImageCopy2D(const UnsignedLong bufferOffset, const UnsignedInt bufferRowLength, const ImageAspect aspect, const Int level, const Range2Di& range): BufferImageCopy{bufferOffset, bufferRowLength, 0, aspect, level, 0, 1, {{range.min(), 0}, {range.max(), 1}}} {}

BufferImageCopy3D::BufferImageCopy3D(const UnsignedLong bufferOffset, const UnsignedInt bufferRowLength, const UnsignedInt bufferImageHeight, const ImageAspect aspect, const Int level, const Range3Di& range): BufferImageCopy{bufferOffset, bufferRowLength, bufferImageHeight, aspect, level, 0, 1, range} {}

BufferImageCopy1DArray::BufferImageCopy1DArray(const UnsignedLong bufferOffset, const UnsignedInt bufferRowLength, const ImageAspect aspect, const Int level, const Range2Di& range): BufferImageCopy{bufferOffset, bufferRowLength, 0, aspect, level, range.min().y(), range.sizeY(), {{range.min().x(), 0, 0}, {range.max().x(), 1, 1}}} {}

BufferImageCopy2DArray::BufferImageCopy2DArray(const UnsignedLong bufferOffset, const UnsignedInt bufferRowLength, const UnsignedInt bufferImageHeight, const ImageAspect aspect, const Int level, const Range3Di& range): BufferImageCopy{bufferOffset, bufferRowLength, bufferImageHeight, aspect, level, range.min().z(), range.sizeZ(), {{range.min().xy(), 0}, {range.max().xy(), 1}}} {}

BufferImageCopyCubeMap::BufferImageCopyCubeMap(const UnsignedLong bufferOffset, const UnsignedInt bufferRowLength, const UnsignedInt bufferImageHeight, const ImageAspect aspect, const Int level, const Range2Di& range): BufferImageCopy{bufferOffset, bufferRowLength, bufferImageHeight, aspect, level, 0, 6, {{range.min(), 0}, {range.max(), 1}}} {}

BufferImageCopyCubeMapArray::BufferImageCopyCubeMapArray(const UnsignedLong bufferOffset, const UnsignedInt bufferRowLength, const UnsignedInt bufferImageHeight, const ImageAspect aspect, const Int level, const Range3Di& range): BufferImageCopy{bufferOffset, bufferRowLength, bufferImageHeight, aspect, level, range.min().z(), range.sizeZ(), {{range.min().xy(), 0}, {range.max().xy(), 1}}} {}

CopyBufferToImageInfo::CopyBufferToImageInfo(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const Containers::ArrayView<const BufferImageCopy> regions): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2_KHR;
    _info.srcBuffer = source;
    _info.dstImage = destination;
    _info.dstImageLayout = VkImageLayout(destinationLayout);

    /* While not strictly needed right now, storing the original
       BufferImageCopy instances as well to prepare for a future case where
       VkBufferImageCopy2KHR::pNext will reference something stored there (such
       as copy transformation) */
    Containers::ArrayView<BufferImageCopy> wrappers;
    Containers::ArrayView<VkBufferImageCopy2KHR> vkBufferImageCopies2;
    _data = Containers::ArrayTuple{
        {NoInit, regions.size(), wrappers},
        {NoInit, regions.size(), vkBufferImageCopies2}
    };

    for(std::size_t i = 0; i != regions.size(); ++i) {
        new(wrappers + i) BufferImageCopy{regions[i]};
        /* Can't use {} with GCC 4.8 here because it tries to initialize the
           first member instead of doing a copy */
        new(vkBufferImageCopies2 + i) VkBufferImageCopy2KHR(wrappers[i]);
    }

    _info.regionCount = regions.size();
    _info.pRegions = vkBufferImageCopies2;
}

CopyBufferToImageInfo::CopyBufferToImageInfo(NoInitT) noexcept {}

CopyBufferToImageInfo::CopyBufferToImageInfo(const VkCopyBufferToImageInfo2KHR& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

CopyBufferToImageInfo::CopyBufferToImageInfo(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const std::initializer_list<BufferImageCopy> regions): CopyBufferToImageInfo{source, destination, destinationLayout, Containers::arrayView(regions)} {}

Containers::Array<VkBufferImageCopy> CopyBufferToImageInfo::vkBufferImageCopies() const {
    Containers::Array<VkBufferImageCopy> out{NoInit, _info.regionCount};
    for(std::size_t i = 0; i != _info.regionCount; ++i)
        new(out + i) VkBufferImageCopy(vkBufferImageCopy(_info.pRegions[i]));
    return out;
}

CopyBufferToImageInfo1D::CopyBufferToImageInfo1D(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const Containers::ArrayView<const BufferImageCopy1D> regions): CopyBufferToImageInfo{source, destination, destinationLayout, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyBufferToImageInfo1D::CopyBufferToImageInfo1D(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const std::initializer_list<BufferImageCopy1D> regions): CopyBufferToImageInfo1D{source, destination, destinationLayout, Containers::arrayView(regions)} {}

CopyBufferToImageInfo2D::CopyBufferToImageInfo2D(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const Containers::ArrayView<const BufferImageCopy2D> regions): CopyBufferToImageInfo{source, destination, destinationLayout, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyBufferToImageInfo2D::CopyBufferToImageInfo2D(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const std::initializer_list<BufferImageCopy2D> regions): CopyBufferToImageInfo2D{source, destination, destinationLayout, Containers::arrayView(regions)} {}

CopyBufferToImageInfo3D::CopyBufferToImageInfo3D(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const Containers::ArrayView<const BufferImageCopy3D> regions): CopyBufferToImageInfo{source, destination, destinationLayout, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyBufferToImageInfo3D::CopyBufferToImageInfo3D(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const std::initializer_list<BufferImageCopy3D> regions): CopyBufferToImageInfo3D{source, destination, destinationLayout, Containers::arrayView(regions)} {}

CopyBufferToImageInfo1DArray::CopyBufferToImageInfo1DArray(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const Containers::ArrayView<const BufferImageCopy1DArray> regions): CopyBufferToImageInfo{source, destination, destinationLayout, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyBufferToImageInfo1DArray::CopyBufferToImageInfo1DArray(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const std::initializer_list<BufferImageCopy1DArray> regions): CopyBufferToImageInfo1DArray{source, destination, destinationLayout, Containers::arrayView(regions)} {}

CopyBufferToImageInfo2DArray::CopyBufferToImageInfo2DArray(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const Containers::ArrayView<const BufferImageCopy2DArray> regions): CopyBufferToImageInfo{source, destination, destinationLayout, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyBufferToImageInfo2DArray::CopyBufferToImageInfo2DArray(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const std::initializer_list<BufferImageCopy2DArray> regions): CopyBufferToImageInfo2DArray{source, destination, destinationLayout, Containers::arrayView(regions)} {}

CopyBufferToImageInfoCubeMap::CopyBufferToImageInfoCubeMap(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const Containers::ArrayView<const BufferImageCopyCubeMap> regions): CopyBufferToImageInfo{source, destination, destinationLayout, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyBufferToImageInfoCubeMap::CopyBufferToImageInfoCubeMap(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const std::initializer_list<BufferImageCopyCubeMap> regions): CopyBufferToImageInfoCubeMap{source, destination, destinationLayout, Containers::arrayView(regions)} {}

CopyBufferToImageInfoCubeMapArray::CopyBufferToImageInfoCubeMapArray(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const Containers::ArrayView<const BufferImageCopyCubeMapArray> regions): CopyBufferToImageInfo{source, destination, destinationLayout, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyBufferToImageInfoCubeMapArray::CopyBufferToImageInfoCubeMapArray(const VkBuffer source, const VkImage destination, const ImageLayout destinationLayout, const std::initializer_list<BufferImageCopyCubeMapArray> regions): CopyBufferToImageInfoCubeMapArray{source, destination, destinationLayout, Containers::arrayView(regions)} {}

CopyImageToBufferInfo::CopyImageToBufferInfo(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const Containers::ArrayView<const BufferImageCopy> regions): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2_KHR;
    _info.srcImage = source;
    _info.srcImageLayout = VkImageLayout(sourceLayout);
    _info.dstBuffer = destination;

    /* While not strictly needed right now, storing the original
       BufferImageCopy instances as well to prepare for a future case where
       VkBufferImageCopy2KHR::pNext will reference something stored there (such
       as copy transformation) */
    Containers::ArrayView<BufferImageCopy> wrappers;
    Containers::ArrayView<VkBufferImageCopy2KHR> vkBufferImageCopies2;
    _data = Containers::ArrayTuple{
        {NoInit, regions.size(), wrappers},
        {NoInit, regions.size(), vkBufferImageCopies2}
    };

    for(std::size_t i = 0; i != regions.size(); ++i) {
        new(wrappers + i) BufferImageCopy{regions[i]};
        /* Can't use {} with GCC 4.8 here because it tries to initialize the
           first member instead of doing a copy */
        new(vkBufferImageCopies2 + i) VkBufferImageCopy2KHR(wrappers[i]);
    }

    _info.regionCount = regions.size();
    _info.pRegions = vkBufferImageCopies2;
}

CopyImageToBufferInfo::CopyImageToBufferInfo(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const std::initializer_list<BufferImageCopy> regions): CopyImageToBufferInfo{source, sourceLayout, destination, Containers::arrayView(regions)} {}

CopyImageToBufferInfo::CopyImageToBufferInfo(NoInitT) noexcept {}

CopyImageToBufferInfo::CopyImageToBufferInfo(const VkCopyImageToBufferInfo2KHR& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

Containers::Array<VkBufferImageCopy> CopyImageToBufferInfo::vkBufferImageCopies() const {
    Containers::Array<VkBufferImageCopy> out{NoInit, _info.regionCount};
    for(std::size_t i = 0; i != _info.regionCount; ++i)
        new(out + i) VkBufferImageCopy(vkBufferImageCopy(_info.pRegions[i]));
    return out;
}

CopyImageToBufferInfo1D::CopyImageToBufferInfo1D(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const Containers::ArrayView<const BufferImageCopy1D> regions): CopyImageToBufferInfo{source, sourceLayout, destination, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyImageToBufferInfo1D::CopyImageToBufferInfo1D(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const std::initializer_list<BufferImageCopy1D> regions): CopyImageToBufferInfo1D{source, sourceLayout, destination, Containers::arrayView(regions)} {}

CopyImageToBufferInfo2D::CopyImageToBufferInfo2D(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const Containers::ArrayView<const BufferImageCopy2D> regions): CopyImageToBufferInfo{source, sourceLayout, destination, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyImageToBufferInfo2D::CopyImageToBufferInfo2D(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const std::initializer_list<BufferImageCopy2D> regions): CopyImageToBufferInfo2D{source, sourceLayout, destination, Containers::arrayView(regions)} {}

CopyImageToBufferInfo3D::CopyImageToBufferInfo3D(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const Containers::ArrayView<const BufferImageCopy3D> regions): CopyImageToBufferInfo{source, sourceLayout, destination, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyImageToBufferInfo3D::CopyImageToBufferInfo3D(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const std::initializer_list<BufferImageCopy3D> regions): CopyImageToBufferInfo3D{source, sourceLayout, destination, Containers::arrayView(regions)} {}

CopyImageToBufferInfo1DArray::CopyImageToBufferInfo1DArray(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const Containers::ArrayView<const BufferImageCopy1DArray> regions): CopyImageToBufferInfo{source, sourceLayout, destination, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyImageToBufferInfo1DArray::CopyImageToBufferInfo1DArray(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const std::initializer_list<BufferImageCopy1DArray> regions): CopyImageToBufferInfo1DArray{source, sourceLayout, destination, Containers::arrayView(regions)} {}

CopyImageToBufferInfo2DArray::CopyImageToBufferInfo2DArray(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const Containers::ArrayView<const BufferImageCopy2DArray> regions): CopyImageToBufferInfo{source, sourceLayout, destination, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyImageToBufferInfo2DArray::CopyImageToBufferInfo2DArray(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const std::initializer_list<BufferImageCopy2DArray> regions): CopyImageToBufferInfo2DArray{source, sourceLayout, destination, Containers::arrayView(regions)} {}

CopyImageToBufferInfoCubeMap::CopyImageToBufferInfoCubeMap(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const Containers::ArrayView<const BufferImageCopyCubeMap> regions): CopyImageToBufferInfo{source, sourceLayout, destination, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyImageToBufferInfoCubeMap::CopyImageToBufferInfoCubeMap(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const std::initializer_list<BufferImageCopyCubeMap> regions): CopyImageToBufferInfoCubeMap{source, sourceLayout, destination, Containers::arrayView(regions)} {}

CopyImageToBufferInfoCubeMapArray::CopyImageToBufferInfoCubeMapArray(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const Containers::ArrayView<const BufferImageCopyCubeMapArray> regions): CopyImageToBufferInfo{source, sourceLayout, destination, Containers::arrayCast<const BufferImageCopy>(regions)} {}

CopyImageToBufferInfoCubeMapArray::CopyImageToBufferInfoCubeMapArray(const VkImage source, const ImageLayout sourceLayout, const VkBuffer destination, const std::initializer_list<BufferImageCopyCubeMapArray> regions): CopyImageToBufferInfoCubeMapArray{source, sourceLayout, destination, Containers::arrayView(regions)} {}

CommandBuffer& CommandBuffer::clearColorImage(const VkImage image, const ImageLayout layout, const Color4& color) {
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    /* Why this is passed via a pointer, why?! */
    const VkClearColorValue clear(color);
    (**_device).CmdClearColorImage(_handle, image, VkImageLayout(layout), &clear, 1, &range);
    return *this;
}

CommandBuffer& CommandBuffer::clearColorImage(const VkImage image, const ImageLayout layout, const Vector4i& color) {
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    /* Why this is passed via a pointer, why?! */
    const VkClearColorValue clear(color);
    (**_device).CmdClearColorImage(_handle, image, VkImageLayout(layout), &clear, 1, &range);
    return *this;
}

CommandBuffer& CommandBuffer::clearColorImage(const VkImage image, const ImageLayout layout, const Vector4ui& color) {
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    /* Why this is passed via a pointer, why?! */
    const VkClearColorValue clear(color);
    (**_device).CmdClearColorImage(_handle, image, VkImageLayout(layout), &clear, 1, &range);
    return *this;
}

CommandBuffer& CommandBuffer::clearDepthStencilImage(const VkImage image, const ImageLayout layout, const Float depth, const UnsignedInt stencil) {
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    /* Why this is passed via a pointer, why?! */
    const VkClearDepthStencilValue clear{depth, stencil};
    (**_device).CmdClearDepthStencilImage(_handle, image, VkImageLayout(layout), &clear, 1, &range);
    return *this;
}

CommandBuffer& CommandBuffer::clearDepthImage(const VkImage image, const ImageLayout layout, const Float depth) {
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    /* Why this is passed via a pointer, why?! */
    const VkClearDepthStencilValue clear{depth, 0};
    (**_device).CmdClearDepthStencilImage(_handle, image, VkImageLayout(layout), &clear, 1, &range);
    return *this;
}

CommandBuffer& CommandBuffer::clearStencilImage(const VkImage image, const ImageLayout layout, const UnsignedInt stencil) {
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    /* Why this is passed via a pointer, why?! Also, the depth value will get
       unused anyway, but I think it's good to maintain the fact that default
       depth clear value is 1.0. */
    const VkClearDepthStencilValue clear{1.0f, stencil};
    (**_device).CmdClearDepthStencilImage(_handle, image, VkImageLayout(layout), &clear, 1, &range);
    return *this;
}

CommandBuffer& CommandBuffer::copyImage(const CopyImageInfo& info) {
    _device->state().cmdCopyImageImplementation(*this, info);
    return *this;
}

namespace {

/* See the "swiftshader-image-copy-extent-instead-of-layers" workaround for
   more info */
void fixupImageCopySwiftShader(VkImageSubresourceLayers& subresource, VkOffset3D& offset, VkExtent3D& extent) {
    /* Not a layered image, nothing to do */
    if(subresource.baseArrayLayer == 0 && subresource.layerCount == 1) return;

    /* When copying 2D array to 3D, depth is already at the value we want it to
       be */
    CORRADE_INTERNAL_ASSERT(offset.z == 0 && (extent.depth == 1 || extent.depth == subresource.layerCount));

    /* Put the layer info into the third extent dimension instead of the layer
       fields, as those seem to be interpreted in a wrong way. However those
       still need to be set to values that make sense in total, otherwise nasty
       crashes happen.

       Fortunately this works for 1D array images as well, and we don't need to
       do extra voodoo to detect if the image is 1D to use y / width instead of
       z / depth. */
    offset.z = subresource.baseArrayLayer;
    extent.depth = subresource.layerCount;
    subresource.baseArrayLayer = 0;
    subresource.layerCount = 1;
}

}

void CommandBuffer::copyImageImplementationDefault(CommandBuffer& self, const CopyImageInfo& info) {
    CORRADE_ASSERT(!info->pNext,
        "Vk::CommandBuffer::copyImage(): disallowing extraction of CopyImageInfo with non-empty pNext to prevent information loss", );
    return (**self._device).CmdCopyImage(self, info->srcImage, info->srcImageLayout, info->dstImage, info->dstImageLayout, info->regionCount, info.vkImageCopies());
}

void CommandBuffer::copyImageImplementationSwiftShader(CommandBuffer& self, const CopyImageInfo& info) {
    CORRADE_ASSERT(!info->pNext,
        "Vk::CommandBuffer::copyImage(): disallowing extraction of CopyImageInfo with non-empty pNext to prevent information loss", );

    Containers::Array<VkImageCopy> copies = info.vkImageCopies();
    for(VkImageCopy& copy: copies) {
        fixupImageCopySwiftShader(copy.srcSubresource, copy.srcOffset, copy.extent);
        fixupImageCopySwiftShader(copy.dstSubresource, copy.dstOffset, copy.extent);
    }

    return (**self._device).CmdCopyImage(self, info->srcImage, info->srcImageLayout, info->dstImage, info->dstImageLayout, info->regionCount, copies);
}

void CommandBuffer::copyImageImplementationKHR(CommandBuffer& self, const CopyImageInfo& info) {
    return (**self._device).CmdCopyImage2KHR(self, info);
}

CommandBuffer& CommandBuffer::copyBufferToImage(const CopyBufferToImageInfo& info) {
    _device->state().cmdCopyBufferToImageImplementation(*this, info);
    return *this;
}

void CommandBuffer::copyBufferToImageImplementationDefault(CommandBuffer& self, const CopyBufferToImageInfo& info) {
    CORRADE_ASSERT(!info->pNext,
        "Vk::CommandBuffer::copyBufferToImage(): disallowing extraction of CopyBufferToImageInfo with non-empty pNext to prevent information loss", );
    return (**self._device).CmdCopyBufferToImage(self, info->srcBuffer, info->dstImage, info->dstImageLayout, info->regionCount, info.vkBufferImageCopies());
}

void CommandBuffer::copyBufferToImageImplementationSwiftShader(CommandBuffer& self, const CopyBufferToImageInfo& info) {
    CORRADE_ASSERT(!info->pNext,
        "Vk::CommandBuffer::copyBufferToImage(): disallowing extraction of CopyBufferToImageInfo with non-empty pNext to prevent information loss", );

    Containers::Array<VkBufferImageCopy> copies = info.vkBufferImageCopies();
    for(VkBufferImageCopy& copy: copies) {
        fixupImageCopySwiftShader(copy.imageSubresource, copy.imageOffset, copy.imageExtent);
    }

    return (**self._device).CmdCopyBufferToImage(self, info->srcBuffer, info->dstImage, info->dstImageLayout, info->regionCount, copies);
}

void CommandBuffer::copyBufferToImageImplementationKHR(CommandBuffer& self, const CopyBufferToImageInfo& info) {
    return (**self._device).CmdCopyBufferToImage2KHR(self, info);
}

CommandBuffer& CommandBuffer::copyImageToBuffer(const CopyImageToBufferInfo& info) {
    _device->state().cmdCopyImageToBufferImplementation(*this, info);
    return *this;
}

void CommandBuffer::copyImageToBufferImplementationDefault(CommandBuffer& self, const CopyImageToBufferInfo& info) {
    CORRADE_ASSERT(!info->pNext,
        "Vk::CommandBuffer::copyImageToBuffer(): disallowing extraction of CopyImageToBufferInfo with non-empty pNext to prevent information loss", );
    return (**self._device).CmdCopyImageToBuffer(self, info->srcImage, info->srcImageLayout, info->dstBuffer, info->regionCount, info.vkBufferImageCopies());
}

void CommandBuffer::copyImageToBufferImplementationSwiftShader(CommandBuffer& self, const CopyImageToBufferInfo& info) {
    CORRADE_ASSERT(!info->pNext,
        "Vk::CommandBuffer::copyImageToBuffer(): disallowing extraction of CopyImageToBufferInfo with non-empty pNext to prevent information loss", );

    Containers::Array<VkBufferImageCopy> copies = info.vkBufferImageCopies();
    for(VkBufferImageCopy& copy: copies) {
        fixupImageCopySwiftShader(copy.imageSubresource, copy.imageOffset, copy.imageExtent);
    }

    return (**self._device).CmdCopyImageToBuffer(self, info->srcImage, info->srcImageLayout, info->dstBuffer, info->regionCount, copies);
}

void CommandBuffer::copyImageToBufferImplementationKHR(CommandBuffer& self, const CopyImageToBufferInfo& info) {
    return (**self._device).CmdCopyImageToBuffer2KHR(self, info);
}

}}
