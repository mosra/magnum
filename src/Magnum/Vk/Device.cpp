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

Device::Device(const PhysicalDevice& physicalDevice,
    std::initializer_list<DeviceQueueCreateInfo> requestedQueues,
    std::initializer_list<const char*> extensions,
    std::initializer_list<const char*> validationLayers,
    const DeviceFeatures& features):
    _physicalDevice(physicalDevice)
{
    VkPhysicalDeviceFeatures vkPhysicalDevice = features;
    std::vector<DeviceQueueCreateInfo> qci{requestedQueues};
    std::vector<const char*> exts{extensions};
    std::vector<const char*> val{validationLayers};

    VkDeviceCreateInfo deviceCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0,
        UnsignedInt(qci.size()), reinterpret_cast<const VkDeviceQueueCreateInfo*>(qci.data()),
        UnsignedInt(val.size()), val.data(),
        UnsignedInt(exts.size()), exts.data(),
        &vkPhysicalDevice};

    vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &_device);

    for(const DeviceQueueCreateInfo& info : requestedQueues) {
        for(UnsignedInt i = 0; i < info.queueCount(); ++i) {
            _queues.emplace_back(new Queue{*this, info.queueFamilyIndex(), i});
        }
    }
}


Device::~Device() {
    vkDestroyDevice(_device, nullptr);
}

}}
