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

#include "ExtensionWrangler.h"

#include <cstdlib>
#include <Utility/Debug.h>

#include "Magnum.h"

namespace Magnum { namespace Platform {

void ExtensionWrangler::initialize(ExperimentalFeatures experimentalFeatures) {
    #ifndef MAGNUM_TARGET_GLES
    /* Enable experimental features */
    if(experimentalFeatures == ExperimentalFeatures::Enable)
        glewExperimental = true;

    /* Init GLEW */
    GLenum err = glewInit();
    if(err != GLEW_OK) {
        Error() << "ExtensionWrangler: cannot initialize GLEW:" << glewGetErrorString(err);
        std::exit(1);
    }
    #else
    static_cast<void>(experimentalFeatures); /* Shut up about unused parameter */
    #endif
}

}}
