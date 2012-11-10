#ifndef Magnum_Platform_GlxContextHandler_h
#define Magnum_Platform_GlxContextHandler_h
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
 * @brief Class Magnum::Platform::GlxContextHandler
 */

#include "Magnum.h"
#include <GL/glx.h>
/* undef Xlib nonsense to avoid conflicts */
#undef None
#undef Always

#include "AbstractContextHandler.h"

#include "magnumCompatibility.h"

namespace Magnum { namespace Platform {

/**
@brief GLX context

Creates OpenGL 3.2 core context or OpenGL ES 2.0 context, if targetting
OpenGL ES.

Used in GlxApplication.
*/
class GlxContextHandler: public AbstractContextHandler<Display*, VisualID, Window> {
    public:
        ~GlxContextHandler();

        VisualID getVisualId(Display* nativeDisplay) override;
        void createContext(Window nativeWindow) override;

        /* This must be enabled, otherwise (on my NVidia) it crashes when creating VAO. WTF. */
        inline ExtensionWrangler::ExperimentalFeatures experimentalExtensionWranglerFeatures() const override {
            return ExtensionWrangler::ExperimentalFeatures::Enable;
        }

        inline void makeCurrent() override {
            glXMakeCurrent(display, window, context);
        }

        inline void swapBuffers() override {
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
