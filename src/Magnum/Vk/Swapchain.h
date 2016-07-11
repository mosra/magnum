#ifndef Magnum_Vk_Swapchain_h
#define Magnum_Vk_Swapchain_h
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
 * @brief Class @ref Magnum::Vk::Swapchain
 */

#include "Magnum/Magnum.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/ImageView.h"
#include "Magnum/Vk/Semaphore.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

typedef struct {
    std::unique_ptr<Image> image;
    std::unique_ptr<ImageView> view;
} SwapchainBuffer;

class MAGNUM_VK_EXPORT Swapchain {
    public:

        Swapchain(Device& device, CommandBuffer& cb, VkSurfaceKHR surface);

        /** @brief Copying is not allowed */
        Swapchain(const Swapchain&) = delete;

        /** @brief Move constructor */
        Swapchain(Swapchain&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroySwapchain}
         */
        ~Swapchain();

        /** @brief Copying is not allowed */
        Swapchain& operator=(const Swapchain&) = delete;

        /** @brief Move assignment is not allowed */
        Swapchain& operator=(Swapchain&&) = delete;

        VkSwapchainKHR vkSwapchain() {
            return _swapchain;
        }

        Swapchain& acquireNextImage(Semaphore& presentCompleteSemaphore) {
            VkResult err = vkAcquireNextImageKHR(_device.vkDevice(),
                                         _swapchain, UINT64_MAX,
                                         presentCompleteSemaphore.vkSemaphore(),
                                         VkFence(nullptr), &_currentIndex);
            MAGNUM_VK_ASSERT_ERROR(err);

            return *this;
        }

        Swapchain& queuePresent(VkQueue queue, UnsignedInt currentBuffer);

        /**
         * Present the current image to the given
         * TODO
         * @brief queuePresent
         * @param queue
         * @param currentBuffer
         * @param waitSemaphore
         * @return Reference to self (for method chaining)
         */
        Swapchain& queuePresent(Queue& queue, UnsignedInt currentBuffer, Semaphore& waitSemaphore);

        /**
         * @brief Number of images in the swapchain
         */
        UnsignedInt imageCount() const {
            return _buffers.size();
        }

        /**
         * @brief Index of current buffer in swapchain
         */
        UnsignedInt currentIndex() const {
            return _currentIndex;
        }

        /**
         * @brief VkImageView at current index
         */
        Image& image() {
            return *_buffers[_currentIndex].image;
        }

        /**
         * @brief VkImageView at the given index
         */
        Image& image(UnsignedInt index) {
            return *_buffers[index].image;
        }

        /**
         * @brief VkImageView at the given index
         */
        Vk::ImageView& imageView(UnsignedInt index) {
            return *_buffers[index].view;
        }

    private:
        PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
        PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
        PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
        PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;

        PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
        PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
        PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
        PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
        PFN_vkQueuePresentKHR vkQueuePresentKHR;

        Device& _device;
        VkSurfaceKHR _surface;
        VkSwapchainKHR _swapchain;
        std::vector<SwapchainBuffer> _buffers;
        UnsignedInt _currentIndex;
};

}}

#endif
