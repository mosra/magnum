/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>

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

#include "Context.h"

#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/String.h>
#include "vulkan.h"


namespace Magnum { namespace Vk {

PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT;
PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;

unsigned int layerCount = 2;
const char *validationLayerNames[] =
{
    // This is a meta layer that enables all of the standard
    // validation layers in the correct order :
    // threading, parameter_validation, device_limits, object_tracker, image, core_validation, swapchain, and unique_objects
    "VK_LAYER_LUNARG_standard_validation",
    "VK_LAYER_LUNARG_api_dump"
};

Context* Context::_current = nullptr;

bool Context::hasCurrent() { return _current; }

Context& Context::current() {
    CORRADE_ASSERT(_current, "Context::current(): no current context", *_current);
    return *_current;
}


Context::Context(): Context{Flags{}} {}

Context::Context(Flags flags): _functionLoader{nullptr}, _version{Version::None}, _flags(flags) {
    create();
}

Context::Context(Context&& other): _version{std::move(other._version)},
    _flags{std::move(other._flags)}
{
    if(_current == &other) _current = this;
}

Context::~Context() {
    if(_current == this) _current = nullptr;

    if(_instance == nullptr) {
        Error() << "No VkInstance to destroy";
        return;
    }

    if (_flags >= Flag::EnableValidation) {
        vkDestroyDebugReportCallbackEXT(_instance, _callback, nullptr);
    }
    vkDestroyInstance(_instance, nullptr);
}

void Context::create() {
    /* Hard exit if the context cannot be created */
    if(!tryCreate()) std::exit(1);
}

VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
    VkDebugReportFlagsEXT       flags,
    VkDebugReportObjectTypeEXT  objectType,
    uint64_t                    object,
    size_t                      location,
    int32_t                     messageCode,
    const char*                 pLayerPrefix,
    const char*                 pMessage,
    void*                       pUserData)
{
    if(flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT != 0) {
        Debug() << "[Debug][" << pLayerPrefix << "]" << pMessage;
    } else if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        Error() << "[Error][" << pLayerPrefix << "]" << pMessage;
    } else if(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        Warning() << "[Warning][" << pLayerPrefix << "]" << pMessage;
    } else if(flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        Warning() << "[Perf Warning][" << pLayerPrefix << "]" << pMessage;
    }
    
    return VK_FALSE;
}

bool Context::tryCreate() {
    _version = Version::Vulkan_1_0;

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Example";
    appInfo.pEngineName = "Magnum";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> enabledExtensions = {VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface"};

    // Enable surface extensions depending on os
    // enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = nullptr;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    if (_flags >= Flag::EnableValidation) {
        Debug() << "Enabling Validation";
        enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

        instanceCreateInfo.enabledLayerCount = layerCount;
        instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
    }

    if (!enabledExtensions.empty()) {
        instanceCreateInfo.enabledExtensionCount = enabledExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
    }

    VkResult ret = vkCreateInstance(&instanceCreateInfo, nullptr, &_instance);
    if(ret != VK_SUCCESS) {
        Error() << "Vulkan instance creation failed with error" << ret;
        return false;
    }

    _current = this;

    /* setup debugging */
    if (_flags >= Flag::EnableValidation) {
        /* Load VK_EXT_debug_report entry points in debug builds */
        vkCreateDebugReportCallbackEXT =
            reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>
                (vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT"));
        vkDebugReportMessageEXT =
            reinterpret_cast<PFN_vkDebugReportMessageEXT>
                (vkGetInstanceProcAddr(_instance, "vkDebugReportMessageEXT"));
        vkDestroyDebugReportCallbackEXT =
            reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>
                (vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT"));

        /* Setup callback creation information */
        VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
        callbackCreateInfo.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        callbackCreateInfo.pNext       = nullptr;
        callbackCreateInfo.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                         VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                         VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        callbackCreateInfo.pfnCallback = &MyDebugReportCallback;
        callbackCreateInfo.pUserData   = nullptr;

        /* Register the callback */
        VkResult err = vkCreateDebugReportCallbackEXT(_instance, &callbackCreateInfo, nullptr, &_callback);
        
        if (err != VK_SUCCESS) {
            Error() << "Could not setup Debug callback";
        }
    }
    return true;
}

bool Context::isVersionSupported(Version) const {
    return true;
}

Debug& operator<<(Debug& debug, Result value) {
    switch(value) {
        #define _c(value) case Result::value: return debug << "Vk::Result::" #value;
        _c(Success)
        _c(NotReady)
        _c(Timeout)
        _c(EventSet)
        _c(EventReset)
        _c(Incomplete)
        _c(ErrorOutOfHostMemory)
        _c(ErrorOutOfDevieMemory)
        _c(ErrorInitializationFailed)
        _c(ErrorDeviceLost)
        _c(ErrorMemoryMapFailed)
        _c(ErrorLayerNotPresent)
        _c(ErrorExtensionNotPresent)
        _c(ErrorFeatureNotPresent)
        _c(ErrorIncompatibleDriver)
        _c(ErrorTooManyObjects)
        _c(ErrorFormatNotSupported)
        _c(ErrorSurfaceLost)
        _c(ErrorNativeWindowInUse)
        _c(Suboptimal)
        _c(ErrorOutOfDate)
        _c(ErrorIncompatibleDisplay)
        _c(ErrorValidationFailed)
        _c(ErrorInvalidShader)
        _c(ErrorInvalidParameter)
        _c(ErrorInvalidAlignment)
        #undef _c
    }

    return debug << "Vk::Result::(invalid)";
}

Debug& operator<<(Debug& debug, Context::Flag value) {
    switch(value) {
        #define _c(value) case Context::Flag::value: return debug << "Context::Flag::" #value;
        _c(EnableValidation)
        #undef _c
    }

    return debug << "Context::Flag::(invalid)";
}

Debug& operator<<(Debug& debug, Version value) {
    switch(value) {
        #define _c(value) case Version::value: return debug << "Vk::Version::" #value;
        _c(None)
        _c(Vulkan_1_0)
        #undef _c
    }

    return debug << "Vk::Version::(invalid)";
}

}}
