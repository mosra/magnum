#ifndef Magnum_Vk_Device_h
#define Magnum_Vk_Device_h
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
 * @brief Class @ref Magnum::Vk::DeviceCreateInfo, @ref Magnum::Vk::Device
 * @m_since_latest
 */

#include <cstddef>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/Reference.h>

#include "Magnum/Tags.h"
#include "Magnum/Math/BoolVector.h"
#include "Magnum/Vk/TypeTraits.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation {
    enum: std::size_t { ExtensionCount = 72 };

    struct DeviceState;
}

/**
@brief Device creation info
@m_since_latest

Wraps a @type_vk_keyword{DeviceCreateInfo}.
@see @ref Device
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
         *      needed.
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
        explicit DeviceCreateInfo(DeviceProperties&& deviceProperties, const ExtensionProperties* extensionProperties, Flags flags = {}): DeviceCreateInfo{deviceProperties, extensionProperties, flags} {}

        /** @overload */
        explicit DeviceCreateInfo(DeviceProperties& deviceProperties, Flags flags = {}): DeviceCreateInfo{deviceProperties, nullptr, flags} {}

        /** @overload */
        explicit DeviceCreateInfo(DeviceProperties&& deviceProperties, Flags flags = {}): DeviceCreateInfo{std::move(deviceProperties), nullptr, flags} {}

        /**
         * @brief Construct for an implicitly picked device
         *
         * Calls @ref DeviceCreateInfo(DeviceProperties&, const ExtensionProperties*, Flags)
         * with a device picked from @p instance using @ref pickDevice().
         */
        explicit DeviceCreateInfo(Instance& instance, Flags flags = {});

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

        ~DeviceCreateInfo();

        /**
         * @brief Add enabled device extensions
         * @return Reference to self (for method chaining)
         *
         * All listed extensions are expected to be supported either globally
         * or in at least one of the enabled layers, use
         * @ref ExtensionProperties::isSupported() to check for their presence.
         *
         * The function makes copies of string views that are not owning or
         * null-terminated, use the @link Containers::Literals::operator""_s() @endlink
         * literal to prevent that where possible.
         */
        DeviceCreateInfo& addEnabledExtensions(Containers::ArrayView<const Containers::StringView> extensions);
        /** @overload */
        DeviceCreateInfo& addEnabledExtensions(std::initializer_list<Containers::StringView> extension);
        /** @overload */
        DeviceCreateInfo& addEnabledExtensions(Containers::ArrayView<const Extension> extensions);
        /** @overload */
        DeviceCreateInfo& addEnabledExtensions(std::initializer_list<Extension> extension);
        /** @overload */
        template<class ...E> DeviceCreateInfo& addEnabledExtensions() {
            static_assert(Implementation::IsExtension<E...>::value, "expected only Vulkan device extensions");
            return addEnabledExtensions({E{}...});
        }

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
         * @see @ref DeviceProperties::pickQueueFamily()
         */
        DeviceCreateInfo& addQueues(UnsignedInt family, Containers::ArrayView<const Float> priorities, Containers::ArrayView<const Containers::Reference<Queue>> output);
        /** @overload */
        DeviceCreateInfo& addQueues(UnsignedInt family, std::initializer_list<Float> priorities, std::initializer_list<Containers::Reference<Queue>> output);

        /**
         * @brief Add queues using raw info
         * @return Reference to self (for method chaining)
         *
         * Compared to @ref addQueues() this allows you to specify additional
         * queue properties using the `pNext` chain. The info is uses as-is,
         * with all pointers expected to stay in scope until device creation.
         */
        DeviceCreateInfo& addQueues(const VkDeviceQueueCreateInfo& info);

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

/**
@brief Device
@m_since_latest

Wraps a @type_vk_keyword{Device} and stores all device-specific function
pointers.
*/
class MAGNUM_VK_EXPORT Device {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param instance      Vulkan instance the device is created on
         * @param handle        The @type_vk{Device} handle
         * @param version       Vulkan version that's assumed to be used on the
         *      instance
         * @param enabledExtensions Extensions that are assumed to be enabled
         *      on the instance
         * @param flags         Handle flags
         *
         * The @p handle is expected to be originating from @p instance. The
         * @p version and @p enabledExtensions parameters populate internal
         * info about supported version and extensions and will be reflected in
         * @ref isVersionSupported() and @ref isExtensionEnabled(), among other
         * things. If @p enabledExtensions is empty, the device will behave as
         * if no extensions were enabled.
         *
         * Unlike a device created using a constructor, the Vulkan device is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static Device wrap(Instance& instance, VkDevice handle, Version version, Containers::ArrayView<const Containers::StringView> enabledExtensions, HandleFlags flags = {});

        /** @overload */
        static Device wrap(Instance& instance, VkDevice handle, Version version, std::initializer_list<Containers::StringView> enabledExtensions, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param instance  Vulkan instance to create the device on
         * @param info      Device creation info
         *
         * After creating the device requests device queues added via
         * @ref DeviceCreateInfo::addQueues(UnsignedInt, Containers::ArrayView<const Float>, Containers::ArrayView<const Containers::Reference<Queue>>), populating the @ref Queue references.
         * @see @fn_vk_keyword{CreateDevice}, @fn_vk_keyword{GetDeviceQueue2},
         *      @fn_vk_keyword{GetDeviceQueue}
         */
        explicit Device(Instance& instance, const DeviceCreateInfo& info);

        /**
         * @brief Construct without creating the device
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Device(NoCreateT);

        /** @brief Copying is not allowed */
        Device(const Device&) = delete;

        /** @brief Move constructor */
        Device(Device&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{Device} handle, unless the instance
         * was created using @ref wrap() without @ref HandleFlag::DestroyOnDestruction
         * specified.
         * @see @fn_vk_keyword{DestroyDevice}, @ref release()
         */
        ~Device();

        /** @brief Copying is not allowed */
        Device& operator=(const Device&) = delete;

        /** @brief Move assignment */
        Device& operator=(Device&& other) noexcept;

        /** @brief Underlying @type_vk{Device} handle */
        VkDevice handle() { return _handle; }
        /** @overload */
        operator VkDevice() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /** @brief Device version */
        Version version() const { return _version; }

        /** @brief Whether given version is supported on the device */
        bool isVersionSupported(Version version) const {
            return _version >= version;
        }

        /**
         * @brief Whether given extension is enabled
         *
         * Accepts device extensions from the @ref Extensions namespace, listed
         * also in the @ref vulkan-support "Vulkan support tables". Search
         * complexity is @f$ \mathcal{O}(1) @f$. Example usage:
         *
         * @snippet MagnumVk.cpp Device-isExtensionEnabled
         *
         * Note that this returns @cpp true @ce only if given extension is
         * supported by the driver *and* it was enabled in
         * @ref DeviceCreateInfo when creating the @ref Device. For querying
         * extension support before creating a device use
         * @ref ExtensionProperties::isSupported().
         */
        template<class E> bool isExtensionEnabled() const {
            static_assert(Implementation::IsExtension<E>::value, "expected a Vulkan device extension");
            return _extensionStatus[E::Index];
        }

        /** @overload */
        bool isExtensionEnabled(const Extension& extension) const;

        /**
         * @brief Device-specific Vulkan function pointers
         *
         * Function pointers are implicitly stored per-device, use
         * @ref populateGlobalFunctionPointers() to populate the global `vk*`
         * functions.
         */
        const FlextVkDevice& operator*() const { return _functionPointers; }
        /** @overload */
        const FlextVkDevice* operator->() const { return &_functionPointers; }

        /**
         * @brief Release the underlying Vulkan device
         *
         * Releases ownership of the Vulkan device and returns its handle so
         * @fn_vk{DestroyDevice} is not called on destruction. The internal
         * state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkDevice release();

        /**
         * @brief Populate global device-level function pointers to be used with third-party code
         *
         * Populates device-level global function pointers so third-party
         * code is able to call global device-level `vk*` functions:
         *
         * @snippet MagnumVk.cpp Device-global-function-pointers
         *
         * Use @ref Instance::populateGlobalFunctionPointers() to populate
         * instance-level global function pointers.
         * @attention This operation is changing global state. You need to
         *      ensure that this function is not called simultaenously from
         *      multiple threads and code using those function points is
         *      calling them with the same device as the one returned by
         *      @ref handle().
         */
        void populateGlobalFunctionPointers();

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #endif
        Implementation::DeviceState& state() { return *_state; }

    private:
        friend Implementation::DeviceState;

        template<class T> MAGNUM_VK_LOCAL void initializeExtensions(Containers::ArrayView<const T> enabledExtensions);
        MAGNUM_VK_LOCAL void initialize(Instance& instance, Version version);

        MAGNUM_VK_LOCAL static void getQueueImplementationDefault(Device& self, const VkDeviceQueueInfo2& info, VkQueue& queue);
        MAGNUM_VK_LOCAL static void getQueueImplementation11(Device& self, const VkDeviceQueueInfo2& info, VkQueue& queue);

        VkDevice _handle;
        HandleFlags _flags;
        Version _version;
        Math::BoolVector<Implementation::ExtensionCount> _extensionStatus;
        Containers::Pointer<Implementation::DeviceState> _state;

        /* This member is bigger than you might think */
        FlextVkDevice _functionPointers;
};

}}

#endif
