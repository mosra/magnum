#ifndef Magnum_Vk_Texture_h
#define Magnum_Vk_Texture_h
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
 * @brief Class @ref Magnum::Vk::Texture, @ref Magnum::Vk::Texture1D, @ref Magnum::Vk::Texture2D, @ref Magnum::Vk::Texture3D
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector.h"
#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/ImageView.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

/**
 @brief Texture

 A Sampler combined with an Image.
*/
class MAGNUM_VK_EXPORT Texture {
    public:

        Texture(Device& device, std::unique_ptr<ImageView>&& imageView, ImageLayout layout, Int numMipLevels):
            _device{device},
            _imageView{std::move(imageView)},
            _imageLayout{layout},
            _mipLevels{numMipLevels}
        {
            int mipLevels = 0;
            VkSamplerCreateInfo sampler{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, nullptr, 0};
            sampler.magFilter = VK_FILTER_LINEAR;
            sampler.minFilter = VK_FILTER_LINEAR;
            sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler.mipLodBias = 0.0f;
            sampler.compareOp = VK_COMPARE_OP_NEVER;
            sampler.minLod = 0.0f;
            sampler.maxLod = float(mipLevels);
            sampler.maxAnisotropy = 8;
            sampler.anisotropyEnable = VK_TRUE;
            sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

            VkResult err = vkCreateSampler(device, &sampler, nullptr, &_sampler);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        /** @brief Copying is not allowed */
        Texture(const Texture&) = delete;

        /** @brief Move constructor */
        Texture(Texture&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyBuffer}
         */
        ~Texture() {
            vkDestroySampler(_device, _sampler, nullptr);
        }

        /** @brief Copying is not allowed */
        Texture& operator=(const Texture&) = delete;

        /** @brief Move assignment is not allowed */
        Texture& operator=(Texture&&) = delete;

        ImageView& imageView() {
            return *_imageView;
        }

        operator VkSampler() const {
            return _sampler;
        }

        UnsignedInt mipLevels() const {
            return _mipLevels;
        }

        ImageLayout imageLayout() const {
            return _imageLayout;
        }

        VkDescriptorImageInfo getDescriptor() const {
            return VkDescriptorImageInfo{_sampler, *_imageView, VkImageLayout(_imageLayout)};
        }

    private:
        Device& _device;

        VkSampler _sampler;
        std::unique_ptr<Vk::ImageView> _imageView;
        ImageLayout _imageLayout;
        Int _mipLevels;
};

}}

#endif
