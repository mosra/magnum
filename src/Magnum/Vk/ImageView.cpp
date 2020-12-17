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

#include "ImageView.h"
#include "ImageViewCreateInfo.h"

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Image.h"

namespace Magnum { namespace Vk {

namespace {

/* Vulkan, it would kill you if 0 was a valid default, right?! ffs */
/** @todo this might be useful elsewhere as well */
VkImageAspectFlags aspectFor(const VkFormat format) {
    if(format == VK_FORMAT_D16_UNORM_S8_UINT ||
       format == VK_FORMAT_D24_UNORM_S8_UINT ||
       format == VK_FORMAT_D32_SFLOAT_S8_UINT)
        return VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT;
    if(format == VK_FORMAT_D16_UNORM ||
       format == VK_FORMAT_D32_SFLOAT)
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    if(format == VK_FORMAT_S8_UINT)
        return VK_IMAGE_ASPECT_STENCIL_BIT;

    /** @todo planar formats */

    return VK_IMAGE_ASPECT_COLOR_BIT;
}

}

ImageViewCreateInfo::ImageViewCreateInfo(const VkImageViewType type, const VkImage image, const VkFormat format, const UnsignedInt layerOffset, const UnsignedInt layerCount, const UnsignedInt levelOffset, const UnsignedInt levelCount, const Flags flags): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    _info.flags = VkImageViewCreateFlags(flags);
    _info.image = image;
    _info.viewType = type;
    _info.format = format;
    _info.subresourceRange.aspectMask = aspectFor(format);
    _info.subresourceRange.baseMipLevel = levelOffset;
    _info.subresourceRange.levelCount = levelCount;
    _info.subresourceRange.baseArrayLayer = layerOffset;
    _info.subresourceRange.layerCount = layerCount;
}

ImageViewCreateInfo::ImageViewCreateInfo(const VkImageViewType type, Image& image, const UnsignedInt layerOffset, const UnsignedInt layerCount, const UnsignedInt levelOffset, const UnsignedInt levelCount, const Flags flags): ImageViewCreateInfo{type, image, image.format(), layerOffset, layerCount, levelOffset, levelCount, flags} {
    CORRADE_ASSERT(image.format(),
        "Vk::ImageViewCreateInfo: the image has unknown format, you have to specify it explicitly", );
}

ImageViewCreateInfo::ImageViewCreateInfo(NoInitT) noexcept {}

ImageViewCreateInfo::ImageViewCreateInfo(const VkImageViewCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

ImageView ImageView::wrap(Device& device, const VkImageView handle, const HandleFlags flags) {
    ImageView out{NoCreate};
    out._device = &device;
    out._handle = handle;
    out._flags = flags;
    return out;
}

ImageView::ImageView(Device& device, const ImageViewCreateInfo& info): _device{&device}, _flags{HandleFlag::DestroyOnDestruction} {
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(device->CreateImageView(device, info, nullptr, &_handle));
}

ImageView::ImageView(NoCreateT): _device{}, _handle{} {}

ImageView::ImageView(ImageView&& other) noexcept: _device{other._device}, _handle{other._handle}, _flags{other._flags} {
    other._handle = {};
}

ImageView::~ImageView() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        (**_device).DestroyImageView(*_device, _handle, nullptr);
}

ImageView& ImageView::operator=(ImageView&& other) noexcept {
    using std::swap;
    swap(other._device, _device);
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    return *this;
}

VkImageView ImageView::release() {
    const VkImageView handle = _handle;
    _handle = {};
    return handle;
}

}}
