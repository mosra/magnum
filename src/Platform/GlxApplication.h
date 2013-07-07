#ifndef Magnum_Platform_GlxApplication_h
#define Magnum_Platform_GlxApplication_h
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
 * @brief Class Magnum::Platform::GlxApplication
 */

#include "AbstractXApplication.h"
#include "GlxContextHandler.h"

namespace Magnum { namespace Platform {

/**
@brief GLX application

Uses GlxContextHandler. See @ref platform for brief introduction.

@section GlxApplication-usage Usage

You need to implement at least drawEvent() and viewportEvent() to be able to
draw on the screen. The subclass can be then used directly in `main()` - see
convenience macro MAGNUM_GLXAPPLICATION_MAIN().
@code
class MyApplication: public Magnum::Platform::GlxApplication {
    // implement required methods...
};
MAGNUM_GLXAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.
*/
class GlxApplication: public AbstractXApplication {
    public:
        /** @copydoc GlutApplication::GlutApplication(const Arguments&, const Configuration&) */
        explicit GlxApplication(const Arguments& arguments, const Configuration& configuration = Configuration()): AbstractXApplication(new GlxContextHandler, arguments, configuration) {}

        /** @copydoc GlutApplication::GlutApplication(const Arguments&, std::nullptr_t) */
        explicit GlxApplication(const Arguments& arguments, std::nullptr_t): AbstractXApplication(new GlxContextHandler, arguments, nullptr) {}

    protected:
        /* Nobody will need to have (and delete) GlxApplication*, thus this is
           faster than public pure virtual destructor */
        ~GlxApplication() = default;
};

/** @hideinitializer
@brief Entry point for GLX-based applications
@param className Class name

Can be used with GlxApplication subclasses as equivalent to the following code
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
#define MAGNUM_GLXAPPLICATION_MAIN(className)                               \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef GlxApplication Application;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_GLXAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

}}

#endif
