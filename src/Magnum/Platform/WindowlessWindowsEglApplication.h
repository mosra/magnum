#ifndef Magnum_Platform_WindowlessWindowsEglApplication_h
#define Magnum_Platform_WindowlessWindowsEglApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Platform::WindowlessWindowsEglApplication, @ref Magnum::Platform::WindowlessWindowsEglContext, macro @ref MAGNUM_WINDOWLESSWINDOWSEGLAPPLICATION_MAIN()
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#ifndef DOXYGEN_GENERATING_OUTPUT
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN
#endif
#include <windows.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/Platform/Platform.h"

namespace Magnum { namespace Platform {

/**
@brief Windowless Windows/EGL context

@m_keywords{WindowlessGLContext EGL}

GL context using pure WINAPI and EGL, used in @ref WindowlessWindowsEglApplication.

Meant to be used when there is a need to manage (multiple) GL contexts
manually. See @ref platform-windowless-contexts for more information. If no
other application header is included, this class is also aliased to
@cpp Platform::WindowlessGLContext @ce.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class WindowlessWindowsEglContext {
    public:
        class Configuration;

        /**
         * @brief Constructor
         * @param configuration Context configuration
         * @param context       Optional Magnum context instance constructed
         *      using @ref NoCreate to manage driver workarounds
         *
         * Once the context is created, make it current using
         * @ref makeCurrent() and create @ref Platform::GLContext instance to
         * be able to use Magnum.
         * @see @ref isCreated()
         */
        explicit WindowlessWindowsEglContext(const Configuration& configuration, GLContext* context = nullptr);

        /**
         * @brief Construct without creating the context
         *
         * Move a instance with created context over to make it usable.
         */
        explicit WindowlessWindowsEglContext(NoCreateT) {}

        /** @brief Copying is not allowed */
        WindowlessWindowsEglContext(const WindowlessWindowsEglContext&) = delete;

        /** @brief Move constructor */
        WindowlessWindowsEglContext(WindowlessWindowsEglContext&& other);

        /** @brief Copying is not allowed */
        WindowlessWindowsEglContext& operator=(const WindowlessWindowsEglContext&) = delete;

        /** @brief Move assignment */
        WindowlessWindowsEglContext& operator=(WindowlessWindowsEglContext&& other);

        /**
         * @brief Destructor
         *
         * Destroys the context, if any.
         */
        ~WindowlessWindowsEglContext();

        /** @brief Whether the context is created */
        bool isCreated() const { return _context; }

        /**
         * @brief Make the context current
         *
         * Prints error message and returns @cpp false @ce on failure,
         * otherwise returns @cpp true @ce.
         */
        bool makeCurrent();

        /**
         * @brief Underlying OpenGL context
         * @m_since{2020,06}
         *
         * Use in case you need to call EGL functionality directly or in order
         * to create a shared context. Returns @cpp nullptr @ce in case the
         * context was not created yet.
         * @see @ref Configuration::setSharedContext()
         */
        EGLContext glContext() { return _context; }

    private:
        HWND _window{};
        EGLDisplay _display{};
        EGLSurface _surface{};
        EGLContext _context{};
};

/**
@brief Configuration

@see @ref WindowlessWindowsEglContext(),
    @ref WindowlessWindowsEglApplication::WindowlessWindowsEglApplication(),
    @ref WindowlessWindowsEglApplication::createContext(),
    @ref WindowlessWindowsEglApplication::tryCreateContext()
*/
class WindowlessWindowsEglContext::Configuration {
    public:
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref setFlags(), @ref GL::Context::Flag
         */
        enum class Flag: int {
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
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typedef Containers::EnumSet<Flag, EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR> Flags;
        #else
        typedef Containers::EnumSet<Flag> Flags;
        #endif

        constexpr /*implicit*/ Configuration() {}

        /** @brief Context flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is no flag. See also @ref GL::Context::flags().
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
         */
        Configuration& clearFlags(Flags flags) {
            _flags &= ~flags;
            return *this;
        }

        /**
         * @brief Create a shared context
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * When set, the created context will share a subset of OpenGL objects
         * with @p context, instead of being independent. Many caveats and
         * limitations apply to shared OpenGL contexts, please consult the
         * OpenGL specification for details. Default is `EGL_NO_CONTEXT`, i.e.
         * no sharing.
         * @see @ref WindowlessWindowsEglContext::glContext(),
         *      @ref WindowlessWindowsEglApplication::glContext()
         */
        Configuration& setSharedContext(EGLContext context) {
            _sharedContext = context;
            return *this;
        }

        /**
         * @brief Shared context
         * @m_since{2020,06}
         */
        EGLContext sharedContext() const { return _sharedContext; }

    private:
        Flags _flags;
        EGLContext _sharedContext = EGL_NO_CONTEXT;
};

CORRADE_ENUMSET_OPERATORS(WindowlessWindowsEglContext::Configuration::Flags)

/**
@brief Windowless Windows/EGL application

@m_keywords{WindowlessApplication EGL}

Application for offscreen rendering using @ref WindowlessWindowsEglContext.
This application library is available on OpenGL ES (also ANGLE) on Windows.

@section Platform-WindowlessWindowsEglApplication-bootstrap Bootstrap application

Fully contained windowless application using @ref WindowlessWindowsEglApplication
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

@section Platform-WindowlessWindowsEglApplication-usage General usage

This application library is built if `WITH_WINDOWLESSWINDOWSEGLAPPLICATION` is
enabled when building Magnum. To use this library from CMake, put [FindEGL.cmake](https://github.com/mosra/magnum/blob/master/modules/FindEGL.cmake)
into your `modules/` directory, request the `WindowlessWindowsEglApplication`
component of the `Magnum` package and link to the
`Magnum::WindowlessWindowsEglApplication` target:

@code{.cmake}
find_package(Magnum REQUIRED)
if(CORRADE_TARGET_WINDOWS)
    find_package(Magnum REQUIRED WindowlessWindowsEglApplication)
endif()

# ...
if(CORRADE_TARGET_WINDOWS)
    target_link_libraries(your-app PRIVATE Magnum::WindowlessWindowsEglApplication)
endif()
@endcode

Additionally, if you're using Magnum as a CMake subproject, do the following
* *before* calling @cmake find_package() @ce to ensure it's enabled, as the
library is not built by default:

@code{.cmake}
set(WITH_WINDOWLESSWINDOWSEGLAPPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

If no other application is requested, you can also use the generic
`Magnum::WindowlessApplication` alias to simplify porting. Again, see
@ref building and @ref cmake for more information.

Place your code into @ref exec(). The subclass can be then used in main
function using @ref MAGNUM_WINDOWLESSWINDOWSEGLAPPLICATION_MAIN() macro. See
@ref platform for more information.

@code{.cpp}
class MyApplication: public Platform::WindowlessWindowsEglApplication {
    // implement required methods...
};
MAGNUM_WINDOWLESSWINDOWSEGLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
@cpp Platform::WindowlessApplication @ce and the macro is aliased to
@cpp MAGNUM_WINDOWLESSAPPLICATION_MAIN() @ce to simplify porting.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class WindowlessWindowsEglApplication {
    public:
        /** @brief Application arguments */
        struct Arguments {
            /** @brief Constructor */
            /*implicit*/ constexpr Arguments(int& argc, char** argv) noexcept: argc{argc}, argv{argv} {}

            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        /**
         * @brief Configuration
         *
         * @see @ref WindowlessWindowsEglApplication(), @ref createContext(),
         *      @ref tryCreateContext()
         */
        typedef WindowlessWindowsEglContext::Configuration Configuration;

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
        explicit WindowlessWindowsEglApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit WindowlessWindowsEglApplication(const Arguments& arguments, const Configuration& configuration);
        explicit WindowlessWindowsEglApplication(const Arguments& arguments);
        #endif

        /**
         * @brief Constructor
         * @param arguments     Application arguments
         *
         * Unlike above, the context is not created and must be created later
         * with @ref createContext() or @ref tryCreateContext().
         */
        explicit WindowlessWindowsEglApplication(const Arguments& arguments, NoCreateT);

        /** @brief Copying is not allowed */
        WindowlessWindowsEglApplication(const WindowlessWindowsEglApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessWindowsEglApplication(WindowlessWindowsEglApplication&&) = delete;

        /** @brief Copying is not allowed */
        WindowlessWindowsEglApplication& operator=(const WindowlessWindowsEglApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessWindowsEglApplication& operator=(WindowlessWindowsEglApplication&&) = delete;

        /**
         * @brief Execute application
         * @return Value for returning from @cpp main() @ce
         *
         * See @ref MAGNUM_WINDOWLESSWINDOWSEGLAPPLICATION_MAIN() for usage
         * information.
         */
        virtual int exec() = 0;

        /**
         * @brief Underlying OpenGL context
         * @m_since{2020,06}
         *
         * Use in case you need to call EGL functionality directly or in order
         * to create a shared context. Returns @cpp nullptr @ce in case the
         * context was not created yet.
         * @see @ref Configuration::setSharedContext()
         */
        EGLContext glContext() { return _glContext.glContext(); }

    protected:
        /* Nobody will need to have (and delete) WindowlessWindowsEglApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessWindowsEglApplication();

        /**
         * @brief Create context with given configuration
         *
         * Must be called if and only if the context wasn't created by the
         * constructor itself. Error message is printed and the program exits
         * if the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
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
        WindowlessWindowsEglContext _glContext;
        Containers::Pointer<Platform::GLContext> _context;
};

/** @hideinitializer
@brief Entry point for windowless Windows/EGL application
@param className Class name

@m_keywords{MAGNUM_WINDOWLESSAPPLICATION_MAIN()}

See @ref Magnum::Platform::WindowlessWindowsEglApplication "Platform::WindowlessWindowsEglApplication"
for usage information.This macro abstracts out platform-specific entry point
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
#define MAGNUM_WINDOWLESSWINDOWSEGLAPPLICATION_MAIN(className)              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_WINDOWLESSAPPLICATION_MAIN
typedef WindowlessWindowsEglApplication WindowlessApplication;
typedef WindowlessWindowsEglContext WindowlessGLContext;
#define MAGNUM_WINDOWLESSAPPLICATION_MAIN(className) MAGNUM_WINDOWLESSWINDOWSEGLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_WINDOWLESSAPPLICATION_MAIN
#endif
#endif

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
