#ifndef Magnum_Platform_EmscriptenApplication_h
#define Magnum_Platform_EmscriptenApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2018, 2019 Jonathan Hale <squareys@googlemail.com>

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
#endif

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief Emscripten application

Application running on Emscripten.

This application library is available only on
@ref CORRADE_TARGET_EMSCRIPTEN "Emscripten", see respective sections
in the @ref building-corrade-cross-emscripten "Corrade" and
@ref building-cross-emscripten "Magnum" building documentation. It is built if
`WITH_EMSCRIPTENAPPLICATION` is enabled when building Magnum.

@section Platform-EmscriptenApplication-bootstrap Bootstrap application

Fully contained base application using @ref EmscriptenApplication for Emscripten
build and CMake setup is available in `base-emscripten` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-emscripten.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-emscripten.zip)
file. After extracting the downloaded archive, you can build in
the same way as with @ref Sdl2Application.

Detailed information about deployment for Emscripten and all needed boilerplate
together with a troubleshooting guide is available in @ref platforms-emscripten.

@section Platform-EmscriptenApplication-usage General usage

Request the `EmscriptenApplication` component of the `Magnum` package and link
to the `Magnum::EmscriptenApplication` target:

@code{.cmake}
find_package(Magnum REQUIRED)
if(CORRADE_TARGET_EMSCRIPTEN)
    find_package(Magnum REQUIRED EmscriptenApplication)
endif()

# ...
if(CORRADE_TARGET_EMSCRIPTEN)
    target_link_libraries(your-app Magnum::EmscriptenApplication)
endif()
@endcode

If no other application is requested, you can also use the generic
`Magnum::Application` alias to simplify porting. Again, see @ref building and
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
@cpp Platform::Application @ce and the macro is aliased to @cpp MAGNUM_APPLICATION_MAIN() @ce
to simplify porting.
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

        /**
         * @brief Execute main loop
         *
         * Calls @ref mainLoopIteration() in a loop until @ref exit() is
         * called. See @ref MAGNUM_EMSCRIPTENAPPLICATION_MAIN() for usage
         * information.
         */
        void exec();

        /**
         * @brief Exit application main loop
         * @param exitCode Exit code for compatibility with other application
         *                 implementations
         *
         * Stops main loop started by @ref exec().
         */
        void exit(int exitCode = 0);

        /**
         * @brief Run one iteration of application main loop
         *
         * Called internally from @ref exec(). If you want to have better
         * control over how the main loop behaves, you can call this function
         * yourself from your own `main()` function instead of it being called
         * automatically from @ref exec() / @ref MAGNUM_EMSCRIPTENAPPLICATION_MAIN().
         */
        void mainLoopIteration();

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

    protected:
        /* Nobody will need to have (and delete) EmscriptenApplication*, thus
           this is faster than public pure virtual destructor */
        ~EmscriptenApplication();

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
         * Note that this method is named "windowSize" to be API compatible with
         * Application implementations on other platforms.
         *
         * Window size to which all input event coordinates can be related.
         */
        Vector2i windowSize() const;

        /**
         * @brief DPI scaling
         *
         * How the content should be scaled relative to system defaults for
         * given @ref windowSize(). If a window is not created yet, returns
         * zero vector, use @ref dpiScaling(const Configuration&) const for
         * calculating a value independently. See @ref Platform-Sdl2Application-dpi
         * for more information.
         * @see @ref Sdl2Application::dpiScaling(), @ref framebufferSize()
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

        /**
         * @brief DPI scaling for given configuration
         *
         * Calculates DPI scaling that would be used when creating a window
         * with given @p configuration. Takes into account DPI scaling policy
         * and custom scaling specified on the command-line. See
         * @ref Platform-Sdl2Application-dpi for more information.
         */
        Vector2 dpiScaling(const Configuration& configuration) const;

        #if defined(MAGNUM_TARGET_GL) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Framebuffer size
         *
         * Always the same as @ref windowSize() on
         * @ref CORRADE_TARGET_EMSCRIPTEN "Emscripten". See
         * @ref Platform-Sdl2Application-dpi for more information.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         *
         * @see @ref Sdl2Application::framebufferSize()
         */
        Vector2i framebufferSize() const { return windowSize(); }
        #endif

    protected:
        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers();

        /** @copydoc Sdl2Application::redraw() */
        void redraw() { _flags |= Flag::Redraw; }

    private:
        /** @copydoc GlfwApplication::viewportEvent(ViewportEvent&) */
        virtual void viewportEvent(ViewportEvent& event);

        /** @copydoc Sdl2Application::drawEvent() */
        virtual void drawEvent() = 0;

        /*@}*/

        /** @{ @name Keyboard handling */

        /** @copydoc Sdl2Application::keyPressEvent() */
        virtual void keyPressEvent(KeyEvent& event);

        /** @copydoc Sdl2Application::keyReleaseEvent() */
        virtual void keyReleaseEvent(KeyEvent& event);

        /*@}*/

        /** @{ @name Mouse handling */

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
        bool isTextInputActive() const {
            return !!(_flags & Flag::TextInputActive);
        }

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

        /*@}*/

    private:
        enum class Flag: UnsignedByte {
            Redraw = 1 << 0,
            Resizable = 1 << 1,
            TextInputActive = 1 << 2,
        };
        typedef Containers::EnumSet<Flag> Flags;

        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)

        void setupCallbacks();

        Vector2 _dpiScaling;
        Vector2i _lastKnownCanvasSize;

        Flags _flags;

        #ifdef MAGNUM_TARGET_GL
        Containers::Pointer<Platform::GLContext> _context;
        #endif

        /* These are saved from command-line arguments */
        bool _verboseLog{};
        Vector2 _commandLineDpiScaling;
};

CORRADE_ENUMSET_OPERATORS(EmscriptenApplication::Flags)

/**
@brief WebGL context configuration

Double-buffered RGBA canvas with depth and stencil buffers.
@see @ref EmscriptenApplication(), @ref Configuration, @ref create(),
    @ref tryCreate()
*/
class EmscriptenApplication::GLConfiguration {
    public:
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref setFlags(), @ref Context::Flag
         * @requires_gles Context flags are not available in WebGL.
         */
        enum class Flag: Int {
            /**
             * Premultiplied alpha
             *
             * If set, the alpha channel of the rendering context will be
             * treated as representing premultiplied alpha values. If not set, the
             * alpha channel represents non-premultiplied alpha.
             */
            PremultipliedAlpha = 1 << 0,

            /**
             * Preserve drawing buffer
             *
             * If set, the contents of the drawing buffer are preserved between
             * consecutive @ref EmscriptenApplication::drawEvent() calls. If not,
             * color, depth and stencil are cleared at before
             * @ref EmscriptenApplication::drawEvent().
             * Not setting this gives better performance.
             */
            PreserveDrawingBuffer = 1 << 1,

            /**
             * Prefer low power to high performance
             *
             * If set, the WebGL power preference will be set to reduce power
             * consumption.
             */
            PreferLowPowerToHighPerformance = 1 << 2,

            /**
             * Fail if major performance caveat
             *
             * If set, requests context creation to abort if the browser is
             * only able to create a context that does not give good hardware-
             * accelerated performance.
             */
            FailIfMajorPerformanceCaveat = 1 << 3,

            /**
             * Explicit swap control
             *
             * For more details, see the
             * [Emscripten API reference](https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes.explicitSwapControl)
             * for more details.
             */
            ExplicitSwapControl = 1 << 4,

            /**
             * Enable WebGL extensions by default
             *
             * For more details, see the
             * [Emscripten API reference](https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes.enableExtensionsByDefault)
             * for more details.
             */
            EnableExtensionsByDefault = 1 << 5,

            /**
             * Render via offscreen back buffer
             *
             * For more details, see the
             * [Emscripten API reference](https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes.renderViaOffscreenBackBuffer)
             * for more details.
             */
            RenderViaOffscreenBackBuffer = 1 << 6,

            /**
             * Proxy content to main thread
             *
             * For more details, see the
             * [Emscripten API reference](https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes.proxyContextToMainThread)
             * for more details.
             */
            ProxyContextToMainThread = 1 << 7
        };

        /**
         * @brief Context flags
         *
         * @see @ref setFlags(), @ref Context::Flags
         */
        typedef Containers::EnumSet<Flag> Flags;

        /*implicit*/ GLConfiguration();

        /**
         * @brief Context flags
         */
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
         * Default is @cpp {8, 8, 8, 0} @ce (8-bit-per-channel RGB, no alpha).
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

/**
@brief Configuration

Double-buffered RGBA canvas with depth and stencil buffers.
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

            Resizable = 1 << 1
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
         *      compatibility with other toolkits. You need to set the title
         *      separately in the `EmscriptenManifest.xml` file.
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
         * or canvas size is autodetected. See @ref Platform-Sdl2Application-dpi
         * for more information.
         * When @p dpiScaling is not a zero vector, this function sets the DPI
         * scaling directly. The resulting @ref EmscriptenApplication::windowSize()
         * is @cpp size*dpiScaling @ce and @ref EmscriptenApplication::dpiScaling()
         * is @p dpiScaling.
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
         * The `--magnum-dpi-scaling` command-line option has a priority
         * over any application-set value.
         * @see @ref setSize(const Vector2i&, const Vector2&)
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

        /** @brief Window flags */
        WindowFlags windowFlags() const {
            return _windowFlags;
        }

        /**
         * @brief Set window flags
         * @return  Reference to self (for method chaining)
         *
         * Default is @ref WindowFlag::Focused.
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
         * Note that this method is named "windowSize" to be API compatible with
         * Application implementations on other platforms.
         *
         * Equivalent to @ref framebufferSize(). See @ref Platform-Sdl2Application-dpi
         * for more information.
         * @see @ref EmscriptenApplication::windowSize()
         */
        Vector2i windowSize() const { return _windowSize; }

        #if defined(MAGNUM_TARGET_GL) || defined(DOXYGEN_GENERATING_OUTPUT)
        /**
         * @brief Framebuffer size
         *
         * Equivalent to @ref windowSize(). See
         * @ref Platform-Sdl2Application-dpi for more information.
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
         * On some platforms moving an app between displays can result in DPI
         * scaling value being changed in tandem with a canvas/framebuffer
         * size. Simply resizing a canvas doesn't change the DPI scaling value.
         * See @ref Platform-Sdl2Application-dpi for more information.
         * @see @ref EmscriptenApplication::dpiScaling()
         */
        Vector2 dpiScaling() const { return _dpiScaling; }

    private:
        friend EmscriptenApplication;

        explicit ViewportEvent(
            const Vector2i& windowSize,
            #ifdef MAGNUM_TARGET_GL
            const Vector2i& framebufferSize,
            #endif
            const Vector2& dpiScaling):
                _windowSize{windowSize},
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

    private:
        friend EmscriptenApplication;

        explicit MouseEvent(const EmscriptenMouseEvent* event): _event(event) {}

        const EmscriptenMouseEvent* const _event;
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

        /** @brief Mouse buttons */
        Buttons buttons() const;

        /** @brief Modifiers */
        Modifiers modifiers() const;

    private:
        friend EmscriptenApplication;

        explicit MouseMoveEvent(const EmscriptenMouseEvent* event): _event{event} {}

        const EmscriptenMouseEvent* const _event;
};

CORRADE_ENUMSET_OPERATORS(EmscriptenApplication::MouseMoveEvent::Buttons)

/**
@brief Mouse scroll event

@see @ref MouseEvent, @ref MouseMoveEvent, @ref mouseScrollEvent()
*/
class EmscriptenApplication::MouseScrollEvent: public EmscriptenApplication::InputEvent {
    public:

        /**
         * @brief Scroll offset
         *
         * Offset in the unit described by @ref MouseScrollEvent::deltaMode().
         */
        Vector2 offset() const;

        /** @brief Position */
        Vector2i position() const;

        /** @brief Modifiers */
        Modifiers modifiers() const;

    private:
        friend EmscriptenApplication;

        explicit MouseScrollEvent(const EmscriptenWheelEvent* event): _event{event} {}

        const EmscriptenWheelEvent* const _event;
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

            Enter,          /**< Enter */
            Esc,            /**< Escape */

            Up,             /**< Up arrow */
            Down,           /**< Down arrow */
            Left,           /**< Left arrow */
            Right,          /**< Right arrow */
            Home,           /**< Home */
            End,            /**< End */
            PageUp,         /**< Page up */
            PageDown,       /**< Page down */
            Backspace,      /**< Backspace */
            Insert,         /**< Insert */
            Delete,         /**< Delete */

            F1,             /**< F1 */
            F2,             /**< F2 */
            F3,             /**< F3 */
            F4,             /**< F4 */
            F5,             /**< F5 */
            F6,             /**< F6 */
            F7,             /**< F7 */
            F8,             /**< F8 */
            F9,             /**< F9 */
            F10,            /**< F10 */
            F11,            /**< F11 */
            F12,            /**< F12 */

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

            Space,          /**< Space */
            Tab,            /**< Tab */
            Comma,          /**< Comma */
            Period,         /**< Period */
            Minus,          /**< Minus */
            /* Note: This may only be represented as SHIFT + = */
            Plus,               /**< Plus */
            Slash,              /**< Slash */
            /* Note: This may only be represented as SHIFT + 5 */
            Percent,            /**< Percent */
            Equal,              /**< Equal */

            Backquote,          /**< Backquote */
            Backslash,          /**< Backslash */
            Quote,              /**< Quote */
            BracketRight,       /**< BracketRight */
            BracketLeft,        /**< BracketLeft */

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

        /** @brief Key
         *
         * Note that the key is mapped from `EmscriptenKeyboardEvent::code` in
         * all cases except A-Z, which are mapped from `EmscriptenkeyboardEvent::key`,
         * which respects the keyboard layout.
         */
        Key key() const;

        /** @brief Key name */
        std::string keyName() const;

        /** @brief Modifiers */
        Modifiers modifiers() const;

    private:
        friend EmscriptenApplication;

        explicit KeyEvent(const EmscriptenKeyboardEvent* event): _event{event} {}

        const EmscriptenKeyboardEvent* const _event;
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
        Containers::ArrayView<const char> text() const { return _text; }

    private:
        friend EmscriptenApplication;

        explicit TextInputEvent(Containers::ArrayView<const char> text): _text{text}, _accepted{false} {}

        const Containers::ArrayView<const char> _text;
        bool _accepted;
};

/** @hideinitializer
@brief Entry point for Emscripten applications
@param className Class name

See @ref Magnum::Platform::EmscriptenApplication "Platform::EmscriptenApplication"
for usage information. This macro abstracts out platform-specific entry point
code. See
@ref portability-applications for more information.

@code{.cpp}
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode

When no other application header is included this macro is also aliased to
@cpp MAGNUM_APPLICATION_MAIN() @ce.
*/
#define MAGNUM_EMSCRIPTENAPPLICATION_MAIN(className)                        \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        app.exec();                                                         \
        return 0;                                                           \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef EmscriptenApplication Application;
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
