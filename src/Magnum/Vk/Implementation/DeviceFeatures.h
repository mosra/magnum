#ifndef Magnum_Vk_Implementation_DeviceFeatures_h
#define Magnum_Vk_Implementation_DeviceFeatures_h
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

#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk { namespace Implementation {

/* Everything except the top-level VkPhysicalDeviceFeatures2 structure, which
   is treated differently when querying and enabling the features. Used by
   DeviceProperties.cpp and Device.cpp, in addition needs to be kept in sync
   with the list in Device.cpp passed to Implementation::structureDisconnectChain()
   (however there's a test that should catch *all* errors with forgotten or
   wrongly ordered structures there). */
struct DeviceFeatures {
    VkPhysicalDeviceProtectedMemoryFeatures protectedMemory;
    VkPhysicalDeviceMultiviewFeatures multiview;
    VkPhysicalDeviceShaderDrawParametersFeatures shaderDrawParameters;
    VkPhysicalDeviceTextureCompressionASTCHDRFeaturesEXT textureCompressionAstcHdr;
    VkPhysicalDeviceShaderFloat16Int8Features shaderFloat16Int8;
    VkPhysicalDevice16BitStorageFeatures _16BitStorage;
    VkPhysicalDeviceImagelessFramebufferFeatures imagelessFramebuffer;
    VkPhysicalDeviceVariablePointersFeatures variablePointers;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructure;
    VkPhysicalDeviceSamplerYcbcrConversionFeatures samplerYcbcrConversion;
    VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexing;
    VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilitySubset;
    VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures shaderSubgroupExtendedTypes;
    VkPhysicalDevice8BitStorageFeatures _8BitStorage;
    VkPhysicalDeviceShaderAtomicInt64Features shaderAtomicInt64;
    VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT vertexAttributeDivisor;
    VkPhysicalDeviceTimelineSemaphoreFeatures timelineSemaphore;
    VkPhysicalDeviceVulkanMemoryModelFeatures vulkanMemoryModel;
    VkPhysicalDeviceScalarBlockLayoutFeatures scalarBlockLayout;
    VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures separateDepthStencilLayouts;
    VkPhysicalDeviceUniformBufferStandardLayoutFeatures uniformBufferStandardLayout;
    VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddress;
    VkPhysicalDeviceHostQueryResetFeatures hostQueryReset;
    VkPhysicalDeviceIndexTypeUint8FeaturesEXT indexTypeUint8;
    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicState;
    VkPhysicalDeviceRobustness2FeaturesEXT robustness2;
    VkPhysicalDeviceImageRobustnessFeaturesEXT imageRobustness;
    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipeline;
    VkPhysicalDeviceRayQueryFeaturesKHR rayQuery;
};

constexpr Vk::DeviceFeatures deviceFeaturesPortabilitySubset() {
    return
        DeviceFeature::ConstantAlphaColorBlendFactors|
        DeviceFeature::Events|
        DeviceFeature::ImageViewFormatReinterpretation|
        DeviceFeature::ImageViewFormatSwizzle|
        DeviceFeature::ImageView2DOn3DImage|
        DeviceFeature::MultisampleArrayImage|
        DeviceFeature::MutableComparisonSamplers|
        DeviceFeature::PointPolygons|
        DeviceFeature::SamplerMipLodBias|
        DeviceFeature::SeparateStencilMaskRef|
        DeviceFeature::ShaderSampleRateInterpolationFunctions|
        DeviceFeature::TessellationIsolines|
        DeviceFeature::TessellationPointMode|
        DeviceFeature::TriangleFans|
        DeviceFeature::VertexAttributeAccessBeyondStride;
}

}}}

#endif
