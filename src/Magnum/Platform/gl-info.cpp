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

#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/BufferTexture.h"
#endif
#include "Magnum/GL/Context.h"
#include "Magnum/GL/CubeMapTexture.h"
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/CubeMapTextureArray.h"
#endif
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/DebugOutput.h"
#endif
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/MultisampleTexture.h"
#endif
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/GL/RectangleTexture.h"
#endif
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/GL/Texture.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/TextureArray.h"
#include "Magnum/GL/TransformFeedback.h"
#endif

#if defined(MAGNUM_TARGET_HEADLESS) || defined(CORRADE_TARGET_EMSCRIPTEN) || defined(CORRADE_TARGET_ANDROID)
#include "Magnum/Platform/WindowlessEglApplication.h"
#elif defined(CORRADE_TARGET_IOS)
#include "Magnum/Platform/WindowlessIosApplication.h"
#elif defined(CORRADE_TARGET_APPLE)
#include "Magnum/Platform/WindowlessCglApplication.h"
#elif defined(CORRADE_TARGET_UNIX)
#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessEglApplication.h"
#else
#include "Magnum/Platform/WindowlessGlxApplication.h"
#endif
#elif defined(CORRADE_TARGET_WINDOWS)
#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessWindowsEglApplication.h"
#else
#include "Magnum/Platform/WindowlessWglApplication.h"
#endif
#else
#error no windowless application available on this platform
#endif

namespace Magnum {

/** @page magnum-gl-info Magnum GL Info
@brief Displays information about Magnum engine OpenGL capabilities

@m_footernavigation
@m_keywords{magnum-gl-info gl-info}

@m_div{m-button m-primary} <a href="https://magnum.graphics/showcase/gl-info/">@m_div{m-big}Live web version @m_enddiv @m_div{m-small} uses WebAssembly & WebGL @m_enddiv </a> @m_enddiv

This utility is built if both `WITH_GL` and `WITH_GL_INFO` is enabled when
building Magnum. To use this utility with CMake, you need to request the
`gl-info` component of the `Magnum` package and use the `Magnum::gl-info`
target for example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED gl-info)

add_custom_command(OUTPUT ... COMMAND Magnum::gl-info ...)
@endcode

See @ref building, @ref cmake and the @ref GL namespace for more information.

@section magnum-gl-info-usage Usage

@code{.sh}
magnum-info [--magnum-...] [-h|--help] [-s|--short] [--all-extensions]
    [--limits]
@endcode

Arguments:

-   `-h`, `--help` --- display this help message and exit
-   `-s`, `--short` --- display just essential info and exit
-   `--all-extensions` --- display extensions also for fully supported versions
-   `--limits` --- display also limits and implementation-defined values
-   `--magnum-...` --- engine-specific options (see
    @ref GL-Context-command-line for details)

@subsection magnum-gl-info-usage-emscripten Usage on Emscripten

When installing Magnum, point the `MAGNUM_DEPLOY_PREFIX` CMake variable to your
webserver location. The Magnum Info utility is then available in a `magnum-info/`
subdirectory of given location. You can pass all command-line arguments from
above to it via GET parameters. See the relevant section of the
@ref platforms-html5-environment "Emscripten guide" for more information.

@subsection magnum-gl-info-usage-android Usage on Android

When you have developer-enabled Android device connected or Android emulator
running, you can use ADB to upload the built executable to device temp
directory and run it on the device:

@code{.sh}
adb push build-android-arm64/src/Magnum/Platform/magnum-info /data/local/tmp
adb shell /data/local/tmp/magnum-info
@endcode

You can also use @cb{.sh} adb shell @ce to log directly into the device shell
and continue from there. All @ref magnum-gl-info-usage "command-line" arguments
are supported.

@section magnum-gl-info-example Example output

@code{.shell-session}

  +---------------------------------------------------------+
  |   Information about Magnum engine OpenGL capabilities   |
  +---------------------------------------------------------+

Used application: Platform::WindowlessGlxApplication
Compilation flags:
    CORRADE_BUILD_DEPRECATED
    CORRADE_TARGET_UNIX
    MAGNUM_BUILD_DEPRECATED

Renderer: AMD Radeon R7 M260 Series by ATI Technologies Inc.
OpenGL version: 4.5.13399 Compatibility Profile Context 15.201.1151
Using optional features:
    GL_ARB_ES2_compatibility
    GL_ARB_direct_state_access
    GL_ARB_get_texture_sub_image
    GL_ARB_invalidate_subdata
    ...
Using driver workarounds:
    no-forward-compatible-core-context
    no-layout-qualifiers-on-old-glsl
Context flags:
Supported GLSL versions:
    430 core
    430 compatibility
    420 core
    ...

Vendor extension support:
    GL_AMD_vertex_shader_layer                                    SUPPORTED
    GL_AMD_shader_trinary_minmax                                  SUPPORTED
    GL_ARB_robustness                                                 -
    GL_ARB_robustness_isolation                                       -
    ...
@endcode

@note This executable is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/

class MagnumInfo: public Platform::WindowlessApplication {
    public:
        explicit MagnumInfo(const Arguments& arguments);

        int exec() override { return 0; }
};

MagnumInfo::MagnumInfo(const Arguments& arguments): Platform::WindowlessApplication{arguments, NoCreate} {
    Utility::Arguments args;
    args.addBooleanOption('s', "short").setHelp("short", "display just essential info and exit")
        .addBooleanOption("extension-strings").setHelp("extension-strings", "list all extension strings provided by the driver (implies --short)")
        .addBooleanOption("all-extensions").setHelp("all-extensions", "display extensions also for fully supported versions")
        .addBooleanOption("limits").setHelp("limits", "display also limits and implementation-defined values")
        .addSkippedPrefix("magnum", "engine-specific options")
        .setGlobalHelp("Displays information about Magnum engine and OpenGL capabilities.");

    #ifdef CORRADE_TARGET_IOS
    {
        static_cast<void>(arguments);
        const char* iosArguments[] = { "", "--limits" };
        args.parse(2, iosArguments);
    }
    #else
    args.parse(arguments.argc, arguments.argv);
    #endif

    Debug{} << "";
    Debug{} << "  +---------------------------------------------------------+";
    Debug{} << "  |   Information about Magnum engine OpenGL capabilities   |";
    Debug{} << "  +---------------------------------------------------------+";
    Debug{} << "";

    #ifdef MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN
    Debug{} << "Used application: Platform::WindowlessEglApplication";
    #elif defined(MAGNUM_WINDOWLESSIOSAPPLICATION_MAIN)
    Debug{} << "Used application: Platform::WindowlessIosApplication";
    #elif defined(MAGNUM_WINDOWLESSCGLAPPLICATION_MAIN)
    Debug{} << "Used application: Platform::WindowlessCglApplication";
    #elif defined(MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN)
    Debug{} << "Used application: Platform::WindowlessGlxApplication";
    #elif defined(MAGNUM_WINDOWLESSWGLAPPLICATION_MAIN)
    Debug{} << "Used application: Platform::WindowlessWglApplication";
    #elif defined(MAGNUM_WINDOWLESSWINDOWSEGLAPPLICATION_MAIN)
    Debug{} << "Used application: Platform::WindowlessWindowsEglApplication";
    #else
    #error no windowless application available on this platform
    #endif
    Debug{} << "Compilation flags:";
    #ifdef CORRADE_BUILD_DEPRECATED
    Debug{} << "    CORRADE_BUILD_DEPRECATED";
    #endif
    #ifdef CORRADE_BUILD_STATIC
    Debug{} << "    CORRADE_BUILD_STATIC";
    #endif
    #ifdef CORRADE_BUILD_MULTITHREADED
    Debug{} << "    CORRADE_BUILD_MULTITHREADED";
    #endif
    #ifdef CORRADE_TARGET_UNIX
    Debug{} << "    CORRADE_TARGET_UNIX";
    #endif
    #ifdef CORRADE_TARGET_APPLE
    Debug{} << "    CORRADE_TARGET_APPLE";
    #endif
    #ifdef CORRADE_TARGET_IOS
    Debug{} << "    CORRADE_TARGET_IOS";
    #endif
    #ifdef CORRADE_TARGET_WINDOWS
    Debug{} << "    CORRADE_TARGET_WINDOWS";
    #endif
    #ifdef CORRADE_TARGET_WINDOWS_RT
    Debug{} << "    CORRADE_TARGET_WINDOWS_RT";
    #endif
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    Debug{} << "    CORRADE_TARGET_EMSCRIPTEN (" << Debug::nospace
        << __EMSCRIPTEN_major__ << Debug::nospace << "." << Debug::nospace
        << __EMSCRIPTEN_minor__ << Debug::nospace << "." << Debug::nospace
        << __EMSCRIPTEN_tiny__ << Debug::nospace << ")";
    #endif
    #ifdef CORRADE_TARGET_ANDROID
    Debug{} << "    CORRADE_TARGET_ANDROID";
    #endif
    #ifdef CORRADE_TARGET_X86
    Debug{} << "    CORRADE_TARGET_X86";
    #endif
    #ifdef CORRADE_TARGET_ARM
    Debug{} << "    CORRADE_TARGET_ARM";
    #endif
    #ifdef CORRADE_TARGET_POWERPC
    Debug{} << "    CORRADE_TARGET_POWERPC";
    #endif
    #ifdef CORRADE_TARGET_BIG_ENDIAN
    Debug{} << "    CORRADE_TARGET_BIG_ENDIAN";
    #endif
    #ifdef CORRADE_TARGET_GCC
    Debug{} << "    CORRADE_TARGET_GCC";
    #endif
    #ifdef CORRADE_TARGET_CLANG
    Debug{} << "    CORRADE_TARGET_CLANG";
    #endif
    #ifdef CORRADE_TARGET_APPLE_CLANG
    Debug{} << "    CORRADE_TARGET_APPLE_CLANG";
    #endif
    #ifdef CORRADE_TARGET_CLANG_CL
    Debug{} << "    CORRADE_TARGET_CLANG_CL";
    #endif
    #ifdef CORRADE_TARGET_MSVC
    Debug{} << "    CORRADE_TARGET_MSVC";
    #endif
    #ifdef CORRADE_TARGET_MINGW
    Debug{} << "    CORRADE_TARGET_MINGW";
    #endif
    #ifdef CORRADE_TARGET_LIBCXX
    Debug{} << "    CORRADE_TARGET_LIBCXX";
    #endif
    #ifdef CORRADE_TARGET_LIBSTDCXX
    Debug{} << "    CORRADE_TARGET_LIBSTDCXX";
    #endif
    #ifdef CORRADE_TARGET_DINKUMWARE
    Debug{} << "    CORRADE_TARGET_DINKUMWARE";
    #endif
    #ifdef CORRADE_TARGET_SSE2
    Debug{} << "    CORRADE_TARGET_SSE2";
    #endif
    #ifdef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
    Debug{} << "    CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT";
    #endif
    #ifdef CORRADE_TESTSUITE_TARGET_XCTEST
    Debug{} << "    CORRADE_TESTSUITE_TARGET_XCTEST";
    #endif
    #ifdef CORRADE_UTILITY_USE_ANSI_COLORS
    Debug{} << "    CORRADE_UTILITY_USE_ANSI_COLORS";
    #endif
    #ifdef MAGNUM_BUILD_DEPRECATED
    Debug{} << "    MAGNUM_BUILD_DEPRECATED";
    #endif
    #ifdef MAGNUM_BUILD_STATIC
    Debug{} << "    MAGNUM_BUILD_STATIC";
    #endif
    #ifdef MAGNUM_TARGET_GLES
    Debug{} << "    MAGNUM_TARGET_GLES";
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    Debug{} << "    MAGNUM_TARGET_GLES2";
    #endif
    #ifdef MAGNUM_TARGET_DESKTOP_GLES
    Debug{} << "    MAGNUM_TARGET_DESKTOP_GLES";
    #endif
    #ifdef MAGNUM_TARGET_WEBGL
    Debug{} << "    MAGNUM_TARGET_WEBGL";
    #endif
    #ifdef MAGNUM_TARGET_HEADLESS
    Debug{} << "    MAGNUM_TARGET_HEADLESS";
    #endif
    Debug{} << "";

    /* Create context here, so the context creation info is displayed at proper
       place */
    createContext();
    GL::Context& c = GL::Context::current();

    Debug{} << "";

    #ifndef MAGNUM_TARGET_GLES
    Debug{} << "Core profile:" << (c.isCoreProfile() ? "yes" : "no");
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    Debug{} << "Context flags:" << c.flags();
    #endif
    Debug{} << "Detected driver:" << c.detectedDriver();

    Debug{} << "Supported GLSL versions:";
    Debug{} << "   " << Utility::String::joinWithoutEmptyParts(c.shadingLanguageVersionStrings(), ", ");

    if(args.isSet("extension-strings")) {
        Debug{} << "Extension strings:" << Debug::newline
            << c.extensionStrings();
        return;
    }

    if(args.isSet("short")) return;

    Debug{} << "";

    /* Get first future (not supported) version */
    std::vector<GL::Version> versions{
        #ifndef MAGNUM_TARGET_GLES
        GL::Version::GL300,
        GL::Version::GL310,
        GL::Version::GL320,
        GL::Version::GL330,
        GL::Version::GL400,
        GL::Version::GL410,
        GL::Version::GL420,
        GL::Version::GL430,
        GL::Version::GL440,
        GL::Version::GL450,
        GL::Version::GL460,
        #else
        GL::Version::GLES300,
        #ifndef MAGNUM_TARGET_WEBGL
        GL::Version::GLES310,
        GL::Version::GLES320,
        #endif
        #endif
        GL::Version::None
    };
    std::size_t future = 0;

    if(!args.isSet("all-extensions"))
        while(versions[future] != GL::Version::None && c.isVersionSupported(versions[future]))
            ++future;

    /* Display supported OpenGL extensions from unsupported versions */
    for(std::size_t i = future; i != versions.size(); ++i) {
        if(versions[i] != GL::Version::None)
            Debug{} << versions[i] << "extension support:";
        else Debug{} << "Vendor extension support:";

        for(const auto& extension: GL::Extension::extensions(versions[i])) {
            std::string extensionName = extension.string();
            Debug d;
            d << "   " << extensionName << std::string(60-extensionName.size(), ' ');
            if(c.isExtensionSupported(extension))
                d << "SUPPORTED";
            else if(c.isExtensionDisabled(extension))
                d << " removed";
            else if(c.isVersionSupported(extension.requiredVersion()))
                d << "    -";
            else
                d << "   n/a";
        }

        Debug{} << "";
    }

    if(!args.isSet("limits")) return;

    /* Limits and implementation-defined values */
    #define _h(val) Debug{} << "\n " << GL::Extensions::val::string() << Debug::nospace << ":";
    #define _l(val) Debug{} << "   " << #val << (sizeof(#val) > 64 ? "\n" + std::string(68, ' ') : std::string(64 - sizeof(#val), ' ')) << val;
    #define _lvec(val) Debug{} << "   " << #val << (sizeof(#val) > 42 ? "\n" + std::string(46, ' ') : std::string(42 - sizeof(#val), ' ')) << val;

    Debug{} << "Limits and implementation-defined values:";
    _lvec(GL::AbstractFramebuffer::maxViewportSize())
    _l(GL::AbstractFramebuffer::maxDrawBuffers())
    _l(GL::Framebuffer::maxColorAttachments())
    _l(GL::Mesh::maxVertexAttributeStride())
    #ifndef MAGNUM_TARGET_GLES2
    _l(GL::Mesh::maxElementIndex())
    _l(GL::Mesh::maxElementsIndices())
    _l(GL::Mesh::maxElementsVertices())
    #endif
    _lvec(GL::Renderer::lineWidthRange())
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    _l(GL::Renderer::maxClipDistances())
    #endif
    _l(GL::Renderbuffer::maxSize())
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    _l(GL::Renderbuffer::maxSamples())
    #endif
    _l(GL::Shader::maxVertexOutputComponents())
    _l(GL::Shader::maxFragmentInputComponents())
    _l(GL::Shader::maxTextureImageUnits(GL::Shader::Type::Vertex))
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    _l(GL::Shader::maxTextureImageUnits(GL::Shader::Type::TessellationControl))
    _l(GL::Shader::maxTextureImageUnits(GL::Shader::Type::TessellationEvaluation))
    _l(GL::Shader::maxTextureImageUnits(GL::Shader::Type::Geometry))
    _l(GL::Shader::maxTextureImageUnits(GL::Shader::Type::Compute))
    #endif
    _l(GL::Shader::maxTextureImageUnits(GL::Shader::Type::Fragment))
    _l(GL::Shader::maxCombinedTextureImageUnits())
    _l(GL::Shader::maxUniformComponents(GL::Shader::Type::Vertex))
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    _l(GL::Shader::maxUniformComponents(GL::Shader::Type::TessellationControl))
    _l(GL::Shader::maxUniformComponents(GL::Shader::Type::TessellationEvaluation))
    _l(GL::Shader::maxUniformComponents(GL::Shader::Type::Geometry))
    _l(GL::Shader::maxUniformComponents(GL::Shader::Type::Compute))
    #endif
    _l(GL::Shader::maxUniformComponents(GL::Shader::Type::Fragment))
    _l(GL::AbstractShaderProgram::maxVertexAttributes())
    #ifndef MAGNUM_TARGET_GLES2
    _l(GL::AbstractTexture::maxLodBias())
    #endif
    #ifndef MAGNUM_TARGET_GLES
    _lvec(GL::Texture1D::maxSize())
    #endif
    _lvec(GL::Texture2D::maxSize())
    #ifndef MAGNUM_TARGET_GLES2
    _lvec(GL::Texture3D::maxSize()) /* Checked ES2 version below */
    #endif
    _lvec(GL::CubeMapTexture::maxSize())

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::blend_func_extended>()) {
        _h(ARB::blend_func_extended)

        _l(GL::AbstractFramebuffer::maxDualSourceDrawBuffers())
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::cull_distance>())
    #else
    if(c.isExtensionSupported<GL::Extensions::EXT::clip_cull_distance>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::cull_distance)
        #else
        _h(EXT::clip_cull_distance)
        #endif

        _l(GL::Renderer::maxCullDistances())
        _l(GL::Renderer::maxCombinedClipAndCullDistances())
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::compute_shader>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::compute_shader)
        #endif

        _l(GL::AbstractShaderProgram::maxComputeSharedMemorySize())
        _l(GL::AbstractShaderProgram::maxComputeWorkGroupInvocations())
        _lvec(GL::AbstractShaderProgram::maxComputeWorkGroupCount())
        _lvec(GL::AbstractShaderProgram::maxComputeWorkGroupSize())
    }

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::explicit_uniform_location>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::explicit_uniform_location)
        #endif

        _l(GL::AbstractShaderProgram::maxUniformLocations())
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::map_buffer_alignment>()) {
        _h(ARB::map_buffer_alignment)

        _l(GL::Buffer::minMapAlignment())
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::shader_atomic_counters>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::shader_atomic_counters)
        #endif

        _l(GL::Buffer::maxAtomicCounterBindings())
        _l(GL::Shader::maxAtomicCounterBuffers(GL::Shader::Type::Vertex))
        _l(GL::Shader::maxAtomicCounterBuffers(GL::Shader::Type::TessellationControl))
        _l(GL::Shader::maxAtomicCounterBuffers(GL::Shader::Type::TessellationEvaluation))
        _l(GL::Shader::maxAtomicCounterBuffers(GL::Shader::Type::Geometry))
        _l(GL::Shader::maxAtomicCounterBuffers(GL::Shader::Type::Compute))
        _l(GL::Shader::maxAtomicCounterBuffers(GL::Shader::Type::Fragment))
        _l(GL::Shader::maxCombinedAtomicCounterBuffers())
        _l(GL::Shader::maxAtomicCounters(GL::Shader::Type::Vertex))
        _l(GL::Shader::maxAtomicCounters(GL::Shader::Type::TessellationControl))
        _l(GL::Shader::maxAtomicCounters(GL::Shader::Type::TessellationEvaluation))
        _l(GL::Shader::maxAtomicCounters(GL::Shader::Type::Geometry))
        _l(GL::Shader::maxAtomicCounters(GL::Shader::Type::Compute))
        _l(GL::Shader::maxAtomicCounters(GL::Shader::Type::Fragment))
        _l(GL::Shader::maxCombinedAtomicCounters())
        _l(GL::AbstractShaderProgram::maxAtomicCounterBufferSize())
    }

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::shader_image_load_store>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::shader_image_load_store)
        #endif

        _l(GL::Shader::maxImageUniforms(GL::Shader::Type::Vertex))
        _l(GL::Shader::maxImageUniforms(GL::Shader::Type::TessellationControl))
        _l(GL::Shader::maxImageUniforms(GL::Shader::Type::TessellationEvaluation))
        _l(GL::Shader::maxImageUniforms(GL::Shader::Type::Geometry))
        _l(GL::Shader::maxImageUniforms(GL::Shader::Type::Compute))
        _l(GL::Shader::maxImageUniforms(GL::Shader::Type::Fragment))
        _l(GL::Shader::maxCombinedImageUniforms())
        _l(GL::AbstractShaderProgram::maxCombinedShaderOutputResources())
        _l(GL::AbstractShaderProgram::maxImageUnits())
        #ifndef MAGNUM_TARGET_GLES
        _l(GL::AbstractShaderProgram::maxImageSamples())
        #endif
    }

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::shader_storage_buffer_object>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::shader_storage_buffer_object)
        #endif

        _l(GL::Buffer::shaderStorageOffsetAlignment())
        _l(GL::Buffer::maxShaderStorageBindings())
        _l(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Vertex))
        _l(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::TessellationControl))
        _l(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::TessellationEvaluation))
        _l(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Geometry))
        _l(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Compute))
        _l(GL::Shader::maxShaderStorageBlocks(GL::Shader::Type::Fragment))
        _l(GL::Shader::maxCombinedShaderStorageBlocks())
        /* AbstractShaderProgram::maxCombinedShaderOutputResources() already in shader_image_load_store */
        _l(GL::AbstractShaderProgram::maxShaderStorageBlockSize())
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::texture_multisample>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::texture_multisample)
        #endif

        _l(GL::AbstractTexture::maxColorSamples())
        _l(GL::AbstractTexture::maxDepthSamples())
        _l(GL::AbstractTexture::maxIntegerSamples())
        _lvec(GL::MultisampleTexture2D::maxSize())
        _lvec(GL::MultisampleTexture2DArray::maxSize())
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::texture_rectangle>()) {
        _h(ARB::texture_rectangle)

        _lvec(GL::RectangleTexture::maxSize())
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::uniform_buffer_object>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::uniform_buffer_object)
        #endif

        _l(GL::Buffer::uniformOffsetAlignment())
        _l(GL::Buffer::maxUniformBindings())
        _l(GL::Shader::maxUniformBlocks(GL::Shader::Type::Vertex))
        #ifndef MAGNUM_TARGET_WEBGL
        _l(GL::Shader::maxUniformBlocks(GL::Shader::Type::TessellationControl))
        _l(GL::Shader::maxUniformBlocks(GL::Shader::Type::TessellationEvaluation))
        _l(GL::Shader::maxUniformBlocks(GL::Shader::Type::Geometry))
        _l(GL::Shader::maxUniformBlocks(GL::Shader::Type::Compute))
        #endif
        _l(GL::Shader::maxUniformBlocks(GL::Shader::Type::Fragment))
        _l(GL::Shader::maxCombinedUniformBlocks())
        _l(GL::Shader::maxCombinedUniformComponents(GL::Shader::Type::Vertex))
        #ifndef MAGNUM_TARGET_WEBGL
        _l(GL::Shader::maxCombinedUniformComponents(GL::Shader::Type::TessellationControl))
        _l(GL::Shader::maxCombinedUniformComponents(GL::Shader::Type::TessellationEvaluation))
        _l(GL::Shader::maxCombinedUniformComponents(GL::Shader::Type::Geometry))
        _l(GL::Shader::maxCombinedUniformComponents(GL::Shader::Type::Compute))
        #endif
        _l(GL::Shader::maxCombinedUniformComponents(GL::Shader::Type::Fragment))
        _l(GL::AbstractShaderProgram::maxUniformBlockSize())
    }

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::EXT::gpu_shader4>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(EXT::gpu_shader4)
        #endif

        _l(GL::AbstractShaderProgram::minTexelOffset())
        _l(GL::AbstractShaderProgram::maxTexelOffset())
    }

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::EXT::texture_array>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(EXT::texture_array)
        #endif

        #ifndef MAGNUM_TARGET_GLES
        _lvec(GL::Texture1DArray::maxSize())
        #endif
        _lvec(GL::Texture2DArray::maxSize())
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::EXT::transform_feedback>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(EXT::transform_feedback)
        #endif

        _l(GL::TransformFeedback::maxInterleavedComponents())
        _l(GL::TransformFeedback::maxSeparateAttributes())
        _l(GL::TransformFeedback::maxSeparateComponents())
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::transform_feedback3>()) {
        _h(ARB::transform_feedback3)

        _l(GL::TransformFeedback::maxBuffers())
        _l(GL::TransformFeedback::maxVertexStreams())
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::geometry_shader4>())
    #else
    if(c.isExtensionSupported<GL::Extensions::EXT::geometry_shader>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::geometry_shader4)
        #else
        _h(EXT::geometry_shader)
        #endif

        _l(GL::AbstractShaderProgram::maxGeometryOutputVertices())
        _l(GL::Shader::maxGeometryInputComponents())
        _l(GL::Shader::maxGeometryOutputComponents())
        _l(GL::Shader::maxGeometryTotalOutputComponents())
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::tessellation_shader>())
    #else
    if(c.isExtensionSupported<GL::Extensions::EXT::tessellation_shader>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::tessellation_shader)
        #else
        _h(EXT::tessellation_shader)
        #endif

        _l(GL::Shader::maxTessellationControlInputComponents())
        _l(GL::Shader::maxTessellationControlOutputComponents())
        _l(GL::Shader::maxTessellationControlTotalOutputComponents())
        _l(GL::Shader::maxTessellationEvaluationInputComponents())
        _l(GL::Shader::maxTessellationEvaluationOutputComponents())
        _l(GL::Renderer::maxPatchVertexCount())
    }
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::texture_buffer_object>())
    #else
    if(c.isExtensionSupported<GL::Extensions::EXT::texture_buffer>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::texture_buffer_object)
        #else
        _h(EXT::texture_buffer)
        #endif

        _l(GL::BufferTexture::maxSize())
    }

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::texture_buffer_range>())
    #else
    if(c.isExtensionSupported<GL::Extensions::EXT::texture_buffer>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::texture_buffer_range)
        #else
        /* Header added above */
        #endif

        _l(GL::BufferTexture::offsetAlignment())
    }

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::texture_cube_map_array>())
    #else
    if(c.isExtensionSupported<GL::Extensions::EXT::texture_cube_map_array>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::texture_cube_map_array)
        #else
        _h(EXT::texture_cube_map_array)
        #endif

        _lvec(GL::CubeMapTextureArray::maxSize())
    }
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(c.isExtensionSupported<GL::Extensions::ARB::texture_filter_anisotropic>()) {
        _h(ARB::texture_filter_anisotropic)

        _l(GL::Sampler::maxMaxAnisotropy())
    } else
    #endif
    if(c.isExtensionSupported<GL::Extensions::EXT::texture_filter_anisotropic>()) {
        _h(EXT::texture_filter_anisotropic)

        _l(GL::Sampler::maxMaxAnisotropy())
    }

    #ifndef MAGNUM_TARGET_WEBGL
    if(c.isExtensionSupported<GL::Extensions::KHR::debug>()) {
        _h(KHR::debug)

        _l(GL::AbstractObject::maxLabelLength())
        _l(GL::DebugOutput::maxLoggedMessages())
        _l(GL::DebugOutput::maxMessageLength())
        _l(GL::DebugGroup::maxStackDepth())
    }
    #endif

    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    if(c.isExtensionSupported<GL::Extensions::OES::texture_3D>()) {
        _h(OES::texture_3D)

        _lvec(GL::Texture3D::maxSize())
    }
    #endif

    #undef _l
    #undef _h
}

}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(Magnum::MagnumInfo)
