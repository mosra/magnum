#ifndef Magnum_Platform_XEglApplication_h
#define Magnum_Platform_XEglApplication_h
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
 * @brief Class @ref Magnum::Platform::XEglApplication, macro @ref MAGNUM_XEGLAPPLICATION_MAIN()
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/Platform/Platform.h"
#include "Magnum/Platform/AbstractXApplication.h"

namespace Magnum { namespace Platform {

/**
@brief X/EGL application

@m_keywords{Application}

Application using pure X11 and EGL. Supports keyboard and mouse handling.

This application library is available on both desktop OpenGL and
@ref MAGNUM_TARGET_GLES "OpenGL ES" on Linux.

@section Platform-XEglApplication-bootstrap Bootstrap application

The usage is very similar to @ref Sdl2Application, for which fully contained
base application along with CMake setup is available, see its documentation for
more information.

@section Platform-XEglApplication-usage General usage

This application library depends on **X11** and **EGL** libraries and is built
if `WITH_XEGLAPPLICATION` is enabled when building Magnum. To use this library
from CMake, put [FindEGL.cmake](https://github.com/mosra/magnum/blob/master/modules/FindEGL.cmake)
into your `modules/` directory, request the `XEglApplication` component of the
`Magnum` package and link to the `Magnum::XEglApplication` target:

@code{.cmake}
find_package(Magnum REQUIRED XEglApplication)

# ...
target_link_libraries(your-app PRIVATE Magnum::XEglApplication)
@endcode

Additionally, if you're using Magnum as a CMake subproject, do the following
* *before* calling @cmake find_package() @ce to ensure it's enabled, as the
library is not built by default:

@code{.cmake}
set(WITH_XEGLAPPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

If no other application is requested, you can also use the generic
`Magnum::Application` alias to simplify porting. Again, see @ref building and
@ref cmake for more information.

In C++ code you need to implement at least @ref drawEvent() to be able to draw
on the screen. The subclass can be then used directly in @cpp main() @ce
--- see convenience macro @ref MAGNUM_XEGLAPPLICATION_MAIN(). See @ref platform
for more information.

@code{.cpp}
class MyApplication: public Platform::XEglApplication {
    // implement required methods...
};
MAGNUM_XEGLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
@cpp Platform::Application @ce and the macro is aliased to
@cpp MAGNUM_APPLICATION_MAIN() @ce to simplify porting.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class XEglApplication: public AbstractXApplication {
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
        explicit XEglApplication(const Arguments& arguments, const Configuration& configuration = Configuration{}, const GLConfiguration& glConfiguration = GLConfiguration{});

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, NoCreateT) */
        explicit XEglApplication(const Arguments& arguments, NoCreateT);

    protected:
        /* Nobody will need to have (and delete) XEglApplication*, thus this is
           faster than public pure virtual destructor */
        ~XEglApplication();
};

/** @hideinitializer
@brief Entry point for X/EGL-based applications
@param className Class name

@m_keywords{MAGNUM_APPLICATION_MAIN()}

See @ref Magnum::Platform::XEglApplication "Platform::XEglApplication" for
usage information. This macro abstracts out platform-specific entry point code
and is equivalent to the following, see @ref portability-applications for more
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
#define MAGNUM_XEGLAPPLICATION_MAIN(className)                              \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef XEglApplication Application;
typedef BasicScreen<XEglApplication> Screen;
typedef BasicScreenedApplication<XEglApplication> ScreenedApplication;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_XEGLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
