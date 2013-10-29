#ifndef Magnum_Platform_WindowlessGlxApplication_h
#define Magnum_Platform_WindowlessGlxApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Platform::WindowlessGlxApplication
 */

#include <utility>
#include "OpenGL.h"
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
/* undef Xlib nonsense to avoid conflicts */
#undef Complex
#undef None
#undef Always

#include "Magnum.h"

namespace Magnum { namespace Platform {

/**
@brief Windowless GLX application

Application for offscreen rendering using pure X11 and GLX. See @ref platform
for brief introduction.

This application library is available on desktop OpenGL and
@ref MAGNUM_TARGET_DESKTOP_GLES "OpenGL ES emulation on desktop" on Linux. It
depends on **X11** library and is built if `WITH_WINDOWLESSGLXAPPLICATION` is
enabled in CMake. To use it, you need to request `%WindowlessGlxApplication`
component in CMake, add `${MAGNUM_WINDOWLESSGLXAPPLICATION_INCLUDE_DIRS}` to
include path and link to `${MAGNUM_WINDOWLESSGLXAPPLICATION_LIBRARIES}`. If no
other windowless application is requested, you can also use generic
`${MAGNUM_WINDOWLESSAPPLICATION_INCLUDE_DIRS}` and
`${MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES}` aliases to simplify porting.

@section WindowlessGlxApplication-usage Usage

Place your code into @ref exec(). The subclass can be then used directly in
`main()` -- see convenience macro @ref MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN().
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
            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        class Configuration;

        /** @copydoc GlutApplication::GlutApplication(const Arguments&, const Configuration&) */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit WindowlessGlxApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit WindowlessGlxApplication(const Arguments& arguments, const Configuration& configuration);
        explicit WindowlessGlxApplication(const Arguments& arguments);
        #endif

        /** @copydoc GlutApplication::GlutApplication(const Arguments&, std::nullptr_t) */
        explicit WindowlessGlxApplication(const Arguments& arguments, std::nullptr_t);

        /**
         * @brief Execute application
         * @return Value for returning from `main()`.
         */
        virtual int exec() = 0;

    protected:
        /* Nobody will need to have (and delete) WindowlessGlxApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessGlxApplication();

        /** @copydoc GlutApplication::createContext() */
        void createContext(const Configuration& configuration);

    private:
        Display* display;
        GLXContext context;
        GLXPbuffer pbuffer;

        Context* c;
};

/**
@brief %Configuration

@see WindowlessGlxApplication(), createContext()
*/
class WindowlessGlxApplication::Configuration {
    Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration& operator=(Configuration&&) = delete;

    public:
        /*implicit*/ Configuration();
        ~Configuration();
};

/** @hideinitializer
@brief Entry point for windowless GLX application
@param className Class name

Can be used as equivalent to the following code to achieve better portability,
see @ref portability-applications for more information.
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
