#ifndef Magnum_Vk_DeviceCreateInfo_h
#define Magnum_Vk_DeviceCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::DeviceCreateInfo
 * @m_since_latest
 */

#include <cstddef>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Tags.h"
#include "Magnum/Vk/TypeTraits.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Device creation info
@m_since_latest

Wraps a @type_vk_keyword{DeviceCreateInfo}. See
@ref Vk-Device-creation "Device creation" for usage information.
*/
class MAGNUM_VK_EXPORT DeviceCreateInfo {
    public:
        /**
         * @brief Device creation flag
         *
         * Wraps @type_vk_keyword{DeviceCreateFlagBits}.
         * @see @ref Flags, @ref DeviceCreateInfo(DeviceProperties&, const ExtensionProperties*, Flags)
         */
        enum class Flag: UnsignedInt {
            /* Any magnum-specific flags added here have to be filtered out
               when passing them to _info.flags in the constructor. Using the
               highest bits in a hope to prevent conflicts with Vulkan instance
               flags added in the future. */

            /**
             * Don't implicitly enable any extensions.
             *
             * By default, the engine enables various extensions such as
             * @vk_extension{KHR,get_memory_requirements2} to provide a broader
             * functionality. If you want to have a complete control over what
             * gets enabled, set this flag.
             *
             * This flag also affects enabling of
             * @vk_extension{KHR,portability_subset},which is *required* to be
             * enabled by the spec on any device that advertises it, and
             * behavior of related @ref DeviceFeatures. See
             * @ref Vk-Device-portability-subset for details.
             */
            NoImplicitExtensions = 1u << 31
        };

        /**
         * @brief Device creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{DeviceCreateFlags}.
         * @see @ref DeviceCreateInfo(DeviceProperties&, const ExtensionProperties*, Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param deviceProperties  A device to use
         * @param extensionProperties Existing @ref ExtensionProperties
         *      instance for querying available Vulkan extensions. If
         *      @cpp nullptr @ce, a new instance may be created internally if
         *      needed. If a r-value is passed, the instance is later available
         *      through @ref Device::properties().
         * @param flags             Device creation flags
         *
         * The following @type_vk{DeviceCreateInfo} fields are pre-filled in
         * addition to `sType`:
         *
         * -    `flags`
         *
         * You need to call at least @ref addQueues() for a valid setup.
         */
        explicit DeviceCreateInfo(DeviceProperties& deviceProperties, const ExtensionProperties* extensionProperties, Flags flags = {});

        /** @overload */
        explicit DeviceCreateInfo(DeviceProperties& deviceProperties, Flags flags = {}): DeviceCreateInfo{deviceProperties, nullptr, flags} {}

        /**
         * @brief Construct with allowing to reuse already populated device properties
         *
         * Compared to @ref DeviceCreateInfo(DeviceProperties&, const ExtensionProperties*, Flags),
         * if the @ref Device is subsequently constructed via
         * @ref Device::Device(Instance&, DeviceCreateInfo&&), the
         * @p deviceProperties instance gets directly transferred to the
         * device, meaning @ref Device::properties() and any APIs relying on it
         * can reuse what was possibly already queried without having to repeat
         * the potentially complex queries second time.
         */
        explicit DeviceCreateInfo(DeviceProperties&& deviceProperties, const ExtensionProperties* extensionProperties, Flags flags = {});

        /** @overload */
        explicit DeviceCreateInfo(DeviceProperties&& deviceProperties, Flags flags = {}): DeviceCreateInfo{std::move(deviceProperties), nullptr, flags} {}

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit DeviceCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit DeviceCreateInfo(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo& info);

        /** @brief Copying is not allowed */
        DeviceCreateInfo(const DeviceCreateInfo&) = delete;

        /** @brief Move constructor */
        DeviceCreateInfo(DeviceCreateInfo&& other) noexcept;

        ~DeviceCreateInfo();

        /** @brief Copying is not allowed */
        DeviceCreateInfo& operator=(const DeviceCreateInfo&) = delete;

        /** @brief Move assignment */
        DeviceCreateInfo& operator=(DeviceCreateInfo&& other) noexcept;

        /* All the && overloads below are there in order to allow code like

            Device device{instance, DeviceCreateInfo{pickDevice(instance)}
                .addQueues(...)
                .addEnabledExtensions(...)
                ...
            };

           to work and correctly move the DeviceProperties to the Device.
           When adding new APIs, expand DeviceVkTest::createInfoRvalue() to
           verify everything still works. */

        /**
         * @brief Add enabled device extensions
         * @return Reference to self (for method chaining)
         *
         * All listed extensions are expected to be supported either globally
         * or in at least one of the enabled layers, use
         * @ref ExtensionProperties::isSupported() to check for their presence.
         *
         * The following @type_vk{DeviceCreateInfo} fields are set by this
         * function:
         *
         * -    `enabledExtensionCount` to the count of extensions added
         *      previously by this function plus @cpp extensions.size() @ce
         * -    `pEnabledExtensionNames` to an array containing all extension
         *      strings added previously by this function together with ones
         *      from @p extensions (doing a copy where needed)
         *
         * @note The function makes copies of string views that are not global
         *      or null-terminated, use the
         *      @link Containers::Literals::operator""_s() @endlink
         *      literal to prevent that where possible.
         */
        DeviceCreateInfo& addEnabledExtensions(Containers::ArrayView<const Containers::StringView> extensions) &;
        /** @overload */
        DeviceCreateInfo&& addEnabledExtensions(Containers::ArrayView<const Containers::StringView> extensions) &&;
        /** @overload */
        DeviceCreateInfo& addEnabledExtensions(std::initializer_list<Containers::StringView> extension) &;
        /** @overload */
        DeviceCreateInfo&& addEnabledExtensions(std::initializer_list<Containers::StringView> extension) &&;
        /** @overload */
        DeviceCreateInfo& addEnabledExtensions(Containers::ArrayView<const Extension> extensions) &;
        /** @overload */
        DeviceCreateInfo&& addEnabledExtensions(Containers::ArrayView<const Extension> extensions) &&;
        /** @overload */
        DeviceCreateInfo& addEnabledExtensions(std::initializer_list<Extension> extension) &;
        /** @overload */
        DeviceCreateInfo&& addEnabledExtensions(std::initializer_list<Extension> extension) &&;
        /** @overload */
        template<class ...E> DeviceCreateInfo& addEnabledExtensions() & {
            static_assert(Implementation::IsExtension<E...>::value, "expected only Vulkan device extensions");
            return addEnabledExtensions(std::initializer_list<Extension>{E{}...});
        }
        /** @overload */
        template<class ...E> DeviceCreateInfo&& addEnabledExtensions() && {
            addEnabledExtensions<E...>();
            return std::move(*this);
        }

        /**
         * @brief Add enabled device features
         * @return Reference to self (for method chaining)
         *
         * All enabled features are expected to be reported as supported by the
         * device and either their core version supported by the device or the
         * corresponding extension enabled via @ref addEnabledExtensions(). Use
         * @ref DeviceProperties::features() to check for feature support.
         *
         * If Vulkan 1.1 is not supported and
         * @vk_extension{KHR,get_physical_device_properties2} is not enabled on
         * the instance, the `pEnabledFeatures` field in
         * @type_vk{DeviceCreateInfo} is set to the Vulkan 1.0 subset of
         * @p features. Otherwise, depending on what features are enabled, a
         * subset of the following structures will be added to the `pNext`
         * chain:
         *
         * -    @type_vk_keyword{PhysicalDeviceProtectedMemoryFeatures} (Vulkan
         *      1.1)
         * -    @type_vk_keyword{PhysicalDeviceMultiviewFeatures} (Vulkan 1.1,
         *      @vk_extension{KHR,multiview})
         * -    @type_vk_keyword{PhysicalDeviceShaderDrawParametersFeatures}
         *      (Vulkan 1.1, @vk_extension{KHR,shader_draw_parameters})
         * -    @type_vk_keyword{PhysicalDeviceTextureCompressionASTCHDRFeaturesEXT}
         *      (@vk_extension{EXT,texture_compression_astc_hdr})
         * -    @type_vk_keyword{PhysicalDeviceShaderFloat16Int8Features}
         *      (Vulkan 1.2, @vk_extension{KHR,shader_float16_int8})
         * -    @type_vk_keyword{PhysicalDevice16BitStorageFeatures} (Vulkan
         *      1.1, @vk_extension{KHR,16bit_storage})
         * -    @type_vk_keyword{PhysicalDeviceImagelessFramebufferFeatures}
         *      (Vulkan 1.2, @vk_extension{KHR,imageless_framebuffer})
         * -    @type_vk_keyword{PhysicalDeviceVariablePointersFeatures}
         *      (Vulkan 1.1, @vk_extension{KHR,variable_pointers})
         * -    @type_vk_keyword{PhysicalDeviceAccelerationStructureFeaturesKHR}
         *      (@vk_extension{KHR,acceleration_structure})
         * -    @type_vk_keyword{PhysicalDeviceSamplerYcbcrConversionFeatures}
         *      (Vulkan 1.1, @vk_extension{KHR,sampler_ycbcr_conversion})
         * -    @type_vk_keyword{PhysicalDeviceDescriptorIndexingFeatures}
         *      (Vulkan 1.2, @vk_extension{EXT,descriptor_indexing})
         * -    @type_vk_keyword{PhysicalDevicePortabilitySubsetFeaturesKHR}
         *      (@vk_extension{KHR,portability_subset})
         * -    @type_vk_keyword{PhysicalDeviceShaderSubgroupExtendedTypesFeatures}
         *      (Vulkan 1.2, @vk_extension{KHR,shader_subgroup_extended_types})
         * -    @type_vk_keyword{PhysicalDevice8BitStorageFeatures} (Vulkan
         *      1.2, @vk_extension{KHR,8bit_storage})
         * -    @type_vk_keyword{PhysicalDeviceShaderAtomicInt64Features}
         *      (Vulkan 1.2, @vk_extension{KHR,shader_atomic_int64})
         * -    @type_vk_keyword{PhysicalDeviceVertexAttributeDivisorFeaturesEXT}
         *      (@vk_extension{EXT,vertex_attribute_divisor})
         * -    @type_vk_keyword{PhysicalDeviceTimelineSemaphoreFeatures}
         *      (Vulkan 1.2, @vk_extension{KHR,timeline_semaphore})
         * -    @type_vk_keyword{PhysicalDeviceVulkanMemoryModelFeatures}
         *      (Vulkan 1.2, @vk_extension{KHR,vulkan_memory_model})
         * -    @type_vk_keyword{PhysicalDeviceScalarBlockLayoutFeatures}
         *      (Vulkan 1.2, @vk_extension{EXT,scalar_block_layout})
         * -    @type_vk_keyword{PhysicalDeviceSeparateDepthStencilLayoutsFeatures}
         *      (Vulkan 1.2, @vk_extension{KHR,separate_depth_stencil_layouts})
         * -    @type_vk_keyword{PhysicalDeviceUniformBufferStandardLayoutFeatures}
         *      (Vulkan 1.2, @vk_extension{KHR,uniform_buffer_standard_layout})
         * -    @type_vk_keyword{PhysicalDeviceBufferDeviceAddressFeatures}
         *      (Vulkan 1.2, @vk_extension{KHR,buffer_device_address})
         * -    @type_vk_keyword{PhysicalDeviceHostQueryResetFeatures} (Vulkan
         *      1.2, @vk_extension{EXT,host_query_reset})
         * -    @type_vk_keyword{PhysicalDeviceIndexTypeUint8FeaturesEXT}
         *      (@vk_extension{EXT,index_type_uint8})
         * -    @type_vk_keyword{PhysicalDeviceExtendedDynamicStateFeaturesEXT}
         *      (@vk_extension{EXT,extended_dynamic_state})
         * -    @type_vk_keyword{PhysicalDeviceRobustness2FeaturesEXT}
         *      (@vk_extension{EXT,robustness2})
         * -    @type_vk_keyword{PhysicalDeviceImageRobustnessFeaturesEXT}
         *      (@vk_extension{EXT,image_robustness})
         * -    @type_vk_keyword{PhysicalDeviceRayTracingPipelineFeaturesKHR}
         *      (@vk_extension{KHR,ray_tracing_pipeline})
         * -    @type_vk_keyword{PhysicalDeviceRayQueryFeaturesKHR}
         *      (@vk_extension{KHR,ray_query})
         */
        DeviceCreateInfo& setEnabledFeatures(const DeviceFeatures& features) &;
        /** @overload */
        DeviceCreateInfo&& setEnabledFeatures(const DeviceFeatures& features) &&;

        /**
         * @brief Add queues
         * @param[in] family        Family index, smaller than
         *      @ref DeviceProperties::queueFamilyCount()
         * @param[in] priorities    Queue priorities. Size of the array implies
         *      how many queues to add and has to be at least one.
         * @param[out] output       Where to save resulting queues once the
         *      device is created. Has to have the same sizes as @p priorities.
         * @return Reference to self (for method chaining)
         *
         * At least one queue has to be added.
         *
         * The following @type_vk{DeviceCreateInfo} fields are set by this
         * function:
         *
         * -    `queueCreateInfoCount` to the count of queues added previously
         *      by this function plus @cpp queues.size() @ce
         * -    `pQueueCreateInfos` to an array containing all queue create
         *      infos added previously by this function together with ones
         *      from @p family and @p priorities
         *
         * @see @ref DeviceProperties::pickQueueFamily()
         * @todoc link to addQueues(QueueFlags) once doxygen finally GROWS UP
         *      and can link to &-qualified functions FFS
         */
        DeviceCreateInfo& addQueues(UnsignedInt family, Containers::ArrayView<const Float> priorities, Containers::ArrayView<const Containers::Reference<Queue>> output) &;
        /** @overload */
        DeviceCreateInfo&& addQueues(UnsignedInt family, Containers::ArrayView<const Float> priorities, Containers::ArrayView<const Containers::Reference<Queue>> output) &&;
        /** @overload */
        DeviceCreateInfo& addQueues(UnsignedInt family, std::initializer_list<Float> priorities, std::initializer_list<Containers::Reference<Queue>> output) &;
        /** @overload */
        DeviceCreateInfo&& addQueues(UnsignedInt family, std::initializer_list<Float> priorities, std::initializer_list<Containers::Reference<Queue>> output) &&;

        /**
         * @brief Add queues of family matching given flags
         *
         * Equivalent to picking a queue family first using
         * @ref DeviceProperties::pickQueueFamily() based on @p flags and then
         * calling the above @ref addQueues() variant with the family index.
         *
         * Note that @ref DeviceProperties::pickQueueFamily() exits in case it
         * doesn't find any family satisfying given @p flags --- for a
         * failproof scenario you may want to go with the above overload and
         * @ref DeviceProperties::tryPickQueueFamily() instead.
         * @todoc link to addQueues(UnsignedInt) above once doxygen finally
         *      GROWS UP and can link to &-qualified functions FFS
         */
        DeviceCreateInfo& addQueues(QueueFlags flags, Containers::ArrayView<const Float> priorities, Containers::ArrayView<const Containers::Reference<Queue>> output) &;
        /** @overload */
        DeviceCreateInfo&& addQueues(QueueFlags flags, Containers::ArrayView<const Float> priorities, Containers::ArrayView<const Containers::Reference<Queue>> output) &&;
        /** @overload */
        DeviceCreateInfo& addQueues(QueueFlags flags, std::initializer_list<Float> priorities, std::initializer_list<Containers::Reference<Queue>> output) &;
        /** @overload */
        DeviceCreateInfo&& addQueues(QueueFlags flags, std::initializer_list<Float> priorities, std::initializer_list<Containers::Reference<Queue>> output) &&;

        /**
         * @brief Add queues using raw info
         * @return Reference to self (for method chaining)
         *
         * Compared to @ref addQueues() this allows you to specify additional
         * queue properties using the `pNext` chain. The info is used as-is,
         * with all pointers expected to stay in scope until device creation.
         *
         * The following @type_vk{DeviceCreateInfo} fields are set by this
         * function:
         *
         * -    `queueCreateInfoCount` to the count of queues added previously
         *      by this function plus @cpp 1 @ce
         * -    `pQueueCreateInfos` to an array containing all queue create
         *      infos added previously by this function together with @p info
         */
        DeviceCreateInfo& addQueues(const VkDeviceQueueCreateInfo& info) &;
        /** @overload */
        DeviceCreateInfo&& addQueues(const VkDeviceQueueCreateInfo& info) &&;

        /** @brief Underlying @type_vk{DeviceCreateInfo} structure */
        VkDeviceCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkDeviceCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkDeviceCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkDeviceCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkDeviceCreateInfo*() const { return &_info; }

    private:
        friend Device;

        VkPhysicalDevice _physicalDevice;
        VkDeviceCreateInfo _info;
        struct State;
        Containers::Pointer<State> _state;
};

CORRADE_ENUMSET_OPERATORS(DeviceCreateInfo::Flags)

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/Device.h"

#endif
