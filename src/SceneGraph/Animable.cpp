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

#include "Animable.hpp"

namespace Magnum { namespace SceneGraph {

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_SCENEGRAPH_EXPORT Animable<2>;
template class MAGNUM_SCENEGRAPH_EXPORT Animable<3>;
template class MAGNUM_SCENEGRAPH_EXPORT AnimableGroup<2>;
template class MAGNUM_SCENEGRAPH_EXPORT AnimableGroup<3>;
#endif

Debug operator<<(Debug debug, AnimationState value) {
    switch(value) {
        #define _c(value) case AnimationState::value: return debug << "SceneGraph::AnimationState::" #value;
        _c(Stopped)
        _c(Paused)
        _c(Running)
        #undef _c
    }

    return debug << "SceneGraph::AnimationState::(invalid)";
}

}}
