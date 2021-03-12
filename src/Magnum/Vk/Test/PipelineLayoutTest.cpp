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
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Vk/PipelineLayoutCreateInfo.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct PipelineLayoutTest: TestSuite::Tester {
    explicit PipelineLayoutTest();

    void createInfoConstruct();
    void createInfoConstructDescriptorSetLayouts();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();
    void createInfoConstructCopy();
    void createInfoConstructMove();

    void constructNoCreate();
    void constructCopy();
};

PipelineLayoutTest::PipelineLayoutTest() {
    addTests({&PipelineLayoutTest::createInfoConstruct,
              &PipelineLayoutTest::createInfoConstructDescriptorSetLayouts,
              &PipelineLayoutTest::createInfoConstructNoInit,
              &PipelineLayoutTest::createInfoConstructFromVk,
              &PipelineLayoutTest::createInfoConstructCopy,
              &PipelineLayoutTest::createInfoConstructMove,

              &PipelineLayoutTest::constructNoCreate,
              &PipelineLayoutTest::constructCopy});
}

void PipelineLayoutTest::createInfoConstruct() {
    PipelineLayoutCreateInfo info;
    CORRADE_COMPARE(info->flags, 0);
    CORRADE_COMPARE(info->setLayoutCount, 0);
}

void PipelineLayoutTest::createInfoConstructDescriptorSetLayouts() {
    VkDescriptorSetLayout layouts[]{reinterpret_cast<VkDescriptorSetLayout>(0xdead), reinterpret_cast<VkDescriptorSetLayout>(0xbeef)};

    PipelineLayoutCreateInfo info{layouts};
    CORRADE_COMPARE(info->setLayoutCount, 2);
    CORRADE_VERIFY(info->pSetLayouts);
    /* The contents should be copied */
    CORRADE_VERIFY(info->pSetLayouts != layouts);
    CORRADE_COMPARE(info->pSetLayouts[0], reinterpret_cast<VkDescriptorSetLayout>(0xdead));
    CORRADE_COMPARE(info->pSetLayouts[1], reinterpret_cast<VkDescriptorSetLayout>(0xbeef));
}

void PipelineLayoutTest::createInfoConstructNoInit() {
    PipelineLayoutCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) PipelineLayoutCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY(std::is_nothrow_constructible<PipelineLayoutCreateInfo, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, PipelineLayoutCreateInfo>::value);
}

void PipelineLayoutTest::createInfoConstructFromVk() {
    VkPipelineLayoutCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    PipelineLayoutCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void PipelineLayoutTest::createInfoConstructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<PipelineLayoutCreateInfo>{});
    CORRADE_VERIFY(!std::is_copy_assignable<PipelineLayoutCreateInfo>{});
}

void PipelineLayoutTest::createInfoConstructMove() {
    PipelineLayoutCreateInfo a{reinterpret_cast<VkDescriptorSetLayout>(0xdead), reinterpret_cast<VkDescriptorSetLayout>(0xbeef)};
    CORRADE_COMPARE(a->setLayoutCount, 2);
    CORRADE_VERIFY(a->pSetLayouts);

    PipelineLayoutCreateInfo b = std::move(a);
    CORRADE_COMPARE(a->setLayoutCount, 0);
    CORRADE_VERIFY(!a->pSetLayouts);
    CORRADE_COMPARE(b->setLayoutCount, 2);
    CORRADE_VERIFY(b->pSetLayouts);
    CORRADE_COMPARE(b->pSetLayouts[1], reinterpret_cast<VkDescriptorSetLayout>(0xbeef));

    PipelineLayoutCreateInfo c;
    c = std::move(b);
    CORRADE_COMPARE(b->setLayoutCount, 0);
    CORRADE_VERIFY(!b->pSetLayouts);
    CORRADE_COMPARE(c->setLayoutCount, 2);
    CORRADE_VERIFY(c->pSetLayouts);
    CORRADE_COMPARE(c->pSetLayouts[1], reinterpret_cast<VkDescriptorSetLayout>(0xbeef));
}

void PipelineLayoutTest::constructNoCreate() {
    {
        PipelineLayout layout{NoCreate};
        CORRADE_VERIFY(!layout.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, PipelineLayout>::value);
}

void PipelineLayoutTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<PipelineLayout>{});
    CORRADE_VERIFY(!std::is_copy_assignable<PipelineLayout>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::PipelineLayoutTest)
