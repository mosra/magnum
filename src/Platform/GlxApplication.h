#ifndef Magnum_Platform_GlxApplication_h
#define Magnum_Platform_GlxApplication_h
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
 * @brief Class Magnum::Platform::GlxApplication
 */

#include "AbstractXApplication.h"
#include "GlxContextHandler.h"

namespace Magnum { namespace Platform {

/**
@brief GLX application

Uses GlxContextHandler.
*/
class GlxApplication: public AbstractXApplication {
    public:
        /**
         * @brief Constructor
         * @param argc      Count of arguments of `main()` function
         * @param argv      Arguments of `main()` function
         * @param title     Window title
         * @param size      Window size
         *
         * Creates window with double-buffered OpenGL 3.2 core context or
         * OpenGL ES 2.0 context, if targetting OpenGL ES.
         */
        inline GlxApplication(int& argc, char** argv, const std::string& title = "Magnum GLX application", const Math::Vector2<GLsizei>& size = Math::Vector2<GLsizei>(800, 600)): AbstractXApplication(new GlxContextHandler, argc, argv, title, size) {}
};

}}

#endif
