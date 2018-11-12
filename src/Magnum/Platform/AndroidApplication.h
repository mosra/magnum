#ifndef Magnum_Platform_AndroidApplication_h
#define Magnum_Platform_AndroidApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#if defined(CORRADE_TARGET_ANDROID) || defined(DOXYGEN_GENERATING_OUTPUT)
/** @file
 * @brief Class @ref Magnum::Platform::AndroidApplication, macro @ref MAGNUM_ANDROIDAPPLICATION_MAIN()
 */
#endif

#include <memory>
#include <EGL/egl.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/GL/GL.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Platform/Platform.h"

#if defined(CORRADE_TARGET_ANDROID) || defined(DOXYGEN_GENERATING_OUTPUT)
#include <android/input.h>

/* Undef Xlib nonsense which might get pulled in by EGL */
#undef None

struct android_app;
struct ANativeActivity;

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief Android application

Application running on Android.

This application library is available only on
@ref CORRADE_TARGET_ANDROID "Android", see respective sections
in the @ref building-corrade-cross-android "Corrade" and
@ref building-cross-android "Magnum" building documentation. It is built if
`WITH_ANDROIDAPPLICATION` is enabled when building Magnum.

@section Platform-AndroidApplication-bootstrap Bootstrap application

Fully contained base application using @ref Sdl2Application for desktop build
and @ref AndroidApplication for Android build along with full Android packaging
stuff and CMake setup is available in `base-android` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-android.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-android.zip)
file. After extracting the downloaded archive, you can do the desktop build in
the same way as with @ref Sdl2Application.

In order to build the application, you need Gradle and Android build of Corrade
and Magnum. Gradle is usually able to download all SDK dependencies on its own
and then you can just build and install the app on a connected device or
emulator like this:

@code{.sh}
gradle build
gradle installDebug
@endcode

Detailed information about deployment for Android and all needed boilerplate
together with a troubleshooting guide is available in @ref platforms-android.

@section Platform-AndroidApplication-usage General usage

In order to use this library from CMake, you need to copy `FindEGL.cmake`
and `FindOpenGLES2.cmake` (or `FindOpenGLES3.cmake`) from the `modules/`
directory in Magnum source to the `modules/` dir in your project (so it is able
to find EGL and OpenGL ES libraries). Request the `AndroidApplication`
component of the `Magnum` package and link to the `Magnum::AndroidApplication`
target:

@code{.cmake}
find_package(Magnum REQUIRED)
if(CORRADE_TARGET_ANDROID)
    find_package(Magnum REQUIRED AndroidApplication)
endif()

# ...
if(CORRADE_TARGET_ANDROID)
    target_link_libraries(your-app Magnum::AndroidApplication)
endif()
@endcode

If no other application is requested, you can also use the generic
`Magnum::Application` alias to simplify porting. Again, see @ref building and
@ref cmake for more information. Note that unlike on other platforms you need
to create *shared library* instead of executable.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass must be then made accessible from JNI using
@ref MAGNUM_ANDROIDAPPLICATION_MAIN() macro. See @ref platform for more
information.

@code{.cpp}
class MyApplication: public Platform::AndroidApplication {
    // implement required methods...
};
MAGNUM_ANDROIDAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
@cpp Platform::Application @ce and the macro is aliased to @cpp MAGNUM_APPLICATION_MAIN() @ce
to simplify porting.

@section Platform-AndroidApplication-output-redirection Redirecting output to Android log buffer

The application by default redirects @ref Corrade::Utility::Debug "Debug",
@ref Corrade::Utility::Warning "Warning" and @ref Corrade::Utility::Error "Error"
output to Android log buffer with tag `"magnum"`, which can be then accessed
through `logcat` utility. See also @ref Corrade::Utility::AndroidLogStreamBuffer
for more information.
*/
class AndroidApplication {
    public:
        /** @brief Application arguments */
        typedef android_app* Arguments;

        class Configuration;
        class GLConfiguration;
        class ViewportEvent;
        class InputEvent;
        class MouseEvent;
        class MouseMoveEvent;

        /**
         * @brief Execute the application
         *
         * See @ref MAGNUM_ANDROIDAPPLICATION_MAIN() for usage information.
         */
        static void exec(android_app* state, std::unique_ptr<AndroidApplication>(*instancer)(const Arguments&));

        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class T> static std::unique_ptr<AndroidApplication> instancer(const Arguments& arguments) {
            return std::unique_ptr<AndroidApplication>{new T{arguments}};
        }
        #endif

        /**
         * @brief Construct with given configuration for OpenGL context
         * @param arguments         Application arguments
         * @param configuration     Application configuration
         * @param glConfiguration   OpenGL context configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref Configuration for more information. The program exits if
         * the context cannot be created, see @ref tryCreate() for an
         * alternative.
         */
        explicit AndroidApplication(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration);

        /**
         * @brief Construct with given configuration
         *
         * Equivalent to calling @ref AndroidApplication(const Arguments&, const Configuration&, const GLConfiguration&)
         * with default-constructed @ref GLConfiguration.
         */
        explicit AndroidApplication(const Arguments& arguments, const Configuration& configuration);

        /**
         * @brief Construct with default configuration
         *
         * Equivalent to calling @ref AndroidApplication(const Arguments&, const Configuration&)
         * with default-constructed @ref Configuration.
         */
        explicit AndroidApplication(const Arguments& arguments);

        /**
         * @brief Construct without creating a window
         * @param arguments     Application arguments
         *
         * Unlike above, the window is not created and must be created later
         * with @ref create() or @ref tryCreate().
         */
        explicit AndroidApplication(const Arguments& arguments, NoCreateT);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief @copybrief AndroidApplication(const Arguments&, NoCreateT)
         * @deprecated Use @ref AndroidApplication(const Arguments&, NoCreateT) instead.
         */
        CORRADE_DEPRECATED("use AndroidApplication(const Arguments&, NoCreateT) instead") explicit AndroidApplication(const Arguments& arguments, std::nullptr_t): AndroidApplication{arguments, NoCreate} {}
        #endif

        /** @brief Copying is not allowed */
        AndroidApplication(const AndroidApplication&) = delete;

        /** @brief Moving is not allowed */
        AndroidApplication(AndroidApplication&&) = delete;

        virtual ~AndroidApplication();

        /** @brief Copying is not allowed */
        AndroidApplication& operator=(const AndroidApplication&) = delete;

        /** @brief Moving is not allowed */
        AndroidApplication& operator=(AndroidApplication&&) = delete;

        /**
         * @brief Underlying native activity handle
         *
         * Use in case you need to call NDK functionality directly.
         */
        ANativeActivity* nativeActivity();

    protected:
        /**
         * @brief Create a window with given configuration for OpenGL context
         * @param configuration     Application configuration
         * @param glConfiguration   OpenGL context configuration
         *
         * Must be called only if the context wasn't created by the constructor
         * itself, i.e. when passing @ref NoCreate to it. Error message is
         * printed and the program exits if the context cannot be created, see
         * @ref tryCreate() for an alternative.
         */
        void create(const Configuration& configuration, const GLConfiguration& glConfiguration);

        /**
         * @brief Create a window with given configuration and OpenGL context
         *
         * Equivalent to calling @ref create(const Configuration&, const GLConfiguration&)
         * with default-constructed @ref GLConfiguration.
         */
        void create(const Configuration& configuration);

        /**
         * @brief Create a window with default configuration and OpenGL context
         *
         * Equivalent to calling @ref create(const Configuration&) with
         * default-constructed @ref Configuration.
         */
        void create();

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief create(const Configuration&, const GLConfiguration&)
         * @deprecated Use @ref create(const Configuration&, const GLConfiguration&) instead.
         */
        CORRADE_DEPRECATED("use create(const Configuration&, const GLConfiguration&) instead") void createContext(const Configuration& configuration) {
            create(configuration);
        }

        /** @brief @copybrief create()
         * @deprecated Use @ref create() instead.
         */
        CORRADE_DEPRECATED("use create() instead") void createContext() {
            create();
        }
        #endif

        /**
         * @brief Try to create context with given configuration for OpenGL context
         *
         * Unlike @ref create(const Configuration&, const GLConfiguration&)
         * returns @cpp false @ce if the context cannot be created,
         * @cpp true @ce otherwise.
         */
        bool tryCreate(const Configuration& configuration, const GLConfiguration& glConfiguration);

        /**
         * @brief Try to create context with given configuration and OpenGL context
         *
         * Unlike @ref create(const Configuration&) returns @cpp false @ce if
         * the context cannot be created, @cpp true @ce otherwise.
         */
        bool tryCreate(const Configuration& configuration);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief tryCreate(const Configuration&, const GLConfiguration&)
         * @deprecated Use @ref tryCreate(const Configuration&, const GLConfiguration&) instead.
         */
        CORRADE_DEPRECATED("use tryCreate(const Configuration&) instead") bool tryCreateContext(const Configuration& configuration) {
            return tryCreate(configuration);
        }
        #endif

        /** @{ @name Screen handling */

    public:
        /**
         * @brief Window size
         *
         * Window size to which all input event coordinates can be related.
         */
        Vector2i windowSize() const;

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers();

        /** @copydoc Sdl2Application::redraw() */
        void redraw() { _flags |= Flag::Redraw; }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @copydoc GlfwApplication::viewportEvent(ViewportEvent&) */
        virtual void viewportEvent(ViewportEvent& event);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copydoc GlfwApplication::viewportEvent(const Vector2i&) */
        virtual CORRADE_DEPRECATED("use viewportEvent(ViewportEvent&) instead") void viewportEvent(const Vector2i& size);
        #endif

        /** @copydoc Sdl2Application::drawEvent() */
        virtual void drawEvent() = 0;

        /*@}*/

        /** @{ @name Mouse handling */

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

        /*@}*/

    private:
        struct LogOutput;

        enum class Flag: UnsignedByte {
            Redraw = 1 << 0
        };
        typedef Containers::EnumSet<Flag> Flags;

        static void commandEvent(android_app* state, std::int32_t cmd);
        static std::int32_t inputEvent(android_app* state, AInputEvent* event);

        android_app* const _state;
        Flags _flags;

        EGLDisplay _display;
        EGLSurface _surface;
        EGLContext _glContext;

        std::unique_ptr<Platform::GLContext> _context;
        std::unique_ptr<LogOutput> _logOutput;

        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)
};

CORRADE_ENUMSET_OPERATORS(AndroidApplication::Flags)

/**
@brief OpenGL context configuration

Double-buffered RGBA canvas with depth and stencil buffers.
@see @ref AndroidApplication(), @ref Configuration, @ref create(),
    @ref tryCreate()
*/
class AndroidApplication::GLConfiguration {
    public:
        /*implicit*/ GLConfiguration();

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

    private:
        Vector4i _colorBufferSize;
        Int _depthBufferSize, _stencilBufferSize;
};

/**
@brief Configuration

Double-buffered RGBA canvas with depth and stencil buffers.
@see @ref AndroidApplication(), @ref GLConfiguration, @ref create(),
    @ref tryCreate()
*/
class AndroidApplication::Configuration {
    public:
        constexpr /*implicit*/ Configuration() {}

        /**
         * @brief Set window title
         * @return Reference to self (for method chaining)
         *
         * @note This function does nothing and is included only for
         *      compatibility with other toolkits. You need to set the title
         *      separately in the `AndroidManifest.xml` file.
         */
        template<class T> Configuration& setTitle(const T&) { return *this; }

        /** @brief Window size */
        Vector2i size() const { return _size; }

        /**
         * @brief Set window size
         * @return Reference to self (for method chaining)
         *
         * Default is @cpp {0, 0} @ce, which means that the size of the
         * physical window will be used. If set to different value than the
         * physical size, the surface will be scaled.
         */
        Configuration& setSize(const Vector2i& size) {
            _size = size;
            return *this;
        }

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @brief @copybrief GLConfiguration::setVersion()
         * @deprecated Use @ref GLConfiguration::setVersion() instead.
         */
        CORRADE_DEPRECATED("use GLConfiguration::setVersion() instead") Configuration& setVersion(GL::Version) { return *this; }
        #endif

    private:
        Vector2i _size;
};

/**
@brief Viewport event

@see @ref viewportEvent()
*/
class AndroidApplication::ViewportEvent {
    public:
        /**
         * @brief Window size
         *
         * @see @ref AndroidApplication::windowSize()
         */
        Vector2i windowSize() const { return _windowSize; }

        /**
         * @brief Framebuffer size
         *
         * The same as @ref windowSize().
         * @todo this might not be true, implement properly!
         */
        Vector2i framebufferSize() const { return _windowSize; }

    private:
        friend AndroidApplication;

        explicit ViewportEvent(const Vector2i& windowSize): _windowSize{windowSize} {}

        Vector2i _windowSize;
};

/**
@brief Base for input events

@see @ref MouseEvent, @ref MouseMoveEvent, @ref mousePressEvent(),
    @ref mouseReleaseEvent(), @ref mouseMoveEvent()
*/
class AndroidApplication::InputEvent {
    public:
        /** @brief Copying is not allowed */
        InputEvent(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent(InputEvent&&) = delete;

        /** @brief Copying is not allowed */
        InputEvent& operator=(const InputEvent&) = delete;

        /** @brief Moving is not allowed */
        InputEvent& operator=(InputEvent&&) = delete;

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it will be
         * propagated elsewhere, for example to the Android system or to
         * another screen when using @ref BasicScreenedApplication "ScreenedApplication".
         * By default is each event ignored and thus propagated.
         */
        void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Whether the event is accepted */
        bool isAccepted() const { return _accepted; }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        explicit InputEvent(AInputEvent* event): _event(event), _accepted(false) {}

        ~InputEvent() = default;

        AInputEvent* _event;
    #endif

    private:
        bool _accepted;
};

/**
@brief Mouse event

@see @ref MouseMoveEvent, @ref mousePressEvent(), @ref mouseReleaseEvent()
*/
class AndroidApplication::MouseEvent: public InputEvent {
    friend AndroidApplication;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref button()
         */
        enum class Button: std::int32_t {
            /** No button was pressed (touch or stylus event) */
            None = 0,

            /**
             * Left mouse button. Note that this button is not set if only
             * touch or stylus event occured.
             * @attention Available since Android 4.0 (API level 14), not
             *      detectable in earlier versions.
             */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || __ANDROID_API__ >= 14
            Left = AMOTION_EVENT_BUTTON_PRIMARY,
            #else
            Left = 1 << 0,
            #endif

            /**
             * Middle mouse button or second stylus button
             * @attention Available since Android 4.0 (API level 14), not
             *      detectable in earlier versions.
             */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || __ANDROID_API__ >= 14
            Middle = AMOTION_EVENT_BUTTON_TERTIARY,
            #else
            Middle = 1 << 1,
            #endif

            /**
             * Right mouse button or first stylus button
             * @attention Available since Android 4.0 (API level 14), not
             *      detectable in earlier versions.
             */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || __ANDROID_API__ >= 14
            Right = AMOTION_EVENT_BUTTON_SECONDARY
            #else
            Right = 1 << 2
            #endif
        };

        /** @brief Button */
        Button button() {
            #if __ANDROID_API__ >= 14
            return Button(AMotionEvent_getButtonState(_event));
            #else
            return Button::None;
            #endif
        }

        /** @brief Position */
        Vector2i position() {
            return {Int(AMotionEvent_getX(_event, 0)),
                    Int(AMotionEvent_getY(_event, 0))};
        }

    private:
        explicit MouseEvent(AInputEvent* event): InputEvent(event) {}
};

/**
@brief Mouse move event

@see @ref MouseEvent, @ref mouseMoveEvent()
*/
class AndroidApplication::MouseMoveEvent: public InputEvent {
    friend AndroidApplication;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref buttons()
         */
        enum class Button: std::int32_t {
            /**
             * Left mouse button. Note that this button is not set if only
             * touch or stylus event occured.
             * @attention Available since Android 4.0 (API level 14), not
             *      detectable in earlier versions.
             */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || __ANDROID_API__ >= 14
            Left = AMOTION_EVENT_BUTTON_PRIMARY,
            #else
            Left = 1 << 0,
            #endif

            /**
             * Middle mouse button or second stylus button
             * @attention Available since Android 4.0 (API level 14), not
             *      detectable in earlier versions.
             */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || __ANDROID_API__ >= 14
            Middle = AMOTION_EVENT_BUTTON_TERTIARY,
            #else
            Middle = 1 << 1,
            #endif

            /**
             * Right mouse button or first stylus button
             * @attention Available since Android 4.0 (API level 14), not
             *      detectable in earlier versions.
             */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || __ANDROID_API__ >= 14
            Right = AMOTION_EVENT_BUTTON_SECONDARY
            #else
            Right = 1 << 2
            #endif
        };

        /**
         * @brief Set of mouse buttons
         *
         * @see @ref buttons()
         */
        typedef Containers::EnumSet<Button> Buttons;

        /** @brief Position */
        Vector2i position() const {
            return {Int(AMotionEvent_getX(_event, 0)),
                    Int(AMotionEvent_getY(_event, 0))};
        }

        /** @brief Mouse buttons */
        Buttons buttons() const {
            #if __ANDROID_API__ >= 14
            return Button(AMotionEvent_getButtonState(_event));
            #else
            return {};
            #endif
        }

    private:
        explicit MouseMoveEvent(AInputEvent* event): InputEvent(event) {}
};

CORRADE_ENUMSET_OPERATORS(AndroidApplication::MouseMoveEvent::Buttons)

/** @hideinitializer
@brief Entry point for Android applications
@param className Class name

See @ref Magnum::Platform::AndroidApplication "Platform::AndroidApplication"
for usage information. This macro abstracts out platform-specific entry point
code (the classic @cpp main() @ce function cannot be used in Android). See
@ref portability-applications for more information. When no other application
header is included this macro is also aliased to @cpp MAGNUM_APPLICATION_MAIN() @ce.
*/
#define MAGNUM_ANDROIDAPPLICATION_MAIN(className)                           \
    extern "C" CORRADE_VISIBILITY_EXPORT void android_main(android_app* state); \
    extern "C" void android_main(android_app* state) {                      \
        Magnum::Platform::AndroidApplication::exec(state,                   \
            Magnum::Platform::AndroidApplication::instancer<className>);    \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef AndroidApplication Application;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_ANDROIDAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

}}
#else
#error this file is available only on Android build
#endif

#endif
