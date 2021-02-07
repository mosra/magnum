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
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Math/Range.h"
#include "Magnum/Vk/BufferCreateInfo.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPoolCreateInfo.h"
#include "Magnum/Vk/ComputePipelineCreateInfo.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/ImageCreateInfo.h"
#include "Magnum/Vk/MeshLayout.h"
#include "Magnum/Vk/Pipeline.h"
#include "Magnum/Vk/PipelineLayoutCreateInfo.h"
#include "Magnum/Vk/PixelFormat.h"
#include "Magnum/Vk/RasterizationPipelineCreateInfo.h"
#include "Magnum/Vk/RenderPassCreateInfo.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/ShaderCreateInfo.h"
#include "Magnum/Vk/ShaderSet.h"
#include "Magnum/Vk/VertexFormat.h"
#include "Magnum/Vk/VulkanTester.h"

#include "configure.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct PipelineVkTest: VulkanTester {
    explicit PipelineVkTest();

    void constructRasterization();
    void constructRasterizationViewportNotSet();
    void constructRasterizationViewportNotSetDiscardEnabled();
    void constructRasterizationViewportNotSetDynamic();
    void constructCompute();
    void constructMove();

    void wrapRasterization();
    void wrapCompute();

    void dynamicRasterizationStatesNotRasterization();

    void cmdBind();

    void cmdBarrier();
    void cmdBarrierExecutionOnly();
    void cmdBarrierGlobalMemory();
    void cmdBarrierBufferMemory();
    void cmdBarrierImageMemory();
};

PipelineVkTest::PipelineVkTest() {
    addTests({&PipelineVkTest::constructRasterization,
              &PipelineVkTest::constructRasterizationViewportNotSet,
              &PipelineVkTest::constructRasterizationViewportNotSetDiscardEnabled,
              &PipelineVkTest::constructRasterizationViewportNotSetDynamic,
              &PipelineVkTest::constructCompute,
              &PipelineVkTest::constructMove,

              &PipelineVkTest::wrapRasterization,
              &PipelineVkTest::wrapCompute,

              &PipelineVkTest::dynamicRasterizationStatesNotRasterization,

              &PipelineVkTest::cmdBind,

              &PipelineVkTest::cmdBarrier,
              &PipelineVkTest::cmdBarrierExecutionOnly,
              &PipelineVkTest::cmdBarrierGlobalMemory,
              &PipelineVkTest::cmdBarrierBufferMemory,
              &PipelineVkTest::cmdBarrierImageMemory});
}

using namespace Containers::Literals;

void PipelineVkTest::constructRasterization() {
    /* Wonderful, this contains basically EVERYTHING ELSE that got implemented
       until now. */

    {
        RenderPass renderPass{device(), RenderPassCreateInfo{}
            .setAttachments({
                AttachmentDescription{PixelFormat::RGBA8Unorm,
                    AttachmentLoadOperation::Clear,
                    AttachmentStoreOperation::Store,
                    ImageLayout::Undefined,
                    ImageLayout::ColorAttachment}
            })
            .addSubpass(SubpassDescription{}.setColorAttachments({
                AttachmentReference{0, ImageLayout::ColorAttachment}
            }))
        };

        /* Not sure if this is really needed, but the shader needs those inputs
           so playing it safe */
        MeshLayout meshLayout{MeshPrimitive::Triangles};
        meshLayout
            .addBinding(0, 2*4*4)
            .addAttribute(0, 0, Vk::VertexFormat::Vector4, 0)
            .addAttribute(1, 0, Vk::VertexFormat::Vector4, 4*4);

        PipelineLayout pipelineLayout{device(), PipelineLayoutCreateInfo{}};

        Shader shader{device(), ShaderCreateInfo{
            Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "triangle-shaders.spv"))
        }};

        ShaderSet shaderSet;
        shaderSet
            .addShader(ShaderStage::Vertex, shader, "ver"_s)
            .addShader(ShaderStage::Fragment, shader, "fra"_s);

        Pipeline pipeline{device(), RasterizationPipelineCreateInfo{
                shaderSet, meshLayout, pipelineLayout, renderPass, 0, 1}
            .setViewport({{}, {200, 200}})
            .setDynamicStates(DynamicRasterizationState::LineWidth|DynamicRasterizationState::DepthBias)
        };
        CORRADE_VERIFY(pipeline.handle());
        CORRADE_COMPARE(pipeline.handleFlags(), HandleFlag::DestroyOnDestruction);
        CORRADE_COMPARE(pipeline.bindPoint(), PipelineBindPoint::Rasterization);
        CORRADE_COMPARE(pipeline.dynamicRasterizationStates(), DynamicRasterizationState::LineWidth|DynamicRasterizationState::DepthBias);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void PipelineVkTest::constructRasterizationViewportNotSet() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    MeshLayout meshLayout{MeshPrimitive::Triangles};

    PipelineLayout pipelineLayout{device(), PipelineLayoutCreateInfo{}};

    ShaderSet shaderSet;

    RasterizationPipelineCreateInfo info{
        shaderSet, meshLayout, pipelineLayout, {}, 0, 1
    };
    CORRADE_VERIFY(!info->pViewportState);

    std::ostringstream out;
    Error redirectError{&out};
    Pipeline pipeline{device(), info};
    CORRADE_COMPARE(out.str(), "Vk::Pipeline: if rasterization discard is not enabled, the viewport has to be either dynamic or set via setViewport()\n");
}

void PipelineVkTest::constructRasterizationViewportNotSetDiscardEnabled() {
    RenderPass renderPass{device(), RenderPassCreateInfo{}
        .setAttachments({
            AttachmentDescription{PixelFormat::RGBA8Unorm,
                AttachmentLoadOperation::Clear,
                AttachmentStoreOperation::Store,
                ImageLayout::Undefined,
                ImageLayout::ColorAttachment}
        })
        .addSubpass(SubpassDescription{}.setColorAttachments({
            AttachmentReference{0, ImageLayout::ColorAttachment}
        }))
    };

    /* Not sure if this is really needed, but the shader needs those inputs so
       playing it safe */
    MeshLayout meshLayout{MeshPrimitive::Triangles};
    meshLayout
        .addBinding(0, 2*4*4)
        .addAttribute(0, 0, Vk::VertexFormat::Vector4, 0)
        .addAttribute(1, 0, Vk::VertexFormat::Vector4, 4*4);

    PipelineLayout pipelineLayout{device(), PipelineLayoutCreateInfo{}};

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "triangle-shaders.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    RasterizationPipelineCreateInfo info{shaderSet, meshLayout, pipelineLayout, renderPass, 0, 1};
    CORRADE_VERIFY(!info->pViewportState);
    /** @todo switch to Magnum API once exposed */
    const_cast<VkPipelineRasterizationStateCreateInfo*>(info->pRasterizationState)->rasterizerDiscardEnable = true;
    Pipeline pipeline{device(), info};

    /* The only thing I want to verify is that this doesn't crash or assert */
    CORRADE_VERIFY(pipeline.handle());
}

void PipelineVkTest::constructRasterizationViewportNotSetDynamic() {
    RenderPass renderPass{device(), RenderPassCreateInfo{}
        .setAttachments({
            AttachmentDescription{PixelFormat::RGBA8Unorm,
                AttachmentLoadOperation::Clear,
                AttachmentStoreOperation::Store,
                ImageLayout::Undefined,
                ImageLayout::ColorAttachment}
        })
        .addSubpass(SubpassDescription{}.setColorAttachments({
            AttachmentReference{0, ImageLayout::ColorAttachment}
        }))
    };

    /* Not sure if this is really needed, but the shader needs those inputs
       so playing it safe */
    MeshLayout meshLayout{MeshPrimitive::Triangles};
    meshLayout
        .addBinding(0, 2*4*4)
        .addAttribute(0, 0, Vk::VertexFormat::Vector4, 0)
        .addAttribute(1, 0, Vk::VertexFormat::Vector4, 4*4);

    PipelineLayout pipelineLayout{device(), PipelineLayoutCreateInfo{}};

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "triangle-shaders.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    RasterizationPipelineCreateInfo info{
        shaderSet, meshLayout, pipelineLayout, renderPass, 0, 1};
    info.setViewport(Range3D{}) /* Has to be set because the count is used */
        .setDynamicStates(DynamicRasterizationState::Viewport|DynamicRasterizationState::Scissor);
    /* But the data don't have to be there */
    const_cast<VkPipelineViewportStateCreateInfo*>(info->pViewportState)->pViewports = nullptr;
    const_cast<VkPipelineViewportStateCreateInfo*>(info->pViewportState)->pScissors = nullptr;

    Pipeline pipeline{device(), info};

    /* The main thing I want to verify is that this doesn't crash or assert */
    CORRADE_VERIFY(pipeline.handle());
    CORRADE_COMPARE(pipeline.dynamicRasterizationStates(), DynamicRasterizationState::Viewport|DynamicRasterizationState::Scissor);
}

void PipelineVkTest::constructCompute() {
    {
        PipelineLayout pipelineLayout{device(), PipelineLayoutCreateInfo{}};

        Shader shader{device(), ShaderCreateInfo{
            Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "compute-noop.spv"))
        }};

        ShaderSet shaderSet;
        shaderSet.addShader(ShaderStage::Compute, shader, "main"_s);

        Pipeline pipeline{device(), ComputePipelineCreateInfo{
            shaderSet, pipelineLayout
        }};
        CORRADE_VERIFY(pipeline.handle());
        CORRADE_COMPARE(pipeline.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void PipelineVkTest::constructMove() {
    RenderPass renderPass{device(), RenderPassCreateInfo{}
        .setAttachments({
            AttachmentDescription{PixelFormat::RGBA8Unorm,
                AttachmentLoadOperation::Clear,
                AttachmentStoreOperation::Store,
                ImageLayout::Undefined,
                ImageLayout::ColorAttachment}
        })
        .addSubpass(SubpassDescription{}.setColorAttachments({
            AttachmentReference{0, ImageLayout::ColorAttachment}
        }))
    };

    /* Not sure if this is really needed, but the shader needs those inputs so
       playing it safe */
    MeshLayout meshLayout{MeshPrimitive::Triangles};
    meshLayout
        .addBinding(0, 2*4*4)
        .addAttribute(0, 0, Vk::VertexFormat::Vector4, 0)
        .addAttribute(1, 0, Vk::VertexFormat::Vector4, 4*4);

    PipelineLayout pipelineLayout{device(), PipelineLayoutCreateInfo{}};

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "triangle-shaders.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    Pipeline a{device(), RasterizationPipelineCreateInfo{
            shaderSet, meshLayout, pipelineLayout, renderPass, 0, 1}
        .setViewport({{}, {200, 200}})
        .setDynamicStates(DynamicRasterizationState::LineWidth|DynamicRasterizationState::DepthBias)
    };
    VkPipeline handle = a.handle();

    Pipeline b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(b.bindPoint(), PipelineBindPoint::Rasterization);
    CORRADE_COMPARE(b.dynamicRasterizationStates(), DynamicRasterizationState::LineWidth|DynamicRasterizationState::DepthBias);

    Pipeline c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(c.bindPoint(), PipelineBindPoint::Rasterization);
    CORRADE_COMPARE(c.dynamicRasterizationStates(), DynamicRasterizationState::LineWidth|DynamicRasterizationState::DepthBias);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Pipeline>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Pipeline>::value);
}

void PipelineVkTest::wrapRasterization() {
    RenderPass renderPass{device(), RenderPassCreateInfo{}
        .setAttachments({
            AttachmentDescription{PixelFormat::RGBA8Unorm,
                AttachmentLoadOperation::Clear,
                AttachmentStoreOperation::Store,
                ImageLayout::Undefined,
                ImageLayout::ColorAttachment}
        })
        .addSubpass(SubpassDescription{}.setColorAttachments({
            AttachmentReference{0, ImageLayout::ColorAttachment}
        }))
    };

    /* Not sure if this is really needed, but the shader needs those inputs so
       playing it safe */
    MeshLayout meshLayout{MeshPrimitive::Triangles};
    meshLayout
        .addBinding(0, 2*4*4)
        .addAttribute(0, 0, Vk::VertexFormat::Vector4, 0)
        .addAttribute(1, 0, Vk::VertexFormat::Vector4, 4*4);

    PipelineLayout pipelineLayout{device(), PipelineLayoutCreateInfo{}};

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "triangle-shaders.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet
        .addShader(ShaderStage::Vertex, shader, "ver"_s)
        .addShader(ShaderStage::Fragment, shader, "fra"_s);

    VkPipeline pipeline{};
    CORRADE_COMPARE(Result(device()->CreateGraphicsPipelines(device(), {}, 1,
        RasterizationPipelineCreateInfo{shaderSet, meshLayout, pipelineLayout, renderPass, 0, 1}
            .setViewport({{}, {200, 200}})
            .setDynamicStates(DynamicRasterizationState::LineWidth|DynamicRasterizationState::DepthBias),
        nullptr, &pipeline)), Result::Success);

    auto wrapped = Pipeline::wrap(device(), PipelineBindPoint::Rasterization, pipeline, DynamicRasterizationState::LineWidth|DynamicRasterizationState::DepthBias, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), pipeline);
    CORRADE_COMPARE(wrapped.bindPoint(), PipelineBindPoint::Rasterization);
    CORRADE_COMPARE(wrapped.dynamicRasterizationStates(), DynamicRasterizationState::LineWidth|DynamicRasterizationState::DepthBias);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), pipeline);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyPipeline(device(), pipeline, nullptr);
}

void PipelineVkTest::wrapCompute() {
    PipelineLayout pipelineLayout{device(), PipelineLayoutCreateInfo{}};

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "compute-noop.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet.addShader(ShaderStage::Compute, shader, "main"_s);

    VkPipeline pipeline{};
    CORRADE_COMPARE(Result(device()->CreateComputePipelines(device(), {}, 1,
        ComputePipelineCreateInfo{shaderSet, pipelineLayout},
        nullptr, &pipeline)), Result::Success);

    auto wrapped = Pipeline::wrap(device(), PipelineBindPoint::Compute, pipeline, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), pipeline);
    CORRADE_COMPARE(wrapped.bindPoint(), PipelineBindPoint::Compute);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), pipeline);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyPipeline(device(), pipeline, nullptr);
}

void PipelineVkTest::dynamicRasterizationStatesNotRasterization() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    PipelineLayout pipelineLayout{device(), PipelineLayoutCreateInfo{}};

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "compute-noop.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet.addShader(ShaderStage::Compute, shader, "main"_s);

    Pipeline pipeline{device(), ComputePipelineCreateInfo{
        shaderSet, pipelineLayout
    }};

    std::ostringstream out;
    Error redirectError{&out};
    pipeline.dynamicRasterizationStates();
    CORRADE_COMPARE(out.str(), "Vk::Pipeline::dynamicRasterizationStates(): not a rasterization pipeline\n");
}

void PipelineVkTest::cmdBind() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        /* This might blow up if queue() isn't the one matching this family */
        device().properties().pickQueueFamily(QueueFlag::Graphics|QueueFlag::Compute)}};

    PipelineLayout pipelineLayout{device(), PipelineLayoutCreateInfo{}};

    Shader shader{device(), ShaderCreateInfo{
        Utility::Directory::read(Utility::Directory::join(VK_TEST_DIR, "compute-noop.spv"))
    }};

    ShaderSet shaderSet;
    shaderSet.addShader(ShaderStage::Compute, shader, "main"_s);

    Pipeline pipeline{device(), ComputePipelineCreateInfo{
        shaderSet, pipelineLayout
    }};

    pool.allocate()
        .begin()
        .bindPipeline(pipeline)
        .end();

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

void PipelineVkTest::cmdBarrier() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    Buffer buffer{device(), BufferCreateInfo{
        BufferUsage::TransferDestination|BufferUsage::VertexBuffer, 16
    }, MemoryFlag::DeviceLocal};
    Image image{device(), ImageCreateInfo2D{
        ImageUsage::TransferDestination|ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {4, 4}, 1
    }, MemoryFlag::DeviceLocal};

    pool.allocate()
        .begin()
        .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host|PipelineStage::VertexInput|PipelineStage::FragmentShader, {
            {Access::TransferWrite, Access::HostRead}
        }, {
            {Access::TransferWrite, Access::VertexAttributeRead, buffer}
        }, {
            {Access::TransferWrite, Access::ShaderRead,
             ImageLayout::Preinitialized, ImageLayout::ShaderReadOnly,
             image}
        })
        .end();

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

void PipelineVkTest::cmdBarrierExecutionOnly() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    /* A subset of the above, just to test the convenience overloads */

    pool.allocate()
        .begin()
        .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host)
        .end();

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

void PipelineVkTest::cmdBarrierGlobalMemory() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    /* A subset of the above, just to test the convenience overloads */

    pool.allocate()
        .begin()
        .pipelineBarrier(PipelineStage::Transfer, PipelineStage::Host, {
            {Access::TransferWrite, Access::HostRead}
        })
        .end();

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

void PipelineVkTest::cmdBarrierBufferMemory() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    Buffer buffer{device(), BufferCreateInfo{
        BufferUsage::TransferDestination|BufferUsage::VertexBuffer, 16
    }, MemoryFlag::DeviceLocal};

    pool.allocate()
        .begin()
        .pipelineBarrier(PipelineStage::Transfer, PipelineStage::VertexInput, {
            {Access::TransferWrite, Access::VertexAttributeRead, buffer}
        })
        .end();

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

void PipelineVkTest::cmdBarrierImageMemory() {
    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};

    Image image{device(), ImageCreateInfo2D{
        ImageUsage::TransferDestination|ImageUsage::Sampled,
        PixelFormat::RGBA8Unorm, {4, 4}, 1
    }, MemoryFlag::DeviceLocal};

    pool.allocate()
        .begin()
        .pipelineBarrier(PipelineStage::Transfer, PipelineStage::FragmentShader, {
            {Access::TransferWrite, Access::ShaderRead,
             ImageLayout::Preinitialized, ImageLayout::ShaderReadOnly,
             image}
        })
        .end();

    /* Does not do anything visible, so just test that it didn't blow up */
    CORRADE_VERIFY(true);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::PipelineVkTest)
