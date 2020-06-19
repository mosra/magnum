#ifndef _flextVkGlobal_h_
#define _flextVkGlobal_h_
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

#include "MagnumExternal/Vulkan/flextVk.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT /* So the macro names are preserved in the docs */

/* Per-instance function pointers */
extern FLEXTVK_EXPORT FlextVkInstance flextVkInstance;

/* VK_EXT_debug_marker */


/* VK_EXT_debug_report */

#define vkCreateDebugReportCallbackEXT flextVkInstance.CreateDebugReportCallbackEXT
#define vkDebugReportMessageEXT flextVkInstance.DebugReportMessageEXT
#define vkDestroyDebugReportCallbackEXT flextVkInstance.DestroyDebugReportCallbackEXT

/* VK_EXT_debug_utils */

#define vkCreateDebugUtilsMessengerEXT flextVkInstance.CreateDebugUtilsMessengerEXT
#define vkDestroyDebugUtilsMessengerEXT flextVkInstance.DestroyDebugUtilsMessengerEXT
#define vkSubmitDebugUtilsMessageEXT flextVkInstance.SubmitDebugUtilsMessageEXT

/* VK_EXT_host_query_reset */


/* VK_KHR_bind_memory2 */


/* VK_KHR_buffer_device_address */


/* VK_KHR_create_renderpass2 */


/* VK_KHR_descriptor_update_template */


/* VK_KHR_device_group */


/* VK_KHR_device_group_creation */

#define vkEnumeratePhysicalDeviceGroupsKHR flextVkInstance.EnumeratePhysicalDeviceGroupsKHR

/* VK_KHR_draw_indirect_count */


/* VK_KHR_external_fence_capabilities */

#define vkGetPhysicalDeviceExternalFencePropertiesKHR flextVkInstance.GetPhysicalDeviceExternalFencePropertiesKHR

/* VK_KHR_external_memory_capabilities */

#define vkGetPhysicalDeviceExternalBufferPropertiesKHR flextVkInstance.GetPhysicalDeviceExternalBufferPropertiesKHR

/* VK_KHR_external_semaphore_capabilities */

#define vkGetPhysicalDeviceExternalSemaphorePropertiesKHR flextVkInstance.GetPhysicalDeviceExternalSemaphorePropertiesKHR

/* VK_KHR_get_memory_requirements2 */


/* VK_KHR_get_physical_device_properties2 */

#define vkGetPhysicalDeviceFeatures2KHR flextVkInstance.GetPhysicalDeviceFeatures2KHR
#define vkGetPhysicalDeviceFormatProperties2KHR flextVkInstance.GetPhysicalDeviceFormatProperties2KHR
#define vkGetPhysicalDeviceImageFormatProperties2KHR flextVkInstance.GetPhysicalDeviceImageFormatProperties2KHR
#define vkGetPhysicalDeviceMemoryProperties2KHR flextVkInstance.GetPhysicalDeviceMemoryProperties2KHR
#define vkGetPhysicalDeviceProperties2KHR flextVkInstance.GetPhysicalDeviceProperties2KHR
#define vkGetPhysicalDeviceQueueFamilyProperties2KHR flextVkInstance.GetPhysicalDeviceQueueFamilyProperties2KHR
#define vkGetPhysicalDeviceSparseImageFormatProperties2KHR flextVkInstance.GetPhysicalDeviceSparseImageFormatProperties2KHR

/* VK_KHR_maintenance1 */


/* VK_KHR_maintenance3 */


/* VK_KHR_sampler_ycbcr_conversion */


/* VK_KHR_timeline_semaphore */


/* VK_VERSION_1_0 */

#define vkCreateDevice flextVkInstance.CreateDevice
#define vkDestroyInstance flextVkInstance.DestroyInstance
#define vkEnumerateDeviceExtensionProperties flextVkInstance.EnumerateDeviceExtensionProperties
#define vkEnumeratePhysicalDevices flextVkInstance.EnumeratePhysicalDevices
#define vkGetDeviceProcAddr flextVkInstance.GetDeviceProcAddr
#define vkGetPhysicalDeviceFeatures flextVkInstance.GetPhysicalDeviceFeatures
#define vkGetPhysicalDeviceFormatProperties flextVkInstance.GetPhysicalDeviceFormatProperties
#define vkGetPhysicalDeviceImageFormatProperties flextVkInstance.GetPhysicalDeviceImageFormatProperties
#define vkGetPhysicalDeviceMemoryProperties flextVkInstance.GetPhysicalDeviceMemoryProperties
#define vkGetPhysicalDeviceProperties flextVkInstance.GetPhysicalDeviceProperties
#define vkGetPhysicalDeviceQueueFamilyProperties flextVkInstance.GetPhysicalDeviceQueueFamilyProperties
#define vkGetPhysicalDeviceSparseImageFormatProperties flextVkInstance.GetPhysicalDeviceSparseImageFormatProperties

/* VK_VERSION_1_1 */

#define vkEnumeratePhysicalDeviceGroups flextVkInstance.EnumeratePhysicalDeviceGroups
#define vkGetPhysicalDeviceExternalBufferProperties flextVkInstance.GetPhysicalDeviceExternalBufferProperties
#define vkGetPhysicalDeviceExternalFenceProperties flextVkInstance.GetPhysicalDeviceExternalFenceProperties
#define vkGetPhysicalDeviceExternalSemaphoreProperties flextVkInstance.GetPhysicalDeviceExternalSemaphoreProperties
#define vkGetPhysicalDeviceFeatures2 flextVkInstance.GetPhysicalDeviceFeatures2
#define vkGetPhysicalDeviceFormatProperties2 flextVkInstance.GetPhysicalDeviceFormatProperties2
#define vkGetPhysicalDeviceImageFormatProperties2 flextVkInstance.GetPhysicalDeviceImageFormatProperties2
#define vkGetPhysicalDeviceMemoryProperties2 flextVkInstance.GetPhysicalDeviceMemoryProperties2
#define vkGetPhysicalDeviceProperties2 flextVkInstance.GetPhysicalDeviceProperties2
#define vkGetPhysicalDeviceQueueFamilyProperties2 flextVkInstance.GetPhysicalDeviceQueueFamilyProperties2
#define vkGetPhysicalDeviceSparseImageFormatProperties2 flextVkInstance.GetPhysicalDeviceSparseImageFormatProperties2

/* VK_VERSION_1_2 */


/* Per-device function pointers */
extern FLEXTVK_EXPORT FlextVkDevice flextVkDevice;

/* VK_EXT_debug_marker */

#define vkCmdDebugMarkerBeginEXT flextVkDevice.CmdDebugMarkerBeginEXT
#define vkCmdDebugMarkerEndEXT flextVkDevice.CmdDebugMarkerEndEXT
#define vkCmdDebugMarkerInsertEXT flextVkDevice.CmdDebugMarkerInsertEXT
#define vkDebugMarkerSetObjectNameEXT flextVkDevice.DebugMarkerSetObjectNameEXT
#define vkDebugMarkerSetObjectTagEXT flextVkDevice.DebugMarkerSetObjectTagEXT

/* VK_EXT_debug_report */


/* VK_EXT_debug_utils */

#define vkCmdBeginDebugUtilsLabelEXT flextVkDevice.CmdBeginDebugUtilsLabelEXT
#define vkCmdEndDebugUtilsLabelEXT flextVkDevice.CmdEndDebugUtilsLabelEXT
#define vkCmdInsertDebugUtilsLabelEXT flextVkDevice.CmdInsertDebugUtilsLabelEXT
#define vkQueueBeginDebugUtilsLabelEXT flextVkDevice.QueueBeginDebugUtilsLabelEXT
#define vkQueueEndDebugUtilsLabelEXT flextVkDevice.QueueEndDebugUtilsLabelEXT
#define vkQueueInsertDebugUtilsLabelEXT flextVkDevice.QueueInsertDebugUtilsLabelEXT
#define vkSetDebugUtilsObjectNameEXT flextVkDevice.SetDebugUtilsObjectNameEXT
#define vkSetDebugUtilsObjectTagEXT flextVkDevice.SetDebugUtilsObjectTagEXT

/* VK_EXT_host_query_reset */

#define vkResetQueryPoolEXT flextVkDevice.ResetQueryPoolEXT

/* VK_KHR_bind_memory2 */

#define vkBindBufferMemory2KHR flextVkDevice.BindBufferMemory2KHR
#define vkBindImageMemory2KHR flextVkDevice.BindImageMemory2KHR

/* VK_KHR_buffer_device_address */

#define vkGetBufferDeviceAddressKHR flextVkDevice.GetBufferDeviceAddressKHR
#define vkGetBufferOpaqueCaptureAddressKHR flextVkDevice.GetBufferOpaqueCaptureAddressKHR
#define vkGetDeviceMemoryOpaqueCaptureAddressKHR flextVkDevice.GetDeviceMemoryOpaqueCaptureAddressKHR

/* VK_KHR_create_renderpass2 */

#define vkCmdBeginRenderPass2KHR flextVkDevice.CmdBeginRenderPass2KHR
#define vkCmdEndRenderPass2KHR flextVkDevice.CmdEndRenderPass2KHR
#define vkCmdNextSubpass2KHR flextVkDevice.CmdNextSubpass2KHR
#define vkCreateRenderPass2KHR flextVkDevice.CreateRenderPass2KHR

/* VK_KHR_descriptor_update_template */

#define vkCreateDescriptorUpdateTemplateKHR flextVkDevice.CreateDescriptorUpdateTemplateKHR
#define vkDestroyDescriptorUpdateTemplateKHR flextVkDevice.DestroyDescriptorUpdateTemplateKHR
#define vkUpdateDescriptorSetWithTemplateKHR flextVkDevice.UpdateDescriptorSetWithTemplateKHR

/* VK_KHR_device_group */

#define vkCmdDispatchBaseKHR flextVkDevice.CmdDispatchBaseKHR
#define vkCmdSetDeviceMaskKHR flextVkDevice.CmdSetDeviceMaskKHR
#define vkGetDeviceGroupPeerMemoryFeaturesKHR flextVkDevice.GetDeviceGroupPeerMemoryFeaturesKHR

/* VK_KHR_device_group_creation */


/* VK_KHR_draw_indirect_count */

#define vkCmdDrawIndexedIndirectCountKHR flextVkDevice.CmdDrawIndexedIndirectCountKHR
#define vkCmdDrawIndirectCountKHR flextVkDevice.CmdDrawIndirectCountKHR

/* VK_KHR_external_fence_capabilities */


/* VK_KHR_external_memory_capabilities */


/* VK_KHR_external_semaphore_capabilities */


/* VK_KHR_get_memory_requirements2 */

#define vkGetBufferMemoryRequirements2KHR flextVkDevice.GetBufferMemoryRequirements2KHR
#define vkGetImageMemoryRequirements2KHR flextVkDevice.GetImageMemoryRequirements2KHR
#define vkGetImageSparseMemoryRequirements2KHR flextVkDevice.GetImageSparseMemoryRequirements2KHR

/* VK_KHR_get_physical_device_properties2 */


/* VK_KHR_maintenance1 */

#define vkTrimCommandPoolKHR flextVkDevice.TrimCommandPoolKHR

/* VK_KHR_maintenance3 */

#define vkGetDescriptorSetLayoutSupportKHR flextVkDevice.GetDescriptorSetLayoutSupportKHR

/* VK_KHR_sampler_ycbcr_conversion */

#define vkCreateSamplerYcbcrConversionKHR flextVkDevice.CreateSamplerYcbcrConversionKHR
#define vkDestroySamplerYcbcrConversionKHR flextVkDevice.DestroySamplerYcbcrConversionKHR

/* VK_KHR_timeline_semaphore */

#define vkGetSemaphoreCounterValueKHR flextVkDevice.GetSemaphoreCounterValueKHR
#define vkSignalSemaphoreKHR flextVkDevice.SignalSemaphoreKHR
#define vkWaitSemaphoresKHR flextVkDevice.WaitSemaphoresKHR

/* VK_VERSION_1_0 */

#define vkAllocateCommandBuffers flextVkDevice.AllocateCommandBuffers
#define vkAllocateDescriptorSets flextVkDevice.AllocateDescriptorSets
#define vkAllocateMemory flextVkDevice.AllocateMemory
#define vkBeginCommandBuffer flextVkDevice.BeginCommandBuffer
#define vkBindBufferMemory flextVkDevice.BindBufferMemory
#define vkBindImageMemory flextVkDevice.BindImageMemory
#define vkCmdBeginQuery flextVkDevice.CmdBeginQuery
#define vkCmdBeginRenderPass flextVkDevice.CmdBeginRenderPass
#define vkCmdBindDescriptorSets flextVkDevice.CmdBindDescriptorSets
#define vkCmdBindIndexBuffer flextVkDevice.CmdBindIndexBuffer
#define vkCmdBindPipeline flextVkDevice.CmdBindPipeline
#define vkCmdBindVertexBuffers flextVkDevice.CmdBindVertexBuffers
#define vkCmdBlitImage flextVkDevice.CmdBlitImage
#define vkCmdClearAttachments flextVkDevice.CmdClearAttachments
#define vkCmdClearColorImage flextVkDevice.CmdClearColorImage
#define vkCmdClearDepthStencilImage flextVkDevice.CmdClearDepthStencilImage
#define vkCmdCopyBuffer flextVkDevice.CmdCopyBuffer
#define vkCmdCopyBufferToImage flextVkDevice.CmdCopyBufferToImage
#define vkCmdCopyImage flextVkDevice.CmdCopyImage
#define vkCmdCopyImageToBuffer flextVkDevice.CmdCopyImageToBuffer
#define vkCmdCopyQueryPoolResults flextVkDevice.CmdCopyQueryPoolResults
#define vkCmdDispatch flextVkDevice.CmdDispatch
#define vkCmdDispatchIndirect flextVkDevice.CmdDispatchIndirect
#define vkCmdDraw flextVkDevice.CmdDraw
#define vkCmdDrawIndexed flextVkDevice.CmdDrawIndexed
#define vkCmdDrawIndexedIndirect flextVkDevice.CmdDrawIndexedIndirect
#define vkCmdDrawIndirect flextVkDevice.CmdDrawIndirect
#define vkCmdEndQuery flextVkDevice.CmdEndQuery
#define vkCmdEndRenderPass flextVkDevice.CmdEndRenderPass
#define vkCmdExecuteCommands flextVkDevice.CmdExecuteCommands
#define vkCmdFillBuffer flextVkDevice.CmdFillBuffer
#define vkCmdNextSubpass flextVkDevice.CmdNextSubpass
#define vkCmdPipelineBarrier flextVkDevice.CmdPipelineBarrier
#define vkCmdPushConstants flextVkDevice.CmdPushConstants
#define vkCmdResetEvent flextVkDevice.CmdResetEvent
#define vkCmdResetQueryPool flextVkDevice.CmdResetQueryPool
#define vkCmdResolveImage flextVkDevice.CmdResolveImage
#define vkCmdSetBlendConstants flextVkDevice.CmdSetBlendConstants
#define vkCmdSetDepthBias flextVkDevice.CmdSetDepthBias
#define vkCmdSetDepthBounds flextVkDevice.CmdSetDepthBounds
#define vkCmdSetEvent flextVkDevice.CmdSetEvent
#define vkCmdSetLineWidth flextVkDevice.CmdSetLineWidth
#define vkCmdSetScissor flextVkDevice.CmdSetScissor
#define vkCmdSetStencilCompareMask flextVkDevice.CmdSetStencilCompareMask
#define vkCmdSetStencilReference flextVkDevice.CmdSetStencilReference
#define vkCmdSetStencilWriteMask flextVkDevice.CmdSetStencilWriteMask
#define vkCmdSetViewport flextVkDevice.CmdSetViewport
#define vkCmdUpdateBuffer flextVkDevice.CmdUpdateBuffer
#define vkCmdWaitEvents flextVkDevice.CmdWaitEvents
#define vkCmdWriteTimestamp flextVkDevice.CmdWriteTimestamp
#define vkCreateBuffer flextVkDevice.CreateBuffer
#define vkCreateBufferView flextVkDevice.CreateBufferView
#define vkCreateCommandPool flextVkDevice.CreateCommandPool
#define vkCreateComputePipelines flextVkDevice.CreateComputePipelines
#define vkCreateDescriptorPool flextVkDevice.CreateDescriptorPool
#define vkCreateDescriptorSetLayout flextVkDevice.CreateDescriptorSetLayout
#define vkCreateEvent flextVkDevice.CreateEvent
#define vkCreateFence flextVkDevice.CreateFence
#define vkCreateFramebuffer flextVkDevice.CreateFramebuffer
#define vkCreateGraphicsPipelines flextVkDevice.CreateGraphicsPipelines
#define vkCreateImage flextVkDevice.CreateImage
#define vkCreateImageView flextVkDevice.CreateImageView
#define vkCreatePipelineCache flextVkDevice.CreatePipelineCache
#define vkCreatePipelineLayout flextVkDevice.CreatePipelineLayout
#define vkCreateQueryPool flextVkDevice.CreateQueryPool
#define vkCreateRenderPass flextVkDevice.CreateRenderPass
#define vkCreateSampler flextVkDevice.CreateSampler
#define vkCreateSemaphore flextVkDevice.CreateSemaphore
#define vkCreateShaderModule flextVkDevice.CreateShaderModule
#define vkDestroyBuffer flextVkDevice.DestroyBuffer
#define vkDestroyBufferView flextVkDevice.DestroyBufferView
#define vkDestroyCommandPool flextVkDevice.DestroyCommandPool
#define vkDestroyDescriptorPool flextVkDevice.DestroyDescriptorPool
#define vkDestroyDescriptorSetLayout flextVkDevice.DestroyDescriptorSetLayout
#define vkDestroyDevice flextVkDevice.DestroyDevice
#define vkDestroyEvent flextVkDevice.DestroyEvent
#define vkDestroyFence flextVkDevice.DestroyFence
#define vkDestroyFramebuffer flextVkDevice.DestroyFramebuffer
#define vkDestroyImage flextVkDevice.DestroyImage
#define vkDestroyImageView flextVkDevice.DestroyImageView
#define vkDestroyPipeline flextVkDevice.DestroyPipeline
#define vkDestroyPipelineCache flextVkDevice.DestroyPipelineCache
#define vkDestroyPipelineLayout flextVkDevice.DestroyPipelineLayout
#define vkDestroyQueryPool flextVkDevice.DestroyQueryPool
#define vkDestroyRenderPass flextVkDevice.DestroyRenderPass
#define vkDestroySampler flextVkDevice.DestroySampler
#define vkDestroySemaphore flextVkDevice.DestroySemaphore
#define vkDestroyShaderModule flextVkDevice.DestroyShaderModule
#define vkDeviceWaitIdle flextVkDevice.DeviceWaitIdle
#define vkEndCommandBuffer flextVkDevice.EndCommandBuffer
#define vkFlushMappedMemoryRanges flextVkDevice.FlushMappedMemoryRanges
#define vkFreeCommandBuffers flextVkDevice.FreeCommandBuffers
#define vkFreeDescriptorSets flextVkDevice.FreeDescriptorSets
#define vkFreeMemory flextVkDevice.FreeMemory
#define vkGetBufferMemoryRequirements flextVkDevice.GetBufferMemoryRequirements
#define vkGetDeviceMemoryCommitment flextVkDevice.GetDeviceMemoryCommitment
#define vkGetDeviceQueue flextVkDevice.GetDeviceQueue
#define vkGetEventStatus flextVkDevice.GetEventStatus
#define vkGetFenceStatus flextVkDevice.GetFenceStatus
#define vkGetImageMemoryRequirements flextVkDevice.GetImageMemoryRequirements
#define vkGetImageSparseMemoryRequirements flextVkDevice.GetImageSparseMemoryRequirements
#define vkGetImageSubresourceLayout flextVkDevice.GetImageSubresourceLayout
#define vkGetPipelineCacheData flextVkDevice.GetPipelineCacheData
#define vkGetQueryPoolResults flextVkDevice.GetQueryPoolResults
#define vkGetRenderAreaGranularity flextVkDevice.GetRenderAreaGranularity
#define vkInvalidateMappedMemoryRanges flextVkDevice.InvalidateMappedMemoryRanges
#define vkMapMemory flextVkDevice.MapMemory
#define vkMergePipelineCaches flextVkDevice.MergePipelineCaches
#define vkQueueBindSparse flextVkDevice.QueueBindSparse
#define vkQueueSubmit flextVkDevice.QueueSubmit
#define vkQueueWaitIdle flextVkDevice.QueueWaitIdle
#define vkResetCommandBuffer flextVkDevice.ResetCommandBuffer
#define vkResetCommandPool flextVkDevice.ResetCommandPool
#define vkResetDescriptorPool flextVkDevice.ResetDescriptorPool
#define vkResetEvent flextVkDevice.ResetEvent
#define vkResetFences flextVkDevice.ResetFences
#define vkSetEvent flextVkDevice.SetEvent
#define vkUnmapMemory flextVkDevice.UnmapMemory
#define vkUpdateDescriptorSets flextVkDevice.UpdateDescriptorSets
#define vkWaitForFences flextVkDevice.WaitForFences

/* VK_VERSION_1_1 */

#define vkBindBufferMemory2 flextVkDevice.BindBufferMemory2
#define vkBindImageMemory2 flextVkDevice.BindImageMemory2
#define vkCmdDispatchBase flextVkDevice.CmdDispatchBase
#define vkCmdSetDeviceMask flextVkDevice.CmdSetDeviceMask
#define vkCreateDescriptorUpdateTemplate flextVkDevice.CreateDescriptorUpdateTemplate
#define vkCreateSamplerYcbcrConversion flextVkDevice.CreateSamplerYcbcrConversion
#define vkDestroyDescriptorUpdateTemplate flextVkDevice.DestroyDescriptorUpdateTemplate
#define vkDestroySamplerYcbcrConversion flextVkDevice.DestroySamplerYcbcrConversion
#define vkGetBufferMemoryRequirements2 flextVkDevice.GetBufferMemoryRequirements2
#define vkGetDescriptorSetLayoutSupport flextVkDevice.GetDescriptorSetLayoutSupport
#define vkGetDeviceGroupPeerMemoryFeatures flextVkDevice.GetDeviceGroupPeerMemoryFeatures
#define vkGetDeviceQueue2 flextVkDevice.GetDeviceQueue2
#define vkGetImageMemoryRequirements2 flextVkDevice.GetImageMemoryRequirements2
#define vkGetImageSparseMemoryRequirements2 flextVkDevice.GetImageSparseMemoryRequirements2
#define vkTrimCommandPool flextVkDevice.TrimCommandPool
#define vkUpdateDescriptorSetWithTemplate flextVkDevice.UpdateDescriptorSetWithTemplate

/* VK_VERSION_1_2 */

#define vkCmdBeginRenderPass2 flextVkDevice.CmdBeginRenderPass2
#define vkCmdDrawIndexedIndirectCount flextVkDevice.CmdDrawIndexedIndirectCount
#define vkCmdDrawIndirectCount flextVkDevice.CmdDrawIndirectCount
#define vkCmdEndRenderPass2 flextVkDevice.CmdEndRenderPass2
#define vkCmdNextSubpass2 flextVkDevice.CmdNextSubpass2
#define vkCreateRenderPass2 flextVkDevice.CreateRenderPass2
#define vkGetBufferDeviceAddress flextVkDevice.GetBufferDeviceAddress
#define vkGetBufferOpaqueCaptureAddress flextVkDevice.GetBufferOpaqueCaptureAddress
#define vkGetDeviceMemoryOpaqueCaptureAddress flextVkDevice.GetDeviceMemoryOpaqueCaptureAddress
#define vkGetSemaphoreCounterValue flextVkDevice.GetSemaphoreCounterValue
#define vkResetQueryPool flextVkDevice.ResetQueryPool
#define vkSignalSemaphore flextVkDevice.SignalSemaphore
#define vkWaitSemaphores flextVkDevice.WaitSemaphores

#endif

#ifdef __cplusplus
}
#endif

#endif
