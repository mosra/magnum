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

#include "Magnum/Context.h"

#include <algorithm>

#include "Magnum/Extensions.h"
#include "Magnum/Math/Range.h"

namespace Magnum {

namespace {
    /* Search the code for the following strings to see where they are implemented. */
    std::vector<std::string> KnownWorkarounds{
        #if !defined(MAGNUM_TARGET_GLES) && !defined(CORRADE_TARGET_APPLE)
        /* Creating core context with specific version on AMD and NV
           proprietary drivers on Linux/Windows and Intel drivers on Windows
           causes the context to be forced to given version instead of
           selecting latest available version */
        "no-forward-compatible-core-context",
        #endif

        #if !defined(MAGNUM_TARGET_GLES) && defined(CORRADE_TARGET_WINDOWS)
        /* On Windows Intel drivers ARB_shading_language_420pack is exposed in
           GLSL even though the extension (e.g. binding keyword) is not
           supported */
        "intel-windows-glsl-exposes-unsupported-shading-language-420pack",
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && defined(CORRADE_TARGET_WINDOWS)
        /* On Windows NVidia drivers the glTransformFeedbackVaryings() does not
           make a copy of its char* arguments so it fails at link time when the
           original char arrays are not in scope anymore. Enabling
           *synchronous* debug output circumvents this bug. Can be triggered by
           running TransformFeedbackGLTest with GL_KHR_debug extension
           disabled. */
        "nv-windows-dangling-transform-feedback-varying-names",
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /* Layout qualifier causes compiler error with GLSL 1.20 on Mesa, GLSL
           1.30 on NVidia and 1.40 on macOS. Everything is fine when using
           newer GLSL version. */
        "no-layout-qualifiers-on-old-glsl",

        /* NVidia drivers (358.16) report compressed block size from internal
           format query in bits instead of bytes */
        "nv-compressed-block-size-in-bits",

        /* NVidia drivers (358.16) report different compressed image size for
           cubemaps based on whether the texture is immutable or not and not
           based on whether I'm querying all faces (ARB_DSA) or a single face
           (non-DSA, EXT_DSA) */
        "nv-cubemap-inconsistent-compressed-image-size",

        /* NVidia drivers (358.16) return only the first slice of compressed
           cube map image when querying all six slice using ARB_DSA API */
        "nv-cubemap-broken-full-compressed-image-query",

        /* NVidia drivers return 0 when asked for GL_CONTEXT_PROFILE_MASK,
           so it needs to be worked around by asking for GL_ARB_compatibility */
        "nv-zero-context-profile-mask",
        #endif

        #ifndef MAGNUM_TARGET_GLES
        /* SVGA3D (VMware host GL driver) glDrawArrays() draws nothing when the
           vertex buffer memory is initialized using glNamedBufferData() from
           ARB_DSA. Using the non-DSA glBufferData() works. */
        "svga3d-broken-dsa-bufferdata",

        /* SVGA3D does out-of-bound writes in some cases of glGetTexSubImage(),
           leading to memory corruption on client machines. That's nasty, so the
           whole ARB_get_texture_sub_image is disabled. */
        "svga3d-gettexsubimage-oob-write",
        #endif

        /* SVGA3D has broken handling of glTex[ture][Sub]Image*D() for 1D
           arrays, 2D arrays, 3D textures and cube map textures where it
           uploads just the first slice in the last dimension. This is only
           with copies from host memory, not with buffer images. Seems to be
           fixed in Mesa 13, but I have no such system to verify that on.
           https://github.com/mesa3d/mesa/commit/2aa9ff0cda1f6ad97c83d5583fab7a84efabe19e */
        "svga3d-texture-upload-slice-by-slice"
    };
}

namespace Implementation {

/* Used in Shader.cpp (duh) */
bool isShaderCompilationLogEmpty(const std::string&);
bool isShaderCompilationLogEmpty(const std::string& result) {
    #if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES)
    /* Intel Windows drivers are too chatty */
    if((Context::current().detectedDriver() & Context::DetectedDriver::IntelWindows) && result == "No errors.\n")
        return true;
    #else
    static_cast<void>(result);
    #endif

    return false;
}

/* Used in AbstractShaderProgram.cpp (duh) */
bool isProgramLinkLogEmpty(const std::string&);
bool isProgramLinkLogEmpty(const std::string& result) {
    #if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES)
    /* Intel Windows drivers are too chatty */
    if((Context::current().detectedDriver() & Context::DetectedDriver::IntelWindows) && result == "No errors.\n")
        return true;
    #else
    static_cast<void>(result);
    #endif

    return false;
}

}

auto Context::detectedDriver() -> DetectedDrivers {
    if(_detectedDrivers) return *_detectedDrivers;

    _detectedDrivers = DetectedDrivers{};

    const std::string renderer = rendererString();
    const std::string vendor = vendorString();
    const std::string version = versionString();

    /* Apple has its own drivers */
    #if !defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_WEBGL)
    /* AMD binary desktop drivers */
    if(vendor.find("ATI Technologies Inc.") != std::string::npos)
        return *_detectedDrivers |= DetectedDriver::Amd;

    #ifdef CORRADE_TARGET_WINDOWS
    /* Intel Windows drivers */
    if(vendor.find("Intel") != std::string::npos)
        return *_detectedDrivers |= DetectedDriver::IntelWindows;
    #endif

    /* Mesa drivers */
    if(version.find("Mesa") != std::string::npos) {
        *_detectedDrivers |= DetectedDriver::Mesa;

        if(renderer.find("SVGA3D") != std::string::npos)
            *_detectedDrivers |= DetectedDriver::Svga3D;

        return *_detectedDrivers;
    }

    if(vendor.find("NVIDIA Corporation") != std::string::npos)
        return *_detectedDrivers |= DetectedDriver::NVidia;
    #endif

    /** @todo there is also D3D9/D3D11 distinction on webglreport.com, is it useful? */
    #ifdef MAGNUM_TARGET_GLES
    /* OpenGL ES implementation using ANGLE. Taken from these sources:
       http://stackoverflow.com/a/20149090
       http://webglreport.com
    */
    {
        Range1Di range;
        glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, range.data());
        if(range.min() == 1 && range.max() == 1 && vendor != "Internet Explorer")
            return *_detectedDrivers |= DetectedDriver::Angle;
    }
    #endif

    return *_detectedDrivers;
}

void Context::disableDriverWorkaround(const std::string& workaround) {
    /* Ignore unknown workarounds */
    if(std::find(KnownWorkarounds.begin(), KnownWorkarounds.end(), workaround) == KnownWorkarounds.end()) {
        Warning() << "Unknown workaround" << workaround;
        return;
    }
    _driverWorkarounds.emplace_back(workaround, true);
}

bool Context::isDriverWorkaroundDisabled(const std::string& workaround) {
    CORRADE_INTERNAL_ASSERT(std::find(KnownWorkarounds.begin(), KnownWorkarounds.end(), workaround) != KnownWorkarounds.end());

    /* If the workaround was already asked for or disabled, return its state,
       otherwise add it to the list as used one */
    for(const auto& i: _driverWorkarounds)
        if(i.first == workaround) return i.second;
    _driverWorkarounds.emplace_back(workaround, false);
    return false;
}

void Context::setupDriverWorkarounds() {
    #define _setRequiredVersion(extension, version)                           \
        if(_extensionRequiredVersion[Extensions::extension::Index] < Version::version) \
            _extensionRequiredVersion[Extensions::extension::Index] = Version::version

    #ifndef MAGNUM_TARGET_GLES
    #ifdef CORRADE_TARGET_WINDOWS
    if((detectedDriver() & DetectedDriver::IntelWindows) && !isExtensionSupported<Extensions::GL::ARB::shading_language_420pack>() && !isDriverWorkaroundDisabled("intel-windows-glsl-exposes-unsupported-shading-language-420pack"))
        _setRequiredVersion(GL::ARB::shading_language_420pack, None);
    #endif

    if(!isDriverWorkaroundDisabled("no-layout-qualifiers-on-old-glsl")) {
        _setRequiredVersion(GL::ARB::explicit_attrib_location, GL320);
        _setRequiredVersion(GL::ARB::explicit_uniform_location, GL320);
        _setRequiredVersion(GL::ARB::shading_language_420pack, GL320);
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((detectedDriver() & DetectedDriver::Svga3D) &&
       isExtensionSupported<Extensions::GL::ARB::get_texture_sub_image>() &&
       !isDriverWorkaroundDisabled("svga3d-gettexsubimage-oob-write"))
        _setRequiredVersion(GL::ARB::get_texture_sub_image, None);
    #endif

    #undef _setRequiredVersion
}

}
