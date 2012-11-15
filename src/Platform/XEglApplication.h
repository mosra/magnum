#ifndef Magnum_Platform_XEglApplication_h
#define Magnum_Platform_XEglApplication_h
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
 * @brief Class Magnum::Platform::XEglApplication
 */

#include "AbstractXApplication.h"
#include "EglContextHandler.h"

namespace Magnum { namespace Platform {

/**
@brief X/EGL application

Uses EglContextHandler.

@section XEglApplication-usage Usage

You need to implement at least drawEvent() and viewportEvent() to be able to
draw on the screen.  The subclass can be then used directly in `main()` - see
convenience macro MAGNUM_XAPPLICATION_MAIN().
@code
class MyApplication: public Magnum::Platform::XEglApplication {
    // implement required methods...
};
MAGNUM_XAPPLICATION_MAIN(MyApplication)
@endcode
*/
class XEglApplication: public AbstractXApplication {
    public:
        /**
         * @brief Constructor
         * @param argc      Count of arguments of `main()` function
         * @param argv      Arguments of `main()` function
         * @param title     Window title
         * @param size      Window size
         *
         * Creates window with double-buffered OpenGL ES 2 context.
         */
        inline XEglApplication(int& argc, char** argv, const std::string& title = "Magnum X/EGL application", const Math::Vector2<GLsizei>& size = Math::Vector2<GLsizei>(800, 600)): AbstractXApplication(new EglContextHandler, argc, argv, title, size) {}
};

}}

#endif
