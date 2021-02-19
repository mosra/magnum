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

#include "Device.h"
#include "DeviceCreateInfo.h"

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
#include "Magnum/Vk/DeviceFeatures.h"
#include "Magnum/Vk/DeviceProperties.h"
#include "Magnum/Vk/Extensions.h"
#include "Magnum/Vk/ExtensionProperties.h"
#include "Magnum/Vk/Queue.h"
#include "Magnum/Vk/Result.h"
#include "Magnum/Vk/Version.h"
#include "Magnum/Vk/Implementation/Arguments.h"
#include "Magnum/Vk/Implementation/InstanceState.h"
#include "Magnum/Vk/Implementation/DeviceFeatures.h"
#include "Magnum/Vk/Implementation/DeviceState.h"
#include "Magnum/Vk/Implementation/DriverWorkaround.h"
#include "Magnum/Vk/Implementation/structureHelpers.h"
#include "MagnumExternal/Vulkan/flextVkGlobal.h"

namespace Magnum { namespace Vk {

/* In any other CreateInfo, we could simply populate a pNext chain of a
   supported / enabled subset of all structures that might ever get used and
   then only populate their contents without having to fumble with the linked
   list connections. That's unfortunately not possible with DeviceCreateInfo,
   because *don't know yet* what extensions will get enabled. So for everything
   that might live on the pNext chain (currently just features, but over time
   it'll be also multi-device setup, swapchain, ...) we need to:

   -    ensure we're not stomping on something in pNext that's defined
        externally (when constructing DeviceCreateInfo from a raw
        VkDeviceCreateInfo or when the user directly adds something to pNext),
        thus only connecting things to _info.pNext, not anywhere else as that
        might be const memory
    -   ensure the externally supplied pNext pointers are not lost when we
        connect our own things
    -   ensure when e.g. setEnabledFeatures() gets called twice, we don't
        connect the same structure chain again, ending up with a loop. Which
        means going through the existing chain and breaking up links that point
        to the structures we're going to reconnect, this isn't really fast but
        also it's not really common to call the same API more than once. This
        also assumes that our structures are pointed to only by our structures
        again and not something external (that might be const memory again).

*/
struct DeviceCreateInfo::State {
    Containers::Array<Containers::String> ownedStrings;
    Containers::Array<const char*> extensions;

    VkPhysicalDeviceFeatures2 features2{};
    Implementation::DeviceFeatures features{};
    DeviceFeatures enabledFeatures;
    /* Some features are treated as implicitly enabled. Currently this includes
       KHR_portability_subset features on devices that *don't* advertise the
       extension, in the future it might be for example features unique to
       Vulkan1[12]Features (which isn't present in the pNext chain), for which
       the corresponding extension got enabled and thus implicitly enabled
       those. For all those is common that those don't get explicitly marked as
       enabled on device creation and are also not listed among enabled
       features in the startup log. */
    DeviceFeatures implicitFeatures;
    void* firstEnabledFeature{};
    /* Used for checking if the device enables extensions required by features */
    #ifndef CORRADE_NO_ASSERT
    Math::BoolVector<Implementation::ExtensionCount> featuresRequiredExtensions;
    #endif

    Containers::String disabledExtensionsStorage;
    Containers::Array<Containers::StringView> disabledExtensions;
    /* .second = true means the workaround is disabled; the views always point
       to the internal KnownWorkarounds array */
    Containers::Array<std::pair<Containers::StringView, bool>> encounteredWorkarounds;
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
       version via --magnum-vulkan-version, which will be later used to cap available features. */
    _state->version = Version(Math::min(UnsignedInt(deviceProperties._instance->version()), UnsignedInt(deviceProperties.version())));

    /* If there are any disabled workarounds, save them until initialize() uses
       them on device creation. The disableWorkaround() function saves the
       internal string view instead of the one passed from the command line so
       we don't need to bother with String allocations. */
    Containers::StringView disabledWorkarounds = args.value<Containers::StringView>("disable-workarounds");
    if(!disabledWorkarounds.isEmpty()) {
        Containers::Array<Containers::StringView> split = disabledWorkarounds.splitWithoutEmptyParts();
        arrayReserve(_state->encounteredWorkarounds, split.size());
        for(Containers::StringView workaround: split)
            Implementation::disableWorkaround(_state->encounteredWorkarounds, workaround);
    }

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

        /* Only if we don't have Vulkan 1.1, on which these are core */
        if(_state->version < Version::Vk11) {
            if(extensionProperties->isSupported<Extensions::KHR::get_memory_requirements2>())
                addEnabledExtensions<Extensions::KHR::get_memory_requirements2>();
            if(extensionProperties->isSupported<Extensions::KHR::bind_memory2>())
                addEnabledExtensions<Extensions::KHR::bind_memory2>();
        }
        /* Only if we don't have Vulkan 1.2, on which these are core */
        if(_state->version < Version::Vk12) {
            if(extensionProperties->isSupported<Extensions::KHR::create_renderpass2>())
                addEnabledExtensions<Extensions::KHR::create_renderpass2>();
        }

        /* Enable the KHR_copy_commands2 and EXT_extended_dynamic_state
           extensions. Not in any Vulkan version yet. */
        if(extensionProperties->isSupported<Extensions::KHR::copy_commands2>())
            addEnabledExtensions<Extensions::KHR::copy_commands2>();
        if(extensionProperties->isSupported<Extensions::EXT::extended_dynamic_state>())
            addEnabledExtensions<Extensions::EXT::extended_dynamic_state>();

        /* Enable the KHR_portability_subset extension, which *has to be*
           enabled when available. Not enabling any of its features though,
           that responsibility lies on the user. */
        if(extensionProperties->isSupported<Extensions::KHR::portability_subset>()) {
            addEnabledExtensions<Extensions::KHR::portability_subset>();

        /* Otherwise, if KHR_portability_subset is not supported, mark its
           features as *implicitly* supported -- those don't get explicitly
           enabled and are also not listed in the list of enabled features in
           the startup log */
        /** @todo wrap this under a NoImplicitFeatures flag? it doesn't actually
            *do* anything though */
        } else {
            _state->implicitFeatures = Implementation::deviceFeaturesPortabilitySubset();
        }
    }

    /* Conservatively populate the device properties.
       - In case the DeviceCreateInfo(DeviceProperties&&) constructor is used,
         it'll get overwritten straight away with a populated instance.
       - In case neither the addQueues(QueueFlags) API (which queries the
         properties for queue family index) nor the setEnabledFeatures() API
         (which needs to check where to connect based on version and KHR_gpdp2
         presence) is not used and DeviceCreateInfo isn't subsequently moved to
         the Device, it'll never get touched again and Device will wrap() its
         own.
       - In case addQueues(QueueFlags) / setEnabledFeatures() is used it'll get
         populated and then possibly discarded if it isn't subsequently moved
         to the Device. */
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

DeviceCreateInfo::DeviceCreateInfo(DeviceCreateInfo&& other) noexcept:
    _physicalDevice{other._physicalDevice},
    /* Can't use {} with GCC 4.8 here because it tries to initialize the first
       member instead of doing a copy */
    _info(other._info),
    _state{std::move(other._state)}
{
    /* Ensure the previous instance doesn't reference state that's now ours */
    /** @todo this is now more like a destructible move, do it more selectively
        and clear only what's really ours and not external? */
    other._info.pNext = nullptr;
    other._info.enabledExtensionCount = 0;
    other._info.ppEnabledExtensionNames = nullptr;
    other._info.queueCreateInfoCount = 0;
    other._info.pQueueCreateInfos = nullptr;
}

DeviceCreateInfo::~DeviceCreateInfo() = default;

DeviceCreateInfo& DeviceCreateInfo::operator=(DeviceCreateInfo&& other) noexcept {
    using std::swap;
    swap(other._physicalDevice, _physicalDevice);
    swap(other._info, _info);
    swap(other._state, other._state);
    return *this;
}

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
    return std::move(addEnabledExtensions(extensions));
}

DeviceCreateInfo& DeviceCreateInfo::addEnabledExtensions(const std::initializer_list<Containers::StringView> extensions) & {
    return addEnabledExtensions(Containers::arrayView(extensions));
}

DeviceCreateInfo&& DeviceCreateInfo::addEnabledExtensions(const std::initializer_list<Containers::StringView> extensions) && {
    return std::move(addEnabledExtensions(extensions));
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
    return std::move(addEnabledExtensions(extensions));
}

DeviceCreateInfo& DeviceCreateInfo::addEnabledExtensions(const std::initializer_list<Extension> extensions) & {
    return addEnabledExtensions(Containers::arrayView(extensions));
}

DeviceCreateInfo&& DeviceCreateInfo::addEnabledExtensions(const std::initializer_list<Extension> extensions) && {
    return std::move(addEnabledExtensions(extensions));
}

namespace {

template<class T> void structureConnectIfUsed(Containers::Reference<const void*>& next, void*& firstFeatureStructure, T& structure, VkStructureType type) {
    if(structure.sType) {
        if(!firstFeatureStructure) firstFeatureStructure = &structure;
        Implementation::structureConnect(next, structure, type);
    }
}

}

DeviceCreateInfo& DeviceCreateInfo::setEnabledFeatures(const DeviceFeatures& features_) & {
    /* Filter out implicit features as those are treated as being present even
       if not explicitly enabled (such as KHR_portability_subset on devices
       that *don't* advertise the extension */
    const DeviceFeatures features = features_ & ~_state->implicitFeatures;

    /* Remember the features to pass them to Device later. This gets combined
       with implicitFeatures again for Device::enabledFeatures(). */
    _state->enabledFeatures = features;

    /* Clear any existing pointers to the feature structure chain. This needs
       to be done in order to avoid pointing to them again from a different
       place, creating a loop. Additionally, the pNext chain may contain
       additional structures after the features and we don't want to lose those
       -- so it's not possible to simply disconnect and clear them, but we need
       to first find and preserve what is connected after.

       To avoid quadratic complexity by going through each of the feature
       structs and attempting to find it in the pNext chain,
       _state->firstEnabledFeature remembers the first structure in the chain
       that was enabled previously. We find what structure points to it and
       then the structureDisconnectChain() goes through the chain and repoints
       the structure to the first structure that's not from the list, thus
       preserving the remaining part of the chain. */
    _info.pEnabledFeatures = nullptr;
    if(_state->firstEnabledFeature) {
        const void** const pointerToFirst = Implementation::structureFind(_info.pNext, *static_cast<const VkBaseInStructure*>(_state->firstEnabledFeature));
        if(pointerToFirst) Implementation::structureDisconnectChain(*pointerToFirst, {
            /* This list needs to be kept in sync with
               Implementation::DeviceFeatures, keeping the same order (however
               there's a test that should catch *all* errors with forgotten or
               wrongly ordered structures) */
            _state->features2,
            _state->features.protectedMemory,
            _state->features.multiview,
            _state->features.shaderDrawParameters,
            _state->features.textureCompressionAstcHdr,
            _state->features.shaderFloat16Int8,
            _state->features._16BitStorage,
            _state->features.imagelessFramebuffer,
            _state->features.variablePointers,
            _state->features.accelerationStructure,
            _state->features.samplerYcbcrConversion,
            _state->features.descriptorIndexing,
            _state->features.portabilitySubset,
            _state->features.shaderSubgroupExtendedTypes,
            _state->features._8BitStorage,
            _state->features.shaderAtomicInt64,
            _state->features.vertexAttributeDivisor,
            _state->features.timelineSemaphore,
            _state->features.vulkanMemoryModel,
            _state->features.scalarBlockLayout,
            _state->features.separateDepthStencilLayouts,
            _state->features.uniformBufferStandardLayout,
            _state->features.bufferDeviceAddress,
            _state->features.hostQueryReset,
            _state->features.indexTypeUint8,
            _state->features.extendedDynamicState,
            _state->features.robustness2,
            _state->features.imageRobustness,
            _state->features.rayTracingPipeline,
            _state->features.rayQuery
        });

        _state->firstEnabledFeature = {};
    }

    /* Now that the feature chain is disconnected from the pNext chain, we can
       safely clear it */
    _state->features2 = {};
    _state->features = {};

    /* If there's no features to enable, exit */
    if(!features) return *this;

    /* Otherwise, first set enabled bits in each structure and remember which
       structures have bits set */
    #define _c(value, field)                                                \
        if(features & DeviceFeature::value) {                               \
            _state->features2.sType = VkStructureType(1);                   \
            _state->features2.features.field = VK_TRUE;                     \
        }
    #ifdef CORRADE_NO_ASSERT
    #define _cver(value, field, suffix, version)                            \
        if(features & DeviceFeature::value) {                               \
            _state->features.suffix.sType = VkStructureType(1);             \
            _state->features.suffix.field = VK_TRUE;                        \
        }
    #define _cext _cver
    #else
    /* Not checking anything for the version, since if a device doesn't support
       given version, it simply won't report the feature as supported */
    #define _cver(value, field, suffix, version)                            \
        if(features & DeviceFeature::value) {                               \
            _state->features.suffix.sType = VkStructureType(1);             \
            _state->features.suffix.field = VK_TRUE;                        \
        }
    #define _cext(value, field, suffix, extension)                          \
        if(features & DeviceFeature::value) {                               \
            _state->features.suffix.sType = VkStructureType(1);             \
            _state->features.suffix.field = VK_TRUE;                        \
            _state->featuresRequiredExtensions.set(Extensions::extension::Index, true); \
        }
    #endif
    #include "Magnum/Vk/Implementation/deviceFeatureMapping.hpp"
    #undef _c
    #undef _cver
    #undef _cext

    /* First handle compatibility with unextended Vulkan 1.0 -- there we can
       only add VkPhysicalDeviceFeatures to pEnabledFeatures and have to ignore
       the rest. */
    if(!_state->properties.canUseFeatures2ForDeviceCreation()) {
        /* Only point to the structure if something was actually enabled there.
           If not, there's no point in referencing it. */
        if(_state->features2.sType)
            _info.pEnabledFeatures = &_state->features2.features;
        return *this;
    }

    /* Otherwise we can start from _info.pNext */
    Containers::Reference<const void*> next = _info.pNext;

    /* Connect together all structures that have something enabled. That
       includes the VkPhysicalDeviceFeatures2 -- if it doesn't have anything
       enabled, it's not included in the chain at all. The
       _state->firstEnabledFeature pointer points to the first enabled feature
       which will be useful to clean up the previous state if
       setEnabledFeatures() gets called again. */
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features2, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.protectedMemory, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.multiview, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.shaderDrawParameters, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.textureCompressionAstcHdr, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TEXTURE_COMPRESSION_ASTC_HDR_FEATURES_EXT);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.shaderFloat16Int8, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features._16BitStorage, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.imagelessFramebuffer, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.variablePointers, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTERS_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.accelerationStructure, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.samplerYcbcrConversion, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.descriptorIndexing, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.portabilitySubset, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.shaderSubgroupExtendedTypes, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_SUBGROUP_EXTENDED_TYPES_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features._8BitStorage, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.shaderAtomicInt64, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.vertexAttributeDivisor, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.timelineSemaphore, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.vulkanMemoryModel, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.scalarBlockLayout, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SCALAR_BLOCK_LAYOUT_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.separateDepthStencilLayouts, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.uniformBufferStandardLayout, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFORM_BUFFER_STANDARD_LAYOUT_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.bufferDeviceAddress, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.hostQueryReset, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.indexTypeUint8, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_INDEX_TYPE_UINT8_FEATURES_EXT);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.extendedDynamicState, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.robustness2, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.imageRobustness, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_ROBUSTNESS_FEATURES_EXT);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.rayTracingPipeline, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR);
    structureConnectIfUsed(next, _state->firstEnabledFeature,
        _state->features.rayQuery, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR);

    return *this;
}

DeviceCreateInfo&& DeviceCreateInfo::setEnabledFeatures(const DeviceFeatures& features) && {
    return std::move(setEnabledFeatures(features));
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
    return std::move(addQueues(family, priorities, output));
}

DeviceCreateInfo& DeviceCreateInfo::addQueues(const UnsignedInt family, const std::initializer_list<Float> priorities, const std::initializer_list<Containers::Reference<Queue>> output) & {
    return addQueues(family, Containers::arrayView(priorities), Containers::arrayView(output));
}

DeviceCreateInfo&& DeviceCreateInfo::addQueues(const UnsignedInt family, const std::initializer_list<Float> priorities, const std::initializer_list<Containers::Reference<Queue>> output) && {
    return std::move(addQueues(family, priorities, output));
}

DeviceCreateInfo& DeviceCreateInfo::addQueues(const QueueFlags flags, const Containers::ArrayView<const Float> priorities, const Containers::ArrayView<const Containers::Reference<Queue>> output) & {
    return addQueues(_state->properties.pickQueueFamily(flags), priorities, output);
}

DeviceCreateInfo&& DeviceCreateInfo::addQueues(const QueueFlags flags, const Containers::ArrayView<const Float> priorities, const Containers::ArrayView<const Containers::Reference<Queue>> output) && {
    return std::move(addQueues(flags, priorities, output));
}

DeviceCreateInfo& DeviceCreateInfo::addQueues(const QueueFlags flags, const std::initializer_list<Float> priorities, const std::initializer_list<Containers::Reference<Queue>> output) & {
    return addQueues(flags, Containers::arrayView(priorities), Containers::arrayView(output));
}

DeviceCreateInfo&& DeviceCreateInfo::addQueues(const QueueFlags flags, const std::initializer_list<Float> priorities, const std::initializer_list<Containers::Reference<Queue>> output) && {
    return std::move(addQueues(flags, priorities, output));
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
    return std::move(addQueues(info));
}

namespace {

constexpr Version KnownVersionsForExtensions[]{
    Version::None,
  /*Version::Vk10, has no extensions */
    Version::Vk11,
    Version::Vk12
};

}

void Device::wrap(Instance& instance, const VkPhysicalDevice physicalDevice, const VkDevice handle, const Version version, const Containers::ArrayView<const Containers::StringView> enabledExtensions, const DeviceFeatures& enabledFeatures, const HandleFlags flags) {
    CORRADE_ASSERT(!_handle,
        "Vk::Device::wrap(): device already created", );

    /* Compared to the constructor nothing is printed here as it would be just
       repeating what was passed via the arguments */
    _handle = handle;
    _flags = flags;
    _properties.emplace(DeviceProperties::wrap(instance, physicalDevice));
    initializeExtensions(enabledExtensions);

    /* Because we have no control over extensions / features, no workarounds
       are used here -- better to just do nothing than just a partial attempt */
    Containers::Array<std::pair<Containers::StringView, bool>> encounteredWorkarounds = Implementation::disableAllWorkarounds();
    initialize(instance, version, encounteredWorkarounds, enabledFeatures);
}

void Device::wrap(Instance& instance, const VkPhysicalDevice physicalDevice, const VkDevice handle, const Version version, const std::initializer_list<Containers::StringView> enabledExtensions, const DeviceFeatures& enabledFeatures, const HandleFlags flags) {
    wrap(instance, physicalDevice, handle, version, Containers::arrayView(enabledExtensions), enabledFeatures, flags);
}

Device::Device(Instance& instance, const DeviceCreateInfo& info): Device{NoCreate} {
    create(instance, info);
}

Device::Device(Instance& instance, DeviceCreateInfo&& info): Device{NoCreate} {
    create(instance, std::move(info));
}

Device::Device(NoCreateT): _handle{}, _functionPointers{} {}

Device::~Device() {
    if(_handle && (_flags & HandleFlag::DestroyOnDestruction))
        _functionPointers.DestroyDevice(_handle, nullptr);
}

void Device::create(Instance& instance, const DeviceCreateInfo& info) {
    if(tryCreate(instance, info) != Result::Success) std::exit(1);
}

void Device::create(Instance& instance, DeviceCreateInfo&& info) {
    if(tryCreate(instance, std::move(info)) != Result::Success) std::exit(1);
}

Result Device::tryCreate(Instance& instance, const DeviceCreateInfo& info) {
    return tryCreateInternal(instance, info, DeviceProperties::wrap(instance, info._physicalDevice));
}

Result Device::tryCreate(Instance& instance, DeviceCreateInfo&& info) {
    return tryCreateInternal(instance, info, std::move(info._state->properties));
}

Result Device::tryCreateInternal(Instance& instance, const DeviceCreateInfo& info, DeviceProperties&& properties) {
    CORRADE_ASSERT(!_handle,
        "Vk::Device::tryCreate(): device already created", {});
    CORRADE_ASSERT(info._info.queueCreateInfoCount,
        "Vk::Device::tryCreate(): needs at least one queue", {});

    _flags = HandleFlag::DestroyOnDestruction;
    _properties.emplace(std::move(properties));

    /* The properties should always be a valid instance, either moved from
       outside or created again from VkPhysicalDevice, in case it couldn't be
       moved. If it's not, something in DeviceCreateInfo or here got messed
       up. */
    CORRADE_INTERNAL_ASSERT(_properties->handle());

    /* Check that all enabled features were actually reported as supported.
       I happily assumed the drivers would do that, but as far as my testing
       goes it happens only for the VkPhysicalDeviceFeatures2, and not for
       anything added by extensions after that, which is quite disappointing
       -- I expected those to be checked as strictly as extensions, but not
       even the validation layers seem to check those.

       Making this silently pass isn't a good idea because it might (or might
       not) fail later in an unpredictable way. Fortunately it's rather easy
       to check thanks to how these are designed :D */
    CORRADE_ASSERT(info._state->enabledFeatures <= _properties->features(),
        "Vk::Device::tryCreate(): some enabled features are not supported:" <<
        (info._state->enabledFeatures & ~_properties->features()), {});

    const Version version = info._state->version != Version::None ?
        info._state->version : _properties->version();

    /* Print all enabled extensions and features if we're not told to be quiet.
       The implicit features (such as KHR_portability_subset features on
       devices that *don't* advertise the extension) are not listed here but
       are added to Device::enabledFeatures() below. */
    if(!info._state->quietLog) {
        Debug{} << "Device:" << _properties->name();
        Debug{} << "Device version:" << version;

        if(info->enabledExtensionCount) {
            Debug{} << "Enabled device extensions:";
            for(std::size_t i = 0, max = info->enabledExtensionCount; i != max; ++i)
                Debug{} << "   " << info->ppEnabledExtensionNames[i];
        }

        if(info._state->enabledFeatures) {
            Debug{} << "Enabled features:";
            for(std::size_t i = 0, max = DeviceFeatures::Size*64; i != max; ++i) {
                if(!(info._state->enabledFeatures & DeviceFeature(i))) continue;
                Debug{} << "   " << DeviceFeature(i);
            }
        }
    }

    if(const VkResult result = instance->CreateDevice(info._physicalDevice, info, nullptr, &_handle)) {
        Error{} << "Vk::Device::tryCreate(): device creation failed:" << Result(result);
        return Result(result);
    }

    /* Make a copy of the workarounds list coming from DeviceCreateInfo as
       initialize() may modify it */
    /** @todo switch to Containers::Pair once it exists and use Utility::copy()
        (std::pair isn't trivially copyable, ffs) */
    Containers::Array<std::pair<Containers::StringView, bool>> encounteredWorkarounds{info._state->encounteredWorkarounds.size()};
    for(std::size_t i = 0; i != encounteredWorkarounds.size(); ++i)
        encounteredWorkarounds[i] = info._state->encounteredWorkarounds[i];

    /* Initialize the enabled extension list and feature-, extension-,
       workaround-dependent function pointers */
    initializeExtensions<const char*>({info->ppEnabledExtensionNames, info->enabledExtensionCount});
    initialize(instance, version, encounteredWorkarounds, info._state->enabledFeatures | info._state->implicitFeatures);

    /* Print a list of used workarounds */
    if(!info._state->quietLog) {
        bool workaroundHeaderPrinted = false;
        for(const auto& workaround: encounteredWorkarounds) {
            /* Skip disabled workarounds */
            if(workaround.second) continue;

            if(!workaroundHeaderPrinted) {
                workaroundHeaderPrinted = true;
                Debug{} << "Using device driver workarounds:";
            }

            Debug{} << "   " << workaround.first;
        }
    }

    #ifndef CORRADE_NO_ASSERT
    /* This is a dumb O(n^2) search but in an assert that's completely fine */
    const Math::BoolVector<Implementation::ExtensionCount> missingExtensions = ~_enabledExtensions & info._state->featuresRequiredExtensions;
    if(missingExtensions.any()) {
        for(std::size_t i = 0; i != Implementation::ExtensionCount; ++i) {
            if(!missingExtensions[i]) continue;
            for(const Version version: KnownVersionsForExtensions) {
                for(const Extension extension: Extension::extensions(version)) {
                    if(extension.index() != i) continue;
                    CORRADE_ASSERT_UNREACHABLE("Vk::Device::tryCreate(): some enabled features need" << extension.string() << "enabled", {});
                }
            }
        }
    }
    #endif

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

    return Result::Success;
}

template<class T> void Device::initializeExtensions(const Containers::ArrayView<const T> enabledExtensions) {
    /* Mark all known extensions as enabled */
    for(const T extension: enabledExtensions) {
        for(const Version version: KnownVersionsForExtensions) {
            const Containers::ArrayView<const Extension> knownExtensions =
                Extension::extensions(version);
            auto found = std::lower_bound(knownExtensions.begin(), knownExtensions.end(), extension, [](const Extension& a, const T& b) {
                return a.string() < static_cast<const Containers::StringView&>(b);
            });
            if(found->string() != extension) continue;
            _enabledExtensions.set(found->index(), true);
        }
    }
}

void Device::initialize(Instance& instance, const Version version, Containers::Array<std::pair<Containers::StringView, bool>>& encounteredWorkarounds, const DeviceFeatures& enabledFeatures) {
    /* Init version, features, function pointers */
    _version = version;
    _enabledFeatures = enabledFeatures;
    flextVkInitDevice(_handle, &_functionPointers, instance->GetDeviceProcAddr);

    /* Set up extension-dependent functionality */
    _state.emplace(*this, encounteredWorkarounds);
}

bool Device::isExtensionEnabled(const Extension& extension) const {
    return _enabledExtensions[extension.index()];
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
