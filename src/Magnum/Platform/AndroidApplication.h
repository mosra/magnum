#ifndef Magnum_Platform_AndroidApplication_h
#define Magnum_Platform_AndroidApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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
 * @brief Class @ref Magnum::Platform::AndroidApplication
 */

#include <memory>
#include <EGL/egl.h>
#include <android_native_app_glue.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Platform/Platform.h"

#ifndef CORRADE_TARGET_ANDROID
#error This file is available only on Android
#endif

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief Android application

Application running in Android.

This application library is available only in
@ref CORRADE_TARGET_ANDROID "Android", see respective sections
in @ref building-corrade-cross-android "Corrade's" and @ref building-cross-android "Magnum's"
building documentation. It is built if `WITH_ANDROIDAPPLICATION` is enabled in
CMake.

## Bootstrap application

Fully contained base application using @ref GlutApplication for desktop build
and @ref AndroidApplication for Android build along with full Android packaging
stuff and CMake setup is available in `base-android` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-android.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-android.zip) file.
After extracting the downloaded archive, you can do the desktop build in the
same way as with @ref GlutApplication. For the Android build you also
need to put the contents of toolchains repository from https://github.com/mosra/toolchains
in `toolchains/` subdirectory. Don't forget to adapt `ANDROID_NDK_ROOT` in
`toolchains/generic/Android-*.cmake` to path where NDK is installed. Default is
`/opt/android-ndk`. Adapt also `ANDROID_SYSROOT` to your preferred API level.
You might also need to update `ANDROID_TOOLCHAIN_PREFIX` and
`ANDROID_TOOLCHAIN_ROOT` to fit your system.

First you need to update Android project files with the following command. It
will create `build.xml` file for Ant and a bunch of other files. You need to
specify the target for which you will build in the `-t` parameter. List of all
targets can be obtained by calling `android list target`.

    android update project -p . -t "android-19"

Then create build directories for ARM and x86 and run `cmake` and build command
in them. The toolchains need access to the platform file, so be sure to
properly set **absolute** path to `toolchains/modules/` directory containing
`Platform/Android.cmake`.

    mkdir build-android-arm && cd build-android-arm
    cmake .. \
        -DCMAKE_MODULE_PATH="/absolute/path/to/toolchains/modules" \
        -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/Android-ARM.cmake"
    cmake --build .

    mkdir build-android-x86 && cd build-android-x86
    cmake .. \
        -DCMAKE_MODULE_PATH="/absolute/path/to/toolchains/modules" \
        -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/Android-x86.cmake"
    cmake --build .

The compiled binaries will be put into `lib/armeabi-v7a` and `lib/x86`. You can
then build the APK package simply by running `ant`. The resulting APK package
can be then installed directly on the device or emulator using `adb install`.

    ant debug
    adb install bin/NativeActivity-debug.apk

## General usage

For CMake you need to copy `FindEGL.cmake` and `FindOpenGLES2.cmake` (or
`FindOpenGLES3.cmake`) from `modules/` directory in %Magnum source to `modules/`
dir in your project (so it is able to find EGL and OpenGL ES libraries).
Request `%AndroidApplication` component, add
`${MAGNUM_ANDROIDAPPLICATION_INCLUDE_DIRS}` to include path and link to
`${MAGNUM_ANDROIDAPPLICATION_LIBRARIES}`. If no other application is requested,
you can also use generic `${MAGNUM_APPLICATION_INCLUDE_DIRS}` and
`${MAGNUM_APPLICATION_LIBRARIES}` aliases to simplify porting. See
@ref building and @ref cmake for more information. Note that unlike on other
platforms you need to create *shared library* instead of executable. The
resulting binary then needs to be copied to `lib/armeabi-v7a` and `lib/x86`,
you can do that automatically in CMake using the following commands:

    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}")

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass must be then made accessible from JNI using
@ref MAGNUM_ANDROIDAPPLICATION_MAIN() macro. See @ref platform for more
information.
@code
class MyApplication: public Platform::AndroidApplication {
    // implement required methods...
};
MAGNUM_ANDROIDAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.

### Android packaging stuff

The application needs at least the `AndroidManifest.xml` with the following
contents:

    <?xml version="1.0" encoding="utf-8"?>
    <manifest xmlns:android="http://schemas.android.com/apk/res/android" package="cz.mosra.magnum.application" android:versionCode="1" android:versionName="1.0">
        <uses-sdk android:minSdkVersion="9" />
        <uses-feature android:glEsVersion="0x00020000" />
        <application android:label="Magnum Android Application" android:hasCode="false">
            <activity android:name="android.app.NativeActivity" android:label="Magnum Android Application">
                <meta-data android:name="android.app.lib_name" android:value="{{application}}" />
                <intent-filter>
                    <action android:name="android.intent.action.MAIN" />
                    <category android:name="android.intent.category.LAUNCHER" />
                </intent-filter>
            </activity>
        </application>
    </manifest>

Modify `android:label` to your liking, set unique `package` name and replace
`{{application}}` with name of the binary file (without extension). If you plan
to use OpenGL ES, set `android:glEsVersion` to `0x00030000`. The resulting APK
file will be named `NativeActivity.apk` by default, you can change that either
by passing `-n` parameter to `android update project` or later by editing first
line of the generated `build.xml` file.

## Redirecting output to Android log buffer

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

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, const Configuration&) */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit AndroidApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit AndroidApplication(const Arguments& arguments, const Configuration& configuration);
        explicit AndroidApplication(const Arguments& arguments);
        #endif

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, std::nullptr_t) */
        explicit AndroidApplication(const Arguments& arguments, std::nullptr_t);

        /** @brief Copying is not allowed */
        AndroidApplication(const AndroidApplication&) = delete;

        /** @brief Moving is not allowed */
        AndroidApplication(AndroidApplication&&) = delete;

        virtual ~AndroidApplication();

        /** @brief Copying is not allowed */
        AndroidApplication& operator=(const AndroidApplication&) = delete;

        /** @brief Moving is not allowed */
        AndroidApplication& operator=(AndroidApplication&&) = delete;

    protected:
        /** @copydoc Sdl2Application::createContext() */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void createContext(const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        void createContext(const Configuration& configuration);
        void createContext();
        #endif

        /** @copydoc Sdl2Application::tryCreateContext() */
        bool tryCreateContext(const Configuration& configuration);

        /** @{ @name Screen handling */

        /** @copydoc Sdl2Application::swapBuffers() */
        void swapBuffers();

        /** @copydoc Sdl2Application::redraw() */
        void redraw() { _flags |= Flag::Redraw; }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /** @copydoc Sdl2Application::viewportEvent() */
        virtual void viewportEvent(const Vector2i& size);

        /** @copydoc Sdl2Application::drawEvent() */
        virtual void drawEvent() = 0;

        /*@}*/

    private:
        struct LogOutput;

        enum class Flag: UnsignedByte {
            Redraw = 1 << 1
        };
        typedef Containers::EnumSet<Flag, UnsignedByte> Flags;

        static void commandEvent(android_app* app, std::int32_t cmd);
        static std::int32_t inputEvent(android_app* app, AInputEvent* event);

        void initialize();

        android_app* const _state;
        Flags _flags;

        EGLDisplay _display;
        EGLSurface _surface;
        EGLContext _context;

        std::unique_ptr<Context> _c;
        std::unique_ptr<LogOutput> _logOutput;

        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)
};

CORRADE_ENUMSET_OPERATORS(AndroidApplication::Flags)

/**
@brief %Configuration

Double-buffered RGBA canvas with depth and stencil buffers.
@see @ref AndroidApplication(), @ref createContext(), @ref tryCreateContext()
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
         * Default is `{0, 0}`, which means that the size of the physical
         * window will be used. If set to different value than the physical
         * size, the surface will be scaled.
         */
        Configuration& setSize(const Vector2i& size) {
            _size = size;
            return *this;
        }

        /**
         * @brief Set context version
         *
         * @note This function does nothing and is included only for
         *      compatibility with other toolkits. @ref Version::GLES200 or
         *      @ref Version::GLES300 is used based on engine compile-time
         *      settings.
         */
        Configuration& setVersion(Version) { return *this; }

    private:
        Vector2i _size;
};

/** @hideinitializer
@brief Entry point for Android applications
@param className Class name

See @ref Magnum::Platform::AndroidApplication "Platform::AndroidApplication"
for usage information. This macro abstracts out platform-specific entry point
code (the classic `main()` function cannot be used in Android). See
@ref portability-applications for more information. When no other application
header is included this macro is also aliased to `MAGNUM_APPLICATION_MAIN()`.
*/
#define MAGNUM_ANDROIDAPPLICATION_MAIN(className)                           \
    void android_main(android_app* state) {                                 \
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

#endif
