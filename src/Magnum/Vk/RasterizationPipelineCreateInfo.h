#ifndef Magnum_Vk_RasterizationPipelineCreateInfo_h
#define Magnum_Vk_RasterizationPipelineCreateInfo_h
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

/** @file
 * @brief Class @ref Magnum::Vk::RasterizationPipelineCreateInfo, enum @ref Magnum::Vk::DynamicRasterizationState, enum set @ref Magnum::Vk::DynamicRasterizationStates
 * @m_since_latest
 */

#include <Corrade/Containers/BigEnumSet.h>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Dynamic rasterization state
@m_since_latest

Contains rasterization-related information from the
@type_vk_keyword{DynamicState} enum but with contiguos numbering to make it
usable as a set of @ref DynamicRasterizationStates.
@see @ref RasterizationPipelineCreateInfo::setDynamicStates()
*/
enum class DynamicRasterizationState: UnsignedByte {
    /**
     * Viewport range set in
     * @ref RasterizationPipelineCreateInfo::setViewport() is ignored and is
     * expected to be set dynamically using @fn_vk{CmdSetViewport}. Viewport
     * count is still set in @ref RasterizationPipelineCreateInfo, see
     * @ref DynamicRasterizationState::ViewportWithCount for having both
     * dynamic.
     * @m_keywords{VK_DYNAMIC_STATE_VIEWPORT}
     * @todoc link the multi-view API instead when exposed
     */
    Viewport,

    /**
     * Scissor rectangle set in
     * @ref RasterizationPipelineCreateInfo::setViewport() is ignored and is
     * expected to be set dynamically using @fn_vk{CmdSetScissor}. Scissor
     * count is still set in @ref RasterizationPipelineCreateInfo, see
     * @ref DynamicRasterizationState::ScissorWithCount for having both
     * dynamic.
     * @m_keywords{VK_DYNAMIC_STATE_SCISSOR}
     * @todoc link the multi-view API instead when exposed
     */
    Scissor,

    /**
     * Line width set in @ref RasterizationPipelineCreateInfo is ignored and is
     * expected to be set dynamically using @fn_vk{CmdSetLineWidth}.
     * @requires_vk_feature @ref DeviceFeature::WideLines
     * @m_keywords{VK_DYNAMIC_STATE_LINE_WIDTH}
     * @todoc link to the actual API when exposed
     */
    LineWidth,

    /**
     * Depth bias constant factor, depth bias clamp and depth bias slope factor
     * set in @ref RasterizationPipelineCreateInfo are ignored and expected to
     * be set dynamically using @fn_vk{CmdSetDepthBias}.
     * @m_keywords{VK_DYNAMIC_STATE_DEPTH_BIAS}
     * @todoc link to the actual API when exposed
     */
    DepthBias,

    /**
     * Blend constants set in @ref RasterizationPipelineCreateInfo are ignored
     * and expected to be set dynamically using @fn_vk{CmdSetBlendConstants}.
     * @m_keywords{VK_DYNAMIC_STATE_BLEND_CONSTANTS}
     * @todoc link to the actual API when exposed
     */
    BlendConstants,

    /**
     * Min and max depth bounds set in @ref RasterizationPipelineCreateInfo are
     * ignored and expected to be set dynamically using
     * @fn_vk{CmdSetDepthBounds}.
     * @see @ref DynamicRasterizationState::DepthBoundsTestEnable
     * @requires_vk_feature @ref DeviceFeature::DepthBounds
     * @m_keywords{VK_DYNAMIC_STATE_DEPTH_BOUNDS}
     * @todoc link to the actual API when exposed
     */
    DepthBounds,

    /**
     * Stencil compare mask set in @ref RasterizationPipelineCreateInfo is
     * ignored and expected to be set dynamically using
     * @fn_vk{CmdSetStencilCompareMask}.
     * @m_keywords{VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK}
     * @todoc link to the actual API when exposed
     */
    StencilCompareMask,

    /**
     * Stencil write mask set in @ref RasterizationPipelineCreateInfo is
     * ignored and expected to be set dynamically using
     * @fn_vk{CmdSetStencilWriteMask}.
     * @m_keywords{VK_DYNAMIC_STATE_STENCIL_WRITE_MASK}
     * @todoc link to the actual API when exposed
     */
    StencilWriteMask,

    /**
     * Stencil reference set in @ref RasterizationPipelineCreateInfo is ignored
     * and expected to be set dynamically using @fn_vk{CmdSetStencilReference}.
     * @m_keywords{VK_DYNAMIC_STATE_STENCIL_REFERENCE}
     * @todoc link to the actual API when exposed
     */
    StencilReference,

    /**
     * Cull mode set in @ref RasterizationPipelineCreateInfo is ignored and
     * expected to be set dynamically using @fn_vk{CmdSetCullModeEXT}.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_CULL_MODE_EXT}
     * @todoc link to the actual API when exposed
     */
    CullMode,

    /**
     * Front face set in @ref RasterizationPipelineCreateInfo is ignored and
     * expected to be set dynamically using @fn_vk{CmdSetFrontFaceEXT}.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_FRONT_FACE_EXT}
     * @todoc link to the actual API when exposed
     */
    FrontFace,

    /**
     * Only the @ref MeshPrimitive topology class set in @ref MeshLayout and
     * passed to @ref RasterizationPipelineCreateInfo is used and the specific
     * topology order and adjacency is expected to be set dynamically.
     * @ref CommandBuffer::draw() does this automatically if a pipeline with
     * this dynamic state is bound.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT}
     */
    MeshPrimitive,

    /**
     * Both the number of viewports and their ranges set in
     * @ref RasterizationPipelineCreateInfo::setViewport() are ignored and
     * expected to be set dynamically using @fn_vk{CmdSetViewportWithCountEXT}.
     * A superset of @ref DynamicRasterizationState::Viewport.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT_EXT}
     * @todoc link the multi-view API instead when exposed
     */
    ViewportWithCount,

    /**
     * Both the number of scissors and their rectangles set in
     * @ref RasterizationPipelineCreateInfo::setViewport() are ignored and
     * expected to be set dynamically using @fn_vk{CmdSetScissorWithCountEXT}.
     * A superset of @ref DynamicRasterizationState::Scissor.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT_EXT}
     * @todoc link the multi-view API instead when exposed
     */
    ScissorWithCount,

    /**
     * Stride set in @ref MeshLayout::addBinding() and passed to
     * @ref RasterizationPipelineCreateInfo is ignored and expected to be set
     * dynamically. @ref CommandBuffer::draw() does this automatically if a
     * pipeline with this dynamic state is bound.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT}
     */
    VertexInputBindingStride,

    /**
     * Depth test enablement in @ref RasterizationPipelineCreateInfo is ignored
     * and expected to be set dynamically using @fn_vk{CmdSetDepthTestEnableEXT}.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE_EXT}
     * @todoc link to the actual API when exposed
     */
    DepthTestEnable,

    /**
     * Depth write enablement in @ref RasterizationPipelineCreateInfo is
     * ignored and expected to be set dynamically using
     * @fn_vk{CmdSetDepthWriteEnableEXT}.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE_EXT}
     * @todoc link to the actual API when exposed
     */
    DepthWriteEnable,

    /**
     * Depth compare operation in @ref RasterizationPipelineCreateInfo is
     * ignored and expected to be set dynamically using
     * @fn_vk{CmdSetDepthCompareOpEXT}.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_DEPTH_COMPARE_OP_EXT}
     * @todoc link to the actual API when exposed
     */
    DepthCompareOperation,

    /**
     * Depth bounds test enablement in @ref RasterizationPipelineCreateInfo is
     * ignored and expected to be set dynamically using
     * @fn_vk{CmdSetDepthBoundsTestEnableEXT}.
     * @requires_vk_feature @ref DeviceFeature::DepthBounds and
     *      @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE_EXT}
     * @todoc link to the actual API when exposed
     */
    DepthBoundsTestEnable,

    /**
     * Stencil test enablement in @ref RasterizationPipelineCreateInfo is
     * ignored and expected to be set dynamically using
     * @fn_vk{CmdSetStencilTestEnableEXT}.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE_EXT}
     * @todoc link to the actual API when exposed
     */
    StencilTestEnable,

    /**
     * Stencil operation in @ref RasterizationPipelineCreateInfo is ignored and
     * expected to be set dynamically using @fn_vk{CmdSetStencilOpEXT}.
     * @requires_vk_feature @ref DeviceFeature::ExtendedDynamicState
     * @m_keywords{VK_DYNAMIC_STATE_STENCIL_OP_EXT}
     * @todoc link to the actual API when exposed
     */
    StencilOperation,

    /** @todo RayTracingPipelineStackSize, or should that go into a dedicated
        enum for RT pipeline? */
};

/**
@brief Dynamic rasterization states
@m_since_latest

A set of rasterization-related information from the
@type_vk_keyword{DynamicState} enum.
@see @ref RasterizationPipelineCreateInfo::setDynamicStates()
*/
typedef Containers::BigEnumSet<DynamicRasterizationState, 1> DynamicRasterizationStates;

CORRADE_ENUMSET_OPERATORS(DynamicRasterizationStates)

/**
@debugoperatorenum{DynamicRasterizationState}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, DynamicRasterizationState value);

/**
@debugoperatorenum{DynamicRasterizationState}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, const DynamicRasterizationStates& value);

/**
@brief Rasterization pipeline creation info
@m_since_latest

@m_keywords{GraphicsPipelineCreateInfo}

Wraps a @type_vk_keyword{GraphicsPipelineCreateInfo}, along with

-   @type_vk_keyword{PipelineViewportStateCreateInfo},
-   @type_vk_keyword{PipelineRasterizationStateCreateInfo},
-   @type_vk_keyword{PipelineMultisampleStateCreateInfo},
-   @type_vk_keyword{PipelineDepthStencilStateCreateInfo},
-   @type_vk_keyword{PipelineColorBlendStateCreateInfo} containing
    @type_vk_keyword{PipelineColorBlendAttachmentState} and
-   @type_vk_keyword{PipelineDynamicStateCreateInfo}.

See @ref Vk-Pipeline-creation-rasterization "Rasterization pipeline creation"
for usage information.
@see @ref ComputePipelineCreateInfo
*/
class MAGNUM_VK_EXPORT RasterizationPipelineCreateInfo {
    public:
        /**
         * @brief Rasterization pipeline creation flag
         *
         * Wraps the rasterization-related subset of
         * @type_vk_keyword{PipelineCreateFlagBits}.
         * @see @ref Flags, @ref RasterizationPipelineCreateInfo(const ShaderSet&, const MeshLayout&, VkPipelineLayout, VkRenderPass, UnsignedInt, UnsignedInt, Flags)
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /**
             * Create the pipeline without optimization.
             *
             * @m_class{m-note m-success}
             *
             * @par
             *      Setting this flag on single-use pipelines might help
             *      drivers pick a better tradeoff between CPU time spent
             *      optimizing the pipeline and GPU time spent executing it.
             */
            DisableOptimization = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT,

            /**
             * Allow derivatives to be subsequently created from this pipeline.
             */
            AllowDerivatives = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT,

            /** Derivative of a pipeline created earlier. */
            Derivative = VK_PIPELINE_CREATE_DERIVATIVE_BIT
        };

        /**
         * @brief Rasterization pipeline creation flags
         *
         * Type-safe wrapper for the rasterization-related subset of
         * @type_vk_keyword{PipelineCreateFlags}.
         * @see @ref RasterizationPipelineCreateInfo(const ShaderSet&, const MeshLayout&, VkPipelineLayout, VkRenderPass, UnsignedInt, UnsignedInt, Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param shaderSet         Set of shaders to use for this pipeline
         * @param meshLayout        Mesh layout to use in this pipeline
         * @param pipelineLayout    A @ref PipelineLayout or a raw Vulkan
         *      pipeline layout handle
         * @param renderPass        A @ref RenderPass or a raw Vulkan render
         *      pass handle
         * @param subpass           Subpass index
         * @param subpassColorAttachmentCount Number of color attachments the
         *      @p subpass uses
         * @param flags             Rasterization pipeline creation flags
         *
         * Note that the @p shaderSet and @p meshLayout structure internals are
         * referenced, not copied, and thus have to stay in scope until the
         * @ref Pipeline object is created.
         *
         * The following @type_vk{GraphicsPipelineCreateInfo} and substructure
         * fields are pre-filled in addition to `sType` of all referenced
         * structures, everything else is zero-filled:
         *
         * -    `flags`
         * -    `stageCount` and `pStages` to @p shaderSet
         * -    `pVertexInputState` and `pInputAssemblyState` to @p meshLayout
         * -    `pRasterizationState`
         * -    @cpp pRasterizationState->polygonMode @ce to
         *      @val_vk{POLYGON_MODE_FILL,PolygonMode}
         * -    @cpp pRasterizationState->frontFace @ce to
         *      @val_vk{FRONT_FACE_COUNTER_CLOCKWISE,FrontFace}
         * -    @cpp pRasterizationState->lineWidth @ce to @cpp 1.0f @ce
         * -    `pMultisampleState`
         * -    @cpp pMultisampleState->rasterizationSamples @ce to
         *      @val_vk{SAMPLE_COUNT_1_BIT,SampleCountFlagBits}
         * -    `pDepthStencilState`
         * -    `pColorBlendState`
         * -    @cpp pColorBlendState->attachmentCount @ce to
         *      @p subpassColorAttachmentCount
         * -    @cpp pColorBlendState->pAttachments @ce <b></b>
         * -    @cpp pColorBlendState->pAttachments[i].colorWriteMask @ce to
         *      @val_vk{COLOR_COMPONENT_R_BIT,ColorComponentFlagBits},
         *      @val_vk{COLOR_COMPONENT_G_BIT,ColorComponentFlagBits},
         *      @val_vk{COLOR_COMPONENT_B_BIT,ColorComponentFlagBits} and
         *      @val_vk{COLOR_COMPONENT_A_BIT,ColorComponentFlagBits}
         * -    `layout` to @p pipelineLayout
         * -    `renderPass`
         * -    `subpass`
         *
         * You need to call at least @ref setViewport() or specifying
         * @ref DynamicRasterizationState::Viewport in
         * @ref setDynamicStates() for a valid setup.
         */
        explicit RasterizationPipelineCreateInfo(const ShaderSet& shaderSet, const MeshLayout& meshLayout, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, UnsignedInt subpass, UnsignedInt subpassColorAttachmentCount, Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field nor the nested structure
         * pointers are set --- the structure has to be fully initialized
         * afterwards in order to be usable.
         */
        explicit RasterizationPipelineCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values *including* the pointed-to
         *
         * -    @type_vk{PipelineViewportStateCreateInfo},
         * -    @type_vk{PipelineRasterizationStateCreateInfo},
         * -    @type_vk{PipelineMultisampleStateCreateInfo},
         * -    @type_vk{PipelineDepthStencilStateCreateInfo},
         * -    @type_vk{PipelineColorBlendStateCreateInfo} and
         * -    @type_vk{PipelineDynamicStateCreateInfo}
         *
         * @m_class{m-noindent}
         *
         * structures verbatim, remaining pointers are kept unchanged without
         * taking over the ownership. Modifying the newly created instance will
         * not modify the original data nor the pointed-to data.
         */
        explicit RasterizationPipelineCreateInfo(const VkGraphicsPipelineCreateInfo& info);

        /** @brief Copying is not allowed */
        RasterizationPipelineCreateInfo(const RasterizationPipelineCreateInfo&) = delete;

        /** @brief Move constructor */
        RasterizationPipelineCreateInfo(RasterizationPipelineCreateInfo&& other) noexcept;

        ~RasterizationPipelineCreateInfo();

        /** @brief Copying is not allowed */
        RasterizationPipelineCreateInfo& operator=(const RasterizationPipelineCreateInfo&) = delete;

        /** @brief Move assignment */
        RasterizationPipelineCreateInfo& operator=(RasterizationPipelineCreateInfo&& other) noexcept;

        /**
         * @brief Set viewport and scissor rectangle
         * @param viewport      Viewport rectangle and depth range
         * @param scissor       Scissor rectangle
         * @return Reference to self (for method chaining)
         *
         * While the @p scissor rectangle is full pixels, the @p viewport XY
         * range can have sub-pixel precision. The depth should be in range
         * @f$ [0.0, 1.0] @f$.
         *
         * The following @type_vk{GraphicsPipelineCreateInfo} and substructure
         * fields are modified, in addition to `sType` of newly referenced
         * structures:
         *
         * -    `pViewportState`
         * -    @cpp pViewportState->viewportCount @ce to @cpp 1 @ce
         * -    @cpp pViewportState->pViewports[0] @ce to @p viewport
         * -    @cpp pViewportState->scissorCount @ce to @cpp 1 @ce
         * -    @cpp pViewportState->pScissors[0] @ce to @p scissor
         *
         * @see @ref setViewport(const Range3D&)
         *
         * @requires_vk11 With the @vk_extension{KHR,maintenance1} extension
         *      it's possible to switch the origin from top left and Y down to
         *      bottom left and Y up (matching OpenGL) by supplying negative
         *      height and an adjusted Y offset
         */
        RasterizationPipelineCreateInfo& setViewport(const Range3D& viewport, const Range2Di& scissor);

        /**
         * @brief Set viewport with an implicit scissor rectangle
         *
         * Equivalent to calling @ref setViewport(const Range3D&, const Range2Di&)
         * with @p scissor set to the @ref Range3D::xy() part of @p viewport.
         */
        RasterizationPipelineCreateInfo& setViewport(const Range3D& viewport);

        /**
         * @brief Set viewport with an implicit depth range
         *
         * Equivalent to calling @ref setViewport(const Range3D&, const Range2Di&)
         * with depth from @cpp 0.0f @ce to @cpp 1.0f @ce.
         */
        RasterizationPipelineCreateInfo& setViewport(const Range2D& viewport, const Range2Di& scissor);

        /**
         * @brief Set viewport with an implicit depth range and scissor rectangle
         *
         * Equivalent to calling @ref setViewport(const Range3D&, const Range2Di&)
         * with depth from @cpp 0.0f @ce to @cpp 1.0f @ce and @p scissor same
         * as @p viewport.
         */
        RasterizationPipelineCreateInfo& setViewport(const Range2D& viewport);

        /**
         * @brief Set dynamic states
         *
         * The following @type_vk{GraphicsPipelineCreateInfo} and substructure
         * fields are modified, in addition to `sType` of newly referenced
         * structures:
         *
         * -    `pDynamicState`
         * -    @cpp pDynamicState->dynamicStateCount @ce to count of values
         *      enabled in @p states
         * -    @cpp pDynamicState->pDynamicStates @ce to a list of
         *      @type_vk{DynamicState} corresponding to
         *      @ref DynamicRasterizationState values enabled in @p states
         */
        RasterizationPipelineCreateInfo& setDynamicStates(const DynamicRasterizationStates& states);

        /** @brief Underlying @type_vk{GraphicsPipelineCreateInfo} structure */
        VkGraphicsPipelineCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkGraphicsPipelineCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkGraphicsPipelineCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkGraphicsPipelineCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkGraphicsPipelineCreateInfo*() const { return &_info; }

    private:
        friend Pipeline;

        VkGraphicsPipelineCreateInfo _info;
        VkPipelineViewportStateCreateInfo _viewportInfo;
        VkPipelineRasterizationStateCreateInfo _rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo _multisampleInfo;
        VkPipelineDepthStencilStateCreateInfo _depthStencilInfo;
        VkPipelineColorBlendStateCreateInfo _colorBlendInfo;
        VkPipelineDynamicStateCreateInfo _dynamicInfo;

        struct State;
        Containers::Pointer<State> _state;
};

CORRADE_ENUMSET_OPERATORS(RasterizationPipelineCreateInfo::Flags)

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/Pipeline.h"

#endif
