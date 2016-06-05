#ifndef Magnum_Vk_Framebuffer_h
#define Magnum_Vk_Framebuffer_h
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
 * @brief Class @ref Magnum::Vk::Framebuffer
 */

#include "Magnum/Magnum.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/RenderPass.h"
#include "Magnum/Vk/ImageView.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

class MAGNUM_VK_EXPORT Framebuffer {
    public:

        Framebuffer(Device& device, RenderPass& renderPass, const Vector3ui& size,
                    const std::initializer_list<std::reference_wrapper<const Vk::ImageView>>& attachments):
            _device{device}
        {
            std::vector<VkImageView> vkAttachments;
            vkAttachments.reserve(attachments.size());
            for (auto& imageView : attachments) {
                vkAttachments.push_back(imageView.get());
            }

            VkFramebufferCreateInfo createInfo = {
                VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                nullptr, 0,
                renderPass,
                vkAttachments.size(),
                vkAttachments.data(),
                size.x(), size.y(), size.z()
            };

            VkResult err = vkCreateFramebuffer(_device, &createInfo, nullptr, &_framebuffer);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        /** @brief Copying is not allowed */
        Framebuffer(const Framebuffer&) = delete;

        /** @brief Move constructor */
        Framebuffer(Framebuffer&& other):
            _device{other._device},
            _framebuffer{other._framebuffer}
        {
            other._framebuffer = VK_NULL_HANDLE;
        }

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyFramebuffer}
         */
        ~Framebuffer();

        /** @brief Copying is not allowed */
        Framebuffer& operator=(const Framebuffer&) = delete;

        /** @brief Move assignment is not allowed */
        Framebuffer& operator=(Framebuffer&&) = delete;

        operator VkFramebuffer() const {
            return _framebuffer;
        }

    private:
        Device& _device;
        VkFramebuffer _framebuffer;
};

}}

#endif
