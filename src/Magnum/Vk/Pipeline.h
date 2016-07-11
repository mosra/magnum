#ifndef Magnum_Vk_GraphicsPipeline_h
#define Magnum_Vk_GraphicsPipeline_h
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

/** @file
 * @brief Class @ref Magnum::Vk::GraphicsPipeline
 */

#include "Magnum/Magnum.h"
#include "Magnum/Vk/visibility.h"

#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/RenderPass.h"
#include "Magnum/Vk/Shader.h"
#include "Magnum/Vk/DescriptorSet.h"

#include "Magnum/Math/Vector3.h" // TEMPORARY!!!

#include <Corrade/Containers/Array.h>

#include "vulkan.h"

namespace Magnum { namespace Vk {

class DescriptorSetLayout;

enum class DynamicState: UnsignedInt {
    Viewport = VK_DYNAMIC_STATE_VIEWPORT,
    Scissor = VK_DYNAMIC_STATE_SCISSOR,
    LineWidth = VK_DYNAMIC_STATE_LINE_WIDTH,
    DepthBias = VK_DYNAMIC_STATE_DEPTH_BIAS,
    BlendConstant = VK_DYNAMIC_STATE_BLEND_CONSTANTS,
    DepthBounds = VK_DYNAMIC_STATE_DEPTH_BOUNDS,
    CompareMask = VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
    WriteMask = VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
    StencilReference = VK_DYNAMIC_STATE_STENCIL_REFERENCE,
};

enum class Topology: UnsignedInt {
    PointList = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    LineList = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    LineStrip = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    TriangleList = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    TriangleStrip = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    TriangleFan = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
    LineListWithAdjacency = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
    LineStripWithAdjacency = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
    TriangleListWithAdjacency = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
    TriangleStripWithAdjacency = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
    PatchList = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
};

enum class PipelineStage: UnsignedInt {
    TopOfThePipe = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    DrawIndirect = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
    VertexInput = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
    VertexShader = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
    TesslationControlShader = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,
    TesslationEvaluationShader = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,
    GeometryShader = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,
    FragmentShader = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    EarlyFragmentTest = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
    LateFragmentTests = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
    ColorAttachmentOutput = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    ComputeShader = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    StageTransfer = VK_PIPELINE_STAGE_TRANSFER_BIT,
    BottomOfPipe = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    Host = VK_PIPELINE_STAGE_HOST_BIT,
    AllGraphics = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    AllCommands = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
};

typedef Containers::EnumSet<PipelineStage> PipelineStageFlags;

CORRADE_ENUMSET_OPERATORS(PipelineStageFlags)

enum class BindPoint: UnsignedInt {
    Graphics = VK_PIPELINE_BIND_POINT_GRAPHICS,
    Compute = VK_PIPELINE_BIND_POINT_COMPUTE,
};

class MAGNUM_VK_EXPORT Pipeline {
    public:
        Pipeline(Device& device, VkPipeline pipeline, VkPipelineCache cache, VkPipelineLayout layout):
            _device{device},
            _pipeline{pipeline},
            _cache{cache},
            _layout{layout}
        {}

        /** @brief Copying is not allowed */
        Pipeline(const Pipeline&) = delete;

        /** @brief Move constructor */
        Pipeline(Pipeline&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyPipeline}
         */
        ~Pipeline();

        /** @brief Copying is not allowed */
        Pipeline& operator=(const Pipeline&) = delete;

        /** @brief Move assignment is not allowed */
        Pipeline& operator=(Pipeline&&) = delete;

        operator VkPipeline() const {
            return _pipeline;
        }

        VkPipelineLayout layout() {
            return _layout;
        }

        auto bind(BindPoint bindPoint) {
            const VkPipeline pipeline = _pipeline;
            return [bindPoint, pipeline](VkCommandBuffer cmdBuffer){
                vkCmdBindPipeline(cmdBuffer,
                                VkPipelineBindPoint(bindPoint),
                                pipeline);
            };
        }

        auto bindDescriptorSets(BindPoint bindPoint, std::initializer_list<std::reference_wrapper<DescriptorSet>> descriptorSets) {
            const VkPipelineLayout pl = _layout;
            return [bindPoint, &descriptorSets, pl](VkCommandBuffer cmdBuffer){

                std::vector<VkDescriptorSet> vkDescriptorSets{};
                vkDescriptorSets.reserve(descriptorSets.size());

                for (auto& ds : descriptorSets) {
                    vkDescriptorSets.push_back(VkDescriptorSet(ds.get()));
                }

                vkCmdBindDescriptorSets(cmdBuffer,
                                        VkPipelineBindPoint(bindPoint),
                                        pl, 0,
                                        descriptorSets.size(), vkDescriptorSets.data(),
                                        0, nullptr);
            };
        }

    private:
        Device& _device;
        VkPipeline _pipeline;
        VkPipelineCache _cache;
        VkPipelineLayout _layout;
};

class MAGNUM_VK_EXPORT GraphicsPipelineBuilder {
    public:

        GraphicsPipelineBuilder(Device& device):
            _device{device} {

            _inputAssemblyState = {
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                0 /* primitive restart enable */
            };

            _multisampleState = {
                VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, nullptr, 0,
                VK_SAMPLE_COUNT_1_BIT,
                VK_FALSE,
                0.0f,
                nullptr,
                VK_FALSE,
                VK_FALSE
            };

           _depthStencilState = {
                VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_TRUE, /* depth test enable */
                VK_TRUE, /* depth write enable */
                VK_COMPARE_OP_LESS_OR_EQUAL, /* depth compare op */
                VK_FALSE, /* depth bounds test enable */
                VK_FALSE, /* stencil test enable */
                VkStencilOpState{VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_NEVER, 0, 0, 0}, /* front */
                VkStencilOpState{VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_NEVER, 0, 0, 0}, /* back */
                0.0f, 0.0f /* depth bounds */
            };

            _rasterizationState = {
                VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_FALSE, /* depth clamp enable */
                VK_FALSE, /* rasterizer discard enable */
                VK_POLYGON_MODE_FILL, /* poygon mode */
                VK_CULL_MODE_NONE, /* cull mode */
                VK_FRONT_FACE_COUNTER_CLOCKWISE, /* front face */
                VK_FALSE, /* depth bias enable */
                0.0f, 0.0f, 0.0f, 0.0f
           };

            VkPipelineColorBlendAttachmentState blendAttachmentState = {
                VK_FALSE,
                VK_BLEND_FACTOR_ZERO,
                VK_BLEND_FACTOR_ZERO,
                VK_BLEND_OP_ADD,
                VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO,
                VK_BLEND_OP_ADD,
                0xf
            };
            _blendAttachments.push_back(blendAttachmentState);

            _colorBlendState = {
                VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_FALSE,
                VK_LOGIC_OP_CLEAR,
                _blendAttachments.size(),
                _blendAttachments.data(),
                {0.0f, 0.0f, 0.0f, 0.0f}
            };

            _viewportState = {
                VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                nullptr,
                0,
                1,
                nullptr,  // TODO currently expecting dynamic state here...
                1,
                nullptr
            };

            VkVertexInputBindingDescription bindingDesc;
            bindingDesc.binding = 0;
            bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDesc.stride = sizeof(Vector3)*2;

            _vertexInputBindings.push_back(bindingDesc);

            VkVertexInputAttributeDescription attributeDesc[2];
            attributeDesc[0].binding = 0;
            attributeDesc[0].location = 0;
            attributeDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDesc[0].offset = 0;

            _vertexInputAttrbutes.push_back(attributeDesc[0]);

            // Color
            attributeDesc[1].binding = 0;
            attributeDesc[1].location = 1;
            attributeDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDesc[1].offset = sizeof(Vector3);

            _vertexInputAttrbutes.push_back(attributeDesc[1]);

            _vertexInputState = {
                VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                nullptr,
                0,
                _vertexInputBindings.size(),
                _vertexInputBindings.data(),
                _vertexInputAttrbutes.size(),
                _vertexInputAttrbutes.data(),
            };
        }

        /** @brief Copying is not allowed */ // TODO(squareys) probably should be, though
        GraphicsPipelineBuilder(const GraphicsPipelineBuilder&) = delete;

        /** @brief Move constructor */
        GraphicsPipelineBuilder(GraphicsPipelineBuilder&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyPipeline}
         */
        ~GraphicsPipelineBuilder() {
        }

        /** @brief Copying is not allowed */
        GraphicsPipelineBuilder& operator=(const GraphicsPipelineBuilder&) = delete;

        /** @brief Move assignment is not allowed */
        GraphicsPipelineBuilder& operator=(GraphicsPipelineBuilder&&) = delete;

        GraphicsPipelineBuilder& addShader(ShaderStage stage, Shader& shader) {
            VkPipelineShaderStageCreateInfo shaderStage = {
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                nullptr,
                0,
                VkShaderStageFlagBits(stage),
                shader.vkShaderModule(),
                "main",
                nullptr // TODO: SpecializationInfo?
            };
            _shaderStages.push_back(shaderStage);
            return *this;
        }

        std::unique_ptr<Pipeline> build();

        GraphicsPipelineBuilder& setRenderPass(RenderPass& renderPass) {
            _renderPass = &renderPass;
            return *this;
        }

        GraphicsPipelineBuilder& setTopology(Topology topology) {
            _inputAssemblyState.topology = VkPrimitiveTopology(topology);
            return *this;
        }

        GraphicsPipelineBuilder& setDynamicStates(std::initializer_list<DynamicState> states) {
            _dynamicStates = std::vector<DynamicState>(states);
            return *this;
        }

        GraphicsPipelineBuilder& addDescriptorSetLayout(const DescriptorSetLayout& layout) {
            _setLayouts.push_back(layout);
            return *this;
        }

    private:
        Device& _device;
        VkGraphicsPipelineCreateInfo _createInfo;

        VkPipelineInputAssemblyStateCreateInfo _inputAssemblyState;
        VkPipelineVertexInputStateCreateInfo _vertexInputState;
        VkPipelineRasterizationStateCreateInfo _rasterizationState;
        VkPipelineColorBlendStateCreateInfo _colorBlendState;
        VkPipelineMultisampleStateCreateInfo _multisampleState;
        VkPipelineViewportStateCreateInfo _viewportState;
        VkPipelineDepthStencilStateCreateInfo _depthStencilState;

        std::vector<DynamicState> _dynamicStates;
        std::vector<VkPipelineColorBlendAttachmentState> _blendAttachments;
        std::vector<VkDescriptorSetLayout> _setLayouts;
        std::vector<VkVertexInputBindingDescription> _vertexInputBindings;
        std::vector<VkVertexInputAttributeDescription> _vertexInputAttrbutes;
        std::vector<VkPushConstantRange> _pushConstantRanges;

        RenderPass* _renderPass;
        std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
};

}}

#endif
