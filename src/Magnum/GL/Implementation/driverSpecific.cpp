/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringIterable.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/Math/Range.h"

#if defined(MAGNUM_TARGET_WEBGL) && defined(CORRADE_TARGET_EMSCRIPTEN)
/* Including any Emscripten header should also make __EMSCRIPTEN_major__ etc
   macros available, independently of whether they're passed implicitly (before
   version 3.1.23) or taken from a version header (after version 3.1.4).
   https://github.com/emscripten-core/emscripten/commit/f99af02045357d3d8b12e63793cef36dfde4530a
   https://github.com/emscripten-core/emscripten/commit/f76ddc702e4956aeedb658c49790cc352f892e4c */
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

namespace Magnum { namespace GL {

namespace {

using namespace Containers::Literals;

/* Search the code for the following strings to see where they are implemented */
constexpr Containers::StringView KnownWorkarounds[]{
/* [workarounds] */
#if defined(CORRADE_TARGET_ANDROID) && defined(MAGNUM_TARGET_GLES)
/* Android Emulator can run with a SwiftShader GPU and thus needs some of the
   SwiftShader context creation workarounds. However, it's impossible to
   detect, as EGL_VERSION is always "1.4 Android META-EGL" and EGL_VENDOR
   always "Android". As there's nothing that would hint at SwiftShader being
   used, we conservatively assume every emulator can be a SwiftShader. But
   that's not easy either, the only vague hint that we're dealing with an
   emulator is the HOSTNAME env var, which is set to e.g. generic_x86, but to
   e.g. HWVTR on a device, so try that. */
"android-generic-hostname-might-be-swiftshader"_s,
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
/* ANGLE's shader linker insists on returning a message consisting of a
   single newline on success, causing annoying noise in the console. Similar to
   "intel-windows-chatty-shader-compiler". Not present on WebGL, apparently
   browsers filter the noise out on their own. */
"angle-chatty-shader-compiler"_s,
#endif

#ifdef MAGNUM_TARGET_GLES
/* ANGLE has a buggy bounds validation when drawing a mesh with instanced
   attributes added (with divisor set) using non-instanced glDraw*() APIs (in
   particular, when instance count is 1). This should be allowed according to
   the GL spec, which describes e.g. glDrawElements() as a special case of
   the "virtual" glDrawElementsOneInstance(). To work around the validation
   bug, gl*Draw*Instanced() is used unconditionally for all meshes that have
   instanced attributes. A test that triggers this issue is in
   MeshGLTest::drawInstancedAttributeSingleInstance(). */
"angle-instanced-attributes-always-draw-instanced"_s,
#endif

#if defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_GLES)
/* Calling glBufferData(), glMapBuffer(), glMapBufferRange() or glUnmapBuffer()
   on ANY buffer when ANY buffer is attached to a currently bound
   GL_TEXTURE_BUFFER crashes in gleUpdateCtxDirtyStateForBufStampChange deep
   inside Apple's GLengine. This can be worked around by unbinding all buffer
   textures before attempting to do such operation.

   A previous iteration of this workaround was to remember if a buffer is
   attached to a buffer texture, temporarily detaching it, calling given
   data-modifying API and then attaching it back with the same parameters.
   Unfortunately we also had to cache the internal texture format, as
   GL_TEXTURE_INTERNAL_FORMAT query is broken for buffer textures as well,
   returning always GL_R8 (the spec-mandated default). "Fortunately" macOS
   doesn't support ARB_texture_buffer_range so we didn't need to store also
   offset/size, only texture ID and its internal format, wasting 8 bytes per
   Buffer instance. HOWEVER, then we discovered this is not enough and also
   completely unrelated buffers suffer from the same crash. Fixing that
   properly in a similar manner would mean going through all live buffer
   texture instances and temporarily detaching their buffer when doing *any*
   data modification on *any* buffer, which would have extreme perf
   implications. So FORTUNATELY unbinding the textures worked around this too,
   and is a much nicer workaround after all. */
"apple-buffer-texture-unbind-on-buffer-modify"_s,
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
/* Qualcomm Adreno drivers V@0615.65 (and possibly others) report __VERSION__
   as 300 even for GLSL ES 3.10 and 3.20, breaking version-dependent shader
   code. */
"adreno-glsl-version-stuck-at-300"_s,
#endif

#if defined(CORRADE_TARGET_ANDROID) && defined(MAGNUM_TARGET_GLES)
/* glBeginQuery() with GL_TIME_ELAPSED causes a GL_OUT_OF_MEMORY error when
   running from the Android shell (through ADB). No such error happens in an
   APK. Detecting using the $SHELL environment variable and disabling
   GL_EXT_disjoint_timer_query in that case. */
"arm-mali-timer-queries-oom-in-shell"_s,
#endif

#if !defined(MAGNUM_TARGET_GLES) && defined(CORRADE_TARGET_WINDOWS)
/* ARB_direct_state_access on AMD Windows drivers has broken
   glTextureSubImage3D() / glGetTextureImage() on cube map textures (but not
   cube map arrays), always failing with erros like
   `glTextureSubImage3D has generated an error (GL_INVALID_VALUE)` if Z size or
   offset is larger than 1. Working around that by up/downloading
   slice-by-slice using non-DSA APIs, similarly to the
   svga3d-texture-upload-slice-by-slice workaround. The compressed image up/
   download is affected as well, but we lack APIs for easy format-dependent
   slicing and offset calculation, so those currently still fail. */
"amd-windows-cubemap-image3d-slice-by-slice"_s,

/* AMD Windows drivers have broken the DSA glCopyTextureSubImage3D(), returning
   GL_INVALID_VALUE. The non-DSA code path works. */
"amd-windows-broken-dsa-cubemap-copy"_s,

/* AMD Windows glCreateQueries() works for everything except
   GL_TRANSFORM_FEEDBACK_[STREAM_]OVERFLOW, probably they just forgot to adapt
   it to this new GL 4.6 addition. Calling the non-DSA code path in that case
   instead. Similar to "mesa-dsa-createquery-except-pipeline-stats". */
"amd-windows-dsa-createquery-except-xfb-overflow"_s,
#endif

#if !defined(MAGNUM_TARGET_GLES) && !defined(CORRADE_TARGET_APPLE)
/* Creating core context with specific version on AMD and NV proprietary
   drivers on Linux/Windows and Intel drivers on Windows causes the context to
   be forced to given version instead of selecting latest available version */
"no-forward-compatible-core-context"_s,
#endif

#if !defined(MAGNUM_TARGET_GLES) && defined(CORRADE_TARGET_WINDOWS)
/* On Windows Intel drivers ARB_shading_language_420pack is exposed in GLSL
   even though the extension (e.g. binding keyword) is not supported */
"intel-windows-glsl-exposes-unsupported-shading-language-420pack"_s,
#endif

#ifndef MAGNUM_TARGET_GLES
/* Mesa glCreateQueries() works for everything except stuff from GL 4.6
   ARB_pipeline_statistics_query, probably just forgotten. Calling the non-DSA
   code path in that case instead. Similar to
   "amd-windows-dsa-createquery-except-xfb-overflow". */
"mesa-dsa-createquery-except-pipeline-stats"_s,

/* Forward-compatible GL contexts on Mesa still report line width range as
   [1, 7], but setting wide line width fails. According to the specs the max
   value on forward compatible contexts should be 1.0, so patching it. */
"mesa-forward-compatible-line-width-range"_s,
#endif

#if !defined(MAGNUM_TARGET_GLES2) && defined(CORRADE_TARGET_WINDOWS)
/* On Windows NVidia drivers the glTransformFeedbackVaryings() does not make a
   copy of its char* arguments so it fails at link time when the original char
   arrays are not in scope anymore. Enabling *synchronous* debug output
   circumvents this bug. Can be triggered by running TransformFeedbackGLTest
   with GL_KHR_debug extension disabled. */
"nv-windows-dangling-transform-feedback-varying-names"_s,
#endif

#ifndef MAGNUM_TARGET_GLES
/* Layout qualifier causes compiler error with GLSL 1.20 on Mesa, GLSL 1.30 on
   NVidia and 1.40 on macOS. Everything is fine when using a newer GLSL
   version. */
"no-layout-qualifiers-on-old-glsl"_s,

/* NVidia drivers (358.16) report compressed block size from internal format
   query in bits instead of bytes */
"nv-compressed-block-size-in-bits"_s,

/* NVidia drivers (358.16) report different compressed image size for cubemaps
   based on whether the texture is immutable or not and not based on whether
   I'm querying all faces (ARB_DSA) or a single face (non-DSA, EXT_DSA) */
"nv-cubemap-inconsistent-compressed-image-size"_s,

/* NVidia drivers (358.16) return only the first slice of compressed cube map
   image when querying all six slices using the ARB_DSA API */
"nv-cubemap-broken-full-compressed-image-query"_s,

/* NVidia drivers return 0 when asked for GL_CONTEXT_PROFILE_MASK, so it needs
   to be worked around by asking for GL_ARB_compatibility */
"nv-zero-context-profile-mask"_s,

/* (Headless) EGL contexts for desktop GL on NVidia 384 and 390 drivers don't
   have correct statically linked GL 1.0 and 1.1 functions (such as
   glGetString()) and one has to retrieve them explicitly using
   eglGetProcAddress(). Doesn't seem to happen on pre-384 and 396, but it's not
   possible to get driver version through EGL, so enabling this unconditionally
   on all EGL NV contexts. */
"nv-egl-incorrect-gl11-function-pointers"_s,

/* On NV driver 450.80.02, eglQueryDeviceAttribEXT() segfaults when querying
   GPUs that the user does not have access to (i.e. via cgroup). Instead,
   always call eglQueryDeviceStringEXT() as that doesn't segfault and sets an
   error that can be retrieved via eglGetError() to see if the user has access
   to that device. On well-behaved driver versions, eglQueryDeviceAttribEXT()
   returns false instead of segfaulting. */
"nv-egl-crashy-query-device-attrib"_s,

/* On NV driver 572.83 and likely 566.24 as well, DSA buffer APIs don't work.
   This was reported on Windows with a NVIDIA RTX 2000 ADA generation graphics
   card, and downgrading to 556.39 fixes that. On Arch, RTX 3050 and 570.86 it
   doesn't happen. Not sure if it's really specific to that GPU generation or
   it's just a regression in the platform-independent GL frontend that affects
   only some cards somehow.

   The behavior is similar to the one explained below in the
   "intel-windows-crazy-broken-buffer-dsa" workaround (ImGui rendering
   flickering a lot), but as I cannot reproduce locally I'm not doing the same
   investigation. Yet at least. */
"nv-broken-buffer-dsa"_s,
#endif

#ifndef MAGNUM_TARGET_GLES
/* SVGA3D (VMware host GL driver) glDrawArrays() draws nothing when the vertex
   buffer memory is initialized using glNamedBufferData() from ARB_DSA. Using
   the non-DSA glBufferData() works. */
"svga3d-broken-dsa-bufferdata"_s,

/* SVGA3D does out-of-bound writes in some cases of glGetTexSubImage(), leading
   to memory corruption on client machines. That's nasty, so the whole
   ARB_get_texture_sub_image is disabled. */
"svga3d-gettexsubimage-oob-write"_s,
#endif

/* SVGA3D has broken handling of glTex[ture][Sub]Image*D() for 1D arrays, 2D
   arrays, 3D textures and cube map textures where it uploads just the first
   slice in the last dimension. This is only with copies from host memory, not
   with buffer images. Seems to be fixed in Mesa 13, but I have no such system
   to verify that on.
   https://github.com/mesa3d/mesa/commit/2aa9ff0cda1f6ad97c83d5583fab7a84efabe19e */
"svga3d-texture-upload-slice-by-slice"_s,

#if defined(CORRADE_TARGET_EMSCRIPTEN) && defined(__EMSCRIPTEN_PTHREADS__)
/* Shader sources containing UTF-8 characters are converted to empty strings
   when running on Emscripten with -s USE_PTHREADS=1. Working around that by
   replacing all chars > 127 with spaces. Relevant code:
   https://github.com/kripken/emscripten/blob/7f89560101843198787530731f40a65288f6f15f/src/fetch-worker.js#L54-L58 */
"emscripten-pthreads-broken-unicode-shader-sources"_s,
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
/* Empty EGL_CONTEXT_FLAGS_KHR cause SwiftShader 3.3 to fail context creation
   with EGL_BAD_ATTRIBUTE. Not sending the flags then. Relevant code:
    https://github.com/google/swiftshader/blob/5fb5e817a20d3e60f29f7338493f922b5ac9d7c4/src/OpenGL/libEGL/libEGL.cpp#L794-L810
   Version 4.1 suffers from the same thing, but 4.0 on Android not, for some
   reason. */
"swiftshader-no-empty-egl-context-flags"_s,

/* SwiftShader 3.3.0.1 crashes deep inside eglMakeCurrent() when using
   EGL_NO_SURFACE. Supplying a 32x32 PBuffer to work around that. (Android's)
   SwiftShader 4.0 needs it too, but doesn't crash, only fails to make the
   context current with EGL_BAD_MATCH. Version 4.1 doesn't seem to need this
   workaround anymore. */
"swiftshader-egl-context-needs-pbuffer"_s,
#endif

#if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/* SwiftShader 4.1.0 on ES2 contexts reports GL_ANGLE_instanced_arrays and
   GL_EXT_instanced_arrays but has no glDrawArraysInstancedANGLE /
   glDrawArraysInstancedEXT nor glDrawElementsInstancedANGLE /
   glDrawElementsInstancedEXT entrypoints, only the unsuffixed versions for
   ES3. OTOH, glVertexAttribDivisor is there for both ANGLE and EXT. Relevant
   code: https://github.com/google/swiftshader/blob/ad5c2952ca88730c07e04f6f1566194b66860c26/src/OpenGL/libGLESv2/libGLESv2.cpp#L6352-L6357
   Disabling the two extensions on ES2 contexts to avoid nullptr crashes. */
"swiftshader-no-es2-draw-instanced-entrypoints"_s,

/* SwiftShader 4.1.0 on ES2 contexts reports GL_OES_texture_3D but from all its
   entrypoints only glTexImage3DOES is present, all others are present only in
   the ES3 unsuffixed versions. Relevant code:
   https://github.com/google/swiftshader/blob/ad5c2952ca88730c07e04f6f1566194b66860c26/src/OpenGL/libGLESv2/libGLESv2.cpp#L6504
   Disabling the extension on ES2 contexts to avoid nullptr crashes. */
"swiftshader-no-es2-oes-texture-3d-entrypoints"_s,
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
/* SwiftShader 4.1.0 has special handling for binding buffers to the transform
   feedback target, requiring an XFB object to be active when a buffer is bound
   to GL_TRANSFORM_FEEDBACK_BUFFER and ignoring the glBindBuffer() call
   otherwise. No other driver does that. As a workaround, setting
   Buffer::TargetHint::TransformFeedback will make it use
   Buffer::TargetHint::Array instead, as that works okay. */
"swiftshader-broken-xfb-buffer-binding-target"_s,

/* SwiftShader 4.1.0 does implement gl_VertexID for ES3 contexts, but in
   practice it doesn't work, returning a constant value. In order to make this
   easier to check, there's a dummy MAGNUM_shader_vertex_id extension that's
   defined on all GL 3.0+ and GLES 3.0+ / WebGL 2+ contexts *except* for
   SwiftShader. */
"swiftshader-broken-shader-vertex-id"_s,
#endif

#ifndef MAGNUM_TARGET_GLES
/* Even with the DSA variant, where GL_IMPLEMENTATION_COLOR_READ_* is passed to
   glGetNamedFramebufferParameter(), Mesa complains that the framebuffer is not
   bound for reading. Relevant code:
   https://github.com/mesa3d/mesa/blob/212c0c630a849e4737e2808a993d708cbb2f18f7/src/mesa/main/framebuffer.c#L841-L843
   Workaround is to explicitly bind the framebuffer for reading. */
"mesa-implementation-color-read-format-dsa-explicit-binding"_s,
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
"intel-windows-implementation-color-read-format-completely-broken"_s,
#endif

#if !defined(MAGNUM_TARGET_GLES) && defined(CORRADE_TARGET_WINDOWS)
/* Intel drivers on Windows have some synchronization / memory alignment bug in
   the DSA glNamedBufferData() when the same buffer is set as an index buffer
   to a mesh right after or repeatedly. Calling glBindBuffer() right before or
   after the data upload fixes the issue. The above is reproducible with the
   2019.01 ImGui example, and used to be worked around in a more hopeful way.
   However, the reports about things going *bad* in heavier ImGui-based apps
   didn't stop with that and none of my tests were able to reproduce anything.
   Since I lost patience already, I'm disabling the DSA code paths for
   everything related to buffers. (Two weeks pass.) But wait! while that fixed
   all issues for *some* users, it made things completely broken elsewhere,
   causing an app to render just a clear color and nothing else. The cancer
   apparently spread further, so I'm disabling all VAO-related DSA code paths
   as well now. Workarounds listed separately, in case someone might want to
   dig further or experience the misery of only one of them being active.

   To save you time experimenting:

   - (Epilepsy warning!) With the former disabled and no matter whether the
     second is disabled or not, the ImGui example (or any other ImGui-based
     app, really), the screen will start flickering heavily under *some*
     circumstances. This is known since drivers 24 at least.
   - With the former enabled and the second disabled, you might either
     experience a total doom, where just the framebuffer clear color is
     visible, or your app is totally fine. This is reproducible with drivers 25
     or 26 at least. Note that modifying the code to enable this workaround on
     other drivers (AMD on Windows, e.g.) doesn't break anything, so it's not
     like the workaround would be incomplete with some code paths still relying
     on DSA that's not there. It's clearly Intel drivers fault.
   - With both enabled, things seem to be fine, and I hope it stays that way
     also for future driver updates.

   See also the "nv-broken-buffer-dsa" workaround, which is similar. */
"intel-windows-crazy-broken-buffer-dsa"_s,
"intel-windows-crazy-broken-vao-dsa"_s,

/* ARB_direct_state_access implementation on Intel Windows drivers has broken
   *everything* related to cube map textures (but not cube map arrays) -- data
   upload, data queries, framebuffer attachment, framebuffer copies, all
   complaining about "Wrong <func> 6 provided for <target> 34067" and similar
   (GL_TEXTURE_CUBE_MAP is 34067). Using the non-DSA code paths as a
   workaround (for the 3D image up/download as well). */
"intel-windows-broken-dsa-for-cubemaps"_s,

/* DSA glBindTextureUnit() on Intel Windows drivers simply doesn't work when
   passing 0 to it. Non-zero IDs work correctly except for cube maps. Using the
   non-DSA code path for unbinding and cube maps as a workaround. */
"intel-windows-half-baked-dsa-texture-bind"_s,

/* DSA glNamedFramebufferTexture() on Intel Windows drivers doesn't work for
   layered cube map array attachments. Non-layered or non-array cube map
   attachment works. Using the non-DSA code path as a workaround. */
"intel-windows-broken-dsa-layered-cubemap-array-framebuffer-attachment"_s,

/* DSA glClearNamedFramebuffer*() on Intel Windows drivers doesn't do anything.
   Using the non-DSA code path as a workaournd. See also
   "mesa-broken-dsa-framebuffer-clear" below. */
"intel-windows-broken-dsa-framebuffer-clear"_s,

/* Using DSA glCreateQueries() on Intel Windows drivers breaks
   glBeginQueryIndexed(). Using the non-DSA glGenQueries() instead makes it
   work properly. See TransformFeedbackGLTest for a test. */
"intel-windows-broken-dsa-indexed-queries"_s,

/* DSA-ified "vertex layout" glVertexArrayAttribIFormat() is broken when
   passing shorts instead of full 32bit ints. Using the old-style
   glVertexAttribIPointer() works correctly. No idea if the non-DSA
   glVertexAttribIFormat() works or not. A test that triggers this issue is in
   MeshGLTest::addVertexBufferIntWithShort(). */
"intel-windows-broken-dsa-integer-vertex-attributes"_s,

/* Shader compiler on Intel Windows drivers insists on telling me "No errors."
   when it should just stay silent. See also "angle-chatty-shader-compiler". */
"intel-windows-chatty-shader-compiler"_s,

/* When using more than just a vertex and fragment shader (geometry shader,
   e.g.), ARB_explicit_uniform_location on Intel silently uses wrong
   locations, blowing up with either a non-descript
    Error has been generated. GL error GL_INVALID_OPERATION in ProgramUniformMatrix4fv: (ID: 2052228270) Generic error
   or, if you are lucky, a highly-cryptic-but-still-better-than-nothing
    Error has been generated. GL error GL_INVALID_OPERATION in ProgramUniform4fv: (ID: 1725519030) GL error GL_INVALID_OPERATION: mismatched type setting uniform of location "3" in program 1, "" using shaders, 2, "", 3, "", 8, ""
   *unless* you have vertex uniform locations first, fragment locations second
   and geometry locations last. Another case is happening with color for a
   Flat3D shader --  because a (compiled out / unused) texture matrix was at
   location 1, setting color to location 2 didn't work, ending up with a
   Generic error again (driver version 27). Because this is impossible to
   prevent, the extension is completely disabled on all Intel Windows drivers. */
"intel-windows-explicit-uniform-location-is-less-explicit-than-you-hoped"_s,
#endif

#ifndef MAGNUM_TARGET_GLES
/* Mesa 24 (or, 24.2 at least) crashes on exit deep inside X11 if the DSA
   glClearNamedFramebuffer() APIs are used. Not sure what's up, couldn't find
   any relevant changelog entry and unfortunately the previous version I had
   was only 23.3.5, so it could be anything in between. My hunch is that it's
   due to some new code that deals with framebuffer compression and which was
   only correctly cleaned up in the non-DSA code path. Or something. See also
   "intel-windows-broken-dsa-framebuffer-clear" above. */
"mesa-broken-dsa-framebuffer-clear"_s,
#endif

#ifndef MAGNUM_TARGET_GLES
/* NVidia seems to be returning values for the default framebuffer when
   GL_IMPLEMENTATION_COLOR_READ_FORMAT and _TYPE is queried using
   glGetNamedFramebufferParameter(). Using either glGetInteger() or
   glGetFramebufferParameter() works correctly. */
"nv-implementation-color-read-format-dsa-broken"_s,
#endif

#ifndef MAGNUM_TARGET_GLES
/* ApiTrace needs an explicit initial glViewport() call to initialize its
   framebuffer size, otherwise it assumes it's zero-sized. */
"apitrace-zero-initial-viewport"_s,
#endif

#if defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
/* While the EXT_disjoint_timer_query extension should be only on WebGL 1 and
   EXT_disjoint_timer_query_webgl2 only on WebGL 2, Firefox reports
   EXT_disjoint_timer_query on both. The entry points work correctly however,
   so this workaround makes Magnum pretend EXT_disjoint_timer_query_webgl2 is
   available when it detects EXT_disjoint_timer_query on WebGL 2 builds on
   Firefox. See also https://bugzilla.mozilla.org/show_bug.cgi?id=1328882,
   https://www.khronos.org/webgl/public-mailing-list/public_webgl/1705/msg00015.php
   and https://github.com/emscripten-core/emscripten/pull/9652 for the
   Emscripten-side part of this workaround. */
"firefox-fake-disjoint-timer-query-webgl2"_s,
#endif

#ifdef MAGNUM_TARGET_WEBGL
/* Firefox 92+ says "WEBGL_debug_renderer_info is deprecated in Firefox and
   will be removed. Please use RENDERER." if attempting to use the unmasked
   renderer / vendor string. The information is provided through the regular
   APIs instead. Disabling the extension if present on the new versions to
   avoid console spam. */
"firefox-deprecated-debug-renderer-info"_s
#endif
/* [workarounds] */
};

/* I could use std::find(), right? Well, it'd be a whole lot more typing and
   an #include <algorithm> *and* #include <iterator> or whatever as well,
   because apparently ONE CAN'T GET std::begin() / std::end() without including
   tens thousands lines of irrelevant shit, FFS.

   Also the comparison to array end to discover if it wasn't found is just a
   useless verbose crap shit as well, so we'll do better here and return a null
   view instead.

   Since the workaround list isn't really huge for an average platform (16 on
   Linux, Windows probably ~30?) and there's very few used heavily, I won't
   bother with some binary search, which needs extra testing effort. */
Containers::StringView findWorkaround(Containers::StringView workaround) {
    for(Containers::StringView i: KnownWorkarounds)
        if(workaround == i) return i;
    return {};
}

}

auto Context::detectedDriver() -> DetectedDrivers {
    if(_detectedDrivers) return *_detectedDrivers;

    _detectedDrivers = DetectedDrivers{};

    #if !defined(CORRADE_TARGET_APPLE) || defined(MAGNUM_TARGET_GLES)
    const Containers::StringView renderer =
        #ifndef MAGNUM_TARGET_WEBGL
        rendererString()
        #else
        rendererStringUnmasked()
        #endif
        ;
    #endif
    #ifndef CORRADE_TARGET_APPLE
    const Containers::StringView vendor =
        #ifndef MAGNUM_TARGET_WEBGL
        vendorString()
        #else
        vendorStringUnmasked()
        #endif
        ;
    #endif
    #if !defined(CORRADE_TARGET_APPLE) && !defined(MAGNUM_TARGET_WEBGL)
    const Containers::StringView version = versionString();
    #endif

    /* In some cases we can have a combination of drivers (e.g. ANGLE running
       on top of Mesa, Mesa Zink running on top of NVidia drivers...) so the
       detection has no early returns. */

    /* Apple has its own drivers */
    /** @todo when Zink on Apple is a thing, this needs an update */
    #ifndef CORRADE_TARGET_APPLE
    /* AMD binary desktop drivers */
    if(vendor.contains("ATI Technologies Inc."_s))
        *_detectedDrivers |= DetectedDriver::Amd;

    #ifdef CORRADE_TARGET_WINDOWS
    /* Intel Windows drivers */
    if(vendor.contains("Intel"_s))
        *_detectedDrivers |= DetectedDriver::IntelWindows;
    #endif

    /* Mesa drivers.
        -   Intel GL/GLES drivers contain Mesa both in renderer and version
            string
        -   AMD GL/GLES drivers have Mesa only in the version string
        -   On WebGL the version string is useless, always saying just
            "WebGL X.Y (Chromium)" or "WebGL X.Y"
        -   Mesa is in renderer string in Chromium and nowhere in Firefox for
            some reason

       To avoid misdetecting the driver, simply test both where it makes
       sense. */
    if(renderer.contains("Mesa"_s)
        #ifndef MAGNUM_TARGET_WEBGL
        || version.contains("Mesa"_s)
        #endif
    ) {
        *_detectedDrivers |= DetectedDriver::Mesa;

        if(renderer.contains("SVGA3D"_s))
            *_detectedDrivers |= DetectedDriver::Svga3D;

        *_detectedDrivers;
    }

    if(vendor.contains("NVIDIA Corporation"_s))
        *_detectedDrivers |= DetectedDriver::NVidia;
    #endif

    #ifdef MAGNUM_TARGET_GLES
    /* ANGLE. On WebGL only if we are so lucky and have access to the unmasked
       renderer string. */
    if(renderer.contains("ANGLE"_s))
        *_detectedDrivers |= DetectedDriver::Angle;
    #ifdef MAGNUM_TARGET_WEBGL
    /* If the unmasked renderer string is not available, try other means */
    /** @todo this (and below) incorrectly detects ANGLE on FF 92+, as it has
        the WEBGL_debug_renderer_info disabled */
    else if(!isExtensionSupported<Extensions::WEBGL::debug_renderer_info>()) {
        /* Otherwise assume ANGLE is present if the ANGLE_instanced_arrays is
           present on WebGL 1. Although e.g. Firefox exposes it even though it
           renders directly through GL drivers on Linux, so this may catch more
           drivers than just ANGLE. */
        #ifdef MAGNUM_TARGET_GLES2
        if(isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        /* Or if WEBGL_multi_draw (which is based on ANGLE_multi_draw) is
           present on WebGL 2. This extension is rather recent (appearing in
           browsers in late 2020) so it may not catch all ANGLE
           implementations. */
        #else
        if(isExtensionSupported<Extensions::WEBGL::multi_draw>())
        #endif
            *_detectedDrivers |= DetectedDriver::Angle;
        /* Otherwise try to detect a D3D ANGLE backend by querying line width.
           It's always exactly just 1 on D3D, usually (but not always) more on
           GL, not sure about Metal. So this is not a 100% match. Sources:
           http://stackoverflow.com/a/20149090 and http://webglreport.com */
        else {
            Range1Di range;
            glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, range.data());
            if(range.min() == 1 && range.max() == 1 && vendor != "Internet Explorer"_s) {
                *_detectedDrivers |= DetectedDriver::Angle;
            }
        }
    }
    #endif

    /* SwiftShader */
    if(renderer.contains("SwiftShader"_s))
        *_detectedDrivers |= DetectedDriver::SwiftShader;
    #endif

    #ifdef CORRADE_TARGET_ANDROID
    if(vendor.contains("ARM"_s) && renderer.contains("Mali"_s))
        *_detectedDrivers |= DetectedDriver::ArmMali;
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(CORRADE_TARGET_APPLE)
    if(vendor.contains("Qualcomm"_s) && renderer.contains("Adreno"_s))
        *_detectedDrivers |= DetectedDriver::QualcommAdreno;
    #endif

    return *_detectedDrivers;
}

void Context::disableDriverWorkaround(const Containers::StringView workaround) {
    /* Find the workaround. Note that we'll add the found view to the array
       and not the passed view, as the found view is guaranteed to stay in
       scope */
    const Containers::StringView found = findWorkaround(workaround);

    /* Ignore unknown workarounds */
    /** @todo this will probably cause false positives when both GL and Vulkan
        is used together? */
    if(found.isEmpty()) {
        Warning{} << "GL: unknown workaround" << workaround;
        return;
    }

    arrayAppend(_driverWorkarounds, InPlaceInit, found, true);
}

bool Context::isDriverWorkaroundDisabled(const Containers::StringView workaround) {
    /* Find the workaround. Note that we'll add the found view to the array
       and not the passed view, as the found view is guaranteed to stay in
       scope */
    Containers::StringView found = findWorkaround(workaround);
    CORRADE_INTERNAL_ASSERT(!found.isEmpty());

    /* If the workaround was already asked for or disabled, return its state,
       otherwise add it to the list as used one. Here we again cheat a bit and
       compare just data pointers instead of the whole string as we store only
       the views in the KnownWorkarounds list. */
    for(const auto& i: _driverWorkarounds)
        if(i.first().data() == found.data()) return i.second();
    arrayAppend(_driverWorkarounds, InPlaceInit, found, false);
    return false;
}

void Context::setupDriverWorkarounds() {
    #define _setRequiredVersion(extension, version)                           \
        if(_extensionRequiredVersion[Extensions::extension::Index] < Version::version) \
            _extensionRequiredVersion[Extensions::extension::Index] = Version::version

    /* Using WEBGL_debug_renderer_info results in deprecation warnings on
       Firefox 92+, Firefox 92+ exposes the unmasked renderer and vendor string
       through the usual APIs. Needs to be above the code that explicitly
       enables the extension! */
    #if defined(MAGNUM_TARGET_WEBGL) && defined(CORRADE_TARGET_EMSCRIPTEN)
    /* Assuming the extension gets eventually removed, check for Firefox
       version only if the extension is actually present. Then first detect the
       version and only then ask if the workaround is disabled in order to
       avoid having the workaround listed as used on older versions or other
       browsers */
    if(isExtensionSupported<Extensions::WEBGL::debug_renderer_info>()) {
        /* Unlike other EM_ASM() macros, this one isn't put into a JS library
           as it neither has any dependencies nor has code that may benefit
           from settings-based preprocessing done for minification */
        #pragma GCC diagnostic push
        /* The damn thing moved the warning to another group in some version.
           Not sure if it happened in Clang 10 already, but -Wc++20-extensions
           is new in Clang 10, so just ignore both. HOWEVER, Emscripten often
           uses a prerelease Clang, so if it reports version 10, it's likely
           version 9. So check for versions _above_ 10 instead. */
        #pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
        #if __clang_major__ > 10
        #pragma GCC diagnostic ignored "-Wc++20-extensions"
        #endif
        const Int firefoxVersion = EM_ASM_INT({
            var match = navigator.userAgent.match(/Firefox\\\\/(\\\\d+)/);
            if(match) return match[1]|0; /* coerce to an int (remember asm.js?) */
            return 0;
        });
        #pragma GCC diagnostic pop
        if(firefoxVersion >= 92 && !isDriverWorkaroundDisabled("firefox-deprecated-debug-renderer-info"_s))
            _setRequiredVersion(WEBGL::debug_renderer_info, None);
    }
    #endif

    /* WEBGL_debug_renderer_info needs to be explicitly requested,
       independently of whether Emscripten was told to implicitly request
       extensions or not. Has to be done before any call to detectedDriver(),
       which relies on this extension, but only after all other workarounds
       that disable it! */
    #if defined(MAGNUM_TARGET_WEBGL) && defined(CORRADE_TARGET_EMSCRIPTEN)
    if(isExtensionSupported<Extensions::WEBGL::debug_renderer_info>()) {
        CORRADE_INTERNAL_ASSERT_OUTPUT(emscripten_webgl_enable_extension(emscripten_webgl_get_current_context(), "WEBGL_debug_renderer_info"));
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(!isDriverWorkaroundDisabled("no-layout-qualifiers-on-old-glsl"_s)) {
        _setRequiredVersion(ARB::explicit_attrib_location, GL320);
        _setRequiredVersion(ARB::explicit_uniform_location, GL320);
        _setRequiredVersion(ARB::shading_language_420pack, GL320);
    }

    #ifdef CORRADE_TARGET_WINDOWS
    if((detectedDriver() & DetectedDriver::IntelWindows) && !isExtensionSupported<Extensions::ARB::shading_language_420pack>() && !isDriverWorkaroundDisabled("intel-windows-glsl-exposes-unsupported-shading-language-420pack"_s))
        _setRequiredVersion(ARB::shading_language_420pack, None);

    if((detectedDriver() & DetectedDriver::IntelWindows) && isExtensionSupported<Extensions::ARB::explicit_uniform_location>() && !isDriverWorkaroundDisabled("intel-windows-explicit-uniform-location-is-less-explicit-than-you-hoped"_s)) {
        _setRequiredVersion(ARB::explicit_uniform_location, None);
    }
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if((detectedDriver() & DetectedDriver::Svga3D) &&
       isExtensionSupported<Extensions::ARB::get_texture_sub_image>() &&
       !isDriverWorkaroundDisabled("svga3d-gettexsubimage-oob-write"_s))
        _setRequiredVersion(ARB::get_texture_sub_image, None);
    #endif

    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(detectedDriver() & Context::DetectedDriver::SwiftShader) {
        if((isExtensionSupported<Extensions::ANGLE::instanced_arrays>() ||
            isExtensionSupported<Extensions::EXT::instanced_arrays>()) &&
           !isDriverWorkaroundDisabled("swiftshader-no-es2-draw-instanced-entrypoints"_s)) {
            _setRequiredVersion(ANGLE::instanced_arrays, None);
            _setRequiredVersion(EXT::instanced_arrays, None);
        }

        if(isExtensionSupported<Extensions::OES::texture_3D>() &&
           !isDriverWorkaroundDisabled("swiftshader-no-es2-oes-texture-3d-entrypoints"_s))
            _setRequiredVersion(OES::texture_3D, None);
    }
    #endif

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if((detectedDriver() & Context::DetectedDriver::SwiftShader) &&
       !isDriverWorkaroundDisabled("swiftshader-broken-shader-vertex-id"_s))
        _setRequiredVersion(MAGNUM::shader_vertex_id, None);
    #endif

    #if defined(CORRADE_TARGET_ANDROID) && defined(MAGNUM_TARGET_GLES)
    if((detectedDriver() & Context::DetectedDriver::ArmMali) &&
        std::getenv("SHELL") && !isDriverWorkaroundDisabled("arm-mali-timer-queries-oom-in-shell"_s))
        _setRequiredVersion(EXT::disjoint_timer_query, None);
    #endif

    #ifdef MAGNUM_TARGET_WEBGL
    /* The WEBGL_multi_draw entrypoints are only available since Emscripten
       2.0.0: https://github.com/emscripten-core/emscripten/pull/11650
       However, the extension is advertised even on older versions and we have
       no way to link to those entrypoints there. */
    #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ < 20000
    _setRequiredVersion(WEBGL::multi_draw, None);
    #endif
    /* EXT_clip_control, EXT_polygon_offset_clamp and WEBGL_polygon_mode
       entrypoints are only available since Emscripten 3.1.66:
       https://github.com/emscripten-core/emscripten/pull/20841
       However, the extension is advertised even on older versions and we have
       no way to link to those entrypoints there. */
    #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ < 30166
    _setRequiredVersion(EXT::clip_control, None);
    _setRequiredVersion(EXT::polygon_offset_clamp, None);
    _setRequiredVersion(WEBGL::polygon_mode, None);
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    /* WEBGL_multi_draw_instanced_base_vertex_base_instance only since
       Emscripten 2.0.5: https://github.com/emscripten-core/emscripten/pull/12282 */
    #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ < 20005
    _setRequiredVersion(WEBGL::multi_draw_instanced_base_vertex_base_instance, None);
    #endif
    /* WEBGL_draw_instanced_base_vertex_base_instance only since Emscripten
       1.39.15: https://github.com/emscripten-core/emscripten/pull/11054 */
    #if __EMSCRIPTEN_major__*10000 + __EMSCRIPTEN_minor__*100 + __EMSCRIPTEN_tiny__ < 13915
    _setRequiredVersion(WEBGL::draw_instanced_base_vertex_base_instance, None);
    #endif
    #endif
    #endif

    /* WEBGL_compressed_texture_astc has an extremely silly way of reporting
       whether the LDR or HDR profile is supported. All other platforms simply
       expose a _hdr / _ldr variants of the extension, here I have to call some
       fucking getter. Restore sanity and provide this info in fake
       Magnum-specific MAGNUM_compressed_texture_astc_ldr / _hdr extensions
       instead.

       What's the most funny about this is that the extension at
        https://registry.khronos.org/webgl/extensions/WEBGL_compressed_texture_astc/
       explicitly says that
        The intent of the getSupportedProfiles function is to allow easy
        reconstruction of the underlying OpenGL or OpenGL ES extension strings
        for environments like Emscripten, by prepending the string GL_KHR_texture_compression_astc_ to the returned profile names.
       Which ... is a noble _intent_, but it only misses one small thing, to
       have someone actually TELL THE EMSCRIPTEN DEVS TO IMPLEMENT SUCH A
       THING!!! Which of course never happened. Since 2015. Goddamit.

       And even then, still, why couldn't you just do it like ALL OTHER
       PLATFORMS? WHY SUCH A STUPID SPECIAL CASE?! */
    #ifdef MAGNUM_TARGET_WEBGL
    if(isExtensionSupported<Extensions::WEBGL::compressed_texture_astc>()) {
        /* Unlike other EM_ASM() macros, this one isn't put into a JS library
           as it neither has any dependencies nor has code that may benefit
           from settings-based preprocessing done for minification */

        #pragma GCC diagnostic push
        /* The damn thing moved the warning to another group in some version.
           Not sure if it happened in Clang 10 already, but -Wc++20-extensions
           is new in Clang 10, so just ignore both. HOWEVER, Emscripten often
           uses a prerelease Clang, so if it reports version 10, it's likely
           version 9. So check for versions _above_ 10 instead. */
        #pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
        #if __clang_major__ > 10
        #pragma GCC diagnostic ignored "-Wc++20-extensions"
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        const UnsignedInt which = EM_ASM_INT({
            /* Originally this was accessing GL.contexts[$0].GLctx where $0 was
               an integer passed from emscripten_webgl_get_current_context(),
               basically relying on some undocumented part of Emscripten
               internals. Moreover it didn't work with closure compiler enabled
               (which mangled even the GL variable, causing it to be impossible
               to find). Accessing Module.canvas should work, and according to
                https://developer.mozilla.org/en-US/docs/Web/API/HTMLCanvasElement/getContext
               calling getContext() on it again (with a correct string,
               matching what was created in the first place) should return the
               existing context, not creating a new one.

               In this case, the closure compiler *may* mangle the Module name,
               but will do so in a way that sill matches it, fortunately, The
               getSupportedProfiles it will however not, so it has to be
               queried as a string. */
            var supported = Module.canvas.getContext('webgl').getExtension('WEBGL_compressed_texture_astc')['getSupportedProfiles']();
            return
                (supported.indexOf('ldr') >= 0 ? 1 : 0)|
                (supported.indexOf('hdr') >= 0 ? 2 : 0);
        });
        #else
        const UnsignedInt which = EM_ASM_INT({
            /* Because yes of course there's no way to pass a compile-time
               macro into EM_ASM_INT so I have to duplicate like a madman. I
               could pass it as an argument, but that'd be allocating a
               JavaScript string. Do I look like I want to deal with that? */
            var supported = Module.canvas.getContext('webgl2').getExtension('WEBGL_compressed_texture_astc')['getSupportedProfiles']();
            return
                (supported.indexOf('ldr') >= 0 ? 1 : 0)|
                (supported.indexOf('hdr') >= 0 ? 2 : 0);
        });
        #endif
        #pragma GCC diagnostic pop
        _extensionStatus.set(Extensions::MAGNUM::compressed_texture_astc_ldr::Index, which & 0x01);
        _extensionStatus.set(Extensions::MAGNUM::compressed_texture_astc_hdr::Index, which & 0x02);
    }
    #endif

    #undef _setRequiredVersion

    #ifndef MAGNUM_TARGET_GLES
    if(isExtensionSupported<Extensions::GREMEDY::string_marker>() &&
       !isDriverWorkaroundDisabled("apitrace-zero-initial-viewport"_s)) {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    }
    #endif

    #if defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
    /* Firefox used to report both rendererString() and vendorString() as
       Mozilla (printing silly `Renderer: Mozilla by Mozilla` in the Magnum
       startup log), but possibly in relation with WEBGL_debug_renderer_info
       being deprecated in FF 92+ this changed and now Mozilla is only in
       vendorString() and rendererString() is the actual system GPU. */
    if(vendorString() == "Mozilla"_s) {
        for(const auto& extension: extensionStrings()) {
            if(extension == "GL_EXT_disjoint_timer_query"_s && !isDriverWorkaroundDisabled("firefox-fake-disjoint-timer-query-webgl2"_s)) {
                _extensionStatus.set(Extensions::EXT::disjoint_timer_query_webgl2::Index, true);
            }
        }
    }
    #endif
}

Context::Configuration& Context::Configuration::addDisabledWorkarounds(const Containers::StringIterable& workarounds) {
    arrayReserve(_disabledWorkarounds, _disabledWorkarounds.size() + workarounds.size());

    for(const Containers::StringView workaround: workarounds) {
        /* Find the workaround. Note that we'll add the found view to the array
           and not the passed view, as the found view is guaranteed to stay in
           scope */
        const Containers::StringView found = findWorkaround(workaround);

        /* Ignore unknown workarounds */
        /** @todo this will probably cause false positives when both GL and
            Vulkan is used together? */
        if(found.isEmpty()) {
            Warning{} << "GL::Context::Configuration::addDisabledWorkarounds(): unknown workaround" << workaround;
            continue;
        }

        arrayAppend(_disabledWorkarounds, InPlaceInit, found);
    }

    return *this;
}

}}
