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

#include "Resource.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug MAGNUM_EXPORT operator<<(Debug debug, ResourceState value) {
    switch(value) {
        #define _c(value) case ResourceState::value: return debug << "ResourceState::" #value;
        _c(NotLoaded)
        _c(NotLoadedFallback)
        _c(Loading)
        _c(LoadingFallback)
        _c(NotFound)
        _c(NotFoundFallback)
        _c(Mutable)
        _c(Final)
        #undef _c
    }

    return debug << "ResourceState::(invalid)";
}
#endif

}
