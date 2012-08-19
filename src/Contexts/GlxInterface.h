#ifndef Magnum_Contexts_EglInterface_h
#define Magnum_Contexts_EglInterface_h
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
 * @brief Class Magnum::Contexts::GlxInterface
 */

#include "Magnum.h"
#include <GL/glx.h>

#include "AbstractGlInterface.h"

namespace Magnum { namespace Contexts {

/**
@brief GLX interface

Creates OpenGL 3.3 core context or OpenGL ES 2.0 context, if targetting
OpenGL ES.

Used in GlxContext.
*/
class GlxInterface: public AbstractGlInterface<Display*, VisualID, Window> {
    public:
        ~GlxInterface();

        VisualID getVisualId(Display* nativeDisplay);
        void createContext(Window nativeWindow);

        /* This must be enabled, otherwise (on my NVidia) it crashes when creating VAO. WTF. */
        inline ExtensionWrangler::ExperimentalFeatures experimentalExtensionWranglerFeatures() const {
            return ExtensionWrangler::ExperimentalFeatures::Enable;
        }

        inline void makeCurrent() {
            glXMakeCurrent(display, window, context);
        }

        inline void swapBuffers() {
            glXSwapBuffers(display, window);
        }

    private:
        Display* display;
        Window window;
        GLXFBConfig* configs;
        GLXContext context;
};

}}

#endif
