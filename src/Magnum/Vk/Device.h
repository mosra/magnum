#ifndef Magnum_Vk_Device_h
#define Magnum_Vk_Device_h
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
 * @brief Class @ref Magnum::Vk::Device
 */

#include <memory>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Context.h"
#include "Magnum/Vk/PhysicalDevice.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

class CommandPool;
class Queue;

class MAGNUM_VK_EXPORT Device {
    public:

        /** @brief Copying is not allowed */
        Device(const Device&) = delete;

        /** @brief Move constructor */
        Device(Device&& other);

        Device(PhysicalDevice& physicalDevice, VkDeviceQueueCreateInfo& requestedQueues);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyDevice}
         */
        ~Device();

        /** @brief Copying is not allowed */
        Device& operator=(const Device&) = delete;

        /** @brief Move assignment is not allowed */
        Device& operator=(Device&&) = delete;

        /**
         * @brief Return the underlying VkDevice handle
         */
        VkDevice vkDevice() {
            return _device;
        }

        /**
         * @return Reference to the PhysicalDevice this Device was created from
         */
        PhysicalDevice& physicalDevice() {
            return _physicalDevice;
        }

        /**
         * @brief Wait until the device is idle
         * @return Reference to self (for method chaining)
         */
        Device& waitIdle() {
            VkResult err = vkDeviceWaitIdle(_device);
            MAGNUM_VK_ASSERT_ERROR(err);
            return *this;
        }

        operator VkDevice() const {
            return _device;
        }

    private:
        VkDevice _device;
        PhysicalDevice& _physicalDevice;
};

}}

#endif
