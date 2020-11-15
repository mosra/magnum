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

#include "LayerProperties.h"

#include <algorithm>
#include <Corrade/Containers/StringView.h>

#include "Magnum/Vk/Assert.h"

namespace Magnum { namespace Vk {

LayerProperties::LayerProperties(NoCreateT) {}

LayerProperties::LayerProperties() = default;

LayerProperties::LayerProperties(LayerProperties&&) noexcept = default;

LayerProperties::~LayerProperties() = default;

LayerProperties& LayerProperties::operator=(LayerProperties&&) noexcept = default;

Containers::ArrayView<const Containers::StringView> LayerProperties::names() {
    return {reinterpret_cast<const Containers::StringView*>(_layers.end()), _layers.size()};
}

bool LayerProperties::isSupported(const Containers::StringView layer) {
    return std::binary_search(
        reinterpret_cast<const Containers::StringView*>(_layers.end()),
        reinterpret_cast<const Containers::StringView*>(_layers.end()) + _layers.size(),
        layer);
}

UnsignedInt LayerProperties::count() {
    return UnsignedInt(_layers.size());
}

Containers::StringView LayerProperties::name(const UnsignedInt id) {
    CORRADE_ASSERT(id < _layers.size(),
        "Vk::LayerProperties::name(): index" << id << "out of range for" << _layers.size() << "entries", {});
    /* Not returning the string views at the end because those are in a
       different order */
    return _layers[id].layerName;
}

UnsignedInt LayerProperties::revision(const UnsignedInt id) {
    CORRADE_ASSERT(id < _layers.size(),
        "Vk::LayerProperties::revision(): index" << id << "out of range for" << _layers.size() << "entries", {});
    return _layers[id].implementationVersion;
}

Version LayerProperties::version(const UnsignedInt id) {
    CORRADE_ASSERT(id < _layers.size(),
        "Vk::LayerProperties::version(): index" << id << "out of range for" << _layers.size() << "entries", {});
    return Version(_layers[id].specVersion);
}

Containers::StringView LayerProperties::description(const UnsignedInt id) {
    CORRADE_ASSERT(id < _layers.size(),
        "Vk::LayerProperties::description(): index" << id << "out of range for" << _layers.size() << "entries", {});
    return _layers[id].description;
}

LayerProperties enumerateLayerProperties() {
    LayerProperties out;

    /* Retrieve layer count */
    UnsignedInt count;
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(vkEnumerateInstanceLayerProperties(&count, nullptr));

    /* No layers, nothing to do */
    if(!count) return out;

    /* Allocate extra for a list of string views that we'll use to sort &
       search the values; query the layers */
    out._layers = Containers::Array<VkLayerProperties>{
        reinterpret_cast<VkLayerProperties*>(new char[count*(sizeof(VkLayerProperties) + sizeof(Containers::StringView))]),
        count,
        [](VkLayerProperties* data, std::size_t) {
            delete[] reinterpret_cast<char*>(data);
        }};
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(vkEnumerateInstanceLayerProperties(&count, out._layers.data()));

    /* Expect the layer count didn't change between calls */
    CORRADE_INTERNAL_ASSERT(count == out._layers.size());

    /* Populate the views and sort them so we can search in O(log n) later */
    Containers::ArrayView<Containers::StringView> layerNames{reinterpret_cast<Containers::StringView*>(out._layers.end()), count};
    for(std::size_t i = 0; i != layerNames.size(); ++i)
        layerNames[i] = out._layers[i].layerName;
    std::sort(layerNames.begin(), layerNames.end());

    return out;
}

}}
