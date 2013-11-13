#ifndef Magnum_Platform_XEglApplication_h
#define Magnum_Platform_XEglApplication_h
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
 * @brief Class @ref Magnum::Platform::XEglApplication
 */

#include "Platform/Platform.h"
#include "Platform/AbstractXApplication.h"

namespace Magnum { namespace Platform {

/**
@brief X/EGL application

Application using pure X11 and EGL. Supports keyboard and mouse handling.

This application library is available on both desktop OpenGL and
@ref MAGNUM_TARGET_GLES "OpenGL ES" on Linux. It depends on **X11** and **EGL**
libraries and is built if `WITH_XEGLAPPLICATION` is enabled in CMake. To use
it, you need to copy `FindEGL.cmake` from `modules/` directory in %Magnum
source to `modules/` dir in your project (so CMake is able to find EGL),
request `%XEglApplication` component in CMake, add `${MAGNUM_XEGLAPPLICATION_INCLUDE_DIRS}`
to include path and link to `${MAGNUM_XEGLAPPLICATION_LIBRARIES}`.  If no other
application is requested, you can also use generic `${MAGNUM_APPLICATION_INCLUDE_DIRS}`
and `${MAGNUM_APPLICATION_LIBRARIES}` aliases to simplify porting. See
@ref building, @ref cmake and @ref platform for more information.

@section XEglApplication-usage Usage

You need to implement at least @ref drawEvent() and @ref viewportEvent() to be
able to draw on the screen.  The subclass can be then used directly in `main()`
-- see convenience macro @ref MAGNUM_XEGLAPPLICATION_MAIN().
@code
class MyApplication: public Platform::XEglApplication {
    // implement required methods...
};
MAGNUM_XEGLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.
*/
class XEglApplication: public AbstractXApplication {
    public:
        /** @copydoc GlutApplication::GlutApplication(const Arguments&, const Configuration&) */
        explicit XEglApplication(const Arguments& arguments, const Configuration& configuration = Configuration());

        /** @copydoc GlutApplication::GlutApplication(const Arguments&, std::nullptr_t) */
        #ifndef CORRADE_GCC45_COMPATIBILITY
        explicit XEglApplication(const Arguments& arguments, std::nullptr_t);
        #else
        explicit XEglApplication(const Arguments& arguments, void*);
        #endif

    protected:
        /* Nobody will need to have (and delete) XEglApplication*, thus this is
           faster than public pure virtual destructor */
        ~XEglApplication();
};

XEglApplication::~XEglApplication() = default;

/** @hideinitializer
@brief Entry point for X/EGL-based applications
@param className Class name

Can be used with XEglApplication subclasses as equivalent to the following code
to achieve better portability, see @ref portability-applications for more
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

#endif
