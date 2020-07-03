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

#include "TextureState.h"

#include <tuple>
#include <Corrade/Utility/Assert.h>

#include "Magnum/GL/AbstractTexture.h"
#include "Magnum/GL/CubeMapTexture.h"
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/BufferTexture.h"
#endif
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"

#include "State.h"

namespace Magnum { namespace GL { namespace Implementation {

TextureState::TextureState(Context& context, std::vector<std::string>& extensions): maxSize{},
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    max3DSize{},
    #endif
    maxCubeMapSize{},
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
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        extensions.emplace_back(Extensions::ARB::direct_state_access::string());
        createImplementation = &AbstractTexture::createImplementationDSA;

    } else
    #endif
    {
        createImplementation = &AbstractTexture::createImplementationDefault;
    }

    /* Single bind implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        /* Extension name added below */

        #ifdef CORRADE_TARGET_WINDOWS
        if((context.detectedDriver() & Context::DetectedDriver::IntelWindows) &&
            !context.isDriverWorkaroundDisabled("intel-windows-half-baked-dsa-texture-bind"))
        {
            unbindImplementation = &AbstractTexture::unbindImplementationDefault;
            bindImplementation = &AbstractTexture::bindImplementationDSAIntelWindows;
        } else
        #endif
        {
            unbindImplementation = &AbstractTexture::unbindImplementationDSA;
            bindImplementation = &AbstractTexture::bindImplementationDSA;
        }

    } else if(context.isExtensionSupported<Extensions::ARB::multi_bind>()) {
        /* Extension name added below */

        unbindImplementation = &AbstractTexture::unbindImplementationMulti;
        bindImplementation = &AbstractTexture::bindImplementationMulti;

    } else
    #endif
    {
        unbindImplementation = &AbstractTexture::unbindImplementationDefault;
        /* This is additionally modified below for the
           apple-buffer-texture-unbind-on-buffer-modify workaround */
        bindImplementation = &AbstractTexture::bindImplementationDefault;
    }

    /* Multi bind implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::multi_bind>()) {
        extensions.emplace_back(Extensions::ARB::multi_bind::string());

        bindMultiImplementation = &AbstractTexture::bindImplementationMulti;

    } else
    #endif
    {
        bindMultiImplementation = &AbstractTexture::bindImplementationFallback;
    }

    /* DSA/non-DSA implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        extensions.emplace_back(Extensions::ARB::direct_state_access::string());

        parameteriImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterfImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterivImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterfvImplementation = &AbstractTexture::parameterImplementationDSA;
        parameterIuivImplementation = &AbstractTexture::parameterIImplementationDSA;
        parameterIivImplementation = &AbstractTexture::parameterIImplementationDSA;
        getLevelParameterivImplementation = &AbstractTexture::getLevelParameterImplementationDSA;
        mipmapImplementation = &AbstractTexture::mipmapImplementationDSA;
        subImage1DImplementation = &AbstractTexture::subImageImplementationDSA;
        subImage2DImplementation = &AbstractTexture::subImage2DImplementationDSA;
        subImage3DImplementation = &AbstractTexture::subImage3DImplementationDSA;
        compressedSubImage1DImplementation = &AbstractTexture::compressedSubImageImplementationDSA;
        compressedSubImage2DImplementation = &AbstractTexture::compressedSubImageImplementationDSA;
        compressedSubImage3DImplementation = &AbstractTexture::compressedSubImageImplementationDSA;

        setBufferImplementation = &BufferTexture::setBufferImplementationDSA;
        setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationDSA;

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
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        getLevelParameterivImplementation = &AbstractTexture::getLevelParameterImplementationDefault;
        #endif
        mipmapImplementation = &AbstractTexture::mipmapImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        subImage1DImplementation = &AbstractTexture::subImageImplementationDefault;
        compressedSubImage1DImplementation = &AbstractTexture::compressedSubImageImplementationDefault;
        #endif
        subImage2DImplementation = &AbstractTexture::subImage2DImplementationDefault;
        compressedSubImage2DImplementation = &AbstractTexture::compressedSubImageImplementationDefault;
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        subImage3DImplementation = &AbstractTexture::subImage3DImplementationDefault;
        compressedSubImage3DImplementation = &AbstractTexture::compressedSubImageImplementationDefault;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        setBufferImplementation = &BufferTexture::setBufferImplementationDefault;
        setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationDefault;
        #endif
    }

    /* DSA/non-DSA implementation for cubemaps, because Intel (and AMD) Windows
       drivers have to be broken in a special way */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {

        #ifdef CORRADE_TARGET_WINDOWS
        if((context.detectedDriver() & Context::DetectedDriver::IntelWindows) && !context.isDriverWorkaroundDisabled("intel-windows-broken-dsa-for-cubemaps")) {
            getCubeLevelParameterivImplementation = &CubeMapTexture::getLevelParameterImplementationDefault;
            cubeSubImageImplementation = &CubeMapTexture::subImageImplementationDefault;
            cubeCompressedSubImageImplementation = &CubeMapTexture::compressedSubImageImplementationDefault;
        } else if((context.detectedDriver() & Context::DetectedDriver::Amd) && !context.isDriverWorkaroundDisabled("amd-windows-cubemap-image3d-slice-by-slice")) {
            /* This one is not broken, but the others are */
            getCubeLevelParameterivImplementation = &CubeMapTexture::getLevelParameterImplementationDSA;
            cubeSubImageImplementation = &CubeMapTexture::subImageImplementationDefault;
            cubeCompressedSubImageImplementation = &CubeMapTexture::compressedSubImageImplementationDefault;
        } else
        #endif
        {
            /* Extension name added above */

            getCubeLevelParameterivImplementation = &CubeMapTexture::getLevelParameterImplementationDSA;
            cubeSubImageImplementation = &CubeMapTexture::subImageImplementationDSA;
            cubeCompressedSubImageImplementation = &CubeMapTexture::compressedSubImageImplementationDSA;
        }
    } else
    #endif
    {
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        getCubeLevelParameterivImplementation = &CubeMapTexture::getLevelParameterImplementationDefault;
        #endif
        cubeSubImageImplementation = &CubeMapTexture::subImageImplementationDefault;
        cubeCompressedSubImageImplementation = &CubeMapTexture::compressedSubImageImplementationDefault;
    }

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Integer parameter implementation for ES3 */
    if(context.isVersionSupported(Version::GLES320)) {
        parameterIuivImplementation = &AbstractTexture::parameterIImplementationDefault;
        parameterIivImplementation = &AbstractTexture::parameterIImplementationDefault;
    } else if(context.isExtensionSupported<Extensions::EXT::texture_border_clamp>()) {
        parameterIuivImplementation = &AbstractTexture::parameterIImplementationEXT;
        parameterIivImplementation = &AbstractTexture::parameterIImplementationEXT;
    } else {
        parameterIuivImplementation = nullptr;
        parameterIivImplementation = nullptr;
    }

    /* Buffer texture implementation for ES3 */
    if(context.isVersionSupported(Version::GLES320)) {
        setBufferImplementation = &BufferTexture::setBufferImplementationDefault;
        setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationDefault;
    } else if(context.isExtensionSupported<Extensions::EXT::texture_buffer>()) {
        setBufferImplementation = &BufferTexture::setBufferImplementationEXT;
        setBufferRangeImplementation = &BufferTexture::setBufferRangeImplementationEXT;
    } else {
        setBufferImplementation = nullptr;
        setBufferRangeImplementation = nullptr;
    }
    #endif

    /* Data invalidation implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::invalidate_subdata>()) {
        extensions.emplace_back(Extensions::ARB::invalidate_subdata::string());

        invalidateImageImplementation = &AbstractTexture::invalidateImageImplementationARB;
        invalidateSubImageImplementation = &AbstractTexture::invalidateSubImageImplementationARB;
    } else
    #endif
    {
        invalidateImageImplementation = &AbstractTexture::invalidateImageImplementationNoOp;
        invalidateSubImageImplementation = &AbstractTexture::invalidateSubImageImplementationNoOp;
    }

    #ifndef MAGNUM_TARGET_GLES
    /* Compressed cubemap image size query implementation (extensions added
       above) */
    if((context.detectedDriver() & Context::DetectedDriver::NVidia) &&
        !context.isDriverWorkaroundDisabled("nv-cubemap-inconsistent-compressed-image-size")) {
        if(context.isExtensionSupported<Extensions::ARB::direct_state_access>())
            getCubeLevelCompressedImageSizeImplementation = &CubeMapTexture::getLevelCompressedImageSizeImplementationDSANonImmutableWorkaround;
        else getCubeLevelCompressedImageSizeImplementation = &CubeMapTexture::getLevelCompressedImageSizeImplementationDefaultImmutableWorkaround;
    } else if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()
        #ifdef CORRADE_TARGET_WINDOWS
        && (!(context.detectedDriver() & Context::DetectedDriver::IntelWindows) ||
            context.isDriverWorkaroundDisabled("intel-windows-broken-dsa-for-cubemaps"))
        #endif
    ) {
        getCubeLevelCompressedImageSizeImplementation = &CubeMapTexture::getLevelCompressedImageSizeImplementationDSA;
    } else {
        getCubeLevelCompressedImageSizeImplementation = &CubeMapTexture::getLevelCompressedImageSizeImplementationDefault;
    }

    /* Image retrieval implementation */
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        /* Extension name added above */
        getImageImplementation = &AbstractTexture::getImageImplementationDSA;
        getCompressedImageImplementation = &AbstractTexture::getCompressedImageImplementationDSA;

    } else if(context.isExtensionSupported<Extensions::ARB::robustness>()) {
        extensions.emplace_back(Extensions::ARB::robustness::string());
        getImageImplementation = &AbstractTexture::getImageImplementationRobustness;
        getCompressedImageImplementation = &AbstractTexture::getCompressedImageImplementationRobustness;

    } else {
        getImageImplementation = &AbstractTexture::getImageImplementationDefault;
        getCompressedImageImplementation = &AbstractTexture::getCompressedImageImplementationDefault;
    }

    /* Image retrieval implementation for cube map */
    if(context.isExtensionSupported<Extensions::ARB::get_texture_sub_image>()) {
        extensions.emplace_back(Extensions::ARB::get_texture_sub_image::string());
        getCubeImageImplementation = &CubeMapTexture::getImageImplementationDSA;
        getCompressedCubeImageImplementation = &CubeMapTexture::getCompressedImageImplementationDSA;

    } else if(context.isExtensionSupported<Extensions::ARB::robustness>()) {
        /* Extension name added above */
        getCubeImageImplementation = &CubeMapTexture::getImageImplementationRobustness;
        getCompressedCubeImageImplementation = &CubeMapTexture::getCompressedImageImplementationRobustness;

    } else {
        getCubeImageImplementation = &CubeMapTexture::getImageImplementationDefault;
        getCompressedCubeImageImplementation = &CubeMapTexture::getCompressedImageImplementationDefault;
    }

    /* Full compressed cubemap image query implementation (extensions added
       above) */
    if((context.detectedDriver() & Context::DetectedDriver::NVidia) &&
        context.isExtensionSupported<Extensions::ARB::direct_state_access>() &&
        !context.isDriverWorkaroundDisabled("nv-cubemap-broken-full-compressed-image-query"))
        getFullCompressedCubeImageImplementation = &CubeMapTexture::getCompressedImageImplementationDSASingleSliceWorkaround;
    else
        getFullCompressedCubeImageImplementation = &CubeMapTexture::getCompressedImageImplementationDSA;

    if((context.detectedDriver() & Context::DetectedDriver::Amd) &&
        context.isExtensionSupported<Extensions::ARB::direct_state_access>() &&
        !context.isDriverWorkaroundDisabled("amd-windows-cubemap-image3d-slice-by-slice"))
        getFullCubeImageImplementation = &CubeMapTexture::getImageImplementationDSAAmdSliceBySlice;
    else if((context.detectedDriver() & Context::DetectedDriver::IntelWindows) &&
        context.isExtensionSupported<Extensions::ARB::direct_state_access>() &&
        !context.isDriverWorkaroundDisabled("intel-windows-broken-dsa-for-cubemaps"))
        getFullCubeImageImplementation = &CubeMapTexture::getImageImplementationSliceBySlice;
    else
        getFullCubeImageImplementation = &CubeMapTexture::getImageImplementationDSA;
    #endif

    /* Texture storage implementation for desktop and ES */
    #ifndef MAGNUM_TARGET_WEBGL
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::texture_storage>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(context.isExtensionSupported<Extensions::EXT::texture_storage>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.emplace_back(Extensions::ARB::texture_storage::string());
        #elif defined(MAGNUM_TARGET_GLES2)
        extensions.push_back(Extensions::EXT::texture_storage::string());
        #endif

        #ifndef MAGNUM_TARGET_GLES
        if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
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
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    else {
        #ifndef MAGNUM_TARGET_GLES
        storage1DImplementation = &AbstractTexture::storageImplementationFallback;
        #endif
        storage2DImplementation = &AbstractTexture::storageImplementationFallback;
        storage3DImplementation = &AbstractTexture::storageImplementationFallback;
    }
    #endif

    /* Texture storage implementation for WebGL 1.0 */
    #elif defined(MAGNUM_TARGET_GLES2)
    storage2DImplementation = &AbstractTexture::storageImplementationFallback;

    /* Texture storage implementation for WebGL 2.0 */
    #else
    storage2DImplementation = &AbstractTexture::storageImplementationDefault;
    storage3DImplementation = &AbstractTexture::storageImplementationDefault;
    #endif

    #ifndef MAGNUM_TARGET_GLES
    /* Storage implementation for multisample textures. The fallback doesn't
       have DSA alternative, so it must be handled specially. */
    if(context.isExtensionSupported<Extensions::ARB::texture_storage_multisample>()) {
        extensions.emplace_back(Extensions::ARB::texture_storage_multisample::string());

        if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
            storage2DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDSA;
            storage3DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDSA;
        } else {
            storage2DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDefault;
            storage3DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDefault;
        }
    } else {
        storage2DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationFallback;
        storage3DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationFallback;
    }
    #elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    storage2DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDefault;

    if(context.isVersionSupported(Version::GLES320))
        storage3DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationDefault;
    else if(context.isExtensionSupported<Extensions::OES::texture_storage_multisample_2d_array>())
        storage3DMultisampleImplementation = &AbstractTexture::storageMultisampleImplementationOES;
    else
        storage3DMultisampleImplementation = nullptr;
    #endif

    /* Anisotropic filter implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::EXT::texture_filter_anisotropic>()) {
        extensions.emplace_back(Extensions::ARB::texture_filter_anisotropic::string());

        setMaxAnisotropyImplementation = &AbstractTexture::setMaxAnisotropyImplementationArb;
    } else
    #endif
    if(context.isExtensionSupported<Extensions::EXT::texture_filter_anisotropic>()) {
        extensions.emplace_back(Extensions::EXT::texture_filter_anisotropic::string());

        setMaxAnisotropyImplementation = &AbstractTexture::setMaxAnisotropyImplementationExt;
    } else setMaxAnisotropyImplementation = &AbstractTexture::setMaxAnisotropyImplementationNoOp;

    #ifndef MAGNUM_TARGET_GLES
    /* NVidia workaround for compressed block data size implementation */
    if((context.detectedDriver() & Context::DetectedDriver::NVidia) &&
        !context.isDriverWorkaroundDisabled("nv-compressed-block-size-in-bits"))
        compressedBlockDataSizeImplementation = &AbstractTexture::compressedBlockDataSizeImplementationBitsWorkaround;
    else
        compressedBlockDataSizeImplementation = &AbstractTexture::compressedBlockDataSizeImplementationDefault;
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /* SVGA3D workaround for array / 3D / cube map texture upload. Overrides
       the DSA / non-DSA function pointers set above. */
    if((context.detectedDriver() & Context::DetectedDriver::Svga3D) &&
       !context.isDriverWorkaroundDisabled("svga3d-texture-upload-slice-by-slice")) {
        #ifndef MAGNUM_TARGET_GLES
        image2DImplementation = &AbstractTexture::imageImplementationSvga3DSliceBySlice;
        #endif
        image3DImplementation = &AbstractTexture::imageImplementationSvga3DSliceBySlice;
        #ifndef MAGNUM_TARGET_GLES
        if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
            #ifndef MAGNUM_TARGET_GLES
            subImage2DImplementation = &AbstractTexture::subImageImplementationSvga3DSliceBySlice<&AbstractTexture::subImage2DImplementationDSA>;
            #endif
            subImage3DImplementation = &AbstractTexture::subImageImplementationSvga3DSliceBySlice<&AbstractTexture::subImage3DImplementationDSA>;

        } else
        #endif
        {
            #ifndef MAGNUM_TARGET_GLES
            subImage2DImplementation = &AbstractTexture::subImageImplementationSvga3DSliceBySlice<&AbstractTexture::subImage2DImplementationDefault>;
            #endif
            subImage3DImplementation = &AbstractTexture::subImageImplementationSvga3DSliceBySlice<&AbstractTexture::subImage3DImplementationDefault>;
        }
    } else
    #endif
    {
        /* These need to be set up in any case */
        image2DImplementation = &AbstractTexture::imageImplementationDefault;
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        image3DImplementation = &AbstractTexture::imageImplementationDefault;
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    /* SVGA3D and Intel workaround for cube map texture upload. Overrides the
       DSA / non-DSA function pointers set above. */
    if((context.detectedDriver() & Context::DetectedDriver::Svga3D) &&
       !context.isDriverWorkaroundDisabled("svga3d-texture-upload-slice-by-slice")) {
        if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
            cubeSubImage3DImplementation = &CubeMapTexture::subImageImplementationDSASliceBySlice;
        } else {
            cubeSubImage3DImplementation = &CubeMapTexture::subImageImplementationSliceBySlice;
        }
    } else if((context.detectedDriver() & Context::DetectedDriver::IntelWindows) &&
       !context.isDriverWorkaroundDisabled("intel-windows-broken-dsa-for-cubemaps")) {
        cubeSubImage3DImplementation = &CubeMapTexture::subImageImplementationSliceBySlice;
    } else if((context.detectedDriver() & Context::DetectedDriver::Amd) &&
       !context.isDriverWorkaroundDisabled("amd-windows-cubemap-image3d-slice-by-slice")) {
        /* DSA version is broken (non-zero Z offset not allowed), need to
           emulate using classic APIs */
        cubeSubImage3DImplementation = &CubeMapTexture::subImageImplementationSliceBySlice;
    } else if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        cubeSubImage3DImplementation = &CubeMapTexture::subImageImplementationDSA;
    } else
    {
        cubeSubImage3DImplementation = &CubeMapTexture::subImageImplementationSliceBySlice;
    }
    #endif

    /* Allocate texture bindings array to hold all possible texture units */
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    CORRADE_INTERNAL_ASSERT(maxTextureUnits > 0);
    bindings = Containers::Array<std::pair<GLenum, GLuint>>{Containers::ValueInit, std::size_t(maxTextureUnits)};

    #if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
    if(!context.isDriverWorkaroundDisabled("apple-buffer-texture-unbind-on-buffer-modify")) {
        CORRADE_INTERNAL_ASSERT(std::size_t(maxTextureUnits) <= decltype(bufferTextureBound)::Size);
        /* Assume ARB_multi_bind is not supported, otherwise we'd need to
           implement the workaround also for bindMultiImplementation */
        CORRADE_INTERNAL_ASSERT(!context.isExtensionSupported<Extensions::ARB::multi_bind>());
        bindImplementation = &AbstractTexture::bindImplementationAppleBufferTextureWorkaround;
        bindInternalImplementation = &AbstractTexture::bindImplementationAppleBufferTextureWorkaround;
    } else
    #endif
    {
        /* bindImplementation already set above */
        bindInternalImplementation = &AbstractTexture::bindImplementationDefault;
    }

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Allocate image bindings array to hold all possible image units */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::shader_image_load_store>())
    #else
    if(context.isVersionSupported(Version::GLES310))
    #endif
    {
        GLint maxImageUnits;
        glGetIntegerv(GL_MAX_IMAGE_UNITS, &maxImageUnits);
        imageBindings = Containers::Array<std::tuple<GLuint, GLint, GLboolean, GLint, GLenum>>{Containers::ValueInit, std::size_t(maxImageUnits)};
    }
    #endif
}

TextureState::~TextureState() = default;

void TextureState::reset() {
    std::fill_n(bindings.begin(), bindings.size(), std::pair<GLenum, GLuint>{{}, State::DisengagedBinding});
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    std::fill_n(imageBindings.begin(), imageBindings.size(), std::tuple<GLuint, GLint, GLboolean, GLint, GLenum>{State::DisengagedBinding, 0, false, 0, 0});
    #endif
}

}}}
