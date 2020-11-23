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

Wraps a @type_vk_keyword{DeviceCreateInfo}. See @ref Device for usage
information.
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

        ~DeviceCreateInfo();

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
         * The function makes copies of string views that are not owning or
         * null-terminated, use the @link Containers::Literals::operator""_s() @endlink
         * literal to prevent that where possible.
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
         * queue properties using the `pNext` chain. The info is uses as-is,
         * with all pointers expected to stay in scope until device creation.
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

/**
@brief Device
@m_since_latest

Wraps a @type_vk_keyword{Device} and stores device-specific Vulkan function
pointers.

@section Vk-Device-usage Usage

With an @ref Instance ready, a device has to be picked first. Commonly it's
done by calling @ref pickDevice() and letting the library choose. This
selection is affected by the `--magnum-device`
@ref Vk-Device-command-line "command-line option", giving the end users an
ability to pick a particular device, choose a discrete or integrated GPU or
even a software implementation. If the application needs something specific,
you can use @ref enumerateDevices() instead, pick a device from the list
manually, provide the users with a list to choose from etc.

The picked device is then passed to @ref DeviceCreateInfo. At the very least
you'll also need to set up queues, as every Vulkan device needs at least one.
That's done by creating an empty @ref Queue instance and then referencing it
from @ref DeviceCreateInfo::addQueues(). After the device is constructed, the
queue gets populated and is ready to be used.

@snippet MagnumVk.cpp Device-usage-construct-queue

In the above snippet, we requested a graphics queue via a convenience API. The
information about available queues and other device properties is stored in a
@ref DeviceProperties that got returned from @ref pickDevice() and
@ref DeviceCreateInfo called @ref DeviceProperties::pickQueueFamily() for us.
As with device picking, you can also iterate through all
@ref DeviceProperties::queueFamilyCount() and choose one manually.

Same as with @ref Instance, the above won't enable any additional extensions
except for what the engine itself needs or what's supplied on the command line. Use @ref DeviceCreateInfo::addEnabledExtensions() to enable them, you can use
both string names as well as predefined *device* extensions from the
@ref Extensions namespace. Later on, presence of predefined extensions can be
checked with @ref isExtensionEnabled().

@snippet MagnumVk.cpp Device-usage-extensions

Usually you'll be first checking for extension availability instead, which is
again accessible through the @ref DeviceProperties instance:

@snippet MagnumVk.cpp Device-usage-check-supported

With both @ref Instance and @ref Device created, you can proceed to setting up
a @ref CommandPool.

@see @ref vulkan-wrapping-optimizing-properties

@section Vk-Device-command-line Command-line options

The @ref Device inherits a subset of the
@ref Vk-Instance-command-line "Instance command-line options", in particular
the following. If the @ref Instance didn't get `argc` / `argv` passed, only the
environment variables are used.

-   `--magnum-disable-extensions LIST` --- Vulkan instance or device extensions
    to disable, meaning @ref DeviceCreateInfo::addEnabledExtensions() will skip
    them (environment: `MAGNUM_DISABLE_EXTENSIONS`)
-   `--magnum-enable-extensions LIST` --- Vulkan device extensions to enable in
    addition to @ref DeviceCreateInfo defaults and what the application
    requests (environment: `MAGNUM_ENABLE_EXTENSIONS`)
-   `--magnum-vulkan-version X.Y` --- force @ref Device Vulkan version instead
    of using what the device reports as supported, affecting what entrypoints
    and extensions get used (environment: `MAGNUM_VULKAN_VERSION`)
-   `--magnum-log default|quiet|verbose` --- console logging (environment:
    `MAGNUM_LOG`) (default: `default`)
-   `--magnum-device ID|integrated|discrete|virtual|cpu` --- device ID or kind
    to pick in @ref pickDevice(); if a device is selected through
    @ref enumerateDevices() or any other way, this option has no effect
    (environment: `MAGNUM_DEVICE`)

@section Vk-Device-raw Interaction with raw Vulkan code

In addition to the common properties explained in @ref vulkan-wrapping-raw,
the @ref Device contains device-level Vulkan function pointers, accessible
through @ref operator->():

@snippet MagnumVk.cpp Device-function-pointers

These functions are by default not accessible globally (and neither there is a
global "current instance"), which is done in order to avoid multiple
independent instances affecting each other. Sometimes it is however desirable
to have global function pointers --- for example when a 3rd party code needs to
operate on the same instance, or when writing quick prototype code --- and then
it's possible to populate those using @ref populateGlobalFunctionPointers().
Compared to the above, the same custom code would then look like this:

@snippet MagnumVk.cpp Device-global-function-pointers

Similarly you can use @ref Instance::populateGlobalFunctionPointers() to
populate instance-level global function pointers.
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
         * Note that this function retrieves all device-specific Vulkan
         * function pointers, which is a relatively costly operation. It's thus
         * not recommended to call this function repeatedly for creating
         * short-lived device instances, even though it's technically correct.
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
         * @ref DeviceCreateInfo::addQueues(), populating the @ref Queue
         * references.
         * @see @fn_vk_keyword{CreateDevice}, @fn_vk_keyword{GetDeviceQueue2},
         *      @fn_vk_keyword{GetDeviceQueue}
         * @todoc link to a concrete addQueues() overload above once doxygen
         *      finally GROWS UP and can link to &-qualified functions FFS
         */
        explicit Device(Instance& instance, const DeviceCreateInfo& info);

        /**
         * @brief Construct with reusing already populated device properties
         *
         * Compared to @ref Device(Instance&, const DeviceCreateInfo&), it can
         * take ownership of the @ref DeviceProperties added to @p info earlier
         * via @ref DeviceCreateInfo::DeviceCreateInfo(DeviceProperties&&, const ExtensionProperties*, Flags) or any of the other r-value-taking
         * constructors.
         *
         * With that, the @ref properties() getter and any APIs relying on it
         * can reuse what was possibly already queried without having to repeat
         * the potentially complex queries second time.
         */
        explicit Device(Instance& instance, DeviceCreateInfo&& info);

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

        /**
         * @brief Device properties
         *
         * If a r-value @ref DeviceProperties instance was propagated to
         * @ref DeviceCreateInfo and then to @ref Device, it's reused here.
         * Otherwise the contents are populated on first use.
         */
        DeviceProperties& properties() { return *_properties; }

        /**
         * @brief Version supported by the device
         *
         * Unless overriden using `--magnum-vulkan-version` on the
         * @ref Vk-Device-command-line "command line", corresponds to
         * @ref DeviceProperties::version().
         */
        Version version() const { return _version; }

        /**
         * @brief Whether given version is supported on the device
         *
         * Compares @p version against @ref version().
         */
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
         * code is able to call global device-level `vk*` functions. See
         * @ref Vk-Device-raw for more information.
         *
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

        /* Common guts for Device(Instance&, DeviceCreateInfo&) and
           Device(Instance&, DeviceCreateInfo&&) */
        explicit Device(Instance& isntance, const DeviceCreateInfo&, DeviceProperties&&);

        template<class T> MAGNUM_VK_LOCAL void initializeExtensions(Containers::ArrayView<const T> enabledExtensions);
        MAGNUM_VK_LOCAL void initialize(Instance& instance, Version version);

        MAGNUM_VK_LOCAL static void getQueueImplementationDefault(Device& self, const VkDeviceQueueInfo2& info, VkQueue& queue);
        MAGNUM_VK_LOCAL static void getQueueImplementation11(Device& self, const VkDeviceQueueInfo2& info, VkQueue& queue);

        VkDevice _handle;
        HandleFlags _flags;
        Version _version;
        Math::BoolVector<Implementation::ExtensionCount> _extensionStatus;
        Containers::Pointer<DeviceProperties> _properties;
        Containers::Pointer<Implementation::DeviceState> _state;

        /* This member is bigger than you might think */
        FlextVkDevice _functionPointers;
};

}}

#endif
