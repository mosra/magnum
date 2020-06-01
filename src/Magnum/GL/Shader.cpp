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

#include "Shader.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/ShaderState.h"
#include "Magnum/Math/Functions.h"

/* libgles-omap3-dev_4.03.00.02-r15.6 on BeagleBoard/Ångström linux 2011.3 doesn't have GLchar */
#ifdef MAGNUM_TARGET_GLES
typedef char GLchar;
#endif

namespace Magnum { namespace GL {

namespace Implementation {
    /* defined in Implementation/driverSpecific.cpp */
    bool isShaderCompilationLogEmpty(const std::string& result);
}

namespace {

std::string shaderName(const Shader::Type type) {
    switch(type) {
        case Shader::Type::Vertex:                  return "vertex";
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        case Shader::Type::Geometry:                return "geometry";
        case Shader::Type::TessellationControl:     return "tessellation control";
        case Shader::Type::TessellationEvaluation:  return "tessellation evaluation";
        case Shader::Type::Compute:                 return "compute";
        #endif
        case Shader::Type::Fragment:                return "fragment";
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

UnsignedInt typeToIndex(const Shader::Type type) {
    switch(type) {
        case Shader::Type::Vertex:                  return 0;
        case Shader::Type::Fragment:                return 1;
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        case Shader::Type::Compute:                 return 2;
        case Shader::Type::Geometry:                return 3;
        case Shader::Type::TessellationControl:     return 4;
        case Shader::Type::TessellationEvaluation:  return 5;
        #endif
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

#ifndef MAGNUM_TARGET_GLES
bool isTypeSupported(const Shader::Type type) {
    if(type == Shader::Type::Geometry && !Context::current().isExtensionSupported<Extensions::ARB::geometry_shader4>())
        return false;

    if((type == Shader::Type::TessellationControl || type == Shader::Type::TessellationEvaluation) && !Context::current().isExtensionSupported<Extensions::ARB::tessellation_shader>())
        return false;

    if(type == Shader::Type::Compute && !Context::current().isExtensionSupported<Extensions::ARB::compute_shader>())
        return false;

    return true;
}
#elif !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
bool isTypeSupported(const Shader::Type type) {
    if(type == Shader::Type::Geometry && !Context::current().isExtensionSupported<Extensions::EXT::geometry_shader>())
        return false;

    if((type == Shader::Type::TessellationControl || type == Shader::Type::TessellationEvaluation) && !Context::current().isExtensionSupported<Extensions::EXT::tessellation_shader>())
        return false;

    if(type == Shader::Type::Compute && !Context::current().isVersionSupported(Version::GLES310))
        return false;

    return true;
}
#else
constexpr bool isTypeSupported(Shader::Type) { return true; }
#endif

}

Int Shader::maxVertexOutputComponents() {
    GLint& value = Context::current().state().shader->maxVertexOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES
        if(Context::current().isVersionSupported(Version::GL320))
            glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &value);
        else
            glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &value);
        #elif defined(MAGNUM_TARGET_GLES2)
        glGetIntegerv(GL_MAX_VARYING_VECTORS, &value);
        value *= 4;
        #else
        glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &value);
        #endif
    }

    return value;
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Int Shader::maxTessellationControlInputComponents() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::tessellation_shader>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::tessellation_shader>())
        return 0;
    #endif

    GLint& value = Context::current().state().shader->maxTessellationControlInputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxTessellationControlOutputComponents() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::tessellation_shader>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::tessellation_shader>())
        return 0;
    #endif

    GLint& value = Context::current().state().shader->maxTessellationControlOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxTessellationControlTotalOutputComponents() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::tessellation_shader>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::tessellation_shader>())
        return 0;
    #endif

    GLint& value = Context::current().state().shader->maxTessellationControlTotalOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxTessellationEvaluationInputComponents() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::tessellation_shader>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::tessellation_shader>())
        return 0;
    #endif

    GLint& value = Context::current().state().shader->maxTessellationEvaluationInputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxTessellationEvaluationOutputComponents() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::tessellation_shader>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::tessellation_shader>())
        return 0;
    #endif

    GLint& value = Context::current().state().shader->maxTessellationEvaluationOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxGeometryInputComponents() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::geometry_shader4>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::geometry_shader>())
        return 0;
    #endif

    GLint& value = Context::current().state().shader->maxGeometryInputComponents;

    /* Get the value, if not already cached */
    /** @todo The extension has only `GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB`, this is supported since GL 3.2 (wtf?) */
    if(value == 0)
        glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxGeometryOutputComponents() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::geometry_shader4>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::geometry_shader>())
        return 0;
    #endif

    GLint& value = Context::current().state().shader->maxGeometryOutputComponents;

    /* Get the value, if not already cached */
    /** @todo The extension has only `GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB`, this is supported since GL 3.2 (wtf?) */
    if(value == 0)
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxGeometryTotalOutputComponents() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::geometry_shader4>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::geometry_shader>())
        return 0;
    #endif

    GLint& value = Context::current().state().shader->maxGeometryTotalOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &value);

    return value;
}
#endif

Int Shader::maxFragmentInputComponents() {
    GLint& value = Context::current().state().shader->maxFragmentInputComponents;

    /* Get the value, if not already cached */
    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES
        if(Context::current().isVersionSupported(Version::GL320))
            glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &value);
        else
            glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &value);
        #elif defined(MAGNUM_TARGET_GLES2)
        glGetIntegerv(GL_MAX_VARYING_VECTORS, &value);
        value *= 4;
        #else
        glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &value);
        #endif
    }

    return value;
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Int Shader::maxAtomicCounterBuffers(const Type type) {
    if(
        #ifndef MAGNUM_TARGET_GLES
        !Context::current().isExtensionSupported<Extensions::ARB::shader_atomic_counters>() ||
        #else
        !Context::current().isVersionSupported(Version::GLES310) ||
        #endif
        !isTypeSupported(type))
    {
        return 0;
    }

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current().state().shader->maxAtomicCounterBuffers[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS,
        GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS,
        GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS,
        GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS,
        GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS,
        GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedAtomicCounterBuffers() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_atomic_counters>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().shader->maxCombinedAtomicCounterBuffers;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS, &value);

    return value;
}

Int Shader::maxAtomicCounters(const Type type) {
    if(
        #ifndef MAGNUM_TARGET_GLES
        !Context::current().isExtensionSupported<Extensions::ARB::shader_atomic_counters>() ||
        #else
        !Context::current().isVersionSupported(Version::GLES310) ||
        #endif
        !isTypeSupported(type))
    {
        return 0;
    }

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current().state().shader->maxAtomicCounters[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_ATOMIC_COUNTERS,
        GL_MAX_FRAGMENT_ATOMIC_COUNTERS,
        GL_MAX_COMPUTE_ATOMIC_COUNTERS,
        GL_MAX_GEOMETRY_ATOMIC_COUNTERS,
        GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS,
        GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedAtomicCounters() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_atomic_counters>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().shader->maxCombinedAtomicCounters;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, &value);

    return value;
}

Int Shader::maxImageUniforms(const Type type) {
    if(
        #ifndef MAGNUM_TARGET_GLES
        !Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>() ||
        #else
        !Context::current().isVersionSupported(Version::GLES310) ||
        #endif
        !isTypeSupported(type))
    {
        return 0;
    }

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current().state().shader->maxImageUniforms[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_IMAGE_UNIFORMS,
        GL_MAX_FRAGMENT_IMAGE_UNIFORMS,
        GL_MAX_COMPUTE_IMAGE_UNIFORMS,
        GL_MAX_GEOMETRY_IMAGE_UNIFORMS,
        GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS,
        GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedImageUniforms() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().shader->maxCombinedImageUniforms;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_IMAGE_UNIFORMS, &value);

    return value;
}

Int Shader::maxShaderStorageBlocks(const Type type) {
    if(
        #ifndef MAGNUM_TARGET_GLES
        !Context::current().isExtensionSupported<Extensions::ARB::shader_storage_buffer_object>() ||
        #else
        !Context::current().isVersionSupported(Version::GLES310) ||
        #endif
        !isTypeSupported(type))
    {
        return 0;
    }

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current().state().shader->maxShaderStorageBlocks[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,
        GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,
        GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,
        GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,
        GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,
        GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedShaderStorageBlocks() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_atomic_counters>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().shader->maxCombinedShaderStorageBlocks;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &value);

    return value;
}
#endif

Int Shader::maxTextureImageUnits(const Type type) {
    if(!isTypeSupported(type))
        return 0;

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current().state().shader->maxTextureImageUnits[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
        GL_MAX_TEXTURE_IMAGE_UNITS,
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS,
        GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,
        GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS,
        GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS
        #endif
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedTextureImageUnits() {
    GLint& value = Context::current().state().shader->maxTextureImageUnitsCombined;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value);

    return value;
}

#ifndef MAGNUM_TARGET_GLES2
Int Shader::maxUniformBlocks(const Type type) {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>() || !isTypeSupported(type))
    #else
    if(!isTypeSupported(type))
    #endif
        return 0;

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current().state().shader->maxUniformBlocks[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_UNIFORM_BLOCKS,
        GL_MAX_FRAGMENT_UNIFORM_BLOCKS,
        #ifndef MAGNUM_TARGET_WEBGL
        GL_MAX_COMPUTE_UNIFORM_BLOCKS,
        GL_MAX_GEOMETRY_UNIFORM_BLOCKS,
        GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS,
        GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS
        #endif
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedUniformBlocks() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>())
        return 0;
    #endif

    GLint& value = Context::current().state().shader->maxCombinedUniformBlocks;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &value);

    return value;
}
#endif

Int Shader::maxUniformComponents(const Type type) {
    if(!isTypeSupported(type))
        return 0;

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current().state().shader->maxUniformComponents[index];

    /* Get the value, if not already cached */
    #ifndef MAGNUM_TARGET_GLES2
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_UNIFORM_COMPONENTS,
        GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
        #ifndef MAGNUM_TARGET_WEBGL
        GL_MAX_COMPUTE_UNIFORM_COMPONENTS,
        GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,
        GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS,
        GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS
        #endif
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);
    #else
    /* For ES2 we need to multiply _VECTORS by 4 */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_UNIFORM_VECTORS,
        GL_MAX_FRAGMENT_UNIFORM_VECTORS
    };
    if(value == 0) {
        GLint vectors;
        glGetIntegerv(what[index], &vectors);
        value = vectors*4;
    }
    #endif

    return value;
}

#ifndef MAGNUM_TARGET_GLES2
Int Shader::maxCombinedUniformComponents(const Type type) {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>() || !isTypeSupported(type))
    #else
    if(!isTypeSupported(type))
    #endif
        return 0;

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current().state().shader->maxCombinedUniformComponents[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
        GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
        #ifndef MAGNUM_TARGET_WEBGL
        GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS,
        GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS,
        GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS,
        GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS
        #endif
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}
#endif

Shader::Shader(const Version version, const Type type): _type(type), _id(0) {
    _id = glCreateShader(GLenum(_type));

    switch(version) {
        #ifndef MAGNUM_TARGET_GLES
        case Version::GL210: _sources.emplace_back("#version 120\n"); return;
        case Version::GL300: _sources.emplace_back("#version 130\n"); return;
        case Version::GL310: _sources.emplace_back("#version 140\n"); return;
        case Version::GL320: _sources.emplace_back("#version 150\n"); return;
        case Version::GL330: _sources.emplace_back("#version 330\n"); return;
        case Version::GL400: _sources.emplace_back("#version 400\n"); return;
        case Version::GL410: _sources.emplace_back("#version 410\n"); return;
        case Version::GL420: _sources.emplace_back("#version 420\n"); return;
        case Version::GL430: _sources.emplace_back("#version 430\n"); return;
        case Version::GL440: _sources.emplace_back("#version 440\n"); return;
        case Version::GL450: _sources.emplace_back("#version 450\n"); return;
        case Version::GL460: _sources.emplace_back("#version 460\n"); return;
        #endif
        /* `#version 100` really is GLSL ES 1.00 and *not* GLSL 1.00. What a mess. */
        case Version::GLES200: _sources.emplace_back("#version 100\n"); return;
        case Version::GLES300: _sources.emplace_back("#version 300 es\n"); return;
        #ifndef MAGNUM_TARGET_WEBGL
        case Version::GLES310: _sources.emplace_back("#version 310 es\n"); return;
        case Version::GLES320: _sources.emplace_back("#version 320 es\n"); return;
        #endif

        /* The user is responsible for (not) adding #version directive */
        case Version::None: return;
    }

    CORRADE_ASSERT_UNREACHABLE("GL::Shader::Shader(): unsupported version" << version, );
}

Shader::~Shader() {
    /* Moved out, nothing to do */
    if(!_id) return;

    glDeleteShader(_id);
}

#ifndef MAGNUM_TARGET_WEBGL
std::string Shader::label() const {
    #ifndef MAGNUM_TARGET_GLES2
    return Context::current().state().debug->getLabelImplementation(GL_SHADER, _id);
    #else
    return Context::current().state().debug->getLabelImplementation(GL_SHADER_KHR, _id);
    #endif
}

Shader& Shader::setLabelInternal(const Containers::ArrayView<const char> label) {
    #ifndef MAGNUM_TARGET_GLES2
    Context::current().state().debug->labelImplementation(GL_SHADER, _id, label);
    #else
    Context::current().state().debug->labelImplementation(GL_SHADER_KHR, _id, label);
    #endif
    return *this;
}
#endif

std::vector<std::string> Shader::sources() const { return _sources; }

Shader& Shader::addSource(std::string source) {
    if(!source.empty()) {
        auto addSource = Context::current().state().shader->addSourceImplementation;

        /* Fix line numbers, so line 41 of third added file is marked as 3(41)
           in case shader version was not Version::None, because then source 0
           is the #version directive added in constructor.

           If version was Version::None, line 41 of third added file is marked
           as 2(41). We apparently can't add even the #line directive before
           the potential `#version` directive -- in that case the first source
           file is not marked with any file number, thus having number 0. In
           order to avoid complex logic in compile() where we assert for at
           least some user-provided source, an empty string is added here
           instead. */
        if(!_sources.empty()) (this->*addSource)("#line 1 " + std::to_string((_sources.size()+1)/2) + '\n');
        else (this->*addSource)({});

        (this->*addSource)(std::move(source));
    }

    return *this;
}

void Shader::addSourceImplementationDefault(std::string source) {
    _sources.push_back(std::move(source));
}

#if defined(CORRADE_TARGET_EMSCRIPTEN) && defined(__EMSCRIPTEN_PTHREADS__)
void Shader::addSourceImplementationEmscriptenPthread(std::string source) {
    /* See the "emscripten-pthreads-broken-unicode-shader-sources"
       workaround description for details */
    for(char& c: source) if(c < 0) c = ' ';
    _sources.push_back(std::move(source));
}
#endif

Shader& Shader::addFile(const std::string& filename) {
    CORRADE_ASSERT(Utility::Directory::exists(filename),
        "GL::Shader file " << '\'' + filename + '\'' << " cannot be read.", *this);

    addSource(Utility::Directory::readString(filename));
    return *this;
}

bool Shader::compile() { return compile({*this}); }

bool Shader::compile(std::initializer_list<Containers::Reference<Shader>> shaders) {
    bool allSuccess = true;

    /* Allocate large enough array for source pointers and sizes (to avoid
       reallocating it for each of them) */
    std::size_t maxSourceCount = 0;
    for(Shader& shader: shaders) {
        CORRADE_ASSERT(shader._sources.size() > 1, "GL::Shader::compile(): no files added", false);
        maxSourceCount = Math::max(shader._sources.size(), maxSourceCount);
    }
    /** @todo ArrayTuple/VLAs */
    Containers::Array<const GLchar*> pointers(maxSourceCount);
    Containers::Array<GLint> sizes(maxSourceCount);

    /* Upload sources of all shaders */
    for(Shader& shader: shaders) {
        for(std::size_t i = 0; i != shader._sources.size(); ++i) {
            pointers[i] = static_cast<const GLchar*>(shader._sources[i].data());
            sizes[i] = shader._sources[i].size();
        }

        glShaderSource(shader._id, shader._sources.size(), pointers, sizes);
    }

    /* Invoke (possibly parallel) compilation on all shaders */
    for(Shader& shader: shaders) glCompileShader(shader._id);

    /* After compilation phase, check status of all shaders */
    Int i = 1;
    for(Shader& shader: shaders) {
        GLint success, logLength;
        glGetShaderiv(shader._id, GL_COMPILE_STATUS, &success);
        glGetShaderiv(shader._id, GL_INFO_LOG_LENGTH, &logLength);

        /* Error or warning message. The string is returned null-terminated,
           scrap the \0 at the end afterwards */
        std::string message(logLength, '\0');
        if(message.size() > 1)
            glGetShaderInfoLog(shader._id, message.size(), nullptr, &message[0]);
        message.resize(Math::max(logLength, 1)-1);

        /* Show error log */
        if(!success) {
            Error out{Debug::Flag::NoNewlineAtTheEnd};
            out << "GL::Shader::compile(): compilation of" << shaderName(shader._type) << "shader";
            if(shaders.size() != 1) out << i;
            out << "failed with the following message:" << Debug::newline << message;

        /* Or just warnings, if any */
        } else if(!message.empty() && !Implementation::isShaderCompilationLogEmpty(message)) {
            Warning out{Debug::Flag::NoNewlineAtTheEnd};
            out << "GL::Shader::compile(): compilation of" << shaderName(shader._type) << "shader";
            if(shaders.size() != 1) out << i;
            out << "succeeded with the following message:" << Debug::newline << message;
        }

        /* Success of all depends on each of them */
        allSuccess = allSuccess && success;
        ++i;
    }

    return allSuccess;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const Shader::Type value) {
    debug << "GL::Shader::Type" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Shader::Type::value: return debug << "::" #value;
        _c(Vertex)
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        _c(TessellationControl)
        _c(TessellationEvaluation)
        _c(Geometry)
        _c(Compute)
        #endif
        _c(Fragment)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLenum(value)) << Debug::nospace << ")";
}
#endif

}}
