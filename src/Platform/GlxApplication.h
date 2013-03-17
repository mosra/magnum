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

Creates window with double-buffered OpenGL or OpenGL ES 2.0 context, if
targetting OpenGL ES. Uses GlxContextHandler.

@section GlxApplication-usage Usage

You need to implement at least drawEvent() and viewportEvent() to be able to
draw on the screen. The subclass can be then used directly in `main()` - see
convenience macro MAGNUM_XAPPLICATION_MAIN().
@code
class MyApplication: public Magnum::Platform::GlxApplication {
    // implement required methods...
};
MAGNUM_XAPPLICATION_MAIN(MyApplication)
@endcode
*/
class GlxApplication: public AbstractXApplication {
    public:
        /**
         * @brief Constructor
         * @param argc      Count of arguments of `main()` function
         * @param argv      Arguments of `main()` function
         * @param title     Window title
         * @param size      Window size
         */
        inline explicit GlxApplication(int& argc, char** argv, const std::string& title = "Magnum GLX application", const Vector2i& size = Vector2i(800, 600)): AbstractXApplication(new GlxContextHandler, argc, argv, title, size) {}
};

}}

#endif
