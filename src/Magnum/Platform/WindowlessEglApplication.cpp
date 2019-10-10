/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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
    /* If relevant extensions are supported, try to find some display using
       those APIs, as that works reliably also when running headless. This
       would ideally use EGL 1.5 APIs but since we still want to support
       systems which either have old EGL headers or old EGL implementation,
       we'd need to have a code path for 1.4 *and* 1.5, plus do complicated
       version parsing from a string. Not feeling like doing that today, no. */
    const char* const extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if(extensions &&
        /* eglQueryDevicesEXT(). NVidia exposes only EGL_EXT_device_base, which
           is an older version of EGL_EXT_device_enumeration before it got
           split to that and EGL_EXT_device_query, so test for both. */
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
           --magnum-gpu-validation is enabled because otherwise it's fucking
           hard to discover what's to blame (lost > 3 hours already). See class
           docs for more info and a workaround. */
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

        if(magnumContext && (magnumContext->internalFlags() >= GL::Context::InternalFlag::DisplayVerboseInitializationLog)) {
            Debug{} << "Platform::WindowlessEglApplication: found" << count << "EGL devices, choosing device" << configuration.device();
        }

        /* Assuming the same thing won't suddenly start failing when called the
           second time */
        Containers::Array<EGLDeviceEXT> devices{configuration.device() + 1};
        CORRADE_INTERNAL_ASSERT_OUTPUT(eglQueryDevices(configuration.device() + 1, devices, &count));

        if(!(_display = reinterpret_cast<EGLDisplay(*)(EGLenum, void*, const EGLint*)>(eglGetProcAddress("eglGetPlatformDisplayEXT"))(EGL_PLATFORM_DEVICE_EXT, devices[configuration.device()], nullptr))) {
            Error{} << "Platform::WindowlessEglApplication::tryCreateContext(): cannot get platform display for a device:" << Implementation::eglErrorString(eglGetError());
            return;
        }
    } else
    #endif
    /* Otherwise initialize the classic way. WebGL doesn't have any of the
       above, so no need to compile that at all. */
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

    #ifndef MAGNUM_TARGET_GLES
    /* NVidia doesn't like EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR for
       some reason, failing eglCreateContext() with EGL_BAD_MATCH. If this flag
       is set, wipe it away. */
    const char* vendor = eglQueryString(_display, EGL_VENDOR);
    if((flags & Configuration::Flag::ForwardCompatible) && vendor && std::strcmp(vendor, "NVIDIA") == 0 && (!magnumContext || !magnumContext->isDriverWorkaroundDisabled("nv-egl-forward-compatible-context-unhappy"))) {
        flags &= ~Configuration::Flag::ForwardCompatible;
    }
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

    if(!(_context = eglCreateContext(_display, config, EGL_NO_CONTEXT, attributes))) {
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

WindowlessEglContext::WindowlessEglContext(WindowlessEglContext&& other): _display{other._display}, _context{other._context} {
    other._display = {};
    other._context = {};
}

WindowlessEglContext::~WindowlessEglContext() {
    if(_context) eglDestroyContext(_display, _context);
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    if(_surface) eglDestroySurface(_display, _surface);
    #endif
    if(_display) eglTerminate(_display);
}

WindowlessEglContext& WindowlessEglContext::operator=(WindowlessEglContext && other) {
    using std::swap;
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
        .setFromEnvironment("device");
    #endif
    _context.reset(new GLContext{NoCreate, args, arguments.argc, arguments.argv});

    #ifndef MAGNUM_TARGET_WEBGL
    if(args.value("device").empty())
        _commandLineDevice = 0;
    else
        _commandLineDevice = args.value<UnsignedInt>("device");
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
        mergedConfiguration.setDevice(_commandLineDevice);
    #endif

    WindowlessEglContext glContext{mergedConfiguration, _context.get()};
    if(!glContext.isCreated() || !glContext.makeCurrent() || !_context->tryCreate())
        return false;

    _glContext = std::move(glContext);
    return true;
}

WindowlessEglApplication::~WindowlessEglApplication() = default;

}}
