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

#include "RendererState.h"

#include "Magnum/PixelStorage.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

namespace Magnum { namespace GL { namespace Implementation {

RendererState::RendererState(Context& context, ContextState& contextState, std::vector<std::string>& extensions)
    #ifndef MAGNUM_TARGET_WEBGL
    : resetNotificationStrategy()
    #endif
{
    /* Float depth clear value implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::ES2_compatibility>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.emplace_back(Extensions::ARB::ES2_compatibility::string());
        #endif

        clearDepthfImplementation = &Renderer::clearDepthfImplementationES;
    }
    #ifndef MAGNUM_TARGET_GLES
    else clearDepthfImplementation = &Renderer::clearDepthfImplementationDefault;
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /* Graphics reset status implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::robustness>())
    #else
    if(context.isExtensionSupported<Extensions::EXT::robustness>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.emplace_back(Extensions::ARB::robustness::string());
        #else
        extensions.push_back(Extensions::EXT::robustness::string());
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
    if(!context.isExtensionSupported<Extensions::EXT::unpack_subimage>())
        unpackPixelStorage.disengagedRowLength = 0;
    if(!context.isExtensionSupported<Extensions::NV::pack_subimage>())
        packPixelStorage.disengagedRowLength = 0;
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((context.detectedDriver() & Context::DetectedDriver::Mesa) &&
       (context.flags() & Context::Flag::ForwardCompatible) &&
        !context.isDriverWorkaroundDisabled("mesa-forward-compatible-line-width-range"))
        lineWidthRangeImplementation = &Renderer::lineWidthRangeImplementationMesaForwardCompatible;
    else
    #endif
    {
        lineWidthRangeImplementation = &Renderer::lineWidthRangeImplementationDefault;
    }

    #ifndef MAGNUM_TARGET_GLES
    minSampleShadingImplementation = &Renderer::minSampleShadingImplementationDefault;
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(context.isVersionSupported(Version::GLES320))
        minSampleShadingImplementation = &Renderer::minSampleShadingImplementationDefault;
    else if(context.isExtensionSupported<Extensions::OES::sample_shading>())
        minSampleShadingImplementation = &Renderer::minSampleShadingImplementationOES;
    else
        minSampleShadingImplementation = nullptr;
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifdef MAGNUM_TARGET_GLES
    if(context.isVersionSupported(Version::GLES320))
    #endif
    {
        patchParameteriImplementation = glPatchParameteri;
    }
    #ifdef MAGNUM_TARGET_GLES
    else {
        /* Not checking for the extension (nor adding it to the extension list)
           as this is not any optional feature -- it can be only used when
           the extension is present, and if it's not, the pointers are null */
        patchParameteriImplementation = glPatchParameteriEXT;
    }
    #endif
    #endif

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifdef MAGNUM_TARGET_GLES
    if(context.isVersionSupported(Version::GLES320))
    #endif
    {
        enableiImplementation = glEnablei;
        disableiImplementation = glDisablei;
        colorMaskiImplementation = glColorMaski;
        blendFunciImplementation = glBlendFunci;
        blendFuncSeparateiImplementation = glBlendFuncSeparatei;
        blendEquationiImplementation = glBlendEquationi;
        blendEquationSeparateiImplementation = glBlendEquationSeparatei;
    }
    #endif
    #ifdef MAGNUM_TARGET_GLES
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    else
    #endif
    {
        /* Not checking for the extension (nor adding it to the extension list)
           as this is not any optional feature -- it can be only used when
           the extension is present, and if it's not, the pointers are null */
        #ifndef MAGNUM_TARGET_WEBGL
        enableiImplementation = glEnableiEXT;
        disableiImplementation = glDisableiEXT;
        colorMaskiImplementation = glColorMaskiEXT;
        blendFunciImplementation = glBlendFunciEXT;
        blendFuncSeparateiImplementation = glBlendFuncSeparateiEXT;
        blendEquationiImplementation = glBlendEquationiEXT;
        blendEquationSeparateiImplementation = glBlendEquationSeparateiEXT;
        #else
        /* Emscripten doesn't support these yet (last checked Feb 2020) */
        enableiImplementation = nullptr;
        disableiImplementation = nullptr;
        colorMaskiImplementation = nullptr;
        blendFunciImplementation = nullptr;
        blendFuncSeparateiImplementation = nullptr;
        blendEquationiImplementation = nullptr;
        blendEquationSeparateiImplementation = nullptr;
        #endif
    }
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /* On compatibility profile we need to explicitly enable GL_POINT_SPRITE
       in order to have gl_PointCoord working (on NVidia at least, Mesa behaves
       as if it was always enabled). On core profile this is enabled
       implicitly, thus GL_POINT_SPRITE is not even in headers and calling
       glEnable(GL_POINT_SPRITE) would cause a GL error. See
       RendererGLTest::pointCoord() for more information. */
    if(!context.isCoreProfileInternal(contextState)) {
        glEnable(0x8861 /*GL_POINT_SPRITE*/);
    }
    #else
    static_cast<void>(contextState);
    #endif
}

RendererState::PixelStorage::PixelStorage():
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


void RendererState::applyPixelStorageInternal(const Magnum::PixelStorage& storage, const bool isUnpack) {
    PixelStorage& state = isUnpack ? unpackPixelStorage : packPixelStorage;

    /* Alignment */
    if(state.alignment == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.alignment != storage.alignment())
        glPixelStorei(isUnpack ? GL_UNPACK_ALIGNMENT : GL_PACK_ALIGNMENT,
            state.alignment = storage.alignment());

    /* Row length */
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    if(state.rowLength == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.rowLength != storage.rowLength())
    {
        /** @todo Use real value for GL_PACK_ROW_LENGTH_NV when it is in headers */
        #ifndef MAGNUM_TARGET_GLES2
        glPixelStorei(isUnpack ? GL_UNPACK_ROW_LENGTH : GL_PACK_ROW_LENGTH,
            state.rowLength = storage.rowLength());
        #elif !defined(MAGNUM_TARGET_WEBGL)
        glPixelStorei(isUnpack ? GL_UNPACK_ROW_LENGTH_EXT : 0xD02 /*GL_PACK_ROW_LENGTH_NV*/,
            state.rowLength = storage.rowLength());
        #endif
    }
    #else
    CORRADE_ASSERT(!storage.rowLength(),
        "GL: non-default PixelStorage::rowLength() is not supported in WebGL 1.0", );
    #endif

    /* Image height (not on ES2, on ES3 for unpack only) */
    #ifndef MAGNUM_TARGET_GLES2
    if(state.imageHeight == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.imageHeight != storage.imageHeight()) {
        #ifndef MAGNUM_TARGET_GLES
        glPixelStorei(isUnpack ? GL_UNPACK_IMAGE_HEIGHT : GL_PACK_IMAGE_HEIGHT,
            state.imageHeight = storage.imageHeight());
        #else
        if(isUnpack) glPixelStorei(GL_UNPACK_IMAGE_HEIGHT,
            state.imageHeight = storage.imageHeight());
        else CORRADE_ASSERT(!storage.imageHeight(),
            "GL: non-default PixelStorage::imageHeight() for pack is not supported in OpenGL ES", );
        #endif
    }
    #else
    CORRADE_ASSERT(!storage.imageHeight(),
        "GL: non-default PixelStorage::imageHeight() is not supported in OpenGL ES 2", );
    #endif

    /* On ES2 done by modifying data pointer */
    #ifndef MAGNUM_TARGET_GLES2
    /* Skip pixels */
    if(state.skip.x() == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.skip.x() != storage.skip().x())
        glPixelStorei(isUnpack ? GL_UNPACK_SKIP_PIXELS : GL_PACK_SKIP_PIXELS,
            state.skip.x() = storage.skip().x());

    /* Skip rows */
    if(state.skip.y() == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.skip.y() != storage.skip().y())
        glPixelStorei(isUnpack ? GL_UNPACK_SKIP_ROWS : GL_PACK_SKIP_ROWS,
            state.skip.y() = storage.skip().y());

    /* Skip images (on ES3 for unpack only) */
    if(state.skip.z() == GL::Implementation::RendererState::PixelStorage::DisengagedValue || state.skip.z() != storage.skip().z()) {
        #ifndef MAGNUM_TARGET_GLES
        glPixelStorei(isUnpack ? GL_UNPACK_SKIP_IMAGES : GL_PACK_SKIP_IMAGES,
            state.skip.z() = storage.skip().z());
        #else
        if(isUnpack) glPixelStorei(GL_UNPACK_SKIP_IMAGES,
            state.skip.z() = storage.skip().z());
        else CORRADE_ASSERT(!storage.skip().z(),
            "GL: non-default PixelStorage::skip().z() for pack is not supported in OpenGL ES", );
        #endif
    }
    #endif
}

void RendererState::applyPixelStorageInternal(const CompressedPixelStorage& storage, const bool isUnpack) {
    #ifdef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(storage == CompressedPixelStorage{},
        "GL: non-default CompressedPixelStorage parameters are not supported on OpenGLES or WebGL", );
    static_cast<void>(isUnpack);
    #else
    applyPixelStorageInternal(static_cast<const Magnum::PixelStorage&>(storage), isUnpack);

    PixelStorage& state = isUnpack ? unpackPixelStorage : packPixelStorage;

    /* Compressed block width */
    if(state.compressedBlockSize.x() == PixelStorage::DisengagedValue ||
       state.compressedBlockSize.x() != storage.compressedBlockSize().x())
        glPixelStorei(isUnpack ? GL_UNPACK_COMPRESSED_BLOCK_WIDTH : GL_PACK_COMPRESSED_BLOCK_WIDTH,
            state.compressedBlockSize.x() = storage.compressedBlockSize().x());

    /* Compressed block height */
    if(state.compressedBlockSize.y() == PixelStorage::DisengagedValue ||
       state.compressedBlockSize.y() != storage.compressedBlockSize().y())
        glPixelStorei(isUnpack ? GL_UNPACK_COMPRESSED_BLOCK_HEIGHT : GL_PACK_COMPRESSED_BLOCK_HEIGHT,
            state.compressedBlockSize.y() = storage.compressedBlockSize().y());

    /* Compressed block depth */
    if(state.compressedBlockSize.z() == PixelStorage::DisengagedValue ||
       state.compressedBlockSize.z() != storage.compressedBlockSize().z())
        glPixelStorei(isUnpack ? GL_UNPACK_COMPRESSED_BLOCK_DEPTH : GL_PACK_COMPRESSED_BLOCK_DEPTH,
            state.compressedBlockSize.z() = storage.compressedBlockSize().z());

    /* Compressed block size */
    if(state.compressedBlockDataSize == PixelStorage::DisengagedValue ||
       state.compressedBlockDataSize != storage.compressedBlockDataSize())
        glPixelStorei(isUnpack ? GL_UNPACK_COMPRESSED_BLOCK_SIZE : GL_PACK_COMPRESSED_BLOCK_SIZE,
            state.compressedBlockDataSize = storage.compressedBlockDataSize());
    #endif
}

}}}
