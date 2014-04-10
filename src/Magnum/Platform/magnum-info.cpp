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

#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/compatibility.h>
#ifdef CORRADE_TARGET_NACL
#include <Corrade/Utility/NaClStreamBuffer.h>
#endif

#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/BufferTexture.h"
#endif
#include "Magnum/Context.h"
#include "Magnum/DebugMessage.h"
#include "Magnum/Extensions.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/Shader.h"
#ifndef CORRADE_TARGET_NACL
#include "Magnum/Platform/WindowlessGlxApplication.h"
#else
#include "Magnum/Platform/WindowlessNaClApplication.h"
#endif

namespace Magnum {

class MagnumInfo: public Platform::WindowlessApplication {
    public:
        explicit MagnumInfo(const Arguments& arguments);

        int exec() override { return 0; }
};

MagnumInfo::MagnumInfo(const Arguments& arguments): Platform::WindowlessApplication(arguments, nullptr) {
    Utility::Arguments args;
    args.addBooleanOption("all-extensions")
        .setHelp("all-extensions", "show extensions also for fully supported versions")
        .addBooleanOption("limits")
        .setHelp("limits", "display also limits and implementation-defined values")
        .setHelp("Displays information about Magnum engine and OpenGL capabilities.");

    /**
     * @todo Make this work in NaCl, somehow the arguments aren't passed to
     *      constructor but to Init() or whatnot
     */
    #ifndef CORRADE_TARGET_NACL
    args.parse(arguments.argc, arguments.argv);
    #endif

    /* Pass debug output as messages to JavaScript */
    #ifdef CORRADE_TARGET_NACL
    Utility::NaClMessageStreamBuffer buffer(this);
    std::ostream out(&buffer);
    Debug::setOutput(&out);
    #endif

    Debug() << "";
    Debug() << "  +---------------------------------------------------------+";
    Debug() << "  | Information about Magnum engine and OpenGL capabilities |";
    Debug() << "  +---------------------------------------------------------+";
    Debug() << "";

    #ifdef CORRADE_TARGET_NACL
    Debug() << "Used application: Platform::WindowlessNaClApplication";
    #else
    Debug() << "Used application: Platform::WindowlessGlxApplication";
    #endif
    Debug() << "Compilation flags:";
    #ifdef CORRADE_GCC46_COMPATIBILITY
    Debug() << "    CORRADE_GCC46_COMPATIBILITY";
    #endif
    #ifdef CORRADE_GCC47_COMPATIBILITY
    Debug() << "    CORRADE_GCC47_COMPATIBILITY";
    #endif
    #ifdef CORRADE_BUILD_DEPRECATED
    Debug() << "    CORRADE_BUILD_DEPRECATED";
    #endif
    #ifdef CORRADE_BUILD_STATIC
    Debug() << "    CORRADE_BUILD_STATIC";
    #endif
    #ifdef CORRADE_TARGET_NACL
    Debug() << "    CORRADE_TARGET_NACL";
    #endif
    #ifdef CORRADE_TARGET_NACL_NEWLIB
    Debug() << "    CORRADE_TARGET_NACL_NEWLIB";
    #endif
    #ifdef CORRADE_TARGET_NACL_GLIBC
    Debug() << "    CORRADE_TARGET_NACL_GLIBC";
    #endif
    #ifdef CORRADE_TARGET_EMSCRIPTEN
    Debug() << "    CORRADE_TARGET_EMSCRIPTEN";
    #endif
    #ifdef MAGNUM_BUILD_DEPRECATED
    Debug() << "    MAGNUM_BUILD_DEPRECATED";
    #endif
    #ifdef MAGNUM_BUILD_STATIC
    Debug() << "    MAGNUM_BUILD_STATIC";
    #endif
    #ifdef MAGNUM_TARGET_GLES
    Debug() << "    MAGNUM_TARGET_GLES";
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    Debug() << "    MAGNUM_TARGET_GLES2";
    #endif
    #ifdef MAGNUM_TARGET_DESKTOP_GLES
    Debug() << "    MAGNUM_TARGET_DESKTOP_GLES";
    #endif
    #ifdef MAGNUM_TARGET_WEBGL
    Debug() << "    MAGNUM_TARGET_WEBGL";
    #endif
    Debug() << "";

    /* Create context here, so the context creation info is displayed at proper
       place */
    createContext();
    Context* c = Context::current();
    Debug() << "Vendor:" << c->vendorString();
    Debug() << "Renderer:" << c->rendererString();
    Debug() << "OpenGL version:" << c->version() << '(' + c->versionString() + ')';

    Debug() << "Context flags:";
    #ifndef MAGNUM_TARGET_GLES
    for(const auto flag: {Context::Flag::Debug, Context::Flag::RobustAccess})
    #else
    for(const auto flag: {Context::Flag::Debug})
    #endif
        if(c->flags() & flag) Debug() << "   " << flag;

    Debug() << "Supported GLSL versions:";
    const std::vector<std::string> shadingLanguageVersions = c->shadingLanguageVersionStrings();
    for(const auto& version: shadingLanguageVersions)
        Debug() << "   " << version;

    Debug() << "";

    /* Get first future (not supported) version */
    std::vector<Version> versions{
        #ifndef MAGNUM_TARGET_GLES
        Version::GL300,
        Version::GL310,
        Version::GL320,
        Version::GL330,
        Version::GL400,
        Version::GL410,
        Version::GL420,
        Version::GL430,
        Version::GL440,
        #else
        Version::GLES300,
        #endif
        Version::None
    };
    std::size_t future = 0;

    if(!args.isSet("all-extensions"))
        while(versions[future] != Version::None && c->isVersionSupported(versions[future]))
            ++future;

    /* Display supported OpenGL extensions from unsupported versions */
    for(std::size_t i = future; i != versions.size(); ++i) {
        if(versions[i] != Version::None)
            Debug() << versions[i] << "extension support:";
        else Debug() << "Vendor extension support:";

        for(const auto& extension: Extension::extensions(versions[i])) {
            std::string extensionName = extension.string();
            Debug d;
            d << "   " << extensionName << std::string(60-extensionName.size(), ' ');
            if(c->isExtensionSupported(extension))
                d << "SUPPORTED";
            else if(c->isExtensionDisabled(extension))
                d << " removed";
            else if(c->isVersionSupported(extension.requiredVersion()))
                d << "    -";
            else
                d << "   n/a";
        }

        Debug() << "";
    }

    if(!args.isSet("limits")) return;

    /* Limits and implementation-defined values */
    #define _h(val) Debug() << "\n " << Extensions::GL::val::string() + std::string(":");
    #define _l(val) Debug() << "   " << #val << (sizeof(#val) > 64 ? "\n" + std::string(68, ' ') : std::string(64 - sizeof(#val), ' ')) << val;

    Debug() << "Limits and implementation-defined values:";
    _l(AbstractFramebuffer::maxViewportSize())
    _l(AbstractFramebuffer::maxDrawBuffers())
    _l(Framebuffer::maxColorAttachments())
    #ifndef MAGNUM_TARGET_GLES2
    _l(Mesh::maxElementsIndices())
    _l(Mesh::maxElementsVertices())
    #endif
    _l(Renderbuffer::maxSize())
    _l(Renderbuffer::maxSamples())
    _l(Shader::maxVertexOutputComponents())
    _l(Shader::maxFragmentInputComponents())
    _l(Shader::maxTextureImageUnits(Shader::Type::Vertex))
    #ifndef MAGNUM_TARGET_GLES
    _l(Shader::maxTextureImageUnits(Shader::Type::TessellationControl))
    _l(Shader::maxTextureImageUnits(Shader::Type::TessellationEvaluation))
    _l(Shader::maxTextureImageUnits(Shader::Type::Geometry))
    _l(Shader::maxTextureImageUnits(Shader::Type::Compute))
    #endif
    _l(Shader::maxTextureImageUnits(Shader::Type::Fragment))
    _l(Shader::maxCombinedTextureImageUnits())
    _l(Shader::maxUniformComponents(Shader::Type::Vertex))
    #ifndef MAGNUM_TARGET_GLES
    _l(Shader::maxUniformComponents(Shader::Type::TessellationControl))
    _l(Shader::maxUniformComponents(Shader::Type::TessellationEvaluation))
    _l(Shader::maxUniformComponents(Shader::Type::Geometry))
    _l(Shader::maxUniformComponents(Shader::Type::Compute))
    #endif
    _l(Shader::maxUniformComponents(Shader::Type::Fragment))
    #ifndef MAGNUM_TARGET_GLES
    _l(AbstractShaderProgram::maxUniformLocations())
    #endif
    _l(AbstractShaderProgram::maxVertexAttributes())
    #ifndef MAGNUM_TARGET_GLES
    _l(AbstractTexture::maxColorSamples())
    _l(AbstractTexture::maxDepthSamples())
    _l(AbstractTexture::maxIntegerSamples())
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _l(AbstractTexture::maxLodBias())
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(c->isExtensionSupported<Extensions::GL::ARB::blend_func_extended>()) {
        _h(ARB::blend_func_extended)

        _l(AbstractFramebuffer::maxDualSourceDrawBuffers())
    }

    if(c->isExtensionSupported<Extensions::GL::ARB::compute_shader>()) {
        _h(ARB::compute_shader)

        _l(AbstractShaderProgram::maxComputeSharedMemorySize())
        _l(AbstractShaderProgram::maxComputeWorkGroupInvocations())
    }

    if(c->isExtensionSupported<Extensions::GL::ARB::geometry_shader4>()) {
        _h(ARB::geometry_shader4)

        _l(Shader::maxGeometryInputComponents())
        _l(Shader::maxGeometryOutputComponents())
        _l(Shader::maxGeometryTotalOutputComponents())
    }

    if(c->isExtensionSupported<Extensions::GL::ARB::shader_atomic_counters>()) {
        _h(ARB::shader_atomic_counters)

        _l(Buffer::maxAtomicCounterBindings())
        _l(Shader::maxAtomicCounterBuffers(Shader::Type::Vertex))
        _l(Shader::maxAtomicCounterBuffers(Shader::Type::TessellationControl))
        _l(Shader::maxAtomicCounterBuffers(Shader::Type::TessellationEvaluation))
        _l(Shader::maxAtomicCounterBuffers(Shader::Type::Geometry))
        _l(Shader::maxAtomicCounterBuffers(Shader::Type::Compute))
        _l(Shader::maxAtomicCounterBuffers(Shader::Type::Fragment))
        _l(Shader::maxCombinedAtomicCounterBuffers())
        _l(Shader::maxAtomicCounters(Shader::Type::Vertex))
        _l(Shader::maxAtomicCounters(Shader::Type::TessellationControl))
        _l(Shader::maxAtomicCounters(Shader::Type::TessellationEvaluation))
        _l(Shader::maxAtomicCounters(Shader::Type::Geometry))
        _l(Shader::maxAtomicCounters(Shader::Type::Compute))
        _l(Shader::maxAtomicCounters(Shader::Type::Fragment))
        _l(Shader::maxCombinedAtomicCounters())
        _l(AbstractShaderProgram::maxAtomicCounterBufferSize())
    }

    if(c->isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>()) {
        _h(ARB::shader_image_load_store)

        _l(Shader::maxImageUniforms(Shader::Type::Vertex))
        _l(Shader::maxImageUniforms(Shader::Type::TessellationControl))
        _l(Shader::maxImageUniforms(Shader::Type::TessellationEvaluation))
        _l(Shader::maxImageUniforms(Shader::Type::Geometry))
        _l(Shader::maxImageUniforms(Shader::Type::Compute))
        _l(Shader::maxImageUniforms(Shader::Type::Fragment))
        _l(Shader::maxCombinedImageUniforms())
        _l(AbstractShaderProgram::maxCombinedShaderOutputResources())
        _l(AbstractShaderProgram::maxImageUnits())
        _l(AbstractShaderProgram::maxImageSamples())
    }

    if(c->isExtensionSupported<Extensions::GL::ARB::shader_storage_buffer_object>()) {
        _h(ARB::shader_storage_buffer_object)

        _l(Shader::maxShaderStorageBlocks(Shader::Type::Vertex))
        _l(Shader::maxShaderStorageBlocks(Shader::Type::TessellationControl))
        _l(Shader::maxShaderStorageBlocks(Shader::Type::TessellationEvaluation))
        _l(Shader::maxShaderStorageBlocks(Shader::Type::Geometry))
        _l(Shader::maxShaderStorageBlocks(Shader::Type::Compute))
        _l(Shader::maxShaderStorageBlocks(Shader::Type::Fragment))
        _l(Shader::maxCombinedShaderStorageBlocks())
        /* AbstractShaderProgram::maxCombinedShaderOutputResources() already in shader_image_load_store */
        _l(AbstractShaderProgram::maxShaderStorageBlockSize())
    }

    if(c->isExtensionSupported<Extensions::GL::ARB::tessellation_shader>()) {
        _h(ARB::tessellation_shader)

        _l(Buffer::shaderStorageOffsetAlignment())
        _l(Buffer::maxShaderStorageBindings())
        _l(Shader::maxTessellationControlInputComponents())
        _l(Shader::maxTessellationControlOutputComponents())
        _l(Shader::maxTessellationControlTotalOutputComponents())
        _l(Shader::maxTessellationEvaluationInputComponents())
        _l(Shader::maxTessellationEvaluationOutputComponents())
    }

    if(c->isExtensionSupported<Extensions::GL::ARB::texture_buffer_range>()) {
        _h(ARB::texture_buffer_range)

        _l(BufferTexture::offsetAlignment())
    }
    #endif

    /** @todo Somehow sort the following into previous list for ES3 */

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    if(c->isExtensionSupported<Extensions::GL::ARB::uniform_buffer_object>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(ARB::uniform_buffer_object)
        #endif

        _l(Shader::maxUniformBlocks(Shader::Type::Vertex))
        #ifndef MAGNUM_TARGET_GLES
        _l(Shader::maxUniformBlocks(Shader::Type::TessellationControl))
        _l(Shader::maxUniformBlocks(Shader::Type::TessellationEvaluation))
        _l(Shader::maxUniformBlocks(Shader::Type::Geometry))
        _l(Shader::maxUniformBlocks(Shader::Type::Compute))
        #endif
        _l(Shader::maxUniformBlocks(Shader::Type::Fragment))
        _l(Shader::maxCombinedUniformBlocks())
        _l(Shader::maxCombinedUniformComponents(Shader::Type::Vertex))
        #ifndef MAGNUM_TARGET_GLES
        _l(Shader::maxCombinedUniformComponents(Shader::Type::TessellationControl))
        _l(Shader::maxCombinedUniformComponents(Shader::Type::TessellationEvaluation))
        _l(Shader::maxCombinedUniformComponents(Shader::Type::Geometry))
        _l(Shader::maxCombinedUniformComponents(Shader::Type::Compute))
        #endif
        _l(Shader::maxCombinedUniformComponents(Shader::Type::Fragment))
        _l(AbstractShaderProgram::maxUniformBlockSize())
    }

    #ifndef MAGNUM_TARGET_GLES
    if(c->isExtensionSupported<Extensions::GL::EXT::gpu_shader4>())
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        _h(EXT::gpu_shader4)
        #endif

        _l(Buffer::maxUniformBindings())
        _l(AbstractShaderProgram::minTexelOffset())
        _l(AbstractShaderProgram::maxTexelOffset())
    }
    #endif

    if(c->isExtensionSupported<Extensions::GL::EXT::texture_filter_anisotropic>()) {
        _h(EXT::texture_filter_anisotropic)

        _l(Sampler::maxMaxAnisotropy())
    }

    if(c->isExtensionSupported<Extensions::GL::KHR::debug>()) {
        _h(KHR::debug)

        _l(AbstractObject::maxLabelLength())
        _l(DebugMessage::maxLoggedMessages())
        _l(DebugMessage::maxMessageLength())
    }

    #undef _l
    #undef _h
}

}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(Magnum::MagnumInfo)
