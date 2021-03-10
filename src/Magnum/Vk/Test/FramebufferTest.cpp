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

#include <new>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Containers/Reference.h>

#include "Magnum/Math/Vector3.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/FramebufferCreateInfo.h"
#include "Magnum/Vk/ImageView.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct FramebufferTest: TestSuite::Tester {
    explicit FramebufferTest();

    void createInfoConstruct();
    void createInfoConstructLayered();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();
    void createInfoConstructCopy();
    void createInfoConstructMove();

    void constructNoCreate();
    void constructCopy();
};

FramebufferTest::FramebufferTest() {
    addTests({&FramebufferTest::createInfoConstruct,
              &FramebufferTest::createInfoConstructLayered,
              &FramebufferTest::createInfoConstructNoInit,
              &FramebufferTest::createInfoConstructFromVk,
              &FramebufferTest::createInfoConstructCopy,
              &FramebufferTest::createInfoConstructMove,

              &FramebufferTest::constructNoCreate,
              &FramebufferTest::constructCopy});
}

void FramebufferTest::createInfoConstruct() {
    Device device{NoCreate};
    auto renderPass = reinterpret_cast<VkRenderPass>(0xdeadbeef);
    auto a = ImageView::wrap(device, reinterpret_cast<VkImageView>(0xcafe1), {});
    auto b = ImageView::wrap(device, reinterpret_cast<VkImageView>(0xcafe2), {});
    auto c = ImageView::wrap(device, reinterpret_cast<VkImageView>(0xcafe3), {});

    /** @todo use a real flag once it exists */
    FramebufferCreateInfo info{renderPass, {a, b, c}, {256, 512}, FramebufferCreateInfo::Flag(0xbadda9)};
    CORRADE_COMPARE(info->flags, 0xbadda9);
    CORRADE_COMPARE(info->renderPass, reinterpret_cast<VkRenderPass>(0xdeadbeef));
    CORRADE_COMPARE(info->attachmentCount, 3);
    CORRADE_VERIFY(info->pAttachments);
    CORRADE_COMPARE(info->pAttachments[0], reinterpret_cast<VkImageView>(0xcafe1));
    CORRADE_COMPARE(info->pAttachments[1], reinterpret_cast<VkImageView>(0xcafe2));
    CORRADE_COMPARE(info->pAttachments[2], reinterpret_cast<VkImageView>(0xcafe3));
    CORRADE_COMPARE(info->width, 256);
    CORRADE_COMPARE(info->height, 512);
    CORRADE_COMPARE(info->layers, 1);
}

void FramebufferTest::createInfoConstructLayered() {
    Device device{NoCreate};
    auto renderPass = reinterpret_cast<VkRenderPass>(0xdeadbeef);
    auto a = ImageView::wrap(device, reinterpret_cast<VkImageView>(0xcafe1), {});
    auto b = ImageView::wrap(device, reinterpret_cast<VkImageView>(0xcafe2), {});
    auto c = ImageView::wrap(device, reinterpret_cast<VkImageView>(0xcafe3), {});

    /** @todo use a real flag once it exists */
    FramebufferCreateInfo info{renderPass, {a, b, c}, {256, 512, 5}, FramebufferCreateInfo::Flag(0xbadda9)};
    CORRADE_COMPARE(info->flags, 0xbadda9);
    CORRADE_COMPARE(info->renderPass, reinterpret_cast<VkRenderPass>(0xdeadbeef));
    CORRADE_COMPARE(info->attachmentCount, 3);
    CORRADE_VERIFY(info->pAttachments);
    CORRADE_COMPARE(info->pAttachments[0], reinterpret_cast<VkImageView>(0xcafe1));
    CORRADE_COMPARE(info->pAttachments[1], reinterpret_cast<VkImageView>(0xcafe2));
    CORRADE_COMPARE(info->pAttachments[2], reinterpret_cast<VkImageView>(0xcafe3));
    CORRADE_COMPARE(info->width, 256);
    CORRADE_COMPARE(info->height, 512);
    CORRADE_COMPARE(info->layers, 5);
}

void FramebufferTest::createInfoConstructNoInit() {
    FramebufferCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) FramebufferCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY(std::is_nothrow_constructible<FramebufferCreateInfo, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, FramebufferCreateInfo>::value);
}

void FramebufferTest::createInfoConstructFromVk() {
    VkFramebufferCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    FramebufferCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void FramebufferTest::createInfoConstructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<FramebufferCreateInfo>{});
    CORRADE_VERIFY(!std::is_copy_assignable<FramebufferCreateInfo>{});
}

void FramebufferTest::createInfoConstructMove() {
    Device device{NoCreate};
    auto renderPass = reinterpret_cast<VkRenderPass>(0xdeadbeef);
    auto view = ImageView::wrap(device, reinterpret_cast<VkImageView>(0xcafe), {});

    FramebufferCreateInfo a{renderPass, {view}, {256, 512}};

    FramebufferCreateInfo b = std::move(a);
    CORRADE_COMPARE(a->attachmentCount, 0);
    CORRADE_VERIFY(!a->pAttachments);
    CORRADE_COMPARE(b->renderPass, reinterpret_cast<VkRenderPass>(0xdeadbeef));
    CORRADE_COMPARE(b->attachmentCount, 1);
    CORRADE_VERIFY(b->pAttachments);
    CORRADE_COMPARE(b->pAttachments[0], reinterpret_cast<VkImageView>(0xcafe));

    FramebufferCreateInfo c{VkFramebufferCreateInfo{}};
    c = std::move(b);
    CORRADE_COMPARE(b->attachmentCount, 0);
    CORRADE_VERIFY(!b->pAttachments);
    CORRADE_COMPARE(c->renderPass, reinterpret_cast<VkRenderPass>(0xdeadbeef));
    CORRADE_COMPARE(c->attachmentCount, 1);
    CORRADE_VERIFY(c->pAttachments);
    CORRADE_COMPARE(c->pAttachments[0], reinterpret_cast<VkImageView>(0xcafe));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<FramebufferCreateInfo>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<FramebufferCreateInfo>::value);
}

void FramebufferTest::constructNoCreate() {
    {
        Framebuffer view{NoCreate};
        CORRADE_VERIFY(!view.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, Framebuffer>::value);
}

void FramebufferTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<Framebuffer>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Framebuffer>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::FramebufferTest)
