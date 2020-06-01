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

#include "FramebufferState.h"

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Renderbuffer.h"

#include "State.h"

namespace Magnum { namespace GL { namespace Implementation {

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
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        extensions.emplace_back(Extensions::ARB::direct_state_access::string());
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
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        /* Extension added above */

        checkStatusImplementation = &AbstractFramebuffer::checkStatusImplementationDSA;

        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationDSA;
        drawBufferImplementation = &AbstractFramebuffer::drawBufferImplementationDSA;
        readBufferImplementation = &AbstractFramebuffer::readBufferImplementationDSA;

        copySub1DImplementation = &AbstractFramebuffer::copySub1DImplementationDSA;
        copySub2DImplementation = &AbstractFramebuffer::copySub2DImplementationDSA;
        copySub3DImplementation = &AbstractFramebuffer::copySub3DImplementationDSA;

        renderbufferImplementation = &Framebuffer::renderbufferImplementationDSA;
        /* The 1D implementation uses the same function as the layered attachment */
        texture1DImplementation = &Framebuffer::textureImplementationDSA;
        /* DSA doesn't have texture target parameter so we need to use different
           function to specify cube map face */
        texture2DImplementation = &Framebuffer::texture2DImplementationDSA;
        textureImplementation = &Framebuffer::textureImplementationDSA;
        textureLayerImplementation = &Framebuffer::textureLayerImplementationDSA;

        renderbufferStorageImplementation = &Renderbuffer::storageImplementationDSA;

    } else
    #endif
    {
        checkStatusImplementation = &AbstractFramebuffer::checkStatusImplementationDefault;

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
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        copySub3DImplementation = &AbstractFramebuffer::copySub3DImplementationDefault;
        #endif

        renderbufferImplementation = &Framebuffer::renderbufferImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        texture1DImplementation = &Framebuffer::texture1DImplementationDefault;
        #endif
        /* The default implementation is the same for both 2D and cube map textures */
        texture2DImplementation = &Framebuffer::texture2DImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        textureImplementation = &Framebuffer::textureImplementationDefault;
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        textureLayerImplementation = &Framebuffer::textureLayerImplementationDefault;
        #endif

        renderbufferStorageImplementation = &Renderbuffer::storageImplementationDefault;
    }

    /* DSA/non-DSA implementation for cubemaps, because Intel Windows drivers
       have to be broken in a special way. Also reusing this branch for and AMD
       workaround for cubemap copy. */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        #ifdef CORRADE_TARGET_WINDOWS
        if(context.detectedDriver() & Context::DetectedDriver::IntelWindows && !context.isDriverWorkaroundDisabled("intel-windows-broken-dsa-for-cubemaps")) {
            copySubCubeMapImplementation = &AbstractFramebuffer::copySub2DImplementationDefault;
            textureCubeMapImplementation = &Framebuffer::texture2DImplementationDefault;
        } else if(context.detectedDriver() & Context::DetectedDriver::Amd && !context.isDriverWorkaroundDisabled("amd-windows-broken-dsa-cubemap-copy")) {
            copySubCubeMapImplementation = &AbstractFramebuffer::copySub2DImplementationDefault;
            /* Cube map attachment is not broken */
            textureCubeMapImplementation = &Framebuffer::texture2DImplementationDSA;
        } else
        #endif
        {
            /* Extension name added above */

            copySubCubeMapImplementation = &AbstractFramebuffer::copySubCubeMapImplementationDSA;
            textureCubeMapImplementation = &Framebuffer::textureCubeMapImplementationDSA;
        }
    } else
    #endif
    {
        copySubCubeMapImplementation = &AbstractFramebuffer::copySub2DImplementationDefault;
        textureCubeMapImplementation = &Framebuffer::texture2DImplementationDefault;
    }

    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    /* DSA/non-DSA implementation for attaching layered cubemap arrays, because
       ... well, guess why. */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()
        #ifdef CORRADE_TARGET_WINDOWS
        && (!(context.detectedDriver() & Context::DetectedDriver::IntelWindows) ||
            context.isDriverWorkaroundDisabled("intel-windows-broken-dsa-layered-cubemap-array-framebuffer-attachment"))
        #endif
    ) {
        /* Extension name added above */

        layeredTextureCubeMapArrayImplementation = &Framebuffer::textureImplementationDSA;
    } else
    #endif
    {
        layeredTextureCubeMapArrayImplementation = &Framebuffer::textureImplementationDefault;
    }
    #endif

    /* DSA/non-DSA implementation for framebuffer clearing. Yes, it's because
       Intel Windows drivers are shit. */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()
        #ifdef CORRADE_TARGET_WINDOWS
        && (!(context.detectedDriver() & Context::DetectedDriver::IntelWindows) ||
            context.isDriverWorkaroundDisabled("intel-windows-broken-dsa-framebuffer-clear"))
        #endif
    ) {
        /* Extension name added above */

        clearIImplementation = &AbstractFramebuffer::clearImplementationDSA;
        clearUIImplementation = &AbstractFramebuffer::clearImplementationDSA;
        clearFImplementation = &AbstractFramebuffer::clearImplementationDSA;
        clearFIImplementation = &AbstractFramebuffer::clearImplementationDSA;
    } else
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES2
        clearIImplementation = &AbstractFramebuffer::clearImplementationDefault;
        clearUIImplementation = &AbstractFramebuffer::clearImplementationDefault;
        clearFImplementation = &AbstractFramebuffer::clearImplementationDefault;
        clearFIImplementation = &AbstractFramebuffer::clearImplementationDefault;
        #endif
    }

    /* Framebuffer texture attachment on ES3 */
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    if(context.isVersionSupported(Version::GLES320))
        textureImplementation = &Framebuffer::textureImplementationDefault;
    else if(context.isExtensionSupported<Extensions::EXT::geometry_shader>())
        textureImplementation = &Framebuffer::textureImplementationEXT;
    else
        textureImplementation = nullptr;
    #endif

    #ifdef MAGNUM_TARGET_GLES2
    /* Framebuffer binding and checking on ES2 */
    /* Optimistically set separate binding targets and check if one of the
       extensions providing them is available */
    #ifndef MAGNUM_TARGET_WEBGL
    bindImplementation = &Framebuffer::bindImplementationDefault;
    bindInternalImplementation = &Framebuffer::bindImplementationDefault;
    checkStatusImplementation = &Framebuffer::checkStatusImplementationDefault;

    if(context.isExtensionSupported<Extensions::ANGLE::framebuffer_blit>()) {
        extensions.push_back(Extensions::ANGLE::framebuffer_blit::string());

    } else if(context.isExtensionSupported<Extensions::APPLE::framebuffer_multisample>()) {
        extensions.push_back(Extensions::APPLE::framebuffer_multisample::string());

    } else if(context.isExtensionSupported<Extensions::NV::framebuffer_blit>()) {
        extensions.push_back(Extensions::NV::framebuffer_blit::string());

    /* NV_framebuffer_multisample requires NV_framebuffer_blit, which has these
       enums. However, on my system only NV_framebuffer_multisample is
       supported, but NV_framebuffer_blit isn't. I will hold my breath and
       assume these enums are available. */
    } else if(context.isExtensionSupported<Extensions::NV::framebuffer_multisample>()) {
        extensions.push_back(Extensions::NV::framebuffer_multisample::string());

    /* If no such extension is available, reset back to single target */
    } else {
        bindImplementation = &Framebuffer::bindImplementationSingle;
        bindInternalImplementation = &Framebuffer::bindImplementationSingle;
        checkStatusImplementation = &Framebuffer::checkStatusImplementationSingle;
    }
    #else
    checkStatusImplementation = &Framebuffer::checkStatusImplementationSingle;
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /* Framebuffer draw mapping on ES2 */
    if(context.isExtensionSupported<Extensions::EXT::draw_buffers>()) {
        extensions.push_back(Extensions::EXT::draw_buffers::string());

        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationEXT;
    } else if(context.isExtensionSupported<Extensions::NV::draw_buffers>()) {
        extensions.push_back(Extensions::NV::draw_buffers::string());

        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationNV;
    } else drawBuffersImplementation = nullptr;
    #else
    if(context.isExtensionSupported<Extensions::WEBGL::draw_buffers>()) {
        extensions.push_back(Extensions::WEBGL::draw_buffers::string());
        /* The EXT implementation is exposed in Emscripten */
        drawBuffersImplementation = &AbstractFramebuffer::drawBuffersImplementationEXT;
    } else drawBuffersImplementation = nullptr;
    #endif
    #endif

    /* Implementation-specific color read format/type implementation */
    #ifndef MAGNUM_TARGET_GLES
    /* Get(Named)FramebufferParameteriv() supports querying
       GL_IMPLEMENTATION_COLOR_READ_{FORMAT,TYPE} since GL 4.5. No
       corresponding extension enabling this, only a mention of Bug 12360
       that's supposed to have more information about this. But the Khronos
       bugzilla is lost to internet history now and everything gets redirected
       to the mostly-empty GitHub issue tracker (and it doesn't even have the
       old bugs imported), so this is all I got. The whole thing is a
       clusterfuck:
        -   ES3.1 adds GetFramebufferParameteriv() but it *doesn't* allow
            GL_IMPLEMENTATION_COLOR_READ_FORMAT to be used with it. ES3.2
            doesn't fix that omission either. Funnily enough, most drivers
            (including NV, Mesa and SwiftShader) support such a query, the only
            driver which doesn't (and thus matches the spec) is on my Huawei
            P10. What.
        -   Intel implementation on Windows, even though supporting 4.5 and
            DSA, returns absolute garbage on everything except the most basic
            GetInteger query
        -   NVidia returns broken values when calling the DSA code path
        -   Mesa needs the framebuffer to be bound even for DSA queries */
    if(context.isVersionSupported(Version::GL450)
        #ifdef CORRADE_TARGET_WINDOWS
        && !((context.detectedDriver() & Context::DetectedDriver::IntelWindows) &&
        !context.isDriverWorkaroundDisabled("intel-windows-implementation-color-read-format-completely-broken"))
        #endif
    ) {
        if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()
            && !((context.detectedDriver() & Context::DetectedDriver::NVidia) && !context.isDriverWorkaroundDisabled("nv-implementation-color-read-format-dsa-broken"))
        ) {
            /* DSA extension added above */

            if((context.detectedDriver() & Context::DetectedDriver::Mesa) && !context.isDriverWorkaroundDisabled("mesa-implementation-color-read-format-dsa-explicit-binding"))
                implementationColorReadFormatTypeImplementation = &AbstractFramebuffer::implementationColorReadFormatTypeImplementationFramebufferDSAMesa;
            else implementationColorReadFormatTypeImplementation = &AbstractFramebuffer::implementationColorReadFormatTypeImplementationFramebufferDSA;
        } else {
            implementationColorReadFormatTypeImplementation = &AbstractFramebuffer::implementationColorReadFormatTypeImplementationFramebuffer;
        }
    } else
    #endif
    {
        implementationColorReadFormatTypeImplementation = &AbstractFramebuffer::implementationColorReadFormatTypeImplementationGlobal;
    }

    /* Framebuffer reading implementation in desktop/ES */
    #ifndef MAGNUM_TARGET_WEBGL
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

        readImplementation = &AbstractFramebuffer::readImplementationRobustness;
    } else readImplementation = &AbstractFramebuffer::readImplementationDefault;

    /* Framebuffer reading in WebGL */
    #else
    readImplementation = &AbstractFramebuffer::readImplementationDefault;
    #endif

    /* Multisample renderbuffer storage implementation */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        /* Extension added above */

        renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationDSA;

    } else
    #endif
    {
        #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        if(context.isExtensionSupported<Extensions::ANGLE::framebuffer_multisample>()) {
            extensions.push_back(Extensions::ANGLE::framebuffer_multisample::string());

            renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationANGLE;
        } else if (context.isExtensionSupported<Extensions::NV::framebuffer_multisample>()) {
            extensions.push_back(Extensions::NV::framebuffer_multisample::string());

            renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationNV;
        } else renderbufferStorageMultisampleImplementation = nullptr;
        #elif !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        renderbufferStorageMultisampleImplementation = &Renderbuffer::storageMultisampleImplementationDefault;
        #endif
    }

    /* Framebuffer invalidation implementation on desktop GL */
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::ARB::invalidate_subdata>()) {
        extensions.emplace_back(Extensions::ARB::invalidate_subdata::string());

        if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
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
    if(context.isExtensionSupported<Extensions::EXT::discard_framebuffer>()) {
        extensions.push_back(Extensions::EXT::discard_framebuffer::string());

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
    if(context.isExtensionSupported<Extensions::ARB::direct_state_access>()) {
        /* Extension added above */
        blitImplementation = &AbstractFramebuffer::blitImplementationDSA;

    } else blitImplementation = &AbstractFramebuffer::blitImplementationDefault;

    /* Blit implementation on ES2 */
    #elif defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(context.isExtensionSupported<Extensions::ANGLE::framebuffer_blit>()) {
        extensions.push_back(Extensions::ANGLE::framebuffer_blit::string());
        blitImplementation = &AbstractFramebuffer::blitImplementationANGLE;

    } else if(context.isExtensionSupported<Extensions::NV::framebuffer_blit>()) {
        extensions.push_back(Extensions::NV::framebuffer_blit::string());
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

}}}
