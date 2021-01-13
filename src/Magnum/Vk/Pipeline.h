#ifndef Magnum_Vk_Pipeline_h
#define Magnum_Vk_Pipeline_h
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
 * @brief Enum @ref Magnum::Vk::PipelineStage, @ref Magnum::Vk::Access, @ref Magnum::Vk::DependencyFlag, enum set @ref Magnum::Vk::PipelineStages, @ref Magnum::Vk::Accesses, @ref Magnum::Vk::DependencyFlags
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Pipeline stage
@m_since_latest

Wraps @type_vk_keyword{PipelineStageFlagBits}.
@see @ref PipelineStages, @ref SubpassDependency
@m_enum_values_as_keywords
*/
enum class PipelineStage: UnsignedInt {
    /**
     * Top of pipe. Equivalent to @ref PipelineStage::AllCommands with
     * empty @ref Accesses when specified in the second synchronization scope,
     * but specifies no stages in the first scope.
     */
    TopOfPipe = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,

    /** Where indirect draw/dispatch/raytrace data structures are consumed */
    DrawIndirect = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,

    /** Where vertex and index buffers are consumed */
    VertexInput = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,

    /** Execution of a vertex shader */
    VertexShader = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,

    /**
     * Execution of a tessellation control shader.
     *
     * @requires_vk_feature @ref DeviceFeature::TessellationShader
     */
    TessellationControlShader = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,

    /**
     * Execution of a tessellation evaluation shader.
     *
     * @requires_vk_feature @ref DeviceFeature::TessellationShader
     */
    TessellationEvaluationShader = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,

    /**
     * Execution of a geometry shader
     *
     * @requires_vk_feature @ref DeviceFeature::GeometryShader
     */
    GeometryShader = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,

    /** Execution of a fragment shader */
    FragmentShader = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,

    /**
     * Where early fragment tests (depth and stencil tests before fragment
     * shader execution) are performed. Includes also subpass load operations
     * for framebuffer attachments with a depth/stencil format.
     */
    EarlyFragmentTests = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,

    /**
     * Where late fragment tests (depth and stencil tests after fragment shader
     * execution) are performed. Includes also subpass store operations for
     * framebuffer attachments with a depth/stencil format.
     */
    LateFragmentTests = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,

    /**
     * Where the final color values are output from the pipeline. Includes also
     * subpass load and store operations and multisample resolve operations for
     * framebuffer attachments with a color or depth/stencil format.
     */
    ColorAttachmentOutput = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,

    /** Execution of a compute shader */
    ComputeShader = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,

    /** Copy, blit, resolve and clear commands */
    Transfer = VK_PIPELINE_STAGE_TRANSFER_BIT,

    /**
     * Equivalent to @ref PipelineStage::AllCommands with empty @ref Accesses
     * when specified in the first synchronization scope, but specifies no
     * stages in the second scope.
     */
    BottomOfPipe = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,

    /** Execution of host read/writes of device memory */
    Host = VK_PIPELINE_STAGE_HOST_BIT,

    /** Execution of commands related to an acceleration structure */
    AccelerationStructureBuild = VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,

    /** Execution of ray tracing shaders */
    RayTracingShader = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,

    /**
     * Execution of all graphics stages. While numerically a single bit, it's
     * equivalent to the logical OR of a supported and enabled subset of the
     * following:
     *
     * -    @ref PipelineStage::DrawIndirect
     * -    @ref PipelineStage::VertexInput
     * -    @ref PipelineStage::VertexShader
     * -    @ref PipelineStage::TessellationControlShader
     * -    @ref PipelineStage::TessellationEvaluationShader
     * -    @ref PipelineStage::GeometryShader
     * -    @ref PipelineStage::FragmentShader
     * -    @ref PipelineStage::EarlyFragmentTests
     * -    @ref PipelineStage::LateFragmentTests
     * -    @ref PipelineStage::ColorAttachmentOutput
     *
     * Note that this *does not* include @ref PipelineStage::RayTracingShader
     * or @ref PipelineStage::AccelerationStructureBuild.
     */
    AllGraphics = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,

    /** All commands */
    AllCommands = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
};

/**
@brief Pipeline stages
@m_since_latest

Type-safe wrapper for @type_vk_keyword{PipelineStageFlags}.
@see @ref SubpassDependency
*/
typedef Containers::EnumSet<PipelineStage> PipelineStages;

CORRADE_ENUMSET_OPERATORS(PipelineStages)

/**
@brief Memory access type participating in a memory dependency
@m_since_latest

Wraps @type_vk_keyword{AccessFlagBits}. Certain access types are performed only
on a subset of pipeline stages and thus can be used only if a corresponding
@ref PipelineStage is present --- see documentation of each value for more
information.
@see @ref Accesses, @ref SubpassDependency
@m_enum_values_as_keywords
*/
enum class Access: UnsignedInt {
    /**
     * Indirect command data read as part of an indirect build, trace, drawing
     * or dispatch command.
     *
     * Valid for @ref PipelineStage::DrawIndirect and
     * @ref PipelineStage::AccelerationStructureBuild "AccelerationStructureBuild".
     */
    IndirectCommandRead = VK_ACCESS_INDIRECT_COMMAND_READ_BIT,

    /**
     * Index buffer read as part of an indexed drawing command.
     *
     * Valid for @ref PipelineStage::VertexInput.
     */
    IndexRead = VK_ACCESS_INDEX_READ_BIT,

    /**
     * Vertex buffer read as part of a drawing command.
     *
     * Valid for @ref PipelineStage::VertexInput.
     */
    VertexAttributeRead = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,

    /**
     * Uniform buffer read.
     *
     * Valid for @ref PipelineStage::VertexShader,
     * @ref PipelineStage::FragmentShader "FragmentShader",
     * @ref PipelineStage::GeometryShader "GeometryShader",
     * @ref PipelineStage::TessellationControlShader "TessellationControlShader",
     * @ref PipelineStage::TessellationEvaluationShader "TessellationEvaluationShader",
     * @ref PipelineStage::ComputeShader "ComputeShader" and
     * @ref PipelineStage::RayTracingShader "RayTracingShader".
     */
    UniformRead = VK_ACCESS_UNIFORM_READ_BIT,

    /**
     * Input atachment read within a render pass during fragment shader
     * execution.
     *
     * Valid for @ref PipelineStage::FragmentShader.
     */
    InputAttachmentRead = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,

    /**
     * Storage buffer, physical storage buffer, shader binding table, uniform
     * texel buffer, storage texel buffer, sampled image, or storage image
     * read.
     *
     * Valid for @ref PipelineStage::AccelerationStructureBuild,
     * @ref PipelineStage::VertexShader "VertexShader",
     * @ref PipelineStage::FragmentShader "FragmentShader",
     * @ref PipelineStage::GeometryShader "GeometryShader",
     * @ref PipelineStage::TessellationControlShader "TessellationControlShader",
     * @ref PipelineStage::TessellationEvaluationShader "TessellationEvaluationShader",
     * @ref PipelineStage::ComputeShader "ComputeShader" and
     * @ref PipelineStage::RayTracingShader "RayTracingShader".
     */
    ShaderRead = VK_ACCESS_SHADER_READ_BIT,

    /**
     * Storage buffer, physical storage buffer, storage texel buffer, or
     * storage image write.
     *
     * Valid for @ref PipelineStage::VertexShader,
     * @ref PipelineStage::FragmentShader "FragmentShader",
     * @ref PipelineStage::GeometryShader "GeometryShader",
     * @ref PipelineStage::TessellationControlShader "TessellationControlShader",
     * @ref PipelineStage::TessellationEvaluationShader "TessellationEvaluationShader",
     * @ref PipelineStage::ComputeShader "ComputeShader" and
     * @ref PipelineStage::RayTracingShader "RayTracingShader".
     */
    ShaderWrite = VK_ACCESS_SHADER_WRITE_BIT,

    /**
     * Color attachment read via blending, logic operations or certain subpass
     * load operations. Doesn't include advanced blend operations.
     *
     * Valid for @ref PipelineStage::ColorAttachmentOutput.
     */
    ColorAttachmentRead = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,

    /**
     * Color, resolve or depth/stencil resolve attachment write during a render
     * pass or via certain subpass load and store operations.
     *
     * Valid for @ref PipelineStage::ColorAttachmentOutput.
     */
    ColorAttachmentWrite = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,

    /**
     * Depth/stencil attachment read via depth or stencil operations or certain
     * subpass load operations.
     *
     * Valid for @ref PipelineStage::EarlyFragmentTests and
     * @ref PipelineStage::LateFragmentTests "LateFragmentTests".
     */
    DepthStencilAttachmentRead = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,

    /**
     * Depth/stencil attachment write via depth or stencil operations or
     * certain subpass load and store operations.
     *
     * Valid for @ref PipelineStage::EarlyFragmentTests and
     * @ref PipelineStage::LateFragmentTests "LateFragmentTests".
     */
    DepthStencilAttachmentWrite = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,

    /**
     * Buffer or image read in a copy operation.
     *
     * Valid for @ref PipelineStage::Transfer and
     * @ref PipelineStage::AccelerationStructureBuild "AccelerationStructureBuild".
     */
    TransferRead = VK_ACCESS_TRANSFER_READ_BIT,

    /**
     * Buffer or image write in a copy or clear operation.
     *
     * Valid for @ref PipelineStage::Transfer and
     * @ref PipelineStage::AccelerationStructureBuild "AccelerationStructureBuild".
     */
    TransferWrite = VK_ACCESS_TRANSFER_WRITE_BIT,

    /**
     * Direct memory read from a host.
     *
     * Valid for @ref PipelineStage::Host.
     */
    HostRead = VK_ACCESS_HOST_READ_BIT,

    /**
     * Direct memory write from a host.
     *
     * Valid for @ref PipelineStage::Host.
     */
    HostWrite = VK_ACCESS_HOST_WRITE_BIT,

    /**
     * All read accesses. Valid for any @ref PipelineStage, treated as
     * equivalent of a combination of all `*Read` flags valid in given context.
     */
    MemoryRead = VK_ACCESS_MEMORY_READ_BIT,

    /**
     * All write accesses. Valid for any @ref PipelineStage, treated as
     * equivalent of a combination of all `*Write` flags valid in given
     * context.
     */
    MemoryWrite = VK_ACCESS_MEMORY_WRITE_BIT,

    /**
     * Acceleration structure read as part of a trace, build, or copy command,
     * or an acceleration structure scratch buffer read as part of a build
     * command.
     *
     * Valid for @ref PipelineStage::AccelerationStructureBuild,
     * @ref PipelineStage::VertexShader "VertexShader",
     * @ref PipelineStage::FragmentShader "FragmentShader",
     * @ref PipelineStage::GeometryShader "GeometryShader",
     * @ref PipelineStage::TessellationControlShader "TessellationControlShader",
     * @ref PipelineStage::TessellationEvaluationShader "TessellationEvaluationShader",
     * @ref PipelineStage::ComputeShader "ComputeShader" and
     * @ref PipelineStage::RayTracingShader "RayTracingShader".
     */
    AccelerationStructureRead = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR,

    /**
     * Acceleration structure or acceleration structure scratch buffer write as
     * part of a build or copy command.
     *
     * Valid for @ref PipelineStage::AccelerationStructureBuild.
     */
    AccelerationStructureWrite = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR
};

/**
@brief Memory access type participating in a memory dependency
@m_since_latest

Type-safe wrapper for @type_vk_keyword{AccessFlags}.
@see @ref SubpassDependency
*/
typedef Containers::EnumSet<Access> Accesses;

CORRADE_ENUMSET_OPERATORS(Accesses)

/**
@brief Execution and memory dependency flag
@m_since_latest

Wraps @type_vk_keyword{DependencyFlagBits}.
@see @ref DependencyFlags, @ref SubpassDependency
@m_enum_values_as_keywords
*/
enum class DependencyFlag: UnsignedInt {
    /**
     * Dependencies will be framebuffer-local. This flag has to be set in
     * case @ref SubpassDependency source and destination stage index is equal.
     */
    ByRegion = VK_DEPENDENCY_BY_REGION_BIT,

    /** @todo DeviceGroup, ViewLocal */
};

/**
@brief Execution and memory dependency flags
@m_since_latest

Type-safe wrapper for @type_vk_keyword{DependencyFlags}.
@see @ref SubpassDependency
*/
typedef Containers::EnumSet<DependencyFlag> DependencyFlags;

CORRADE_ENUMSET_OPERATORS(DependencyFlags)

}}

#endif
