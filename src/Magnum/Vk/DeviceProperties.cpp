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
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/Implementation/InstanceState.h"

namespace Magnum { namespace Vk {

struct DeviceProperties::State {
    VkPhysicalDeviceProperties2 properties{};
};

DeviceProperties::DeviceProperties(NoCreateT) noexcept: _instance{}, _handle{} {}

DeviceProperties::DeviceProperties(Instance& instance, VkPhysicalDevice handle): _instance{&instance}, _handle{handle} {}

/* The VkDeviceProperties handle doesn't need to be destroyed so it's enough to
   just rely on the implicit behavior */
DeviceProperties::DeviceProperties(DeviceProperties&&) noexcept = default;

DeviceProperties::~DeviceProperties() = default;

DeviceProperties& DeviceProperties::operator=(DeviceProperties&&) noexcept = default;

Containers::StringView DeviceProperties::name() {
    return properties().properties.deviceName;
}

const VkPhysicalDeviceProperties2& DeviceProperties::properties() {
    if(!_state) _state.emplace();

    /* Properties not fetched yet, do that now */
    if(!_state->properties.sType) {
        _state->properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

        _instance->state().getPhysicalDevicePropertiesImplementation(*this, _state->properties);
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

Containers::Array<DeviceProperties> enumerateDevices(Instance& instance) {
    /* Retrieve total device count */
    UnsignedInt count;
    MAGNUM_VK_INTERNAL_ASSERT_RESULT(instance->EnumeratePhysicalDevices(instance, &count, nullptr));

    /* Allocate memory for the output, fetch the handles into it */
    Containers::Array<DeviceProperties> out{Containers::NoInit, count};
    Containers::ArrayView<VkPhysicalDevice> handles{reinterpret_cast<VkPhysicalDevice*>(out.data()), count};
    MAGNUM_VK_INTERNAL_ASSERT_RESULT(instance->EnumeratePhysicalDevices(instance, &count, handles.data()));

    /* Expect the device count didn't change between calls */
    CORRADE_INTERNAL_ASSERT(count == out.size());

    /* Construct actual DeviceProperties instances from these, go backwards so
       we don't overwrite the not-yet-processed handles */
    for(std::size_t i = count; i != 0; --i)
        new(out.data() + i - 1) DeviceProperties{instance, handles[i - 1]};

    return out;
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

}}
