#ifndef Magnum_Platform_AndroidApplication_h
#define Magnum_Platform_AndroidApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2021 nodoteve <nodoteve@yandex.com>

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

#include <EGL/egl.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Math/Vector4.h"
#include "Magnum/Platform/Platform.h"
#include "Magnum/Platform/GLContext.h"

#if defined(CORRADE_TARGET_ANDROID) || defined(DOXYGEN_GENERATING_OUTPUT)
#include <android/input.h>

/* Undef Xlib nonsense which might get pulled in by EGL */
#undef None

struct android_app;
struct ANativeActivity;

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief Android application

@m_keywords{Application}

Application running on Android. Available only on
@ref CORRADE_TARGET_ANDROID "Android", see respective sections
in the @ref building-corrade-cross-android "Corrade" and
@ref building-cross-android "Magnum" building documentation.

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

This application library is built if `MAGNUM_WITH_ANDROIDAPPLICATION` is
enabled when building Magnum. To use this library with CMake, request the
`AndroidApplication` component of the `Magnum` package and link to the
`Magnum::AndroidApplication` target:

@code{.cmake}
find_package(Magnum REQUIRED)
if(CORRADE_TARGET_ANDROID)
    find_package(Magnum REQUIRED AndroidApplication)
endif()

# ...
if(CORRADE_TARGET_ANDROID)
    target_link_libraries(your-app PRIVATE Magnum::AndroidApplication)
endif()
@endcode

Additionally, if you're using Magnum as a CMake subproject, do the following
* *before* calling @cmake find_package() @ce to ensure it's enabled, as the
library is not built by default:

@code{.cmake}
set(MAGNUM_WITH_ANDROIDAPPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

If no other application is requested, you can also use the generic
`Magnum::Application` alias to simplify porting. Again, see @ref building and
@ref cmake for more information. Note that unlike on other platforms you need
to create a *shared library* instead of an executable:

@code{.cmake}
if(NOT CORRADE_TARGET_ANDROID)
    add_executable(your-app ...)
else()
    add_library(your-app SHARED ...)
endif()
@endcode

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

@section Platform-AndroidApplication-resizing Responding to viewport events

Unlike in desktop application implementations, where this is controlled via
@ref Sdl2Application::Configuration::WindowFlag::Resizable, for example, on
Android you have to describe this in the `AndroidManifest.xml` file, as by
default the application gets killed and relaunched on screen orientation
change. See the @ref platforms-android-apps-manifest-screen-resize "manifest file docs"
for more information.

@section Platform-AndroidApplication-output-redirection Redirecting output to Android log buffer

The application by default redirects @ref Corrade::Utility::Debug "Debug",
@ref Corrade::Utility::Warning "Warning" and @ref Corrade::Utility::Error "Error"
output to Android log buffer with tag `"magnum"`, which can be then accessed
through `logcat` utility. See also @ref Corrade::Utility::AndroidLogStreamBuffer
for more information.

@section Platform-AndroidApplication-touch Touch input on Android

The application recognizes touch and pen input and reports it as
@ref Pointer::Finger, @ref Pointer::Pen, @ref Pointer::Eraser with
@ref PointerEventSource::Touch and @ref PointerEventSource::Pen.

In case of a multi-touch scenario, @ref PointerEvent::isPrimary() /
@ref PointerMoveEvent::isPrimary() can be used to distinguish the primary touch
from secondary. For example, if an application doesn't need to recognize
gestures like pinch to zoom or rotate, it can ignore all non-primary pointer
events. @ref PointerEventSource::Mouse and @ref PointerEventSource::Pen events
are always marked as primary, for touch input the first pressed finger is
marked as primary and all following pressed fingers are non-primary. Note that
there can be up to one primary pointer for each pointer event source, e.g. a
finger, pen and a mouse press may all be marked as primary. On the other hand,
in a multi-touch scenario, if the first (and thus primary) finger is lifted, no
other finger becomes primary until all others are lifted as well. This is
consistent with the logic in @ref Sdl2Application and @ref EmscriptenApplication
but may not necessarily match what other Android applications do.

If gesture recognition is desirable, @ref PointerEvent::id() /
@ref PointerMoveEvent::id() contains a pointer ID that's unique among all
pointer event sources, which can be used to track movements of secondary,
tertiary and further touch points. The ID allocation isn't defined and you
can't rely on it to be contiguous or in any bounded range --- for example,
each new touch may generate a new ID that's only used until given finger is
lifted, and then never again, or the IDs may get heavily reused, being unique
only for the period given finger is pressed. For @ref PointerEventSource::Mouse
and @ref PointerEventSource::Pen  the ID is a constant, as there's always just
a single mouse cursor or a pen stylus.

See also @ref platform-windowed-pointer-events for general information about
handling pointer input in a portable way. There's also a
@ref Platform::TwoFingerGesture helper for recognition of common two-finger
gestures for zoom, rotation and pan.
*/
class AndroidApplication {
    public:
        /** @brief Application arguments */
        typedef android_app* Arguments;

        class Configuration;
        class GLConfiguration;
        class ViewportEvent;
        class InputEvent;
        class PointerEvent;
        class PointerMoveEvent;
        #ifdef MAGNUM_BUILD_DEPRECATED
        class MouseEvent;
        class MouseMoveEvent;
        #endif

        /* The damn thing cannot handle forward enum declarations */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        enum class PointerEventSource: UnsignedByte;
        enum class Pointer: UnsignedByte;
        #endif

        /**
         * @brief Set of pointer types
         * @m_since_latest
         *
         * @see @ref PointerMoveEvent::pointers(),
         *      @ref platform-windowed-pointer-events
         */
        typedef Containers::EnumSet<Pointer> Pointers;

        /**
         * @brief Execute the application
         *
         * See @ref MAGNUM_ANDROIDAPPLICATION_MAIN() for usage information.
         */
        static void exec(android_app* state, Containers::Pointer<AndroidApplication>(*instancer)(const Arguments&));

        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class T> static Containers::Pointer<AndroidApplication> instancer(const Arguments& arguments) {
            return Containers::Pointer<AndroidApplication>{new T{arguments}};
        }
        #endif

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
         */
        explicit AndroidApplication(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration);

        /**
         * @brief Construct without explicit GPU context configuration
         *
         * Equivalent to calling @ref AndroidApplication(const Arguments&, const Configuration&, const GLConfiguration&)
         * with default-constructed @ref GLConfiguration.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit AndroidApplication(const Arguments& arguments, const Configuration& configuration = Configuration{});
        #else
        explicit AndroidApplication(const Arguments& arguments, const Configuration& configuration);
        explicit AndroidApplication(const Arguments& arguments);
        #endif

        /**
         * @brief Construct without creating a window
         * @param arguments     Application arguments
         *
         * Unlike above, the window is not created and must be created later
         * with @ref create() or @ref tryCreate().
         */
        explicit AndroidApplication(const Arguments& arguments, NoCreateT);

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

        /** @{ @name Screen handling */

    public:
        /**
         * @brief Window size
         *
         * Window size to which all input event coordinates can be related.
         * Expects that a window is already created, equivalent to
         * @ref framebufferSize().
         *
         * @attention The reported value will be incorrect in case you use
         *      the [Screen Compatibility Mode](http://www.androiddocs.com/guide/practices/screen-compat-mode.html).
         *      See @ref platforms-android-apps-manifest-screen-compatibility-mode
         *      for details.
         */
        Vector2i windowSize() const { return framebufferSize(); }

        /**
         * @brief Framebuffer size
         *
         * Size of the default framebuffer, equivalent to @ref windowSize().
         * Expects that a window is already created.
         * @see @ref dpiScaling()
         */
        Vector2i framebufferSize() const;

        /**
         * @brief DPI scaling
         *
         * Provided only for compatibility with other toolkits. Returns always
         * @cpp {1.0f, 1.0f} @ce.
         * @see @ref framebufferSize()
         */
        Vector2 dpiScaling() const { return Vector2{1.0f}; }

        /**
         * @brief DPI scaling for given configuration
         *
         * Provided only for compatibility with other toolkits. Returns always
         * @cpp {1.0f, 1.0f} @ce.
         * @see @ref framebufferSize()
         */
        Vector2 dpiScaling(const Configuration& configuration) const {
            static_cast<void>(configuration);
            return Vector2{1.0f};
        }

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers();

        /** @copydoc Sdl2Application::redraw() */
        void redraw();

    private:
        /**
         * @brief Viewport event
         *
         * Called when window size changes, for example after device
         * orientation change. The default implementation does nothing. If you
         * want to respond to size changes, you should pass the new size to
         * @ref GL::DefaultFramebuffer::setViewport() (if using OpenGL) and
         * possibly elsewhere (to @ref SceneGraph::Camera::setViewport(), other
         * framebuffers...).
         *
         * @attention Android by default kills and fully recreates the
         *      application on device orientation change instead of calling the
         *      viewport event. To prevent that, you need to modify the
         *      `AndroidManifest.xml` file. See the
         *      @ref platforms-android-apps-manifest-screen-resize "manifest file docs"
         *      for more information.
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

        /** @{ @name Pointer handling */

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
         * @see @ref platform-windowed-pointer-events,
         *      @ref Platform-AndroidApplication-touch
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
         * @see @ref platform-windowed-pointer-events,
         *      @ref Platform-AndroidApplication-touch
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
         * @see @ref platform-windowed-pointer-events,
         *      @ref Platform-AndroidApplication-touch
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
        /* Not ApplicationScrollEventMixin, as this class doesn't have the
           deprecated mouseScrollEvent() */
        #endif

        struct LogOutput;

        enum class Flag: UnsignedByte;
        typedef Containers::EnumSet<Flag> Flags;

        static void commandEvent(android_app* state, std::int32_t cmd);
        static std::int32_t inputEvent(android_app* state, AInputEvent* event);

        android_app* const _state;
        Flags _flags;

        EGLDisplay _display;
        EGLSurface _surface;
        EGLContext _glContext;

        /* We have no way to query previous pointer positions, so we have to
           maintain them like this. For pointers capable of hover (mouse, pen)
           the _previousHoverPointerPosition is used, NaN signalling that the
           previous position is unknown. */
        Vector2 _previousHoverPointerPosition{Constants::nan()};
        /* For touches the _previousTouches array is used. The id is ~Int{} if
           given slot is unused, 32 "should be enough" and is consistent with
           what EmscriptenApplication does here. */
        struct {
            Int id = ~Int{};
            Vector2 position;
        } _previousTouches[32];
        Int _primaryFingerId = ~Int{};

        /* In order to know which mouse button was pressed / released in
           current event. Contains just the Mouse* values. */
        Pointers _previousPressedButtons;

        /* Has to be in an Optional because it gets explicitly destroyed before
           the GL context */
        Containers::Optional<Platform::GLContext> _context;
        Containers::Pointer<LogOutput> _logOutput;

        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)
};

/**
@brief Pointer event source
@m_since_latest

@see @ref PointerEvent::source(), @ref PointerMoveEvent::source(),
    @ref platform-windowed-pointer-events,
    @ref Platform-AndroidApplication-touch
*/
enum class AndroidApplication::PointerEventSource: UnsignedByte {
    /**
     * The event source is unknown. Corresponds to
     * `AMOTION_EVENT_TOOL_TYPE_UNKNOWN` and other types not listed below.
     * @see @ref Pointer::Unknown
     */
    Unknown,

    /**
     * The event is coming from a mouse. Corresponds to
     * `AMOTION_EVENT_TOOL_TYPE_MOUSE`.
     * @see @ref Pointer::MouseLeft, @ref Pointer::MouseMiddle,
     *      @ref Pointer::MouseRight
     */
    Mouse,

    /**
     * The event is coming from a touch contact, Corresponds to
     * `AMOTION_EVENT_TOOL_TYPE_FINGER`.
     * @see @ref Pointer::Finger
     */
    Touch,

    /**
     * The event is coming from a pen stylus. Corresponds to
     * `AMOTION_EVENT_TOOL_TYPE_STYLUS` and `AMOTION_EVENT_TOOL_TYPE_ERASER`.
     * @see @ref Pointer::Pen, @ref Pointer::Eraser
     */
    Pen
};

/**
@brief Pointer type
@m_since_latest

@see @ref Pointers, @ref PointerEvent::pointer(),
    @ref PointerMoveEvent::pointer(), @ref PointerMoveEvent::pointers(),
    @ref platform-windowed-pointer-events,
    @ref Platform-AndroidApplication-touch
*/
enum class AndroidApplication::Pointer: UnsignedByte {
    /**
     * Unknown. Corresponds to `AMOTION_EVENT_TOOL_TYPE_UNKNOWN` and other
     * types not listed below.
     * @see @ref PointerEventSource::Unknown
     */
    Unknown = 1 << 0,

    /**
     * Left mouse button. Corresponds to `AMOTION_EVENT_TOOL_TYPE_MOUSE` and
     * `AMOTION_EVENT_BUTTON_PRIMARY`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseLeft = 1 << 1,

    /**
     * Middle mouse button. Corresponds to `AMOTION_EVENT_TOOL_TYPE_MOUSE` and
     * `AMOTION_EVENT_BUTTON_SECONDARY`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseMiddle = 1 << 2,

    /**
     * Right mouse button. Corresponds to `AMOTION_EVENT_TOOL_TYPE_MOUSE` and
     * `AMOTION_EVENT_BUTTON_TERTIARY`.
     * @see @ref PointerEventSource::Mouse
     */
    MouseRight = 1 << 3,

    /** @todo AMOTION_EVENT_BUTTON_BACK, AMOTION_EVENT_BUTTON_FORWARD once it's
        possible to verify they match MouseButton4 / MouseButton5 in
        GlfwApplication and Sdl2Application */

    /**
     * Finger. Corresponds to `AMOTION_EVENT_TOOL_TYPE_FINGER`.
     * @see @ref PointerEventSource::Touch
     */
    Finger = 1 << 4,

    /** @todo There's AMOTION_EVENT_TOOL_TYPE_PALM, but no corresponding
        constant on the Java MotionEvent class, and all links to it broken.
        Accidental omission? Some scrapped feature with leftover traces? */

    /**
     * Pen. Corresponds to `AMOTION_EVENT_TOOL_TYPE_STYLUS`.
     * @see @ref PointerEventSource::Pen
     */
    Pen = 1 << 5,

    /** @todo There's AMOTION_EVENT_BUTTON_STYLUS_PRIMARY and
        AMOTION_EVENT_BUTTON_STYLUS_SECONDARY, expose once similar constants
        exist for EmscriptenApplication / Sdl3Application; implement chorded
        behavior for those like w/ mouse buttons */

    /**
     * Eraser. Corresponds to `AMOTION_EVENT_TOOL_TYPE_ERASER`.
     * @see @ref PointerEventSource::Pen
     */
    Eraser = 1 << 6
};

CORRADE_ENUMSET_OPERATORS(AndroidApplication::Pointers)

/**
@brief OpenGL context configuration

Double-buffered RGBA canvas with depth and stencil buffers.
@see @ref AndroidApplication(), @ref Configuration, @ref create(),
    @ref tryCreate()
*/
class AndroidApplication::GLConfiguration: public GL::Context::Configuration {
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

        /*implicit*/ GLConfiguration();

        /** @brief Context flags */
        Flags flags() const {
            return Flag(UnsignedLong(GL::Context::Configuration::flags()));
        }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is no flag. To avoid clearing default flags by accident,
         * prefer to use @ref addFlags() and @ref clearFlags() instead.
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

        /* Overloads to remove WTF-factor from method chaining order */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        MAGNUM_GL_CONTEXT_CONFIGURATION_SUBCLASS_IMPLEMENTATION(GLConfiguration)
        #endif

    private:
        Vector4i _colorBufferSize;
        Int _depthBufferSize, _stencilBufferSize;
};

/**
@brief Configuration

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

    private:
        Vector2i _size;
};

/**
@brief Viewport event

@see @ref viewportEvent(), @ref platform-windowed-viewport-events
*/
class AndroidApplication::ViewportEvent {
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
         * The same as @ref framebufferSize(). See
         * @ref AndroidApplication::windowSize() for possible caveats.
         */
        Vector2i windowSize() const { return _windowSize; }

        /**
         * @brief Framebuffer size
         *
         * The same as @ref windowSize(). See
         * @ref AndroidApplication::framebufferSize() for possible caveats.
         */
        Vector2i framebufferSize() const { return _windowSize; }

        /**
         * @brief DPI scaling
         *
         * Always @cpp {1.0f, 1.0f} @ce.
         */
        Vector2 dpiScaling() const { return Vector2{1.0f}; }

    private:
        friend AndroidApplication;

        explicit ViewportEvent(const Vector2i& windowSize): _windowSize{windowSize} {}

        const Vector2i _windowSize;
};

/**
@brief Base for input events

@see @ref PointerEvent, @ref PointerMoveEvent, @ref pointerPressEvent(),
    @ref pointerReleaseEvent(), @ref pointerMoveEvent()
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

        AInputEvent* const _event;
    #endif

    private:
        bool _accepted;
};

/**
@brief Pointer event
@m_since_latest

@see @ref PointerMoveEvent, @ref pointerPressEvent(),
    @ref pointerReleaseEvent(), @ref platform-windowed-pointer-events,
    @ref Platform-AndroidApplication-touch
*/
class AndroidApplication::PointerEvent: public InputEvent {
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
         * scenario. See @ref Platform-AndroidApplication-touch for more
         * information.
         */
        bool isPrimary() const { return _primary; }

        /**
         * @brief Pointer ID
         *
         * Useful to distinguish among multiple pointers in a multi-touch
         * scenario. See @ref Platform-AndroidApplication-touch for more
         * information.
         */
        /* Long is for consistency with Sdl2Application, Android uses just an
           Int */
        Long id() const { return _id; }

        /**
         * @brief Position
         *
         * May return fractional values if the touch hardware has sub-pixel
         * precision. Use @ref Math::round() to snap them to the nearest window
         * pixel.
         */
        Vector2 position() const {
            return {AMotionEvent_getX(_event, _i),
                    AMotionEvent_getY(_event, _i)};
        }

    private:
        friend AndroidApplication;

        explicit PointerEvent(AInputEvent* event, UnsignedByte i, PointerEventSource source, Pointer pointer, bool primary, Int id): InputEvent(event), _source{source}, _pointer{pointer}, _primary{primary}, _i{i}, _id{id} {}

        const PointerEventSource _source;
        const Pointer _pointer;
        const bool _primary;
        const UnsignedByte _i; /* Pointer index, not ID */
        const Int _id;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Mouse event
@m_deprecated_since_latest Use @ref PointerEvent, @ref pointerPressEvent() and
    @ref pointerReleaseEvent() instead, which is a better abstraction for
    covering both mouse and touch / pen input.

@see @ref MouseMoveEvent, @ref mousePressEvent(), @ref mouseReleaseEvent()
*/
class CORRADE_DEPRECATED("use PointerEvent, pointerPressEvent() and pointerReleaseEvent() instead") AndroidApplication::MouseEvent: public InputEvent {
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
             * touch or stylus event occurred.
             */
            Left = AMOTION_EVENT_BUTTON_PRIMARY,

            /** Middle mouse button or second stylus button */
            Middle = AMOTION_EVENT_BUTTON_TERTIARY,

            /** Right mouse button or first stylus button */
            Right = AMOTION_EVENT_BUTTON_SECONDARY
        };

        /** @brief Button */
        Button button() {
            return Button(AMotionEvent_getButtonState(_event));
        }

        /** @brief Position */
        Vector2i position() {
            return {Int(AMotionEvent_getX(_event, 0)),
                    Int(AMotionEvent_getY(_event, 0))};
        }

    private:
        explicit MouseEvent(AInputEvent* event): InputEvent(event) {}
};
#endif

/**
@brief Pointer move event
@m_since_latest

@see @ref PointerEvent, @ref pointerMoveEvent(),
    @ref platform-windowed-pointer-events,
    @ref Platform-AndroidApplication-touch
*/
class AndroidApplication::PointerMoveEvent: public InputEvent {
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
         * scenario. See @ref Platform-AndroidApplication-touch for more
         * information.
         */
        bool isPrimary() const { return _primary; }

        /**
         * @brief Pointer ID
         *
         * Useful to distinguish among multiple pointers in a multi-touch
         * scenario. See @ref Platform-AndroidApplication-touch for more
         * information.
         */
        /* Long is for consistency with Sdl2Application, Android uses just an
           Int */
        Long id() const { return _id; }

        /**
         * @brief Position
         *
         * May return fractional values if the touch hardware has sub-pixel
         * precision. Use @ref Math::round() to snap them to the nearest window
         * pixel.
         */
        Vector2 position() const {
            return {AMotionEvent_getX(_event, _i),
                    AMotionEvent_getY(_event, _i)};
        }

        /**
         * @brief Position relative to the previous touch event
         *
         * May return fractional values if the touch hardware has sub-pixel
         * precision. Use @ref Math::round() to snap them to the nearest window
         * pixel. Unlike @ref Sdl2Application, Android APIs don't provide
         * relative position directly, so this is calculated explicitly as a
         * delta from previous move event position.
         */
        Vector2 relativePosition() const { return _relativePosition; }

    private:
        friend AndroidApplication;

        explicit PointerMoveEvent(AInputEvent* event, UnsignedByte i, PointerEventSource source, Containers::Optional<Pointer> pointer, Pointers pointers, bool primary, Int id, const Vector2& relativePosition): InputEvent{event}, _source{source}, _pointer{pointer}, _pointers{pointers}, _primary{primary}, _i{i}, _id{id}, _relativePosition{relativePosition} {}

        const PointerEventSource _source;
        const Containers::Optional<Pointer> _pointer;
        const Pointers _pointers;
        const bool _primary;
        const UnsignedByte _i; /* Pointer index, not ID */
        const Int _id;
        const Vector2 _relativePosition;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Mouse move event
@m_deprecated_since_latest Use @ref PointerMoveEvent and
    @ref pointerMoveEvent() instead, which is a better abstraction for covering
    both mouse and touch / pen input.

@see @ref MouseEvent, @ref mouseMoveEvent()
*/
class CORRADE_DEPRECATED("use PointerMoveEvent and pointerMoveEvent() instead") AndroidApplication::MouseMoveEvent: public InputEvent {
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
             * touch or stylus event occurred.
             */
            Left = AMOTION_EVENT_BUTTON_PRIMARY,

            /** Middle mouse button or second stylus button */
            Middle = AMOTION_EVENT_BUTTON_TERTIARY,

            /** Right mouse button or first stylus button */
            Right = AMOTION_EVENT_BUTTON_SECONDARY
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

        /**
         * @brief Relative position
         * @m_since{2019,10}
         *
         * Position relative to previous move event. Unlike
         * @ref Sdl2Application, Android APIs don't provide relative position
         * directly, so this is calculated explicitly as a delta from previous
         * move event position.
         */
        Vector2i relativePosition() const { return _relativePosition; }

        /** @brief Mouse buttons */
        Buttons buttons() const {
            return Button(AMotionEvent_getButtonState(_event));
        }

    private:
        explicit MouseMoveEvent(AInputEvent* event, Vector2i relativePosition): InputEvent{event}, _relativePosition{relativePosition} {}

        const Vector2i _relativePosition;
};

CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_ENUMSET_OPERATORS(AndroidApplication::MouseMoveEvent::Buttons)
CORRADE_IGNORE_DEPRECATED_POP
#endif

/** @hideinitializer
@brief Entry point for Android applications
@param className Class name

@m_keywords{MAGNUM_APPLICATION_MAIN()}

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
typedef BasicScreen<AndroidApplication> Screen;
typedef BasicScreenedApplication<AndroidApplication> ScreenedApplication;
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
