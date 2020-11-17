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
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/Queue.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/Version.h"
#include "Magnum/Vk/VulkanTester.h"

#include "MagnumExternal/Vulkan/flextVkGlobal.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DeviceVkTest: VulkanTester {
    explicit DeviceVkTest();

    void createInfoConstruct();
    void createInfoConstructNoImplicitExtensions();
    void createInfoExtensions();
    void createInfoCopiedStrings();
    void createInfoNoQueuePriorities();
    void createInfoWrongQueueOutputCount();
    void createInfoRvalue();

    void construct();
    void constructQueueFromFlags();
    void constructExtensions();
    void constructTransferDeviceProperties();
    void constructExtensionsCommandLineDisable();
    void constructExtensionsCommandLineEnable();
    void constructMultipleQueues();
    void constructRawQueue();
    void constructMove();
    void constructUnknownExtension();
    void constructNoQueue();

    void wrap();

    void populateGlobalFunctionPointers();
};

struct {
    const char* nameDisable;
    const char* nameEnable;
    Containers::Array<const char*> argsDisable, argsEnable;
    bool driverVersionSupported, debugMarkerEnabled, maintenance1Enabled;
    const char* log;
} ConstructCommandLineData[] {
    /* Shouldn't print anything about version, enabled layers/exts if quier
       output is enabled. */
    {"quiet", "quiet, enabled extensions",
        Containers::array({"", "--magnum-log", "quiet"}),
        Containers::array({"", "--magnum-log", "quiet",
            "--magnum-enable-extensions", "VK_EXT_debug_marker VK_KHR_maintenance1"}),
        true, true, true,
        ""},
    {"", "enabled extensions", nullptr,
        Containers::array({"",
            "--magnum-enable-extensions", "VK_EXT_debug_marker VK_KHR_maintenance1"}),
        true, true, true,
        "Device: {}\n"
        "Device version: Vulkan {}.{}{}\n"
        "Enabled device extensions:\n"
        "    VK_EXT_debug_marker\n"
        "    VK_KHR_maintenance1\n"},
    {"forced version", "forced version, enabled extensions",
        Containers::array({"",
            "--magnum-vulkan-version", "1.0"}),
        Containers::array({"",
            "--magnum-vulkan-version", "1.0",
            "--magnum-enable-extensions", "VK_EXT_debug_marker VK_KHR_maintenance1"}),
        false, true, true,
        "Device: {}\n"
        "Device version: Vulkan 1.0\n"
        "Enabled device extensions:\n"
        "    VK_EXT_debug_marker\n"
        "    VK_KHR_maintenance1\n"},
    {"disabled one extension", "enabled one extension",
        Containers::array({"",
            "--magnum-disable-extensions", "VK_EXT_debug_marker"}),
        Containers::array({"",
            "--magnum-enable-extensions", "VK_KHR_maintenance1"}),
        true, false, true,
        "Device: {}\n"
        "Device version: Vulkan {}.{}{}\n"
        "Enabled device extensions:\n"
        "    VK_KHR_maintenance1\n"},
    {"disabled extensions", "",
        Containers::array({"",
            "--magnum-disable-extensions", "VK_EXT_debug_marker VK_KHR_maintenance1"}),
        nullptr,
        true, false, false,
        "Device: {}\n"
        "Device version: Vulkan {}.{}{}\n"},
};

DeviceVkTest::DeviceVkTest(): VulkanTester{NoCreate} {
    addTests({&DeviceVkTest::createInfoConstruct,
              &DeviceVkTest::createInfoConstructNoImplicitExtensions,
              &DeviceVkTest::createInfoExtensions,
              &DeviceVkTest::createInfoCopiedStrings,
              &DeviceVkTest::createInfoNoQueuePriorities,
              &DeviceVkTest::createInfoWrongQueueOutputCount,
              &DeviceVkTest::createInfoRvalue,

              &DeviceVkTest::construct,
              &DeviceVkTest::constructQueueFromFlags,
              &DeviceVkTest::constructExtensions,
              &DeviceVkTest::constructTransferDeviceProperties});

    addInstancedTests({&DeviceVkTest::constructExtensionsCommandLineDisable,
                       &DeviceVkTest::constructExtensionsCommandLineEnable},
        Containers::arraySize(ConstructCommandLineData));

    addTests({&DeviceVkTest::constructMultipleQueues,
              &DeviceVkTest::constructRawQueue,

              &DeviceVkTest::constructMove,
              &DeviceVkTest::constructUnknownExtension,
              &DeviceVkTest::constructNoQueue,

              &DeviceVkTest::wrap,
              &DeviceVkTest::populateGlobalFunctionPointers});
}

using namespace Containers::Literals;

void DeviceVkTest::createInfoConstruct() {
    DeviceCreateInfo info{pickDevice(instance())};
    CORRADE_VERIFY(info->sType);
    CORRADE_VERIFY(!info->pNext);
    /* Extensions might or might not be enabled */
}

void DeviceVkTest::createInfoConstructNoImplicitExtensions() {
    DeviceCreateInfo info{pickDevice(instance()), DeviceCreateInfo::Flag::NoImplicitExtensions};
    CORRADE_VERIFY(info->sType);
    CORRADE_VERIFY(!info->pNext);
    /* No extensions enabled as we explicitly disabled that */
    CORRADE_VERIFY(!info->ppEnabledExtensionNames);
    CORRADE_COMPARE(info->enabledExtensionCount, 0);
}

void DeviceVkTest::createInfoExtensions() {
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    DeviceCreateInfo info{pickDevice(instance()), DeviceCreateInfo::Flag::NoImplicitExtensions};
    CORRADE_VERIFY(!info->ppEnabledExtensionNames);
    CORRADE_COMPARE(info->enabledExtensionCount, 0);

    info.addEnabledExtensions<Extensions::KHR::maintenance1>();
    CORRADE_VERIFY(info->ppEnabledExtensionNames);
    CORRADE_COMPARE(info->enabledExtensionCount, 1);
    /* The pointer should be to the global data */
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledExtensionNames[0]),
        Extensions::KHR::maintenance1::string().data());

    info.addEnabledExtensions({
        Extensions::KHR::draw_indirect_count{},
        Extensions::KHR::get_memory_requirements2{}
    });
    CORRADE_COMPARE(info->enabledExtensionCount, 3);
    /* The pointer should be to the global data */
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledExtensionNames[0]),
        Extensions::KHR::maintenance1::string().data());
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledExtensionNames[1]),
        Extensions::KHR::draw_indirect_count::string().data());
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledExtensionNames[2]),
        Extensions::KHR::get_memory_requirements2::string().data());
}

void DeviceVkTest::createInfoCopiedStrings() {
    Containers::StringView globalButNotNullTerminated = "VK_KHR_maintenance25"_s.except(1);
    Containers::String localButNullTerminated = Extensions::KHR::draw_indirect_count::string();

    DeviceCreateInfo info{pickDevice(instance()), DeviceCreateInfo::Flag::NoImplicitExtensions};
    info.addEnabledExtensions({
        globalButNotNullTerminated,
        localButNullTerminated
    });
    CORRADE_COMPARE(info->enabledExtensionCount, 2);

    CORRADE_COMPARE(info->ppEnabledExtensionNames[0], globalButNotNullTerminated);
    CORRADE_VERIFY(info->ppEnabledExtensionNames[0] != globalButNotNullTerminated.data());

    CORRADE_COMPARE(info->ppEnabledExtensionNames[1], localButNullTerminated);
    CORRADE_VERIFY(info->ppEnabledExtensionNames[1] != localButNullTerminated.data());
}

void DeviceVkTest::createInfoNoQueuePriorities() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    DeviceCreateInfo{pickDevice(instance())}.addQueues(0, {}, {});
    CORRADE_COMPARE(out.str(), "Vk::DeviceCreateInfo::addQueues(): at least one queue priority has to be specified\n");
}

void DeviceVkTest::createInfoWrongQueueOutputCount() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Queue a{NoCreate}, b{NoCreate};
    DeviceCreateInfo{pickDevice(instance())}.addQueues(0, {0.0f, 1.0f, 0.3f}, {a, b});
    CORRADE_COMPARE(out.str(), "Vk::DeviceCreateInfo::addQueues(): expected 3 outuput queue references but got 2\n");
}

void DeviceVkTest::createInfoRvalue() {
    /* Verify that there actually are graphics queues so we don't exit inside
       addQueues() */
    CORRADE_VERIFY(pickDevice(instance()).tryPickQueueFamily(QueueFlag::Graphics));

    Float zero[1]{};
    Queue a{NoCreate}, b{NoCreate}, c{NoCreate}, d{NoCreate};
    Containers::Reference<Queue> referenceA[1]{a};
    Containers::Reference<Queue> referenceC[1]{c};

    VkDeviceQueueCreateInfo rawQueueInfo{};
    rawQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    rawQueueInfo.pQueuePriorities = zero;
    rawQueueInfo.queueFamilyIndex = 0;
    rawQueueInfo.queueCount = 1;

    DeviceCreateInfo&& info = DeviceCreateInfo{pickDevice(instance())}
        .addEnabledExtensions(Containers::ArrayView<const Containers::StringView>{})
        .addEnabledExtensions(std::initializer_list<Containers::StringView>{})
        .addEnabledExtensions(Containers::ArrayView<const Extension>{})
        .addEnabledExtensions(std::initializer_list<Extension>{})
        .addEnabledExtensions<>()
        .addQueues(0, zero, referenceA)
        .addQueues(0, {0.0f}, {b})
        .addQueues(QueueFlag::Graphics, zero, referenceC)
        .addQueues(QueueFlag::Graphics, {0.0f}, {d})
        .addQueues(rawQueueInfo);

    /* Just to test something, main point is that the above compiles, links and
       returns a &&. Can't test anything related to the contents because the
       destructor gets called at the end of the expression. */
    CORRADE_VERIFY(&info);
}

void DeviceVkTest::construct() {
    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");

    {
        DeviceProperties deviceProperties = pickDevice(instance());
        Queue queue{NoCreate};
        Device device{instance(), DeviceCreateInfo{deviceProperties}
            .addQueues(0, {0.0f}, {queue})
        };
        CORRADE_VERIFY(device.handle());
        /* Device function pointers should be populated */
        CORRADE_VERIFY(device->CreateBuffer);
        CORRADE_COMPARE(device.handleFlags(), HandleFlag::DestroyOnDestruction);
        CORRADE_COMPARE(device.version(), deviceProperties.version());
        /* Device version is supported */
        CORRADE_VERIFY(device.isVersionSupported(deviceProperties.version()));
        CORRADE_VERIFY(!device.isVersionSupported(Version::None));
        /* No extensions are enabled by default ... */
        CORRADE_VERIFY(!device.isExtensionEnabled<Extensions::EXT::debug_marker>());
        /* ... and thus also no function pointers loaded */
        CORRADE_VERIFY(!device->CmdDebugMarkerInsertEXT);

        /* The queue should be also filled in */
        CORRADE_VERIFY(queue.handle());

        /* Device properties should be lazy-populated and different from the
           above instances because we didn't transfer the ownership */
        CORRADE_COMPARE(device.properties().name(), deviceProperties.name());
        CORRADE_VERIFY(&device.properties().properties() != &deviceProperties.properties());
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void DeviceVkTest::constructQueueFromFlags() {
    DeviceProperties deviceProperties = pickDevice(instance());

    /* Verify that there actually are graphics queues so we don't exit after */
    CORRADE_VERIFY(deviceProperties.tryPickQueueFamily(QueueFlag::Graphics));

    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{deviceProperties}
        .addQueues(QueueFlag::Graphics, {0.0f}, {queue})};
    CORRADE_VERIFY(device.handle());

    /* The queue should be filled in like usual */
    CORRADE_VERIFY(queue.handle());
}

void DeviceVkTest::constructExtensions() {
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    if(!enumerateLayerProperties().isSupported("VK_LAYER_KHRONOS_validation"))
        CORRADE_SKIP("VK_LAYER_KHRONOS_validation not supported, can't test");

    /* Creating a dedicated instance so we can pass custom args and enable
       layers & exts independently */
    Instance instance{InstanceCreateInfo{}
        .addEnabledLayers({"VK_LAYER_KHRONOS_validation"})
        /* Needed by VK_EXT_debug_marker */
        .addEnabledExtensions<Extensions::EXT::debug_report>()
    };

    DeviceProperties deviceProperties = pickDevice(instance);
    ExtensionProperties extensions = deviceProperties.enumerateExtensionProperties({"VK_LAYER_KHRONOS_validation"});
    if(!extensions.isSupported<Extensions::EXT::debug_marker>())
        CORRADE_SKIP("VK_EXT_debug_marker not supported, can't test");
    if(!extensions.isSupported<Extensions::KHR::maintenance1>())
        CORRADE_SKIP("VK_KHR_maintenance1 not supported, can't test");

    Queue queue{NoCreate};
    Device device{instance, DeviceCreateInfo{deviceProperties}
        .addQueues(0, {0.0f}, {queue})
        .addEnabledExtensions({
            Extensions::EXT::debug_marker::string(),
            "VK_KHR_maintenance1"_s
        })};
    CORRADE_VERIFY(device.handle());

    /* Extensions should be reported as enabled ... */
    CORRADE_VERIFY(device.isExtensionEnabled<Extensions::EXT::debug_marker>());
    CORRADE_VERIFY(device.isExtensionEnabled(Extensions::KHR::maintenance1{}));
    /* ... and function pointers loaded */
    CORRADE_VERIFY(device->CmdDebugMarkerInsertEXT);
    CORRADE_VERIFY(device->TrimCommandPoolKHR);
}

void DeviceVkTest::constructTransferDeviceProperties() {
    DeviceProperties deviceProperties = pickDevice(instance());
    const void* vkProperties = &deviceProperties.properties();
    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{std::move(deviceProperties)}
        .addQueues(0, {0.0f}, {queue})
    };

    /* Device properties should be the same address as in the original instance
       because the ownership got transferred through */
    CORRADE_COMPARE(&device.properties().properties(), vkProperties);
}

void DeviceVkTest::constructExtensionsCommandLineDisable() {
    auto&& data = ConstructCommandLineData[testCaseInstanceId()];
    setTestCaseDescription(data.nameDisable);

    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");
    if(std::getenv("MAGNUM_DISABLE_LAYERS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_LAYERS environment variable set");
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    if(!enumerateLayerProperties().isSupported("VK_LAYER_KHRONOS_validation"))
        CORRADE_SKIP("VK_LAYER_KHRONOS_validation not supported, can't test");

    /* Creating a dedicated instance so we can pass custom args and enable
       layers independently */
    Instance instance2{InstanceCreateInfo{Int(data.argsDisable.size()), data.argsDisable}
        .addEnabledLayers({"VK_LAYER_KHRONOS_validation"})
        /* Needed by VK_EXT_debug_marker */
        .addEnabledExtensions<Extensions::EXT::debug_report>()
    };

    DeviceProperties deviceProperties = pickDevice(instance2);
    ExtensionProperties extensions = deviceProperties.enumerateExtensionProperties({"VK_LAYER_KHRONOS_validation"});
    if(!extensions.isSupported<Extensions::EXT::debug_marker>())
        CORRADE_SKIP("VK_EXT_debug_marker not supported, can't test");
    if(!extensions.isSupported<Extensions::KHR::maintenance1>())
        CORRADE_SKIP("VK_KHR_maintenance1 not supported, can't test");

    std::ostringstream out;
    Debug redirectOutput{&out};
    Queue queue{NoCreate};
    Device device{instance2, DeviceCreateInfo{deviceProperties, DeviceCreateInfo::Flag::NoImplicitExtensions}
        .addQueues(0, {0.0f}, {queue})
        .addEnabledExtensions<
            Extensions::EXT::debug_marker,
            Extensions::KHR::maintenance1
        >()};
    CORRADE_VERIFY(device.handle());
    CORRADE_COMPARE(device.isVersionSupported(deviceProperties.version()), data.driverVersionSupported);
    CORRADE_COMPARE(device.isExtensionEnabled<Extensions::EXT::debug_marker>(), data.debugMarkerEnabled);
    CORRADE_COMPARE(device.isExtensionEnabled<Extensions::KHR::maintenance1>(), data.maintenance1Enabled);

    /** @todo cleanup when Debug::toString() or some similar utility exists */
    UnsignedInt major = versionMajor(deviceProperties.version());
    UnsignedInt minor = versionMinor(deviceProperties.version());
    UnsignedInt patch = versionPatch(deviceProperties.version());
    /* SwiftShader reports just 1.1 with no patch version, special-case that */
    CORRADE_COMPARE(out.str(), Utility::formatString(data.log, deviceProperties.name(), major, minor, patch ? Utility::formatString(".{}", patch) : ""));

    /* Verify that the entrypoint is actually (not) loaded as expected, to
       avoid all the above reporting being just smoke & mirrors */
    CORRADE_COMPARE(!!device->CmdDebugMarkerInsertEXT, data.debugMarkerEnabled);
    CORRADE_COMPARE(!!device->TrimCommandPoolKHR, data.maintenance1Enabled);
}

void DeviceVkTest::constructExtensionsCommandLineEnable() {
    auto&& data = ConstructCommandLineData[testCaseInstanceId()];
    setTestCaseDescription(data.nameEnable);

    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");
    if(std::getenv("MAGNUM_DISABLE_LAYERS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_LAYERS environment variable set");
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    if(!enumerateLayerProperties().isSupported("VK_LAYER_KHRONOS_validation"))
        CORRADE_SKIP("VK_LAYER_KHRONOS_validation not supported, can't test");

    /* Creating a dedicated instance so we can pass custom args and enable
       layers independently */
    Instance instance2{InstanceCreateInfo{Int(data.argsEnable.size()), data.argsEnable}
        .addEnabledLayers({"VK_LAYER_KHRONOS_validation"})
        /* Needed by VK_EXT_debug_marker */
        .addEnabledExtensions<Extensions::EXT::debug_report>()
    };

    DeviceProperties deviceProperties = pickDevice(instance2);
    ExtensionProperties extensions = deviceProperties.enumerateExtensionProperties({"VK_LAYER_KHRONOS_validation"});
    if(!extensions.isSupported<Extensions::EXT::debug_marker>())
        CORRADE_SKIP("VK_EXT_debug_marker not supported, can't test");
    if(!extensions.isSupported<Extensions::KHR::maintenance1>())
        CORRADE_SKIP("VK_KHR_maintenance1 not supported, can't test");

    std::ostringstream out;
    Debug redirectOutput{&out};
    Queue queue{NoCreate};
    Device device{instance2, DeviceCreateInfo{pickDevice(instance2), DeviceCreateInfo::Flag::NoImplicitExtensions}
        .addQueues(0, {0.0f}, {queue})
        /* Nothing enabled by the application */
    };
    CORRADE_VERIFY(device.handle());
    CORRADE_COMPARE(device.isVersionSupported(deviceProperties.version()), data.driverVersionSupported);
    CORRADE_COMPARE(device.isExtensionEnabled<Extensions::EXT::debug_marker>(), data.debugMarkerEnabled);
    CORRADE_COMPARE(device.isExtensionEnabled<Extensions::KHR::maintenance1>(), data.maintenance1Enabled);

    /** @todo cleanup when Debug::toString() or some similar utility exists */
    UnsignedInt major = versionMajor(deviceProperties.version());
    UnsignedInt minor = versionMinor(deviceProperties.version());
    UnsignedInt patch = versionPatch(deviceProperties.version());
    /* SwiftShader reports just 1.1 with no patch version, special-case that */
    CORRADE_COMPARE(out.str(), Utility::formatString(data.log, deviceProperties.name(), major, minor, patch ? Utility::formatString(".{}", patch) : ""));

    /* Verify that the entrypoint is actually (not) loaded as expected, to
       avoid all the above reporting being just smoke & mirrors */
    CORRADE_COMPARE(!!device->CmdDebugMarkerInsertEXT, data.debugMarkerEnabled);
    CORRADE_COMPARE(!!device->TrimCommandPoolKHR, data.maintenance1Enabled);
}

void DeviceVkTest::constructMultipleQueues() {
    /* Find a GPU that has at least two queue families and at least four
       queues in one family */
    Containers::Array<DeviceProperties> deviceProperties = enumerateDevices(instance());

    DeviceProperties* deviceWithMultipleQueues = nullptr;
    UnsignedInt largeFamily = ~UnsignedInt{};
    for(DeviceProperties& i: deviceProperties) {
        if(i.queueFamilyCount() < 2) continue;
        for(UnsignedInt family = 0; family != i.queueFamilyCount(); ++family) {
            if(i.queueFamilySize(family) < 4) continue;
            largeFamily = family;
            break;
        }

        deviceWithMultipleQueues = &i;
        break;
    }

    if(!deviceWithMultipleQueues || largeFamily == ~UnsignedInt{})
        CORRADE_SKIP("No device with at least two queue families and at least four queues in one family found, can't test");

    const UnsignedInt otherFamily = largeFamily == 0 ? 1 : 0;

    constexpr Float zero = 0.0f;
    VkDeviceQueueCreateInfo rawQueueInfo{};
    rawQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    rawQueueInfo.pQueuePriorities = &zero;
    rawQueueInfo.queueFamilyIndex = otherFamily;
    rawQueueInfo.queueCount = 1;

    Queue a{NoCreate}, b{NoCreate}, c{NoCreate};
    Device device{instance(), DeviceCreateInfo{*deviceWithMultipleQueues}
        /* Request a raw queue in the middle of it all to test we skip it when
           populating the outputs, and correctly offset the next IDs. According
           to the spec we can request each family only once, which makes the
           implementation and testing slightly simpler. */
        .addQueues(rawQueueInfo)
        /* Request multiple queues in a single family to test we correctly loop
           over these as well */
        .addQueues(largeFamily, {0.5f, 0.75f, 1.0f}, {a, b, c})};

    /* All queues should be found and different */
    CORRADE_VERIFY(a.handle());
    CORRADE_VERIFY(b.handle());
    CORRADE_VERIFY(c.handle());
    CORRADE_VERIFY(a.handle() != b.handle());
    CORRADE_VERIFY(a.handle() != c.handle());
    CORRADE_VERIFY(b.handle() != c.handle());

    /* Fetching the same queue again should give the same handle */
    VkQueue aAgain;
    device->GetDeviceQueue(device, largeFamily, 0, &aAgain);
    CORRADE_COMPARE(aAgain, a.handle());

    /* Fetch the raw queue, should be different from everything else as well */
    VkQueue rawQueue;
    device->GetDeviceQueue(device, otherFamily, 0, &rawQueue);
    CORRADE_VERIFY(rawQueue);
    CORRADE_VERIFY(rawQueue != a.handle());
    CORRADE_VERIFY(rawQueue != b.handle());
    CORRADE_VERIFY(rawQueue != c.handle());
}

void DeviceVkTest::constructRawQueue() {
    /* Testing a subset of constructQueues() again because not all drivers
       have multiple queues and we want to have the coverage */
    constexpr Float zero = 0.0f;
    VkDeviceQueueCreateInfo rawQueueInfo{};
    rawQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    rawQueueInfo.pQueuePriorities = &zero;
    rawQueueInfo.queueFamilyIndex = 0;
    rawQueueInfo.queueCount = 1;
    Device device{instance(), DeviceCreateInfo{pickDevice(instance())}
        .addQueues(rawQueueInfo)};

    /* Fetch the raw queue */
    VkQueue rawQueue;
    device->GetDeviceQueue(device, 0, 0, &rawQueue);
    CORRADE_VERIFY(rawQueue);
}

void DeviceVkTest::constructMove() {
    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");

    DeviceProperties deviceProperties = pickDevice(instance());
    ExtensionProperties extensions = deviceProperties.enumerateExtensionProperties();
    if(!extensions.isSupported<Extensions::KHR::maintenance1>())
        CORRADE_SKIP("VK_KHR_maintenance1 not supported, can't test");

    Queue queue{NoCreate};
    Device a{instance(), DeviceCreateInfo{deviceProperties}
        .addQueues(0, {0.0f}, {queue})
        .addEnabledExtensions<Extensions::KHR::maintenance1>()
    };
    VkDevice handle = a.handle();
    Version version = a.version();
    CORRADE_VERIFY(handle);
    CORRADE_VERIFY(version != Version{});
    CORRADE_VERIFY(version != Version::None);

    Device b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.version(), version);
    CORRADE_COMPARE(b.properties().version(), version);
    CORRADE_VERIFY(b.isExtensionEnabled<Extensions::KHR::maintenance1>());
    /* Function pointers in a are left in whatever state they were before, as
       that doesn't matter */
    CORRADE_VERIFY(b->CreateBuffer);

    Device c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.version(), version);
    CORRADE_COMPARE(c.properties().version(), version);
    CORRADE_VERIFY(c.isExtensionEnabled<Extensions::KHR::maintenance1>());
    /* Everything is swapped, including function pointers */
    CORRADE_VERIFY(!b->CreateBuffer);
    CORRADE_VERIFY(c->CreateBuffer);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Device>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Device>::value);
}

void DeviceVkTest::constructUnknownExtension() {
    CORRADE_SKIP("Currently this hits an internal assert, which can't be tested.");

    std::ostringstream out;
    Error redirectError{&out};
    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{pickDevice(instance())}
        .addQueues(0, {0.0f}, {queue})
        .addEnabledExtensions({"VK_this_doesnt_exist"_s})};
    CORRADE_COMPARE(out.str(), "TODO");
}

void DeviceVkTest::constructNoQueue() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Device device{instance(), DeviceCreateInfo{pickDevice(instance())}};
    CORRADE_COMPARE(out.str(), "Vk::Device: needs to be created with at least one queue\n");
}

void DeviceVkTest::wrap() {
    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");
    if(std::getenv("MAGNUM_DISABLE_LAYERS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_LAYERS environment variable set");
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    if(!enumerateLayerProperties().isSupported("VK_LAYER_KHRONOS_validation"))
        CORRADE_SKIP("VK_LAYER_KHRONOS_validation not supported, can't test");

    /* Creating a dedicated instance so we can enable layers independently */
    Instance instance2{InstanceCreateInfo{}
        .addEnabledLayers({"VK_LAYER_KHRONOS_validation"})
        /* Needed by VK_EXT_debug_marker */
        .addEnabledExtensions<Extensions::EXT::debug_report>()
    };

    DeviceProperties deviceProperties = pickDevice(instance2);
    ExtensionProperties extensions = deviceProperties.enumerateExtensionProperties({"VK_LAYER_KHRONOS_validation"});
    if(!extensions.isSupported<Extensions::EXT::debug_marker>())
        CORRADE_SKIP("VK_EXT_debug_marker not supported, can't test");
    if(!extensions.isSupported<Extensions::KHR::maintenance1>())
        CORRADE_SKIP("VK_KHR_maintenance1 not supported, can't test");

    VkDevice device;
    Queue queue{NoCreate};
    CORRADE_COMPARE(Result(instance2->CreateDevice(deviceProperties,
        DeviceCreateInfo{pickDevice(instance2)}
            .addQueues(0, {0.0f}, {queue})
            .addEnabledExtensions<
                Extensions::EXT::debug_marker,
                Extensions::KHR::maintenance1
            >(),
        nullptr, &device)), Result::Success);
    CORRADE_VERIFY(device);
    /* Populating the queue handle is done only from Device itself, so it won't
       happen here -- would need to call vkGetDeviceQueue[2] directly */
    CORRADE_VERIFY(!queue.handle());

    {
        /* Wrapping should load the basic function pointers */
        auto wrapped = Device::wrap(instance2, device, Version::Vk11, {
            Extensions::EXT::debug_marker::string()
        }, HandleFlag::DestroyOnDestruction);
        CORRADE_VERIFY(wrapped->DestroyDevice);

        /* Specified version should be reported as supported but higher not
           regardless of the actual driver version */
        CORRADE_VERIFY(wrapped.isVersionSupported(Version::Vk11));
        CORRADE_VERIFY(!wrapped.isVersionSupported(Version::Vk12));

        /* Listed extensions should be reported as enabled and function
           pointers loaded as well */
        CORRADE_VERIFY(wrapped.isExtensionEnabled<Extensions::EXT::debug_marker>());
        CORRADE_VERIFY(wrapped->CmdDebugMarkerInsertEXT);

        /* Unlisted not, but function pointers should still be loaded as the
           actual instance does have the extension enabled */
        CORRADE_VERIFY(!wrapped.isExtensionEnabled<Extensions::KHR::maintenance1>());
        CORRADE_VERIFY(wrapped->TrimCommandPoolKHR);

        /* Releasing won't destroy anything ... */
        CORRADE_COMPARE(wrapped.release(), device);
    }

    /* ...so we can wrap it again, non-owned, and then destroy it manually */
    auto wrapped = Device::wrap(instance2, device, Version::Vk10, {});
    CORRADE_VERIFY(wrapped->DestroyDevice);
    wrapped->DestroyDevice(device, nullptr);
}

void DeviceVkTest::populateGlobalFunctionPointers() {
    vkDestroyDevice = nullptr;

    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{pickDevice(instance())}
        .addQueues(0, {0.0f}, {queue})
    };
    CORRADE_VERIFY(!vkDestroyDevice);
    device.populateGlobalFunctionPointers();
    CORRADE_VERIFY(vkDestroyDevice);
    CORRADE_VERIFY(vkDestroyDevice == device->DestroyDevice);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::DeviceVkTest)
