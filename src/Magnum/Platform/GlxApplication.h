#ifndef Magnum_Platform_GlxApplication_h
#define Magnum_Platform_GlxApplication_h
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
 * @brief Class @ref Magnum::Platform::GlxApplication, macro @ref MAGNUM_GLXAPPLICATION_MAIN()
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/Platform/AbstractXApplication.h"
#include "Magnum/Platform/Platform.h"

namespace Magnum { namespace Platform {

/**
@brief GLX application

@m_keywords{Application}

Application using pure X11 and GLX. Supports keyboard and mouse handling.
Available on desktop OpenGL and
@ref MAGNUM_TARGET_DESKTOP_GLES "OpenGL ES emulation on desktop" on Linux.

@section Platform-GlxApplication-bootstrap Bootstrap application

The usage is very similar to @ref Sdl2Application, for which fully contained
base application along with CMake setup is available, see its documentation for
more information.

@section Platform-GlxApplication-usage General usage

This application library depends on the **X11** library and is built if `WITH_GLXAPPLICATION` is enabled when building Magnum. To use this library from
CMake, request the `GlxApplication` component of the `Magnum` package and link
to the `Magnum::GlxApplication` target:

@code{.cmake}
find_package(Magnum REQUIRED GlxApplication)

# ...
target_link_libraries(your-app PRIVATE Magnum::GlxApplication)
@endcode

Additionally, if you're using Magnum as a CMake subproject, do the following
* *before* calling @cmake find_package() @ce to ensure it's enabled, as the
library is not built by default:

@code{.cmake}
set(WITH_GLXAPPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

If no other application is requested, you can also use the generic
`Magnum::Application` alias to simplify porting. See @ref building and
@ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass can be then used directly in @cpp main() @ce
--- see convenience macro @ref MAGNUM_GLXAPPLICATION_MAIN(). See @ref platform
for more information.

@code{.cpp}
class MyApplication: public Platform::GlxApplication {
    // implement required methods...
};
MAGNUM_GLXAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
@cpp Platform::Application @ce and the macro is aliased to
@cpp MAGNUM_APPLICATION_MAIN() @ce to simplify porting.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class GlxApplication: public AbstractXApplication {
    public:
        /**
         * @brief Construct with given configuration for OpenGL context
         * @param arguments         Application arguments
         * @param configuration     Application configuration
         * @param glConfiguration   OpenGL context configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref AbstractXApplication::Configuration "Configuration" and
         * @ref AbstractXApplication::GLConfiguration "GLConfiguration" for
         * more information. The program exits if the context cannot be
         * created, see @ref tryCreate() for an alternative.
         */
        explicit GlxApplication(const Arguments& arguments, const Configuration& configuration = Configuration{}, const GLConfiguration& glConfiguration = GLConfiguration{});

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, NoCreateT) */
        explicit GlxApplication(const Arguments& arguments, NoCreateT);

    protected:
        /* Nobody will need to have (and delete) GlxApplication*, thus this is
           faster than public pure virtual destructor */
        ~GlxApplication();
};

/** @hideinitializer
@brief Entry point for GLX-based applications
@param className Class name

@m_keywords{MAGNUM_APPLICATION_MAIN()}

See @ref Magnum::Platform::GlxApplication "Platform::GlxApplication" for usage
information. This macro abstracts out platform-specific entry point code and is
equivalent to the following, see @ref portability-applications for more
information.

@code{.cpp}
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode

When no other application header is included this macro is also aliased to
@cpp MAGNUM_APPLICATION_MAIN() @ce.
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
#else
#error this header is available only in the OpenGL build
#endif

#endif
