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

#include "Image.h"

#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Integration.h"
#include "Magnum/Vk/Result.h"

namespace Magnum { namespace Vk {

ImageCreateInfo::ImageCreateInfo(const VkImageType type, const ImageUsages usages, const VkFormat format, const Vector3i& size, const Int layers, const Int levels, const Int samples, const Flags flags): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    _info.flags = VkImageCreateFlags(flags);
    _info.imageType = type;
    _info.format = format;
    _info.extent = VkExtent3D(size);
    _info.mipLevels = levels;
    _info.arrayLayers = layers;
    _info.samples = VkSampleCountFlagBits(samples);
    _info.tiling = VK_IMAGE_TILING_OPTIMAL;
    _info.usage = VkImageUsageFlags(usages);
    /* _info.sharingMode is implicitly VK_SHARING_MODE_EXCLUSIVE;
       _info.queueFamilyIndexCount and _info.pQueueFamilyIndices should be
       filled only for VK_SHARING_MODE_CONCURRENT */
    /* _info.initialLayout is implicitly VK_IMAGE_LAYOUT_UNDEFINED. The only
       other possible value is VK_IMAGE_LAYOUT_PREDEFINED, which however also
       needs VK_IMAGE_TILING_LINEAR, one sample and possibly other restrictions
       -- https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkImageCreateInfo.html#_description

       Such images need to be allocated from host-visible memory which on
       discrete GPUs is not fast for device access and thus is recommended to
       go through a staging buffer (not image) instead. This is however still
       useful for iGPUs, as the the memory is shared and this avoid an
       expensive extra copy. */
}

ImageCreateInfo::ImageCreateInfo(NoInitT) noexcept {}

ImageCreateInfo::ImageCreateInfo(const VkImageCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

Image Image::wrap(Device& device, const VkImage handle, const HandleFlags flags) {
    Image out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;
    return out;
}

Image::Image(Device& device, const ImageCreateInfo& info, NoAllocateT): _device{&device}, _flags{HandleFlag::DestroyOnDestruction} {
    MAGNUM_VK_INTERNAL_ASSERT_RESULT(device->CreateImage(device, info, nullptr, &_handle));
}

Image::Image(NoCreateT): _device{}, _handle{} {}

Image::Image(Image&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags} {
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
    return *this;
}

VkImage Image::release() {
    const VkImage handle = _handle;
    _handle = {};
    return handle;
}

}}
