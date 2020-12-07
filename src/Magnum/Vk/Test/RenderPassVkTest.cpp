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

#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/RenderPass.h"
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
};

RenderPassVkTest::RenderPassVkTest() {
    addTests({&RenderPassVkTest::construct,
              &RenderPassVkTest::constructNoSubpasses,
              &RenderPassVkTest::constructSubpassNoAttachments,
              &RenderPassVkTest::constructMove,

              &RenderPassVkTest::wrap});
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

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::RenderPassVkTest)
