/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/Integration.h"
#include "MagnumExternal/Vulkan/flextVkGlobal.h"

using namespace Magnum;

#define DOXYGEN_IGNORE(...) __VA_ARGS__

int main() {
{
Vk::Device device{NoCreate};
/* [Device-isExtensionEnabled] */
if(device.isExtensionEnabled<Vk::Extensions::EXT::index_type_uint8>()) {
    // keep mesh indices 8bit
} else {
    // convert them to 16bit
}
/* [Device-isExtensionEnabled] */
}

{
Vk::Instance instance;
/* Header included again inside a function, but it's fine as the guards will
   make it empty */
/* [Device-global-function-pointers] */
#include <MagnumExternal/Vulkan/flextVkGlobal.h>

// …

Vk::Device device{DOXYGEN_IGNORE(instance, Vk::DeviceCreateInfo{instance})};
device.populateGlobalFunctionPointers();

VkCommandPool commandPool;
VkCommandPoolCreateInfo info{};
// …
vkCreateCommandPool(device, &info, nullptr, &commandPool);
/* [Device-global-function-pointers] */
}

{
Vk::Instance instance;
/* [Instance-isExtensionEnabled] */
if(instance.isExtensionEnabled<Vk::Extensions::EXT::debug_utils>()) {
    // use the fancy debugging APIs
} else if(instance.isExtensionEnabled<Vk::Extensions::EXT::debug_report>()) {
    // use the non-fancy and deprecated debugging APIs
} else {
    // well, tough luck
}
/* [Instance-isExtensionEnabled] */
}

{
/* Header included again inside a function, but it's fine as the guards will
   make it empty */
/* [Instance-global-function-pointers] */
#include <MagnumExternal/Vulkan/flextVkGlobal.h>

// …

Vk::Instance instance;
instance.populateGlobalFunctionPointers();

VkPhysicalDeviceGroupProperties properties[10];
UnsignedInt count = Containers::arraySize(properties);
vkEnumeratePhysicalDeviceGroupsKHR(instance, &count, properties);
/* [Instance-global-function-pointers] */
}

{
/* [Integration] */
VkOffset2D a{64, 32};
Vector2i b(a);

using namespace Math::Literals;
VkClearColorValue c = VkClearColorValue(0xff9391_srgbf);
/* [Integration] */
static_cast<void>(b);
static_cast<void>(c);
}
}
