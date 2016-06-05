#ifndef Magnum_Vk_Image_h
#define Magnum_Vk_Image_h
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

/** @file
 * @brief Class @ref Magnum::Vk::Image
 */
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/DeviceMemory.h"
#include "Magnum/Vk/Math.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

class Image;

enum class ImageUsageFlag: UnsignedInt {
    TransferSrc = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
    TransferDst = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    Sampled = VK_IMAGE_USAGE_SAMPLED_BIT,
    Storage = VK_IMAGE_USAGE_STORAGE_BIT,
    ColorAttachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    StencilAttachment = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    TransientAttachment = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
};

typedef Corrade::Containers::EnumSet<ImageUsageFlag> ImageUsageFlags;

CORRADE_ENUMSET_OPERATORS(ImageUsageFlags)

enum class Access: UnsignedInt {
    IndirectCommandRead = VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
    IndexRead = VK_ACCESS_INDEX_READ_BIT,
    VertexAttributeRead = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
    UniformRead = VK_ACCESS_UNIFORM_READ_BIT,
    InputAttachmentRead = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
    ShaderRead = VK_ACCESS_SHADER_READ_BIT,
    ShaderWrite = VK_ACCESS_SHADER_WRITE_BIT,
    ColorAttachmentRead = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
    ColorAttachmentWrite = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    DepthStencilAttachmentRead = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    DepthStencilAttachmentWrite = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    TransferRead = VK_ACCESS_TRANSFER_READ_BIT,
    TransferWrite = VK_ACCESS_TRANSFER_WRITE_BIT,
    HostRead = VK_ACCESS_HOST_READ_BIT,
    HostWrite = VK_ACCESS_HOST_WRITE_BIT,
    MemoryRead = VK_ACCESS_MEMORY_READ_BIT,
    MemoryWrite = VK_ACCESS_MEMORY_WRITE_BIT,
};

typedef Containers::EnumSet<Access> AccessFlags;

CORRADE_ENUMSET_OPERATORS(AccessFlags)

enum class ImageLayout: UnsignedInt {
    Undefined = VK_IMAGE_LAYOUT_UNDEFINED,
    General = VK_IMAGE_LAYOUT_GENERAL,
    ColorAttachmentOptimal = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    DepthStencilAttachmentOptimal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    DepthStencilReadOnlyOptimal = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    ShaderReadOnlyOptimal = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    TransferSrcOptimal = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    TransferDstOptimal = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    Preinitialized = VK_IMAGE_LAYOUT_PREINITIALIZED,
    PresentSrc = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
};

enum class ObjectFlag: UnsignedByte {
    /** Delete the object on destruction. */
    DeleteOnDestruction = 1 << 1
};

typedef Containers::EnumSet<ObjectFlag> ObjectFlags;

CORRADE_ENUMSET_OPERATORS(ObjectFlags)

class MAGNUM_VK_EXPORT Image {
    public:

        Image(Device& device, VkImage vkImage):
            _device{device},
            _image{vkImage},
            _flags{}
        {}

        Image(Device& device, Vector3ui extent, VkFormat format, ImageUsageFlags usage):
            _device{device}, _flags{ObjectFlag::DeleteOnDestruction}
        {
            VkImageCreateInfo image = {};
            image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image.flags = 0;
            image.pNext = nullptr;
            image.imageType = VK_IMAGE_TYPE_2D;
            image.format = format;
            image.extent = VkExtent3D(extent);
            image.samples = VK_SAMPLE_COUNT_1_BIT;
            image.mipLevels = 1;
            image.arrayLayers = 1;
            image.tiling = VK_IMAGE_TILING_OPTIMAL;
            image.usage = UnsignedInt(usage);

            VkResult err = vkCreateImage(_device, &image, nullptr, &_image);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        /** @brief Copying is not allowed */
        Image(const Image&) = delete;

        /** @brief Move constructor */
        Image(Image&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyImage}
         */
        ~Image();

        /** @brief Copying is not allowed */
        Image& operator=(const Image&) = delete;

        /** @brief Move assignment is not allowed */
        Image& operator=(Image&&) = delete;

        operator VkImage() const {
            return _image;
        }

        VkMemoryRequirements getMemoryRequirements() {
            VkMemoryRequirements memReqs;
            vkGetImageMemoryRequirements(_device, _image, &memReqs);

            return memReqs;
        }

        Image& bindImageMemory(DeviceMemory& memory, UnsignedLong memoryOffset=0) {
            VkResult err = vkBindImageMemory(_device, _image, memory, memoryOffset);
            MAGNUM_VK_ASSERT_ERROR(err);

            return *this;
        }

    private:
        Device& _device;
        VkImage _image;
        ObjectFlags _flags;
};

struct ImageMemoryBarrier {

    ImageMemoryBarrier(const Image& image, ImageLayout src, ImageLayout dst,
                       VkImageSubresourceRange range,
                       AccessFlags srcAccessMask = {},
                       AccessFlags destAccessMask = {}):
        _imageMemoryBarrier{
            VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            nullptr,
            VkAccessFlags(srcAccessMask),
            VkAccessFlags(destAccessMask),
            VkImageLayout(src),
            VkImageLayout(dst),
            VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED,
            image,
            range
    }
    {}

    operator VkImageMemoryBarrier() const {
        return _imageMemoryBarrier;
    }

private:
    VkImageMemoryBarrier _imageMemoryBarrier;
};

}}

#endif
