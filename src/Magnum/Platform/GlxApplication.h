#ifndef Magnum_Platform_GlxApplication_h
#define Magnum_Platform_GlxApplication_h
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
 * @brief Class @ref Magnum::Platform::GlxApplication
 */

#include "Magnum/Platform/AbstractXApplication.h"
#include "Magnum/Platform/Platform.h"

namespace Magnum { namespace Platform {

/**
@brief GLX application

Application using pure X11 and GLX. Supports keyboard and mouse handling.

This application library is available on desktop OpenGL and
@ref MAGNUM_TARGET_DESKTOP_GLES "OpenGL ES emulation on desktop" on Linux. It
depends on **X11** library and is built if `WITH_GLXAPPLICATION` is enabled in
CMake. To use it, you need to request `%GlxApplication` component in CMake, add
`${MAGNUM_GLXAPPLICATION_INCLUDE_DIRS}` to include path and link to
`${MAGNUM_GLXAPPLICATION_LIBRARIES}`. If no other application is requested, you
can also use generic `${MAGNUM_APPLICATION_INCLUDE_DIRS}` and
`${MAGNUM_APPLICATION_LIBRARIES}` aliases to simplify porting. See
@ref building and @ref cmake for more information.

@section GlxApplication-usage Usage

You need to implement at least @ref drawEvent() to be able to draw on the
screen. The subclass can be then used directly in `main()` -- see convenience
macro @ref MAGNUM_GLXAPPLICATION_MAIN(). See @ref platform for more
information.
@code
class MyApplication: public Platform::GlxApplication {
    // implement required methods...
};
MAGNUM_GLXAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.
*/
class GlxApplication: public AbstractXApplication {
    public:
        /** @copydoc Sdl2Application::GlutApplication(const Arguments&, const Configuration&) */
        explicit GlxApplication(const Arguments& arguments, const Configuration& configuration = Configuration());

        /** @copydoc Sdl2Application::GlutApplication(const Arguments&, std::nullptr_t) */
        #ifndef CORRADE_GCC45_COMPATIBILITY
        explicit GlxApplication(const Arguments& arguments, std::nullptr_t);
        #else
        explicit GlxApplication(const Arguments& arguments, void*);
        #endif

    protected:
        /* Nobody will need to have (and delete) GlxApplication*, thus this is
           faster than public pure virtual destructor */
        ~GlxApplication();
};

/** @hideinitializer
@brief Entry point for GLX-based applications
@param className Class name

Can be used with @ref Magnum::Platform::GlxApplication "Platform::GlxApplication"
subclasses as equivalent to the following code to achieve better portability,
see @ref portability-applications for more information.
@code
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode
When no other application header is included this macro is also aliased to
`MAGNUM_APPLICATION_MAIN()`.
*/
#define MAGNUM_GLXAPPLICATION_MAIN(className)                               \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef GlxApplication Application;
typedef BasicScreen<GlxApplication> Screen;
typedef BasicScreenedApplication<GlxApplication> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_GLXAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

}}

#endif
