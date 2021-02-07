#ifndef Magnum_Vk_DeviceFeatures_h
#define Magnum_Vk_DeviceFeatures_h
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

Contains information stored in the @type_vk_keyword{PhysicalDeviceFeatures2}
structure chain. Values correspond to equivalently named structure fields,
usually just with the first letter uppercase instead of lowercase.
@see @ref DeviceFeatures, @ref DeviceProperties::features(),
    @ref DeviceCreateInfo::setEnabledFeatures(), @ref Device::enabledFeatures()

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
     * buffer descriptor. Out of bounds accesses are guaranteed to not cause
     * application termination, but have implementation-dependent behavior. A
     * subset of the guarantees provided by
     * @ref DeviceFeature::RobustBufferAccess2.
     * @see @ref DeviceFeature::RobustImageAccess,
     *      @relativeref{DeviceFeature,RobustImageAccess2}
     */
    RobustBufferAccess,

    /**
     * Whether the full 32-bit range is supported for indexed draw calls when
     * using @val_vk{INDEX_TYPE_UINT32,IndexType}.
     * @see @ref DeviceFeature::IndexTypeUnsignedByte
     * @todo expose the `maxDrawIndexedIndexValue` limit
     */
    FullDrawIndexUnsignedInt,

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
     * @relativeref{PixelFormat,CompressedEacR11Snorm},
     * @relativeref{PixelFormat,CompressedEacRG11Unorm},
     * @relativeref{PixelFormat,CompressedEacRG11Snorm},
     * @relativeref{PixelFormat,CompressedEtc2RGB8Unorm},
     * @relativeref{PixelFormat,CompressedEtc2RGB8Srgb},
     * @relativeref{PixelFormat,CompressedEtc2RGB8A1Unorm},
     * @relativeref{PixelFormat,CompressedEtc2RGB8A1Srgb},
     * @relativeref{PixelFormat,CompressedEtc2RGBA8Unorm},
     * @relativeref{PixelFormat,CompressedEtc2RGBA8Unorm} ETC2 and EAC
     * compressed texture formats are *all* supported.
     * @see @ref DeviceFeature::TextureCompressionBc,
     *      @relativeref{DeviceFeature,TextureCompressionAstcLdr},
     *      @relativeref{DeviceFeature,TextureCompressionAstcHdr}
     */
    TextureCompressionEtc2,

    /**
     * Whether @ref PixelFormat::CompressedAstc4x4RGBAUnorm,
     * @relativeref{PixelFormat,CompressedAstc4x4RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc5x4RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc5x4RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc5x5RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc5x5RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc6x5RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc6x5RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc6x6RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc6x6RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc8x5RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc8x5RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc8x6RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc8x6RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc8x8RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc8x8RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc10x5RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc10x5RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc10x6RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc10x6RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc10x8RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc10x8RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc10x10RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc10x10RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc12x10RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc12x10RGBASrgb},
     * @relativeref{PixelFormat,CompressedAstc12x12RGBAUnorm},
     * @relativeref{PixelFormat,CompressedAstc12x12RGBASrgb}, ASTC LDR
     * compressed texture formats are *all* supported.
     * @see @ref DeviceFeature::TextureCompressionAstcHdr,
     *      @relativeref{DeviceFeature,TextureCompressionEtc2},
     *      @relativeref{DeviceFeature,TextureCompressionBc}
     */
    TextureCompressionAstcLdr,

    /**
     * Whether @ref PixelFormat::CompressedBc1RGBUnorm,
     * @relativeref{PixelFormat,CompressedBc1RGBSrgb},
     * @relativeref{PixelFormat,CompressedBc1RGBAUnorm},
     * @relativeref{PixelFormat,CompressedBc1RGBASrgb},
     * @relativeref{PixelFormat,CompressedBc2RGBAUnorm},
     * @relativeref{PixelFormat,CompressedBc2RGBASrgb},
     * @relativeref{PixelFormat,CompressedBc3RGBAUnorm},
     * @relativeref{PixelFormat,CompressedBc3RGBASrgb},
     * @relativeref{PixelFormat,CompressedBc4RUnorm},
     * @relativeref{PixelFormat,CompressedBc4RSnorm},
     * @relativeref{PixelFormat,CompressedBc5RGUnorm},
     * @relativeref{PixelFormat,CompressedBc5RGSnorm},
     * @relativeref{PixelFormat,CompressedBc6hRGBUfloat},
     * @relativeref{PixelFormat,CompressedBc6hRGBSfloat},
     * @relativeref{PixelFormat,CompressedBc7RGBAUnorm},
     * @relativeref{PixelFormat,CompressedBc7RGBASrgb} BC compressed texture
     * formats are *all* supported.
     * @see @ref DeviceFeature::TextureCompressionEtc2,
     *      @relativeref{DeviceFeature,TextureCompressionAstcLdr},
     *      @relativeref{DeviceFeature,TextureCompressionAstcHdr}
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
     * Whether @ref PixelFormat::R8Unorm, @relativeref{PixelFormat,RG8Unorm},
     * @relativeref{PixelFormat,R8Snorm}, @relativeref{PixelFormat,RG8Snorm},
     * @relativeref{PixelFormat,R8UI}, @relativeref{PixelFormat,RG8UI},
     * @relativeref{PixelFormat,R8I}, @relativeref{PixelFormat,RG8I},
     * @relativeref{PixelFormat,R16Unorm}, @relativeref{PixelFormat,RG16Unorm},
     * @relativeref{PixelFormat,RGBA16Unorm}, @relativeref{PixelFormat,R16Snorm},
     * @relativeref{PixelFormat,RG16Snorm}, @relativeref{PixelFormat,RGBA16Snorm},
     * @relativeref{PixelFormat,R16UI}, @relativeref{PixelFormat,RG16UI},
     * @relativeref{PixelFormat,R16I}, @relativeref{PixelFormat,RG16I},
     * @relativeref{PixelFormat,R16F}, @relativeref{PixelFormat,RG16F} are
     * *all* supported for shader storage.
     * @todoc link to the 10/10/10/2 and 10.11.10F formats once exposed
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
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing}
     */
    ShaderUniformBufferArrayDynamicIndexing,

    /**
     * Whether arrays of samplers or sampled images can be indexed by
     * dynamically uniform integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing}
     */
    ShaderSampledImageArrayDynamicIndexing,

    /**
     * Whether arrays of storage buffers can be indexed by dynamically uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing}
     */
    ShaderStorageBufferArrayDynamicIndexing,

    /**
     * Whether arrays of storage images can be indexed by dynamically uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing}
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
     * @see @ref DeviceFeature::ShaderInt16,
     *      @relativeref{DeviceFeature,ShaderInt8},
     *      @relativeref{DeviceFeature,ShaderSubgroupExtendedTypes}
     */
    ShaderInt64,

    /**
     * Whether 16-bit integers are supported in shader code.
     * @see @ref DeviceFeature::ShaderInt64,
     *      @relativeref{DeviceFeature,ShaderInt8},
     *      @relativeref{DeviceFeature,StorageBuffer16BitAccess},
     *      @relativeref{DeviceFeature,UniformAndStorageBuffer16BitAccess},
     *      @relativeref{DeviceFeature,StoragePushConstant16},
     *      @relativeref{DeviceFeature,StorageInputOutput16},
     *      @relativeref{DeviceFeature,ShaderSubgroupExtendedTypes}
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
     * @relativeref{PixelFormat,CompressedAstc5x4RGBAF},
     * @relativeref{PixelFormat,CompressedAstc5x5RGBAF},
     * @relativeref{PixelFormat,CompressedAstc6x5RGBAF},
     * @relativeref{PixelFormat,CompressedAstc6x6RGBAF},
     * @relativeref{PixelFormat,CompressedAstc8x5RGBAF},
     * @relativeref{PixelFormat,CompressedAstc8x6RGBAF},
     * @relativeref{PixelFormat,CompressedAstc8x8RGBAF},
     * @relativeref{PixelFormat,CompressedAstc10x5RGBAF},
     * @relativeref{PixelFormat,CompressedAstc10x6RGBAF},
     * @relativeref{PixelFormat,CompressedAstc10x8RGBAF},
     * @relativeref{PixelFormat,CompressedAstc10x10RGBAF},
     * @relativeref{PixelFormat,CompressedAstc12x10RGBAF},
     * @relativeref{PixelFormat,CompressedAstc12x12RGBAF} ASTC HDR compressed
     * texture formats are *all* supported.
     * @see @ref DeviceFeature::TextureCompressionAstcLdr,
     *      @relativeref{DeviceFeature,TextureCompressionEtc2},
     *      @relativeref{DeviceFeature,TextureCompressionBc}
     * @requires_vk_extension Extension @vk_extension{EXT,texture_compression_astc_hdr}
     */
    TextureCompressionAstcHdr,

    /* VkPhysicalDeviceShaderFloat16Int8Features, #83 */

    /**
     * Whether 16-bit floats are supported in shader code.
     * @see @ref DeviceFeature::ShaderFloat64,
     *      @relativeref{DeviceFeature,StorageBuffer16BitAccess},
     *      @relativeref{DeviceFeature,UniformAndStorageBuffer16BitAccess},
     *      @relativeref{DeviceFeature,StoragePushConstant16},
     *      @relativeref{DeviceFeature,StorageInputOutput16},
     *      @relativeref{DeviceFeature,ShaderSubgroupExtendedTypes}
     * @requires_vk12 Extension @vk_extension{KHR,shader_float16_int8}
     */
    ShaderFloat16,

    /**
     * Whether 8-bit integers are supported in shader code.
     * @see @ref DeviceFeature::ShaderInt64,
     *      @relativeref{DeviceFeature,ShaderInt16},
     *      @relativeref{DeviceFeature,ShaderSubgroupExtendedTypes}
     * @requires_vk12 Extension @vk_extension{KHR,shader_float16_int8}
     */
    ShaderInt8,

    /* VkPhysicalDevice16BitStorageFeatures, #84 */

    /**
     * Whether shader storage buffers can have 16-bit integer and 16-bit
     * floating-point members. Subset of
     * @ref DeviceFeature::UniformAndStorageBuffer16BitAccess.
     * @see @ref DeviceFeature::StorageBuffer8BitAccess,
     *      @relativeref{DeviceFeature,ShaderInt16},
     *      @relativeref{DeviceFeature,ShaderFloat16}
     * @requires_vk11 Extension @vk_extension{KHR,16bit_storage}
     */
    StorageBuffer16BitAccess,

    /**
     * Whether shader uniforms and storage buffers can have 16-bit integer and
     * 16-bit floating-point members. Superset of
     * @ref DeviceFeature::StorageBuffer16BitAccess.
     * @see @ref DeviceFeature::UniformAndStorageBuffer8BitAccess,
     *      @relativeref{DeviceFeature,ShaderInt16},
     *      @relativeref{DeviceFeature,ShaderFloat16}
     * @requires_vk11 Extension @vk_extension{KHR,16bit_storage}
     */
    UniformAndStorageBuffer16BitAccess,

    /**
     * Whether shader push constants can have 16-bit integer and 16-bit
     * floating-point members.
     * @see @ref DeviceFeature::StoragePushConstant8,
     *      @relativeref{DeviceFeature,ShaderInt16},
     *      @relativeref{DeviceFeature,ShaderFloat16}
     * @requires_vk11 Extension @vk_extension{KHR,16bit_storage}
     */
    StoragePushConstant16,

    /**
     * Whether shader inputs and outputs can have 16-bit integer and 16-bit
     * floating-point members.
     * @see @ref DeviceFeature::ShaderInt16,
     *      @relativeref{DeviceFeature,ShaderFloat16}
     * @requires_vk11 Extension @vk_extension{KHR,16bit_storage}
     */
    StorageInputOutput16,

    /* VkPhysicalDeviceImagelessFramebufferFeatures, #109 */

    /**
     * Specifying image view for attachments is possible at a time
     * @ref CommandBuffer::beginRenderPass() is called.
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
     *      @relativeref{DeviceFeature,DescriptorBindingSampledImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingUniformTexelBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageTexelBufferUpdateAfterBind}
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
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderInputAttachmentArrayDynamicIndexing,

    /**
     * Whether arrays of uniform texel buffers can be indexed by dynamically
     * uniform integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderUniformTexelBufferArrayDynamicIndexing,

    /**
     * Whether arrays of storage texel buffers can be indexed by dynamically
     * uniform integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayDynamicIndexing,
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderStorageTexelBufferArrayDynamicIndexing,

    /**
     * Whether arrays of uniform buffers can be indexed by non-uniform integer
     * expressions in shader code.
     * @see @ref DeviceFeature::ShaderSampledImageArrayNonUniformIndexing,
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayDynamicIndexing}
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderUniformBufferArrayNonUniformIndexing,

    /**
     * Whether arrays of samplers or sampled images can be indexed by
     * non-uniform integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayDynamicIndexing}
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderSampledImageArrayNonUniformIndexing,

    /**
     * Whether arrays of storage buffers can be indexed by non-uniform integer
     * expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayDynamicIndexing}
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderStorageBufferArrayNonUniformIndexing,

    /**
     * Whether arrays of storage images can be indexed by non-uniform integer
     * expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayDynamicIndexing}
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderStorageImageArrayNonUniformIndexing,

    /**
     * Whether arrays of input attachments can be indexed by non-uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayDynamicIndexing}
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderInputAttachmentArrayNonUniformIndexing,

    /**
     * Whether arrays of uniform texel buffers can be indexed by non-uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayDynamicIndexing}
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderUniformTexelBufferArrayNonUniformIndexing,

    /**
     * Whether arrays of storage texel buffers can be indexed by non-uniform
     * integer expressions in shader code.
     * @see @ref DeviceFeature::ShaderUniformBufferArrayNonUniformIndexing,
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayNonUniformIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformBufferArrayDynamicIndexing}
     *      @relativeref{DeviceFeature,ShaderSampledImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageImageArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderInputAttachmentArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderUniformTexelBufferArrayDynamicIndexing},
     *      @relativeref{DeviceFeature,ShaderStorageTexelBufferArrayDynamicIndexing}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    ShaderStorageTexelBufferArrayNonUniformIndexing,

    /**
     * Whether uniform buffer descriptors can be updated after a set is bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @relativeref{DeviceFeature,DescriptorBindingSampledImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingUniformTexelBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageTexelBufferUpdateAfterBind}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingUniformBufferUpdateAfterBind,

    /**
     * Whether sampled image descriptors can be updated after a set is bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @relativeref{DeviceFeature,DescriptorBindingUniformBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingUniformTexelBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageTexelBufferUpdateAfterBind}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingSampledImageUpdateAfterBind,

    /**
     * Whether storage image descriptors can be updated after a set is bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @relativeref{DeviceFeature,DescriptorBindingUniformBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingSampledImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingUniformTexelBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageTexelBufferUpdateAfterBind}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingStorageImageUpdateAfterBind,

    /**
     * Whether storage buffer descriptors can be updated after a set is bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @relativeref{DeviceFeature,DescriptorBindingUniformBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingSampledImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingUniformTexelBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageTexelBufferUpdateAfterBind}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingStorageBufferUpdateAfterBind,

    /**
     * Whether uniform texel buffer descriptors can be updated after a set is
     * bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @relativeref{DeviceFeature,DescriptorBindingUniformBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingSampledImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageTexelBufferUpdateAfterBind}
     * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
     */
    DescriptorBindingUniformTexelBufferUpdateAfterBind,

    /**
     * Whether storage texel buffer descriptors can be updated after a set is
     * bound.
     * @see @ref DeviceFeature::DescriptorBindingAccelerationStructureUpdateAfterBind,
     *      @relativeref{DeviceFeature,DescriptorBindingUniformBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingSampledImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageImageUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingStorageBufferUpdateAfterBind},
     *      @relativeref{DeviceFeature,DescriptorBindingUniformTexelBufferUpdateAfterBind}
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
     * @see @ref DeviceFeature::ShaderInt8,
     *      @relativeref{DeviceFeature,ShaderInt16},
     *      @relativeref{DeviceFeature,ShaderInt64},
     *      @relativeref{DeviceFeature,ShaderFloat16}
     * @requires_vk12 Extension @vk_extension{KHR,shader_subgroup_extended_types}
     */
    ShaderSubgroupExtendedTypes,

    /* VkPhysicalDevice8BitStorageFeatures, #178 */

    /**
     * Whether shader storage buffers can have 8-bit integer members. Subset of
     * @ref DeviceFeature::UniformAndStorageBuffer8BitAccess.
     * @see @ref DeviceFeature::StorageBuffer16BitAccess,
     *      @relativeref{DeviceFeature,ShaderInt8}
     * @requires_vk12 Extension @vk_extension{KHR,8bit_storage}
     */
    StorageBuffer8BitAccess,

    /**
     * Whether shader uniforms and storage buffers can have 8-bit integer
     * members. Superset of @ref DeviceFeature::StorageBuffer8BitAccess.
     * @see @ref DeviceFeature::UniformAndStorageBuffer16BitAccess,
     *      @relativeref{DeviceFeature,ShaderInt8}
     * @requires_vk12 Extension @vk_extension{KHR,8bit_storage}
     */
    UniformAndStorageBuffer8BitAccess,

    /**
     * Whether shader push constants can have 8-bit integer members.
     * @see @ref DeviceFeature::StoragePushConstant16,
     *      @relativeref{DeviceFeature,ShaderInt8}
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
     *      @relativeref{DeviceFeature,VulkanMemoryModelAvailabilityVisibilityChains}
     * @requires_vk12 Extension @vk_extension{KHR,vulkan_memory_model}
     */
    VulkanMemoryModel,

    /**
     * Whether the Vulkan Memory Model can use device scope synchronization.
     * @see @ref DeviceFeature::VulkanMemoryModel,
     *      @relativeref{DeviceFeature,VulkanMemoryModelAvailabilityVisibilityChains}
     * @requires_vk12 Extension @vk_extension{KHR,vulkan_memory_model}
     */
    VulkanMemoryModelDeviceScope,

    /**
     * Whether the Vulkan Memory Model can use availability and visibility
     * chains with more than one element.
     * @see @ref DeviceFeature::VulkanMemoryModel,
     *      @relativeref{DeviceFeature,VulkanMemoryModelDeviceScope}
     * @requires_vk12 Extension @vk_extension{KHR,vulkan_memory_model} since
     *      revision 3
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
     *      all exist
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
     *      @relativeref{DeviceFeature,BufferDeviceAddressMultiDevice}
     * @requires_vk12 Extension @vk_extension{KHR,buffer_device_address}
     */
    BufferDeviceAddress,

    /**
     * Whether saving and reusing buffer dand device addresses for trace
     * capture and replay is supported.
     * @see @ref DeviceFeature::BufferDeviceAddress,
     *      @relativeref{DeviceFeature,BufferDeviceAddressMultiDevice}
     * @requires_vk12 Extension @vk_extension{KHR,buffer_device_address}
     */
    BufferDeviceAddressCaptureReplay,

    /**
     * Whether @ref DeviceFeature::BufferDeviceAddress,
     * @relativeref{DeviceFeature,RayTracingPipeline} and
     * @relativeref{DeviceFeature,RayQuery} for logical devices created with
     * multiple physical devices is supported.
     * @requires_vk12 Extension @vk_extension{KHR,buffer_device_address}
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
     * @see @ref DeviceFeature::FullDrawIndexUnsignedInt
     * @requires_vk_extension Extension @vk_extension{EXT,index_type_uint8}
     */
    IndexTypeUnsignedByte,

    /* VkPhysicalDeviceExtendedDynamicStateFeaturesEXT, #268 */

    /**
     * The @ref DynamicRasterizationState::CullMode,
     * @relativeref{DynamicRasterizationState,FrontFace},
     * @relativeref{DynamicRasterizationState,PrimitiveTopology},
     * @relativeref{DynamicRasterizationState,ViewportWithCount},
     * @relativeref{DynamicRasterizationState,ScissorWithCount},
     * @relativeref{DynamicRasterizationState,VertexInputBindingStride},
     * @relativeref{DynamicRasterizationState,DepthTestEnable},
     * @relativeref{DynamicRasterizationState,DepthWriteEnable},
     * @relativeref{DynamicRasterizationState,DepthCompareOperation},
     * @relativeref{DynamicRasterizationState,DepthBoundsTestEnable},
     * @relativeref{DynamicRasterizationState,StencilTestEnable} and
     * @relativeref{DynamicRasterizationState,StencilOperation} extended
     * dynamic state is supported.
     * @requires_vk_extension Extension @vk_extension{EXT,extended_dynamic_state}
     */
    ExtendedDynamicState,

    /* VkPhysicalDeviceRobustness2FeaturesEXT, #287 */

    /**
     * Whether accesses to buffers are bounds-checked against the range of the
     * buffer descriptor, rounded up to a multiple of buffer access size
     * alignment. Out of bounds writes are guaranteed to be discarded, out of
     * bounds reads are guaranteed to return zero values. A stricter but more
     * expensive variant of @ref DeviceFeature::RobustBufferAccess.
     * @requires_vk_extension Extension @vk_extension{EXT,robustness2}
     * @todo expose the alignment limit
     */
    RobustBufferAccess2,

    /**
     * Whether accesses to images are bounds-checked against the dimensions of
     * the image view. Out of bounds loads are guaranteed to return zero
     * values, with @f$ (0, 0, 0, 1) @f$ inserted for missing components based
     * on the image format. A stricted but more expensive variant of
     * @ref DeviceFeature::RobustImageAccess.
     * @see @ref DeviceFeature::RobustBufferAccess,
     *      @relativeref{DeviceFeature,RobustBufferAccess2}
     * @requires_vk_extension Extension @vk_extension{EXT,robustness2}
     */
    RobustImageAccess2,

    /**
     * Whether descriptors can be written with a null resource or view handle,
     * at which point they're considered valid and act as if the descriptor was
     * bound to nothing.
     * @requires_vk_extension Extension @vk_extension{EXT,robustness2}
     */
    NullDescriptor,

    /* VkPhysicalDeviceImageRobustnessFeaturesEXT, #336 */

    /**
     * Whether accesses to images are bounds-checked against the dimensions of
     * the image view. Out of bounds loads are guaranteed to return zero
     * values, with either @f$ (0, 0, 0, 0) @f$ or @f$ (0, 0, 0, 1) @f$
     * inserted for missing components based on the image format, with no
     * guarantees provided for values returned for an invalid mip level. A
     * subset of the guarantees provided by
     * @ref DeviceFeature::RobustImageAccess2.
     * @see @ref DeviceFeature::RobustBufferAccess,
     *      @relativeref{DeviceFeature,RobustBufferAccess2}
     * @requires_vk_extension Extension @vk_extension{EXT,image_robustness}
     */
    RobustImageAccess,

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
