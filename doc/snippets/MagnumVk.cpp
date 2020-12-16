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
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Vk/BufferCreateInfo.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPoolCreateInfo.h"
#include "Magnum/Vk/DeviceCreateInfo.h"
#include "Magnum/Vk/DeviceFeatures.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/InstanceCreateInfo.h"
#include "Magnum/Vk/Integration.h"
#include "Magnum/Vk/ImageCreateInfo.h"
#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/MemoryAllocateInfo.h"
#include "Magnum/Vk/Queue.h"
#include "Magnum/Vk/RenderPassCreateInfo.h"
#include "Magnum/Vk/ShaderCreateInfo.h"
#include "MagnumExternal/Vulkan/flextVkGlobal.h"

/* [wrapping-include-createinfo] */
#include <Magnum/Vk/RenderPassCreateInfo.h>
/* [wrapping-include-createinfo] */

/* [wrapping-include-both] */
#include <Magnum/Vk/RenderPass.h>
#include <Magnum/Vk/RenderPassCreateInfo.h>
/* [wrapping-include-both] */

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
using namespace Containers::Literals;
int argc{};
char** argv{};
/* [wrapping-optimizing-properties-instance] */
Vk::LayerProperties layers = DOXYGEN_IGNORE(Vk::enumerateLayerProperties());
Vk::InstanceExtensionProperties extensions = DOXYGEN_IGNORE(Vk::enumerateInstanceExtensionProperties(layers.names()));

/* Pass the layer and extension properties for use by InstanceCreateInfo */
Vk::InstanceCreateInfo info{argc, argv, &layers, &extensions};
if(layers.isSupported("VK_LAYER_KHRONOS_validation"_s))
    info.addEnabledLayers({"VK_LAYER_KHRONOS_validation"_s});
if(extensions.isSupported<Vk::Extensions::EXT::debug_report>())
    info.addEnabledExtensions<Vk::Extensions::EXT::debug_report>();
DOXYGEN_IGNORE()

Vk::Instance instance{info};
/* [wrapping-optimizing-properties-instance] */
}

{
Vk::Instance instance{NoCreate};
Vk::Queue queue{NoCreate};
/* [wrapping-optimizing-properties-device-single-expression] */
Vk::Device device{instance, Vk::DeviceCreateInfo{Vk::pickDevice(instance)}
    .addQueues(DOXYGEN_IGNORE(Vk::QueueFlag::Graphics, {0.0f}, {queue}))
    DOXYGEN_IGNORE()
};
/* [wrapping-optimizing-properties-device-single-expression] */
}

{
using namespace Containers::Literals;
Vk::Instance instance{NoCreate};
/* [wrapping-optimizing-properties-device-move] */
Vk::DeviceProperties properties = Vk::pickDevice(instance);
Vk::ExtensionProperties extensions = properties.enumerateExtensionProperties();

/* Move the device properties to the info structure, pass extension properties
   to allow reuse as well */
Vk::DeviceCreateInfo info{std::move(properties), &extensions};
if(extensions.isSupported<Vk::Extensions::EXT::index_type_uint8>())
    info.addEnabledExtensions<Vk::Extensions::EXT::index_type_uint8>();
if(extensions.isSupported("VK_NV_mesh_shader"_s))
    info.addEnabledExtensions({"VK_NV_mesh_shader"_s});
DOXYGEN_IGNORE()

/* Finally, be sure to move the info structure to the device as well */
Vk::Device device{instance, std::move(info)};
/* [wrapping-optimizing-properties-device-move] */
}

{
Vk::Device device{NoCreate};
/* The include should be a no-op here since it was already included above */
/* [Buffer-creation] */
#include <Magnum/Vk/BufferCreateInfo.h>

DOXYGEN_IGNORE()

Vk::Buffer buffer{device,
    Vk::BufferCreateInfo{Vk::BufferUsage::VertexBuffer, 1024*1024},
    Vk::MemoryFlag::DeviceLocal
};
/* [Buffer-creation] */
}

{
Vk::Device device{NoCreate};
/* [Buffer-creation-custom-allocation] */
Vk::Buffer buffer{device,
    Vk::BufferCreateInfo{Vk::BufferUsage::VertexBuffer, 1024*1024},
    NoAllocate
};

Vk::MemoryRequirements requirements = buffer.memoryRequirements();
Vk::Memory memory{device, Vk::MemoryAllocateInfo{
    requirements.size(),
    device.properties().pickMemory(Vk::MemoryFlag::DeviceLocal,
        requirements.memories())
}};

buffer.bindMemory(memory, 0);
/* [Buffer-creation-custom-allocation] */
}

{
/* The include should be a no-op here since it was already included above */
/* [CommandPool-creation] */
#include <Magnum/Vk/CommandPoolCreateInfo.h>

DOXYGEN_IGNORE()

Vk::Device device{DOXYGEN_IGNORE(NoCreate)};

Vk::CommandPool graphicsCommandPool{device, Vk::CommandPoolCreateInfo{
    device.properties().pickQueueFamily(Vk::QueueFlag::Graphics)
}};
/* [CommandPool-creation] */

/* [CommandPool-allocation] */
Vk::CommandBuffer commandBuffer = graphicsCommandPool.allocate();

// fill the buffer, submit …
/* [CommandPool-allocation] */
}

{
Vk::Instance instance;
/* The include should be a no-op here since it was already included above */
/* [Device-creation-construct-queue] */
#include <Magnum/Vk/DeviceCreateInfo.h>

DOXYGEN_IGNORE()

Vk::Queue queue{NoCreate};
Vk::Device device{instance, Vk::DeviceCreateInfo{Vk::pickDevice(instance)}
    .addQueues(Vk::QueueFlag::Graphics, {0.0f}, {queue})
};
/* [Device-creation-construct-queue] */
}

{
Vk::Instance instance;
Vk::DeviceProperties properties{NoCreate};
using namespace Containers::Literals;
/* [Device-creation-extensions] */
Vk::Device device{instance, Vk::DeviceCreateInfo{DOXYGEN_IGNORE(properties)}
    DOXYGEN_IGNORE()
    .addEnabledExtensions<                         // predefined extensions
        Vk::Extensions::EXT::index_type_uint8,
        Vk::Extensions::KHR::device_group>()
    .addEnabledExtensions({"VK_NV_mesh_shader"_s}) // can be plain strings too
};
/* [Device-creation-extensions] */
}

{
Vk::Instance instance;
Vk::DeviceProperties properties{NoCreate};
using namespace Containers::Literals;
/* [Device-creation-features] */
Vk::Device device{instance, Vk::DeviceCreateInfo{DOXYGEN_IGNORE(properties)}
    DOXYGEN_IGNORE()
    .setEnabledFeatures(
        Vk::DeviceFeature::IndexTypeUint8|
        Vk::DeviceFeature::SamplerAnisotropy|
        Vk::DeviceFeature::GeometryShader|
        DOXYGEN_IGNORE(Vk::DeviceFeature{}))
};
/* [Device-creation-features] */
}

{
Vk::Instance instance;
using namespace Containers::Literals;
/* [Device-creation-check-supported] */
Vk::DeviceProperties properties = Vk::pickDevice(instance);
Vk::ExtensionProperties extensions = properties.enumerateExtensionProperties();

Vk::DeviceCreateInfo info{properties};
if(extensions.isSupported<Vk::Extensions::EXT::index_type_uint8>())
    info.addEnabledExtensions<Vk::Extensions::EXT::index_type_uint8>();
if(extensions.isSupported("VK_NV_mesh_shader"_s))
    info.addEnabledExtensions({"VK_NV_mesh_shader"_s});
DOXYGEN_IGNORE()
info.setEnabledFeatures(properties.features() & // mask away unsupported ones
    (Vk::DeviceFeature::IndexTypeUint8|
     Vk::DeviceFeature::SamplerAnisotropy|
     Vk::DeviceFeature::GeometryShader|
     DOXYGEN_IGNORE(Vk::DeviceFeature{})));
/* [Device-creation-check-supported] */
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
/* The include should be a no-op here since it was already included above */
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
Vk::Device device{NoCreate};
/* The include should be a no-op here since it was already included above */
/* [Image-creation] */
#include <Magnum/Vk/ImageCreateInfo.h>

DOXYGEN_IGNORE()

Vk::Image image{device, Vk::ImageCreateInfo2D{
        Vk::ImageUsage::Sampled, VK_FORMAT_R8G8B8A8_SRGB, {1024, 1024}, 1
    }, Vk::MemoryFlag::DeviceLocal
};
/* [Image-creation] */
}

{
Vk::Device device{NoCreate};
/* [Image-creation-custom-allocation] */
Vk::Image image{device, Vk::ImageCreateInfo2D{
        Vk::ImageUsage::Sampled, VK_FORMAT_R8G8B8A8_SRGB, {1024, 1024}, 1
    }, NoAllocate
};

Vk::MemoryRequirements requirements = image.memoryRequirements();
Vk::Memory memory{device, Vk::MemoryAllocateInfo{
    requirements.size(),
    device.properties().pickMemory(Vk::MemoryFlag::DeviceLocal,
        requirements.memories())
}};

image.bindMemory(memory, 0);
/* [Image-creation-custom-allocation] */
}

{
int argc{};
const char** argv{};
/* The include should be a no-op here since it was already included above */
/* [Instance-creation-minimal] */
#include <Magnum/Vk/InstanceCreateInfo.h>

DOXYGEN_IGNORE()

Vk::Instance instance{{argc, argv}};
/* [Instance-creation-minimal] */
}

{
int argc{};
const char** argv{};
/* [Instance-creation] */
using namespace Containers::Literals;

Vk::Instance instance{Vk::InstanceCreateInfo{argc, argv}
    .setApplicationInfo("My Vulkan Application"_s, Vk::version(1, 2, 3))
};
/* [Instance-creation] */
}

{
int argc{};
const char** argv{};
using namespace Containers::Literals;
/* [Instance-creation-layers-extensions] */
Vk::Instance instance{Vk::InstanceCreateInfo{argc, argv}
    DOXYGEN_IGNORE()
    .addEnabledLayers({"VK_LAYER_KHRONOS_validation"_s})
    .addEnabledExtensions<                          // predefined extensions
        Vk::Extensions::EXT::debug_report,
        Vk::Extensions::KHR::external_fence_capabilities>()
    .addEnabledExtensions({"VK_KHR_xcb_surface"_s}) // can be plain strings too
};
/* [Instance-creation-layers-extensions] */
}

{
int argc{};
const char** argv{};
using namespace Containers::Literals;
/* [Instance-creation-check-supported] */
/* Query layer and extension support */
Vk::LayerProperties layers = Vk::enumerateLayerProperties();
Vk::InstanceExtensionProperties extensions =
    /* ... including extensions exposed only by the extra layers */
    Vk::enumerateInstanceExtensionProperties(layers.names());

/* Enable only those that are supported */
Vk::InstanceCreateInfo info{argc, argv};
if(layers.isSupported("VK_LAYER_KHRONOS_validation"_s))
    info.addEnabledLayers({"VK_LAYER_KHRONOS_validation"_s});
if(extensions.isSupported<Vk::Extensions::EXT::debug_report>())
    info.addEnabledExtensions<Vk::Extensions::EXT::debug_report>();
DOXYGEN_IGNORE()

Vk::Instance instance{info};
/* [Instance-creation-check-supported] */
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
/* The include should be a no-op here since it was already included above */
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
Vk::Device device{NoCreate};
Containers::ArrayView<const char> vertexData, indexData;
/* The include should be a no-op here since it was already included above */
/* [Memory-allocation] */
#include <Magnum/Vk/MemoryAllocateInfo.h>

DOXYGEN_IGNORE()

/* Create buffers without allocating them */
Vk::Buffer vertices{device,
    Vk::BufferCreateInfo{Vk::BufferUsage::VertexBuffer, vertexData.size()},
    NoAllocate};
Vk::Buffer indices{device,
    Vk::BufferCreateInfo{Vk::BufferUsage::IndexBuffer, vertexData.size()},
    NoAllocate};

/* Query memory requirements of both buffers, calculate max alignment */
Vk::MemoryRequirements verticesRequirements = vertices.memoryRequirements();
Vk::MemoryRequirements indicesRequirements = indices.memoryRequirements();
const UnsignedLong alignment = Math::max(verticesRequirements.alignment(),
                                         indicesRequirements.alignment());

/* Allocate memory that's large enough to contain both buffers including
   the strictest alignment, and is of a type satisfying requirements of both */
Vk::Memory memory{device, Vk::MemoryAllocateInfo{
    verticesRequirements.alignedSize(alignment) +
        indicesRequirements.alignedSize(alignment),
    device.properties().pickMemory(Vk::MemoryFlag::HostVisible,
        verticesRequirements.memories() & indicesRequirements.memories())
}};

const UnsignedLong indicesOffset = verticesRequirements.alignedSize(alignment);

/* Bind the respective sub-ranges to the buffers */
vertices.bindMemory(memory, 0);
indices.bindMemory(memory, indicesOffset);
/* [Memory-allocation] */

/* [Memory-mapping] */
/* The memory gets unmapped again at the end of scope */
{
    Containers::Array<char, Vk::MemoryMapDeleter> mapped = memory.map();
    Utility::copy(vertexData, mapped.prefix(vertexData.size()));
    Utility::copy(indexData,
        mapped.slice(indicesOffset, indicesOffset + indexData.size()));
}
/* [Memory-mapping] */
}

{
Vk::Device device{DOXYGEN_IGNORE(NoCreate)};
/* The include should be a no-op here since it was already included above */
/* [RenderPass-creation] */
#include <Magnum/Vk/RenderPassCreateInfo.h>

DOXYGEN_IGNORE()

Vk::RenderPass renderPass{device, Vk::RenderPassCreateInfo{}
    .setAttachments({
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_D24_UNORM_S8_UINT
    })
    .addSubpass(Vk::SubpassDescription{}
        .setColorAttachments({0})
        .setDepthStencilAttachment(1)
    )
};
/* [RenderPass-creation] */
}

{
Vk::Device device{DOXYGEN_IGNORE(NoCreate)};
/* [RenderPass-creation-load-store] */
Vk::RenderPass renderPass{device, Vk::RenderPassCreateInfo{}
    .setAttachments({
        {VK_FORMAT_R8G8B8A8_SRGB, Vk::AttachmentLoadOperation::Clear, {}},
        {VK_FORMAT_D24_UNORM_S8_UINT, Vk::AttachmentLoadOperation::Clear, {}},
    })
    DOXYGEN_IGNORE()
};
/* [RenderPass-creation-load-store] */
}

{
Vk::Device device{DOXYGEN_IGNORE(NoCreate)};
/* [RenderPass-creation-layout] */
Vk::RenderPass renderPass{device, Vk::RenderPassCreateInfo{}
    .setAttachments({
        {VK_FORMAT_R8G8B8A8_SRGB,
         Vk::AttachmentLoadOperation::Clear, {},
         Vk::ImageLayout::ColorAttachment,
         Vk::ImageLayout::ColorAttachment},
        {VK_FORMAT_D24_UNORM_S8_UINT,
         Vk::AttachmentLoadOperation::Clear, {},
         Vk::ImageLayout::DepthStencilAttachment,
         Vk::ImageLayout::DepthStencilAttachment},
    })
    .addSubpass(Vk::SubpassDescription{}
        .setColorAttachments({{0, Vk::ImageLayout::ColorAttachment}})
        .setDepthStencilAttachment({1, Vk::ImageLayout::ColorAttachment})
    )
};
/* [RenderPass-creation-layout] */
}

{
Vk::Device device{DOXYGEN_IGNORE(NoCreate)};
/* The include should be a no-op here since it was already included above */
/* [Shader-creation] */
#include <Magnum/Vk/ShaderCreateInfo.h>

DOXYGEN_IGNORE()

Vk::ShaderCreateInfo info{
    CORRADE_INTERNAL_ASSERT_EXPRESSION(Utility::Directory::read("shader.spv"))
};

DOXYGEN_IGNORE()

Vk::Shader shader{device, info};
/* [Shader-creation] */
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
