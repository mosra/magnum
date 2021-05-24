#ifndef Magnum_Vk_InstanceCreateInfo_h
#define Magnum_Vk_InstanceCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::InstanceCreateInfo
 * @m_since_latest
 */

#include <Corrade/Containers/Pointer.h>

#include "Magnum/Tags.h"
#include "Magnum/Vk/TypeTraits.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Instance creation info
@m_since_latest

Wraps a @type_vk_keyword{InstanceCreateInfo} and
@type_vk_keyword{ApplicationInfo}. See
@ref Vk-Instance-creation "Instance creation" for usage information.
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
         * The following @type_vk{InstanceCreateInfo} fields are set by this
         * function:
         *
         * -    `pApplicationInfo`
         * -    @cpp pApplicationInfo->pApplicationName @ce to @p name (a copy
         *      of it, if needed)
         * -    @cpp pApplicationInfo->applicationVersion @ce to @p version
         *
         * @note The function makes copies of string views that are not global
         *      or null-terminated, use the
         *      @link Containers::Literals::operator""_s() @endlink literal to
         *      prevent that where possible.
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
         * The following @type_vk{InstanceCreateInfo} fields are set by this
         * function:
         *
         * -    `enabledLayerCount` to the count of layers added previously by
         *      this function plus @cpp layers.size() @ce
         * -    `pEnabledLayerNames` to an array containing all layer strings
         *      added previously by this function together with ones from
         *      @p layers (doing a copy where needed)
         *
         * @note The function makes copies of string views that are not global
         *      or null-terminated, use the
         *      @link Containers::Literals::operator""_s() @endlink literal to
         *      prevent that where possible.
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
         * The following @type_vk{InstanceCreateInfo} fields are set by this
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
         *      @link Containers::Literals::operator""_s() @endlink literal to
         *      prevent that where possible.
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

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/Instance.h"

#endif
