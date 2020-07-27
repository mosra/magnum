/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2020 Erik Wijmans <etw@gatech.edu>

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

#include "WindowlessEglApplication.h"

#include <cstring>
#include <string>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"

#include "Implementation/Egl.h"

/* None of this is in the Emscripten emulation layer, so no need to include
   that there */
#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/Array.h>

#ifndef EGL_EXT_device_base
typedef void *EGLDeviceEXT;
#endif

#ifndef EGL_NV_device_cuda
#define EGL_CUDA_DEVICE_NV 0x323A
#endif

#ifndef EGL_EXT_platform_device
#define EGL_PLATFORM_DEVICE_EXT 0x313F
#endif

#ifndef EGL_VERSION_1_5
typedef intptr_t EGLAttrib;
#endif

#ifndef EGL_KHR_debug
#define EGL_DEBUG_MSG_WARN_KHR 0x33BB
#define EGL_DEBUG_MSG_INFO_KHR 0x33BC
typedef void* EGLObjectKHR;
typedef void* EGLLabelKHR;
typedef void (APIENTRY *EGLDEBUGPROCKHR)(EGLenum error, const char* command, EGLint messageType, EGLLabelKHR threadLabel, EGLLabelKHR objectLabel, const char* message);
#endif
#endif

namespace Magnum { namespace Platform {

#ifndef MAGNUM_TARGET_WEBGL
namespace {

bool extensionSupported(const char* const extensions, Containers::ArrayView<const char> extension) {
    CORRADE_INTERNAL_ASSERT(extensions);
    const char* pos = std::strstr(extensions, extension);
    /* Extension is supported if its string is delimited by a space or end of
       the extension list. The extension.size() is the whole C array including
       a 0-terminator, so subtract 1 to look at one character after. */
    return pos && (pos[extension.size() - 1] == ' ' || pos[extension.size() - 1] == '\0');
}

}
#endif

WindowlessEglContext::WindowlessEglContext(const Configuration& configuration, GLContext* const magnumContext) {
    #ifndef MAGNUM_TARGET_WEBGL
    /* The user provided a shared context, use the associated display
       directly. We don't call eglInitialize() in this case either -- the
       context we share with already did that on the provided display */
    if(configuration.sharedContext() != EGL_NO_CONTEXT && configuration.sharedDisplay() != EGL_NO_DISPLAY) {
        _display = configuration.sharedDisplay();
        _sharedContext = true;
    } else
    #endif
    /* Otherwise find the display and initialize EGL */
    {
        #ifndef MAGNUM_TARGET_WEBGL
        /* If relevant extensions are supported, try to find some display using
           those APIs, as that works reliably also when running headless. This
           would ideally use EGL 1.5 APIs but since we still want to support
           systems which either have old EGL headers or old EGL implementation,
           we'd need to have a code path for 1.4 *and* 1.5, plus do complicated
           version parsing from a string. Not feeling like doing that today,
           no. */
        const char* const extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if(extensions &&
            /* eglQueryDevicesEXT(). NVidia exposes only EGL_EXT_device_base,
               which is an older version of EGL_EXT_device_enumeration before
               it got split to that and EGL_EXT_device_query, so test for both. */
            (extensionSupported(extensions, "EGL_EXT_device_enumeration") || extensionSupported(extensions, "EGL_EXT_device_base")) &&

            /* eglGetPlatformDisplayEXT() */
            extensionSupported(extensions, "EGL_EXT_platform_base") &&

            /* EGL_PLATFORM_DEVICE_EXT (FFS, why it has to be scattered over a
               thousand extensions?!). This is supported only since Mesa 19.2. */
            extensionSupported(extensions, "EGL_EXT_platform_device")
        ) {
            /* When libEGL_nvidia.so is present on a system w/o a NV GPU,
               eglQueryDevicesEXT() fails there with EGL_BAD_ALLOC, but that is
               never propagated to the glvnd wrapper. Enable debug output if
               --magnum-gpu-validation is enabled because otherwise it's
               fucking hard to discover what's to blame (lost > 3 hours
               already). See class docs for more info and a workaround. */
            if(extensionSupported(extensions, "EGL_KHR_debug") && magnumContext && (magnumContext->internalFlags() & GL::Context::InternalFlag::GpuValidation)) {
                auto eglDebugMessageControl = reinterpret_cast<EGLint(*)(EGLDEBUGPROCKHR, const EGLAttrib*)>(eglGetProcAddress("eglDebugMessageControlKHR"));
                const EGLAttrib debugAttribs[] = {
                    EGL_DEBUG_MSG_WARN_KHR, EGL_TRUE,
                    EGL_DEBUG_MSG_INFO_KHR, EGL_TRUE,
                    EGL_NONE
                };
                CORRADE_INTERNAL_ASSERT_OUTPUT(eglDebugMessageControl([](EGLenum, const char* const command, EGLint, EGLLabelKHR, EGLLabelKHR, const char* message) {
                    Debug{} << command << Debug::nospace << "():" << Utility::String::rtrim(message);
                }, debugAttribs) == EGL_SUCCESS);
            }

            EGLint count;
            auto eglQueryDevices = reinterpret_cast<EGLBoolean(*)(EGLint, EGLDeviceEXT*, EGLint*)>(eglGetProcAddress("eglQueryDevicesEXT"));
            if(!eglQueryDevices(0, nullptr, &count)) {
                Error{} << "Platform::WindowlessEglApplication::tryCreateContext(): cannot query EGL devices:" << Implementation::eglErrorString(eglGetError());
                return;
            }

            if(!count) {
                Error e;
                e << "Platform::WindowlessEglApplication::tryCreateContext(): no EGL devices found, likely a driver issue";
                if(!magnumContext || !(magnumContext->internalFlags() & GL::Context::InternalFlag::GpuValidation))
                    e << Debug::nospace << "; enable --magnum-gpu-validation to see additional info";
                return;
            }

            if(configuration.device() >= UnsignedInt(count)) {
                Error{} << "Platform::WindowlessEglContext: requested EGL device" << configuration.device() << "but found only" << count;
                return;
            }

            UnsignedInt selectedDevice;
            Containers::Array<EGLDeviceEXT> devices;

            /* Look for CUDA devices */
            if(configuration.cudaDevice() != ~UnsignedInt{}) {
                if(!(extensionSupported(extensions, "EGL_EXT_device_query") || extensionSupported(extensions, "EGL_EXT_device_base"))) {
                    Error e;
                    e << "Platform::WindowlessEglApplication: CUDA device selection requires EGL_EXT_device_query or EGL_EXT_device_base extensions";
                    return;
                }

                devices = Containers::Array<EGLDeviceEXT>{std::size_t(count)};
                /* Assuming the same thing won't suddenly start failing when
                   called the second time */
                CORRADE_INTERNAL_ASSERT_OUTPUT(eglQueryDevices(devices.size(), devices, &count));

                auto eglQueryDeviceAttribEXT = reinterpret_cast<EGLBoolean (*)(EGLDeviceEXT, EGLint, EGLAttrib*)>(eglGetProcAddress("eglQueryDeviceAttribEXT"));
                auto eglQueryDeviceStringEXT = reinterpret_cast<const char* (*)(EGLDeviceEXT, EGLint)>(eglGetProcAddress("eglQueryDeviceStringEXT"));

                /* Go through the EGL devices and find one that has the desired
                   CUDA device number */
                for(selectedDevice = 0; selectedDevice < UnsignedInt(count); ++selectedDevice) {
                    if(magnumContext && (magnumContext->internalFlags() >= GL::Context::InternalFlag::DisplayVerboseInitializationLog))
                        Debug{} << "Platform::WindowlessEglApplication: eglQueryDeviceStringEXT(EGLDevice=" << Debug::nospace << selectedDevice << Debug::nospace << "):" << eglQueryDeviceStringEXT(devices[selectedDevice], EGL_EXTENSIONS);

                    EGLAttrib cudaDeviceNumber;
                    if(eglQueryDeviceAttribEXT(devices[selectedDevice], EGL_CUDA_DEVICE_NV, &cudaDeviceNumber) && UnsignedInt(cudaDeviceNumber) == configuration.cudaDevice())
                        break;
                }

                /* None found */
                if(selectedDevice == UnsignedInt(count)) {
                    Error e;
                    e << "Platform::WindowlessEglApplication::tryCreateContext(): unable to find EGL device for CUDA device" << configuration.cudaDevice();
                    return;
                }

                if(magnumContext && (magnumContext->internalFlags() >= GL::Context::InternalFlag::DisplayVerboseInitializationLog)) {
                    Debug{} << "Platform::WindowlessEglApplication: found" << count << "EGL devices, choosing EGL device" << selectedDevice << "for CUDA device" << configuration.cudaDevice();
                }

            /* Otherwise just a normal EGL device */
            } else {
                /* Print the log *before* calling eglQueryDevices() again, as
                   the `count` gets overwritten by it */
                if(magnumContext && (magnumContext->internalFlags() >= GL::Context::InternalFlag::DisplayVerboseInitializationLog)) {
                    Debug{} << "Platform::WindowlessEglApplication: found" << count << "EGL devices, choosing device" << configuration.device();
                }

                selectedDevice = configuration.device();
                devices = Containers::Array<EGLDeviceEXT>{configuration.device() + 1};
                /* Assuming the same thing won't suddenly start failing when
                   called the second time */
                CORRADE_INTERNAL_ASSERT_OUTPUT(eglQueryDevices(configuration.device() + 1, devices, &count));
            }

            if(!(_display = reinterpret_cast<EGLDisplay(*)(EGLenum, void*, const EGLint*)>(eglGetProcAddress("eglGetPlatformDisplayEXT"))(EGL_PLATFORM_DEVICE_EXT, devices[selectedDevice], nullptr))) {
                Error{} << "Platform::WindowlessEglApplication::tryCreateContext(): cannot get platform display for a device:" << Implementation::eglErrorString(eglGetError());
                return;
            }
        } else
        /* Otherwise initialize the classic way. WebGL doesn't have any of the
           above, so no need to compile that at all. */
        #endif
        {
            #ifndef MAGNUM_TARGET_WEBGL
            if(configuration.device() != 0) {
                Error{} << "Platform::WindowlessEglContext: requested EGL device" << configuration.device() << "but EGL_EXT_platform_device is not supported and there's just the default one";
                return;
            }
            #endif

            if(!(_display = eglGetDisplay(EGL_DEFAULT_DISPLAY))) {
                Error{} << "Platform::WindowlessEglApplication::tryCreateContext(): cannot get default EGL display:" << Implementation::eglErrorString(eglGetError());
                return;
            }
        }

        if(!eglInitialize(_display, nullptr, nullptr)) {
            Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot initialize EGL:" << Implementation::eglErrorString(eglGetError());
            return;
        }
    }

    const EGLenum api =
        #ifndef MAGNUM_TARGET_GLES
        EGL_OPENGL_API
        #else
        EGL_OPENGL_ES_API
        #endif
        ;
    if(!eglBindAPI(api)) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot bind EGL API:" << Implementation::eglErrorString(eglGetError());
        return;
    }

    /* Choose EGL config */
    static const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        #ifndef MAGNUM_TARGET_GLES
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        #elif defined(MAGNUM_TARGET_GLES2) || defined(CORRADE_TARGET_EMSCRIPTEN)
        /* Emscripten doesn't know about EGL_OPENGL_ES3_BIT_KHR for WebGL 2 and
           the whole thing is controlled only by EGL_CONTEXT_CLIENT_VERSION and
           the `-s USE_WEBGL2=1` flag anyway, so it doesn't matter that we ask
           for ES2 on WebGL 2 as well. */
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        #elif defined(MAGNUM_TARGET_GLES3)
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
        #else
        #error unsupported OpenGL edition
        #endif
        EGL_NONE
    };
    EGLConfig config;
    EGLint configCount;
    if(!eglChooseConfig(_display, attribs, &config, 1, &configCount)) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot get EGL visual config:" << Implementation::eglErrorString(eglGetError());
        return;
    }

    if(!configCount) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): no matching EGL visual config available";
        return;
    }

    #ifndef MAGNUM_TARGET_WEBGL
    /* Request debug context if --magnum-gpu-validation is enabled */
    Configuration::Flags flags = configuration.flags();
    if(magnumContext && magnumContext->internalFlags() & GL::Context::InternalFlag::GpuValidation)
        flags |= Configuration::Flag::Debug;
    #endif

    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_WEBGL)
    const /* Is modified below to work around a SwiftShader limitation */
    #endif
    EGLint attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION,
            #ifdef MAGNUM_TARGET_GLES
            #if defined(MAGNUM_TARGET_GLES2) || (defined(CORRADE_TARGET_EMSCRIPTEN) && __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ < 13824)
            /* Emscripten before 1.38.24 doesn't know about version 3 for WebGL
               2 and the whole thing is controlled by -s USE_WEBGL2=1 flag
               anyway, so it doesn't matter that we ask for ES2 on WebGL 2 as
               well. https://github.com/emscripten-core/emscripten/pull/7858 */
            2,
            #elif defined(MAGNUM_TARGET_GLES3)
            3,
            #else
            #error unsupported OpenGL ES version
            #endif
            #else
            3,
            #endif
        #ifndef MAGNUM_TARGET_WEBGL
        /* Needs to be last because we're zeroing this out for SwiftShader (see
           below) */
        EGL_CONTEXT_FLAGS_KHR, EGLint(flags),
        #endif
        EGL_NONE
    };

    #ifdef MAGNUM_TARGET_WEBGL
    static_cast<void>(configuration);
    static_cast<void>(magnumContext);
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    const char* version = eglQueryString(_display, EGL_VERSION);

    /* SwiftShader 3.3.0.1 blows up on encountering EGL_CONTEXT_FLAGS_KHR with
       a zero value, so erase these. It also doesn't handle them as correct
       flags, but instead checks for the whole value, so a combination won't
       work either: https://github.com/google/swiftshader/blob/5fb5e817a20d3e60f29f7338493f922b5ac9d7c4/src/OpenGL/libEGL/libEGL.cpp#L794-L8104 */
    if(!configuration.flags() && version && std::strstr(version, "SwiftShader") != nullptr && (!magnumContext || !magnumContext->isDriverWorkaroundDisabled("swiftshader-no-empty-egl-context-flags"))) {
        auto& contextFlags = attributes[Containers::arraySize(attributes) - 3];
        CORRADE_INTERNAL_ASSERT(contextFlags == EGL_CONTEXT_FLAGS_KHR);
        contextFlags = EGL_NONE;
    }
    #endif

    _context = eglCreateContext(_display, config,
        #ifndef MAGNUM_TARGET_WEBGL
        configuration.sharedContext(),
        #else
        EGL_NO_CONTEXT,
        #endif
        attributes);

    #ifndef MAGNUM_TARGET_GLES
    /* Fall back to (forward compatible) GL 2.1 if core context creation fails */
    if(!_context) {
        Warning{} << "Platform::WindowlessEglContext: cannot create core context, falling back to compatibility context:" << Implementation::eglErrorString(eglGetError());

        const EGLint fallbackAttributes[] = {
            /* Discard the ForwardCompatible flag for the fallback. Having it
               set makes the fallback context creation fail on Mesa's Zink
               (which is just 2.1) and I assume on others as well. */
            EGL_CONTEXT_FLAGS_KHR, GLint(flags & ~Configuration::Flag::ForwardCompatible),
            EGL_NONE
        };
        _context = eglCreateContext(_display, config, configuration.sharedContext(), fallbackAttributes);

    /* Fall back to (forward compatible) GL 2.1 if we are on binary NVidia/AMD
       drivers on Linux. Instead of creating forward-compatible context with
       highest available version, they force the version to the one specified,
       which is completely useless behavior. */
    } else {
        /* We need to make the context current to read out vendor string, so
           save the previous values so we can safely revert back without
           messing up the state */
        EGLSurface currentSurface = eglGetCurrentSurface(EGL_DRAW);
        EGLSurface currentReadSurface = eglGetCurrentSurface(EGL_READ);
        EGLContext currentContext = eglGetCurrentContext();
        if(!eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, _context)) {
            Error{} << "Platform::WindowlessEglContext: cannot make context current:" << Implementation::eglErrorString(eglGetError());
            return;
        }

        /* The workaround check is the last so it doesn't appear in workaround
           list on unrelated drivers */
        constexpr static const char nvidiaVendorString[] = "NVIDIA Corporation";
        constexpr static const char amdVendorString[] = "ATI Technologies Inc.";
        const char* const vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        /* If context creation fails *really bad*, glGetString() may actually
           return nullptr. Check for that to avoid crashes deep inside
           strncmp() */
        if(vendorString && (std::strncmp(vendorString, nvidiaVendorString, sizeof(nvidiaVendorString)) == 0 ||
            std::strncmp(vendorString, amdVendorString, sizeof(amdVendorString)) == 0) &&
            (!magnumContext || !magnumContext->isDriverWorkaroundDisabled("no-forward-compatible-core-context")))
        {
            /* Destroy the core context and create a compatibility one */
            eglDestroyContext(_display, _context);
            const GLint fallbackAttributes[] = {
                /* Discard the ForwardCompatible flag for the fallback.
                   Compared to the above case of a 2.1 fallback it's not really
                   needed here (AFAIK it works in both cases), but let's be
                   consistent. */
                EGL_CONTEXT_FLAGS_KHR, GLint(flags & ~Configuration::Flag::ForwardCompatible),
                EGL_NONE
            };
            _context = eglCreateContext(_display, config, configuration.sharedContext(), fallbackAttributes);
        }

        /* Revert back the old context */
        if(!eglMakeCurrent(_display, currentSurface, currentReadSurface, currentContext)) {
            Error() << "Platform::WindowlessEglContext: cannot make the previous context current";
            return;
        }
    }
    #endif

    if(!_context) {
        Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot create EGL context:" << Implementation::eglErrorString(eglGetError());
        return;
    }

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    /* SwiftShader 3.3.0.1 needs some pbuffer, otherwise it crashes somewhere
       deep inside when making the context current */
    if(version && std::strstr(version, "SwiftShader") != nullptr && (!magnumContext || !magnumContext->isDriverWorkaroundDisabled("swiftshader-egl-context-needs-pbuffer"))) {
        EGLint surfaceAttributes[] = {
            EGL_WIDTH, 32,
            EGL_HEIGHT, 32,
            EGL_NONE
        };
        _surface = eglCreatePbufferSurface(_display, config, surfaceAttributes);
    }
    #endif
}

WindowlessEglContext::WindowlessEglContext(WindowlessEglContext&& other):
    #ifndef MAGNUM_TARGET_WEBGL
    _sharedContext{other._sharedContext},
    #endif
    _display{other._display}, _context{other._context}
{
    #ifndef MAGNUM_TARGET_WEBGL
    other._sharedContext = false;
    #endif
    other._display = {};
    other._context = {};
}

WindowlessEglContext::~WindowlessEglContext() {
    if(_context) {
        /* eglDestroyContext() doesn't actually destroy the context if it's
           still current, it's only destroyed once eglMakeCurrent() makes some
           other context current. This causes the "cannot make the previous
           context current" error from above to appear if one creates an EGL
           context again for a second time --- we switch from the (now zombie)
           context to a new one to read the vendor string for the
           "no-forward-compatible-core-context" workaround, at which point the
           zombie gets finally killed, which then means we can't
           eglMakeCurrent() it back after. */
        eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(_display, _context);
    }
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(_surface) eglDestroySurface(_display, _surface);
    #endif

    /* Don't terminate EGL if we're a shared context as it would kill all
       others as well. In case of a shared context it's expected that the
       first instance of WindowlessEglContext in the shared chain is destroyed
       last, calling eglTerminate() after all others are gone. */
    if(
        #ifndef MAGNUM_TARGET_WEBGL
        !_sharedContext &&
        #endif
        _display) eglTerminate(_display);
}

WindowlessEglContext& WindowlessEglContext::operator=(WindowlessEglContext&& other) {
    using std::swap;
    #ifndef MAGNUM_TARGET_WEBGL
    swap(other._sharedContext, _sharedContext);
    #endif
    swap(other._display, _display);
    swap(other._context, _context);
    return *this;
}

bool WindowlessEglContext::makeCurrent() {
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    /* _surface is EGL_NO_SURFACE everywhere except on SwiftShader. See above
       for details. */
    if(eglMakeCurrent(_display, _surface, _surface, _context))
        return true;
    #else
    if(eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, _context))
        return true;
    #endif

    Error() << "Platform::WindowlessEglApplication::tryCreateContext(): cannot make context current:" << Implementation::eglErrorString(eglGetError());
    return false;
}

WindowlessEglContext::Configuration::Configuration()
    #ifndef MAGNUM_TARGET_GLES
    : _flags{Flag::ForwardCompatible}, _device{}
    #elif !defined(MAGNUM_TARGET_WEBGL)
    : _flags{}, _device{}
    #endif
    {}

#ifndef DOXYGEN_GENERATING_OUTPUT
WindowlessEglApplication::WindowlessEglApplication(const Arguments& arguments): WindowlessEglApplication{arguments, Configuration{}} {}
#endif

WindowlessEglApplication::WindowlessEglApplication(const Arguments& arguments, const Configuration& configuration): WindowlessEglApplication{arguments, NoCreate} {
    createContext(configuration);
}

WindowlessEglApplication::WindowlessEglApplication(const Arguments& arguments, NoCreateT): _glContext{NoCreate} {
    Utility::Arguments args{"magnum"};
    #ifndef MAGNUM_TARGET_WEBGL
    args.addOption("device", "").setHelp("device", "GPU device to use", "N")
        .setFromEnvironment("device")
        .addOption("cuda-device", "").setHelp("cuda-device", "CUDA device to use. Takes precedence over --magnum-device.", "N")
        .setFromEnvironment("cuda-device");
    #endif
    _context.reset(new GLContext{NoCreate, args, arguments.argc, arguments.argv});

    #ifndef MAGNUM_TARGET_WEBGL
    if(args.value("device").empty())
        _commandLineDevice = 0;
    else
        _commandLineDevice = args.value<UnsignedInt>("device");

    if (args.value("cuda-device").empty())
        _commandLineCudaDevice = ~UnsignedInt{};
    else
        _commandLineCudaDevice = args.value<UnsignedInt>("cuda-device");
    #endif
}

void WindowlessEglApplication::createContext() { createContext({}); }

void WindowlessEglApplication::createContext(const Configuration& configuration) {
    if(!tryCreateContext(configuration)) std::exit(1);
}

bool WindowlessEglApplication::tryCreateContext(const Configuration& configuration) {
    CORRADE_ASSERT(_context->version() == GL::Version::None, "Platform::WindowlessEglApplication::tryCreateContext(): context already created", false);

    /* Command-line arguments override what's set programatically */
    Configuration mergedConfiguration{configuration};
    #ifndef MAGNUM_TARGET_WEBGL
    if(!mergedConfiguration.device())
        mergedConfiguration.setDevice(_commandLineDevice).setCudaDevice(_commandLineCudaDevice);
    #endif

    WindowlessEglContext glContext{mergedConfiguration, _context.get()};
    if(!glContext.isCreated() || !glContext.makeCurrent() || !_context->tryCreate())
        return false;

    _glContext = std::move(glContext);
    return true;
}

#ifndef MAGNUM_TARGET_WEBGL
auto WindowlessEglContext::Configuration::setSharedContext(EGLDisplay display, EGLContext context) -> Configuration& {
    CORRADE_ASSERT((context == EGL_NO_CONTEXT) == (display == EGL_NO_DISPLAY),
        "Platform::WindowlessEglContext::Configuration::setSharedContext(): either both the context and the display have to be valid or both null", *this);
    _sharedDisplay = display;
    _sharedContext = context;
    return *this;
}
#endif

WindowlessEglApplication::~WindowlessEglApplication() = default;

}}
