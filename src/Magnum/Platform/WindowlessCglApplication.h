#ifndef Magnum_Platform_WindowlessCglApplication_h
#define Magnum_Platform_WindowlessCglApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2013 <https://github.com/ArEnSc>
    Copyright © 2014 Travis Watkins <https://github.com/amaranth>

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
 * @brief Class @ref Magnum::Platform::WindowlessCglApplication, @ref Magnum::Platform::WindowlessCglContext, macro @ref MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN()
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/Platform/Platform.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#define GL_SILENCE_DEPRECATION /* YES I KNOW, APPLE! FFS */
#endif
#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLTypes.h>
#include <OpenGL/CGLCurrent.h>

namespace Magnum { namespace Platform {

/**
@brief Windowless CGL context

@m_keywords{WindowlessGLContext CGL}

GL context used in @ref WindowlessCglApplication. Does not have any default
framebuffer.

Meant to be used when there is a need to manage (multiple) GL contexts
manually. See @ref platform-windowless-contexts for more information. If no
other application header is included, this class is also aliased to
@cpp Platform::WindowlessGLContext @ce.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class WindowlessCglContext {
    public:
        class Configuration;

        /**
         * @brief Constructor
         * @param configuration Context configuration
         * @param context       Optional Magnum context instance constructed
         *      using @ref NoCreate to manage driver workarounds
         *
         * If version is not specified in @p configuration, it first tries to
         * create core context (OpenGL 3.2+), if that fails, tries OpenGL 3.0+
         * and as a last attempt falls back to compatibility OpenGL 2.1
         * context.
         *
         * Once the context is created, make it current using @ref makeCurrent()
         * and create @ref Platform::GLContext instance to be able to use
         * Magnum.
         * @see @ref isCreated()
         */
        explicit WindowlessCglContext(const Configuration& configuration, GLContext* context = nullptr);

        /**
         * @brief Construct without creating the context
         *
         * Move a instance with created context over to make it usable.
         */
        explicit WindowlessCglContext(NoCreateT) {}

        /** @brief Copying is not allowed */
        WindowlessCglContext(const WindowlessCglContext&) = delete;

        /** @brief Move constructor */
        WindowlessCglContext(WindowlessCglContext&& other);

        /** @brief Copying is not allowed */
        WindowlessCglContext& operator=(const WindowlessCglContext&) = delete;

        /** @brief Move assignment */
        WindowlessCglContext& operator=(WindowlessCglContext&& other);

        /**
         * @brief Destructor
         *
         * Destroys the context, if any.
         */
        ~WindowlessCglContext();

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
         * Use in case you need to call CGL functionality directly or in order
         * to create a shared context. Returns @cpp nullptr @ce in case the
         * context was not created yet.
         * @see @ref Configuration::setSharedContext()
         */
        CGLContextObj glContext() { return _context; }

    private:
        CGLPixelFormatObj _pixelFormat{};
        CGLContextObj _context{};
};

/**
@brief Configuration

@see @ref WindowlessCglContext(),
    @ref WindowlessCglApplication::WindowlessCglApplication(),
    @ref WindowlessCglApplication::createContext(),
    @ref WindowlessCglApplication::tryCreateContext()
*/
class WindowlessCglContext::Configuration {
    public:
        constexpr /*implicit*/ Configuration() {}

        /**
         * @brief Create a shared context
         * @return Reference to self (for method chaining)
         * @m_since{2020,06}
         *
         * When set, the created context will share a subset of OpenGL objects
         * with @p context, instead of being independent. Many caveats and
         * limitations apply to shared OpenGL contexts, please consult the
         * OpenGL specification for details. Default is @cpp nullptr @ce, i.e.
         * no sharing.
         * @see @ref WindowlessCglContext::glContext(),
         *      @ref WindowlessCglApplication::glContext()
         */
        Configuration& setSharedContext(CGLContextObj context) {
            _sharedContext = context;
            return *this;
        }

        /**
         * @brief Shared context
         * @m_since{2020,06}
         */
        CGLContextObj sharedContext() const { return _sharedContext; }

    private:
        CGLContextObj _sharedContext = nullptr;
};

/**
@brief Windowless CGL application

@m_keywords{WindowlessApplication CGL}

Application for offscreen rendering using @ref WindowlessCglContext. This
application library is available on desktop OpenGL on macOS.

@section Platform-WindowlessCglApplication-bootstrap Bootstrap application

Fully contained windowless application using @ref WindowlessCglApplication
along with CMake setup is available in `base-windowless` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap)
repository, download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-windowless.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-windowless.zip)
file. After extracting the downloaded archive you can build and run the
application with these four commands:

@code{.sh}
mkdir build && cd build
cmake ..
cmake --build .
./src/MyApplication # or ./src/Debug/MyApplication
@endcode

See @ref cmake for more information.

@section Platform-WindowlessCglApplication-usage General usage

This application library is built if `WITH_WINDOWLESSCGLAPPLICATION` is enabled
when building Magnum. To use this library from CMake, request the
`WindowlessCglApplication` component of the `Magnum` package and link to the `Magnum::WindowlessCglApplication` target:

@code{.cmake}
find_package(Magnum REQUIRED)
if(CORRADE_TARGET_APPLE)
    find_package(Magnum REQUIRED WindowlessCglApplication)
endif()

# ...
if(CORRADE_TARGET_APPLE)
    target_link_libraries(your-app PRIVATE Magnum::WindowlessCglApplication)
endif()
@endcode

Additionally, if you're using Magnum as a CMake subproject, do the following
* *before* calling @cmake find_package() @ce to ensure it's enabled, as the
library is not built by default:

@code{.cmake}
set(WITH_WINDOWLESSCGLAPPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

If no other application is requested, you can also use the generic
`Magnum::WindowlessApplication` alias to simplify porting. Again, see
@ref building and @ref cmake for more information.

Place your code into @ref exec(). The subclass can be then used directly in
`main()` --- see convenience macro @ref MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN().
See @ref platform for more information.

@code{.sh}
class MyApplication: public Platform::WindowlessCglApplication {
    // implement required methods...
};
MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
@cpp Platform::WindowlessApplication @ce and the macro is aliased to
@cpp MAGNUM_WINDOWLESSAPPLICATION_MAIN() @ce to simplify porting.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class WindowlessCglApplication {
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
         * @see @ref WindowlessCglApplication(), @ref createContext(),
         *      @ref tryCreateContext()
         */
        typedef WindowlessCglContext::Configuration Configuration;

        /**
         * @brief Default constructor
         * @param arguments     Application arguments
         * @param configuration Configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref Configuration for more information. The program exits if
         * the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         * @see @ref WindowlessCglContext
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit WindowlessCglApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit WindowlessCglApplication(const Arguments& arguments, const Configuration& configuration);
        explicit WindowlessCglApplication(const Arguments& arguments);
        #endif

        /**
         * @brief Construct without creating the context
         * @param arguments     Application arguments
         *
         * Unlike above, the context is not created and must be created later
         * with @ref createContext() or @ref tryCreateContext().
         */
        explicit WindowlessCglApplication(const Arguments& arguments, NoCreateT);

        /** @brief Copying is not allowed */
        WindowlessCglApplication(const WindowlessCglApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessCglApplication(WindowlessCglApplication&&) = delete;

        /** @brief Copying is not allowed */
        WindowlessCglApplication& operator=(const WindowlessCglApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessCglApplication& operator=(WindowlessCglApplication&&) = delete;

        /**
         * @brief Execute application
         * @return Value for returning from @cpp main() @ce
         *
         * See @ref MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN() for usage
         * information.
         */
        virtual int exec() = 0;

        /**
         * @brief Underlying OpenGL context
         * @m_since{2020,06}
         *
         * Use in case you need to call CGL functionality directly or in order
         * to create a shared context. Returns @cpp nullptr @ce in case the
         * context was not created yet.
         * @see @ref Configuration::setSharedContext()
         */
        CGLContextObj glContext() { return _glContext.glContext(); }

    protected:
        /* Nobody will need to have (and delete) WindowlessCglApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessCglApplication();

        /**
         * @brief Create context with given configuration
         *
         * Must be called if and only if the context wasn't created by the
         * constructor itself. Error message is printed and the program exits
         * if the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         * @see @ref WindowlessCglContext
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
        WindowlessCglContext _glContext;
        Containers::Pointer<Platform::GLContext> _context;
};

/** @hideinitializer
@brief Entry point for windowless CGL application
@param className Class name

@m_keywords{MAGNUM_WINDOWLESSAPPLICATION_MAIN()}

See @ref Magnum::Platform::WindowlessCglApplication "Platform::WindowlessCglApplication"
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
#define MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN(className)                     \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_WINDOWLESSAPPLICATION_MAIN
typedef WindowlessCglApplication WindowlessApplication;
typedef WindowlessCglContext WindowlessGLContext;
#define MAGNUM_WINDOWLESSAPPLICATION_MAIN(className) MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_WINDOWLESSAPPLICATION_MAIN
#endif
#endif

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
