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

#include "flextVk.h"
#include "flextVkGlobal.h"

VkResult(VKAPI_PTR *flextvkEnumerateInstanceVersion)(uint32_t*) = reinterpret_cast<VkResult(VKAPI_PTR*)(uint32_t*)>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));

FlextVkInstance flextVkInstance{};

FlextVkDevice flextVkDevice{};

void flextVkInitInstance(VkInstance instance, FlextVkInstance* data) {
    data->CreateDebugReportCallbackEXT = reinterpret_cast<VkResult(VKAPI_PTR*)(VkInstance, const VkDebugReportCallbackCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugReportCallbackEXT*)>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
    data->DebugReportMessageEXT = reinterpret_cast<void(VKAPI_PTR*)(VkInstance, VkDebugReportFlagsEXT, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char*, const char*)>(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));
    data->DestroyDebugReportCallbackEXT = reinterpret_cast<void(VKAPI_PTR*)(VkInstance, VkDebugReportCallbackEXT, const VkAllocationCallbacks*)>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
    data->CreateDebugUtilsMessengerEXT = reinterpret_cast<VkResult(VKAPI_PTR*)(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*)>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    data->DestroyDebugUtilsMessengerEXT = reinterpret_cast<void(VKAPI_PTR*)(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*)>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    data->SubmitDebugUtilsMessageEXT = reinterpret_cast<void(VKAPI_PTR*)(VkInstance, VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*)>(vkGetInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT"));
    data->EnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<VkResult(VKAPI_PTR*)(VkInstance, uint32_t*, VkPhysicalDeviceGroupProperties*)>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceGroupsKHR"));
    data->GetPhysicalDeviceExternalFencePropertiesKHR = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, const VkPhysicalDeviceExternalFenceInfo*, VkExternalFenceProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFencePropertiesKHR"));
    data->GetPhysicalDeviceExternalBufferPropertiesKHR = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, const VkPhysicalDeviceExternalBufferInfo*, VkExternalBufferProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferPropertiesKHR"));
    data->GetPhysicalDeviceExternalSemaphorePropertiesKHR = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo*, VkExternalSemaphoreProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR"));
    data->GetPhysicalDeviceFeatures2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkPhysicalDeviceFeatures2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2KHR"));
    data->GetPhysicalDeviceFormatProperties2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkFormat, VkFormatProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2KHR"));
    data->GetPhysicalDeviceImageFormatProperties2KHR = reinterpret_cast<VkResult(VKAPI_PTR*)(VkPhysicalDevice, const VkPhysicalDeviceImageFormatInfo2*, VkImageFormatProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2KHR"));
    data->GetPhysicalDeviceMemoryProperties2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkPhysicalDeviceMemoryProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2KHR"));
    data->GetPhysicalDeviceProperties2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkPhysicalDeviceProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR"));
    data->GetPhysicalDeviceQueueFamilyProperties2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, uint32_t*, VkQueueFamilyProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR"));
    data->GetPhysicalDeviceSparseImageFormatProperties2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2*, uint32_t*, VkSparseImageFormatProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR"));
    data->CreateDevice = reinterpret_cast<VkResult(VKAPI_PTR*)(VkPhysicalDevice, const VkDeviceCreateInfo*, const VkAllocationCallbacks*, VkDevice*)>(vkGetInstanceProcAddr(instance, "vkCreateDevice"));
    data->DestroyInstance = reinterpret_cast<void(VKAPI_PTR*)(VkInstance, const VkAllocationCallbacks*)>(vkGetInstanceProcAddr(instance, "vkDestroyInstance"));
    data->EnumerateDeviceExtensionProperties = reinterpret_cast<VkResult(VKAPI_PTR*)(VkPhysicalDevice, const char*, uint32_t*, VkExtensionProperties*)>(vkGetInstanceProcAddr(instance, "vkEnumerateDeviceExtensionProperties"));
    data->EnumeratePhysicalDevices = reinterpret_cast<VkResult(VKAPI_PTR*)(VkInstance, uint32_t*, VkPhysicalDevice*)>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
    data->GetDeviceProcAddr = reinterpret_cast<PFN_vkVoidFunction(VKAPI_PTR*)(VkDevice, const char*)>(vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr"));
    data->GetPhysicalDeviceFeatures = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkPhysicalDeviceFeatures*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures"));
    data->GetPhysicalDeviceFormatProperties = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkFormat, VkFormatProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties"));
    data->GetPhysicalDeviceImageFormatProperties = reinterpret_cast<VkResult(VKAPI_PTR*)(VkPhysicalDevice, VkFormat, VkImageType, VkImageTiling, VkImageUsageFlags, VkImageCreateFlags, VkImageFormatProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties"));
    data->GetPhysicalDeviceMemoryProperties = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkPhysicalDeviceMemoryProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties"));
    data->GetPhysicalDeviceProperties = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkPhysicalDeviceProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties"));
    data->GetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, uint32_t*, VkQueueFamilyProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
    data->GetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkFormat, VkImageType, VkSampleCountFlagBits, VkImageUsageFlags, VkImageTiling, uint32_t*, VkSparseImageFormatProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties"));
    data->EnumeratePhysicalDeviceGroups = reinterpret_cast<VkResult(VKAPI_PTR*)(VkInstance, uint32_t*, VkPhysicalDeviceGroupProperties*)>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceGroups"));
    data->GetPhysicalDeviceExternalBufferProperties = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, const VkPhysicalDeviceExternalBufferInfo*, VkExternalBufferProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferProperties"));
    data->GetPhysicalDeviceExternalFenceProperties = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, const VkPhysicalDeviceExternalFenceInfo*, VkExternalFenceProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFenceProperties"));
    data->GetPhysicalDeviceExternalSemaphoreProperties = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo*, VkExternalSemaphoreProperties*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphoreProperties"));
    data->GetPhysicalDeviceFeatures2 = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkPhysicalDeviceFeatures2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2"));
    data->GetPhysicalDeviceFormatProperties2 = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkFormat, VkFormatProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2"));
    data->GetPhysicalDeviceImageFormatProperties2 = reinterpret_cast<VkResult(VKAPI_PTR*)(VkPhysicalDevice, const VkPhysicalDeviceImageFormatInfo2*, VkImageFormatProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2"));
    data->GetPhysicalDeviceMemoryProperties2 = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkPhysicalDeviceMemoryProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2"));
    data->GetPhysicalDeviceProperties2 = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, VkPhysicalDeviceProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2"));
    data->GetPhysicalDeviceQueueFamilyProperties2 = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, uint32_t*, VkQueueFamilyProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2"));
    data->GetPhysicalDeviceSparseImageFormatProperties2 = reinterpret_cast<void(VKAPI_PTR*)(VkPhysicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2*, uint32_t*, VkSparseImageFormatProperties2*)>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2"));
}

void flextVkInitDevice(VkDevice device, FlextVkDevice* data, PFN_vkVoidFunction(VKAPI_PTR *getDeviceProcAddr)(VkDevice, const char*)) {
    data->CmdDebugMarkerBeginEXT = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkDebugMarkerMarkerInfoEXT*)>(getDeviceProcAddr(device, "vkCmdDebugMarkerBeginEXT"));
    data->CmdDebugMarkerEndEXT = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer)>(getDeviceProcAddr(device, "vkCmdDebugMarkerEndEXT"));
    data->CmdDebugMarkerInsertEXT = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkDebugMarkerMarkerInfoEXT*)>(getDeviceProcAddr(device, "vkCmdDebugMarkerInsertEXT"));
    data->DebugMarkerSetObjectNameEXT = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkDebugMarkerObjectNameInfoEXT*)>(getDeviceProcAddr(device, "vkDebugMarkerSetObjectNameEXT"));
    data->DebugMarkerSetObjectTagEXT = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkDebugMarkerObjectTagInfoEXT*)>(getDeviceProcAddr(device, "vkDebugMarkerSetObjectTagEXT"));
    data->CmdBeginDebugUtilsLabelEXT = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkDebugUtilsLabelEXT*)>(getDeviceProcAddr(device, "vkCmdBeginDebugUtilsLabelEXT"));
    data->CmdEndDebugUtilsLabelEXT = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer)>(getDeviceProcAddr(device, "vkCmdEndDebugUtilsLabelEXT"));
    data->CmdInsertDebugUtilsLabelEXT = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkDebugUtilsLabelEXT*)>(getDeviceProcAddr(device, "vkCmdInsertDebugUtilsLabelEXT"));
    data->QueueBeginDebugUtilsLabelEXT = reinterpret_cast<void(VKAPI_PTR*)(VkQueue, const VkDebugUtilsLabelEXT*)>(getDeviceProcAddr(device, "vkQueueBeginDebugUtilsLabelEXT"));
    data->QueueEndDebugUtilsLabelEXT = reinterpret_cast<void(VKAPI_PTR*)(VkQueue)>(getDeviceProcAddr(device, "vkQueueEndDebugUtilsLabelEXT"));
    data->QueueInsertDebugUtilsLabelEXT = reinterpret_cast<void(VKAPI_PTR*)(VkQueue, const VkDebugUtilsLabelEXT*)>(getDeviceProcAddr(device, "vkQueueInsertDebugUtilsLabelEXT"));
    data->SetDebugUtilsObjectNameEXT = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkDebugUtilsObjectNameInfoEXT*)>(getDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT"));
    data->SetDebugUtilsObjectTagEXT = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkDebugUtilsObjectTagInfoEXT*)>(getDeviceProcAddr(device, "vkSetDebugUtilsObjectTagEXT"));
    data->ResetQueryPoolEXT = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkQueryPool, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkResetQueryPoolEXT"));
    data->BindBufferMemory2KHR = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, uint32_t, const VkBindBufferMemoryInfo*)>(getDeviceProcAddr(device, "vkBindBufferMemory2KHR"));
    data->BindImageMemory2KHR = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, uint32_t, const VkBindImageMemoryInfo*)>(getDeviceProcAddr(device, "vkBindImageMemory2KHR"));
    data->GetBufferDeviceAddressKHR = reinterpret_cast<VkDeviceAddress(VKAPI_PTR*)(VkDevice, const VkBufferDeviceAddressInfo*)>(getDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR"));
    data->GetBufferOpaqueCaptureAddressKHR = reinterpret_cast<uint64_t(VKAPI_PTR*)(VkDevice, const VkBufferDeviceAddressInfo*)>(getDeviceProcAddr(device, "vkGetBufferOpaqueCaptureAddressKHR"));
    data->GetDeviceMemoryOpaqueCaptureAddressKHR = reinterpret_cast<uint64_t(VKAPI_PTR*)(VkDevice, const VkDeviceMemoryOpaqueCaptureAddressInfo*)>(getDeviceProcAddr(device, "vkGetDeviceMemoryOpaqueCaptureAddressKHR"));
    data->CmdBeginRenderPass2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkRenderPassBeginInfo*, const VkSubpassBeginInfo*)>(getDeviceProcAddr(device, "vkCmdBeginRenderPass2KHR"));
    data->CmdEndRenderPass2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkSubpassEndInfo*)>(getDeviceProcAddr(device, "vkCmdEndRenderPass2KHR"));
    data->CmdNextSubpass2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkSubpassBeginInfo*, const VkSubpassEndInfo*)>(getDeviceProcAddr(device, "vkCmdNextSubpass2KHR"));
    data->CreateRenderPass2KHR = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkRenderPassCreateInfo2*, const VkAllocationCallbacks*, VkRenderPass*)>(getDeviceProcAddr(device, "vkCreateRenderPass2KHR"));
    data->CreateDescriptorUpdateTemplateKHR = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkDescriptorUpdateTemplateCreateInfo*, const VkAllocationCallbacks*, VkDescriptorUpdateTemplate*)>(getDeviceProcAddr(device, "vkCreateDescriptorUpdateTemplateKHR"));
    data->DestroyDescriptorUpdateTemplateKHR = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkDescriptorUpdateTemplate, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyDescriptorUpdateTemplateKHR"));
    data->UpdateDescriptorSetWithTemplateKHR = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkDescriptorSet, VkDescriptorUpdateTemplate, const void*)>(getDeviceProcAddr(device, "vkUpdateDescriptorSetWithTemplateKHR"));
    data->CmdDispatchBaseKHR = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDispatchBaseKHR"));
    data->CmdSetDeviceMaskKHR = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t)>(getDeviceProcAddr(device, "vkCmdSetDeviceMaskKHR"));
    data->GetDeviceGroupPeerMemoryFeaturesKHR = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, uint32_t, uint32_t, uint32_t, VkPeerMemoryFeatureFlags*)>(getDeviceProcAddr(device, "vkGetDeviceGroupPeerMemoryFeaturesKHR"));
    data->CmdDrawIndexedIndirectCountKHR = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkDeviceSize, VkBuffer, VkDeviceSize, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDrawIndexedIndirectCountKHR"));
    data->CmdDrawIndirectCountKHR = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkDeviceSize, VkBuffer, VkDeviceSize, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDrawIndirectCountKHR"));
    data->GetBufferMemoryRequirements2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, const VkBufferMemoryRequirementsInfo2*, VkMemoryRequirements2*)>(getDeviceProcAddr(device, "vkGetBufferMemoryRequirements2KHR"));
    data->GetImageMemoryRequirements2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, const VkImageMemoryRequirementsInfo2*, VkMemoryRequirements2*)>(getDeviceProcAddr(device, "vkGetImageMemoryRequirements2KHR"));
    data->GetImageSparseMemoryRequirements2KHR = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, const VkImageSparseMemoryRequirementsInfo2*, uint32_t*, VkSparseImageMemoryRequirements2*)>(getDeviceProcAddr(device, "vkGetImageSparseMemoryRequirements2KHR"));
    data->TrimCommandPoolKHR = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkCommandPool, VkCommandPoolTrimFlags)>(getDeviceProcAddr(device, "vkTrimCommandPoolKHR"));
    data->GetDescriptorSetLayoutSupportKHR = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, const VkDescriptorSetLayoutCreateInfo*, VkDescriptorSetLayoutSupport*)>(getDeviceProcAddr(device, "vkGetDescriptorSetLayoutSupportKHR"));
    data->CreateSamplerYcbcrConversionKHR = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkSamplerYcbcrConversionCreateInfo*, const VkAllocationCallbacks*, VkSamplerYcbcrConversion*)>(getDeviceProcAddr(device, "vkCreateSamplerYcbcrConversionKHR"));
    data->DestroySamplerYcbcrConversionKHR = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkSamplerYcbcrConversion, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroySamplerYcbcrConversionKHR"));
    data->GetSemaphoreCounterValueKHR = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkSemaphore, uint64_t*)>(getDeviceProcAddr(device, "vkGetSemaphoreCounterValueKHR"));
    data->SignalSemaphoreKHR = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkSemaphoreSignalInfo*)>(getDeviceProcAddr(device, "vkSignalSemaphoreKHR"));
    data->WaitSemaphoresKHR = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkSemaphoreWaitInfo*, uint64_t)>(getDeviceProcAddr(device, "vkWaitSemaphoresKHR"));
    data->AllocateCommandBuffers = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkCommandBufferAllocateInfo*, VkCommandBuffer*)>(getDeviceProcAddr(device, "vkAllocateCommandBuffers"));
    data->AllocateDescriptorSets = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkDescriptorSetAllocateInfo*, VkDescriptorSet*)>(getDeviceProcAddr(device, "vkAllocateDescriptorSets"));
    data->AllocateMemory = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkMemoryAllocateInfo*, const VkAllocationCallbacks*, VkDeviceMemory*)>(getDeviceProcAddr(device, "vkAllocateMemory"));
    data->BeginCommandBuffer = reinterpret_cast<VkResult(VKAPI_PTR*)(VkCommandBuffer, const VkCommandBufferBeginInfo*)>(getDeviceProcAddr(device, "vkBeginCommandBuffer"));
    data->BindBufferMemory = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkBuffer, VkDeviceMemory, VkDeviceSize)>(getDeviceProcAddr(device, "vkBindBufferMemory"));
    data->BindImageMemory = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkImage, VkDeviceMemory, VkDeviceSize)>(getDeviceProcAddr(device, "vkBindImageMemory"));
    data->CmdBeginQuery = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkQueryPool, uint32_t, VkQueryControlFlags)>(getDeviceProcAddr(device, "vkCmdBeginQuery"));
    data->CmdBeginRenderPass = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkRenderPassBeginInfo*, VkSubpassContents)>(getDeviceProcAddr(device, "vkCmdBeginRenderPass"));
    data->CmdBindDescriptorSets = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkPipelineBindPoint, VkPipelineLayout, uint32_t, uint32_t, const VkDescriptorSet*, uint32_t, const uint32_t*)>(getDeviceProcAddr(device, "vkCmdBindDescriptorSets"));
    data->CmdBindIndexBuffer = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkDeviceSize, VkIndexType)>(getDeviceProcAddr(device, "vkCmdBindIndexBuffer"));
    data->CmdBindPipeline = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkPipelineBindPoint, VkPipeline)>(getDeviceProcAddr(device, "vkCmdBindPipeline"));
    data->CmdBindVertexBuffers = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, uint32_t, const VkBuffer*, const VkDeviceSize*)>(getDeviceProcAddr(device, "vkCmdBindVertexBuffers"));
    data->CmdBlitImage = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkImage, VkImageLayout, VkImage, VkImageLayout, uint32_t, const VkImageBlit*, VkFilter)>(getDeviceProcAddr(device, "vkCmdBlitImage"));
    data->CmdClearAttachments = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, const VkClearAttachment*, uint32_t, const VkClearRect*)>(getDeviceProcAddr(device, "vkCmdClearAttachments"));
    data->CmdClearColorImage = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkImage, VkImageLayout, const VkClearColorValue*, uint32_t, const VkImageSubresourceRange*)>(getDeviceProcAddr(device, "vkCmdClearColorImage"));
    data->CmdClearDepthStencilImage = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkImage, VkImageLayout, const VkClearDepthStencilValue*, uint32_t, const VkImageSubresourceRange*)>(getDeviceProcAddr(device, "vkCmdClearDepthStencilImage"));
    data->CmdCopyBuffer = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkBuffer, uint32_t, const VkBufferCopy*)>(getDeviceProcAddr(device, "vkCmdCopyBuffer"));
    data->CmdCopyBufferToImage = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkImage, VkImageLayout, uint32_t, const VkBufferImageCopy*)>(getDeviceProcAddr(device, "vkCmdCopyBufferToImage"));
    data->CmdCopyImage = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkImage, VkImageLayout, VkImage, VkImageLayout, uint32_t, const VkImageCopy*)>(getDeviceProcAddr(device, "vkCmdCopyImage"));
    data->CmdCopyImageToBuffer = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkImage, VkImageLayout, VkBuffer, uint32_t, const VkBufferImageCopy*)>(getDeviceProcAddr(device, "vkCmdCopyImageToBuffer"));
    data->CmdCopyQueryPoolResults = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkQueryPool, uint32_t, uint32_t, VkBuffer, VkDeviceSize, VkDeviceSize, VkQueryResultFlags)>(getDeviceProcAddr(device, "vkCmdCopyQueryPoolResults"));
    data->CmdDispatch = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDispatch"));
    data->CmdDispatchIndirect = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkDeviceSize)>(getDeviceProcAddr(device, "vkCmdDispatchIndirect"));
    data->CmdDraw = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, uint32_t, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDraw"));
    data->CmdDrawIndexed = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, uint32_t, uint32_t, int32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDrawIndexed"));
    data->CmdDrawIndexedIndirect = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkDeviceSize, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDrawIndexedIndirect"));
    data->CmdDrawIndirect = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkDeviceSize, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDrawIndirect"));
    data->CmdEndQuery = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkQueryPool, uint32_t)>(getDeviceProcAddr(device, "vkCmdEndQuery"));
    data->CmdEndRenderPass = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer)>(getDeviceProcAddr(device, "vkCmdEndRenderPass"));
    data->CmdExecuteCommands = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, const VkCommandBuffer*)>(getDeviceProcAddr(device, "vkCmdExecuteCommands"));
    data->CmdFillBuffer = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkDeviceSize, VkDeviceSize, uint32_t)>(getDeviceProcAddr(device, "vkCmdFillBuffer"));
    data->CmdNextSubpass = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkSubpassContents)>(getDeviceProcAddr(device, "vkCmdNextSubpass"));
    data->CmdPipelineBarrier = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkPipelineStageFlags, VkPipelineStageFlags, VkDependencyFlags, uint32_t, const VkMemoryBarrier*, uint32_t, const VkBufferMemoryBarrier*, uint32_t, const VkImageMemoryBarrier*)>(getDeviceProcAddr(device, "vkCmdPipelineBarrier"));
    data->CmdPushConstants = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkPipelineLayout, VkShaderStageFlags, uint32_t, uint32_t, const void*)>(getDeviceProcAddr(device, "vkCmdPushConstants"));
    data->CmdResetEvent = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkEvent, VkPipelineStageFlags)>(getDeviceProcAddr(device, "vkCmdResetEvent"));
    data->CmdResetQueryPool = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkQueryPool, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdResetQueryPool"));
    data->CmdResolveImage = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkImage, VkImageLayout, VkImage, VkImageLayout, uint32_t, const VkImageResolve*)>(getDeviceProcAddr(device, "vkCmdResolveImage"));
    data->CmdSetBlendConstants = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const float [4])>(getDeviceProcAddr(device, "vkCmdSetBlendConstants"));
    data->CmdSetDepthBias = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, float, float, float)>(getDeviceProcAddr(device, "vkCmdSetDepthBias"));
    data->CmdSetDepthBounds = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, float, float)>(getDeviceProcAddr(device, "vkCmdSetDepthBounds"));
    data->CmdSetEvent = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkEvent, VkPipelineStageFlags)>(getDeviceProcAddr(device, "vkCmdSetEvent"));
    data->CmdSetLineWidth = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, float)>(getDeviceProcAddr(device, "vkCmdSetLineWidth"));
    data->CmdSetScissor = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, uint32_t, const VkRect2D*)>(getDeviceProcAddr(device, "vkCmdSetScissor"));
    data->CmdSetStencilCompareMask = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkStencilFaceFlags, uint32_t)>(getDeviceProcAddr(device, "vkCmdSetStencilCompareMask"));
    data->CmdSetStencilReference = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkStencilFaceFlags, uint32_t)>(getDeviceProcAddr(device, "vkCmdSetStencilReference"));
    data->CmdSetStencilWriteMask = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkStencilFaceFlags, uint32_t)>(getDeviceProcAddr(device, "vkCmdSetStencilWriteMask"));
    data->CmdSetViewport = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, uint32_t, const VkViewport*)>(getDeviceProcAddr(device, "vkCmdSetViewport"));
    data->CmdUpdateBuffer = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkDeviceSize, VkDeviceSize, const void*)>(getDeviceProcAddr(device, "vkCmdUpdateBuffer"));
    data->CmdWaitEvents = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, const VkEvent*, VkPipelineStageFlags, VkPipelineStageFlags, uint32_t, const VkMemoryBarrier*, uint32_t, const VkBufferMemoryBarrier*, uint32_t, const VkImageMemoryBarrier*)>(getDeviceProcAddr(device, "vkCmdWaitEvents"));
    data->CmdWriteTimestamp = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkPipelineStageFlagBits, VkQueryPool, uint32_t)>(getDeviceProcAddr(device, "vkCmdWriteTimestamp"));
    data->CreateBuffer = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkBufferCreateInfo*, const VkAllocationCallbacks*, VkBuffer*)>(getDeviceProcAddr(device, "vkCreateBuffer"));
    data->CreateBufferView = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkBufferViewCreateInfo*, const VkAllocationCallbacks*, VkBufferView*)>(getDeviceProcAddr(device, "vkCreateBufferView"));
    data->CreateCommandPool = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkCommandPoolCreateInfo*, const VkAllocationCallbacks*, VkCommandPool*)>(getDeviceProcAddr(device, "vkCreateCommandPool"));
    data->CreateComputePipelines = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkPipelineCache, uint32_t, const VkComputePipelineCreateInfo*, const VkAllocationCallbacks*, VkPipeline*)>(getDeviceProcAddr(device, "vkCreateComputePipelines"));
    data->CreateDescriptorPool = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkDescriptorPoolCreateInfo*, const VkAllocationCallbacks*, VkDescriptorPool*)>(getDeviceProcAddr(device, "vkCreateDescriptorPool"));
    data->CreateDescriptorSetLayout = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkDescriptorSetLayoutCreateInfo*, const VkAllocationCallbacks*, VkDescriptorSetLayout*)>(getDeviceProcAddr(device, "vkCreateDescriptorSetLayout"));
    data->CreateEvent = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkEventCreateInfo*, const VkAllocationCallbacks*, VkEvent*)>(getDeviceProcAddr(device, "vkCreateEvent"));
    data->CreateFence = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkFenceCreateInfo*, const VkAllocationCallbacks*, VkFence*)>(getDeviceProcAddr(device, "vkCreateFence"));
    data->CreateFramebuffer = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkFramebufferCreateInfo*, const VkAllocationCallbacks*, VkFramebuffer*)>(getDeviceProcAddr(device, "vkCreateFramebuffer"));
    data->CreateGraphicsPipelines = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkPipelineCache, uint32_t, const VkGraphicsPipelineCreateInfo*, const VkAllocationCallbacks*, VkPipeline*)>(getDeviceProcAddr(device, "vkCreateGraphicsPipelines"));
    data->CreateImage = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkImageCreateInfo*, const VkAllocationCallbacks*, VkImage*)>(getDeviceProcAddr(device, "vkCreateImage"));
    data->CreateImageView = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkImageViewCreateInfo*, const VkAllocationCallbacks*, VkImageView*)>(getDeviceProcAddr(device, "vkCreateImageView"));
    data->CreatePipelineCache = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkPipelineCacheCreateInfo*, const VkAllocationCallbacks*, VkPipelineCache*)>(getDeviceProcAddr(device, "vkCreatePipelineCache"));
    data->CreatePipelineLayout = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkPipelineLayoutCreateInfo*, const VkAllocationCallbacks*, VkPipelineLayout*)>(getDeviceProcAddr(device, "vkCreatePipelineLayout"));
    data->CreateQueryPool = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkQueryPoolCreateInfo*, const VkAllocationCallbacks*, VkQueryPool*)>(getDeviceProcAddr(device, "vkCreateQueryPool"));
    data->CreateRenderPass = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkRenderPassCreateInfo*, const VkAllocationCallbacks*, VkRenderPass*)>(getDeviceProcAddr(device, "vkCreateRenderPass"));
    data->CreateSampler = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkSamplerCreateInfo*, const VkAllocationCallbacks*, VkSampler*)>(getDeviceProcAddr(device, "vkCreateSampler"));
    data->CreateSemaphore = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkSemaphoreCreateInfo*, const VkAllocationCallbacks*, VkSemaphore*)>(getDeviceProcAddr(device, "vkCreateSemaphore"));
    data->CreateShaderModule = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkShaderModuleCreateInfo*, const VkAllocationCallbacks*, VkShaderModule*)>(getDeviceProcAddr(device, "vkCreateShaderModule"));
    data->DestroyBuffer = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkBuffer, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyBuffer"));
    data->DestroyBufferView = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkBufferView, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyBufferView"));
    data->DestroyCommandPool = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkCommandPool, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyCommandPool"));
    data->DestroyDescriptorPool = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkDescriptorPool, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyDescriptorPool"));
    data->DestroyDescriptorSetLayout = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkDescriptorSetLayout, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyDescriptorSetLayout"));
    data->DestroyDevice = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyDevice"));
    data->DestroyEvent = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkEvent, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyEvent"));
    data->DestroyFence = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkFence, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyFence"));
    data->DestroyFramebuffer = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkFramebuffer, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyFramebuffer"));
    data->DestroyImage = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkImage, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyImage"));
    data->DestroyImageView = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkImageView, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyImageView"));
    data->DestroyPipeline = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkPipeline, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyPipeline"));
    data->DestroyPipelineCache = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkPipelineCache, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyPipelineCache"));
    data->DestroyPipelineLayout = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkPipelineLayout, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyPipelineLayout"));
    data->DestroyQueryPool = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkQueryPool, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyQueryPool"));
    data->DestroyRenderPass = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkRenderPass, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyRenderPass"));
    data->DestroySampler = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkSampler, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroySampler"));
    data->DestroySemaphore = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkSemaphore, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroySemaphore"));
    data->DestroyShaderModule = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkShaderModule, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyShaderModule"));
    data->DeviceWaitIdle = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice)>(getDeviceProcAddr(device, "vkDeviceWaitIdle"));
    data->EndCommandBuffer = reinterpret_cast<VkResult(VKAPI_PTR*)(VkCommandBuffer)>(getDeviceProcAddr(device, "vkEndCommandBuffer"));
    data->FlushMappedMemoryRanges = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, uint32_t, const VkMappedMemoryRange*)>(getDeviceProcAddr(device, "vkFlushMappedMemoryRanges"));
    data->FreeCommandBuffers = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkCommandPool, uint32_t, const VkCommandBuffer*)>(getDeviceProcAddr(device, "vkFreeCommandBuffers"));
    data->FreeDescriptorSets = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkDescriptorPool, uint32_t, const VkDescriptorSet*)>(getDeviceProcAddr(device, "vkFreeDescriptorSets"));
    data->FreeMemory = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkDeviceMemory, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkFreeMemory"));
    data->GetBufferMemoryRequirements = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkBuffer, VkMemoryRequirements*)>(getDeviceProcAddr(device, "vkGetBufferMemoryRequirements"));
    data->GetDeviceMemoryCommitment = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkDeviceMemory, VkDeviceSize*)>(getDeviceProcAddr(device, "vkGetDeviceMemoryCommitment"));
    data->GetDeviceQueue = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, uint32_t, uint32_t, VkQueue*)>(getDeviceProcAddr(device, "vkGetDeviceQueue"));
    data->GetEventStatus = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkEvent)>(getDeviceProcAddr(device, "vkGetEventStatus"));
    data->GetFenceStatus = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkFence)>(getDeviceProcAddr(device, "vkGetFenceStatus"));
    data->GetImageMemoryRequirements = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkImage, VkMemoryRequirements*)>(getDeviceProcAddr(device, "vkGetImageMemoryRequirements"));
    data->GetImageSparseMemoryRequirements = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkImage, uint32_t*, VkSparseImageMemoryRequirements*)>(getDeviceProcAddr(device, "vkGetImageSparseMemoryRequirements"));
    data->GetImageSubresourceLayout = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkImage, const VkImageSubresource*, VkSubresourceLayout*)>(getDeviceProcAddr(device, "vkGetImageSubresourceLayout"));
    data->GetPipelineCacheData = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkPipelineCache, size_t*, void*)>(getDeviceProcAddr(device, "vkGetPipelineCacheData"));
    data->GetQueryPoolResults = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkQueryPool, uint32_t, uint32_t, size_t, void*, VkDeviceSize, VkQueryResultFlags)>(getDeviceProcAddr(device, "vkGetQueryPoolResults"));
    data->GetRenderAreaGranularity = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkRenderPass, VkExtent2D*)>(getDeviceProcAddr(device, "vkGetRenderAreaGranularity"));
    data->InvalidateMappedMemoryRanges = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, uint32_t, const VkMappedMemoryRange*)>(getDeviceProcAddr(device, "vkInvalidateMappedMemoryRanges"));
    data->MapMemory = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkDeviceMemory, VkDeviceSize, VkDeviceSize, VkMemoryMapFlags, void**)>(getDeviceProcAddr(device, "vkMapMemory"));
    data->MergePipelineCaches = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkPipelineCache, uint32_t, const VkPipelineCache*)>(getDeviceProcAddr(device, "vkMergePipelineCaches"));
    data->QueueBindSparse = reinterpret_cast<VkResult(VKAPI_PTR*)(VkQueue, uint32_t, const VkBindSparseInfo*, VkFence)>(getDeviceProcAddr(device, "vkQueueBindSparse"));
    data->QueueSubmit = reinterpret_cast<VkResult(VKAPI_PTR*)(VkQueue, uint32_t, const VkSubmitInfo*, VkFence)>(getDeviceProcAddr(device, "vkQueueSubmit"));
    data->QueueWaitIdle = reinterpret_cast<VkResult(VKAPI_PTR*)(VkQueue)>(getDeviceProcAddr(device, "vkQueueWaitIdle"));
    data->ResetCommandBuffer = reinterpret_cast<VkResult(VKAPI_PTR*)(VkCommandBuffer, VkCommandBufferResetFlags)>(getDeviceProcAddr(device, "vkResetCommandBuffer"));
    data->ResetCommandPool = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkCommandPool, VkCommandPoolResetFlags)>(getDeviceProcAddr(device, "vkResetCommandPool"));
    data->ResetDescriptorPool = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkDescriptorPool, VkDescriptorPoolResetFlags)>(getDeviceProcAddr(device, "vkResetDescriptorPool"));
    data->ResetEvent = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkEvent)>(getDeviceProcAddr(device, "vkResetEvent"));
    data->ResetFences = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, uint32_t, const VkFence*)>(getDeviceProcAddr(device, "vkResetFences"));
    data->SetEvent = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkEvent)>(getDeviceProcAddr(device, "vkSetEvent"));
    data->UnmapMemory = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkDeviceMemory)>(getDeviceProcAddr(device, "vkUnmapMemory"));
    data->UpdateDescriptorSets = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, uint32_t, const VkWriteDescriptorSet*, uint32_t, const VkCopyDescriptorSet*)>(getDeviceProcAddr(device, "vkUpdateDescriptorSets"));
    data->WaitForFences = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, uint32_t, const VkFence*, VkBool32, uint64_t)>(getDeviceProcAddr(device, "vkWaitForFences"));
    data->BindBufferMemory2 = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, uint32_t, const VkBindBufferMemoryInfo*)>(getDeviceProcAddr(device, "vkBindBufferMemory2"));
    data->BindImageMemory2 = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, uint32_t, const VkBindImageMemoryInfo*)>(getDeviceProcAddr(device, "vkBindImageMemory2"));
    data->CmdDispatchBase = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDispatchBase"));
    data->CmdSetDeviceMask = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, uint32_t)>(getDeviceProcAddr(device, "vkCmdSetDeviceMask"));
    data->CreateDescriptorUpdateTemplate = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkDescriptorUpdateTemplateCreateInfo*, const VkAllocationCallbacks*, VkDescriptorUpdateTemplate*)>(getDeviceProcAddr(device, "vkCreateDescriptorUpdateTemplate"));
    data->CreateSamplerYcbcrConversion = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkSamplerYcbcrConversionCreateInfo*, const VkAllocationCallbacks*, VkSamplerYcbcrConversion*)>(getDeviceProcAddr(device, "vkCreateSamplerYcbcrConversion"));
    data->DestroyDescriptorUpdateTemplate = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkDescriptorUpdateTemplate, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroyDescriptorUpdateTemplate"));
    data->DestroySamplerYcbcrConversion = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkSamplerYcbcrConversion, const VkAllocationCallbacks*)>(getDeviceProcAddr(device, "vkDestroySamplerYcbcrConversion"));
    data->GetBufferMemoryRequirements2 = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, const VkBufferMemoryRequirementsInfo2*, VkMemoryRequirements2*)>(getDeviceProcAddr(device, "vkGetBufferMemoryRequirements2"));
    data->GetDescriptorSetLayoutSupport = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, const VkDescriptorSetLayoutCreateInfo*, VkDescriptorSetLayoutSupport*)>(getDeviceProcAddr(device, "vkGetDescriptorSetLayoutSupport"));
    data->GetDeviceGroupPeerMemoryFeatures = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, uint32_t, uint32_t, uint32_t, VkPeerMemoryFeatureFlags*)>(getDeviceProcAddr(device, "vkGetDeviceGroupPeerMemoryFeatures"));
    data->GetDeviceQueue2 = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, const VkDeviceQueueInfo2*, VkQueue*)>(getDeviceProcAddr(device, "vkGetDeviceQueue2"));
    data->GetImageMemoryRequirements2 = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, const VkImageMemoryRequirementsInfo2*, VkMemoryRequirements2*)>(getDeviceProcAddr(device, "vkGetImageMemoryRequirements2"));
    data->GetImageSparseMemoryRequirements2 = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, const VkImageSparseMemoryRequirementsInfo2*, uint32_t*, VkSparseImageMemoryRequirements2*)>(getDeviceProcAddr(device, "vkGetImageSparseMemoryRequirements2"));
    data->TrimCommandPool = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkCommandPool, VkCommandPoolTrimFlags)>(getDeviceProcAddr(device, "vkTrimCommandPool"));
    data->UpdateDescriptorSetWithTemplate = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkDescriptorSet, VkDescriptorUpdateTemplate, const void*)>(getDeviceProcAddr(device, "vkUpdateDescriptorSetWithTemplate"));
    data->CmdBeginRenderPass2 = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkRenderPassBeginInfo*, const VkSubpassBeginInfo*)>(getDeviceProcAddr(device, "vkCmdBeginRenderPass2"));
    data->CmdDrawIndexedIndirectCount = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkDeviceSize, VkBuffer, VkDeviceSize, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDrawIndexedIndirectCount"));
    data->CmdDrawIndirectCount = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, VkBuffer, VkDeviceSize, VkBuffer, VkDeviceSize, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkCmdDrawIndirectCount"));
    data->CmdEndRenderPass2 = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkSubpassEndInfo*)>(getDeviceProcAddr(device, "vkCmdEndRenderPass2"));
    data->CmdNextSubpass2 = reinterpret_cast<void(VKAPI_PTR*)(VkCommandBuffer, const VkSubpassBeginInfo*, const VkSubpassEndInfo*)>(getDeviceProcAddr(device, "vkCmdNextSubpass2"));
    data->CreateRenderPass2 = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkRenderPassCreateInfo2*, const VkAllocationCallbacks*, VkRenderPass*)>(getDeviceProcAddr(device, "vkCreateRenderPass2"));
    data->GetBufferDeviceAddress = reinterpret_cast<VkDeviceAddress(VKAPI_PTR*)(VkDevice, const VkBufferDeviceAddressInfo*)>(getDeviceProcAddr(device, "vkGetBufferDeviceAddress"));
    data->GetBufferOpaqueCaptureAddress = reinterpret_cast<uint64_t(VKAPI_PTR*)(VkDevice, const VkBufferDeviceAddressInfo*)>(getDeviceProcAddr(device, "vkGetBufferOpaqueCaptureAddress"));
    data->GetDeviceMemoryOpaqueCaptureAddress = reinterpret_cast<uint64_t(VKAPI_PTR*)(VkDevice, const VkDeviceMemoryOpaqueCaptureAddressInfo*)>(getDeviceProcAddr(device, "vkGetDeviceMemoryOpaqueCaptureAddress"));
    data->GetSemaphoreCounterValue = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, VkSemaphore, uint64_t*)>(getDeviceProcAddr(device, "vkGetSemaphoreCounterValue"));
    data->ResetQueryPool = reinterpret_cast<void(VKAPI_PTR*)(VkDevice, VkQueryPool, uint32_t, uint32_t)>(getDeviceProcAddr(device, "vkResetQueryPool"));
    data->SignalSemaphore = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkSemaphoreSignalInfo*)>(getDeviceProcAddr(device, "vkSignalSemaphore"));
    data->WaitSemaphores = reinterpret_cast<VkResult(VKAPI_PTR*)(VkDevice, const VkSemaphoreWaitInfo*, uint64_t)>(getDeviceProcAddr(device, "vkWaitSemaphores"));
}
