/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include <Corrade/Utility/Assert.h>

#include "Magnum/AbstractTexture.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/BufferTexture.h"
#endif
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "State.h"

namespace Magnum { namespace Implementation {

TextureState::TextureState(Context& context, std::vector<std::string>& extensions): maxSize{}, max3DSize{}, maxCubeMapSize{},
    #ifndef MAGNUM_TARGET_GLES2
    maxArrayLayers{},
    #endif
    #ifndef MAGNUM_TARGET_GLES
    maxRectangleSize{}, maxBufferSize{},
    #endif
    maxTextureUnits(0),
    #ifndef MAGNUM_TARGET_GLES2
    maxLodBias{0.0f},
    #endif
    maxMaxAnisotropy(0.0f), currentTextureUnit(0)
    #ifndef MAGNUM_TARGET_GLES2
    , maxColorSamples(0), maxDepthSamples(0), maxIntegerSamples(0)
    #endif
    #ifndef MAGNUM_TARGET_GLES
    , bufferOffsetAlignment(0)
    #endif
{
    /* Create implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
        extensions.push_back(Extensions::GL::ARB::direct_state_access::string());
        createImplementation = &AbstractTexture::createImplementationDSA;

    } else
    #endif
    {
        createImplementation = &AbstractTexture::createImplementationDefault;
    }

    /* Single bind implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
        /* Extension name added below */

        unbindImplementation = &AbstractTexture::unbindImplementationDSA;
        bindImplementation = &AbstractTexture::bindImplementationDSA;
    } else if(context.isExtensionSupported<Extensions::GL::ARB::multi_bind>()) {
        /* Extension name added below */

        unbindImplementation = &AbstractTexture::unbindImplementationMulti;
        bindImplementation = &AbstractTexture::bindImplementationMulti;

    } else if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        /* Extension name added below */

        unbindImplementation = &AbstractTexture::unbindImplementationDSAEXT;
        bindImplementation = &AbstractTexture::bindImplementationDSAEXT;

    } else
    #endif
    {
        unbindImplementation = &AbstractTexture::unbindImplementationDefault;
        bindImplementation = &AbstractTexture::bindImplementationDefault;
    }

    /* Multi bind implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::multi_bind>()) {
        extensions.push_back(Extensions::GL::ARB::multi_bind::string());

        bindMultiImplementation = &AbstractTexture::bindImplementationMulti;

    } else
    #endif
    {
        bindMultiImplementation = &AbstractTexture::bindImplementationFallback;
    }

    /* DSA/non-DSA implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
        extensions.push_back(Extensions::GL::ARB::direct_state_access::string());

        parameteriImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterfImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterivImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterfvImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterIuivImplementation = &AbstractTexture::parameterIImplementationDSA;
        parameterIivImplementation = &AbstractTexture::parameterIImplementationDSA;
        getLevelParameterivImplementation = &AbstractTexture::getLevelParameterImplementationDSA;
        mipmapImplementation = &AbstractTexture::mipmapImplementationDSA;
        subImage1DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage2DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage3DImplementation = &AbstractTexture::subImageImplementationDSA;

        setBufferImplementation = &BufferTexture::setBufferImplementationDSA;
        setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationDSA;

        getCubeImageSizeImplementation = &CubeMapTexture::getImageSizeImplementationDSA;
        cubeSubImageImplementation = &CubeMapTexture::subImageImplementationDSA;

    } else if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        extensions.push_back(Extensions::GL::EXT::direct_state_access::string());

        parameteriImplementation = &AbstractTexture::parameterImplementationDSAEXT;
        parameterfImplementation = &AbstractTexture::parameterImplementationDSAEXT;
        parameterivImplementation = &AbstractTexture::parameterImplementationDSAEXT;
        parameterfvImplementation = &AbstractTexture::parameterImplementationDSAEXT;
        parameterIuivImplementation = &AbstractTexture::parameterIImplementationDSAEXT;
        parameterIivImplementation = &AbstractTexture::parameterIImplementationDSAEXT;
        getLevelParameterivImplementation = &AbstractTexture::getLevelParameterImplementationDSAEXT;
        mipmapImplementation = &AbstractTexture::mipmapImplementationDSAEXT;
        subImage1DImplementation = &AbstractTexture::subImageImplementationDSAEXT;
        subImage2DImplementation = &AbstractTexture::subImageImplementationDSAEXT;
        subImage3DImplementation = &AbstractTexture::subImageImplementationDSAEXT;

        setBufferImplementation = &BufferTexture::setBufferImplementationDSAEXT;
        setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationDSAEXT;

        getCubeImageSizeImplementation = &CubeMapTexture::getImageSizeImplementationDSAEXT;
        cubeSubImageImplementation = &CubeMapTexture::subImageImplementationDSAEXT;

    } else
    #endif
    {
        parameteriImplementation = &AbstractTexture::parameterImplementationDefault;
        parameterfImplementation = &AbstractTexture::parameterImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES2
        parameterivImplementation = &AbstractTexture::parameterImplementationDefault;
        #endif
        parameterfvImplementation = &AbstractTexture::parameterImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        parameterIuivImplementation = &AbstractTexture::parameterIImplementationDefault;
        parameterIivImplementation = &AbstractTexture::parameterIImplementationDefault;
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        getLevelParameterivImplementation = &AbstractTexture::getLevelParameterImplementationDefault;
        #endif
        mipmapImplementation = &AbstractTexture::mipmapImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        subImage1DImplementation = &AbstractTexture::subImageImplementationDefault;
        #endif
        subImage2DImplementation = &AbstractTexture::subImageImplementationDefault;
        subImage3DImplementation = &AbstractTexture::subImageImplementationDefault;

        #ifndef MAGNUM_TARGET_GLES
        setBufferImplementation = &BufferTexture::setBufferImplementationDefault;
        setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationDefault;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        getCubeImageSizeImplementation = &CubeMapTexture::getImageSizeImplementationDefault;
        #endif
        cubeSubImageImplementation = &CubeMapTexture::subImageImplementationDefault;
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

    #ifndef MAGNUM_TARGET_GLES
    /* Image retrieval implementation */
    if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
        /* Extension name added above */
        getImageImplementation = &AbstractTexture::getImageImplementationDSA;

    } else if(context.isExtensionSupported<Extensions::GL::ARB::robustness>()) {
        extensions.push_back(Extensions::GL::ARB::robustness::string());
        getImageImplementation = &AbstractTexture::getImageImplementationRobustness;

    } else if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        /* Extension name added above */
        getImageImplementation = &AbstractTexture::getImageImplementationDSAEXT;

    } else getImageImplementation = &AbstractTexture::getImageImplementationDefault;

    /* Image retrieval implementation for cube map */
    if(context.isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>()) {
        extensions.push_back(Extensions::GL::ARB::get_texture_sub_image::string());
        getCubeImageImplementation = &CubeMapTexture::getImageImplementationDSA;

    } else if(context.isExtensionSupported<Extensions::GL::ARB::robustness>()) {
        /* Extension name added above */
        getCubeImageImplementation = &CubeMapTexture::getImageImplementationRobustness;

    } else if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        /* Extension name added above */
        getCubeImageImplementation = &CubeMapTexture::getImageImplementationDSAEXT;

    } else getCubeImageImplementation = &CubeMapTexture::getImageImplementationDefault;
    #endif

    /* Texture storage implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::texture_storage>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(context.isExtensionSupported<Extensions::GL::EXT::texture_storage>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.push_back(Extensions::GL::ARB::texture_storage::string());
        #elif defined(MAGNUM_TARGET_GLES2)
        extensions.push_back(Extensions::GL::EXT::texture_storage::string());
        #endif

        #ifndef MAGNUM_TARGET_GLES
        if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
            storage1DImplementation = &AbstractTexture::storageImplementationDSA;
            storage2DImplementation = &AbstractTexture::storageImplementationDSA;
            storage3DImplementation = &AbstractTexture::storageImplementationDSA;

        } else if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
            storage1DImplementation = &AbstractTexture::storageImplementationDSAEXT;
            storage2DImplementation = &AbstractTexture::storageImplementationDSAEXT;
            storage3DImplementation = &AbstractTexture::storageImplementationDSAEXT;
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
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    else {
        #ifndef MAGNUM_TARGET_GLES
        storage1DImplementation = &AbstractTexture::storageImplementationFallback;
        #endif
        storage2DImplementation = &AbstractTexture::storageImplementationFallback;
        storage3DImplementation = &AbstractTexture::storageImplementationFallback;
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /* Storage implementation for multisample textures. The fallback doesn't
       have DSA alternative, so it must be handled specially. */
    if(context.isExtensionSupported<Extensions::GL::ARB::texture_storage_multisample>()) {
        extensions.push_back(Extensions::GL::ARB::texture_storage_multisample::string());

        if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
            storage2DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDSA;
            storage3DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDSA;
        } else if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
            storage2DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDSAEXT;
            storage3DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDSAEXT;
        } else {
            storage2DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDefault;
            storage3DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDefault;
        }
    } else {
        storage2DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationFallback;
        storage3DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationFallback;
    }
    #elif !defined(MAGNUM_TARGET_GLES2)
    storage2DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDefault;
    #endif

    /* Anisotropic filter implementation */
    if(context.isExtensionSupported<Extensions::GL::EXT::texture_filter_anisotropic>()) {
        extensions.push_back(Extensions::GL::EXT::texture_filter_anisotropic::string());

        setMaxAnisotropyImplementation = &AbstractTexture::setMaxAnisotropyImplementationExt;
    } else setMaxAnisotropyImplementation = &AbstractTexture::setMaxAnisotropyImplementationNoOp;

    /* Resize bindings array to hold all possible texture units */
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    CORRADE_INTERNAL_ASSERT(maxTextureUnits > 0);
    bindings.resize(maxTextureUnits);
}

TextureState::~TextureState() = default;

void TextureState::reset() {
    std::fill_n(bindings.begin(), bindings.size(), std::pair<GLenum, GLuint>{{}, State::DisengagedBinding});
}

}}
