/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2019 Marco Melorio <m.melorio@icloud.com>

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

#include "Sdl2Application.h"

#include <cstring>
#ifdef CORRADE_TARGET_CLANG_CL
/* SDL does #pragma pack(push,8) and #pragma pack(pop,8) in different headers
   (begin_code.h and end_code.h) and clang-cl doesn't like that, even though it
   is completely fine. Silence the warning. */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#endif
#include <SDL.h>
#ifdef CORRADE_TARGET_CLANG_CL
#pragma clang diagnostic pop
#endif
#ifndef CORRADE_TARGET_EMSCRIPTEN
#include <tuple>
#else
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif
#include <Corrade/Utility/Arguments.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Platform/ScreenedApplication.hpp"
#include "Magnum/Platform/Implementation/DpiScaling.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"
#endif

#if defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)
/* For physical DPI scaling */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace Magnum { namespace Platform {

namespace {

/*
 * Fix up the modifiers -- we want >= operator to work properly on Shift,
 * Ctrl, Alt, but SDL generates different event for left / right keys, thus
 * (modifiers >= Shift) would pass only if both left and right were pressed,
 * which is usually not what the developers wants.
 */
Sdl2Application::InputEvent::Modifiers fixedModifiers(Uint16 mod) {
    Sdl2Application::InputEvent::Modifiers modifiers(static_cast<Sdl2Application::InputEvent::Modifier>(mod));
    if(modifiers & Sdl2Application::InputEvent::Modifier::Shift) modifiers |= Sdl2Application::InputEvent::Modifier::Shift;
    if(modifiers & Sdl2Application::InputEvent::Modifier::Ctrl) modifiers |= Sdl2Application::InputEvent::Modifier::Ctrl;
    if(modifiers & Sdl2Application::InputEvent::Modifier::Alt) modifiers |= Sdl2Application::InputEvent::Modifier::Alt;
    if(modifiers & Sdl2Application::InputEvent::Modifier::Super) modifiers |= Sdl2Application::InputEvent::Modifier::Alt;
    return modifiers;
}

}

enum class Sdl2Application::Flag: UnsignedByte {
    Redraw = 1 << 0,
    VSyncEnabled = 1 << 1,
    NoTickEvent = 1 << 2,
    NoAnyEvent = 1 << 3,
    Exit = 1 << 4,
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    TextInputActive = 1 << 5,
    Resizable = 1 << 6,
    #endif
    #ifdef CORRADE_TARGET_APPLE
    HiDpiWarningPrinted = 1 << 7
    #endif
};

Sdl2Application::Sdl2Application(const Arguments& arguments): Sdl2Application{arguments, Configuration{}} {}

Sdl2Application::Sdl2Application(const Arguments& arguments, const Configuration& configuration): Sdl2Application{arguments, NoCreate} {
    create(configuration);
}

#ifdef MAGNUM_TARGET_GL
Sdl2Application::Sdl2Application(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration): Sdl2Application{arguments, NoCreate} {
    create(configuration, glConfiguration);
}
#endif

Sdl2Application::Sdl2Application(const Arguments& arguments, NoCreateT):
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    _minimalLoopPeriod{0},
    #endif
    _flags{Flag::Redraw}
{
    Utility::Arguments args{Implementation::windowScalingArguments()};
    #ifdef MAGNUM_TARGET_GL
    _context.reset(new GLContext{NoCreate, args, arguments.argc, arguments.argv});
    #else
    /** @todo this is duplicated here and in GlfwApplication, figure out a nice
        non-duplicated way to handle this */
    args.addOption("log", "default").setHelp("log", "console logging", "default|quiet|verbose")
        .setFromEnvironment("log")
        .parse(arguments.argc, arguments.argv);
    #endif

    /* Available since 2.0.4, disables interception of SIGINT and SIGTERM so
       it's possible to Ctrl-C the application even if exitEvent() doesn't set
       event.setAccepted(). */
    #ifdef SDL_HINT_NO_SIGNAL_HANDLERS
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
    #endif
    /* Available since 2.0.6, uses dedicated OpenGL ES drivers by default and a
       desktop GLES context only if MAGNUM_TARGET_DESKTOP_GLES is defined as
       well. */
    #if !defined(MAGNUM_TARGET_DESKTOP_GLES) && defined(SDL_HINT_OPENGL_ES_DRIVER)
    SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
    #endif
    /* Available since 2.0.8, disables compositor bypass on X11, which causes
       flickering on KWin as the compositor gets shut down on every startup */
    #ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    #endif

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error() << "Cannot initialize SDL:" << SDL_GetError();
        std::exit(1);
    }

    /* Save command-line arguments */
    if(args.value("log") == "verbose") _verboseLog = true;
    const std::string dpiScaling = args.value("dpi-scaling");
    if(dpiScaling == "default")
        _commandLineDpiScalingPolicy = Implementation::Sdl2DpiScalingPolicy::Default;
    #ifdef CORRADE_TARGET_APPLE
    else if(dpiScaling == "framebuffer")
        _commandLineDpiScalingPolicy = Implementation::Sdl2DpiScalingPolicy::Framebuffer;
    #endif
    #ifndef CORRADE_TARGET_APPLE
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
    else if(dpiScaling == "virtual")
        _commandLineDpiScalingPolicy = Implementation::Sdl2DpiScalingPolicy::Virtual;
    #endif
    else if(dpiScaling == "physical")
        _commandLineDpiScalingPolicy = Implementation::Sdl2DpiScalingPolicy::Physical;
    #endif
    else if(dpiScaling.find_first_of(" \t\n") != std::string::npos)
        _commandLineDpiScaling = args.value<Vector2>("dpi-scaling");
    else
        _commandLineDpiScaling = Vector2{args.value<Float>("dpi-scaling")};
}

void Sdl2Application::create() {
    create(Configuration{});
}

void Sdl2Application::create(const Configuration& configuration) {
    if(!tryCreate(configuration)) std::exit(1);
}

#ifdef MAGNUM_TARGET_GL
void Sdl2Application::create(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    if(!tryCreate(configuration, glConfiguration)) std::exit(1);
}
#endif

Vector2 Sdl2Application::dpiScaling(const Configuration& configuration) {
    std::ostream* verbose = _verboseLog ? Debug::output() : nullptr;

    /* Print a helpful warning in case some extra steps are needed for HiDPI
       support */
    #ifdef CORRADE_TARGET_APPLE
    if(!Implementation::isAppleBundleHiDpiEnabled() && !(_flags & Flag::HiDpiWarningPrinted)) {
        Warning{} << "Platform::Sdl2Application: warning: the executable is not a HiDPI-enabled app bundle";
        _flags |= Flag::HiDpiWarningPrinted;
    }
    #elif defined(CORRADE_TARGET_WINDOWS)
    /* Handled below, warning printed only when using virtual DPI scaling */
    #endif

    /* Use values from the configuration only if not overriden on command line
       to something non-default. In any case explicit scaling has a precedence
       before the policy. */
    Implementation::Sdl2DpiScalingPolicy dpiScalingPolicy{};
    if(!_commandLineDpiScaling.isZero()) {
        Debug{verbose} << "Platform::Sdl2Application: user-defined DPI scaling" << _commandLineDpiScaling.x();
        return _commandLineDpiScaling;
    } else if(_commandLineDpiScalingPolicy != Implementation::Sdl2DpiScalingPolicy::Default) {
        dpiScalingPolicy = _commandLineDpiScalingPolicy;
    } else if(!configuration.dpiScaling().isZero()) {
        Debug{verbose} << "Platform::Sdl2Application: app-defined DPI scaling" << _commandLineDpiScaling.x();
        return configuration.dpiScaling();
    } else {
        dpiScalingPolicy = configuration.dpiScalingPolicy();
    }

    /* There's no choice on Apple, it's all controlled by the plist file. So
       unless someone specified custom scaling via config or command-line
       above, return the default. */
    #ifdef CORRADE_TARGET_APPLE
    return Vector2{1.0f};

    /* Otherwise there's a choice between virtual and physical DPI scaling */
    #else
    /* Try to get virtual DPI scaling first, if supported and requested */
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
    if(dpiScalingPolicy == Implementation::Sdl2DpiScalingPolicy::Virtual) {
        /* Use Xft.dpi on X11, because SDL_GetDisplayDPI() returns the useless
           physical value on Linux, while the virtual value on Windows. */
        #ifdef _MAGNUM_PLATFORM_USE_X11
        const Vector2 dpiScaling{Implementation::x11DpiScaling()};
        if(!dpiScaling.isZero()) {
            Debug{verbose} << "Platform::Sdl2Application: virtual DPI scaling" << dpiScaling.x();
            return dpiScaling;
        }

        /* Check for DPI awareness on (non-RT) Windows and then ask for DPI.
           SDL_GetDisplayDPI() is querying GetDpiForMonitor() --
           https://github.com/spurious/SDL-mirror/blob/17af4584cb28cdb3c2feba17e7d989a806007d9f/src/video/windows/SDL_windowsmodes.c#L266
           and GetDpiForMonitor() returns 96 if the application is DPI unaware.
           So we instead check for DPI awareness first (and tell the user if
           not), and only if the app is, then we use SDL_GetDisplayDPI(). */
        #elif defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)
        if(!Implementation::isWindowsAppDpiAware()) {
            Warning{verbose} << "Platform::Sdl2Application: your application is not set as DPI-aware, DPI scaling won't be used";
            return Vector2{1.0f};
        }
        Vector2 dpi;
        if(SDL_GetDisplayDPI(0, nullptr, &dpi.x(), &dpi.y()) == 0) {
            const Vector2 dpiScaling{dpi/96.0f};
            Debug{verbose} << "Platform::Sdl2Application: virtual DPI scaling" << dpiScaling;
            return dpiScaling;
        }

        /* Otherwise ¯\_(ツ)_/¯ */
        #else
        Debug{verbose} << "Platform::Sdl2Application: sorry, virtual DPI scaling not implemented on this platform yet, falling back to physical DPI scaling";
        #endif
    }
    #endif

    /* At this point, either the virtual DPI query failed or a physical DPI
       scaling is requested */
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
    CORRADE_INTERNAL_ASSERT(dpiScalingPolicy == Implementation::Sdl2DpiScalingPolicy::Virtual || dpiScalingPolicy == Implementation::Sdl2DpiScalingPolicy::Physical);
    #else
    CORRADE_INTERNAL_ASSERT(dpiScalingPolicy == Implementation::Sdl2DpiScalingPolicy::Physical);
    #endif

    /* Take device pixel ratio on Emscripten */
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    const Vector2 dpiScaling{Implementation::emscriptenDpiScaling()};
    Debug{verbose} << "Platform::Sdl2Application: physical DPI scaling" << dpiScaling.x();
    return dpiScaling;

    /* Take a physical display DPI. On Linux it gets the (usually very off)
       physical value from X11. Also only since SDL 2.0.4. */
    #elif defined(CORRADE_TARGET_UNIX)
    #if SDL_VERSION_ATLEAST(2, 0, 4)
    Vector2 dpi;
    if(SDL_GetDisplayDPI(0, nullptr, &dpi.x(), &dpi.y()) == 0) {
        const Vector2 dpiScaling{dpi/96.0f};
        Debug{verbose} << "Platform::Sdl2Application: physical DPI scaling" << dpiScaling;
        return dpiScaling;
    }

    Warning{} << "Platform::Sdl2Application: can't get physical display DPI, falling back to no scaling:" << SDL_GetError();
    #else
    Debug{verbose} << "Platform::Sdl2Application: sorry, physical DPI scaling only available on SDL 2.0.4+";
    #endif
    return Vector2{1.0f};

    /* HOWEVER, on Windows it gets the virtual DPI scaling, which we don't
       want, so we need to call Windows APIs directly instead. Consistency my
       ass. Related bug report that will probably never get actually
       implemented: https://bugzilla.libsdl.org/show_bug.cgi?id=2473 */
    #elif defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)
    HDC hDC = GetWindowDC(nullptr);
    Vector2i monitorSize{GetDeviceCaps(hDC, HORZSIZE), GetDeviceCaps(hDC, VERTSIZE)};
    SDL_DisplayMode mode;
    CORRADE_INTERNAL_ASSERT(SDL_GetDesktopDisplayMode(0, &mode) == 0);
    auto dpi = Vector2{Vector2i{mode.w, mode.h}*25.4f/Vector2{monitorSize}};
    const Vector2 dpiScaling{dpi/96.0f};
    Debug{verbose} << "Platform::Sdl2Application: physical DPI scaling" << dpiScaling;
    return dpiScaling;

    /* Not implemented otherwise */
    #else
    Debug{verbose} << "Platform::Sdl2Application: sorry, physical DPI scaling not implemented on this platform yet";
    return Vector2{1.0f};
    #endif
    #endif
}

void Sdl2Application::setWindowTitle(const std::string& title) {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_SetWindowTitle(_window, title.data());
    #else
    /* We don't have the _window because SDL_CreateWindow() doesn't exist in
       the SDL1/2 hybrid. But it's not used anyway, so pass nullptr there. */
    SDL_SetWindowTitle(nullptr, title.data());
    #endif
}

#if !defined(CORRADE_TARGET_EMSCRIPTEN) && SDL_MAJOR_VERSION*1000 + SDL_MINOR_VERSION*100 + SDL_PATCHLEVEL >= 2005
void Sdl2Application::setWindowIcon(const ImageView2D& image) {
    Uint32 format; /** @todo handle sRGB differently? */
    switch(image.format()) {
        case PixelFormat::RGB8Srgb:
        case PixelFormat::RGB8Unorm:
            format = SDL_PIXELFORMAT_RGB24;
            break;
        case PixelFormat::RGBA8Srgb:
        case PixelFormat::RGBA8Unorm:
            format = SDL_PIXELFORMAT_RGBA32;
            break;
        default:
            CORRADE_ASSERT_UNREACHABLE("Platform::Sdl2Application::setWindowIcon(): unexpected format" << image.format(), );
    }

    /* Images are loaded with origin at bottom left, flip it to top left.
       Fortunately SDL accepts negative stride, so we don't need to do an
       expensive flip ourselves. */
    Containers::StridedArrayView3D<const char> pixels = image.pixels().flipped<0>();
    SDL_Surface* icon = SDL_CreateRGBSurfaceWithFormatFrom(const_cast<void*>(pixels.data()) , image.size().x(), image.size().y(), 32, pixels.stride()[0], format);
    SDL_SetWindowIcon(_window, icon);
    SDL_FreeSurface(icon);
}
#endif

bool Sdl2Application::tryCreate(const Configuration& configuration) {
    #ifdef MAGNUM_TARGET_GL
    if(!(configuration.windowFlags() & Configuration::WindowFlag::Contextless))
        return tryCreate(configuration, GLConfiguration{});
    #endif

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Scale window based on DPI */
    _dpiScaling = dpiScaling(configuration);
    const Vector2i scaledWindowSize = configuration.size()*_dpiScaling;

    /* Create window */
    if(!(_window = SDL_CreateWindow(
        #ifndef CORRADE_TARGET_IOS
        configuration.title().data(),
        #else
        nullptr,
        #endif
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        scaledWindowSize.x(), scaledWindowSize.y(),
        SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_OPENGL|Uint32(configuration.windowFlags() & ~Configuration::WindowFlag::Contextless))))
    {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create window:" << SDL_GetError();
        return false;
    }

    /* Emscripten-specific initialization */
    #else
    /* Get CSS canvas size. This is used later to detect canvas resizes and
       fire viewport events, because Emscripten doesn't do that. Related info:
       https://github.com/kripken/emscripten/issues/1731 */
    /** @todo don't hardcode "#canvas" here, make it configurable from outside */
    {
        Vector2d canvasSize;
        /* Emscripten 1.38.27 changed to generic CSS selectors from element
           IDs depending on -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1
           being set (which we can't detect at compile time). Fortunately,
           using #canvas works the same way both in the previous versions and
           the current one. Unfortunately, this is also the only value that
           works the same way for both. Further details at
           https://github.com/emscripten-core/emscripten/pull/7977 */
        emscripten_get_element_css_size("#canvas", &canvasSize.x(), &canvasSize.y());
        _lastKnownCanvasSize = Vector2i{canvasSize};
    }

    /* By default Emscripten creates a 300x150 canvas. That's so freaking
       random I'm getting mad. Use the real (CSS pixels) canvas size instead,
       if the size is not hardcoded from the configuration. This is then
       multiplied by the DPI scaling. */
    Vector2i windowSize;
    if(!configuration.size().isZero()) {
        windowSize = configuration.size();
    } else {
        windowSize = _lastKnownCanvasSize;
        Debug{_verboseLog ? Debug::output() : nullptr} << "Platform::Sdl2Application::tryCreate(): autodetected canvas size" << windowSize;
    }
    _dpiScaling = dpiScaling(configuration);
    const Vector2i scaledWindowSize = windowSize*_dpiScaling;

    Uint32 flags = SDL_OPENGL|SDL_HWSURFACE|SDL_DOUBLEBUF;
    if(configuration.windowFlags() & Configuration::WindowFlag::Resizable) {
        _flags |= Flag::Resizable;
        /* Actually not sure if this makes any difference:
           https://github.com/kripken/emscripten/issues/1731 */
        flags |= SDL_RESIZABLE;
    }

    if(!(_surface = SDL_SetVideoMode(scaledWindowSize.x(), scaledWindowSize.y(), 24, flags))) {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create context:" << SDL_GetError();
        return false;
    }
    #endif

    return true;
}

#ifdef MAGNUM_TARGET_GL
bool Sdl2Application::tryCreate(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    CORRADE_ASSERT(_context->version() == GL::Version::None, "Platform::Sdl2Application::tryCreate(): context already created", false);

    /* Enable double buffering, set up buffer sizes */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, glConfiguration.colorBufferSize().r());
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, glConfiguration.colorBufferSize().g());
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, glConfiguration.colorBufferSize().b());
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, glConfiguration.colorBufferSize().a());
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, glConfiguration.depthBufferSize());
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, glConfiguration.stencilBufferSize());

    /* Multisampling */
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, glConfiguration.sampleCount() > 1 ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, glConfiguration.sampleCount());

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* sRGB */
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, glConfiguration.isSrgbCapable());
    #endif

    /** @todo Remove when Emscripten has proper SDL2 support */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Scale window based on DPI */
    _dpiScaling = dpiScaling(configuration);
    const Vector2i scaledWindowSize = configuration.size()*_dpiScaling;

    /* Request debug context if --magnum-gpu-validation is enabled */
    GLConfiguration::Flags glFlags = glConfiguration.flags();
    if(_context->internalFlags() & GL::Context::InternalFlag::GpuValidation)
        glFlags |= GLConfiguration::Flag::Debug;

    /* Set context version, if user-specified */
    if(glConfiguration.version() != GL::Version::None) {
        Int major, minor;
        std::tie(major, minor) = version(glConfiguration.version());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);

        #ifndef MAGNUM_TARGET_GLES
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, glConfiguration.version() >= GL::Version::GL310 ?
            SDL_GL_CONTEXT_PROFILE_CORE : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        #endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(glFlags));

    /* Request usable version otherwise */
    } else {
        #ifndef MAGNUM_TARGET_GLES
        /* First try to create core context. This is needed mainly on macOS and
           Mesa, as support for recent OpenGL versions isn't implemented in
           compatibility contexts (which are the default). At least GL 3.2 is
           needed on macOS, at least GL 3.1 is needed on Mesa. Bite the bullet
           and try 3.1 also elsewhere. */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        #ifdef CORRADE_TARGET_APPLE
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        #endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(glFlags));
        #else
        /* For ES the major context version is compile-time constant */
        #ifdef MAGNUM_TARGET_GLES3
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        #elif defined(MAGNUM_TARGET_GLES2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        #else
        #error unsupported OpenGL ES version
        #endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        #endif
    }

    /* Create window. Hide it by default so we don't have distracting window
       blinking in case we have to destroy it again right away */
    if(!(_window = SDL_CreateWindow(
        #ifndef CORRADE_TARGET_IOS
        configuration.title().data(),
        #else
        nullptr,
        #endif
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        scaledWindowSize.x(), scaledWindowSize.y(),
        SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN|SDL_WINDOW_ALLOW_HIGHDPI|Uint32(configuration.windowFlags()))))
    {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create window:" << SDL_GetError();
        return false;
    }

    /* Create context */
    _glContext = SDL_GL_CreateContext(_window);

    #ifndef MAGNUM_TARGET_GLES
    /* Fall back to (forward compatible) GL 2.1, if version is not
       user-specified and either core context creation fails or we are on
       binary NVidia/AMD drivers on Linux/Windows or Intel Windows drivers.
       Instead of creating forward-compatible context with highest available
       version, they force the version to the one specified, which is
       completely useless behavior. */
    #ifndef CORRADE_TARGET_APPLE
    constexpr static const char nvidiaVendorString[] = "NVIDIA Corporation";
    #ifdef CORRADE_TARGET_WINDOWS
    constexpr static const char intelVendorString[] = "Intel";
    #endif
    constexpr static const char amdVendorString[] = "ATI Technologies Inc.";
    const char* vendorString;
    #endif
    if(glConfiguration.version() == GL::Version::None && (!_glContext
        #ifndef CORRADE_TARGET_APPLE
        /* If context creation fails *really bad*, glGetString() may actually
           return nullptr. Check for that to avoid crashes deep inside
           strncmp(). Sorry about the UGLY code, HOPEFULLY THERE WON'T BE MORE
           WORKAROUNDS */
        || (vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
        vendorString && (std::strncmp(vendorString, nvidiaVendorString, sizeof(nvidiaVendorString)) == 0 ||
         #ifdef CORRADE_TARGET_WINDOWS
         std::strncmp(vendorString, intelVendorString, sizeof(intelVendorString)) == 0 ||
         #endif
         std::strncmp(vendorString, amdVendorString, sizeof(amdVendorString)) == 0)
         && !_context->isDriverWorkaroundDisabled("no-forward-compatible-core-context"))
        #endif
    )) {
        /* Don't print any warning when doing the workaround, because the bug
           will be there probably forever */
        if(!_glContext) Warning()
            << "Platform::Sdl2Application::tryCreate(): cannot create core context:"
            << SDL_GetError() << "(falling back to compatibility context)";
        else SDL_GL_DeleteContext(_glContext);

        SDL_DestroyWindow(_window);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        /* Discard the ForwardCompatible flag for the fallback. Having it set
           makes the fallback context creation fail on Mesa's Zink (which is
           just 2.1) and I assume on others as well. */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(glFlags) & int(~GLConfiguration::Flag::ForwardCompatible));

        if(!(_window = SDL_CreateWindow(configuration.title().data(),
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            scaledWindowSize.x(), scaledWindowSize.y(),
            SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN|SDL_WINDOW_ALLOW_HIGHDPI|Uint32(configuration.windowFlags()&~Configuration::WindowFlag::Contextless))))
        {
            Error() << "Platform::Sdl2Application::tryCreate(): cannot create window:" << SDL_GetError();
            return false;
        }

        /* Create compatibility context */
        _glContext = SDL_GL_CreateContext(_window);
    }
    #endif

    /* Cannot create context (or fallback compatibility context on desktop) */
    if(!_glContext) {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create context:" << SDL_GetError();
        SDL_DestroyWindow(_window);
        _window = nullptr;
        return false;
    }

    #ifdef CORRADE_TARGET_IOS
    /* iOS has zero initial GL_VIEWPORT size, get drawable size and put it back
       in so all other code can assume that the viewport is set to sane values.
       Fortunately on iOS we also don't have to load any function pointers so
       it's safe to do the glViewport() call as it is linked statically. */
    {
        const Vector2i viewport = framebufferSize();
        glViewport(0, 0, viewport.x(), viewport.y());
    }
    #endif

    /* Emscripten-specific initialization */
    #else
    /* Get CSS canvas size. This is used later to detect canvas resizes and
       fire viewport events, because Emscripten doesn't do that. Related info:
       https://github.com/kripken/emscripten/issues/1731 */
    /** @todo don't hardcode "#canvas" here, make it configurable from outside */
    {
        Vector2d canvasSize;
        /* Emscripten 1.38.27 changed to generic CSS selectors from element
           IDs depending on -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1
           being set (which we can't detect at compile time). Fortunately,
           using #canvas works the same way both in the previous versions and
           the current one. Unfortunately, this is also the only value that
           works the same way for both. Further details at
           https://github.com/emscripten-core/emscripten/pull/7977 */
        emscripten_get_element_css_size("#canvas", &canvasSize.x(), &canvasSize.y());
        _lastKnownCanvasSize = Vector2i{canvasSize};
    }

    /* By default Emscripten creates a 300x150 canvas. That's so freaking
       random I'm getting mad. Use the real (CSS pixels) canvas size instead,
       if the size is not hardcoded from the configuration. This is then
       multiplied by the DPI scaling. */
    Vector2i windowSize;
    if(!configuration.size().isZero()) {
        windowSize = configuration.size();
    } else {
        windowSize = _lastKnownCanvasSize;
        Debug{_verboseLog ? Debug::output() : nullptr} << "Platform::Sdl2Application::tryCreate(): autodetected canvas size" << windowSize;
    }
    _dpiScaling = dpiScaling(configuration);
    const Vector2i scaledWindowSize = windowSize*_dpiScaling;

    Uint32 flags = SDL_OPENGL|SDL_HWSURFACE|SDL_DOUBLEBUF;
    if(configuration.windowFlags() & Configuration::WindowFlag::Resizable) {
        _flags |= Flag::Resizable;
        /* Actually not sure if this makes any difference:
           https://github.com/kripken/emscripten/issues/1731 */
        flags |= SDL_RESIZABLE;
    }

    if(!(_surface = SDL_SetVideoMode(scaledWindowSize.x(), scaledWindowSize.y(), 24, flags))) {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create context:" << SDL_GetError();
        return false;
    }
    #endif

    /* Destroy everything also when the Magnum context creation fails */
    if(!_context->tryCreate()) {
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        SDL_GL_DeleteContext(_glContext);
        SDL_DestroyWindow(_window);
        _window = nullptr;
        #else
        SDL_FreeSurface(_surface);
        #endif
        return false;
    }

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Show the window once we are sure that everything is okay */
    if(!(configuration.windowFlags() & Configuration::WindowFlag::Hidden))
        SDL_ShowWindow(_window);
    #endif

    /* Return true if the initialization succeeds */
    return true;
}
#endif

Vector2i Sdl2Application::windowSize() const {
    Vector2i size;
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::windowSize(): no window opened", {});
    SDL_GetWindowSize(_window, &size.x(), &size.y());
    #else
    CORRADE_ASSERT(_surface, "Platform::Sdl2Application::windowSize(): no window opened", {});
    emscripten_get_canvas_element_size("#canvas", &size.x(), &size.y());
    #endif
    return size;
}

#ifndef CORRADE_TARGET_EMSCRIPTEN
void Sdl2Application::setWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::setWindowSize(): no window opened", );

    const Vector2i newSize = _dpiScaling*size;
    SDL_SetWindowSize(_window, newSize.x(), newSize.y());
}

void Sdl2Application::setMinWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::setMinWindowSize(): no window opened", );

    const Vector2i newSize = _dpiScaling*size;
    SDL_SetWindowMinimumSize(_window, newSize.x(), newSize.y());
}

void Sdl2Application::setMaxWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::setMaxWindowSize(): no window opened", );

    const Vector2i newSize = _dpiScaling*size;
    SDL_SetWindowMaximumSize(_window, newSize.x(), newSize.y());
}
#endif

#ifdef MAGNUM_TARGET_GL
Vector2i Sdl2Application::framebufferSize() const {
    Vector2i size;
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::framebufferSize(): no window opened", {});
    SDL_GL_GetDrawableSize(_window, &size.x(), &size.y());
    #else
    CORRADE_ASSERT(_surface, "Platform::Sdl2Application::framebufferSize(): no window opened", {});
    emscripten_get_canvas_element_size("#canvas", &size.x(), &size.y());
    #endif
    return size;
}
#endif

#ifdef CORRADE_TARGET_EMSCRIPTEN
void Sdl2Application::setContainerCssClass(const std::string& cssClass) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
    EM_ASM_({
        /* Handle also the classic #container for backwards compatibility. We
           also need to preserve the mn-container otherwise next time we'd have
           no way to look for it anymore. */
        (Module['canvas'].closest('.mn-container') ||
         document.getElementById('container')).className = (['mn-container', AsciiToString($0)]).join(' ');
    }, cssClass.data());
    #pragma GCC diagnostic pop
}
#endif

void Sdl2Application::swapBuffers() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_GL_SwapWindow(_window);
    #else
    SDL_Flip(_surface);
    #endif
}

Int Sdl2Application::swapInterval() const {
    return SDL_GL_GetSwapInterval();
}

bool Sdl2Application::setSwapInterval(const Int interval) {
    if(SDL_GL_SetSwapInterval(interval) == -1) {
        Error() << "Platform::Sdl2Application::setSwapInterval(): cannot set swap interval:" << SDL_GetError();
        _flags &= ~Flag::VSyncEnabled;
        return false;
    }

    if(SDL_GL_GetSwapInterval() != interval) {
        Error() << "Platform::Sdl2Application::setSwapInterval(): swap interval setting ignored by the driver";
        _flags &= ~Flag::VSyncEnabled;
        return false;
    }

    if(interval) _flags |= Flag::VSyncEnabled;
    else _flags &= ~Flag::VSyncEnabled;
    return true;
}

void Sdl2Application::redraw() { _flags |= Flag::Redraw; }

Sdl2Application::~Sdl2Application() {
    /* SDL_DestroyWindow(_window) crashes on windows when _window is nullptr
       (it doesn't seem to crash on Linux). Because this seems to be yet
       another pointless platform difference, to be safe do the same check with
       all. */

    #ifdef MAGNUM_TARGET_GL
    _context.reset();

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    if(_glContext) SDL_GL_DeleteContext(_glContext);
    #else
    if(_surface) SDL_FreeSurface(_surface);
    #endif
    #endif

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    for(auto& cursor: _cursors)
        SDL_FreeCursor(cursor);
    #endif

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    if(_window) SDL_DestroyWindow(_window);
    #endif
    SDL_Quit();
}

int Sdl2Application::exec() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    while(mainLoopIteration()) {}
    #else
    emscripten_set_main_loop_arg([](void* arg) {
        static_cast<Sdl2Application*>(arg)->mainLoopIteration();
    }, this, 0, true);
    #endif
    return _exitCode;
}

void Sdl2Application::exit(const int exitCode) {
    /* On Emscripten this flag is used only to indicate a desire to exit from
       mainLoopIteration() */
    _flags |= Flag::Exit;
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    emscripten_cancel_main_loop();
    #endif
    _exitCode = exitCode;
}

bool Sdl2Application::mainLoopIteration() {
    /* If exit was requested directly in the constructor, exit immediately
       without calling anything else */
    if(_flags & Flag::Exit) return false;

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::mainLoopIteration(): no window opened", {});
    #else
    CORRADE_ASSERT(_surface, "Platform::Sdl2Application::mainLoopIteration(): no window opened", {});
    #endif

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    const UnsignedInt timeBefore = _minimalLoopPeriod ? SDL_GetTicks() : 0;
    #endif

    #ifdef CORRADE_TARGET_EMSCRIPTEN
    /* The resize event is not fired on window resize, so poll for the canvas
       size here. But only if the window was requested to be resizable, to
       avoid resizing the canvas when the user doesn't want that. Related
       issue: https://github.com/kripken/emscripten/issues/1731 */
    if(_flags & Flag::Resizable) {
        Vector2d canvasSize;
        /* Emscripten 1.38.27 changed to generic CSS selectors from element
           IDs depending on -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1
           being set (which we can't detect at compile time). See above for the
           reason why we hardcode #canvas here. */
        emscripten_get_element_css_size("#canvas", &canvasSize.x(), &canvasSize.y());

        const Vector2i canvasSizei{canvasSize};
        if(canvasSizei != _lastKnownCanvasSize) {
            _lastKnownCanvasSize = canvasSizei;
            const Vector2i size = _dpiScaling*canvasSizei;
            emscripten_set_canvas_element_size("#canvas", size.x(), size.y());
            ViewportEvent e{
                #ifdef MAGNUM_TARGET_GL
                size,
                #endif
                size, _dpiScaling};
            viewportEvent(e);
            _flags |= Flag::Redraw;
        }
    }
    #endif

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_WINDOWEVENT:
                switch(event.window.event) {
                    /* Not using SDL_WINDOWEVENT_RESIZED, because that doens't
                       get fired when the window is resized programmatically
                       (such as through setMaxWindowSize()) */
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        #ifdef CORRADE_TARGET_EMSCRIPTEN
                        /* If anybody sees this assert, then emscripten finally
                           implemented resize events. Praise them for that.
                           https://github.com/kripken/emscripten/issues/1731 */
                        CORRADE_INTERNAL_ASSERT_UNREACHABLE();
                        #else
                        /* {event.window.data1, event.window.data2} seems to be
                           framebuffer size and not window size on macOS, which
                           is weird. Query the values directly instead to be
                           really sure. */
                        ViewportEvent e{event, windowSize(),
                            #ifdef MAGNUM_TARGET_GL
                            framebufferSize(),
                            #endif
                            _dpiScaling};
                        /** @todo handle also WM_DPICHANGED events when a window is moved between displays with different DPI */
                        viewportEvent(e);
                        _flags |= Flag::Redraw;
                        #endif
                    } break;
                    /* Direct everything that wasn't exposed via a callback to
                       anyEvent(), so users can implement event handling for
                       things not present in the Application APIs */
                    case SDL_WINDOWEVENT_EXPOSED:
                        _flags |= Flag::Redraw;
                        if(!(_flags & Flag::NoAnyEvent)) anyEvent(event);
                        break;
                    default:
                        if(!(_flags & Flag::NoAnyEvent)) anyEvent(event);
                } break;

            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                KeyEvent e{event, static_cast<KeyEvent::Key>(event.key.keysym.sym), fixedModifiers(event.key.keysym.mod), event.key.repeat != 0};
                event.type == SDL_KEYDOWN ? keyPressEvent(e) : keyReleaseEvent(e);
            } break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                MouseEvent e{event, static_cast<MouseEvent::Button>(event.button.button), {event.button.x, event.button.y}
                    #ifndef CORRADE_TARGET_EMSCRIPTEN
                    , event.button.clicks
                    #endif
                    };
                event.type == SDL_MOUSEBUTTONDOWN ? mousePressEvent(e) : mouseReleaseEvent(e);
            } break;

            case SDL_MOUSEWHEEL: {
                MouseScrollEvent e{event, {Float(event.wheel.x), Float(event.wheel.y)}};
                mouseScrollEvent(e);
            } break;

            case SDL_MOUSEMOTION: {
                MouseMoveEvent e{event, {event.motion.x, event.motion.y}, {event.motion.xrel, event.motion.yrel}, static_cast<MouseMoveEvent::Button>(event.motion.state)};
                mouseMoveEvent(e);
                break;
            }

            case SDL_MULTIGESTURE: {
                MultiGestureEvent e{event, {event.mgesture.x, event.mgesture.y}, event.mgesture.dTheta, event.mgesture.dDist, event.mgesture.numFingers};
                multiGestureEvent(e);
                break;
            }

            case SDL_TEXTINPUT: {
                TextInputEvent e{event, {event.text.text, std::strlen(event.text.text)}};
                textInputEvent(e);
            } break;

            case SDL_TEXTEDITING: {
                TextEditingEvent e{event, {event.edit.text, std::strlen(event.text.text)}, event.edit.start, event.edit.length};
                textEditingEvent(e);
            } break;

            case SDL_QUIT: {
                ExitEvent e{event};
                exitEvent(e);
                if(e.isAccepted()) {
                    /* On Emscripten this flag is used only to indicate a
                       desire to exit from mainLoopIteration() */
                    _flags |= Flag::Exit;
                    #ifdef CORRADE_TARGET_EMSCRIPTEN
                    emscripten_cancel_main_loop();
                    #endif
                    return !(_flags & Flag::Exit);
                }
            } break;

            /* Direct everything else to anyEvent(), so users can implement
               event handling for things not present in the Application APIs */
            default: if(!(_flags & Flag::NoAnyEvent)) anyEvent(event);
        }
    }

    /* Tick event */
    if(!(_flags & Flag::NoTickEvent)) tickEvent();

    /* Draw event */
    if(_flags & Flag::Redraw) {
        _flags &= ~Flag::Redraw;
        drawEvent();

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /* If VSync is not enabled, delay to prevent CPU hogging (if set) */
        if(!(_flags & Flag::VSyncEnabled) && _minimalLoopPeriod) {
            const UnsignedInt loopTime = SDL_GetTicks() - timeBefore;
            if(loopTime < _minimalLoopPeriod)
                SDL_Delay(_minimalLoopPeriod - loopTime);
        }
        #endif

        return !(_flags & Flag::Exit);
    }

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* If not drawing anything, delay to prevent CPU hogging (if set) */
    if(_minimalLoopPeriod) {
        const UnsignedInt loopTime = SDL_GetTicks() - timeBefore;
        if(loopTime < _minimalLoopPeriod)
            SDL_Delay(_minimalLoopPeriod - loopTime);
    }

    /* Then, if the tick event doesn't need to be called periodically, wait
       indefinitely for next input event */
    if(_flags & Flag::NoTickEvent) SDL_WaitEvent(nullptr);
    #endif
    return !(_flags & Flag::Exit);
}

namespace {

#ifndef CORRADE_TARGET_EMSCRIPTEN
constexpr SDL_SystemCursor CursorMap[] {
    SDL_SYSTEM_CURSOR_ARROW,
    SDL_SYSTEM_CURSOR_IBEAM,
    SDL_SYSTEM_CURSOR_WAIT,
    SDL_SYSTEM_CURSOR_CROSSHAIR,
    SDL_SYSTEM_CURSOR_WAITARROW,
    SDL_SYSTEM_CURSOR_SIZENWSE,
    SDL_SYSTEM_CURSOR_SIZENESW,
    SDL_SYSTEM_CURSOR_SIZEWE,
    SDL_SYSTEM_CURSOR_SIZENS,
    SDL_SYSTEM_CURSOR_SIZEALL,
    SDL_SYSTEM_CURSOR_NO,
    SDL_SYSTEM_CURSOR_HAND
};
#else
constexpr const char* CursorMap[] {
    "default",
    "text",
    "wait",
    "crosshair",
    "progress",
    "nwse-resize",
    "nesw-resize",
    "ew-resize",
    "ns-resize",
    "move",
    "not-allowed",
    "pointer",
    "none"
    /* Hidden & locked not supported yet */
};
#endif

}

void Sdl2Application::setCursor(Cursor cursor) {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_INTERNAL_ASSERT(UnsignedInt(cursor) < Containers::arraySize(_cursors));

    if(cursor == Cursor::Hidden) {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_SetWindowGrab(_window, SDL_FALSE);
        SDL_SetRelativeMouseMode(SDL_FALSE);
        return;
    } else if(cursor == Cursor::HiddenLocked) {
        SDL_SetWindowGrab(_window, SDL_TRUE);
        SDL_SetRelativeMouseMode(SDL_TRUE);
        return;
    } else {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetWindowGrab(_window, SDL_FALSE);
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    if(!_cursors[UnsignedInt(cursor)])
        _cursors[UnsignedInt(cursor)] = SDL_CreateSystemCursor(CursorMap[UnsignedInt(cursor)]);

    SDL_SetCursor(_cursors[UnsignedInt(cursor)]);
    #else
    _cursor = cursor;
    CORRADE_INTERNAL_ASSERT(UnsignedInt(cursor) < Containers::arraySize(CursorMap));
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
    EM_ASM_({Module['canvas'].style.cursor = AsciiToString($0);}, CursorMap[UnsignedInt(cursor)]);
    #pragma GCC diagnostic pop
    #endif
}

Sdl2Application::Cursor Sdl2Application::cursor() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    if(SDL_GetRelativeMouseMode())
        return Cursor::HiddenLocked;
    else if(!SDL_ShowCursor(SDL_QUERY))
        return Cursor::Hidden;

    SDL_Cursor* cursor = SDL_GetCursor();

    if(cursor) for(UnsignedInt i = 0; i < sizeof(_cursors); i++)
        if(_cursors[i] == cursor) return Cursor(i);

    return Cursor::Arrow;
    #else
    return _cursor;
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
void Sdl2Application::setMouseLocked(bool enabled) {
    /** @todo Implement this in Emscripten */
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_SetWindowGrab(_window, enabled ? SDL_TRUE : SDL_FALSE);
    SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
    #else
    CORRADE_ASSERT_UNREACHABLE("Sdl2Application::setMouseLocked(): not implemented", );
    static_cast<void>(enabled);
    #endif
}
#endif

bool Sdl2Application::isTextInputActive() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    return SDL_IsTextInputActive();
    #else
    return !!(_flags & Flag::TextInputActive);
    #endif
}

void Sdl2Application::startTextInput() {
    SDL_StartTextInput();
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    _flags |= Flag::TextInputActive;
    #endif
}

void Sdl2Application::stopTextInput() {
    SDL_StopTextInput();
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    _flags &= ~Flag::TextInputActive;
    #endif
}

void Sdl2Application::setTextInputRect(const Range2Di& rect) {
    SDL_Rect r{rect.min().x(), rect.min().y(), rect.sizeX(), rect.sizeY()};
    SDL_SetTextInputRect(&r);
}

void Sdl2Application::exitEvent(ExitEvent& event) {
    event.setAccepted();
}

void Sdl2Application::tickEvent() {
    /* If this got called, the tick event is not implemented by user and thus
       we don't need to call it ever again */
    _flags |= Flag::NoTickEvent;
}

void Sdl2Application::anyEvent(SDL_Event&) {
    /* If this got called, the any event is not implemented by user and thus
       we don't need to call it ever again */
    _flags |= Flag::NoAnyEvent;
}

void Sdl2Application::viewportEvent(ViewportEvent& event) {
    static_cast<void>(event);
}

void Sdl2Application::keyPressEvent(KeyEvent&) {}
void Sdl2Application::keyReleaseEvent(KeyEvent&) {}
void Sdl2Application::mousePressEvent(MouseEvent&) {}
void Sdl2Application::mouseReleaseEvent(MouseEvent&) {}
void Sdl2Application::mouseMoveEvent(MouseMoveEvent&) {}
void Sdl2Application::mouseScrollEvent(MouseScrollEvent&) {}
void Sdl2Application::multiGestureEvent(MultiGestureEvent&) {}
void Sdl2Application::textInputEvent(TextInputEvent&) {}
void Sdl2Application::textEditingEvent(TextEditingEvent&) {}

#ifdef MAGNUM_TARGET_GL
Sdl2Application::GLConfiguration::GLConfiguration():
    _colorBufferSize{8, 8, 8, 8}, _depthBufferSize{24}, _stencilBufferSize{0},
    _sampleCount(0)
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    , _version(GL::Version::None),
    #ifndef MAGNUM_TARGET_GLES
    _flags{Flag::ForwardCompatible},
    #else
    _flags{},
    #endif
    _srgbCapable{false}
    #endif
    {}

Sdl2Application::GLConfiguration::~GLConfiguration() = default;
#endif

Sdl2Application::Configuration::Configuration():
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
    _title("Magnum SDL2 Application"),
    #endif
    #if !defined(CORRADE_TARGET_IOS) && !defined(CORRADE_TARGET_EMSCRIPTEN)
    _size{800, 600},
    #else
    _size{}, /* SDL2 detects someting for us */
    #endif
    _dpiScalingPolicy{DpiScalingPolicy::Default} {}

Sdl2Application::Configuration::~Configuration() = default;

std::string Sdl2Application::KeyEvent::keyName(const Key key) {
    return SDL_GetKeyName(SDL_Keycode(key));
}

std::string Sdl2Application::KeyEvent::keyName() const {
    return keyName(_key);
}

Sdl2Application::InputEvent::Modifiers Sdl2Application::MouseEvent::modifiers() {
    if(_modifiers) return *_modifiers;
    return *(_modifiers = fixedModifiers(Uint16(SDL_GetModState())));
}

Sdl2Application::InputEvent::Modifiers Sdl2Application::MouseMoveEvent::modifiers() {
    if(_modifiers) return *_modifiers;
    return *(_modifiers = fixedModifiers(Uint16(SDL_GetModState())));
}

Vector2i Sdl2Application::MouseScrollEvent::position() {
    if(_position) return *_position;
    _position = Vector2i{};
    SDL_GetMouseState(&_position->x(), &_position->y());
    return *_position;
}

Sdl2Application::InputEvent::Modifiers Sdl2Application::MouseScrollEvent::modifiers() {
    if(_modifiers) return *_modifiers;
    return *(_modifiers = fixedModifiers(Uint16(SDL_GetModState())));
}

template class BasicScreen<Sdl2Application>;
template class BasicScreenedApplication<Sdl2Application>;

}}
