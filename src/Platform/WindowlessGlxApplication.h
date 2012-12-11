#ifndef Magnum_Platform_WindowlessGlxApplication_h
#define Magnum_Platform_WindowlessGlxApplication_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Platform::WindowlessGlxApplication
 */

#include "Magnum.h"
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
/* undef Xlib nonsense to avoid conflicts */
#undef None
#undef Always

#include "AbstractContextHandler.h"

namespace Magnum { namespace Platform {

/**
@brief Windowless GLX application

@section WindowlessGlxApplication-usage Usage

Place your code into exec(). The subclass can be then used directly in
`main()` - see convenience macro MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN().
@code
class MyApplication: public Magnum::Platform::WindowlessGlxApplication {
    // implement required methods...
};
MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN(MyApplication)
@endcode
*/
class WindowlessGlxApplication {
    public:
        /**
         * @brief Constructor
         * @param argc      Count of arguments of `main()` function
         * @param argv      Arguments of `main()` function
         *
         * Creates window with double-buffered OpenGL 3.2 core context or
         * OpenGL ES 2.0 context, if targetting OpenGL ES.
         */
        WindowlessGlxApplication(int& argc, char** argv);

        ~WindowlessGlxApplication();

        /**
         * @brief Execute application
         * @return Value for returning from `main()`.
         */
        virtual int exec() = 0;

    private:
        Display* display;
        GLXContext context;
        GLXPbuffer pbuffer;

        Context* c;
};

/** @hideinitializer
@brief Entry point for windowless GLX application
@param className Class name

Can be used as equivalent to the following code to achieve better portability,
see @ref portability-applications for more information.
@code
int main(int argc, char** argv) {
    className app(argc, argv);
    return app.exec();
}
@endcode
When no other application header is included this macro is also aliased to
`MAGNUM_APPLICATION_MAIN()`.
*/
#define MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN(className)                     \
    int main(int argc, char** argv) {                                       \
        className app(argc, argv);                                          \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

}}

#endif
