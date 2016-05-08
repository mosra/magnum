#ifndef Magnum_Platform_WindowlessCglApplication_h
#define Magnum_Platform_WindowlessCglApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
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
 * @brief Class @ref Magnum::Platform::WindowlessCglApplication, macro @ref MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN()
 */

#include <memory>

#include "Magnum/OpenGL.h"
#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLTypes.h>
#include <OpenGL/CGLCurrent.h>

#include "Magnum/Magnum.h"
#include "Magnum/Platform/Platform.h"

namespace Magnum { namespace Platform {

/**
@brief Windowless CGL application

Application for offscreen rendering using pure CGL. Does not have any default
framebuffer.

This application library is available on desktop OpenGL on OS X. It
is built if `WITH_WINDOWLESSCGLAPPLICATION` is enabled in CMake.

## Bootstrap application

Fully contained windowless application using @ref WindowlessCglApplication
along with CMake setup is available in `base-windowless` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap)
repository, download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/base-windowless.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/base-windowless.zip)
file. After extracting the downloaded archive you can build and run the
application with these four commands:

    mkdir build && cd build
    cmake ..
    cmake --build .
    ./src/MyApplication # or ./src/Debug/MyApplication

See @ref cmake for more information.

## General usage

In CMake you need to request `WindowlessCglApplication` component, add
`${MAGNUM_WINDOWLESSCGLAPPLICATION_INCLUDE_DIRS}` to include path and link to
`${MAGNUM_WINDOWLESSCGLAPPLICATION_LIBRARIES}`. If no other windowless
application is requested, you can also use generic
`${MAGNUM_WINDOWLESSAPPLICATION_INCLUDE_DIRS}` and
`${MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES}` aliases to simplify porting. Again,
see @ref building and @ref cmake for more information.

Place your code into @ref exec(). The subclass can be then used directly in
`main()` -- see convenience macro @ref MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN().
See @ref platform for more information.
@code
class MyApplication: public Platform::WindowlessCglApplication {
    // implement required methods...
};
MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::WindowlessApplication` and the macro is aliased to
`MAGNUM_WINDOWLESSAPPLICATION_MAIN()` to simplify porting.
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

        class Configuration;

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, const Configuration&) */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit WindowlessCglApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit WindowlessCglApplication(const Arguments& arguments, const Configuration& configuration);
        explicit WindowlessCglApplication(const Arguments& arguments);
        #endif

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, std::nullptr_t) */
        explicit WindowlessCglApplication(const Arguments& arguments, std::nullptr_t);

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
         * @return Value for returning from `main()`
         *
         * See @ref MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN() for usage
         * information.
         */
        virtual int exec() = 0;

    protected:
        /* Nobody will need to have (and delete) WindowlessCglApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessCglApplication();

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

    private:
        CGLContextObj _glContext;
        CGLPixelFormatObj _pixelFormat;

        std::unique_ptr<Platform::Context> _context;
};

/**
@brief Configuration

@see @ref WindowlessCglApplication(), @ref createContext(),
    @ref tryCreateContext()
*/
class WindowlessCglApplication::Configuration {
    public:
        constexpr /*implicit*/ Configuration() {}
};

/** @hideinitializer
@brief Entry point for windowless CGL application
@param className Class name

See @ref Magnum::Platform::WindowlessCglApplication "Platform::WindowlessCglApplication"
for usage information. This macro abstracts out platform-specific entry point
code and is equivalent to the following, see @ref portability-applications for
more information.
@code
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode
When no other windowless application header is included this macro is also
aliased to `MAGNUM_WINDOWLESSAPPLICATION_MAIN()`.
*/
#define MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN(className)                     \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_WINDOWLESSAPPLICATION_MAIN
typedef WindowlessCglApplication WindowlessApplication;
#define MAGNUM_WINDOWLESSAPPLICATION_MAIN(className) MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_WINDOWLESSAPPLICATION_MAIN
#endif
#endif

}}

#endif
