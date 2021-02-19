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

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/CompareImage.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Vk/BufferCreateInfo.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPoolCreateInfo.h"
#include "Magnum/Vk/DeviceCreateInfo.h"
#include "Magnum/Vk/DeviceFeatures.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/Fence.h"
#include "Magnum/Vk/FramebufferCreateInfo.h"
#include "Magnum/Vk/ImageCreateInfo.h"
#include "Magnum/Vk/ImageViewCreateInfo.h"
#include "Magnum/Vk/Mesh.h"
#include "Magnum/Vk/PipelineLayoutCreateInfo.h"
#include "Magnum/Vk/PixelFormat.h"
#include "Magnum/Vk/RasterizationPipelineCreateInfo.h"
#include "Magnum/Vk/RenderPassCreateInfo.h"
#include "Magnum/Vk/ShaderCreateInfo.h"
#include "Magnum/Vk/ShaderSet.h"
#include "Magnum/Vk/VertexFormat.h"
#include "Magnum/Vk/VulkanTester.h"

#include "configure.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct MeshVkTest: VulkanTester {
    explicit MeshVkTest();

    void setup(Device& device);
    void setup() { setup(device()); }
    void setupRobustness2();
    void setupExtendedDynamicState();
    void teardown();

    void cmdDraw();
    void cmdDrawIndexed();
    void cmdDrawTwoAttributes();
    void cmdDrawTwoAttributesTwoBindings();
    void cmdDrawNullBindingRobustness2();
    void cmdDrawZeroCount();
    void cmdDrawNoCountSet();

    void cmdDrawDynamicPrimitive();
    void cmdDrawDynamicStride();
    void cmdDrawDynamicStrideInsufficientImplementation();

    Queue _queue{NoCreate};
    Device _deviceRobustness2{NoCreate}, _deviceExtendedDynamicState{NoCreate};
    CommandPool _pool{NoCreate};
    Image _color{NoCreate};
    RenderPass _renderPass{NoCreate};
    ImageView _colorView{NoCreate};
    Framebuffer _framebuffer{NoCreate};
    PipelineLayout _pipelineLayout{NoCreate};
    Buffer _pixels{NoCreate};

    private:
        PluginManager::Manager<Trade::AbstractImporter> _manager{"nonexistent"};
};

using namespace Containers::Literals;
using namespace Math::Literals;

const struct {
    const char* name;
    UnsignedInt count;
    UnsignedInt instanceCount;
} CmdDrawZeroCountData[] {
    {"zero elements", 0, 1},
    {"zero instances", 4, 0}
};

MeshVkTest::MeshVkTest() {
    addTests({&MeshVkTest::cmdDraw,
              &MeshVkTest::cmdDrawIndexed,
              &MeshVkTest::cmdDrawTwoAttributes,
              &MeshVkTest::cmdDrawTwoAttributesTwoBindings},
        &MeshVkTest::setup,
        &MeshVkTest::teardown);

    addTests({&MeshVkTest::cmdDrawNullBindingRobustness2},
        &MeshVkTest::setupRobustness2,
        &MeshVkTest::teardown);

    addInstancedTests({&MeshVkTest::cmdDrawZeroCount},
        Containers::arraySize(CmdDrawZeroCountData),
        &MeshVkTest::setup,
        &MeshVkTest::teardown);

    addTests({&MeshVkTest::cmdDrawNoCountSet},
        &MeshVkTest::setup,
        &MeshVkTest::teardown);

    addTests({&MeshVkTest::cmdDrawDynamicPrimitive,
              &MeshVkTest::cmdDrawDynamicStride},
        &MeshVkTest::setupExtendedDynamicState,
        &MeshVkTest::teardown);

    addTests({&MeshVkTest::cmdDrawDynamicStrideInsufficientImplementation},
        &MeshVkTest::setup,
        &MeshVkTest::teardown);

    /* Load the plugins directly from the build tree. Otherwise they're either
       static and already loaded or not present in the build tree */
    #ifdef ANYIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(ANYIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    #ifdef TGAIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT_OUTPUT(_manager.load(TGAIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void MeshVkTest::setup(Device& device) {
    _pool = CommandPool{device, CommandPoolCreateInfo{
        device.properties().pickQueueFamily(QueueFlag::Graphics)}};
    _color = Image{device, ImageCreateInfo2D{
        ImageUsage::ColorAttachment|ImageUsage::TransferSource,
        PixelFormat::RGBA8Srgb, {32, 32}, 1
    }, Vk::MemoryFlag::DeviceLocal};
    _renderPass = RenderPass{device, RenderPassCreateInfo{}
        .setAttachments({AttachmentDescription{
            _color.format(),
            AttachmentLoadOperation::Clear,
            AttachmentStoreOperation::Store,
            ImageLayout::Undefined,
            ImageLayout::TransferSource
        }})
        .addSubpass(SubpassDescription{}.setColorAttachments({
            AttachmentReference{0, ImageLayout::ColorAttachment}
        }))
        /* So the color data are visible for the transfer */
        .setDependencies({SubpassDependency{
            0, SubpassDependency::External,
            PipelineStage::ColorAttachmentOutput,
            PipelineStage::Transfer,
            Access::ColorAttachmentWrite,
            Access::TransferRead
        }})
    };
    _colorView = ImageView{device, ImageViewCreateInfo2D{_color}};
    _framebuffer = Framebuffer{device, FramebufferCreateInfo{_renderPass, {
        _colorView
    }, {32, 32}}};
    _pipelineLayout = PipelineLayout{device, PipelineLayoutCreateInfo{}};
    _pixels = Buffer{device, BufferCreateInfo{
        BufferUsage::TransferDestination, 32*32*4
    }, Vk::MemoryFlag::HostVisible};
}

void MeshVkTest::setupRobustness2() {
    DeviceProperties properties = pickDevice(instance());
    /* If the extension / feature isn't supported, do nothing */
    if(!properties.enumerateExtensionProperties().isSupported<Extensions::EXT::robustness2>() ||
       !(properties.features() & DeviceFeature::NullDescriptor))
        return;

    /* Create the device only if not already, to avoid spamming the output */
    if(!_deviceRobustness2.handle()) _deviceRobustness2.create(instance(), DeviceCreateInfo{std::move(properties)}
        .addQueues(QueueFlag::Graphics, {0.0f}, {_queue})
        .addEnabledExtensions<Extensions::EXT::robustness2>()
        .setEnabledFeatures(DeviceFeature::NullDescriptor)
    );

    setup(_deviceRobustness2);
}

void MeshVkTest::setupExtendedDynamicState() {
    DeviceProperties properties = pickDevice(instance());
    /* If the extension / feature isn't supported, do nothing */
    if(!properties.enumerateExtensionProperties().isSupported<Extensions::EXT::extended_dynamic_state>() ||
       !(properties.features() & DeviceFeature::ExtendedDynamicState))
        return;

    /* Create the device only if not already, to avoid spamming the output */
    if(!_deviceExtendedDynamicState.handle()) _deviceExtendedDynamicState.create(instance(), DeviceCreateInfo{std::move(properties)}
        .addQueues(QueueFlag::Graphics, {0.0f}, {_queue})
        .addEnabledExtensions<Extensions::EXT::extended_dynamic_state>()
        .setEnabledFeatures(DeviceFeature::ExtendedDynamicState)
    );

    setup(_deviceExtendedDynamicState);
}

void MeshVkTest::teardown() {
    _pool = CommandPool{NoCreate};
    _renderPass = RenderPass{NoCreate};
    _color = Image{NoCreate};
    _colorView = ImageView{NoCreate};
    _framebuffer = Framebuffer{NoCreate};
    _pipelineLayout = PipelineLayout{NoCreate};
    _pixels = Buffer{NoCreate};
}

const struct Quad {
    Vector3 position;
    Vector3 color;
} QuadData[] {
    {{-0.5f, -0.5f, 0.0f}, 0xff0000_srgbf},
    {{ 0.5f, -0.5f, 0.0f}, 0x00ff00_srgbf},
    {{-0.5f,  0.5f, 0.0f}, 0x0000ff_srgbf},
    {{ 0.5f,  0.5f, 0.0f}, 0xffffff_srgbf}
};

constexpr UnsignedShort QuadIndexData[] {
    0, 1, 2, 2, 1, 3
};

void MeshVkTest::cmdDraw() {
    /* This is the most simple binding (no offsets, single attribute, single
       buffer) to test the basic workflow. The cmdDrawIndexed() test and others
       pile on the complexity, but when everything goes wrong it's good to have
       a simple test case. */

    Mesh mesh{MeshLayout{MeshPrimitive::TriangleStrip}
        .addBinding(0, sizeof(Vector3))
        .addAttribute(0, 0, VertexFormat::Vector3, 0)
    };
    {
        Buffer buffer{device(), BufferCreateInfo{
            BufferUsage::VertexBuffer,
            sizeof(Vector3)*4
        }, MemoryFlag::HostVisible};
        /** @todo ffs fucking casts!!! */
        Utility::copy(
            Containers::stridedArrayView(QuadData).slice(&Quad::position),
            Containers::arrayCast<Vector3>(Containers::arrayView(buffer.dedicatedMemory().map())));
        mesh.addVertexBuffer(0, std::move(buffer), 0)
            .setCount(4);
    }

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/flat.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    Pipeline pipeline{device(), RasterizationPipelineCreateInfo{
            shaderSet, mesh.layout(), _pipelineLayout, _renderPass, 0, 1}
        .setViewport({{}, Vector2{_framebuffer.size().xy()}})
    };

    CommandBuffer cmd = _pool.allocate();
    cmd.begin()
       .beginRenderPass(Vk::RenderPassBeginInfo{_renderPass, _framebuffer}
           .clearColor(0, 0x1f1f1f_srgbf)
        )
       .bindPipeline(pipeline)
       .draw(mesh)
       .endRenderPass()
       .copyImageToBuffer({_color, Vk::ImageLayout::TransferSource, _pixels, {
            Vk::BufferImageCopy2D{0, Vk::ImageAspect::Color, 0, {{}, _framebuffer.size().xy()}}
        }})
       .pipelineBarrier(Vk::PipelineStage::Transfer, Vk::PipelineStage::Host, {
            {Vk::Access::TransferWrite, Vk::Access::HostRead, _pixels}
        })
       .end();

    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH((ImageView2D{Magnum::PixelFormat::RGBA8Unorm,
        _framebuffer.size().xy(),
        _pixels.dedicatedMemory().mapRead()}),
        Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/flat.tga"),
        DebugTools::CompareImageToFile{_manager});
}

void MeshVkTest::cmdDrawIndexed() {
    Mesh mesh{MeshLayout{MeshPrimitive::Triangles}
        .addBinding(0, sizeof(Vector3))
        .addAttribute(0, 0, VertexFormat::Vector3, 0)
    };
    {
        Buffer buffer{device(), BufferCreateInfo{
            BufferUsage::VertexBuffer|BufferUsage::IndexBuffer,
            /* Artifical offset at the beginning to test that the offset is
               used correctly in both cases */
            32 + 12*4 + sizeof(QuadIndexData)
        }, MemoryFlag::HostVisible};
        Containers::Array<char, MemoryMapDeleter> data = buffer.dedicatedMemory().map();
        /** @todo ffs fucking casts!!! */
        Utility::copy(Containers::stridedArrayView(QuadData).slice(&Quad::position),
            Containers::arrayCast<Vector3>(data.slice(32, 32 + 12*4)));
        Utility::copy(Containers::arrayCast<const char>(QuadIndexData),
            Containers::stridedArrayView(data).suffix(32 + 12*4));
        mesh.addVertexBuffer(0, buffer, 32)
            .setIndexBuffer(std::move(buffer), 32 + 12*4, MeshIndexType::UnsignedShort)
            .setCount(6);
    }

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/flat.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    Pipeline pipeline{device(), RasterizationPipelineCreateInfo{
            shaderSet, mesh.layout(), _pipelineLayout, _renderPass, 0, 1}
        .setViewport({{}, Vector2{_framebuffer.size().xy()}})
    };

    CommandBuffer cmd = _pool.allocate();
    cmd.begin()
       .beginRenderPass(Vk::RenderPassBeginInfo{_renderPass, _framebuffer}
           .clearColor(0, 0x1f1f1f_srgbf)
        )
       .bindPipeline(pipeline)
       .draw(mesh)
       .endRenderPass()
       .copyImageToBuffer({_color, Vk::ImageLayout::TransferSource, _pixels, {
            Vk::BufferImageCopy2D{0, Vk::ImageAspect::Color, 0, {{}, _framebuffer.size().xy()}}
        }})
       .pipelineBarrier(Vk::PipelineStage::Transfer, Vk::PipelineStage::Host, {
            {Vk::Access::TransferWrite, Vk::Access::HostRead, _pixels}
        })
       .end();

    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH((ImageView2D{Magnum::PixelFormat::RGBA8Unorm,
        _framebuffer.size().xy(),
        _pixels.dedicatedMemory().mapRead()}),
        Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/flat.tga"),
        DebugTools::CompareImageToFile{_manager});
}

void MeshVkTest::cmdDrawTwoAttributes() {
    Mesh mesh{MeshLayout{MeshPrimitive::TriangleStrip}
        .addBinding(0, sizeof(Quad))
        .addAttribute(0, 0, VertexFormat::Vector3, offsetof(Quad, position))
        .addAttribute(1, 0, VertexFormat::Vector3, offsetof(Quad, color))
    };
    {
        Buffer buffer{device(), BufferCreateInfo{
            BufferUsage::VertexBuffer,
            sizeof(QuadData)
        }, MemoryFlag::HostVisible};
        /** @todo ffs fucking casts!!! */
        Utility::copy(Containers::arrayCast<const char>(QuadData),
            Containers::stridedArrayView(buffer.dedicatedMemory().map()));
        mesh.addVertexBuffer(0, std::move(buffer), 0)
            .setCount(4);
    }

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/vertexcolor.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    Pipeline pipeline{device(), RasterizationPipelineCreateInfo{
            shaderSet, mesh.layout(), _pipelineLayout, _renderPass, 0, 1}
        .setViewport({{}, Vector2{_framebuffer.size().xy()}})
    };

    CommandBuffer cmd = _pool.allocate();
    cmd.begin()
       .beginRenderPass(Vk::RenderPassBeginInfo{_renderPass, _framebuffer}
           .clearColor(0, 0x1f1f1f_srgbf)
        )
       .bindPipeline(pipeline)
       .draw(mesh)
       .endRenderPass()
       .copyImageToBuffer({_color, Vk::ImageLayout::TransferSource, _pixels, {
            Vk::BufferImageCopy2D{0, Vk::ImageAspect::Color, 0, {{}, _framebuffer.size().xy()}}
        }})
       .pipelineBarrier(Vk::PipelineStage::Transfer, Vk::PipelineStage::Host, {
            {Vk::Access::TransferWrite, Vk::Access::HostRead, _pixels}
        })
       .end();

    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH((ImageView2D{Magnum::PixelFormat::RGBA8Unorm,
        _framebuffer.size().xy(),
        _pixels.dedicatedMemory().mapRead()}),
        Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/vertexcolor.tga"),
        /* ARM Mali (Android) has some minor off-by-one differences */
        (DebugTools::CompareImageToFile{_manager, 0.5f, 0.012f}));
}

void MeshVkTest::cmdDrawTwoAttributesTwoBindings() {
    Mesh mesh{MeshLayout{MeshPrimitive::TriangleStrip}
        .addBinding(0, sizeof(Vector3))
        .addBinding(1, sizeof(Vector3))
        .addAttribute(0, 0, VertexFormat::Vector3, 0)
        .addAttribute(1, 1, VertexFormat::Vector3, 0)
    };
    {
        Buffer positions{device(), BufferCreateInfo{
            BufferUsage::VertexBuffer,
            sizeof(Vector3)*4
        }, MemoryFlag::HostVisible};
        Buffer colors{device(), BufferCreateInfo{
            BufferUsage::VertexBuffer,
            sizeof(Vector3)*4
        }, MemoryFlag::HostVisible};
        /** @todo ffs fucking casts!!! */
        Utility::copy(Containers::stridedArrayView(QuadData).slice(&Quad::position),
            Containers::arrayCast<Vector3>(Containers::arrayView(positions.dedicatedMemory().map())));
        Utility::copy(Containers::stridedArrayView(QuadData).slice(&Quad::color),
            Containers::arrayCast<Vector3>(Containers::arrayView(colors.dedicatedMemory().map())));
        mesh.addVertexBuffer(0, std::move(positions), 0)
            .addVertexBuffer(1, std::move(colors), 0)
            .setCount(4);
    }

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/vertexcolor.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    Pipeline pipeline{device(), RasterizationPipelineCreateInfo{
            shaderSet, mesh.layout(), _pipelineLayout, _renderPass, 0, 1}
        .setViewport({{}, Vector2{_framebuffer.size().xy()}})
    };

    CommandBuffer cmd = _pool.allocate();
    cmd.begin()
       .beginRenderPass(Vk::RenderPassBeginInfo{_renderPass, _framebuffer}
           .clearColor(0, 0x1f1f1f_srgbf)
        )
       .bindPipeline(pipeline)
       .draw(mesh)
       .endRenderPass()
       .copyImageToBuffer({_color, Vk::ImageLayout::TransferSource, _pixels, {
            Vk::BufferImageCopy2D{0, Vk::ImageAspect::Color, 0, {{}, _framebuffer.size().xy()}}
        }})
       .pipelineBarrier(Vk::PipelineStage::Transfer, Vk::PipelineStage::Host, {
            {Vk::Access::TransferWrite, Vk::Access::HostRead, _pixels}
        })
       .end();

    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH((ImageView2D{Magnum::PixelFormat::RGBA8Unorm,
        _framebuffer.size().xy(),
        _pixels.dedicatedMemory().mapRead()}),
        Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/vertexcolor.tga"),
        /* ARM Mali (Android) has some minor off-by-one differences */
        (DebugTools::CompareImageToFile{_manager, 0.5f, 0.012f}));
}

void MeshVkTest::cmdDrawNullBindingRobustness2() {
    if(!(_deviceRobustness2.enabledFeatures() & DeviceFeature::NullDescriptor))
        CORRADE_SKIP("DeviceFeature::NullDescriptor not supported, can't test.");

    Mesh mesh{MeshLayout{MeshPrimitive::TriangleStrip}
        .addBinding(0, sizeof(Vector3))
        .addBinding(1, sizeof(Vector3))
        .addAttribute(0, 0, VertexFormat::Vector3, 0)
        .addAttribute(1, 1, VertexFormat::Vector3, 0)
    };
    {
        Buffer positions{_deviceRobustness2, BufferCreateInfo{
            BufferUsage::VertexBuffer,
            sizeof(Vector3)*4
        }, MemoryFlag::HostVisible};
        /** @todo ffs fucking casts!!! */
        Utility::copy(Containers::stridedArrayView(QuadData).slice(&Quad::position),
            Containers::arrayCast<Vector3>(Containers::arrayView(positions.dedicatedMemory().map())));
        mesh.addVertexBuffer(0, std::move(positions), 0)
            .setCount(4);
    }

    Shader shader{_deviceRobustness2, ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/vertexcolor.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    Pipeline pipeline{_deviceRobustness2, RasterizationPipelineCreateInfo{
            shaderSet, mesh.layout(), _pipelineLayout, _renderPass, 0, 1}
        .setViewport({{}, Vector2{_framebuffer.size().xy()}})
    };

    CommandBuffer cmd = _pool.allocate();
    cmd.begin()
       .beginRenderPass(Vk::RenderPassBeginInfo{_renderPass, _framebuffer}
           .clearColor(0, 0x1f1f1f_srgbf)
        )
       .bindPipeline(pipeline)
       .draw(mesh)
       .endRenderPass()
       .copyImageToBuffer({_color, Vk::ImageLayout::TransferSource, _pixels, {
            Vk::BufferImageCopy2D{0, Vk::ImageAspect::Color, 0, {{}, _framebuffer.size().xy()}}
        }})
       .pipelineBarrier(Vk::PipelineStage::Transfer, Vk::PipelineStage::Host, {
            {Vk::Access::TransferWrite, Vk::Access::HostRead, _pixels}
        })
       .end();

    _queue.submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH((ImageView2D{Magnum::PixelFormat::RGBA8Unorm,
        _framebuffer.size().xy(),
        _pixels.dedicatedMemory().mapRead()}),
        Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/nullcolor.tga"),
        /* ARM Mali (Android) has some minor off-by-one differences */
        (DebugTools::CompareImageToFile{_manager}));
}

void MeshVkTest::cmdDrawZeroCount() {
    auto&& data = CmdDrawZeroCountData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Mesh mesh{MeshLayout{MeshPrimitive::Triangles}
        .addBinding(0, sizeof(Vector3))
        .addAttribute(0, 0, VertexFormat::Vector3, 0)
    };
    /* Deliberately not setting up any buffer -- the draw() should be a no-op
       and thus no draw validation (and error messages) should happen */
    mesh.setCount(data.count)
        .setInstanceCount(data.instanceCount);

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/flat.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    Pipeline pipeline{device(), RasterizationPipelineCreateInfo{
            shaderSet, mesh.layout(), _pipelineLayout, _renderPass, 0, 1}
        .setViewport({{}, Vector2{_framebuffer.size().xy()}})
    };

    CommandBuffer cmd = _pool.allocate();
    cmd.begin()
       .beginRenderPass(Vk::RenderPassBeginInfo{_renderPass, _framebuffer}
           .clearColor(0, 0x1f1f1f_srgbf)
        )
       .bindPipeline(pipeline)
       .draw(mesh)
       .endRenderPass()
       .copyImageToBuffer({_color, Vk::ImageLayout::TransferSource, _pixels, {
            Vk::BufferImageCopy2D{0, Vk::ImageAspect::Color, 0, {{}, _framebuffer.size().xy()}}
        }})
       .pipelineBarrier(Vk::PipelineStage::Transfer, Vk::PipelineStage::Host, {
            {Vk::Access::TransferWrite, Vk::Access::HostRead, _pixels}
        })
       .end();

    queue().submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH((ImageView2D{Magnum::PixelFormat::RGBA8Unorm,
        _framebuffer.size().xy(),
        _pixels.dedicatedMemory().mapRead()}),
        Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/noop.tga"),
        DebugTools::CompareImageToFile{_manager});
}

void MeshVkTest::cmdDrawNoCountSet() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Mesh mesh{MeshLayout{MeshPrimitive::Triangles}};

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/noop.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    Pipeline pipeline{device(), RasterizationPipelineCreateInfo{
            shaderSet, mesh.layout(), _pipelineLayout, _renderPass, 0, 1}
        .setViewport({{}, Vector2{_framebuffer.size().xy()}})
    };

    CommandBuffer cmd = _pool.allocate();
    cmd.begin()
       .beginRenderPass(Vk::RenderPassBeginInfo{_renderPass, _framebuffer}
           .clearColor(0, 0x1f1f1f_srgbf)
        )
       .bindPipeline(pipeline);

    std::ostringstream out;
    Error redirectError{&out};
    cmd.draw(mesh);
    CORRADE_COMPARE(out.str(), "Vk::CommandBuffer::draw(): Mesh::setCount() was never called, probably a mistake?\n");
}

void MeshVkTest::cmdDrawDynamicPrimitive() {
    if(!(_deviceExtendedDynamicState.enabledFeatures() & DeviceFeature::ExtendedDynamicState))
        CORRADE_SKIP("DeviceFeature::ExtendedDynamicState not supported, can't test.");

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Mesh mesh{MeshLayout{MeshPrimitive::TriangleStrip}
        .addBinding(0, sizeof(Vector3))
        .addAttribute(0, 0, VertexFormat::Vector3, 0)
    };
    {
        Buffer buffer{_deviceExtendedDynamicState, BufferCreateInfo{
            BufferUsage::VertexBuffer,
            sizeof(Vector3)*4
        }, MemoryFlag::HostVisible};
        /** @todo ffs fucking casts!!! */
        Utility::copy(
            Containers::stridedArrayView(QuadData).slice(&Quad::position),
            Containers::arrayCast<Vector3>(Containers::arrayView(buffer.dedicatedMemory().map())));
        mesh.addVertexBuffer(0, std::move(buffer), 0)
            .setCount(4);
    }

    Shader shader{_deviceExtendedDynamicState, ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/flat.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    /* Create the pipeline with Triangles while the mesh is TriangleStrip */
    MeshLayout pipelineLayout{MeshPrimitive::Triangles};
    pipelineLayout
        .addBinding(0, sizeof(Vector3))
        .addAttribute(0, 0, VertexFormat::Vector3, 0);
    Pipeline pipeline{_deviceExtendedDynamicState, RasterizationPipelineCreateInfo{
            shaderSet, pipelineLayout, _pipelineLayout, _renderPass, 0, 1}
        .setViewport({{}, Vector2{_framebuffer.size().xy()}})
        .setDynamicStates(DynamicRasterizationState::MeshPrimitive)
    };

    CommandBuffer cmd = _pool.allocate();
    cmd.begin()
       .beginRenderPass(Vk::RenderPassBeginInfo{_renderPass, _framebuffer}
           .clearColor(0, 0x1f1f1f_srgbf)
        )
       .bindPipeline(pipeline)
       .draw(mesh)
       .endRenderPass()
       .copyImageToBuffer({_color, Vk::ImageLayout::TransferSource, _pixels, {
            Vk::BufferImageCopy2D{0, Vk::ImageAspect::Color, 0, {{}, _framebuffer.size().xy()}}
        }})
       .pipelineBarrier(Vk::PipelineStage::Transfer, Vk::PipelineStage::Host, {
            {Vk::Access::TransferWrite, Vk::Access::HostRead, _pixels}
        })
       .end();

    _queue.submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH((ImageView2D{Magnum::PixelFormat::RGBA8Unorm,
        _framebuffer.size().xy(),
        _pixels.dedicatedMemory().mapRead()}),
        Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/flat.tga"),
        DebugTools::CompareImageToFile{_manager});
}

void MeshVkTest::cmdDrawDynamicStride() {
    if(!(_deviceExtendedDynamicState.enabledFeatures() & DeviceFeature::ExtendedDynamicState))
        CORRADE_SKIP("DeviceFeature::ExtendedDynamicState not supported, can't test.");

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    Mesh mesh{MeshLayout{MeshPrimitive::TriangleStrip}
        .addBinding(0, sizeof(Vector3))
        .addAttribute(0, 0, VertexFormat::Vector3, 0)
    };
    {
        Buffer buffer{_deviceExtendedDynamicState, BufferCreateInfo{
            BufferUsage::VertexBuffer,
            sizeof(Vector3)*4
        }, MemoryFlag::HostVisible};
        /** @todo ffs fucking casts!!! */
        Utility::copy(
            Containers::stridedArrayView(QuadData).slice(&Quad::position),
            Containers::arrayCast<Vector3>(Containers::arrayView(buffer.dedicatedMemory().map())));
        mesh.addVertexBuffer(0, std::move(buffer), 0)
            .setCount(4);
    }

    Shader shader{_deviceExtendedDynamicState, ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/flat.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    /* Create the pipeline with a 1 kB stride, while the actual stride is
       different */
    MeshLayout pipelineLayout{MeshPrimitive::TriangleStrip};
    pipelineLayout
        .addBinding(0, 1024)
        .addAttribute(0, 0, VertexFormat::Vector3, 0);
    Pipeline pipeline{_deviceExtendedDynamicState, RasterizationPipelineCreateInfo{
            shaderSet, pipelineLayout, _pipelineLayout, _renderPass, 0, 1}
        .setViewport({{}, Vector2{_framebuffer.size().xy()}})
        .setDynamicStates(DynamicRasterizationState::VertexInputBindingStride)
    };

    CommandBuffer cmd = _pool.allocate();
    cmd.begin()
       .beginRenderPass(Vk::RenderPassBeginInfo{_renderPass, _framebuffer}
           .clearColor(0, 0x1f1f1f_srgbf)
        )
       .bindPipeline(pipeline)
       .draw(mesh)
       .endRenderPass()
       .copyImageToBuffer({_color, Vk::ImageLayout::TransferSource, _pixels, {
            Vk::BufferImageCopy2D{0, Vk::ImageAspect::Color, 0, {{}, _framebuffer.size().xy()}}
        }})
       .pipelineBarrier(Vk::PipelineStage::Transfer, Vk::PipelineStage::Host, {
            {Vk::Access::TransferWrite, Vk::Access::HostRead, _pixels}
        })
       .end();

    _queue.submit({SubmitInfo{}.setCommandBuffers({cmd})}).wait();

    if(!(_manager.loadState("AnyImageImporter") & PluginManager::LoadState::Loaded) ||
       !(_manager.loadState("TgaImporter") & PluginManager::LoadState::Loaded))
        CORRADE_SKIP("AnyImageImporter / TgaImporter plugins not found.");

    CORRADE_COMPARE_WITH((ImageView2D{Magnum::PixelFormat::RGBA8Unorm,
        _framebuffer.size().xy(),
        _pixels.dedicatedMemory().mapRead()}),
        Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/flat.tga"),
        DebugTools::CompareImageToFile{_manager});
}

void MeshVkTest::cmdDrawDynamicStrideInsufficientImplementation() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(device().isExtensionEnabled<Extensions::EXT::extended_dynamic_state>())
        CORRADE_SKIP("VK_EXT_extended_dynamic_state enabled, can't test.");

    Mesh mesh{MeshLayout{MeshPrimitive::TriangleStrip}
        .addBinding(0, sizeof(Vector3))
        .addAttribute(0, 0, VertexFormat::Vector3, 0)
    };
    {
        Buffer buffer{device(), BufferCreateInfo{
            BufferUsage::VertexBuffer,
            sizeof(Vector3)*4
        }, MemoryFlag::HostVisible};
        /** @todo ffs fucking casts!!! */
        Utility::copy(
            Containers::stridedArrayView(QuadData).slice(&Quad::position),
            Containers::arrayCast<Vector3>(Containers::arrayView(buffer.dedicatedMemory().map())));
        mesh.addVertexBuffer(0, std::move(buffer), 0)
            .setCount(4);
    }

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "MeshTestFiles/flat.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    /* Create a pipeline without any dynamic state and then wrap it with fake
       enabled vertex input binding stride -- doing so directly would trigger
       validation layer failures (using dynamic state from a non-enabled ext),
       which we don't want */
    Pipeline pipeline{device(), RasterizationPipelineCreateInfo{
            shaderSet, mesh.layout(), _pipelineLayout, _renderPass, 0, 1}
        .setViewport({{}, Vector2{_framebuffer.size().xy()}})
    };
    Pipeline fakeDynamicStatePipeline = Pipeline::wrap(device(),
        PipelineBindPoint::Rasterization, pipeline,
        DynamicRasterizationState::VertexInputBindingStride);

    CommandBuffer cmd = _pool.allocate();
    cmd.begin()
       .beginRenderPass(Vk::RenderPassBeginInfo{_renderPass, _framebuffer}
           .clearColor(0, 0x1f1f1f_srgbf)
        )
       .bindPipeline(fakeDynamicStatePipeline);

    std::ostringstream out;
    Error redirectError{&out};
    cmd.draw(mesh);
    CORRADE_COMPARE(out.str(), "Vk::CommandBuffer::draw(): dynamic strides supplied for an implementation without extended dynamic state\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::MeshVkTest)
