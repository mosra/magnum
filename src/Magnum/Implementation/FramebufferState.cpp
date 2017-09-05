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

#include "FramebufferState.h"

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Renderbuffer.h"

#include "State.h"

namespace Magnum { namespace Implementation {

constexpr const Range2Di FramebufferState::DisengagedViewport;

FramebufferState::FramebufferState(Context& context, std::vector<std::string>& extensions): readBinding{0}, drawBinding{0}, renderbufferBinding{0}, maxDrawBuffers{0}, maxColorAttachments{0}, maxRenderbufferSize{0},
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    maxSamples{0},
    #endif
    #ifndef MAGNUM_TARGET_GLES
    maxDualSourceDrawBuffers{0},
    #endif
    viewport{DisengagedViewport}
{
    /* Create implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
        extensions.emplace_back(Extensions::GL::ARB::direct_state_access::string());
        createImplementation = &Framebuffer::createImplementationDSA;
        createRenderbufferImplementation = &Renderbuffer::createImplementationDSA;

    } else
    #endif
    {
        createImplementation = &Framebuffer::createImplementationDefault;
        createRenderbufferImplementation = &Renderbuffer::createImplementationDefault;
    }

    /* DSA/non-DSA implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
        /* Extension added above */

        checkStatusImplementation = &AbstractFramebuffer::checkStatusImplementationDSA;

        clearIImplementation = &AbstractFramebuffer::clearImplementationDSA;
        clearUIImplementation = &AbstractFramebuffer::clearImplementationDSA;
        clearFImplementation = &AbstractFramebuffer::clearImplementationDSA;
        clearFIImplementation = &AbstractFramebuffer::clearImplementationDSA;

        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationDSA;
        drawBufferImplementation = &AbstractFramebuffer::drawBufferImplementationDSA;
        readBufferImplementation = &AbstractFramebuffer::readBufferImplementationDSA;

        copySub1DImplementation = &AbstractFramebuffer::copySub1DImplementationDSA;
        copySub2DImplementation = &AbstractFramebuffer::copySub2DImplementationDSA;
        copySubCubeMapImplementation = &AbstractFramebuffer::copySubCubeMapImplementationDSA;
        copySub3DImplementation = &AbstractFramebuffer::copySub3DImplementationDSA;

        renderbufferImplementation = &Framebuffer::renderbufferImplementationDSA;
        /* The 1D implementation uses the same function as the layered attachment */
        texture1DImplementation = &Framebuffer::textureImplementationDSA;
        /* DSA doesn't have texture target parameter so we need to use different
           function to specify cube map face */
        texture2DImplementation = &Framebuffer::texture2DImplementationDSA;
        textureImplementation = &Framebuffer::textureImplementationDSA;
        textureCubeMapImplementation = &Framebuffer::textureCubeMapImplementationDSA;
        textureLayerImplementation = &Framebuffer::textureLayerImplementationDSA;

        renderbufferStorageImplementation = &Renderbuffer::storageImplementationDSA;

    } else if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        extensions.emplace_back(Extensions::GL::EXT::direct_state_access::string());

        checkStatusImplementation = &AbstractFramebuffer::checkStatusImplementationDSAEXT;

        /* I don't bother with EXT_DSA anymore */
        clearIImplementation = &AbstractFramebuffer::clearImplementationDefault;
        clearUIImplementation = &AbstractFramebuffer::clearImplementationDefault;
        clearFImplementation = &AbstractFramebuffer::clearImplementationDefault;
        clearFIImplementation = &AbstractFramebuffer::clearImplementationDefault;

        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationDSAEXT;
        drawBufferImplementation = &AbstractFramebuffer::drawBufferImplementationDSAEXT;
        readBufferImplementation = &AbstractFramebuffer::readBufferImplementationDSAEXT;

        copySub1DImplementation = &AbstractFramebuffer::copySub1DImplementationDSAEXT;
        copySub2DImplementation = &AbstractFramebuffer::copySub2DImplementationDSAEXT;
        copySubCubeMapImplementation = &AbstractFramebuffer::copySub2DImplementationDSAEXT;
        copySub3DImplementation = &AbstractFramebuffer::copySub3DImplementationDSAEXT;

        renderbufferImplementation = &Framebuffer::renderbufferImplementationDSAEXT;
        texture1DImplementation = &Framebuffer::texture1DImplementationDSAEXT;
        /* The EXT_DSA implementation is the same for both 2D and cube map textures */
        texture2DImplementation = &Framebuffer::texture2DImplementationDSAEXT;
        textureImplementation = &Framebuffer::textureImplementationDSAEXT;
        textureCubeMapImplementation = &Framebuffer::texture2DImplementationDSAEXT;
        textureLayerImplementation = &Framebuffer::textureLayerImplementationDSAEXT;

        renderbufferStorageImplementation = &Renderbuffer::storageImplementationDSAEXT;
    } else
    #endif
    {
        checkStatusImplementation = &AbstractFramebuffer::checkStatusImplementationDefault;

        #ifndef MAGNUM_TARGET_GLES2
        clearIImplementation = &AbstractFramebuffer::clearImplementationDefault;
        clearUIImplementation = &AbstractFramebuffer::clearImplementationDefault;
        clearFImplementation = &AbstractFramebuffer::clearImplementationDefault;
        clearFIImplementation = &AbstractFramebuffer::clearImplementationDefault;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationDefault;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        drawBufferImplementation = &AbstractFramebuffer::drawBufferImplementationDefault;
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        readBufferImplementation = &AbstractFramebuffer::readBufferImplementationDefault;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        copySub1DImplementation = &AbstractFramebuffer::copySub1DImplementationDefault;
        #endif
        copySub2DImplementation = &AbstractFramebuffer::copySub2DImplementationDefault;
        copySubCubeMapImplementation = &AbstractFramebuffer::copySub2DImplementationDefault;
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        copySub3DImplementation = &AbstractFramebuffer::copySub3DImplementationDefault;
        #endif

        renderbufferImplementation = &Framebuffer::renderbufferImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        texture1DImplementation = &Framebuffer::texture1DImplementationDefault;
        #endif
        /* The default implementation is the same for both 2D and cube map textures */
        texture2DImplementation = &Framebuffer::texture2DImplementationDefault;
        #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
        textureImplementation = &Framebuffer::textureImplementationDefault;
        #endif
        textureCubeMapImplementation = &Framebuffer::texture2DImplementationDefault;
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        textureLayerImplementation = &Framebuffer::textureLayerImplementationDefault;
        #endif

        renderbufferStorageImplementation = &Renderbuffer::storageImplementationDefault;
    }

    #ifdef MAGNUM_TARGET_GLES2
    /* Framebuffer binding and checking on ES2 */
    /* Optimistically set separate binding targets and check if one of the
       extensions providing them is available */
    #ifndef MAGNUM_TARGET_WEBGL
    bindImplementation = &Framebuffer::bindImplementationDefault;
    bindInternalImplementation = &Framebuffer::bindImplementationDefault;
    #endif
    checkStatusImplementation = &Framebuffer::checkStatusImplementationDefault;

    #ifndef MAGNUM_TARGET_WEBGL
    if(context.isExtensionSupported<Extensions::GL::ANGLE::framebuffer_blit>()) {
        extensions.push_back(Extensions::GL::ANGLE::framebuffer_blit::string());

    } else if(context.isExtensionSupported<Extensions::GL::APPLE::framebuffer_multisample>()) {
        extensions.push_back(Extensions::GL::APPLE::framebuffer_multisample::string());

    } else if(context.isExtensionSupported<Extensions::GL::NV::framebuffer_blit>()) {
        extensions.push_back(Extensions::GL::NV::framebuffer_blit::string());

    /* NV_framebuffer_multisample requires NV_framebuffer_blit, which has these
       enums. However, on my system only NV_framebuffer_multisample is
       supported, but NV_framebuffer_blit isn't. I will hold my breath and
       assume these enums are available. */
    } else if(context.isExtensionSupported<Extensions::GL::NV::framebuffer_multisample>()) {
        extensions.push_back(Extensions::GL::NV::framebuffer_multisample::string());

    /* If no such extension is available, reset back to single target */
    } else {
        bindImplementation = &Framebuffer::bindImplementationSingle;
        bindInternalImplementation = &Framebuffer::bindImplementationSingle;
        checkStatusImplementation = &Framebuffer::checkStatusImplementationSingle;
    }
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /* Framebuffer draw mapping on ES2 */
    if(context.isExtensionSupported<Extensions::GL::EXT::draw_buffers>()) {
        extensions.push_back(Extensions::GL::EXT::draw_buffers::string());

        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationEXT;
    } else if(context.isExtensionSupported<Extensions::GL::NV::draw_buffers>()) {
        extensions.push_back(Extensions::GL::NV::draw_buffers::string());

        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationNV;
    } else drawBuffersImplementation = nullptr;
    #else
    if(context.isExtensionSupported<Extensions::GL::WEBGL::draw_buffers>()) {
        extensions.push_back(Extensions::GL::WEBGL::draw_buffers::string());
        /* The EXT implementation is exposed in Emscripten */
        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationEXT;
    } else drawBuffersImplementation = nullptr;
    #endif
    #endif

    /* Framebuffer reading implementation in desktop/ES */
    #ifndef MAGNUM_TARGET_WEBGL
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

        readImplementation = &AbstractFramebuffer::readImplementationRobustness;
    } else readImplementation = &AbstractFramebuffer::readImplementationDefault;

    /* Framebuffer reading in WebGL */
    #else
    readImplementation = &AbstractFramebuffer::readImplementationDefault;
    #endif

    /* Multisample renderbuffer storage implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
        /* Extension added above */

        renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationDSA;

    } else if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
        /* Extension added above */

        renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationDSAEXT;
    } else
    #endif
    {
        #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(context.isExtensionSupported<Extensions::GL::ANGLE::framebuffer_multisample>()) {
            extensions.push_back(Extensions::GL::ANGLE::framebuffer_multisample::string());

            renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationANGLE;
        } else if (context.isExtensionSupported<Extensions::GL::NV::framebuffer_multisample>()) {
            extensions.push_back(Extensions::GL::NV::framebuffer_multisample::string());

            renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationNV;
        } else renderbufferStorageMultisampleImplementation = nullptr;
        #elif !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationDefault;
        #endif
    }

    /* Framebuffer invalidation implementation on desktop GL */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::invalidate_subdata>()) {
        extensions.emplace_back(Extensions::GL::ARB::invalidate_subdata::string());

        if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
            /* Extension added above */
            invalidateImplementation = &AbstractFramebuffer::invalidateImplementationDSA;
            invalidateSubImplementation = &AbstractFramebuffer::invalidateImplementationDSA;
        } else {
            invalidateImplementation = &AbstractFramebuffer::invalidateImplementationDefault;
            invalidateSubImplementation = &AbstractFramebuffer::invalidateImplementationDefault;
        }

    } else {
        invalidateImplementation = &AbstractFramebuffer::invalidateImplementationNoOp;
        invalidateSubImplementation = &AbstractFramebuffer::invalidateImplementationNoOp;
    }

    /* Framebuffer invalidation implementation on ES2 */
    #elif defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(context.isExtensionSupported<Extensions::GL::EXT::discard_framebuffer>()) {
        extensions.push_back(Extensions::GL::EXT::discard_framebuffer::string());

        invalidateImplementation = &AbstractFramebuffer::invalidateImplementationDefault;
    } else {
        invalidateImplementation = &AbstractFramebuffer::invalidateImplementationNoOp;
    }

    /* Always available on ES3 */
    #elif !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    invalidateImplementation = &AbstractFramebuffer::invalidateImplementationDefault;
    invalidateSubImplementation = &AbstractFramebuffer::invalidateImplementationDefault;
    #endif

    /* Blit implementation on desktop GL */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::ARB::direct_state_access>()) {
        /* Extension added above */
        blitImplementation = &AbstractFramebuffer::blitImplementationDSA;

    } else blitImplementation = &AbstractFramebuffer::blitImplementationDefault;

    /* Blit implementation on ES2 */
    #elif defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(context.isExtensionSupported<Extensions::GL::ANGLE::framebuffer_blit>()) {
        extensions.push_back(Extensions::GL::ANGLE::framebuffer_blit::string());
        blitImplementation = &AbstractFramebuffer::blitImplementationANGLE;

    } else if(context.isExtensionSupported<Extensions::GL::NV::framebuffer_blit>()) {
        extensions.push_back(Extensions::GL::NV::framebuffer_blit::string());
        blitImplementation = &AbstractFramebuffer::blitImplementationNV;

    } else blitImplementation = nullptr;

    /* Always available on ES3 and WebGL 2 */
    #elif !defined(MAGNUM_TARGET_GLES2)
    blitImplementation = &AbstractFramebuffer::blitImplementationDefault;
    #endif

    #if defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif
}

void FramebufferState::reset() {
    readBinding = drawBinding = renderbufferBinding = State::DisengagedBinding;
    viewport = DisengagedViewport;
}

}}
