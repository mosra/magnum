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
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/Memory.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/Version.h"
#include "Magnum/Vk/VulkanTester.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DevicePropertiesVkTest: VulkanTester {
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

    void driverProperties();

    void queueFamilies();
    void queueFamiliesOutOfRange();
    void queueFamiliesPick();
    void queueFamiliesPickFailed();

    void memoryHeaps();
    void memoryHeapOutOfRange();

    void memoryTypes();
    void memoryTypeOutOfRange();
    void memoryTypesPick();
    void memoryTypesPickIgnoreSomePreferred();
    void memoryTypesPickFailed();

    void pickDevice();
    void pickDeviceIndex();
    void pickDeviceType();
    void pickDeviceError();
};

const struct {
    const char* name;
    Containers::Array<const char*> args;
    const char* message;
} PickDeviceErrorData[] {
    {"nothing for type found", Containers::array({"", "--magnum-device", "virtual"}),
        "Vk::tryPickDevice(): no Vk::DeviceType::VirtualGpu found among {} Vulkan devices\n"},
    {"index out of bounds", Containers::array({"", "--magnum-device", "666"}),
        "Vk::tryPickDevice(): index 666 out of bounds for {} Vulkan devices\n"},
    {"unknown type", Containers::array({"", "--magnum-device", "FAST"}),
        "Vk::tryPickDevice(): unknown Vulkan device type FAST\n"}
};

DevicePropertiesVkTest::DevicePropertiesVkTest(): VulkanTester{NoCreate} {
    addTests({&DevicePropertiesVkTest::enumerate,
              &DevicePropertiesVkTest::constructMove,
              &DevicePropertiesVkTest::wrap,

              &DevicePropertiesVkTest::enumerateExtensions,
              &DevicePropertiesVkTest::enumerateExtensionsWithKhronosValidationLayer,
              &DevicePropertiesVkTest::enumerateExtensionsNonexistentLayer,

              &DevicePropertiesVkTest::extensionConstructMove,
              &DevicePropertiesVkTest::extensionIsSupported,
              &DevicePropertiesVkTest::extensionNamedRevision,

              &DevicePropertiesVkTest::driverProperties,

              &DevicePropertiesVkTest::queueFamilies,
              &DevicePropertiesVkTest::queueFamiliesOutOfRange,
              &DevicePropertiesVkTest::queueFamiliesPick,
              &DevicePropertiesVkTest::queueFamiliesPickFailed,

              &DevicePropertiesVkTest::memoryHeaps,
              &DevicePropertiesVkTest::memoryHeapOutOfRange,

              &DevicePropertiesVkTest::memoryTypes,
              &DevicePropertiesVkTest::memoryTypeOutOfRange,
              &DevicePropertiesVkTest::memoryTypesPick,
              &DevicePropertiesVkTest::memoryTypesPickIgnoreSomePreferred,
              &DevicePropertiesVkTest::memoryTypesPickFailed,

              &DevicePropertiesVkTest::pickDevice,
              &DevicePropertiesVkTest::pickDeviceIndex,
              &DevicePropertiesVkTest::pickDeviceType});

    addInstancedTests({&DevicePropertiesVkTest::pickDeviceError},
        Containers::arraySize(PickDeviceErrorData));
}

void DevicePropertiesVkTest::enumerate() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    Debug{} << "Found" << devices.size() << "devices";
    CORRADE_VERIFY(!devices.empty());

    for(DeviceProperties& device: devices) {
        CORRADE_ITERATION(device.name());

        CORRADE_VERIFY(device.handle());
        CORRADE_COMPARE_AS(device.version(), Version::Vk10,
            TestSuite::Compare::GreaterOrEqual);
        /* Device version is supported */
        CORRADE_VERIFY(device.isVersionSupported(device.version()));
        CORRADE_VERIFY(!device.isVersionSupported(Version::None));
        CORRADE_COMPARE_AS(device.driverVersion(), Version::Vk10,
            TestSuite::Compare::GreaterOrEqual);
        CORRADE_VERIFY(device.type() != DeviceType::Other);
        CORRADE_VERIFY(!device.name().isEmpty());
    }
}

void DevicePropertiesVkTest::constructMove() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());
    VkPhysicalDevice handle = devices[0].handle();
    Containers::StringView name = devices[0].name();

    DeviceProperties a = std::move(devices[0]);
    CORRADE_COMPARE(a.handle(), handle);
    CORRADE_COMPARE(a.name(), name);

    DeviceProperties b = DeviceProperties::wrap(instance(), nullptr);
    b = std::move(a);
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.name(), name);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<DeviceProperties>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<DeviceProperties>::value);
}

void DevicePropertiesVkTest::wrap() {
    VkPhysicalDevice handle;
    UnsignedInt count = 1;
    auto result = Result(instance()->EnumeratePhysicalDevices(instance(), &count, &handle));
    {
        /** @todo clean up once Compare::AnyOf exists */
        CORRADE_ITERATION(result);
        CORRADE_VERIFY(result == Result::Success || result == Result::Incomplete);
    }

    DeviceProperties wrapped = DeviceProperties::wrap(instance(), handle);
    CORRADE_VERIFY(wrapped.handle());

    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_COMPARE(wrapped.name(), devices[0].name());
}

void DevicePropertiesVkTest::driverProperties() {
    Containers::Optional<DeviceProperties> device = tryPickDevice(instance());
    CORRADE_VERIFY(device);

    Debug{} << "Driver ID:" << device->driver();

    if(device->driver() == DeviceDriver::Unknown) {
        CORRADE_COMPARE(device->driverName(), "");
        CORRADE_COMPARE(device->driverInfo(), "");
        CORRADE_SKIP("KHR_driver_properties not supported.");
    }

    CORRADE_VERIFY(!device->driverName().isEmpty());
    {
        CORRADE_EXPECT_FAIL_IF(device->driver() == DeviceDriver::GoogleSwiftShader,
            "SwiftShader reports empty driver info.");
        CORRADE_VERIFY(!device->driverInfo().isEmpty());
    }
}

void DevicePropertiesVkTest::enumerateExtensions() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
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

    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
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
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
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
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
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
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
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

void DevicePropertiesVkTest::queueFamilies() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    Debug{} << "Available queue family count:" << devices[0].queueFamilyCount();

    CORRADE_COMPARE_AS(devices[0].queueFamilyCount(), 0,
        TestSuite::Compare::Greater);

    for(std::size_t i = 0; i != devices[0].queueFamilyCount(); ++i) {
        CORRADE_ITERATION(i);
        CORRADE_ITERATION(devices[0].queueFamilyFlags(i));

        CORRADE_VERIFY(devices[0].queueFamilyFlags(i) != QueueFlags{});
        CORRADE_COMPARE(devices[0].queueFamilyFlags(i), QueueFlag(devices[0].queueFamilyProperties()[i].queueFamilyProperties.queueFlags));

        CORRADE_COMPARE_AS(devices[0].queueFamilySize(i), 0,
            TestSuite::Compare::Greater);
        CORRADE_COMPARE(devices[0].queueFamilySize(i), devices[0].queueFamilyProperties()[i].queueFamilyProperties.queueCount);
    }
}

void DevicePropertiesVkTest::queueFamiliesOutOfRange() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    const UnsignedInt count = devices[0].queueFamilyCount();

    std::ostringstream out;
    Error redirectError{&out};
    devices[0].queueFamilySize(count);
    devices[0].queueFamilyFlags(count);
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Vk::DeviceProperties::queueFamilySize(): index {0} out of range for {0} entries\n"
        "Vk::DeviceProperties::queueFamilyFlags(): index {0} out of range for {0} entries\n", count));
}

void DevicePropertiesVkTest::queueFamiliesPick() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    Containers::Optional<UnsignedInt> id = devices[0].tryPickQueueFamily(QueueFlag::Compute|QueueFlag::Graphics);
    CORRADE_VERIFY(id);
    CORRADE_COMPARE_AS(*id, devices[0].queueFamilyCount(), TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(devices[0].queueFamilyFlags(*id),
        QueueFlag::Compute|QueueFlag::Graphics,
        TestSuite::Compare::GreaterOrEqual);

    /* Pick should return the same ID, and shouldn't exit */
    CORRADE_COMPARE(devices[0].pickQueueFamily(QueueFlag::Compute|QueueFlag::Graphics), id);
}

void DevicePropertiesVkTest::queueFamiliesPickFailed() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!devices[0].tryPickQueueFamily(QueueFlag(0xc0ffeee0)));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Vk::DeviceProperties::tryPickQueueFamily(): no Vk::QueueFlag(0xc0ffeee0) found among {} queue families\n", devices[0].queueFamilyCount()));
}

void DevicePropertiesVkTest::memoryHeaps() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    Debug{} << "Available memory heap count:" << devices[0].memoryHeapCount();

    CORRADE_COMPARE_AS(devices[0].memoryHeapCount(), 0,
        TestSuite::Compare::Greater);

    bool atLeastOneDeviceLocal = false;
    for(std::size_t i = 0; i != devices[0].memoryHeapCount(); ++i) {
        CORRADE_ITERATION(i);
        CORRADE_ITERATION(devices[0].memoryHeapFlags(i));

        if(devices[0].memoryHeapFlags(i) & MemoryHeapFlag::DeviceLocal)
            atLeastOneDeviceLocal = true;

        /* A heap should have at least 64 MB (more like at least 512 MB
           nowadays but let's be conservative) */
        CORRADE_COMPARE_AS(devices[0].memoryHeapSize(i), std::size_t{1024*1024*64},
            TestSuite::Compare::Greater);
    }

    CORRADE_VERIFY(atLeastOneDeviceLocal);
}

void DevicePropertiesVkTest::memoryHeapOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    const UnsignedInt count = devices[0].memoryHeapCount();

    std::ostringstream out;
    Error redirectError{&out};
    devices[0].memoryHeapSize(count);
    devices[0].memoryHeapFlags(count);
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Vk::DeviceProperties::memoryHeapSize(): index {0} out of range for {0} memory heaps\n"
        "Vk::DeviceProperties::memoryHeapFlags(): index {0} out of range for {0} memory heaps\n", count));
}

void DevicePropertiesVkTest::memoryTypes() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    Debug{} << "Available memory type count:" << devices[0].memoryCount();

    CORRADE_COMPARE_AS(devices[0].memoryCount(), 0,
        TestSuite::Compare::Greater);

    bool atLeastOneDeviceLocal = false;
    for(std::size_t i = 0; i != devices[0].memoryCount(); ++i) {
        CORRADE_ITERATION(i);
        CORRADE_ITERATION(devices[0].memoryFlags(i));

        if(devices[0].memoryFlags(i) & MemoryFlag::DeviceLocal)
            atLeastOneDeviceLocal = true;

        /* Heap index should be in range */
        CORRADE_COMPARE_AS(devices[0].memoryHeapIndex(i), devices[0].memoryHeapCount(),
            TestSuite::Compare::Less);
    }

    CORRADE_VERIFY(atLeastOneDeviceLocal);
}

void DevicePropertiesVkTest::memoryTypeOutOfRange() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    const UnsignedInt count = devices[0].memoryCount();

    std::ostringstream out;
    Error redirectError{&out};
    devices[0].memoryFlags(count);
    devices[0].memoryHeapIndex(count);
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Vk::DeviceProperties::memoryFlags(): index {0} out of range for {0} memory types\n"
        "Vk::DeviceProperties::memoryHeapIndex(): index {0} out of range for {0} memory types\n", count));
}

void DevicePropertiesVkTest::memoryTypesPick() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    Containers::Optional<UnsignedInt> id = devices[0].tryPickMemory(MemoryFlag::HostVisible|MemoryFlag::HostCoherent);
    CORRADE_VERIFY(id);
    CORRADE_COMPARE_AS(*id, devices[0].memoryCount(), TestSuite::Compare::Less);
    CORRADE_COMPARE_AS(devices[0].memoryFlags(*id),
        MemoryFlag::HostVisible|MemoryFlag::HostCoherent,
        TestSuite::Compare::GreaterOrEqual);

    /* Pick should return the same ID, and shouldn't exit. Test also the
       overload with no preferred flags. */
    CORRADE_COMPARE(devices[0].pickMemory(MemoryFlag::HostVisible|MemoryFlag::HostCoherent), id);
    CORRADE_COMPARE(devices[0].pickMemory(MemoryFlag::HostVisible|MemoryFlag::HostCoherent, ~UnsignedInt{}), id);

    /* If we put the same into preferred flags and leave the required empty, it
       should pick the same (the first one as well) */
    Containers::Optional<UnsignedInt> idPreferred = devices[0].tryPickMemory({}, MemoryFlag::HostVisible|MemoryFlag::HostCoherent);
    CORRADE_COMPARE(idPreferred, id);
}

void DevicePropertiesVkTest::memoryTypesPickIgnoreSomePreferred() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    Containers::Optional<UnsignedInt> id = devices[0].tryPickMemory({}, MemoryFlag::HostVisible|MemoryFlag::HostCoherent|MemoryFlag(0xcafe0000u));
    CORRADE_VERIFY(id);
    CORRADE_COMPARE_AS(*id, devices[0].memoryCount(), TestSuite::Compare::Less);
    /* Should pick all what makes sense and ignore what doesn't */
    CORRADE_COMPARE_AS(devices[0].memoryFlags(*id),
        MemoryFlag::HostVisible|MemoryFlag::HostCoherent,
        TestSuite::Compare::GreaterOrEqual);

    /* And should be the same as picking the same required or halfway */
    CORRADE_COMPARE(id, devices[0].tryPickMemory(MemoryFlag::HostVisible|MemoryFlag::HostCoherent));
    CORRADE_COMPARE(id, devices[0].tryPickMemory(MemoryFlag::HostVisible, MemoryFlag::HostCoherent));
}

void DevicePropertiesVkTest::memoryTypesPickFailed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!devices[0].tryPickMemory(MemoryFlag(0xc0ffeee0)));
    CORRADE_VERIFY(!devices[0].tryPickMemory({}, {}, 0));
    CORRADE_VERIFY(!devices[0].tryPickMemory({}, 0));
    CORRADE_COMPARE(out.str(), Utility::formatString(
        "Vk::DeviceProperties::tryPickMemory(): no Vk::MemoryFlag(0xc0ffeee0) found among {} considered memory types\n"
        "Vk::DeviceProperties::tryPickMemory(): no Vk::MemoryFlags{{}} found among 0 considered memory types\n"
        "Vk::DeviceProperties::tryPickMemory(): no Vk::MemoryFlags{{}} found among 0 considered memory types\n", devices[0].memoryCount()));
}

void DevicePropertiesVkTest::pickDevice() {
    /* Default behavior */
    Containers::Optional<DeviceProperties> device = tryPickDevice(instance());
    CORRADE_VERIFY(device);
}

void DevicePropertiesVkTest::pickDeviceIndex() {
    Containers::Array<DeviceProperties> devices = enumerateDevices(instance());
    CORRADE_VERIFY(!devices.empty());

    /* Pick the last one */
    CORRADE_COMPARE_AS(devices.size(), 10, TestSuite::Compare::Less);
    const char id[] {char('0' + devices.size() - 1), '\0'};
    const char* argv[] {"", "--magnum-device", id};

    /* Creating another dedicated instance so we can pass custom args */
    Instance instance{InstanceCreateInfo{Int(Containers::arraySize(argv)), argv}};

    Containers::Optional<DeviceProperties> device = tryPickDevice(instance);
    CORRADE_VERIFY(device);
}

void DevicePropertiesVkTest::pickDeviceType() {
    const char* argv[] {"", "--magnum-device", "cpu"};

    /* Creating a dedicated instance so we can pass custom args */
    Instance instance2{InstanceCreateInfo{Int(Containers::arraySize(argv)), argv}};

    Containers::Optional<DeviceProperties> device = tryPickDevice(instance2);
    if(!device) CORRADE_SKIP("No CPU device found.");

    CORRADE_VERIFY(device->type() == DeviceType::Cpu);
}

void DevicePropertiesVkTest::pickDeviceError() {
    auto&& data = PickDeviceErrorData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    /* Creating a dedicated instance so we can pass custom args */
    Instance instance2{InstanceCreateInfo{Int(data.args.size()), const_cast<const char**>(data.args.data())}};

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_VERIFY(!tryPickDevice(instance2));
    CORRADE_COMPARE(out.str(), Utility::formatString(data.message, enumerateDevices(instance2).size()));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DevicePropertiesVkTest)
