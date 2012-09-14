#ifndef Magnum_Contexts_XEglWindowContext_h
#define Magnum_Contexts_XEglWindowContext_h
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
 * @brief Class Magnum::Contexts::XEglWindowContext
 */

#include "AbstractXWindowContext.h"
#include "EglContextHandler.h"

namespace Magnum { namespace Contexts {

/**
@brief X/EGL context

Uses EglContextHandler.
*/
class XEglWindowContext: public AbstractXWindowContext {
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
        inline XEglWindowContext(int& argc, char** argv, const std::string& title = "Magnum X/EGL context", const Math::Vector2<GLsizei>& size = Math::Vector2<GLsizei>(800, 600)): AbstractXWindowContext(new EglContextHandler, argc, argv, title, size) {}
};

}}

#endif
