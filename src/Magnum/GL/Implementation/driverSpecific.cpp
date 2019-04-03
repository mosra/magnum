/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <algorithm>
#include <string>
#include <vector>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace GL {

namespace {
    /* Search the code for the following strings to see where they are implemented. */
    std::vector<std::string> KnownWorkarounds{
/* [workarounds] */
#if defined(CORRADE_TARGET_ANDROID) && defined(MAGNUM_TARGET_GLES)
/* glBeginQuery() with GL_TIME_ELAPSED causes a GL_OUT_OF_MEMORY error when
   running from the Android shell (through ADB). No such error happens in an
   APK. Detecting using the $SHELL environment variable and disabling
   GL_EXT_disjoint_timer_query in that case. */
"arm-mali-timer-queries-oom-in-shell",
#endif

#if !defined(MAGNUM_TARGET_GLES) && !defined(CORRADE_TARGET_APPLE)
/* Creating core context with specific version on AMD and NV proprietary
   drivers on Linux/Windows and Intel drivers on Windows causes the context to
   be forced to given version instead of selecting latest available version */
"no-forward-compatible-core-context",
#endif

#if !defined(MAGNUM_TARGET_GLES) && defined(CORRADE_TARGET_WINDOWS)
/* On Windows Intel drivers ARB_shading_language_420pack is exposed in GLSL
   even though the extension (e.g. binding keyword) is not supported */
"intel-windows-glsl-exposes-unsupported-shading-language-420pack",
#endif

#ifndef MAGNUM_TARGET_GLES
/* Forward-compatible GL contexts on Mesa still report line width range as
   [1, 7], but setting wide line width fails. According to the specs the max
   value on forward compatible contexts should be 1.0, so patching it. */
"mesa-forward-compatible-line-width-range",
#endif

#if !defined(MAGNUM_TARGET_GLES2) && defined(CORRADE_TARGET_WINDOWS)
/* On Windows NVidia drivers the glTransformFeedbackVaryings() does not make a
   copy of its char* arguments so it fails at link time when the original char
   arrays are not in scope anymore. Enabling *synchronous* debug output
   circumvents this bug. Can be triggered by running TransformFeedbackGLTest
   with GL_KHR_debug extension disabled. */
"nv-windows-dangling-transform-feedback-varying-names",
#endif

#ifndef MAGNUM_TARGET_GLES
/* Layout qualifier causes compiler error with GLSL 1.20 on Mesa, GLSL 1.30 on
   NVidia and 1.40 on macOS. Everything is fine when using a newer GLSL
   version. */
"no-layout-qualifiers-on-old-glsl",

/* NVidia drivers (358.16) report compressed block size from internal format
   query in bits instead of bytes */
"nv-compressed-block-size-in-bits",

/* NVidia drivers (358.16) report different compressed image size for cubemaps
   based on whether the texture is immutable or not and not based on whether
   I'm querying all faces (ARB_DSA) or a single face (non-DSA, EXT_DSA) */
"nv-cubemap-inconsistent-compressed-image-size",

/* NVidia drivers (358.16) return only the first slice of compressed cube map
   image when querying all six slices using the ARB_DSA API */
"nv-cubemap-broken-full-compressed-image-query",

/* NVidia drivers return 0 when asked for GL_CONTEXT_PROFILE_MASK, so it needs
   to be worked around by asking for GL_ARB_compatibility */
"nv-zero-context-profile-mask",

/* (Headless) EGL contexts for desktop GL on NVidia 384 and 390 drivers don't
   have correct statically linked GL 1.0 and 1.1 functions (such as
   glGetString()) and one has to retrieve them explicitly using
   eglGetProcAddress(). Doesn't seem to happen on pre-384 and 396, but it's not
   possible to get driver version through EGL, so enabling this unconditionally
   on all EGL NV contexts. */
"nv-egl-incorrect-gl11-function-pointers",
#endif

#ifndef MAGNUM_TARGET_GLES
/* SVGA3D (VMware host GL driver) glDrawArrays() draws nothing when the vertex
   buffer memory is initialized using glNamedBufferData() from ARB_DSA. Using
   the non-DSA glBufferData() works. */
"svga3d-broken-dsa-bufferdata",

/* SVGA3D does out-of-bound writes in some cases of glGetTexSubImage(), leading
   to memory corruption on client machines. That's nasty, so the whole
   ARB_get_texture_sub_image is disabled. */
"svga3d-gettexsubimage-oob-write",
#endif

/* SVGA3D has broken handling of glTex[ture][Sub]Image*D() for 1D arrays, 2D
   arrays, 3D textures and cube map textures where it uploads just the first
   slice in the last dimension. This is only with copies from host memory, not
   with buffer images. Seems to be fixed in Mesa 13, but I have no such system
   to verify that on.
   https://github.com/mesa3d/mesa/commit/2aa9ff0cda1f6ad97c83d5583fab7a84efabe19e */
"svga3d-texture-upload-slice-by-slice",

#if defined(CORRADE_TARGET_EMSCRIPTEN) && defined(__EMSCRIPTEN_PTHREADS__)
/* Shader sources containing UTF-8 characters are converted to empty strings
   when running on Emscripten with -s USE_PTHREADS=1. Working around that by
   replacing all chars > 127 with spaces. Relevant code:
   https://github.com/kripken/emscripten/blob/7f89560101843198787530731f40a65288f6f15f/src/fetch-worker.js#L54-L58 */
"emscripten-pthreads-broken-unicode-shader-sources",
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
/* Empty EGL_CONTEXT_FLAGS_KHR cause SwiftShader 3.3.0.1 to fail context
   creation with EGL_BAD_ATTRIBUTE. Not sending the flags then. Relevant code:
   https://github.com/google/swiftshader/blob/5fb5e817a20d3e60f29f7338493f922b5ac9d7c4/src/OpenGL/libEGL/libEGL.cpp#L794-L810 */
"swiftshader-no-empty-egl-context-flags",

/* SwiftShader 3.3.0.1 crashes deep inside eglMakeCurrent() when using
   EGL_NO_SURFACE. Supplying a 32x32 PBuffer to work around that. */
"swiftshader-egl-context-needs-pbuffer",
#endif

#if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/* SwiftShader 4.1.0 on ES2 contexts reports GL_ANGLE_instanced_arrays and
   GL_EXT_instanced_arrays but has no glDrawArraysInstancedANGLE /
   glDrawArraysInstancedEXT nor glDrawElementsInstancedANGLE /
   glDrawElementsInstancedEXT entrypoints, only the unsuffixed versions for
   ES3. OTOH, glVertexAttribDivisor is there for both ANGLE and EXT. Relevant
   code: https://github.com/google/swiftshader/blob/ad5c2952ca88730c07e04f6f1566194b66860c26/src/OpenGL/libGLESv2/libGLESv2.cpp#L6352-L6357
   Disabling the two extensions on ES2 contexts to avoid nullptr crashes. */
"swiftshader-no-es2-draw-instanced-entrypoints",

/* SwiftShader 4.1.0 on ES2 contexts reports GL_OES_texture_3D but from all its
   entrypoints only glTexImage3DOES is present, all others are present only in
   the ES3 unsuffixed versions. Relevant code:
   https://github.com/google/swiftshader/blob/ad5c2952ca88730c07e04f6f1566194b66860c26/src/OpenGL/libGLESv2/libGLESv2.cpp#L6504
   Disabling the extension on ES2 contexts to avoid nullptr crashes. */
"swiftshader-no-es2-oes-texture-3d-entrypoints",
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/* SwiftShader 4.1.0 has special handling for binding buffers to the transform
   feedback target, requiring an XFB object to be active when a buffer is bound
   to GL_TRANSFORM_FEEDBACK_BUFFER and ignoring the glBindBuffer() call
   otherwise. No other driver does that. As a workaround, setting
   Buffer::TargetHint::TransformFeedback will make it use
   Buffer::TargetHint::Array instead, as that works okay. */
"swiftshader-broken-xfb-buffer-binding-target",

/* SwiftShader 4.1.0 does implement gl_VertexID for ES3 contexts, but in
   practice it doesn't work, returning a constant value. In order to make this
   easier to check, there's a dummy MAGNUM_shader_vertex_id extension that's
   defined on all GL 3.0+ and GLES 3.0+ / WebGL 2+ contexts *except* for
   SwiftShader. */
"swiftshader-broken-shader-vertex-id",
#endif

#ifndef MAGNUM_TARGET_GLES
/* Even with the DSA variant, where GL_IMPLEMENTATION_COLOR_READ_* is passed to
   glGetNamedFramebufferParameter(), Mesa complains that the framebuffer is not
   bound for reading. Relevant code:
   https://github.com/mesa3d/mesa/blob/212c0c630a849e4737e2808a993d708cbb2f18f7/src/mesa/main/framebuffer.c#L841-L843
   Workaround is to explicitly bind the framebuffer for reading. */
"mesa-implementation-color-read-format-dsa-explicit-binding",
#endif

#if !defined(MAGNUM_TARGET_GLES2) && defined(CORRADE_TARGET_WINDOWS)
/* Intel drivers on Windows return GL_UNSIGNED_BYTE for *both*
   GL_IMPLEMENTATION_COLOR_READ_FORMAT and _TYPE when using either
   glGetNamedFramebufferParameter() or glGetFramebufferParameter(),
   independently on what's the actual framebuffer format. Using glGetInteger()
   makes it return GL_RGBA and GL_UNSIGNED_BYTE for RGBA8 framebuffers, and
   cause an "Error has been generated. GL error GL_INVALID_OPERATION in
   GetIntegerv: (ID: 2576729458) Generic error" when it is not. Since
   glGetInteger() is actually able to return a correct value in *one
   circumstance*, it's preferrable to the other random shit the driver is
   doing. */
"intel-windows-implementation-color-read-format-completely-broken",
#endif

#if !defined(MAGNUM_TARGET_GLES) && defined(CORRADE_TARGET_WINDOWS)
/* Intel drivers on Windows have some synchronization / memory alignment bug in
   the DSA glNamedBufferData() when the same buffer is set as an index buffer
   to a mesh right after or repeatedly. Calling glBindBuffer() right before or
   after the data upload fixes the issue. Note that this workaround is done
   only for buffers with TargetHint::ElementArray, as the issue was not
   observed elsewhere. Reproducible with the 2019.01 ImGui example,
   unfortunately I was not able to create a standalone minimal repro case. */
"intel-windows-buggy-dsa-bufferdata-for-index-buffers",

/* ARB_direct_state_access implementation on Intel Windows drivers has broken
   *everything* related to cube map textures (but not cube map arrays) -- data
   upload, data queries, framebuffer attachment, framebuffer copies, all
   complaining about "Wrong <func> 6 provided for <target> 34067" and similar
   (GL_TEXTURE_CUBE_MAP is 34067). Using the non-DSA code path as a
   workaround. */
"intel-windows-broken-dsa-for-cubemaps",

/* DSA glBindTextureUnit() on Intel Windows drivers simply doesn't work when
   passing 0 to it. Non-zero IDs work correctly except for cube maps. Using the
   non-DSA code path for unbinding and cube maps as a workaround. */
"intel-windows-half-baked-dsa-texture-bind",

/* DSA glNamedFramebufferTexture() on Intel Windows drivers doesn't work for
   layered cube map array attachments. Non-layered or non-array cube map
   attachment works. Using the non-DSA code path as a workaround. */
"intel-windows-broken-dsa-layered-cubemap-array-framebuffer-attachment",

/* DSA glClearNamedFramebuffer*() on Intel Windows drivers doesn't do anything.
   Using the non-DSA code path as a workaournd. */
"intel-windows-broken-dsa-framebuffer-clear",

/* Using DSA glCreateQueries() on Intel Windows drivers breaks
   glBeginQueryIndexed(). Using the non-DSA glGenQueries() instead makes it
   work properly. See TransformFeedbackGLTest for a test. */
"intel-windows-broken-dsa-indexed-queries",

/* DSA-ified "vertex layout" glVertexArrayAttribIFormat() is broken when
   passing shorts instead of full 32bit ints. Using the old-style
   glVertexAttribIPointer() works correctly. No idea if the non-DSA
   glVertexAttribIFormat() works or not. A test that triggers this issue is in
   MeshGLTest::addVertexBufferIntWithShort(). */
"intel-windows-broken-dsa-integer-vertex-attributes",
#endif

#ifndef MAGNUM_TARGET_GLES
/* NVidia seems to be returning values for the default framebuffer when
   GL_IMPLEMENTATION_COLOR_READ_FORMAT and _TYPE is queried using
   glGetNamedFramebufferParameter(). Using either glGetInteger() or
   glGetFramebufferParameter() works correctly. */
"nv-implementation-color-read-format-dsa-broken",
#endif
/* [workarounds] */
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
    /* ANGLE. Can detect easily on ES, have to resort to hacks on WebGL.
       Sources: http://stackoverflow.com/a/20149090 + http://webglreport.com */
    #ifndef MAGNUM_TARGET_WEBGL
    if(renderer.find("ANGLE") != std::string::npos)
        return *_detectedDrivers |= DetectedDriver::Angle;
    #else
    {
        Range1Di range;
        glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, range.data());
        if(range.min() == 1 && range.max() == 1 && vendor != "Internet Explorer")
            return *_detectedDrivers |= DetectedDriver::Angle;
    }
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    /* SwiftShader */
    if(renderer.find("SwiftShader") != std::string::npos)
        return *_detectedDrivers |= DetectedDriver::SwiftShader;
    #endif
    #endif

    #ifdef CORRADE_TARGET_ANDROID
    if(vendor.find("ARM") != std::string::npos && renderer.find("Mali") != std::string::npos)
        return *_detectedDrivers |= DetectedDriver::ArmMali;
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

bool Context::isDriverWorkaroundDisabled(const char* workaround) {
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
    if((detectedDriver() & DetectedDriver::IntelWindows) && !isExtensionSupported<Extensions::ARB::shading_language_420pack>() && !isDriverWorkaroundDisabled("intel-windows-glsl-exposes-unsupported-shading-language-420pack"))
        _setRequiredVersion(ARB::shading_language_420pack, None);
    #endif

    if(!isDriverWorkaroundDisabled("no-layout-qualifiers-on-old-glsl")) {
        _setRequiredVersion(ARB::explicit_attrib_location, GL320);
        _setRequiredVersion(ARB::explicit_uniform_location, GL320);
        _setRequiredVersion(ARB::shading_language_420pack, GL320);
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((detectedDriver() & DetectedDriver::Svga3D) &&
       isExtensionSupported<Extensions::ARB::get_texture_sub_image>() &&
       !isDriverWorkaroundDisabled("svga3d-gettexsubimage-oob-write"))
        _setRequiredVersion(ARB::get_texture_sub_image, None);
    #endif

    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(detectedDriver() & Context::DetectedDriver::SwiftShader) {
        if((isExtensionSupported<Extensions::ANGLE::instanced_arrays>() ||
            isExtensionSupported<Extensions::EXT::instanced_arrays>()) &&
           !isDriverWorkaroundDisabled("swiftshader-no-es2-draw-instanced-entrypoints")) {
            _setRequiredVersion(ANGLE::instanced_arrays, None);
            _setRequiredVersion(EXT::instanced_arrays, None);
        }

        if(isExtensionSupported<Extensions::OES::texture_3D>() &&
           !isDriverWorkaroundDisabled("swiftshader-no-es2-oes-texture-3d-entrypoints"))
            _setRequiredVersion(OES::texture_3D, None);
    }
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if((detectedDriver() & Context::DetectedDriver::SwiftShader) &&
       !isDriverWorkaroundDisabled("swiftshader-broken-shader-vertex-id"))
        _setRequiredVersion(MAGNUM::shader_vertex_id, None);
    #endif

    #if defined(CORRADE_TARGET_ANDROID) && defined(MAGNUM_TARGET_GLES)
    if((detectedDriver() & Context::DetectedDriver::ArmMali) &&
        std::getenv("SHELL") && !isDriverWorkaroundDisabled("arm-mali-timer-queries-oom-in-shell"))
        _setRequiredVersion(EXT::disjoint_timer_query, None);
    #endif

    #undef _setRequiredVersion
}

}}
