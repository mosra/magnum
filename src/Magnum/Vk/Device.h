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
 * @brief Class @ref Magnum::Vk::Device
 * @m_since_latest
 */

#include <cstddef>
#include <Corrade/Containers/Pointer.h>

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
@brief Device
@m_since_latest

Wraps a @type_vk_keyword{Device} and stores device-specific Vulkan function
pointers.

@section Vk-Device-creation Device creation

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

@snippet MagnumVk.cpp Device-creation-construct-queue

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

@snippet MagnumVk.cpp Device-creation-extensions

Usually you'll be first checking for extension availability instead, which is
again accessible through the @ref DeviceProperties instance:

@snippet MagnumVk.cpp Device-creation-check-supported

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
            return _enabledExtensions[E::Index];
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
         *      ensure that this function is not called simultaneously from
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
        Math::BoolVector<Implementation::ExtensionCount> _enabledExtensions;
        Containers::Pointer<DeviceProperties> _properties;
        Containers::Pointer<Implementation::DeviceState> _state;

        /* This member is bigger than you might think */
        FlextVkDevice _functionPointers;
};

}}

#endif
