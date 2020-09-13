/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/State.h"

namespace Magnum { namespace GL { namespace Implementation {

const Buffer::TargetHint BufferState::targetForIndex[] = {
    Buffer::TargetHint::Array,
    Buffer::TargetHint::ElementArray,
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::TargetHint::CopyRead,
    Buffer::TargetHint::CopyWrite,
    Buffer::TargetHint::PixelPack,
    Buffer::TargetHint::PixelUnpack,
    Buffer::TargetHint::TransformFeedback,
    Buffer::TargetHint::Uniform,
    #ifndef MAGNUM_TARGET_WEBGL
    Buffer::TargetHint::AtomicCounter,
    Buffer::TargetHint::DispatchIndirect,
    Buffer::TargetHint::DrawIndirect,
    Buffer::TargetHint::ShaderStorage,
    Buffer::TargetHint::Texture
    #endif
    #endif
};

std::size_t BufferState::indexForTarget(Buffer::TargetHint target) {
    switch(target) {
        case Buffer::TargetHint::Array:             return 1;
        case Buffer::TargetHint::ElementArray:      return 2;
        #ifndef MAGNUM_TARGET_GLES2
        case Buffer::TargetHint::CopyRead:          return 3;
        case Buffer::TargetHint::CopyWrite:         return 4;
        case Buffer::TargetHint::PixelPack:         return 5;
        case Buffer::TargetHint::PixelUnpack:       return 6;
        case Buffer::TargetHint::TransformFeedback: return 7;
        case Buffer::TargetHint::Uniform:           return 8;
        #ifndef MAGNUM_TARGET_WEBGL
        case Buffer::TargetHint::AtomicCounter:     return 9;
        case Buffer::TargetHint::DispatchIndirect:  return 10;
        case Buffer::TargetHint::DrawIndirect:      return 11;
        case Buffer::TargetHint::ShaderStorage:     return 12;
        case Buffer::TargetHint::Texture:           return 13;
        #endif
        #endif
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

BufferState::BufferState(Context& context, std::vector<std::string>& extensions): bindings()
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    , minMapAlignment(0)
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    , maxAtomicCounterBindings{0}, maxShaderStorageBindings{0}, shaderStorageOffsetAlignment{0}
    #endif
    , uniformOffsetAlignment{0}, maxUniformBindings{0}
    #endif
{
    /* DSA, except Intel Windows, because I have no patience for that anymore */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()
        #ifdef CORRADE_TARGET_WINDOWS
        && (!(context.detectedDriver() & Context::DetectedDriver::IntelWindows) ||
        context.isDriverWorkaroundDisabled("intel-windows-crazy-broken-buffer-dsa"))
        #endif
    ) {
        extensions.emplace_back(Extensions::ARB::direct_state_access::string());

        createImplementation = &Buffer::createImplementationDSA;
        copyImplementation = &Buffer::copyImplementationDSA;
        #ifndef MAGNUM_TARGET_GLES
        storageImplementation = &Buffer::storageImplementationDSA;
        #endif
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
        createImplementation = &Buffer::createImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES2
        copyImplementation = &Buffer::copyImplementationDefault;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        storageImplementation = &Buffer::storageImplementationDefault;
        #endif
        getParameterImplementation = &Buffer::getParameterImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        getSubDataImplementation = &Buffer::getSubDataImplementationDefault;
        #endif
        dataImplementation = &Buffer::dataImplementationDefault;
        subDataImplementation = &Buffer::subDataImplementationDefault;
        #ifndef MAGNUM_TARGET_WEBGL
        mapImplementation = &Buffer::mapImplementationDefault;
        mapRangeImplementation = &Buffer::mapRangeImplementationDefault;
        flushMappedRangeImplementation = &Buffer::flushMappedRangeImplementationDefault;
        unmapImplementation = &Buffer::unmapImplementationDefault;
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::invalidate_subdata>()) {
        extensions.emplace_back(Extensions::ARB::invalidate_subdata::string());

        invalidateImplementation = &Buffer::invalidateImplementationARB;
        invalidateSubImplementation = &Buffer::invalidateSubImplementationARB;
    } else
    #endif
    {
        invalidateImplementation = &Buffer::invalidateImplementationNoOp;
        invalidateSubImplementation = &Buffer::invalidateSubImplementationNoOp;
    }

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::multi_bind>()) {
        extensions.emplace_back(Extensions::ARB::multi_bind::string());

        bindBasesImplementation = &Buffer::bindImplementationMulti;
        bindRangesImplementation = &Buffer::bindImplementationMulti;
    } else
    #endif
    {
        bindBasesImplementation = &Buffer::bindImplementationFallback;
        bindRangesImplementation = &Buffer::bindImplementationFallback;
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>() &&
      (context.detectedDriver() & Context::DetectedDriver::Svga3D) &&
      !context.isDriverWorkaroundDisabled("svga3d-broken-dsa-bufferdata"))
    {
        dataImplementation = &Buffer::dataImplementationDefault;
    }
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if((context.detectedDriver() & Context::DetectedDriver::SwiftShader) &&
      !context.isDriverWorkaroundDisabled("swiftshader-broken-xfb-buffer-binding-target"))
    {
        setTargetHintImplementation = &Buffer::setTargetHintImplementationSwiftShader;
    } else
    #endif
    {
        setTargetHintImplementation = &Buffer::setTargetHintImplementationDefault;
    }

    #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
    if(!context.isDriverWorkaroundDisabled("apple-buffer-texture-unbind-on-buffer-modify")) {
        dataImplementation = &Buffer::dataImplementationApple;
        subDataImplementation = &Buffer::subDataImplementationApple;
        mapImplementation = &Buffer::mapImplementationApple;
        mapRangeImplementation = &Buffer::mapRangeImplementationApple;
        unmapImplementation = &Buffer::unmapImplementationApple;
        /* No need for Apple-specific invalidate*Implementation, as the
           extension isn't supported anyway */
        CORRADE_INTERNAL_ASSERT(!context.isExtensionSupported<Extensions::ARB::invalidate_subdata>());
    }
    #endif

    #ifdef MAGNUM_TARGET_GLES
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif
}

void BufferState::reset() {
    /* libc++ complains about decrementing enum value otherwise */
    std::fill_n(bindings, std::size_t{TargetCount}, State::DisengagedBinding);
}

}}}
