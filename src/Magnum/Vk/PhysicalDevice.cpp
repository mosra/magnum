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

#include "PhysicalDevice.h"

#include <vector>

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>


namespace Magnum { namespace Vk {

PhysicalDevice::~PhysicalDevice() {
}

VkFormat PhysicalDevice::getSupportedDepthFormat() {
    // Since all depth formats may be optional, we need to find a suitable depth format to use
    // Start with the highest precision packed format
    std::vector<VkFormat> depthFormats = {
       VK_FORMAT_D32_SFLOAT_S8_UINT,
       VK_FORMAT_D32_SFLOAT,
       VK_FORMAT_D24_UNORM_S8_UINT,
       VK_FORMAT_D16_UNORM_S8_UINT,
       VK_FORMAT_D16_UNORM
    };

    VkFormatProperties formatProps;
    for(auto& format : depthFormats) {
       vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &formatProps);
       // Format must support depth stencil attachment for optimal tiling
       if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
           return format;
       }
    }

    return VK_FORMAT_UNDEFINED;
}

UnsignedInt PhysicalDevice::getQueueFamilyIndex(QueueFamily family) {
    // Find a queue that supports graphics
    UnsignedInt queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueCount, nullptr);

    if(queueCount <= 0) { // TODO I belive there allways is at least one queue of a certain specified type, lookup in spec
        Error() << "Physical Device does not provide queues"; // TODO
    }

    std::vector<VkQueueFamilyProperties> queueProps; // TODO Use NoInit Containers::Array
    queueProps.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueCount, queueProps.data());

    for(UnsignedInt i = 0; i < queueCount; ++i) {
        if(queueProps[i].queueFlags & VkQueueFlags(family)) {
            return i;
        }
    }

    CORRADE_ASSERT(false, "The device does not support the given queue family.", -1);  // TODO
}

UnsignedInt PhysicalDevice::getMemoryType(UnsignedInt typeBits, VkFlags properties) {
    for (uint32_t i = 0; i < 32; i++) {
        if((typeBits & 1) != 0) {
            if((_deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        typeBits >>= 1;
    }

    CORRADE_ASSERT(false, "Physical devices does not support memory with given properties.", -1);  // TODO
}

}}
