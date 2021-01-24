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

#include "ExtensionProperties.h"

#include <algorithm>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/Version.h"

namespace Magnum { namespace Vk {

ExtensionProperties::ExtensionProperties(NoCreateT) {}

ExtensionProperties::ExtensionProperties(ExtensionProperties&&) noexcept = default;

ExtensionProperties::~ExtensionProperties() = default;

ExtensionProperties& ExtensionProperties::operator=(ExtensionProperties&&) noexcept = default;

ExtensionProperties::ExtensionProperties(const Containers::ArrayView<const Containers::StringView> layers, VkResult(*const enumerator)(void*, const char*, UnsignedInt*, VkExtensionProperties*), void* const state) {
    /* Retrieve total extension count for all layers + the global extensions */
    std::size_t totalCount = 0;
    for(std::size_t i = 0; i <= layers.size(); ++i) {
        UnsignedInt count;
        MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(enumerator(state,
            i == 0 ? nullptr :
                Containers::String::nullTerminatedView(layers[i - 1]).data(),
            &count, nullptr));

        totalCount += count;
    }

    /* Allocate extra for a list of string views that we'll use to sort &
       search the values and a layer index so we can map the extensions back
       to which layer they come from */
    _data = Containers::ArrayTuple{
        {NoInit, totalCount, _extensions},
        {totalCount, _names},
        {NoInit, totalCount, _extensionLayers}
    };

    /* Query the extensions, save layer ID for each */
    std::size_t offset = 0;
    for(std::size_t i = 0; i <= layers.size(); ++i) {
        UnsignedInt count = totalCount - offset;
        MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(enumerator(state,
            i == 0 ? nullptr :
                Containers::String::nullTerminatedView(layers[i - 1]).data(),
            &count, _extensions + offset));
        for(std::size_t j = 0; j != count; ++j) _extensionLayers[offset + j] = i;
        offset += count;
    }

    /* Expect the total extension count didn't change between calls. For some
       reason, when using LunarG vkconfig to add VK_LAYER_KHRONOS_validation
       to implicit layers, on NVidia Windows drivers the device extension count
       (reported with nullptr pProperties) is one extension more than when
       querying the actual data (124 vs 123). I suspect the driver (or the
       loader (or the layer)) is doing some deduplication in one case but not
       in the other. The offending extension is possibly VK_EXT_tooling_info
       that seems to be included in "core" extensions even without the
       validation layer enabled.

       I couldn't reproduce on Mesa / Intel, but I suspect this is more
       widespread, so I didn't want to do just a silent fix. OTOH using the
       driver workaround framework (and listing this among used workarounds
       instead of a warning that can't be disabled) is problematic because the
       bug can't be reliably discovered at instance creation without doing the
       costly enumeration. */
    if(offset < totalCount) {
        Warning{} << "Vk::ExtensionProperties: inconsistent extension count reported by the driver, expected" << totalCount << "but got only" << offset;
        _names = {_names, offset};
        _extensionLayers = {_extensionLayers, offset};
    } else CORRADE_INTERNAL_ASSERT(offset == totalCount);

    /* Populate the views, sort them and remove duplicates so we can search in
       O(log n) later */
    for(std::size_t i = 0; i != _names.size(); ++i)
        _names[i] = _extensions[i].extensionName;
    std::sort(_names.begin(), _names.end());
    _names = _names.prefix(std::unique(_names.begin(), _names.end()));
}

Containers::ArrayView<const Containers::StringView> ExtensionProperties::names() const {
    return _names;
}

bool ExtensionProperties::isSupported(const Containers::StringView extension, const UnsignedInt revision) const {
    /* Thanks, C++, for forcing me to have a larger bug surface instead of
       providing a library helper to find the damn thing. */
    auto found = std::lower_bound(_names.begin(), _names.end(), extension);

    /* The view target is contents of the VkExtensionProperties structure,
       the revision is stored nearby */
    return found != _names.end() && *found == extension && reinterpret_cast<const VkExtensionProperties*>(found->data() - offsetof(VkExtensionProperties, extensionName))->specVersion >= revision;
}

bool ExtensionProperties::isSupported(const Extension& extension, const UnsignedInt revision) const {
    return isSupported(extension.string(), revision);
}

Containers::StringView ExtensionProperties::name(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _extensions.size(),
        "Vk::ExtensionProperties::name(): index" << id << "out of range for" << _extensions.size() << "entries", {});
    /* Not returning the string views at the end because those are in a
       different order */
    return _extensions[id].extensionName;
}

UnsignedInt ExtensionProperties::revision(const Extension& extension) const {
    return revision(extension.string());
}

UnsignedInt ExtensionProperties::revision(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _extensions.size(),
        "Vk::ExtensionProperties::revision(): index" << id << "out of range for" << _extensions.size() << "entries", {});
    /* WTF, why VkLayerProperties::specVersion is an actual Vulkan version and
       here it is a revision number?! Consistency my ass. */
    return _extensions[id].specVersion;
}

UnsignedInt ExtensionProperties::revision(const Containers::StringView extension) const {
    /* Thanks, C++, for forcing me to have a larger bug surface instead of
       providing a library helper to find the damn thing. */
    auto found = std::lower_bound(_names.begin(), _names.end(), extension);
    if(found == _names.end() || *found != extension) return 0;

    /* The view target is contents of the VkExtensionProperties structure,
       the revision is stored nearby */
    return reinterpret_cast<const VkExtensionProperties*>(found->data() - offsetof(VkExtensionProperties, extensionName))->specVersion;
}

UnsignedInt ExtensionProperties::layer(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _extensions.size(),
        "Vk::xtensionProperties::layer(): index" << id << "out of range for" << _extensions.size() << "entries", {});
    return _extensionLayers[id];
}

InstanceExtensionProperties::InstanceExtensionProperties(InstanceExtensionProperties&&) noexcept = default;

InstanceExtensionProperties::~InstanceExtensionProperties() = default;

InstanceExtensionProperties& InstanceExtensionProperties::operator=(InstanceExtensionProperties&&) noexcept = default;

bool InstanceExtensionProperties::isSupported(Containers::StringView extension, const UnsignedInt revision) const {
    return ExtensionProperties::isSupported(extension, revision);
}

bool InstanceExtensionProperties::isSupported(const InstanceExtension& extension, const UnsignedInt revision) const {
    return isSupported(extension.string(), revision);
}

UnsignedInt InstanceExtensionProperties::revision(Containers::StringView extension) const {
    return ExtensionProperties::revision(extension);
}

UnsignedInt InstanceExtensionProperties::revision(const InstanceExtension& extension) const {
    return revision(extension.string());
}

InstanceExtensionProperties enumerateInstanceExtensionProperties(const Containers::ArrayView<const Containers::StringView> layers) {
    return InstanceExtensionProperties{layers, [](void*, const char* const layer, UnsignedInt* count, VkExtensionProperties* properties) {
        return vkEnumerateInstanceExtensionProperties(layer, count, properties);
    }, nullptr};
}

InstanceExtensionProperties enumerateInstanceExtensionProperties(const std::initializer_list<Containers::StringView> layers) {
    return enumerateInstanceExtensionProperties(Containers::arrayView(layers));
}

}}
