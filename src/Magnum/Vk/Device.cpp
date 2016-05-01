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

#include "Device.h"

#include "Magnum/Vk/CommandPool.h"
#include "Magnum/Vk/Queue.h"


namespace Magnum { namespace Vk {

Device::Device(PhysicalDevice& physicalDevice, VkDeviceQueueCreateInfo& requestedQueues):
    _physicalDevice(physicalDevice)
{
    constexpr const char* enabledExtensions[]{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    constexpr const char* validationLayerNames[] = {"VK_LAYER_LUNARG_standard_validation"};

    // TODO: Allow disabling validation layers

    VkDeviceCreateInfo deviceCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr,
        0, /* flags */
        1, &requestedQueues,
        1, validationLayerNames,
        1, enabledExtensions,
        nullptr};

    vkCreateDevice(physicalDevice.vkPhysicalDevice(), &deviceCreateInfo, nullptr, &_device);
}

Device::~Device() {
    vkDestroyDevice(_device, nullptr);
}

}}
