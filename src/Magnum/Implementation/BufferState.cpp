/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "BufferState.h"

#include <Corrade/Utility/Assert.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "State.h"

namespace Magnum { namespace Implementation {

const Buffer::Target BufferState::targetForIndex[] = {
    Buffer::Target::Array,
    Buffer::Target::ElementArray,
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::Target::CopyRead,
    Buffer::Target::CopyWrite,
    Buffer::Target::PixelPack,
    Buffer::Target::PixelUnpack,
    Buffer::Target::TransformFeedback,
    Buffer::Target::Uniform,
    Buffer::Target::AtomicCounter,
    Buffer::Target::DispatchIndirect,
    Buffer::Target::DrawIndirect,
    Buffer::Target::ShaderStorage,
    #ifndef MAGNUM_TARGET_GLES
    Buffer::Target::Texture
    #endif
    #endif
};

std::size_t BufferState::indexForTarget(Buffer::Target target) {
    switch(target) {
        case Buffer::Target::Array:             return 1;
        case Buffer::Target::ElementArray:      return 2;
        #ifndef MAGNUM_TARGET_GLES2
        case Buffer::Target::CopyRead:          return 3;
        case Buffer::Target::CopyWrite:         return 4;
        case Buffer::Target::PixelPack:         return 5;
        case Buffer::Target::PixelUnpack:       return 6;
        case Buffer::Target::TransformFeedback: return 7;
        case Buffer::Target::Uniform:           return 8;
        case Buffer::Target::AtomicCounter:     return 9;
        case Buffer::Target::DispatchIndirect:  return 10;
        case Buffer::Target::DrawIndirect:      return 11;
        case Buffer::Target::ShaderStorage:     return 12;
        #ifndef MAGNUM_TARGET_GLES
        case Buffer::Target::Texture:           return 13;
        #endif
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE();
}

BufferState::BufferState(Context& context, std::vector<std::string>& extensions): bindings()
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    , minMapAlignment(0)
    #endif
    , maxAtomicCounterBindings{0}, maxShaderStorageBindings{0}, shaderStorageOffsetAlignment{0}, uniformOffsetAlignment{0}, maxUniformBindings{0}
    #endif
{
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        extensions.push_back(Extensions::GL::EXT::direct_state_access::string());

        copyImplementation = &Buffer::copyImplementationDSA;
        getParameterImplementation = &Buffer::getParameterImplementationDSA;
        getSubDataImplementation = &Buffer::getSubDataImplementationDSA;
        dataImplementation = &Buffer::dataImplementationDSA;
        subDataImplementation = &Buffer::subDataImplementationDSA;
        mapImplementation = &Buffer::mapImplementationDSA;
        mapRangeImplementation = &Buffer::mapRangeImplementationDSA;
        flushMappedRangeImplementation = &Buffer::flushMappedRangeImplementationDSA;
        unmapImplementation = &Buffer::unmapImplementationDSA;
    } else
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES2
        copyImplementation = &Buffer::copyImplementationDefault;
        #endif
        getParameterImplementation = &Buffer::getParameterImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        getSubDataImplementation = &Buffer::getSubDataImplementationDefault;
        #endif
        dataImplementation = &Buffer::dataImplementationDefault;
        subDataImplementation = &Buffer::subDataImplementationDefault;
        mapImplementation = &Buffer::mapImplementationDefault;
        mapRangeImplementation = &Buffer::mapRangeImplementationDefault;
        flushMappedRangeImplementation = &Buffer::flushMappedRangeImplementationDefault;
        unmapImplementation = &Buffer::unmapImplementationDefault;
    }

    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::invalidate_subdata>()) {
        extensions.push_back(Extensions::GL::ARB::invalidate_subdata::string());

        invalidateImplementation = &Buffer::invalidateImplementationARB;
        invalidateSubImplementation = &Buffer::invalidateSubImplementationARB;
    } else
    #endif
    {
        invalidateImplementation = &Buffer::invalidateImplementationNoOp;
        invalidateSubImplementation = &Buffer::invalidateSubImplementationNoOp;
    }

    #ifdef MAGNUM_TARGET_GLES
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif
}

void BufferState::reset() {
    std::fill_n(bindings, TargetCount, State::DisengagedBinding);
}

}}
