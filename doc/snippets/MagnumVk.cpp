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

#include <string>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPool.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/Integration.h"
#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/Queue.h"
#include "Magnum/Vk/Shader.h"
#include "MagnumExternal/Vulkan/flextVkGlobal.h"

using namespace Magnum;

#define DOXYGEN_IGNORE(...) __VA_ARGS__

int main() {

{
/* [wrapping-extending-create-info] */
Vk::InstanceCreateInfo info{DOXYGEN_IGNORE()};

/* Add a custom validation features setup */
VkValidationFeaturesEXT validationFeatures{};
validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
validationFeatures.enabledValidationFeatureCount = 1;
constexpr auto bestPractices = VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT;
validationFeatures.pEnabledValidationFeatures = &bestPractices;
CORRADE_INTERNAL_ASSERT(!info->pNext); // or find the end of the pNext chain
info->pNext = &validationFeatures;
/* [wrapping-extending-create-info] */
}

{
/* [CommandPool-usage] */
Vk::Device device{DOXYGEN_IGNORE(NoCreate)};

Vk::CommandPool graphicsCommandPool{device, Vk::CommandPoolCreateInfo{
    device.properties().pickQueueFamily(Vk::QueueFlag::Graphics)
}};
/* [CommandPool-usage] */

/* [CommandPool-usage-allocate] */
Vk::CommandBuffer commandBuffer = graphicsCommandPool.allocate();

// fill the buffer, submit …
/* [CommandPool-usage-allocate] */
}

{
Vk::Instance instance;
/* [Device-usage-construct-queue] */
Vk::Queue queue{NoCreate};
Vk::Device device{instance, Vk::DeviceCreateInfo{Vk::pickDevice(instance)}
    .addQueues(Vk::QueueFlag::Graphics, {0.0f}, {queue})
};
/* [Device-usage-construct-queue] */
}

{
Vk::Instance instance;
Vk::DeviceProperties properties{NoCreate};
using namespace Containers::Literals;
/* [Device-usage-extensions] */
Vk::Device device{instance, Vk::DeviceCreateInfo{DOXYGEN_IGNORE(properties)}
    DOXYGEN_IGNORE()
    .addEnabledExtensions<                         // predefined extensions
        Vk::Extensions::EXT::index_type_uint8,
        Vk::Extensions::KHR::device_group>()
    .addEnabledExtensions({"VK_NV_mesh_shader"_s}) // can be plain strings too
};
/* [Device-usage-extensions] */
}

{
Vk::Instance instance;
using namespace Containers::Literals;
/* [Device-usage-check-supported] */
Vk::DeviceProperties properties = Vk::pickDevice(instance);
Vk::ExtensionProperties extensions = properties.enumerateExtensionProperties();

Vk::DeviceCreateInfo info{properties};
if(extensions.isSupported<Vk::Extensions::EXT::index_type_uint8>())
    info.addEnabledExtensions<Vk::Extensions::EXT::index_type_uint8>();
if(extensions.isSupported("VK_NV_mesh_shader"_s))
    info.addEnabledExtensions({"VK_NV_mesh_shader"_s});
DOXYGEN_IGNORE()
/* [Device-usage-check-supported] */
}

{
Vk::Instance instance;
VkQueryPool pool{};
/* [Device-function-pointers] */
Vk::Device device{DOXYGEN_IGNORE(NoCreate)};

// ...
device->ResetQueryPoolEXT(device, DOXYGEN_IGNORE(pool, 0, 0));
/* [Device-function-pointers] */
}

{
VkQueryPool pool{};
/* Header included again inside a function, but it's fine as the guards will
   make it empty */
/* [Device-global-function-pointers] */
#include <MagnumExternal/Vulkan/flextVkGlobal.h>

DOXYGEN_IGNORE()

Vk::Device device{DOXYGEN_IGNORE(NoCreate)};
device.populateGlobalFunctionPointers();

DOXYGEN_IGNORE()
vkResetQueryPoolEXT(device, DOXYGEN_IGNORE(pool, 0, 0));
/* [Device-global-function-pointers] */
}

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
int argc{};
const char** argv{};
/* [Instance-usage-minimal] */
Vk::Instance instance{{argc, argv}};
/* [Instance-usage-minimal] */
}

{
int argc{};
const char** argv{};
/* [Instance-usage] */
using namespace Containers::Literals;

Vk::Instance instance{Vk::InstanceCreateInfo{argc, argv}
    .setApplicationInfo("My Vulkan Application"_s, Vk::version(1, 2, 3))
};
/* [Instance-usage] */
}

{
int argc{};
const char** argv{};
using namespace Containers::Literals;
/* [Instance-usage-layers-extensions] */
Vk::Instance instance{Vk::InstanceCreateInfo{argc, argv}
    DOXYGEN_IGNORE()
    .addEnabledLayers({"VK_LAYER_KHRONOS_validation"_s})
    .addEnabledExtensions<                          // predefined extensions
        Vk::Extensions::EXT::debug_report,
        Vk::Extensions::KHR::external_fence_capabilities>()
    .addEnabledExtensions({"VK_KHR_xcb_surface"_s}) // can be plain strings too
};
/* [Instance-usage-layers-extensions] */
}

{
int argc{};
const char** argv{};
using namespace Containers::Literals;
/* [Instance-usage-check-supported] */
/* Query layer and extension support */
Vk::LayerProperties layers = Vk::enumerateLayerProperties();
Vk::InstanceExtensionProperties extensions =
    /* ... including extensions exposed only by the extra layers */
    Vk::enumerateInstanceExtensionProperties(layers.names());

/* Enable only those that are supported */
Vk::InstanceCreateInfo info{argc, argv, &layers, &extensions};
if(layers.isSupported("VK_LAYER_KHRONOS_validation"_s))
    info.addEnabledLayers({"VK_LAYER_KHRONOS_validation"_s});
if(extensions.isSupported<Vk::Extensions::EXT::debug_report>())
    info.addEnabledExtensions<Vk::Extensions::EXT::debug_report>();
DOXYGEN_IGNORE()

Vk::Instance instance{info};
/* [Instance-usage-check-supported] */
}

{
/* [Instance-function-pointers] */
Vk::Instance instance{DOXYGEN_IGNORE()};

VkPhysicalDeviceGroupPropertiesKHR properties[10];
UnsignedInt count = Containers::arraySize(properties);
instance->EnumeratePhysicalDeviceGroupsKHR(instance, &count, properties);
/* [Instance-function-pointers] */
}

{
Vk::Instance instance;
/* Header included again inside a function, but it's fine as the guards will
   make it empty */
/* [Instance-global-function-pointers] */
#include <MagnumExternal/Vulkan/flextVkGlobal.h>

DOXYGEN_IGNORE()

instance.populateGlobalFunctionPointers();

VkPhysicalDeviceGroupPropertiesKHR properties[10];
UnsignedInt count = Containers::arraySize(properties);
vkEnumeratePhysicalDeviceGroupsKHR(instance, &count, properties);
/* [Instance-global-function-pointers] */
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
Vk::Device device{DOXYGEN_IGNORE(NoCreate)};
/* [Shader-usage] */
Vk::ShaderCreateInfo info{
    CORRADE_INTERNAL_ASSERT_EXPRESSION(Utility::Directory::read("shader.spv"))
};

DOXYGEN_IGNORE()

Vk::Shader shader{device, info};
/* [Shader-usage] */
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
