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

#include "ExtensionProperties.h"

#include <algorithm>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Assert.h>

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
    _extensions = Containers::Array<VkExtensionProperties>{
        reinterpret_cast<VkExtensionProperties*>(new char[totalCount*(sizeof(VkExtensionProperties) + sizeof(Containers::StringView) + sizeof(UnsignedInt))]),
        totalCount,
        [](VkExtensionProperties* data, std::size_t) {
            delete[] reinterpret_cast<char*>(data);
        }};
    Containers::ArrayView<Containers::StringView> extensionNames{reinterpret_cast<Containers::StringView*>(_extensions.end()), totalCount};
    Containers::ArrayView<UnsignedInt> extensionLayers{reinterpret_cast<UnsignedInt*>(extensionNames.end()), totalCount};

    /* Query the extensions, save layer ID for each */
    std::size_t offset = 0;
    for(std::size_t i = 0; i <= layers.size(); ++i) {
        UnsignedInt count = totalCount - offset;
        MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(enumerator(state,
            i == 0 ? nullptr :
                Containers::String::nullTerminatedView(layers[i - 1]).data(),
            &count, reinterpret_cast<VkExtensionProperties*>(_extensions.data()) + offset));
        for(std::size_t j = 0; j != count; ++j) extensionLayers[offset + j] = i;
        offset += count;
    }

    /* Expect the total extension count didn't change between calls */
    CORRADE_INTERNAL_ASSERT(offset == totalCount);

    /* Populate the views, sort them and remove duplicates so we can search in
       O(log n) later */
    for(std::size_t i = 0; i != extensionNames.size(); ++i)
        extensionNames[i] = _extensions[i].extensionName;
    std::sort(extensionNames.begin(), extensionNames.end());
    _uniqueExtensionCount = std::unique(extensionNames.begin(), extensionNames.end()) - extensionNames.begin();
}

Containers::ArrayView<const Containers::StringView> ExtensionProperties::names() const {
    return {reinterpret_cast<const Containers::StringView*>(_extensions.end()), _uniqueExtensionCount};
}

bool ExtensionProperties::isSupported(const Containers::StringView extension) const {
    return std::binary_search(
        reinterpret_cast<const Containers::StringView*>(_extensions.end()),
        reinterpret_cast<const Containers::StringView*>(_extensions.end()) + _uniqueExtensionCount,
        extension);
}

bool ExtensionProperties::isSupported(const Extension& extension) const {
    return isSupported(extension.string());
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
    /* Thanks, C++, for forcing me to do one more comparison than strictly
       necessary */
    auto found = std::lower_bound(
        reinterpret_cast<const Containers::StringView*>(_extensions.end()),
        reinterpret_cast<const Containers::StringView*>(_extensions.end()) + _uniqueExtensionCount,
        extension);
    if(*found != extension) return 0;

    /* The view target is contents of the VkExtensionProperties structure,
       the revision is stored nearby */
    return reinterpret_cast<const VkExtensionProperties*>(found->data() - offsetof(VkExtensionProperties, extensionName))->specVersion;
}

UnsignedInt ExtensionProperties::layer(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _extensions.size(),
        "Vk::xtensionProperties::layer(): index" << id << "out of range for" << _extensions.size() << "entries", {});
    return reinterpret_cast<const UnsignedInt*>(reinterpret_cast<const Containers::StringView*>(_extensions.end()) + _extensions.size())[id];
}

InstanceExtensionProperties::InstanceExtensionProperties(InstanceExtensionProperties&&) noexcept = default;

InstanceExtensionProperties::~InstanceExtensionProperties() = default;

InstanceExtensionProperties& InstanceExtensionProperties::operator=(InstanceExtensionProperties&&) noexcept = default;

bool InstanceExtensionProperties::isSupported(Containers::StringView extension) const {
    return ExtensionProperties::isSupported(extension);
}

bool InstanceExtensionProperties::isSupported(const InstanceExtension& extension) const {
    return isSupported(extension.string());
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
