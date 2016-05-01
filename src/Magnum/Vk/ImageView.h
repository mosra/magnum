#ifndef Magnum_Vk_ImageView_h
#define Magnum_Vk_ImageView_h
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
 * @brief Class @ref Magnum::Vk::ImageView
 */

#include "Magnum/Magnum.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Image.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

enum class ImageAspect: UnsignedInt {
    Color = VK_IMAGE_ASPECT_COLOR_BIT,
    Depth = VK_IMAGE_ASPECT_DEPTH_BIT,
    Stencil = VK_IMAGE_ASPECT_STENCIL_BIT,
    Metadata = VK_IMAGE_ASPECT_METADATA_BIT,
};

typedef Containers::EnumSet<ImageAspect> ImageAspects;

CORRADE_ENUMSET_OPERATORS(ImageAspects)

class MAGNUM_VK_EXPORT ImageView {
    public:

    ImageView(Device& device, Image& image, VkFormat format,
              VkImageViewType type, ImageAspects aspects,
              VkComponentMapping components = {}):
            _device{device}
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.flags = 0;
            createInfo.viewType = type;
            createInfo.format = format;
            createInfo.subresourceRange = {
                VkImageAspectFlags(aspects),
                0, /* base mip level */
                1, /* layer count */
                0, /* base array layer */
                1, /* layer count */
            };
            createInfo.image = image;
            createInfo.components = components;

            VkResult err = vkCreateImageView(_device, &createInfo, nullptr, &_imageView);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        /** @brief Copying is not allowed */
        ImageView(const ImageView&) = delete;

        /** @brief Move constructor */
        ImageView(ImageView&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyImageView}
         */
        ~ImageView();

        /** @brief Copying is not allowed */
        ImageView& operator=(const ImageView&) = delete;

        /** @brief Move assignment is not allowed */
        ImageView& operator=(ImageView&&) = delete;

        operator VkImageView() const {
            return _imageView;
        }

    private:
        Device& _device;
        VkImageView _imageView;
};

}}

#endif
