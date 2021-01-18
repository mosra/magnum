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
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPoolCreateInfo.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/FramebufferCreateInfo.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/ImageCreateInfo.h"
#include "Magnum/Vk/ImageViewCreateInfo.h"
#include "Magnum/Vk/PixelFormat.h"
#include "Magnum/Vk/RenderPassCreateInfo.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct RenderPassVkTest: VulkanTester {
    explicit RenderPassVkTest();

    void construct();
    void constructNoSubpasses();
    void constructSubpassNoAttachments();
    void constructMove();

    void wrap();

    void cmdBeginEnd();
    void cmdBeginEndDisallowedConversion();
};

RenderPassVkTest::RenderPassVkTest() {
    addTests({&RenderPassVkTest::construct,
              &RenderPassVkTest::constructNoSubpasses,
              &RenderPassVkTest::constructSubpassNoAttachments,
              &RenderPassVkTest::constructMove,

              &RenderPassVkTest::wrap,

              &RenderPassVkTest::cmdBeginEnd,
              &RenderPassVkTest::cmdBeginEndDisallowedConversion});
}

void RenderPassVkTest::construct() {
    {
        RenderPass renderPass{device(), RenderPassCreateInfo{}
            .setAttachments({
                AttachmentDescription{PixelFormat::RGBA8Unorm,
                    AttachmentLoadOperation::Clear,
                    AttachmentStoreOperation::Store,
                    ImageLayout::Undefined,
                    ImageLayout::General}
            })
            .addSubpass(SubpassDescription{}.setColorAttachments({
                AttachmentReference{0, ImageLayout::General}
            }))
        };
        CORRADE_VERIFY(renderPass.handle());
        CORRADE_COMPARE(renderPass.handleFlags(), HandleFlag::DestroyOnDestruction);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void RenderPassVkTest::constructNoSubpasses() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    RenderPass{device(), RenderPassCreateInfo{}};
    CORRADE_COMPARE(out.str(), "Vk::RenderPass: needs to be created with at least one subpass\n");
}

void RenderPassVkTest::constructSubpassNoAttachments() {
    /* The spec requires at least one subpass, but it doesn't say anything
       about attachments, so this should work */
    RenderPass renderPass{device(), RenderPassCreateInfo{}
        .addSubpass(SubpassDescription{})
    };
    CORRADE_VERIFY(renderPass.handle());
}

void RenderPassVkTest::constructMove() {
    RenderPass a{device(), RenderPassCreateInfo{}
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
    VkRenderPass handle = a.handle();

    RenderPass b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);

    RenderPass c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<RenderPass>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<RenderPass>::value);
}

void RenderPassVkTest::wrap() {
    VkRenderPass renderPass{};
    CORRADE_COMPARE(Result(device()->CreateRenderPass(device(),
        RenderPassCreateInfo{}
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
        .vkRenderPassCreateInfo(),
        nullptr, &renderPass)), Result::Success);

    auto wrapped = RenderPass::wrap(device(), renderPass, HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(wrapped.handle(), renderPass);

    /* Release the handle again, destroy by hand */
    CORRADE_COMPARE(wrapped.release(), renderPass);
    CORRADE_VERIFY(!wrapped.handle());
    device()->DestroyRenderPass(device(), renderPass, nullptr);
}

void RenderPassVkTest::cmdBeginEnd() {
    using namespace Math::Literals;

    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();

    /* Using a depth attachment as well even though not strictly necessary to
       catch potential unexpected bugs */
    Image color{device(), ImageCreateInfo2D{ImageUsage::ColorAttachment,
        PixelFormat::RGBA8Unorm, {256, 256}, 1}, MemoryFlag::DeviceLocal};
    Image depth{device(), ImageCreateInfo2D{ImageUsage::DepthStencilAttachment,
        PixelFormat::Depth24UnormStencil8UI, {256, 256}, 1}, MemoryFlag::DeviceLocal};
    ImageView colorView{device(), ImageViewCreateInfo2D{color}};
    ImageView depthView{device(), ImageViewCreateInfo2D{depth}};

    RenderPass renderPass{device(), RenderPassCreateInfo{}
        .setAttachments({
            AttachmentDescription{color.format(),
                AttachmentLoadOperation::Clear, {},
                ImageLayout::Undefined,
                ImageLayout::ColorAttachment},
            AttachmentDescription{depth.format(),
                AttachmentLoadOperation::Clear, {},
                ImageLayout::Undefined,
                ImageLayout::ColorAttachment},
        })
        .addSubpass(SubpassDescription{}
            .setColorAttachments({
                AttachmentReference{0, ImageLayout::ColorAttachment}
            })
            .setDepthStencilAttachment(
                AttachmentReference{1, ImageLayout::DepthStencilAttachment}
            )
        )
        /* Further subpasses with no attachments so we can test nextSubpass()
           but don't need to specify subpass dependencies (which I have no idea
           about yet) */
        .addSubpass(SubpassDescription{})
        .addSubpass(SubpassDescription{})
    };

    Framebuffer framebuffer{device(), FramebufferCreateInfo{renderPass, {
        colorView,
        depthView
    }, {256, 256}}};

    cmd.begin()
       .beginRenderPass(RenderPassBeginInfo{renderPass, framebuffer}
           .clearColor(0, 0x1f1f1f_rgbf)
           .clearDepthStencil(1, 1.0f, 0))
       .nextSubpass()
       /* The above overload goes through a different code path than this */
       .nextSubpass(SubpassEndInfo{})
       .endRenderPass()
       .end();

    /* Err there's not really anything visible to verify */
    CORRADE_VERIFY(cmd.handle());
}

void RenderPassVkTest::cmdBeginEndDisallowedConversion() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    if(device().isVersionSupported(Version::Vk12) ||device().isExtensionEnabled<Extensions::KHR::create_renderpass2>())
        CORRADE_SKIP("KHR_create_renderpass2 enabled on the device, can't test");

    CommandPool pool{device(), CommandPoolCreateInfo{
        device().properties().pickQueueFamily(QueueFlag::Graphics)}};
    CommandBuffer cmd = pool.allocate();
    SubpassEndInfo endInfo;
    endInfo->pNext = &endInfo;
    SubpassBeginInfo beginInfo;
    beginInfo->pNext = &beginInfo;

    /* The commands shouldn't do anything, so it should be fine to just call
       them without any render pass set up */
    std::ostringstream out;
    Error redirectError{&out};
    cmd.beginRenderPass(RenderPassBeginInfo{NoInit}, beginInfo)
       .nextSubpass(beginInfo)
       .nextSubpass(endInfo)
       .endRenderPass(endInfo);
    CORRADE_COMPARE(out.str(),
        "Vk::CommandBuffer::beginRenderPass(): disallowing conversion of SubpassBeginInfo to VkSubpassContents with non-empty pNext to prevent information loss\n"
        "Vk::CommandBuffer::nextRenderPass(): disallowing conversion of SubpassBeginInfo to VkSubpassContents with non-empty pNext to prevent information loss\n"
        "Vk::CommandBuffer::nextRenderPass(): disallowing omission of SubpassEndInfo with non-empty pNext to prevent information loss\n"
        "Vk::CommandBuffer::endRenderPass(): disallowing omission of SubpassEndInfo with non-empty pNext to prevent information loss\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::RenderPassVkTest)
