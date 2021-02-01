#ifndef Magnum_Vk_Device_h
#define Magnum_Vk_Device_h
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
 * @brief Class @ref Magnum::Vk::Device
 * @m_since_latest
 */

#include <cstddef>
#include <Corrade/Containers/BigEnumSet.h>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Tags.h"
#include "Magnum/Math/BoolVector.h"
#include "Magnum/Vk/TypeTraits.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation {
    enum: std::size_t { ExtensionCount = 96 };

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
except for what the engine itself needs or what's supplied on the command line.
Use @ref DeviceCreateInfo::addEnabledExtensions() to enable them, you can use
both string names as well as predefined *device* extensions from the
@ref Extensions namespace. Later on, presence of predefined extensions can be
checked with @ref isExtensionEnabled().

@snippet MagnumVk.cpp Device-creation-extensions

In addition to extensions, you'll be usually enabling features as well. These
are all exposed in a giant @ref DeviceFeatures enum and you can simply OR them
together. Internally, those get translated to @type_vk{PhysicalDeviceFeatures2}
and related structures, features that are not exposed in the enum can be
enabled by adding a corresponding structure to the `pNext` chain. As with
extensions, the set of enabled features can be later checked with
@ref enabledFeatures().

@snippet MagnumVk.cpp Device-creation-features

However, usually you'll be checking for extension and feature availability
first, which is doable through
@ref DeviceProperties::enumerateExtensionProperties() and
@ref ExtensionProperties::isSupported() for extensions, and
@ref DeviceProperties::features() for features. In case of features you can
make use of the enum set operations and simply mask away features that are not
available --- however note that some features also require an extension to be
explicitly enabled.

@snippet MagnumVk.cpp Device-creation-check-supported

With both @ref Instance and @ref Device created, you can proceed to setting up
a @ref CommandPool.

@subsection Vk-Device-portability-subset Vulkan portability subset

To simplify porting to platforms with the Portability Subset, Magnum implicitly
enables the @vk_extension{KHR,portability_subset} extension on all devices that
advertise it, as required by the spec, so you don't need to handle that part.
This behavior can be disabled with
@ref DeviceCreateInfo::Flag::NoImplicitExtensions.

For portability-related @ref DeviceFeatures, on conformant Vulkan
implementations (which don't advertise @vk_extension{KHR,portability_subset})
these are all implicitly marked as supported in @ref DeviceProperties::features()
and then implicitly marked as enabled in @ref Device::enabledFeatures(),
independently of whether you enable them or not. On devices having only the
Portability Subset, the supported features are listed in
@ref DeviceProperties::features() but you're expected to manually enable them
on device creation --- that part is *not done implicitly* by the engine.

A workflow that supports both conformant and Portability Subset devices with a
single code path is outlined in the following snippet --- on device creation
you request features that you want (which is a no-op on conformant
implementations), and at runtime you query those features in appropriate cases
(which will be always @cpp true @ce on conformant implementations). As with
other features, all APIs that require a particular Portability Subset feature
are marked as such and also listed among others at @ref requires-vk-feature.

@snippet MagnumVk.cpp Device-creation-portability-subset

@see @ref vulkan-wrapping-optimizing-properties

@section Vk-Device-command-line Command-line options

The @ref Device inherits a subset of the
@ref Vk-Instance-command-line "Instance command-line options", in particular
the following. If the @ref Instance didn't get `argc` / `argv` passed, only the
environment variables are used.

-   `--magnum-disable-workarounds LIST` --- Vulkan driver workarounds to
    disable (see @ref vulkan-workarounds for detailed info) (environment:
    `MAGNUM_DISABLE_WORKAROUNDS`)
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

@section Vk-Device-disabled-move Disabled move and delayed device creation

Due to the way @ref Queue instances are populated on device creation, and
for safety reasons as all device-dependent objects internally have to keep a
pointer to the originating @ref Device to access Vulkan function pointers, the
@ref Device class is not movable. This leads to a difference compared to other
Vulkan object wrappers, where you can use the @ref NoCreate tag to construct an
empty instance (for example as a class member) and do a delayed creation by
moving a new instance over the empty one. Here you have to use the
@ref create() function instead:

@snippet MagnumVk.cpp Device-delayed-creation

Similar case is with @ref wrap() --- instead of being @cpp static @ce, you have
to call it on a @ref Device(NoCreateT) "NoCreate"'d instance. The @ref Instance
class behaves equivalently.
*/
class MAGNUM_VK_EXPORT Device {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param instance          Vulkan instance the device is created on
         * @param physicalDevice    Physical device the @type_vk{VkDevice} was
         *      created from. Used to populate @ref properties().
         * @param handle            The @type_vk{Device} handle
         * @param version           Vulkan version that's assumed to be used on
         *      the device
         * @param enabledExtensions Extensions that are assumed to be enabled
         *      on the device
         * @param enabledFeatures   Features that are assumed to be enabled on
         *      the device
         * @param flags             Handle flags
         *
         * <b></b>
         *
         * @m_class{m-note m-warning}
         *
         * @par
         *      Unlike with other Vulkan object wrappers, this isn't a
         *      @cpp static @ce function returning a new @ref Device, instead
         *      it's expected to be called on a @ref NoCreate "NoCreate"'d
         *      instance. See @ref Vk-Device-disabled-move for more
         *      information.
         *
         * The @p handle is expected to be originating from @p instance and
         * @p physicalDevice. The @p version, @p enabledExtensions and
         * @p enabledFeatures parameters populate internal info about supported
         * version, enabled extensions and enabled features and will be
         * reflected in @ref isVersionSupported(), @ref isExtensionEnabled()
         * and @ref enabledFeatures(), among other things. If
         * @p enabledExtensions / @p enabledFeatures is empty, the device will
         * behave as if no extensions / no features were enabled.
         *
         * @m_class{m-note m-danger}
         *
         * @par
         *      Due to the extension / feature list being outside of library
         *      control here, driver bug workarounds are not detected and
         *      enabled when using this function. Depending on bug severity,
         *      that may lead to crashes and unexpected behavior that wouldn't
         *      otherwise happen with a @ref Device created the usual way.
         *
         * Note that this function retrieves all device-specific Vulkan
         * function pointers, which is a relatively costly operation. It's thus
         * not recommended to call this function repeatedly for creating
         * short-lived device instances, even though it's technically correct.
         *
         * Unlike a device created using the constructor or @ref create(), the
         * Vulkan device is by default not deleted on destruction. Use @p flags
         * for different behavior.
         * @see @ref release()
         */
        void wrap(Instance& instance, VkPhysicalDevice physicalDevice, VkDevice handle, Version version, Containers::ArrayView<const Containers::StringView> enabledExtensions, const DeviceFeatures& enabledFeatures, HandleFlags flags = {});

        /** @overload */
        void wrap(Instance& instance, VkPhysicalDevice physicalDevice, VkDevice handle, Version version, std::initializer_list<Containers::StringView> enabledExtensions, const DeviceFeatures& enabledFeatures, HandleFlags flags = {});

        /**
         * @brief Constructor
         *
         * Equivalent to calling @ref Device(NoCreateT) followed by
         * @ref create(Instance&, const DeviceCreateInfo&).
         */
        explicit Device(Instance& instance, const DeviceCreateInfo& info);

        /**
         * @brief Construct, reusing already populated device properties
         *
         * Equivalent to calling @ref Device(NoCreateT) followed by
         * @ref create(Instance&, DeviceCreateInfo&&).
         */
        explicit Device(Instance& instance, DeviceCreateInfo&& info);

        /**
         * @brief Construct without creating the device
         *
         * Use @ref create() or @ref tryCreate() to create the device.
         */
        explicit Device(NoCreateT);

        /** @brief Copying is not allowed */
        Device(const Device&) = delete;

        /**
         * @brief Moving is not allowed
         *
         * See @ref Vk-Device-disabled-move for more information.
         */
        Device(Device&& other) = delete;

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

        /**
         * @brief Moving is not allowed
         *
         * See @ref Vk-Device-disabled-move for more information.
         */
        Device& operator=(Device&&) = delete;

        /** @brief Underlying @type_vk{Device} handle */
        VkDevice handle() { return _handle; }
        /** @overload */
        operator VkDevice() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Create a device
         * @param instance  Vulkan instance to create the device on
         * @param info      Device creation info
         *
         * Meant to be called on a @ref Device(NoCreateT) "NoCreate"'d
         * instance. After creating the device populates device-level
         * function pointers and runtime information about enabled extensions
         * and features based on @p info, and finally requests device queues
         * added via @ref DeviceCreateInfo::addQueues(), populating the
         * @ref Queue references.
         *
         * If device creation fails, a message is printed to error output and
         * the application exits --- if you need a different behavior, use
         * @ref tryCreate() instead.
         * @see @ref Device(Instance&, const DeviceCreateInfo&),
         *      @fn_vk_keyword{CreateDevice}, @fn_vk_keyword{GetDeviceQueue2},
         *      @fn_vk_keyword{GetDeviceQueue}
         * @todoc link to a concrete addQueues() overload above once doxygen
         *      finally GROWS UP and can link to &-qualified functions FFS
         */
        void create(Instance& instance, const DeviceCreateInfo& info);

        /**
         * @brief Create a device, reusing already populated device properties
         *
         * Compared to @ref create(Instance&, const DeviceCreateInfo&), it can
         * take ownership of the @ref DeviceProperties added to @p info earlier
         * via @ref DeviceCreateInfo::DeviceCreateInfo(DeviceProperties&&, const ExtensionProperties*, Flags)
         * or any of the other r-value-taking constructors.
         *
         * With that, the @ref properties() getter and any APIs relying on it
         * can reuse what was possibly already queried without having to repeat
         * the potentially complex queries second time.
         * @see @ref Device(Instance&, DeviceCreateInfo&&),
         *      @ref tryCreate(Instance&, DeviceCreateInfo&&)
         */
        void create(Instance& instance, DeviceCreateInfo&& info);

        /**
         * @brief Try to create a device
         *
         * Unlike @ref create(Instance&, const DeviceCreateInfo&), instead of
         * exiting on error, prints a message to error output and returns a
         * corresponding result value. On success returns @ref Result::Success.
         */
        Result tryCreate(Instance& instance, const DeviceCreateInfo& info);

        /**
         * @brief Try to create a device, reusing already populated device properties
         *
         * Unlike @ref create(Instance&, DeviceCreateInfo&&), instead of
         * exiting on error, prints a message to error output and returns a
         * corresponding result value. On success returns @ref Result::Success.
         */
        Result tryCreate(Instance& instance, DeviceCreateInfo&& info);

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
         * supported by the driver *and* it was enabled via
         * @ref DeviceCreateInfo::addEnabledExtensions(). For querying
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
         * @brief Features enabled on the device
         *
         * @see @ref DeviceCreateInfo::setEnabledFeatures(),
         *      @ref DeviceProperties::features()
         */
        const DeviceFeatures& enabledFeatures() const { return _enabledFeatures; }

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

        /* Common guts for tryCreate(Instance&, DeviceCreateInfo&) and
           tryCreate(Instance&, DeviceCreateInfo&&) */
        Result tryCreateInternal(Instance& isntance, const DeviceCreateInfo&, DeviceProperties&&);

        template<class T> MAGNUM_VK_LOCAL void initializeExtensions(Containers::ArrayView<const T> enabledExtensions);
        MAGNUM_VK_LOCAL void initialize(Instance& instance, Version version, Containers::Array<std::pair<Containers::StringView, bool>>& encounteredWorkarounds, const DeviceFeatures& enabledFeatures);

        MAGNUM_VK_LOCAL static void getQueueImplementationDefault(Device& self, const VkDeviceQueueInfo2& info, VkQueue& queue);
        MAGNUM_VK_LOCAL static void getQueueImplementation11(Device& self, const VkDeviceQueueInfo2& info, VkQueue& queue);

        VkDevice _handle;
        HandleFlags _flags;
        Version _version;
        Math::BoolVector<Implementation::ExtensionCount> _enabledExtensions;
        DeviceFeatures _enabledFeatures;
        Containers::Pointer<DeviceProperties> _properties;
        Containers::Pointer<Implementation::DeviceState> _state;

        /* This member is bigger than you might think */
        FlextVkDevice _functionPointers;
};

}}

#endif
