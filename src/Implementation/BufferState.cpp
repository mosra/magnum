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

#include "BufferState.h"

#include <Utility/Debug.h>

namespace Magnum { namespace Implementation {

const Buffer::Target BufferState::targetForIndex[] = {
    Buffer::Target::Array,
    Buffer::Target::CopyRead,
    Buffer::Target::CopyWrite,
    Buffer::Target::ElementArray,
    Buffer::Target::PixelPack,
    Buffer::Target::PixelUnpack,
    Buffer::Target::TransformFeedback,
    Buffer::Target::Uniform,
    #ifndef MAGNUM_TARGET_GLES
    Buffer::Target::AtomicCounter,
    Buffer::Target::DispatchIndirect,
    Buffer::Target::DrawIndirect,
    Buffer::Target::ShaderStorage,
    Buffer::Target::Texture
    #endif
};

std::size_t BufferState::indexForTarget(Buffer::Target target) {
    switch(target) {
        case Buffer::Target::Array:             return 1;
        case Buffer::Target::CopyRead:          return 2;
        case Buffer::Target::CopyWrite:         return 3;
        case Buffer::Target::ElementArray:      return 4;
        case Buffer::Target::PixelPack:         return 5;
        case Buffer::Target::PixelUnpack:       return 6;
        case Buffer::Target::TransformFeedback: return 7;
        case Buffer::Target::Uniform:           return 8;
        #ifndef MAGNUM_TARGET_GLES
        case Buffer::Target::AtomicCounter:     return 9;
        case Buffer::Target::DispatchIndirect:  return 10;
        case Buffer::Target::DrawIndirect:      return 11;
        case Buffer::Target::ShaderStorage:     return 12;
        case Buffer::Target::Texture:           return 13;
        #endif
    }

    CORRADE_ASSERT(false, "Unknown Buffer target", 0);
    return 0;
}

}}
