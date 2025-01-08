#ifndef Magnum_Platform_GlfwApplication_h
#define Magnum_Platform_GlfwApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016, 2018 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2019 Konstantinos Chatzilygeroudis <costashatz@gmail.com>
    Copyright © 2019, 2020 Marco Melorio <m.melorio@icloud.com>
    Copyright © 2022 Andreas Leroux <andreas.leroux@epitech.eu>

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
 * @brief Class @ref Magnum::Platform::GlfwApplication, macro @ref MAGNUM_GLFWAPPLICATION_MAIN()
 */

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/String.h> /** @todo PIMPL Configuration instead? */

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Platform/Platform.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/Platform/GLContext.h"
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#ifdef MAGNUM_BUILD_DEPRECATED
/* Some APIs used to take or return a std::string before */
#include <Corrade/Containers/StringStl.h>
#endif

namespace Magnum { namespace Platform {

namespace Implementation {
    enum class GlfwDpiScalingPolicy: UnsignedByte;
}

/** @nosubgrouping
@brief GLFW application

@m_keywords{Application}

Application using the [GLFW](http://glfw.org) toolkit. Supports keyboard and
mouse handling with support for changing cursor and mouse tracking and warping.
Available on all platforms where GLFW is ported.

@m_class{m-block m-success}

@thirdparty This library makes use of [GLFW](https://www.glfw.org/), released
    under the @m_class{m-label m-success} **zlib/libpng license**
    ([license text](https://www.glfw.org/license.html),
    [choosealicense.com](https://choosealicense.com/licenses/zlib/)).
    Attribution is appreciated but not required.

@section Platform-GlfwApplication-bootstrap Bootstrap application

Fully contained base application using @ref GlfwApplication along with CMake
setup is available in `base-glfw` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-glfw.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-glfw.zip) file.
After extracting the downloaded archive you can build and run the application
with these four commands:

@code{.sh}
mkdir build && cd build
cmake ..
cmake --build .
./src/MyApplication # or ./src/Debug/MyApplication
@endcode

See @ref cmake for more information.

@section Platform-GlfwApplication-usage General usage

This application library depends on [GLFW](http://glfw.org) 3.2 and newer and
is built if `MAGNUM_WITH_GLFWAPPLICATION` is enabled when building Magnum. To use
this library with CMake, request the `GlfwApplication` component of the
`Magnum` package and link to the `Magnum::GlfwApplication` target:

@code{.cmake}
find_package(Magnum REQUIRED GlfwApplication)

# ...
target_link_libraries(your-app PRIVATE Magnum::GlfwApplication)
@endcode

Additionally, if you're using Magnum as a CMake subproject, bundle the
[glfw repository](https://github.com/glfw/glfw) and do the following
* *before* calling @cmake find_package() @ce to ensure it's enabled, as the
library is not built by default. If you want to use system-installed GLFW, omit
the first part and point `CMAKE_PREFIX_PATH` to its installation dir if
necessary.

@code{.cmake}
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# These two are be off-by-default and thus no longer needed with GLFW 3.4+
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw EXCLUDE_FROM_ALL)

set(MAGNUM_WITH_GLFWAPPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

If no other application is requested, you can also use the generic
`Magnum::Application` alias to simplify porting. Again, see @ref building and
@ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass can be then used directly in @cpp main() @ce
--- see convenience macro @ref MAGNUM_GLFWAPPLICATION_MAIN(). See @ref platform
for more information.

@code{.cpp}
class MyApplication: public Platform::GlfwApplication {
    // implement required methods...
};
MAGNUM_GLFWAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
@cpp Platform::Application @ce and the macro is aliased to
@cpp MAGNUM_APPLICATION_MAIN() @ce to simplify porting.

@section Platform-GlfwApplication-dpi DPI awareness

DPI awareness behavior is consistent with @ref Sdl2Application except that iOS
or Emscripten specifics don't apply here. In addition, on Windows, GLFW is
implicitly advertising DPI awareness, so the manifest file described in
@ref platforms-windows-hidpi doesn't necessarily need to be supplied. See
@ref Platform-Sdl2Application-dpi "Sdl2Application DPI awareness documentation"
for more information.
*/
class GlfwApplication {
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
        class InputEvent;
        class KeyEvent;
        class PointerEvent;
        class PointerMoveEvent;
        class ScrollEvent;
        #ifdef MAGNUM_BUILD_DEPRECATED
        class MouseEvent;
        class MouseMoveEvent;
        class MouseScrollEvent;
        #endif
        class TextInputEvent;

        /* The damn thing cannot handle forward enum declarations */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        enum class Modifier: Int;
        enum class Key: Int;
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
        explicit GlfwApplication(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration);
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
         * @ref GlfwApplication(const Arguments&, const Configuration&, const GLConfiguration&)
         * with default-constructed @ref GLConfiguration.
         *
         * See also @ref building-features for more information.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit GlfwApplication(const Arguments& arguments, const Configuration& configuration = Configuration{});
        #else
        /* Configuration is only forward-declared at this point */
        explicit GlfwApplication(const Arguments& arguments, const Configuration& configuration);
        explicit GlfwApplication(const Arguments& arguments);
        #endif

        /**
         * @brief Construct without creating a window
         * @param arguments     Application arguments
         *
         * Unlike above, the window is not created and must be created later
         * with @ref create() or @ref tryCreate().
         */
        explicit GlfwApplication(const Arguments& arguments, NoCreateT);

        /** @brief Copying is not allowed */
        GlfwApplication(const GlfwApplication&) = delete;

        /** @brief Moving is not allowed */
        GlfwApplication(GlfwApplication&&) = delete;

        /** @brief Copying is not allowed */
        GlfwApplication& operator=(const GlfwApplication&) = delete;

        /** @brief Moving is not allowed */
        GlfwApplication& operator=(GlfwApplication&&) = delete;

        /**
         * @brief Name for given key
         * @m_since_latest
         *
         * Human-readable localized UTF-8 name for given @p key, intended for
         * displaying to the user in e.g. a key binding configuration. For
         * @ref Key::Unknown, a concrete @p scanCode can be passed. If there is
         * no name for given key, an empty string is returned. If non-empty,
         * the returned view is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * is valid until the keyboard layout is changed or the application
         * exits.
         *
         * Unlike e.g. @ref Sdl2Application::keyName(), the function isn't
         * @cpp static @ce because it relies on GLFW being initialized.
         * @see @ref KeyEvent::keyName(), @ref keyToScanCode()
         */
        Containers::StringView keyName(Key key, UnsignedInt scanCode = 0) const;

        #if GLFW_VERSION_MAJOR*100 + GLFW_VERSION_MINOR >= 303 || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Scan code for given key
         * @m_since_latest
         *
         * If @p key doesn't correspond to a physical key supported on given
         * platform, returns @relativeref{Corrade,Containers::NullOpt}. Unlike
         * e.g. @ref Sdl2Application::scanCodeToKey(), GLFW doesn't provide any
         * way to map from a scan code to a key.
         *
         * Unlike e.g. @ref Sdl2Application::keyToScanCode(), the function
         * isn't @cpp static @ce because it relies on GLFW being initialized.
         * @note Available since GLFW 3.3.
         * @see @ref KeyEvent::key(), @ref KeyEvent::scanCode(), @ref keyName()
         */
        Containers::Optional<UnsignedInt> keyToScanCode(Key key) const;
        #endif

        /**
         * @brief Execute main loop
         * @return Value for returning from @cpp main() @ce
         *
         * Calls @ref mainLoopIteration() in a loop until @ref exit() is
         * called. See @ref MAGNUM_GLFWAPPLICATION_MAIN() for usage
         * information.
         */
        int exec();

        /**
         * @brief Run one iteration of application main loop
         * @return @cpp false @ce if @ref exit() was called and the application
         *      should exit, @cpp true @ce otherwise
         * @m_since{2020,06}
         *
         * Called internally from @ref exec(). If you want to have better
         * control over how the main loop behaves, you can call this function
         * yourself from your own `main()` function instead of it being called
         * automatically from @ref exec() / @ref MAGNUM_GLFWAPPLICATION_MAIN().
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

        /**
         * @brief Underlying window handle
         *
         * Use in case you need to call GLFW functionality directly.
         */
        GLFWwindow* window() { return _window; }

    protected:
        /* Nobody will need to have (and delete) GlfwApplication*, thus this is
           faster than public pure virtual destructor */
        ~GlfwApplication();

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Create a window with given configuration for OpenGL context
         * @param configuration     Application configuration
         * @param glConfiguration   OpenGL context configuration
         *
         * Must be called only if the context wasn't created by the constructor
         * itself, i.e. when passing @ref NoCreate to it. Error message is
         * printed and the program exits if the context cannot be created, see
         * @ref tryCreate() for an alternative.
         *
         * On desktop GL, if version is not specified in @p glConfiguration,
         * the application first tries to create core context (OpenGL 3.2+) and
         * if that fails, falls back to compatibility OpenGL 2.1 context.
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
         * See @ref Platform-GlfwApplication-dpi for more information.
         * @see @ref dpiScaling()
         */
        Vector2i windowSize() const;

        /**
         * @brief Set window size
         * @param size    The size, in screen coordinates
         * @m_since{2020,06}
         *
         * To make the sizing work independently of the display DPI, @p size is
         * internally multiplied with @ref dpiScaling() before getting applied.
         * Expects that a window is already created.
         * @see @ref setMinWindowSize(), @ref setMaxWindowSize()
         */
        void setWindowSize(const Vector2i& size);

        /**
         * @brief Set window minimum size
         * @param size    The minimum size, in screen coordinates
         * @m_since{2019,10}
         *
         * If a value is set to @cpp -1 @ce, it will disable/remove the
         * corresponding limit. To make the sizing work independently of the
         * display DPI, @p size is internally multiplied with @ref dpiScaling()
         * before getting applied. Expects that a window is already created.
         * @see @ref setMaxWindowSize(), @ref setWindowSize()
         */
        void setMinWindowSize(const Vector2i& size = {-1, -1});

        /**
         * @brief Set window maximum size
         * @param size    The maximum size, in screen coordinates
         * @m_since{2019,10}
         *
         * If a value is set to @cpp -1 @ce, it will disable/remove the
         * corresponding limit. To make the sizing work independently of the
         * display DPI, @p size is internally multiplied with @ref dpiScaling()
         * before getting applied. Expects that a window is already created.
         * @see @ref setMinWindowSize(), @ref setMaxWindowSize()
         */
        void setMaxWindowSize(const Vector2i& size = {-1, -1});

        #if defined(MAGNUM_TARGET_GL) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Framebuffer size
         *
         * Size of the default framebuffer. Note that, especially on HiDPI
         * systems, it may be different from @ref windowSize(). Expects that a
         * window is already created. See @ref Platform-GlfwApplication-dpi for
         * more information.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         *
         * @see @ref dpiScaling()
         */
        Vector2i framebufferSize() const;
        #endif

        /**
         * @brief DPI scaling
         *
         * How the content should be scaled relative to system defaults for
         * given @ref windowSize(). If a window is not created yet, returns
         * zero vector, use @ref dpiScaling(const Configuration&) for
         * calculating a value independently. See @ref Platform-GlfwApplication-dpi
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
         * @ref Platform-GlfwApplication-dpi for more information.
         */
        Vector2 dpiScaling(const Configuration& configuration);

        /**
         * @brief Set window title
         * @m_since{2019,10}
         *
         * The @p title is expected to be encoded in UTF-8.
         */
        void setWindowTitle(Containers::StringView title);

        /**
         * @brief Set window icon
         * @m_since_latest
         *
         * The @p images are expected to be with origin at bottom left (which
         * is the default for imported images) and in one of
         * @ref PixelFormat::RGB8Unorm, @ref PixelFormat::RGB8Srgb,
         * @ref PixelFormat::RGBA8Unorm or @ref PixelFormat::RGBA8Srgb formats.
         * If you have just one image, you can use
         * @ref setWindowIcon(const ImageView2D&) instead.
         * @note The function has no effect on macOS / Wayland, see
         *      @m_class{m-doc-external} [glfwSetWindowIcon()](https://www.glfw.org/docs/latest/group__window.html#gadd7ccd39fe7a7d1f0904666ae5932dc5)
         *      for more information.
         * @see @ref platform-windows-icon "Excecutable icon on Windows",
         *      @ref Trade::IcoImporter "IcoImporter"
         */
        void setWindowIcon(Containers::ArrayView<const ImageView2D> images);

        /**
         * @overload
         * @m_since{2020,06}
         */
        void setWindowIcon(std::initializer_list<ImageView2D> images);

        /**
         * @overload
         * @m_since{2020,06}
         */
        void setWindowIcon(const ImageView2D& image);

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers() { glfwSwapBuffers(_window); }

        /**
         * @brief Set swap interval
         *
         * Set @cpp 0 @ce for no VSync, @cpp 1 @ce for enabled VSync. Some
         * platforms support @cpp -1 @ce for late swap tearing. Default is
         * driver-dependent.
         *
         * @note Unlike SDL2, GLFW doesn't provide any getter for the swap
         *      interval, so this class doesn't provide any equivalent to
         *      @ref Sdl2Application::swapInterval().
         */
        void setSwapInterval(Int interval);

        /**
         * @brief Set minimal loop period
         * @m_since_latest
         *
         * This setting reduces the main loop frequency in case
         * @ref setSwapInterval() wasn't called at all, was called with
         * @cpp 0 @ce, or no drawing is done and just @ref tickEvent() is being
         * executed. The @p time is expected to be non-negative, default is
         * @cpp 0_nsec @ce (i.e., looping at maximum frequency). If the
         * application is drawing on the screen and VSync was enabled by
         * calling @ref setSwapInterval(), this setting is ignored.
         *
         * Note that as the VSync default is driver-dependent,
         * @ref setSwapInterval() has to be explicitly called to make the
         * interaction between the two work correctly.
         * @see @ref setSwapInterval()
         */
        void setMinimalLoopPeriod(Nanoseconds time);

        /** @copydoc Sdl2Application::redraw() */
        void redraw();

    private:
        /**
         * @brief Viewport event
         *
         * Called when window size changes. The default implementation does
         * nothing. If you want to respond to size changes, you should pass the
         * new size to @ref GL::DefaultFramebuffer::setViewport() (if using
         * OpenGL) and possibly elsewhere (to
         * @ref SceneGraph::Camera::setViewport(), other framebuffers...).
         *
         * Note that this function might not get called at all if the window
         * size doesn't change. You should configure the initial state of your
         * cameras, framebuffers etc. in application constructor rather than
         * relying on this function to be called. Size of the window can be
         * retrieved using @ref windowSize(), size of the backing framebuffer
         * via @ref framebufferSize() and DPI scaling using @ref dpiScaling().
         * See @ref Platform-GlfwApplication-dpi for detailed info about these
         * values.
         * @see @ref platform-windowed-viewport-events
         */
        virtual void viewportEvent(ViewportEvent& event);

        /** @copydoc Sdl2Application::drawEvent() */
        virtual void drawEvent() = 0;

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Keyboard handling */

        /**
         * @brief Key press event
         *
         * Called when a key is pressed. Default implementation does nothing.
         * @see @ref platform-windowed-key-events
         */
        virtual void keyPressEvent(KeyEvent& event);

        /**
         * @brief Key release event
         *
         * Called when a key is released. Default implementation does nothing.
         * @see @ref platform-windowed-key-events
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
            Crosshair,      /**< Crosshair */

            /* Checking for GLFW_RESIZE_NWSE_CURSOR being defined instead of a
               version check because older Git clones have version set to 3.4
               but don't contain those defines. All new cursors were added in
               the same commit, so it's okay to test for just one define. */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || defined(GLFW_RESIZE_NWSE_CURSOR)
            /**
             * Double arrow pointing northwest and southeast
             * @note Available since GLFW 3.4.
             */
            ResizeNWSE,

            /**
             * Double arrow pointing northeast and southwest
             * @note Available since GLFW 3.4.
             */
            ResizeNESW,
            #endif

            ResizeWE,       /**< Double arrow pointing west and east */
            ResizeNS,       /**< Double arrow pointing north and south */

            #if defined(DOXYGEN_GENERATING_OUTPUT) || defined(GLFW_RESIZE_NWSE_CURSOR)
            /**
             * Four pointed arrow pointing north, south, east, and west
             * @note Available since GLFW 3.4.
             */
            ResizeAll,

            /**
             * Slashed circle or crossbones
             * @note Available since GLFW 3.4.
             */
            No,
            #endif

            Hand,           /**< Hand */
            Hidden,         /**< Hidden */
            HiddenLocked    /**< Hidden and locked */
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

        /**
         * @brief Warp mouse cursor to given coordinates
         * @m_since{2020,06}
         */
        void warpCursor(const Vector2i& position) {
            glfwSetCursorPos(_window, Double(position.x()), Double(position.y()));
        }

    private:
        /**
         * @brief Pointer press event
         * @m_since_latest
         *
         * Called when a mouse is pressed. Note that if at least one mouse
         * button is already pressed and another button gets pressed in
         * addition, @ref pointerMoveEvent() with the new combination is
         * called, not this function.
         *
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, default
         * implementation delegates to @ref mousePressEvent(). On builds with
         * deprecated functionality disabled, default implementation does
         * nothing.
         * @see @ref platform-windowed-pointer-events
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
         * Called when a mouse is released. Note that if multiple mouse buttons
         * are pressed and one of these is released, @ref pointerMoveEvent()
         * with the new combination is called, not this function.
         *
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, default
         * implementation delegates to @ref mouseReleaseEvent(). On builds with
         * deprecated functionality disabled, default implementation does
         * nothing.
         * @see @ref platform-windowed-pointer-events
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
         * On builds with @ref MAGNUM_BUILD_DEPRECATED enabled, default
         * implementation delegates to @ref mouseMoveEvent(), or if
         * @ref PointerMoveEvent::pointer() is not
         * @relativeref{Corrade,Containers::NullOpt}, to either
         * @ref mousePressEvent() or @ref mouseReleaseEvent(). On builds with
         * deprecated functionality disabled, default implementation does
         * nothing.
         * @see @ref platform-windowed-pointer-events
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
         * @ref textInputEvent().
         * @see @ref startTextInput(), @ref stopTextInput(),
         *      @ref platform-windowed-key-events
         */
        bool isTextInputActive() const;

        /**
         * @brief Start text input
         *
         * Starts text input that will go to @ref textInputEvent().
         * @see @ref stopTextInput(), @ref isTextInputActive(),
         *      @ref platform-windowed-key-events
         */
        void startTextInput();

        /**
         * @brief Stop text input
         *
         * Stops text input that went to @ref textInputEvent().
         * @see @ref startTextInput(), @ref isTextInputActive(),
         *      @ref textInputEvent(), @ref platform-windowed-key-events
         */
        void stopTextInput();

    private:
        /**
         * @brief Text input event
         *
         * Called when text input is active and the text is being input.
         * @see @ref isTextInputActive(), @ref platform-windowed-key-events
         */
        virtual void textInputEvent(TextInputEvent& event);

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
         */
        virtual void exitEvent(ExitEvent& event);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

    protected:
        /**
         * @brief Tick event
         * @m_since_latest
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
        #ifdef MAGNUM_BUILD_DEPRECATED
        /* Calls the base pointer*Event() in order to delegate to deprecated
           mouse events */
        template<class> friend class BasicScreenedApplication;
        template<class, bool> friend struct Implementation::ApplicationScrollEventMixin;
        #endif

        enum class Flag: UnsignedByte;
        typedef Containers::EnumSet<Flag> Flags;
        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        Vector2 dpiScalingInternal(Implementation::GlfwDpiScalingPolicy configurationDpiScalingPolicy, const Vector2& configurationDpiScaling) const;

        void setupCallbacks();

        /* Corresponds to size of the Cursor enum, the two Hidden cursors are
           handled differently */
        GLFWcursor* _cursors[
            #ifndef GLFW_RESIZE_NWSE_CURSOR
            6
            #else
            10
            #endif
        ]{};
        Cursor _cursor = Cursor::Arrow;

        /* These are saved from command-line arguments, and from configuration
           to be reused in dpiScaling() and viewportEvent() later */
        bool _verboseLog{};
        Implementation::GlfwDpiScalingPolicy _commandLineDpiScalingPolicy{}, _configurationDpiScalingPolicy{};
        Vector2 _commandLineDpiScaling, _configurationDpiScaling;

        GLFWwindow* _window{nullptr};
        /* Not using Nanoseconds as that would require including Time.h */
        UnsignedInt _minimalLoopPeriodNanoseconds{};
        Flags _flags;
        #ifdef MAGNUM_TARGET_GL
        /* Has to be in an Optional because we delay-create it in a constructor
           with populated Arguments and it gets explicitly destroyed before the
           GL context */
        Containers::Optional<Platform::GLContext> _context;
        #endif
        int _exitCode = 0;

        Vector2i _minWindowSize, _maxWindowSize;
        Vector2 _previousMouseMovePosition{Constants::nan()};
};

/**
@brief Keyboard modifier
@m_since_latest

@see @ref Modifiers, @ref KeyEvent::modifiers(),
    @ref PointerEvent::modifiers(), @ref PointerMoveEvent::modifiers(),
    @ref ScrollEvent::modifiers(), @ref platform-windowed-key-events
*/
enum class GlfwApplication::Modifier: Int {
    /**
     * Shift
     *
     * @see @ref KeyEvent::Key::LeftShift, @ref KeyEvent::Key::RightShift
     */
    Shift = GLFW_MOD_SHIFT,

    /**
     * Ctrl
     *
     * @see @ref KeyEvent::Key::LeftCtrl, @ref KeyEvent::Key::RightCtrl
     */
    Ctrl = GLFW_MOD_CONTROL,

    /**
     * Alt
     *
     * @see @ref KeyEvent::Key::LeftAlt, @ref KeyEvent::Key::RightAlt
     */
    Alt = GLFW_MOD_ALT,

    /**
     * Super key (Windows/⌘)
     *
     * @see @ref KeyEvent::Key::LeftSuper, @ref KeyEvent::Key::RightSuper
     */
    Super = GLFW_MOD_SUPER
};

CORRADE_ENUMSET_OPERATORS(GlfwApplication::Modifiers)

/**
@brief Key
@m_since_latest

@see @ref KeyEvent::key(), @ref platform-windowed-key-events
*/
enum class GlfwApplication::Key: Int {
    Unknown = GLFW_KEY_UNKNOWN,         /**< Unknown key */

    /**
     * Left Shift
     *
     * @see @ref InputEvent::Modifier::Shift
     */
    LeftShift = GLFW_KEY_LEFT_SHIFT,

    /**
     * Right Shift
     *
     * @see @ref InputEvent::Modifier::Shift
     */
    RightShift = GLFW_KEY_RIGHT_SHIFT,

    /**
     * Left Ctrl
     *
     * @see @ref InputEvent::Modifier::Ctrl
     */
    LeftCtrl = GLFW_KEY_LEFT_CONTROL,

    /**
     * Right Ctrl
     *
     * @see @ref InputEvent::Modifier::Ctrl
     */
    RightCtrl = GLFW_KEY_RIGHT_CONTROL,

    /**
     * Left Alt
     *
     * @see @ref InputEvent::Modifier::Alt
     */
    LeftAlt = GLFW_KEY_LEFT_ALT,

    /**
     * Right Alt
     *
     * @see @ref InputEvent::Modifier::Alt
     */
    RightAlt = GLFW_KEY_RIGHT_ALT,

    /**
     * Left Super key (Windows/⌘)
     *
     * @see @ref InputEvent::Modifier::Super
     */
    LeftSuper = GLFW_KEY_LEFT_SUPER,

    /**
     * Right Super key (Windows/⌘)
     *
     * @see @ref InputEvent::Modifier::Super
     */
    RightSuper = GLFW_KEY_RIGHT_SUPER,

    /* no equivalent for Sdl2Application's AltGr */

    Enter = GLFW_KEY_ENTER,             /**< Enter */
    Esc = GLFW_KEY_ESCAPE,              /**< Escape */

    Up = GLFW_KEY_UP,                   /**< Up arrow */
    Down = GLFW_KEY_DOWN,               /**< Down arrow */
    Left = GLFW_KEY_LEFT,               /**< Left arrow */
    Right = GLFW_KEY_RIGHT,             /**< Right arrow */
    Home = GLFW_KEY_HOME,               /**< Home */
    End = GLFW_KEY_END,                 /**< End */
    PageUp = GLFW_KEY_PAGE_UP,          /**< Page up */
    PageDown = GLFW_KEY_PAGE_DOWN,      /**< Page down */
    Backspace = GLFW_KEY_BACKSPACE,     /**< Backspace */
    Insert = GLFW_KEY_INSERT,           /**< Insert */
    Delete = GLFW_KEY_DELETE,           /**< Delete */

    F1 = GLFW_KEY_F1,                   /**< F1 */
    F2 = GLFW_KEY_F2,                   /**< F2 */
    F3 = GLFW_KEY_F3,                   /**< F3 */
    F4 = GLFW_KEY_F4,                   /**< F4 */
    F5 = GLFW_KEY_F5,                   /**< F5 */
    F6 = GLFW_KEY_F6,                   /**< F6 */
    F7 = GLFW_KEY_F7,                   /**< F7 */
    F8 = GLFW_KEY_F8,                   /**< F8 */
    F9 = GLFW_KEY_F9,                   /**< F9 */
    F10 = GLFW_KEY_F10,                 /**< F10 */
    F11 = GLFW_KEY_F11,                 /**< F11 */
    F12 = GLFW_KEY_F12,                 /**< F12 */

    Space = GLFW_KEY_SPACE,             /**< Space */
    Tab = GLFW_KEY_TAB,                 /**< Tab */

    /**
     * Quote (<tt>'</tt>)
     * @m_since{2020,06}
     */
    Quote = GLFW_KEY_APOSTROPHE,

    Comma = GLFW_KEY_COMMA,             /**< Comma */
    Period = GLFW_KEY_PERIOD,           /**< Period */
    Minus = GLFW_KEY_MINUS,             /**< Minus */

    /**
     * Plus. On the US keyboard layout this may only be representable as
     * @m_class{m-label m-warning} **Shift** @m_class{m-label m-default} <b>=</b>.
     */
    Plus = '+',

    Slash = GLFW_KEY_SLASH,             /**< Slash */

    /**
     * Percent. On the US keyboard layout this may only be representable as
     * @m_class{m-label m-warning} **Shift** @m_class{m-label m-default} **5**.
     */
    Percent = '%',

    Semicolon = GLFW_KEY_SEMICOLON,     /**< Semicolon (`;`) */
    Equal = GLFW_KEY_EQUAL,             /**< Equal */

    /**
     * Left bracket (`[`)
     * @m_since{2020,06}
     */
    LeftBracket = GLFW_KEY_LEFT_BRACKET,

    /**
     * Right bracket (`]`)
     * @m_since{2020,06}
     */
    RightBracket = GLFW_KEY_RIGHT_BRACKET,

    /**
     * Backslash (`\`)
     * @see @ref Key::World1, @ref Key::World2
     * @m_since{2020,06}
     */
    Backslash = GLFW_KEY_BACKSLASH,

    /**
     * Backquote (<tt>`</tt>)
     * @m_since{2020,06}
     */
    Backquote = GLFW_KEY_GRAVE_ACCENT,

    /**
     * Non-US \#1. Can be for example a backslash (`\`) next to left Shift.
     * @see @ref Key::Backslash
     * @m_since{2020,06}
     * @todo Revisit / rename together with World2 once
     *      https://github.com/glfw/glfw/issues/2481 is resolved. SDL
     *      scancode for this key is SDL_SCANCODE_NONUSBACKSLASH, HTML5
     *      names it IntlBackslash.
     */
    World1 = GLFW_KEY_WORLD_1,

    /**
     * Non-US \#2
     * @see @ref Key::Backslash
     * @m_since{2020,06}
     */
    World2 = GLFW_KEY_WORLD_2,

    Zero = GLFW_KEY_0,                  /**< Zero */
    One = GLFW_KEY_1,                   /**< One */
    Two = GLFW_KEY_2,                   /**< Two */
    Three = GLFW_KEY_3,                 /**< Three */
    Four = GLFW_KEY_4,                  /**< Four */
    Five = GLFW_KEY_5,                  /**< Five */
    Six = GLFW_KEY_6,                   /**< Six */
    Seven = GLFW_KEY_7,                 /**< Seven */
    Eight = GLFW_KEY_8,                 /**< Eight */
    Nine = GLFW_KEY_9,                  /**< Nine */

    A = GLFW_KEY_A,                     /**< Letter A */
    B = GLFW_KEY_B,                     /**< Letter B */
    C = GLFW_KEY_C,                     /**< Letter C */
    D = GLFW_KEY_D,                     /**< Letter D */
    E = GLFW_KEY_E,                     /**< Letter E */
    F = GLFW_KEY_F,                     /**< Letter F */
    G = GLFW_KEY_G,                     /**< Letter G */
    H = GLFW_KEY_H,                     /**< Letter H */
    I = GLFW_KEY_I,                     /**< Letter I */
    J = GLFW_KEY_J,                     /**< Letter J */
    K = GLFW_KEY_K,                     /**< Letter K */
    L = GLFW_KEY_L,                     /**< Letter L */
    M = GLFW_KEY_M,                     /**< Letter M */
    N = GLFW_KEY_N,                     /**< Letter N */
    O = GLFW_KEY_O,                     /**< Letter O */
    P = GLFW_KEY_P,                     /**< Letter P */
    Q = GLFW_KEY_Q,                     /**< Letter Q */
    R = GLFW_KEY_R,                     /**< Letter R */
    S = GLFW_KEY_S,                     /**< Letter S */
    T = GLFW_KEY_T,                     /**< Letter T */
    U = GLFW_KEY_U,                     /**< Letter U */
    V = GLFW_KEY_V,                     /**< Letter V */
    W = GLFW_KEY_W,                     /**< Letter W */
    X = GLFW_KEY_X,                     /**< Letter X */
    Y = GLFW_KEY_Y,                     /**< Letter Y */
    Z = GLFW_KEY_Z,                     /**< Letter Z */

    CapsLock = GLFW_KEY_CAPS_LOCK,      /**< Caps lock */
    ScrollLock = GLFW_KEY_SCROLL_LOCK,  /**< Scroll lock */
    NumLock = GLFW_KEY_NUM_LOCK,        /**< Num lock */
    PrintScreen = GLFW_KEY_PRINT_SCREEN,/**< Print screen */
    Pause = GLFW_KEY_PAUSE,             /**< Pause */
    Menu = GLFW_KEY_MENU,               /**< Menu */

    NumZero = GLFW_KEY_KP_0,            /**< Numpad zero */
    NumOne = GLFW_KEY_KP_1,             /**< Numpad one */
    NumTwo = GLFW_KEY_KP_2,             /**< Numpad two */
    NumThree = GLFW_KEY_KP_3,           /**< Numpad three */
    NumFour = GLFW_KEY_KP_4,            /**< Numpad four */
    NumFive = GLFW_KEY_KP_5,            /**< Numpad five */
    NumSix = GLFW_KEY_KP_6,             /**< Numpad six */
    NumSeven = GLFW_KEY_KP_7,           /**< Numpad seven */
    NumEight = GLFW_KEY_KP_8,           /**< Numpad eight */
    NumNine = GLFW_KEY_KP_9,            /**< Numpad nine */
    NumDecimal = GLFW_KEY_KP_DECIMAL,   /**< Numpad decimal */
    NumDivide = GLFW_KEY_KP_DIVIDE,     /**< Numpad divide */
    NumMultiply = GLFW_KEY_KP_MULTIPLY, /**< Numpad multiply */
    NumSubtract = GLFW_KEY_KP_SUBTRACT, /**< Numpad subtract */
    NumAdd = GLFW_KEY_KP_ADD,           /**< Numpad add */
    NumEnter = GLFW_KEY_KP_ENTER,       /**< Numpad enter */
    NumEqual = GLFW_KEY_KP_EQUAL        /**< Numpad equal */
};

/**
@brief Pointer event source
@m_since_latest

@see @ref PointerEvent::source(), @ref PointerMoveEvent::source(),
    @ref platform-windowed-pointer-events
*/
enum class GlfwApplication::PointerEventSource: UnsignedByte {
    /**
     * The event is coming from a mouse
     * @see @ref Pointer::MouseLeft, @ref Pointer::MouseMiddle,
     *      @ref Pointer::MouseRight, @ref Pointer::MouseButton4,
     *      @ref Pointer::MouseButton5, @ref Pointer::MouseButton6,
     *      @ref Pointer::MouseButton7, @ref Pointer::MouseButton8
     */
    Mouse
};

/**
@brief Pointer type
@m_since_latest

@see @ref Pointers, @ref PointerEvent::pointer(),
    @ref PointerMoveEvent::pointer(), @ref PointerMoveEvent::pointers(),
    @ref platform-windowed-pointer-events
*/
enum class GlfwApplication::Pointer: UnsignedByte {
    /**
     * Left mouse button. Corresponds to `GLFW_MOUSE_BUTTON_LEFT` or
     * `GLFW_MOUSE_BUTTON_1`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseLeft = 1 << 0,

    /**
     * Middle mouse button. Corresponds to `GLFW_MOUSE_BUTTON_MIDDLE` or
     * `GLFW_MOUSE_BUTTON_2`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseMiddle = 1 << 1,

    /**
     * Right mouse button. Corresponds to `GLFW_MOUSE_BUTTON_RIGHT` or
     * `GLFW_MOUSE_BUTTON_3`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseRight = 1 << 2,

    /**
     * Fourth mouse button, such as wheel left. Corresponds to
     * `GLFW_MOUSE_BUTTON_4`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseButton4 = 1 << 3,

    /**
     * Fifth mouse button, such as wheel right. Corresponds to
     * `GLFW_MOUSE_BUTTON_5`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseButton5 = 1 << 4,

    /**
     * Sixth mouse button. Corresponds to `GLFW_MOUSE_BUTTON_6`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseButton6 = 1 << 5,

    /**
     * Seventh mouse button. Corresponds to `GLFW_MOUSE_BUTTON_7`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseButton7 = 1 << 6,

    /**
     * Eighth mouse button. Corresponds to `GLFW_MOUSE_BUTTON_8`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseButton8 = 1 << 7

    /** @todo look into touch / pen input once anything from
        https://github.com/glfw/glfw/issues/42
        https://github.com/glfw/glfw/issues/403
        https://github.com/glfw/glfw/pull/1445
        https://github.com/glfw/glfw/pull/1736 gets any updates */
};

CORRADE_ENUMSET_OPERATORS(GlfwApplication::Pointers)

#ifdef MAGNUM_TARGET_GL
/**
@brief OpenGL context configuration

The created window is always with a double-buffered OpenGL context.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref GlfwApplication(), @ref create(), @ref tryCreate()
*/
class GlfwApplication::GLConfiguration: public GL::Context::Configuration {
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
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Forward compatible context
             *
             * @requires_gl Core/compatibility profile distinction and forward
             *      compatibility applies only to desktop GL.
             */
            ForwardCompatible = 1 << 0,
            #endif

            /**
             * Context without error reporting. Might result in better
             * performance, but situations that would have generated errors
             * instead cause undefined behavior. Enabled automatically if
             * supported by the driver and the @ref Flag::GpuValidationNoError
             * flag is set or if the `--magnum-gpu-validation` @ref GL-Context-usage-command-line "command-line option"
             * is set to `no-error`.
             */
            NoError = 1 << 1,

            /**
             * Debug context. Enabled automatically if supported by the driver
             * and the @ref Flag::GpuValidation flag is set or if the
             * `--magnum-gpu-validation` @ref GL-Context-usage-command-line "command-line option"
             * is set to `on`.
             */
            Debug = 1 << 2,

            Stereo = 1 << 3,    /**< Stereo rendering */

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

        /** @brief Context version */
        GL::Version version() const { return _version; }

        /**
         * @brief Set context version
         *
         * If requesting version greater or equal to OpenGL 3.2, core profile
         * is used. The created context will then have any version which is
         * backwards-compatible with requested one. Default is
         * @ref GL::Version::None, i.e. any provided version is used.
         */
        GLConfiguration& setVersion(GL::Version version) {
            _version = version;
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
         * Default is @cpp 0 @ce, thus no multisampling. The actual sample
         * count is ignored, GLFW either enables it or disables. See also
         * @ref GL::Renderer::Feature::Multisampling.
         */
        GLConfiguration& setSampleCount(Int count) {
            _sampleCount = count;
            return *this;
        }

        /** @brief sRGB-capable default framebuffer */
        bool isSrgbCapable() const { return _srgbCapable; }

        /**
         * @brief Set sRGB-capable default framebuffer
         *
         * Default is @cpp false @ce. See also
         * @ref GL::Renderer::Feature::FramebufferSrgb.
         * @return Reference to self (for method chaining)
         */
        GLConfiguration& setSrgbCapable(bool enabled) {
            _srgbCapable = enabled;
            return *this;
        }

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        MAGNUM_GL_CONTEXT_CONFIGURATION_SUBCLASS_IMPLEMENTATION(GLConfiguration)
        #endif

    private:
        Vector4i _colorBufferSize;
        Int _depthBufferSize, _stencilBufferSize;
        Int _sampleCount;
        GL::Version _version;
        bool _srgbCapable;
};

CORRADE_ENUMSET_OPERATORS(GlfwApplication::GLConfiguration::Flags)
#endif

namespace Implementation {
    enum class GlfwDpiScalingPolicy: UnsignedByte {
        /* Using 0 for an "unset" value */

        #ifdef CORRADE_TARGET_APPLE
        Framebuffer = 1,
        #endif

        #ifndef CORRADE_TARGET_APPLE
        Virtual = 2,

        Physical = 3,
        #endif

        Default
            #ifdef CORRADE_TARGET_APPLE
            = Framebuffer
            #else
            = Virtual
            #endif
    };
}

/**
@brief Configuration

@see @ref GlfwApplication(), @ref create(), @ref tryCreate()
*/
class GlfwApplication::Configuration {
    public:
        /**
         * @brief Window flag
         *
         * @see @ref WindowFlags, @ref setWindowFlags()
         */
        enum class WindowFlag: UnsignedShort {
            Fullscreen = 1 << 0,   /**< Fullscreen window */

            /**
             * No window decoration
             * @m_since{2020,06}
             */
            Borderless = 1 << 1,

            Resizable = 1 << 2,    /**< Resizable window */
            Hidden = 1 << 3,       /**< Hidden window */
            Maximized = 1 << 4,    /**< Maximized window */

            Minimized = 1 << 5,    /**< Minimized window */

            /**
             * Always on top
             * @m_since{2020,06}
             */
            AlwaysOnTop = 1 << 6,

            #ifdef MAGNUM_BUILD_DEPRECATED
            /**
             * Always on top
             * @m_deprecated_since{2020,06} Use @ref WindowFlag::AlwaysOnTop instead.
             */
            Floating CORRADE_DEPRECATED_ENUM("use AlwaysOnTop instead") = AlwaysOnTop,
            #endif

            /**
             * Automatically iconify (minimize) if fullscreen window loses
             * input focus
             */
            AutoIconify = 1 << 7,

            /**
             * Window has input focus
             *
             * @todo there's also GLFW_FOCUS_ON_SHOW, what's the difference?
             */
            Focused = 1 << 8,

            /**
             * Do not create any GPU context. Use together with
             * @ref GlfwApplication(const Arguments&, const Configuration&),
             * @ref create(const Configuration&) or
             * @ref tryCreate(const Configuration&) to prevent implicit
             * creation of an OpenGL context. Can't be used with
             * @ref GlfwApplication(const Arguments&, const Configuration&, const GLConfiguration&),
             * @ref create(const Configuration&, const GLConfiguration&) or
             * @ref tryCreate(const Configuration&, const GLConfiguration&).
             */
            Contextless = 1 << 9
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
        typedef Implementation::GlfwDpiScalingPolicy DpiScalingPolicy;
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Cursor mode
         *
         * @m_deprecated_since{2020,06} Use @ref GlfwApplication::setCursor()
         *      instead.
         */
        enum class CORRADE_DEPRECATED_ENUM("use GlfwApplication::setCursor() instead") CursorMode: Int {
            /**
             * Visible unconstrained cursor
             *
             * @m_deprecated_since{2020,06} Use @ref GlfwApplication::setCursor()
             *      with @ref Cursor::Arrow (or any other) instead.
             */
            Normal CORRADE_DEPRECATED_ENUM("use GlfwApplication::setCursor() with Cursor::Arrow instead") = GLFW_CURSOR_NORMAL,

            /**
             * Hidden cursor
             *
             * @m_deprecated_since{2020,06} Use @ref GlfwApplication::setCursor()
             *      with @ref Cursor::Hidden instead.
             */
            Hidden CORRADE_DEPRECATED_ENUM("use GlfwApplication::setCursor() with Cursor::Hidden instead") = GLFW_CURSOR_HIDDEN,

            /**
             * Cursor hidden and locked window
             *
             * @m_deprecated_since{2020,06} Use @ref GlfwApplication::setCursor()
             *      with @ref Cursor::HiddenLocked instead.
             */
            Disabled CORRADE_DEPRECATED_ENUM("use GlfwApplication::setCursor() with Cursor::HiddenLocked instead") = GLFW_CURSOR_DISABLED
        };
        #endif

        /*implicit*/ Configuration();
        ~Configuration();

        /**
         * @brief Window title
         *
         * The returned view is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * is valid until the next call to @ref setTitle().
         */
        Containers::StringView title() const { return _title; }

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp "Magnum GLFW Application" @ce.
         */
        Configuration& setTitle(Containers::StringView title) {
            _title = Containers::String::nullTerminatedGlobalView(title);
            return *this;
        }

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
         * @todo change this on a DPI change event (GLFW 3.3 has a callback:
         *  https://github.com/mosra/magnum/issues/243#issuecomment-388384089)
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

        /**
         * @brief Set window size
         * @param size              Desired window size
         * @param dpiScalingPolicy  Policy based on which DPI scaling will be set
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp {800, 600} @ce. See @ref Platform-GlfwApplication-dpi
         * for more information.
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
         * @ref GlfwApplication::windowSize() is @cpp size*dpiScaling @ce and
         * @ref GlfwApplication::dpiScaling() is @p dpiScaling.
         */
        Configuration& setSize(const Vector2i& size, const Vector2& dpiScaling) {
            _size = size;
            _dpiScaling = dpiScaling;
            return *this;
        }

        /** @brief Window flags */
        WindowFlags windowFlags() const {
            return _windowFlags;
        }

        /**
         * @brief Set window flags
         * @return  Reference to self (for method chaining)
         *
         * Default is @ref WindowFlag::Focused. To avoid clearing default flags
         * by accident, prefer to use @ref addWindowFlags() and
         * @ref clearWindowFlags() instead.
         */
        Configuration& setWindowFlags(WindowFlags windowFlags) {
            _windowFlags = windowFlags;
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

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Cursor mode
         *
         * @m_deprecated_since{2020,06} Use @ref GlfwApplication::cursor()
         *      instead.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH
        CORRADE_DEPRECATED("use GlfwApplication::cursor() instead") CursorMode cursorMode() const {
            return _cursorMode;
        }
        CORRADE_IGNORE_DEPRECATED_POP

        /**
         * @brief Set cursor mode
         * @return  Reference to self (for method chaining)
         *
         * Default is @ref CursorMode::Normal.
         *
         * @m_deprecated_since{2020,06} Use @ref GlfwApplication::setCursor()
         *      instead.
         */
        CORRADE_IGNORE_DEPRECATED_PUSH
        CORRADE_DEPRECATED("use GlfwApplication::setCursor() instead") Configuration& setCursorMode(CursorMode cursorMode) {
            _cursorMode = cursorMode;
            return *this;
        }
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

    private:
        Containers::String _title;
        Vector2i _size;
        WindowFlags _windowFlags;
        DpiScalingPolicy _dpiScalingPolicy;
        Vector2 _dpiScaling;
        #ifdef MAGNUM_BUILD_DEPRECATED
        CORRADE_IGNORE_DEPRECATED_PUSH
        CursorMode _cursorMode = CursorMode::Normal;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
};

CORRADE_ENUMSET_OPERATORS(GlfwApplication::Configuration::WindowFlags)

/**
@brief Exit event

@see @ref exitEvent()
*/
class GlfwApplication::ExitEvent {
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

    private:
        friend GlfwApplication;

        explicit ExitEvent(): _accepted(false) {}

        bool _accepted;
};

/**
@brief Viewport event

@see @ref viewportEvent(), @ref platform-windowed-viewport-events
*/
class GlfwApplication::ViewportEvent {
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
         * from @ref framebufferSize(). See @ref Platform-GlfwApplication-dpi
         * for more information.
         * @see @ref GlfwApplication::windowSize()
         */
        Vector2i windowSize() const { return _windowSize; }

        #if defined(MAGNUM_TARGET_GL) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Framebuffer size
         *
         * On some platforms with HiDPI displays, framebuffer size can be
         * different from @ref windowSize(). See
         * @ref Platform-GlfwApplication-dpi for more information.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         *
         * @see @ref GlfwApplication::framebufferSize(), @ref dpiScaling()
         */
        Vector2i framebufferSize() const { return _framebufferSize; }
        #endif

        /**
         * @brief DPI scaling
         *
         * On some platforms moving an app between displays can result in DPI
         * scaling value being changed in tandem with a window/framebuffer
         * size. Simply resizing a window doesn't change the DPI scaling value.
         * See @ref Platform-GlfwApplication-dpi for more information.
         * @see @ref GlfwApplication::dpiScaling(), @ref framebufferSize()
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

    private:
        friend GlfwApplication;

        explicit ViewportEvent(const Vector2i& windowSize,
            #ifdef MAGNUM_TARGET_GL
            const Vector2i& framebufferSize,
            #endif
            const Vector2& dpiScaling): _windowSize{windowSize},
                #ifdef MAGNUM_TARGET_GL
                _framebufferSize{framebufferSize},
                #endif
                _dpiScaling{dpiScaling} {}

        const Vector2i _windowSize;
        #ifdef MAGNUM_TARGET_GL
        const Vector2i _framebufferSize;
        #endif
        const Vector2 _dpiScaling;
};

/**
@brief Base for input events

@see @ref KeyEvent, @ref PointerEvent, @ref PointerMoveEvent, @ref ScrollEvent,
    @ref keyPressEvent(), @ref keyReleaseEvent(), @ref pointerPressEvent(),
    @ref pointerReleaseEvent(), @ref pointerMoveEvent(), @ref scrollEvent()
*/
class GlfwApplication::InputEvent {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief GlfwApplication::Modifier
         * @m_deprecated_since_latest Use @ref GlfwApplication::Modifier
         *      instead.
         */
        typedef CORRADE_DEPRECATED("use GlfwApplication::Modifier instead") GlfwApplication::Modifier Modifier;

        /**
         * @brief @copybrief GlfwApplication::Modifiers
         * @m_deprecated_since_latest Use @ref GlfwApplication::Modifiers
         *      instead.
         */
        typedef CORRADE_DEPRECATED("use GlfwApplication::Modifiers instead") GlfwApplication::Modifiers Modifiers;
        #endif

        /** @brief Copying is not allowed */
        InputEvent(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent(InputEvent&&) = delete;

        /** @brief Copying is not allowed */
        InputEvent& operator=(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent& operator=(InputEvent&&) = delete;

        /** @copydoc Sdl2Application::InputEvent::setAccepted() */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @copydoc Sdl2Application::InputEvent::isAccepted() */
        bool isAccepted() const { return _accepted; }

    protected:
        explicit InputEvent(): _accepted(false) {}

        ~InputEvent() = default;

    private:
        bool _accepted;
};

/**
@brief Key event

@see @ref keyPressEvent(), @ref keyReleaseEvent(),
    @ref platform-windowed-key-events
*/
class GlfwApplication::KeyEvent: public GlfwApplication::InputEvent {
    public:
        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief GlfwApplication::Key
         * @m_deprecated_since_latest Use @ref GlfwApplication::Key instead.
         */
        typedef CORRADE_DEPRECATED("use GlfwApplication::Key instead") GlfwApplication::Key Key;

        /**
         * @brief @copybrief GlfwApplication::keyName()
         * @m_deprecated_since_latest Use @ref GlfwApplication::keyName()
         *      instead.
         */
        CORRADE_DEPRECATED("use GlfwApplication::keyName() instead") static Containers::StringView keyName(GlfwApplication::Key key);
        #endif

        /**
         * @brief Key
         *
         * Layout-dependent name of given key. Use @ref scanCode() to get a
         * platform-specific but layout-independent identifier of given key.
         * @see @ref keyName() const
         */
        GlfwApplication::Key key() const { return _key; }

        /**
         * @brief Scancode
         * @m_since_latest
         *
         * Platform-specific but layout-independent identifier of given key.
         * Use @ref key() to get a key name in the currently used layout.
         */
        UnsignedInt scanCode() const { return _scancode; }

        /**
         * @brief Key name
         *
         * Human-readable localized UTF-8 name for the key returned by
         * @ref key() and @ref scanCode(), intended for displaying to the user
         * in e.g. key binding configuration. If there is no name for that key,
         * an empty string is returned. If non-empty, the returned view is
         * always @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * and is valid until the keyboard layout is changed or the application
         * exits.
         * @see @ref keyName(Key)
         */
        Containers::StringView keyName() const;

        /** @brief Keyboard modifiers */
        GlfwApplication::Modifiers modifiers() const { return _modifiers; }

        /** @copydoc Sdl2Application::KeyEvent::isRepeated() */
        bool isRepeated() const { return _repeated; }

    private:
        friend GlfwApplication;

        explicit KeyEvent(GlfwApplication::Key key, UnsignedInt scancode, GlfwApplication::Modifiers modifiers, bool repeated): _repeated{repeated}, _modifiers{modifiers}, _key{key}, _scancode{scancode} {}

        const bool _repeated;
        const GlfwApplication::Modifiers _modifiers;
        const GlfwApplication::Key _key;
        const UnsignedInt _scancode;
};

/**
@brief Pointer event
@m_since_latest

@see @ref PointerMoveEvent, @ref ScrollEvent, @ref pointerPressEvent(),
    @ref pointerReleaseEvent(), @ref platform-windowed-pointer-events
*/
class GlfwApplication::PointerEvent: public InputEvent {
    public:
        /** @brief Copying is not allowed */
        PointerEvent(const PointerEvent&) = delete;

        /** @brief Moving is not allowed */
        PointerEvent(PointerEvent&&) = delete;

        /** @brief Copying is not allowed */
        PointerEvent& operator=(const PointerEvent&) = delete;

        /** @brief Moving is not allowed */
        PointerEvent& operator=(PointerEvent&&) = delete;

        /**
         * @brief Pointer event source
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @ref PointerEventSource::Mouse.
         */
        PointerEventSource source() const { return PointerEventSource::Mouse; }

        /** @brief Pointer type that was pressed or released */
        Pointer pointer() const { return _pointer; }

        /**
         * @brief Whether the pointer is primary
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @cpp true @ce.
         */
        bool isPrimary() const { return true; }

        /**
         * @brief Pointer ID
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @cpp 0 @ce.
         */
        Long id() const { return 0; }

        /**
         * @brief Position
         *
         * May return fractional values for example on HiDPI systems where
         * window size is smaller than actual framebuffer size. Use
         * @ref Math::round() to snap them to the nearest window pixel.
         */
        Vector2 position() const { return _position; }

        /** @brief Keyboard modifiers */
        GlfwApplication::Modifiers modifiers() const { return _modifiers; }

    private:
        friend GlfwApplication;

        explicit PointerEvent(Pointer pointer, const Vector2& position, GlfwApplication::Modifiers modifiers): _pointer(pointer), _position{position}, _modifiers{modifiers} {}

        const Pointer _pointer;
        const Vector2 _position;
        const GlfwApplication::Modifiers _modifiers;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Mouse event
@m_deprecated_since_latest Use @ref PointerEvent, @ref pointerPressEvent() and
    @ref pointerReleaseEvent() instead, which is a better abstraction for
    covering both mouse and touch input.

@see @ref MouseMoveEvent, @ref MouseScrollEvent, @ref mousePressEvent(),
    @ref mouseReleaseEvent()
*/
class CORRADE_DEPRECATED("use PointerEvent, pointerPressEvent() and pointerReleaseEvent() instead") GlfwApplication::MouseEvent: public InputEvent {
    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: int {
            Left = GLFW_MOUSE_BUTTON_LEFT,        /**< Left button */
            Middle = GLFW_MOUSE_BUTTON_MIDDLE,    /**< Middle button */
            Right = GLFW_MOUSE_BUTTON_RIGHT,      /**< Right button */
            Button1 = GLFW_MOUSE_BUTTON_1,        /**< Mouse button 1 */
            Button2 = GLFW_MOUSE_BUTTON_2,        /**< Mouse button 2 */
            Button3 = GLFW_MOUSE_BUTTON_3,        /**< Mouse button 3 */
            Button4 = GLFW_MOUSE_BUTTON_4,        /**< Mouse button 4 */
            Button5 = GLFW_MOUSE_BUTTON_5,        /**< Mouse button 5 */
            Button6 = GLFW_MOUSE_BUTTON_6,        /**< Mouse button 6 */
            Button7 = GLFW_MOUSE_BUTTON_7,        /**< Mouse button 7 */
            Button8 = GLFW_MOUSE_BUTTON_8,        /**< Mouse button 8 */
        };

        /** @brief Button */
        Button button() const { return _button; }

        /** @brief Position */
        Vector2i position() const { return _position; }

        /** @brief Keyboard modifiers */
        GlfwApplication::Modifiers modifiers() const { return _modifiers; }

    private:
        friend GlfwApplication;

        explicit MouseEvent(Button button, const Vector2i& position, GlfwApplication::Modifiers modifiers): _button{button}, _position{position}, _modifiers{modifiers} {}

        const Button _button;
        const Vector2i _position;
        const GlfwApplication::Modifiers _modifiers;
};
#endif

/**
@brief Pointer move event
@m_since_latest

@see @ref PointerEvent, @ref ScrollEvent, @ref pointerMoveEvent(),
    @ref platform-windowed-pointer-events
*/
class GlfwApplication::PointerMoveEvent: public InputEvent {
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
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @ref PointerEventSource::Mouse.
         */
        PointerEventSource source() const { return PointerEventSource::Mouse; }

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
         * example when a mouse is just moved around. Lazily populated on first
         * request.
         * @see @ref pointer()
         */
        Pointers pointers();

        /**
         * @brief Whether the pointer is primary
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @cpp true @ce.
         */
        bool isPrimary() const { return true; }

        /**
         * @brief Pointer ID
         *
         * Included mainly for compatibility with touch-aware application
         * implementations such as @ref Sdl2Application, returns always
         * @cpp 0 @ce.
         */
        Long id() const { return 0; }

        /**
         * @brief Position
         *
         * May return fractional values for example on HiDPI systems where
         * window size is smaller than actual framebuffer size. Use
         * @ref Math::round() to snap them to the nearest window pixel.
         */
        Vector2 position() const { return _position; }

        /**
         * @brief Position relative to the previous touch event
         *
         * May return fractional values for example on HiDPI systems where
         * window size is smaller than actual framebuffer size. Use
         * @ref Math::round() to snap them to the nearest window pixel. Unlike
         * @ref Sdl2Application, GLFW doesn't provide relative position
         * directly, so this is calculated explicitly as a delta from previous
         * move event position.
         */
        Vector2 relativePosition() const { return _relativePosition; }

        /**
         * @brief Keyboard modifiers
         *
         * Lazily populated on first request.
         */
        GlfwApplication::Modifiers modifiers();

    private:
        friend GlfwApplication;

        explicit PointerMoveEvent(GLFWwindow* window, Containers::Optional<Pointer> pointer, const Vector2& position, const Vector2& relativePosition): _window{window}, _pointer{pointer}, _position{position}, _relativePosition{relativePosition} {}

        GLFWwindow* const _window;
        const Containers::Optional<Pointer> _pointer;
        Containers::Optional<Pointers> _pointers;
        const Vector2 _position, _relativePosition;
        Containers::Optional<GlfwApplication::Modifiers> _modifiers;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Mouse move event
@m_deprecated_since_latest Use @ref PointerMoveEvent and
    @ref pointerMoveEvent() instead, which is a better abstraction for covering
    both mouse and touch / pen input.

@see @ref MouseEvent, @ref MouseScrollEvent, @ref mouseMoveEvent()
*/
class CORRADE_DEPRECATED("use PointerMoveEvent and pointerMoveEvent() instead") GlfwApplication::MouseMoveEvent: public GlfwApplication::InputEvent {
    public:
        /**
         * @brief Mouse button
         *
         * @see @ref Buttons, @ref buttons()
         */
        enum class Button: UnsignedInt {
            Left = 1 << GLFW_MOUSE_BUTTON_LEFT,     /**< Left button */
            Middle = 1 << GLFW_MOUSE_BUTTON_MIDDLE, /**< Middle button */
            Right = 1 << GLFW_MOUSE_BUTTON_RIGHT    /**< Right button */
        };

        /**
         * @brief Set of mouse buttons
         *
         * @see @ref buttons()
         */
        typedef Containers::EnumSet<Button> Buttons;

        /**
         * @brief Mouse buttons
         *
         * Lazily populated on first request.
         */
        Buttons buttons();

        /** @brief Position */
        Vector2i position() const { return _position; }

        /**
         * @brief Relative position
         * @m_since{2019,10}
         *
         * Position relative to previous move event. Unlike
         * @ref Sdl2Application, GLFW doesn't provide relative position
         * directly, so this is calculated explicitly as a delta from previous
         * move event position.
         */
        Vector2i relativePosition() const { return _relativePosition; }

        /**
         * @brief Keyboard modifiers
         *
         * Lazily populated on first request.
         */
        GlfwApplication::Modifiers modifiers();

    private:
        friend GlfwApplication;

        explicit MouseMoveEvent(GLFWwindow* window, const Vector2i& position, const Vector2i& relativePosition): _window{window}, _position{position}, _relativePosition{relativePosition} {}

        GLFWwindow* const _window;
        const Vector2i _position, _relativePosition;
        Containers::Optional<Buttons> _buttons;
        Containers::Optional<GlfwApplication::Modifiers> _modifiers;
};

CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_ENUMSET_OPERATORS(GlfwApplication::MouseMoveEvent::Buttons)
CORRADE_IGNORE_DEPRECATED_POP
#endif

/**
@brief Scroll event
@m_since_latest

@see @ref PointerEvent, @ref PointerMoveEvent, @ref scrollEvent(),
    @ref platform-windowed-pointer-events
*/
class GlfwApplication::ScrollEvent: public InputEvent {
    public:
        /** @brief Scroll offset */
        Vector2 offset() const { return _offset; }

        /**
         * @brief Position
         *
         * May return fractional values for example on HiDPI systems where
         * window size is smaller than actual framebuffer size. Use
         * @ref Math::round() to snap them to the nearest window pixel. Lazily
         * populated on first request.
         */
        Vector2 position();

        /**
         * @brief Keyboard modifiers
         *
         * Lazily populated on first request.
         */
        GlfwApplication::Modifiers modifiers();

    private:
        friend GlfwApplication;

        explicit ScrollEvent(GLFWwindow* window, const Vector2& offset): _window{window}, _offset{offset} {}

        GLFWwindow* const _window;
        const Vector2 _offset;
        Containers::Optional<Vector2> _position;
        Containers::Optional<GlfwApplication::Modifiers> _modifiers;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Mouse scroll event
@m_deprecated_since_latest Use @ref ScrollEvent and @ref scrollEvent() instead,
    which isn't semantically tied to just a mouse.

@see @ref MouseEvent, @ref MouseMoveEvent, @ref mouseScrollEvent()
*/
class CORRADE_DEPRECATED("use ScrollEvent and scrollEvent() instead") GlfwApplication::MouseScrollEvent: public GlfwApplication::InputEvent {
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
        GlfwApplication::Modifiers modifiers();

    private:
        friend GlfwApplication;

        explicit MouseScrollEvent(GLFWwindow* window, const Vector2& offset): _window{window}, _offset{offset} {}

        GLFWwindow* const _window;
        const Vector2 _offset;
        Containers::Optional<Vector2i> _position;
        Containers::Optional<GlfwApplication::Modifiers> _modifiers;
};
#endif

/**
@brief Text input event

@see @ref textInputEvent(), @ref platform-windowed-key-events
*/
class GlfwApplication::TextInputEvent {
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

    private:
        friend GlfwApplication;

        explicit TextInputEvent(Containers::StringView text): _text{text}, _accepted{false} {}

        const Containers::StringView _text;
        bool _accepted;
};

/** @hideinitializer
@brief Entry point for GLFW-based applications
@param className Class name

@m_keywords{MAGNUM_APPLICATION_MAIN()}

See @ref Magnum::Platform::GlfwApplication "Platform::GlfwApplication" for
usage information. This macro abstracts out platform-specific entry point code
and is equivalent to the following, see @ref portability-applications for more
information.

@code{.cpp}
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode

When no other application header is included this macro is also aliased to
@cpp MAGNUM_APPLICATION_MAIN() @ce.
*/
#define MAGNUM_GLFWAPPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef GlfwApplication Application;
typedef BasicScreen<GlfwApplication> Screen;
typedef BasicScreenedApplication<GlfwApplication> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_GLFWAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

}}

#endif
