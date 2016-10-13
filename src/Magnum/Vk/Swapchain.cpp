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

#include "Swapchain.h"

#include "Magnum/Vk/Queue.h"
#include "Magnum/Vk/Format.h"
#include "Magnum/Vk/ImageView.h"

namespace Magnum { namespace Vk {

// TODO Wrap surface
Swapchain::Swapchain(Device& device, CommandBuffer& cb, VkSurfaceKHR surface):
    _device{device},
    _surface{surface},
    _swapchain{VK_NULL_HANDLE},
    _currentIndex{0}
{
    VkDevice vkDevice = _device;

    #define GET_INSTANCE_PROC_ADDR(entrypoint) vk##entrypoint = PFN_vk##entrypoint(vkGetInstanceProcAddr(Vk::Instance::current(), "vk"#entrypoint)); do{if(vk##entrypoint == nullptr) { Error() << "Failed to get function pointer.";} }while(false)
    #define GET_DEVICE_PROC_ADDR(entrypoint) vk##entrypoint = PFN_vk##entrypoint(vkGetDeviceProcAddr(vkDevice, "vk"#entrypoint)); do{ if(vk##entrypoint == nullptr) { Error() << "Failed to get function pointer.";} }while(false)

    GET_INSTANCE_PROC_ADDR(GetPhysicalDeviceSurfaceSupportKHR);
    GET_INSTANCE_PROC_ADDR(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_INSTANCE_PROC_ADDR(GetPhysicalDeviceSurfaceFormatsKHR);
    GET_INSTANCE_PROC_ADDR(GetPhysicalDeviceSurfacePresentModesKHR);

    GET_DEVICE_PROC_ADDR(CreateSwapchainKHR);
    GET_DEVICE_PROC_ADDR(DestroySwapchainKHR);
    GET_DEVICE_PROC_ADDR(GetSwapchainImagesKHR);
    GET_DEVICE_PROC_ADDR(AcquireNextImageKHR);
    GET_DEVICE_PROC_ADDR(QueuePresentKHR);

    #undef GET_INSTANCE_PROC_ADDR
    #undef GET_DEVICE_PROC_ADDR

    VkPhysicalDevice vkPhysicalDevice = _device.physicalDevice();
    VkResult err;

    // Get available queue family properties TODO(squareys): Move to PhysicalDevice
    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice,
                                             &queueCount, nullptr);
    assert(queueCount >= 1);

    // TODO(squareys): Move to PhysicalDevice
    std::vector<VkQueueFamilyProperties> queueProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice,
                                             &queueCount, queueProps.data());

    // TODO(squareys): Move to Device/PhysicalDevice instead
    std::vector<VkBool32> supportsPresent(queueCount);
    for (uint32_t i = 0; i < queueCount; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, surface, &supportsPresent[i]);
    }

    // Search for a graphics and a present queue in the array of queue
    // families, try to find one that supports both TODO(squareys): Move to PhysicalDevice
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueCount; i++) {
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (graphicsQueueNodeIndex == UINT32_MAX) {
                graphicsQueueNodeIndex = i;
            }

            if (supportsPresent[i] == VK_TRUE) {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            } else {
                Warning() << "Queue" << i << "does not support present.";
            }
        }
    }

    if (presentQueueNodeIndex == UINT32_MAX) {
        // If there's no queue that supports both present and graphics
        // try to find a separate present queue
        for (uint32_t i = 0; i < queueCount; ++i) {
            if (supportsPresent[i] == VK_TRUE) {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    // Exit if either a graphics or a presenting queue hasn't been found
    CORRADE_ASSERT(graphicsQueueNodeIndex != UINT32_MAX, "No graphics queue found.", );
    CORRADE_ASSERT(presentQueueNodeIndex != UINT32_MAX, "No present queue found.", );
    CORRADE_ASSERT(graphicsQueueNodeIndex == presentQueueNodeIndex, "Separate graphics and present queues are not supported (yet)", );

    // Get list of supported surface formats TODO(squareys): Move to PhysicalDevice
    UnsignedInt formatCount;
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount, nullptr);
    MAGNUM_VK_ASSERT_ERROR(err);
    CORRADE_ASSERT(formatCount > 0, "The device does not support any surface formats.", );

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    err = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice,
                                               _surface, &formatCount,
                                               surfaceFormats.data());
    MAGNUM_VK_ASSERT_ERROR(err);

    VkColorSpaceKHR colorSpace = surfaceFormats[0].colorSpace;
    Format colorFormat = Vk::Format(VK_FORMAT_R8G8B8A8_UNORM);
    if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
        /* no preferred format */
        colorFormat = Vk::Format(VK_FORMAT_R8G8B8A8_UNORM);
    } else {
        // Always select the first available color format
        // If you need a specific format (e.g. SRGB) you'd need to
        // iterate over the list of available surface format and
        // check for it's presence
        colorFormat = Format(surfaceFormats[0].format);
    }

    // Get physical device surface properties and formats
    VkSurfaceCapabilitiesKHR surfCaps;
    err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice,
                                                    surface, &surfCaps);
    MAGNUM_VK_ASSERT_ERROR(err);

    // Get available present modes
    UnsignedInt presentModeCount;
    err = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice,
                                                    surface, &presentModeCount, nullptr);
    MAGNUM_VK_ASSERT_ERROR(err);
    if (presentModeCount < 1) {
        Error() << "The device does not support any surface present mode."; // TODO: Can this even happen?
    }

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    err = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice,
                                                    surface, &presentModeCount,
                                                    presentModes.data());
    MAGNUM_VK_ASSERT_ERROR(err);

    VkExtent2D swapchainExtent = surfCaps.currentExtent;
    if (surfCaps.currentExtent.width == UnsignedInt(-1)) {
        Error() << "The surface has undefined extents.";
        return;
    }

    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (auto presentMode : presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    UnsignedInt desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
    if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount)) {
        desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
    }

    VkSurfaceTransformFlagsKHR preTransform;
    if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfCaps.currentTransform;
    }

    VkSwapchainKHR oldSwapchain = _swapchain;

    VkSwapchainCreateInfoKHR swapchainCI = {};
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.pNext = nullptr;
    swapchainCI.flags = 0;

    swapchainCI.surface = _surface;
    swapchainCI.imageFormat = VkFormat(colorFormat);
    swapchainCI.imageColorSpace = colorSpace;
    swapchainCI.imageExtent = swapchainExtent;
    swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCI.preTransform = VkSurfaceTransformFlagBitsKHR(preTransform);
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCI.queueFamilyIndexCount = 0;
    swapchainCI.pQueueFamilyIndices = nullptr;
    swapchainCI.presentMode = swapchainPresentMode;
    swapchainCI.oldSwapchain = VK_NULL_HANDLE;
    swapchainCI.clipped = VK_TRUE;
    swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    err = vkCreateSwapchainKHR(vkDevice, &swapchainCI, nullptr, &_swapchain);
    MAGNUM_VK_ASSERT_ERROR(err);

    // If an existing sawp chain is re-created, destroy the old swap chain
    // This also cleans up all the presentable images
    if (oldSwapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(vkDevice, oldSwapchain, nullptr);
    }

    UnsignedInt imageCount = 0;
    err = vkGetSwapchainImagesKHR(vkDevice, _swapchain, &imageCount, nullptr);
    MAGNUM_VK_ASSERT_ERROR(err);

    std::vector<VkImage> images;
    images.resize(imageCount);

    err = vkGetSwapchainImagesKHR(vkDevice, _swapchain, &imageCount, images.data());
    MAGNUM_VK_ASSERT_ERROR(err);

    _buffers.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; i++) {
        _buffers[i].image.reset(new Vk::Image(_device, images[i]));

        // Create an image barrier object
        VkImageMemoryBarrier imageMemoryBarrier = {
            VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            nullptr,
            0, /* src access mask */
            0, /* dst access mask */
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED,
            *_buffers[i].image,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
        };

        vkCmdPipelineBarrier(
            cb,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);

        _buffers[i].view.reset(
            new Vk::ImageView(_device, *_buffers[i].image, colorFormat,
                              VK_IMAGE_VIEW_TYPE_2D, ImageAspect::Color,
                              VkComponentMapping{VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A}
            )
        );
    }
}

Swapchain::~Swapchain() {
    vkDestroySwapchainKHR(_device, _swapchain, nullptr);
    vkDestroySurfaceKHR(Vk::Instance::current(), _surface, nullptr);
}

Swapchain& Swapchain::queuePresent(VkQueue queue, UnsignedInt currentBuffer) {
   VkPresentInfoKHR presentInfo = {};
   presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
   presentInfo.pNext = nullptr;
   presentInfo.swapchainCount = 1;
   presentInfo.pSwapchains = &_swapchain;
   presentInfo.pImageIndices = &currentBuffer;
   VkResult err = vkQueuePresentKHR(queue, &presentInfo);
   MAGNUM_VK_ASSERT_ERROR(err);

   return *this;
}

Swapchain& Swapchain::queuePresent(Queue& queue, UnsignedInt currentBuffer, Semaphore& waitSemaphore) {
    VkSemaphore sem = waitSemaphore;
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapchain;
    presentInfo.pImageIndices = &currentBuffer;
    presentInfo.pWaitSemaphores = &sem;
    presentInfo.waitSemaphoreCount = 1;
    VkResult err = vkQueuePresentKHR(queue.vkQueue(), &presentInfo);
    MAGNUM_VK_ASSERT_ERROR(err);

    return *this;
}

}}
