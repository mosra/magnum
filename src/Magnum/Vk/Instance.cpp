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

#include "Instance.h"

#include <algorithm>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Arguments.h>

#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Version.h"
#include "Magnum/Vk/Implementation/Arguments.h"
#include "Magnum/Vk/Implementation/InstanceState.h"
#include "MagnumExternal/Vulkan/flextVkGlobal.h"

namespace Magnum { namespace Vk {

struct InstanceCreateInfo::State {
    Containers::String applicationName;
    Containers::Array<Containers::String> ownedStrings;
    Containers::Array<const char*> layers;
    Containers::Array<const char*> extensions;

    Containers::String disabledLayersStorage, disabledExtensionsStorage;
    Containers::Array<Containers::StringView> disabledLayers, disabledExtensions;
    bool quietLog = false;
    Version version = Version::None;
    Int argc;
    const char** argv;
};

InstanceCreateInfo::InstanceCreateInfo(const Int argc, const char** const argv, const LayerProperties* const layerProperties, const InstanceExtensionProperties* extensionProperties, const Flags flags): _info{}, _applicationInfo{} {
    Utility::Arguments args = Implementation::arguments();
    args.parse(argc, argv);

    if(args.value("log") == "quiet")
        _state.emplace().quietLog = true;
    if(argc && argv) {
        if(!_state) _state.emplace();
        _state->argc = argc;
        _state->argv = argv;
    }

    _info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    _info.flags = VkInstanceCreateFlags(flags & ~Flag::NoImplicitExtensions);
    _info.pApplicationInfo = &_applicationInfo;
    _applicationInfo.pEngineName = "Magnum";
    /** @todo magnum version? 2020 can't fit into Vulkan's version
        representation, sigh */

    /* If there's a forced Vulkan version, use that, otherwise use the reported
       instance version */
    Containers::StringView version = args.value<Containers::StringView>("vulkan-version");
    if(!version.isEmpty()) {
        if(!_state) _state.emplace();

        if((_state->version = args.value<Version>("vulkan-version")) == Version::None)
            Warning{} << "Invalid --magnum-vulkan-version" << args.value<Containers::StringView>("vulkan-version") << Debug::nospace << ", ignoring";
    }
    if(_state && _state->version == Version::None)
        _state->version = enumerateInstanceVersion();
    _applicationInfo.apiVersion = UnsignedInt(_state ? _state->version : enumerateInstanceVersion());

    /* If there are any disabled layers or extensions, sort them and save for
       later -- we'll use them to filter the ones added by the app */
    Containers::String disabledLayers = args.value<Containers::String>("disable-layers");
    Containers::String disabledExtensions = args.value<Containers::String>("disable-extensions");
    if(!disabledLayers.isEmpty()) {
        if(!_state) _state.emplace();

        _state->disabledLayersStorage = std::move(disabledLayers);
        _state->disabledLayers = Containers::StringView{_state->disabledLayersStorage}.splitWithoutEmptyParts();
        std::sort(_state->disabledLayers.begin(), _state->disabledLayers.end());
    }
    if(!disabledExtensions.isEmpty()) {
        if(!_state) _state.emplace();

        _state->disabledExtensionsStorage = std::move(disabledExtensions);
        _state->disabledExtensions = Containers::StringView{_state->disabledExtensionsStorage}.splitWithoutEmptyParts();
        std::sort(_state->disabledExtensions.begin(), _state->disabledExtensions.end());
    }

    /* Add all layers and extensions enabled on command-line. The blacklist is
       applied on those as well. */
    /** @todo use a generator split() so we can avoid the growing allocation
        of the output array */
    /** @todo unfortunately even though the split and value retrieval is mostly
        allocation-free, the strings will be turned into owning copies because
        none of them is null-terminated or global -- could be a better idea to
        just grow one giant string internally (once we have growable strings) */
    addEnabledLayers(args.value<Containers::StringView>("enable-layers").splitWithoutEmptyParts());
    addEnabledExtensions(args.value<Containers::StringView>("enable-instance-extensions").splitWithoutEmptyParts());

    /** @todo use this (enabling debug layers etc.) */
    static_cast<void>(layerProperties);

    /* Enable implicit extensions unless that's forbidden */
    /** @todo move this somewhere else as this will grow significantly? */
    if(!(flags & Flag::NoImplicitExtensions)) {
        if(!_state) _state.emplace();

        /* Fetch searchable extension properties if not already */
        Containers::Optional<InstanceExtensionProperties> extensionPropertiesStorage;
        if(!extensionProperties) {
            extensionPropertiesStorage = enumerateInstanceExtensionProperties();
            extensionProperties = &*extensionPropertiesStorage;
        }

        /* Only if we don't have Vulkan 1.1, on which this is core */
        if(_state->version < Version::Vk11 && extensionProperties->isSupported<Extensions::KHR::get_physical_device_properties2>())
            addEnabledExtensions<Extensions::KHR::get_physical_device_properties2>();
    }
}

InstanceCreateInfo::InstanceCreateInfo(NoInitT) noexcept {}

InstanceCreateInfo::InstanceCreateInfo(const VkInstanceCreateInfo& info):
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

InstanceCreateInfo::~InstanceCreateInfo() = default;

InstanceCreateInfo& InstanceCreateInfo::setApplicationInfo(const Containers::StringView name, const Version version) {
    /* Keep an owned copy of the name if it's not global / null-terminated;
       Use nullptr if the view is empty */
    if(!name.isEmpty()) {
        if(!_state) _state.emplace();

        _state->applicationName = Containers::String::nullTerminatedGlobalView(name);
        _applicationInfo.pApplicationName = _state->applicationName.data();
    } else {
        if(_state) _state->applicationName = nullptr;
        _applicationInfo.pApplicationName = nullptr;
    }

    _applicationInfo.applicationVersion = UnsignedInt(version);
    return *this;
}

InstanceCreateInfo& InstanceCreateInfo::addEnabledLayers(const Containers::ArrayView<const Containers::StringView> layers) {
    if(layers.empty()) return *this;
    if(!_state) _state.emplace();

    /* Add null-terminated strings to the layer array */
    arrayReserve(_state->layers, _state->layers.size() + layers.size());
    for(const Containers::StringView layer: layers) {
        /* If the layer is blacklisted, skip it */
        if(std::binary_search(_state->disabledLayers.begin(), _state->disabledLayers.end(), layer)) continue;

        /* Keep an owned *allocated* copy of the string if it's not global or
           null-terminated -- ideally, if people use string view literals,
           those will be, so this won't allocate. Allocated so the pointers
           don't get invalidated when the array gets reallocated. */
        const char* data;
        if(!(layer.flags() >= (Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global)))
            data = arrayAppend(_state->ownedStrings, Containers::InPlaceInit,
                Containers::AllocatedInit, layer).data();
        else data = layer.data();

        arrayAppend(_state->layers, data);
    }

    /* Update the layer count, re-route the pointer to the layers array in case
       it got reallocated */
    _info.enabledLayerCount = _state->layers.size();
    _info.ppEnabledLayerNames = _state->layers.data();
    return *this;
}

InstanceCreateInfo& InstanceCreateInfo::addEnabledLayers(const std::initializer_list<Containers::StringView> layers) {
    return addEnabledLayers(Containers::arrayView(layers));
}

InstanceCreateInfo& InstanceCreateInfo::addEnabledExtensions(const Containers::ArrayView<const Containers::StringView> extensions) {
    if(extensions.empty()) return *this;
    if(!_state) _state.emplace();

    /* Add null-terminated strings to the extension array */
    arrayReserve(_state->extensions, _state->extensions.size() + extensions.size());
    for(const Containers::StringView extension: extensions) {
        /* If the extension is blacklisted, skip it */
        if(std::binary_search(_state->disabledExtensions.begin(), _state->disabledExtensions.end(), extension)) continue;

        /* Keep an owned *allocated* copy of the string if it's not global or
           null-terminated -- ideally, if people use string view literals,
           those will be, so this won't allocate. Allocated so the pointers
           don't get invalidated when the array gets reallocated. */
        const char* data;
        if(!(extension.flags() >= (Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global)))
            data = arrayAppend(_state->ownedStrings, Containers::InPlaceInit,
                Containers::AllocatedInit, extension).data();
        else data = extension.data();

        arrayAppend(_state->extensions, data);
    }

    /* Update the extension count, re-route the pointer to the layers array in
       case it got reallocated */
    _info.enabledExtensionCount = _state->extensions.size();
    _info.ppEnabledExtensionNames = _state->extensions.data();
    return *this;
}

InstanceCreateInfo& InstanceCreateInfo::addEnabledExtensions(const std::initializer_list<Containers::StringView> extensions) {
    return addEnabledExtensions(Containers::arrayView(extensions));
}

InstanceCreateInfo& InstanceCreateInfo::addEnabledExtensions(const Containers::ArrayView<const InstanceExtension> extensions) {
    if(extensions.empty()) return *this;
    if(!_state) _state.emplace();

    arrayReserve(_state->extensions, _state->extensions.size() + extensions.size());
    for(const InstanceExtension& extension: extensions) {
        /* If the extension is blacklisted, skip it */
        if(std::binary_search(_state->disabledExtensions.begin(), _state->disabledExtensions.end(), extension.string())) continue;

        arrayAppend(_state->extensions, extension.string().data());
    }

    /* Update the extension count, re-route the pointer to the layers array in
       case it got reallocated */
    _info.enabledExtensionCount = _state->extensions.size();
    _info.ppEnabledExtensionNames = _state->extensions.data();
    return *this;
}

InstanceCreateInfo& InstanceCreateInfo::addEnabledExtensions(const std::initializer_list<InstanceExtension> extensions) {
    return addEnabledExtensions(Containers::arrayView(extensions));
}

Instance Instance::wrap(const VkInstance handle, const Version version, const Containers::ArrayView<const Containers::StringView> enabledExtensions, const HandleFlags flags) {
    /* Compared to the constructor nothing is printed here as it would be just
       repeating what was passed to the constructor */
    Instance out{NoCreate};
    out._handle = handle;
    out._flags = flags;
    out.initializeExtensions(enabledExtensions);
    out.initialize(version, 0, nullptr);
    return out;
}

Instance Instance::wrap(const VkInstance handle, const Version version, const std::initializer_list<Containers::StringView> enabledExtensions, const HandleFlags flags) {
    return wrap(handle, version, Containers::arrayView(enabledExtensions), flags);
}

Instance::Instance(const InstanceCreateInfo& info): _flags{HandleFlag::DestroyOnDestruction} {
    const Version version = info._state && info._state->version != Version::None ? info._state->version : enumerateInstanceVersion();

    /* Print all enabled layers and extensions if we're not told to be quiet */
    if(!info._state || !info._state->quietLog) {
        Debug{} << "Instance version:" << version;

        if(info->enabledLayerCount) {
            Debug{} << "Enabled layers:";
            for(std::size_t i = 0, max = info->enabledLayerCount; i != max; ++i)
                Debug{} << "   " << info->ppEnabledLayerNames[i];
        }

        if(info->enabledExtensionCount) {
            Debug{} << "Enabled instance extensions:";
            for(std::size_t i = 0, max = info->enabledExtensionCount; i != max; ++i)
                Debug{} << "   " << info->ppEnabledExtensionNames[i];
        }
    }

    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(vkCreateInstance(info, nullptr, &_handle));

    initializeExtensions<const char*>({info->ppEnabledExtensionNames, info->enabledExtensionCount});
    if(info._state)
        initialize(version, info._state->argc, info._state->argv);
    else
        initialize(version, 0, nullptr);
}

Instance::Instance(NoCreateT): _handle{}, _functionPointers{} {}

Instance::Instance(Instance&& other) noexcept: _handle{other._handle},
    _flags{other._flags}, _version{other._version},
    _extensionStatus{other._extensionStatus}, _state{std::move(other._state)},
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _functionPointers(other._functionPointers)
{
    other._handle = nullptr;
    other._functionPointers = {};
}

Instance::~Instance() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        _functionPointers.DestroyInstance(_handle, nullptr);
}

Instance& Instance::operator=(Instance&& other) noexcept {
    using std::swap;
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    swap(other._version, _version);
    swap(other._extensionStatus, _extensionStatus);
    swap(other._state, _state);
    swap(other._functionPointers, _functionPointers);
    return *this;
}

template<class T> void Instance::initializeExtensions(const Containers::ArrayView<const T> enabledExtensions) {
    /* Mark all known extensions as enabled */
    for(const T extension: enabledExtensions) {
        for(Containers::ArrayView<const InstanceExtension> knownExtensions: {
            InstanceExtension::extensions(Version::None),
          /*InstanceExtension::extensions(Version::Vk10), is empty */
            InstanceExtension::extensions(Version::Vk11),
          /*InstanceExtension::extensions(Version::Vk12) is empty */
        }) {
            auto found = std::lower_bound(knownExtensions.begin(), knownExtensions.end(), extension, [](const InstanceExtension& a, const T& b) {
                return a.string() < static_cast<const Containers::StringView&>(b);
            });
            if(found->string() != extension) continue;
            _extensionStatus.set(found->index(), true);
        }
    }
}

void Instance::initialize(const Version version, const Int argc, const char** const argv) {
    /* Init version, function pointers */
    _version = version;
    flextVkInitInstance(_handle, &_functionPointers);

    /* Set up extension-dependent functionality */
    _state.emplace(*this, argc, argv);
}

VkInstance Instance::release() {
    const VkInstance handle = _handle;
    _handle = nullptr;
    return handle;
}

bool Instance::isExtensionEnabled(const InstanceExtension& extension) const {
    return _extensionStatus[extension.index()];
}

void Instance::populateGlobalFunctionPointers() {
    flextVkInstance = _functionPointers;
}

}}
