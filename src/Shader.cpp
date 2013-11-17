/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <fstream>
#include <Containers/Array.h>
#include <Utility/Assert.h>

#include "Extensions.h"
#include "Implementation/State.h"
#include "Implementation/ShaderState.h"

#if defined(CORRADE_TARGET_NACL_NEWLIB) || defined(__MINGW32__)
#include <sstream>
#endif

/* libgles-omap3-dev_4.03.00.02-r15.6 on BeagleBoard/Ångström linux 2011.3 doesn't have GLchar */
#ifdef MAGNUM_TARGET_GLES
typedef char GLchar;
#endif

namespace Magnum {

namespace {

std::string shaderName(const Shader::Type type) {
    switch(type) {
        case Shader::Type::Vertex:                  return "vertex";
        #ifndef MAGNUM_TARGET_GLES
        case Shader::Type::Geometry:                return "geometry";
        case Shader::Type::TessellationControl:     return "tessellation control";
        case Shader::Type::TessellationEvaluation:  return "tessellation evaluation";
        case Shader::Type::Compute:                 return "compute";
        #endif
        case Shader::Type::Fragment:                return "fragment";
    }

    CORRADE_ASSERT_UNREACHABLE();
}

UnsignedInt typeToIndex(const Shader::Type type) {
    switch(type) {
        case Shader::Type::Vertex:                  return 0;
        case Shader::Type::Fragment:                return 1;
        #ifndef MAGNUM_TARGET_GLES
        case Shader::Type::Geometry:                return 2;
        case Shader::Type::TessellationControl:     return 3;
        case Shader::Type::TessellationEvaluation:  return 4;
        case Shader::Type::Compute:                 return 5;
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE();
}

#ifndef MAGNUM_TARGET_GLES
bool isTypeSupported(const Shader::Type type) {
    if(type == Shader::Type::Geometry && !Context::current()->isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        return false;

    if((type == Shader::Type::TessellationControl || type == Shader::Type::TessellationEvaluation) && !Context::current()->isExtensionSupported<Extensions::GL::ARB::tessellation_shader>())
        return false;

    if(type == Shader::Type::Compute && !Context::current()->isExtensionSupported<Extensions::GL::ARB::compute_shader>())
        return false;

    return true;
}
#else
constexpr bool isTypeSupported(Shader::Type) { return true; }
#endif

}

Int Shader::maxVertexOutputComponents() {
    GLint& value = Context::current()->state().shader->maxVertexOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES
        if(Context::current()->isVersionSupported(Version::GL320))
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

#ifndef MAGNUM_TARGET_GLES
Int Shader::maxTessellationControlInputComponents() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::tessellation_shader>())
        return 0;

    GLint& value = Context::current()->state().shader->maxTessellationControlInputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxTessellationControlOutputComponents() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::tessellation_shader>())
        return 0;

    GLint& value = Context::current()->state().shader->maxTessellationControlOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxTessellationControlTotalOutputComponents() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::tessellation_shader>())
        return 0;

    GLint& value = Context::current()->state().shader->maxTessellationControlTotalOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxTessellationEvaluationInputComponents() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::tessellation_shader>())
        return 0;

    GLint& value = Context::current()->state().shader->maxTessellationEvaluationInputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxTessellationEvaluationOutputComponents() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::tessellation_shader>())
        return 0;

    GLint& value = Context::current()->state().shader->maxTessellationEvaluationOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxGeometryInputComponents() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        return 0;

    GLint& value = Context::current()->state().shader->maxGeometryInputComponents;

    /* Get the value, if not already cached */
    /** @todo The extension has only `GL_MAX_GEOMETRY_VARYING_COMPONENTS_ARB`, this is supported since GL 3.2 (wtf?) */
    if(value == 0)
        glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxGeometryOutputComponents() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        return 0;

    GLint& value = Context::current()->state().shader->maxGeometryOutputComponents;

    /* Get the value, if not already cached */
    /** @todo The extension has only `GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB`, this is supported since GL 3.2 (wtf?) */
    if(value == 0)
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &value);

    return value;
}

Int Shader::maxGeometryTotalOutputComponents() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::geometry_shader4>())
        return 0;

    GLint& value = Context::current()->state().shader->maxGeometryTotalOutputComponents;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &value);

    return value;
}
#endif

Int Shader::maxFragmentInputComponents() {
    GLint& value = Context::current()->state().shader->maxFragmentInputComponents;

    /* Get the value, if not already cached */
    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES
        if(Context::current()->isVersionSupported(Version::GL320))
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

#ifndef MAGNUM_TARGET_GLES
Int Shader::maxAtomicCounterBuffers(const Type type) {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_atomic_counters>() || !isTypeSupported(type))
        return 0;

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current()->state().shader->maxAtomicCounterBuffers[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS,
        GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS,
        GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS,
        GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS,
        GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS,
        GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedAtomicCounterBuffers() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_atomic_counters>())
        return 0;

    GLint& value = Context::current()->state().shader->maxCombinedAtomicCounterBuffers;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS, &value);

    return value;
}

Int Shader::maxAtomicCounters(const Type type) {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_atomic_counters>() || !isTypeSupported(type))
        return 0;

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current()->state().shader->maxAtomicCounters[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_ATOMIC_COUNTERS,
        GL_MAX_FRAGMENT_ATOMIC_COUNTERS,
        GL_MAX_GEOMETRY_ATOMIC_COUNTERS,
        GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS,
        GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS,
        GL_MAX_COMPUTE_ATOMIC_COUNTERS
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedAtomicCounters() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_atomic_counters>())
        return 0;

    GLint& value = Context::current()->state().shader->maxCombinedAtomicCounters;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, &value);

    return value;
}

Int Shader::maxImageUniforms(const Type type) {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>() || !isTypeSupported(type))
        return 0;

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current()->state().shader->maxImageUniforms[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_IMAGE_UNIFORMS,
        GL_MAX_FRAGMENT_IMAGE_UNIFORMS,
        GL_MAX_GEOMETRY_IMAGE_UNIFORMS,
        GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS,
        GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS,
        GL_MAX_COMPUTE_IMAGE_UNIFORMS
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedImageUniforms() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_image_load_store>())
        return 0;

    GLint& value = Context::current()->state().shader->maxCombinedImageUniforms;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_IMAGE_UNIFORMS, &value);

    return value;
}

Int Shader::maxShaderStorageBlocks(const Type type) {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_storage_buffer_object>() || !isTypeSupported(type))
        return 0;

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current()->state().shader->maxShaderStorageBlocks[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,
        GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,
        GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,
        GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,
        GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,
        GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedShaderStorageBlocks() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::shader_atomic_counters>())
        return 0;

    GLint& value = Context::current()->state().shader->maxCombinedShaderStorageBlocks;

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
    GLint& value = Context::current()->state().shader->maxTextureImageUnits[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
        GL_MAX_TEXTURE_IMAGE_UNITS,
        #ifndef MAGNUM_TARGET_GLES
        GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,
        GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS,
        GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS,
        GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS
        #endif
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedTextureImageUnits() {
    GLint& value = Context::current()->state().shader->maxTextureImageUnitsCombined;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value);

    return value;
}

#ifndef MAGNUM_TARGET_GLES2
Int Shader::maxUniformBlocks(const Type type) {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::uniform_buffer_object>() || !isTypeSupported(type))
    #else
    if(!isTypeSupported(type))
    #endif
        return 0;

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current()->state().shader->maxUniformBlocks[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_UNIFORM_BLOCKS,
        GL_MAX_FRAGMENT_UNIFORM_BLOCKS,
        #ifndef MAGNUM_TARGET_GLES
        /** @todo Fix this when glLoadGen has GL_MAX_GEOMETRY_UNIFORM_BLOCKS enum */
        0x8A2C /*GL_MAX_GEOMETRY_UNIFORM_BLOCKS*/,
        GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS,
        GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS,
        GL_MAX_COMPUTE_UNIFORM_BLOCKS
        #endif
    };
    if(value == 0)
        glGetIntegerv(what[index], &value);

    return value;
}

Int Shader::maxCombinedUniformBlocks() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::uniform_buffer_object>())
        return 0;
    #endif

    GLint& value = Context::current()->state().shader->maxCombinedUniformBlocks;

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
    GLint& value = Context::current()->state().shader->maxUniformComponents[index];

    /* Get the value, if not already cached */
    #ifndef MAGNUM_TARGET_GLES2
    constexpr static GLenum what[] = {
        GL_MAX_VERTEX_UNIFORM_COMPONENTS,
        GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
        #ifndef MAGNUM_TARGET_GLES
        GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,
        GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS,
        GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS,
        GL_MAX_COMPUTE_UNIFORM_COMPONENTS
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
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::uniform_buffer_object>() || !isTypeSupported(type))
    #else
    if(!isTypeSupported(type))
    #endif
        return 0;

    const UnsignedInt index = typeToIndex(type);
    GLint& value = Context::current()->state().shader->maxCombinedUniformComponents[index];

    /* Get the value, if not already cached */
    constexpr static GLenum what[] = {
        GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
        GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
        #ifndef MAGNUM_TARGET_GLES
        /** @todo Fix this when glLoadGen has GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS enum */
        0x8A32 /*GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS*/,
        GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS,
        GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS,
        GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS
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
        case Version::GL210: sources.push_back("#version 120\n"); return;
        case Version::GL300: sources.push_back("#version 130\n"); return;
        case Version::GL310: sources.push_back("#version 140\n"); return;
        case Version::GL320: sources.push_back("#version 150\n"); return;
        case Version::GL330: sources.push_back("#version 330\n"); return;
        case Version::GL400: sources.push_back("#version 400\n"); return;
        case Version::GL410: sources.push_back("#version 410\n"); return;
        case Version::GL420: sources.push_back("#version 420\n"); return;
        case Version::GL430: sources.push_back("#version 430\n"); return;
        case Version::GL440: sources.push_back("#version 440\n"); return;
        #else
        case Version::GLES200: sources.push_back("#version 100\n"); return;
        case Version::GLES300: sources.push_back("#version 300\n"); return;
        #endif

        case Version::None:
            CORRADE_ASSERT(false, "Shader::Shader(): unsupported version" << version, );
    }

    CORRADE_ASSERT_UNREACHABLE();
}

Shader::Shader(Shader&& other): _type(other._type), _id(other._id), sources(std::move(other.sources)) {
    other._id = 0;
}

Shader::~Shader() {
    if(_id) glDeleteShader(_id);
}

Shader& Shader::operator=(Shader&& other) {
    glDeleteShader(_id);

    _type = other._type;
    sources = std::move(other.sources);
    _id = other._id;

    other._id = 0;

    return *this;
}

Shader& Shader::addSource(std::string source) {
    if(!source.empty()) {
        #if defined(CORRADE_TARGET_NACL_NEWLIB) || defined(__MINGW32__)
        std::ostringstream converter;
        converter << (sources.size()+1)/2;
        #endif

        /* Fix line numbers, so line 41 of third added file is marked as 3(41).
           Source 0 is the #version string added in constructor. */
        sources.push_back("#line 1 " +
            #if !defined(CORRADE_TARGET_NACL_NEWLIB) && !defined(__MINGW32__)
            std::to_string((sources.size()+1)/2) +
            #else
            converter.str() +
            #endif
            '\n');
        sources.push_back(std::move(source));
    }

    return *this;
}

Shader& Shader::addFile(const std::string& filename) {
    /* Open file */
    std::ifstream file(filename.c_str());
    CORRADE_ASSERT(file.good(), "Shader file " << '\'' + filename + '\'' << " cannot be opened.", *this);

    /* Get size of shader and initialize buffer */
    file.seekg(0, std::ios::end);
    std::string source(file.tellg(), '\0');

    /* Read data, close */
    file.seekg(0, std::ios::beg);
    file.read(&source[0], source.size());
    file.close();

    /* Move to sources */
    addSource(std::move(source));

    return *this;
}

bool Shader::compile() {
    CORRADE_ASSERT(sources.size() > 1, "Shader::compile(): no files added", false);

    /* Array of source string pointers and their lengths */
    /** @todo Use `Containers::::ArrayTuple` to avoid one allocation if it ever
        gets to be implemented (we need properly aligned memory too) */
    Containers::Array<const GLchar*> pointers(sources.size());
    Containers::Array<GLint> sizes(sources.size());
    for(std::size_t i = 0; i != sources.size(); ++i) {
        pointers[i] = static_cast<const GLchar*>(sources[i].data());
        sizes[i] = sources[i].size();
    }

    /* Create shader and set its source */
    glShaderSource(_id, sources.size(), pointers, sizes);

    /* Compile shader */
    glCompileShader(_id);

    /* Check compilation status */
    GLint success, logLength;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
    glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength);

    /* Error or warning message. The string is returned null-terminated, scrap
       the \0 at the end afterwards */
    std::string message(logLength, '\0');
    if(message.size() > 1)
        glGetShaderInfoLog(_id, message.size(), nullptr, &message[0]);
    message.resize(std::max(logLength, 1)-1);

    /* Show error log */
    if(!success) {
        Error out;
        out.setFlag(Debug::NewLineAtTheEnd, false);
        out.setFlag(Debug::SpaceAfterEachValue, false);
        out << "Shader: " << shaderName(_type)
            << " shader failed to compile with the following message:\n"
            << message;

    /* Or just message, if any */
    } else if(!message.empty()) {
        Error out;
        out.setFlag(Debug::NewLineAtTheEnd, false);
        out.setFlag(Debug::SpaceAfterEachValue, false);
        out << "Shader: " << shaderName(_type)
            << " shader was successfully compiled with the following message:\n"
            << message;
    }

    return success;
}

}
