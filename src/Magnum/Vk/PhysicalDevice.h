#ifndef Magnum_Vk_PhysicalDevice_h
#define Magnum_Vk_PhysicalDevice_h
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
 * @brief Class @ref Magnum::Vk::PhysicalDevice
 */

#include "Magnum/Magnum.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"


namespace Magnum { namespace Vk {

enum class QueueFamily: UnsignedInt {
    Graphics = VK_QUEUE_GRAPHICS_BIT,
    Compute = VK_QUEUE_COMPUTE_BIT,
    Transfer = VK_QUEUE_TRANSFER_BIT,
};

enum class QueueFlag: UnsignedInt {
    SparseBinding = VK_QUEUE_SPARSE_BINDING_BIT,
};


class MAGNUM_VK_EXPORT PhysicalDevice {
    public:

        /** @brief Copying is not allowed */
        PhysicalDevice(const PhysicalDevice&) = delete;

        /** @brief Move constructor */
        PhysicalDevice(PhysicalDevice&& other);

        /**
         * @brief Construct from VkPhysicalDevice
         */
        PhysicalDevice(const VkPhysicalDevice& device):
            _physicalDevice(device)
        {
            // Gather physical device memory properties
            vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &_deviceMemoryProperties);
        }

        /** @brief Destructor */
        ~PhysicalDevice();

        /** @brief Copying is not allowed */
        PhysicalDevice& operator=(const PhysicalDevice&) = delete;

        /** @brief Move assignment is not allowed */
        PhysicalDevice& operator=(PhysicalDevice&&) = delete;

        /**
         * @brief Get the underlying VkPhysicalDevice handle
         */
        VkPhysicalDevice vkPhysicalDevice() {
            return _physicalDevice;
        }

        VkFormat getSupportedDepthFormat();

        UnsignedInt getQueueFamilyIndex(QueueFamily family);

        VkPhysicalDeviceProperties getProperties() {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);

            return deviceProperties;
        }

        UnsignedInt getMemoryType(UnsignedInt typeBits, VkFlags properties);

    private:
        VkPhysicalDevice _physicalDevice;
        VkPhysicalDeviceMemoryProperties _deviceMemoryProperties;
};

}}

#endif
