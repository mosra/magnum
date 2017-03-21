#ifndef Magnum_Platform_GlxApplication_h
#define Magnum_Platform_GlxApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::Platform::GlxApplication, macro @ref MAGNUM_GLXAPPLICATION_MAIN()
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
CMake.

## Bootstrap application

The usage is very similar to @ref Sdl2Application, for which fully contained
base application along with CMake setup is available, see its documentation for
more information.

## General usage

In CMake you need to request `GlxApplication` component of `Magnum` package and
link to `Magnum::GlxApplication` target. If no other application is requested,
you can also use generic `Magnum::Application` alias to simplify porting. See
@ref building and @ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass can be then used directly in `main()` -- see
convenience macro @ref MAGNUM_GLXAPPLICATION_MAIN(). See @ref platform for more
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
        /**
         * @brief Default constructor
         * @param arguments     Application arguments
         * @param configuration Configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref AbstractXApplication::Configuration "Configuration" for
         * more information. The program exits if the context cannot be
         * created, see below for an alternative.
         * @todoc make this copydoc from Sdl2Application when Doxygen is able
         *      to find Configuration subclass
         */
        explicit GlxApplication(const Arguments& arguments, const Configuration& configuration = Configuration());

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, NoCreateT) */
        explicit GlxApplication(const Arguments& arguments, NoCreateT);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief GlxApplication(const Arguments&, NoCreateT)
         * @deprecated Use @ref GlxApplication(const Arguments&, NoCreateT) instead.
         */
        CORRADE_DEPRECATED("use GlxApplication(const Arguments&, NoCreateT) instead") explicit GlxApplication(const Arguments& arguments, std::nullptr_t): GlxApplication{arguments, NoCreate} {}
        #endif

    protected:
        /* Nobody will need to have (and delete) GlxApplication*, thus this is
           faster than public pure virtual destructor */
        ~GlxApplication();
};

/** @hideinitializer
@brief Entry point for GLX-based applications
@param className Class name

See @ref Magnum::Platform::GlxApplication "Platform::GlxApplication" for usage
information. This macro abstracts out platform-specific entry point code and is
equivalent to the following, see @ref portability-applications for more
information.
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
