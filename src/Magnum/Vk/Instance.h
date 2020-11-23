#ifndef Magnum_Vk_Instance_h
#define Magnum_Vk_Instance_h
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
 * @brief Class @ref Magnum::Vk::InstanceCreateInfo, @ref Magnum::Vk::Instance
 * @m_since_latest
 */

#include <Corrade/Containers/Pointer.h>

#include "Magnum/Tags.h"
#include "Magnum/Math/BoolVector.h"
#include "Magnum/Vk/TypeTraits.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation {
    enum: std::size_t { InstanceExtensionCount = 16 };

    struct InstanceState;
}

/**
@brief Instance creation info
@m_since_latest

Wraps a @type_vk_keyword{InstanceCreateInfo} and
@type_vk_keyword{ApplicationInfo}. See @ref Instance for usage information.
*/
class MAGNUM_VK_EXPORT InstanceCreateInfo {
    public:
        /**
         * @brief Instance creation flag
         *
         * Wraps @type_vk_keyword{InstanceCreateFlagBits}.
         * @see @ref Flags, @ref InstanceCreateInfo(Int, const char**, const LayerProperties*, const InstanceExtensionProperties*, Flags)
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
             * @vk_extension{KHR,get_physical_device_properties2} to provide a
             * broader functionality. If you want to have a complete control
             * over what gets enabled, set this flag.
             */
            NoImplicitExtensions = 1u << 31
        };

        /**
         * @brief Instance creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{InstanceCreateFlags}.
         * @see @ref InstanceCreateInfo(Int, const char**, const LayerProperties*, const InstanceExtensionProperties*, Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param argc          Command-line argument count. Can be @cpp 0 @ce.
         * @param argv          Command-line argument values. Can be
         *      @cpp nullptr @ce. If set, is expected to stay in scope for the
         *      whole instance lifetime.
         * @param layerProperties Existing @ref LayerProperties instance for
         *      querying available Vulkan layers. If @cpp nullptr @ce, a new
         *      instance may be created internally if needed.
         * @param extensionProperties Existing @ref InstanceExtensionProperties
         *      instance for querying available Vulkan extensions. If
         *      @cpp nullptr @ce, a new instance may be created internally if
         *      needed.
         * @param flags         Instance creation flags
         *
         * The following @type_vk{InstanceCreateInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `pApplicationInfo`
         * -    @cpp pApplicationInfo->apiVersion @ce to
         *      @ref enumerateInstanceVersion()
         * -    @cpp pApplicationInfo->engineName @ce to @cpp "Magnum" @ce
         */
        /* All those are implicit in order to allow writing
            Instance instance{{argc, argv}};
           It's a tradeoff between a completely verbose way and
            Instance instance{argc, argv};
           in which case all these overloads would need to be duplicated on
           Instance as well. */
        /*implicit*/ InstanceCreateInfo(Int argc, const char** argv, const LayerProperties* layerProperties, const InstanceExtensionProperties* const extensionProperties, Flags flags = {});

        /** @overload */
        /*implicit*/ InstanceCreateInfo(Int argc, char** argv, const LayerProperties* layerProperties, const InstanceExtensionProperties* extensionProperties, Flags flags = {}): InstanceCreateInfo{argc, const_cast<const char**>(argv), layerProperties, extensionProperties, flags} {}

        /** @overload */
        /*implicit*/ InstanceCreateInfo(Int argc, std::nullptr_t argv, const LayerProperties* layerProperties, const InstanceExtensionProperties* extensionProperties, Flags flags = {}): InstanceCreateInfo{argc, static_cast<const char**>(argv), layerProperties, extensionProperties, flags} {}

        /** @overload */
        /*implicit*/ InstanceCreateInfo(Int argc, const char** argv, Flags flags = {}): InstanceCreateInfo{argc, argv, nullptr, nullptr, flags} {}

        /** @overload */
        /*implicit*/ InstanceCreateInfo(Int argc, char** argv, Flags flags = {}): InstanceCreateInfo{argc, argv, nullptr, nullptr, flags} {}

        /** @overload */
        /*implicit*/ InstanceCreateInfo(Int argc, std::nullptr_t argv, Flags flags = {}): InstanceCreateInfo{argc, argv, nullptr, nullptr, flags} {}

        /** @overload */
        /*implicit*/ InstanceCreateInfo(Flags flags = {}): InstanceCreateInfo{0, nullptr, flags} {}

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit InstanceCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit InstanceCreateInfo(const VkInstanceCreateInfo& info);

        ~InstanceCreateInfo();

        /**
         * @brief Set application info
         * @return Reference to self (for method chaining)
         *
         * Use the @ref version() helper to create the @p version value. The
         * name is @cpp nullptr @ce by default.
         *
         * The function makes copies of string views that are not owning or
         * null-terminated, use the @link Containers::Literals::operator""_s() @endlink
         * literal to prevent that where possible.
         */
        InstanceCreateInfo& setApplicationInfo(Containers::StringView name, Version version);

        /**
         * @brief Add enabled layers
         * @return Reference to self (for method chaining)
         *
         * All listed layers are expected be supported, use
         * @ref LayerProperties::isSupported() to check for their presence. If
         * a particular layer is listed among `--magnum-disable-layers` in
         * @ref Vk-Instance-command-line "command-line options", it's not
         * added.
         *
         * The function makes copies of string views that are not owning or
         * null-terminated, use the @link Containers::Literals::operator""_s() @endlink
         * literal to prevent that where possible.
         */
        InstanceCreateInfo& addEnabledLayers(Containers::ArrayView<const Containers::StringView> layers);
        /** @overload */
        InstanceCreateInfo& addEnabledLayers(std::initializer_list<Containers::StringView> layers);

        /**
         * @brief Add enabled instance extensions
         * @return Reference to self (for method chaining)
         *
         * All listed extensions are expected to be supported either globally
         * or in at least one of the enabled layers, use
         * @ref InstanceExtensionProperties::isSupported() to check for their
         * presence. If a particular extension is listed among
         * `--magnum-disable-extensions` in
         * @ref Vk-Instance-command-line "command-line options", it's not
         * added.
         *
         * The function makes copies of string views that are not owning or
         * null-terminated, use the @link Containers::Literals::operator""_s() @endlink
         * literal to prevent that where possible.
         */
        InstanceCreateInfo& addEnabledExtensions(Containers::ArrayView<const Containers::StringView> extensions);
        /** @overload */
        InstanceCreateInfo& addEnabledExtensions(std::initializer_list<Containers::StringView> extension);
        /** @overload */
        InstanceCreateInfo& addEnabledExtensions(Containers::ArrayView<const InstanceExtension> extensions);
        /** @overload */
        InstanceCreateInfo& addEnabledExtensions(std::initializer_list<InstanceExtension> extension);
        /** @overload */
        template<class ...E> InstanceCreateInfo& addEnabledExtensions() {
            static_assert(Implementation::IsInstanceExtension<E...>::value, "expected only Vulkan instance extensions");
            return addEnabledExtensions({E{}...});
        }

        /** @brief Underlying @type_vk{InstanceCreateInfo} structure */
        VkInstanceCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkInstanceCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkInstanceCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkInstanceCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkInstanceCreateInfo*() const { return &_info; }

    private:
        friend Instance;

        VkInstanceCreateInfo _info;
        VkApplicationInfo _applicationInfo;
        struct State;
        Containers::Pointer<State> _state;
};

CORRADE_ENUMSET_OPERATORS(InstanceCreateInfo::Flags)

/**
@brief Instance
@m_since_latest

Wraps a @type_vk_keyword{Instance} and stores instance-specific Vulkan function
pointers.

@section Vk-Instance-usage Usage

While an @ref Instance can be default-constructed without much fuss, it's
recommended to pass a @ref InstanceCreateInfo with at least the `argc` / `argv`
pair, which allows you to use various `--magnum-*`
@ref Vk-Instance-command-line "command-line options":

@snippet MagnumVk.cpp Instance-usage-minimal

In addition to command-line arguments, setting application info isn't strictly
required either, but may be beneficial for the driver:

@snippet MagnumVk.cpp Instance-usage

<b></b>

@m_class{m-note m-success}

@par
    The above code uses the @link Containers::Literals::operator""_s() @endlink
    literal, which lets the library know that given string is global and
    null-terminated. Such strings then don't need to be copied internally to
    keep them in scope until they're consumed by Vulkan APIs. The same is
    recommended to do for extension and layer names where possible.

The above won't enable any additional layers or extensions except for what the
engine itself needs or what's supplied on the command line. Use
@ref InstanceCreateInfo::addEnabledLayers() and
@ref InstanceCreateInfo::addEnabledExtensions() to enable them, you can use
both string names as well as predefined *instance* extensions from the
@ref Extensions namespace. Later on, presence of predefined extensions can be
checked with @ref isExtensionEnabled().

@snippet MagnumVk.cpp Instance-usage-layers-extensions

However, with the above approach, if any layer or extension isn't available,
the instance creation will abort. The recommended workflow is thus first
checking layer and extension availability using @ref enumerateLayerProperties()
and @ref enumerateInstanceExtensionProperties():

@snippet MagnumVk.cpp Instance-usage-check-supported

Next step after creating a Vulkan instance is picking and creating a
@ref Device.

@see @ref vulkan-wrapping-optimizing-properties

@section Vk-Instance-command-line Command-line options

The @ref Instance is configurable through command-line options that are passed
through the @ref InstanceCreateInfo `argc` / `argv` parameters. If those are
not passed, only the environment variables are used. A subset of these options
is reused by a subsequently created @ref Device as well.

@code{.sh}
<application> [--magnum-help]
    [--magnum-disable-layers LIST]
    [--magnum-disable-extensions LIST]
    [--magnum-enable-layers LIST]
    [--magnum-enable-instance-extensions LIST]
    [--magnum-enable-extensions LIST]
    [--magnum-vulkan-version X.Y]
    [--magnum-log default|quiet|verbose]
    [--magnum-device ID|integrated|discrete|virtual|cpu] ...
@endcode

Arguments:

-   `...` --- main application arguments (see `-h` or `--help` for details)
-   `--magnum-help` --- display this help message and exit
-   `--magnum-disable-layers LIST` --- Vulkan layers to disable, meaning
    @ref InstanceCreateInfo::addEnabledLayers() will skip them (environment:
    `MAGNUM_DISABLE_LAYERS`)
-   `--magnum-disable-extensions LIST` --- Vulkan instance or device extensions
    to disable, meaning @ref InstanceCreateInfo::addEnabledExtensions() and
    @ref DeviceCreateInfo::addEnabledExtensions() will skip them (environment:
    `MAGNUM_DISABLE_EXTENSIONS`)
-   `--magnum-enable-layers LIST` --- Vulkan layers to enable in addition to
    @ref InstanceCreateInfo defaults and what the application requests
    (environment: `MAGNUM_ENABLE_LAYERS`)
-   `--magnum-enable-instance-extensions LIST` --- Vulkan instance extensions
    to enable in addition to @ref InstanceCreateInfo defaults and what the
    application requests (environment: `MAGNUM_ENABLE_INSTANCE_EXTENSIONS`)
-   `--magnum-enable-extensions LIST` --- Vulkan device extensions to enable in
    addition to @ref DeviceCreateInfo defaults and what the application
    requests (environment: `MAGNUM_ENABLE_EXTENSIONS`)
-   `--magnum-vulkan-version X.Y` --- force @ref Instance and @ref Device
    Vulkan version instead of using what the instance / device reports as
    supported, affecting what entrypoints and extensions get used (environment:
    `MAGNUM_VULKAN_VERSION`)
-   `--magnum-log default|quiet|verbose` --- console logging (environment:
    `MAGNUM_LOG`) (default: `default`)
-   `--magnum-device ID|integrated|discrete|virtual|cpu` --- device ID or kind
    to pick in @ref pickDevice(); if a device is selected through
    @ref enumerateDevices() or any other way, this option has no effect
    (environment: `MAGNUM_DEVICE`)

@section Vk-Instance-raw Interaction with raw Vulkan code

In addition to the common properties explained in @ref vulkan-wrapping-raw, the
@ref Instance contains instance-level Vulkan function pointers, accessible
through @ref operator->():

@snippet MagnumVk.cpp Instance-function-pointers

These functions are by default not accessible globally (and neither there is a
global "current instance"), which is done in order to avoid multiple
independent instances affecting each other. Sometimes it is however desirable
to have global function pointers --- for example when a 3rd party code needs to
operate on the same instance, or when writing quick prototype code --- and then
it's possible to populate those using @ref populateGlobalFunctionPointers().
Compared to the above, the same custom code would then look like this:

@snippet MagnumVk.cpp Instance-global-function-pointers

Similarly you can use @ref Device::populateGlobalFunctionPointers() to populate
device-level global function pointers.
*/
class MAGNUM_VK_EXPORT Instance {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param handle        The @type_vk{Instance} handle
         * @param version       Vulkan version that's assumed to be used on the
         *      instance
         * @param enabledExtensions Extensions that are assumed to be enabled
         *      on the instance
         * @param flags         Handle flags
         *
         * The @p handle is expected to be of an existing Vulkan instance. The
         * @p version and @p enabledExtensions parameters populate internal
         * info about supported version and extensions and will be reflected in
         * @ref isVersionSupported() and @ref isExtensionEnabled(), among other
         * things. If @p enabledExtensions empty, the instance will behave as
         * if no extensions were enabled.
         *
         * Note that this function retrieves all instance-specific Vulkan
         * function pointers, which is a relatively costly operation. It's thus
         * not recommended to call this function repeatedly for creating
         * short-lived instances, even though it's technically correct.
         *
         * Unlike an instance created using a constructor, the Vulkan instance
         * is by default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static Instance wrap(VkInstance handle, Version version, Containers::ArrayView<const Containers::StringView> enabledExtensions, HandleFlags flags = {});

        /** @overload */
        static Instance wrap(VkInstance handle, Version version, std::initializer_list<Containers::StringView> enabledExtensions, HandleFlags flags = {});

        /**
         * @brief Constructor
         *
         * @see @fn_vk_keyword{CreateInstance}
         */
        explicit Instance(const InstanceCreateInfo& info = InstanceCreateInfo{});

        /**
         * @brief Construct without creating the instance
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Instance(NoCreateT);

        /** @brief Copying is not allowed */
        Instance(const Instance&) = delete;

        /** @brief Move constructor */
        Instance(Instance&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{Instance} handle, unless the instance
         * was created using @ref wrap() without @ref HandleFlag::DestroyOnDestruction
         * specified.
         * @see @fn_vk_keyword{DestroyInstance}, @ref release()
         */
        ~Instance();

        /** @brief Copying is not allowed */
        Instance& operator=(const Instance&) = delete;

        /** @brief Move assignment */
        Instance& operator=(Instance&& other) noexcept;

        /** @brief Underlying @type_vk{Instance} handle */
        VkInstance handle() { return _handle; }
        /** @overload */
        operator VkInstance() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Version supported by the instance
         *
         * Unless overriden using `--magnum-vulkan-version` on the
         * @ref Vk-Device-command-line "command line", corresponds to
         * @ref enumerateInstanceVersion().
         */
        Version version() const { return _version; }

        /**
         * @brief Whether given version is supported on the instance
         *
         * Compares @p version against @ref version().
         */
        bool isVersionSupported(Version version) const {
            return _version >= version;
        }

        /**
         * @brief Whether given extension is enabled
         *
         * Accepts instance extensions from the @ref Extensions namespace,
         * listed also in the @ref vulkan-support "Vulkan support tables".
         * Search complexity is @f$ \mathcal{O}(1) @f$. Example usage:
         *
         * @snippet MagnumVk.cpp Instance-isExtensionEnabled
         *
         * Note that this returns @cpp true @ce only if given extension is
         * supported by the driver *and* it was enabled in
         * @ref InstanceCreateInfo when creating the @ref Instance. For
         * querying extension support before creating an instance use
         * @ref InstanceExtensionProperties::isSupported().
         */
        template<class E> bool isExtensionEnabled() const {
            static_assert(Implementation::IsInstanceExtension<E>::value, "expected a Vulkan instance extension");
            return _extensionStatus[E::InstanceIndex];
        }

        /** @overload */
        bool isExtensionEnabled(const InstanceExtension& extension) const;

        /**
         * @brief Instance-specific Vulkan function pointers
         *
         * Function pointers are implicitly stored per-instance, use
         * @ref populateGlobalFunctionPointers() to populate the global `vk*`
         * functions.
         */
        const FlextVkInstance& operator*() const { return _functionPointers; }
        /** @overload */
        const FlextVkInstance* operator->() const { return &_functionPointers; }

        /**
         * @brief Release the underlying Vulkan instance
         *
         * Releases ownership of the Vulkan instance and returns its handle so
         * @fn_vk{DestroyInstance} is not called on destruction. The internal
         * state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkInstance release();

        /**
         * @brief Populate global instance-level function pointers to be used with third-party code
         *
         * Populates instance-level global function pointers so third-party
         * code is able to call global instance-level `vk*` functions. See
         * @ref Vk-Instance-raw for more information.
         *
         * @attention This operation is changing global state. You need to
         *      ensure that this function is not called simultaenously from
         *      multiple threads and code using those function points is
         *      calling them with the same instance as the one returned by
         *      @ref handle().
         */
        void populateGlobalFunctionPointers();

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #endif
        Implementation::InstanceState& state() { return *_state; }

    private:
        template<class T> MAGNUM_VK_LOCAL void initializeExtensions(Containers::ArrayView<const T> enabledExtensions);
        MAGNUM_VK_LOCAL void initialize(Version version, Int argc, const char** argv);

        VkInstance _handle;
        HandleFlags _flags;
        Version _version;
        Math::BoolVector<Implementation::InstanceExtensionCount> _extensionStatus;
        Containers::Pointer<Implementation::InstanceState> _state;

        /* This member is bigger than you might think */
        FlextVkInstance _functionPointers;
};

}}

#endif
