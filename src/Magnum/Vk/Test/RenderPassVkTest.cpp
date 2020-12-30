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

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/CommandPoolCreateInfo.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/FramebufferCreateInfo.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/ImageCreateInfo.h"
#include "Magnum/Vk/ImageViewCreateInfo.h"
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
};

RenderPassVkTest::RenderPassVkTest() {
    addTests({&RenderPassVkTest::construct,
              &RenderPassVkTest::constructNoSubpasses,
              &RenderPassVkTest::constructSubpassNoAttachments,
              &RenderPassVkTest::constructMove,

              &RenderPassVkTest::wrap,

              &RenderPassVkTest::cmdBeginEnd});
}

void RenderPassVkTest::construct() {
    {
        RenderPass renderPass{device(), RenderPassCreateInfo{}
            .setAttachments({VK_FORMAT_R8G8B8A8_SNORM})
            .addSubpass(SubpassDescription{}.setColorAttachments({0}))
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
        .setAttachments({VK_FORMAT_R8G8B8A8_SNORM})
        .addSubpass(SubpassDescription{}.setColorAttachments({0}))
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
            .setAttachments({VK_FORMAT_R8G8B8A8_SNORM})
            .addSubpass(SubpassDescription{}.setColorAttachments({0}))
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
        VK_FORMAT_R8G8B8A8_UNORM, {256, 256}, 1}, MemoryFlag::DeviceLocal};
    Image depth{device(), ImageCreateInfo2D{ImageUsage::DepthStencilAttachment,
        VK_FORMAT_D24_UNORM_S8_UINT, {256, 256}, 1}, MemoryFlag::DeviceLocal};
    ImageView colorView{device(), ImageViewCreateInfo2D{color}};
    ImageView depthView{device(), ImageViewCreateInfo2D{depth}};

    RenderPass renderPass{device(), RenderPassCreateInfo{}
        .setAttachments({
            {color.format(), AttachmentLoadOperation::Clear, {}},
            {depth.format(), AttachmentLoadOperation::Clear, {}},
        })
        .addSubpass(SubpassDescription{}
            .setColorAttachments({0})
            .setDepthStencilAttachment(1)
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
       .beginRenderPass(RenderPassBeginInfo{renderPass, framebuffer, {{}, {256, 256}}}
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

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::RenderPassVkTest)
