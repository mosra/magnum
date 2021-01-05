#ifndef Magnum_Vk_DeviceFeatures_h
#define Magnum_Vk_DeviceFeatures_h
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

/** @file
 * @brief Enum @ref Magnum::Vk::DeviceFeature, enum set @ref Magnum::Vk::DeviceFeatures
 * @m_since_latest
 */

#include <Corrade/Containers/BigEnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Device feature
@m_since_latest

Contains information stored in the @type_vk{PhysicalDeviceFeatures2} structure
chain. Values correspond to equivalently named structure fields, usually just
with the first letter uppercase instead of lowercase.
@see @ref DeviceFeatures, @ref DeviceProperties::features()

@todo @type_vk{PhysicalDeviceVulkan12Features} additionally has
    `samplerMirrorClampToEdge`, `drawIndirectCount`, `descriptorIndexing`,
    `samplerFilterMinMax`, `shaderOutputViewportIndex`, `shaderOutputLayer`, `subgroupBroadcastDynamicId` that aren't exposed in any extension -- does
    that mean we have to enable such extension even on 1.2 in order to enable
    those? Provide some compatibility, as in show them as supported when
    the exts are present and enable corresponding exts when those get enabled
    on device creation?
*/
enum class DeviceFeature: UnsignedShort {
    /**
     * Whether accesses to buffers are bounds-checked against the range of the
     * buffer descriptor.
     * @todo expose @vk_extension{EXT,robustness2}, reference from here
     */
    RobustBufferAccess,

    /**
     * Whether the full 32-bit range is supported for indexed draw calls when
     * using @val_vk{INDEX_TYPE_UINT32,IndexType}.
     * @see @ref DeviceFeature::IndexTypeUint8
     * @todo expose the `maxDrawIndexedIndexValue` limit
     */
    FullDrawIndexUint32,

    /**
     * Whether image views with @val_vk{IMAGE_VIEW_TYPE_CUBE_ARRAY,ImageViewType}
     * can be created and used in shaders.
     */
    ImageCubeArray,

    /** Whether blend state can be different per attachment. */
    IndependentBlend,

    /** Whether geometry shaders are supported. */
    GeometryShader,

    /** Whether tessellation control and evaluation shaders are supported. */
    TessellationShader,

    /** Whether sample shading and multisample interpolation is supported. */
    SampleRateShading,

    /** Whether blend operations which take two sources are supported. */
    DualSrcBlend,

    /** Whether logic operations are supported. */
    LogicOp,

    /** Whether multiple draw indirect is supported. */
    MultiDrawIndirect,

    /** Whether indirect draw calls support the first instance parameter. */
    DrawIndirectFirstInstance,

    /** Whether depth clamping is supported. */
    DepthClamp,

    /** Whether depth bias clamping is supported. */
    DepthBiasClamp,

    /** Whether point and wireframe fill modes are supported. */
    FillModeNonSolid,

    /** Whether depth bound tests are supported. */
    DepthBounds,

    /** Whether lines with a width other than @cpp 1.0f @ce are supported. */
    WideLines,

    /** Whether points with size greater than @cpp 1.0f @ce are supported. */
    LargePoints,

    /**
     * Whether replacing the alpha of color fragment output from the fragment
     * shader with @cpp 1.0f @ce is supported.
     */
    AlphaToOne,

    /** Whether rendering to more than one viewport is supported. */
    MultiViewport,

    /** Whether anisotropic filtering is supported. */
    SamplerAnisotropy,

    /**
     * Whether @ref PixelFormat::CompressedEacR11Unorm,
     * @ref PixelFormat::CompressedEacR11Snorm,
     * @ref PixelFormat::CompressedEacRG11Unorm,
     * @ref PixelFormat::CompressedEacRG11Snorm,
     * @ref PixelFormat::CompressedEtc2RGB8Unorm,
     * @ref PixelFormat::CompressedEtc2RGB8Srgb,
     * @ref PixelFormat::CompressedEtc2RGB8A1Unorm,
     * @ref PixelFormat::CompressedEtc2RGB8A1Srgb,
     * @ref PixelFormat::CompressedEtc2RGBA8Unorm,
     * @ref PixelFormat::CompressedEtc2RGBA8Unorm ETC2 and EAC compressed
     * texture formats are *all* supported.
     * @see @ref DeviceFeature::TextureCompressionBc,
     *      @ref DeviceFeature::TextureCompressionAstcLdr,
     *      @ref DeviceFeature::TextureCompressionAstcHdr
     */
    TextureCompressionEtc2,

    /**
     * Whether @ref PixelFormat::CompressedAstc4x4RGBAUnorm,
     * @ref PixelFormat::CompressedAstc4x4RGBASrgb,
     * @ref PixelFormat::CompressedAstc5x4RGBAUnorm,
     * @ref PixelFormat::CompressedAstc5x4RGBASrgb,
     * @ref PixelFormat::CompressedAstc5x5RGBAUnorm,
     * @ref PixelFormat::CompressedAstc5x5RGBASrgb,
     * @ref PixelFormat::CompressedAstc6x5RGBAUnorm,
     * @ref PixelFormat::CompressedAstc6x5RGBASrgb,
     * @ref PixelFormat::CompressedAstc6x6RGBAUnorm,
     * @ref PixelFormat::CompressedAstc6x6RGBASrgb,
     * @ref PixelFormat::CompressedAstc8x5RGBAUnorm,
     * @ref PixelFormat::CompressedAstc8x5RGBASrgb,
     * @ref PixelFormat::CompressedAstc8x6RGBAUnorm,
     * @ref PixelFormat::CompressedAstc8x6RGBASrgb,
     * @ref PixelFormat::CompressedAstc8x8RGBAUnorm,
     * @ref PixelFormat::CompressedAstc8x8RGBASrgb,
     * @ref PixelFormat::CompressedAstc10x5RGBAUnorm,
     * @ref PixelFormat::CompressedAstc10x5RGBASrgb,
     * @ref PixelFormat::CompressedAstc10x6RGBAUnorm,
     * @ref PixelFormat::CompressedAstc10x6RGBASrgb,
     * @ref PixelFormat::CompressedAstc10x8RGBAUnorm,
     * @ref PixelFormat::CompressedAstc10x8RGBASrgb,
     * @ref PixelFormat::CompressedAstc10x10RGBAUnorm,
     * @ref PixelFormat::CompressedAstc10x10RGBASrgb,
     * @ref PixelFormat::CompressedAstc12x10RGBAUnorm,
     * @ref PixelFormat::CompressedAstc12x10RGBASrgb,
     * @ref PixelFormat::CompressedAstc12x12RGBAUnorm,
     * @ref PixelFormat::CompressedAstc12x12RGBASrgb,
     * ASTC LDR compressed texture formats are *all* supported.
     * @see @ref DeviceFeature::TextureCompressionAstcHdr,
     *      @ref DeviceFeature::TextureCompressionEtc2,
     *      @ref DeviceFeature::TextureCompressionBc
     */
    TextureCompressionAstcLdr,

    /**
     * Whether @ref PixelFormat::CompressedBc1RGBUnorm,
     * @ref PixelFormat::CompressedBc1RGBSrgb,
     * @ref PixelFormat::CompressedBc1RGBAUnorm,
     * @ref PixelFormat::CompressedBc1RGBASrgb,
     * @ref PixelFormat::CompressedBc2RGBAUnorm,
     * @ref PixelFormat::CompressedBc2RGBASrgb,
     * @ref PixelFormat::CompressedBc3RGBAUnorm,
     * @ref PixelFormat::CompressedBc3RGBASrgb,
     * @ref PixelFormat::CompressedBc4RUnorm,
     * @ref PixelFormat::CompressedBc4RSnorm,
     * @ref PixelFormat::CompressedBc5RGUnorm,
     * @ref PixelFormat::CompressedBc5RGSnorm,
     * @ref PixelFormat::CompressedBc6hRGBUfloat,
     * @ref PixelFormat::CompressedBc6hRGBSfloat,
     * @ref PixelFormat::CompressedBc7RGBAUnorm,
     * @ref PixelFormat::CompressedBc7RGBASrgb BC compressed texture formats
     * are *all* supported.
     * @see @ref DeviceFeature::TextureCompressionEtc2,
     *      @ref DeviceFeature::TextureCompressionAstcLdr,
     *      @ref DeviceFeature::TextureCompressionAstcHdr
     */
    TextureCompressionBc,

    /**
     * Whether occlusion queries returning actual sample counts are supported.
     */
    OcclusionQueryPrecise,

    /** Whether pipeline statistics queries are supported. */
    PipelineStatisticsQuery,

    /**
     * Whether storage buffers and images support stores and atomic operations
     * in the vertex, tessellation and geometry shader stages.
     */
    VertexPipelineStoresAndAtomics,

    /**
     * Whether storage buffers and images support stores and atomic operations
     * in the fragment shader stage.
     */
    FragmentStoresAndAtomics,

    /**
     * Whether point size is accessible in tessellation control, tessellation
     * evaluation and geometry shader stages.
     */
    ShaderTessellationAndGeometryPointSize,

    /**
     * Whether the extended set of image gather instructions is available in
     * shader code.
     */
    ShaderImageGatherExtended,

    /**
     * Whether *all* shader storage image extended formats are supported.
     * @todoc link to the formats once we have our own PixelFormat enum
     */
    ShaderStorageImageExtendedFormats,

    /**
     * Whether multisampled storage images are supported.
     * @see @ref ImageUsage::Storage
     */
    ShaderStorageImageMultisample,

    /**
     * Whether storage images require a format qualifier to be specified when
     * reading from storage images.
     * @see @ref DeviceFeature::ShaderStorageImageWriteWithoutFormat
     */
    ShaderStorageImageReadWithoutFormat,

    /**
     * Whether storage images require a format qualifier to be specified when
     * writing to storage images.
     * @see @ref DeviceFeature::ShaderStorageImageReadWithoutFormat
     */
    ShaderStorageImageWriteWithoutFormat,

    /**
     * Whether arrays of uniform buffers can be indexed by dynamically uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing
     */
    ShaderUniformBufferArrayDynamicIndexing,

    /**
     * Whether arrays of samplers or sampled images can be indexed by
     * dynamically uniform integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing
     */
    ShaderSampledImageArrayDynamicIndexing,

    /**
     * Whether arrays of storage buffers can be indexed by dynamically uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing
     */
    ShaderStorageBufferArrayDynamicIndexing,

    /**
     * Whether arrays of storage images can be indexed by dynamically uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing
     */
    ShaderStorageImageArrayDynamicIndexing,

    /** Whether clip distances are supported in shader code. */
    ShaderClipDistance,

    /** Whether cull distances are supported in shader code. */
    ShaderCullDistance,

    /**
     * Whether 64-bit floats are supported in shader code.
     * @see @ref DeviceFeature::ShaderFloat16
     */
    ShaderFloat64,

    /**
     * Whether 64-bit integers are supported in shader code.
     * @see @ref DeviceFeature::ShaderInt16, @ref DeviceFeature::ShaderInt8,
     *      @ref DeviceFeature::ShaderSubgroupExtendedTypes
     */
    ShaderInt64,

    /**
     * Whether 16-bit integers are supported in shader code.
     * @see @ref DeviceFeature::ShaderInt64, @ref DeviceFeature::ShaderInt8,
     *      @ref DeviceFeature::StorageBuffer16BitAccess,
     *      @ref DeviceFeature::UniformAndStorageBuffer16BitAccess,
     *      @ref DeviceFeature::StoragePushConstant16,
     *      @ref DeviceFeature::StorageInputOutput16,
     *      @ref DeviceFeature::ShaderSubgroupExtendedTypes
     */
    ShaderInt16,

    /**
     * Whether image operations that return resource residency information are
     * supported in shader code.
     */
    ShaderResourceResidency,

    /**
     * Whether image operations specifying minimum resource LOD are supported
     * in shader code.
     */
    ShaderResourceMinLod,

    /**
     * Whether resource memory can be managed at opaque sparse block level
     * instead of at the object level.
     */
    SparseBinding,

    /** Whether the device can access sparse buffers. */
    SparseResidencyBuffer,

    /**
     * Whether the device can access sparse 2D images with 1 sample
     * per pixel.
     * @todo reference VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT once that's exposed
     *      in ImageCreateInfo
     */
    SparseResidencyImage2D,

    /** Whether the device can access sparse 3D images. */
    SparseResidencyImage3D,

    /**
     * Whether the device can access sparse 2D images with 2
     * samples per pixel.
     * @todo reference VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT once that's exposed
     *      in ImageCreateInfo
     */
    SparseResidency2Samples,

    /**
     * Whether the device can access sparse 2D images with 4
     * samples per pixel.
     * @todo reference VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT once that's exposed
     *      in ImageCreateInfo
     */
    SparseResidency4Samples,

    /**
     * Whether the device can access sparse 2D images with 8
     * samples per pixel.
     * @todo reference VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT once that's exposed
     *      in ImageCreateInfo
     */
    SparseResidency8Samples,

    /**
     * Whether the device can access sparse 2D images with 16
     * samples per pixel.
     * @todo reference VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT once that's exposed
     *      in ImageCreateInfo
     */
    SparseResidency16Samples,

    /**
     * Whether the device can correctly access sparse data aliased into
     * multiple locations.
     */
    SparseResidencyAliased,

    /**
     * Whether all pipelines bound to a command buffer during a subpass which
     * uses no attachment must have the same samples.
     * @todoc crossreference this with PipelineCreateInfo
     */
    VariableMultisampleRate,

    /**
     * Whether a secondary command buffer may be executed while a query is
     * active.
     */
    InheritedQueries,

    /* VkPhysicalDeviceProtectedMemoryFeatures, Vk11 */

    /**
     * Whether protected memory is supported.
     * @requires_vk11 Available only on Vulkan 1.1.
     */
    ProtectedMemory,

    /* VkPhysicalDeviceMultiviewFeatures, #54 */

    /**
     * Whether multiview rendering within a render pass is supported.
     * @requires_vk11 Extension @vk_extension{KHR,multiview}
     */
    Multiview,

    /**
     * Whether multiview rendering within a render pass with geometry shaders
     * is supported.
     * @requires_vk11 Extension @vk_extension{KHR,multiview}
     */
    MultiviewGeometryShader,

    /**
     * Whether multiview rendering within a render pass with tessellation
     * shaders is supported.
     * @requires_vk11 Extension @vk_extension{KHR,multiview}
     */
    MultiviewTessellationShader,

    /* VkPhysicalDeviceShaderDrawParametersFeatures, #64 */

    /**
     * Whether shader draw parameters are supported.
     * @requires_vk11 Extension @vk_extension{KHR,shader_draw_parameters}
     */
    ShaderDrawParameters,

    /* VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT, #67 */

    /**
     * Whether @ref PixelFormat::CompressedAstc4x4RGBAF,
     * @ref PixelFormat::CompressedAstc5x4RGBAF,
     * @ref PixelFormat::CompressedAstc5x5RGBAF,
     * @ref PixelFormat::CompressedAstc6x5RGBAF,
     * @ref PixelFormat::CompressedAstc6x6RGBAF,
     * @ref PixelFormat::CompressedAstc8x5RGBAF,
     * @ref PixelFormat::CompressedAstc8x6RGBAF,
     * @ref PixelFormat::CompressedAstc8x8RGBAF,
     * @ref PixelFormat::CompressedAstc10x5RGBAF,
     * @ref PixelFormat::CompressedAstc10x6RGBAF,
     * @ref PixelFormat::CompressedAstc10x8RGBAF,
     * @ref PixelFormat::CompressedAstc10x10RGBAF,
     * @ref PixelFormat::CompressedAstc12x10RGBAF,
     * @ref PixelFormat::CompressedAstc12x12RGBAF ASTC HDR compressed
     * texture formats are *all* supported.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr,
     *      @ref DeviceFeature::TextureCompressionEtc2,
     *      @ref DeviceFeature::TextureCompressionBc
     * @requires_vk_extension Extension @vk_extension{EXT,texture_compression_astc_hdr}
     */
    TextureCompressionAstcHdr,

    /* VkPhysicalDeviceShaderFloat16Int8Features, #83 */

    /**
     * Whether 16-bit floats are supported in shader code.
     * @see @ref DeviceFeature::ShaderFloat64,
     *      @ref DeviceFeature::StorageBuffer16BitAccess,
     *      @ref DeviceFeature::UniformAndStorageBuffer16BitAccess,
     *      @ref DeviceFeature::StoragePushConstant16,
     *      @ref DeviceFeature::StorageInputOutput16,
     *      @ref DeviceFeature::ShaderSubgroupExtendedTypes
     * @requires_vk12 Extension @vk_extension{KHR,shader_float16_int8}
     */
    ShaderFloat16,

    /**
     * Whether 8-bit integers are supported in shader code.
     * @see @ref DeviceFeature::ShaderInt64, @ref DeviceFeature::ShaderInt16,
     *      @ref DeviceFeature::ShaderSubgroupExtendedTypes
     * @requires_vk12 Extension @vk_extension{KHR,shader_float16_int8}
     */
    ShaderInt8,

    /* VkPhysicalDevice16BitStorageFeatures, #84 */

    /**
     * Whether shader storage buffers can have 16-bit integer and 16-bit
     * floating-point members. Subset of
     * @ref DeviceFeature::UniformAndStorageBuffer16BitAccess.
     * @see @ref DeviceFeature::StorageBuffer8BitAccess,
     *      @ref DeviceFeature::ShaderInt16, @ref DeviceFeature::ShaderFloat16
     * @requires_vk11 Extension @vk_extension{KHR,16bit_storage}
     */
    StorageBuffer16BitAccess,

    /**
     * Whether shader uniforms and storage buffers can have 16-bit integer and
     * 16-bit floating-point members. Superset of
     * @ref DeviceFeature::StorageBuffer16BitAccess.
     * @see @ref DeviceFeature::UniformAndStorageBuffer8BitAccess,
     *      @ref DeviceFeature::ShaderInt16, @ref DeviceFeature::ShaderFloat16
     * @requires_vk11 Extension @vk_extension{KHR,16bit_storage}
     */
    UniformAndStorageBuffer16BitAccess,

    /**
     * Whether shader push constants can have 16-bit integer and 16-bit
     * floating-point members.
     * @see @ref DeviceFeature::StoragePushConstant8,
     *      @ref DeviceFeature::ShaderInt16, @ref DeviceFeature::ShaderFloat16
     * @requires_vk11 Extension @vk_extension{KHR,16bit_storage}
     */
    StoragePushConstant16,

    /**
     * Whether shader inputs and outputs can have 16-bit integer and 16-bit
     * floating-point members.
     * @see @ref DeviceFeature::ShaderInt16, @ref DeviceFeature::ShaderFloat16
     * @requires_vk11 Extension @vk_extension{KHR,16bit_storage}
     */
    StorageInputOutput16,

    /* VkPhysicalDeviceImagelessFramebufferFeatures, #109 */

    /**
     * Specifying image view for attachments is possible at render pass begin
     * time.
     * @todoc crosslink with RenderPass::begin() when that's implemented.
     * @requires_vk12 Extension @vk_extension{KHR,imageless_framebuffer}
     */
    ImagelessFramebuffer,

    /* VkPhysicalDeviceVariablePointersFeatures, #121 */

    /**
     * Whether SPIR-V variable pointer storage buffers are supported.
     * @requires_vk11 Extension @vk_extension{KHR,variable_pointers}
     */
    VariablePointersStorageBuffer,

    /**
     * Whether SPIR-V variable pointers are supported.
     * @requires_vk11 Extension @vk_extension{KHR,variable_pointers}
     */
    VariablePointers,

    /* VkPhysicalDeviceAccelerationStructureFeaturesKHR, #151 */

    /**
     * Whether acceleration structure functionality is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,acceleration_structure}
     */
    AccelerationStructure,

    /**
     * Whether saving and reusing acceleration structure devices for trace
     * capture and replay is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,acceleration_structure}
     */
    AccelerationStructureCaptureReplay,

    /**
     * Whether indirect acceleration structure builds are supported.
     * @requires_vk_extension Extension @vk_extension{KHR,acceleration_structure}
     */
    AccelerationStructureIndirectBuild,

    /**
     * Whether host-side acceleration structure commands are supported.
     * @requires_vk_extension Extension @vk_extension{KHR,acceleration_structure}
     */
    AccelerationStructureHostCommands,

    /**
     * Whether acceleration structure descriptors can be updated after a set is
     * bound.
     * @see @ref DeviceFeature::DescriptorBindingUniformBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingSampledImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformTexelBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageTexelBufferUpdateAfterBind
     * @requires_vk_extension Extension @vk_extension{KHR,acceleration_structure}
     */
    DescriptorBindingAccelerationStructureUpdateAfterBind,

    /* VkPhysicalDeviceSamplerYcbcrConversionFeatures, #157 */

    /**
     * Whether sampler Y'C<sub>B</sub>C<sub>R</sub> conversion is supported.
     * @requires_vk11 Extension @vk_extension{KHR,sampler_ycbcr_conversion}
     */
    SamplerYcbcrConversion,

    /* VkPhysicalDeviceDescriptorIndexingFeatures, #162 */

    /**
     * Whether arrays of input attachment can be indexed by dynamically uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderInputAttachmentArrayDynamicIndexing,

    /**
     * Whether arrays of uniform texel buffers can be indexed by dynamically
     * uniform integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderUniformTexelBufferArrayDynamicIndexing,

    /**
     * Whether arrays of storage texel buffers can be indexed by dynamically
     * uniform integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderStorageTexelBufferArrayDynamicIndexing,

    /**
     * Whether arrays of uniform buffers can be indexed by non-uniform integer
     * expressions in shader code.
     * @see @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderUniformBufferArrayNonUniformIndexing,

    /**
     * Whether arrays of samplers or sampled images can be indexed by
     * non-uniform integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderSampledImageArrayNonUniformIndexing,

    /**
     * Whether arrays of storage buffers can be indexed by non-uniform integer
     * expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderStorageBufferArrayNonUniformIndexing,

    /**
     * Whether arrays of storage images can be indexed by non-uniform integer
     * expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderStorageImageArrayNonUniformIndexing,

    /**
     * Whether arrays of input attachments can be indexed by non-uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderInputAttachmentArrayNonUniformIndexing,

    /**
     * Whether arrays of uniform texel buffers can be indexed by non-uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderUniformTexelBufferArrayNonUniformIndexing,

    /**
     * Whether arrays of storage texel buffers can be indexed by non-uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayNonUniformIndexing,
     *      @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing
     *      @ref DeviceFeature::ShaderSampledImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageImageArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderInputAttachmentArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderUniformTexelBufferArrayDynamicIndexing,
     *      @ref DeviceFeature::ShaderStorageTexelBufferArrayDynamicIndexing
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderStorageTexelBufferArrayNonUniformIndexing,

    /**
     * Whether uniform buffer descriptors can be updated after a set is bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingSampledImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformTexelBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageTexelBufferUpdateAfterBind
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingUniformBufferUpdateAfterBind,

    /**
     * Whether sampled image descriptors can be updated after a set is bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformTexelBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageTexelBufferUpdateAfterBind
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingSampledImageUpdateAfterBind,

    /**
     * Whether storage image descriptors can be updated after a set is bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingSampledImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformTexelBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageTexelBufferUpdateAfterBind
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingStorageImageUpdateAfterBind,

    /**
     * Whether storage buffer descriptors can be updated after a set is bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingSampledImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformTexelBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageTexelBufferUpdateAfterBind
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingStorageBufferUpdateAfterBind,

    /**
     * Whether uniform texel buffer descriptors can be updated after a set is
     * bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingSampledImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageTexelBufferUpdateAfterBind
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingUniformTexelBufferUpdateAfterBind,

    /**
     * Whether storage texel buffer descriptors can be updated after a set is
     * bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingSampledImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageImageUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingStorageBufferUpdateAfterBind,
     *      @ref DeviceFeature::DescriptorBindingUniformTexelBufferUpdateAfterBind
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingStorageTexelBufferUpdateAfterBind,

    /**
     * Whether descriptors can be updated while the set is in use.
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingUpdateUnusedWhilePending,

    /**
     * Whether a descriptor set binding in which some descriptors are not valid
     * can be statically used.
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingPartiallyBound,

    /**
     * Whether descriptor sets with a variably-sized last binding are
     * supported.
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingVariableDescriptorCount,

    /**
     * Whether SPIR-V runtime descriptor arrays are supported.
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    RuntimeDescriptorArray,

    /* VkPhysicalDevicePortabilitySubsetFeaturesKHR, #164 */

    /**
     * Whether constant alpha as source or destination blend color is
     * supported.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    ConstantAlphaColorBlendFactors,

    /**
     * Whether synchronization using events is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    Events,

    /**
     * Whether an @ref ImageView can be created with a format containing
     * different number of components or a different number of bits in each
     * component than the format of the underlying @ref Image.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    ImageViewFormatReinterpretation,

    /**
     * Whether remapping format components in an @ref ImageView is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    ImageViewFormatSwizzle,

    /**
     * Whether a 2D view can be created on a 3D @ref Image can be created.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    ImageView2DOn3DImage,

    /**
     * Whether multisample 2D array @ref Image can be created.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    MultisampleArrayImage,

    /**
     * Whether descriptors with comparison samplers can be updated.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    MutableComparisonSamplers,

    /**
     * Whether rasterization using a point polygon mode is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    PointPolygons,

    /**
     * Whether setting a mipmap LOD bias when creating a sampler is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    SamplerMipLodBias,

    /**
     * Whether separate front and back stencil test reference values are
     * supported.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    SeparateStencilMaskRef,

    /**
     * Whether interpolation at centroid, offset and sample is supported.
     * Depends on @ref DeviceFeature::SampleRateShading.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    ShaderSampleRateInterpolationFunctions,

    /**
     * Whether isoline output from a tessellation shader stage is supported.
     * Depends on @ref DeviceFeature::TessellationShader.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    TessellationIsolines,

    /**
     * Whether point output from a tessellation shader stage is supported.
     * Depends on @ref DeviceFeature::TessellationShader.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    TessellationPointMode,

    /**
     * Whether triangle fan mesh primitives are supported.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    TriangleFans,

    /**
     * Whether accessing a vertex input attribute beyond the stride of
     * corresponding vertex input binding is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,portability_subset},
     *      see @ref Vk-Device-portability-subset for details.
     */
    VertexAttributeAccessBeyondStride,

    /* VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures, #176 */

    /**
     * Whether subgroup operations can use 8-bit integer, 16-bit integer,
     * 64-bit integer and 16-bit floating point types in group operations with
     * subgroup scope, providing the types are supported.
     * @see @ref DeviceFeature::ShaderInt8, @ref DeviceFeature::ShaderInt16,
     *      @ref DeviceFeature::ShaderInt64, @ref DeviceFeature::ShaderFloat16
     * @requires_vk12 Extension @vk_extension{KHR,shader_subgroup_extended_types}
     */
    ShaderSubgroupExtendedTypes,

    /* VkPhysicalDevice8BitStorageFeatures, #178 */

    /**
     * Whether shader storage buffers can have 8-bit integer members. Subset of
     * @ref DeviceFeature::UniformAndStorageBuffer8BitAccess.
     * @see @ref DeviceFeature::StorageBuffer16BitAccess,
     *      @ref DeviceFeature::ShaderInt8
     * @requires_vk12 Extension @vk_extension{KHR,8bit_storage}
     */
    StorageBuffer8BitAccess,

    /**
     * Whether shader uniforms and storage buffers can have 8-bit integer
     * members. Superset of @ref DeviceFeature::StorageBuffer8BitAccess.
     * @see @ref DeviceFeature::UniformAndStorageBuffer16BitAccess,
     *      @ref DeviceFeature::ShaderInt8
     * @requires_vk12 Extension @vk_extension{KHR,8bit_storage}
     */
    UniformAndStorageBuffer8BitAccess,

    /**
     * Whether shader push constants can have 8-bit integer members.
     * @see @ref DeviceFeature::StoragePushConstant16,
     *      @ref DeviceFeature::ShaderInt8
     * @requires_vk12 Extension @vk_extension{KHR,8bit_storage}
     */
    StoragePushConstant8,

    /* VkPhysicalDeviceShaderAtomicInt64Features, #181 */

    /**
     * Whether shaders can perform 64-bit integer atomic operations on buffers.
     * @see @ref DeviceFeature::ShaderSharedInt64Atomics
     * @requires_vk12 Extension @vk_extension{KHR,shader_atomic_int64}
     */
    ShaderBufferInt64Atomics,

    /**
     * Whether shaders can perform 64-bit integer atomic operations on shared
     * memory.
     * @see @ref DeviceFeature::ShaderBufferInt64Atomics
     * @requires_vk12 Extension @vk_extension{KHR,shader_atomic_int64}
     */
    ShaderSharedInt64Atomics,

    /* VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT, #191 */

    /**
     * Whether vertex attribute fetching may be repeated in case of instanced
     * rendering.
     * @requires_vk_extension Extension @vk_extension{EXT,vertex_attribute_divisor}
     */
    VertexAttributeInstanceRateDivisor,

    /**
     * Whether zero vertex attribute divisor is supported.
     * @requires_vk_extension Extension @vk_extension{EXT,vertex_attribute_divisor}
     */
    VertexAttributeInstanceRateZeroDivisor,

    /* VkPhysicalDeviceTimelineSemaphoreFeatures, #208 */

    /**
     * Whether timeline semaphores are supported.
     * @requires_vk12 Extension @vk_extension{KHR,timeline_semaphore}
     */
    TimelineSemaphore,

    /* VkPhysicalDeviceVulkanMemoryModelFeatures, #212 */

    /**
     * Whether the Vulkan Memory Model is supported.
     * @see @ref DeviceFeature::VulkanMemoryModelDeviceScope,
     *      @ref DeviceFeature::VulkanMemoryModelAvailabilityVisibilityChains
     * @requires_vk12 Extension @vk_extension{KHR,vulkan_memory_model}
     */
    VulkanMemoryModel,

    /**
     * Whether the Vulkan Memory Model can use device scope synchronization.
     * @see @ref DeviceFeature::VulkanMemoryModel,
     *      @ref DeviceFeature::VulkanMemoryModelAvailabilityVisibilityChains
     * @requires_vk12 Extension @vk_extension{KHR,vulkan_memory_model}
     */
    VulkanMemoryModelDeviceScope,

    /**
     * Whether the Vulkan Memory Model can use availability and visibility
     * chains with more than one element.
     * @see @ref DeviceFeature::VulkanMemoryModel,
     *      @ref DeviceFeature::VulkanMemoryModelDeviceScope
     * @requires_vk12 Extension @vk_extension{KHR,vulkan_memory_model}
     */
    VulkanMemoryModelAvailabilityVisibilityChains,

    /* VkPhysicalDeviceScalarBlockLayoutFeatures, #222 */

    /**
     * Whether layout of resource blocks in shaders using scalar alignment is
     * supported.
     * @requires_vk12 Extension @vk_extension{EXT,scalar_block_layout}
     */
    ScalarBlockLayout,

    /* VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures, #242 */

    /**
     * Whether the implementation supports an image barrier for a depth/stencil
     * image with only one of the depth/stencil aspects and whether separate
     * depth/stencil layouts can be used.
     * @requires_vk12 Extension @vk_extension{KHR,separate_depth_stencil_layouts}
     * @todoc link to the actual ImageAspect and ImageLayout members once they
     *      exist
     */
    SeparateDepthStencilLayouts,

    /* VkPhysicalDeviceUniformBufferStandardLayoutFeatures, #254 */

    /**
     * Whether same layouts for uniform buffers as for storage and other kinds
     * of buffers are supported.
     * @requires_vk12 Extension @vk_extension{KHR,uniform_buffer_standard_layout}
     */
    UniformBufferStandardLayout,

    /* VkPhysicalDeviceBufferDeviceAddressFeatures, #258 */

    /**
     * Whether accessing buffer memory in shaders as storage buffers via an
     * address queried from @fn_vk{GetBufferDeviceAddress} is supported.
     * @see @ref DeviceFeature::BufferDeviceAddressCaptureReplay,
     *      @ref DeviceFeature::BufferDeviceAddressMultiDevice
     * @requires_vk12 Extension @vk_extension{KHR,buffer_device_address}
     */
    BufferDeviceAddress,

    /**
     * Whether saving and reusing buffer dand device addresses for trace
     * capture and replay is supported.
     * @see @ref DeviceFeature::BufferDeviceAddress,
     *      @ref DeviceFeature::BufferDeviceAddressMultiDevice
     * @requires_vk12 Extension @vk_extension{KHR,buffer_device_address}
     */
    BufferDeviceAddressCaptureReplay,

    /**
     * Whether @ref DeviceFeature::BufferDeviceAddress for logical devices
     * created with multiple physical devices is supported.
     * @requires_vk12 Extension @vk_extension{KHR,buffer_device_address}
     * @todoc this also references `rayTracingPipeline`, `rayQuery`, update
     *      when those are exposed
     */
    BufferDeviceAddressMultiDevice,

    /* VkPhysicalDeviceHostQueryResetFeatures, #262 */

    /**
     * Whether queries can be reset from the host with @fn_vk{ResetQueryPool}.
     * @requires_vk12 Extension @vk_extension{EXT,host_query_reset}
     */
    HostQueryReset,

    /* VkPhysicalDeviceIndexTypeUint8FeaturesEXT, #266 */

    /**
     * Whether an 8-bit type can be used in index buffers.
     * @see @ref DeviceFeature::FullDrawIndexUint32
     * @requires_vk_extension Extension @vk_extension{EXT,index_type_uint8}
     */
    IndexTypeUint8,

    /* VkPhysicalDeviceRayTracingPipelineFeaturesKHR, #348 */

    /**
     * Whether ray tracing pipeline functionality is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,ray_tracing_pipeline}
     */
    RayTracingPipeline,

    /**
     * Whether saving and reusing shader group handles for trace capture and
     * replay is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,ray_tracing_pipeline}
     */
    RayTracingPipelineShaderGroupHandleCaptureReplay,

    /**
     * Whether reused shader group handles can be arbitrarily mixed with
     * creation of non-reused shader group handles.
     * @requires_vk_extension Extension @vk_extension{KHR,ray_tracing_pipeline}
     */
    RayTracingPipelineShaderGroupHandleCaptureReplayMixed,

    /**
     * Whether indirect trace ray commands are supported.
     * @requires_vk_extension Extension @vk_extension{KHR,ray_tracing_pipeline}
     */
    RayTracingPipelineTraceRaysIndirect,

    /**
     * Whether primitive culling during ray traversal is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,ray_tracing_pipeline}
     */
    RayTraversalPrimitiveCulling,

    /* VkPhysicalDeviceRayQueryFeaturesKHR, #349 */

    /**
     * Whether ray query functionality is supported.
     * @requires_vk_extension Extension @vk_extension{KHR,ray_query}
     */
    RayQuery
};

/**
@brief Device features
@m_since_latest

Combines information stored in the @type_vk{PhysicalDeviceFeatures2} structure
chain.
@see @ref DeviceProperties::features()
*/
typedef Containers::BigEnumSet<DeviceFeature, 4> DeviceFeatures;

CORRADE_ENUMSET_OPERATORS(DeviceFeatures)

/**
@debugoperatorenum{DeviceFeature}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, DeviceFeature value);

/**
@debugoperatorenum{DeviceFeatures}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, const DeviceFeatures& value);

}}

#endif
