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

#include "DeviceProperties.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StaticArray.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Vk/Assert.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/Memory.h"
#include "Magnum/Vk/Version.h"
#include "Magnum/Vk/Implementation/Arguments.h"
#include "Magnum/Vk/Implementation/InstanceState.h"

namespace Magnum { namespace Vk {

struct DeviceProperties::State {
    explicit State(Instance& instance, VkPhysicalDevice handle);

    /* Cached device extension properties to dispatch on when querying
       properties. Should be only used through
       DeviceProperties::extensionPropertiesInternal(). */
    Containers::Optional<ExtensionProperties> extensions;

    void(*getPropertiesImplementation)(DeviceProperties&, VkPhysicalDeviceProperties2&);
    void(*getQueueFamilyPropertiesImplementation)(DeviceProperties&, UnsignedInt&, VkQueueFamilyProperties2*);
    void(*getMemoryPropertiesImplementation)(DeviceProperties&, VkPhysicalDeviceMemoryProperties2&);

    VkPhysicalDeviceProperties2 properties{};
    VkPhysicalDeviceDriverProperties driverProperties{};
    VkPhysicalDeviceMemoryProperties2 memoryProperties{};
    Containers::Array<VkQueueFamilyProperties2> queueFamilyProperties;
};

DeviceProperties::State::State(Instance& instance, const VkPhysicalDevice handle) {
    /* All this extension-dependent dispatch has to be stored per physical
       device, not just on instance, because it's actually instance-level
       functionality depending on a version of a particular device. According
       to https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap3.html#fundamentals-validusage-versions :

        Physical-device-level functionality or behavior added by a new core
        version of the API must not be used unless it is supported by the
        physical device as determined by VkPhysicalDeviceProperties::apiVersion
        and the specified version of VkApplicationInfo::apiVersion.

       and https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap4.html#_extending_physical_device_core_functionality :

        New core physical-device-level functionality can be used when the
        physical-device version is greater than or equal to the version of
        Vulkan that added the new functionality. The Vulkan version supported
        by a physical device can be obtained by calling
        vkGetPhysicalDeviceProperties.

       and https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap4.html#initialization-phys-dev-extensions :

        Applications must not use a VkPhysicalDevice in any command added by an
        extension or core version that is not supported by that physical
        device.

       Which means for example, if Vulkan 1.1 is supported by the instance, it
       doesn't actually imply I can use vkGetPhysicalDeviceProperties2() -- I
       can only use that in case the device supports 1.1 as well, which means I
       have to call vkGetPhysicalDeviceProperties() first in order to be able
       to call vkGetPhysicalDeviceProperties2().

       On the other hand, if the device is 1.0 but the instance
       supports VK_KHR_get_physical_device_properties2, I can call
       vkGetPhysicalDeviceProperties2KHR() directly -- https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap4.html#initialization-phys-dev-extensions :

        When the VK_KHR_get_physical_device_properties2 extension is enabled,
        or when both the instance and the physical-device versions are at least
        1.1, physical-device-level functionality of a device extension can be
        used with a physical device if the corresponding extension is
        enumerated by vkEnumerateDeviceExtensionProperties for that physical
        device, even before a logical device has been created.

       This also explains why e.g. VK_KHR_driver_properties is a device
       extension and not instance extension -- I can only add it to the pNext
       chain if the device is able to understand it, even though it's shoveled
       there by an instance-level API. */

    instance->GetPhysicalDeviceProperties(handle, &properties.properties);

    /* Have to check both the instance and device version, see above */
    if(instance.isVersionSupported(Version::Vk11) && Version(properties.properties.apiVersion) >= Version::Vk11) {
        getPropertiesImplementation = &DeviceProperties::getPropertiesImplementation11;
        getQueueFamilyPropertiesImplementation = &DeviceProperties::getQueueFamilyPropertiesImplementation11;
        getMemoryPropertiesImplementation = &DeviceProperties::getMemoryPropertiesImplementation11;
    } else if(instance.isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>()) {
        getPropertiesImplementation = &DeviceProperties::getPropertiesImplementationKHR;
        getQueueFamilyPropertiesImplementation = &DeviceProperties::getQueueFamilyPropertiesImplementationKHR;
        getMemoryPropertiesImplementation = &DeviceProperties::getMemoryPropertiesImplementationKHR;
    } else {
        getPropertiesImplementation = DeviceProperties::getPropertiesImplementationDefault;
        getQueueFamilyPropertiesImplementation = &DeviceProperties::getQueueFamilyPropertiesImplementationDefault;
        getMemoryPropertiesImplementation = &DeviceProperties::getMemoryPropertiesImplementationDefault;
    }
}

DeviceProperties::DeviceProperties(NoCreateT) noexcept: _instance{}, _handle{} {}

DeviceProperties::DeviceProperties(Instance& instance, VkPhysicalDevice handle): _instance{&instance}, _handle{handle} {}

/* The VkDeviceProperties handle doesn't need to be destroyed so it's enough to
   just rely on the implicit behavior */
DeviceProperties::DeviceProperties(DeviceProperties&&) noexcept = default;

DeviceProperties::~DeviceProperties() = default;

DeviceProperties& DeviceProperties::operator=(DeviceProperties&&) noexcept = default;

Version DeviceProperties::version() {
    return Version(properties1().apiVersion);
}

bool DeviceProperties::isVersionSupported(const Version version) {
    return Version(properties1().apiVersion) >= version;
}

DeviceType DeviceProperties::type() {
    return DeviceType(properties1().deviceType);
}

Containers::StringView DeviceProperties::name() {
    return properties1().deviceName;
}

DeviceDriver DeviceProperties::driver() {
    /* Ensure the values are populated first */
    return properties(), DeviceDriver(_state->driverProperties.driverID);
}

Version DeviceProperties::driverVersion() {
    return Version(properties1().driverVersion);
}

Containers::StringView DeviceProperties::driverName() {
    /* Ensure the values are populated first */
    return properties(), _state->driverProperties.driverName;
}

Containers::StringView DeviceProperties::driverInfo() {
    /* Ensure the values are populated first */
    return properties(), _state->driverProperties.driverInfo;
}

const VkPhysicalDeviceProperties& DeviceProperties::properties1() {
    if(!_state) _state.emplace(*_instance, _handle);

    return _state->properties.properties;
}

const VkPhysicalDeviceProperties2& DeviceProperties::properties() {
    if(!_state) _state.emplace(*_instance, _handle);

    /* Properties not fetched yet, do that now */
    if(!_state->properties.sType) {
        _state->properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

        Containers::Reference<void*> next = _state->properties.pNext;

        /* Fetch driver properties, if supported */
        if(isVersionSupported(Version::Vk12) || extensionPropertiesInternal().isSupported<Extensions::KHR::driver_properties>()) {
            *next = &_state->driverProperties;
            next = _state->driverProperties.pNext;
            _state->driverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
        }

        _state->getPropertiesImplementation(*this, _state->properties);
    }

    return _state->properties;
}

void DeviceProperties::getPropertiesImplementationDefault(DeviceProperties& self, VkPhysicalDeviceProperties2& properties) {
    return (**self._instance).GetPhysicalDeviceProperties(self._handle, &properties.properties);
}

void DeviceProperties::getPropertiesImplementationKHR(DeviceProperties& self, VkPhysicalDeviceProperties2& properties) {
    return (**self._instance).GetPhysicalDeviceProperties2KHR(self._handle, &properties);
}

void DeviceProperties::getPropertiesImplementation11(DeviceProperties& self, VkPhysicalDeviceProperties2& properties) {
    return (**self._instance).GetPhysicalDeviceProperties2(self._handle, &properties);
}

ExtensionProperties DeviceProperties::enumerateExtensionProperties(Containers::ArrayView<const Containers::StringView> layers) {
    return InstanceExtensionProperties{layers, [](void* state, const char* const layer, UnsignedInt* count, VkExtensionProperties* properties) {
        auto& deviceProperties = *static_cast<DeviceProperties*>(state);
        return (**deviceProperties._instance).EnumerateDeviceExtensionProperties(deviceProperties._handle, layer, count, properties);
    }, this};
}

ExtensionProperties DeviceProperties::enumerateExtensionProperties(std::initializer_list<Containers::StringView> layers) {
    return enumerateExtensionProperties(Containers::arrayView(layers));
}

const ExtensionProperties& DeviceProperties::extensionPropertiesInternal() {
    if(!_state) _state.emplace(*_instance, _handle);
    if(!_state->extensions) _state->extensions = enumerateExtensionProperties();
    return *_state->extensions;
}

Containers::ArrayView<const VkQueueFamilyProperties2> DeviceProperties::queueFamilyProperties() {
    if(!_state) _state.emplace(*_instance, _handle);

    /* Fetch if not already */
    if(_state->queueFamilyProperties.empty()) {
        UnsignedInt count;
        _state->getQueueFamilyPropertiesImplementation(*this, count, nullptr);

        _state->queueFamilyProperties = Containers::Array<VkQueueFamilyProperties2>{Containers::ValueInit, count};
        for(VkQueueFamilyProperties2& i: _state->queueFamilyProperties)
            i.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        _state->getQueueFamilyPropertiesImplementation(*this, count, _state->queueFamilyProperties);
        CORRADE_INTERNAL_ASSERT(count == _state->queueFamilyProperties.size());
    }

    return _state->queueFamilyProperties;
}

void DeviceProperties::getQueueFamilyPropertiesImplementationDefault(DeviceProperties& self, UnsignedInt& count, VkQueueFamilyProperties2* properties) {
    (**self._instance).GetPhysicalDeviceQueueFamilyProperties(self._handle,  &count, reinterpret_cast<VkQueueFamilyProperties*>(properties));

    /* "Sparsen" the returned data to the version 2 structure layout. If the
       pointer is null we were just querying the count. */
    if(properties) {
        Containers::ArrayView<VkQueueFamilyProperties> src{reinterpret_cast<VkQueueFamilyProperties*>(properties), count};
        Containers::ArrayView<VkQueueFamilyProperties2> dst{properties, count};
        /* Go backwards so we don't overwrite the yet-to-be-processed data,
           additionally copy the VkQueueFamilyProperties first so we don't
           overwrite them by setting sType and pNext. */
        for(std::size_t i = count; i != 0; --i) {
            dst[i - 1].queueFamilyProperties = src[i - 1];
            dst[i - 1].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
            dst[i - 1].pNext = nullptr;
        }
    }
}

void DeviceProperties::getQueueFamilyPropertiesImplementationKHR(DeviceProperties& self, UnsignedInt& count, VkQueueFamilyProperties2* properties) {
    return (**self._instance).GetPhysicalDeviceQueueFamilyProperties2KHR(self._handle, &count, properties);
}

void DeviceProperties::getQueueFamilyPropertiesImplementation11(DeviceProperties& self, UnsignedInt& count, VkQueueFamilyProperties2* properties) {
    return (**self._instance).GetPhysicalDeviceQueueFamilyProperties2(self._handle, &count, properties);
}

UnsignedInt DeviceProperties::queueFamilyCount() {
    return queueFamilyProperties().size();
}

UnsignedInt DeviceProperties::queueFamilySize(const UnsignedInt id) {
    const Containers::ArrayView<const VkQueueFamilyProperties2> properties = queueFamilyProperties();
    CORRADE_ASSERT(id < properties.size(),
        "Vk::DeviceProperties::queueFamilySize(): index" << id << "out of range for" << properties.size() << "entries", {});
    return properties[id].queueFamilyProperties.queueCount;
}

QueueFlags DeviceProperties::queueFamilyFlags(const UnsignedInt id) {
    const Containers::ArrayView<const VkQueueFamilyProperties2> properties = queueFamilyProperties();
    CORRADE_ASSERT(id < properties.size(),
        "Vk::DeviceProperties::queueFamilyFlags(): index" << id << "out of range for" << properties.size() << "entries", {});
    return QueueFlag(properties[id].queueFamilyProperties.queueFlags);
}

UnsignedInt DeviceProperties::pickQueueFamily(const QueueFlags flags) {
    Containers::Optional<UnsignedInt> id = tryPickQueueFamily(flags);
    if(id) return *id;
    std::exit(1); /* LCOV_EXCL_LINE */
}

Containers::Optional<UnsignedInt> DeviceProperties::tryPickQueueFamily(const QueueFlags flags) {
    const Containers::ArrayView<const VkQueueFamilyProperties2> properties = queueFamilyProperties();
    for(UnsignedInt i = 0; i != properties.size(); ++i)
        if(QueueFlag(properties[i].queueFamilyProperties.queueFlags) >= flags) return i;

    Error{} << "Vk::DeviceProperties::tryPickQueueFamily(): no" << flags << "found among" << properties.size() << "queue families";
    return {};
}

const VkPhysicalDeviceMemoryProperties2& DeviceProperties::memoryProperties() {
    if(!_state) _state.emplace(*_instance, _handle);

    if(!_state->memoryProperties.sType) {
        _state->memoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
        _state->getMemoryPropertiesImplementation(*this, _state->memoryProperties);
    }

    return _state->memoryProperties;
}

void DeviceProperties::getMemoryPropertiesImplementationDefault(DeviceProperties& self, VkPhysicalDeviceMemoryProperties2& properties) {
    return (**self._instance).GetPhysicalDeviceMemoryProperties(self._handle, &properties.memoryProperties);
}

void DeviceProperties::getMemoryPropertiesImplementationKHR(DeviceProperties& self, VkPhysicalDeviceMemoryProperties2& properties) {
    return (**self._instance).GetPhysicalDeviceMemoryProperties2KHR(self._handle, &properties);
}

void DeviceProperties::getMemoryPropertiesImplementation11(DeviceProperties& self, VkPhysicalDeviceMemoryProperties2& properties) {
    return (**self._instance).GetPhysicalDeviceMemoryProperties2(self._handle, &properties);
}

UnsignedInt DeviceProperties::memoryHeapCount() {
    return memoryProperties().memoryProperties.memoryHeapCount;
}

UnsignedLong DeviceProperties::memoryHeapSize(const UnsignedInt heap) {
    const VkPhysicalDeviceMemoryProperties& properties = memoryProperties().memoryProperties;
    CORRADE_ASSERT(heap < properties.memoryHeapCount,
        "Vk::DeviceProperties::memoryHeapSize(): index" << heap << "out of range for" << properties.memoryHeapCount << "memory heaps", {});
    return properties.memoryHeaps[heap].size;
}

MemoryHeapFlags DeviceProperties::memoryHeapFlags(const UnsignedInt heap) {
    const VkPhysicalDeviceMemoryProperties& properties = memoryProperties().memoryProperties;
    CORRADE_ASSERT(heap < properties.memoryHeapCount,
        "Vk::DeviceProperties::memoryHeapFlags(): index" << heap << "out of range for" << properties.memoryHeapCount << "memory heaps", {});
    return MemoryHeapFlag(properties.memoryHeaps[heap].flags);
}

UnsignedInt DeviceProperties::memoryCount() {
    return memoryProperties().memoryProperties.memoryTypeCount;
}

MemoryFlags DeviceProperties::memoryFlags(const UnsignedInt memory) {
    const VkPhysicalDeviceMemoryProperties& properties = memoryProperties().memoryProperties;
    CORRADE_ASSERT(memory < properties.memoryTypeCount,
        "Vk::DeviceProperties::memoryFlags(): index" << memory << "out of range for" << properties.memoryTypeCount << "memory types", {});
    return MemoryFlag(properties.memoryTypes[memory].propertyFlags);
}

UnsignedInt DeviceProperties::memoryHeapIndex(const UnsignedInt memory) {
    const VkPhysicalDeviceMemoryProperties& properties = memoryProperties().memoryProperties;
    CORRADE_ASSERT(memory < properties.memoryTypeCount,
        "Vk::DeviceProperties::memoryHeapIndex(): index" << memory << "out of range for" << properties.memoryTypeCount << "memory types", {});
    return properties.memoryTypes[memory].heapIndex;
}

UnsignedInt DeviceProperties::pickMemory(const MemoryFlags requiredFlags, const MemoryFlags preferredFlags, const UnsignedInt memories) {
    Containers::Optional<UnsignedInt> id = tryPickMemory(requiredFlags, preferredFlags, memories);
    if(id) return *id;
    std::exit(1); /* LCOV_EXCL_LINE */
}

UnsignedInt DeviceProperties::pickMemory(const MemoryFlags requiredFlags, const UnsignedInt memories) {
    return pickMemory(requiredFlags, {}, memories);
}

Containers::Optional<UnsignedInt> DeviceProperties::tryPickMemory(const MemoryFlags requiredFlags, const MemoryFlags preferredFlags, const UnsignedInt memories) {
    const VkPhysicalDeviceMemoryProperties properties = memoryProperties().memoryProperties;

    /* The picking strategy is basically equivalent to vmaFindMemoryTypeIndex()
       from AMD's Vulkan Memory Allocator -- choosing the one that has the most
       bits set. */
    Int maxPreferredBitCount = -1;
    UnsignedInt maxPreferredBitCountMemory = ~UnsignedInt{};
    UnsignedInt bit = 1;
    for(UnsignedInt i = 0; i != properties.memoryTypeCount; ++i, bit <<= 1) {
        /* Not among considered memory types, skip */
        if(!(memories & bit))
            continue;

        /* Not all required flags present, skip */
        if(!(MemoryFlag(properties.memoryTypes[i].propertyFlags) >= requiredFlags))
            continue;

        /* Check how many of the preferred flags are present and use the one
           with highest count */
        const Int preferredBitCount = Math::popcount(properties.memoryTypes[i].propertyFlags & UnsignedInt(preferredFlags));
        if(preferredBitCount > maxPreferredBitCount) {
            maxPreferredBitCount = preferredBitCount;
            maxPreferredBitCountMemory = i;
        }
    }

    if(maxPreferredBitCount >= 0) return maxPreferredBitCountMemory;

    Error{} << "Vk::DeviceProperties::tryPickMemory(): no" << requiredFlags << "found among" << Math::popcount(memories & ((1 << properties.memoryTypeCount) - 1)) << "considered memory types";
    return {};
}

Containers::Optional<UnsignedInt> DeviceProperties::tryPickMemory(const MemoryFlags requiredFlags, const UnsignedInt memories) {
    return tryPickMemory(requiredFlags, {}, memories);
}

/* Can't be inside an anonymous namespace as it's friended to DeviceProperties */
namespace Implementation {

UnsignedInt enumerateDevicesInto(Instance& instance, Containers::ArrayView<DeviceProperties> out) {
    /* Allocate memory for the output, fetch the handles into it */
    Containers::ArrayView<VkPhysicalDevice> handles{reinterpret_cast<VkPhysicalDevice*>(out.data()), out.size()};
    UnsignedInt count = out.size();
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS_OR_INCOMPLETE(instance->EnumeratePhysicalDevices(instance, &count, handles.data()));

    /* Expect the final count isn't larger than the output array */
    CORRADE_INTERNAL_ASSERT(count <= out.size());

    /* Construct actual DeviceProperties instances from these, go backwards so
       we don't overwrite the not-yet-processed handles */
    for(std::size_t i = count; i != 0; --i)
        new(out.data() + i - 1) DeviceProperties{instance, handles[i - 1]};
    /* Construct the remaining entries so the array destructor doesn't crash */
    for(std::size_t i = count; i != out.size(); ++i)
        new(out.data() + i) DeviceProperties{NoCreate};

    return count;
}

}

Containers::Array<DeviceProperties> enumerateDevices(Instance& instance) {
    /* Retrieve total device count */
    UnsignedInt count;
    MAGNUM_VK_INTERNAL_ASSERT_SUCCESS(instance->EnumeratePhysicalDevices(instance, &count, nullptr));

    /* Fetch device handles, expect the device count didn't change between
       calls */
    Containers::Array<DeviceProperties> out{Containers::NoInit, count};
    CORRADE_INTERNAL_ASSERT_OUTPUT(Implementation::enumerateDevicesInto(instance, out) == out.size());

    return out;
}

Containers::Optional<DeviceProperties> tryPickDevice(Instance& instance) {
    Utility::Arguments args = Implementation::arguments();
    args.parse(instance.state().argc, instance.state().argv);

    /* Pick the first by default */
    if(args.value("device").empty()) {
        Containers::Array1<DeviceProperties> devices{Containers::NoInit};
        if(!Implementation::enumerateDevicesInto(instance, devices)) {
            Error{} << "Vk::tryPickDevice(): no Vulkan devices found";
            return {};
        }

        return std::move(devices.front());
    }

    /* Pick by ID */
    if(args.value("device")[0] >= '0' && args.value("device")[0] <= '9') {
        const UnsignedInt id = args.value<UnsignedInt>("device");
        Containers::Array<DeviceProperties> devices{Containers::NoInit, id + 1};
        const UnsignedInt count = Implementation::enumerateDevicesInto(instance, devices);
        if(id >= count) {
            Error{} << "Vk::tryPickDevice(): index" << id << "out of bounds for" << count << "Vulkan devices";
            return {};
        }

        return std::move(devices[id]);
    }

    Containers::Array<DeviceProperties> devices = enumerateDevices(instance);

    /* Pick by type */
    DeviceType type;
    if(args.value("device") == "integrated")
        type = DeviceType::IntegratedGpu;
    else if(args.value("device") == "discrete")
        type = DeviceType::DiscreteGpu;
    else if(args.value("device") == "virtual")
        type = DeviceType::VirtualGpu;
    else if(args.value("device") == "cpu")
        type = DeviceType::Cpu;
    else {
        Error{} << "Vk::tryPickDevice(): unknown Vulkan device type" << args.value<Containers::StringView>("device");
        return {};
    }

    for(DeviceProperties& device: devices)
        if(device.type() == type) return std::move(device);

    Error{} << "Vk::tryPickDevice(): no" << type << "found among" << devices.size() << "Vulkan devices";
    return {};
}

DeviceProperties pickDevice(Instance& instance) {
    Containers::Optional<DeviceProperties> device = tryPickDevice(instance);
    if(device) return *std::move(device);
    std::exit(1); /* LCOV_EXCL_LINE */
}

Debug& operator<<(Debug& debug, const DeviceType value) {
    debug << "Vk::DeviceType" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::DeviceType::value: return debug << "::" << Debug::nospace << #value;
        _c(Other)
        _c(IntegratedGpu)
        _c(DiscreteGpu)
        _c(VirtualGpu)
        _c(Cpu)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DeviceDriver value) {
    debug << "Vk::DeviceDriver" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::DeviceDriver::value: return debug << "::" << Debug::nospace << #value;
        _c(Unknown)
        _c(AmdOpenSource)
        _c(AmdProprietary)
        _c(ArmProprietary)
        _c(BroadcomProprietary)
        _c(GgpProprietary)
        _c(GoogleSwiftShader)
        _c(ImaginationProprietary)
        _c(IntelOpenSourceMesa)
        _c(IntelProprietaryWindows)
        _c(MesaLlvmpipe)
        _c(MesaRadv)
        _c(MoltenVk)
        _c(NVidiaProprietary)
        _c(QualcommProprietary)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Vulkan docs have the values in decimal, so not converting to hex */
    return debug << "(" << Debug::nospace << Int(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const QueueFlag value) {
    debug << "Vk::QueueFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::QueueFlag::value: return debug << "::" << Debug::nospace << #value;
        _c(Graphics)
        _c(Compute)
        _c(Transfer)
        _c(SparseBinding)
        _c(Protected)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Flag bits should be in hex, unlike plain values */
    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const QueueFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Vk::QueueFlags{}", {
        Vk::QueueFlag::Graphics,
        Vk::QueueFlag::Compute,
        Vk::QueueFlag::Transfer,
        Vk::QueueFlag::SparseBinding,
        Vk::QueueFlag::Protected});
}

Debug& operator<<(Debug& debug, const MemoryHeapFlag value) {
    debug << "Vk::MemoryHeapFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Vk::MemoryHeapFlag::value: return debug << "::" << Debug::nospace << #value;
        _c(DeviceLocal)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    /* Flag bits should be in hex, unlike plain values */
    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedInt(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const MemoryHeapFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Vk::MemoryHeapFlags{}", {
        Vk::MemoryHeapFlag::DeviceLocal});
}

}}
