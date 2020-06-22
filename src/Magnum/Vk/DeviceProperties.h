#ifndef Magnum_Vk_DeviceProperties_h
#define Magnum_Vk_DeviceProperties_h
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
 * @brief Class @ref Magnum::Vk::DeviceProperties, enum @ref Magnum::Vk::DeviceType, function @ref Magnum::Vk::enumerateDevices()
 * @m_since_latest
 */

#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/Reference.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation { struct InstanceState; }

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
@brief Physical device properties
@m_since_latest

Wraps a @type_vk_keyword{PhysicalDevice} along with its (lazy-populated)
properties.
@see @ref enumeratePhysicalDevices()
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

        /** @brief Underlying @type_vk{PhysicalDevice} handle */
        VkPhysicalDevice handle() { return _handle; }
        /** @overload */
        operator VkPhysicalDevice() { return _handle; }

        /**
         * @brief Raw device properties
         *
         * Populated lazily on first request. If Vulkan 1.1 or the
         * @vk_extension{KHR,get_physical_device_properties2} extension is not
         * enabled on the originating instance, only the Vulkan 1.0 subset of
         * device properties is queried, with the `pNext` member being
         * @cpp nullptr @ce.
         * @see @fn_vk_keyword{GetPhysicalDeviceProperties2},
         *      @fn_vk_keyword{GetPhysicalDeviceProperties}
         */
        const VkPhysicalDeviceProperties2& properties();

        /**
         * @brief API version
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        Version apiVersion() {
            return Version(properties().properties.apiVersion);
        }

        /**
         * @brief Driver version
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        Version driverVersion() {
            return Version(properties().properties.driverVersion);
        }

        /**
         * @brief Device type
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        DeviceType type() {
            return DeviceType(properties().properties.deviceType);
        }

        /**
         * @brief Device name
         *
         * Convenience access to @ref properties() internals, populated lazily
         * on first request.
         */
        Containers::StringView name();

    private:
        friend Implementation::InstanceState;

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* The DAMN THING lists this among friends, which is AN IMPLEMENTATION
           DETAIL */
        friend MAGNUM_VK_EXPORT Containers::Array<DeviceProperties> enumerateDevices(Instance&);
        #endif

        explicit DeviceProperties(Instance& instance, VkPhysicalDevice handle);

        MAGNUM_VK_LOCAL static void getPropertiesImplementationDefault(DeviceProperties& self, VkPhysicalDeviceProperties2& properties);
        MAGNUM_VK_LOCAL static void getPropertiesImplementationKHR(DeviceProperties& self, VkPhysicalDeviceProperties2& properties);
        MAGNUM_VK_LOCAL static void getPropertiesImplementation11(DeviceProperties& self, VkPhysicalDeviceProperties2& properties);

        /* Can't be a reference because of the NoCreate constructor */
        Instance* _instance;

        VkPhysicalDevice _handle;
        struct State;
        Containers::Pointer<State> _state;
};

/**
@brief Enumerate physical devices
@m_since_latest

@see @fn_vk_keyword{EnumeratePhysicalDevices}
*/
MAGNUM_VK_EXPORT Containers::Array<DeviceProperties> enumerateDevices(Instance& instance);

}}

#endif
