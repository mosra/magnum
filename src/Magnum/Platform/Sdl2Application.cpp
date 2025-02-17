/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2019 Marco Melorio <m.melorio@icloud.com>
    Copyright © 2022 Pablo Escobar <mail@rvrs.in>

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
#ifdef CORRADE_TARGET_EMSCRIPTEN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Arguments.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Range.h"
#ifndef CORRADE_TARGET_EMSCRIPTEN
#include "Magnum/Math/Time.h"
#endif
#include "Magnum/Platform/ScreenedApplication.hpp"
#include "Magnum/Platform/Implementation/DpiScaling.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Version.h"
#endif

#if defined(CORRADE_TARGET_WINDOWS) && !defined(CORRADE_TARGET_WINDOWS_RT)
/* For physical DPI scaling */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#ifdef CORRADE_TARGET_EMSCRIPTEN
/* Implemented in Platform.js.in */
extern "C" {
    void magnumPlatformSetContainerCssClass(const char* string, std::size_t size);
    void magnumPlatformSetCursor(const char* string, std::size_t size);
}
#endif

namespace Magnum { namespace Platform {

using namespace Containers::Literals;
using namespace Math::Literals;

namespace {

/*
 * Fix up the modifiers -- we want >= operator to work properly on Shift,
 * Ctrl, Alt, but SDL generates different event for left / right keys, thus
 * (modifiers >= Shift) would pass only if both left and right were pressed,
 * which is usually not what the developers wants.
 */
Sdl2Application::Modifiers fixedModifiers(Uint16 mod) {
    Sdl2Application::Modifiers modifiers{mod};
    if(modifiers & Sdl2Application::Modifier::Shift)
        modifiers |= Sdl2Application::Modifier::Shift;
    if(modifiers & Sdl2Application::Modifier::Ctrl)
        modifiers |= Sdl2Application::Modifier::Ctrl;
    if(modifiers & Sdl2Application::Modifier::Alt)
        modifiers |= Sdl2Application::Modifier::Alt;
    if(modifiers & Sdl2Application::Modifier::Super)
        modifiers |= Sdl2Application::Modifier::Super;
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

Containers::StringView Sdl2Application::keyName(const Key key) {
    return SDL_GetKeyName(SDL_Keycode(key));
}

#ifndef CORRADE_TARGET_EMSCRIPTEN
Containers::StringView Sdl2Application::scanCodeName(const UnsignedInt scanCode) {
    return SDL_GetScancodeName(SDL_Scancode(scanCode));
}
#endif

/* https://github.com/emscripten-core/emscripten/pull/18060 */
#if !defined(CORRADE_TARGET_EMSCRIPTEN) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 30125
Containers::Optional<UnsignedInt> Sdl2Application::keyToScanCode(const Key key) {
    static_assert(SDL_SCANCODE_UNKNOWN == 0, "assumed SDL_SCANCODE_UNKNOWN to be 0");
    if(const SDL_Scancode scanCode = SDL_GetScancodeFromKey(SDL_Keycode(key)))
        return UnsignedInt(scanCode);
    return {};
}
#endif

#ifndef CORRADE_TARGET_EMSCRIPTEN
Containers::Optional<Sdl2Application::Key> Sdl2Application::scanCodeToKey(const UnsignedInt scanCode) {
    if(const SDL_Keycode key = SDL_GetKeyFromScancode(SDL_Scancode(scanCode)))
        return Key(key);
    return {};
}
#endif

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
    _flags{Flag::Redraw}
{
    Utility::Arguments args{Implementation::windowScalingArguments()};
    #ifdef MAGNUM_TARGET_GL
    _context.emplace(NoCreate, args, arguments.argc, arguments.argv);
    #else
    /** @todo this is duplicated here, in GlfwApplication and in
        EmscriptenApplication, figure out a nice non-duplicated way to handle
        this */
    args.addOption("log", "default").setHelp("log", "console logging", "default|quiet|verbose")
        .setFromEnvironment("log")
        .parse(arguments.argc, arguments.argv);
    #endif

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Disable translation of touch events to mouse events and vice versa as
       that's a very poor way of freeing users from having to implement
       separate event handling for mouse and touch (and, in SDL3, pen). Instead
       the Sdl2Application is providing a PointerEvent abstracting all of
       those, so no event translation needs to take place anymore.

       Though, just for historical records, what is quite funny / strange about
       the SDL's translation, is that when the touch goes out of the window,
       translated mouse events get clamped to the window size and thus also not
       even being reported if the clamped value doesn't change. On the other
       hand, with a regular mouse event, if a drag goes out of the window, it's
       still reported correctly, with the coordinates being either larger than
       the window size or negative. No idea why the SDL touch->mouse emulation
       doesn't do this -- maybe because having a touchscreen device with a
       window manager is still relatively rare so nobody reported that? Heh.

       These enums are not exposed in the minimal Emscripten SDL implementation
       which in turn means touch support there isn't implemented, because I
       don't want to filter duplicate events by hand. Use EmscriptenApplication
       instead, please. */
    /* Added in 2.0.6, before it was apparently impossible to turn off the
       event translation altogether. I could also make the touch available only
       on 2.0.6+, but 2.0.6 is from 2017 and I don't think it makes sense to
       bother with support for older versions. Ubuntu 18.04 has 2.0.8.
        https://github.com/libsdl-org/SDL/commit/56cab6d45280fbb4b645083eceeaa8f474c0aac3 */
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    /* Added in 2.0.10, before mouse events don't generate touch events
        https://github.com/libsdl-org/SDL/commit/e41576188d17fd09c95777d665f6c4532574f8ac */
    #ifdef SDL_HINT_MOUSE_TOUCH_EVENTS
    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");
    #endif
    #endif
    /* Available since 2.0.4, disables interception of SIGINT and SIGTERM so
       it's possible to Ctrl-C the application even if exitEvent() doesn't set
       event.setAccepted(). */
    #ifdef SDL_HINT_NO_SIGNAL_HANDLERS
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
    #endif
    /* Available since 2.0.6, uses dedicated OpenGL ES drivers if EGL is used,
       and desktop GLES context otherwise. */
    #if defined(MAGNUM_TARGET_GLES) && defined(MAGNUM_TARGET_EGL) && defined(SDL_HINT_OPENGL_ES_DRIVER)
    SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
    #endif
    /* Available since 2.0.8, disables compositor bypass on X11, which causes
       flickering on KWin as the compositor gets shut down on every startup */
    #ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    #endif
    /* By default, SDL behaves like if it was playing a video or whatever,
       preventing the computer from turning off the screen or going to sleep.
       While it sorta makes sense for games, it's useless and annoying for
       regular apps. Together with the compositor disabling those two are the
       most stupid defaults. */
    #ifdef SDL_HINT_VIDEO_ALLOW_SCREENSAVER
    SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");
    #endif
    /* Available since 2.0.12, use EGL if desired */
    #if defined(MAGNUM_TARGET_EGL) && defined(SDL_HINT_VIDEO_X11_FORCE_EGL)
    SDL_SetHint(SDL_HINT_VIDEO_X11_FORCE_EGL, "1");
    #endif

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error() << "Cannot initialize SDL:" << SDL_GetError();
        std::exit(1);
    }

    /* Save command-line arguments */
    if(args.value("log") == "verbose") _verboseLog = true;
    const Containers::StringView dpiScaling = args.value<Containers::StringView>("dpi-scaling");
    if(dpiScaling == "default"_s)
        _commandLineDpiScalingPolicy = Implementation::Sdl2DpiScalingPolicy::Default;
    #ifdef CORRADE_TARGET_APPLE
    else if(dpiScaling == "framebuffer"_s)
        _commandLineDpiScalingPolicy = Implementation::Sdl2DpiScalingPolicy::Framebuffer;
    #endif
    #ifndef CORRADE_TARGET_APPLE
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
    else if(dpiScaling == "virtual"_s)
        _commandLineDpiScalingPolicy = Implementation::Sdl2DpiScalingPolicy::Virtual;
    #endif
    else if(dpiScaling == "physical"_s)
        _commandLineDpiScalingPolicy = Implementation::Sdl2DpiScalingPolicy::Physical;
    #endif
    else if(dpiScaling.containsAny(" \t\n"_s))
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
    /* Print a helpful warning in case some extra steps are needed for HiDPI
       support */
    #ifdef CORRADE_TARGET_APPLE
    if(!Implementation::isAppleBundleHiDpiEnabled() && !(_flags & Flag::HiDpiWarningPrinted)) {
        Warning{} << "Platform::Sdl2Application: warning: the executable is not a HiDPI-enabled app bundle";
        _flags |= Flag::HiDpiWarningPrinted;
    }
    #elif defined(CORRADE_TARGET_WINDOWS)
    /* Handled in dpiScalingInternal(), warning printed only when using virtual
       DPI scaling (and thus not setting the HiDpiWarningPrinted flag) */
    #endif

    return dpiScalingInternal(configuration.dpiScalingPolicy(), configuration.dpiScaling());
}

Vector2 Sdl2Application::dpiScalingInternal(const Implementation::Sdl2DpiScalingPolicy configurationDpiScalingPolicy, const Vector2& configurationDpiScaling) const {
    std::ostream* verbose = _verboseLog ? Debug::output() : nullptr;

    /* Use values from the configuration only if not overridden on command line
       to something non-default. In any case explicit scaling has a precedence
       before the policy. */
    #ifndef CORRADE_TARGET_APPLE
    Implementation::Sdl2DpiScalingPolicy dpiScalingPolicy{};
    #endif
    if(!_commandLineDpiScaling.isZero()) {
        Debug{verbose} << "Platform::Sdl2Application: user-defined DPI scaling" << _commandLineDpiScaling;
        return _commandLineDpiScaling;
    } else if(_commandLineDpiScalingPolicy != Implementation::Sdl2DpiScalingPolicy::Default) {
        #ifndef CORRADE_TARGET_APPLE
        dpiScalingPolicy = _commandLineDpiScalingPolicy;
        #endif
    } else if(!configurationDpiScaling.isZero()) {
        Debug{verbose} << "Platform::Sdl2Application: app-defined DPI scaling" << configurationDpiScaling;
        return configurationDpiScaling;
    } else {
        #ifndef CORRADE_TARGET_APPLE
        dpiScalingPolicy = configurationDpiScalingPolicy;
        #else
        static_cast<void>(configurationDpiScalingPolicy);
        #endif
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
       physical value from X11. */
    #elif defined(CORRADE_TARGET_UNIX)
    Vector2 dpi;
    if(SDL_GetDisplayDPI(0, nullptr, &dpi.x(), &dpi.y()) == 0) {
        const Vector2 dpiScaling{dpi/96.0f};
        Debug{verbose} << "Platform::Sdl2Application: physical DPI scaling" << dpiScaling;
        return dpiScaling;
    }

    Warning{} << "Platform::Sdl2Application: can't get physical display DPI, falling back to no scaling:" << SDL_GetError();
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

void Sdl2Application::setWindowTitle(const Containers::StringView title) {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    SDL_SetWindowTitle(_window,
        Containers::String::nullTerminatedGlobalView(title).data());
    #else
    /* We don't have the _window because SDL_CreateWindow() doesn't exist in
       the SDL1/2 hybrid. But it's not used anyway, so pass nullptr there. */
    SDL_SetWindowTitle(nullptr,
        Containers::String::nullTerminatedGlobalView(title).data());
    #endif
}

#ifndef CORRADE_TARGET_EMSCRIPTEN
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

    /* Images are loaded with origin at bottom left, flip it to top left. SDL
       only accepted a negative stride until version 2.23.1 and commit
        https://github.com/libsdl-org/SDL/commit/535fdc3adcdc08a193ab0d45540014fd536cf251
       so we need to manually flip the image now */
    /** @todo take ImageFlag::YUp into account once it exists */
    Image2D flippedImage{PixelStorage{}.setAlignment(1), image.format(), image.size(), Containers::Array<char>{NoInit, std::size_t(image.size().product()*image.pixelSize())}};
    const Containers::StridedArrayView3D<char> flippedPixels = flippedImage.pixels();
    Utility::copy(image.pixels().flipped<0>(), flippedPixels);

    SDL_Surface* const icon = SDL_CreateRGBSurfaceWithFormatFrom(const_cast<void*>(flippedPixels.data()) , flippedImage.size().x(), flippedImage.size().y(), 32, flippedPixels.stride()[0], format);
    CORRADE_INTERNAL_ASSERT(icon);

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
    /* Save DPI scaling values from configuration for future use, scale window
       based on those */
    _configurationDpiScalingPolicy = configuration.dpiScalingPolicy();
    _configurationDpiScaling = configuration.dpiScaling();
    const Vector2i scaledWindowSize = configuration.size()*dpiScaling(configuration);

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
        /* Because hardcoding canvas size for WebGL is usually a wrong thing to
           do, notify about that in the verbose output */
        Debug{_verboseLog ? Debug::output() : nullptr} << "Platform::Sdl2Application::tryCreate(): hardcoded canvas size" << windowSize;
    } else {
        windowSize = _lastKnownCanvasSize;
        Debug{_verboseLog ? Debug::output() : nullptr} << "Platform::Sdl2Application::tryCreate(): autodetected canvas size" << windowSize;
    }
    /* Save DPI scaling values from configuration for future use, scale window
       based on those */
    _configurationDpiScalingPolicy = configuration.dpiScalingPolicy();
    _configurationDpiScaling = configuration.dpiScaling();
    const Vector2i scaledWindowSize = windowSize*dpiScaling(configuration);

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
    CORRADE_ASSERT(!(configuration.windowFlags() & Configuration::WindowFlag::Contextless),
        "Platform::Sdl2Application::tryCreate(): cannot pass Configuration::WindowFlag::Contextless when creating an OpenGL context", false);
    CORRADE_ASSERT(_context->version() == GL::Version::None,
        "Platform::Sdl2Application::tryCreate(): context already created", false);

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

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Save DPI scaling values from configuration for future use, scale window
       based on those */
    _configurationDpiScalingPolicy = configuration.dpiScalingPolicy();
    _configurationDpiScaling = configuration.dpiScaling();
    const Vector2i scaledWindowSize = configuration.size()*dpiScaling(configuration);

    /* Request debug context if GpuValidation is enabled either via the
       configuration or via command-line */
    GLConfiguration::Flags glFlags = glConfiguration.flags();
    if((glFlags & GLConfiguration::Flag::GpuValidation) || (_context->configurationFlags() & GL::Context::Configuration::Flag::GpuValidation))
        glFlags |= GLConfiguration::Flag::Debug;
    else if((glFlags & GLConfiguration::Flag::GpuValidationNoError) || (_context->configurationFlags() & GL::Context::Configuration::Flag::GpuValidationNoError))
        glFlags |= GLConfiguration::Flag::NoError;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_NO_ERROR, glFlags >= GLConfiguration::Flag::NoError);

    /* Set context version, if user-specified */
    if(glConfiguration.version() != GL::Version::None) {
        const Containers::Pair<Int, Int> versionMajorMinor = version(glConfiguration.version());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, versionMajorMinor.first());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, versionMajorMinor.second());

        #ifndef MAGNUM_TARGET_GLES
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, glConfiguration.version() >= GL::Version::GL310 ?
            SDL_GL_CONTEXT_PROFILE_CORE : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        #endif

        /* Mask out the upper 32bits used for other flags */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(UnsignedLong(glFlags) & 0xffffffffu));

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
        /* Mask out the upper 32bits used for other flags */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(UnsignedLong(glFlags) & 0xffffffffu));
        #else
        /* For ES the major context version is compile-time constant */
        #ifdef MAGNUM_TARGET_GLES2
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        #endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        #endif
    }

    /* Create a window. Hide it by default so we don't have distracting window
       blinking in case the context creation fails due to unsupported
       configuration or if it gets destroyed for fallback context creation
       below. */
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
    Containers::StringView vendorString;
    #endif
    if(glConfiguration.version() == GL::Version::None && (!_glContext
        #ifndef CORRADE_TARGET_APPLE
        /* Sorry about the UGLY code, HOPEFULLY THERE WON'T BE MORE WORKAROUNDS */
        || (vendorString = reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
        (vendorString == "NVIDIA Corporation"_s ||
         #ifdef CORRADE_TARGET_WINDOWS
         vendorString == "Intel"_s ||
         #endif
         vendorString == "ATI Technologies Inc."_s)
        && !_context->isDriverWorkaroundDisabled("no-forward-compatible-core-context"_s))
        #endif
    )) {
        /* Don't print any warning when doing the workaround, because the bug
           will be there probably forever */
        if(!_glContext) Warning()
            << "Platform::Sdl2Application::tryCreate(): cannot create core context:"
            << SDL_GetError() << "(falling back to compatibility context)";
        else SDL_GL_DeleteContext(_glContext);

        /* Destroy the original window. SDL_GL_SetAttribute() says it should be
           called before creating a window, which kind of implies the
           attributes affect how the window is created:
            https://wiki.libsdl.org/SDL2/SDL_GL_SetAttribute
           Which means, if I attempt to set them differently after the window
           is created, it *might* not work correctly. It doesn't seem to matter
           on Linux at least, but better stay on the safe side as this way
           worked correctly for 10+ years on all platforms and reusing an
           existing window might not. */
        SDL_DestroyWindow(_window);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        /* Discard the ForwardCompatible flag for the fallback. Having it set
           makes the fallback context creation fail on Mesa's Zink (which is
           just 2.1) and I assume on others as well.

           Also mask out the upper 32bits used for other flags. */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(UnsignedLong(glFlags & ~GLConfiguration::Flag::ForwardCompatible) & 0xffffffffu));

        /* Create a new window using the refreshed GL attributes */
        if(!(_window = SDL_CreateWindow(configuration.title().data(),
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            scaledWindowSize.x(), scaledWindowSize.y(),
            SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN|SDL_WINDOW_ALLOW_HIGHDPI|Uint32(configuration.windowFlags()))))
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
        /* Because hardcoding canvas size for WebGL is usually a wrong thing to
           do, notify about that in the verbose output */
        Debug{_verboseLog ? Debug::output() : nullptr} << "Platform::Sdl2Application::tryCreate(): hardcoded canvas size" << windowSize;
    } else {
        windowSize = _lastKnownCanvasSize;
        Debug{_verboseLog ? Debug::output() : nullptr} << "Platform::Sdl2Application::tryCreate(): autodetected canvas size" << windowSize;
    }
    /* Save DPI scaling values from configuration for future use, scale window
       based on those */
    _configurationDpiScalingPolicy = configuration.dpiScalingPolicy();
    _configurationDpiScaling = configuration.dpiScaling();
    const Vector2i scaledWindowSize = windowSize*dpiScaling();

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
    if(!_context->tryCreate(glConfiguration)) {
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

    const Vector2i newSize = dpiScaling()*size;
    SDL_SetWindowSize(_window, newSize.x(), newSize.y());
}

void Sdl2Application::setMinWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::setMinWindowSize(): no window opened", );

    const Vector2i newSize = dpiScaling()*size;
    SDL_SetWindowMinimumSize(_window, newSize.x(), newSize.y());
}

void Sdl2Application::setMaxWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::setMaxWindowSize(): no window opened", );

    const Vector2i newSize = dpiScaling()*size;
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

Vector2 Sdl2Application::dpiScaling() const {
    return dpiScalingInternal(_configurationDpiScalingPolicy, _configurationDpiScaling);
}

#ifdef CORRADE_TARGET_EMSCRIPTEN
void Sdl2Application::setContainerCssClass(const Containers::StringView cssClass) {
    magnumPlatformSetContainerCssClass(cssClass.data(), cssClass.size());
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

    /* Setting interval to 1 may cause SDL_GL_GetSwapInterval() to return -1,
       which is a valid case */
    const Int actualInterval = SDL_GL_GetSwapInterval();
    if(actualInterval != interval && !(interval == 1 && actualInterval == -1)) {
        Error() << "Platform::Sdl2Application::setSwapInterval(): swap interval setting ignored by the driver:" << SDL_GetError();
        _flags &= ~Flag::VSyncEnabled;
        return false;
    }

    if(interval) _flags |= Flag::VSyncEnabled;
    else _flags &= ~Flag::VSyncEnabled;
    return true;
}

#ifndef CORRADE_TARGET_EMSCRIPTEN
void Sdl2Application::setMinimalLoopPeriod(const Nanoseconds time) {
    CORRADE_ASSERT(time >= 0_nsec,
        "Platform::Sdl2Application::setMinimalLoopPeriod(): expected non-negative time, got" << time, );
    _minimalLoopPeriodMilliseconds = Long(time)/1000000;
}

#ifdef MAGNUM_BUILD_DEPRECATED
void Sdl2Application::setMinimalLoopPeriod(const UnsignedInt milliseconds) {
    _minimalLoopPeriodMilliseconds = milliseconds;
}
#endif
#endif

void Sdl2Application::redraw() { _flags |= Flag::Redraw; }

Sdl2Application::~Sdl2Application() {
    /* SDL_DestroyWindow(_window) crashes on windows when _window is nullptr
       (it doesn't seem to crash on Linux). Because this seems to be yet
       another pointless platform difference, to be safe do the same check with
       all. */

    #ifdef MAGNUM_TARGET_GL
    /* Destroy Magnum context first to avoid it potentially accessing the
       now-destroyed GL context after */
    _context = Containers::NullOpt;

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

namespace {

Sdl2Application::Pointer buttonToPointer(const Uint8 button) {
    switch(button) {
        case SDL_BUTTON_LEFT:
            return Sdl2Application::Pointer::MouseLeft;
        case SDL_BUTTON_MIDDLE:
            return Sdl2Application::Pointer::MouseMiddle;
        case SDL_BUTTON_RIGHT:
            return Sdl2Application::Pointer::MouseRight;
        case SDL_BUTTON_X1:
            return Sdl2Application::Pointer::MouseButton4;
        case SDL_BUTTON_X2:
            return Sdl2Application::Pointer::MouseButton5;
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE();
}

Sdl2Application::Pointers buttonsToPointers(const Uint32 buttons) {
    Sdl2Application::Pointers pointers;
    if(buttons & SDL_BUTTON_LMASK)
        pointers |= Sdl2Application::Pointer::MouseLeft;
    if(buttons & SDL_BUTTON_MMASK)
        pointers |= Sdl2Application::Pointer::MouseMiddle;
    if(buttons & SDL_BUTTON_RMASK)
        pointers |= Sdl2Application::Pointer::MouseRight;
    if(buttons & SDL_BUTTON_X1MASK)
        pointers |= Sdl2Application::Pointer::MouseButton4;
    if(buttons & SDL_BUTTON_X2MASK)
        pointers |= Sdl2Application::Pointer::MouseButton5;
    return pointers;
}

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
    const Nanoseconds timeBefore = _minimalLoopPeriodMilliseconds ? SDL_GetTicks()*1.0_msec : Nanoseconds{};
    #endif

    #ifdef CORRADE_TARGET_EMSCRIPTEN
    /* The resize event is not fired on window resize, so poll for the canvas
       size here. But only if the window was requested to be resizable, to
       avoid resizing the canvas when the user doesn't want that. Related
       issue: https://github.com/kripken/emscripten/issues/1731 */
    /** @todo don't hardcode "#canvas" here, make it configurable from outside */
    if(_flags & Flag::Resizable) {
        Vector2d canvasSize;
        emscripten_get_element_css_size("#canvas", &canvasSize.x(), &canvasSize.y());

        const Vector2i canvasSizei{canvasSize};
        if(canvasSizei != _lastKnownCanvasSize) {
            _lastKnownCanvasSize = canvasSizei;
            const Vector2i size = dpiScaling()*canvasSizei;
            emscripten_set_canvas_element_size("#canvas", size.x(), size.y());
            ViewportEvent e{
                #ifdef MAGNUM_TARGET_GL
                size,
                #endif
                size, dpiScaling()};
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
                    /* Not using SDL_WINDOWEVENT_RESIZED, because that doesn't
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
                            dpiScaling()};
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
                KeyEvent e{event, Key(event.key.keysym.sym), UnsignedInt(event.key.keysym.scancode), fixedModifiers(event.key.keysym.mod), event.key.repeat != 0};
                event.type == SDL_KEYDOWN ? keyPressEvent(e) : keyReleaseEvent(e);
            } break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                const Pointer pointer = buttonToPointer(event.button.button);
                const Vector2 position{Float(event.button.x), Float(event.button.y)};

                /* If an additional mouse button was pressed or some buttons
                   are still left pressed after a release, call a move event
                   instead */
                const Uint32 buttons = SDL_GetMouseState(nullptr, nullptr);
                if((event.type == SDL_MOUSEBUTTONDOWN && (buttons & ~SDL_BUTTON(event.button.button))) ||
                   (event.type == SDL_MOUSEBUTTONUP && buttons)) {
                    Pointers pointers = buttonsToPointers(buttons);
                    PointerMoveEvent e{event, PointerEventSource::Mouse, pointer, pointers, true,
                        #ifdef CORRADE_TARGET_EMSCRIPTEN
                        0,
                        /* Since 2.0.22, added w/ SDL_HINT_MOUSE_TOUCH_EVENTS */
                        #elif defined(SDL_MOUSE_TOUCHID)
                        SDL_MOUSE_TOUCHID,
                        #else
                        -1,
                        #endif
                        position, {}};
                    pointerMoveEvent(e);
                } else {
                    PointerEvent e{event, PointerEventSource::Mouse, pointer, true,
                        #ifdef CORRADE_TARGET_EMSCRIPTEN
                        0,
                        /* Since 2.0.22, added w/ SDL_HINT_MOUSE_TOUCH_EVENTS */
                        #elif defined(SDL_MOUSE_TOUCHID)
                        SDL_MOUSE_TOUCHID,
                        #else
                        -1,
                        #endif
                        position
                        #ifndef CORRADE_TARGET_EMSCRIPTEN
                        , event.button.clicks
                        #endif
                    };
                    event.type == SDL_MOUSEBUTTONDOWN ?
                        pointerPressEvent(e) : pointerReleaseEvent(e);
                }
            } break;

            case SDL_MOUSEWHEEL: {
                ScrollEvent e{event,
                    #if SDL_VERSION_ATLEAST(2, 0, 18)
                    {event.wheel.preciseX, event.wheel.preciseY}
                    #else
                    {Float(event.wheel.x), Float(event.wheel.y)}
                    #endif
                    /* Yeah, it's 2.0.22 and then 2.24.0, they changed the
                       versioning */
                    #if SDL_VERSION_ATLEAST(2, 26, 0)
                    , {Float(event.wheel.mouseX), Float(event.wheel.mouseY)}
                    #endif
                };
                scrollEvent(e);
            } break;

            case SDL_MOUSEMOTION: {
                PointerMoveEvent e{event, PointerEventSource::Mouse, {}, buttonsToPointers(event.motion.state), true,
                    #ifdef CORRADE_TARGET_EMSCRIPTEN
                    0,
                    /* Since 2.0.22, added w/ SDL_HINT_MOUSE_TOUCH_EVENTS */
                    #elif defined(SDL_MOUSE_TOUCHID)
                    SDL_MOUSE_TOUCHID,
                    #else
                    -1,
                    #endif
                    {Float(event.motion.x), Float(event.motion.y)},
                    {Float(event.motion.xrel), Float(event.motion.yrel)}};
                pointerMoveEvent(e);
            } break;

            #ifndef CORRADE_TARGET_EMSCRIPTEN
            case SDL_FINGERDOWN:
            case SDL_FINGERUP: {
                /* Scale the event from useless [0, 1] to the actual window
                   size, not sure why is it so weird. Also let's hope the
                   SDL_GetWindowSize() call isn't too demanding, I don't want
                   to be caching this value, it's bad enough to have to track
                   that on Emscripten. */
                Vector2i windowSize{NoInit};
                SDL_GetWindowSize(_window, &windowSize.x(), &windowSize.y());

                /* Update primary finger info. If there's no primary finger yet
                   and this is the first finger pressed, it becomes the primary
                   finger. If the primary finger is lifted, no other finger
                   becomes primary until all others are lifted as well. This
                   was empirically verified by looking at behavior of a mouse
                   cursor on a multi-touch screen under X11, it's possible that
                   other systems do it differently. The same logic is used in
                   EmscriptenApplication and AndroidApplication. Also, right
                   now there's an assumption that there is just one touch
                   device, fingers from different touch devices would steal the
                   primary bit from each other on every press. */
                bool primary;
                if(_primaryFingerId == ~Long{} && event.type == SDL_FINGERDOWN && SDL_GetNumTouchFingers(event.tfinger.touchId) == 1) {
                    primary = true;
                    _primaryFingerId = event.tfinger.fingerId;
                /* Otherwise, if this is the primary finger, mark it as such */
                } else if(_primaryFingerId == event.tfinger.fingerId) {
                    primary = true;
                    /* ... but if it's a release, it's no longer primary */
                    if(event.type == SDL_FINGERUP)
                        _primaryFingerId = ~Long{};
                /* Otherwise this is not the primary finger */
                } else primary = false;

                /* Make it so that value of 0 is reported as 0 and 1 is
                   reported as the rightmost / bottommost pixel, i.e. 799 / 599
                   for 800x600. This matches with what SDL itself does for the
                   touch event translation. */
                const Vector2 scale = Vector2{windowSize - Vector2i{1}};
                PointerEvent e{event, PointerEventSource::Touch,
                    Pointer::Finger, primary, event.tfinger.fingerId,
                    Vector2{event.tfinger.x, event.tfinger.y}*scale, 1};
                event.type == SDL_FINGERDOWN ?
                    pointerPressEvent(e) : pointerReleaseEvent(e);
            } break;

            case SDL_FINGERMOTION: {
                /* Scale the event from useless [0, 1] to the actual window
                   size, not sure why is it so weird. Also let's hope the
                   SDL_GetWindowSize() call isn't too demanding, I don't want
                   to be caching this value, it's bad enough to have to track
                   that on Emscripten. */
                Vector2i windowSize{NoInit};
                SDL_GetWindowSize(_window, &windowSize.x(), &windowSize.y());

                /* In this case, it's a primary finger only if it was
                   registered as such during the last press. If the primary
                   finger was lifted, no other finger will step into its place
                   until all others are lifted as well. */
                const bool primary = _primaryFingerId == event.tfinger.fingerId;

                /* Make it so that value of 0 is reported as 0 and 1 is
                   reported as the rightmost / bottommost pixel, i.e. 799 / 599
                   for 800x600. This matches with what SDL itself does for the
                   touch event translation. */
                const Vector2 scale = Vector2{windowSize - Vector2i{1}};
                PointerMoveEvent e{event, PointerEventSource::Touch, {},
                    Pointer::Finger, primary, event.tfinger.fingerId,
                    Vector2{event.tfinger.x, event.tfinger.y}*scale,
                    Vector2{event.tfinger.dx, event.tfinger.dy}*scale};
                pointerMoveEvent(e);
                break;
            }
            #endif

            #ifdef MAGNUM_BUILD_DEPRECATED
            case SDL_MULTIGESTURE: {
                CORRADE_IGNORE_DEPRECATED_PUSH
                MultiGestureEvent e{event, {event.mgesture.x, event.mgesture.y}, event.mgesture.dTheta, event.mgesture.dDist, event.mgesture.numFingers};
                multiGestureEvent(e);
                CORRADE_IGNORE_DEPRECATED_POP
                break;
            }
            #endif

            case SDL_TEXTINPUT: {
                TextInputEvent e{event, event.text.text};
                textInputEvent(e);
            } break;

            case SDL_TEXTEDITING: {
                TextEditingEvent e{event, event.edit.text, event.edit.start, event.edit.length};
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
        if(!(_flags & Flag::VSyncEnabled) && _minimalLoopPeriodMilliseconds) {
            const Nanoseconds loopTime = SDL_GetTicks()*1.0_msec - timeBefore;
            if(loopTime < _minimalLoopPeriodMilliseconds*1.0_msec)
                SDL_Delay(_minimalLoopPeriodMilliseconds - loopTime/1.0_msec);
        }
        #endif

        return !(_flags & Flag::Exit);
    }

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* If not drawing anything, delay to prevent CPU hogging (if set) */
    if(_minimalLoopPeriodMilliseconds) {
        const Nanoseconds loopTime = SDL_GetTicks()*1.0_msec - timeBefore;
        if(loopTime < _minimalLoopPeriodMilliseconds*1.0_msec)
            SDL_Delay(_minimalLoopPeriodMilliseconds - loopTime/1.0_msec);
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
constexpr Containers::StringView CursorMap[] {
    "default"_s,
    "text"_s,
    "wait"_s,
    "crosshair"_s,
    "progress"_s,
    "nwse-resize"_s,
    "nesw-resize"_s,
    "ew-resize"_s,
    "ns-resize"_s,
    "move"_s,
    "not-allowed"_s,
    "pointer"_s,
    "none"_s
    /* Hidden & locked not supported yet */
};
#endif

}

void Sdl2Application::setCursor(Cursor cursor) {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::setCursor(): no window opened", );

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

    /* The second condition could be a static assert but it doesn't let me
       because "this pointer only accessible in a constexpr function". Thanks
       for nothing, C++. */
    CORRADE_INTERNAL_ASSERT(UnsignedInt(cursor) < Containers::arraySize(_cursors) && Containers::arraySize(_cursors) == Containers::arraySize(CursorMap));

    if(!_cursors[UnsignedInt(cursor)])
        _cursors[UnsignedInt(cursor)] = SDL_CreateSystemCursor(CursorMap[UnsignedInt(cursor)]);

    SDL_SetCursor(_cursors[UnsignedInt(cursor)]);
    #else
    CORRADE_ASSERT(_surface, "Platform::Sdl2Application::setCursor(): no window opened", );
    _cursor = cursor;
    CORRADE_INTERNAL_ASSERT(UnsignedInt(cursor) < Containers::arraySize(CursorMap));
    magnumPlatformSetCursor(CursorMap[UnsignedInt(cursor)].data(),
                            CursorMap[UnsignedInt(cursor)].size());
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

void Sdl2Application::viewportEvent(ViewportEvent&) {}
void Sdl2Application::keyPressEvent(KeyEvent&) {}
void Sdl2Application::keyReleaseEvent(KeyEvent&) {}

#ifdef MAGNUM_BUILD_DEPRECATED
namespace {

CORRADE_IGNORE_DEPRECATED_PUSH
Sdl2Application::MouseEvent::Button pointerToButton(const Sdl2Application::Pointer pointer) {
    switch(pointer) {
        case Sdl2Application::Pointer::MouseLeft:
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        case Sdl2Application::Pointer::Finger:
        #endif
            return Sdl2Application::MouseEvent::Button::Left;
        case Sdl2Application::Pointer::MouseMiddle:
            return Sdl2Application::MouseEvent::Button::Middle;
        case Sdl2Application::Pointer::MouseRight:
            return Sdl2Application::MouseEvent::Button::Right;
        case Sdl2Application::Pointer::MouseButton4:
            return Sdl2Application::MouseEvent::Button::X1;
        case Sdl2Application::Pointer::MouseButton5:
            return Sdl2Application::MouseEvent::Button::X2;
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE();
}
CORRADE_IGNORE_DEPRECATED_POP

}
#endif

void Sdl2Application::pointerPressEvent(PointerEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    if(!event.isPrimary())
        return;

    CORRADE_IGNORE_DEPRECATED_PUSH
    MouseEvent mouseEvent{event.event(), pointerToButton(event.pointer()), Vector2i{Math::round(event.position())}
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        , event.clickCount()
        #endif
    };
    mousePressEvent(mouseEvent);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void Sdl2Application::mousePressEvent(MouseEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void Sdl2Application::pointerReleaseEvent(PointerEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    if(!event.isPrimary())
        return;

    CORRADE_IGNORE_DEPRECATED_PUSH
    MouseEvent mouseEvent{event.event(), pointerToButton(event.pointer()), Vector2i{Math::round(event.position())}
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        , event.clickCount()
        #endif
    };
    mouseReleaseEvent(mouseEvent);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void Sdl2Application::mouseReleaseEvent(MouseEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void Sdl2Application::pointerMoveEvent(PointerMoveEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    if(!event.isPrimary())
        return;

    const Vector2i roundedPosition{Math::round(event.position())};

    CORRADE_IGNORE_DEPRECATED_PUSH
    /* If the event is due to some button being additionally pressed or one
       button from a larger set being released, delegate to a press/release
       event instead */
    if(event.pointer()) {
        /* SDL2 reports either a move or a press/release, so there shouldn't be
           any move in this case */
        CORRADE_INTERNAL_ASSERT(event.relativePosition() == Vector2{});
        MouseEvent mouseEvent{event.event(), pointerToButton(*event.pointer()),
            roundedPosition
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            , 1
            #endif
        };
        event.pointers() >= *event.pointer() ?
            mousePressEvent(mouseEvent) : mouseReleaseEvent(mouseEvent);
    } else {
        MouseMoveEvent::Buttons buttons;
        if(event.pointers() & (Pointer::MouseLeft
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            |Pointer::Finger
            #endif
        ))
            buttons |= MouseMoveEvent::Button::Left;
        if(event.pointers() & Pointer::MouseMiddle)
            buttons |= MouseMoveEvent::Button::Middle;
        if(event.pointers() & Pointer::MouseRight)
            buttons |= MouseMoveEvent::Button::Right;
        if(event.pointers() & Pointer::MouseButton4)
            buttons |= MouseMoveEvent::Button::X1;
        if(event.pointers() & Pointer::MouseButton5)
            buttons |= MouseMoveEvent::Button::X2;

        /* Can't do just Math::round(event.relativePosition()) because if the
           previous position was 4.6 and the new 5.3, they both round to 5 but
           the relativePosition is 0.6 and rounds to 1. Conversely, if it'd be
           5.3 and 5.6, the positions round to 5 and 6 but relative position
           stays 0. */
        const Vector2i previousRoundedPosition{Math::round(event.position() - event.relativePosition())};
        /* Call the event only if the integer values actually changed */
        if(roundedPosition != previousRoundedPosition) {
            MouseMoveEvent mouseEvent{event.event(), roundedPosition, roundedPosition - previousRoundedPosition, buttons};
            mouseMoveEvent(mouseEvent);
        }
    }
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void Sdl2Application::mouseMoveEvent(MouseMoveEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void Sdl2Application::scrollEvent(ScrollEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    MouseScrollEvent mouseEvent{event.event(), event.offset()};
    mouseScrollEvent(mouseEvent);
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
void Sdl2Application::mouseScrollEvent(MouseScrollEvent&) {}
void Sdl2Application::multiGestureEvent(MultiGestureEvent&) {}
CORRADE_IGNORE_DEPRECATED_POP
#endif

void Sdl2Application::textInputEvent(TextInputEvent&) {}
void Sdl2Application::textEditingEvent(TextEditingEvent&) {}

#ifdef MAGNUM_TARGET_GL
Sdl2Application::GLConfiguration::GLConfiguration():
    _colorBufferSize{8, 8, 8, 8}, _depthBufferSize{24}, _stencilBufferSize{0},
    _sampleCount(0)
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    , _version{GL::Version::None}, _srgbCapable{false}
    #endif
{
    #ifndef MAGNUM_TARGET_GLES
    addFlags(Flag::ForwardCompatible);
    #endif
}

Sdl2Application::GLConfiguration::~GLConfiguration() = default;
#endif

Sdl2Application::Configuration::Configuration():
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
    _title(Containers::String::nullTerminatedGlobalView("Magnum SDL2 Application"_s)),
    #endif
    #if !defined(CORRADE_TARGET_IOS) && !defined(CORRADE_TARGET_EMSCRIPTEN)
    _size{800, 600},
    #else
    _size{}, /* SDL2 detects someting for us */
    #endif
    _dpiScalingPolicy{DpiScalingPolicy::Default} {}

Sdl2Application::Configuration::~Configuration() = default;

#ifdef MAGNUM_BUILD_DEPRECATED
Containers::StringView Sdl2Application::KeyEvent::keyName(const Sdl2Application::Key key) {
    return Sdl2Application::keyName(key);
}
#endif

Containers::StringView Sdl2Application::KeyEvent::keyName() const {
    return Sdl2Application::keyName(_key);
}

#ifndef CORRADE_TARGET_EMSCRIPTEN
Containers::StringView Sdl2Application::KeyEvent::scanCodeName() const {
    return Sdl2Application::scanCodeName(_scancode);
}
#endif

Sdl2Application::Modifiers Sdl2Application::PointerEvent::modifiers() {
    if(!_modifiers)
        _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
    return *_modifiers;
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Sdl2Application::Modifiers Sdl2Application::MouseEvent::modifiers() {
    if(_modifiers) return *_modifiers;
    return *(_modifiers = fixedModifiers(Uint16(SDL_GetModState())));
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

Sdl2Application::Modifiers Sdl2Application::PointerMoveEvent::modifiers() {
    if(!_modifiers)
        _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
    return *_modifiers;
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Sdl2Application::Modifiers Sdl2Application::MouseMoveEvent::modifiers() {
    if(_modifiers) return *_modifiers;
    return *(_modifiers = fixedModifiers(Uint16(SDL_GetModState())));
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

/* Yeah, it's 2.0.22 and then 2.24.0, they changed the versioning */
#if !SDL_VERSION_ATLEAST(2, 26, 0)
Vector2 Sdl2Application::ScrollEvent::position() {
    if(!_position) {
        Vector2i position;
        SDL_GetMouseState(&position.x(), &position.y());
        _position = Vector2{position};
    }
    return *_position;
}
#endif

Sdl2Application::Modifiers Sdl2Application::ScrollEvent::modifiers() {
    if(!_modifiers)
        _modifiers = fixedModifiers(Uint16(SDL_GetModState()));
    return *_modifiers;
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
Vector2i Sdl2Application::MouseScrollEvent::position() {
    if(_position) return *_position;
    _position = Vector2i{};
    SDL_GetMouseState(&_position->x(), &_position->y());
    return *_position;
}

Sdl2Application::Modifiers Sdl2Application::MouseScrollEvent::modifiers() {
    if(_modifiers) return *_modifiers;
    return *(_modifiers = fixedModifiers(Uint16(SDL_GetModState())));
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

/* WinRT builds by default have deprecation warnings as errors. Combined with a
   MSVC 2017 bug where deprecation warning suppression doesn't work on virtual
   function overrides this make the build fail on deprecation warnings due to
   ScreenedApplication overriding mousePressEvent(), mouseReleaseEvent(),
   mouseMoveEvent() and mouseScrollEvent(). Disable the warnings at a higher
   level instead. */
#if defined(MAGNUM_BUILD_DEPRECATED) && defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG) && _MSC_VER < 1920
CORRADE_IGNORE_DEPRECATED_PUSH
#endif
template class BasicScreen<Sdl2Application>;
template class BasicScreenedApplication<Sdl2Application>;
#if defined(MAGNUM_BUILD_DEPRECATED) && defined(CORRADE_TARGET_MSVC) && !defined(CORRADE_TARGET_CLANG) && _MSC_VER < 1920
CORRADE_IGNORE_DEPRECATED_POP
#endif

}}
