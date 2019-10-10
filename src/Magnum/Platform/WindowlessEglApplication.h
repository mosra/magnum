#ifndef Magnum_Platform_WindowlessEglApplication_h
#define Magnum_Platform_WindowlessEglApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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
 * @brief Class @ref Magnum::Platform::WindowlessEglApplication, @ref Magnum::Platform::WindowlessEglContext, macro @ref MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN()
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <EGL/egl.h>
#include <EGL/eglext.h>
/* undef Xlib nonsense to avoid conflicts */
#undef Always
#undef Complex
#undef None
#undef Status
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/Tags.h"
#include "Magnum/Platform/Platform.h"

namespace Magnum { namespace Platform {

/**
@brief Windowless EGL context

@m_keywords{WindowlessGLContext}

GL context using EGL without any windowing system, used in
@ref WindowlessEglApplication. Does not have any default framebuffer. It is
built if `WITH_WINDOWLESSEGLAPPLICATION` is enabled in CMake.

Meant to be used when there is a need to manage (multiple) GL contexts
manually. See @ref platform-windowless-contexts for more information. If no
other application header is included, this class is also aliased to
@cpp Platform::WindowlessGLContext @ce.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class WindowlessEglContext {
    public:
        class Configuration;

        /**
         * @brief Constructor
         * @param configuration Context configuration
         * @param context       Optional Magnum context instance constructed
         *      using @ref NoCreate to manage driver workarounds
         *
         * Once the context is created, make it current using @ref makeCurrent()
         * and create @ref Platform::GLContext instance to be able to use
         * Magnum.
         * @see @ref isCreated()
         */
        explicit WindowlessEglContext(const Configuration& configuration, GLContext* context = nullptr);

        /**
         * @brief Construct without creating the context
         *
         * Move a instance with created context over to make it usable.
         */
        explicit WindowlessEglContext(NoCreateT) {}

        /** @brief Copying is not allowed */
        WindowlessEglContext(const WindowlessEglContext&) = delete;

        /** @brief Move constructor */
        WindowlessEglContext(WindowlessEglContext&& other);

        /** @brief Copying is not allowed */
        WindowlessEglContext& operator=(const WindowlessEglContext&) = delete;

        /** @brief Move assignment */
        WindowlessEglContext& operator=(WindowlessEglContext&& other);

        /**
         * @brief Destructor
         *
         * Destroys the context, if any.
         */
        ~WindowlessEglContext();

        /** @brief Whether the context is created */
        bool isCreated() const { return _context; }

        /**
         * @brief Make the context current
         *
         * Prints error message and returns @cpp false @ce on failure,
         * otherwise returns @cpp true @ce.
         */
        bool makeCurrent();

    private:
        EGLDisplay _display{};
        EGLContext _context{};
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        /* Needed only by SwiftShader, using EGL_NO_SURFACE everywhere else */
        EGLSurface _surface = EGL_NO_SURFACE;
        #endif
};

/**
@brief Configuration

@see @ref WindowlessEglContext(),
    @ref WindowlessEglApplication::WindowlessEglApplication(),
    @ref WindowlessEglApplication::createContext(),
    @ref WindowlessEglApplication::tryCreateContext()
*/
class WindowlessEglContext::Configuration {
    public:
        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref setFlags(), @ref GL::Context::Flag
         * @requires_gles Context flags are not available in WebGL.
         */
        enum class Flag: int {
            #ifndef MAGNUM_TARGET_GLES
            /**
             * Forward compatible context
             *
             * @requires_gl Core/compatibility profile distinction and forward
             *      compatibility applies only to desktop GL.
             */
            ForwardCompatible = EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR,
            #endif

            /**
             * Debug context. Enabled automatically if the
             * `--magnum-gpu-validation` @ref GL-Context-command-line "command-line option"
             * is present.
             */
            Debug = EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR
        };

        /**
         * @brief Context flags
         *
         * @see @ref setFlags(), @ref Context::Flags
         * @requires_gles Context flags are not available in WebGL.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typedef Containers::EnumSet<Flag, EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR
            #ifndef MAGNUM_TARGET_GLES
            |EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR
            #endif
            > Flags;
        #else
        typedef Containers::EnumSet<Flag> Flags;
        #endif
        #endif

        /*implicit*/ Configuration();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Context flags
         *
         * @requires_gles Context flags are not available in WebGL.
         */
        Flags flags() const { return _flags; }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is @ref Flag::ForwardCompatible on desktop GL and no flags
         * on OpenGL ES.
         * @see @ref addFlags(), @ref clearFlags(), @ref GL::Context::flags()
         * @requires_gles Context flags are not available in WebGL.
         */
        Configuration& setFlags(Flags flags) {
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
         * @requires_gles Context flags are not available in WebGL.
         */
        Configuration& addFlags(Flags flags) {
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
         * @requires_gles Context flags are not available in WebGL.
         */
        Configuration& clearFlags(Flags flags) {
            _flags &= ~flags;
            return *this;
        }

        /**
         * @brief Device ID to use
         *
         * @requires_gles Device selection is not available in WebGL.
         */
        UnsignedInt device() const { return _device; }

        /**
         * @brief Set device ID to use
         * @return Reference to self (for method chaining)
         *
         * The device ID is expected to be smaller than the count of devices
         * reported by EGL. When using @ref WindowlessEglApplication, this is
         * also exposed as a `--magnum-device` command-line option and a
         * `MAGNUM_DEVICE` environment variable.
         * @requires_gles Device selection is not available in WebGL.
         */
        Configuration& setDevice(UnsignedInt id) {
            _device = id;
            return *this;
        }
        #endif

    private:
        #ifndef MAGNUM_TARGET_WEBGL
        Flags _flags;
        UnsignedInt _device;
        #endif
};

#ifndef MAGNUM_TARGET_WEBGL
CORRADE_ENUMSET_OPERATORS(WindowlessEglContext::Configuration::Flags)
#endif

/**
@brief Windowless EGL application

@m_keywords{WindowlessApplication}

Application for offscreen rendering using @ref WindowlessEglContext. This
application library is in theory available for all platforms for which EGL
works (Linux desktop or ES, @ref CORRADE_TARGET_IOS "iOS",
@ref CORRADE_TARGET_ANDROID "Android" and also
@ref CORRADE_TARGET_EMSCRIPTEN "Emscripten"). See other
`Windowless*Application` classes for an alternative. It is built if
`WITH_WINDOWLESSEGLAPPLICATION` is enabled in CMake.

@section Platform-WindowlessEglApplication-bootstrap Bootstrap application

Fully contained windowless application using @ref WindowlessEglApplication
along with CMake setup is available in `windowless` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/windowless.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/windowless.zip)
file. After extracting the downloaded archive you can build and run the
application with these four commands:

@code{.sh}
mkdir build && cd build
cmake ..
cmake --build .
./src/MyApplication # or ./src/Debug/MyApplication
@endcode

See @ref cmake for more information.

@section Platform-WindowlessEglApplication-bootstrap-emscripten Bootstrap application for Emscripten

Fully contained windowless application together with Emscripten support along
with full HTML markup and CMake setup is available in `windowless-emscripten`
branch of [Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap)
repository, download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/windowless-emscripten.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/windowless-emscripten.zip)
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

@code{.sh}
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/Emscripten.cmake" \
    -DCMAKE_PREFIX_PATH=/usr/lib/emscripten/system \
    -DCMAKE_INSTALL_PREFIX=/srv/http/emscripten
cmake --build .
cmake --build . --target install
@endcode

You can then open `MyApplication.html` in your browser (through a webserver,
e.g. http://localhost/emscripten/MyApplication.html).

Detailed information about deployment for Emscripten and all needed boilerplate
together with a troubleshooting guide is available in @ref platforms-html5.

@section Platform-WindowlessEglApplication-usage General usage

In order to use this library from CMake, you need to copy `FindEGL.cmake` from
the modules directory in Magnum source to the `modules/` dir in your project
(so it is able to find the EGL library). Request the `WindowlessEglApplication`
component of the `Magnum` package and link to the
`Magnum::WindowlessEglApplication` target:

@code{.cmake}
find_package(Magnum REQUIRED WindowlessEglApplication)

# ...
target_link_libraries(your-app Magnum::WindowlessEglApplication)
@endcode

If no other application is requested, you can also use the generic
`Magnum::WindowlessApplication` alias to simplify porting. Again, see
@ref building and @ref cmake for more information.

Place your code into @ref exec(). The subclass can be then used in main
function using @ref MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN() macro. See
@ref platform for more information.

@code{.cpp}
class MyApplication: public Platform::WindowlessEglApplication {
    // implement required methods...
};
MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
@cpp Platform::WindowlessApplication @ce and the macro is aliased to
@cpp MAGNUM_WINDOWLESSAPPLICATION_MAIN() @ce to simplify porting.

@section Platform-WindowlessEglApplication-device-selection GPU device selection

The application prefers to use the @m_class{m-doc-external}
[EGL_EXT_device_enumeration](https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_device_enumeration.txt),
@m_class{m-doc-external} [EGL_EXT_platform_base](https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_platform_base.txt) and
@m_class{m-doc-external} [EGL_EXT_platform_device](https://www.khronos.org/registry/EGL/extensions/EXT/EGL_EXT_platform_device.txt)
extensions where available instead of `EGL_DEFAULT_DISPLAY` to work better on
headless setups. The application chooses the first found device by default, you
can override that either with @ref Configuration::setDevice() or using a
`--magnum-device` command-line option (and the `MAGNUM_DEVICE` environment
variable). Unfortunately EGL doesn't provide any reasonable way to enumerate or
filter named devices, so the best you can do is checking reported device count
printed by the `--magnum-log verbose` @ref GL-Context-command-line "command-line option",
and then going from `0` up to figure out the desired device ID.

@m_class{m-block m-danger}

@par No EGL devices found
@parblock
Systems running Mesa 19.2 (which has the above extensions) that also have
`libEGL_nvidia.so` installed (for example as a CUDA dependency) may fail
to create the context with the following error (with additional output
produced when the `--magnum-gpu-validation`
@ref GL-Context-command-line "command-line option" is enabled):

@m_class{m-console-wrap}

@code{.shell-session}
eglQueryDevicesEXT(): EGL_BAD_ALLOC error: In internal function: Additional INFO may be available
eglQueryDevicesEXT(): EGL_BAD_ALLOC error: In function eglQueryDevicesEXT(), backend failed to query devices
Platform::WindowlessEglApplication::tryCreateContext(): no EGL devices found
@endcode

This is due to the NVidia's EGL implementation failing to enumerate devices
(because there aren't any), which then causes the GLVND wrapper to stop
instead of enumerating the Mesa devices as well. The solution is
whitelisting all EGL implementations except the NVidia one
<a href="https://github.com/NVIDIA/libglvnd/blob/master/src/EGL/icd_enumeration.md">as described in the libglvnd documentation</a>
using the `__EGL_VENDOR_LIBRARY_FILENAMES` environment variable, for example:

@m_class{m-console-wrap}

@code{.sh}
__EGL_VENDOR_LIBRARY_FILENAMES=/usr/share/glvnd/egl_vendor.d/50_mesa.json ./my-application
@endcode
@endparblock
*/
class WindowlessEglApplication {
    public:
        /** @brief Application arguments */
        struct Arguments {
            /** @brief Constructor */
            /*implicit*/ constexpr Arguments(int& argc, char** argv) noexcept: argc{argc}, argv{argv}{}

            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        /**
         * @brief Configuration
         *
         * @see @ref WindowlessEglApplication(), @ref createContext(),
         *      @ref tryCreateContext()
         */
        typedef WindowlessEglContext::Configuration Configuration;

        /**
         * @brief Default constructor
         * @param arguments     Application arguments
         * @param configuration Configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref Configuration for more information. The program exits if
         * the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         * @see @ref WindowlessEglContext
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit WindowlessEglApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit WindowlessEglApplication(const Arguments& arguments, const Configuration& configuration);
        explicit WindowlessEglApplication(const Arguments& arguments);
        #endif

        /**
         * @brief Constructor
         * @param arguments     Application arguments
         *
         * Unlike above, the context is not created and must be created later
         * with @ref createContext() or @ref tryCreateContext().
         */
        explicit WindowlessEglApplication(const Arguments& arguments, NoCreateT);

        /** @brief Copying is not allowed */
        WindowlessEglApplication(const WindowlessEglApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessEglApplication(WindowlessEglApplication&&) = delete;

        /** @brief Copying is not allowed */
        WindowlessEglApplication& operator=(const WindowlessEglApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessEglApplication& operator=(WindowlessEglApplication&&) = delete;

        /**
         * @brief Execute application
         * @return Value for returning from @cpp main() @ce
         *
         * See @ref MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN() for usage
         * information.
         */
        virtual int exec() = 0;

    protected:
        /* Nobody will need to have (and delete) WindowlessEglApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessEglApplication();

        /**
         * @brief Create context with given configuration
         *
         * Must be called if and only if the context wasn't created by the
         * constructor itself. Error message is printed and the program exits
         * if the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         * @see @ref WindowlessEglContext
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
         * Unlike @ref createContext() returns @cpp false @ce if the context
         * cannot be created, @cpp true @ce otherwise.
         */
        bool tryCreateContext(const Configuration& configuration);

    private:
        WindowlessEglContext _glContext;
        Containers::Pointer<Platform::GLContext> _context;

        #ifndef MAGNUM_TARGET_WEBGL
        /* These are saved from command-line arguments */
        UnsignedInt _commandLineDevice;
        #endif
};

/** @hideinitializer
@brief Entry point for windowless EGL application
@param className Class name

See @ref Magnum::Platform::WindowlessEglApplication "Platform::WindowlessEglApplication"
for usage information. This macro abstracts out platform-specific entry point
code and is equivalent to the following, see @ref portability-applications for
more information.

@code{.cpp}
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode

When no other windowless application header is included this macro is also
aliased to @cpp MAGNUM_WINDOWLESSAPPLICATION_MAIN() @ce.
*/
#define MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN(className)                     \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_WINDOWLESSAPPLICATION_MAIN
typedef WindowlessEglApplication WindowlessApplication;
typedef WindowlessEglContext WindowlessGLContext;
#define MAGNUM_WINDOWLESSAPPLICATION_MAIN(className) MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_WINDOWLESSAPPLICATION_MAIN
#endif
#endif

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
