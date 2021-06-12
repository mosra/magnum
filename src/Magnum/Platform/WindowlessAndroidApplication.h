#ifndef Magnum_Platform_WindowlessAndroidApplication_h
#define Magnum_Platform_WindowlessAndroidApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Platform::WindowlessAndroidApplication, macro @ref MAGNUM_WindowlessAndroidApplication_MAIN()
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
#include "MotionEventSerializer.h"

#if defined(CORRADE_TARGET_ANDROID) || defined(DOXYGEN_GENERATING_OUTPUT)
#include <android/input.h>
#include <android/keycodes.h>
#include <jni.h>
#include <android/native_window_jni.h>
#include <atomic>

/* Undef Xlib nonsense which might get pulled in by EGL */
#undef None

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief Android application
@m_keywords{Application}
Application running on Android. Available only on
@ref CORRADE_TARGET_ANDROID "Android", see respective sections
in the @ref building-corrade-cross-android "Corrade" and
@ref building-cross-android "Magnum" building documentation.
@section Platform-WindowlessAndroidApplication-bootstrap Bootstrap application
Fully contained base application using @ref Sdl2Application for desktop build
and @ref WindowlessAndroidApplication for Android build along with full Android packaging
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
@section Platform-WindowlessAndroidApplication-usage General usage
This application library is built if `WITH_WindowlessAndroidApplication` is enabled when
building Magnum. To use this library with CMake, put [FindEGL.cmake](https://github.com/mosra/magnum/blob/master/modules/FindEGL.cmake)
and [FindOpenGLES2.cmake](https://github.com/mosra/magnum/blob/master/modules/FindOpenGLES2.cmake) (or
[FindOpenGLES3.cmake](https://github.com/mosra/magnum/blob/master/modules/FindOpenGLES3.cmake))
into your `modules/` directory, request the `WindowlessAndroidApplication` component of
the `Magnum` package and link to the `Magnum::WindowlessAndroidApplication` target:
@code{.cmake}
find_package(Magnum REQUIRED)
if(CORRADE_TARGET_ANDROID)
    find_package(Magnum REQUIRED WindowlessAndroidApplication)
endif()
# ...
if(CORRADE_TARGET_ANDROID)
    target_link_libraries(your-app PRIVATE Magnum::WindowlessAndroidApplication)
endif()
@endcode
Additionally, if you're using Magnum as a CMake subproject, do the following
* *before* calling @cmake find_package() @ce to ensure it's enabled, as the
library is not built by default:
@code{.cmake}
set(WITH_WindowlessAndroidApplication ON CACHE BOOL "" FORCE)
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
@ref MAGNUM_WindowlessAndroidApplication_MAIN() macro. See @ref platform for more
information.
@code{.cpp}
class MyApplication: public Platform::WindowlessAndroidApplication {
    // implement required methods...
};
MAGNUM_WindowlessAndroidApplication_MAIN(MyApplication)
@endcode
If no other application header is included, this class is also aliased to
@cpp Platform::Application @ce and the macro is aliased to @cpp MAGNUM_APPLICATION_MAIN() @ce
to simplify porting.
@section Platform-WindowlessAndroidApplication-resizing Responding to viewport events
Unlike in desktop application implementations, where this is controlled via
@ref Sdl2Application::Configuration::WindowFlag::Resizable, for example, on
Android you have to describe this in the `AndroidManifest.xml` file, as by
default the application gets killed and relaunched on screen orientation
change. See the @ref platforms-android-apps-manifest-screen-resize "manifest file docs"
for more information.
@section Platform-WindowlessAndroidApplication-output-redirection Redirecting output to Android log buffer
The application by default redirects @ref Corrade::Utility::Debug "Debug",
@ref Corrade::Utility::Warning "Warning" and @ref Corrade::Utility::Error "Error"
output to Android log buffer with tag `"magnum"`, which can be then accessed
through `logcat` utility. See also @ref Corrade::Utility::AndroidLogStreamBuffer
for more information.
*/
class WindowlessAndroidApplication {
public:

    class Configuration;
    class GLConfiguration;
    class ViewportEvent;
    class InputEvent;
    class MouseEvent;
    class MouseMoveEvent;

    /**
     * @brief Construct without creating a window
     *
     * The window must be created later
     * with @ref tryCreate().
     */
    explicit WindowlessAndroidApplication() = default;
    explicit WindowlessAndroidApplication(JNIEnv *jenv, jobject classInstance, jstring name, jstring signature);
    ~WindowlessAndroidApplication();

    /** @brief Copying is not allowed */
    WindowlessAndroidApplication(const WindowlessAndroidApplication&) = delete;

    /** @brief Moving is not allowed */
    WindowlessAndroidApplication(WindowlessAndroidApplication&&) = delete;

    /** @brief Copying is not allowed */
    WindowlessAndroidApplication& operator=(const WindowlessAndroidApplication&) = delete;

    /** @brief Moving is not allowed */
    WindowlessAndroidApplication& operator=(WindowlessAndroidApplication&&) = delete;

    /** @{ @name Screen handling */

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

    /**
     * @brief Redraw immediately
     *
     * Marks the window for redrawing, resulting in call to @ref drawEvent()
     * in the next iteration. You can call it from @ref drawEvent() itself
     * to redraw immediately without waiting for user input.
     */
    void redraw();

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

    /**
     * @brief native onDraw
     *
     * calls drawEvent if it should be redrawn
     *
     * call this from java jni onDraw method/native
     */
    void onDraw();

    bool onTouchEvent(JNIEnv *jenv, jfloatArray motionEventData);
    void surfaceChanged(int w, int h);

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
     */
    virtual void viewportEvent(ViewportEvent& event);

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
    /**
     * @}
     */

    /** @{ @name Mouse handling */

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

    /* Since 1.8.17, the original short-hand group closing doesn't work
       anymore. FFS. */
    /**
     * @}
     */

    enum class Flag: UnsignedByte;
    typedef Containers::EnumSet<Flag> Flags;
    Flags _flags;
    int width, height;

    class JVM_MANAGER {
    public:
        JavaVM * jvm;
        JNIEnv* jenv;
        JavaVMAttachArgs jvmArgs;
        bool needsToDetach;

        JVM_MANAGER();
        bool getJVM(JNIEnv * env);
        bool attachJVM();
        void detachJVM();

        template <typename J, typename O> O globalRef(J j, O o) {
            return reinterpret_cast<O>(j->NewGlobalRef(o));
        }
    };


    JVM_MANAGER jvmManager;
    jobject jObject;
    jclass jClass;
    jmethodID jSwapBuffers;

    Vector2i _previousMouseMovePosition{-1};
    CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)
public:
    /* workaround for error "undefined reference to vtable for ..." */
    /**
     * @brief Context Container
     *
     * wraps @ref Containers::Optional<Platform::GLContext>
     * inside of a container to enable context creation and cleanup
     * as surface is created and destroyed.
     */
    struct ContextContainer {
        Containers::Optional<Platform::GLContext> context{InPlaceInit, NoCreate};
    };
    ContextContainer * magnum_context = nullptr;
};

/**
@brief Configuration

@see @ref WindowlessAndroidApplication(), @ref GLConfiguration, @ref create(),
    @ref tryCreate()
*/
class WindowlessAndroidApplication::Configuration {
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
@brief OpenGL context configuration

Double-buffered RGBA canvas with depth and stencil buffers.
@see @ref WindowlessAndroidApplication(), @ref Configuration, @ref create(),
    @ref tryCreate()
*/
class WindowlessAndroidApplication::GLConfiguration: public GL::Context::Configuration {
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
         * @see @ref setFlags(), @ref Context::Flags
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
@brief Viewport event
@see @ref viewportEvent()
*/
class WindowlessAndroidApplication::ViewportEvent {
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
         * @ref WindowlessAndroidApplication::windowSize() for possible caveats.
         */
        Vector2i windowSize() const { return _windowSize; }

        /**
         * @brief Framebuffer size
         *
         * The same as @ref windowSize(). See
         * @ref WindowlessAndroidApplication::framebufferSize() for possible caveats.
         */
        Vector2i framebufferSize() const { return _windowSize; }

        /**
         * @brief DPI scaling
         *
         * Always @cpp {1.0f, 1.0f} @ce.
         */
        Vector2 dpiScaling() const { return Vector2{1.0f}; }

    private:
        friend WindowlessAndroidApplication;

        explicit ViewportEvent(const Vector2i& windowSize): _windowSize{windowSize} {}

        const Vector2i _windowSize;
};

/**
@brief Base for input events
@see @ref MouseEvent, @ref MouseMoveEvent, @ref mousePressEvent(),
    @ref mouseReleaseEvent(), @ref mouseMoveEvent()
*/
class WindowlessAndroidApplication::InputEvent {
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
        explicit InputEvent(MotionEventSerializer & event): _event(event), _accepted(false) {}

        ~InputEvent() = default;

        MotionEventSerializer & _event;
    #endif

    private:
        bool _accepted;
};

/**
@brief Mouse event
@see @ref MouseMoveEvent, @ref mousePressEvent(), @ref mouseReleaseEvent()
*/
class WindowlessAndroidApplication::MouseEvent: public InputEvent {
    friend WindowlessAndroidApplication;

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
             * Left mouse button. Note that this button is set for
             * compatibility with Desktop
             */
            Left = MotionEventSerializer::MOTION_EVENT_BUTTON_PRIMARY,

            /**
             * Middle mouse button or second stylus button
             * @attention Available since Android 4.0 (API level 14), not
             *      detectable in earlier versions.
             */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || __ANDROID_API__ >= 14
            Middle = MotionEventSerializer::MOTION_EVENT_BUTTON_TERTIARY,
            #else
            Middle = 1 << 1,
            #endif

            /**
             * Right mouse button or first stylus button
             * @attention Available since Android 4.0 (API level 14), not
             *      detectable in earlier versions.
             */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || __ANDROID_API__ >= 14
            Right = MotionEventSerializer::MOTION_EVENT_BUTTON_SECONDARY
            #else
            Right = 1 << 2
            #endif
        };

        /** @brief Button */
        Button button() {
            // redirect touch events to button left
            // for compatibility with desktop
            // button_state 0 is a touch on the touchscreen
            return button_state == 0 ? Button::Left : Button(button_state);
        }

        /** @brief Position */
        Vector2i position() {
            return {Int(x),Int(y)};
        }

    private:
        float x;
        float y;
        int button_state;
        explicit MouseEvent(MotionEventSerializer & event, float x_, float y_, int button_state_):
            InputEvent(event), x(x_), y(y_), button_state(button_state_) {}
};

/**
@brief Mouse move event
@see @ref MouseEvent, @ref mouseMoveEvent()
*/
class WindowlessAndroidApplication::MouseMoveEvent: public InputEvent {
    friend WindowlessAndroidApplication;

    public:
        /**
         * @brief Mouse button
         *
         * @see @ref buttons()
         */
        enum class Button: std::int32_t {
            /**
             * Left mouse button. Note that this button set for
             * compatibility with desktop
             */
            Left = MotionEventSerializer::MOTION_EVENT_BUTTON_PRIMARY,

            /**
             * Middle mouse button or second stylus button
             * @attention Available since Android 4.0 (API level 14), not
             *      detectable in earlier versions.
             */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || __ANDROID_API__ >= 14
            Middle = MotionEventSerializer::MOTION_EVENT_BUTTON_TERTIARY,
            #else
            Middle = 1 << 1,
            #endif

            /**
             * Right mouse button or first stylus button
             * @attention Available since Android 4.0 (API level 14), not
             *      detectable in earlier versions.
             */
            #if defined(DOXYGEN_GENERATING_OUTPUT) || __ANDROID_API__ >= 14
            Right = MotionEventSerializer::MOTION_EVENT_BUTTON_SECONDARY
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
            return {Int(x), Int(y)};
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
            // redirect touch events to button left
            // for compatibility with desktop
            // button_state 0 is a touch on the touchscreen
            return button_state == 0 ? Button::Left : Button(button_state);
        }

    private:
        float x;
        float y;
        int button_state;
        explicit MouseMoveEvent(MotionEventSerializer & event, float x_, float y_,
                                int button_state_, Vector2i relativePosition):
                                InputEvent(event), x(x_), y(y_), button_state(button_state_),
                                _relativePosition{relativePosition} {}

        const Vector2i _relativePosition;
};

CORRADE_ENUMSET_OPERATORS(WindowlessAndroidApplication::MouseMoveEvent::Buttons)

}}
#else
#error this file is available only on Android build
#endif

#endif
