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

/* See Magnum/Vk/Device.cpp, Magnum/Vk/DeviceFeatures.cpp,
   Magnum/Vk/DeviceProperties.cpp and Magnum/Vk/vk-info.cpp for actual usage
   which explains what each parameter represents. Needs the _c(), _cver() and
   _cext() macros defined, for core, version-dependent and extension-dependent
   features respectively:

    _c(value, field)
    _cver(value, field, suffix, version)
    _cext(value, field, suffix, extension)

*/
#ifdef _c
_c(RobustBufferAccess, robustBufferAccess)
_c(FullDrawIndexUnsignedInt, fullDrawIndexUint32)
_c(ImageCubeArray, imageCubeArray)
_c(IndependentBlend, independentBlend)
_c(GeometryShader, geometryShader)
_c(TessellationShader, tessellationShader)
_c(SampleRateShading, sampleRateShading)
_c(DualSrcBlend, dualSrcBlend)
_c(LogicOp, logicOp)
_c(MultiDrawIndirect, multiDrawIndirect)
_c(DrawIndirectFirstInstance, drawIndirectFirstInstance)
_c(DepthClamp, depthClamp)
_c(DepthBiasClamp, depthBiasClamp)
_c(FillModeNonSolid, fillModeNonSolid)
_c(DepthBounds, depthBounds)
_c(WideLines, wideLines)
_c(LargePoints, largePoints)
_c(AlphaToOne, alphaToOne)
_c(MultiViewport, multiViewport)
_c(SamplerAnisotropy, samplerAnisotropy)
_c(TextureCompressionEtc2, textureCompressionETC2)
_c(TextureCompressionAstcLdr, textureCompressionASTC_LDR)
_c(TextureCompressionBc, textureCompressionBC)
_c(OcclusionQueryPrecise, occlusionQueryPrecise)
_c(PipelineStatisticsQuery, pipelineStatisticsQuery)
_c(VertexPipelineStoresAndAtomics, vertexPipelineStoresAndAtomics)
_c(FragmentStoresAndAtomics, fragmentStoresAndAtomics)
_c(ShaderTessellationAndGeometryPointSize, shaderTessellationAndGeometryPointSize)
_c(ShaderImageGatherExtended, shaderImageGatherExtended)
_c(ShaderStorageImageExtendedFormats, shaderStorageImageExtendedFormats)
_c(ShaderStorageImageMultisample, shaderStorageImageMultisample)
_c(ShaderStorageImageReadWithoutFormat, shaderStorageImageReadWithoutFormat)
_c(ShaderStorageImageWriteWithoutFormat, shaderStorageImageWriteWithoutFormat)
_c(ShaderUniformBufferArrayDynamicIndexing, shaderUniformBufferArrayDynamicIndexing)
_c(ShaderSampledImageArrayDynamicIndexing, shaderSampledImageArrayDynamicIndexing)
_c(ShaderStorageBufferArrayDynamicIndexing, shaderStorageBufferArrayDynamicIndexing)
_c(ShaderStorageImageArrayDynamicIndexing, shaderStorageImageArrayDynamicIndexing)
_c(ShaderClipDistance, shaderClipDistance)
_c(ShaderCullDistance, shaderCullDistance)
_c(ShaderFloat64, shaderFloat64)
_c(ShaderInt64, shaderInt64)
_c(ShaderInt16, shaderInt16)
_c(ShaderResourceResidency, shaderResourceResidency)
_c(ShaderResourceMinLod, shaderResourceMinLod)
_c(SparseBinding, sparseBinding)
_c(SparseResidencyBuffer, sparseResidencyBuffer)
_c(SparseResidencyImage2D, sparseResidencyImage2D)
_c(SparseResidencyImage3D, sparseResidencyImage3D)
_c(SparseResidency2Samples, sparseResidency2Samples)
_c(SparseResidency4Samples, sparseResidency4Samples)
_c(SparseResidency8Samples, sparseResidency8Samples)
_c(SparseResidency16Samples, sparseResidency16Samples)
_c(SparseResidencyAliased, sparseResidencyAliased)
_c(VariableMultisampleRate, variableMultisampleRate)
_c(InheritedQueries, inheritedQueries)

_cver(ProtectedMemory, protectedMemory, protectedMemory, Vk11)

#define _ce(value, field) _cext(value, field, multiview, KHR::multiview)
_ce(Multiview, multiview)
_ce(MultiviewGeometryShader, multiviewGeometryShader)
_ce(MultiviewTessellationShader, multiviewTessellationShader)
#undef _ce

_cext(ShaderDrawParameters, shaderDrawParameters, shaderDrawParameters, KHR::shader_draw_parameters)

_cext(TextureCompressionAstcHdr, textureCompressionASTC_HDR, textureCompressionAstcHdr, EXT::texture_compression_astc_hdr)

#define _ce(value, field) _cext(value, field, shaderFloat16Int8, KHR::shader_float16_int8)
_ce(ShaderFloat16, shaderFloat16)
_ce(ShaderInt8, shaderInt8)
#undef _ce

#define _ce(value, field) _cext(value, field, _16BitStorage, KHR::_16bit_storage)
_ce(StorageBuffer16BitAccess, storageBuffer16BitAccess)
_ce(UniformAndStorageBuffer16BitAccess, uniformAndStorageBuffer16BitAccess)
_ce(StoragePushConstant16, storagePushConstant16)
_ce(StorageInputOutput16, storageInputOutput16)
#undef _ce

_cext(ImagelessFramebuffer, imagelessFramebuffer, imagelessFramebuffer, KHR::imageless_framebuffer)

#define _ce(value, field) _cext(value, field, variablePointers, KHR::variable_pointers)
_ce(VariablePointersStorageBuffer, variablePointersStorageBuffer)
_ce(VariablePointers, variablePointers)
#undef _ce

#define _ce(value, field) _cext(value, field, accelerationStructure, KHR::acceleration_structure)
_ce(AccelerationStructure, accelerationStructure)
_ce(AccelerationStructureCaptureReplay, accelerationStructureCaptureReplay)
_ce(AccelerationStructureIndirectBuild, accelerationStructureIndirectBuild)
_ce(AccelerationStructureHostCommands, accelerationStructureHostCommands)
_ce(DescriptorBindingAccelerationStructureUpdateAfterBind, descriptorBindingAccelerationStructureUpdateAfterBind)
#undef _ce

_cext(SamplerYcbcrConversion, samplerYcbcrConversion, samplerYcbcrConversion, KHR::sampler_ycbcr_conversion)

#define _ce(value, field) _cext(value, field, descriptorIndexing, EXT::descriptor_indexing)
_ce(ShaderInputAttachmentArrayDynamicIndexing, shaderInputAttachmentArrayDynamicIndexing)
_ce(ShaderUniformTexelBufferArrayDynamicIndexing, shaderUniformTexelBufferArrayDynamicIndexing)
_ce(ShaderStorageTexelBufferArrayDynamicIndexing, shaderStorageTexelBufferArrayDynamicIndexing)
_ce(ShaderUniformBufferArrayNonUniformIndexing, shaderUniformBufferArrayNonUniformIndexing)
_ce(ShaderSampledImageArrayNonUniformIndexing, shaderSampledImageArrayNonUniformIndexing)
_ce(ShaderStorageBufferArrayNonUniformIndexing, shaderStorageBufferArrayNonUniformIndexing)
_ce(ShaderStorageImageArrayNonUniformIndexing, shaderStorageImageArrayNonUniformIndexing)
_ce(ShaderInputAttachmentArrayNonUniformIndexing, shaderInputAttachmentArrayNonUniformIndexing)
_ce(ShaderUniformTexelBufferArrayNonUniformIndexing, shaderUniformTexelBufferArrayNonUniformIndexing)
_ce(ShaderStorageTexelBufferArrayNonUniformIndexing, shaderStorageTexelBufferArrayNonUniformIndexing)
_ce(DescriptorBindingUniformBufferUpdateAfterBind, descriptorBindingUniformBufferUpdateAfterBind)
_ce(DescriptorBindingSampledImageUpdateAfterBind, descriptorBindingSampledImageUpdateAfterBind)
_ce(DescriptorBindingStorageImageUpdateAfterBind, descriptorBindingStorageImageUpdateAfterBind)
_ce(DescriptorBindingStorageBufferUpdateAfterBind, descriptorBindingStorageBufferUpdateAfterBind)
_ce(DescriptorBindingUniformTexelBufferUpdateAfterBind, descriptorBindingUniformTexelBufferUpdateAfterBind)
_ce(DescriptorBindingStorageTexelBufferUpdateAfterBind, descriptorBindingStorageTexelBufferUpdateAfterBind)
_ce(DescriptorBindingUpdateUnusedWhilePending, descriptorBindingUpdateUnusedWhilePending)
_ce(DescriptorBindingPartiallyBound, descriptorBindingPartiallyBound)
_ce(DescriptorBindingVariableDescriptorCount, descriptorBindingVariableDescriptorCount)
_ce(RuntimeDescriptorArray, runtimeDescriptorArray)
#undef _ce

#define _ce(value, field) _cext(value, field, portabilitySubset, KHR::portability_subset)
_ce(ConstantAlphaColorBlendFactors, constantAlphaColorBlendFactors)
_ce(Events, events)
_ce(ImageViewFormatReinterpretation, imageViewFormatReinterpretation)
_ce(ImageViewFormatSwizzle, imageViewFormatSwizzle)
_ce(ImageView2DOn3DImage, imageView2DOn3DImage)
_ce(MultisampleArrayImage, multisampleArrayImage)
_ce(MutableComparisonSamplers, mutableComparisonSamplers)
_ce(PointPolygons, pointPolygons)
_ce(SamplerMipLodBias, samplerMipLodBias)
_ce(SeparateStencilMaskRef, separateStencilMaskRef)
_ce(ShaderSampleRateInterpolationFunctions, shaderSampleRateInterpolationFunctions)
_ce(TessellationIsolines, tessellationIsolines)
_ce(TessellationPointMode, tessellationPointMode)
_ce(TriangleFans, triangleFans)
_ce(VertexAttributeAccessBeyondStride, vertexAttributeAccessBeyondStride)
#undef _ce

_cext(ShaderSubgroupExtendedTypes, shaderSubgroupExtendedTypes, shaderSubgroupExtendedTypes, KHR::shader_subgroup_extended_types)

#define _ce(value, field) _cext(value, field, _8BitStorage, KHR::_8bit_storage)
_ce(StorageBuffer8BitAccess, storageBuffer8BitAccess)
_ce(UniformAndStorageBuffer8BitAccess, uniformAndStorageBuffer8BitAccess)
_ce(StoragePushConstant8, storagePushConstant8)
#undef _ce

#define _ce(value, field) _cext(value, field, shaderAtomicInt64, KHR::shader_atomic_int64)
_ce(ShaderBufferInt64Atomics, shaderBufferInt64Atomics)
_ce(ShaderSharedInt64Atomics, shaderSharedInt64Atomics)
#undef _ce

#define _ce(value, field) _cext(value, field, vertexAttributeDivisor, EXT::vertex_attribute_divisor)
_ce(VertexAttributeInstanceRateDivisor, vertexAttributeInstanceRateDivisor)
_ce(VertexAttributeInstanceRateZeroDivisor, vertexAttributeInstanceRateZeroDivisor)
#undef _ce

_cext(TimelineSemaphore, timelineSemaphore, timelineSemaphore, KHR::timeline_semaphore)

#define _ce(value, field) _cext(value, field, vulkanMemoryModel, KHR::vulkan_memory_model)
_ce(VulkanMemoryModel, vulkanMemoryModel)
_ce(VulkanMemoryModelDeviceScope, vulkanMemoryModelDeviceScope)
_ce(VulkanMemoryModelAvailabilityVisibilityChains,vulkanMemoryModelAvailabilityVisibilityChains)
#undef _ce

_cext(ScalarBlockLayout, scalarBlockLayout, scalarBlockLayout, EXT::scalar_block_layout)

_cext(SeparateDepthStencilLayouts, separateDepthStencilLayouts, separateDepthStencilLayouts, KHR::separate_depth_stencil_layouts)

_cext(UniformBufferStandardLayout, uniformBufferStandardLayout, uniformBufferStandardLayout, KHR::uniform_buffer_standard_layout)

#define _ce(value, field) _cext(value, field, bufferDeviceAddress, KHR::buffer_device_address)
_ce(BufferDeviceAddress, bufferDeviceAddress)
_ce(BufferDeviceAddressCaptureReplay, bufferDeviceAddressCaptureReplay)
_ce(BufferDeviceAddressMultiDevice, bufferDeviceAddressMultiDevice)
#undef _ce

_cext(HostQueryReset, hostQueryReset, hostQueryReset, EXT::host_query_reset)

_cext(IndexTypeUnsignedByte, indexTypeUint8, indexTypeUint8, EXT::index_type_uint8)

_cext(ExtendedDynamicState, extendedDynamicState, extendedDynamicState, EXT::extended_dynamic_state)

#define _ce(value, field) _cext(value, field, robustness2, EXT::robustness2)
_ce(RobustBufferAccess2, robustBufferAccess2)
_ce(RobustImageAccess2, robustImageAccess2)
_ce(NullDescriptor, nullDescriptor)
#undef _ce

_cext(RobustImageAccess, robustImageAccess, imageRobustness, EXT::image_robustness)

#define _ce(value, field) _cext(value, field, rayTracingPipeline, KHR::ray_tracing_pipeline)
_ce(RayTracingPipeline, rayTracingPipeline)
_ce(RayTracingPipelineShaderGroupHandleCaptureReplay, rayTracingPipelineShaderGroupHandleCaptureReplay)
_ce(RayTracingPipelineShaderGroupHandleCaptureReplayMixed, rayTracingPipelineShaderGroupHandleCaptureReplayMixed)
_ce(RayTracingPipelineTraceRaysIndirect, rayTracingPipelineTraceRaysIndirect)
_ce(RayTraversalPrimitiveCulling, rayTraversalPrimitiveCulling)
#undef _ce

_cext(RayQuery, rayQuery, rayQuery, KHR::ray_query)
#endif
