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
 * @brief Class @ref Magnum::Vk::Pipeline, @ref Magnum::Vk::MemoryBarrier, @ref Magnum::Vk::BufferMemoryBarrier, @ref Magnum::Vk::ImageMemoryBarrier, enum @ref Magnum::Vk::PipelineBindPoint, @ref Magnum::Vk::PipelineStage, @ref Magnum::Vk::Access, @ref Magnum::Vk::DependencyFlag, enum set @ref Magnum::Vk::PipelineStages, @ref Magnum::Vk::Accesses, @ref Magnum::Vk::DependencyFlags
 * @m_since_latest
 */

#include <Corrade/Containers/BigEnumSet.h>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Pipeline bind point
@m_since_latest

Wraps a @type_vk_keyword{PipelineBindPoint}.
@see @ref Pipeline::bindPoint()
@m_enum_values_as_keywords
*/
enum class PipelineBindPoint: Int {
    /**
     * Rasterization pipeline
     * @see @ref RasterizationPipelineCreateInfo
     */
    Rasterization = VK_PIPELINE_BIND_POINT_GRAPHICS,

    /** Ray tracing pipeline */
    RayTracing = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,

    /**
     * Compute pipeline
     * @see @ref ComputePipelineCreateInfo
     */
    Compute = VK_PIPELINE_BIND_POINT_COMPUTE
};

/**
@debugoperatorenum{PipelineBindPoint}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, PipelineBindPoint value);

/**
@brief Pipeline
@m_since_latest

Wraps a @type_vk_keyword{Pipeline}.

@section Vk-Pipeline-creation-rasterization Rasterization pipeline creation

A @ref RasterizationPipelineCreateInfo is constructed from a @ref ShaderSet,
@ref MeshLayout, @ref PipelineLayout and a @ref RenderPass together with
subpass index and the count of color attachments. Apart from that you need to
set a viewport using @ref RasterizationPipelineCreateInfo::setViewport()
and you end up with a minimal setup needed for color-only rendering:

@snippet MagnumVk.cpp Pipeline-creation-rasterization

Certain aspects of the pipeline can be set as dynamic using
@relativeref{RasterizationPipelineCreateInfo,setDynamicStates()} --- in that
case a subset of the values passed to the constructor will be ignored. See the
particular @ref DynamicRasterizationState values for more information.

@section Vk-Pipeline-creation-compute Compute pipeline creation

Compared to a rasterization pipeline, @ref ComputePipelineCreateInfo only takes
a @ref ShaderSet containing a single @ref ShaderStage::Compute shader and a
@link PipelineLayout @endlink:

@snippet MagnumVk.cpp Pipeline-creation-compute

@section Vk-Pipeline-usage Pipeline usage

A pipeline is bound to a compatible command buffer using
@ref CommandBuffer::bindPipeline(), which replaces any pipeline bound
previously to the same @ref bindPoint():

@snippet MagnumVk.cpp Pipeline-usage
*/
class MAGNUM_VK_EXPORT Pipeline {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device            Vulkan device the pipeline is created on
         * @param bindPoint         Pipeline bind point. Available through
         *      @ref bindPoint() afterwards.
         * @param handle            The @type_vk{Pipeline} handle
         * @param dynamicStates     Dynamic states enabled on the rasterization
         *      pipeline. Available through @ref dynamicRasterizationStates()
         *      afterwards.
         * @param flags             Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * a pipeline layout created using a constructor, the Vulkan pipeline
         * layout is by default not deleted on destruction, use @p flags for
         * different behavior.
         * @see @ref release()
         */
        static Pipeline wrap(Device& device, PipelineBindPoint bindPoint, VkPipeline handle, const DynamicRasterizationStates& dynamicStates, HandleFlags flags = {});
        /** @overload */
        static Pipeline wrap(Device& device, PipelineBindPoint bindPoint, VkPipeline handle, HandleFlags flags = {});

        /**
         * @brief Construct a rasterization pipeline
         * @param device    Vulkan device to create the pipeline on
         * @param info      Rasterization pipeline creation info
         *
         * THe @ref bindPoint() is set to @ref PipelineBindPoint::Rasterization.
         * @see @fn_vk_keyword{CreateGraphicsPipelines}
         */
        explicit Pipeline(Device& device, const RasterizationPipelineCreateInfo& info);

        /**
         * @brief Construct a compute pipeline
         * @param device    Vulkan device to create the pipeline on
         * @param info      Compite pipeline creation info
         *
         * THe @ref bindPoint() is set to @ref PipelineBindPoint::Compute.
         * @see @fn_vk_keyword{CreateComputePipelines}
         */
        explicit Pipeline(Device& device, const ComputePipelineCreateInfo& info);

        /**
         * @brief Construct without creating the pipeline layout
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Pipeline(NoCreateT);

        /** @brief Copying is not allowed */
        Pipeline(const Pipeline&) = delete;

        /** @brief Move constructor */
        Pipeline(Pipeline&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{Pipeline} handle, unless the instance
         * was created using @ref wrap() without
         * @ref HandleFlag::DestroyOnDestruction specified.
         * @see @fn_vk_keyword{DestroyPipeline}, @ref release()
         */
        ~Pipeline();

        /** @brief Copying is not allowed */
        Pipeline& operator=(const Pipeline&) = delete;

        /** @brief Move assignment */
        Pipeline& operator=(Pipeline&& other) noexcept;

        /** @brief Underlying @type_vk{Pipeline} handle */
        VkPipeline handle() { return _handle; }
        /** @overload */
        operator VkPipeline() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Pipeline bind point
         *
         * Either set implicitly based on what constructor was used, or coming
         * from the @ref wrap() call.
         */
        PipelineBindPoint bindPoint() const { return _bindPoint; }

        /**
         * @brief Dynamic rasterization states enabled in this pipeline
         *
         * Contains the states passed to @ref RasterizationPipelineCreateInfo::setDynamicStates()
         * or to the @ref wrap() call. Expects that @ref bindPoint() is
         * @ref PipelineBindPoint::Rasterization.
         */
        DynamicRasterizationStates dynamicRasterizationStates() const;

        /**
         * @brief Release the underlying Vulkan pipeline
         *
         * Releases ownership of the Vulkan pipeline and returns its
         * handle so @fn_vk{DestroyPipeline} is not called on destruction. The
         * internal state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkPipeline release();

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkPipeline _handle;
        PipelineBindPoint _bindPoint;
        HandleFlags _flags;
        union {
            DynamicRasterizationStates rasterization;
        } _dynamicStates;
};

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

    /**
     * Execution of a fragment shader. Does not include color or depth/stencil
     * attachment writes, those are a separate
     * @ref PipelineStage::ColorAttachmentOutput stage for color and both
     * @ref PipelineStage::EarlyFragmentTests and
     * @ref PipelineStage::LateFragmentTests stages for depth/stencil.
     */
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

    /**
     * Execution of commands related to an acceleration structure
     * @requires_vk_feature @ref DeviceFeature::AccelerationStructure
     */
    AccelerationStructureBuild = VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,

    /**
     * Execution of ray tracing shaders
     * @requires_vk_feature @ref DeviceFeature::RayTracingPipeline
     */
    RayTracingShader = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,

    /**
     * Execution of all rasterization stages. While numerically a single bit,
     * it's equivalent to the logical OR of a supported and enabled subset of
     * the following:
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
     * As the name suggests, this *does not* include
     * @ref PipelineStage::RayTracingShader or
     * @ref PipelineStage::AccelerationStructureBuild.
     * @todo mention mesh / task shaders once exposed
     */
    AllRasterization = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,

    /**
     * All commands.
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      To avoid pipeline stalls and unnecessary synchronization, it's not
     *      advised to use this flag except for debugging synchronization
     *      issues.
     */
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
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      To avoid pipeline stalls and unnecessary synchronization, it's not
     *      advised to use this flag except for debugging synchronization
     *      issues.
     */
    MemoryRead = VK_ACCESS_MEMORY_READ_BIT,

    /**
     * All write accesses. Valid for any @ref PipelineStage, treated as
     * equivalent of a combination of all `*Write` flags valid in given
     * context.
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      To avoid pipeline stalls and unnecessary synchronization, it's not
     *      advised to use this flag except for debugging synchronization
     *      issues.
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
     * @requires_vk_feature @ref DeviceFeature::AccelerationStructure
     */
    AccelerationStructureRead = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR,

    /**
     * Acceleration structure or acceleration structure scratch buffer write as
     * part of a build or copy command.
     *
     * Valid for @ref PipelineStage::AccelerationStructureBuild.
     * @requires_vk_feature @ref DeviceFeature::AccelerationStructure
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

/**
@brief Global memory barrier
@m_since_latest

Wraps a @type_vk_keyword{MemoryBarrier}. This class is subsequently used in
@ref CommandBuffer::pipelineBarrier(), see @ref Vk-Image-usage-copy and
@ref Vk-Buffer-usage-copy for usage examples.
@see @ref BufferMemoryBarrier, @ref ImageMemoryBarrier
*/
class MAGNUM_VK_EXPORT MemoryBarrier {
    public:
        /**
         * @brief Constructor
         * @param sourceAccesses        Source memory access types
         *      participating in a dependency. While allowed, passing `Read`
         *      accesses here is redundant --- that's already taken care of by
         *      the stage execution dependency in
         *      @ref CommandBuffer::pipelineBarrier().
         * @param destinationAccesses   Destination memory access types
         *      participating in a dependency
         *
         * The following @type_vk{MemoryBarrier} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `srcAccessMask` to @p sourceAccesses
         * -    `dstAccessMask` to @p destinationAccesses
         */
        /*implicit*/ MemoryBarrier(Accesses sourceAccesses, Accesses destinationAccesses);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit MemoryBarrier(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit MemoryBarrier(const VkMemoryBarrier& barrier);

        /** @brief Underlying @type_vk{MemoryBarrier} structure */
        VkMemoryBarrier& operator*() { return _barrier; }
        /** @overload */
        const VkMemoryBarrier& operator*() const { return _barrier; }
        /** @overload */
        VkMemoryBarrier* operator->() { return &_barrier; }
        /** @overload */
        const VkMemoryBarrier* operator->() const { return &_barrier; }
        /** @overload */
        operator const VkMemoryBarrier*() const { return &_barrier; }

    private:
        VkMemoryBarrier _barrier;
};

/**
@brief Memory barrier affecting a single buffer
@m_since_latest

Wraps a @type_vk_keyword{BufferMemoryBarrier}. Compared to @ref MemoryBarrier
only affects a single buffer. This class is subsequently used in
@ref CommandBuffer::pipelineBarrier(), see @ref Vk-Buffer-usage-copy for usage
examples.
*/
class MAGNUM_VK_EXPORT BufferMemoryBarrier {
    public:
        /**
         * @brief Constructor
         * @param sourceAccesses        Source memory access types
         *      participating in a dependency. While allowed, passing `Read`
         *      accesses here is redundant --- that's already taken care of by
         *      the stage execution dependency in
         *      @ref CommandBuffer::pipelineBarrier().
         * @param destinationAccesses   Destination memory access types
         *      participating in a dependency
         * @param buffer                A @ref Buffer or a raw Vulkan buffer
         *      handle affected by the barrier
         * @param offset                Buffer memory offset affected by the
         *      barrier, in bytes
         * @param size                  Buffer memory size affected by the
         *      barrier, in bytes
         *
         * The following @type_vk{BufferMemoryBarrier} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `srcAccessMask` to @p sourceAccesses
         * -    `dstAccessMask` to @p destinationAccesses
         * -    `buffer`
         * -    `offset`
         * -    `size`
         */
        /*implicit*/ BufferMemoryBarrier(Accesses sourceAccesses, Accesses destinationAccesses, VkBuffer buffer, UnsignedLong offset = 0, UnsignedLong size = VK_WHOLE_SIZE);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit BufferMemoryBarrier(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit BufferMemoryBarrier(const VkBufferMemoryBarrier& barrier);

        /** @brief Underlying @type_vk{BufferMemoryBarrier} structure */
        VkBufferMemoryBarrier& operator*() { return _barrier; }
        /** @overload */
        const VkBufferMemoryBarrier& operator*() const { return _barrier; }
        /** @overload */
        VkBufferMemoryBarrier* operator->() { return &_barrier; }
        /** @overload */
        const VkBufferMemoryBarrier* operator->() const { return &_barrier; }
        /** @overload */
        operator const VkBufferMemoryBarrier*() const { return &_barrier; }

    private:
        VkBufferMemoryBarrier _barrier;
};

/**
@brief Memory barrier affecting a single image
@m_since_latest

Wraps a @type_vk_keyword{ImageMemoryBarrier}. Compared to @ref MemoryBarrier
only affects a single image and additionally performs @ref ImageLayout
transitions. This class is subsequently used in
@ref CommandBuffer::pipelineBarrier(), see @ref Vk-Image-usage-copy for usage
examples.
*/
class MAGNUM_VK_EXPORT ImageMemoryBarrier {
    public:
        /**
         * @brief Constructor
         * @param sourceAccesses        Source memory access types
         *      participating in a dependency. While allowed, passing `Read`
         *      accesses here is redundant --- that's already taken care of by
         *      the stage execution dependency in
         *      @ref CommandBuffer::pipelineBarrier().
         * @param oldLayout             Old layout in an image layout
         *      transition
         * @param destinationAccesses   Destination memory access types
         *      participating in a dependency
         * @param newLayout             New layout in an image layout
         *      transition
         * @param image                 An @ref Image or a raw Vulkan image
         *      handle affected by the barrier
         * @param aspects               Image aspects affected by the barrier
         * @param layerOffset           Offset to the first layer affected by
         *      the barrier
         * @param layerCount            Layer count affected by the barrier
         * @param levelOffset           Offset to the first mip level affected
         *      by the barrier
         * @param levelCount            Mip level count affected by the barrier
         *
         * The following @type_vk{ImageMemoryBarrier} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `srcAccessMask` to @p sourceAccesses
         * -    `dstAccessMask` to @p destinationAccesses
         * -    `oldLayout`
         * -    `newLayout`
         * -    `image`
         * -    `subresourceRange.aspectMask` to @p aspects
         * -    `subresourceRange.levelOffset` to @p levelOffset
         * -    `subresourceRange.levelCount` to @p levelCount
         * -    `subresourceRange.baseArrayLayer` to @p layerOffset
         * -    `subresourceRange.layerCount` to @p layerCount
         */
        /*implicit*/ ImageMemoryBarrier(Accesses sourceAccesses, Accesses destinationAccesses, ImageLayout oldLayout, ImageLayout newLayout, VkImage image, ImageAspects aspects, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS);

        /**
         * @brief Construct with an implicit image aspect
         *
         * Compared to @ref ImageMemoryBarrier(Accesses, Accesses, ImageLayout, ImageLayout, VkImage, ImageAspects, UnsignedInt, UnsignedInt, UnsignedInt, UnsignedInt)
         * the @ref ImageAspects are chosen implicitly using
         * @ref imageAspectsFor() from @ref Image::format().
         */
        /*implicit*/ ImageMemoryBarrier(Accesses sourceAccesses, Accesses destinationAccesses, ImageLayout oldLayout, ImageLayout newLayout, Image& image, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit ImageMemoryBarrier(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit ImageMemoryBarrier(const VkImageMemoryBarrier& barrier);

        /** @brief Underlying @type_vk{ImageMemoryBarrier} structure */
        VkImageMemoryBarrier& operator*() { return _barrier; }
        /** @overload */
        const VkImageMemoryBarrier& operator*() const { return _barrier; }
        /** @overload */
        VkImageMemoryBarrier* operator->() { return &_barrier; }
        /** @overload */
        const VkImageMemoryBarrier* operator->() const { return &_barrier; }
        /** @overload */
        operator const VkImageMemoryBarrier*() const { return &_barrier; }

    private:
        VkImageMemoryBarrier _barrier;
};

}}

#endif
