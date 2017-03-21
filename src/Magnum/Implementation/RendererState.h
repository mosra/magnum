#ifndef Magnum_Implementation_RendererState_h
#define Magnum_Implementation_RendererState_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include <string>
#include <vector>

#include "Magnum/Renderer.h"
#include "Magnum/Math/Vector3.h"
#include "MagnumExternal/Optional/optional.hpp"

namespace Magnum { namespace Implementation {

struct RendererState {
    explicit RendererState(Context& context, std::vector<std::string>& extensions);

    void(*clearDepthfImplementation)(GLfloat);
    #ifndef MAGNUM_TARGET_WEBGL
    Renderer::GraphicsResetStatus(*graphicsResetStatusImplementation)();

    Renderer::ResetNotificationStrategy resetNotificationStrategy;
    #endif

    struct PixelStorage {
        enum: Int { DisengagedValue = -1 };

        explicit PixelStorage();

        void reset();

        #ifndef MAGNUM_TARGET_GLES
        std::optional<bool> swapBytes;
        #endif
        Int alignment;
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        Int rowLength;
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        Int imageHeight;
        Vector3i skip;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        Vector3i compressedBlockSize;
        Int compressedBlockDataSize;
        #endif

        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        Int disengagedRowLength;
        #endif
    };

    PixelStorage packPixelStorage, unpackPixelStorage;
};

}}

#endif
