/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>
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
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Arguments.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/Math/Range.h"
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

enum class Sdl2ApplicationWindow::WindowFlag: UnsignedByte {
    Redraw = 1 << 0,
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    TextInputActive = 1 << 1,
    Resizable = 1 << 2,
    #endif
};

Sdl2ApplicationWindow::Sdl2ApplicationWindow(Sdl2Application& application, NoCreateT): _application{application}, _windowFlags{WindowFlag::Redraw} {}

#ifndef CORRADE_TARGET_EMSCRIPTEN
Sdl2ApplicationWindow::Sdl2ApplicationWindow(Sdl2Application& application, const Configuration& configuration): Sdl2ApplicationWindow{application, NoCreate} {
    if(!tryCreateWindow(configuration)) std::exit(1);
}

Sdl2ApplicationWindow::Sdl2ApplicationWindow(Sdl2Application& application): Sdl2ApplicationWindow{application, Configuration{}} {}
#endif

Sdl2ApplicationWindow::~Sdl2ApplicationWindow() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* If the window isn't created yet because tryCreateWindow(), nothing to
       do. This also covers the case for the main application window, which
       gets destroyed (and reset to null) from within ~Sdl2Application()
       because it has to be done before SDL_Quit(), and this destructor is
       called after that. There it also means that it's not needed to remove
       itself from the window list, as the list is already gone at this
       point. */
    if(_window)
        destroyWindow();
    #endif
}

bool Sdl2ApplicationWindow::tryCreateWindow(const Configuration& configuration) {
    CORRADE_ASSERT(!_window,
        "Platform::Sdl2ApplicationWindow::tryCreateWindow(): window already created", false);

    /* Save DPI scaling values from configuration for future use, scale window
       based on those */
    _configurationDpiScalingPolicy = configuration.dpiScalingPolicy();
    _configurationDpiScaling = configuration.dpiScaling();
    const Vector2i scaledWindowSize = configuration.size()*dpiScaling(configuration);

    /* Create a window */
    if(!(_window = SDL_CreateWindow(
        #ifndef CORRADE_TARGET_IOS
        configuration.title().data(),
        #else
        nullptr,
        #endif
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        scaledWindowSize.x(), scaledWindowSize.y(),
        SDL_WINDOW_ALLOW_HIGHDPI|Uint32(configuration.windowFlags())|_application._configurationFlags)))
    {
        Error() << "Platform::Sdl2ApplicationWindow::tryCreateWindow(): cannot create a window:" << SDL_GetError();
        return false;
    }

    /* Add itself to the window list */
    const std::size_t windowId = SDL_GetWindowID(_window);
    if(windowId >= _application._windows.size()) {
        for(Sdl2ApplicationWindow*& i: arrayAppend(_application._windows, NoInit, windowId - _application._windows.size() + 1))
            i = nullptr;
    }
    CORRADE_INTERNAL_ASSERT(!_application._windows[windowId]);
    _application._windows[windowId] = this;

    return true;
}

#ifndef CORRADE_TARGET_EMSCRIPTEN
void Sdl2ApplicationWindow::destroyWindow() {
    /* To prevent accidental double destructions and such, this function should
       only be called if a window is actually created */
    CORRADE_INTERNAL_ASSERT(_window);

    /* Remove itself from the window list */
    const std::size_t id = SDL_GetWindowID(_window);
    CORRADE_INTERNAL_ASSERT(id < _application._windows.size());
    _application._windows[id] = nullptr;

    SDL_DestroyWindow(_window);
}
#endif

Vector2 Sdl2ApplicationWindow::dpiScaling(const Configuration& configuration) {
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

Vector2 Sdl2ApplicationWindow::dpiScalingInternal(const Implementation::Sdl2DpiScalingPolicy configurationDpiScalingPolicy, const Vector2& configurationDpiScaling) const {
    std::ostream* verbose = _application._verboseLog ? Debug::output() : nullptr;

    /* Use values from the configuration only if not overridden on command line
       to something non-default. In any case explicit scaling has a precedence
       before the policy. */
    Implementation::Sdl2DpiScalingPolicy dpiScalingPolicy{};
    if(!_application._commandLineDpiScaling.isZero()) {
        Debug{verbose} << "Platform::Sdl2Application: user-defined DPI scaling" << _application._commandLineDpiScaling;
        return _application._commandLineDpiScaling;
    } else if(_application._commandLineDpiScalingPolicy != Implementation::Sdl2DpiScalingPolicy::Default) {
        dpiScalingPolicy = _application._commandLineDpiScalingPolicy;
    } else if(!configurationDpiScaling.isZero()) {
        Debug{verbose} << "Platform::Sdl2Application: app-defined DPI scaling" << configurationDpiScaling;
        return configurationDpiScaling;
    } else {
        dpiScalingPolicy = configurationDpiScalingPolicy;
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

void Sdl2ApplicationWindow::setWindowTitle(const Containers::StringView title) {
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

#if !defined(CORRADE_TARGET_EMSCRIPTEN) && SDL_MAJOR_VERSION*1000 + SDL_MINOR_VERSION*100 + SDL_PATCHLEVEL >= 2005
void Sdl2ApplicationWindow::setWindowIcon(const ImageView2D& image) {
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
            CORRADE_ASSERT_UNREACHABLE("Platform::Sdl2ApplicationWindow::setWindowIcon(): unexpected format" << image.format(), );
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

Vector2i Sdl2ApplicationWindow::windowSize() const {
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
void Sdl2ApplicationWindow::setWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::setWindowSize(): no window opened", );

    const Vector2i newSize = dpiScaling()*size;
    SDL_SetWindowSize(_window, newSize.x(), newSize.y());
}

void Sdl2ApplicationWindow::setMinWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::setMinWindowSize(): no window opened", );

    const Vector2i newSize = dpiScaling()*size;
    SDL_SetWindowMinimumSize(_window, newSize.x(), newSize.y());
}

void Sdl2ApplicationWindow::setMaxWindowSize(const Vector2i& size) {
    CORRADE_ASSERT(_window, "Platform::Sdl2Application::setMaxWindowSize(): no window opened", );

    const Vector2i newSize = dpiScaling()*size;
    SDL_SetWindowMaximumSize(_window, newSize.x(), newSize.y());
}
#endif

#ifdef MAGNUM_TARGET_GL
Vector2i Sdl2ApplicationWindow::framebufferSize() const {
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

Vector2 Sdl2ApplicationWindow::dpiScaling() const {
    return dpiScalingInternal(_application._configurationDpiScalingPolicy, _application._configurationDpiScaling);
}

void Sdl2ApplicationWindow::redraw() { _windowFlags |= WindowFlag::Redraw; }

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

void Sdl2ApplicationWindow::setCursor(Cursor cursor) {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    CORRADE_ASSERT(_window, "Platform::Sdl2ApplicationWindow::setCursor(): no window opened", );

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
    CORRADE_INTERNAL_ASSERT(UnsignedInt(cursor) < Containers::arraySize(_application._cursors) && Containers::arraySize(_application._cursors) == Containers::arraySize(CursorMap));

    if(!_application._cursors[UnsignedInt(cursor)])
        _application._cursors[UnsignedInt(cursor)] = SDL_CreateSystemCursor(CursorMap[UnsignedInt(cursor)]);

    SDL_SetCursor(_application._cursors[UnsignedInt(cursor)]);
    #else
    CORRADE_ASSERT(_surface, "Platform::Sdl2Application::setCursor(): no window opened", );
    _cursor = cursor;
    CORRADE_INTERNAL_ASSERT(UnsignedInt(cursor) < Containers::arraySize(CursorMap));
    magnumPlatformSetCursor(CursorMap[UnsignedInt(cursor)].data(),
                            CursorMap[UnsignedInt(cursor)].size());
    #endif
}

Sdl2Application::Cursor Sdl2ApplicationWindow::cursor() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    if(SDL_GetRelativeMouseMode())
        return Cursor::HiddenLocked;
    else if(!SDL_ShowCursor(SDL_QUERY))
        return Cursor::Hidden;

    SDL_Cursor* cursor = SDL_GetCursor();

    if(cursor) for(UnsignedInt i = 0; i < sizeof(_application._cursors); i++)
        if(_application._cursors[i] == cursor) return Cursor(i);

    return Cursor::Arrow;
    #else
    return _cursor;
    #endif
}

void Sdl2ApplicationWindow::viewportEvent(ViewportEvent&) {}
void Sdl2ApplicationWindow::keyPressEvent(KeyEvent&) {}
void Sdl2ApplicationWindow::keyReleaseEvent(KeyEvent&) {}
void Sdl2ApplicationWindow::mousePressEvent(MouseEvent&) {}
void Sdl2ApplicationWindow::mouseReleaseEvent(MouseEvent&) {}
void Sdl2ApplicationWindow::mouseMoveEvent(MouseMoveEvent&) {}
void Sdl2ApplicationWindow::mouseScrollEvent(MouseScrollEvent&) {}
void Sdl2ApplicationWindow::multiGestureEvent(MultiGestureEvent&) {}
void Sdl2ApplicationWindow::textInputEvent(TextInputEvent&) {}
void Sdl2ApplicationWindow::textEditingEvent(TextEditingEvent&) {}

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
    if(modifiers & Sdl2Application::InputEvent::Modifier::Super) modifiers |= Sdl2Application::InputEvent::Modifier::Super;
    return modifiers;
}

}

enum class Sdl2Application::Flag: UnsignedByte {
    VSyncEnabled = 1 << 0,
    NoTickEvent = 1 << 1,
    NoAnyEvent = 1 << 2,
    Exit = 1 << 3,
    #ifdef CORRADE_TARGET_APPLE
    HiDpiWarningPrinted = 1 << 4
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

Sdl2Application::Sdl2Application(const Arguments& arguments, NoCreateT): Sdl2ApplicationWindow{*this, NoCreate}
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    , _minimalLoopPeriod{0}
    #endif
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

bool Sdl2Application::tryCreate(const Configuration& configuration) {
    #ifdef MAGNUM_TARGET_GL
    if(!(configuration.flags() & Configuration::Flag::Contextless))
        return tryCreate(configuration, GLConfiguration{});
    #endif

    /* Save the application-global configuration flags to be used to create all
       windows */
    _configurationFlags = Uint32(configuration.flags());

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Create the main window */
    if(!tryCreateWindow(configuration))
        return false;

    /* Register the main window in the window list */
    CORRADE_INTERNAL_ASSERT(_windows.isEmpty());
    arrayAppend(_windows, this);

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
    const Vector2i scaledWindowSize = configuration.size()*dpiScaling(configuration);

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
    CORRADE_ASSERT(!(configuration.flags() & Configuration::Flag::Contextless),
        "Platform::Sdl2Application::tryCreate(): cannot pass Configuration::Flag::Contextless when creating an OpenGL context", false);
    CORRADE_ASSERT(_context->version() == GL::Version::None,
        "Platform::Sdl2Application::tryCreate(): context already created", false);

    /* Save the application-global configuration flags to be used to create all
       windows. Since we're creating a GL context, request the window to also
       be OpenGL-enabled. */
    _configurationFlags = Uint32(configuration.flags()|Configuration::Flag::OpenGL);

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
    /* Request debug context if GpuValidation is enabled either via the
       configuration or via command-line */
    GLConfiguration::Flags glFlags = glConfiguration.flags();
    if((glFlags & GLConfiguration::Flag::GpuValidation) || (_context->configurationFlags() & GL::Context::Configuration::Flag::GpuValidation))
        glFlags |= GLConfiguration::Flag::Debug;
    #if SDL_MAJOR_VERSION*1000 + SDL_MINOR_VERSION*100 + SDL_PATCHLEVEL >= 2006
    else if((glFlags & GLConfiguration::Flag::GpuValidationNoError) || (_context->configurationFlags() & GL::Context::Configuration::Flag::GpuValidationNoError))
        glFlags |= GLConfiguration::Flag::NoError;
    #endif

    #if SDL_MAJOR_VERSION*1000 + SDL_MINOR_VERSION*100 + SDL_PATCHLEVEL >= 2006
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_NO_ERROR, glFlags >= GLConfiguration::Flag::NoError);
    #endif

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

    /* Hide the main window by default so we don't have distracting window
       blinking in case the context creation fails due to unsupported
       configuration or if it gets destroyed for fallback context creation
       below. */
    Sdl2ApplicationWindow::Configuration hiddenWindowConfiguration{configuration};
    hiddenWindowConfiguration.addWindowFlags(Sdl2ApplicationWindow::Configuration::WindowFlag::Hidden);

    /* Create a window */
    if(!tryCreateWindow(hiddenWindowConfiguration))
        return false;

    /* Create a context */
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
        destroyWindow();
        _window = nullptr;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        /* Discard the ForwardCompatible flag for the fallback. Having it set
           makes the fallback context creation fail on Mesa's Zink (which is
           just 2.1) and I assume on others as well.

           Also mask out the upper 32bits used for other flags. */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, int(UnsignedLong(glFlags & ~GLConfiguration::Flag::ForwardCompatible) & 0xffffffffu));

        /* Create a new window using the refreshed GL attributes */
        if(!tryCreateWindow(hiddenWindowConfiguration))
            return false;

        /* Create compatibility context */
        _glContext = SDL_GL_CreateContext(_window);
    }
    #endif

    /* Cannot create context (or fallback compatibility context on desktop) */
    if(!_glContext) {
        Error() << "Platform::Sdl2Application::tryCreate(): cannot create context:" << SDL_GetError();
        destroyWindow();
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
    const Vector2i scaledWindowSize = configuration.size()*dpiScaling(configuration);

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
        destroyWindow();
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

#ifdef CORRADE_TARGET_EMSCRIPTEN
void Sdl2Application::setContainerCssClass(const Containers::StringView cssClass) {
    magnumPlatformSetContainerCssClass(cssClass.data(), cssClass.size());
}
#endif

void Sdl2ApplicationWindow::swapBuffers() {
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

Sdl2Application::~Sdl2Application() {
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /* Destroy the main SDL window first. After that, there should be no
       remaining registered windows. */
    if(_window)
        destroyWindow();
    for(Sdl2ApplicationWindow* i: _windows)
        CORRADE_ASSERT(!i, "Sdl2Application: destructed with windows still open", );
    #else
    /* SDL_DestroyWindow(_window) crashes on windows when _window is nullptr
       (it doesn't seem to crash on Linux). Because this seems to be yet
       another pointless platform difference, to be safe do the same check with
       all APIs. */
    if(_window) SDL_DestroyWindow(_window);
    #endif

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

#ifndef CORRADE_TARGET_EMSCRIPTEN
void Sdl2Application::makeContextCurrent(Sdl2ApplicationWindow& window) {
    /* Only do it if it is not active already */
    if(_activeGlContextWindow != window._window) {
        SDL_GL_MakeCurrent(window._window, _glContext);
        _activeGlContextWindow = window._window;
#warning TODO
        // Context::current().resetState(Context::State::WindowSpecific);
    }
}
#endif

template<class ...Args> inline void Sdl2Application::callEventHandler(std::size_t windowId, void(Sdl2ApplicationWindow::*eventHandler)(Args...), Args&&... args) {
    // if(!(windowId < _windows.size() && _windows[windowId])) {
    //     Debug() << "HUH" << windowId << _windows.size();
    //     return;
    // }

    CORRADE_INTERNAL_ASSERT(windowId < _windows.size() && _windows[windowId]);
    (_windows[windowId]->*eventHandler)(std::forward<Args>(args)...);
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
                CORRADE_INTERNAL_ASSERT(event.window.windowID < _windows.size() && _windows[event.window.windowID]);
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
#warning make context current here? probably??
                        makeContextCurrent(*_windows[event.window.windowID]);
                        callEventHandler(event.window.windowID, &Sdl2ApplicationWindow::viewportEvent, e);
                        _windows[event.window.windowID]->_windowFlags |= WindowFlag::Redraw;
                        #endif
                    } break;
#warning SDL_WINDOWEVENT_CLOSE
                    /* Direct everything that wasn't exposed via a callback to
                       anyEvent(), so users can implement event handling for
                       things not present in the Application APIs */
                    case SDL_WINDOWEVENT_EXPOSED:
                        _windows[event.window.windowID]->_windowFlags |= WindowFlag::Redraw;
                        if(!(_flags & Flag::NoAnyEvent)) anyEvent(event);
                        break;
                    default:
                        if(!(_flags & Flag::NoAnyEvent)) anyEvent(event);
                } break;

            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                KeyEvent e{event, static_cast<KeyEvent::Key>(event.key.keysym.sym), fixedModifiers(event.key.keysym.mod), event.key.repeat != 0};
                callEventHandler(event.key.windowID,
                    event.type == SDL_KEYDOWN ? &Sdl2ApplicationWindow::keyPressEvent : &Sdl2ApplicationWindow::keyReleaseEvent,
                    e);
            } break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                MouseEvent e{event, static_cast<MouseEvent::Button>(event.button.button), {event.button.x, event.button.y}
                    #ifndef CORRADE_TARGET_EMSCRIPTEN
                    , event.button.clicks
                    #endif
                    };
                callEventHandler(event.key.windowID,
                    event.type == SDL_MOUSEBUTTONDOWN ? &Sdl2ApplicationWindow::mousePressEvent : &Sdl2ApplicationWindow::mouseReleaseEvent,
                    e);
            } break;

            case SDL_MOUSEWHEEL: {
                MouseScrollEvent e{event, {Float(event.wheel.x), Float(event.wheel.y)}};
                callEventHandler(event.wheel.windowID, &Sdl2ApplicationWindow::mouseScrollEvent, e);
            } break;

            case SDL_MOUSEMOTION: {
                MouseMoveEvent e{event, {event.motion.x, event.motion.y}, {event.motion.xrel, event.motion.yrel}, static_cast<MouseMoveEvent::Button>(event.motion.state)};
                callEventHandler(event.motion.windowID, &Sdl2ApplicationWindow::mouseMoveEvent, e);
                break;
            }

            case SDL_MULTIGESTURE: {
                MultiGestureEvent e{event, {event.mgesture.x, event.mgesture.y}, event.mgesture.dTheta, event.mgesture.dDist, event.mgesture.numFingers};
#warning wtf, why no window ID?!
                multiGestureEvent(e);
                break;
            }

            case SDL_TEXTINPUT: {
                TextInputEvent e{event, event.text.text};
                callEventHandler(event.text.windowID, &Sdl2ApplicationWindow::textInputEvent, e);
            } break;

            case SDL_TEXTEDITING: {
                TextEditingEvent e{event, event.edit.text, event.edit.start, event.edit.length};
                callEventHandler(event.edit.windowID, &Sdl2ApplicationWindow::textEditingEvent, e);
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

    /* Draw events */
    bool somethingDrawn = false;
    for(std::size_t i = 0; i != _windows.size(); ++i) {
        if(!_windows[i] || !(_windows[i]->_windowFlags & WindowFlag::Redraw)) continue;

        _windows[i]->_windowFlags &= ~WindowFlag::Redraw;
        callEventHandler(i,
            &Sdl2ApplicationWindow::drawEvent);
        somethingDrawn = true;
    }

    if(somethingDrawn) {
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

Sdl2ApplicationWindow::Configuration::Configuration():
    #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
    _title(Containers::String::nullTerminatedGlobalView("Magnum SDL2 Application"_s)),
    #endif
    #if !defined(CORRADE_TARGET_IOS) && !defined(CORRADE_TARGET_EMSCRIPTEN)
    _size{800, 600},
    #else
    _size{}, /* SDL2 detects someting for us */
    #endif
    _dpiScalingPolicy{DpiScalingPolicy::Default} {}

Sdl2ApplicationWindow::Configuration::~Configuration() = default;

#ifdef MAGNUM_BUILD_DEPRECATED
namespace {
    CORRADE_IGNORE_DEPRECATED_PUSH
    constexpr Sdl2ApplicationWindow::Configuration::WindowFlags DeprecatedWindowFlags = Sdl2ApplicationWindow::Configuration::WindowFlag::Contextless|Sdl2ApplicationWindow::Configuration::WindowFlag::OpenGL|Sdl2ApplicationWindow::Configuration::WindowFlag::Vulkan;
    CORRADE_IGNORE_DEPRECATED_POP
}
#endif

Sdl2Application::Configuration& Sdl2Application::Configuration::setWindowFlags(WindowFlags flags) {
    /* If deprecated flags are present, reset these bits in the application
       flags instead and remove them from the to-be-set window flags */
    #ifdef MAGNUM_BUILD_DEPRECATED
    if(const WindowFlags deprecatedFlags = flags & DeprecatedWindowFlags) {
        _flags &= ~Flags(Uint32(DeprecatedWindowFlags));
        _flags |= Flags(Uint32(deprecatedFlags));
        flags &= ~deprecatedFlags;
    }
    #endif

    Sdl2ApplicationWindow::Configuration::setWindowFlags(flags);
    return *this;
}

Sdl2Application::Configuration& Sdl2Application::Configuration::addWindowFlags(WindowFlags flags) {
    /* If deprecated flags are present, add these bits in the application
       flags instead and remove them from the to-be-added window flags */
    #ifdef MAGNUM_BUILD_DEPRECATED
    if(const WindowFlags deprecatedFlags = flags & DeprecatedWindowFlags) {
        _flags |= Flags(Uint32(deprecatedFlags));
        flags &= ~deprecatedFlags;
    }
    #endif

    Sdl2ApplicationWindow::Configuration::addWindowFlags(flags);
    return *this;
}

Sdl2Application::Configuration& Sdl2Application::Configuration::clearWindowFlags(WindowFlags flags) {
    /* If deprecated flags are present, add these bits in the application
       flags instead and remove them from the to-be-cleared window flags */
    #ifdef MAGNUM_BUILD_DEPRECATED
    if(const WindowFlags deprecatedFlags = flags & DeprecatedWindowFlags) {
        _flags &= ~Flags(Uint32(deprecatedFlags));
        flags &= ~deprecatedFlags;
    }
    #endif

    Sdl2ApplicationWindow::Configuration::clearWindowFlags(flags);
    return *this;
}

Containers::StringView Sdl2ApplicationWindow::KeyEvent::keyName(const Key key) {
    return SDL_GetKeyName(SDL_Keycode(key));
}

Containers::StringView Sdl2ApplicationWindow::KeyEvent::keyName() const {
    return keyName(_key);
}

Sdl2ApplicationWindow::InputEvent::Modifiers Sdl2ApplicationWindow::MouseEvent::modifiers() {
    if(_modifiers) return *_modifiers;
    return *(_modifiers = fixedModifiers(Uint16(SDL_GetModState())));
}

Sdl2ApplicationWindow::InputEvent::Modifiers Sdl2ApplicationWindow::MouseMoveEvent::modifiers() {
    if(_modifiers) return *_modifiers;
    return *(_modifiers = fixedModifiers(Uint16(SDL_GetModState())));
}

Vector2i Sdl2ApplicationWindow::MouseScrollEvent::position() {
    if(_position) return *_position;
    _position = Vector2i{};
    SDL_GetMouseState(&_position->x(), &_position->y());
    return *_position;
}

Sdl2ApplicationWindow::InputEvent::Modifiers Sdl2ApplicationWindow::MouseScrollEvent::modifiers() {
    if(_modifiers) return *_modifiers;
    return *(_modifiers = fixedModifiers(Uint16(SDL_GetModState())));
}

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

template class BasicScreen<Sdl2Application>;
template class BasicScreenedApplication<Sdl2Application>;

}}
