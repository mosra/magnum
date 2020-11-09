#ifndef Magnum_Platform_EmscriptenApplication_h
#define Magnum_Platform_EmscriptenApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018, 2019 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2020 Pablo Escobar <mail@rvrs.in>

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

#if defined(CORRADE_TARGET_EMSCRIPTEN) || defined(DOXYGEN_GENERATING_OUTPUT)
/** @file
 * @brief Class @ref Magnum::Platform::EmscriptenApplication, macro @ref MAGNUM_EMSCRIPTENAPPLICATION_MAIN()
 * @m_since{2019,10}
 */
#endif

#include <string>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/GL/GL.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Platform/Platform.h"

#if defined(CORRADE_TARGET_EMSCRIPTEN) || defined(DOXYGEN_GENERATING_OUTPUT)

#ifndef DOXYGEN_GENERATING_OUTPUT
struct EmscriptenKeyboardEvent;
struct EmscriptenMouseEvent;
struct EmscriptenWheelEvent;
struct EmscriptenUiEvent;
typedef int EMSCRIPTEN_WEBGL_CONTEXT_HANDLE;
#endif

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief Emscripten application
@m_since{2019,10}

@m_keywords{Application}

Application running on Emscripten. Available only on
@ref CORRADE_TARGET_EMSCRIPTEN "Emscripten", see respective sections
in the @ref building-corrade-cross-emscripten "Corrade" and
@ref building-cross-emscripten "Magnum" building documentation.

@section Platform-EmscriptenApplication-bootstrap Bootstrap application

Fully contained base application using @ref Sdl2Application for desktop build
and @ref EmscriptenApplication for Emscripten build along with full HTML markup
and CMake setup is available in `base-emscripten` branch of the
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-emscripten.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-emscripten.zip)
file. After extracting the downloaded archive, you can do the desktop build in
the same way as with @ref Sdl2Application. For the Emscripten build you also
need to put the contents of toolchains repository from
https://github.com/mosra/toolchains in a `toolchains/` subdirectory. There are
two toolchain files. The `generic/Emscripten.cmake` is for the classical
(asm.js) build, the `generic/Emscripten-wasm.cmake` is for WebAssembly build.
Don't forget to adapt `EMSCRIPTEN_PREFIX` variable in
`toolchains/generic/Emscripten*.cmake` to path where Emscripten is installed;
you can also pass it explicitly on command-line using `-DEMSCRIPTEN_PREFIX`.
Default is `/usr/emscripten`.

Then create build directory and run `cmake` and build/install commands in it.
Set `CMAKE_PREFIX_PATH` to where you have all the dependencies installed, set
`CMAKE_INSTALL_PREFIX` to have the files installed in proper location (a
webserver, e.g.  `/srv/http/emscripten`).

@code{.sh}
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=path/to/toolchains/generic/Emscripten-wasm.cmake \
    -DCMAKE_PREFIX_PATH=/usr/lib/emscripten/system \
    -DCMAKE_INSTALL_PREFIX=/srv/http/emscripten
cmake --build .
cmake --build . --target install
@endcode

You can then open `MyApplication.html` in your browser (through a webserver,
e.g. http://localhost/emscripten/MyApplication.html).

Detailed information about deployment for Emscripten and all needed boilerplate
together with a troubleshooting guide is available in @ref platforms-html5.

@section Platform-EmscriptenApplication-usage General usage

This application library is built if `WITH_EMSCRIPTENAPPLICATION` is enabled
when building Magnum. To use this library with CMake, put
[FindOpenGLES2.cmake](https://github.com/mosra/magnum/blob/master/modules/FindOpenGLES2.cmake) (or
[FindOpenGLES3.cmake](https://github.com/mosra/magnum/blob/master/modules/FindOpenGLES3.cmake))
into your `modules/` directory, request the `EmscriptenApplication` component
of the `Magnum` package and link to the `Magnum::EmscriptenApplication` target:

@code{.cmake}
find_package(Magnum REQUIRED)
if(CORRADE_TARGET_EMSCRIPTEN)
    find_package(Magnum REQUIRED EmscriptenApplication)
endif()

# ...
if(CORRADE_TARGET_EMSCRIPTEN)
    target_link_libraries(your-app PRIVATE Magnum::EmscriptenApplication)
endif()
@endcode

Additionally, if you're using Magnum as a CMake subproject, do the following
* *before* calling @cmake find_package() @ce to ensure it's enabled, as the
library is not built by default:

@code{.cmake}
set(WITH_EMSCRIPTENAPPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

If no other application is requested, you can also use the generic
`Magnum::Application` alias to simplify porting. See @ref building and
@ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen.

@code{.cpp}
class MyApplication: public Platform::EmscriptenApplication {
    // implement required methods...
};
MAGNUM_EMSCRIPTENAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
@cpp Platform::Application @ce and the macro is aliased to
@cpp MAGNUM_APPLICATION_MAIN() @ce to simplify porting.

@section Platform-EmscriptenApplication-browser Browser-specific behavior

Leaving a default (zero) size in @ref Configuration will cause the app to use a
size that corresponds to *CSS pixel size* of the @cb{.html} <canvas> @ce
element. The size is then multiplied by DPI scaling value, see
@ref Platform-EmscriptenApplication-dpi "DPI awareness" below for details.

If you enable @ref Configuration::WindowFlag::Resizable, the canvas will be
resized when size of the canvas changes and you get @ref viewportEvent(). If
the flag is not enabled, no canvas resizing is performed.

Unlike desktop platforms, the browser has no concept of application exit code,
so the return value of @ref exec() is always @cpp 0 @ce and whatever is passed
to @ref exit(int) is ignored.

@subsection Platform-EmscriptenApplication-browser-main-loop Main loop implementation

Magnum application implementations default to redrawing only when needed to
save power and while this is simple to implement efficiently on desktop apps
where the application has the full control over the main loop, it's harder in
the callback-based browser environment.

@ref Sdl2Application makes use of @m_class{m-doc-external} [emscripten_set_main_loop()](https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_set_main_loop),
which periodically calls @m_class{m-doc-external} [window.requestAnimationFrame()](https://developer.mozilla.org/en-US/docs/Web/API/window/requestAnimationFrame)
in order to maintain a steady frame rate. For apps that need to redraw only
when needed this means the callback will be called 60 times per second only to
be a no-op. While that's still significantly more efficient than drawing
everything each time, it still means the browser has to wake up 60 times per
second to do nothing.

@ref EmscriptenApplication instead makes use of `requestAnimationFrame()`
directly --- on initialization and on @ref redraw(), an animation frame will be
requested and the callback set up. The callback will immediately schedule
another animation frame, but cancel that request after @ref drawEvent() if
@ref redraw() was not requested. Note that due to the way Emscripten internals
work, this also requires the class instance to be stored as a global variable
instead of a local variable in @cpp main() @ce. The
@ref MAGNUM_EMSCRIPTENAPPLICATION_MAIN() macro handles this in a portable way
for you.

For testing purposes or for more predictable behavior for example when the
application has to redraw all the time anyway this can be disabled using
@ref Configuration::WindowFlag::AlwaysRequestAnimationFrame. Setting the flag
will make the main loop behave equivalently to @ref Sdl2Application.

@section Platform-EmscriptenApplication-webgl WebGL-specific behavior

While WebGL itself requires all extensions to be
[enabled explicitly](https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Using_Extensions),
by default Emscripten enables all supported extensions that don't have a
negative effect on performance to simplify porting. This is controlled by
@ref GLConfiguration::Flag::EnableExtensionsByDefault and the flag is enabled
by default. When disabled, you are expected to enable desired extensions
manually using @m_class{m-doc-external} [emscripten_webgl_enable_extension()](https://emscripten.org/docs/api_reference/html5.h.html#c.emscripten_webgl_enable_extension).

@attention Because @ref GLConfiguration::Flag::EnableExtensionsByDefault is
    among default flags, calling @ref GLConfiguration::setFlags() will reset
    this default, causing crashes at runtime when extension functionality is
    used. To be safe, you might want to use @ref GLConfiguration::addFlags()
    and @ref GLConfiguration::clearFlags() instead.

@section Platform-EmscriptenApplication-dpi DPI awareness

Since this application targets only web browsers, DPI handling isn't as general
as in case of @ref Sdl2Application or @ref GlfwApplication. See
@ref Platform-Sdl2Application-dpi "Sdl2Application DPI awareness" documentation
for a guide covering all platform differences.

For this application in particular, @ref windowSize() can be different than
@ref framebufferSize() on HiDPI displays --- which is different from
@ref Sdl2Application behavior on Emscripten. By default, @ref dpiScaling() is
@cpp 1.0f @ce in both dimensions but it can be overriden using custom DPI
scaling --- the `--magnum-dpi-scaling` command-line options are supported the
same way as in @ref Sdl2Application, only in the form of URL GET parameters,
similarly to all other @ref platforms-html5-environment "command-line options".

Having @ref dpiScaling() set to @cpp 1.0f @ce is done in order to have
consistent behavior with other platforms --- platforms have either
@ref windowSize() equivalent to @ref framebufferSize() and then
@ref dpiScaling() specifies the UI scale (Windows/Linux/Android-like) or
@ref windowSize() different from @ref framebufferSize() (which defines the UI
scale) and then @ref dpiScaling() is @cpp 1.0f @ce (macOS/iOS-like), so this
is the second case. The actual device pixel ratio is expressed in the ratio of
@ref windowSize() and @ref framebufferSize() so crossplatform code shouldn't
have a need to query it, however for completeness it's exposed in
@ref devicePixelRatio() and @ref ViewportEvent::devicePixelRatio().

Setting custom DPI scaling will affect @ref framebufferSize()
(larger values making the canvas backing framebuffer larger and vice versa),
@ref windowSize() will stay unaffected as it's controlled by the CSS, and
@ref devicePixelRatio() will stay the same as well as it's defined by the
browser.

To avoid confusion, documentation of all @ref EmscriptenApplication APIs always
mentions only the web case, consult equivalent APIs in @ref Sdl2Application or
@ref GlfwApplication for behavior in those implementations.
*/
class EmscriptenApplication {
    public:
        /** @brief Application arguments */
        struct Arguments {
            /** @brief Constructor */
            /*implicit*/ constexpr Arguments(int& argc, char** argv) noexcept: argc{argc}, argv{argv} {}

            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        class Configuration;
        class GLConfiguration;
        class ViewportEvent;
        class InputEvent;
        class MouseEvent;
        class MouseMoveEvent;
        class MouseScrollEvent;
        class KeyEvent;
        class TextInputEvent;

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Construct with given configuration for WebGL context
         * @param arguments         Application arguments
         * @param configuration     Application configuration
         * @param glConfiguration   WebGL context configuration
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
        explicit EmscriptenApplication(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration);
        #endif

        /**
         * @brief Construct with given configuration
         *
         * If @ref Configuration::WindowFlag::Contextless is present or Magnum
         * was not built with @ref MAGNUM_TARGET_GL, this creates a window
         * without any GPU context attached, leaving that part on the user.
         *
         * If none of the flags is present and Magnum was built with
         * @ref MAGNUM_TARGET_GL, this is equivalent to calling
         * @ref EmscriptenApplication(const Arguments&, const Configuration&, const GLConfiguration&)
         * with default-constructed @ref GLConfiguration.
         *
         * See also @ref building-features for more information.
         */
        explicit EmscriptenApplication(const Arguments& arguments, const Configuration& configuration);

        /**
         * @brief Construct with default configuration
         *
         * Equivalent to calling @ref EmscriptenApplication(const Arguments&, const Configuration&)
         * with default-constructed @ref Configuration.
         */
        explicit EmscriptenApplication(const Arguments& arguments);

        /**
         * @brief Construct without setting up a canvas
         * @param arguments     Application arguments
         *
         * Unlike above, the canvas is not set up and must be created later
         * with @ref create() or @ref tryCreate().
         */
        explicit EmscriptenApplication(const Arguments& arguments, NoCreateT);

        /** @brief Copying is not allowed */
        EmscriptenApplication(const EmscriptenApplication&) = delete;

        /** @brief Moving is not allowed */
        EmscriptenApplication(EmscriptenApplication&&) = delete;

        /** @brief Copying is not allowed */
        EmscriptenApplication& operator=(const EmscriptenApplication&) = delete;

        /** @brief Moving is not allowed */
        EmscriptenApplication& operator=(EmscriptenApplication&&) = delete;

        /**
         * @brief Execute the application
         *
         * Sets up Emscripten to execute event handlers until @ref exit() is
         * called. See @ref MAGNUM_EMSCRIPTENAPPLICATION_MAIN() for usage
         * information.
         */
        int exec();

        /**
         * @brief Exit application main loop
         * @param exitCode  Ignored, present only for API compatibility with
         *      other app implementations.
         *
         * When called from application constructor, it will cause the
         * application to exit immediately after constructor ends, without any
         * events being processed. Calling this function is recommended over
         * @ref std::exit() or @ref Corrade::Utility::Fatal "Fatal", which exit
         * immediately and without calling destructors on local scope. Note
         * that, however, you need to explicitly @cpp return @ce after calling
         * it, as it can't exit the constructor on its own:
         *
         * @snippet MagnumPlatform.cpp exit-from-constructor
         *
         * When called from the main loop, the application exits cleanly
         * before next main loop iteration is executed.
         */
        void exit(int exitCode = 0);

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Underlying WebGL context
         *
         * Use in case you need to call Emscripten functionality directly.
         * Returns @cpp 0 @ce in case the context was not created yet.
         */
        EMSCRIPTEN_WEBGL_CONTEXT_HANDLE glContext() { return _glContext; }
        #endif

    protected:
        /* Nobody will need to have (and delete) EmscriptenApplication*, thus
           just making it protected would be faster than public pure virtual
           destructor. However, because we store it in a Pointer in
           MAGNUM_EMSCRIPTENAPPLICATION_MAIN(), Clang complains that
           "delete called on non-final 'MyApplication' that has virtual
           functions but non-virtual destructor", so we have to mark it virtual
           anyway. */
        virtual ~EmscriptenApplication();

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Set up a canvas with given configuration for WebGL context
         * @param configuration     Application configuration
         * @param glConfiguration   WebGL context configuration
         *
         * Must be called only if the context wasn't created by the constructor
         * itself, i.e. when passing @ref NoCreate to it. Error message is
         * printed and the program exits if the context cannot be created, see
         * @ref tryCreate() for an alternative.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         */
        void create(const Configuration& configuration, const GLConfiguration& glConfiguration);
        #endif

        /**
         * @brief Set up a canvas with given configuration and WebGL context
         *
         * Equivalent to calling @ref create(const Configuration&, const GLConfiguration&)
         * with default-constructed @ref GLConfiguration.
         */
        void create(const Configuration& configuration);

        /**
         * @brief Set up a canvas with default configuration and WebGL context
         *
         * Equivalent to calling @ref create(const Configuration&) with
         * default-constructed @ref Configuration.
         */
        void create();

        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Try to create context with given configuration for WebGL context
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
         * @brief Canvas size
         *
         * Canvas size to which all input event coordinates can be related.
         * On HiDPI displays, canvas size can be different from
         * @ref framebufferSize(). See @ref Platform-Sdl2Application-dpi for
         * more information. Note that this method is named "window size" to be
         * API-compatible with Application implementations on other platforms.
         */
        Vector2i windowSize() const;

        #if defined(MAGNUM_TARGET_GL) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Framebuffer size
         *
         * On HiDPI displays, framebuffer size can be different from
         * @ref windowSize(). See @ref Platform-EmscriptenApplication-dpi for
         * more information.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         *
         * @see @ref Sdl2Application::framebufferSize()
         */
        Vector2i framebufferSize() const;
        #endif

        /**
         * @brief DPI scaling
         *
         * How the content should be scaled relative to system defaults for
         * given @ref windowSize(). If a window is not created yet, returns
         * zero vector, use @ref dpiScaling(const Configuration&) const for
         * calculating a value depending on user configuration. By default set
         * to @cpp 1.0 @ce, see @ref Platform-EmscriptenApplication-dpi for
         * more information.
         * @see @ref framebufferSize(), @ref devicePixelRatio()
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

        /**
         * @brief DPI scaling for given configuration
         *
         * Calculates DPI scaling that would be used when creating a window
         * with given @p configuration. Takes into account DPI scaling policy
         * and custom scaling specified via URL GET parameters. See
         * @ref Platform-EmscriptenApplication-dpi for more information.
         * @ref devicePixelRatio()
         */
        Vector2 dpiScaling(const Configuration& configuration) const;

        /**
         * @brief Device pixel ratio
         *
         * Crossplatform code shouldn't need to query this value because the
         * pixel ratio is already expressed in the ratio of @ref windowSize()
         * and @ref framebufferSize() values.
         * @see @ref dpiScaling()
         */
        Vector2 devicePixelRatio() const { return _devicePixelRatio; }

        /**
         * @brief Set window title
         *
         * The @p title is expected to be encoded in UTF-8.
         */
        void setWindowTitle(const std::string& title);

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
         * @m_class{m-note m-danger}
         *
         * @par
         *      For backwards compatibility purposes the function will look for
         *      *any* @cb{.html} <div id="container"> @ce in case the
         *      @cb{.html} <div class="mn-container"> @ce is not found. This
         *      compatibility is scheduled to be removed in the future.
         */
        void setContainerCssClass(const std::string& cssClass);

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers();

        /** @copydoc Sdl2Application::redraw() */
        void redraw();

    private:
        /** @copydoc GlfwApplication::viewportEvent(ViewportEvent&) */
        virtual void viewportEvent(ViewportEvent& event);

        /** @copydoc Sdl2Application::drawEvent() */
        virtual void drawEvent() = 0;

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Keyboard handling */

        /** @copydoc Sdl2Application::keyPressEvent() */
        virtual void keyPressEvent(KeyEvent& event);

        /** @copydoc Sdl2Application::keyReleaseEvent() */
        virtual void keyReleaseEvent(KeyEvent& event);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Mouse handling */

    public:
        /**
         * @brief Cursor type
         * @m_since{2020,06}
         *
         * Value names in this enum don't necessarily match the CSS names in
         * order to be compatible with @ref Sdl2Application and
         * @ref GlfwApplication.
         * @see @ref setCursor()
         */
        enum class Cursor: UnsignedInt {
            /* Keeping the same order as at
               https://developer.mozilla.org/en-US/docs/Web/CSS/cursor, which
               is different than in Sdl2Application / GlfwApplication */

            /**
             * The browser determines the cursor depending on the context.
             * Since this affects the cursor when hovering the
             * @cb{.html} <canvas> @ce element, this is usually equivalent to
             * @ref Cursor::Arrow. Matches @cb{.css} cursor: auto @ce in CSS.
             */
            Auto,

            /** Arrow. Matches @cb{.css} cursor: default @ce in CSS. */
            Arrow,

            /** Hidden. Matches @cb{.css} cursor: none @ce in CSS. */
            Hidden,

            /**
             * Context menu. Matches @cb{.css} cursor: context-menu @ce in CSS.
             */
            ContextMenu,

            /** Help. Matches @cb{.css} cursor: help @ce in CSS. */
            Help,

            /** Hand. Matches @cb{.css} cursor: pointer @ce in CSS. */
            Hand,

            /**
             * Small wait cursor. Matches @cb{.css} cursor: progress @ce in
             * CSS.
             */
            WaitArrow,

            /** Wait. Matches @cb{.css} cursor: wait @ce in CSS. */
            Wait,

            /** Cell. Matches @cb{.css} cursor: cell @ce in CSS. */
            Cell,

            /** Crosshair. Matches @cb{.css} cursor: crosshair @ce in CSS. */
            Crosshair,

            /** Text input. Matches @cb{.css} cursor: text @ce in CSS. */
            TextInput,

            /**
             * Vertical text input. Matches @cb{.css} cursor: vertical-text @ce
             * in CSS.
             */
            VerticalTextInput,

            /** Alias. Matches @cb{.css} cursor: alias @ce in CSS. */
            Alias,

            /** Copy. Matches @cb{.css} cursor: copy @ce in CSS. */
            Copy,

            /**
             * Four pointed arrow pointing north, south, east, and west.
             * Matches @cb{.css} cursor: move @ce in CSS.
             */
            ResizeAll,

            /**
             * Drop not allowed. Matches @cb{.css} cursor: no-drop @ce in CSS.
             */
            NoDrop,

            /**
             * Slashed circle or crossbones. Matches
             * @cb{.css} cursor: not-allowed @ce in CSS.
             */
            No,

            /** Grab. Matches @cb{.css} cursor: grab @ce in CSS. */
            Grab,

            /** Grabbing. Matches @cb{.css} cursor: grabbing @ce in CSS. */
            Grabbing,

            /**
             * Scroll in any direction. Matches
             * @cb{.css} cursor: all-scroll @ce in CSS.
             */
            AllScroll,

            /**
             * Column resize. Matches @cb{.css} cursor: col-resize @ce in CSS.
             */
            ColResize,

            /**
             * Row resize. Matches @cb{.css} cursor: row-resize @ce in CSS.
             */
            RowResize,

            /**
             * Resize arrow pointing north. Matches
             * @cb{.css} cursor: n-resize @ce in CSS.
             */
            ResizeN,

            /**
             * Resize arrow pointing east. Matches
             * @cb{.css} cursor: e-resize @ce in CSS.
             */
            ResizeE,

            /**
             * Resize arrow pointing south. Matches
             * @cb{.css} cursor: s-resize @ce in CSS.
             */
            ResizeS,

            /**
             * Resize arrow pointing west. Matches
             * @cb{.css} cursor: w-resize @ce in CSS.
             */
            ResizeW,

            /**
             * Resize arrow pointing northeast. Matches
             * @cb{.css} cursor: ne-resize @ce in CSS.
             */
            ResizeNE,

            /**
             * Resize arrow pointing northwest. Matches
             * @cb{.css} cursor: nw-resize @ce in CSS.
             */
            ResizeNW,

            /**
             * Resize arrow pointing southeast. Matches
             * @cb{.css} cursor: se-resize @ce in CSS.
             */
            ResizeSE,

            /**
             * Resize arrow pointing southwest. Matches
             * @cb{.css} cursor: se-resize @ce in CSS.
             */
            ResizeSW,

            /**
             * Double resize arrow pointing west and east. Matches
             * @cb{.css} cursor: ew-resize @ce in CSS.
             */
            /* Kept like this for compatibility with Sdl2 and GlfwApp (winapi
               has it like this, too) */
            ResizeWE,

            /**
             * Double resize arrow pointing north and south. Matches
             * @cb{.css} cursor: ns-resize @ce in CSS.
             */
            ResizeNS,

            /**
             * Double resize arrow pointing northeast and southwest. Matches
             * @cb{.css} cursor: nesw-resize @ce in CSS.
             */
            ResizeNESW,

            /**
             * Double resize arrow pointing northwest and southeast. Matches
             * @cb{.css} cursor: nwse-resize @ce in CSS.
             */
            ResizeNWSE,

            /** Zoom in. Matches @cb{.css} cursor: zoom-in @ce in CSS. */
            ZoomIn,

            /** Zoom out. Matches @cb{.css} cursor: zoom-out @ce in CSS. */
            ZoomOut
        };

        /**
         * @brief Set cursor type
         * @m_since{2020,06}
         *
         * Default is @ref Cursor::Arrow.
         */
        void setCursor(Cursor cursor);

        /**
         * @brief Get current cursor type
         * @m_since{2020,06}
         */
        Cursor cursor();

    private:
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
         * If text input is active, text input events go to @ref textInputEvent().
         * @note Note that in @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten" the
         *      value is emulated and might not reflect external events like
         *      closing on-screen keyboard.
         * @see @ref startTextInput(), @ref stopTextInput()
         */
        bool isTextInputActive() const {
            return !!(_flags & Flag::TextInputActive);
        }

        /**
         * @brief Start text input
         *
         * Starts text input that will go to @ref textInputEvent().
         * @see @ref stopTextInput(), @ref isTextInputActive(),
         *      @ref setTextInputRect()
         */
        void startTextInput();

        /**
         * @brief Stop text input
         *
         * Stops text input that went to @ref textInputEvent().
         * @see @ref startTextInput(), @ref isTextInputActive(),
         *      @ref textInputEvent()
         */
        void stopTextInput();

        /**
         * @brief Set text input rectangle
         *
         * The @p rect defines an area where the text is being displayed, for
         * example to hint the system where to place on-screen keyboard.
         * @note Currently not implemented, included only for compatibility with
         * other Application implementations.
         */
        void setTextInputRect(const Range2Di& rect);

    private:
        /**
         * @brief Text input event
         *
         * Called when text input is active and the text is being input.
         * @see @ref isTextInputActive()
         */
        virtual void textInputEvent(TextInputEvent& event);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

    private:
        enum class Flag: UnsignedByte {
            Redraw = 1 << 0,
            TextInputActive = 1 << 1,
            ExitRequested = 1 << 2,
            LoopActive = 1 << 3
        };
        typedef Containers::EnumSet<Flag> Flags;

        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        void handleCanvasResize(const EmscriptenUiEvent* event);
        /* Sorry, but can't use Configuration::WindowFlags here :( */
        void setupCallbacks(bool resizable);
        void setupAnimationFrame(bool ForceAnimationFrame);

        Vector2 _devicePixelRatio, _dpiScaling;
        Vector2i _lastKnownCanvasSize, _previousMouseMovePosition{-1};

        Flags _flags;
        Cursor _cursor;

        bool _deprecatedTargetBehavior{};
        std::string _canvasTarget;

        #ifdef MAGNUM_TARGET_GL
        EMSCRIPTEN_WEBGL_CONTEXT_HANDLE _glContext{};
        Containers::Pointer<Platform::GLContext> _context;
        #endif

        /* These are saved from command-line arguments */
        bool _verboseLog{};
        Vector2 _commandLineDpiScaling;

        /* Animation frame callback */
        int (*_callback)(void*);
};

#ifdef MAGNUM_TARGET_GL
/**
@brief WebGL context configuration

The created context is always with a double-buffered OpenGL context.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref EmscriptenApplication(), @ref Configuration, @ref create(),
    @ref tryCreate()
*/
class EmscriptenApplication::GLConfiguration {
    public:
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref setFlags(), @ref GL::Context::Flag
         */
        enum class Flag: Int {
            /**
             * Premultiplied alpha. If set, the alpha channel of the rendering
             * context will be treated as representing premultiplied alpha
             * values. If not set, the alpha channel represents
             * non-premultiplied alpha.
             */
            PremultipliedAlpha = 1 << 0,

            /**
             * Preserve drawing buffer. If set, the contents of the drawing
             * buffer are preserved between consecutive @ref drawEvent() calls.
             * If not, color, depth and stencil are cleared before entering
             * @ref drawEvent(). Not setting this gives better performance.
             */
            PreserveDrawingBuffer = 1 << 1,

            /**
             * Prefer low power to high performance. If set, the WebGL power
             * preference will be set to reduce power consumption.
             */
            PreferLowPowerToHighPerformance = 1 << 2,

            /**
             * Fail if major performance caveat. If set, requests context
             * creation to abort if the browser is only able to create a
             * context that does not give good hardware-accelerated
             * performance.
             */
            FailIfMajorPerformanceCaveat = 1 << 3,

            /**
             * Explicit swap control. For more details, see the
             * [Emscripten API reference](https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes.explicitSwapControl).
             */
            ExplicitSwapControl = 1 << 4,

            /**
             * Enable WebGL extensions by default. Enabled by default. For more
             * details, see @ref Platform-EmscriptenApplication-webgl and the
             * [Emscripten API reference](https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes.enableExtensionsByDefault).
             */
            EnableExtensionsByDefault = 1 << 5,

            /**
             * Render via offscreen back buffer. For more details, see the
             * [Emscripten API reference](https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes.renderViaOffscreenBackBuffer).
             */
            RenderViaOffscreenBackBuffer = 1 << 6,

            /**
             * Proxy content to main thread. For more details, see the
             * [Emscripten API reference](https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes.proxyContextToMainThread).
             */
            ProxyContextToMainThread = 1 << 7
        };

        /**
         * @brief Context flags
         *
         * @see @ref setFlags(), @ref GL::Context::Flags
         */
        typedef Containers::EnumSet<Flag> Flags;

        /*implicit*/ GLConfiguration();

        /** @brief Context flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is @ref Flag::EnableExtensionsByDefault.
         * @see @ref addFlags(), @ref clearFlags(), @ref GL::Context::flags()
         */
        GLConfiguration& setFlags(Flags flags) {
            _flags = flags;
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
            _flags |= flags;
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
            _flags &= ~flags;
            return *this;
        }

        /**
         * @brief Set context version
         *
         * @note This function does nothing and is included only for
         *      compatibility with other toolkits. @ref GL::Version::GLES200 or
         *      @ref GL::Version::GLES300 is used based on engine compile-time
         *      settings.
         */
        GLConfiguration& setVersion(GL::Version) { return *this; }

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
         * Note that WebGL does not allow setting the sample count, but merely
         * enabling or disabling multisampling. Multisampling will be enabled
         * if sample count is greater than @cpp 0 @ce.
         */
        GLConfiguration& setSampleCount(Int count) {
            _sampleCount = count;
            return *this;
        }

    private:
        Vector4i _colorBufferSize;
        Int _depthBufferSize, _stencilBufferSize;
        Int _sampleCount;

        Flags _flags{Flag::EnableExtensionsByDefault};
};

CORRADE_ENUMSET_OPERATORS(EmscriptenApplication::GLConfiguration::Flags)
#endif

/**
@brief Configuration

@see @ref EmscriptenApplication(), @ref GLConfiguration, @ref create(),
    @ref tryCreate()
*/
class EmscriptenApplication::Configuration {
    public:
        /**
         * @brief Window flag
         *
         * @see @ref WindowFlags, @ref setWindowFlags()
         */
        enum class WindowFlag: UnsignedShort {
            /**
             * Do not create any GPU context. Use together with
             * @ref EmscriptenApplication(const Arguments&),
             * @ref EmscriptenApplication(const Arguments&, const Configuration&),
             * @ref create(const Configuration&) or
             * @ref tryCreate(const Configuration&) to prevent implicit
             * creation of an WebGL context.
             */
            Contextless = 1 << 0,

            /**
             * Resizable canvas. This causes the framebuffer to be resized
             * when the @cb{.html} <canvas> @ce size changes, either directly
             * or as a consequence of browser window size change.
             *
             * Implement @ref viewportEvent() to react to the resizing events.
             */
            Resizable = 1 << 1,

            /**
             * Always request the next animation frame. Disables the
             * idle-efficient main loop described in
             * @ref Platform-EmscriptenApplication-browser-main-loop and
             * unconditionally schedules @m_class{m-doc-external} [window.requestAnimationFrame()](https://developer.mozilla.org/en-US/docs/Web/API/window/requestAnimationFrame),
             * matching the behavior of @ref Sdl2Application. Useful for
             * testing or for simpler internal state when your app is going to
             * redraw all the time anyway.
             *
             * Note that this does not affect how @ref drawEvent() is executed
             * --- it depends on @ref redraw() being called independently of
             * this flag being set.
             */
            AlwaysRequestAnimationFrame = 1 << 2
        };

        /**
         * @brief Window flags
         *
         * @see @ref setWindowFlags()
         */
        typedef Containers::EnumSet<WindowFlag> WindowFlags;

        constexpr /*implicit*/ Configuration() {}

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * @note This function does nothing and is included only for
         *      compatibility with other toolkits, as the page title is
         *      expected to be set by the HTML markup. However, it's possible
         *      to change the page title later (for example in response to
         *      application state change) using @ref setWindowTitle().
         */
        template<class T> Configuration& setTitle(const T&) { return *this; }

        /** @brief Window size */
        Vector2i size() const { return _size; }

        /**
         * @brief Set canvas size
         * @param size              Desired canvas size
         * @param dpiScaling        Custom DPI scaling value
         *
         * Default is a zero vector, meaning a value that matches the display
         * or canvas size is autodetected. See
         * @ref Platform-EmscriptenApplication-dpi for more information. When
         * @p dpiScaling is not a zero vector, this function sets the DPI
         * scaling directly. The resulting @ref windowSize() is
         * @cpp size*dpiScaling @ce and @ref dpiScaling() is @p dpiScaling.
         */
        Configuration& setSize(const Vector2i& size, const Vector2& dpiScaling = {}) {
            _size = size;
            _dpiScaling = dpiScaling;
            return *this;
        }

        /**
         * @brief Custom DPI scaling
         *
         * If zero, the devices pixel ratio has a priority over this value.
         * The `--magnum-dpi-scaling` option (specified via URL GET parameters)
         * has a priority over any application-set value.
         * @see @ref setSize(const Vector2i&, const Vector2&)
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

        /** @brief Window flags */
        WindowFlags windowFlags() const {
            return _windowFlags;
        }

        /**
         * @brief Set window flags
         * @return Reference to self (for method chaining)
         */
        Configuration& setWindowFlags(WindowFlags windowFlags) {
            _windowFlags = windowFlags;
            return *this;
        }

    private:
        Vector2i _size;
        Vector2 _dpiScaling;
        WindowFlags _windowFlags;
};

CORRADE_ENUMSET_OPERATORS(EmscriptenApplication::Configuration::WindowFlags)

/**
@brief Viewport event

@see @ref viewportEvent()
*/
class EmscriptenApplication::ViewportEvent {
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
         * @brief Canvas size
         *
         * On HiDPI displays, window size can be different from
         * @ref framebufferSize(). See @ref Platform-EmscriptenApplication-dpi
         * for more information. Note that this method is named "window size"
         * to be API-compatible with Application implementations on other
         * platforms.
         * @see @ref EmscriptenApplication::windowSize()
         */
        Vector2i windowSize() const { return _windowSize; }

        #if defined(MAGNUM_TARGET_GL) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Framebuffer size
         *
         * On HiDPI displays, framebuffer size can be different from
         * @ref windowSize(). See @ref Platform-EmscriptenApplication-dpi for
         * more information.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         *
         * @see @ref EmscriptenApplication::framebufferSize()
         */
        Vector2i framebufferSize() const { return _framebufferSize; }
        #endif

        /**
         * @brief DPI scaling
         *
         * On some platforms moving a browser window between displays can
         * result in DPI scaling value being changed in tandem with a
         * canvas/framebuffer size. Simply resizing the canvas doesn't change
         * the DPI scaling value. See @ref Platform-EmscriptenApplication-dpi
         * for more information.
         * @see @ref EmscriptenApplication::dpiScaling()
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

        /**
         * @brief Device pixel ratio
         *
         * On some platforms moving a browser window between displays can
         * result in device pixel ratio value being changed. Crossplatform code
         * shouldn't need to query this value because the ratio is already
         * expressed in the ratio of @ref windowSize() and @ref framebufferSize()
         * values. See @ref Platform-EmscriptenApplication-dpi for more
         * information.
         * @see @ref EmscriptenApplication::devicePixelRatio()
         */
        Vector2 devicePixelRatio() const { return _devicePixelRatio; }

        /**
         * @brief Underlying Emscripten event
         *
         * If the viewport event doesn't come from a browser event (for example
         * when the canvas was resized programatically and not as a consequence
         * of window size change), the function returns @cpp nullptr @ce.
         */
        const EmscriptenUiEvent* event() const { return _event; }

    private:
        friend EmscriptenApplication;

        explicit ViewportEvent(const EmscriptenUiEvent* event,
            const Vector2i& windowSize,
            #ifdef MAGNUM_TARGET_GL
            const Vector2i& framebufferSize,
            #endif
            const Vector2& dpiScaling, const Vector2& devicePixelRatio):
                _event{event},
                _windowSize{windowSize},
                #ifdef MAGNUM_TARGET_GL
                _framebufferSize{framebufferSize},
                #endif
                _dpiScaling{dpiScaling}, _devicePixelRatio{devicePixelRatio} {}

        const EmscriptenUiEvent* _event;
        const Vector2i _windowSize;
        #ifdef MAGNUM_TARGET_GL
        const Vector2i _framebufferSize;
        #endif
        const Vector2 _dpiScaling, _devicePixelRatio;
};

/**
@brief Base for input events

@see @ref KeyEvent, @ref MouseEvent, @ref MouseMoveEvent, @ref keyPressEvent(),
    @ref mousePressEvent(), @ref mouseReleaseEvent(), @ref mouseMoveEvent()
*/
class EmscriptenApplication::InputEvent {
    public:
        /**
         * @brief Modifier
         *
         * @see @ref Modifiers, @ref KeyEvent::modifiers(),
         *      @ref MouseEvent::modifiers()
         */
        enum class Modifier: Int {
            /**
             * Shift
             *
             * @see @ref KeyEvent::Key::LeftShift, @ref KeyEvent::Key::RightShift
             */
            Shift = 1 << 0,

            /**
             * Ctrl
             *
             * @see @ref KeyEvent::Key::LeftCtrl, @ref KeyEvent::Key::RightCtrl
             */
            Ctrl = 1 << 1,

            /**
             * Alt
             *
             * @see @ref KeyEvent::Key::LeftAlt, @ref KeyEvent::Key::RightAlt
             */
            Alt = 1 << 2,

            /**
             * Super key (Windows/⌘)
             *
             * @see @ref KeyEvent::Key::LeftSuper, @ref KeyEvent::Key::RightSuper
             */
            Super = 1 << 3
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
        bool isAccepted() const { return _accepted; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it is
         * propagated to other elements on the page. By default each event is
         * ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

    protected:
        explicit InputEvent(): _accepted(false) {}

        ~InputEvent() = default;

    private:
        bool _accepted;
};

CORRADE_ENUMSET_OPERATORS(EmscriptenApplication::InputEvent::Modifiers)

/**
@brief Mouse event

@see @ref MouseMoveEvent, @ref mousePressEvent(), @ref mouseReleaseEvent()
*/
class EmscriptenApplication::MouseEvent: public EmscriptenApplication::InputEvent {
    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: std::int32_t {
            Left,       /**< Left mouse button */
            Middle,     /**< Middle mouse button */
            Right       /**< Right mouse button */
        };

        /** @brief Button */
        Button button() const;

        /** @brief Position */
        Vector2i position() const;

        /** @brief Modifiers */
        Modifiers modifiers() const;

        /** @brief Underlying Emscripten event */
        const EmscriptenMouseEvent& event() const { return _event; }

    private:
        friend EmscriptenApplication;

        explicit MouseEvent(const EmscriptenMouseEvent& event): _event(event) {}

        const EmscriptenMouseEvent& _event;
};

/**
@brief Mouse move event

@see @ref MouseEvent, @ref mouseMoveEvent()
*/
class EmscriptenApplication::MouseMoveEvent: public EmscriptenApplication::InputEvent {
    public:
        /**
         * @brief Mouse button
         *
         * @see @ref buttons()
         */
        enum class Button: Int {
            /** Left mouse button */
            Left = 1 << 0,

            /** Middle mouse button */
            Middle = 1 << 1,

            /** Right mouse button */
            Right = 1 << 2
        };

        /**
         * @brief Set of mouse buttons
         *
         * @see @ref buttons()
         */
        typedef Containers::EnumSet<Button> Buttons;

        /** @brief Position */
        Vector2i position() const;

        /**
         * @brief Relative position
         *
         * Position relative to previous move event. Unlike
         * @ref Sdl2Application, HTML APIs don't provide relative position
         * directly, so this is calculated explicitly as a delta from previous
         * move event position.
         */
        Vector2i relativePosition() const { return _relativePosition; }

        /** @brief Mouse buttons */
        Buttons buttons() const;

        /** @brief Modifiers */
        Modifiers modifiers() const;

        /** @brief Underlying Emscripten event */
        const EmscriptenMouseEvent& event() const { return _event; }

    private:
        friend EmscriptenApplication;

        explicit MouseMoveEvent(const EmscriptenMouseEvent& event, const Vector2i& relativePosition): _event(event), _relativePosition{relativePosition} {}

        const EmscriptenMouseEvent& _event;
        const Vector2i _relativePosition;
};

CORRADE_ENUMSET_OPERATORS(EmscriptenApplication::MouseMoveEvent::Buttons)

/**
@brief Mouse scroll event

@see @ref MouseEvent, @ref MouseMoveEvent, @ref mouseScrollEvent()
*/
class EmscriptenApplication::MouseScrollEvent: public EmscriptenApplication::InputEvent {
    public:
        /** @brief Scroll offset */
        Vector2 offset() const;

        /** @brief Position */
        Vector2i position() const;

        /** @brief Modifiers */
        Modifiers modifiers() const;

        /** @brief Underlying Emscripten event */
        const EmscriptenWheelEvent& event() const { return _event; }

    private:
        friend EmscriptenApplication;

        explicit MouseScrollEvent(const EmscriptenWheelEvent& event): _event(event) {}

        const EmscriptenWheelEvent& _event;
};

/**
@brief Key event

@see @ref keyPressEvent(), @ref keyReleaseEvent()
*/
class EmscriptenApplication::KeyEvent: public EmscriptenApplication::InputEvent {
    public:
        /**
         * @brief Key
         *
         * @see @ref key()
         */
        enum class Key: Int {
            Unknown,         /**< Unknown key */

            /**
             * Left Shift
             *
             * @see @ref InputEvent::Modifier::Shift
             */
            LeftShift,

            /**
             * Right Shift
             *
             * @see @ref InputEvent::Modifier::Shift
             */
            RightShift,

            /**
             * Left Ctrl
             *
             * @see @ref InputEvent::Modifier::Ctrl
             */
            LeftCtrl,

            /**
             * Right Ctrl
             *
             * @see @ref InputEvent::Modifier::Ctrl
             */
            RightCtrl,

            /**
             * Left Alt
             *
             * @see @ref InputEvent::Modifier::Alt
             */
            LeftAlt,

            /**
             * Right Alt
             *
             * @see @ref InputEvent::Modifier::Alt
             */
            RightAlt,

            /**
             * Left Super key (Windows/⌘)
             *
             * @see @ref InputEvent::Modifier::Super
             */
            LeftSuper,

            /**
             * Right Super key (Windows/⌘)
             *
             * @see @ref InputEvent::Modifier::Super
             */
            RightSuper,

            /* no equivalent for Sdl2Application's AltGr */

            Enter,              /**< Enter */
            Esc,                /**< Escape */

            Up,                 /**< Up arrow */
            Down,               /**< Down arrow */
            Left,               /**< Left arrow */
            Right,              /**< Right arrow */
            Home,               /**< Home */
            End,                /**< End */
            PageUp,             /**< Page up */
            PageDown,           /**< Page down */
            Backspace,          /**< Backspace */
            Insert,             /**< Insert */
            Delete,             /**< Delete */

            F1,                 /**< F1 */
            F2,                 /**< F2 */
            F3,                 /**< F3 */
            F4,                 /**< F4 */
            F5,                 /**< F5 */
            F6,                 /**< F6 */
            F7,                 /**< F7 */
            F8,                 /**< F8 */
            F9,                 /**< F9 */
            F10,                /**< F10 */
            F11,                /**< F11 */
            F12,                /**< F12 */

            Zero = '0',         /**< Zero */
            One,                /**< One */
            Two,                /**< Two */
            Three,              /**< Three */
            Four,               /**< Four */
            Five,               /**< Five */
            Six,                /**< Six */
            Seven,              /**< Seven */
            Eight,              /**< Eight */
            Nine,               /**< Nine */

            A = 'a',            /**< Letter A */
            B,                  /**< Letter B */
            C,                  /**< Letter C */
            D,                  /**< Letter D */
            E,                  /**< Letter E */
            F,                  /**< Letter F */
            G,                  /**< Letter G */
            H,                  /**< Letter H */
            I,                  /**< Letter I */
            J,                  /**< Letter J */
            K,                  /**< Letter K */
            L,                  /**< Letter L */
            M,                  /**< Letter M */
            N,                  /**< Letter N */
            O,                  /**< Letter O */
            P,                  /**< Letter P */
            Q,                  /**< Letter Q */
            R,                  /**< Letter R */
            S,                  /**< Letter S */
            T,                  /**< Letter T */
            U,                  /**< Letter U */
            V,                  /**< Letter V */
            W,                  /**< Letter W */
            X,                  /**< Letter X */
            Y,                  /**< Letter Y */
            Z,                  /**< Letter Z */

            Space,              /**< Space */
            Tab,                /**< Tab */
            Quote,              /**< Quote (<tt>'</tt>) */
            Comma,              /**< Comma */
            Period,             /**< Period */
            Minus,              /**< Minus */
            /* Note: This may only be represented as SHIFT + = */
            Plus,               /**< Plus */
            Slash,              /**< Slash */
            /* Note: This may only be represented as SHIFT + 5 */
            Percent,            /**< Percent */

            /**
             * Semicolon (`;`)
             * @m_since{2020,06}
             */
            Semicolon,

            Equal,              /**< Equal */
            LeftBracket,        /**< Left bracket (`[`) */
            RightBracket,       /**< Right bracket (`]`) */
            Backslash,          /**< Backslash (`\`) */
            Backquote,          /**< Backquote (<tt>`</tt>) */

            /* no equivalent for GlfwApplication's World1 / World2 */

            CapsLock,           /**< Caps lock */
            ScrollLock,         /**< Scroll lock */
            NumLock,            /**< Num lock */
            PrintScreen,        /**< Print screen */
            Pause,              /**< Pause */
            Menu,               /**< Menu */

            NumZero,            /**< Numpad zero */
            NumOne,             /**< Numpad one */
            NumTwo,             /**< Numpad two */
            NumThree,           /**< Numpad three */
            NumFour,            /**< Numpad four */
            NumFive,            /**< Numpad five */
            NumSix,             /**< Numpad six */
            NumSeven,           /**< Numpad seven */
            NumEight,           /**< Numpad eight */
            NumNine,            /**< Numpad nine */
            NumDecimal,         /**< Numpad decimal */
            NumDivide,          /**< Numpad divide */
            NumMultiply,        /**< Numpad multiply */
            NumSubtract,        /**< Numpad subtract */
            NumAdd,             /**< Numpad add */
            NumEnter,           /**< Numpad enter */
            NumEqual            /**< Numpad equal */
        };

        /**
         * @brief Key
         *
         * Note that the key is mapped from @m_class{m-doc-external}
         * [EmscriptenKeyboardEvent::code](https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenKeyboardEvent.code)
         * in all cases except A--Z, which are mapped from
         * @m_class{m-doc-external} [EmscriptenkeyboardEvent::key](https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenKeyboardEvent.key),
         * which respects the keyboard layout.
         */
        Key key() const;

        /** @brief Key name */
        std::string keyName() const;

        /** @brief Modifiers */
        Modifiers modifiers() const;

        /** @brief Underlying Emscripten event */
        const EmscriptenKeyboardEvent& event() const { return _event; }

    private:
        friend EmscriptenApplication;

        explicit KeyEvent(const EmscriptenKeyboardEvent& event): _event(event) {}

        const EmscriptenKeyboardEvent& _event;
};

/**
@brief Text input event

@see @ref textInputEvent()
*/
class EmscriptenApplication::TextInputEvent {
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

        /** @copydoc EmscriptenApplication::InputEvent::setAccepted() */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Input text in UTF-8 */
        Containers::ArrayView<const char> text() const { return _text; }

        /** @brief Underlying Emscripten event */
        const EmscriptenKeyboardEvent& event() const { return _event; }

    private:
        friend EmscriptenApplication;

        explicit TextInputEvent(const EmscriptenKeyboardEvent& event, Containers::ArrayView<const char> text): _event(event), _text{text}, _accepted{false} {}

        const EmscriptenKeyboardEvent& _event;
        const Containers::ArrayView<const char> _text;
        bool _accepted;
};

/** @hideinitializer
@brief Entry point for Emscripten applications
@param className Class name

@m_keywords{MAGNUM_APPLICATION_MAIN()}

See @ref Magnum::Platform::EmscriptenApplication "Platform::EmscriptenApplication"
for usage information. This macro abstracts out platform-specific entry point
code. See @ref portability-applications for more information.

When no other application header is included this macro is also aliased to
@cpp MAGNUM_APPLICATION_MAIN() @ce.

Compared to for example @ref MAGNUM_SDL2APPLICATION_MAIN(), the macro
instantiates the application instance as a global variable instead of a local
variable inside @cpp main() @ce. This is in order to support the
@ref Platform-EmscriptenApplication-browser-main-loop "idle-efficient main loop",
as otherwise the local scope would end before any event callback has a chance
to happen.
*/
#define MAGNUM_EMSCRIPTENAPPLICATION_MAIN(className)                        \
    namespace { Corrade::Containers::Pointer<className> emscriptenApplicationInstance ; } \
    int main(int argc, char** argv) {                                       \
        emscriptenApplicationInstance.reset(new className{{argc, argv}});   \
        return emscriptenApplicationInstance->exec();                       \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef EmscriptenApplication Application;
typedef BasicScreen<EmscriptenApplication> Screen;
typedef BasicScreenedApplication<EmscriptenApplication> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_EMSCRIPTENAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

}}
#else
#error this file is available only on Emscripten build
#endif

#endif
