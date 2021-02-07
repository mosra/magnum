/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Magnum.h"
#include "Magnum/Mesh.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/VertexFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/BufferCreateInfo.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPoolCreateInfo.h"
#include "Magnum/Vk/ComputePipelineCreateInfo.h"
#include "Magnum/Vk/DeviceCreateInfo.h"
#include "Magnum/Vk/DeviceFeatures.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/FenceCreateInfo.h"
#include "Magnum/Vk/FramebufferCreateInfo.h"
#include "Magnum/Vk/InstanceCreateInfo.h"
#include "Magnum/Vk/Integration.h"
#include "Magnum/Vk/ImageCreateInfo.h"
#include "Magnum/Vk/ImageViewCreateInfo.h"
#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/MemoryAllocateInfo.h"
#include "Magnum/Vk/MeshLayout.h"
#include "Magnum/Vk/Pipeline.h"
#include "Magnum/Vk/PipelineLayout.h"
#include "Magnum/Vk/PixelFormat.h"
#include "Magnum/Vk/Queue.h"
#include "Magnum/Vk/RasterizationPipelineCreateInfo.h"
#include "Magnum/Vk/RenderPassCreateInfo.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/ShaderCreateInfo.h"
#include "Magnum/Vk/ShaderSet.h"
#include "MagnumExternal/Vulkan/flextVkGlobal.h"

/* [wrapping-include-createinfo] */
#include <Magnum/Vk/RenderPassCreateInfo.h>
/* [wrapping-include-createinfo] */

/* [wrapping-include-both] */
#include <Magnum/Vk/RenderPass.h>
#include <Magnum/Vk/RenderPassCreateInfo.h>
/* [wrapping-include-both] */

using namespace Magnum;
using namespace Magnum::Math::Literals;

#define DOXYGEN_IGNORE(...) __VA_ARGS__

/* [Instance-delayed-creation] */
class MyApplication {
    public:
        explicit MyApplication();

    private:
        Vk::Instance _instance{NoCreate};
};

MyApplication::MyApplication() {
    // decide on layers, extensions, ...

    _instance.create(Vk::InstanceCreateInfo{DOXYGEN_IGNORE()}
        DOXYGEN_IGNORE()
    );
}
/* [Instance-delayed-creation] */

namespace B {

/* [Device-delayed-creation] */
class MyApplication {
    public:
        explicit MyApplication(DOXYGEN_IGNORE(Vk::Instance& instance));

    private:
        Vk::Device _device{NoCreate};
};

MyApplication::MyApplication(DOXYGEN_IGNORE(Vk::Instance& instance)) {
    // decide on extensions, features, ...

    _device.create(instance, Vk::DeviceCreateInfo{DOXYGEN_IGNORE(Vk::pickDevice(instance))}
        DOXYGEN_IGNORE()
    );
}
/* [Device-delayed-creation] */

}

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
VkFence fence{};
/* [MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR] */
const Vk::Result result = MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR(NotReady,
    vkGetFenceStatus(device, fence));
if(result == Vk::Result::Success) {
    // signaled
} else {
    // Vk::Result::NotReady, not signaled yet
}
/* [MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR] */
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
Vk::Device device{NoCreate};
Vk::CommandBuffer cmd{NoCreate};
/* [Buffer-usage-fill] */
Vk::Buffer buffer{device, Vk::BufferCreateInfo{
    Vk::BufferUsage::TransferDestination|DOXYGEN_IGNORE(Vk::BufferUsage{}), DOXYGEN_IGNORE(0)
}, DOXYGEN_IGNORE(Vk::MemoryFlag{})};

DOXYGEN_IGNORE()

cmd.fillBuffer(buffer, 0x00000000);
/* [Buffer-usage-fill] */
}

{
Vk::Device device{NoCreate};
Vk::CommandBuffer cmd{NoCreate};
UnsignedLong size{};
/* [Buffer-usage-copy] */
Vk::Buffer input{device, Vk::BufferCreateInfo{
    Vk::BufferUsage::TransferSource, size
}, Vk::MemoryFlag::HostVisible};
Vk::Buffer vertices{device, Vk::BufferCreateInfo{
    Vk::BufferUsage::TransferDestination|Vk::BufferUsage::VertexBuffer, size
}, Vk::MemoryFlag::DeviceLocal};

DOXYGEN_IGNORE()

cmd.copyBuffer({input, vertices, {
        {0, 0, size} /* Copy the whole buffer */
    }})
   .pipelineBarrier(Vk::PipelineStage::Transfer, Vk::PipelineStage::VertexInput, {
        /* Make the buffer memory available for vertex input */
        {Vk::Access::TransferWrite, Vk::Access::VertexAttributeRead, vertices}
    });
/* [Buffer-usage-copy] */
}

{
/* The include should be a no-op here since it was already included above */
/* [CommandPool-creation] */
#include <Magnum/Vk/CommandPoolCreateInfo.h>

DOXYGEN_IGNORE()

Vk::Device device{DOXYGEN_IGNORE(NoCreate)};

Vk::CommandPool commandPool{device, Vk::CommandPoolCreateInfo{
    device.properties().pickQueueFamily(Vk::QueueFlag::Graphics)
}};
/* [CommandPool-creation] */
}

{
Vk::Device device{NoCreate};
/* [CommandBuffer-allocation] */
Vk::CommandPool commandPool{device, DOXYGEN_IGNORE(Vk::CommandPoolCreateInfo{0})};

Vk::CommandBuffer cmd = commandPool.allocate();
/* [CommandBuffer-allocation] */

/* [CommandBuffer-usage] */
cmd.begin()
   DOXYGEN_IGNORE()
   .end();
/* [CommandBuffer-usage] */

/* [CommandBuffer-usage-submit] */
Vk::Queue queue{DOXYGEN_IGNORE(NoCreate)};

Vk::Fence fence{device};
queue.submit({Vk::SubmitInfo{}.setCommandBuffers({cmd})}, fence);
fence.wait();
/* [CommandBuffer-usage-submit] */
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
        Vk::DeviceFeature::IndexTypeUnsignedByte|
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
    (Vk::DeviceFeature::IndexTypeUnsignedByte|
     Vk::DeviceFeature::SamplerAnisotropy|
     Vk::DeviceFeature::GeometryShader|
     DOXYGEN_IGNORE(Vk::DeviceFeature{})));
/* [Device-creation-check-supported] */
}

{
Vk::Instance instance;
/* [Device-creation-portability-subset] */
Vk::DeviceProperties properties = DOXYGEN_IGNORE(Vk::pickDevice(instance));
Vk::Device device{instance, Vk::DeviceCreateInfo{properties}
    /* enable triangle fans only if actually supported */
    .setEnabledFeatures(properties.features() & Vk::DeviceFeature::TriangleFans)
    DOXYGEN_IGNORE()
};

DOXYGEN_IGNORE()

if(device.enabledFeatures() & Vk::DeviceFeature::TriangleFans) {
    // draw a triangle fan mesh
} else {
    // indexed draw fallback
}
/* [Device-creation-portability-subset] */
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
Vk::Device device{DOXYGEN_IGNORE(NoCreate)};
/* The include should be a no-op here since it was already included above */
/* [Fence-creation] */
#include <Magnum/Vk/FenceCreateInfo.h>

DOXYGEN_IGNORE()

Vk::Fence fence{device, Vk::FenceCreateInfo{Vk::FenceCreateInfo::Flag::Signaled}};
/* [Fence-creation] */
}

{
Vk::Device device{DOXYGEN_IGNORE(NoCreate)};
Vector2i size;
/* The include should be a no-op here since it was already included above */
/* [Framebuffer-creation] */
#include <Magnum/Vk/FramebufferCreateInfo.h>

DOXYGEN_IGNORE()

Vk::Image color{device, Vk::ImageCreateInfo2D{               /* created before */
    Vk::ImageUsage::ColorAttachment,
    Vk::PixelFormat::RGBA8Unorm, size, 1}, DOXYGEN_IGNORE(NoAllocate)};
Vk::Image depth{device, Vk::ImageCreateInfo2D{
    Vk::ImageUsage::DepthStencilAttachment,
    Vk::PixelFormat::Depth24UnormStencil8UI, size, 1}, DOXYGEN_IGNORE(NoAllocate)};
Vk::ImageView colorView{device, Vk::ImageViewCreateInfo2D{color}};
Vk::ImageView depthView{device, Vk::ImageViewCreateInfo2D{depth}};

Vk::RenderPass renderPass{device, Vk::RenderPassCreateInfo{} /* created before */
    .setAttachments({
        Vk::AttachmentDescription{color.format(), DOXYGEN_IGNORE({}, {}, {}, {})},
        Vk::AttachmentDescription{depth.format(), DOXYGEN_IGNORE({}, {}, {}, {})},
    })
    DOXYGEN_IGNORE()
};

Vk::Framebuffer framebuffer{device, Vk::FramebufferCreateInfo{renderPass, {
    colorView,
    depthView
}, size}};
/* [Framebuffer-creation] */
}

{
Vk::Device device{NoCreate};
/* The include should be a no-op here since it was already included above */
/* [Image-creation] */
#include <Magnum/Vk/ImageCreateInfo.h>

DOXYGEN_IGNORE()

Vk::Image image{device, Vk::ImageCreateInfo2D{
    Vk::ImageUsage::Sampled, PixelFormat::RGBA8Srgb, {1024, 1024}, 1
}, Vk::MemoryFlag::DeviceLocal};
/* [Image-creation] */
}

{
Vk::Device device{NoCreate};
/* [Image-creation-custom-allocation] */
Vk::Image image{device, Vk::ImageCreateInfo2D{
    Vk::ImageUsage::Sampled, PixelFormat::RGBA8Srgb, {1024, 1024}, 1
}, NoAllocate};

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
Vk::Device device{NoCreate};
Vk::CommandBuffer cmd{NoCreate};
/* [Image-usage-clear] */
Vk::Image image{device, Vk::ImageCreateInfo2D{
    Vk::ImageUsage::TransferDestination|DOXYGEN_IGNORE(Vk::ImageUsage{}), Vk::PixelFormat::RGBA8Srgb, DOXYGEN_IGNORE({}, 1)
}, DOXYGEN_IGNORE(Vk::MemoryFlag{})};

DOXYGEN_IGNORE()

cmd.pipelineBarrier(Vk::PipelineStage::TopOfPipe, Vk::PipelineStage::Transfer, {
        /* Transition the image to a layout required by the clear operation */
        {Vk::Accesses{}, Vk::Access::TransferWrite,
         Vk::ImageLayout::Undefined, Vk::ImageLayout::TransferDestination, image}
    })
   .clearColorImage(image, Vk::ImageLayout::TransferDestination, 0x1f1f1f_srgbf);
/* [Image-usage-clear] */
}

{
Vk::Device device{NoCreate};
Vk::CommandBuffer cmd{NoCreate};
/* [Image-usage-copy-from-buffer] */
Vk::Buffer input{device, Vk::BufferCreateInfo{
    Vk::BufferUsage::TransferSource, 256*256*4 DOXYGEN_IGNORE()
}, Vk::MemoryFlag::HostVisible};
Vk::Image texture{device, Vk::ImageCreateInfo2D{
    Vk::ImageUsage::TransferDestination|Vk::ImageUsage::Sampled,
    Vk::PixelFormat::RGBA8Srgb, {256, 256}, DOXYGEN_IGNORE(1)
}, Vk::MemoryFlag::DeviceLocal};

DOXYGEN_IGNORE()

cmd.pipelineBarrier(Vk::PipelineStage::TopOfPipe, Vk::PipelineStage::Transfer, {
        /* Transition the image to a layout required by the copy operation */
        {Vk::Accesses{}, Vk::Access::TransferWrite,
         Vk::ImageLayout::Undefined, Vk::ImageLayout::TransferDestination, texture}
    })
   .copyBufferToImage({input, texture, Vk::ImageLayout::TransferDestination, {
        /* Copy the whole buffer to the first level of the image */
        Vk::BufferImageCopy2D{0, Vk::ImageAspect::Color, 0, {{}, {256, 256}}}
    }})
   .pipelineBarrier(Vk::PipelineStage::Transfer, Vk::PipelineStage::FragmentShader, {
        /* Make the image memory available for fragment shader sampling */
        {Vk::Access::TransferWrite, Vk::Access::ShaderRead,
         Vk::ImageLayout::TransferDestination, Vk::ImageLayout::ShaderReadOnly, texture}
    });
/* [Image-usage-copy-from-buffer] */

/* [Image-usage-copy-from-buffer-multiple] */
cmd.copyBufferToImage(Vk::CopyBufferToImageInfo2D{
    input, texture, Vk::ImageLayout::Undefined, {
        /* Assuming mip levels are tightly packed after each other */
        {     0, Vk::ImageAspect::Color, 0, {{}, {256, 256}}},
        {262144, Vk::ImageAspect::Color, 1, {{}, {128, 128}}},
        {327680, Vk::ImageAspect::Color, 2, {{}, { 64,  64}}},
        DOXYGEN_IGNORE()
    }
});
/* [Image-usage-copy-from-buffer-multiple] */
}

{
Vk::Device device{NoCreate};
Vk::CommandBuffer cmd{NoCreate};
/* [Image-usage-copy-from-image] */
Vk::Image a{device, Vk::ImageCreateInfo2D{
    Vk::ImageUsage::TransferSource|DOXYGEN_IGNORE(Vk::ImageUsage{}),
    Vk::PixelFormat::RGBA8Srgb, {256, 256}, DOXYGEN_IGNORE(1)
}, DOXYGEN_IGNORE({})};
Vk::Image b{device, Vk::ImageCreateInfo2D{
    Vk::ImageUsage::TransferDestination|DOXYGEN_IGNORE(Vk::ImageUsage{}), Vk::PixelFormat::RGBA8Srgb, {256, 256}, DOXYGEN_IGNORE(1)
}, DOXYGEN_IGNORE({})};

DOXYGEN_IGNORE()

cmd.pipelineBarrier(Vk::PipelineStage::TopOfPipe, Vk::PipelineStage::Transfer, {
        /* Transfer both images to a layout required by the copy operation */
        {Vk::Accesses{}, Vk::Access::TransferRead,
            Vk::ImageLayout::DOXYGEN_IGNORE(Undefined), Vk::ImageLayout::TransferSource, a},
        {Vk::Accesses{}, Vk::Access::TransferWrite,
            Vk::ImageLayout::Undefined, Vk::ImageLayout::TransferDestination, b}
    })
   .copyImage({a, Vk::ImageLayout::TransferSource,
               b, Vk::ImageLayout::TransferDestination, {
        /* Copy the whole first layer/level between the images */
        {Vk::ImageAspect::Color, 0, 0, 1, {}, 0, 0, 1, {}, {256, 256, 1}}
    }});
/* [Image-usage-copy-from-image] */
}

{
Vk::Device device{NoCreate};
/* The include should be a no-op here since it was already included above */
/* [ImageView-creation] */
#include <Magnum/Vk/ImageViewCreateInfo.h>

DOXYGEN_IGNORE()

Vk::Image image{device, Vk::ImageCreateInfo2DArray{ /* created before */
    DOXYGEN_IGNORE(Vk::ImageUsage{}, PixelFormat{}, {}, 1)
}, DOXYGEN_IGNORE(Vk::MemoryFlag{})};

Vk::ImageView view{device, Vk::ImageViewCreateInfo2DArray{image}};
/* [ImageView-creation] */
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
/* [MeshLayout-usage] */
constexpr UnsignedInt BufferBinding = 0;

constexpr UnsignedInt PositionLocation = 0;
constexpr UnsignedInt TextureCoordinateLocation = 1;
constexpr UnsignedInt NormalLocation = 5;

Vk::MeshLayout meshLayout{MeshPrimitive::Triangles};
meshLayout
    .addBinding(BufferBinding,
        sizeof(Vector3) + sizeof(Vector2) + sizeof(Vector3))
    .addAttribute(PositionLocation, BufferBinding, VertexFormat::Vector3,
        0)
    .addAttribute(TextureCoordinateLocation, BufferBinding, VertexFormat::Vector2,
        sizeof(Vector3))
    .addAttribute(NormalLocation, BufferBinding, VertexFormat::Vector3,
        sizeof(Vector3) + sizeof(Vector2));
/* [MeshLayout-usage] */
}

{
Vk::Device device{NoCreate};
/* The include should be a no-op here since it was already included above */
/* [Pipeline-creation-rasterization] */
#include <Magnum/Vk/RasterizationPipelineCreateInfo.h>

DOXYGEN_IGNORE()

Vk::ShaderSet shaderSet{DOXYGEN_IGNORE()};
Vk::MeshLayout meshLayout{DOXYGEN_IGNORE(MeshPrimitive{})};
Vk::PipelineLayout pipelineLayout{DOXYGEN_IGNORE(NoCreate)};
Vk::RenderPass renderPass{DOXYGEN_IGNORE(NoCreate)};

Vk::Pipeline pipeline{device, Vk::RasterizationPipelineCreateInfo{
        shaderSet, meshLayout, pipelineLayout, renderPass, 0, 1}
    .setViewport({{}, {800.0f, 600.0f}})
};
/* [Pipeline-creation-rasterization] */
}

{
Vk::Device device{NoCreate};
/* The include should be a no-op here since it was already included above */
/* [Pipeline-creation-compute] */
#include <Magnum/Vk/ComputePipelineCreateInfo.h>

DOXYGEN_IGNORE()

Vk::ShaderSet shaderSet{DOXYGEN_IGNORE()};
Vk::PipelineLayout pipelineLayout{DOXYGEN_IGNORE(NoCreate)};

Vk::Pipeline pipeline{device, Vk::ComputePipelineCreateInfo{
    shaderSet, pipelineLayout
}};
/* [Pipeline-creation-compute] */
}

{
Vk::CommandBuffer cmd{NoCreate};
/* [Pipeline-usage] */
Vk::Pipeline pipeline{DOXYGEN_IGNORE(NoCreate)};

DOXYGEN_IGNORE()

cmd.bindPipeline(pipeline);
/* [Pipeline-usage] */
}

{
Vk::Device device{NoCreate};
/* The include should be a no-op here since it was already included above */
/* [RenderPass-creation] */
#include <Magnum/Vk/RenderPassCreateInfo.h>

DOXYGEN_IGNORE()

Vk::RenderPass renderPass{device, Vk::RenderPassCreateInfo{}
    .setAttachments({
        Vk::AttachmentDescription{Vk::PixelFormat::RGBA8Srgb,
            Vk::AttachmentLoadOperation::Clear,
            Vk::AttachmentStoreOperation::Store,
            Vk::ImageLayout::Undefined,
            Vk::ImageLayout::TransferSource},
        Vk::AttachmentDescription{Vk::PixelFormat::Depth24UnormStencil8UI,
            Vk::AttachmentLoadOperation::Clear,
            Vk::AttachmentStoreOperation::DontCare,
            Vk::ImageLayout::Undefined,
            Vk::ImageLayout::DepthStencilAttachment}
    })
    .addSubpass(Vk::SubpassDescription{}
        .setColorAttachments({
            Vk::AttachmentReference{0, Vk::ImageLayout::ColorAttachment}
        })
        .setDepthStencilAttachment(
            Vk::AttachmentReference{1, Vk::ImageLayout::DepthStencilAttachment}
        )
    )
/* [RenderPass-creation] */
/* [RenderPass-dependencies] */
    .setDependencies({
        Vk::SubpassDependency{
            /* An operation external to the render pass depends on the first
               subpass */
            0, Vk::SubpassDependency::External,
            /* where transfer gets executed only after color output is done */
            Vk::PipelineStage::ColorAttachmentOutput,
            Vk::PipelineStage::Transfer,
            /* and color data written are available for the transfer to read */
            Vk::Access::ColorAttachmentWrite,
            Vk::Access::TransferRead}
    })
};
/* [RenderPass-dependencies] */

Vk::Framebuffer framebuffer{NoCreate};
/* [RenderPass-usage-begin] */
Vk::CommandBuffer cmd = DOXYGEN_IGNORE(Vk::CommandBuffer{NoCreate});
cmd.begin()
   DOXYGEN_IGNORE()
   .beginRenderPass(Vk::RenderPassBeginInfo{renderPass, framebuffer}
        .clearColor(0, 0x1f1f1f_srgbf)
        .clearDepthStencil(1, 1.0f, 0))
/* [RenderPass-usage-begin] */
/* [RenderPass-usage-end] */
   .endRenderPass()
   DOXYGEN_IGNORE()
   .end();
/* [RenderPass-usage-end] */
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
/* [ShaderSet-usage] */
Vk::Shader vert{DOXYGEN_IGNORE(NoCreate)}, frag{DOXYGEN_IGNORE(NoCreate)};

using namespace Containers::Literals;

Vk::ShaderSet set;
set.addShader(Vk::ShaderStage::Vertex, vert, "main"_s)
   .addShader(Vk::ShaderStage::Fragment, frag, "main"_s);
/* [ShaderSet-usage] */

/* [ShaderSet-usage-specializations] */
set.addShader(Vk::ShaderStage::Fragment, frag, "main"_s, {
    {0, 3},
    {1, 0.25f},
    {2, false}
});
/* [ShaderSet-usage-specializations] */
}

{
using namespace Containers::Literals;
/* [ShaderSet-usage-ownership-transfer] */
Vk::Shader shader{DOXYGEN_IGNORE(NoCreate)};

Vk::ShaderSet set;
set.addShader(Vk::ShaderStage::Vertex, shader, "vert"_s)
   .addShader(Vk::ShaderStage::Fragment, std::move(shader), "frag"_s);
/* [ShaderSet-usage-ownership-transfer] */
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
