#ifndef Magnum_Contexts_AbstractGlInterface_h
#define Magnum_Contexts_AbstractGlInterface_h
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
 * @brief Class Magnum::Contexts::AbstractGlInterface
 */

#include "ExtensionWrangler.h"

namespace Magnum { namespace Contexts {

/** @brief Base for OpenGL interfaces */
template<class Display, class VisualId, class Window> class AbstractGlInterface {
    public:
        /**
         * @brief Get visual ID
         *
         * Initializes the interface on given display and returns visual ID.
         */
        virtual VisualId getVisualId(Display nativeDisplay) = 0;

        /**
         * @brief Destructor
         *
         * Finalizes and closes the interface.
         */
        virtual ~AbstractGlInterface() {}

        /** @brief Create context */
        virtual void createContext(Window nativeWindow) = 0;

        /**
         * @brief Whether to enable experimental extension wrangler features
         *
         * Default is to disable.
         */
        virtual inline ExtensionWrangler::ExperimentalFeatures experimentalExtensionWranglerFeatures() const {
            return ExtensionWrangler::ExperimentalFeatures::Disable;
        }

        /** @brief Make the context current */
        virtual void makeCurrent() = 0;

        /** @brief Swap buffers */
        virtual void swapBuffers() = 0;
};

}}

#endif
