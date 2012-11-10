#ifndef Magnum_Implementation_BufferState_h
#define Magnum_Implementation_BufferState_h
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

#include "Magnum.h"

#include "Buffer.h"

namespace Magnum { namespace Implementation {

struct BufferState {
    #ifndef MAGNUM_TARGET_GLES
    static const std::size_t TargetCount = 13+1;
    #else
    #ifndef MAGNUM_TARGET_GLES2
    static const std::size_t TargetCount = 8+1;
    #else
    static const std::size_t TargetCount = 2+1;
    #endif
    #endif

    /* Target <-> index mapping */
    static std::size_t indexForTarget(Buffer::Target target);
    static const Buffer::Target targetForIndex[TargetCount-1];

    inline constexpr BufferState(): bindings() {}

    /* Currently bound buffer for all targets */
    GLuint bindings[TargetCount];
};

}}

#endif
