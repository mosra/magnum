/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>

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
#include <Corrade/Containers/GrowableArray.h>
#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/Reference.h>
#endif
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>

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

using namespace Containers::Literals;

namespace {

Containers::StringView shaderName(const Shader::Type type) {
    switch(type) {
        case Shader::Type::Vertex:                  return "vertex"_s;
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        case Shader::Type::Geometry:                return "geometry"_s;
        case Shader::Type::TessellationControl:     return "tessellation control"_s;
        case Shader::Type::TessellationEvaluation:  return "tessellation evaluation"_s;
        case Shader::Type::Compute:                 return "compute"_s;
        #endif
        case Shader::Type::Fragment:                return "fragment"_s;
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
    GLint& value = Context::current().state().shader.maxVertexOutputComponents;

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

    GLint& value = Context::current().state().shader.maxTessellationControlInputComponents;

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

    GLint& value = Context::current().state().shader.maxTessellationControlOutputComponents;

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

    GLint& value = Context::current().state().shader.maxTessellationControlTotalOutputComponents;

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

    GLint& value = Context::current().state().shader.maxTessellationEvaluationInputComponents;

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

    GLint& value = Context::current().state().shader.maxTessellationEvaluationOutputComponents;

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

    GLint& value = Context::current().state().shader.maxGeometryInputComponents;

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

    GLint& value = Context::current().state().shader.maxGeometryOutputComponents;

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

    GLint& value = Context::current().state().shader.maxGeometryTotalOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &value);

    return value;
}
#endif

Int Shader::maxFragmentInputComponents() {
    GLint& value = Context::current().state().shader.maxFragmentInputComponents;

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
    GLint& value = Context::current().state().shader.maxAtomicCounterBuffers[index];

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

    GLint& value = Context::current().state().shader.maxCombinedAtomicCounterBuffers;

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
    GLint& value = Context::current().state().shader.maxAtomicCounters[index];

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

    GLint& value = Context::current().state().shader.maxCombinedAtomicCounters;

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
    GLint& value = Context::current().state().shader.maxImageUniforms[index];

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

    GLint& value = Context::current().state().shader.maxCombinedImageUniforms;

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
    GLint& value = Context::current().state().shader.maxShaderStorageBlocks[index];

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

    GLint& value = Context::current().state().shader.maxCombinedShaderStorageBlocks;

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
    GLint& value = Context::current().state().shader.maxTextureImageUnits[index];

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
    GLint& value = Context::current().state().shader.maxTextureImageUnitsCombined;

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
    GLint& value = Context::current().state().shader.maxUniformBlocks[index];

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

    GLint& value = Context::current().state().shader.maxCombinedUniformBlocks;

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
    GLint& value = Context::current().state().shader.maxUniformComponents[index];

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
    GLint& value = Context::current().state().shader.maxCombinedUniformComponents[index];

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

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_APPLE)
Containers::StringView Shader::workaroundDefinesImplementationNoOp(Version) {
    return {};
}

Containers::StringView Shader::workaroundDefinesImplementationAdrenoVersion(const Version version) {
    if(version == Version::GLES310)
        return "#if __VERSION__ < 310\n"
               "#define MAGNUM_GLSL_VERSION 310\n"
               "#endif\n";
    if(version == Version::GLES320)
        return "#if __VERSION__ < 320\n"
               "#define MAGNUM_GLSL_VERSION 320\n"
               "#endif\n";
    return {};
}
#endif

Shader::Shader(const Version version, const Type type): _type{type}, _flags{ObjectFlag::DeleteOnDestruction|ObjectFlag::Created} {
    _id = glCreateShader(GLenum(_type));

    /* Used by addSource() / addFile() to insert either `#line 0` (for GLSL <
       330, which interprets it as affecting this line) or `#line 1` (for GLSL
       >= 330 which interprets it as affecting next line). GLSL ES has it like
       GLSL >= 330 always. */
    #ifndef MAGNUM_TARGET_GLES
    if(version < Version::GL330)
        _offsetLineByOneOnOldGlsl = true;
    else
        _offsetLineByOneOnOldGlsl = false;
    #endif

    Containers::Optional<Containers::StringView> versionString;
    switch(version) {
        #ifndef MAGNUM_TARGET_GLES
        case Version::GL210: versionString = "#version 120\n"_s; break;
        case Version::GL300: versionString = "#version 130\n"_s; break;
        case Version::GL310: versionString = "#version 140\n"_s; break;
        case Version::GL320: versionString = "#version 150\n"_s; break;
        case Version::GL330: versionString = "#version 330\n"_s; break;
        case Version::GL400: versionString = "#version 400\n"_s; break;
        case Version::GL410: versionString = "#version 410\n"_s; break;
        case Version::GL420: versionString = "#version 420\n"_s; break;
        case Version::GL430: versionString = "#version 430\n"_s; break;
        case Version::GL440: versionString = "#version 440\n"_s; break;
        case Version::GL450: versionString = "#version 450\n"_s; break;
        case Version::GL460: versionString = "#version 460\n"_s; break;
        #endif
        /* `#version 100` really is GLSL ES 1.00 and *not* GLSL 1.00. What a mess. */
        case Version::GLES200: versionString = "#version 100\n"_s; break;
        case Version::GLES300: versionString = "#version 300 es\n"_s; break;
        #ifndef MAGNUM_TARGET_WEBGL
        case Version::GLES310: versionString = "#version 310 es\n"_s; break;
        case Version::GLES320: versionString = "#version 320 es\n"_s; break;
        #endif

        /* The user is responsible for (not) adding a #version directive, and
           also any workarounds that depend on it */
        case Version::None: versionString = ""_s; break;
    }

    CORRADE_ASSERT(versionString, "GL::Shader::Shader(): unsupported version" << version, );

    if(*versionString) {
        arrayAppend(_sources, Containers::String::nullTerminatedGlobalView(*versionString));

        #if !defined(MAGNUM_TARGET_GLES) || !defined(MAGNUM_TARGET_GLES2)
        Context& context = Context::current();
        #endif

        /* Supply macros for functionality advertised by the GLSL compiler that
           isn't actually working on given version. Search for these macros in
           Implementation/driverSpecific.cpp for more information. */
        #ifndef MAGNUM_TARGET_GLES
        if(context.isExtensionDisabled<Extensions::ARB::explicit_attrib_location>(version))
            arrayAppend(_sources, Containers::String::nullTerminatedGlobalView("#define MAGNUM_DISABLE_GL_ARB_explicit_attrib_location\n"_s));
        if(context.isExtensionDisabled<Extensions::ARB::shading_language_420pack>(version))
            arrayAppend(_sources, Containers::String::nullTerminatedGlobalView("#define MAGNUM_DISABLE_GL_ARB_shading_language_420pack\n"_s));
        if(context.isExtensionDisabled<Extensions::ARB::explicit_uniform_location>(version))
            arrayAppend(_sources, Containers::String::nullTerminatedGlobalView("#define MAGNUM_DISABLE_GL_ARB_explicit_uniform_location\n"_s));
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        if(type == Type::Vertex && context.isExtensionDisabled<Extensions::MAGNUM::shader_vertex_id>(version))
            arrayAppend(_sources, Containers::String::nullTerminatedGlobalView("#define MAGNUM_DISABLE_GL_MAGNUM_shader_vertex_id\n"_s));
        #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_APPLE)
        if(const Containers::StringView defines = context.state().shader.workaroundDefinesImplementation(version)) {
            arrayAppend(_sources, Containers::String::nullTerminatedGlobalView(defines));
        }
        #endif

        /* Remember how many initial sources were added to have consistent
           #line numbering later */
        _fileIndexOffset = _sources.size() - 1;
    } else {
        _fileIndexOffset = 0;
    }
}

Shader::Shader(const Type type, const GLuint id, ObjectFlags flags) noexcept: _type{type}, _id{id}, _flags{flags},
    #ifndef MAGNUM_TARGET_GLES
    _offsetLineByOneOnOldGlsl{},
    #endif
    _fileIndexOffset{}
    {}

Shader::Shader(NoCreateT) noexcept: _type{}, _id{0} {}

Shader::Shader(Shader&& other) noexcept: _type{other._type}, _id{other._id}, _flags{other._flags},
    #ifndef MAGNUM_TARGET_GLES
    _offsetLineByOneOnOldGlsl{other._flags},
    #endif
    _fileIndexOffset{other._fileIndexOffset},
    _sources{Utility::move(other._sources)}
{
    other._id = 0;
}

Shader::~Shader() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction)) return;

    glDeleteShader(_id);
}

Shader& Shader::operator=(Shader&& other) noexcept {
    using Utility::swap;
    swap(_type, other._type);
    swap(_id, other._id);
    swap(_flags, other._flags);
    #ifndef MAGNUM_TARGET_GLES
    swap(_offsetLineByOneOnOldGlsl, other._offsetLineByOneOnOldGlsl);
    #endif
    swap(_fileIndexOffset, other._fileIndexOffset);
    swap(_sources, other._sources);
    return *this;
}

#ifndef MAGNUM_TARGET_WEBGL
Containers::String Shader::label() const {
    #ifndef MAGNUM_TARGET_GLES2
    return Context::current().state().debug.getLabelImplementation(GL_SHADER, _id);
    #else
    return Context::current().state().debug.getLabelImplementation(GL_SHADER_KHR, _id);
    #endif
}

Shader& Shader::setLabel(const Containers::StringView label) {
    #ifndef MAGNUM_TARGET_GLES2
    Context::current().state().debug.labelImplementation(GL_SHADER, _id, label);
    #else
    Context::current().state().debug.labelImplementation(GL_SHADER_KHR, _id, label);
    #endif
    return *this;
}
#endif

Containers::StringIterable Shader::sources() const { return _sources; }

Shader& Shader::addSource(const Containers::StringView source) {
    return addSourceInternal(Containers::String::nullTerminatedGlobalView(source));
}

Shader& Shader::addSourceInternal(Containers::String&& source) {
    if(!source.isEmpty()) {
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
        if(!_sources.isEmpty()) arrayAppend(_sources,
            /* GLSL < 330 interprets #line 0 as the next line being line 1,
               while GLSL >= 330 which interprets #line 1 as next line being
               line 1; the latter is consistent with the C preprocessor. GLSL
               ES behaves like GLSL >= 330 always. */
            Utility::format(
                #ifndef MAGNUM_TARGET_GLES
                _offsetLineByOneOnOldGlsl ? "#line 0 {}\n" :
                #endif
                "#line 1 {}\n",
            (_sources.size() + 1 - _fileIndexOffset)/2));
        else arrayAppend(_sources, Containers::String::nullTerminatedGlobalView(""_s));

        Context::current().state().shader.addSourceImplementation(*this, Utility::move(source));
    }

    return *this;
}

void Shader::addSourceImplementationDefault(Shader& self, Containers::String&& source) {
    arrayAppend(self._sources, Utility::move(source));
}

#if defined(CORRADE_TARGET_EMSCRIPTEN) && defined(__EMSCRIPTEN_PTHREADS__)
void Shader::addSourceImplementationEmscriptenPthread(Shader& self, Containers::String&& source) {
    /* Modify the string to remove non-ASCII characters. Ensure we're only
       modifying a string we actually own, if not make a copy first. See the
       "emscripten-pthreads-broken-unicode-shader-sources" workaround
       description for details. */
    if(!source.isSmall() && !source.deleter())
        source = Containers::String{source};
    for(char& c: source) if(c < 0) c = ' ';
    arrayAppend(self._sources, Utility::move(source));
}
#endif

Shader& Shader::addFile(const Containers::StringView filename) {
    Containers::Optional<Containers::String> string = Utility::Path::readString(filename);
    CORRADE_ASSERT(string, "GL::Shader::addFile(): can't read" << filename, *this);
    addSource(*Utility::move(string));
    return *this;
}

bool Shader::compile() {
    submitCompile();
    return checkCompile();
}

void Shader::submitCompile() {
    CORRADE_ASSERT(_sources.size() > 1, "GL::Shader::compile(): no files added", );

    /* Coalesce the two arrays into one allocation. Still worse than not
       allocating anything, OTOH the driver (or at least Mesa) then performs
       ~45k allocations on its own, so this is just a drop in the ocean. */
    Containers::ArrayView<const GLchar*> pointers;
    Containers::ArrayView<GLint> sizes;
    Containers::ArrayTuple data{
        {NoInit, _sources.size(), pointers},
        {NoInit, _sources.size(), sizes},
    };

    /* Upload sources of all shaders */
    for(std::size_t i = 0; i != _sources.size(); ++i) {
        pointers[i] = static_cast<const GLchar*>(_sources[i].data());
        sizes[i] = _sources[i].size();
    }

    glShaderSource(_id, _sources.size(), pointers, sizes);
    glCompileShader(_id);
}

bool Shader::checkCompile() {
    GLint success, logLength;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
    glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength);

    /* Error or warning message. The length is reported including the null
       terminator and the string implicitly has a storage for that, thus
       specify one byte less. */
    Containers::String message{NoInit, std::size_t(Math::max(logLength, 1)) - 1};
    if(logLength > 1)
        glGetShaderInfoLog(_id, logLength, nullptr, message.data());

    /* Some drivers are chatty and can't keep shut when there's nothing to
       be said, handle that as well. */
    Context::current().state().shader.cleanLogImplementation(message);

    /* Usually the driver messages contain a newline at the end. But sometimes
       not, such as in case of a program link error due to shaders not being
       compiled yet on Mesa; sometimes there's two newlines, sometimes just a
       newline and nothing else etc. Because trying do this in driver-specific
       workarounds would involve an impossible task of checking all possible
       error messages on every possible driver, just trim all whitespace around
       the message always and let Debug add its own newline. */
    const Containers::StringView messageTrimmed = Containers::StringView{message}.trimmed();

    /* Show error log */
    if(!success) {
        Error{} << "GL::Shader::compile(): compilation of" << shaderName(_type)
            << "shader failed with the following message:" << Debug::newline
            << messageTrimmed;

    /* Or just warnings, if any */
    } else if(messageTrimmed) {
        Warning{} << "GL::Shader::compile(): compilation of" << shaderName(_type)
            << "shader succeeded with the following message:" << Debug::newline
            << messageTrimmed;
    }

    return success;
}

#ifdef MAGNUM_BUILD_DEPRECATED
bool Shader::compile(std::initializer_list<Containers::Reference<Shader>> shaders) {
    /* Invoke (possibly parallel) compilation on all shaders */
    for(Shader& shader: shaders) shader.submitCompile();
    bool allSuccess = true;
    for(Shader& shader: shaders) allSuccess = allSuccess && shader.checkCompile();
    return allSuccess;
}
#endif

bool Shader::isCompileFinished() {
    GLint success;
    Context::current().state().shader.completionStatusImplementation(_id, GL_COMPLETION_STATUS_KHR, &success);
    return success == GL_TRUE;
}

void Shader::cleanLogImplementationNoOp(Containers::String&) {}

#if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES)
void Shader::cleanLogImplementationIntelWindows(Containers::String& message) {
    if(message == "No errors.\n") message = {};
}
#endif

void Shader::completionStatusImplementationFallback(GLuint, GLenum, GLint* value) {
    *value = GL_TRUE;
}

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

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

}}
