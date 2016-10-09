/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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

#include "Pipeline.h"


namespace Magnum { namespace Vk {

Pipeline::~Pipeline() {
    vkDestroyPipeline(_device, _pipeline, nullptr);
    vkDestroyPipelineLayout(_device, _layout, nullptr);
    vkDestroyPipelineCache(_device, _cache, nullptr);
}

std::unique_ptr<Pipeline> GraphicsPipelineBuilder::build() {
    VkPipeline pipeline;
    VkPipelineCache cache;
    VkPipelineLayout layout;

    /* create pipeline layout */
    VkPipelineLayoutCreateInfo plInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        UnsignedInt(_setLayouts.size()),
        _setLayouts.data(),
        UnsignedInt(_pushConstantRanges.size()),
        _pushConstantRanges.data()
    };

    VkResult err = vkCreatePipelineLayout(_device, &plInfo, nullptr, &layout);
    MAGNUM_VK_ASSERT_ERROR(err);

    /* create pipeline cache */
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        nullptr,
        0,
        0, /* initial data size */
        nullptr /* initial data */
    };


    err = vkCreatePipelineCache(_device, &pipelineCacheCreateInfo, nullptr, &cache);
    MAGNUM_VK_ASSERT_ERROR(err);

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        nullptr,
        0,
        VkPrimitiveTopology(_primitiveTopology),
        0 /* primitive restart enable */
    };

    VkPipelineVertexInputStateCreateInfo vertexInputState = {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        nullptr,
        0,
        UnsignedInt(_vertexInputBindings.size()),
        _vertexInputBindings.data(),
        UnsignedInt(_vertexInputAttrbutes.size()),
        _vertexInputAttrbutes.data(),
    };

    /* create pipeline itself */
    VkPipelineDynamicStateCreateInfo dynamicState = {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        nullptr,
        0,
        UnsignedInt(_dynamicStates.size()),
        reinterpret_cast<VkDynamicState*>(_dynamicStates.data())
    };

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.layout = layout;
    pipelineInfo.renderPass = *_renderPass;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pRasterizationState = &_rasterizationState;
    pipelineInfo.pColorBlendState = &_colorBlendState;
    pipelineInfo.pMultisampleState = &_multisampleState;
    pipelineInfo.pViewportState = &_viewportState;
    pipelineInfo.pDepthStencilState = &_depthStencilState;
    pipelineInfo.stageCount = UnsignedInt(_shaderStages.size());
    pipelineInfo.pStages = _shaderStages.data();
    pipelineInfo.pDynamicState = &dynamicState;

    err = vkCreateGraphicsPipelines(_device, cache, 1, &pipelineInfo, nullptr, &pipeline);
    MAGNUM_VK_ASSERT_ERROR(err);

    return std::unique_ptr<Pipeline>{new Pipeline{_device, pipeline, cache, layout}};
}

}}
