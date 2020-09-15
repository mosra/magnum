#ifndef Magnum_Vk_LayerProperties_h
#define Magnum_Vk_LayerProperties_h
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
 * @brief Class @ref Magnum::Vk::LayerProperties, function @ref Magnum::Vk::enumerateLayerProperties()
 * @m_since_latest
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Vulkan layer properties
@m_since_latest

Provides a searchable container of Vulkan layers enumerated with
@ref enumerateLayerProperties(). Only instance layers are enumerated, as device
layers are [deprecated since Vulkan 1.0.13](https://github.com/KhronosGroup/Vulkan-Docs/commit/2656f459333b3a1dc63619a9ebd83490eea22e93)
and the assumption is that no drivers currently use rely on these anymore. See
[§ 37.3.1](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap38.html#extendingvulkan-layers-devicelayerdeprecation)
for more information.

See the @ref Vk-Instance-usage "Instance class docs" for an example of using
this class for checking available layers before enabling them on an instance.

@see @ref ExtensionProperties, @ref enumerateInstanceExtensionProperties(),
    @type_vk_keyword{LayerProperties}
*/
class MAGNUM_VK_EXPORT LayerProperties {
    public:
        /**
         * @brief Construct without populating the contents
         *
         * Equivalent to a moved-from state. Move over the result of
         * @ref enumerateLayerProperties() to make it usable.
         */
        explicit LayerProperties(NoCreateT);

        ~LayerProperties();

        /** @brief Copying is not allowed */
        LayerProperties(const LayerProperties&) = delete;

        /** @brief Move constructor */
        LayerProperties(LayerProperties&&) noexcept;

        /** @brief Copying is not allowed */
        LayerProperties& operator=(const LayerProperties&) = delete;

        /** @brief Move assignment */
        LayerProperties& operator=(LayerProperties&&) noexcept;

        /**
         * @brief Layer names
         *
         * A list of all layers reported by the driver. Use @ref isSupported()
         * to query support of a particular layer name. Note that the list is
         * sorted and thus may be different than the order in which the
         * @ref name(), @ref revision(), @ref version() and @ref description()
         * accessors return values.
         *
         * The returned views are owned by the @ref LayerProperties instance
         * (i.e., *not* a global memory).
         */
        Containers::ArrayView<const Containers::StringView> names();

        /**
         * @brief Whether given layer is supported
         *
         * Search complexity is @f$ \mathcal{O}(\log n) @f$ in the total layer
         * count.
         */
        bool isSupported(Containers::StringView layer);

        /** @brief Count of layers reported by the driver */
        UnsignedInt count();

        /**
         * @brief Layer name
         * @param id Layer index, expected to be smaller than @ref count()
         *
         * The returned view is owned by the @ref LayerProperties instance
         * (i.e., *not* a global memory).
         */
        Containers::StringView name(UnsignedInt id);

        /**
         * @brief Layer revision
         * @param id Layer index, expected to be smaller than @ref count()
         */
        UnsignedInt revision(UnsignedInt id);

        /**
         * @brief Vulkan version the layer is implemented against
         * @param id Layer index, expected to be smaller than @ref count()
         */
        Version version(UnsignedInt id);

        /**
         * @brief Layer description
         * @param id Layer index, expected to be smaller than @ref count()
         *
         * The returned view is owned by the @ref LayerProperties instance
         * (i.e., *not* a global memory).
         */
        Containers::StringView description(UnsignedInt id);

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* The DAMN THING lists this among friends, which is AN IMPLEMENTATION
           DETAIL */
        friend MAGNUM_VK_EXPORT LayerProperties enumerateLayerProperties();
        #endif

        explicit LayerProperties();

        Containers::Array<VkLayerProperties> _layers;
};

/**
@brief Enumerate instance layers
@m_since_latest

@see @fn_vk_keyword{EnumerateInstanceLayerProperties}
*/
MAGNUM_VK_EXPORT LayerProperties enumerateLayerProperties();

}}

#endif
