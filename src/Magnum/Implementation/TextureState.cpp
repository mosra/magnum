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

#include "TextureState.h"

#include "Magnum/AbstractTexture.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/BufferTexture.h"
#endif
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

namespace Magnum { namespace Implementation {

TextureState::TextureState(Context& context, std::vector<std::string>& extensions): maxMaxAnisotropy(0.0f), currentLayer(0)
    #ifndef MAGNUM_TARGET_GLES
    , maxColorSamples(0), maxDepthSamples(0), maxIntegerSamples(0), bufferOffsetAlignment(0)
    #endif
{
    /* DSA/non-DSA implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        extensions.push_back(Extensions::GL::EXT::direct_state_access::string());

        bindImplementation = &AbstractTexture::bindImplementationDSA;
        parameteriImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterfImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterfvImplementation = &AbstractTexture::parameterImplementationDSA;
        getLevelParameterivImplementation = &AbstractTexture::getLevelParameterImplementationDSA;
        mipmapImplementation = &AbstractTexture::mipmapImplementationDSA;
        getImageImplementation = &AbstractTexture::getImageImplementationDSA;
        image1DImplementation = &AbstractTexture::imageImplementationDSA;
        image2DImplementation = &AbstractTexture::imageImplementationDSA;
        image3DImplementation = &AbstractTexture::imageImplementationDSA;
        subImage1DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage2DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage3DImplementation = &AbstractTexture::subImageImplementationDSA;

        setBufferImplementation = &BufferTexture::setBufferImplementationDSA;
        setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationDSA;
    } else
    #endif
    {
        bindImplementation = &AbstractTexture::bindImplementationDefault;
        parameteriImplementation = &AbstractTexture::parameterImplementationDefault;
        parameterfImplementation = &AbstractTexture::parameterImplementationDefault;
        parameterfvImplementation = &AbstractTexture::parameterImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        getLevelParameterivImplementation = &AbstractTexture::getLevelParameterImplementationDefault;
        #endif
        mipmapImplementation = &AbstractTexture::mipmapImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        getImageImplementation = &AbstractTexture::getImageImplementationDefault;
        image1DImplementation = &AbstractTexture::imageImplementationDefault;
        #endif
        image2DImplementation = &AbstractTexture::imageImplementationDefault;
        image3DImplementation = &AbstractTexture::imageImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        subImage1DImplementation = &AbstractTexture::subImageImplementationDefault;
        #endif
        subImage2DImplementation = &AbstractTexture::subImageImplementationDefault;
        subImage3DImplementation = &AbstractTexture::subImageImplementationDefault;

        #ifndef MAGNUM_TARGET_GLES
        setBufferImplementation = &BufferTexture::setBufferImplementationDefault;
        setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationDefault;
        #endif
    }

    /* Data invalidation implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::invalidate_subdata>()) {
        extensions.push_back(Extensions::GL::ARB::invalidate_subdata::string());

        invalidateImageImplementation = &AbstractTexture::invalidateImageImplementationARB;
        invalidateSubImageImplementation = &AbstractTexture::invalidateSubImageImplementationARB;
    } else
    #endif
    {
        invalidateImageImplementation = &AbstractTexture::invalidateImageImplementationNoOp;
        invalidateSubImageImplementation = &AbstractTexture::invalidateSubImageImplementationNoOp;
    }

    /* Image retrieval implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::robustness>() &&
       !context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        extensions.push_back(Extensions::GL::ARB::robustness::string());

        getImageImplementation = &AbstractTexture::getImageImplementationRobustness;
    } else getImageImplementation = &AbstractTexture::getImageImplementationDefault;
    #endif

    /* Texture storage implementation */
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::texture_storage>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.push_back(Extensions::GL::ARB::texture_storage::string());
        #endif

        #ifndef MAGNUM_TARGET_GLES
        if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
            storage1DImplementation = &AbstractTexture::storageImplementationDSA;
            storage2DImplementation = &AbstractTexture::storageImplementationDSA;
            storage3DImplementation = &AbstractTexture::storageImplementationDSA;
        } else
        #endif
        {
            #ifndef MAGNUM_TARGET_GLES
            storage1DImplementation = &AbstractTexture::storageImplementationDefault;
            #endif
            storage2DImplementation = &AbstractTexture::storageImplementationDefault;
            storage3DImplementation = &AbstractTexture::storageImplementationDefault;
        }
    }
    #endif
    #ifndef MAGNUM_TARGET_GLES3
    #ifndef MAGNUM_TARGET_GLES
    else
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        storage1DImplementation = &AbstractTexture::storageImplementationFallback;
        #endif
        storage2DImplementation = &AbstractTexture::storageImplementationFallback;
        storage3DImplementation = &AbstractTexture::storageImplementationFallback;
    }
    #endif

    /* Anisotropic filter implementation */
    if(context.isExtensionSupported<Extensions::GL::EXT::texture_filter_anisotropic>()) {
        extensions.push_back(Extensions::GL::EXT::texture_filter_anisotropic::string());

        setMaxAnisotropyImplementation = &AbstractTexture::setMaxAnisotropyImplementationExt;
    } else setMaxAnisotropyImplementation = &AbstractTexture::setMaxAnisotropyImplementationNoOp;

    /* Resize bindings array to hold all possible layers */
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxLayers);
    bindings.resize(maxLayers);
}

TextureState::~TextureState() = default;

}}
