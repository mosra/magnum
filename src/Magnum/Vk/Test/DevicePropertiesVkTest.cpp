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

#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/Version.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DevicePropertiesVkTest: TestSuite::Tester {
    explicit DevicePropertiesVkTest();

    void enumerate();
    void constructMove();
    void wrap();

    /* Most of this tested already in ExtensionPropertiesVkTest, this only
       covers what isn't there already */
    void enumerateExtensions();
    void enumerateExtensionsWithKhronosValidationLayer();
    void enumerateExtensionsNonexistentLayer();

    void extensionConstructMove();
    void extensionIsSupported();
    void extensionNamedRevision();

    Instance _instance;
};

DevicePropertiesVkTest::DevicePropertiesVkTest(): _instance{InstanceCreateInfo{arguments().first, arguments().second}} {
    addTests({&DevicePropertiesVkTest::enumerate,
              &DevicePropertiesVkTest::constructMove,
              &DevicePropertiesVkTest::wrap,

              &DevicePropertiesVkTest::enumerateExtensions,
              &DevicePropertiesVkTest::enumerateExtensionsWithKhronosValidationLayer,
              &DevicePropertiesVkTest::enumerateExtensionsNonexistentLayer,

              &DevicePropertiesVkTest::extensionConstructMove,
              &DevicePropertiesVkTest::extensionIsSupported,
              &DevicePropertiesVkTest::extensionNamedRevision});
}

void DevicePropertiesVkTest::enumerate() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(_instance);
    Debug{} << "Found" << devices.size() << "devices";
    CORRADE_VERIFY(!devices.empty());

    for(DeviceProperties& device: devices) {
        CORRADE_ITERATION(device.name());

        CORRADE_VERIFY(device.handle());
        CORRADE_COMPARE_AS(device.apiVersion(), Version::Vk10,
            TestSuite::Compare::GreaterOrEqual);
        CORRADE_COMPARE_AS(device.driverVersion(), Version::Vk10,
            TestSuite::Compare::GreaterOrEqual);
        CORRADE_VERIFY(device.type() != DeviceType::Other);
        CORRADE_VERIFY(!device.name().isEmpty());
    }
}

void DevicePropertiesVkTest::constructMove() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(_instance);
    CORRADE_VERIFY(!devices.empty());
    VkPhysicalDevice handle = devices[0].handle();
    Containers::StringView name = devices[0].name();

    DeviceProperties a = std::move(devices[0]);
    CORRADE_COMPARE(a.handle(), handle);
    CORRADE_COMPARE(a.name(), name);

    DeviceProperties b = DeviceProperties::wrap(_instance, nullptr);
    b = std::move(a);
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.name(), name);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DeviceProperties>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DeviceProperties>::value);
}

void DevicePropertiesVkTest::wrap() {
    VkPhysicalDevice handle;
    UnsignedInt count = 1;
    auto result = Result(_instance->EnumeratePhysicalDevices(_instance, &count, &handle));
    {
        /** @todo clean up once Compare::AnyOf exists */
        CORRADE_ITERATION(result);
        CORRADE_VERIFY(result == Result::Success || result == Result::Incomplete);
    }

    DeviceProperties wrapped = DeviceProperties::wrap(_instance, handle);
    CORRADE_VERIFY(wrapped.handle());

    Containers::Array<DeviceProperties> devices = enumerateDevices(_instance);
    CORRADE_COMPARE(wrapped.name(), devices[0].name());
}

void DevicePropertiesVkTest::enumerateExtensions() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(_instance);
    CORRADE_VERIFY(!devices.empty());

    ExtensionProperties properties = devices[0].enumerateExtensionProperties();
    Debug{} << "Available device extension count:" << properties.names().size();

    CORRADE_COMPARE_AS(properties.count(), 0, TestSuite::Compare::Greater);

    /* The extension list should be sorted and unique (so Less, not
       LessOrEqual) */
    Containers::ArrayView<const Containers::StringView> extensions = properties.names();
    for(std::size_t i = 1; i != extensions.size(); ++i) {
        CORRADE_COMPARE_AS(extensions[i - 1], extensions[i],
            TestSuite::Compare::Less);
    }
}

void DevicePropertiesVkTest::enumerateExtensionsWithKhronosValidationLayer() {
    if(!enumerateLayerProperties().isSupported("VK_LAYER_KHRONOS_validation"))
        CORRADE_SKIP("VK_LAYER_KHRONOS_validation not supported, can't test");

    Containers::Array<DeviceProperties> devices = enumerateDevices(_instance);
    CORRADE_VERIFY(!devices.empty());

    /* There should be more extensions with this layer enabled */
    ExtensionProperties global = devices[0].enumerateExtensionProperties();
    ExtensionProperties withKhronosValidation = devices[0].enumerateExtensionProperties({"VK_LAYER_KHRONOS_validation"});
    CORRADE_COMPARE_AS(global.count(),
        withKhronosValidation.count(),
        TestSuite::Compare::Less);

    /* VK_EXT_tooling_info is only in the layer */
    CORRADE_VERIFY(!global.isSupported("VK_EXT_tooling_info"));
    CORRADE_VERIFY(withKhronosValidation.isSupported("VK_EXT_tooling_info"));
}

void DevicePropertiesVkTest::enumerateExtensionsNonexistentLayer() {
    CORRADE_SKIP("Currently this hits an internal assert, which can't be tested.");

    std::ostringstream out;
    Error redirectError{&out};
    enumerateInstanceExtensionProperties({"VK_LAYER_this_doesnt_exist"});
    CORRADE_COMPARE(out.str(), "TODO");
}

void DevicePropertiesVkTest::extensionConstructMove() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(_instance);
    CORRADE_VERIFY(!devices.empty());

    ExtensionProperties a = devices[0].enumerateExtensionProperties();
    const UnsignedInt count = a.count();
    if(!count) CORRADE_SKIP("No extensions reported, can't test");

    ExtensionProperties b = std::move(a);
    CORRADE_COMPARE(b.count(), count);

    ExtensionProperties c{NoCreate};
    c = std::move(b);
    CORRADE_COMPARE(c.count(), count);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<ExtensionProperties>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<ExtensionProperties>::value);
}

void DevicePropertiesVkTest::extensionIsSupported() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(_instance);
    CORRADE_VERIFY(!devices.empty());

    ExtensionProperties properties = devices[0].enumerateExtensionProperties();

    /* This extension should be available almost always */
    if(!properties.isSupported("VK_KHR_maintenance1"))
        CORRADE_SKIP("VK_KHR_maintenance1 not supported, can't fully test");

    /* Verify the overloads that take our extension wrappers work as well */
    CORRADE_VERIFY(properties.isSupported<Extensions::KHR::maintenance1>());
    CORRADE_VERIFY(properties.isSupported(Extensions::KHR::maintenance1{}));
}

void DevicePropertiesVkTest::extensionNamedRevision() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(_instance);
    CORRADE_VERIFY(!devices.empty());

    ExtensionProperties properties = devices[0].enumerateExtensionProperties();

    /* This extension should be available almost always */
    if(!properties.isSupported("VK_KHR_maintenance1"))
        CORRADE_SKIP("VK_KHR_maintenance1 not supported, can't fully test");

    /* This isn't tested in ExtensionPropertiesVkTest because there's an
       overload which takes only InstanceExtensions */
    CORRADE_COMPARE_AS(properties.revision<Extensions::KHR::maintenance1>(), 0,
        TestSuite::Compare::GreaterOrEqual);
    CORRADE_COMPARE_AS(properties.revision(Extensions::KHR::maintenance1{}), 0,
        TestSuite::Compare::GreaterOrEqual);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DevicePropertiesVkTest)
