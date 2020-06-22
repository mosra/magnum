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
#include <Corrade/Containers/StringStl.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/Version.h"

#include "MagnumExternal/Vulkan/flextVkGlobal.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct InstanceVkTest: TestSuite::Tester {
    explicit InstanceVkTest();

    void createInfoConstructDefault();
    void createInfoConstructNoImplicitExtensions();
    void createInfoApplicationInfo();
    void createInfoLayers();
    void createInfoExtensions();
    void createInfoCopiedStrings();

    void construct();
    void constructLayerExtension();
    void constructCommandLineDisable();
    void constructCommandLineEnable();
    void constructMove();
    void constructUnknownLayer();
    void constructUnknownExtension();
    void wrap();
    void populateGlobalFunctionPointers();
};

struct {
    const char* nameDisable;
    const char* nameEnable;
    Containers::Array<const char*> argsDisable, argsEnable;
    bool driverVersionSupported, debugReportEnabled, validationFeaturesEnabled;
    const char* log;
} ConstructCommandLineData[] {
    /* Shouldn't print anything about version, enabled layers/exts if quiet
       output is enabled. */
    {"quiet", "quiet, enabled layer + both extensions",
        Containers::array({"", "--magnum-log", "quiet"}),
        Containers::array({"", "--magnum-log", "quiet",
            "--magnum-enable-layers", "VK_LAYER_KHRONOS_validation",
            "--magnum-enable-instance-extensions", "VK_EXT_debug_report VK_EXT_validation_features"}),
        true, true, true,
        ""},
    {"", "enabled layer + both extensions", nullptr,
        Containers::array({"",
            "--magnum-enable-layers", "VK_LAYER_KHRONOS_validation",
            "--magnum-enable-instance-extensions", "VK_EXT_debug_report VK_EXT_validation_features"}),
        true, true, true,
        "Instance version: Vulkan {}.{}{}\n"
        "Enabled layers:\n"
        "    VK_LAYER_KHRONOS_validation\n"
        "Enabled instance extensions:\n"
        "    VK_EXT_debug_report\n"
        "    VK_EXT_validation_features\n"},
    {"forced invalid version", "forced invalid version, enabled layer + both extensions",
        Containers::array({"", "--magnum-vulkan-version", "eh"}),
        Containers::array({"", "--magnum-vulkan-version", "eh",
            "--magnum-enable-layers", "VK_LAYER_KHRONOS_validation",
            "--magnum-enable-instance-extensions", "VK_EXT_debug_report VK_EXT_validation_features"}),
        true, true, true,
        "Invalid --magnum-vulkan-version eh, ignoring\n"
        "Instance version: Vulkan {}.{}{}\n"
        "Enabled layers:\n"
        "    VK_LAYER_KHRONOS_validation\n"
        "Enabled instance extensions:\n"
        "    VK_EXT_debug_report\n"
        "    VK_EXT_validation_features\n"},
    {"forced version", "forced version, enabled layer + both extensions",
        Containers::array({"", "--magnum-vulkan-version", "1.0"}),
        Containers::array({"", "--magnum-vulkan-version", "1.0",
            "--magnum-enable-layers", "VK_LAYER_KHRONOS_validation",
            "--magnum-enable-instance-extensions", "VK_EXT_debug_report VK_EXT_validation_features"}),
        false, true, true,
        "Instance version: Vulkan 1.0\n"
        "Enabled layers:\n"
        "    VK_LAYER_KHRONOS_validation\n"
        "Enabled instance extensions:\n"
        "    VK_EXT_debug_report\n"
        "    VK_EXT_validation_features\n"},
    {"disabled layer + layer-only extension", "enabled extension",
        Containers::array({"",
            "--magnum-disable-layers", "VK_LAYER_KHRONOS_validation",
            "--magnum-disable-extensions", "VK_EXT_validation_features"}),
        Containers::array({"",
            "--magnum-enable-instance-extensions", "VK_EXT_debug_report"}),
        true, true, false,
        "Instance version: Vulkan {}.{}{}\n"
        "Enabled instance extensions:\n"
        "    VK_EXT_debug_report\n"},
    {"disabled extension", "enabled layer + one extension",
        Containers::array({"",
            "--magnum-disable-extensions", "VK_EXT_debug_report"}),
        Containers::array({"",
            "--magnum-enable-layers", "VK_LAYER_KHRONOS_validation",
            "--magnum-enable-instance-extensions", "VK_EXT_validation_features"}),
        true, false, true,
        "Instance version: Vulkan {}.{}{}\n"
        "Enabled layers:\n"
        "    VK_LAYER_KHRONOS_validation\n"
        "Enabled instance extensions:\n"
        "    VK_EXT_validation_features\n"},
    {"disabled extensions + layer", "",
        Containers::array({"",
            "--magnum-disable-layers", "VK_LAYER_KHRONOS_validation",
            "--magnum-disable-extensions", "VK_EXT_debug_report VK_EXT_validation_features"}),
        nullptr,
        true, false, false,
        "Instance version: Vulkan {}.{}{}\n"},
};

InstanceVkTest::InstanceVkTest() {
    addTests({&InstanceVkTest::createInfoConstructDefault,
              &InstanceVkTest::createInfoConstructNoImplicitExtensions,
              &InstanceVkTest::createInfoApplicationInfo,
              &InstanceVkTest::createInfoLayers,
              &InstanceVkTest::createInfoExtensions,
              &InstanceVkTest::createInfoCopiedStrings,

              &InstanceVkTest::construct,
              &InstanceVkTest::constructLayerExtension});

    addInstancedTests({&InstanceVkTest::constructCommandLineDisable,
                       &InstanceVkTest::constructCommandLineEnable},
        Containers::arraySize(ConstructCommandLineData));

    addTests({&InstanceVkTest::constructMove,
              &InstanceVkTest::constructUnknownLayer,
              &InstanceVkTest::constructUnknownExtension,
              &InstanceVkTest::wrap,
              &InstanceVkTest::populateGlobalFunctionPointers});
}

using namespace Containers::Literals;

void InstanceVkTest::createInfoConstructDefault() {
    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");

    InstanceCreateInfo info;
    CORRADE_VERIFY(info->sType);
    CORRADE_VERIFY(!info->pNext);
    CORRADE_VERIFY(!info->ppEnabledLayerNames);
    CORRADE_COMPARE(info->enabledLayerCount, 0);
    /* Extensions might or might not be enabled */

    CORRADE_VERIFY(info->pApplicationInfo);
    CORRADE_COMPARE(Version(info->pApplicationInfo->apiVersion), enumerateInstanceVersion());
    CORRADE_COMPARE(info->pApplicationInfo->applicationVersion, 0);
    CORRADE_COMPARE(info->pApplicationInfo->engineVersion, 0);
    CORRADE_COMPARE(info->pApplicationInfo->pEngineName, "Magnum"_s);
}

void InstanceVkTest::createInfoConstructNoImplicitExtensions() {
    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");

    InstanceCreateInfo info{InstanceCreateInfo::Flag::NoImplicitExtensions};
    CORRADE_VERIFY(info->sType);
    CORRADE_VERIFY(!info->pNext);
    CORRADE_VERIFY(!info->ppEnabledLayerNames);
    CORRADE_COMPARE(info->enabledLayerCount, 0);
    CORRADE_VERIFY(!info->ppEnabledExtensionNames);
    CORRADE_COMPARE(info->enabledExtensionCount, 0);

    CORRADE_VERIFY(info->pApplicationInfo);
    CORRADE_COMPARE(Version(info->pApplicationInfo->apiVersion), enumerateInstanceVersion());
    CORRADE_COMPARE(info->pApplicationInfo->applicationVersion, 0);
    CORRADE_COMPARE(info->pApplicationInfo->engineVersion, 0);
    CORRADE_COMPARE(info->pApplicationInfo->pEngineName, "Magnum"_s);
}

void InstanceVkTest::createInfoApplicationInfo() {
    Containers::StringView name = "Magnum::Vk::Test::InstanceVkTest"_s;

    InstanceCreateInfo info;
    CORRADE_VERIFY(info->pApplicationInfo);
    CORRADE_VERIFY(!info->pApplicationInfo->pApplicationName);
    CORRADE_COMPARE(Version(info->pApplicationInfo->applicationVersion), Version{});

    /* Setting an empty name should do nothing */
    info.setApplicationInfo({}, {});
    CORRADE_VERIFY(!info->pApplicationInfo->pApplicationName);
    CORRADE_COMPARE(Version(info->pApplicationInfo->applicationVersion), Version{});

    info.setApplicationInfo(name, version(0, 0, 1));
    /* The pointer should be to the global data */
    CORRADE_COMPARE(static_cast<const void*>(info->pApplicationInfo->pApplicationName), name.data());
    CORRADE_COMPARE(Version(info->pApplicationInfo->applicationVersion), version(0, 0, 1));

    /* Setting an empty view should put nullptr back */
    info.setApplicationInfo({}, {});
    CORRADE_VERIFY(!info->pApplicationInfo->pApplicationName);
    CORRADE_COMPARE(Version(info->pApplicationInfo->applicationVersion), Version{});
}

void InstanceVkTest::createInfoLayers() {
    Containers::StringView layer = "VK_LAYER_KHRONOS_validation"_s;
    Containers::StringView another = "VK_LAYER_this_doesnt_exist"_s;

    InstanceCreateInfo info;
    CORRADE_VERIFY(!info->ppEnabledLayerNames);
    CORRADE_COMPARE(info->enabledLayerCount, 0);

    info.addEnabledLayers({layer});
    CORRADE_VERIFY(info->ppEnabledLayerNames);
    CORRADE_COMPARE(info->enabledLayerCount, 1);
    /* The pointer should be to the global data */
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledLayerNames[0]), layer.data());

    info.addEnabledLayers({another, layer});
    CORRADE_COMPARE(info->enabledLayerCount, 3);
    /* The pointer should be to the global data */
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledLayerNames[0]), layer.data());
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledLayerNames[1]), another.data());
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledLayerNames[2]), layer.data());
}

void InstanceVkTest::createInfoExtensions() {
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    InstanceCreateInfo info{InstanceCreateInfo::Flag::NoImplicitExtensions};
    CORRADE_VERIFY(!info->ppEnabledExtensionNames);
    CORRADE_COMPARE(info->enabledExtensionCount, 0);

    info.addEnabledExtensions<Extensions::KHR::external_fence_capabilities>();
    CORRADE_VERIFY(info->ppEnabledExtensionNames);
    CORRADE_COMPARE(info->enabledExtensionCount, 1);
    /* The pointer should be to the global data */
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledExtensionNames[0]),
        Extensions::KHR::external_fence_capabilities::string().data());

    info.addEnabledExtensions(
        {Extensions::KHR::external_semaphore_capabilities{},
         Extensions::KHR::get_physical_device_properties2{}});
    CORRADE_COMPARE(info->enabledExtensionCount, 3);
    /* The pointer should be to the global data */
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledExtensionNames[0]),
        Extensions::KHR::external_fence_capabilities::string().data());
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledExtensionNames[1]),
        Extensions::KHR::external_semaphore_capabilities::string().data());
    CORRADE_COMPARE(static_cast<const void*>(info->ppEnabledExtensionNames[2]),
        Extensions::KHR::get_physical_device_properties2::string().data());
}

void InstanceVkTest::createInfoCopiedStrings() {
    Containers::StringView globalButNotNullTerminated = "VK_LAYER_KHRONOS_validation3"_s.except(1);
    Containers::String localButNullTerminated = Extensions::KHR::external_memory_capabilities::string();

    InstanceCreateInfo info{InstanceCreateInfo::Flag::NoImplicitExtensions};
    info.setApplicationInfo(localButNullTerminated, {})
        .addEnabledLayers({globalButNotNullTerminated})
        .addEnabledExtensions({localButNullTerminated});
    CORRADE_COMPARE(info->enabledLayerCount, 1);
    CORRADE_COMPARE(info->enabledExtensionCount, 1);

    CORRADE_COMPARE(info->pApplicationInfo->pApplicationName, localButNullTerminated);
    CORRADE_VERIFY(info->pApplicationInfo->pApplicationName != localButNullTerminated.data());

    CORRADE_COMPARE(info->ppEnabledLayerNames[0], globalButNotNullTerminated);
    CORRADE_VERIFY(info->ppEnabledLayerNames[0] != globalButNotNullTerminated.data());

    CORRADE_COMPARE(info->ppEnabledExtensionNames[0], localButNullTerminated);
    CORRADE_VERIFY(info->ppEnabledExtensionNames[0] != localButNullTerminated.data());
}

void InstanceVkTest::construct() {
    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");

    {
        Instance instance;
        CORRADE_VERIFY(instance.handle());
        /* Instance function pointers should be populated */
        CORRADE_VERIFY(instance->CreateDevice);
        CORRADE_COMPARE(instance.handleFlags(), HandleFlag::DestroyOnDestruction);
        CORRADE_COMPARE(instance.version(), enumerateInstanceVersion());
        /* Instance version is supported */
        CORRADE_VERIFY(instance.isVersionSupported(enumerateInstanceVersion()));
        CORRADE_VERIFY(!instance.isVersionSupported(Version::None));
        /* No extensions are enabled by default ... */
        CORRADE_VERIFY(!instance.isExtensionEnabled<Extensions::EXT::debug_report>());
        /* ... and thus also no function pointers loaded */
        CORRADE_VERIFY(!instance->CreateDebugReportCallbackEXT);
    }

    /* Shouldn't crash or anything */
    CORRADE_VERIFY(true);
}

void InstanceVkTest::constructLayerExtension() {
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    if(!enumerateLayerProperties().isSupported("VK_LAYER_KHRONOS_validation"))
        CORRADE_SKIP("VK_LAYER_KHRONOS_validation not supported, can't test");
    if(!enumerateInstanceExtensionProperties({"VK_LAYER_KHRONOS_validation"}).isSupported<Extensions::EXT::debug_report>())
        CORRADE_SKIP("VK_EXT_debug_report not supported, can't test");

    Instance instance{InstanceCreateInfo{}
        .setApplicationInfo("InstanceVkTest", version(0, 0, 1))
        .addEnabledLayers({"VK_LAYER_KHRONOS_validation"_s})
        .addEnabledExtensions({
            Extensions::EXT::debug_report::string(),
            "VK_EXT_validation_features"_s
        })};
    CORRADE_VERIFY(instance.handle());

    /* Extensions should be reported as enabled ... */
    CORRADE_VERIFY(instance.isExtensionEnabled<Extensions::EXT::debug_report>());
    CORRADE_VERIFY(instance.isExtensionEnabled(Extensions::EXT::validation_features{}));
    /* ... and function pointers loaded */
    CORRADE_VERIFY(instance->CreateDebugReportCallbackEXT);
    /* no entrypoints to test for EXT_validation_features */
}

void InstanceVkTest::constructCommandLineDisable() {
    auto&& data = ConstructCommandLineData[testCaseInstanceId()];
    setTestCaseDescription(data.nameDisable);

    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    if(!enumerateLayerProperties().isSupported("VK_LAYER_KHRONOS_validation"))
        CORRADE_SKIP("VK_LAYER_KHRONOS_validation not supported, can't test");
    if(!enumerateInstanceExtensionProperties({"VK_LAYER_KHRONOS_validation"}).isSupported<Extensions::EXT::validation_features>())
        CORRADE_SKIP("VK_EXT_validation_features not supported, can't test");

    std::ostringstream out;
    Warning redirectWarning{&out};
    Debug redirectOutput{&out};
    Instance instance{InstanceCreateInfo{Int(data.argsDisable.size()), data.argsDisable,
            InstanceCreateInfo::Flag::NoImplicitExtensions}
        .setApplicationInfo("InstanceVkTest", version(0, 0, 1))
        .addEnabledLayers({"VK_LAYER_KHRONOS_validation"_s})
        .addEnabledExtensions<Extensions::EXT::debug_report,
            Extensions::EXT::validation_features>()};
    CORRADE_VERIFY(instance.handle());
    CORRADE_COMPARE(instance.isVersionSupported(enumerateInstanceVersion()), data.driverVersionSupported);
    CORRADE_COMPARE(instance.isExtensionEnabled<Extensions::EXT::debug_report>(), data.debugReportEnabled);
    CORRADE_COMPARE(instance.isExtensionEnabled<Extensions::EXT::validation_features>(), data.validationFeaturesEnabled);

    /** @todo cleanup when Debug::toString() or some similar utility exists */
    UnsignedInt major = versionMajor(enumerateInstanceVersion());
    UnsignedInt minor = versionMinor(enumerateInstanceVersion());
    UnsignedInt patch = versionPatch(enumerateInstanceVersion());
    /* Vulkan 1.0 instances report no patch version, special-case that */
    CORRADE_COMPARE(out.str(), Utility::formatString(data.log, major, minor, patch ? Utility::formatString(".{}", patch) : ""));

    /* Verify that the entrypoint is actually (not) loaded as expected, to
       avoid all the above reporting being just smoke & mirrors */
    CORRADE_COMPARE(!!instance->CreateDebugReportCallbackEXT, data.debugReportEnabled);
}

void InstanceVkTest::constructCommandLineEnable() {
    auto&& data = ConstructCommandLineData[testCaseInstanceId()];
    setTestCaseDescription(data.nameEnable);

    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    if(!enumerateLayerProperties().isSupported("VK_LAYER_KHRONOS_validation"))
        CORRADE_SKIP("VK_LAYER_KHRONOS_validation not supported, can't test");
    if(!enumerateInstanceExtensionProperties({"VK_LAYER_KHRONOS_validation"}).isSupported<Extensions::EXT::validation_features>())
        CORRADE_SKIP("VK_EXT_validation_features not supported, can't test");

    std::ostringstream out;
    Warning redirectWarning{&out};
    Debug redirectOutput{&out};
    Instance instance{InstanceCreateInfo{Int(data.argsEnable.size()), data.argsEnable,
            InstanceCreateInfo::Flag::NoImplicitExtensions}
        /* Nothing enabled by the application */
    };
    CORRADE_VERIFY(instance.handle());
    CORRADE_COMPARE(instance.isVersionSupported(enumerateInstanceVersion()), data.driverVersionSupported);
    CORRADE_COMPARE(instance.isExtensionEnabled<Extensions::EXT::debug_report>(), data.debugReportEnabled);
    CORRADE_COMPARE(instance.isExtensionEnabled<Extensions::EXT::validation_features>(), data.validationFeaturesEnabled);

    /** @todo cleanup when Debug::toString() or some similar utility exists */
    UnsignedInt major = versionMajor(enumerateInstanceVersion());
    UnsignedInt minor = versionMinor(enumerateInstanceVersion());
    UnsignedInt patch = versionPatch(enumerateInstanceVersion());
    /* Vulkan 1.0 instances report no patch version, special-case that */
    CORRADE_COMPARE(out.str(), Utility::formatString(data.log, major, minor, patch ? Utility::formatString(".{}", patch) : ""));

    /* Verify that the entrypoint is actually (not) loaded as expected, to
       avoid all the above reporting being just smoke & mirrors */
    CORRADE_COMPARE(!!instance->CreateDebugReportCallbackEXT, data.debugReportEnabled);
}

void InstanceVkTest::constructMove() {
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    InstanceExtensionProperties extensions = enumerateInstanceExtensionProperties();
    if(!extensions.isSupported<Extensions::KHR::get_physical_device_properties2>())
        CORRADE_SKIP("VK_KHR_get_physical_device_properties2 not supported, can't test");

    Instance a{InstanceCreateInfo{}
        .setApplicationInfo("InstanceVkTest", version(0, 0, 1))
        .addEnabledExtensions<Extensions::KHR::get_physical_device_properties2>()};
    VkInstance handle = a.handle();
    Version version = a.version();
    CORRADE_VERIFY(handle);
    CORRADE_VERIFY(version != Version{});

    Instance b = std::move(a);
    CORRADE_VERIFY(!a.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(b.handle(), handle);
    CORRADE_COMPARE(b.version(), version);
    CORRADE_VERIFY(b.isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>());
    /* Function pointers in a are left in whatever state they were before, as
       that doesn't matter */
    CORRADE_VERIFY(b->CreateDevice);

    Instance c{NoCreate};
    c = std::move(b);
    CORRADE_VERIFY(!b.handle());
    CORRADE_COMPARE(b.handleFlags(), HandleFlags{});
    CORRADE_COMPARE(c.handleFlags(), HandleFlag::DestroyOnDestruction);
    CORRADE_COMPARE(c.handle(), handle);
    CORRADE_COMPARE(c.version(), version);
    CORRADE_VERIFY(c.isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>());
    /* Everything is swapped, including function pointers */
    CORRADE_VERIFY(!b->CreateDevice);
    CORRADE_VERIFY(c->CreateDevice);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Instance>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Instance>::value);
}

void InstanceVkTest::constructUnknownLayer() {
    CORRADE_SKIP("Currently this hits an internal assert, which can't be tested.");

    std::ostringstream out;
    Error redirectError{&out};
    Instance instance{InstanceCreateInfo{}
        .addEnabledLayers({"VK_LAYER_this_doesnt_exist"_s})};
    CORRADE_COMPARE(out.str(), "TODO");
}

void InstanceVkTest::constructUnknownExtension() {
    CORRADE_SKIP("Currently this hits an internal assert, which can't be tested.");

    std::ostringstream out;
    Error redirectError{&out};
    Instance instance{InstanceCreateInfo{}
        .addEnabledExtensions({"VK_this_doesnt_exist"_s})};
    CORRADE_COMPARE(out.str(), "TODO");
}

void InstanceVkTest::wrap() {
    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    InstanceExtensionProperties properties = enumerateInstanceExtensionProperties();
    if(!properties.isSupported<Extensions::EXT::debug_report>())
        CORRADE_SKIP("VK_EXT_debug_report not supported, can't test");
    if(!properties.isSupported<Extensions::KHR::get_physical_device_properties2>())
        CORRADE_SKIP("VK_KHR_get_physical_device_properties2 not supported, can't test");

    InstanceCreateInfo info;
    info.addEnabledExtensions<
        Extensions::EXT::debug_report,
        Extensions::KHR::get_physical_device_properties2>();

    VkInstance instance;
    CORRADE_COMPARE(Result(vkCreateInstance(info, nullptr, &instance)), Result::Success);
    CORRADE_VERIFY(instance);

    {
        /* Wrapping should load the basic function pointers */
        auto wrapped = Instance::wrap(instance, Version::Vk11, {
            Extensions::EXT::debug_report::string()
        }, HandleFlag::DestroyOnDestruction);
        CORRADE_VERIFY(wrapped->DestroyInstance);

        /* Specified version should be reported as supported but higher not
           regardless of the actual driver version */
        CORRADE_VERIFY(wrapped.isVersionSupported(Version::Vk11));
        CORRADE_VERIFY(!wrapped.isVersionSupported(Version::Vk12));

        /* Listed extensions should be reported as enabled and function
           pointers loaded as well */
        CORRADE_VERIFY(wrapped.isExtensionEnabled<Extensions::EXT::debug_report>());
        CORRADE_VERIFY(wrapped->CreateDebugReportCallbackEXT);

        /* Unlisted not, but function pointers should still be loaded as the
           actual instance does have the extension enabled */
        CORRADE_VERIFY(!wrapped.isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>());
        CORRADE_VERIFY(wrapped->GetPhysicalDeviceProperties2KHR);

        /* Releasing won't destroy anything ... */
        CORRADE_COMPARE(wrapped.release(), instance);
    }

    /* ...so we can wrap it again, non-owned, and then destroy it manually */
    auto wrapped = Instance::wrap(instance, Version::Vk10, {});
    CORRADE_VERIFY(wrapped->DestroyInstance);
    wrapped->DestroyInstance(instance, nullptr);
}

void InstanceVkTest::populateGlobalFunctionPointers() {
    vkDestroyInstance = nullptr;

    Instance instance;
    CORRADE_VERIFY(!vkDestroyInstance);
    instance.populateGlobalFunctionPointers();
    CORRADE_VERIFY(vkDestroyInstance);
    CORRADE_VERIFY(vkDestroyInstance == instance->DestroyInstance);
}

}}}}

CORRADE_TEST_MAIN(Magnum::Vk::Test::InstanceVkTest)
