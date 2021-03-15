/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/Vk/DeviceCreateInfo.h"
#include "Magnum/Vk/DeviceFeatures.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/InstanceCreateInfo.h"
#include "Magnum/Vk/LayerProperties.h"
#include "Magnum/Vk/Queue.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/Version.h"
#include "Magnum/Vk/VulkanTester.h"

#include "Magnum/Vk/Implementation/DeviceFeatures.h"
    /* for deviceFeaturesPortabilitySubset() */

#include "MagnumExternal/Vulkan/flextVkGlobal.h"

namespace Magnum { namespace Vk { namespace Test { namespace {

struct DeviceVkTest: VulkanTester {
    explicit DeviceVkTest();

    void createInfoConstruct();
    void createInfoConstructNoImplicitExtensions();
    void createInfoExtensions();
    void createInfoExtensionsCopiedStrings();
    void createInfoFeatures();
    void createInfoFeaturesReplaceExternal();
    void createInfoFeaturesReplacePrevious();
    void createInfoFeaturesEnableAllResetAll();
    void createInfoFeaturesNothingInCoreFeatures();
    void createInfoNoQueuePriorities();
    void createInfoWrongQueueOutputCount();
    void createInfoConstructCopy();
    void createInfoConstructMove();
    void createInfoRvalue();

    void construct();
    void constructQueueFromFlags();
    void constructExtensions();
    void constructFeatures();
    void constructFeaturesFromExtensions();
    void constructDeviceCreateInfoConstReference();
    void constructTransferDeviceProperties();
    void constructExtensionsCommandLineDisable();
    void constructExtensionsCommandLineEnable();
    void constructWorkaroundsCommandLineDisable();
    void constructMultipleQueues();
    void constructRawQueue();
    void constructFeatureNotSupported();
    void constructFeatureWithoutExtension();
    void constructNoQueue();

    void constructNoPortability();
    void constructNoPortabilityEnablePortabilityFeatures();
    void constructPortability();

    void tryCreateAlreadyCreated();
    void tryCreateUnknownExtension();

    void wrap();
    void wrapAlreadyCreated();

    void populateGlobalFunctionPointers();
};

struct {
    const char* nameDisable;
    const char* nameEnable;
    Containers::Array<const char*> argsDisable, argsEnable;
    bool driverVersionSupported, debugMarkerEnabled, maintenance1Enabled;
    const char* log;
} ConstructCommandLineData[] {
    /* Shouldn't print anything about device/version, enabled layers/exts if
       quiet output is enabled. */
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

struct {
    const char* name;
    bool shouldPassAlways;
    Containers::Array<const char*> args;
    const char* log;
} ConstructWorkaroundsCommandLineData[] {
    {"default", false, nullptr,
        "Device: {}\n"
        "Device version: Vulkan {}.{}{}\n"
        "Using device driver workarounds:\n"
        "    swiftshader-image-copy-extent-instead-of-layers\n"
        "    swiftshader-spirv-multi-entrypoint-conflicting-locations\n"},
    /* Shouldn't print anything if quiet output is enabled */
    {"quiet", true,
        Containers::array({"",
            "--magnum-log", "quiet"}),
        ""},
    {"disabled workarounds", true,
        Containers::array({"",
            "--magnum-disable-workarounds", "swiftshader-image-copy-extent-instead-of-layers swiftshader-spirv-multi-entrypoint-conflicting-locations"}),
        "Device: {}\n"
        "Device version: Vulkan {}.{}{}\n"}
};

DeviceVkTest::DeviceVkTest(): VulkanTester{NoCreate} {
    addTests({&DeviceVkTest::createInfoConstruct,
              &DeviceVkTest::createInfoConstructNoImplicitExtensions,
              &DeviceVkTest::createInfoExtensions,
              &DeviceVkTest::createInfoExtensionsCopiedStrings,
              &DeviceVkTest::createInfoFeatures,
              &DeviceVkTest::createInfoFeaturesReplaceExternal,
              &DeviceVkTest::createInfoFeaturesReplacePrevious,
              &DeviceVkTest::createInfoFeaturesEnableAllResetAll,
              &DeviceVkTest::createInfoFeaturesNothingInCoreFeatures,
              &DeviceVkTest::createInfoNoQueuePriorities,
              &DeviceVkTest::createInfoWrongQueueOutputCount,
              &DeviceVkTest::createInfoConstructCopy,
              &DeviceVkTest::createInfoConstructMove,
              &DeviceVkTest::createInfoRvalue,

              &DeviceVkTest::construct,
              &DeviceVkTest::constructQueueFromFlags,
              &DeviceVkTest::constructExtensions,
              &DeviceVkTest::constructFeatures,
              &DeviceVkTest::constructFeaturesFromExtensions,
              &DeviceVkTest::constructDeviceCreateInfoConstReference,
              &DeviceVkTest::constructTransferDeviceProperties});

    addInstancedTests({&DeviceVkTest::constructExtensionsCommandLineDisable,
                       &DeviceVkTest::constructExtensionsCommandLineEnable},
        Containers::arraySize(ConstructCommandLineData));

    addInstancedTests({&DeviceVkTest::constructWorkaroundsCommandLineDisable},
        Containers::arraySize(ConstructWorkaroundsCommandLineData));

    addTests({&DeviceVkTest::constructMultipleQueues,
              &DeviceVkTest::constructRawQueue,
              &DeviceVkTest::constructFeatureNotSupported,
              &DeviceVkTest::constructFeatureWithoutExtension,
              &DeviceVkTest::constructNoQueue,

              &DeviceVkTest::constructNoPortability,
              &DeviceVkTest::constructNoPortabilityEnablePortabilityFeatures,
              &DeviceVkTest::constructPortability,

              &DeviceVkTest::tryCreateAlreadyCreated,
              &DeviceVkTest::tryCreateUnknownExtension,

              &DeviceVkTest::wrap,
              &DeviceVkTest::wrapAlreadyCreated,

              &DeviceVkTest::populateGlobalFunctionPointers});
}

using namespace Containers::Literals;

void DeviceVkTest::createInfoConstruct() {
    DeviceCreateInfo info{pickDevice(instance())};
    CORRADE_VERIFY(info->sType);
    CORRADE_VERIFY(!info->pNext);
    /* Extensions might or might not be enabled */
    CORRADE_VERIFY(!info->pEnabledFeatures);
}

void DeviceVkTest::createInfoConstructNoImplicitExtensions() {
    DeviceCreateInfo info{pickDevice(instance()), DeviceCreateInfo::Flag::NoImplicitExtensions};
    CORRADE_VERIFY(info->sType);
    CORRADE_VERIFY(!info->pNext);
    /* No extensions enabled as we explicitly disabled that */
    CORRADE_VERIFY(!info->ppEnabledExtensionNames);
    CORRADE_COMPARE(info->enabledExtensionCount, 0);
    CORRADE_VERIFY(!info->pEnabledFeatures);
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

void DeviceVkTest::createInfoExtensionsCopiedStrings() {
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

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

void DeviceVkTest::createInfoFeatures() {
    DeviceProperties properties = pickDevice(instance());

    /* We don't use the structure for anything, so we don't need to check if
       the structure is actually supported */
    DeviceCreateInfo info{properties};
    info.setEnabledFeatures(DeviceFeature::RobustBufferAccess|DeviceFeature::SamplerYcbcrConversion);

    /* If we have Vulkan 1.1 on both instance and the device or KHR_gpdp2 is
       enabled on the instance, pNext chain will be filled as appropriate */
    if((instance().isVersionSupported(Version::Vk11) && properties.isVersionSupported(Version::Vk11)) || instance().isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>()) {
        CORRADE_VERIFY(!info->pEnabledFeatures);
        CORRADE_VERIFY(info->pNext);
        const auto& features2 = *static_cast<const VkPhysicalDeviceFeatures2*>(info->pNext);
        CORRADE_COMPARE(features2.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
        CORRADE_VERIFY(features2.features.robustBufferAccess);

        CORRADE_VERIFY(features2.pNext);
        const auto& samplerYcbcrConversionFeatures = *static_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeatures*>(features2.pNext);
        CORRADE_COMPARE(samplerYcbcrConversionFeatures.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES);
        CORRADE_VERIFY(samplerYcbcrConversionFeatures.samplerYcbcrConversion);

    /* Otherwise just the pEnabledFeatures will be enabled */
    } else {
        CORRADE_VERIFY(!info->pNext);
        CORRADE_VERIFY(info->pEnabledFeatures);
        CORRADE_VERIFY(info->pEnabledFeatures->robustBufferAccess);
    }
}

void DeviceVkTest::createInfoFeaturesNothingInCoreFeatures() {
    DeviceProperties properties = pickDevice(instance());

    DeviceCreateInfo info{properties};
    info.setEnabledFeatures(DeviceFeature::SamplerYcbcrConversion|DeviceFeature::ImagelessFramebuffer);

    /* If we have Vulkan 1.1 on both instance and the device or KHR_gpdp2 is
       enabled on the instance, pNext chain will be filled as appropriate */
    if((instance().isVersionSupported(Version::Vk11) && properties.isVersionSupported(Version::Vk11)) || instance().isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>()) {
        CORRADE_VERIFY(!info->pEnabledFeatures);
        CORRADE_VERIFY(info->pNext);
        const auto& imagelessFramebufferFeatures = *static_cast<const VkPhysicalDeviceImagelessFramebufferFeatures*>(info->pNext);
        CORRADE_COMPARE(imagelessFramebufferFeatures.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES);
        CORRADE_VERIFY(imagelessFramebufferFeatures.imagelessFramebuffer);

        const auto& samplerYcbcrConversionFeatures = *static_cast<const VkPhysicalDeviceSamplerYcbcrConversionFeatures*>(imagelessFramebufferFeatures.pNext);
        CORRADE_COMPARE(samplerYcbcrConversionFeatures.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES);
        CORRADE_VERIFY(samplerYcbcrConversionFeatures.samplerYcbcrConversion);

    /* Otherwise nothing is enabled as there's nowhere to connect that */
    } else {
        CORRADE_VERIFY(!info->pNext);
        CORRADE_VERIFY(!info->pEnabledFeatures);
    }
}

void DeviceVkTest::createInfoFeaturesReplaceExternal() {
    DeviceProperties properties = pickDevice(instance());

    VkPhysicalDeviceFeatures2 features{};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    VkAttachmentReference2 somethingAfter{};
    somethingAfter.pNext = &features;

    DeviceCreateInfo info{properties};
    info->pNext = &somethingAfter;
    info->pEnabledFeatures = &features.features;

    info.setEnabledFeatures(DeviceFeature::RobustBufferAccess);

    /* Then, if we have Vulkan 1.1 on both instance and the device or KHR_gpdp2
       is enabled on the instance, pNext will be filled and pEnabledFeatures
       reset */
    if((instance().isVersionSupported(Version::Vk11) && properties.isVersionSupported(Version::Vk11)) || instance().isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>()) {
        CORRADE_VERIFY(!info->pEnabledFeatures);
        CORRADE_VERIFY(info->pNext);
        const auto& features2 = *static_cast<const VkPhysicalDeviceFeatures2*>(info->pNext);
        CORRADE_COMPARE(features2.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
        CORRADE_VERIFY(features2.features.robustBufferAccess);

        /* The original chain should be preserved after */
        CORRADE_COMPARE(features2.pNext, &somethingAfter);

    /* Otherwise the pNext chain will be preserved and pEnabledFeatures
       replaced with own instance */
    } else {
        CORRADE_VERIFY(info->pEnabledFeatures);
        CORRADE_VERIFY(info->pEnabledFeatures != &features.features);
        CORRADE_VERIFY(info->pEnabledFeatures->robustBufferAccess);
    }

    /* No changes to the original chain, even though it has a features on its
       own (that's user error)  */
    CORRADE_COMPARE(somethingAfter.pNext, &features);
}

void DeviceVkTest::createInfoFeaturesReplacePrevious() {
    DeviceProperties properties = pickDevice(instance());

    VkAttachmentReference2 somethingAfter{};

    DeviceCreateInfo info{properties};
    info->pNext = &somethingAfter;

    info.setEnabledFeatures(DeviceFeature::RobustBufferAccess);

    /* If we have Vulkan 1.1 on both instance and the device or KHR_gpdp2 is
       enabled on the instance, pNext chain will be filled as appropriate */
    if((instance().isVersionSupported(Version::Vk11) && properties.isVersionSupported(Version::Vk11)) || instance().isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>()) {
        CORRADE_VERIFY(!info->pEnabledFeatures);
        CORRADE_VERIFY(info->pNext);
        const auto& features2 = *static_cast<const VkPhysicalDeviceFeatures2*>(info->pNext);
        CORRADE_COMPARE(features2.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
        CORRADE_VERIFY(features2.features.robustBufferAccess);

        /* The original chain should be preserved after */
        CORRADE_COMPARE(features2.pNext, &somethingAfter);

    /* Otherwise the pNext chain will be preserved and pEnabledFeatures
       replaced with own instance */
    } else {
        CORRADE_COMPARE(info->pNext, &somethingAfter);
        CORRADE_VERIFY(info->pEnabledFeatures);
        CORRADE_VERIFY(info->pEnabledFeatures->robustBufferAccess);
    }

    /* Setting a different non-core feature */
    info.setEnabledFeatures(DeviceFeature::ImagelessFramebuffer);
    if((instance().isVersionSupported(Version::Vk11) && properties.isVersionSupported(Version::Vk11)) || instance().isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>()) {
        CORRADE_VERIFY(!info->pEnabledFeatures);
        CORRADE_VERIFY(info->pNext);
        const auto& imagelessFramebufferFeatures = *static_cast<const VkPhysicalDeviceImagelessFramebufferFeatures*>(info->pNext);
        CORRADE_COMPARE(imagelessFramebufferFeatures.sType, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES);
        CORRADE_VERIFY(imagelessFramebufferFeatures.imagelessFramebuffer);

        /* The original chain should still be preserved after, without the
           structures from the previous case getting in the way */
        CORRADE_COMPARE(imagelessFramebufferFeatures.pNext, &somethingAfter);

    /* Otherwise the pNext chain will still be preserved and pEnabledFeatures
       empty */
    } else {
        CORRADE_COMPARE(info->pNext, &somethingAfter);
        CORRADE_VERIFY(!info->pEnabledFeatures);
    }

    /* Setting no features, everything should be fully discarded, and the
       original chain still kept. This doesn't have any difference between
       versions. */
    info.setEnabledFeatures({});
    CORRADE_COMPARE(info->pNext, &somethingAfter);
    CORRADE_VERIFY(!info->pEnabledFeatures);
    CORRADE_VERIFY(!somethingAfter.pNext);
}

void DeviceVkTest::createInfoFeaturesEnableAllResetAll() {
    DeviceProperties properties = pickDevice(instance());

    if((!instance().isVersionSupported(Version::Vk11) || !properties.isVersionSupported(Version::Vk11)) && !instance().isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>())
        CORRADE_SKIP("Neither Vulkan 1.1 nor KHR_get_physical_device_properties2 is supported, can't test");

    VkAttachmentDescription2 somethingAfter{};
    somethingAfter.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;

    DeviceCreateInfo info{properties};
    info->pNext = &somethingAfter;

    /* This should populate a huge chain of structures */
    info.setEnabledFeatures(~DeviceFeatures{});
    CORRADE_VERIFY(info->pNext != &somethingAfter);
    CORRADE_VERIFY(!somethingAfter.pNext);

    /* And this should disconnect them all again. If this fails, it means the
       resetting code path got out of sync with the structure list. Sorry,
       there's not really a better way how to show *where* it got wrong. */
    info.setEnabledFeatures({});
    CORRADE_COMPARE(info->pNext, &somethingAfter);
    CORRADE_VERIFY(!somethingAfter.pNext);
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

void DeviceVkTest::createInfoConstructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<DeviceCreateInfo>{});
    CORRADE_VERIFY(!std::is_copy_assignable<DeviceCreateInfo>{});
}

void DeviceVkTest::createInfoConstructMove() {
    if(std::getenv("MAGNUM_DISABLE_EXTENSIONS"))
        CORRADE_SKIP("Can't test with the MAGNUM_DISABLE_EXTENSIONS environment variable set");

    Queue queue{NoCreate};
    DeviceCreateInfo a{pickDevice(instance()), DeviceCreateInfo::Flag::NoImplicitExtensions};
    a.addQueues(0, {0.35f}, {queue})
     .addEnabledExtensions<Extensions::KHR::get_memory_requirements2,
                           Extensions::KHR::bind_memory2>();

    DeviceCreateInfo b{std::move(a)};
    CORRADE_COMPARE(a->enabledExtensionCount, 0);
    CORRADE_VERIFY(!a->ppEnabledExtensionNames);
    CORRADE_COMPARE(a->queueCreateInfoCount, 0);
    CORRADE_VERIFY(!a->pQueueCreateInfos);
    CORRADE_COMPARE(b->enabledExtensionCount, 2);
    CORRADE_VERIFY(b->ppEnabledExtensionNames);
    CORRADE_COMPARE(b->ppEnabledExtensionNames[1], "VK_KHR_bind_memory2"_s);
    CORRADE_COMPARE(b->queueCreateInfoCount, 1);
    CORRADE_VERIFY(b->pQueueCreateInfos);
    CORRADE_COMPARE(b->pQueueCreateInfos[0].pQueuePriorities[0], 0.35f);

    DeviceCreateInfo c{{}, {}};
    c = std::move(b);
    CORRADE_COMPARE(b->enabledExtensionCount, 0);
    CORRADE_VERIFY(!b->ppEnabledExtensionNames);
    CORRADE_COMPARE(b->queueCreateInfoCount, 0);
    CORRADE_VERIFY(!b->pQueueCreateInfos);
    CORRADE_COMPARE(c->enabledExtensionCount, 2);
    CORRADE_VERIFY(c->ppEnabledExtensionNames);
    CORRADE_COMPARE(c->ppEnabledExtensionNames[1], "VK_KHR_bind_memory2"_s);
    CORRADE_COMPARE(c->queueCreateInfoCount, 1);
    CORRADE_VERIFY(c->pQueueCreateInfos);
    CORRADE_COMPARE(c->pQueueCreateInfos[0].pQueuePriorities[0], 0.35f);
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

void DeviceVkTest::constructFeatures() {
    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{pickDevice(instance())}
        .addQueues(0, {0.0f}, {queue})
        /* RobustBufferAccess is guaranteed to be supported always, no need to
           check anything */
        .setEnabledFeatures(DeviceFeature::RobustBufferAccess)};
    CORRADE_VERIFY(device.handle());

    /* Features should be reported as enabled. Exclude portability subset
       features that get implicitly marked as enabled on devices w/o
       KHR_portability_subset. */
    CORRADE_COMPARE(device.enabledFeatures() & ~Implementation::deviceFeaturesPortabilitySubset(), DeviceFeature::RobustBufferAccess);
}

void DeviceVkTest::constructFeaturesFromExtensions() {
    DeviceProperties properties = pickDevice(instance());

    if(!properties.enumerateExtensionProperties().isSupported<Extensions::KHR::sampler_ycbcr_conversion>())
        CORRADE_SKIP("VK_KHR_sampler_ycbcr_conversion not supported, can't test");
    if(!(properties.features() & DeviceFeature::SamplerYcbcrConversion))
        CORRADE_SKIP("SamplerYcbcrConversion feature not supported, can't test");

    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{properties}
        .addQueues(0, {0.0f}, {queue})
        .addEnabledExtensions<Extensions::KHR::sampler_ycbcr_conversion>()
        /* RobustBufferAccess is guaranteed to be supported always, no need to
           check anything */
        .setEnabledFeatures(DeviceFeature::RobustBufferAccess|DeviceFeature::SamplerYcbcrConversion)};
    CORRADE_VERIFY(device.handle());

    /* Features should be reported as enabled. Exclude portability subset
       features that get implicitly marked as enabled on devices w/o
       KHR_portability_subset. */
    CORRADE_COMPARE(device.enabledFeatures() & ~Implementation::deviceFeaturesPortabilitySubset(), DeviceFeature::RobustBufferAccess|DeviceFeature::SamplerYcbcrConversion);
}

void DeviceVkTest::constructDeviceCreateInfoConstReference() {
    Queue queue{NoCreate};
    DeviceProperties deviceProperties = pickDevice(instance());
    DeviceCreateInfo info{deviceProperties};
    info.addQueues(0, {0.0f}, {queue});

    /* Just to verify the overload taking const DeviceProperties& works as
       well (most of the above tests verified a move) */
    Device device{instance(), info};
    CORRADE_VERIFY(device.handle());

    /* Device properties should be lazy-populated and different from the
       above instances because we didn't transfer the ownership here either */
    CORRADE_COMPARE(device.properties().name(), deviceProperties.name());
    CORRADE_VERIFY(&device.properties().properties() != &deviceProperties.properties());
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
    std::string expected = Utility::formatString(data.log, deviceProperties.name(), major, minor, patch ? Utility::formatString(".{}", patch) : "");
    /* The output might contain a device workaround list, cut that away.
       That's tested thoroughly in constructWorkaroundsCommandLineDisable(). */
    CORRADE_COMPARE(out.str().substr(0, expected.size()), expected);

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
    std::string expected = Utility::formatString(data.log, deviceProperties.name(), major, minor, patch ? Utility::formatString(".{}", patch) : "");
    /* The output might contain a device workaround list, cut that away.
       That's tested thoroughly in constructWorkaroundsCommandLineDisable(). */
    CORRADE_COMPARE(out.str().substr(0, expected.size()), expected);

    /* Verify that the entrypoint is actually (not) loaded as expected, to
       avoid all the above reporting being just smoke & mirrors */
    CORRADE_COMPARE(!!device->CmdDebugMarkerInsertEXT, data.debugMarkerEnabled);
    CORRADE_COMPARE(!!device->TrimCommandPoolKHR, data.maintenance1Enabled);
}

void DeviceVkTest::constructWorkaroundsCommandLineDisable() {
    auto&& data = ConstructWorkaroundsCommandLineData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(std::getenv("MAGNUM_VULKAN_VERSION"))
        CORRADE_SKIP("Can't test with the MAGNUM_VULKAN_VERSION environment variable set");

    /* Creating a dedicated instance so we can pass custom args */
    Instance instance2{InstanceCreateInfo{Int(data.args.size()), data.args}};

    DeviceProperties deviceProperties = pickDevice(instance2);

    if(!deviceProperties.name().hasPrefix("SwiftShader"_s) && !data.shouldPassAlways)
        CORRADE_SKIP("Workarounds only available on SwiftShader, can't test.");

    std::ostringstream out;
    Debug redirectOutput{&out};
    Queue queue{NoCreate};
    Device device{instance2, DeviceCreateInfo{deviceProperties, DeviceCreateInfo::Flag::NoImplicitExtensions}
        .addQueues(0, {0.0f}, {queue})
    };

    CORRADE_VERIFY(device.handle());

    /** @todo cleanup when Debug::toString() or some similar utility exists */
    UnsignedInt major = versionMajor(deviceProperties.version());
    UnsignedInt minor = versionMinor(deviceProperties.version());
    UnsignedInt patch = versionPatch(deviceProperties.version());
    /* SwiftShader reports just 1.1 with no patch version, special-case that */
    CORRADE_COMPARE(out.str(), Utility::formatString(data.log, deviceProperties.name(), major, minor, patch ? Utility::formatString(".{}", patch) : ""));
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

void DeviceVkTest::constructFeatureNotSupported() {
    DeviceProperties properties = pickDevice(instance());
    if(properties.features() & DeviceFeature::SparseBinding)
        CORRADE_SKIP("The SparseBinding feature is supported, can't test");
    if(properties.features() & DeviceFeature::SparseResidency16Samples)
        CORRADE_SKIP("The SparseResidency16Samples feature is supported, can't test");

    std::ostringstream out;
    Error redirectError{&out};
    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{properties}
        .addQueues(0, {0.0f}, {queue})
        .setEnabledFeatures(DeviceFeature::SparseBinding|DeviceFeature::SparseResidency16Samples)};
    CORRADE_COMPARE(out.str(), "Vk::Device::tryCreate(): some enabled features are not supported: Vk::DeviceFeature::SparseBinding|Vk::DeviceFeature::SparseResidency16Samples\n");
}

void DeviceVkTest::constructFeatureWithoutExtension() {
    DeviceProperties properties = pickDevice(instance());
    if((!instance().isVersionSupported(Version::Vk11) || !properties.isVersionSupported(Version::Vk11)) && !instance().isExtensionEnabled<Extensions::KHR::get_physical_device_properties2>())
        CORRADE_SKIP("Neither Vulkan 1.1 nor KHR_get_physical_device_properties2 is supported, can't test");
    if(!(properties.features() & DeviceFeature::SamplerYcbcrConversion))
        CORRADE_SKIP("The SamplerYcbcrConversion feature is not supported, can't test");

    Queue queue{NoCreate};
    DeviceCreateInfo info{properties};
    info.addQueues(0, {0.0f}, {queue})
        .setEnabledFeatures(DeviceFeature::SamplerYcbcrConversion);

    std::ostringstream out;
    Error redirectError{&out};
    Device device{instance(), info};
    CORRADE_COMPARE(out.str(), "Vk::Device::tryCreate(): some enabled features need VK_KHR_sampler_ycbcr_conversion enabled\n");
}

void DeviceVkTest::constructNoQueue() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    std::ostringstream out;
    Error redirectError{&out};
    Device device{instance(), DeviceCreateInfo{pickDevice(instance())}};
    CORRADE_COMPARE(out.str(), "Vk::Device::tryCreate(): needs at least one queue\n");
}

void DeviceVkTest::constructNoPortability() {
    DeviceProperties properties = pickDevice(instance());

    if(properties.enumerateExtensionProperties().isSupported<Extensions::KHR::portability_subset>())
        CORRADE_SKIP("KHR_portability_subset supported, can't test");

    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{properties}
        .addQueues(0, {0.0f}, {queue})
    };

    /* The extension shouldn't be registered as enabled */
    CORRADE_VERIFY(!device.isExtensionEnabled<Extensions::KHR::portability_subset>());

    /* All features should be marked as enabled */
    CORRADE_COMPARE_AS(device.enabledFeatures(), Implementation::deviceFeaturesPortabilitySubset(),
        TestSuite::Compare::GreaterOrEqual);
}

void DeviceVkTest::constructNoPortabilityEnablePortabilityFeatures() {
    DeviceProperties properties = pickDevice(instance());

    if(properties.enumerateExtensionProperties().isSupported<Extensions::KHR::portability_subset>())
        CORRADE_SKIP("KHR_portability_subset supported, can't test");

    Device device{NoCreate};

    /* Explicitly enabling portability subset features shouldn't do anything
       when the portability extension isn't present */
    Queue queue{NoCreate};
    CORRADE_COMPARE(device.tryCreate(instance(), DeviceCreateInfo{properties}
        .addQueues(0, {0.0f}, {queue})
        .setEnabledFeatures(Implementation::deviceFeaturesPortabilitySubset())
    ), Result::Success);

    /* All features should be marked as enabled */
    CORRADE_COMPARE_AS(device.enabledFeatures(), Implementation::deviceFeaturesPortabilitySubset(),
        TestSuite::Compare::GreaterOrEqual);
}

void DeviceVkTest::constructPortability() {
    DeviceProperties properties = pickDevice(instance());

    if(!properties.enumerateExtensionProperties().isSupported<Extensions::KHR::portability_subset>())
        CORRADE_SKIP("KHR_portability_subset not supported, can't test");

    /* (Same as in DevicePropertiesVkTest.) Not all features should be marked
       as supported... */
    CORRADE_VERIFY((properties.features() & Implementation::deviceFeaturesPortabilitySubset()) != Implementation::deviceFeaturesPortabilitySubset());

    /* ... but there should be at least one feature */
    CORRADE_VERIFY(properties.features() &
Implementation::deviceFeaturesPortabilitySubset());

    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{properties}
        .addQueues(0, {0.0f}, {queue})
        .setEnabledFeatures(properties.features() &
Implementation::deviceFeaturesPortabilitySubset())
    };

    /* All requested features should be marked as enabled */
    CORRADE_COMPARE_AS(device.enabledFeatures(), properties.features() &
Implementation::deviceFeaturesPortabilitySubset(),
        TestSuite::Compare::GreaterOrEqual);
}

void DeviceVkTest::tryCreateAlreadyCreated() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{pickDevice(instance())}
        .addQueues(0, {0.0f}, {queue})
    };
    CORRADE_VERIFY(device.handle());

    std::ostringstream out;
    Error redirectError{&out};
    device.tryCreate(instance(), DeviceCreateInfo{pickDevice(instance())});
    CORRADE_COMPARE(out.str(), "Vk::Device::tryCreate(): device already created\n");
}

void DeviceVkTest::tryCreateUnknownExtension() {
    Queue queue{NoCreate};
    Device device{NoCreate};

    std::ostringstream out;
    Error redirectError{&out};
    CORRADE_COMPARE(device.tryCreate(instance(), DeviceCreateInfo{pickDevice(instance())}
        .addQueues(0, {0.0f}, {queue})
        .addEnabledExtensions({"VK_this_doesnt_exist"_s})), Result::ErrorExtensionNotPresent);
    CORRADE_COMPARE(out.str(), "Vk::Device::tryCreate(): device creation failed: Vk::Result::ErrorExtensionNotPresent\n");
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
            >()
            /* RobustBufferAccess is guaranteed to be supported always, no need
               to check anything */
            .setEnabledFeatures(DeviceFeature::RobustBufferAccess),
        nullptr, &device)), Result::Success);
    CORRADE_VERIFY(device);
    /* Populating the queue handle is done only from Device itself, so it won't
       happen here -- would need to call vkGetDeviceQueue[2] directly */
    CORRADE_VERIFY(!queue.handle());

    {
        /* Wrapping should load the basic function pointers */
        Device wrapped{NoCreate};
        wrapped.wrap(instance2, deviceProperties, device, Version::Vk11, {
            Extensions::EXT::debug_marker::string()
        }, DeviceFeature::RobustBufferAccess, HandleFlag::DestroyOnDestruction);
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

        /* Listed features should be reported as enabled */
        CORRADE_COMPARE(wrapped.enabledFeatures(), DeviceFeature::RobustBufferAccess);

        /* Device properties should be available */
        CORRADE_COMPARE(wrapped.properties().handle(), deviceProperties.handle());

        /* Releasing won't destroy anything ... */
        CORRADE_COMPARE(wrapped.release(), device);
    }

    /* ...so we can wrap it again, non-owned, and then destroy it manually */
    Device wrapped{NoCreate};
    wrapped.wrap(instance2, deviceProperties, device, Version::Vk10, {}, {});
    CORRADE_VERIFY(wrapped->DestroyDevice);
    wrapped->DestroyDevice(device, nullptr);
}

void DeviceVkTest::wrapAlreadyCreated() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    Queue queue{NoCreate};
    Device device{instance(), DeviceCreateInfo{pickDevice(instance())}
        .addQueues(0, {0.0f}, {queue})
    };
    CORRADE_VERIFY(device.handle());

    std::ostringstream out;
    Error redirectError{&out};
    device.wrap(instance(), {}, {}, {}, {}, {});
    CORRADE_COMPARE(out.str(), "Vk::Device::wrap(): device already created\n");
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
