#ifndef Magnum_Platform_Sdl2Application_h
#define Magnum_Platform_Sdl2Application_h
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

/** @file
 * @brief Class @ref Magnum::Platform::Sdl2Application, macro @ref MAGNUM_SDL2APPLICATION_MAIN()
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h> /** @todo PIMPL Configuration instead? */

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Platform/Platform.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/Platform/GLContext.h"
#endif

#ifdef CORRADE_TARGET_WINDOWS /* Windows version of SDL2 redefines main(), we don't want that */
#define SDL_MAIN_HANDLED
#endif

#ifdef CORRADE_TARGET_CLANG_CL
/* SDL does #pragma pack(push,8) and #pragma pack(pop,8) in different headers
   (begin_code.h and end_code.h) and clang-cl doesn't like that, even though it
   is completely fine. Silence the warning. */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpragma-pack"
#endif
/* SDL.h includes the world, adding 50k LOC. We don't want that either. */
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_version.h> /* huh, why is this not pulled in implicitly?! */
#include <SDL_video.h>
#include <SDL_scancode.h>

#ifdef CORRADE_TARGET_IOS
/* Including SDL_main.h unconditionally would mean it'd override Corrade::Main
   on Windows (both are parsing wargv and converting them to UTF-8, but ours
   does that better and additionally enables ANSI colors and UTF-8 console
   output). Right now (SDL 2.0.11) it's only needed for WinRT (which is done
   below), Android (which we don't support for SDL) and iOS, so whitelist it
   only for iOS. */
#include <SDL_main.h>
#endif

#ifdef CORRADE_TARGET_WINDOWS_RT
#include <SDL_main.h> /* For SDL_WinRTRunApp */
#include <wrl.h> /* For the WinMain entrypoint */
#endif
#ifdef CORRADE_TARGET_CLANG_CL
#pragma clang diagnostic pop
#endif

#ifdef MAGNUM_BUILD_DEPRECATED
/* Some APIs used to take or return a std::string before */
#include <Corrade/Containers/StringStl.h>
#endif

#if defined(CORRADE_TARGET_EMSCRIPTEN) || defined(DOXYGEN_GENERATING_OUTPUT)
/* The __EMSCRIPTEN_major__ etc macros used to be passed implicitly, version
   3.1.4 moved them to a version header and version 3.1.23 dropped the
   backwards compatibility. To work consistently on all versions, including the
   header only if the version macros aren't present.
   https://github.com/emscripten-core/emscripten/commit/f99af02045357d3d8b12e63793cef36dfde4530a
   https://github.com/emscripten-core/emscripten/commit/f76ddc702e4956aeedb658c49790cc352f892e4c */
#ifndef __EMSCRIPTEN_major__
#include <emscripten/version.h>
#endif
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
union SDL_Event; /* for anyEvent() */
#endif

namespace Magnum { namespace Platform {

namespace Implementation {
    enum class Sdl2DpiScalingPolicy: UnsignedByte;
}

/** @nosubgrouping
@brief SDL2 application

@m_keywords{Application}

Application using the [Simple DirectMedia Layer](http://www.libsdl.org/)
toolkit. Supports keyboard and mouse handling. This application library is
available for all platforms for which SDL2 is ported except Android (thus is
avaiable also on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten", see respective
sections in @ref building-corrade-cross-emscripten "Corrade's" and
@ref building-cross-emscripten "Magnum's" building documentation).

@m_class{m-block m-success}

@thirdparty This plugin makes use of the [SDL2](https://www.libsdl.org/)
    library, released under the @m_class{m-label m-success} **zlib license**
    ([license text](http://www.gzip.org/zlib/zlib_license.html),
    [choosealicense.com](https://choosealicense.com/licenses/zlib/)).
    Attribution is appreciated but not required.

@section Platform-Sdl2Application-bootstrap Bootstrap application

Fully contained base application using @ref Sdl2Application along with
CMake setup is available in `base` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base.zip) file.
After extracting the downloaded archive you can build and run the application
with these four commands:

@code{.sh}
mkdir build && cd build
cmake ..
cmake --build .
./src/MyApplication # or ./src/Debug/MyApplication
@endcode

See @ref cmake for more information.

@section Platform-Sdl2Application-bootstrap-emscripten Bootstrap application for Emscripten

The dedicated application implementation for Emscripten is
@ref EmscriptenApplication, which also provides a bootstrap project along with
full HTML markup and CMake setup. @ref Sdl2Application however supports
Emscripten as well --- set up the bootstrap application as
@ref Platform-EmscriptenApplication-bootstrap "described in the EmscriptenApplication docs"
and then change `src/CMakeLists.txt` and the @cpp #include @ce to use
@ref Sdl2Application for both the native and the web build.

@section Platform-Sdl2Application-bootstrap-ios Bootstrap application for iOS

Fully contained base application using @ref Sdl2Application for both desktop
and iOS build along with pre-filled `*.plist` is available in `base-ios` branch
of [Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-ios.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-ios.zip) file.
After extracting the downloaded archive, you can do the desktop build in
the same way as above. For the iOS build you also need to put the contents of
toolchains repository from https://github.com/mosra/toolchains in `toolchains/`
subdirectory.

Then create build directory and run `cmake` to generate the Xcode project. Set
`CMAKE_OSX_ROOT` to SDK you want to target and enable all desired architectures
in `CMAKE_OSX_ARCHITECTURES`. Set `CMAKE_PREFIX_PATH` to the directory where
you have all the dependencies.

@code{.sh}
mkdir build-ios && cd build-ios
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=path/to/toolchains/generic/iOS.cmake \
    -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk \
    -DCMAKE_OSX_ARCHITECTURES="arm64;armv7;armv7s" \
    -DCMAKE_PREFIX_PATH=~/ios-libs \
    -G Xcode
@endcode

You can then open the generated project file in Xcode and build/deploy it from
there.

@section Platform-Sdl2Application-bootstrap-winrt Bootstrap application for Windows RT

Fully contained base application using @ref Sdl2Application for both desktop
and Windows Phone / Windows Store build along with all required plumbing is
available in `base-winrt` branch of [Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap)
repository, download it as [zip](https://github.com/mosra/magnum-bootstrap/archive/base-winrt.zip)
file. After extracting the downloaded archive, you can do the desktop build in
the same way as above.

For the Windows RT build you need to provide [your own *.pfx certificate file](https://msdn.microsoft.com/en-us/library/windows/desktop/jj835832.aspx) and
pass it to CMake in a `SIGNING_CERTIFICATE` variable. The bootstrap application
assumes that SDL2 and ANGLE is built as DLL and both Corrade and Magnum are
built statically. Assuming the native Corrade installation is in `C:/Sys` and
all WinRT dependencies are in `C:/Sys-winrt`, the build can be done similarly
to the following:

@code{.bat}
mkdir build-winrt && cd build-winrt
cmake .. ^
    -DCORRADE_RC_EXECUTABLE="C:/Sys/bin/corrade-rc.exe" ^
    -DCMAKE_PREFIX_PATH="C:/Sys-winrt" ^
    -DCMAKE_SYSTEM_NAME=WindowsStore ^
    -DCMAKE_SYSTEM_VERSION=8.1 ^
    -G "Visual Studio 14 2015" ^
    -DSIGNING_CERTIFICATE=<path-to-your-pfx-file>
cmake --build .
@endcode

Change `WindowsStore` to `WindowsPhone` if you want to build for Windows Phone
instead. The `build-winrt/src/AppPackages` directory will then contain the
final package along with a PowerShell script for easy local installation.

@section Platform-Sdl2Application-usage General usage

This application library depends on the [SDL2](http://www.libsdl.org) library
version 2.0.6 and newer. On Emscripten, the builtin minimal SDL implementation
is used. The application library is built if `MAGNUM_WITH_SDL2APPLICATION` is
enabled when building Magnum. To use this library with CMake, request the
`Sdl2Application` component of the `Magnum` package and link to the
`Magnum::Sdl2Application` target:

@code{.cmake}
find_package(Magnum REQUIRED Sdl2Application)

# ...
target_link_libraries(your-app PRIVATE Magnum::Sdl2Application)
@endcode

Additionally, if you're using Magnum as a CMake subproject, bundle the
[SDL repository](https://github.com/libsdl-org/SDL) and do the following
* *before* calling @cmake find_package() @ce to ensure it's enabled, as the
library is not built by default. If you want to use system-installed SDL2,
omit the first part and point `CMAKE_PREFIX_PATH` to its installation dir if
necessary.

@code{.cmake}
# This is the most minimal set of features which still make Sdl2Application
# work. If you need something from these, remove the setting. The SDL_AUDIO and
# SDL_EVENT options should not be needed either as Magnum doesn't use them, but
# if they're disabled they causes compiler or linker errors. Either SDL_DLOPEN
# or SDL_LOADSO needs to be enabled depending on the system to allow linking
# dependencies at runtime, so it's better to just leave them both on. The
# SDL_TIMERS option is important for rendering performance.
set(SDL_ATOMIC OFF CACHE BOOL "" FORCE)
set(SDL_CPUINFO OFF CACHE BOOL "" FORCE)
set(SDL_FILE OFF CACHE BOOL "" FORCE)
set(SDL_FILESYSTEM OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_LOCALE OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_RENDER OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
# This assumes you want to have SDL as a static library. If not, set SDL_STATIC
# to OFF instead.
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(SDL EXCLUDE_FROM_ALL)

set(MAGNUM_WITH_SDL2APPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

<b></b>

@m_class{m-note m-warning}

@par
    While SDL itself, being a C project, builds quite fast, when using it as a
    CMake subproject be prepared that it will *significantly* increase the
    CMake configure time due to excessive platform checks, and pollute the
    CMake option list with a lot of unprefixed SDL-specific options.

If no other application is requested, you can also use the generic
`Magnum::Application` alias to simplify porting. Again, see @ref building and
@ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass can be then used directly in `main()` --- see
convenience macro @ref MAGNUM_SDL2APPLICATION_MAIN(). See @ref platform for
more information.

@code{.cpp}
class MyApplication: public Platform::Sdl2Application {
    // implement required methods...
};
MAGNUM_SDL2APPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
@cpp Platform::Application @ce and the macro is aliased to @cpp MAGNUM_APPLICATION_MAIN() @ce
to simplify porting.

@section Platform-Sdl2Application-touch Touch input in SDL2

The application recognizes touch input and reports it as @ref Pointer::Finger
and @ref PointerEventSource::Touch. Because both mouse and touch events are
exposed through a unified @ref PointerEvent / @ref PointerMoveEvent interface,
there's no need for compatibility mouse events to be synthesized from touch
events and vice versa, and thus given behavior is disabled in SDL. Pen input is
still reported as a mouse because SDL has dedicated support for pen stylus only
since SDL 3.

In case of a multi-touch scenario, @ref PointerEvent::isPrimary() /
@ref PointerMoveEvent::isPrimary() can be used to distinguish the primary touch
from secondary. For example, if an application doesn't need to recognize
gestures like pinch to zoom or rotate, it can ignore all non-primary pointer
events. @ref PointerEventSource::Mouse events are always marked as primary,
for touch input the first pressed finger is marked as primary and all following
pressed fingers are non-primary. Note that there can be up to one primary
pointer for each pointer event source, e.g. a finger and a mouse press may both
be marked as primary. On the other hand, in a multi-touch scenario, if the
first (and thus primary) finger is lifted, no other finger becomes primary
until all others are lifted as well. The same logic is implemented in
@ref EmscriptenApplication and @ref AndroidApplication.

If gesture recognition is desirable, @ref PointerEvent::id() /
@ref PointerMoveEvent::id() contains a pointer ID that's unique among all
pointer event sources, which can be used to track movements of secondary,
tertiary and further touch points. The ID allocation is platform-specific and
you can't rely on it to be contiguous or in any bounded range --- for example,
each new touch may generate a new ID that's only used until given finger is
lifted, and then never again. For @ref PointerEventSource::Mouse the ID is a
constant, as there's always just a single mouse cursor.

See also @ref platform-windowed-pointer-events for general information about
handling pointer input in a portable way. There's also a
@ref Platform::TwoFingerGesture helper for recognition of common two-finger
gestures for zoom, rotation and pan.

@section Platform-Sdl2Application-platform-specific Platform-specific behavior

@subsection Platform-Sdl2Application-platform-specific-power Power management

SDL by default prevents the computer from powering off the or screen going to
sleep. While possibly useful for game-like use cases, it's generally
undesirable for regular applications. @ref Sdl2Application turns this behavior
off. You can restore SDL's default behavior by disabling the
[corresponding SDL hint](https://wiki.libsdl.org/SDL2/CategoryHints) through an
environment variable or through @cpp SDL_SetHint() @ce from your application.

@code{.sh}
SDL_VIDEO_ALLOW_SCREENSAVER=0 ./your-app
@endcode

@subsection Platform-Sdl2Application-platform-specific-posix POSIX specifics

On POSIX systems, SDL by default intercepts the `SIGTERM` signal and generates
an exit event for it, instead of doing the usual application exit. This would
mean that if the application fails to set @ref ExitEvent::setAccepted() in an
@ref exitEvent() override for some reason, pressing
@m_class{m-label m-warning} **Ctrl** @m_class{m-label m-default} **C** would
not terminate it either and you'd have to forcibly kill it instead.
@ref Sdl2Application turns this behavior off, making @ref exitEvent() behave
consistently with other application implementations such as
@ref GlfwApplication. You can turn this behavior back on by enabling the
[corresponding SDL hint](https://wiki.libsdl.org/SDL2/SDL_HINT_NO_SIGNAL_HANDLERS)
through an environment variable:

@code{.sh}
SDL_NO_SIGNAL_HANDLERS=1 ./your-app
@endcode

See also the [SDL Wiki](https://wiki.libsdl.org/SDL2/SDL_EventType#SDL_QUIT) for
details.

@subsection Platform-Sdl2Application-platform-specific-linux Linux specifics

SDL by default attempts to disable compositing, which may cause ugly flickering
for non-fullscreen apps (KWin, among others, is known to respect this setting).
When using SDL >= 2.0.8, @ref Sdl2Application turns this behavior off, keeping
the compositor running to avoid the flicker. You can turn this behavior back on
by enabling the [corresponding SDL hint](https://wiki.libsdl.org/SDL2/CategoryHints)
through an environment variable or through @cpp SDL_SetHint() @ce from your
application.

@code{.sh}
SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR=1 ./your-app
@endcode

If you're running an older version of SDL, you can disallow apps from bypassing
the compositor in system-wide KWin settings.

@subsection Platform-Sdl2Application-platform-specific-ios iOS specifics

Leaving a default (zero) window size in @ref Configuration will cause the app
to autodetect it based on the actual device screen size. This also depends on
@ref Platform-Sdl2Application-dpi "DPI awareness", see below for details.

As noted in the @ref platforms-ios-bundle "iOS platform guide", a lot of
options needs to be set via a `*.plist` file. Some options can be configured
from runtime when creating the SDL2 application window, see documentation of
a particular value for details:

-   @ref Configuration::WindowFlag::Borderless hides the menu bar
-   @ref Configuration::WindowFlag::Resizable makes the application respond to
    device orientation changes

@subsection Platform-Sdl2Application-platform-specific-emscripten Emscripten specifics

On Emscripten, the builtin minimal SDL implementation is used, which is
sufficiently complete, yet without a too large impact on binary sizes. In
particular, the full SDL2 implementation, available when the `-s USE_SDL=2`
option is set, is *not* used, as it only significantly increases the generated
binary size without really offering more. In any case, @ref EmscriptenApplication
is now the preferred application implementation for the web. It offers a
broader range of features, more efficient idle behavior and smaller code size.

Leaving a default (zero) window size in @ref Configuration will cause the app
to use a window size that corresponds to *CSS pixel size* of the
@cb{.html} <canvas> @ce element. The size is then multiplied by DPI scaling
value, see @ref Platform-Sdl2Application-dpi "DPI awareness" below for details.

If you enable @ref Configuration::WindowFlag::Resizable, the canvas will be
resized when size of the canvas changes and you get @ref viewportEvent(). If
the flag is not enabled, no canvas resizing is performed.

@subsection Platform-Sdl2Application-platform-specific-gles OpenGL ES specifics

For OpenGL ES, SDL2 defaults to a "desktop GLES" context of the system driver.
Because Magnum has the opposite default behavior, if @ref MAGNUM_TARGET_GLES is
not defined, @ref Sdl2Application sets the `SDL_HINT_OPENGL_ES_DRIVER` hint to
1, forcing it to load symbols from a dedicated libGLES library instead, making
SDL and Magnum consistently use the same OpenGL entrypoints. This change also
allows @ref platforms-gl-es-angle "ANGLE" to be used on Windows simply by
placing the corresponding `libEGL.dll` and `libGLESv2.dll` files next to the
application executable.

@section Platform-Sdl2Application-dpi DPI awareness

On displays that match the platform default DPI (96 or 72),
@ref Configuration::setSize() will create the window in exactly the requested
size and the framebuffer pixels will match display pixels 1:1. On displays that
have different DPI, there are three possible scenarios, listed below. It's
possible to fine tune the behavior either using extra parameters passed to
@ref Configuration::setSize() or via the `--magnum-dpi-scaling` command-line
option (or the equivalent @cb{.sh} $MAGNUM_DPI_SCALING @ce environment
variable).

-   Framebuffer DPI scaling. The window is created with exactly the requested
    size and all event coordinates are reported also relative to that size.
    However, the window backing framebuffer has a different size. This is only
    supported on macOS and iOS. See @ref platforms-macos-hidpi for details how
    to enable it. Equivalent to passing
    @ref Configuration::DpiScalingPolicy::Framebuffer to
    @ref Configuration::setSize() or `framebuffer` via command line /
    environment.
-   Virtual DPI scaling. Scales the window based on DPI scaling setting in the
    system. For example if a 800x600 window is requested and DPI scaling is set
    to 200%, the resulting window will have 1600x1200 pixels. The backing
    framebuffer will have the same size. This is supported on Linux and
    Windows; on Windows the application is first checked for DPI awareness
    as described in @ref platforms-windows-hidpi and if the application is not
    DPI-aware, 1:1 scaling is used. Equivalent to passing
    @ref Configuration::DpiScalingPolicy::Virtual to
    @ref Configuration::setSize() or `virtual` on command line.
-   Physical DPI scaling. Takes the requested window size as a physical size
    that a window would have on platform's default DPI and scales it to have
    the same physical size on given display physical DPI. So, for example on a
    display with 240 DPI the window size will be 2000x1500 in pixels, but it
    will be 21 centimeters wide, the same as a 800x600 window would be on a 96
    DPI display. On platforms that don't have a concept of a window (such
    as mobile platforms or @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten"), it
    causes the framebuffer to match display pixels 1:1 without any scaling.
    This is supported on Linux and all mobile platforms (except iOS) and
    Emscripten. On Windows this is equivalent to virtual DPI scaling but
    without doing an explicit check for DPI awareness first. Equivalent to
    passing @ref Configuration::DpiScalingPolicy::Physical to
    @ref Configuration::setSize() or `physical` via command line / environment.

Besides the above, it's possible to supply a custom DPI scaling value to
@ref Configuration::setSize() or the `--magnum-dpi-scaling` command-line
option (or environment variable). Using `--magnum-dpi-scaling &lt;number&gt;`
will make the scaling same in both directions, with
`--magnum-dpi-scaling "<horizontal> <vertical>"` the scaling will be different
in each direction. On desktop systems custom DPI scaling value will affect
physical window size (with the content being scaled), on mobile and web it will
affect sharpness of the contents.

The default is depending on the platform:

-   On macOS and iOS, the default and only supported option is
    @ref Configuration::DpiScalingPolicy::Framebuffer. On this platform,
    @ref windowSize() and @ref framebufferSize() will differ depending on
    whether `NSHighResolutionCapable` is enabled in the `*.plist` file or not.
    By default, @ref dpiScaling() is @cpp 1.0f @ce in both dimensions but it
    can be overridden using custom DPI scaling.
-   On Windows, the default is @ref Configuration::DpiScalingPolicy::Framebuffer.
    The @ref windowSize() and @ref framebufferSize() is always the same.
    Depending on whether the DPI awareness was enabled in the manifest file or
    set by the `SetProcessDpiAwareness()` API, @ref dpiScaling() is either
    @cpp 1.0f @ce in both dimensions, indicating a low-DPI screen or a
    non-DPI-aware app, or some other value for HiDPI screens. In both cases the
    value can be overridden using custom DPI scaling.
-   On Linux, the default is @ref Configuration::DpiScalingPolicy::Virtual,
    taken from the `Xft.dpi` property. If the property is not available, it
    falls back to @ref Configuration::DpiScalingPolicy::Physical, querying the
    monitor DPI value. The @ref windowSize() and @ref framebufferSize() is
    always the same, @ref dpiScaling() contains the queried DPI scaling value.
    The value can be overridden using custom DPI scaling.
-   On @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten", the default is physical DPI
    scaling, taken from [Window.getDevicePixelRatio()](https://developer.mozilla.org/en-US/docs/Web/API/Window/devicePixelRatio). The
    @ref windowSize() and @ref framebufferSize() is always the same,
    @ref dpiScaling() contains the queried DPI scaling value. The value can be
    overridden using custom DPI scaling. Note that this is different from the
    behavior in @ref EmscriptenApplication --- Emscripten's SDL implementation
    has some additional emulation code that reports event coordinates in
    framebuffer pixels instead of CSS pixels. See
    @ref Platform-EmscriptenApplication-dpi "EmscriptenApplication DPI awareness docs"
    for more information.

With @ref windowSize(), @ref framebufferSize() and @ref dpiScaling() having a
different relation on each platform, the way to calculate context scaling
consistently everywhere is using this expression:

@snippet Platform.cpp Sdl2Application-dpi-scaling

If your application is saving and restoring window size, it's advisable to take
@ref dpiScaling() into account:

-   Either divide the window size by the DPI scaling value and use that to
    restore the window next time --- but note this might accumulate slight
    differences in window sizes over time, especially if fractional scaling is
    involved.
-   Or save the scaled size and use @ref Configuration::setSize(const Vector2i&, const Vector2&)
    with @cpp 1.0f @ce as custom DPI scaling next time --- but this doesn't
    properly handle cases where the window is opened on a display with
    different DPI.
*/
class Sdl2Application {
    public:
        /** @brief Application arguments */
        struct Arguments {
            /** @brief Constructor */
            /*implicit*/ constexpr Arguments(int& argc, char** argv) noexcept: argc{argc}, argv{argv} {}

            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        class Configuration;
        #ifdef MAGNUM_TARGET_GL
        class GLConfiguration;
        #endif
        class ExitEvent;
        class ViewportEvent;
        class FocusEvent;
        class InputEvent;
        class KeyEvent;
        class PointerEvent;
        class PointerMoveEvent;
        class ScrollEvent;
        #ifdef MAGNUM_BUILD_DEPRECATED
        class MouseEvent;
        class MouseMoveEvent;
        class MouseScrollEvent;
        class MultiGestureEvent;
        #endif
        class TextInputEvent;
        class TextEditingEvent;

        /* The damn thing cannot handle forward enum declarations */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        enum class Modifier: Uint16;
        enum class Key: SDL_Keycode;
        enum class PointerEventSource: UnsignedByte;
        enum class Pointer: UnsignedByte;
        #endif

        /**
         * @brief Set of keyboard modifiers
         * @m_since_latest
         *
         * @see @ref KeyEvent::modifiers(), @ref PointerEvent::modifiers(),
         *      @ref PointerMoveEvent::modifiers(),
         *      @ref ScrollEvent::modifiers(),
         *      @ref platform-windowed-key-events
         */
        typedef Containers::EnumSet<Modifier> Modifiers;

        /**
         * @brief Set of pointer types
         * @m_since_latest
         *
         * @see @ref PointerMoveEvent::pointers(),
         *      @ref platform-windowed-pointer-events
         */
        typedef Containers::EnumSet<Pointer> Pointers;

        /**
         * @brief Name for given key
         * @m_since_latest
         *
         * Human-readable localized UTF-8 name for given @p key, intended for
         * displaying to the user in e.g. a key binding configuration. If there
         * is no name for given key, empty string is returned. The returned
         * view is always @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * and is valid at least until the next call to this function, to
         * @ref KeyEvent::keyName() const or to the underlying
         * @cpp SDL_GetKeyName() @ce API.
         * @see @ref scanCodeName(), @ref keyToScanCode(), @ref KeyEvent::key()
         */
        static Containers::StringView keyName(Key key);

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Name for given key scan code
         * @m_since_latest
         *
         * Human-readable localized UTF-8 name for given @p scancode. Note that
         * unlike @ref keyName(), the scancode names are not consistent across
         * platforms. If there is no name for given scancode, empty string is
         * returned. The returned view is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * is valid at least until the next call to this function, to
         * @ref KeyEvent::scanCodeName() const or to the underlying
         * @cpp SDL_GetScancodeName() @ce API.
         * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref scanCodeToKey(), @ref KeyEvent::scanCode()
         */
        static Containers::StringView scanCodeName(UnsignedInt scanCode);
        #endif

        #if !defined(CORRADE_TARGET_EMSCRIPTEN) || __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ >= 30125
        /**
         * @brief Scan code for given key
         * @m_since_latest
         *
         * If @p key doesn't correspond to a physical key supported on given
         * platform, returns @relativeref{Corrade,Containers::NullOpt}. Note
         * that this is implemented as a linear lookup inside SDL, prefer to
         * query the scan code directly via @ref KeyEvent::scanCode() rather
         * than converting it from a @ref KeyEvent::key() at a later time.
         * @note On @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" available only
         *      since version 3.1.25.
         * @see @ref scanCodeToKey(), @ref keyName()
         */
        static Containers::Optional<UnsignedInt> keyToScanCode(Key key);
        #endif

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Scan code for given key
         * @m_since_latest
         *
         * If @p scanCode isn't a known scan code, returns
         * @relativeref{Corrade,Containers::NullOpt}.
         * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref KeyEvent::key(), @ref KeyEvent::scanCode(),
         *      @ref keyToScanCode(), @ref scanCodeName()
         */
        static Containers::Optional<Key> scanCodeToKey(UnsignedInt scanCode);
        #endif

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Construct with an OpenGL context
         * @param arguments         Application arguments
         * @param configuration     Application configuration
         * @param glConfiguration   OpenGL context configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref Configuration for more information. The program exits if
         * the context cannot be created, see @ref tryCreate() for an
         * alternative.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         */
        explicit Sdl2Application(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration);
        #endif

        /**
         * @brief Construct without explicit GPU context configuration
         *
         * If @ref Configuration::WindowFlag::Contextless is present or Magnum
         * was not built with @ref MAGNUM_TARGET_GL, this creates a window
         * without any GPU context attached, leaving that part on the user.
         *
         * If none of the flags is present and Magnum was built with
         * @ref MAGNUM_TARGET_GL, this is equivalent to calling
         * @ref Sdl2Application(const Arguments&, const Configuration&, const GLConfiguration&)
         * with default-constructed @ref GLConfiguration.
         *
         * See also @ref building-features for more information.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit Sdl2Application(const Arguments& arguments, const Configuration& configuration = Configuration{});
        #else
        /* Configuration is only forward-declared at this point */
        explicit Sdl2Application(const Arguments& arguments, const Configuration& configuration);
        explicit Sdl2Application(const Arguments& arguments);
        #endif

        /**
         * @brief Construct without creating a window
         * @param arguments     Application arguments
         *
         * Unlike above, the window is not created and must be created later
         * with @ref create() or @ref tryCreate().
         */
        explicit Sdl2Application(const Arguments& arguments, NoCreateT);

        /** @brief Copying is not allowed */
        Sdl2Application(const Sdl2Application&) = delete;

        /** @brief Moving is not allowed */
        Sdl2Application(Sdl2Application&&) = delete;

        /** @brief Copying is not allowed */
        Sdl2Application& operator=(const Sdl2Application&) = delete;

        /** @brief Moving is not allowed */
        Sdl2Application& operator=(Sdl2Application&&) = delete;

        /**
         * @brief Execute application main loop
         * @return Value for returning from @cpp main() @ce
         *
         * Calls @ref mainLoopIteration() in a loop until @ref exit() is
         * called. See @ref MAGNUM_SDL2APPLICATION_MAIN() for usage
         * information.
         */
        int exec();

        /**
         * @brief Run one iteration of application main loop
         * @return @cpp false @ce if @ref exit() was called and the application
         *      should exit, @cpp true @ce otherwise
         *
         * Called internally from @ref exec(). If you want to have better
         * control over how the main loop behaves, you can call this function
         * yourself from your own `main()` function instead of it being called
         * automatically from @ref exec() / @ref MAGNUM_SDL2APPLICATION_MAIN().
         */
        bool mainLoopIteration();

        /**
         * @brief Exit application
         * @param exitCode  The exit code the application should return
         *
         * When called from application constructor, it will cause the
         * application to exit immediately after constructor ends, without
         * entering the event loop. When called from within an event handler,
         * it will cause it to exit at the start of next event loop iteration.
         * Compared to requesting an application exit using the window close
         * button or the @m_class{m-label m-default} **Alt**
         * @m_class{m-label m-default} **F4** /
         * @m_class{m-label m-default} **Cmd**
         * @m_class{m-label m-default} **Q** keyboard shortcut, the
         * @ref exitEvent() *isn't* called when using this function.
         *
         * Calling this function from an application constructor is recommended
         * over @ref std::exit() or @ref Corrade::Utility::Fatal "Fatal", which
         * exit without calling destructors on local scope. Note that, however,
         * you need to explicitly @cpp return @ce after calling it, as it can't
         * exit the constructor on its own:
         *
         * @snippet Platform.cpp exit-from-constructor
         */
        void exit(int exitCode = 0);

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Underlying window handle
         *
         * Use in case you need to call SDL functionality directly. Returns
         * @cpp nullptr @ce in case the window was not created yet.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        SDL_Window* window() { return _window; }
        #endif

        #if defined(MAGNUM_TARGET_GL) && !defined(CORRADE_TARGET_EMSCRIPTEN)
        /**
         * @brief Underlying OpenGL context
         * @m_since{2019,10}
         *
         * Use in case you need to call SDL functionality directly. Returns
         * @cpp nullptr @ce in case the context was not created yet.
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information. Not available in
         *      @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        SDL_GLContext glContext() { return _glContext; }
        #endif

    protected:
        /* Nobody will need to have (and delete) Sdl2Application*, thus this is
           faster than public pure virtual destructor */
        ~Sdl2Application();

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Create a window with given configuration for OpenGL context
         * @param configuration     Application configuration
         * @param glConfiguration   OpenGL context configuration
         *
         * Must be called if and only if the context wasn't created by the
         * constructor itself, i.e. when passing @ref NoCreate to it. Error
         * message is printed and the program exits if the context cannot be
         * created, see @ref tryCreate() for an alternative.
         *
         * On desktop GL, if version is not specified in @p glConfiguration,
         * the application first tries to create core context (OpenGL 3.2+ on
         * macOS, OpenGL 3.1+ elsewhere) and if that fails, falls back to
         * compatibility OpenGL 2.1 context.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         */
        void create(const Configuration& configuration, const GLConfiguration& glConfiguration);
        #endif

        /**
         * @brief Create a window with given configuration
         *
         * If @ref Configuration::WindowFlag::Contextless is present or Magnum
         * was not built with @ref MAGNUM_TARGET_GL, this creates a window
         * without any GPU context attached, leaving that part on the user.
         *
         * If none of the flags is present and Magnum was built with
         * @ref MAGNUM_TARGET_GL, this is equivalent to calling
         * @ref create(const Configuration&, const GLConfiguration&) with
         * default-constructed @ref GLConfiguration.
         *
         * See also @ref building-features for more information.
         */
        void create(const Configuration& configuration);

        /**
         * @brief Create a window with default configuration and OpenGL context
         *
         * Equivalent to calling @ref create(const Configuration&) with
         * default-constructed @ref Configuration.
         */
        void create();

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Try to create context with given configuration for OpenGL context
         *
         * Unlike @ref create(const Configuration&, const GLConfiguration&)
         * returns @cpp false @ce if the context cannot be created,
         * @cpp true @ce otherwise.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         */
        bool tryCreate(const Configuration& configuration, const GLConfiguration& glConfiguration);
        #endif

        /**
         * @brief Try to create context with given configuration
         *
         * Unlike @ref create(const Configuration&) returns @cpp false @ce if
         * the context cannot be created, @cpp true @ce otherwise.
         */
        bool tryCreate(const Configuration& configuration);

        /** @{ @name Screen handling */

    public:
        /**
         * @brief Window size
         *
         * Window size to which all input event coordinates can be related.
         * Note that, especially on HiDPI systems, it may be different from
         * @ref framebufferSize(). Expects that a window is already created.
         * See @ref Platform-Sdl2Application-dpi for more information.
         * @see @ref dpiScaling()
         */
        Vector2i windowSize() const;

        #if !defined(CORRADE_TARGET_EMSCRIPTEN) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Set window size
         * @param size    The size, in screen coordinates
         * @m_since{2020,06}
         *
         * To make the sizing work independently of the display DPI, @p size is
         * internally multiplied with @ref dpiScaling() before getting applied.
         * Expects that a window is already created.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref setMinWindowSize(), @ref setMaxWindowSize()
         */
        void setWindowSize(const Vector2i& size);

        /**
         * @brief Set minimum window size
         * @param size    The minimum size, in screen coordinates
         * @m_since{2019,10}
         *
         * Note that, unlike in @ref GlfwApplication, SDL2 doesn't have a way
         * to disable/remove a size limit. To make the sizing work
         * independently of the display DPI, @p size is internally multiplied
         * with @ref dpiScaling() before getting applied. Expects that a window
         * is already created.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref setMaxWindowSize(), @ref setWindowSize()
         */
        void setMinWindowSize(const Vector2i& size);

        /**
         * @brief Set maximal window size
         * @param size    The maximum size, in screen coordinates
         * @m_since{2019,10}
         *
         * Note that, unlike in @ref GlfwApplication, SDL2 doesn't have a way
         * to disable/remove a size limit. To make the sizing work
         * independently of the display DPI, @p size is internally multiplied
         * with @ref dpiScaling() before getting applied. Expects that a window
         * is already created.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref setMinWindowSize(), @ref setMaxWindowSize()
         */
        void setMaxWindowSize(const Vector2i& size);
        #endif

        #if defined(MAGNUM_TARGET_GL) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Framebuffer size
         *
         * Size of the default framebuffer. Note that, especially on HiDPI
         * systems, it may be different from @ref windowSize(). Expects that a
         * window is already created. See @ref Platform-Sdl2Application-dpi for
         * more information.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         *
         * @see @ref Sdl2Application::framebufferSize(), @ref dpiScaling()
         */
        Vector2i framebufferSize() const;
        #endif

        /**
         * @brief DPI scaling
         *
         * How the content should be scaled relative to system defaults for
         * given @ref windowSize(). If a window is not created yet, returns
         * zero vector, use @ref dpiScaling(const Configuration&) for
         * calculating a value independently. See @ref Platform-Sdl2Application-dpi
         * for more information.
         * @see @ref framebufferSize()
         */
        Vector2 dpiScaling() const;

        /**
         * @brief DPI scaling for given configuration
         *
         * Calculates DPI scaling that would be used when creating a window
         * with given @p configuration. Takes into account DPI scaling policy
         * and custom scaling specified on the command-line. See
         * @ref Platform-Sdl2Application-dpi for more information.
         */
        Vector2 dpiScaling(const Configuration& configuration);

        /**
         * @brief Set window title
         * @m_since{2019,10}
         *
         * The @p title is expected to be encoded in UTF-8.
         */
        void setWindowTitle(Containers::StringView title);

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Set window icon
         * @m_since{2020,06}
         *
         * The @p image is expected to be with origin at bottom left (which is
         * the default for imported images) and in one of
         * @ref PixelFormat::RGB8Unorm, @ref PixelFormat::RGB8Srgb,
         * @ref PixelFormat::RGBA8Unorm or @ref PixelFormat::RGBA8Srgb formats.
         * Unlike @ref GlfwApplication::setWindowIcon(), SDL doesn't provide a
         * way to supply multiple images in different sizes.
         * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten",
         *      use @cb{.html} <link rel="icon"> @ce in your HTML markup
         *      instead. Although it's not documented in SDL itself, the
         *      function might have no effect on macOS / Wayland, similarly to
         *      how @ref GlfwApplication::setWindowIcon() behaves on those
         *      platforms.
         * @see @ref platform-windows-icon "Excecutable icon on Windows",
         *      @ref Trade::IcoImporter "IcoImporter"
         */
        void setWindowIcon(const ImageView2D& image);
        #endif

        #if defined(CORRADE_TARGET_EMSCRIPTEN) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Set container CSS class
         *
         * Assigns given CSS class to the @cb{.html} <div class="mn-container"> @ce
         * enclosing the application @cb{.html} <canvas> @ce. Useful for
         * example to change aspect ratio of the view or stretch it to cover
         * the full page. See @ref platforms-html5-layout for more information
         * about possible values. Note that this replaces any existing class
         * (except for @cb{.css} .mn-container @ce, which is kept), to set
         * multiple classes separate them with whitespace.
         *
         * @note Only available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         *
         * @m_class{m-note m-danger}
         *
         * @par
         *      For backwards compatibility purposes the function will look for
         *      *any* @cb{.html} <div id="container"> @ce in case the
         *      @cb{.html} <div class="mn-container"> @ce is not found. This
         *      compatibility is scheduled to be removed in the future.
         */
        void setContainerCssClass(Containers::StringView cssClass);
        #endif

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         * @see @ref setSwapInterval()
         */
        void swapBuffers();

        /** @brief Swap interval */
        Int swapInterval() const;

        /**
         * @brief Set swap interval
         *
         * Set @cpp 0 @ce for no VSync, @cpp 1 @ce for enabled VSync. Some
         * platforms support @cpp -1 @ce for late swap tearing. Prints error
         * message and returns @cpp false @ce if swap interval cannot be set,
         * @cpp true @ce otherwise. Default is driver-dependent, you can query
         * the value with @ref swapInterval().
         * @see @ref setMinimalLoopPeriod()
         */
        bool setSwapInterval(Int interval);

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Set minimal loop period
         * @m_since_latest
         *
         * This setting reduces the main loop frequency in case
         * @ref setSwapInterval() wasn't called at all, was called with
         * @cpp 0 @ce, the call failed, or no drawing is done and just
         * @ref tickEvent() is being executed. The @p time is expected to be
         * non-negative, default is @cpp 0_nsec @ce (i.e., looping at maximum
         * frequency). If the application is drawing on the screen and VSync
         * was enabled by calling @ref setSwapInterval(), this setting is
         * ignored.
         *
         * Note that SDL timer resolution is just milliseconds, so anything
         * below @cpp 1.0_msec @ce will behave the same as @cpp 0_nsec @ce. As
         * the VSync default is driver-dependent, @ref setSwapInterval() has to
         * be explicitly called to make the interaction between the two work
         * correctly.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten",
         *      the browser is managing the frequency instead.
         */
        void setMinimalLoopPeriod(Nanoseconds time);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief setMinimalLoopPeriod(Nanoseconds)
         * @m_deprecated_since_latest Use @ref setMinimalLoopPeriod(Nanoseconds),
         *      which prevents unit mismatch errors, instead.
         */
        CORRADE_DEPRECATED("use setMinimalLoopPeriod(Nanoseconds) instead") void setMinimalLoopPeriod(UnsignedInt milliseconds);
        #endif
        #endif

        /**
         * @brief Redraw immediately
         *
         * Marks the window for redrawing, resulting in call to @ref drawEvent()
         * in the next iteration. You can call it from @ref drawEvent() itself
         * to redraw immediately without waiting for user input.
         */
        void redraw();

    private:
        /**
         * @brief Viewport event
         *
         * Called when window size changes. The default implementation does
         * nothing. If you want to respond to size changes, you should pass the
         * new *framebuffer* size to @ref GL::DefaultFramebuffer::setViewport()
         * (if using OpenGL) and possibly elsewhere (to
         * @ref SceneGraph::Camera::setViewport(), other framebuffers...) and
         * the new *window* size and DPI scaling to APIs that respond to user
         * events or scale UI elements.
         *
         * Note that this function might not get called at all if the window
         * size doesn't change. You should configure the initial state of your
         * cameras, framebuffers etc. in application constructor rather than
         * relying on this function to be called. Size of the window can be
         * retrieved using @ref windowSize(), size of the backing framebuffer
         * via @ref framebufferSize() and DPI scaling using @ref dpiScaling().
         * See @ref Platform-Sdl2Application-dpi for detailed info about these
         * values.
         * @see @ref platform-windowed-viewport-events
         */
        virtual void viewportEvent(ViewportEvent& event);

        /**
         * @brief Window focus event
         * @m_since_latest
         *
         * Called when the window gains (keyboard) input focus. The default
         * implementation does nothing.
         */
        virtual void focusEvent(FocusEvent& event);

        /**
         * @brief Window blur event
         * @m_since_latest
         *
         * Called when the window loses (keyboard) input focus, which for
         * example can be used by the application code to discard all
         * information about currently pressed keys. The default implementation
         * does nothing.
         */
        virtual void blurEvent(FocusEvent& event);

        /**
         * @brief Draw event
         *
         * Called when the screen is redrawn. You should clean the framebuffer
         * using @ref GL::DefaultFramebuffer::clear() (if using OpenGL) and
         * then add your own drawing functions. After drawing is finished, call
         * @ref swapBuffers(). If you want to draw immediately again, call also
         * @ref redraw().
         */
        virtual void drawEvent() = 0;

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Keyboard handling */

    public:
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Whether a key is pressed
         * @m_since_latest
         *
         * If a key is pressed, i.e. @ref keyPressEvent() with given @p key was
         * fired but not a @ref keyReleaseEvent() yet, the function returns
         * @cpp true @ce. For unknown @ref Key values returns @cpp false @ce.
         *
         * This function only queries immediate keyboard state at given point
         * in time, which means that if a particular key got pressed and
         * released again in between calls to this function, it will not be
         * reported as pressed. To avoid losing key presses, prefer to get
         * keyboard press and release events through @ref keyPressEvent() and
         * @ref keyReleaseEvent() instead if possible.
         * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        bool isKeyPressed(Key key);
        #endif

    private:
        /**
         * @brief Key press event
         *
         * Called when a key is pressed. Default implementation does nothing.
         * @see @ref platform-windowed-key-events, @ref isKeyPressed()
         */
        virtual void keyPressEvent(KeyEvent& event);

        /**
         * @brief Key release event
         *
         * Called when a key is released. Default implementation does nothing.
         * @see @ref platform-windowed-key-events, @ref isKeyPressed()
         */
        virtual void keyReleaseEvent(KeyEvent& event);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Pointer handling */

    public:
        /**
         * @brief Cursor type
         * @m_since{2020,06}
         *
         * @see @ref setCursor()
         */
        enum class Cursor: UnsignedInt {
            Arrow,          /**< Arrow */
            TextInput,      /**< Text input */
            Wait,           /**< Wait */
            Crosshair,      /**< Crosshair */
            WaitArrow,      /**< Small wait cursor */
            ResizeNWSE,     /**< Double arrow pointing northwest and southeast */
            ResizeNESW,     /**< Double arrow pointing northeast and southwest */
            ResizeWE,       /**< Double arrow pointing west and east */
            ResizeNS,       /**< Double arrow pointing north and south */
            ResizeAll,      /**< Four pointed arrow pointing north, south, east, and west */
            No,             /**< Slashed circle or crossbones */
            Hand,           /**< Hand */
            Hidden,         /**< Hidden */

            #ifndef CORRADE_TARGET_EMSCRIPTEN
            /**
             * Hidden and locked. When the mouse is locked, only
             * @ref MouseMoveEvent::relativePosition() is changing, absolute
             * position stays the same.
             * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            HiddenLocked
            #endif
        };

        /**
         * @brief Set cursor type
         * @m_since{2020,06}
         *
         * Expects that a window is already created. Default is
         * @ref Cursor::Arrow.
         */
        void setCursor(Cursor cursor);

        /**
         * @brief Get current cursor type
         * @m_since{2020,06}
         */
        Cursor cursor();

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Warp mouse cursor to given coordinates
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        void warpCursor(const Vector2i& position) {
            SDL_WarpMouseInWindow(_window, position.x(), position.y());
        }
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Whether mouse is locked
         *
         * @m_deprecated_since{2020,06} Use @ref cursor() together with
         *      @ref Cursor::HiddenLocked instead.
         */
        CORRADE_DEPRECATED("use cursor() together with Cursor::HiddenLocked instead") bool isMouseLocked() const { return SDL_GetRelativeMouseMode(); }

        /**
         * @brief Enable or disable mouse locking
         *
         * @m_deprecated_since{2020,06} Use @ref setCursor() together with
         *      @ref Cursor::HiddenLocked instead.
         */
        CORRADE_DEPRECATED("use setCursor() together with Cursor::HiddenLocked instead") void setMouseLocked(bool enabled);
        #endif

    private:
        /**
         * @brief Pointer press event
         * @m_since_latest
         *
         * Called when either a mouse or a finger is pressed. Note that if at
         * least one mouse button is already pressed and another button gets
         * pressed in addition, @ref pointerMoveEvent() with the new
         * combination is called, not this function.
         *
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, if the pointer
         * is primary, default implementation delegates to
         * @ref mousePressEvent(), interpreting @ref Pointer::Finger as
         * @ref MouseEvent::Button::Left. On builds with deprecated
         * functionality disabled, default implementation does nothing.
         * @see @ref platform-windowed-pointer-events,
         *      @ref Platform-Sdl2Application-touch
         */
        virtual void pointerPressEvent(PointerEvent& event);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Mouse press event
         * @m_deprecated_since_latest Use @ref pointerPressEvent() instead,
         *      which is a better abstraction for covering both mouse and touch
         *      / pen input.
         *
         * Default implementation does nothing.
         */
        virtual CORRADE_DEPRECATED("use pointerPressEvent() instead") void mousePressEvent(MouseEvent& event);
        #endif

        /**
         * @brief Pointer release event
         * @m_since_latest
         *
         * Called when either a mouse or a finger is released. Note that if
         * multiple mouse buttons are pressed and one of these is released,
         * @ref pointerMoveEvent() with the new combination is called, not this
         * function.
         *
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, if the pointer
         * is primary, default implementation delegates to
         * @ref mouseReleaseEvent(), interpreting @ref Pointer::Finger as
         * @ref MouseEvent::Button::Left. On builds with deprecated
         * functionality disabled, default implementation does nothing.
         * @see @ref platform-windowed-pointer-events,
         *      @ref Platform-Sdl2Application-touch
         */
        virtual void pointerReleaseEvent(PointerEvent& event);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Mouse release event
         * @m_deprecated_since_latest Use @ref pointerReleaseEvent() instead,
         *      which is a better abstraction for covering both mouse and touch
         *      / pen input.
         *
         * Default implementation does nothing.
         */
        virtual CORRADE_DEPRECATED("use pointerReleaseEvent() instead") void mouseReleaseEvent(MouseEvent& event);
        #endif

        /**
         * @brief Pointer move event
         * @m_since_latest
         *
         * Called when any of the currently pressed pointers is moved or
         * changes its properties. Gets called also if the set of pressed mouse
         * buttons changes.
         *
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, if the pointer
         * is primary, default implementation delegates to
         * @ref mouseMoveEvent(), or if @ref PointerMoveEvent::pointer() is not
         * @relativeref{Corrade,Containers::NullOpt}, to either
         * @ref mousePressEvent() or @ref mouseReleaseEvent().
         * @ref Pointer::Finger is interpreted as @ref MouseEvent::Button::Left.
         * On builds with deprecated functionality disabled, default
         * implementation does nothing.
         * @see @ref platform-windowed-pointer-events,
         *      @ref Platform-Sdl2Application-touch
         */
        virtual void pointerMoveEvent(PointerMoveEvent& event);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Mouse move event
         * @m_deprecated_since_latest Use @ref pointerMoveEvent() instead,
         *      which is a better abstraction for covering both mouse and touch
         *      / pen input.
         *
         * Default implementation does nothing.
         */
        virtual CORRADE_DEPRECATED("use pointerMoveEvent() instead") void mouseMoveEvent(MouseMoveEvent& event);
        #endif

        /**
         * @brief Scroll event
         * @m_since_latest
         *
         * Called when a scrolling device is used (mouse wheel or scrolling
         * area on a touchpad).
         *
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, default
         * implementation delegates to @ref mouseScrollEvent(). On builds with
         * deprecated functionality disabled, default implementation does
         * nothing.
         * @see @ref platform-windowed-pointer-events
         */
        virtual void scrollEvent(ScrollEvent& event);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Mouse move event
         * @m_deprecated_since_latest Use @ref scrollEvent() instead, which
         *      isn't semantically tied to just a mouse.
         *
         * Default implementation does nothing.
         */
        virtual CORRADE_DEPRECATED("use scrollEvent() instead") void mouseScrollEvent(MouseScrollEvent& event);

        /**
         * @brief Multi gesture event
         * @m_deprecated_since_latest The SDL event reports relative distance
         *      to previous finger positions instead of a relative diameter
         *      between multiple pressed fingers, making it practically useless
         *      for implementing pinch zoom. It additionally reports everything
         *      in normalized coordinates, thus the distance and rotation is
         *      only meaningful when the window is an exact square. Use the
         *      @ref TwoFingerGesture helper instead and feed it with events
         *      coming from @ref pointerPressEvent(),
         *      @ref pointerReleaseEvent() and @ref pointerMoveEvent().
         *
         * Called when the user performs a gesture using multiple fingers.
         * Default implementation does nothing.
         */
        virtual CORRADE_DEPRECATED("use pointerEvent(), pointerMoveEvent() and TwoFingerGesture instead") void multiGestureEvent(MultiGestureEvent& event);
        #endif

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Text input handling */
    public:
        /**
         * @brief Whether text input is active
         *
         * If text input is active, text input events go to
         * @ref textInputEvent() and @ref textEditingEvent().
         * @note Note that in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" the
         *      value is emulated and might not reflect external events like
         *      closing on-screen keyboard.
         * @see @ref startTextInput(), @ref stopTextInput(),
         *      @ref platform-windowed-key-events
         */
        bool isTextInputActive();

        /**
         * @brief Start text input
         *
         * Starts text input that will go to @ref textInputEvent() and
         * @ref textEditingEvent().
         * @see @ref stopTextInput(), @ref isTextInputActive(),
         *      @ref setTextInputRect(), @ref platform-windowed-key-events
         */
        void startTextInput();

        /**
         * @brief Stop text input
         *
         * Stops text input that went to @ref textInputEvent() and
         * @ref textEditingEvent().
         * @see @ref startTextInput(), @ref isTextInputActive(),
         *      @ref textInputEvent(), @ref textEditingEvent(),
         *      @ref platform-windowed-key-events
         */
        void stopTextInput();

        /**
         * @brief Set text input rectangle
         *
         * The @p rect defines an area where the text is being displayed, for
         * example to hint the system where to place on-screen keyboard.
         * @see @ref platform-windowed-key-events
         */
        void setTextInputRect(const Range2Di& rect);

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Current clipboard text
         * @m_since_latest
         *
         * The returned string is in UTF-8. If the clipboard is empty or
         * contains non-text data (such as an image), the returned string is
         * empty.
         * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        Containers::String clipboardText();

        /**
         * @brief Set current clipboard text
         * @m_since_latest
         *
         * The @p text is expected to be in UTF-8. If it's not
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}, a
         * null-terminated copy is made before passing it to SDL.
         * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        void setClipboardText(Containers::StringView text);
        #endif

    private:
        /**
         * @brief Text input event
         *
         * Called when text input is active and the text is being input.
         * @see @ref isTextInputActive(), @ref platform-windowed-key-events
         */
        virtual void textInputEvent(TextInputEvent& event);

        /**
         * @brief Text editing event
         *
         * Called when text input is active and the text is being edited.
         * @see @ref platform-windowed-key-events
         */
        virtual void textEditingEvent(TextEditingEvent& event);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Special events */

        /**
         * @brief Exit event
         *
         * If implemented, it allows the application to react to an application
         * exit (for example to save its internal state) and suppress it as
         * well (for example to show a exit confirmation dialog). The default
         * implementation calls @ref ExitEvent::setAccepted() on @p event,
         * which tells the application that it's safe to exit.
         *
         * SDL has special behavior on POSIX systems regarding `SIGINT` and
         * `SIGTERM` handling, see @ref Platform-Sdl2Application-platform-specific-posix
         * for more information.
         */
        virtual void exitEvent(ExitEvent& event);

    protected:
        /**
         * @brief Tick event
         *
         * If implemented, this function is called periodically after
         * processing all input events and before draw event even though there
         * might be no input events and redraw is not requested. Useful e.g.
         * for asynchronous task polling. Use @ref setMinimalLoopPeriod() /
         * @ref setSwapInterval() to control main loop frequency.
         *
         * If this implementation gets called from its @cpp override @ce, it
         * will effectively stop the tick event from being fired and the app
         * returns back to waiting for input events. This can be used to
         * disable the tick event when not needed.
         *
         * @m_class{m-block m-success}
         *
         * @par Opting out of tick events at runtime
         *      Unlike other event handlers, this function isn't @cpp private @ce
         *      in order to allow subclasses to conditionally disable the tick
         *      event by calling the base implementation. To the application it
         *      looks the same as if the tick event wasn't overriden at all,
         *      which effectively results in the function not being called ever
         *      again. This is useful for example with
         *      @relativeref{Corrade,Utility::Tweakable}, where periodical
         *      polling for file updates doesn't need to be done if tweakable
         *      constants aren't enabled at all.
         * @par
         *      @snippet Platform.cpp tickEvent-conditional
         * @par
         *      It's not possible to re-enable the tick event afterwards.
         */
        virtual void tickEvent();

    private:
        /**
         * @brief Any event
         *
         * Called in case a SDL event is not handled by any other event
         * functions above.
         * @see @ref ViewportEvent::event(), @ref InputEvent::event(),
         *      @ref MultiGestureEvent::event(), @ref TextInputEvent::event(),
         *      @ref TextEditingEvent::event(), @ref ExitEvent::event()
         */
        virtual void anyEvent(SDL_Event& event);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

    private:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /* Calls the base pointer*Event() in order to delegate to deprecated
           mouse events */
        template<class> friend class BasicScreenedApplication;
        template<class, bool> friend struct Implementation::ApplicationScrollEventMixin;
        #endif

        enum class Flag: UnsignedByte;
        typedef Containers::EnumSet<Flag> Flags;
        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        Vector2 dpiScalingInternal(Implementation::Sdl2DpiScalingPolicy configurationDpiScalingPolicy, const Vector2& configurationDpiScaling) const;

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        SDL_Cursor* _cursors[12]{};
        #else
        Cursor _cursor;
        #endif

        /* These are saved from command-line arguments, and from configuration
           to be reused in dpiScaling() and viewportEvent() later */
        bool _verboseLog{};
        Implementation::Sdl2DpiScalingPolicy _commandLineDpiScalingPolicy{}, _configurationDpiScalingPolicy{};
        Vector2 _commandLineDpiScaling, _configurationDpiScaling;

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        SDL_Window* _window{};
        Long _primaryFingerId = ~Long{};
        /* Not using Nanoseconds as that would require including Time.h */
        UnsignedInt _minimalLoopPeriodMilliseconds{};
        #else
        SDL_Surface* _surface{};
        Vector2i _lastKnownCanvasSize;
        #endif

        #ifdef MAGNUM_TARGET_GL
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        SDL_GLContext _glContext{};
        #endif
        /* Has to be in an Optional because we delay-create it in a constructor
           with populated Arguments and it gets explicitly destroyed before the
           GL context */
        Containers::Optional<Platform::GLContext> _context;
        #endif

        Flags _flags;

        int _exitCode = 0;
};

/**
@brief Keyboard modifier
@m_since_latest

@see @ref Modifiers, @ref KeyEvent::modifiers(),
    @ref PointerEvent::modifiers(), @ref PointerMoveEvent::modifiers(),
    @ref ScrollEvent::modifiers(), @ref platform-windowed-key-events
 */
enum class Sdl2Application::Modifier: Uint16 {
    /**
     * Shift
     *
     * @see @ref KeyEvent::Key::LeftShift, @ref KeyEvent::Key::RightShift
     */
    Shift = KMOD_SHIFT,

    /**
     * Ctrl
     *
     * @see @ref KeyEvent::Key::LeftCtrl, @ref KeyEvent::Key::RightCtrl
     */
    Ctrl = KMOD_CTRL,

    /**
     * Alt
     *
     * @see @ref KeyEvent::Key::LeftAlt, @ref KeyEvent::Key::RightAlt
     */
    Alt = KMOD_ALT,

    /**
     * Super key (Windows/⌘)
     *
     * @see @ref KeyEvent::Key::LeftSuper, @ref KeyEvent::Key::RightSuper
     */
    Super = KMOD_GUI,

    /**
     * AltGr
     *
     * @see @ref KeyEvent::Key::AltGr
     * @todo AltGr gets reported as RightAlt, what's this for?
     */
    AltGr = KMOD_MODE,

    /**
     * Caps lock
     *
     * @see @ref KeyEvent::Key::CapsLock
     */
    CapsLock = KMOD_CAPS,

    /**
     * Num lock
     *
     * @see @ref KeyEvent::Key::NumLock
     */
    NumLock = KMOD_NUM
};

CORRADE_ENUMSET_OPERATORS(Sdl2Application::Modifiers)

/**
@brief Key
@m_since_latest

@see @ref KeyEvent::key(), @ref isKeyPressed(), @ref keyToScanCode(),
    @ref scanCodeToKey(), @ref platform-windowed-key-events
*/
enum class Sdl2Application::Key: SDL_Keycode {
    Unknown = SDLK_UNKNOWN,     /**< Unknown key */

    /**
     * Left Shift
     *
     * @see @ref InputEvent::Modifier::Shift
     */
    LeftShift = SDLK_LSHIFT,

    /**
     * Right Shift
     *
     * @see @ref InputEvent::Modifier::Shift
     */
    RightShift = SDLK_RSHIFT,

    /**
     * Left Ctrl
     *
     * @see @ref InputEvent::Modifier::Ctrl
     */
    LeftCtrl = SDLK_LCTRL,

    /**
     * Right Ctrl
     *
     * @see @ref InputEvent::Modifier::Ctrl
     */
    RightCtrl = SDLK_RCTRL,

    /**
     * Left Alt
     *
     * @see @ref InputEvent::Modifier::Alt
     */
    LeftAlt = SDLK_LALT,

    /**
     * Right Alt
     *
     * @see @ref InputEvent::Modifier::Alt
     */
    RightAlt = SDLK_RALT,

    /**
     * Left Super key (Windows/⌘)
     *
     * @see @ref InputEvent::Modifier::Super
     */
    LeftSuper = SDLK_LGUI,

    /**
     * Right Super key (Windows/⌘)
     *
     * @see @ref InputEvent::Modifier::Super
     */
    RightSuper = SDLK_RGUI,

    /**
     * AltGr
     *
     * @see @ref InputEvent::Modifier::AltGr
     * @todo AltGr gets reported as RightAlt, what's this for?
     */
    AltGr = SDLK_MODE,

    Enter = SDLK_RETURN,        /**< Enter */
    Esc = SDLK_ESCAPE,          /**< Escape */

    Up = SDLK_UP,               /**< Up arrow */
    Down = SDLK_DOWN,           /**< Down arrow */
    Left = SDLK_LEFT,           /**< Left arrow */
    Right = SDLK_RIGHT,         /**< Right arrow */
    Home = SDLK_HOME,           /**< Home */
    End = SDLK_END,             /**< End */
    PageUp = SDLK_PAGEUP,       /**< Page up */
    PageDown = SDLK_PAGEDOWN,   /**< Page down */
    Backspace = SDLK_BACKSPACE, /**< Backspace */
    Insert = SDLK_INSERT,       /**< Insert */
    Delete = SDLK_DELETE,       /**< Delete */

    F1 = SDLK_F1,               /**< F1 */
    F2 = SDLK_F2,               /**< F2 */
    F3 = SDLK_F3,               /**< F3 */
    F4 = SDLK_F4,               /**< F4 */
    F5 = SDLK_F5,               /**< F5 */
    F6 = SDLK_F6,               /**< F6 */
    F7 = SDLK_F7,               /**< F7 */
    F8 = SDLK_F8,               /**< F8 */
    F9 = SDLK_F9,               /**< F9 */
    F10 = SDLK_F10,             /**< F10 */
    F11 = SDLK_F11,             /**< F11 */
    F12 = SDLK_F12,             /**< F12 */

    Space = SDLK_SPACE,         /**< Space */
    Tab = SDLK_TAB,             /**< Tab */

    /**
     * Quote (<tt>'</tt>)
     * @m_since{2020,06}
     */
    Quote = SDLK_QUOTE,

    Comma = SDLK_COMMA,         /**< Comma */
    Period = SDLK_PERIOD,       /**< Period */
    Minus = SDLK_MINUS,         /**< Minus */

    /**
     * Plus. On the US keyboard layout this may only be representable as
     * @m_class{m-label m-warning} **Shift** @m_class{m-label m-default} <b>=</b>.
     */
    Plus = SDLK_PLUS,

    Slash = SDLK_SLASH,         /**< Slash */

    /**
     * Percent. On the US keyboard layout this may only be representable as
     * @m_class{m-label m-warning} **Shift** @m_class{m-label m-default} **5**.
     */
    Percent = SDLK_PERCENT,

    Semicolon = SDLK_SEMICOLON, /**< Semicolon (`;`) */
    Equal = SDLK_EQUALS,        /**< Equal */

    /**
     * Left bracket (`[`)
     * @m_since{2020,06}
     */
    LeftBracket = SDLK_LEFTBRACKET,

    /**
     * Right bracket (`]`)
     * @m_since{2020,06}
     */
    RightBracket = SDLK_RIGHTBRACKET,

    /**
     * Backslash (`\`)
     * @m_since{2020,06}
     */
    Backslash = SDLK_BACKSLASH,

    /**
     * Backquote (<tt>`</tt>)
     * @m_since{2020,06}
     */
    Backquote = SDLK_BACKQUOTE,

    /* no equivalent for GlfwApplication's World1 / World2 */

    Zero = SDLK_0,              /**< Zero */
    One = SDLK_1,               /**< One */
    Two = SDLK_2,               /**< Two */
    Three = SDLK_3,             /**< Three */
    Four = SDLK_4,              /**< Four */
    Five = SDLK_5,              /**< Five */
    Six = SDLK_6,               /**< Six */
    Seven = SDLK_7,             /**< Seven */
    Eight = SDLK_8,             /**< Eight */
    Nine = SDLK_9,              /**< Nine */

    A = SDLK_a,                 /**< Letter A */
    B = SDLK_b,                 /**< Letter B */
    C = SDLK_c,                 /**< Letter C */
    D = SDLK_d,                 /**< Letter D */
    E = SDLK_e,                 /**< Letter E */
    F = SDLK_f,                 /**< Letter F */
    G = SDLK_g,                 /**< Letter G */
    H = SDLK_h,                 /**< Letter H */
    I = SDLK_i,                 /**< Letter I */
    J = SDLK_j,                 /**< Letter J */
    K = SDLK_k,                 /**< Letter K */
    L = SDLK_l,                 /**< Letter L */
    M = SDLK_m,                 /**< Letter M */
    N = SDLK_n,                 /**< Letter N */
    O = SDLK_o,                 /**< Letter O */
    P = SDLK_p,                 /**< Letter P */
    Q = SDLK_q,                 /**< Letter Q */
    R = SDLK_r,                 /**< Letter R */
    S = SDLK_s,                 /**< Letter S */
    T = SDLK_t,                 /**< Letter T */
    U = SDLK_u,                 /**< Letter U */
    V = SDLK_v,                 /**< Letter V */
    W = SDLK_w,                 /**< Letter W */
    X = SDLK_x,                 /**< Letter X */
    Y = SDLK_y,                 /**< Letter Y */
    Z = SDLK_z,                 /**< Letter Z */

    /**
     * Caps lock
     *
     * @see @ref InputEvent::Modifier::CapsLock
     * @m_since_latest
     */
    CapsLock = SDLK_CAPSLOCK,

    /**
     * Scroll lock
     * @m_since_latest
     */
    ScrollLock = SDLK_SCROLLLOCK,

    /**
     * Num lock
     *
     * @see @ref InputEvent::Modifier::NumLock
     * @m_since_latest
     */
    NumLock = SDLK_NUMLOCKCLEAR,

    /**
     * Print screen
     * @m_since_latest
     */
    PrintScreen = SDLK_PRINTSCREEN,

    /**
     * Pause
     * @m_since_latest
     */
    Pause = SDLK_PAUSE,

    /**
     * Menu
     * @m_since_latest
     */
    Menu = SDLK_APPLICATION,

    NumZero = SDLK_KP_0,            /**< Numpad zero */
    NumOne = SDLK_KP_1,             /**< Numpad one */
    NumTwo = SDLK_KP_2,             /**< Numpad two */
    NumThree = SDLK_KP_3,           /**< Numpad three */
    NumFour = SDLK_KP_4,            /**< Numpad four */
    NumFive = SDLK_KP_5,            /**< Numpad five */
    NumSix = SDLK_KP_6,             /**< Numpad six */
    NumSeven = SDLK_KP_7,           /**< Numpad seven */
    NumEight = SDLK_KP_8,           /**< Numpad eight */
    NumNine = SDLK_KP_9,            /**< Numpad nine */
    NumDecimal = SDLK_KP_DECIMAL,   /**< Numpad decimal */
    NumDivide = SDLK_KP_DIVIDE,     /**< Numpad divide */
    NumMultiply = SDLK_KP_MULTIPLY, /**< Numpad multiply */
    NumSubtract = SDLK_KP_MINUS,    /**< Numpad subtract */
    NumAdd = SDLK_KP_PLUS,          /**< Numpad add */
    NumEnter = SDLK_KP_ENTER,       /**< Numpad enter */
    NumEqual = SDLK_KP_EQUALS       /**< Numpad equal */
};

/**
@brief Pointer event source
@m_since_latest

@see @ref PointerEvent::source(), @ref PointerMoveEvent::source(),
    @ref platform-windowed-pointer-events, @ref Platform-Sdl2Application-touch
*/
enum class Sdl2Application::PointerEventSource: UnsignedByte {
    /**
     * The event is coming from a mouse. Corresponds to the
     * `SDL_MOUSEBUTTONDOWN`, `SDL_MOUSEBUTTONUP` and `SDL_MOUSEMOTION` events.
     * @see @ref Pointer::MouseLeft, @ref Pointer::MouseMiddle,
     *      @ref Pointer::MouseRight, @ref Pointer::MouseButton4,
     *      @ref Pointer::MouseButton5
     */
    Mouse,

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /**
     * The event is coming from a touch contact. Corresponds to the
     * `SDL_FINGERDOWN`, `SDL_FINGERUP` and `SDL_FINGERMOTION` events.
     * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" as
     *      the minimal SDL2 implementation there doesn't expose a way to
     *      disable touch to mouse event translation, which would lead to
     *      duplicate events being emitted.
     * @see @ref Pointer::Finger
     */
    Touch
    #endif
};

/**
@brief Pointer type
@m_since_latest

@see @ref Pointers, @ref PointerEvent::pointer(),
    @ref PointerMoveEvent::pointer(), @ref PointerMoveEvent::pointers(),
    @ref platform-windowed-pointer-events, @ref Platform-Sdl2Application-touch
*/
enum class Sdl2Application::Pointer: UnsignedByte {
    /**
     * Left mouse button. Corresponds to `SDL_BUTTON_LEFT` /
     * `SDL_BUTTON_LMASK`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseLeft = 1 << 0,

    /**
     * Middle mouse button. Corresponds to `SDL_BUTTON_MIDDLE` /
     * `SDL_BUTTON_MMASK`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseMiddle = 1 << 1,

    /**
     * Right mouse button. Corresponds to `SDL_BUTTON_RIGHT` /
     * `SDL_BUTTON_RMASK`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseRight = 1 << 2,

    /**
     * Fourth mouse button, such as wheel left. Corresponds to `SDL_BUTTON_X1`
     * / `SDL_BUTTON_X1MASK`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseButton4 = 1 << 3,

    /**
     * Fifth mouse button, such as wheel right. Corresponds to `SDL_BUTTON_X2`
     * / `SDL_BUTTON_X2MASK`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseButton5 = 1 << 4,

    #ifndef CORRADE_TARGET_EMSCRIPTEN
    /**
     * Finger
     * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" as
     *      the minimal SDL2 implementation there doesn't expose a way to
     *      disable touch to mouse event translation, which would lead to
     *      duplicate events being emitted.
     * @see @ref PointerEventSource::Touch
     */
    Finger = 1 << 5,
    #endif

    /* Pen support is since SDL3 only */
};

CORRADE_ENUMSET_OPERATORS(Sdl2Application::Pointers)

#ifdef MAGNUM_TARGET_GL
/**
@brief OpenGL context configuration

The created window is always with a double-buffered OpenGL context.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref Sdl2Application(), @ref create(), @ref tryCreate()
*/
class Sdl2Application::GLConfiguration: public GL::Context::Configuration {
    public:
        /**
         * @brief Context flag
         *
         * Includes also everything from @ref GL::Context::Configuration::Flag
         * except for @relativeref{GL::Context::Configuration,Flag::Windowless},
         * which is not meant to be enabled for windowed apps.
         * @see @ref Flags, @ref setFlags(), @ref GL::Context::Flag
         */
        enum class Flag: UnsignedLong {
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Forward compatible context.
             * @requires_gl Core/compatibility profile distinction and forward
             *      compatibility applies only to desktop GL.
             */
            ForwardCompatible = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG,
            #endif

            /**
             * Debug context. Enabled automatically if supported by the driver
             * and the @ref Flag::GpuValidation flag is set or if the
             * `--magnum-gpu-validation` @ref GL-Context-usage-command-line "command-line option"
             * is set to `on`.
             * @requires_gles Context flags are not available in WebGL.
             */
            Debug = SDL_GL_CONTEXT_DEBUG_FLAG,

            /**
             * Context with robust access.
             * @requires_gles Context flags are not available in WebGL.
             */
            RobustAccess = SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG,

            /**
             * Context with reset isolation.
             * @requires_gles Context flags are not available in WebGL.
             */
            ResetIsolation = SDL_GL_CONTEXT_RESET_ISOLATION_FLAG,

            /**
             * Context without error reporting. Might result in better
             * performance, but situations that would have generated errors
             * instead cause undefined behavior. Enabled automatically if
             * supported by the driver and the @ref Flag::GpuValidationNoError
             * flag is set or if the `--magnum-gpu-validation` @ref GL-Context-usage-command-line "command-line option"
             * is set to `no-error`.
             *
             * @requires_gles Context flags are not available in WebGL.
             * @m_since_latest
             */
            /* Treated as a separate attribute and not a flag in SDL, thus
               handling manually. */
            NoError = 1ull << 32,
            #endif

            /**
             * @copydoc GL::Context::Configuration::Flag::QuietLog
             * @m_since_latest
             */
            QuietLog = UnsignedLong(GL::Context::Configuration::Flag::QuietLog),

            /**
             * @copydoc GL::Context::Configuration::Flag::VerboseLog
             * @m_since_latest
             */
            VerboseLog = UnsignedLong(GL::Context::Configuration::Flag::VerboseLog),

            /**
             * @copydoc GL::Context::Configuration::Flag::GpuValidation
             * @m_since_latest
             */
            GpuValidation = UnsignedLong(GL::Context::Configuration::Flag::GpuValidation),

            /**
             * @copydoc GL::Context::Configuration::Flag::GpuValidationNoError
             * @m_since_latest
             */
            GpuValidationNoError = UnsignedLong(GL::Context::Configuration::Flag::GpuValidationNoError)
        };

        /**
         * @brief Context flags
         *
         * @see @ref setFlags(), @ref GL::Context::Flags
         */
        typedef Containers::EnumSet<Flag> Flags;

        explicit GLConfiguration();
        ~GLConfiguration();

        /** @brief Context flags */
        Flags flags() const {
            return Flag(UnsignedLong(GL::Context::Configuration::flags()));
        }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is @ref Flag::ForwardCompatible on desktop GL and no flags
         * on OpenGL ES. To avoid clearing default flags by accident, prefer to
         * use @ref addFlags() and @ref clearFlags() instead.
         * @see @ref GL::Context::flags()
         */
        GLConfiguration& setFlags(Flags flags) {
            GL::Context::Configuration::setFlags(GL::Context::Configuration::Flag(UnsignedLong(flags)));
            return *this;
        }

        /**
         * @brief Add context flags
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref setFlags(), ORs the flags with existing instead of
         * replacing them. Useful for preserving the defaults.
         * @see @ref clearFlags()
         */
        GLConfiguration& addFlags(Flags flags) {
            GL::Context::Configuration::addFlags(GL::Context::Configuration::Flag(UnsignedLong(flags)));
            return *this;
        }

        /**
         * @brief Clear context flags
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref setFlags(), ANDs the inverse of @p flags with existing
         * instead of replacing them. Useful for removing default flags.
         * @see @ref addFlags()
         */
        GLConfiguration& clearFlags(Flags flags) {
            GL::Context::Configuration::clearFlags(GL::Context::Configuration::Flag(UnsignedLong(flags)));
            return *this;
        }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Context version
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        GL::Version version() const { return _version; }
        #endif

        /**
         * @brief Set context version
         *
         * If requesting version greater or equal to OpenGL 3.1, core profile
         * is used. The created context will then have any version which is
         * backwards-compatible with requested one. Default is
         * @ref GL::Version::None, i.e. any provided version is used.
         * @note In @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" this function
         *      does nothing (@ref GL::Version::GLES200 or
         *      @ref GL::Version::GLES300 is used implicitly based on the
         *      target).
         */
        GLConfiguration& setVersion(GL::Version version) {
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            _version = version;
            #else
            static_cast<void>(version);
            #endif
            return *this;
        }

        /** @brief Color buffer size */
        Vector4i colorBufferSize() const { return _colorBufferSize; }

        /**
         * @brief Set color buffer size
         *
         * Default is @cpp {8, 8, 8, 8} @ce (8-bit-per-channel RGBA).
         * @see @ref setDepthBufferSize(), @ref setStencilBufferSize()
         */
        GLConfiguration& setColorBufferSize(const Vector4i& size) {
            _colorBufferSize = size;
            return *this;
        }

        /** @brief Depth buffer size */
        Int depthBufferSize() const { return _depthBufferSize; }

        /**
         * @brief Set depth buffer size
         *
         * Default is @cpp 24 @ce bits.
         * @see @ref setColorBufferSize(), @ref setStencilBufferSize()
         */
        GLConfiguration& setDepthBufferSize(Int size) {
            _depthBufferSize = size;
            return *this;
        }

        /** @brief Stencil buffer size */
        Int stencilBufferSize() const { return _stencilBufferSize; }

        /**
         * @brief Set stencil buffer size
         *
         * Default is @cpp 0 @ce bits (i.e., no stencil buffer).
         * @see @ref setColorBufferSize(), @ref setDepthBufferSize()
         */
        GLConfiguration& setStencilBufferSize(Int size) {
            _stencilBufferSize = size;
            return *this;
        }

        /** @brief Sample count */
        Int sampleCount() const { return _sampleCount; }

        /**
         * @brief Set sample count
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp 0 @ce, thus no multisampling. See also
         * @ref GL::Renderer::Feature::Multisampling.
         */
        GLConfiguration& setSampleCount(Int count) {
            _sampleCount = count;
            return *this;
        }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief sRGB-capable default framebuffer
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        bool isSrgbCapable() const { return _srgbCapable; }

        /**
         * @brief Set sRGB-capable default framebuffer
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp false @ce. See also
         * @ref GL::Renderer::Feature::FramebufferSrgb.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        GLConfiguration& setSrgbCapable(bool enabled) {
            _srgbCapable = enabled;
            return *this;
        }
        #endif

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        _MAGNUM_GL_CONTEXT_CONFIGURATION_SUBCLASS_IMPLEMENTATION(GLConfiguration)
        #endif

    private:
        Vector4i _colorBufferSize;
        Int _depthBufferSize, _stencilBufferSize;
        Int _sampleCount;
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        GL::Version _version;
        bool _srgbCapable;
        #endif
};

#ifndef CORRADE_TARGET_EMSCRIPTEN
CORRADE_ENUMSET_OPERATORS(Sdl2Application::GLConfiguration::Flags)
#endif
#endif

namespace Implementation {
    enum class Sdl2DpiScalingPolicy: UnsignedByte {
        /* Using 0 for an "unset" value */

        #ifdef CORRADE_TARGET_APPLE
        Framebuffer = 1,
        #endif

        #ifndef CORRADE_TARGET_APPLE
        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
        Virtual = 2,
        #endif

        Physical = 3,
        #endif

        Default
            #ifdef CORRADE_TARGET_APPLE
            = Framebuffer
            #elif !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_ANDROID)
            = Virtual
            #else
            = Physical
            #endif
    };
}

/**
@brief Configuration

@see @ref Sdl2Application(), @ref GLConfiguration, @ref create(),
    @ref tryCreate()
*/
class Sdl2Application::Configuration {
    public:
        /**
         * @brief Window flag
         *
         * @see @ref WindowFlags, @ref setWindowFlags()
         */
        enum class WindowFlag: Uint32 {
            /**
             * Resizable window. On iOS this allows the application to respond
             * to display orientation changes, on
             * @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" this causes the
             * framebuffer to be resized when the @cb{.html} <canvas> @ce size
             * changes.
             *
             * Implement @ref viewportEvent() to react to the resizing events.
             */
            Resizable = SDL_WINDOW_RESIZABLE,

            #ifndef CORRADE_TARGET_EMSCRIPTEN
            /**
             * Fullscreen window
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            Fullscreen = SDL_WINDOW_FULLSCREEN,

            /**
             * Fullscreen window at the current desktop resolution
             * @m_since{2020,06}
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            FullscreenDesktop = SDL_WINDOW_FULLSCREEN_DESKTOP,

            /**
             * No window decoration. On iOS this hides the menu bar.
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            Borderless = SDL_WINDOW_BORDERLESS,
            #endif

            #ifndef CORRADE_TARGET_EMSCRIPTEN
            /**
             * Hidden window
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            Hidden = SDL_WINDOW_HIDDEN,

            /**
             * Maximized window
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            Maximized = SDL_WINDOW_MAXIMIZED,

            /**
             * Minimized window
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            Minimized = SDL_WINDOW_MINIMIZED,

            /**
             * Window with mouse locked
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             * @todo SDL_WINDOW_MOUSE_CAPTURE, also what all those do? isn't it
             *      redundant / better handled with cursor APIs?
             */
            MouseLocked = SDL_WINDOW_INPUT_GRABBED,

            /** @todo SDL_WINDOW_INPUT_FOCUS, SDL_WINDOW_MOUSE_FOCUS, GLFW has
                GLFW_FOCUSED (exposed as Focused) and GLFW_FOCUS_ON_SHOW (not
                exposed) -- what's the relation? How to make these compatible? */

            /**
             * Always on top
             * @m_since{2020,06}
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             *      According to SDL docs works only on X11.
             */
            AlwaysOnTop = SDL_WINDOW_ALWAYS_ON_TOP,

            /**
             * Don't add the window to taskbar
             * @m_since{2020,06}
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             *      According to SDL docs works only on X11.
             */
            SkipTaskbar = SDL_WINDOW_SKIP_TASKBAR,

            /**
             * Window should be treated as a utility window
             * @m_since{2020,06}
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             *      According to SDL docs works only on X11.
             */
            Utility = SDL_WINDOW_UTILITY,

            /**
             * Window should be treated as a tooltip
             * @m_since{2020,06}
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             *      According to SDL docs works only on X11.
             */
            Tooltip = SDL_WINDOW_TOOLTIP,

            /**
             * Window should be treated as a popup menu
             * @m_since{2020,06}
             *
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             *      According to SDL docs works only on X11.
             */
            PopupMenu = SDL_WINDOW_POPUP_MENU,
            #endif

            /**
             * Do not create any GPU context. Use together with
             * @ref Sdl2Application(const Arguments&, const Configuration&),
             * @ref create(const Configuration&) or
             * @ref tryCreate(const Configuration&) to prevent implicit
             * creation of an OpenGL context. Can't be used with
             * @ref Sdl2Application(const Arguments&, const Configuration&, const GLConfiguration&),
             * @ref create(const Configuration&, const GLConfiguration&) or
             * @ref tryCreate(const Configuration&, const GLConfiguration&).
             */
            Contextless = 1u << 31, /* Hope this won't ever conflict with anything */

            /**
             * Request a window for use with OpenGL. Useful in combination with
             * @ref WindowFlag::Contextless, otherwise enabled implicitly when
             * creating an OpenGL context using
             * @ref Sdl2Application(const Arguments&, const Configuration&, const GLConfiguration&),
             * @ref create(const Configuration&, const GLConfiguration&) or
             * @ref tryCreate(const Configuration&, const GLConfiguration&).
             * @m_since{2019,10}
             */
            OpenGL = SDL_WINDOW_OPENGL,

            #ifndef CORRADE_TARGET_EMSCRIPTEN
            /**
             * Request a window for use with Vulkan. Useful in combination with
             * @ref WindowFlag::Contextless.
             * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             * @m_since{2019,10}
             */
            Vulkan = SDL_WINDOW_VULKAN
            #endif
        };

        /**
         * @brief Window flags
         *
         * @see @ref setWindowFlags()
         */
        typedef Containers::EnumSet<WindowFlag> WindowFlags;

        /**
         * @brief DPI scaling policy
         *
         * DPI scaling policy when requesting a particular window size. Can
         * be overridden on command-line using `--magnum-dpi-scaling` or via
         * the `MAGNUM_DPI_SCALING` environment variable.
         * @see @ref setSize(), @ref Platform-Sdl2Application-dpi
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class DpiScalingPolicy: UnsignedByte {
            /**
             * Framebuffer DPI scaling. The window will have the same size as
             * requested, but the framebuffer size will be different. Supported
             * only on macOS and iOS and is also the only supported value
             * there.
             */
            Framebuffer,

            /**
             * Virtual DPI scaling. Scales the window based on UI scaling
             * setting in the system. Falls back to
             * @ref DpiScalingPolicy::Physical on platforms that don't support
             * it. Supported only on desktop platforms (except macOS) and it's
             * the default there.
             *
             * Equivalent to `--magnum-dpi-scaling virtual` passed on
             * command-line.
             */
            Virtual,

            /**
             * Physical DPI scaling. Takes the requested window size as a
             * physical size that a window would have on platform's default DPI
             * and scales it to have the same size on given display physical
             * DPI. On platforms that don't have a concept of a window it
             * causes the framebuffer to match screen pixels 1:1 without any
             * scaling. Supported on desktop platforms except macOS and on
             * mobile and web. Default on mobile and web.
             *
             * Equivalent to `--magnum-dpi-scaling physical` passed on
             * command-line.
             */
            Physical,

            /**
             * Default policy for current platform. Alias to one of
             * @ref DpiScalingPolicy::Framebuffer, @ref DpiScalingPolicy::Virtual
             * or @ref DpiScalingPolicy::Physical depending on platform. See
             * @ref Platform-Sdl2Application-dpi for details.
             */
            Default
        };
        #else
        typedef Implementation::Sdl2DpiScalingPolicy DpiScalingPolicy;
        #endif

        /*implicit*/ Configuration();
        ~Configuration();

        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
        /**
         * @brief Window title
         *
         * The returned string view is
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * is valid until the next call to @ref setTitle().
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten"
         *      and @ref CORRADE_TARGET_IOS "iOS".
         */
        Containers::StringView title() const { return _title; }
        #endif

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp "Magnum SDL2 Application" @ce.
         * @note On @ref CORRADE_TARGET_IOS "iOS" this function does nothing
         *      and is included only for compatibility. You need to set the
         *      title separately in platform-specific configuration file.
         * @note Similarly, on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" this
         *      function is only for compatibility, as the page title is
         *      expected to be set by the HTML markup. However, it's possible
         *      to change the page title later (for example in response to
         *      application state change) using @ref setWindowTitle().
         */
        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
        Configuration& setTitle(const Containers::StringView title) {
            _title = Containers::String::nullTerminatedGlobalView(title);
            return *this;
        }
        #else
        template<class T> Configuration& setTitle(const T&) { return *this; }
        #endif

        /** @brief Window size */
        Vector2i size() const { return _size; }

        /**
         * @brief DPI scaling policy
         *
         * If @ref dpiScaling() is non-zero, it has a priority over this value.
         * The `--magnum-dpi-scaling` command-line option has a priority over
         * any application-set value.
         * @see @ref setSize(const Vector2i&, DpiScalingPolicy)
         */
        DpiScalingPolicy dpiScalingPolicy() const { return _dpiScalingPolicy; }

        /**
         * @brief Custom DPI scaling
         *
         * If zero, then @ref dpiScalingPolicy() has a priority over this
         * value. The `--magnum-dpi-scaling` command-line option has a priority
         * over any application-set value.
         * @see @ref setSize(const Vector2i&, const Vector2&)
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

        /**
         * @brief Set window size
         * @param size              Desired window size
         * @param dpiScalingPolicy  Policy based on which DPI scaling will be set
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp {800, 600} @ce on desktop platforms. On
         * @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" and iOS the default is a
         * zero vector, meaning a value that matches the display or canvas size
         * is autodetected. See @ref Platform-Sdl2Application-dpi for more
         * information.
         * @see @ref setSize(const Vector2i&, const Vector2&)
         */
        Configuration& setSize(const Vector2i& size, DpiScalingPolicy dpiScalingPolicy = DpiScalingPolicy::Default) {
            _size = size;
            _dpiScalingPolicy = dpiScalingPolicy;
            return *this;
        }

        /**
         * @brief Set window size with custom DPI scaling
         * @param size              Desired window size
         * @param dpiScaling        Custom DPI scaling value
         *
         * Compared to @ref setSize(const Vector2i&, DpiScalingPolicy) which
         * autodetects the DPI scaling value according to given policy, this
         * function sets the DPI scaling directly. The resulting
         * @ref Sdl2Application::windowSize() is @cpp size*dpiScaling @ce and
         * @ref Sdl2Application::dpiScaling() is @p dpiScaling.
         */
        Configuration& setSize(const Vector2i& size, const Vector2& dpiScaling) {
            _size = size;
            _dpiScaling = dpiScaling;
            return *this;
        }

        /** @brief Window flags */
        WindowFlags windowFlags() const { return _windowFlags; }

        /**
         * @brief Set window flags
         * @return Reference to self (for method chaining)
         *
         * Default are none. To avoid clearing default flags by accident,
         * prefer to use @ref addWindowFlags() and @ref clearWindowFlags()
         * instead.
         */
        Configuration& setWindowFlags(WindowFlags flags) {
            _windowFlags = flags;
            return *this;
        }

        /**
         * @brief Add window flags
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Unlike @ref setWindowFlags(), ORs the flags with existing instead of
         * replacing them. Useful for preserving the defaults.
         * @see @ref clearWindowFlags()
         */
        Configuration& addWindowFlags(WindowFlags flags) {
            _windowFlags |= flags;
            return *this;
        }

        /**
         * @brief Clear window flags
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * Unlike @ref setWindowFlags(), ANDs the inverse of @p flags with
         * existing instead of replacing them. Useful for removing default
         * flags.
         * @see @ref addWindowFlags()
         */
        Configuration& clearWindowFlags(WindowFlags flags) {
            _windowFlags &= ~flags;
            return *this;
        }

    private:
        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
        Containers::String _title;
        #endif
        Vector2i _size;
        DpiScalingPolicy _dpiScalingPolicy;
        WindowFlags _windowFlags;
        Vector2 _dpiScaling;
};

/**
@brief Exit event

@see @ref exitEvent()
*/
class Sdl2Application::ExitEvent {
    public:
        /** @brief Copying is not allowed */
        ExitEvent(const ExitEvent&) = delete;

        /** @brief Moving is not allowed */
        ExitEvent(ExitEvent&&) = delete;

        /** @brief Copying is not allowed */
        ExitEvent& operator=(const ExitEvent&) = delete;

        /** @brief Moving is not allowed */
        ExitEvent& operator=(ExitEvent&&) = delete;

        /** @brief Whether the event is accepted */
        bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted) in
         * @ref exitEvent(), the application won't exit. Default implementation
         * of @ref exitEvent() accepts the event.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /**
         * @brief Underlying SDL event
         *
         * Of type `SDL_QUIT`.
         * @see @ref Sdl2Application::anyEvent()
         */
        const SDL_Event& event() const { return _event; }

    private:
        friend Sdl2Application;

        explicit ExitEvent(const SDL_Event& event): _event(event), _accepted(false) {}

        const SDL_Event& _event;
        bool _accepted;
};

/**
@brief Viewport event

@see @ref viewportEvent(), @ref platform-windowed-viewport-events
*/
class Sdl2Application::ViewportEvent {
    public:
        /** @brief Copying is not allowed */
        ViewportEvent(const ViewportEvent&) = delete;

        /** @brief Moving is not allowed */
        ViewportEvent(ViewportEvent&&) = delete;

        /** @brief Copying is not allowed */
        ViewportEvent& operator=(const ViewportEvent&) = delete;

        /** @brief Moving is not allowed */
        ViewportEvent& operator=(ViewportEvent&&) = delete;

        /**
         * @brief Window size
         *
         * On some platforms with HiDPI displays, window size can be different
         * from @ref framebufferSize(). See @ref Platform-Sdl2Application-dpi
         * for more information.
         * @see @ref Sdl2Application::windowSize()
         */
        Vector2i windowSize() const { return _windowSize; }

        #if defined(MAGNUM_TARGET_GL) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Framebuffer size
         *
         * On some platforms with HiDPI displays, framebuffer size can be
         * different from @ref windowSize(). See
         * @ref Platform-Sdl2Application-dpi for more information.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         *
         * @see @ref Sdl2Application::framebufferSize()
         */
        Vector2i framebufferSize() const { return _framebufferSize; }
        #endif

        /**
         * @brief DPI scaling
         *
         * On some platforms moving an app between displays can result in DPI
         * scaling value being changed in tandem with a window/framebuffer
         * size. Simply resizing a window doesn't change the DPI scaling value.
         * See @ref Platform-Sdl2Application-dpi for more information.
         * @see @ref Sdl2Application::dpiScaling()
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Underlying SDL event
         *
         * Of type `SDL_WINDOWEVENT`.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref Sdl2Application::anyEvent()
         */
        const SDL_Event& event() const { return _event; }
        #endif

    private:
        friend Sdl2Application;

        explicit ViewportEvent(
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            const SDL_Event& event,
            #endif
            const Vector2i& windowSize,
            #ifdef MAGNUM_TARGET_GL
            const Vector2i& framebufferSize,
            #endif
            const Vector2& dpiScaling):
                #ifndef CORRADE_TARGET_EMSCRIPTEN
                _event(event),
                #endif
                _windowSize{windowSize},
                #ifdef MAGNUM_TARGET_GL
                _framebufferSize{framebufferSize},
                #endif
                _dpiScaling{dpiScaling} {}

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        const SDL_Event& _event;
        #endif
        const Vector2i _windowSize;
        #ifdef MAGNUM_TARGET_GL
        const Vector2i _framebufferSize;
        #endif
        const Vector2 _dpiScaling;
};

/**
@brief Window focus and blur event
@m_since_latest

@see @ref focusEvent(), @ref blurEvent()
*/
class Sdl2Application::FocusEvent {
    public:
        /** @brief Copying is not allowed */
        FocusEvent(const FocusEvent&) = delete;

        /** @brief Moving is not allowed */
        FocusEvent(FocusEvent&&) = delete;

        /** @brief Copying is not allowed */
        FocusEvent& operator=(const FocusEvent&) = delete;

        /** @brief Moving is not allowed */
        FocusEvent& operator=(FocusEvent&&) = delete;

        /**
         * @brief Underlying SDL event
         *
         * Of type `SDL_WINDOWEVENT`
         * @see @ref Sdl2Application::anyEvent()
         */
        const SDL_Event& event() const { return _event; }

    private:
        friend Sdl2Application;

        explicit FocusEvent(const SDL_Event& event): _event(event) {}

        const SDL_Event& _event;
};

/**
@brief Base for input events

@see @ref KeyEvent, @ref PointerEvent, @ref PointerMoveEvent, @ref ScrollEvent,
    @ref keyPressEvent(), @ref keyReleaseEvent(), @ref pointerPressEvent(),
    @ref pointerReleaseEvent(), @ref pointerMoveEvent(), @ref scrollEvent()
*/
class Sdl2Application::InputEvent {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief Sdl2Application::Modifier
         * @m_deprecated_since_latest Use @ref Sdl2Application::Modifier
         *      instead.
         */
        typedef CORRADE_DEPRECATED("use Sdl2Application::Modifier instead") Sdl2Application::Modifier Modifier;

        /**
         * @brief @copybrief Sdl2Application::Modifiers
         * @m_deprecated_since_latest Use @ref Sdl2Application::Modifiers
         *      instead.
         */
        typedef CORRADE_DEPRECATED("use Sdl2Application::Modifiers instead") Sdl2Application::Modifiers Modifiers;
        #endif

        /** @brief Copying is not allowed */
        InputEvent(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent(InputEvent&&) = delete;

        /** @brief Copying is not allowed */
        InputEvent& operator=(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent& operator=(InputEvent&&) = delete;

        /** @brief Whether the event is accepted */
        bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /**
         * @brief Underlying SDL event
         *
         * Of type `SDL_KEYDOWN` / `SDL_KEYUP` for @ref KeyEvent,
         * `SDL_MOUSEBUTTONDOWN` / `SDL_MOUSEBUTTONUP` or `SDL_FINGERDOWN` /
         * `SDL_FINGERUP` for @ref PointerEvent, `SDL_MOUSEMOTION` or
         * `SDL_FINGERMOTION` for @ref PointerMoveEvent and `SDL_MOUSEWHEEL`
         * for @ref ScrollEvent.
         * @see @ref Sdl2Application::anyEvent()
         */
        const SDL_Event& event() const { return _event; }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        explicit InputEvent(const SDL_Event& event): _event(event), _accepted(false) {}

        ~InputEvent() = default;
    #endif

    private:
        const SDL_Event& _event;
        bool _accepted;
};

/**
@brief Key event

@see @ref keyPressEvent(), @ref keyReleaseEvent(),
    @ref platform-windowed-key-events
*/
class Sdl2Application::KeyEvent: public Sdl2Application::InputEvent {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief Sdl2Application::Key
         * @m_deprecated_since_latest Use @ref Sdl2Application::Key instead.
         */
        typedef CORRADE_DEPRECATED("use Sdl2Application::Key instead") Sdl2Application::Key Key;

        /**
         * @brief @copybrief Sdl2Application::keyName()
         * @m_deprecated_since_latest Use @ref Sdl2Application::keyName()
         *      instead.
         */
        CORRADE_DEPRECATED("use Sdl2Application::keyName() instead") static Containers::StringView keyName(Sdl2Application::Key key);
        #endif

        /**
         * @brief Key
         *
         * Layout-dependent name of given key. Use @ref scanCode() to get a
         * platform-specific but layout-independent identifier of given key.
         * @see @ref keyName() const, @ref keyToScanCode()
         */
        Sdl2Application::Key key() const { return _key; }

        /**
         * @brief Scancode
         * @m_since_latest
         *
         * Platform-specific but layout-independent identifier of given key.
         * Use @ref key() to get a key name in the currently used layout.
         * @see @ref scanCodeName(), @ref scanCodeToKey()
         */
        UnsignedInt scanCode() const { return _scancode; }

        /**
         * @brief Key name
         *
         * Human-readable localized UTF-8 name for the key returned by
         * @ref key(), intended for displaying to the user in e.g. a key
         * binding configuration. If there is no name for that key, an empty
         * string is returned. The returned string is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * and is valid at least until the next call to this function, to
         * @ref Sdl2Application::keyName() or to the underlying
         * @cpp SDL_GetKeyName() @ce API.
         * @see @ref scanCodeName()
         */
        Containers::StringView keyName() const;

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Scan code name
         * @m_since_latest
         *
         * Human-readable localized UTF-8 name for the scancode returned by
         * @ref scanCode(), intended for displaying to the user in e.g. a key
         * binding configuration. If there is no name for that key, an
         * empty string is returned. The returned string is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * and is valid at least until the next call to this function, to
         * @ref Sdl2Application::scanCodeName() or to the underlying
         * @cpp SDL_GetScancodeName() @ce API.
         * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref keyName()
         */
        Containers::StringView scanCodeName() const;
        #endif

        /** @brief Keyboard modifiers */
        Sdl2Application::Modifiers modifiers() const { return _modifiers; }

        /**
         * @brief Whether the key press is repeated
         *
         * Returns @cpp true @ce if the key press event is repeated,
         * @cpp false @ce if not or if this was key release event.
         */
        bool isRepeated() const { return _repeated; }

    private:
        friend Sdl2Application;

        explicit KeyEvent(const SDL_Event& event, Sdl2Application::Key key, UnsignedInt scancode, Sdl2Application::Modifiers modifiers, bool repeated): InputEvent{event}, _repeated{repeated}, _modifiers{modifiers}, _key{key}, _scancode{scancode} {}

        const bool _repeated;
        const Sdl2Application::Modifiers _modifiers;
        const Sdl2Application::Key _key;
        const UnsignedInt _scancode;
};

/**
@brief Pointer event
@m_since_latest

@see @ref PointerMoveEvent, @ref pointerPressEvent(),
    @ref pointerReleaseEvent(), @ref platform-windowed-pointer-events,
    @ref Platform-Sdl2Application-touch
*/
class Sdl2Application::PointerEvent: public InputEvent {
    public:
        /** @brief Copying is not allowed */
        PointerEvent(const PointerEvent&) = delete;

        /** @brief Moving is not allowed */
        PointerEvent(PointerEvent&&) = delete;

        /** @brief Copying is not allowed */
        PointerEvent& operator=(const PointerEvent&) = delete;

        /** @brief Moving is not allowed */
        PointerEvent& operator=(PointerEvent&&) = delete;

        /** @brief Pointer event source */
        PointerEventSource source() const { return _source; }

        /** @brief Pointer type that was pressed or released */
        Pointer pointer() const { return _pointer; }

        /**
         * @brief Whether the pointer is primary
         *
         * Useful to distinguish among multiple pointers in a multi-touch
         * scenario. See @ref Platform-Sdl2Application-touch for more
         * information.
         */
        bool isPrimary() const { return _primary; }

        /**
         * @brief Pointer ID
         *
         * Useful to distinguish among multiple pointers in a multi-touch
         * scenario. See @ref Platform-Sdl2Application-touch for more
         * information.
         */
        Long id() const { return _id; }

        /**
         * @brief Position
         *
         * For mouse input the position is always reported in whole pixels. For
         * @ref Pointer::Finger the events may be reported with a subpixel
         * precision, use @ref Math::round() to snap them to the nearest pixel.
         * Note that, unlike the `SDL_TouchFingerEvent`, which is normalized in
         * the @f$ [0, 1] @f$ range, the position for touch events is in the
         * same coordinate system as mouse events.
         */
        Vector2 position() const { return _position; }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Click count
         *
         * For @ref Pointer::Finger is always @cpp 1 @ce.
         * @note Not available on @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        Int clickCount() const { return _clickCount; }
        #endif

        /**
         * @brief Keyboard modifiers
         *
         * Lazily populated on first request.
         */
        Sdl2Application::Modifiers modifiers();

    private:
        friend Sdl2Application;

        explicit PointerEvent(const SDL_Event& event, PointerEventSource source, Pointer pointer, bool primary, Long id, const Vector2& position
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            , Int clickCount
            #endif
        ): InputEvent{event}, _source{source}, _pointer{pointer}, _primary{primary}, _id{id}, _position{position}
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            , _clickCount{clickCount}
            #endif
        {}

        const PointerEventSource _source;
        const Pointer _pointer;
        Containers::Optional<Sdl2Application::Modifiers> _modifiers;
        const bool _primary;
        const Long _id;
        const Vector2 _position;
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        const Int _clickCount;
        #endif
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Mouse event
@m_deprecated_since_latest Use @ref PointerEvent, @ref pointerPressEvent() and
    @ref pointerReleaseEvent() instead, which is a better abstraction for
    covering both mouse and touch / pen input.

@see @ref MouseMoveEvent, @ref MouseScrollEvent, @ref mousePressEvent(),
    @ref mouseReleaseEvent()
*/
class CORRADE_DEPRECATED("use PointerEvent, pointerPressEvent() and pointerReleaseEvent() instead") Sdl2Application::MouseEvent: public InputEvent {
    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: Uint8 {
            Left = SDL_BUTTON_LEFT,         /**< Left button */
            Middle = SDL_BUTTON_MIDDLE,     /**< Middle button */
            Right = SDL_BUTTON_RIGHT,       /**< Right button */

            /** First extra button (e.g. wheel left) */
            X1 = SDL_BUTTON_X1,

            /** Second extra button (e.g. wheel right) */
            X2 = SDL_BUTTON_X2,
        };

        /** @brief Button */
        Button button() const { return _button; }

        /** @brief Position */
        Vector2i position() const { return _position; }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Click count
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        Int clickCount() const { return _clickCount; }
        #endif

        /**
         * @brief Keyboard modifiers
         *
         * Lazily populated on first request.
         */
        Sdl2Application::Modifiers modifiers();

    private:
        friend Sdl2Application;

        explicit MouseEvent(const SDL_Event& event, Button button, const Vector2i& position
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            , Int clickCount
            #endif
            ): InputEvent{event}, _button{button}, _position{position}
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            , _clickCount{clickCount}
            #endif
            {}

        const Button _button;
        const Vector2i _position;
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        const Int _clickCount;
        #endif
        Containers::Optional<Sdl2Application::Modifiers> _modifiers;
};
#endif

/**
@brief Pointer move event
@m_since_latest

@see @ref PointerEvent, @ref pointerMoveEvent(),
    @ref platform-windowed-pointer-events, @ref Platform-Sdl2Application-touch
*/
class Sdl2Application::PointerMoveEvent: public InputEvent {
    public:
        /** @brief Copying is not allowed */
        PointerMoveEvent(const PointerMoveEvent&) = delete;

        /** @brief Moving is not allowed */
        PointerMoveEvent(PointerMoveEvent&&) = delete;

        /** @brief Copying is not allowed */
        PointerMoveEvent& operator=(const PointerMoveEvent&) = delete;

        /** @brief Moving is not allowed */
        PointerMoveEvent& operator=(PointerMoveEvent&&) = delete;

        /**
         * @brief Pointer event source
         *
         * Can be used to distinguish which source the event is coming from in
         * case it's a movement with both @ref pointer() and @ref pointers()
         * being empty.
         */
        PointerEventSource source() const { return _source; }

        /**
         * @brief Pointer type that was added or removed from the set of pressed pointers
         *
         * Use @ref pointers() to query the set of pointers pressed in this
         * event. This field is is non-empty only in case a mouse button was
         * pressed in addition to an already pressed button, or if one mouse
         * button from multiple pressed buttons was released. If non-empty and
         * @ref pointers() don't contain given @ref Pointer value, the button
         * was released, if they contain given value, the button was pressed.
         * @see @ref source()
         */
        Containers::Optional<Pointer> pointer() const { return _pointer; }

        /**
         * @brief Pointer types pressed in this event
         *
         * Returns an empty set if no pointers are pressed, which happens for
         * example when a mouse is just moved around.
         * @see @ref pointer()
         */
        Pointers pointers() const { return _pointers; }

        /**
         * @brief Whether the pointer is primary
         *
         * Useful to distinguish among multiple pointers in a multi-touch
         * scenario. See @ref Platform-Sdl2Application-touch for more
         * information.
         */
        bool isPrimary() const { return _primary; }

        /**
         * @brief Pointer ID
         *
         * Useful to distinguish among multiple pointers in a multi-touch
         * scenario. See @ref Platform-Sdl2Application-touch for more
         * information.
         */
        Long id() const { return _id; }

        /**
         * @brief Position
         *
         * For mouse input the position is always reported in whole pixels. For
         * @ref Pointer::Finger the events may be reported with a subpixel
         * precision. Use @ref Math::round() to snap them to the nearest pixel.
         * Note that, unlike the `SDL_TouchFingerEvent`, which is normalized in
         * the @f$ [0, 1] @f$ range, the position for touch events is in the
         * same coordinate system as mouse events.
         */
        Vector2 position() const { return _position; }

        /**
         * @brief Position relative to the previous touch event
         *
         * For mouse input the position is always reported in whole pixels. For
         * @ref Pointer::Finger the events may be reported with a subpixel
         * precision, meaning that the relative position might be less than a
         * pixel. Note that, unlike the `SDL_TouchFingerEvent`, which is
         * normalized in the @f$ [0, 1] @f$ range, the position for touch
         * events is in the same coordinate system as mouse events.
         */
        Vector2 relativePosition() const { return _relativePosition; }

        /**
         * @brief Keyboard modifiers
         *
         * Lazily populated on first request.
         */
        Sdl2Application::Modifiers modifiers();

    private:
        friend Sdl2Application;

        explicit PointerMoveEvent(const SDL_Event& event, PointerEventSource source, Containers::Optional<Pointer> pointer, Pointers pointers, bool primary, Long id, const Vector2& position, const Vector2& relativePosition): InputEvent{event}, _source{source}, _pointer{pointer}, _pointers{pointers}, _primary{primary}, _id{id}, _position{position}, _relativePosition{relativePosition} {}

        const PointerEventSource _source;
        const Containers::Optional<Pointer> _pointer;
        const Pointers _pointers;
        const bool _primary;
        Containers::Optional<Sdl2Application::Modifiers> _modifiers;
        const Long _id;
        const Vector2 _position, _relativePosition;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Mouse move event
@m_deprecated_since_latest Use @ref PointerMoveEvent and
    @ref pointerMoveEvent() instead, which is a better abstraction for covering
    both mouse and touch / pen input.

@see @ref MouseEvent, @ref MouseScrollEvent, @ref mouseMoveEvent()
*/
class CORRADE_DEPRECATED("use PointerMoveEvent and pointerMoveEvent() instead") Sdl2Application::MouseMoveEvent: public InputEvent {
    public:
        /**
         * @brief Mouse button
         *
         * @see @ref Buttons, @ref buttons()
         */
        enum class Button: Uint32 {
            Left = SDL_BUTTON_LMASK,        /**< Left button */
            Middle = SDL_BUTTON_MMASK,      /**< Middle button */
            Right = SDL_BUTTON_RMASK,       /**< Right button */

            /** First extra button (e.g. wheel left) */
            X1 = SDL_BUTTON_X1MASK,

            /** Second extra button (e.g. wheel right) */
            X2 = SDL_BUTTON_X2MASK
        };

        /**
         * @brief Set of mouse buttons
         *
         * @see @ref buttons()
         */
        typedef Containers::EnumSet<Button> Buttons;

        /** @brief Position */
        Vector2i position() const { return _position; }

        /**
         * @brief Relative position
         *
         * Position relative to previous move event.
         */
        Vector2i relativePosition() const { return _relativePosition; }

        /** @brief Mouse buttons */
        Buttons buttons() const { return _buttons; }

        /**
         * @brief Keyboard modifiers
         *
         * Lazily populated on first request.
         */
        Sdl2Application::Modifiers modifiers();

    private:
        friend Sdl2Application;

        explicit MouseMoveEvent(const SDL_Event& event, const Vector2i& position, const Vector2i& relativePosition, Buttons buttons): InputEvent{event}, _position{position}, _relativePosition{relativePosition}, _buttons{buttons} {}

        const Vector2i _position, _relativePosition;
        const Buttons _buttons;
        Containers::Optional<Sdl2Application::Modifiers> _modifiers;
};

CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_ENUMSET_OPERATORS(Sdl2Application::MouseMoveEvent::Buttons)
CORRADE_IGNORE_DEPRECATED_POP
#endif

/**
@brief Scroll event
@m_since_latest

@see @ref PointerEvent, @ref PointerMoveEvent, @ref scrollEvent(),
    @ref platform-windowed-pointer-events
*/
class Sdl2Application::ScrollEvent: public Sdl2Application::InputEvent {
    public:
        /**
         * @brief Scroll offset
         *
         * @note SDL versions before 2.0.18 only report an integer value, which
         *      may result in the returned value being zero if the offset
         *      reported by the system is a fractional value less than one.
         */
        Vector2 offset() const { return _offset; }

        /**
         * @brief Position
         *
         * For mouse input the position is always reported in whole pixels. On
         * SDL versions before 2.26.0 is lazily populated on first request.
         */
        /* Yeah, it's 2.0.22 and then 2.24.0, they changed the versioning */
        #if SDL_VERSION_ATLEAST(2, 26, 0)
        Vector2 position() const { return _position; }
        #else
        Vector2 position();
        #endif

        /**
         * @brief Keyboard modifiers
         *
         * Lazily populated on first request.
         */
        Sdl2Application::Modifiers modifiers();

    private:
        friend Sdl2Application;

        explicit ScrollEvent(const SDL_Event& event, const Vector2& offset
            /* Yeah, it's 2.0.22 and then 2.24.0, they changed the
               versioning */
            #if SDL_VERSION_ATLEAST(2, 26, 0)
            , const Vector2& position
            #endif
        ): InputEvent{event}, _offset{offset}
            /* Yeah, it's 2.0.22 and then 2.24.0, they changed the
               versioning */
            #if SDL_VERSION_ATLEAST(2, 26, 0)
            , _position{position}
            #endif
            {}

        const Vector2 _offset;
        /* Yeah, it's 2.0.22 and then 2.24.0, they changed the versioning */
        #if SDL_VERSION_ATLEAST(2, 26, 0)
        const Vector2 _position;
        #else
        Containers::Optional<Vector2> _position;
        #endif
        Containers::Optional<Sdl2Application::Modifiers> _modifiers;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Mouse scroll event
@m_deprecated_since_latest Use @ref ScrollEvent and @ref scrollEvent() instead,
    which isn't semantically tied to just a mouse.

@see @ref MouseEvent, @ref MouseMoveEvent, @ref mouseScrollEvent()
*/
class CORRADE_DEPRECATED("use ScrollEvent and scrollEvent() instead") Sdl2Application::MouseScrollEvent: public InputEvent {
    public:
        /** @brief Scroll offset */
        Vector2 offset() const { return _offset; }

        /**
         * @brief Position
         *
         * Lazily populated on first request.
         */
        Vector2i position();

        /**
         * @brief Keyboard modifiers
         *
         * Lazily populated on first request.
         */
        Sdl2Application::Modifiers modifiers();

    private:
        friend Sdl2Application;

        explicit MouseScrollEvent(const SDL_Event& event, const Vector2& offset): InputEvent{event}, _offset{offset} {}

        const Vector2 _offset;
        Containers::Optional<Vector2i> _position;
        Containers::Optional<Sdl2Application::Modifiers> _modifiers;
};

/**
@brief Multi gesture event
@m_deprecated_since_latest The SDL event reports relative distance to previous
    finger positions instead of a relative diameter between multiple pressed
    fingers, making it practically useless for implementing pinch zoom. It
    additionally reports everything in normalized coordinates, thus the
    distance and rotation is only meaningful when the window is an exact
    square. Use the @ref TwoFingerGesture helper instead and feed it with
    events coming from @ref pointerPressEvent(), @ref pointerReleaseEvent() and
    @ref pointerMoveEvent().

@see @ref multiGestureEvent()
*/
class CORRADE_DEPRECATED("use TwoFingerGesture with pointerPressEvent(), pointerReleaseEvent() and pointerMoveEvent() instead") Sdl2Application::MultiGestureEvent {
    public:
        /** @brief Copying is not allowed */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        MultiGestureEvent(const MultiGestureEvent&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        /** @brief Moving is not allowed */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        MultiGestureEvent(MultiGestureEvent&&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        /** @brief Copying is not allowed */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        MultiGestureEvent& operator=(const MultiGestureEvent&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        /** @brief Moving is not allowed */
        CORRADE_IGNORE_DEPRECATED_PUSH /* GCC 4.8 warns due to the argument */
        MultiGestureEvent& operator=(MultiGestureEvent&&) = delete;
        CORRADE_IGNORE_DEPRECATED_POP

        /** @brief Whether the event is accepted */
        bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Gesture center */
        Vector2 center() const { return _center; }

        /**
         * @brief Relative rotation
         *
         * Rotation relative to previous event.
         */
        Float relativeRotation() const { return _relativeRotation; }

        /**
         * @brief Relative distance
         *
         * Distance of the fingers relative to previous event.
         */
        Float relativeDistance() const { return _relativeDistance; }

        /**
         * @brief Finger count
         *
         * Count of fingers performing the gesture.
         */
        Int fingerCount() const { return _fingerCount; }

        /**
         * @brief Underlying SDL event
         *
         * Of type `SDL_MULTIGESTURE`.
         * @see @ref Sdl2Application::anyEvent()
         */
        const SDL_Event& event() const { return _event; }

    private:
        friend Sdl2Application;

        explicit MultiGestureEvent(const SDL_Event& event, const Vector2& center, Float relativeRotation, Float relativeDistance, Int fingerCount): _event(event), _center{center}, _relativeRotation{relativeRotation}, _relativeDistance{relativeDistance}, _fingerCount{fingerCount}, _accepted{false} {}

        const SDL_Event& _event;
        const Vector2 _center;
        const Float _relativeRotation;
        const Float _relativeDistance;
        const Int _fingerCount;
        bool _accepted;
};
#endif

/**
@brief Text input event

@see @ref TextEditingEvent, @ref textInputEvent(),
    @ref platform-windowed-key-events
*/
class Sdl2Application::TextInputEvent {
    public:
        /** @brief Copying is not allowed */
        TextInputEvent(const TextInputEvent&) = delete;

        /** @brief Moving is not allowed */
        TextInputEvent(TextInputEvent&&) = delete;

        /** @brief Copying is not allowed */
        TextInputEvent& operator=(const TextInputEvent&) = delete;

        /** @brief Moving is not allowed */
        TextInputEvent& operator=(TextInputEvent&&) = delete;

        /** @brief Whether the event is accepted */
        bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /**
         * @brief Input text
         *
         * The returned view is in UTF-8 and is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}.
         */
        Containers::StringView text() const { return _text; }

        /**
         * @brief Underlying SDL event
         *
         * Of type `SDL_TEXTINPUT`.
         * @see @ref Sdl2Application::anyEvent()
         */
        const SDL_Event& event() const { return _event; }

    private:
        friend Sdl2Application;

        explicit TextInputEvent(const SDL_Event& event, Containers::StringView text): _event(event), _text{text}, _accepted{false} {}

        const SDL_Event& _event;
        const Containers::StringView _text;
        bool _accepted;
};

/**
@brief Text editing event

@see @ref textEditingEvent(), @ref platform-windowed-key-events
*/
class Sdl2Application::TextEditingEvent {
    public:
        /** @brief Copying is not allowed */
        TextEditingEvent(const TextEditingEvent&) = delete;

        /** @brief Moving is not allowed */
        TextEditingEvent(TextEditingEvent&&) = delete;

        /** @brief Copying is not allowed */
        TextEditingEvent& operator=(const TextEditingEvent&) = delete;

        /** @brief Moving is not allowed */
        TextEditingEvent& operator=(TextEditingEvent&&) = delete;

        /** @brief Whether the event is accepted */
        bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /**
         * @brief Input text
         *
         * The returned view is in UTF-8 and is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}.
         */
        Containers::StringView text() const { return _text; }

        /** @brief Location to begin editing from */
        Int start() const { return _start; }

        /** @brief Number of characters to edit from the start point */
        Int length() const { return _length; }

        /**
         * @brief Underlying SDL event
         *
         * Of type `SDL_TEXTEDITING`.
         * @see @ref Sdl2Application::anyEvent()
         */
        const SDL_Event& event() const { return _event; }

    private:
        friend Sdl2Application;

        explicit TextEditingEvent(const SDL_Event& event, Containers::StringView text, Int start, Int length): _event(event), _text{text}, _start{start}, _length{length}, _accepted{false} {}

        const SDL_Event& _event;
        const Containers::StringView _text;
        const Int _start;
        const Int _length;
        bool _accepted;
};

/** @hideinitializer
@brief Entry point for SDL2-based applications
@param className Class name

@m_keywords{MAGNUM_APPLICATION_MAIN()}

See @ref Magnum::Platform::Sdl2Application "Platform::Sdl2Application" for
usage information. This macro abstracts out platform-specific entry point code
and is equivalent to the following on all supported platforms except
@ref CORRADE_TARGET_WINDOWS_RT "Windows RT", see @ref portability-applications
for more information.

@code{.cpp}
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode

When no other application header is included this macro is also aliased to
@cpp MAGNUM_APPLICATION_MAIN() @ce.
*/
#ifndef CORRADE_TARGET_WINDOWS_RT
#define MAGNUM_SDL2APPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }
#else
#define MAGNUM_SDL2APPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }                                                                       \
    __pragma(warning(push))                                                 \
    __pragma(warning(disable: 4447))                                        \
    int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {                \
        if(FAILED(Windows::Foundation::Initialize(RO_INIT_MULTITHREADED)))  \
            return 1;                                                       \
        return SDL_WinRTRunApp(main, nullptr);                              \
    }                                                                       \
    __pragma(warning(pop))
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef Sdl2Application Application;
typedef BasicScreen<Sdl2Application> Screen;
typedef BasicScreenedApplication<Sdl2Application> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_SDL2APPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

CORRADE_ENUMSET_OPERATORS(Sdl2Application::Configuration::WindowFlags)

}}

#endif
