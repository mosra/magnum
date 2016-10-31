#ifndef Magnum_Vk_Device_h
#define Magnum_Vk_Device_h
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
 * @brief Class @ref Magnum::Vk::Device
 */

#include <memory>

#include <Corrade/Containers/Array.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/PhysicalDevice.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"

namespace Magnum { namespace Extensions {
/*
#ifndef DOXYGEN_GENERATING_OUTPUT
#define _extension(vendor, extension, _requiredVersion, _coreVersion) \
    struct extension {                                                      \
        enum: std::size_t { Index = __LINE__-1 };                                \
        constexpr static Version requiredVersion() { return Version::_requiredVersion; } \
        constexpr static Version coreVersion() { return Version::_coreVersion; } \
        constexpr static const char* string() { return "VK_" #vendor "_" #extension; } \
    };

    // TODO: Encode Device/Instance level into extension structure
namespace Device { namespace Vk {
    namespace AMD {
        _extension(AMD,negative_viewport_height, Vulkan_1_0, None)
    } namespace EXT {
        _extension(EXT,debug_marker,             Vulkan_1_0, None)
        _extension(EXT,debug_reporter,           Vulkan_1_0, None)
    } namespace KHR {
        _extension(KHR,android_surface,          Vulkan_1_0, None)
        _extension(KHR,display,                  Vulkan_1_0, None)
        _extension(KHR,display_swapchain,        Vulkan_1_0, None)
        _extension(KHR,mir_surface,              Vulkan_1_0, None)
        _extension(KHR,sampler_mirror_clamp_to_edge, Vulkan_1_0, None)
        _extension(KHR,swapchain,                Vulkan_1_0, None)
        _extension(KHR,wayland_surface,          Vulkan_1_0, None)
        _extension(KHR,win32_surface,            Vulkan_1_0, None)
        _extension(KHR,xcb_surface,              Vulkan_1_0, None)
        _extension(KHR,xlib_surface,             Vulkan_1_0, None)
    } namespace NV {
        _extension(NV,dedicated_allocation,      Vulkan_1_0, None)
        _extension(NV,glsl_shader,               Vulkan_1_0, None)
    }
}}

namespace Instance { namespace Vk {
    namespace KHR {
        _extension(KHR,surface,                  Vulkan_1_0, None)
    }
}}
*/
}
namespace Vk {

constexpr UnsignedInt deviceFeaturesCount = sizeof(VkPhysicalDeviceFeatures) / 4;

/** @todoc */
enum class DeviceFeature : UnsignedInt {
    RobustBufferAccess = 0,
    FullDrawIndexUint32,
    ImageCubeArray,
    IndependentBlend,
    GeometryShader,
    TessellationShader,
    SampleRateShading,
    DualSrcBlend,
    LogicOp,
    MultiDrawIndirect,
    DrawIndirectFirstInstance,
    DepthClamp,
    DepthBiasClamp,
    FillModeNonSolid,
    DepthBounds,
    WideLines,
    LargePoints,
    AlphaToOne,
    MultiViewport,
    SamplerAnisotropy,
    TextureCompressionETC2,
    TextureCompressionASTC_LDR,
    TextureCompressionBC,
    OcclusionQueryPrecise,
    PipelineStatisticsQuery,
    VertexPipelineStoresAndAtomics,
    FragmentStoresAndAtomics,
    ShaderTessellationAndGeometryPointSize,
    ShaderImageGatherExtended,
    ShaderStorageImageExtendedFormats,
    ShaderStorageImageMultisample,
    ShaderStorageImageReadWithoutFormat,
    ShaderStorageImageWriteWithoutFormat,
    ShaderUniformBufferArrayDynamicIndexing,
    ShaderSampledImageArrayDynamicIndexing,
    ShaderStorageBufferArrayDynamicIndexing,
    ShaderStorageImageArrayDynamicIndexing,
    ShaderClipDistance,
    ShaderCullDistance,
    ShaderFloat64,
    ShaderInt64,
    ShaderInt16,
    ShaderResourceResidency,
    ShaderResourceMinLod,
    SparseBinding,
    SparseResidencyBuffer,
    SparseResidencyImage2D,
    SparseResidencyImage3D,
    SparseResidency2Samples,
    SparseResidency4Samples,
    SparseResidency8Samples,
    SparseResidency16Samples,
    SparseResidencyAliased,
    VariableMultisampleRate,
    InheritedQueries,
};

/**
@brief Device features class

Container for physical device features.
*/
class DeviceFeatures {
public:

    DeviceFeatures(const std::initializer_list<DeviceFeature>& features) {
        VkBool32* enabled = reinterpret_cast<VkBool32*>(&_features);
        std::memset(enabled, 0, sizeof(VkPhysicalDeviceFeatures));
        for(DeviceFeature f : features) {
            enabled[UnsignedInt(f)] = VK_TRUE;
        }
    }

    operator VkPhysicalDeviceFeatures() const {
        return _features;
    }

    operator VkPhysicalDeviceFeatures&() {
        return _features;
    }

private:
    VkPhysicalDeviceFeatures _features;
};

struct DeviceQueueCreateInfo {

    DeviceQueueCreateInfo(UnsignedInt queueFamilyIndex, const std::vector<Float> priorities) :
        _queueFamilyIndex{queueFamilyIndex},
        _queueCount{1},
        _queuePriorities{new float[priorities.size()]}
    {
        std::copy(priorities.begin(), priorities.end(), _queuePriorities.get());
    }

    DeviceQueueCreateInfo(const DeviceQueueCreateInfo& other):
        _queueFamilyIndex{other._queueFamilyIndex},
        _queueCount{other._queueCount},
        _queuePriorities{new float[_queueCount]}
    {
            std::copy(other._queuePriorities.get(), other._queuePriorities.get() + _queueCount,
                      _queuePriorities.get());
    }

    ~DeviceQueueCreateInfo() = default;

    UnsignedInt queueCount() const {
        return _queueCount;
    }

    UnsignedInt queueFamilyIndex() const {
        return _queueFamilyIndex;
    }

private:
    const VkStructureType _type = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    const void* _next = nullptr;
    const VkDeviceQueueCreateFlags _flags = 0;
    UnsignedInt _queueFamilyIndex;
    UnsignedInt _queueCount;
    std::unique_ptr<Float> _queuePriorities;
};

static_assert(sizeof(DeviceQueueCreateInfo) == sizeof(VkDeviceQueueCreateInfo), "DeviceQueueCreateInfo and VkDeviceQueueCreateInfo are required to be of same size.");

class CommandPool;
class Queue;

class MAGNUM_VK_EXPORT Device {
    public:

        /** @brief Copying is not allowed */
        Device(const Device&) = delete;

        /** @brief Move constructor */
        Device(Device&& other);

    public:
        Device::Device(const PhysicalDevice& physicalDevice,
            std::initializer_list<DeviceQueueCreateInfo> requestedQueues,
            std::initializer_list<const char*> extensions,
            std::initializer_list<const char*> validationLayers,
            const DeviceFeatures& features);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyDevice}
         */
        ~Device();

        /** @brief Copying is not allowed */
        Device& operator=(const Device&) = delete;

        /** @brief Move assignment is not allowed */
        Device& operator=(Device&&) = delete;

        /**
         * @brief Return the underlying VkDevice handle
         */
        VkDevice vkDevice() {
            return _device;
        }

        /**
         * @return Reference to the PhysicalDevice this Device was created from
         */
        PhysicalDevice& physicalDevice() {
            return _physicalDevice;
        }

        /**
         * @brief Wait until the device is idle
         * @return Reference to self (for method chaining)
         */
        Device& waitIdle() {
            VkResult err = vkDeviceWaitIdle(_device);
            MAGNUM_VK_ASSERT_ERROR(err);
            return *this;
        }

        operator VkDevice() const {
            return _device;
        }

        /**
         * @brief Get a requested queue
         * @param index Index of the queue in order requested
         */
        Queue& getQueue(UnsignedInt index) {
            return *_queues[index];
        }

    private:
        VkDevice _device;
        PhysicalDevice _physicalDevice;
        std::vector<std::unique_ptr<Queue>> _queues;
};

}}

#endif
