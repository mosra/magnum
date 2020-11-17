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

#include "Device.h"

#include <algorithm>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StaticArray.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Arguments.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Queue.h"
#include "Magnum/Vk/Version.h"
#include "Magnum/Vk/Implementation/Arguments.h"
#include "Magnum/Vk/Implementation/InstanceState.h"
#include "Magnum/Vk/Implementation/DeviceState.h"
#include "MagnumExternal/Vulkan/flextVkGlobal.h"

namespace Magnum { namespace Vk {

struct DeviceCreateInfo::State {
    Containers::Array<Containers::String> ownedStrings;
    Containers::Array<const char*> extensions;

    Containers::String disabledExtensionsStorage;
    Containers::Array<Containers::StringView> disabledExtensions;
    Containers::Array<VkDeviceQueueCreateInfo> queues;
    Containers::StaticArray<32, Float> queuePriorities;
    Containers::StaticArray<32, Queue*> queueOutput;

    std::size_t nextQueuePriority = 0;
    bool quietLog = false;
    Version version = Version::None;
    /* Gets populated at the very end of DeviceCreateInfo(DeviceProperties&)
       and then possibly overwritten in DeviceCreateInfo(DeviceProperties&&).
       Either way, it's meant to be valid after the constructor exits. */
    DeviceProperties properties{NoCreate};
};

DeviceCreateInfo::DeviceCreateInfo(DeviceProperties& deviceProperties, const ExtensionProperties* extensionProperties, const Flags flags): _physicalDevice{deviceProperties}, _info{}, _state{Containers::InPlaceInit} {
    Utility::Arguments args = Implementation::arguments();
    args.parse(deviceProperties._instance->state().argc, deviceProperties._instance->state().argv);

    if(args.value("log") == "quiet")
        _state->quietLog = true;

    _info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    _info.flags = VkDeviceCreateFlags(flags & ~Flag::NoImplicitExtensions);

    /* Take the minimum of instance and device version. Instance version being
       smaller than a device version happens mainly if there's a forced Vulkan
       version via --magnum-vulkan version, which will be later used to cap available features. */
    _state->version = Version(Math::min(UnsignedInt(deviceProperties._instance->version()), UnsignedInt(deviceProperties.version())));

    /* If there are any disabled extensions, sort them and save for later --
       we'll use them to filter the ones added by the app */
    Containers::String disabledExtensions = args.value<Containers::String>("disable-extensions");
    if(!disabledExtensions.isEmpty()) {
        _state->disabledExtensionsStorage = std::move(disabledExtensions);
        _state->disabledExtensions = Containers::StringView{_state->disabledExtensionsStorage}.splitWithoutEmptyParts();
        std::sort(_state->disabledExtensions.begin(), _state->disabledExtensions.end());
    }

    /* Add all extensions enabled on command-line. The blacklist is applied on
       those as well. */
    /** @todo use a generator split() so we can avoid the growing allocation
        of the output array */
    /** @todo unfortunately even though the split and value retrieval is mostly
        allocation-free, the strings will be turned into owning copies because
        none of them is null-terminated or global -- could be a better idea to
        just grow one giant string internally (once we have growable strings) */
    addEnabledExtensions(args.value<Containers::StringView>("enable-extensions").splitWithoutEmptyParts());

    /* Enable implicit extensions unless that's forbidden */
    /** @todo move this somewhere else as this will grow significantly? */
    if(!(flags & Flag::NoImplicitExtensions)) {
        /* Fetch searchable extension properties if not already */
        Containers::Optional<ExtensionProperties> extensionPropertiesStorage;
        if(!extensionProperties) {
            /** @todo i'd like to know which layers are enabled so i can list
                the exts from those .. but how? */
            extensionPropertiesStorage = deviceProperties.enumerateExtensionProperties();
            extensionProperties = &*extensionPropertiesStorage;
        }

        /* Only if we don't have Vulkan 1.1, on which this is core */
        if(_state->version < Version::Vk11 && extensionProperties->isSupported<Extensions::KHR::get_memory_requirements2>())
            addEnabledExtensions<Extensions::KHR::get_memory_requirements2>();
        if(_state->version < Version::Vk11 && extensionProperties->isSupported<Extensions::KHR::bind_memory2>())
            addEnabledExtensions<Extensions::KHR::bind_memory2>();
    }

    /* Conservatively populate the device properties.
       - In case the DeviceCreateInfo(DeviceProperties&&) constructor is used,
         it'll get overwritten straight away with a populated instance.
       - In case the addQueues(QueueFlags) API is not used and DeviceCreateInfo
         isn't subsequently moved to the Device, it'll never get touched again
         and Device will wrap() its own.
       - In case addQueues(QueueFlags) is used it'll get populated and then
         possibly discarded if it isn't subsequently moved to the Device. */
    _state->properties = DeviceProperties::wrap(*deviceProperties._instance, deviceProperties._handle);
}

DeviceCreateInfo::DeviceCreateInfo(DeviceProperties&& deviceProperties, const ExtensionProperties* extensionProperties, const Flags flags): DeviceCreateInfo{deviceProperties, extensionProperties, flags} {
    _state->properties = std::move(deviceProperties);
}

DeviceCreateInfo::DeviceCreateInfo(NoInitT) noexcept {}

DeviceCreateInfo::DeviceCreateInfo(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo& info): _physicalDevice{physicalDevice},
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(info) {}

DeviceCreateInfo::~DeviceCreateInfo() = default;

DeviceCreateInfo& DeviceCreateInfo::addEnabledExtensions(const Containers::ArrayView<const Containers::StringView> extensions) & {
    if(extensions.empty()) return *this;
    /* This can happen in case we used the NoInit or VkDeviceCreateInfo
       constructor */
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

DeviceCreateInfo&& DeviceCreateInfo::addEnabledExtensions(const Containers::ArrayView<const Containers::StringView> extensions) && {
    addEnabledExtensions(extensions);
    return std::move(*this);
}

DeviceCreateInfo& DeviceCreateInfo::addEnabledExtensions(const std::initializer_list<Containers::StringView> extensions) & {
    return addEnabledExtensions(Containers::arrayView(extensions));
}

DeviceCreateInfo&& DeviceCreateInfo::addEnabledExtensions(const std::initializer_list<Containers::StringView> extensions) && {
    addEnabledExtensions(extensions);
    return std::move(*this);
}

DeviceCreateInfo& DeviceCreateInfo::addEnabledExtensions(const Containers::ArrayView<const Extension> extensions) & {
    if(extensions.empty()) return *this;
    /* This can happen in case we used the NoInit or VkDeviceCreateInfo
       constructor */
    if(!_state) _state.emplace();

    arrayReserve(_state->extensions, _state->extensions.size() + extensions.size());
    for(const Extension& extension: extensions) {
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

DeviceCreateInfo&& DeviceCreateInfo::addEnabledExtensions(const Containers::ArrayView<const Extension> extensions) && {
    addEnabledExtensions(extensions);
    return std::move(*this);
}

DeviceCreateInfo& DeviceCreateInfo::addEnabledExtensions(const std::initializer_list<Extension> extensions) & {
    return addEnabledExtensions(Containers::arrayView(extensions));
}

DeviceCreateInfo&& DeviceCreateInfo::addEnabledExtensions(const std::initializer_list<Extension> extensions) && {
    addEnabledExtensions(extensions);
    return std::move(*this);
}

DeviceCreateInfo& DeviceCreateInfo::addQueues(const UnsignedInt family, const Containers::ArrayView<const Float> priorities, const Containers::ArrayView<const Containers::Reference<Queue>> output) & {
    CORRADE_ASSERT(!priorities.empty(), "Vk::DeviceCreateInfo::addQueues(): at least one queue priority has to be specified", *this);
    CORRADE_ASSERT(output.size() == priorities.size(), "Vk::DeviceCreateInfo::addQueues(): expected" << priorities.size() << "outuput queue references but got" << output.size(), *this);

    /* This can happen in case we used the NoInit or VkDeviceCreateInfo
       constructor */
    if(!_state) _state.emplace();

    VkDeviceQueueCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    info.queueFamilyIndex = family;
    info.queueCount = priorities.size();
    info.pQueuePriorities = _state->queuePriorities + _state->nextQueuePriority;

    /* Copy the passed queue priorities and output queue references to an
       internal storage that never reallocates. If this blows up, see the
       definition of queuePriorities for details. We can't easily reallocate if
       this grows too big as all pointers would need to be patched, so there's
       a static limit. */
    CORRADE_INTERNAL_ASSERT(_state->nextQueuePriority + priorities.size() <= _state->queuePriorities.size());
    Utility::copy(priorities, _state->queuePriorities.suffix(_state->nextQueuePriority).prefix(priorities.size()));
    for(std::size_t i = 0; i != priorities.size(); ++i)
        _state->queueOutput[_state->nextQueuePriority + i] = &*output[i];
    _state->nextQueuePriority += priorities.size();

    return addQueues(info);
}

DeviceCreateInfo&& DeviceCreateInfo::addQueues(const UnsignedInt family, const Containers::ArrayView<const Float> priorities, const Containers::ArrayView<const Containers::Reference<Queue>> output) && {
    addQueues(family, priorities, output);
    return std::move(*this);
}

DeviceCreateInfo& DeviceCreateInfo::addQueues(const UnsignedInt family, const std::initializer_list<Float> priorities, const std::initializer_list<Containers::Reference<Queue>> output) & {
    return addQueues(family, Containers::arrayView(priorities), Containers::arrayView(output));
}

DeviceCreateInfo&& DeviceCreateInfo::addQueues(const UnsignedInt family, const std::initializer_list<Float> priorities, const std::initializer_list<Containers::Reference<Queue>> output) && {
    addQueues(family, priorities, output);
    return std::move(*this);
}

DeviceCreateInfo& DeviceCreateInfo::addQueues(const QueueFlags flags, const Containers::ArrayView<const Float> priorities, const Containers::ArrayView<const Containers::Reference<Queue>> output) & {
    return addQueues(_state->properties.pickQueueFamily(flags), priorities, output);
}

DeviceCreateInfo&& DeviceCreateInfo::addQueues(const QueueFlags flags, const Containers::ArrayView<const Float> priorities, const Containers::ArrayView<const Containers::Reference<Queue>> output) && {
    addQueues(flags, priorities, output);
    return std::move(*this);
}

DeviceCreateInfo& DeviceCreateInfo::addQueues(const QueueFlags flags, const std::initializer_list<Float> priorities, const std::initializer_list<Containers::Reference<Queue>> output) & {
    return addQueues(flags, Containers::arrayView(priorities), Containers::arrayView(output));
}

DeviceCreateInfo&& DeviceCreateInfo::addQueues(const QueueFlags flags, const std::initializer_list<Float> priorities, const std::initializer_list<Containers::Reference<Queue>> output) && {
    addQueues(flags, priorities, output);
    return std::move(*this);
}

DeviceCreateInfo& DeviceCreateInfo::addQueues(const VkDeviceQueueCreateInfo& info) & {
    /* This can happen in case we used the NoInit or VkDeviceCreateInfo
       constructor */
    if(!_state) _state.emplace();

    /* Copy the info to an internal storage and re-route the pointer to it.
       This handles a potential reallocation and also the case of replacing the
       default queue on the first call to addQueues(). */
    arrayAppend(_state->queues, info);
    _info.pQueueCreateInfos = _state->queues;
    _info.queueCreateInfoCount = _state->queues.size();

    return *this;
}

DeviceCreateInfo&& DeviceCreateInfo::addQueues(const VkDeviceQueueCreateInfo& info) && {
    addQueues(info);
    return std::move(*this);
}

Device Device::wrap(Instance& instance, const VkDevice handle, const Version version, const Containers::ArrayView<const Containers::StringView> enabledExtensions, const HandleFlags flags) {
    /* Compared to the constructor nothing is printed here as it would be just
       repeating what was passed to the constructor */
    Device out{NoCreate};
    out._handle = handle;
    out._flags = flags;
    out.initializeExtensions(enabledExtensions);
    out.initialize(instance, version);
    return out;
}

Device Device::wrap(Instance& instance, const VkDevice handle, const Version version, const std::initializer_list<Containers::StringView> enabledExtensions, const HandleFlags flags) {
    return wrap(instance, handle, version, Containers::arrayView(enabledExtensions), flags);
}

Device::Device(Instance& instance, const DeviceCreateInfo& info): Device{instance, info, DeviceProperties::wrap(instance, info._physicalDevice)} {}

Device::Device(Instance& instance, DeviceCreateInfo&& info): Device{instance, info, std::move(info._state->properties)} {}

Device::Device(Instance& instance, const DeviceCreateInfo& info, DeviceProperties&& properties):
    #ifdef CORRADE_GRACEFUL_ASSERT
    _handle{}, /* Otherwise the destructor dies if we hit the queue assert */
    #endif
    _flags{HandleFlag::DestroyOnDestruction},
    _properties{Containers::InPlaceInit, std::move(properties)}
{
    /* The properties should always be a valid instance, either moved from
       outside or created again from VkPhysicalDevice, in case it couldn't be
       moved. If it's not, something in DeviceCreateInfo or here got messed
       up. */
    CORRADE_INTERNAL_ASSERT(_properties->handle());

    CORRADE_ASSERT(info._info.queueCreateInfoCount,
        "Vk::Device: needs to be created with at least one queue", );

    const Version version = info._state->version != Version::None ?
        info._state->version : _properties->version();

    /* Print all enabled extensions if we're not told to be quiet */
    if(!info._state || !info._state->quietLog) {
        Debug{} << "Device:" << _properties->name();
        Debug{} << "Device version:" << version;

        if(info->enabledExtensionCount) {
            Debug{} << "Enabled device extensions:";
            for(std::size_t i = 0, max = info->enabledExtensionCount; i != max; ++i)
                Debug{} << "   " << info->ppEnabledExtensionNames[i];
        }
    }

    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(instance->CreateDevice(info._physicalDevice, info, nullptr, &_handle));

    initializeExtensions<const char*>({info->ppEnabledExtensionNames, info->enabledExtensionCount});
    initialize(instance, version);

    /* Extension-dependent state is initialized, now we can retrieve the queues
       from the device and save them to the outputs specified in addQueues().
       Each of those calls added one or more entries into _state->queueOutput,
       maintain an offset into it. */
    UnsignedInt queueOutputIndex = 0;
    for(const VkDeviceQueueCreateInfo& createInfo: info._state->queues) {
        /* If the info structure doesn't point into our priority array, it
           means it was added with the addQueues(VkDeviceQueueCreateInfo)
           overload. For that we didn't remember any output, thus skip it */
        if(createInfo.pQueuePriorities < info._state->queuePriorities.begin() ||
           createInfo.pQueuePriorities >= info._state->queuePriorities.end())
            continue;

        for(UnsignedInt i = 0; i != createInfo.queueCount; ++i) {
            VkDeviceQueueInfo2 requestInfo{};
            requestInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
            requestInfo.queueFamilyIndex = createInfo.queueFamilyIndex;
            /* According to the spec we can request each family only once,
               which means here we don't need to remember the per-family index
               across multiple VkDeviceQueueCreateInfos, making the
               implementation a bit simpler. */
            requestInfo.queueIndex = i;

            /* Retrieve the queue handle, create a new Queue object in desired
               output location, and increment the output location for the next
               queue */
            VkQueue queue;
            _state->getDeviceQueueImplementation(*this, requestInfo, queue);
            *info._state->queueOutput[queueOutputIndex++] = Queue::wrap(*this, queue);
        }
    }
}

Device::Device(NoCreateT): _handle{}, _functionPointers{} {}

Device::Device(Device&& other) noexcept: _handle{other._handle},
    _flags{other._flags}, _version{other._version},
    _extensionStatus{other._extensionStatus}, _properties{std::move(other._properties)}, _state{std::move(other._state)},
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _functionPointers(other._functionPointers)
{
    other._handle = nullptr;
    other._functionPointers = {};
}

Device::~Device() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        _functionPointers.DestroyDevice(_handle, nullptr);
}

Device& Device::operator=(Device&& other) noexcept {
    using std::swap;
    swap(other._handle, _handle);
    swap(other._flags, _flags);
    swap(other._version, _version);
    swap(other._extensionStatus, _extensionStatus);
    swap(other._properties, _properties);
    swap(other._state, _state);
    swap(other._functionPointers, _functionPointers);
    return *this;
}

template<class T> void Device::initializeExtensions(const Containers::ArrayView<const T> enabledExtensions) {
    /* Mark all known extensions as enabled */
    for(const T extension: enabledExtensions) {
        for(Containers::ArrayView<const Extension> knownExtensions: {
            Extension::extensions(Version::None),
          /*Extension::extensions(Version::Vk10), is empty */
            Extension::extensions(Version::Vk11),
            Extension::extensions(Version::Vk12)
        }) {
            auto found = std::lower_bound(knownExtensions.begin(), knownExtensions.end(), extension, [](const Extension& a, const T& b) {
                return a.string() < static_cast<const Containers::StringView&>(b);
            });
            if(found->string() != extension) continue;
            _extensionStatus.set(found->index(), true);
        }
    }
}

void Device::initialize(Instance& instance, const Version version) {
    /* Init version, function pointers */
    _version = version;
    flextVkInitDevice(_handle, &_functionPointers, instance->GetDeviceProcAddr);

    /* Set up extension-dependent functionality */
    _state.emplace(*this);
}

bool Device::isExtensionEnabled(const Extension& extension) const {
    return _extensionStatus[extension.index()];
}

VkDevice Device::release() {
    const VkDevice handle = _handle;
    _handle = nullptr;
    return handle;
}

void Device::populateGlobalFunctionPointers() {
    flextVkDevice = _functionPointers;
}

void Device::getQueueImplementation11(Device& self, const VkDeviceQueueInfo2& info, VkQueue& queue) {
    return self->GetDeviceQueue2(self, &info, &queue);
}

void Device::getQueueImplementationDefault(Device& self, const VkDeviceQueueInfo2& info, VkQueue& queue) {
    return self->GetDeviceQueue(self, info.queueFamilyIndex, info.queueIndex, &queue);
}

}}
