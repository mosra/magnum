#ifndef Magnum_Platform_Sdl2Application_h
#define Magnum_Platform_Sdl2Application_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

/** @file
 * @brief Class @ref Magnum::Platform::Sdl2Application, macro @ref MAGNUM_SDL2APPLICATION_MAIN()
 */

#include <memory>
#include <Corrade/Corrade.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Platform/Platform.h"

#ifdef CORRADE_TARGET_WINDOWS /* Windows version of SDL2 redefines main(), we don't want that */
#define SDL_MAIN_HANDLED
#endif
#include <SDL.h>
#include <SDL_scancode.h>

#ifdef CORRADE_TARGET_WINDOWS_RT
#include <SDL_main.h> /* For SDL_WinRTRunApp */
#include <wrl.h> /* For the WinMain entrypoint */
#endif

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief SDL2 application

Application using [Simple DirectMedia Layer](http://www.libsdl.org/) toolkit.
Supports keyboard and mouse handling.

This application library is in theory available for all platforms for which
SDL2 is ported (thus also @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten", see
respective sections in @ref building-corrade-cross-emscripten "Corrade's" and
@ref building-cross-emscripten "Magnum's" building documentation). It depends
on **SDL2** library (Emscripten has it built in) and is built if
`WITH_SDL2APPLICATION` is enabled in CMake.

## Bootstrap application

Fully contained base application using @ref Sdl2Application along with
CMake setup is available in `base` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base.zip) file.
After extracting the downloaded archive you can build and run the application
with these four commands:

    mkdir build && cd build
    cmake ..
    cmake --build .
    ./src/MyApplication # or ./src/Debug/MyApplication

See @ref cmake for more information.

## Bootstrap application for Emscripten

Fully contained base application using @ref Sdl2Application for both desktop
and Emscripten build along with full HTML markup and CMake setup is available
in `base-emscripten` branch of [Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap)
repository, download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-emscripten.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-emscripten.zip)
file. After extracting the downloaded archive, you can do the desktop build in
the same way as above. For the Emscripten build you also need to put the
contents of toolchains repository from https://github.com/mosra/toolchains
in `toolchains/` subdirectory. There are two toolchain files. The
`generic/Emscripten.cmake` is for the classical (asm.js) build, the
`generic/Emscripten-wasm.cmake` is for WebAssembly build. Don't forget to adapt
`EMSCRIPTEN_PREFIX` variable in `toolchains/generic/Emscripten*.cmake` to path
where Emscripten is installed; you can also pass it explicitly on command-line
using `-DEMSCRIPTEN_PREFIX`. Default is `/usr/emscripten`.

Then create build directory and run `cmake` and build/install commands in it.
Set `CMAKE_PREFIX_PATH` to where you have all the dependencies installed, set
`CMAKE_INSTALL_PREFIX` to have the files installed in proper location (a
webserver, e.g.  `/srv/http/emscripten`).

    mkdir build-emscripten && cd build-emscripten
    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/Emscripten.cmake" \
        -DCMAKE_PREFIX_PATH=/usr/lib/emscripten/system \
        -DCMAKE_INSTALL_PREFIX=/srv/http/emscripten
    cmake --build .
    cmake --build . --target install

You can then open `MyApplication.html` in your browser (through webserver, e.g.
`http://localhost/emscripten/MyApplication.html`).

## Bootstrap application for iOS

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

    mkdir build-ios && cd build-ios
    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=../toolchains/generic/iOS.cmake \
        -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk \
        -DCMAKE_OSX_ARCHITECTURES="arm64;armv7;armv7s" \
        -DCMAKE_PREFIX_PATH=~/ios-libs \
        -G Xcode

You can then open the generated project file in Xcode and build/deploy it from
there.

## Bootstrap application for Windows RT

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

    mkdir build-winrt && cd build-winrt
    cmake -DCORRADE_RC_EXECUTABLE="C:/Sys/bin/corrade-rc.exe" -DCMAKE_PREFIX_PATH="C:/Sys-winrt" -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=8.1 -G "Visual Studio 14 2015" -DSIGNING_CERTIFICATE=<path-to-your-pfx-file> ..
    cmake --build .

Change `WindowsStore` to `WindowsPhone` if you want to build for Windows Phone instead. The `build-winrt/src/AppPackages` directory will then contain the
final package along with a PowerShell script for easy local installation.

## General usage

For CMake you need to copy `FindSDL2.cmake` from `modules/` directory in
Magnum source to `modules/` dir in your project (so it is able to find SDL2).
In case of Emscripten you need also `FindOpenGLES2.cmake`. Request
`Sdl2Application` component of `Magnum` package and link to
`Magnum::Sdl2Application` target. If no other application is requested, you can
also use generic `Magnum::Application` alias to simplify porting. Again, see
@ref building and @ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw on the
screen. The subclass can be then used directly in `main()` -- see convenience
macro @ref MAGNUM_SDL2APPLICATION_MAIN(). See @ref platform for more
information.
@code
class MyApplication: public Platform::Sdl2Application {
    // implement required methods...
};
MAGNUM_SDL2APPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.

## Usage with Emscripten

If you are targetting Emscripten, you need to provide HTML markup for your
application. Template one is below or in the bootstrap application, you can
modify it to your liking. The markup references two files,
`EmscriptenApplication.js` and `WebApplication.css`, both are in `Platform/`
directory in the source tree and are also installed into `share/magnum/` inside
your Emscripten toolchain. Change `&lt;application&gt;` to name of your
executable.
@code
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <title>Magnum Emscripten Application</title>
    <meta charset="utf-8" />
    <link rel="stylesheet" href="WebApplication.css" />
  </head>
  <body>
    <h1>Magnum Emscripten Application</h1>
    <div id="listener">
      <canvas id="module"></canvas>
      <div id="status">Initialization...</div>
      <div id="statusDescription"></div>
      <script src="EmscriptenApplication.js"></script>
      <script async="async" src="<application>.js"></script>
    </div>
  </body>
</html>
@endcode

You can modify all the files to your liking, but the HTML file must contain at
least the `&lt;canvas&gt;` enclosed in listener `&lt;div&gt;`. The JavaScript
file contains event listeners which print loading status on the page. The
status displayed in the remaining two `&lt;div&gt;`s, if they are available.
The CSS file contains rudimentary style to avoid eye bleeding.

The application redirects all output (thus also @ref Corrade::Utility::Debug "Debug",
@ref Corrade::Utility::Warning "Warning" and @ref Corrade::Utility::Error "Error")
to JavaScript console. It's possible to pass command-line arguments to `main()`
using GET URL parameters. For example, `/app/?foo=bar&fizz&buzz=3` will go to
the app as `['--foo', 'bar', '--fizz', '--buzz', '3']`.

## Usage with iOS

A lot of options for iOS build (such as HiDPI/Retina support, supported display
orientation, icons, splash screen...) is specified through the `*.plist` file.
CMake uses its own template that can be configured using various `MACOSX_BUNDLE_*`
variables, but many options are missing from there and you are much better off
rolling your own template and passing **abosolute** path to it to CMake using
the `MACOSX_BUNDLE_INFO_PLIST` property. Below are contents of the `*.plist`
file used in the bootstrap application, requesting OpenGL ES 2.0 and
advertising Retina support:
@code
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en-US</string>
    <key>CFBundleExecutable</key>
    <string>${MACOSX_BUNDLE_EXECUTABLE_NAME}</string>
    <key>CFBundleIdentifier</key>
    <string>cz.mosra.magnum.MyApplication</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>My Application</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>

    <key>UIRequiredDeviceCapabilities</key>
    <array>
        <string>opengles-2</string>
    </array>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
@endcode

Some other options can be configured from runtime when creating the SDL2
application window, see documentation of particular value for details:

-   @ref Configuration::WindowFlag::AllowHighDpi allows creating HiDPI/Retina
    drawable
-   @ref Configuration::WindowFlag::Borderless hides the menu bar
-   @ref Configuration::WindowFlag::Resizable makes the application respond to
    device orientation changes

## Usage with Windows RT

For Windows RT you need to provide logo images and splash screen, all
referenced from the `*.appxmanifest` file. The file is slightly different for
different targets, template for Windows Store and MSVC 2013 is below, others
are in the bootstrap application.
@code
<?xml version="1.0" encoding="utf-8"?>
<Package xmlns="http://schemas.microsoft.com/appx/2010/manifest" xmlns:m2="http://schemas.microsoft.com/appx/2013/manifest">
  <Identity Name="MyApplication" Publisher="CN=A Publisher" Version="1.1.0.0" />
  <Properties>
    <DisplayName>My Application</DisplayName>
    <PublisherDisplayName>A Publisher</PublisherDisplayName>
    <Logo>assets/logo-store.png</Logo>
  </Properties>
  <Resources>
    <Resource Language="x-generate" />
  </Resources>
  <Applications>
    <Application Id="App" Executable="$targetnametoken$.exe" EntryPoint="MyApplication.App">
      <m2:VisualElements
        DisplayName="Magnum Windows Store Application"
        Description="My Application"
        BackgroundColor="#202020"
        ForegroundText="light"
        Square150x150Logo="assets/logo.png"
        Square30x30Logo="assets/logo-small.png">
        <m2:SplashScreen Image="assets/splash.png" />
      </m2:VisualElements>
    </Application>
  </Applications>
</Package>
@endcode

The assets are referenced also from the main `CMakeLists.txt` file. You have to
mark all non-source files (except for the `*.pfx` key) with `VS_DEPLOYMENT_CONTENT`
property and optionally set their location with `VS_DEPLOYMENT_LOCATION`. If
you are using `*.resw` files, these need to have the `VS_TOOL_OVERRIDE`
property set to `PRIResource`.
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
        class InputEvent;
        class KeyEvent;
        class MouseEvent;
        class MouseMoveEvent;
        class MouseScrollEvent;
        class MultiGestureEvent;
        class TextInputEvent;
        class TextEditingEvent;

        /**
         * @brief Default constructor
         * @param arguments     Application arguments
         * @param configuration Configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref Configuration for more information. The program exits if
         * the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit Sdl2Application(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit Sdl2Application(const Arguments& arguments, const Configuration& configuration);
        explicit Sdl2Application(const Arguments& arguments);
        #endif

        /**
         * @brief Constructor
         * @param arguments     Application arguments
         *
         * Unlike above, the context is not created and must be created later
         * with @ref createContext() or @ref tryCreateContext().
         */
        explicit Sdl2Application(const Arguments& arguments, NoCreateT);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief Sdl2Application(const Arguments&, NoCreateT)
         * @deprecated Use @ref Sdl2Application(const Arguments&, NoCreateT) instead.
         */
        CORRADE_DEPRECATED("use Sdl2Application(const Arguments&, NoCreateT) instead") explicit Sdl2Application(const Arguments& arguments, std::nullptr_t): Sdl2Application{arguments, NoCreate} {}
        #endif

        /** @brief Copying is not allowed */
        Sdl2Application(const Sdl2Application&) = delete;

        /** @brief Moving is not allowed */
        Sdl2Application(Sdl2Application&&) = delete;

        /** @brief Copying is not allowed */
        Sdl2Application& operator=(const Sdl2Application&) = delete;

        /** @brief Moving is not allowed */
        Sdl2Application& operator=(Sdl2Application&&) = delete;

        /**
         * @brief Execute main loop
         * @return Value for returning from `main()`
         *
         * Calls @ref mainLoopIteration() in a loop until @ref exit() is
         * called. See @ref MAGNUM_SDL2APPLICATION_MAIN() for usage
         * information.
         */
        int exec();

        /**
         * @brief Exit application main loop
         *
         * Stops main loop started by @ref exec().
         */
        void exit();

        /**
         * @brief Run one iteration of application main loop
         *
         * Called internally from @ref exec(). If you want to have better
         * control over how the main loop behaves, you can call this function
         * yourself from your own `main()` function instead of it being called
         * automatically from @ref exec() / @ref MAGNUM_SDL2APPLICATION_MAIN().
         */
        void mainLoopIteration();

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Underlying window handle
         *
         * Use in case you need to call SDL functionality directly.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        SDL_Window* window() { return _window; }
        #endif

    protected:
        /* Nobody will need to have (and delete) Sdl2Application*, thus this is
           faster than public pure virtual destructor */
        ~Sdl2Application();

        /**
         * @brief Create context with given configuration
         *
         * Must be called if and only if the context wasn't created by the
         * constructor itself. Error message is printed and the program exits
         * if the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         *
         * On desktop GL, if version is not specified in @p configuration, the
         * application first tries to create core context (OpenGL 3.2+ on
         * macOS, OpenGL 3.1+ elsewhere) and if that fails, falls back to
         * compatibility OpenGL 2.1 context.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void createContext(const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        void createContext(const Configuration& configuration);
        void createContext();
        #endif

        /**
         * @brief Try to create context with given configuration
         *
         * Unlike @ref createContext() returns `false` if the context cannot be
         * created, `true` otherwise.
         */
        bool tryCreateContext(const Configuration& configuration);

        /** @{ @name Screen handling */

        /**
         * @brief Window size
         *
         * Window size to which all input event coordinates can be related.
         * Note that especially on HiDPI systems the reported window size might
         * not be the same as framebuffer size.
         */
        Vector2i windowSize();

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
         * Set `0` for no VSync, `1` for enabled VSync. Some platforms support
         * `-1` for late swap tearing. Prints error message and returns `false`
         * if swap interval cannot be set, `true` otherwise. Default is
         * driver-dependent, you can query the value with @ref swapInterval().
         * @see @ref setMinimalLoopPeriod()
         */
        bool setSwapInterval(Int interval);

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Set minimal loop period
         *
         * This setting reduces the main loop frequency in case VSync is
         * not/cannot be enabled or no drawing is done. Default is `0` (i.e.
         * looping at maximum frequency).
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten",
         *      the browser is managing the frequency instead.
         * @see @ref setSwapInterval()
         */
        void setMinimalLoopPeriod(UnsignedInt milliseconds) {
            _minimalLoopPeriod = milliseconds;
        }
        #endif

        /**
         * @brief Redraw immediately
         *
         * Marks the window for redrawing, resulting in call to @ref drawEvent()
         * in the next iteration. You can call it from @ref drawEvent() itself
         * to redraw immediately without waiting for user input.
         */
        void redraw() { _flags |= Flag::Redraw; }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Tick event
         *
         * If implemented, this function is called periodically after
         * processing all input events and before draw event even though there
         * might be no input events and redraw is not requested. Useful e.g.
         * for asynchronous task polling. Use @ref setMinimalLoopPeriod()/
         * @ref setSwapInterval() to control main loop frequency.
         */
        virtual void tickEvent();

        /**
         * @brief Viewport event
         *
         * Called when window size changes. The default implementation does
         * nothing, if you want to respond to size changes, you should pass the
         * new size to @ref DefaultFramebuffer::setViewport() and possibly
         * elsewhere (to @ref SceneGraph::Camera::setViewport(), other
         * framebuffers...).
         *
         * Note that this function might not get called at all if the window
         * size doesn't change. You should configure the initial state of your
         * cameras, framebuffers etc. in application constructor rather than
         * relying on this function to be called. Viewport of default
         * framebuffer can be retrieved via @ref DefaultFramebuffer::viewport().
         */
        virtual void viewportEvent(const Vector2i& size);

        /**
         * @brief Draw event
         *
         * Called when the screen is redrawn. You should clean the framebuffer
         * using @ref DefaultFramebuffer::clear() and then add your own drawing
         * functions. After drawing is finished, call @ref swapBuffers(). If
         * you want to draw immediately again, call also @ref redraw().
         */
        virtual void drawEvent() = 0;

        /*@}*/

        /** @{ @name Keyboard handling */

        /**
         * @brief Key press event
         *
         * Called when an key is pressed. Default implementation does nothing.
         */
        virtual void keyPressEvent(KeyEvent& event);

        /**
         * @brief Key release event
         *
         * Called when an key is released. Default implementation does nothing.
         */
        virtual void keyReleaseEvent(KeyEvent& event);

        /*@}*/

        /** @{ @name Mouse handling */

    public:
        /** @brief Whether mouse is locked */
        bool isMouseLocked() const { return SDL_GetRelativeMouseMode(); }

        /**
         * @brief Enable or disable mouse locking
         *
         * When mouse is locked, the cursor is hidden and only
         * @ref MouseMoveEvent::relativePosition() is changing, absolute
         * position stays the same.
         */
        void setMouseLocked(bool enabled);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Mouse press event
         *
         * Called when mouse button is pressed. Default implementation does
         * nothing.
         */
        virtual void mousePressEvent(MouseEvent& event);

        /**
         * @brief Mouse release event
         *
         * Called when mouse button is released. Default implementation does
         * nothing.
         */
        virtual void mouseReleaseEvent(MouseEvent& event);

        /**
         * @brief Mouse move event
         *
         * Called when mouse is moved. Default implementation does nothing.
         */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

        /**
         * @brief Mouse scroll event
         *
         * Called when a scrolling device is used (mouse wheel or scrolling
         * area on a touchpad). Default implementation does nothing.
         */
        virtual void mouseScrollEvent(MouseScrollEvent& event);

        /*@}*/

        /** @{ @name Touch gesture handling */

        /**
         * @brief Multi gesture event
         *
         * Called when the user performs a gesture using multiple fingers.
         * Default implementation does nothing.
         * @experimental
         */
        virtual void multiGestureEvent(MultiGestureEvent& event);

        /*@}*/

        /** @{ @name Text input handling */
    public:
        /**
         * @brief Whether text input is active
         *
         * If text input is active, text input events go to @ref textInputEvent()
         * and @ref textEditingEvent().
         * @note Note that in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" the
         *      value is emulated and might not reflect external events like
         *      closing on-screen keyboard.
         * @see @ref startTextInput(), @ref stopTextInput()
         */
        bool isTextInputActive();

        /**
         * @brief Start text input
         *
         * Starts text input that will go to @ref textInputEvent() and
         * @ref textEditingEvent().
         * @see @ref stopTextInput(), @ref isTextInputActive(),
         *      @ref setTextInputRect()
         */
        void startTextInput();

        /**
         * @brief Stop text input
         *
         * Stops text input that went to @ref textInputEvent() and
         * @ref textEditingEvent().
         * @see @ref startTextInput(), @ref isTextInputActive(), @ref textInputEvent()
         *      @ref textEditingEvent()
         */
        void stopTextInput();

        /**
         * @brief Set text input rectangle
         *
         * The @p rect defines an area where the text is being displayed, for
         * example to hint the system where to place on-screen keyboard.
         */
        void setTextInputRect(const Range2Di& rect);

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Text input event
         *
         * Called when text input is active and the text is being input.
         * @see @ref isTextInputActive()
         */
        virtual void textInputEvent(TextInputEvent& event);

        /**
         * @brief Text editing event
         *
         * Called when text input is active and the text is being edited.
         */
        virtual void textEditingEvent(TextEditingEvent& event);

        /*@}*/

    private:
        enum class Flag: UnsignedByte {
            Redraw = 1 << 0,
            VSyncEnabled = 1 << 1,
            NoTickEvent = 1 << 2,
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            Exit = 1 << 3
            #endif
            #ifdef CORRADE_TARGET_EMSCRIPTEN
            TextInputActive = 1 << 4
            #endif
        };

        typedef Containers::EnumSet<Flag> Flags;
        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        SDL_Window* _window;
        SDL_GLContext _glContext;
        UnsignedInt _minimalLoopPeriod;
        #else
        SDL_Surface* _glContext;
        #endif

        std::unique_ptr<Platform::Context> _context;

        Flags _flags;
};

/**
@brief Configuration

The created window is always with double-buffered OpenGL context and 24bit
depth buffer.
@see @ref Sdl2Application(), @ref createContext(), @ref tryCreateContext()
*/
class Sdl2Application::Configuration {
    public:
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Context flag
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref Flags, @ref setFlags(), @ref Context::Flag
         * @todo re-enable when Emscripten has proper SDL2 support
         */
        enum class Flag: int {
            Debug = SDL_GL_CONTEXT_DEBUG_FLAG,  /**< Create debug context */

            /** Create context with robust access */
            RobustAccess = SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG,

            /** Create context with reset isolation */
            ResetIsolation = SDL_GL_CONTEXT_RESET_ISOLATION_FLAG
        };

        /**
         * @brief Context flags
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         * @see @ref setFlags(), @ref Context::Flags
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typedef Containers::EnumSet<Flag, SDL_GL_CONTEXT_DEBUG_FLAG|
            SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG|SDL_GL_CONTEXT_RESET_ISOLATION_FLAG> Flags;
        #else
        typedef Containers::EnumSet<Flag> Flags;
        #endif
        #endif

        /**
         * @brief Window flag
         *
         * @see @ref WindowFlags, @ref setWindowFlags()
         */
        enum class WindowFlag: Uint32 {
            /**
             * Resizable window. On iOS this allows the application to respond
             * to display orientation changes. Implement @ref viewportEvent()
             * to react to the resizing events.
             */
            Resizable = SDL_WINDOW_RESIZABLE,

            Fullscreen = SDL_WINDOW_FULLSCREEN,     /**< Fullscreen window */

            /** No window decoration. On iOS this hides the menu bar. */
            Borderless = SDL_WINDOW_BORDERLESS,

            #ifndef CORRADE_TARGET_EMSCRIPTEN
            /**
             * Allow high DPI. On iOS you also have to set the
             * `NSHighResolutionCapable` entry in the `*.plist` file to make
             * it working.
             * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
             */
            AllowHighDpi = SDL_WINDOW_ALLOW_HIGHDPI,
            #endif

            Hidden = SDL_WINDOW_HIDDEN,             /**< Hidden window */
            Maximized = SDL_WINDOW_MAXIMIZED,       /**< Maximized window */
            Minimized = SDL_WINDOW_MINIMIZED,       /**< Minimized window */
            MouseLocked = SDL_WINDOW_INPUT_GRABBED  /**< Window with mouse locked */
        };

        /**
         * @brief Window flags
         *
         * @see @ref setWindowFlags()
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typedef Containers::EnumSet<WindowFlag, SDL_WINDOW_RESIZABLE|
            SDL_WINDOW_FULLSCREEN|SDL_WINDOW_HIDDEN|SDL_WINDOW_MAXIMIZED|
            SDL_WINDOW_MINIMIZED|SDL_WINDOW_INPUT_GRABBED> WindowFlags;
        #else
        typedef Containers::EnumSet<WindowFlag> WindowFlags;
        #endif

        /*implicit*/ Configuration();
        ~Configuration();

        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
        /**
         * @brief Window title
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten"
         *      and @ref CORRADE_TARGET_IOS "iOS".
         */
        std::string title() const { return _title; }
        #endif

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * Default is `"Magnum SDL2 Application"`.
         * @note In @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" and
         *      @ref CORRADE_TARGET_IOS "iOS" this function does nothing and is
         *      included only for compatibility. You need to set the title
         *      separately in platform-specific configuration file.
         */
        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
        Configuration& setTitle(std::string title) {
            _title = std::move(title);
            return *this;
        }
        #else
        template<class T> Configuration& setTitle(const T&) { return *this; }
        #endif

        /** @brief Window size */
        Vector2i size() const { return _size; }

        /**
         * @brief Set window size
         * @return Reference to self (for method chaining)
         *
         * Default is `{800, 600}`. On iOS it defaults to a "reasonable" size
         * based on whether HiDPI support is enabled using
         * @ref WindowFlag::AllowHighDpi, but not necessarily native display
         * resolution (you have to set it explicitly).
         */
        Configuration& setSize(const Vector2i& size) {
            _size = size;
            return *this;
        }

        /** @brief Window flags */
        WindowFlags windowFlags() const { return _windowFlags; }

        /**
         * @brief Set window flags
         * @return Reference to self (for method chaining)
         *
         * Default are none.
         */
        Configuration& setWindowFlags(WindowFlags flags) {
            _windowFlags = flags;
            return *this;
        }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Context flags
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        Flags flags() const { return _flags; }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is no flag. See also @ref Context::flags().
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        /**
         * @brief Context version
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        Version version() const { return _version; }
        #endif

        /**
         * @brief Set context version
         *
         * If requesting version greater or equal to OpenGL 3.1, core profile
         * is used. The created context will then have any version which is
         * backwards-compatible with requested one. Default is
         * @ref Version::None, i.e. any provided version is used.
         * @note In @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" this function
         *      does nothing (@ref Version::GLES200 or @ref Version::GLES300 is
         *      used implicitly based on the target).
         */
        Configuration& setVersion(Version version) {
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            _version = version;
            #else
            static_cast<void>(version);
            #endif
            return *this;
        }

        /** @brief Sample count */
        Int sampleCount() const { return _sampleCount; }

        /**
         * @brief Set sample count
         * @return Reference to self (for method chaining)
         *
         * Default is `0`, thus no multisampling. See also
         * @ref Renderer::Feature::Multisampling.
         */
        Configuration& setSampleCount(Int count) {
            _sampleCount = count;
            return *this;
        }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief sRGB-capable default framebuffer
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        bool isSRGBCapable() const { return _sRGBCapable; }

        /**
         * @brief Set sRGB-capable default framebuffer
         * @return Reference to self (for method chaining)
         *
         * Default is `false`. See also @ref Renderer::Feature::FramebufferSRGB.
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        Configuration& setSRGBCapable(bool enabled) {
            _sRGBCapable = enabled;
            return *this;
        }
        #endif

    private:
        #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_IOS)
        std::string _title;
        #endif
        Vector2i _size;
        WindowFlags _windowFlags;
        Int _sampleCount;
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        Version _version;
        Flags _flags;
        bool _sRGBCapable;
        #endif
};

/**
@brief Base for input events

@see @ref KeyEvent, @ref MouseEvent, @ref MouseMoveEvent, @ref keyPressEvent(),
    @ref keyReleaseEvent(), @ref mousePressEvent(), @ref mouseReleaseEvent(),
    @ref mouseMoveEvent()
*/
class Sdl2Application::InputEvent {
    public:
        /**
         * @brief Modifier
         *
         * @see @ref Modifiers, @ref KeyEvent::modifiers(),
         *      @ref MouseEvent::modifiers(), @ref MouseMoveEvent::modifiers()
         */
        enum class Modifier: Uint16 {
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
             */
            AltGr = KMOD_MODE,

            CapsLock = KMOD_CAPS,       /**< Caps lock */
            NumLock = KMOD_NUM          /**< Num lock */
        };

        /**
         * @brief Set of modifiers
         *
         * @see @ref KeyEvent::modifiers(), @ref MouseEvent::modifiers(),
         *      @ref MouseMoveEvent::modifiers()
         */
        typedef Containers::EnumSet<Modifier> Modifiers;

        /** @brief Copying is not allowed */
        InputEvent(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent(InputEvent&&) = delete;

        /** @brief Copying is not allowed */
        InputEvent& operator=(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent& operator=(InputEvent&&) = delete;

        /** @brief Whether the event is accepted */
        constexpr bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        constexpr explicit InputEvent(): _accepted(false) {}

        ~InputEvent() = default;
    #endif

    private:
        bool _accepted;
};

/**
@brief Key event

@see @ref keyPressEvent(), @ref keyReleaseEvent()
*/
class Sdl2Application::KeyEvent: public Sdl2Application::InputEvent {
    friend Sdl2Application;

    public:
        /**
         * @brief Key
         *
         * @see @ref key()
         */
        enum class Key: SDL_Keycode {
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
            Comma = SDLK_COMMA,         /**< Comma */
            Period = SDLK_PERIOD,       /**< Period */
            Minus = SDLK_MINUS,         /**< Minus */
            Plus = SDLK_PLUS,           /**< Plus */
            Slash = SDLK_SLASH,         /**< Slash */
            Percent = SDLK_PERCENT,     /**< Percent */
            Semicolon = SDLK_SEMICOLON, /**< Semicolon */
            Equal = SDLK_EQUALS,        /**< Equal */

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
            Z = SDLK_z                  /**< Letter Z */
        };

        /**
         * @brief Name for given key
         *
         * Human-readable localized UTF-8 name for given @p key, intended for
         * displaying to the user in e.g. key binding configuration. If there
         * is no name for given key, empty string is returned.
         * @see @ref keyName(Key)
         */
        static std::string keyName(Key key);

        /**
         * @brief Key
         *
         * @see @ref keyName()
         */
        constexpr Key key() const { return _key; }

        /**
         * @brief Key name
         *
         * Human-readable localized UTF-8 name for the key returned by
         * @ref key(), intended for displaying to the user in e.g.
         * key binding configuration. If there is no name for that key, empty
         * string is returned.
         * @see @ref keyName(Key)
         */
        std::string keyName() const;

        /** @brief Modifiers */
        constexpr Modifiers modifiers() const { return _modifiers; }

        /**
         * @brief Whether the key press is repeated
         *
         * Returns `true` if the key press event is repeated, `false` if not or
         * if this was key release event.
         */
        constexpr bool isRepeated() const { return _repeated; }

    private:
        constexpr KeyEvent(Key key, Modifiers modifiers, bool repeated): _key{key}, _modifiers{modifiers}, _repeated{repeated} {}

        const Key _key;
        const Modifiers _modifiers;
        const bool _repeated;
};

/**
@brief Mouse event

@see @ref MouseMoveEvent, @ref MouseScrollEvent, @ref mousePressEvent(),
    @ref mouseReleaseEvent()
*/
class Sdl2Application::MouseEvent: public Sdl2Application::InputEvent {
    friend Sdl2Application;

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

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Wheel up
             * @deprecated Use @ref MouseScrollEvent and @ref mouseScrollEvent() instead.
             */
            WheelUp CORRADE_DEPRECATED_ENUM("use mouseScrollEvent() and MouseScrollEvent instead") = SDL_BUTTON_X2 + 1,

            /**
             * Wheel down
             * @deprecated Use @ref MouseScrollEvent and @ref mouseScrollEvent() instead.
             */
            WheelDown CORRADE_DEPRECATED_ENUM("use mouseScrollEvent() and MouseScrollEvent instead") = SDL_BUTTON_X2 + 2
            #endif
        };

        /** @brief Button */
        constexpr Button button() const { return _button; }

        /** @brief Position */
        constexpr Vector2i position() const { return _position; }

        #ifndef CORRADE_TARGET_EMSCRIPTEN
        /**
         * @brief Click count
         *
         * @note Not available in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten".
         */
        constexpr Int clickCount() const { return _clickCount; }
        #endif

        /**
         * @brief Modifiers
         *
         * Lazily populated on first request.
         */
        Modifiers modifiers();

    private:
        constexpr MouseEvent(Button button, const Vector2i& position
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            , Int clickCount
            #endif
            ): _button{button}, _position{position},
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            _clickCount{clickCount},
            #endif
            _modifiersLoaded{false} {}

        const Button _button;
        const Vector2i _position;
        #ifndef CORRADE_TARGET_EMSCRIPTEN
        const Int _clickCount;
        #endif
        bool _modifiersLoaded;
        Modifiers _modifiers;
};

/**
@brief Mouse move event

@see @ref MouseEvent, @ref MouseScrollEvent, @ref mouseMoveEvent()
*/
class Sdl2Application::MouseMoveEvent: public Sdl2Application::InputEvent {
    friend Sdl2Application;

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
        constexpr Vector2i position() const { return _position; }

        /**
         * @brief Relative position
         *
         * Position relative to previous event.
         */
        constexpr Vector2i relativePosition() const { return _relativePosition; }

        /** @brief Mouse buttons */
        constexpr Buttons buttons() const { return _buttons; }

        /**
         * @brief Modifiers
         *
         * Lazily populated on first request.
         */
        Modifiers modifiers();

    private:
        constexpr MouseMoveEvent(const Vector2i& position, const Vector2i& relativePosition, Buttons buttons): _position{position}, _relativePosition{relativePosition}, _buttons{buttons}, _modifiersLoaded{false} {}

        const Vector2i _position, _relativePosition;
        const Buttons _buttons;
        bool _modifiersLoaded;
        Modifiers _modifiers;
};

/**
@brief Mouse scroll event

@see @ref MouseEvent, @ref MouseMoveEvent, @ref mouseScrollEvent()
*/
class Sdl2Application::MouseScrollEvent: public Sdl2Application::InputEvent {
    friend Sdl2Application;

    public:
        /** @brief Scroll offset */
        constexpr Vector2 offset() const { return _offset; }

        /**
         * @brief Modifiers
         *
         * Lazily populated on first request.
         */
        Modifiers modifiers();

    private:
        constexpr MouseScrollEvent(const Vector2& offset): _offset{offset}, _modifiersLoaded{false} {}

        const Vector2 _offset;
        bool _modifiersLoaded;
        Modifiers _modifiers;
};

/**
@brief Multi gesture event

@experimental
@see @ref multiGestureEvent()
*/
class Sdl2Application::MultiGestureEvent {
    friend Sdl2Application;

    public:
        /** @brief Copying is not allowed */
        MultiGestureEvent(const MultiGestureEvent&) = delete;

        /** @brief Moving is not allowed */
        MultiGestureEvent(MultiGestureEvent&&) = delete;

        /** @brief Copying is not allowed */
        MultiGestureEvent& operator=(const MultiGestureEvent&) = delete;

        /** @brief Moving is not allowed */
        MultiGestureEvent& operator=(MultiGestureEvent&&) = delete;

        /** @brief Whether the event is accepted */
        constexpr bool isAccepted() const { return _accepted; }

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

    private:
        constexpr MultiGestureEvent(const Vector2& center, Float relativeRotation, Float relativeDistance, Int fingerCount): _center{center}, _relativeRotation{relativeRotation}, _relativeDistance{relativeDistance}, _fingerCount{fingerCount}, _accepted{false} {}

        Vector2 _center;
        Float _relativeRotation,
            _relativeDistance;
        Int _fingerCount;
        bool _accepted;
};

/**
@brief Text input event

@see @ref TextEditingEvent, @ref textInputEvent()
*/
class Sdl2Application::TextInputEvent {
    friend Sdl2Application;

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
        constexpr bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Input text in UTF-8 */
        constexpr Containers::ArrayView<const char> text() const { return _text; }

    private:
        constexpr TextInputEvent(Containers::ArrayView<const char> text): _text{text}, _accepted{false} {}

        Containers::ArrayView<const char> _text;
        bool _accepted;
};

/**
@brief Text editing event

@see @ref textEditingEvent()
*/
class Sdl2Application::TextEditingEvent {
    friend Sdl2Application;

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
        constexpr bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it might be
         * propagated elsewhere, for example to another screen when using
         * @ref BasicScreenedApplication "ScreenedApplication". By default is
         * each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Input text in UTF-8 */
        constexpr Containers::ArrayView<const char> text() const { return _text; }

        /** @brief Location to begin editing from */
        constexpr Int start() const { return _start; }

        /** @brief Number of characters to edit from the start point */
        constexpr Int length() const { return _length; }

    private:
        constexpr TextEditingEvent(Containers::ArrayView<const char> text, Int start, Int length): _text{text}, _start{start}, _length{length}, _accepted{false} {}

        Containers::ArrayView<const char> _text;
        Int _start, _length;
        bool _accepted;
};

/** @hideinitializer
@brief Entry point for SDL2-based applications
@param className Class name

See @ref Magnum::Platform::Sdl2Application "Platform::Sdl2Application" for
usage information. This macro abstracts out platform-specific entry point code
and is equivalent to the following on all supported platforms except
@ref CORRADE_TARGET_WINDOWS_RT "Windows RT", see @ref portability-applications
for more information.
@code
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode
When no other application header is included this macro is also aliased to
`MAGNUM_APPLICATION_MAIN()`.
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

CORRADE_ENUMSET_OPERATORS(Sdl2Application::Flags)
#ifndef CORRADE_TARGET_EMSCRIPTEN
CORRADE_ENUMSET_OPERATORS(Sdl2Application::Configuration::Flags)
#endif
CORRADE_ENUMSET_OPERATORS(Sdl2Application::Configuration::WindowFlags)
CORRADE_ENUMSET_OPERATORS(Sdl2Application::InputEvent::Modifiers)
CORRADE_ENUMSET_OPERATORS(Sdl2Application::MouseMoveEvent::Buttons)

}}

#endif
