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

#include "RendererState.h"

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

namespace Magnum { namespace Implementation {

RendererState::RendererState(Context& context, std::vector<std::string>& extensions)
    #ifndef MAGNUM_TARGET_WEBGL
    : resetNotificationStrategy()
    #endif
{
    /* Float depth clear value implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::ES2_compatibility>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.emplace_back(Extensions::GL::ARB::ES2_compatibility::string());
        #endif

        clearDepthfImplementation = &Renderer::clearDepthfImplementationES;
    }
    #ifndef MAGNUM_TARGET_GLES
    else clearDepthfImplementation = &Renderer::clearDepthfImplementationDefault;
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /* Graphics reset status implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::robustness>())
    #else
    if(context.isExtensionSupported<Extensions::GL::EXT::robustness>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.emplace_back(Extensions::GL::ARB::robustness::string());
        #else
        extensions.push_back(Extensions::GL::EXT::robustness::string());
        #endif

        graphicsResetStatusImplementation = &Renderer::graphicsResetStatusImplementationRobustness;
    } else graphicsResetStatusImplementation = &Renderer::graphicsResetStatusImplementationDefault;
    #else
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif

    /* In case the extensions are not supported on ES2, row length is
       constantly 0 to avoid modifying that state */
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    unpackPixelStorage.disengagedRowLength = PixelStorage::DisengagedValue;
    packPixelStorage.disengagedRowLength = PixelStorage::DisengagedValue;
    #ifdef MAGNUM_TARGET_GLES2
    if(!context.isExtensionSupported<Extensions::GL::EXT::unpack_subimage>())
        unpackPixelStorage.disengagedRowLength = 0;
    if(!context.isExtensionSupported<Extensions::GL::NV::pack_subimage>())
        packPixelStorage.disengagedRowLength = 0;
    #endif
    #endif
}

RendererState::PixelStorage::PixelStorage():
    #ifndef MAGNUM_TARGET_GLES
    swapBytes{false},
    #endif
    alignment{4}
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    , rowLength{0}
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    , imageHeight{0},
    skip{0}
    #endif
    #ifndef MAGNUM_TARGET_GLES
    , compressedBlockSize{0},
    compressedBlockDataSize{0}
    #endif
    {}

void RendererState::PixelStorage::reset() {
    #ifndef MAGNUM_TARGET_GLES
    swapBytes = std::nullopt;
    #endif
    alignment = DisengagedValue;
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    /* Resets to 0 instead of DisengagedValue in case the EXT_unpack_subimage/
       NV_pack_image ES2 extension is not supported to avoid modifying that
       state */
    rowLength = disengagedRowLength;
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    imageHeight = DisengagedValue;
    skip = Vector3i{DisengagedValue};
    #endif
    #ifndef MAGNUM_TARGET_GLES
    compressedBlockSize = Vector3i{DisengagedValue};
    compressedBlockDataSize = DisengagedValue;
    #endif
}

}}
