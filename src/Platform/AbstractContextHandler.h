#ifndef Magnum_Platform_AbstractContextHandler_h
#define Magnum_Platform_AbstractContextHandler_h
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
 * @brief Class Magnum::Platform::AbstractContextHandler
 */

#include "ExtensionWrangler.h"

namespace Magnum { namespace Platform {

/** @brief Base for OpenGL context handlers */
template<class Display, class VisualId, class Window> class AbstractContextHandler {
    public:
        /**
         * @brief Get visual ID
         *
         * Initializes the handler on given display and returns visual ID.
         */
        virtual VisualId getVisualId(Display nativeDisplay) = 0;

        explicit AbstractContextHandler() = default;

        /**
         * @brief Destructor
         *
         * Finalizes and closes the handler.
         */
        virtual ~AbstractContextHandler() {}

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
