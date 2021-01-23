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

#include "ImageView.h"
#include "ImageViewCreateInfo.h"

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/PixelFormat.h"

namespace Magnum { namespace Vk {

ImageViewCreateInfo::ImageViewCreateInfo(const VkImageViewType type, const VkImage image, const PixelFormat format, const UnsignedInt layerOffset, const UnsignedInt layerCount, const UnsignedInt levelOffset, const UnsignedInt levelCount, const Flags flags): _info{} {
    _info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    _info.flags = VkImageViewCreateFlags(flags);
    _info.image = image;
    _info.viewType = type;
    _info.format = VkFormat(format);
    _info.subresourceRange.aspectMask = VkImageAspectFlags(imageAspectsFor(format));
    _info.subresourceRange.baseMipLevel = levelOffset;
    _info.subresourceRange.levelCount = levelCount;
    _info.subresourceRange.baseArrayLayer = layerOffset;
    _info.subresourceRange.layerCount = layerCount;
}

ImageViewCreateInfo::ImageViewCreateInfo(const VkImageViewType type, const VkImage image, const Magnum::PixelFormat format, const UnsignedInt layerOffset, const UnsignedInt layerCount, const UnsignedInt levelOffset, const UnsignedInt levelCount, const Flags flags): ImageViewCreateInfo{type, image, pixelFormat(format), layerOffset, layerCount, levelOffset, levelCount, flags} {}

ImageViewCreateInfo::ImageViewCreateInfo(const VkImageViewType type, const VkImage image, const Magnum::CompressedPixelFormat format, const UnsignedInt layerOffset, const UnsignedInt layerCount, const UnsignedInt levelOffset, const UnsignedInt levelCount, const Flags flags): ImageViewCreateInfo{type, image, pixelFormat(format), layerOffset, layerCount, levelOffset, levelCount, flags} {}

ImageViewCreateInfo::ImageViewCreateInfo(const VkImageViewType type, Image& image, const UnsignedInt layerOffset, const UnsignedInt layerCount, const UnsignedInt levelOffset, const UnsignedInt levelCount, const Flags flags): ImageViewCreateInfo{type, image,
    /* Assert here instead of inside the constructor to avoid the
       imageAspectsFor() assert on invalid format blow up first */
    (CORRADE_CONSTEXPR_ASSERT(VkFormat(image.format()),
        "Vk::ImageViewCreateInfo: the image has unknown format, you have to specify it explicitly"), image.format()),
    layerOffset, layerCount, levelOffset, levelCount, flags} {}

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
