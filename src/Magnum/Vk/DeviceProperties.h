#ifndef Magnum_Vk_DeviceProperties_h
#define Magnum_Vk_DeviceProperties_h
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
 * @brief Class @ref Magnum::Vk::DeviceProperties, enum @ref Magnum::Vk::DeviceType, @ref Magnum::Vk::QueueFlag, @ref Magnum::Vk::MemoryHeapFlag, enum set @ref Magnum::Vk::QueueFlags, @ref Magnum::Vk::MemoryHeapFlags, function @ref Magnum::Vk::enumerateDevices(), @ref Magnum::Vk::pickDevice(), @ref Magnum::Vk::tryPickDevice()
 * @m_since_latest
 */

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/Reference.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation {
    struct InstanceState;
    UnsignedInt enumerateDevicesInto(Instance& instance, Containers::ArrayView<DeviceProperties> out);
}

/**
@brief Physical device type
@m_since_latest

Wraps a @type_vk_keyword{PhysicalDeviceType}.
@see @ref DeviceProperties::type()
@m_enum_values_as_keywords
*/
enum class DeviceType: Int {
    /** Anything that does not match any other available types */
    Other = VK_PHYSICAL_DEVICE_TYPE_OTHER,

    /**
     * Typically a device embedded in or tightly coupled with the host
     */
    IntegratedGpu = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,

    /**
     * Typically a separate processor connected to the host via an
     * interlink
     */
    DiscreteGpu = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,

    /** Typically a virtual node in a virtualization environment */
    VirtualGpu = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,

    /** Typically running on the same processors as the host */
    Cpu = VK_PHYSICAL_DEVICE_TYPE_CPU
};

/**
@debugoperatorclassenum{DeviceProperties,DeviceType}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, DeviceType value);

/**
@brief Physical device driver ID
@m_since_latest

Wraps a @type_vk_keyword{DriverId}.
@see @ref DeviceProperties::driver()
@requires_vk12 Extension @vk_extension{KHR,driver_properties}
@m_enum_values_as_keywords
*/
enum class DeviceDriver: Int {
    /**
     * Unknown. Returned from @ref DeviceProperties::driver() in case Vulkan
     * 1.2 or the @vk_extension{KHR,driver_properties} extension isn't
     * supported.
     */
    Unknown = 0,

    /* Unlike with VkDriverId, which gets allocated sequentially, the rest of
       this list is sorted alphabetically for easier lookup. */

    /** Open-source AMD */
    AmdOpenSource = VK_DRIVER_ID_AMD_OPEN_SOURCE,

    /** Proprietary AMD */
    AmdProprietary = VK_DRIVER_ID_AMD_PROPRIETARY,

    /** Proprietary ARM */
    ArmProprietary = VK_DRIVER_ID_ARM_PROPRIETARY,

    /** Proprietary Broadcom */
    BroadcomProprietary = VK_DRIVER_ID_BROADCOM_PROPRIETARY,

    /** Proprietary GGP */
    GgpProprietary = VK_DRIVER_ID_GGP_PROPRIETARY,

    /** Google SwiftShader */
    GoogleSwiftShader = VK_DRIVER_ID_GOOGLE_SWIFTSHADER,

    /** Proprietary Imagination */
    ImaginationProprietary = VK_DRIVER_ID_IMAGINATION_PROPRIETARY,

    /** Open-source Intel Mesa drivers */
    IntelOpenSourceMesa = VK_DRIVER_ID_INTEL_OPEN_SOURCE_MESA,

    /** Proprietary Intel driver on Windows */
    IntelProprietaryWindows = VK_DRIVER_ID_INTEL_PROPRIETARY_WINDOWS,

    /** Mesa LLVMpipe */
    MesaLlvmpipe = VK_DRIVER_ID_MESA_LLVMPIPE,

    /** Mesa RADV */
    MesaRadv = VK_DRIVER_ID_MESA_RADV,

    /** MoltenVK */
    MoltenVk = VK_DRIVER_ID_MOLTENVK,

    /** Proprietary NVidia */
    NVidiaProprietary = VK_DRIVER_ID_NVIDIA_PROPRIETARY,

    /** Proprietary Qualcomm */
    QualcommProprietary = VK_DRIVER_ID_QUALCOMM_PROPRIETARY,
};

/**
@debugoperatorclassenum{DeviceProperties,DeviceDriver}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, DeviceDriver value);

/**
@brief Queue flag
@m_since_latest

Wraps a @type_vk_keyword{QueueFlagBits}.
@see @ref QueueFlags, @ref DeviceProperties::queueFamilyFlags()
@m_enum_values_as_keywords
*/
enum class QueueFlag: UnsignedInt {
    /** Supports general graphics and rasterization operations. */
    Graphics = VK_QUEUE_GRAPHICS_BIT,

    /** Supports compute and ray tracing operations. */
    Compute = VK_QUEUE_COMPUTE_BIT,

    /** Supports transfer operations */
    Transfer = VK_QUEUE_TRANSFER_BIT,

    /** Supports sparse memory management operations */
    SparseBinding = VK_QUEUE_SPARSE_BINDING_BIT,

    /** Supports protected memory operations */
    Protected = VK_QUEUE_PROTECTED_BIT
};

/**
@debugoperatorclassenum{DeviceProperties,QueueFlag}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, QueueFlag value);

/**
@brief Queue flags
@m_since_latest

Type-safe wrapper for @type_vk_keyword{QueueFlags}.
@see @ref DeviceProperties::queueFamilyFlags()
*/
typedef Containers::EnumSet<QueueFlag> QueueFlags;

CORRADE_ENUMSET_OPERATORS(QueueFlags)

/**
@debugoperatorclassenum{DeviceProperties,QueueFlags}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, QueueFlags value);

/**
@brief Memory heap flag
@m_since_latest

Wraps a @type_vk_keyword{MemoryHeapFlagBits}.
@see @ref QueueFlags, @ref DeviceProperties::queueFamilyFlags()
@m_enum_values_as_keywords
*/
enum class MemoryHeapFlag: UnsignedInt {
    /** Corresponds to device-local memory */
    DeviceLocal = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,

    /** @todo MultiInstance, Vk 1.1 */
};

/**
@debugoperatorclassenum{DeviceProperties,MemoryHeapFlag}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, MemoryHeapFlag value);

/**
@brief Memory heap flags
@m_since_latest

Type-safe wrapper for @type_vk_keyword{MemoryHeapFlags}.
@see @ref DeviceProperties::memoryHeapFlags(), @ref MemoryFlags
*/
typedef Containers::EnumSet<MemoryHeapFlag> MemoryHeapFlags;

CORRADE_ENUMSET_OPERATORS(MemoryHeapFlags)

/**
@debugoperatorclassenum{DeviceProperties,MemoryHeapFlags}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, MemoryHeapFlags value);

/* MemoryFlag/MemoryFlags are in Memory.h since those are used mainly in
   contexts where DeviceProperties isn't present */

/**
@brief Physical device properties
@m_since_latest

Wraps a @type_vk_keyword{PhysicalDevice} along with its (lazy-populated)
properties and features. See the @ref Vk-Device-creation "Device creation docs"
for an example of using this class for enumerating available devices and
picking one of them.
@see @ref pickDevice(), @ref enumerateDevices()
*/
class MAGNUM_VK_EXPORT DeviceProperties {
    public:
        /**
         * @brief Wrap existing Vulkan physical device
         * @param instance      Vulkan instance
         * @param handle        The @type_vk{PhysicalDevice} handle
         *
         * The @p handle is expected to be originating from @p instance. Unlike
         * with other handle types, the @type_vk{PhysicalDevice} handles don't
         * have to be destroyed at the end. so there's no equivalent of e.g.
         * @ref Instance::release() or @ref Instance::handleFlags().
         */
        static DeviceProperties wrap(Instance& instance, VkPhysicalDevice handle) {
            return DeviceProperties{instance, handle};
        }

        /**
         * @brief Construct without populating the contents
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit DeviceProperties(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        DeviceProperties(const DeviceProperties&) = delete;

        /** @brief Move constructor */
        DeviceProperties(DeviceProperties&&) noexcept;

        ~DeviceProperties();

        /** @brief Copying is not allowed */
        DeviceProperties& operator=(const DeviceProperties&) = delete;

        /** @brief Move assignment */
        DeviceProperties& operator=(DeviceProperties&&) noexcept;

        /**
         * @brief Underlying @type_vk{PhysicalDevice} handle
         *
         * Unlike most handle getters, this one is marked as @cpp const @ce
         * because the @ref DeviceProperties class is treated as a *view* on
         * the physical device --- it doesn't change the device state in any
         * way, only queries its properties.
         */
        VkPhysicalDevice handle() const { return _handle; }
        /** @overload */
        operator VkPhysicalDevice() const { return _handle; }

        /**
         * @brief Raw device properties
         *
         * Populated lazily on first request. If Vulkan 1.1 is not supported
         * and the @vk_extension{KHR,get_physical_device_properties2} extension
         * is not enabled on the originating instance, only the Vulkan 1.0
         * subset of device properties is queried, with the `pNext` member
         * being @cpp nullptr @ce. Otherwise:
         *
         * -    If Vulkan 1.2 or the @vk_extension{KHR,driver_properties}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceDriverProperties} and
         *      the @ref driver(), @ref driverName() and @ref driverInfo()
         *      properties are populated.
         *
         * @see @fn_vk_keyword{GetPhysicalDeviceProperties2},
         *      @fn_vk_keyword{GetPhysicalDeviceProperties},
         *      @type_vk_keyword{PhysicalDeviceProperties2},
         *      @type_vk_keyword{PhysicalDeviceProperties}
         */
        const VkPhysicalDeviceProperties2& properties();

        /**
         * @brief Device version
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        Version version();

        /**
         * @brief Whether given version is supported on the device
         *
         * Compares @p version against @ref version().
         */
        bool isVersionSupported(Version version);

        /**
         * @brief Device type
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        DeviceType type();

        /**
         * @brief Device name
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        Containers::StringView name();

        /**
         * @brief Driver ID
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request. Only present if Vulkan 1.2 is supported or the
         * @vk_extension{KHR,driver_properties} extension is supported by the
         * device, otherwise returns @ref DeviceDriver::Unknown. Note that
         * there might be driver IDs not yet listed in the @ref DeviceDriver
         * enum, moreover drivers are allowed to return unregistered IDs as
         * well.
         */
        DeviceDriver driver();

        /**
         * @brief Driver version
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        Version driverVersion();

        /**
         * @brief Driver name
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request. Only present if Vulkan 1.2 is supported or the
         * @vk_extension{KHR,driver_properties} extension is supported by the
         * device, otherwise returns an empty string.
         */
        Containers::StringView driverName();

        /**
         * @brief Driver info
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request. Only present if Vulkan 1.2 is supported or the
         * @vk_extension{KHR,driver_properties} extension is supported by the
         * device, otherwise returns an empty string.
         */
        Containers::StringView driverInfo();

        /**
         * @brief Enumerate device extensions
         * @param layers        Additional layers to list extensions from
         *
         * Expects that all listed layers are supported --- however they don't
         * need to be enabled on the instance.
         * @see @ref LayerProperties::isSupported(),
         *      @fn_vk_keyword{EnumerateDeviceExtensionProperties}
         */
        ExtensionProperties enumerateExtensionProperties(Containers::ArrayView<const Containers::StringView> layers = {});

        /** @overload */
        ExtensionProperties enumerateExtensionProperties(std::initializer_list<Containers::StringView> layers);

        /**
         * @brief Device features
         *
         * Populated lazily on first request, extracting the booleans into a
         * more compact set of @ref DeviceFeature values. If Vulkan 1.1 is not
         * supported and the @vk_extension{KHR,get_physical_device_properties2}
         * extension is not enabled on the originating instance, only the
         * Vulkan 1.0 subset of device features is queried. Otherwise:
         *
         * -    If Vulkan 1.1 is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceProtectedMemoryFeatures}
         * -    If Vulkan 1.1 or the @vk_extension{KHR,multiview} extension is
         *      supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceMultiviewFeatures}
         * -    If Vulkan 1.1 or the @vk_extension{KHR,shader_draw_parameters}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceShaderDrawParametersFeatures}
         * -    If the @vk_extension{EXT,texture_compression_astc_hdr}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceTextureCompressionASTCHDRFeaturesEXT}
         * -    If Vulkan 1.2 or the @vk_extension{KHR,shader_float16_int8}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceShaderFloat16Int8Features}
         * -    If Vulkan 1.1 or the @vk_extension{KHR,16bit_storage} extension
         *      is supported by the device, the `pNext` chain contains
         *      @type_vk_keyword{PhysicalDevice16BitStorageFeatures}
         * -    If Vulkan 1.2 or the @vk_extension{KHR,imageless_framebuffer}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceImagelessFramebufferFeatures}
         * -    If Vulkan 1.1 or the @vk_extension{KHR,variable_pointers}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceVariablePointersFeatures}
         * -    If the @vk_extension{KHR,acceleration_structure} extension is
         *      supported by the device, the `pNext` chain contains
         *      @type_vk_keyword{PhysicalDeviceAccelerationStructureFeaturesKHR}
         * -    If Vulkan 1.1 or the @vk_extension{KHR,sampler_ycbcr_conversion}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceSamplerYcbcrConversionFeatures}
         * -    If Vulkan 1.2 or the @vk_extension{EXT,descriptor_indexing}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceDescriptorIndexingFeatures}
         * -    If the @vk_extension{KHR,portability_subset} extension is
         *      supported by the device, the `pNext` chain contains
         *      @type_vk_keyword{PhysicalDevicePortabilitySubsetFeaturesKHR}
         * -    If Vulkan 1.2 or the @vk_extension{KHR,shader_subgroup_extended_types}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceShaderSubgroupExtendedTypesFeatures}
         * -    If Vulkan 1.2 or the @vk_extension{KHR,8bit_storage} extension is
         *      supported by the device, the `pNext` chain contains
         *      @type_vk_keyword{PhysicalDevice8BitStorageFeatures}
         * -    If Vulkan 1.2 or the @vk_extension{KHR,shader_atomic_int64}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceShaderAtomicInt64Features}
         * -    If the @vk_extension{EXT,vertex_attribute_divisor} extension is
         *      supported by the device, the `pNext` chain contains @type_vk_keyword{PhysicalDeviceVertexAttributeDivisorFeaturesEXT}
         * -    If Vulkan 1.2 or the @vk_extension{KHR,timeline_semaphore}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceTimelineSemaphoreFeatures}
         * -    If Vulkan 1.2 or the @vk_extension{KHR,vulkan_memory_model}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceVulkanMemoryModelFeatures}
         * -    If Vulkan 1.2 or the @vk_extension{EXT,scalar_block_layout}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceScalarBlockLayoutFeatures}
         * -    If Vulkan 1.2 or the @vk_extension{KHR,separate_depth_stencil_layouts}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceSeparateDepthStencilLayoutsFeatures}
         * -    If Vulkan 1.2 or the @vk_extension{KHR,uniform_buffer_standard_layout}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceUniformBufferStandardLayoutFeatures}
         * -    If Vulkan 1.2 or the @vk_extension{KHR,buffer_device_address}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceBufferDeviceAddressFeatures}
         * -    If Vulkan 1.2 or the @vk_extension{EXT,host_query_reset}
         *      extension is supported by the device, the `pNext` chain
         *      contains @type_vk_keyword{PhysicalDeviceHostQueryResetFeatures}
         * -    If the @vk_extension{EXT,index_type_uint8} extension is
         *      supported by the device, the `pNext` chain contains
         *      @type_vk_keyword{PhysicalDeviceIndexTypeUint8FeaturesEXT}
         * -    If the @vk_extension{EXT,extended_dynamic_state} extension is
         *      supported by the device, the `pNext` chain contains
         *      @type_vk_keyword{PhysicalDeviceExtendedDynamicStateFeaturesEXT}
         * -    If the @vk_extension{EXT,robustness2} extension is supported by
         *      the device, the `pNext` chain contains
         *      @type_vk_keyword{PhysicalDeviceRobustness2FeaturesEXT}
         * -    If the @vk_extension{EXT,image_robustness} extension is
         *      supported by the device, the `pNext` chain contains
         *      @type_vk_keyword{PhysicalDeviceImageRobustnessFeaturesEXT}
         * -    If the @vk_extension{KHR,ray_tracing_pipeline} extension is
         *      supported by the device, the `pNext` chain contains
         *      @type_vk_keyword{PhysicalDeviceRayTracingPipelineFeaturesKHR}
         * -    If the @vk_extension{KHR,ray_query} extension is supported by
         *      the device, the `pNext` chain contains
         *      @type_vk_keyword{PhysicalDeviceRayQueryFeaturesKHR}
         *
         * If the @vk_extension{KHR,portability_subset} is *not* supported by
         * the device, all features related to it are implicitly marked as
         * supported to simplify portability-aware logic. See
         * @ref Vk-Device-portability-subset for details.
         *
         * @see @ref Device::enabledFeatures(),
         *      @fn_vk_keyword{GetPhysicalDeviceFeatures2},
         *      @fn_vk_keyword{GetPhysicalDeviceFeatures},
         *      @type_vk_keyword{PhysicalDeviceFeatures2},
         *      @type_vk_keyword{PhysicalDeviceFeatures}
         */
        const DeviceFeatures& features();

        /**
         * @brief Queue family properties
         *
         * Populated lazily on first request. If Vulkan 1.1 is not supported
         * and the @vk_extension{KHR,get_physical_device_properties2} extension
         * is not enabled on the originating instance, only the Vulkan 1.0
         * subset of device properties is queried.
         * @see @fn_vk_keyword{GetPhysicalDeviceQueueFamilyProperties2},
         *      @fn_vk_keyword{GetPhysicalDeviceQueueFamilyProperties},
         *      @type_vk_keyword{PhysicalDeviceQueueFamilyProperties2},
         *      @type_vk_keyword{PhysicalDeviceQueueFamilyProperties}
         */
        Containers::ArrayView<const VkQueueFamilyProperties2> queueFamilyProperties();

        /**
         * @brief Queue family count
         *
         * Convenience access to @ref queueFamilyProperties() internals,
         * populated lazily on first request.
         */
        UnsignedInt queueFamilyCount();

        /**
         * @brief Queue count in given family
         * @param queueFamily   Queue family index, expected to be smaller than
         *      @ref queueFamilyCount()
         *
         * Convenience access to @ref queueFamilyProperties() internals,
         * populated lazily on first request.
         */
        UnsignedInt queueFamilySize(UnsignedInt queueFamily);

        /**
         * @brief Queue family flags
         * @param queueFamily   Queue family index, expected to be smaller than
         *      @ref queueFamilyCount()
         *
         * Convenience access to @ref queueFamilyProperties() internals,
         * populated lazily on first request.
         */
        QueueFlags queueFamilyFlags(UnsignedInt queueFamily);

        /**
         * @brief Pick a queue family satisfying given flags
         * @return Queue family index for use in @ref queueFamilySize(),
         *      @ref queueFamilyFlags() and @ref DeviceCreateInfo::addQueues()
         *
         * Queries queue family properties using @ref queueFamilyProperties()
         * and tries to find the first that contains all @p flags. If it is not
         * found, exits. See @ref tryPickQueueFamily() for an alternative that
         * doesn't exit on failure.
         */
        UnsignedInt pickQueueFamily(QueueFlags flags);

        /**
         * @brief Try to pick a queue family satisfying given flags
         *
         * Compared to @ref pickQueueFamily() the function returns
         * @ref Containers::NullOpt if a desired family isn't found instead of
         * exiting.
         */
        Containers::Optional<UnsignedInt> tryPickQueueFamily(QueueFlags flags);

        /**
         * @brief Device memory properties
         *
         * Populated lazily on first request. If Vulkan 1.1 is not supported
         * and the @vk_extension{KHR,get_physical_device_properties2} extension
         * is not enabled on the originating instance, only the Vulkan 1.0
         * subset of device properties is queried.
         * @see @fn_vk_keyword{GetPhysicalDeviceMemoryProperties2},
         *      @fn_vk_keyword{GetPhysicalDeviceMemoryProperties},
         *      @type_vk_keyword{PhysicalDeviceMemoryProperties2},
         *      @type_vk_keyword{PhysicalDeviceMemoryProperties}
         */
        const VkPhysicalDeviceMemoryProperties2& memoryProperties();

        /**
         * @brief Memory heap count
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        UnsignedInt memoryHeapCount();

        /**
         * @brief Memory heap size
         * @param heap   Memory heap index, expected to be smaller than
         *      @ref memoryHeapCount()
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        UnsignedLong memoryHeapSize(UnsignedInt heap);

        /**
         * @brief Memory heap size
         * @param heap   Memory heap index, expected to be smaller than
         *      @ref memoryHeapCount()
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        MemoryHeapFlags memoryHeapFlags(UnsignedInt heap);

        /**
         * @brief Memory type count
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        UnsignedInt memoryCount();

        /**
         * @brief Memory type flags
         * @param memory    Memory type index, expected to be smaller than
         *      @ref memoryCount()
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        MemoryFlags memoryFlags(UnsignedInt memory);

        /**
         * @brief Memory heap index
         * @param memory    Memory type index, expected to be smaller than
         *      @ref memoryCount()
         * @return Memory heap index, always smaller than @ref memoryHeapCount()
         *
         * Convenience access to @ref memoryProperties() internals, populated
         * lazily on first request.
         */
        UnsignedInt memoryHeapIndex(UnsignedInt memory);

        /**
         * @brief Pick a memory type satisfying given flags
         * @param requiredFlags     Memory flags that should be present in
         *      picked memory type. Can be an empty set, but picking such
         *      memory is probably not very useful.
         * @param preferredFlags    If there's more than one memory type
         *      matching @p requiredFlags, prefer one that has most of these
         *      as well. Defaults to an empty set.
         * @param memories          Bits indicating which memory types should
         *      be considered (bit @cpp 0 @ce indicates memory type @cpp 0 @ce
         *      should be considered etc.). Expected to have at least one bit
         *      of the first @ref memoryCount() bits set, otherwise the
         *      function will always fail. Defaults to all bits set, meaning
         *      all memory types are considered. Corresponds to
         *      @ref MemoryRequirements::memories().
         *
         * Queries memory properties using @ref memoryProperties() and out of
         * memory types set in @p memoryBits tries to find one that contains
         * all @p requiredFlags and most of @p optionalFlags. If it is not
         * found, exits. See @ref tryPickMemory() for an alternative that
         * doesn't exit on failure.
         *
         * @m_class{m-note m-success}
         *
         * @par
         *      The @p preferredFlags can be used for example to ask for a
         *      @ref MemoryFlag::HostVisible bit on a
         *      @ref MemoryFlag::DeviceLocal memory --- on discrete GPUs this
         *      combination is usually not possible so you get just a
         *      device-only memory, but on integrated GPUs it can be used to
         *      avoid a need for a copy through a temporary staging buffer.
         */
        UnsignedInt pickMemory(MemoryFlags requiredFlags, MemoryFlags preferredFlags = {}, UnsignedInt memories = ~UnsignedInt{});

        /**
         * @overload
         *
         * Equivalent to calling @ref pickMemory(MemoryFlags, MemoryFlags, UnsignedInt)
         * with empty @p preferredFlags.
         */
        UnsignedInt pickMemory(MemoryFlags requiredFlags, UnsignedInt memories);

        /**
         * @brief Try to pick a memory type satisfying given flags
         *
         * Compared to @ref pickMemory() the function returns
         * @ref Containers::NullOpt if a desired memory type isn't found
         * instead of exiting.
         */
        Containers::Optional<UnsignedInt> tryPickMemory(MemoryFlags requiredFlags, MemoryFlags preferredFlags = {}, UnsignedInt memories = ~UnsignedInt{});

        /**
         * @overload
         *
         * Equivalent to calling @ref tryPickMemory(MemoryFlags, MemoryFlags, UnsignedInt)
         * with empty @p preferredFlags.
         */
        Containers::Optional<UnsignedInt> tryPickMemory(MemoryFlags requiredFlags, UnsignedInt memories);

    private:
        friend Implementation::InstanceState;

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* The DAMN THING lists this among friends, which is AN IMPLEMENTATION
           DETAIL */
        friend DeviceCreateInfo;
        friend UnsignedInt Implementation::enumerateDevicesInto(Instance&, Containers::ArrayView<DeviceProperties>);
        #endif

        explicit DeviceProperties(Instance& instance, VkPhysicalDevice handle);

        /* The vkPhysicalDeviceProperties() function has to get called in any
           case to decide if vkPhysicalDeviceProperties2() can get called. That
           means we can get the VkPhysicalDeviceProperties subset in a cheaper
           way than when going through properties(). This distinction however
           would only confuse the users, so it's a private API and while
           version(), name(), driverVersion() etc. queries use it they only
           mention properties() in the public docs. */
        MAGNUM_VK_LOCAL const VkPhysicalDeviceProperties& properties1();

        /* Cached version of enumerateExtensionProperties() for internal use by
           the property query functions. The enumerateExtensionProperties() API
           returns by-value because every time there can be a different set of
           layers, so we can't do any caching on that level but need a separate
           function. */
        MAGNUM_VK_LOCAL const ExtensionProperties& extensionPropertiesInternal();

        /* Combines isVersionSupported(E::coreVersion()) and
           ExtensionProperties::isSupported<E>(). Used internally to avoid
           accidents with incorrectly specified extension core version. */
        template<class E> MAGNUM_VK_LOCAL bool isOrVersionSupportedInternal();

        /* Used by DeviceCreateInfo */
        MAGNUM_VK_LOCAL bool canUseFeatures2ForDeviceCreation();

        MAGNUM_VK_LOCAL static void getPropertiesImplementationDefault(DeviceProperties& self, VkPhysicalDeviceProperties2& properties);
        MAGNUM_VK_LOCAL static void getPropertiesImplementationKHR(DeviceProperties& self, VkPhysicalDeviceProperties2& properties);
        MAGNUM_VK_LOCAL static void getPropertiesImplementation11(DeviceProperties& self, VkPhysicalDeviceProperties2& properties);

        MAGNUM_VK_LOCAL static void getFeaturesImplementationDefault(DeviceProperties& self, VkPhysicalDeviceFeatures2& features);
        MAGNUM_VK_LOCAL static void getFeaturesImplementationKHR(DeviceProperties& self, VkPhysicalDeviceFeatures2& features);
        MAGNUM_VK_LOCAL static void getFeaturesImplementation11(DeviceProperties& self, VkPhysicalDeviceFeatures2& features);

        MAGNUM_VK_LOCAL static void getQueueFamilyPropertiesImplementationDefault(DeviceProperties& self, UnsignedInt& count, VkQueueFamilyProperties2* properties);
        MAGNUM_VK_LOCAL static void getQueueFamilyPropertiesImplementationKHR(DeviceProperties& self, UnsignedInt& count, VkQueueFamilyProperties2* properties);
        MAGNUM_VK_LOCAL static void getQueueFamilyPropertiesImplementation11(DeviceProperties& self, UnsignedInt& count, VkQueueFamilyProperties2* properties);

        MAGNUM_VK_LOCAL static void getMemoryPropertiesImplementationDefault(DeviceProperties& self, VkPhysicalDeviceMemoryProperties2& properties);
        MAGNUM_VK_LOCAL static void getMemoryPropertiesImplementationKHR(DeviceProperties& self, VkPhysicalDeviceMemoryProperties2& properties);
        MAGNUM_VK_LOCAL static void getMemoryPropertiesImplementation11(DeviceProperties& self, VkPhysicalDeviceMemoryProperties2& properties);

        /* Can't be a reference because of the NoCreate constructor */
        Instance* _instance;

        VkPhysicalDevice _handle;
        struct State;
        Containers::Pointer<State> _state;
};

/**
@brief Enumerate physical devices
@m_since_latest

Returns a list of all devices present on the system. See @ref pickDevice() for
an alternative that pick just a single appropriate device. See @ref Device for
general usage information.
@see @fn_vk_keyword{EnumeratePhysicalDevices}
*/
MAGNUM_VK_EXPORT Containers::Array<DeviceProperties> enumerateDevices(Instance& instance);

/**
@brief Pick a physical device
@m_since_latest

Selects a device based on preferences specified through the `--magnum-device`
@ref Vk-Instance-command-line "command-line option". If a device is not found,
exits. See @ref tryPickDevice() for an alternative that doesn't exit on
failure. See @ref Device for general usage information.

If `--magnum-device` is not specified or `--magnum-device` specifies a device
index, this function enumerates just the first N devices to satisfy the
request. Otherwise calls @ref enumerateDevices() and picks the first device
matching the criteria in `--magnum-device`.
*/
MAGNUM_VK_EXPORT DeviceProperties pickDevice(Instance& instance);

/**
@brief Try to pick a physical device
@m_since_latest

Compared to @ref pickDevice() the function returns @ref Containers::NullOpt if
a device isn't found instead of exiting.
*/
MAGNUM_VK_EXPORT Containers::Optional<DeviceProperties> tryPickDevice(Instance& instance);

}}

#endif
