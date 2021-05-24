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
#include <sstream>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Math/Range.h"
#include "Magnum/Vk/ComputePipelineCreateInfo.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Image.h"
#include "Magnum/Vk/MeshLayout.h"
#include "Magnum/Vk/Pipeline.h"
#include "Magnum/Vk/PixelFormat.h"
#include "Magnum/Vk/RasterizationPipelineCreateInfo.h"
#include "Magnum/Vk/Shader.h"
#include "Magnum/Vk/ShaderSet.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct PipelineTest: TestSuite::Tester {
    explicit PipelineTest();

    void dynamicRasterizationStateMapping();

    void rasterizationCreateInfoConstruct();
    void rasterizationCreateInfoConstructNoInit();
    void rasterizationCreateInfoConstructFromVk();
    void rasterizationCreateInfoConstructCopy();
    void rasterizationCreateInfoConstructMove();
    void rasterizationCreateInfoConstructMoveExternalPointers();

    void rasterizationCreateInfoViewportScissor();
    void rasterizationCreateInfoViewport2DScissor();
    void rasterizationCreateInfoViewportImplicitScissor();
    void rasterizationCreateInfoViewport2DImplicitScissor();
    void rasterizationCreateInfoDynamicState();

    void computeCreateInfoConstruct();
    void computeCreateInfoConstructOwnedEntrypoint();
    void computeCreateInfoConstructNotSingleShader();
    void computeCreateInfoConstructNoInit();
    void computeCreateInfoConstructFromVk();

    void constructNoCreate();
    void constructCopy();

    void memoryBarrierConstruct();
    void memoryBarrierConstructNoInit();
    void memoryBarrierConstructFromVk();

    void bufferMemoryBarrierConstruct();
    void bufferMemoryBarrierConstructNoInit();
    void bufferMemoryBarrierConstructFromVk();

    void imageMemoryBarrierConstruct();
    void imageMemoryBarrierConstructImplicitAspect();
    void imageMemoryBarrierConstructNoInit();
    void imageMemoryBarrierConstructFromVk();

    void debugBindPoint();
    void debugDynamicRasterizationState();
    void debugDynamicRasterizationStates();
};

PipelineTest::PipelineTest() {
    addTests({&PipelineTest::dynamicRasterizationStateMapping,

              &PipelineTest::rasterizationCreateInfoConstruct,
              &PipelineTest::rasterizationCreateInfoConstructNoInit,
              &PipelineTest::rasterizationCreateInfoConstructFromVk,
              &PipelineTest::rasterizationCreateInfoConstructCopy,
              &PipelineTest::rasterizationCreateInfoConstructMove,
              &PipelineTest::rasterizationCreateInfoConstructMoveExternalPointers,

              &PipelineTest::rasterizationCreateInfoViewportScissor,
              &PipelineTest::rasterizationCreateInfoViewport2DScissor,
              &PipelineTest::rasterizationCreateInfoViewportImplicitScissor,
              &PipelineTest::rasterizationCreateInfoViewport2DImplicitScissor,
              &PipelineTest::rasterizationCreateInfoDynamicState,

              &PipelineTest::computeCreateInfoConstruct,
              &PipelineTest::computeCreateInfoConstructOwnedEntrypoint,
              &PipelineTest::computeCreateInfoConstructNotSingleShader,
              &PipelineTest::computeCreateInfoConstructNoInit,
              &PipelineTest::computeCreateInfoConstructFromVk,

              &PipelineTest::constructNoCreate,
              &PipelineTest::constructCopy,

              &PipelineTest::memoryBarrierConstruct,
              &PipelineTest::memoryBarrierConstructNoInit,
              &PipelineTest::memoryBarrierConstructFromVk,

              &PipelineTest::bufferMemoryBarrierConstruct,
              &PipelineTest::bufferMemoryBarrierConstructNoInit,
              &PipelineTest::bufferMemoryBarrierConstructFromVk,

              &PipelineTest::imageMemoryBarrierConstruct,
              &PipelineTest::imageMemoryBarrierConstructImplicitAspect,
              &PipelineTest::imageMemoryBarrierConstructNoInit,
              &PipelineTest::imageMemoryBarrierConstructFromVk,

              &PipelineTest::debugBindPoint,
              &PipelineTest::debugDynamicRasterizationState,
              &PipelineTest::debugDynamicRasterizationStates});
}

using namespace Containers::Literals;

void PipelineTest::dynamicRasterizationStateMapping() {
    /* Same table is in Pipeline.cpp, here just to have something to test the
       order with -- without this, correct order can't be verified */
    constexpr VkDynamicState mapping[]{
        #define _c(state, vkState) VK_DYNAMIC_STATE_ ## vkState,
        #include "Magnum/Vk/Implementation/dynamicRasterizationStateMapping.hpp"
        #undef _c
    };

    /* This goes through all 8 bits of the enum range */
    UnsignedInt firstUnhandled = 0xff;
    UnsignedInt nextHandled = 0;
    for(UnsignedInt i = 0; i <= 0xff; ++i) {
        const auto state = DynamicRasterizationState(i);
        /* Each case verifies:
           - that the entries are ordered by number by comparing a function to
             expected result (so insertion here is done in proper place)
           - that there was no gap (unhandled value inside the range) */
        #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic error "-Wswitch"
        #endif
        switch(state) {
            #define _c(state, vkState) \
                case DynamicRasterizationState::state: { \
                    CORRADE_ITERATION(#state); \
                    CORRADE_COMPARE(nextHandled, i); \
                    CORRADE_COMPARE(firstUnhandled, 0xff); \
                    CORRADE_COMPARE(mapping[i], VK_DYNAMIC_STATE_ ## vkState); \
                    ++nextHandled; \
                    continue; \
                }
            #include "Magnum/Vk/Implementation/dynamicRasterizationStateMapping.hpp"
            #undef _c
        }
        #ifdef __GNUC__
        #pragma GCC diagnostic pop
        #endif

        /* Not handled by any value, remember -- we might either be at the end
           of the enum range (which is okay) or some value might be unhandled
           here */
        firstUnhandled = i;
    }

    CORRADE_COMPARE(firstUnhandled, 0xff);
}

void PipelineTest::rasterizationCreateInfoConstruct() {
    ShaderSet shaderSet;
    shaderSet
        .addShader({}, {}, {})
        .addShader({}, {}, {});

    MeshLayout meshLayout{MeshPrimitive::Triangles};

    RasterizationPipelineCreateInfo info{shaderSet, meshLayout, reinterpret_cast<VkPipelineLayout>(0xdead), reinterpret_cast<VkRenderPass>(0xbeef), 15, 3, RasterizationPipelineCreateInfo::Flag::DisableOptimization|RasterizationPipelineCreateInfo::Flag::AllowDerivatives};
    CORRADE_COMPARE(info->flags, VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT|VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT);
    CORRADE_COMPARE(info->stageCount, 2);
    CORRADE_COMPARE(info->pStages, shaderSet.stages());
    CORRADE_COMPARE(info->pVertexInputState, &meshLayout.vkPipelineVertexInputStateCreateInfo());
    CORRADE_COMPARE(info->pInputAssemblyState, &meshLayout.vkPipelineInputAssemblyStateCreateInfo());
    CORRADE_VERIFY(!info->pViewportState);
    CORRADE_VERIFY(info->pRasterizationState);
    /* Not testing the default-filled pRasterizationState properties, feels as
       excessive as verifying sType would */
    CORRADE_VERIFY(info->pMultisampleState);
    /* Not testing the default-filled pMultisampleState nested properties,
       feels as excessive as verifying sType would */
    CORRADE_VERIFY(info->pDepthStencilState);
    /* Not testing the default-filled pDepthStencilState nested properties,
       feels as excessive as verifying sType would */
    CORRADE_VERIFY(info->pColorBlendState);
    CORRADE_COMPARE(info->pColorBlendState->attachmentCount, 3);
    CORRADE_VERIFY(info->pColorBlendState->pAttachments);
    for(std::size_t i = 0; i != 3; ++i) {
        CORRADE_ITERATION(i);
        CORRADE_COMPARE(info->pColorBlendState->pAttachments[i].colorWriteMask, VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT);
    }
    CORRADE_VERIFY(!info->pDynamicState);
    CORRADE_COMPARE(info->layout, reinterpret_cast<VkPipelineLayout>(0xdead));
    CORRADE_COMPARE(info->renderPass, reinterpret_cast<VkRenderPass>(0xbeef));
    CORRADE_COMPARE(info->subpass, 15);
}

void PipelineTest::rasterizationCreateInfoConstructNoInit() {
    RasterizationPipelineCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) RasterizationPipelineCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY(std::is_nothrow_constructible<RasterizationPipelineCreateInfo, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, RasterizationPipelineCreateInfo>::value);
}

void PipelineTest::rasterizationCreateInfoConstructFromVk() {
    /* These keep referenced */
    int next{};
    VkPipelineShaderStageCreateInfo shaderSet{};
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkPipelineTessellationStateCreateInfo tessellation{};

    /* These get copied. Use deliberately wrong sType fields to check the
       contents are copied verbatim. */
    VkPipelineViewportStateCreateInfo viewport{};
    viewport.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
    VkPipelineRasterizationStateCreateInfo rasterization{};
    rasterization.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2_KHR;
    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES;
    VkPipelineDynamicStateCreateInfo dynamic{};
    dynamic.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
    VkGraphicsPipelineCreateInfo vkInfo{};
    vkInfo.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_DEVICE_GROUP_INFO;
    vkInfo.pNext = &next;
    vkInfo.pStages = &shaderSet;
    vkInfo.pVertexInputState = &vertexInput;
    vkInfo.pInputAssemblyState = &inputAssembly;
    vkInfo.pTessellationState = &tessellation;
    vkInfo.pViewportState = &viewport;
    vkInfo.pRasterizationState = &rasterization;
    vkInfo.pMultisampleState = &multisample;
    vkInfo.pDepthStencilState = &depthStencil;
    vkInfo.pColorBlendState = &colorBlend;
    vkInfo.pDynamicState = &dynamic;

    RasterizationPipelineCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_DEVICE_GROUP_INFO);
    CORRADE_COMPARE(info->pNext, &next);
    CORRADE_COMPARE(info->pStages, &shaderSet);
    CORRADE_COMPARE(info->pVertexInputState, &vertexInput);
    CORRADE_COMPARE(info->pInputAssemblyState, &inputAssembly);
    CORRADE_COMPARE(info->pTessellationState, &tessellation);
    CORRADE_VERIFY(info->pViewportState);
    CORRADE_VERIFY(info->pViewportState != &viewport);
    CORRADE_COMPARE(info->pViewportState->sType, VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2);
    CORRADE_VERIFY(info->pRasterizationState);
    CORRADE_VERIFY(info->pRasterizationState != &rasterization);
    CORRADE_COMPARE(info->pRasterizationState->sType, VK_STRUCTURE_TYPE_BUFFER_COPY_2_KHR);
    CORRADE_VERIFY(info->pMultisampleState);
    CORRADE_VERIFY(info->pMultisampleState != &multisample);
    CORRADE_COMPARE(info->pMultisampleState->sType, VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2);
    CORRADE_VERIFY(info->pDepthStencilState);
    CORRADE_VERIFY(info->pDepthStencilState != &depthStencil);
    CORRADE_COMPARE(info->pDepthStencilState->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
    CORRADE_VERIFY(info->pColorBlendState);
    CORRADE_VERIFY(info->pColorBlendState != &colorBlend);
    CORRADE_COMPARE(info->pColorBlendState->sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES);
    CORRADE_VERIFY(info->pDynamicState);
    CORRADE_VERIFY(info->pDynamicState != &dynamic);
    CORRADE_COMPARE(info->pDynamicState->sType, VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT);
}

void PipelineTest::rasterizationCreateInfoConstructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<RasterizationPipelineCreateInfo>{});
    CORRADE_VERIFY(!std::is_copy_assignable<RasterizationPipelineCreateInfo>{});
}

void PipelineTest::rasterizationCreateInfoConstructMove() {
    ShaderSet shaderSet;
    shaderSet
        .addShader({}, {}, {})
        .addShader({}, {}, {});

    MeshLayout meshLayout{MeshPrimitive::Triangles};

    Containers::Pointer<RasterizationPipelineCreateInfo> a{InPlaceInit, shaderSet, meshLayout, VkPipelineLayout{}, VkRenderPass{}, 0u, 3u};
    (*a).setViewport(Range3D{})
        .setDynamicStates(DynamicRasterizationState::CullMode);

    Containers::Pointer<RasterizationPipelineCreateInfo> b{InPlaceInit, std::move(*a)};
    CORRADE_COMPARE((**a).stageCount, 0);
    CORRADE_VERIFY(!(**a).pStages);
    CORRADE_VERIFY(!(**a).pVertexInputState);
    CORRADE_VERIFY(!(**a).pInputAssemblyState);
    CORRADE_VERIFY(!(**a).pViewportState);
    CORRADE_VERIFY(!(**a).pRasterizationState);
    CORRADE_VERIFY(!(**a).pMultisampleState);
    CORRADE_VERIFY(!(**a).pDepthStencilState);
    CORRADE_VERIFY(!(**a).pColorBlendState);
    CORRADE_VERIFY(!(**a).pDynamicState);

    /* Clear the original instance to verify we don't reference any of its
       members */
    a.reset();
    CORRADE_COMPARE((**b).stageCount, 2);
    CORRADE_COMPARE((**b).pStages, shaderSet.stages());
    CORRADE_COMPARE((**b).pVertexInputState, &meshLayout.vkPipelineVertexInputStateCreateInfo());
    CORRADE_COMPARE((**b).pInputAssemblyState, &meshLayout.vkPipelineInputAssemblyStateCreateInfo());
    CORRADE_VERIFY((**b).pViewportState);
    CORRADE_COMPARE((**b).pViewportState->sType, VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO);
    CORRADE_VERIFY((**b).pRasterizationState);
    CORRADE_COMPARE((**b).pRasterizationState->sType, VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
    CORRADE_VERIFY((**b).pMultisampleState);
    CORRADE_COMPARE((**b).pMultisampleState->sType, VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO);
    CORRADE_VERIFY((**b).pDepthStencilState);
    CORRADE_COMPARE((**b).pDepthStencilState->sType, VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO);
    CORRADE_VERIFY((**b).pColorBlendState);
    CORRADE_COMPARE((**b).pColorBlendState->sType, VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
    CORRADE_COMPARE((**b).pColorBlendState->attachmentCount, 3);
    CORRADE_COMPARE((**b).pColorBlendState->pAttachments[2].colorWriteMask, VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT);
    CORRADE_VERIFY((**b).pDynamicState);
    CORRADE_COMPARE((**b).pDynamicState->sType, VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO);

    Containers::Pointer<RasterizationPipelineCreateInfo> c{InPlaceInit, VkGraphicsPipelineCreateInfo{}};
    *c = std::move(*b);
    CORRADE_COMPARE((**b).stageCount, 0);
    CORRADE_VERIFY(!(**b).pStages);
    CORRADE_VERIFY(!(**b).pVertexInputState);
    CORRADE_VERIFY(!(**b).pInputAssemblyState);
    CORRADE_VERIFY(!(**b).pViewportState);
    CORRADE_VERIFY(!(**b).pRasterizationState);
    CORRADE_VERIFY(!(**b).pMultisampleState);
    CORRADE_VERIFY(!(**b).pDepthStencilState);
    CORRADE_VERIFY(!(**b).pColorBlendState);
    CORRADE_VERIFY(!(**b).pDynamicState);

    /* Clear the original instance to verify we don't reference any of its
       members */
    b.reset();
    CORRADE_COMPARE((**c).stageCount, 2);
    CORRADE_COMPARE((**c).pStages, shaderSet.stages());
    CORRADE_COMPARE((**c).pVertexInputState, &meshLayout.vkPipelineVertexInputStateCreateInfo());
    CORRADE_COMPARE((**c).pInputAssemblyState, &meshLayout.vkPipelineInputAssemblyStateCreateInfo());
    CORRADE_VERIFY((**c).pViewportState);
    CORRADE_COMPARE((**c).pViewportState->sType, VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO);
    CORRADE_VERIFY((**c).pRasterizationState);
    CORRADE_COMPARE((**c).pRasterizationState->sType, VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
    CORRADE_VERIFY((**c).pMultisampleState);
    CORRADE_COMPARE((**c).pMultisampleState->sType, VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO);
    CORRADE_VERIFY((**c).pDepthStencilState);
    CORRADE_COMPARE((**c).pDepthStencilState->sType, VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO);
    CORRADE_VERIFY((**c).pColorBlendState);
    CORRADE_COMPARE((**c).pColorBlendState->sType, VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
    CORRADE_COMPARE((**c).pColorBlendState->attachmentCount, 3);
    CORRADE_COMPARE((**c).pColorBlendState->pAttachments[2].colorWriteMask, VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT);
    CORRADE_VERIFY((**c).pDynamicState);
    CORRADE_COMPARE((**c).pDynamicState->sType, VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<RasterizationPipelineCreateInfo>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<RasterizationPipelineCreateInfo>::value);
}

void PipelineTest::rasterizationCreateInfoConstructMoveExternalPointers() {
    VkPipelineViewportStateCreateInfo viewport{};
    VkPipelineRasterizationStateCreateInfo rasterization{};
    VkPipelineMultisampleStateCreateInfo multisample{};
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    VkPipelineColorBlendStateCreateInfo colorBlend{};
    VkPipelineDynamicStateCreateInfo dynamic{};

    ShaderSet shaderSet;
    shaderSet
        .addShader({}, {}, {})
        .addShader({}, {}, {});

    MeshLayout meshLayout{MeshPrimitive::Triangles};

    /* Reroute the substructure pointers to external data */
    RasterizationPipelineCreateInfo a{shaderSet, meshLayout, {}, {}, 0, 0};
    a->pViewportState = &viewport;
    a->pRasterizationState = &rasterization;
    a->pMultisampleState = &multisample;
    a->pDepthStencilState = &depthStencil;
    a->pColorBlendState = &colorBlend;
    a->pDynamicState = &dynamic;

    /* The external pointers should stay external, not rerouted to internal */
    RasterizationPipelineCreateInfo b = std::move(a);
    CORRADE_COMPARE(b->pViewportState, &viewport);
    CORRADE_COMPARE(b->pRasterizationState, &rasterization);
    CORRADE_COMPARE(b->pMultisampleState, &multisample);
    CORRADE_COMPARE(b->pDepthStencilState, &depthStencil);
    CORRADE_COMPARE(b->pColorBlendState, &colorBlend);
    CORRADE_COMPARE(b->pDynamicState, &dynamic);

    RasterizationPipelineCreateInfo c{VkGraphicsPipelineCreateInfo{}};
    c = std::move(b);
    CORRADE_COMPARE(c->pViewportState, &viewport);
    CORRADE_COMPARE(c->pRasterizationState, &rasterization);
    CORRADE_COMPARE(c->pMultisampleState, &multisample);
    CORRADE_COMPARE(c->pDepthStencilState, &depthStencil);
    CORRADE_COMPARE(c->pColorBlendState, &colorBlend);
    CORRADE_COMPARE(c->pDynamicState, &dynamic);
}

void PipelineTest::rasterizationCreateInfoViewportScissor() {
    ShaderSet shaderSet;
    MeshLayout meshLayout{MeshPrimitive::Triangles};
    RasterizationPipelineCreateInfo info{shaderSet, meshLayout, {}, {}, 0, 1};
    info.setViewport({{1.5f, 3.5f, 5.5f}, {8, 12, 14}}, {{15, 34}, {84, 72}});

    CORRADE_VERIFY(info->pViewportState);
    CORRADE_COMPARE(info->pViewportState->viewportCount, 1);
    CORRADE_VERIFY(info->pViewportState->pViewports);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].x, 1.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].y, 3.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].minDepth, 5.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].width, 6.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].height, 8.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].maxDepth, 14.0f);
    CORRADE_COMPARE(info->pViewportState->scissorCount, 1);
    CORRADE_VERIFY(info->pViewportState->pScissors);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].offset.x, 15);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].offset.y, 34);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].extent.width, 69);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].extent.height, 38);
}

void PipelineTest::rasterizationCreateInfoViewport2DScissor() {
    ShaderSet shaderSet;
    MeshLayout meshLayout{MeshPrimitive::Triangles};
    RasterizationPipelineCreateInfo info{shaderSet, meshLayout, {}, {}, 0, 1};
    info.setViewport({{1.5f, 3.5f}, {8, 12}}, {{15, 34}, {84, 72}});

    CORRADE_VERIFY(info->pViewportState);
    CORRADE_COMPARE(info->pViewportState->viewportCount, 1);
    CORRADE_VERIFY(info->pViewportState->pViewports);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].x, 1.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].y, 3.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].minDepth, 0.0f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].width, 6.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].height, 8.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].maxDepth, 1.0f);
    CORRADE_COMPARE(info->pViewportState->scissorCount, 1);
    CORRADE_VERIFY(info->pViewportState->pScissors);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].offset.x, 15);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].offset.y, 34);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].extent.width, 69);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].extent.height, 38);
}

void PipelineTest::rasterizationCreateInfoViewportImplicitScissor() {
    ShaderSet shaderSet;
    MeshLayout meshLayout{MeshPrimitive::Triangles};
    RasterizationPipelineCreateInfo info{shaderSet, meshLayout, {}, {}, 0, 1};
    info.setViewport({{1.5f, 3.5f, 5.5f}, {8, 12, 14}});

    CORRADE_VERIFY(info->pViewportState);
    CORRADE_COMPARE(info->pViewportState->viewportCount, 1);
    CORRADE_VERIFY(info->pViewportState->pViewports);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].x, 1.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].y, 3.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].minDepth, 5.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].width, 6.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].height, 8.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].maxDepth, 14.0f);
    CORRADE_COMPARE(info->pViewportState->scissorCount, 1);
    CORRADE_VERIFY(info->pViewportState->pScissors);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].offset.x, 1);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].offset.y, 3);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].extent.width, 7);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].extent.height, 9);
}

void PipelineTest::rasterizationCreateInfoViewport2DImplicitScissor() {
    ShaderSet shaderSet;
    MeshLayout meshLayout{MeshPrimitive::Triangles};
    RasterizationPipelineCreateInfo info{shaderSet, meshLayout, {}, {}, 0, 1};
    info.setViewport({{1.5f, 3.5f}, {8, 12}});

    CORRADE_VERIFY(info->pViewportState);
    CORRADE_COMPARE(info->pViewportState->viewportCount, 1);
    CORRADE_VERIFY(info->pViewportState->pViewports);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].x, 1.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].y, 3.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].minDepth, 0.0f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].width, 6.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].height, 8.5f);
    CORRADE_COMPARE(info->pViewportState->pViewports[0].maxDepth, 1.0f);
    CORRADE_COMPARE(info->pViewportState->scissorCount, 1);
    CORRADE_VERIFY(info->pViewportState->pScissors);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].offset.x, 1);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].offset.y, 3);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].extent.width, 7);
    CORRADE_COMPARE(info->pViewportState->pScissors[0].extent.height, 9);
}

void PipelineTest::rasterizationCreateInfoDynamicState() {
    MeshLayout meshLayout{MeshPrimitive::Triangles};
    RasterizationPipelineCreateInfo info{ShaderSet{}, meshLayout, VkPipelineLayout{}, VkRenderPass{}, 0, 0};
    CORRADE_VERIFY(!info->pDynamicState);

    info.setDynamicStates(
        DynamicRasterizationState::CullMode|
        DynamicRasterizationState::DepthBoundsTestEnable|
        DynamicRasterizationState::ScissorWithCount);

    CORRADE_VERIFY(info->pDynamicState);
    CORRADE_COMPARE(info->pDynamicState->dynamicStateCount, 3);
    CORRADE_VERIFY(info->pDynamicState->pDynamicStates);
    CORRADE_COMPARE(info->pDynamicState->pDynamicStates[0], VK_DYNAMIC_STATE_CULL_MODE_EXT);
    CORRADE_COMPARE(info->pDynamicState->pDynamicStates[1], VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT_EXT);
    CORRADE_COMPARE(info->pDynamicState->pDynamicStates[2], VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE_EXT);
}

void PipelineTest::computeCreateInfoConstruct() {
    ShaderSet shaderSet;
    Containers::StringView name = "dead"_s;
    /* Yes, I know Fragment is wrong, it's just for testing */
    shaderSet.addShader(ShaderStage::Fragment, reinterpret_cast<VkShaderModule>(0xbeef), name);

    ComputePipelineCreateInfo info{shaderSet, reinterpret_cast<VkPipelineLayout>(0xdead), ComputePipelineCreateInfo::Flag::DisableOptimization|ComputePipelineCreateInfo::Flag::AllowDerivatives};
    CORRADE_COMPARE(info->flags, VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT|VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT);
    CORRADE_COMPARE(info->stage.stage, VK_SHADER_STAGE_FRAGMENT_BIT);
    CORRADE_COMPARE(info->stage.module, reinterpret_cast<VkShaderModule>(0xbeef));
    CORRADE_COMPARE(info->stage.pName, name.data());
    CORRADE_COMPARE(info->layout, reinterpret_cast<VkPipelineLayout>(0xdead));
}

void PipelineTest::computeCreateInfoConstructOwnedEntrypoint() {
    ShaderSet shaderSet;
    shaderSet.addShader({}, {}, "dead!"_s.except(1));

    ComputePipelineCreateInfo info{shaderSet, {}};
    CORRADE_COMPARE(info->stage.pName, "dead"_s);
    {
        CORRADE_EXPECT_FAIL("ComputePipelineCreateInfo currently expects the ShaderSet to stay in scope, referencing its internals.");
        CORRADE_VERIFY(info->stage.pName != shaderSet.stages()[0].pName);
    }
}

void PipelineTest::computeCreateInfoConstructNotSingleShader() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    ShaderSet shaderSet;

    std::ostringstream out;
    Error redirectError{&out};
    ComputePipelineCreateInfo info{shaderSet, {}};
    CORRADE_COMPARE(out.str(), "Vk::ComputePipelineCreateInfo: the shader set has to contain exactly one shader, got 0\n");
}

void PipelineTest::computeCreateInfoConstructNoInit() {
    ComputePipelineCreateInfo info{NoInit};
    info->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&info) ComputePipelineCreateInfo{NoInit};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY(std::is_nothrow_constructible<ComputePipelineCreateInfo, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, ComputePipelineCreateInfo>::value);
}

void PipelineTest::computeCreateInfoConstructFromVk() {
    VkComputePipelineCreateInfo vkInfo;
    vkInfo.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    ComputePipelineCreateInfo info{vkInfo};
    CORRADE_COMPARE(info->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void PipelineTest::constructNoCreate() {
    {
        Pipeline pipeline{NoCreate};
        CORRADE_VERIFY(!pipeline.handle());
    }

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoCreateT, Pipeline>::value);
}

void PipelineTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<Pipeline>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Pipeline>{});
}

void PipelineTest::memoryBarrierConstruct() {
    MemoryBarrier barrier{Access::ColorAttachmentWrite|Access::DepthStencilAttachmentWrite, Access::TransferRead};
    CORRADE_COMPARE(barrier->srcAccessMask, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    CORRADE_COMPARE(barrier->dstAccessMask, VK_ACCESS_TRANSFER_READ_BIT);
}

void PipelineTest::memoryBarrierConstructNoInit() {
    MemoryBarrier barrier{NoInit};
    barrier->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&barrier) MemoryBarrier{NoInit};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY(std::is_nothrow_constructible<MemoryBarrier, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, MemoryBarrier>::value);
}

void PipelineTest::memoryBarrierConstructFromVk() {
    VkMemoryBarrier vkBarrier;
    vkBarrier.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    MemoryBarrier barrier{vkBarrier};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void PipelineTest::bufferMemoryBarrierConstruct() {
    BufferMemoryBarrier barrier{Access::ColorAttachmentWrite|Access::DepthStencilAttachmentWrite, Access::TransferRead, reinterpret_cast<VkBuffer>(0xdead), 3, 5};
    CORRADE_COMPARE(barrier->srcAccessMask, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    CORRADE_COMPARE(barrier->dstAccessMask, VK_ACCESS_TRANSFER_READ_BIT);
    CORRADE_COMPARE(barrier->buffer, reinterpret_cast<VkBuffer>(0xdead));
    CORRADE_COMPARE(barrier->offset, 3);
    CORRADE_COMPARE(barrier->size, 5);
}

void PipelineTest::bufferMemoryBarrierConstructNoInit() {
    BufferMemoryBarrier barrier{NoInit};
    barrier->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&barrier) BufferMemoryBarrier{NoInit};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY(std::is_nothrow_constructible<BufferMemoryBarrier, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, BufferMemoryBarrier>::value);
}

void PipelineTest::bufferMemoryBarrierConstructFromVk() {
    VkBufferMemoryBarrier vkBarrier;
    vkBarrier.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    BufferMemoryBarrier barrier{vkBarrier};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void PipelineTest::imageMemoryBarrierConstruct() {
    ImageMemoryBarrier barrier{Access::ColorAttachmentWrite|Access::DepthStencilAttachmentWrite, Access::TransferRead, ImageLayout::Preinitialized, ImageLayout::TransferSource, reinterpret_cast<VkImage>(0xdead), ImageAspect::Color|ImageAspect::Depth, 3, 5, 7, 9};
    CORRADE_COMPARE(barrier->srcAccessMask, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    CORRADE_COMPARE(barrier->dstAccessMask, VK_ACCESS_TRANSFER_READ_BIT);
    CORRADE_COMPARE(barrier->oldLayout, VK_IMAGE_LAYOUT_PREINITIALIZED);
    CORRADE_COMPARE(barrier->newLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    CORRADE_COMPARE(barrier->image, reinterpret_cast<VkImage>(0xdead));
    CORRADE_COMPARE(barrier->subresourceRange.aspectMask, VK_IMAGE_ASPECT_COLOR_BIT|VK_IMAGE_ASPECT_DEPTH_BIT);
    CORRADE_COMPARE(barrier->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(barrier->subresourceRange.levelCount, 9);
    CORRADE_COMPARE(barrier->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(barrier->subresourceRange.layerCount, 5);
}

void PipelineTest::imageMemoryBarrierConstructImplicitAspect() {
    Device device{NoCreate};
    Image image = Image::wrap(device, reinterpret_cast<VkImage>(0xdead), PixelFormat::Depth24UnormStencil8UI);

    ImageMemoryBarrier barrier{
        Access::ColorAttachmentRead, Access::TransferWrite,
        ImageLayout::ColorAttachment, ImageLayout::TransferDestination,
        image, 3, 5, 7, 9};
    CORRADE_COMPARE(barrier->srcAccessMask, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT);
    CORRADE_COMPARE(barrier->dstAccessMask, VK_ACCESS_TRANSFER_WRITE_BIT);
    CORRADE_COMPARE(barrier->oldLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    CORRADE_COMPARE(barrier->newLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CORRADE_COMPARE(barrier->image, reinterpret_cast<VkImage>(0xdead));
    CORRADE_COMPARE(barrier->subresourceRange.aspectMask, VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT);
    CORRADE_COMPARE(barrier->subresourceRange.baseMipLevel, 7);
    CORRADE_COMPARE(barrier->subresourceRange.levelCount, 9);
    CORRADE_COMPARE(barrier->subresourceRange.baseArrayLayer, 3);
    CORRADE_COMPARE(barrier->subresourceRange.layerCount, 5);
}

void PipelineTest::imageMemoryBarrierConstructNoInit() {
    ImageMemoryBarrier barrier{NoInit};
    barrier->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    new(&barrier) ImageMemoryBarrier{NoInit};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);

    CORRADE_VERIFY(std::is_nothrow_constructible<ImageMemoryBarrier, NoInitT>::value);

    /* Implicit construction is not allowed */
    CORRADE_VERIFY(!std::is_convertible<NoInitT, ImageMemoryBarrier>::value);
}

void PipelineTest::imageMemoryBarrierConstructFromVk() {
    VkImageMemoryBarrier vkBarrier;
    vkBarrier.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    ImageMemoryBarrier barrier{vkBarrier};
    CORRADE_COMPARE(barrier->sType, VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2);
}

void PipelineTest::debugBindPoint() {
    std::ostringstream out;
    Debug{&out} << PipelineBindPoint::Compute << PipelineBindPoint(-10007655);
    CORRADE_COMPARE(out.str(), "Vk::PipelineBindPoint::Compute Vk::PipelineBindPoint(-10007655)\n");
}

void PipelineTest::debugDynamicRasterizationState() {
    std::ostringstream out;
    Debug{&out} << DynamicRasterizationState::VertexInputBindingStride << DynamicRasterizationState(0xab);
    CORRADE_COMPARE(out.str(), "Vk::DynamicRasterizationState::VertexInputBindingStride Vk::DynamicRasterizationState(0xab)\n");
}

void PipelineTest::debugDynamicRasterizationStates() {
    std::ostringstream out;
    Debug{&out} << (DynamicRasterizationState::Viewport|DynamicRasterizationState::Scissor|DynamicRasterizationState(0x2a)|DynamicRasterizationState(0x3f)) << DynamicRasterizationStates{};
    CORRADE_COMPARE(out.str(), "Vk::DynamicRasterizationState::Viewport|Vk::DynamicRasterizationState::Scissor|Vk::DynamicRasterizationState(0x2a)|Vk::DynamicRasterizationState(0x3f) Vk::DynamicRasterizationStates{}\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::PipelineTest)
