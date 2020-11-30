#ifndef Magnum_Vk_ExtensionProperties_h
#define Magnum_Vk_ExtensionProperties_h
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
 * @brief Class @ref Magnum::Vk::ExtensionProperties, @ref Magnum::Vk::InstanceExtensionProperties, function @ref enumerateInstanceExtensionProperties()
 * @m_since_latest
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/Tags.h"
#include "Magnum/Magnum.h"
#include "Magnum/Vk/TypeTraits.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Extension properties
@m_since_latest

Provides a searchable container of Vulkan device extensions enumerated with
@ref DeviceProperties::enumerateExtensionProperties().

See the @ref Vk-Device-usage "Device class docs" for an example of using this
class for checking available extensions before enabling them on a device. See
@ref Vk-Instance-usage "Instance class docs" for the same but using
@ref InstanceExtensionProperties.

@see @type_vk_keyword{ExtensionProperties}
*/
class MAGNUM_VK_EXPORT ExtensionProperties {
    public:
        /**
         * @brief Construct without populating the contents
         *
         * Equivalent to a moved-from state. Move over the result of
         * @ref DeviceProperties::enumerateExtensionProperties() to make it
         * usable.
         */
        explicit ExtensionProperties(NoCreateT);

        /** @brief Copying is not allowed */
        ExtensionProperties(const ExtensionProperties&) = delete;

        /** @brief Move constructor */
        ExtensionProperties(ExtensionProperties&&) noexcept;

        ~ExtensionProperties();

        /** @brief Copying is not allowed */
        ExtensionProperties& operator=(const ExtensionProperties&) = delete;

        /** @brief Move assignment */
        ExtensionProperties& operator=(ExtensionProperties&&) noexcept;

        /**
         * @brief Instance extensions
         *
         * A list of all extension strings reported by the driver for all
         * layers passed to the constructor, with duplicates removed. Use
         * @ref isSupported() to query support of a particular
         * extension. Note that the list is sorted and thus may be different
         * than the order in which the @ref name() and @ref revision()
         * accessors return values.
         *
         * The returned views are owned by the
         * @ref ExtensionProperties instance (i.e., *not* a global
         * memory).
         */
        Containers::ArrayView<const Containers::StringView> names() const;

        /**
         * @brief Whether given extension is supported
         *
         * Accepts extensions from the @ref Extension namespace as a template
         * parameter. Use the other overloads to query support of a runtime
         * extension or a plain extension string.
         *
         * Search complexity is @f$ \mathcal{O}(\log n) @f$ in the total
         * extension count; in contrast extension queries on a created instance
         * are @f$ \mathcal{O}(1) @f$.
         * @see @ref revision()
         */
        bool isSupported(Containers::StringView extension) const;

        /** @overload */
        bool isSupported(const Extension& extension) const;

        /** @overload */
        template<class E> bool isSupported() const {
            static_assert(Implementation::IsExtension<E>::value, "expected a Vulkan deviceension");
            return isSupported(E::string());
        }

        /**
         * @brief Count of extensions reported by the driver for all layers
         *
         * The count includes potential duplicates when an extension is both
         * available globally and through a particular layer.
         */
        UnsignedInt count() const { return _extensions.size(); }

        /**
         * @brief Extension name
         * @param id Extension index, expected to be smaller than @ref count()
         *
         * The returned view is owned by the
         * @ref ExtensionProperties instance (i.e., *not* a global
         * memory).
         */
        Containers::StringView name(UnsignedInt id) const;

        /**
         * @brief Extension revision
         * @param id Extension index, expected to be smaller than @ref count()
         */
        UnsignedInt revision(UnsignedInt id) const;

        /**
         * @brief Revision of a particular extension name
         *
         * If the extension is not supported, returns @cpp 0 @ce, supported
         * extensions always have a non-zero revision. If the extension is
         * implemented by more than one layer, returns revision of the first
         * layer implementing it --- use @ref revision(UnsignedInt) const
         * to get revision of a concrete extension in a concrete layer.
         * @see @ref isSupported()
         */
        UnsignedInt revision(Containers::StringView extension) const;

        /** @overload */
        UnsignedInt revision(const Extension& extension) const;

        /** @overload */
        template<class E> UnsignedInt revision() const {
            static_assert(Implementation::IsExtension<E>::value, "expected a Vulkan device extension");
            return revision(E::string());
        }

        /**
         * @brief Extension layer index
         * @param id Extension index, expected to be smaller than @ref count()
         *
         * Returns ID of the layer the extension comes from. @cpp 0 @ce is
         * global extensions, @cpp 1 @ce is the first layer passed to
         * @ref enumerateInstanceExtensionProperties() and so on.
         */
        UnsignedInt layer(UnsignedInt id) const;

    private:
        friend class InstanceExtensionProperties;

        explicit ExtensionProperties(const Containers::ArrayView<const Containers::StringView> layers, VkResult(*enumerator)(void*, const char*, UnsignedInt*, VkExtensionProperties*), void* state);

        Containers::Array<VkExtensionProperties> _extensions;
        std::size_t _uniqueExtensionCount;
};

/**
@brief Instance extension properties
@m_since_latest

Provides a searchable container of Vulkan device extensions enumerated with
@ref enumerateInstanceExtensionProperties().
@see @ref ExtensionProperties, @ref LayerProperties
*/
class MAGNUM_VK_EXPORT InstanceExtensionProperties: public ExtensionProperties {
    public:
        /**
         * @brief Construct without populating the contents
         *
         * Equivalent to a moved-from state. Move over the result of
         * @ref enumerateInstanceExtensionProperties() to make it usable.
         */
        explicit InstanceExtensionProperties(NoCreateT): ExtensionProperties{NoCreate} {}

        /** @brief Copying is not allowed */
        InstanceExtensionProperties(const InstanceExtensionProperties&) = delete;

        /** @brief Move constructor */
        InstanceExtensionProperties(InstanceExtensionProperties&&) noexcept;

        ~InstanceExtensionProperties();

        /** @brief Copying is not allowed */
        InstanceExtensionProperties& operator=(const InstanceExtensionProperties&) = delete;

        /** @brief Move assignment */
        InstanceExtensionProperties& operator=(InstanceExtensionProperties&&) noexcept;

        /** @copydoc ExtensionProperties::isSupported(Containers::StringView) const */
        bool isSupported(Containers::StringView extension) const;

        /** @overload */
        bool isSupported(const InstanceExtension& extension) const;

        /** @overload */
        template<class E> bool isSupported() const {
            static_assert(Implementation::IsInstanceExtension<E>::value, "expected a Vulkan instance extension");
            return isSupported(E::string());
        }

        /** @copydoc ExtensionProperties::revision(UnsignedInt) const */
        UnsignedInt revision(UnsignedInt id) const {
            return ExtensionProperties::revision(id);
        }

        /** @copydoc ExtensionProperties::revision(Containers::StringView) const */
        UnsignedInt revision(Containers::StringView extension) const;

        /** @overload */
        UnsignedInt revision(const InstanceExtension& extension) const;

        /** @overload */
        template<class E> UnsignedInt revision() const {
            static_assert(Implementation::IsInstanceExtension<E>::value, "expected a Vulkan instance extension");
            return revision(E::string());
        }

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* The DAMN THING forgets parameter name if this is present, FFS. It
           also lists this among friends, which is AN IMPLEMENTATION DETAIL */
        friend DeviceProperties;
        friend MAGNUM_VK_EXPORT InstanceExtensionProperties enumerateInstanceExtensionProperties(Containers::ArrayView<const Containers::StringView>);
        #endif

        explicit InstanceExtensionProperties(const Containers::ArrayView<const Containers::StringView> layers, VkResult(*enumerator)(void*, const char*, UnsignedInt*, VkExtensionProperties*), void* state): ExtensionProperties{layers, enumerator, state} {}
};

/**
@brief Enumerate instance extension properties
@param layers        Additional layers to list extensions from
@m_since_latest

Expects that all listed layers are supported.
@see @ref LayerProperties::isSupported(),
    @fn_vk_keyword{EnumerateExtensionProperties}
*/
MAGNUM_VK_EXPORT InstanceExtensionProperties enumerateInstanceExtensionProperties(Containers::ArrayView<const Containers::StringView> layers = {});

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_VK_EXPORT InstanceExtensionProperties enumerateInstanceExtensionProperties(std::initializer_list<Containers::StringView> layers);

}}

#endif
