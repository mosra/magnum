#ifndef Magnum_Platform_WindowlessGlxApplication_h
#define Magnum_Platform_WindowlessGlxApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
 * @brief Class @ref Magnum::Platform::WindowlessGlxApplication, macro @ref MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN()
 */

#include <memory>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/OpenGL.h"
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
/* undef Xlib nonsense to avoid conflicts */
#undef Always
#undef Complex
#undef None
#undef Status

#include "Magnum/Magnum.h"
#include "Magnum/Platform/Platform.h"

namespace Magnum { namespace Platform {

/**
@brief Windowless GLX application

Application for offscreen rendering using pure X11 and GLX.

This application library is available on desktop OpenGL and
@ref MAGNUM_TARGET_DESKTOP_GLES "OpenGL ES emulation on desktop" on Linux. It
depends on **X11** library and is built if `WITH_WINDOWLESSGLXAPPLICATION` is
enabled in CMake.

## Bootstrap application

Fully contained windowless application using @ref WindowlessGlxApplication
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

In CMake you need to request `WindowlessGlxApplication` component, add
`${MAGNUM_WINDOWLESSGLXAPPLICATION_INCLUDE_DIRS}` to include path and link to
`${MAGNUM_WINDOWLESSGLXAPPLICATION_LIBRARIES}`. If no other windowless
application is requested, you can also use generic
`${MAGNUM_WINDOWLESSAPPLICATION_INCLUDE_DIRS}` and
`${MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES}` aliases to simplify porting. Again,
see @ref building and @ref cmake for more information.

Place your code into @ref exec(). The subclass can be then used directly in
`main()` -- see convenience macro @ref MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN().
See @ref platform for more information.
@code
class MyApplication: public Platform::WindowlessGlxApplication {
    // implement required methods...
};
MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::WindowlessApplication` and the macro is aliased to
`MAGNUM_WINDOWLESSAPPLICATION_MAIN()` to simplify porting.
*/
class WindowlessGlxApplication {
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
        explicit WindowlessGlxApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit WindowlessGlxApplication(const Arguments& arguments, const Configuration& configuration);
        explicit WindowlessGlxApplication(const Arguments& arguments);
        #endif

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, std::nullptr_t) */
        explicit WindowlessGlxApplication(const Arguments& arguments, std::nullptr_t);

        /** @brief Copying is not allowed */
        WindowlessGlxApplication(const WindowlessGlxApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessGlxApplication(WindowlessGlxApplication&&) = delete;

        /** @brief Copying is not allowed */
        WindowlessGlxApplication& operator=(const WindowlessGlxApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessGlxApplication& operator=(WindowlessGlxApplication&&) = delete;

        /**
         * @brief Execute application
         * @return Value for returning from `main()`
         *
         * See @ref MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN() for usage
         * information.
         */
        virtual int exec() = 0;

    protected:
        /* Nobody will need to have (and delete) WindowlessGlxApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessGlxApplication();

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
        Display* _display;
        GLXContext _glContext;
        GLXPbuffer _pbuffer;

        std::unique_ptr<Platform::Context> _context;
};

/**
@brief Configuration

@see @ref WindowlessGlxApplication(), @ref createContext(),
    @ref tryCreateContext()
*/
class WindowlessGlxApplication::Configuration {
    public:
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref setFlags(), @ref Context::Flag
         */
        enum class Flag: int {
            Debug = GLX_CONTEXT_DEBUG_BIT_ARB   /**< Create debug context */
        };

        /**
         * @brief Context flags
         *
         * @see @ref setFlags(), @ref Context::Flags
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typedef Containers::EnumSet<Flag, GLX_CONTEXT_DEBUG_BIT_ARB> Flags;
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
         * Default is no flag. See also @ref Context::flags().
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

    private:
        Flags _flags;
};

/** @hideinitializer
@brief Entry point for windowless GLX application
@param className Class name

See @ref Magnum::Platform::WindowlessGlxApplication "Platform::WindowlessGlxApplication"
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
#define MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN(className)                     \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_WINDOWLESSAPPLICATION_MAIN
typedef WindowlessGlxApplication WindowlessApplication;
#define MAGNUM_WINDOWLESSAPPLICATION_MAIN(className) MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN(className)
#else
#undef MAGNUM_WINDOWLESSAPPLICATION_MAIN
#endif
#endif

}}

#endif
