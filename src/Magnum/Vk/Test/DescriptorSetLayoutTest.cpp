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

#include "Magnum/Vk/DescriptorSetLayoutCreateInfo.h"
#include "Magnum/Vk/DescriptorType.h"
#include "Magnum/Vk/Shader.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DescriptorSetLayoutTest: TestSuite::Tester {
    explicit DescriptorSetLayoutTest();

    void bindingConstruct();
    void bindingConstructCountStages();
    void bindingConstructFlags();
    void bindingConstructImmutableSamplers();
    void bindingConstructNoInit();
    void bindingConstructFromVk();
    void bindingConstructCopy();
    void bindingConstructMove();

    void createInfoConstruct();
    void createInfoConstructBindingFlags();
    void createInfoConstructBindingImmutableSamplers();
    void createInfoConstructNoInit();
    void createInfoConstructFromVk();
    void createInfoConstructCopy();
    void createInfoConstructMove();

    void constructNoCreate();
    void constructCopy();
};

DescriptorSetLayoutTest::DescriptorSetLayoutTest() {
    addTests({&DescriptorSetLayoutTest::bindingConstruct,
              &DescriptorSetLayoutTest::bindingConstructCountStages,
              &DescriptorSetLayoutTest::bindingConstructFlags,
              &DescriptorSetLayoutTest::bindingConstructImmutableSamplers,
              &DescriptorSetLayoutTest::bindingConstructNoInit,
              &DescriptorSetLayoutTest::bindingConstructFromVk,
              &DescriptorSetLayoutTest::bindingConstructCopy,
              &DescriptorSetLayoutTest::bindingConstructMove,

              &DescriptorSetLayoutTest::createInfoConstruct,
              &DescriptorSetLayoutTest::createInfoConstructBindingFlags,
              &DescriptorSetLayoutTest::createInfoConstructBindingImmutableSamplers,
              &DescriptorSetLayoutTest::createInfoConstructNoInit,
              &DescriptorSetLayoutTest::createInfoConstructFromVk,
              &DescriptorSetLayoutTest::createInfoConstructCopy,
              &DescriptorSetLayoutTest::createInfoConstructMove,

              &DescriptorSetLayoutTest::constructNoCreate,
              &DescriptorSetLayoutTest::constructCopy});
}

void DescriptorSetLayoutTest::bindingConstruct() {
    DescriptorSetLayoutBinding binding{15, DescriptorType::SampledImage};
    CORRADE_COMPARE(binding->binding, 15);
    CORRADE_COMPARE(binding->descriptorType, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
    CORRADE_COMPARE(binding->descriptorCount, 1);
    CORRADE_VERIFY(!binding->pImmutableSamplers);
    CORRADE_COMPARE(binding->stageFlags, VK_SHADER_STAGE_ALL);
    CORRADE_COMPARE(binding.flags(), 0);
}

void DescriptorSetLayoutTest::bindingConstructCountStages() {
    DescriptorSetLayoutBinding binding{15, DescriptorType::SampledImage, 5, ShaderStage::Fragment|ShaderStage::RayMiss};
    CORRADE_COMPARE(binding->binding, 15);
    CORRADE_COMPARE(binding->descriptorType, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
    CORRADE_COMPARE(binding->descriptorCount, 5);
    CORRADE_VERIFY(!binding->pImmutableSamplers);
    CORRADE_COMPARE(binding->stageFlags, VK_SHADER_STAGE_FRAGMENT_BIT|VK_SHADER_STAGE_MISS_BIT_KHR);
    CORRADE_COMPARE(binding.flags(), 0);
}

void DescriptorSetLayoutTest::bindingConstructFlags() {
    DescriptorSetLayoutBinding binding{15, DescriptorType::SampledImage, 2, ShaderStage::Fragment, DescriptorSetLayoutBinding::Flag::UpdateAfterBind|DescriptorSetLayoutBinding::Flag::PartiallyBound};
    CORRADE_COMPARE(binding->binding, 15);
    CORRADE_COMPARE(binding->descriptorType, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
    CORRADE_COMPARE(binding->descriptorCount, 2);
    CORRADE_VERIFY(!binding->pImmutableSamplers);
    CORRADE_COMPARE(binding->stageFlags, VK_SHADER_STAGE_FRAGMENT_BIT);
    CORRADE_COMPARE(binding.flags(), VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT|VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
}

void DescriptorSetLayoutTest::bindingConstructImmutableSamplers() {
    DescriptorSetLayoutBinding binding{15, DescriptorType::SampledImage, {reinterpret_cast<VkSampler>(0xdead), reinterpret_cast<VkSampler>(0xbeef), reinterpret_cast<VkSampler>(0xcafe)}, ShaderStage::Fragment, DescriptorSetLayoutBinding::Flag::UpdateAfterBind};
    CORRADE_COMPARE(binding->binding, 15);
    CORRADE_COMPARE(binding->descriptorType, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
    CORRADE_COMPARE(binding->descriptorCount, 3);
    CORRADE_VERIFY(binding->pImmutableSamplers);
    CORRADE_COMPARE(binding->pImmutableSamplers[0], reinterpret_cast<VkSampler>(0xdead));
    CORRADE_COMPARE(binding->pImmutableSamplers[1], reinterpret_cast<VkSampler>(0xbeef));
    CORRADE_COMPARE(binding->pImmutableSamplers[2], reinterpret_cast<VkSampler>(0xcafe));
    CORRADE_COMPARE(binding->stageFlags, VK_SHADER_STAGE_FRAGMENT_BIT);
    CORRADE_COMPARE(binding.flags(), VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
}

void DescriptorSetLayoutTest::bindingConstructNoInit() {
    VkSampler a;
    DescriptorSetLayoutBinding info{NoInit};
    info->pImmutableSamplers = &a;
    new(&info) DescriptorSetLayoutBinding{NoInit};
    CORRADE_COMPARE(info->pImmutableSamplers, &a);

    CORRADE_VERIFY(std::is_nothrow_constructible<DescriptorSetLayoutBinding, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, DescriptorSetLayoutBinding>::value);
}

void DescriptorSetLayoutTest::bindingConstructFromVk() {
    VkSampler a;

    VkDescriptorSetLayoutBinding vkInfo;
    vkInfo.pImmutableSamplers = &a;

    DescriptorSetLayoutBinding info{vkInfo};
    CORRADE_COMPARE(info->pImmutableSamplers, &a);
}

void DescriptorSetLayoutTest::bindingConstructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DescriptorSetLayoutBinding>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DescriptorSetLayoutBinding>{});
}

void DescriptorSetLayoutTest::bindingConstructMove() {
    DescriptorSetLayoutBinding a{15, DescriptorType::SampledImage, {reinterpret_cast<VkSampler>(0xdead), reinterpret_cast<VkSampler>(0xbeef), reinterpret_cast<VkSampler>(0xcafe)}, ShaderStage::Fragment, DescriptorSetLayoutBinding::Flag::UpdateAfterBind};
    CORRADE_COMPARE(a->descriptorCount, 3);
    CORRADE_VERIFY(a->pImmutableSamplers);
    CORRADE_COMPARE(a->pImmutableSamplers[1], reinterpret_cast<VkSampler>(0xbeef));

    DescriptorSetLayoutBinding b = std::move(a);
    CORRADE_VERIFY(!a->pImmutableSamplers);
    CORRADE_COMPARE(b->descriptorCount, 3);
    CORRADE_VERIFY(b->pImmutableSamplers);
    CORRADE_COMPARE(b->pImmutableSamplers[1], reinterpret_cast<VkSampler>(0xbeef));
    CORRADE_COMPARE(b.flags(), VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);

    DescriptorSetLayoutBinding c{3, {}};
    c = std::move(b);
    CORRADE_VERIFY(!b->pImmutableSamplers);
    CORRADE_COMPARE(c->descriptorCount, 3);
    CORRADE_VERIFY(c->pImmutableSamplers);
    CORRADE_COMPARE(c->pImmutableSamplers[1], reinterpret_cast<VkSampler>(0xbeef));
    CORRADE_COMPARE(c.flags(), VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
}

void DescriptorSetLayoutTest::createInfoConstruct() {
    DescriptorSetLayoutCreateInfo info{{
        /* I hope the {{ will no longer be needed with C++14? */
        {{7, DescriptorType::UniformBuffer}},
        {{12, DescriptorType::CombinedImageSampler}}
    }, DescriptorSetLayoutCreateInfo::Flag(VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT)};
    CORRADE_COMPARE(info->bindingCount, 2);
    CORRADE_VERIFY(info->pBindings);
    CORRADE_COMPARE(info->pBindings[0].binding, 7);
    CORRADE_COMPARE(info->pBindings[0].descriptorType, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    CORRADE_COMPARE(info->pBindings[0].descriptorCount, 1);
    CORRADE_COMPARE(info->pBindings[1].binding, 12);
    CORRADE_COMPARE(info->pBindings[1].descriptorType, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    CORRADE_COMPARE(info->pBindings[1].descriptorCount, 1);
    CORRADE_COMPARE(info->flags, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT);
    CORRADE_VERIFY(!info->pNext);
}

void DescriptorSetLayoutTest::createInfoConstructBindingFlags() {
    DescriptorSetLayoutCreateInfo info{{
        {{7, DescriptorType::UniformBuffer}},
        {{12, DescriptorType::CombinedImageSampler, 1, ~ShaderStages{}, DescriptorSetLayoutBinding::Flag::PartiallyBound}}
    }};
    CORRADE_COMPARE(info->bindingCount, 2);
    CORRADE_VERIFY(info->pBindings);
    CORRADE_COMPARE(info->pBindings[0].binding, 7);
    CORRADE_COMPARE(info->pBindings[0].descriptorType, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    CORRADE_COMPARE(info->pBindings[0].descriptorCount, 1);
    CORRADE_COMPARE(info->pBindings[1].binding, 12);
    CORRADE_COMPARE(info->pBindings[1].descriptorType, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    CORRADE_COMPARE(info->pBindings[1].descriptorCount, 1);

    CORRADE_VERIFY(info->pNext);
    const auto& flags = *static_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(info->pNext);
    CORRADE_COMPARE(flags.bindingCount, 2);
    CORRADE_VERIFY(flags.pBindingFlags);
    CORRADE_COMPARE(flags.pBindingFlags[0], 0);
    CORRADE_COMPARE(flags.pBindingFlags[1], VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
}

void DescriptorSetLayoutTest::createInfoConstructBindingImmutableSamplers() {
    DescriptorSetLayoutBinding binding{3, DescriptorType::Sampler, {reinterpret_cast<VkSampler>(0xdead), reinterpret_cast<VkSampler>(0xbeef)}};

    DescriptorSetLayoutCreateInfo info{{
        {{7, DescriptorType::UniformBuffer}},
        binding,
        {{12, DescriptorType::CombinedImageSampler, {reinterpret_cast<VkSampler>(0xcafe)}}},
    }};
    CORRADE_COMPARE(info->bindingCount, 3);
    CORRADE_VERIFY(info->pBindings);

    CORRADE_COMPARE(info->pBindings[0].binding, 7);
    CORRADE_COMPARE(info->pBindings[0].descriptorType, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    CORRADE_COMPARE(info->pBindings[0].descriptorCount, 1);
    CORRADE_VERIFY(!info->pBindings[0].pImmutableSamplers);

    CORRADE_COMPARE(info->pBindings[1].binding, 3);
    CORRADE_COMPARE(info->pBindings[1].descriptorType, VK_DESCRIPTOR_TYPE_SAMPLER);
    CORRADE_COMPARE(info->pBindings[1].descriptorCount, 2);
    CORRADE_VERIFY(info->pBindings[1].pImmutableSamplers);
    /* The samplers should get copied, not referenced */
    CORRADE_VERIFY(info->pBindings[1].pImmutableSamplers != binding->pImmutableSamplers);
    CORRADE_COMPARE(info->pBindings[1].pImmutableSamplers[0], reinterpret_cast<VkSampler>(0xdead));
    CORRADE_COMPARE(info->pBindings[1].pImmutableSamplers[1], reinterpret_cast<VkSampler>(0xbeef));

    CORRADE_COMPARE(info->pBindings[2].binding, 12);
    CORRADE_COMPARE(info->pBindings[2].descriptorType, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    CORRADE_COMPARE(info->pBindings[2].descriptorCount, 1);
    CORRADE_VERIFY(info->pBindings[2].pImmutableSamplers);
    CORRADE_COMPARE(info->pBindings[2].pImmutableSamplers[0], reinterpret_cast<VkSampler>(0xcafe));

    CORRADE_VERIFY(!info->pNext);
}

void DescriptorSetLayoutTest::createInfoConstructNoInit() {
    DescriptorSetLayoutCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) DescriptorSetLayoutCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY(std::is_nothrow_constructible<DescriptorSetLayoutCreateInfo, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, DescriptorSetLayoutCreateInfo>::value);
}

void DescriptorSetLayoutTest::createInfoConstructFromVk() {
    VkDescriptorSetLayoutCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    DescriptorSetLayoutCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void DescriptorSetLayoutTest::createInfoConstructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DescriptorSetLayoutCreateInfo>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DescriptorSetLayoutCreateInfo>{});
}

void DescriptorSetLayoutTest::createInfoConstructMove() {
    DescriptorSetLayoutCreateInfo a{{
        {{7, DescriptorType::UniformBuffer}},
        {{12, DescriptorType::CombinedImageSampler, 1, ~ShaderStages{}, DescriptorSetLayoutBinding::Flag::PartiallyBound}}
    }};
    CORRADE_COMPARE(a->bindingCount, 2);
    CORRADE_VERIFY(a->pBindings);
    CORRADE_VERIFY(a->pNext);

    DescriptorSetLayoutCreateInfo b = std::move(a);
    CORRADE_COMPARE(a->bindingCount, 0);
    CORRADE_VERIFY(!a->pBindings);
    CORRADE_VERIFY(!a->pNext);
    CORRADE_VERIFY(b->pBindings);
    CORRADE_VERIFY(b->pNext);
    CORRADE_COMPARE(b->pBindings[1].binding, 12);
    CORRADE_VERIFY(static_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(b->pNext)->pBindingFlags);
    CORRADE_COMPARE(static_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(b->pNext)->pBindingFlags[1], VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

    DescriptorSetLayoutCreateInfo c{VkDescriptorSetLayoutCreateInfo{}};
    c = std::move(b);
    CORRADE_COMPARE(b->bindingCount, 0);
    CORRADE_VERIFY(!b->pBindings);
    CORRADE_VERIFY(!b->pNext);
    CORRADE_VERIFY(c->pBindings);
    CORRADE_VERIFY(c->pNext);
    CORRADE_COMPARE(c->pBindings[1].binding, 12);
    CORRADE_VERIFY(static_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(c->pNext)->pBindingFlags);
    CORRADE_COMPARE(static_cast<const VkDescriptorSetLayoutBindingFlagsCreateInfo*>(c->pNext)->pBindingFlags[1], VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);
}

void DescriptorSetLayoutTest::constructNoCreate() {
    {
        DescriptorSetLayout fence{NoCreate};
        CORRADE_VERIFY(!fence.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, DescriptorSetLayout>::value);
}

void DescriptorSetLayoutTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DescriptorSetLayout>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DescriptorSetLayout>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DescriptorSetLayoutTest)
