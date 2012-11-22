/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "Context.h"

#include <string>
#include <unordered_map>
#include <Utility/Debug.h>

#include "AbstractShaderProgram.h"
#include "AbstractTexture.h"
#include "Buffer.h"
#include "BufferedTexture.h"
#include "Extensions.h"
#include "IndexedMesh.h"
#include "Mesh.h"
#include "Implementation/State.h"
#include "DebugMarker.h"

using namespace std;

namespace Magnum {

Debug operator<<(Debug debug, Version value) {
    switch(value) {
        #define _c(value, string) case Version::value: return debug << string;
        _c(None, "None")
        #ifndef MAGNUM_TARGET_GLES
        _c(GL210, "OpenGL 2.1")
        _c(GL300, "OpenGL 3.0")
        _c(GL310, "OpenGL 3.1")
        _c(GL320, "OpenGL 3.2")
        _c(GL330, "OpenGL 3.3")
        _c(GL400, "OpenGL 4.0")
        _c(GL410, "OpenGL 4.1")
        _c(GL420, "OpenGL 4.2")
        _c(GL430, "OpenGL 4.3")
        #else
        _c(GLES200, "OpenGL ES 2.0")
        _c(GLES300, "OpenGL ES 3.0")
        #endif
        #undef _c
    }

    return debug << "Invalid";
}

const std::vector<Extension>& Extension::extensions(Version version) {
    #define _extension(prefix, vendor, extension)                           \
        {Extensions::prefix::vendor::extension::Index, Extensions::prefix::vendor::extension::requiredVersion(), Extensions::prefix::vendor::extension::coreVersion(), Extensions::prefix::vendor::extension::string()}
    static const std::vector<Extension> empty;
    #ifndef MAGNUM_TARGET_GLES
    static const std::vector<Extension> extensions{
        _extension(GL,AMD,vertex_shader_layer),
        _extension(GL,AMD,shader_trinary_minmax),
        _extension(GL,EXT,texture_filter_anisotropic),
        _extension(GL,EXT,direct_state_access),
        _extension(GL,GREMEDY,string_marker)};
    static const std::vector<Extension> extensions300{
        _extension(GL,APPLE,flush_buffer_range),
        _extension(GL,APPLE,vertex_array_object),
        _extension(GL,ARB,color_buffer_float),
        _extension(GL,ARB,half_float_pixel),
        _extension(GL,ARB,texture_float),
        _extension(GL,ARB,depth_buffer_float),
        _extension(GL,ARB,texture_rg),
        _extension(GL,EXT,framebuffer_object),
        _extension(GL,EXT,packed_depth_stencil),
        _extension(GL,EXT,framebuffer_blit),
        _extension(GL,EXT,framebuffer_multisample),
        _extension(GL,EXT,gpu_shader4),
        _extension(GL,EXT,packed_float),
        _extension(GL,EXT,texture_array),
        _extension(GL,EXT,texture_compression_rgtc),
        _extension(GL,EXT,texture_shared_exponent),
        _extension(GL,EXT,framebuffer_sRGB),
        _extension(GL,EXT,draw_buffers2),
        _extension(GL,EXT,texture_integer),
        _extension(GL,EXT,transform_feedback),
        _extension(GL,NV,half_float),
        _extension(GL,NV,depth_buffer_float),
        _extension(GL,NV,conditional_render)};
    static const std::vector<Extension> extensions310{
        _extension(GL,ARB,texture_rectangle),
        _extension(GL,ARB,draw_instanced),
        _extension(GL,ARB,texture_buffer_object),
        _extension(GL,ARB,uniform_buffer_object),
        _extension(GL,ARB,copy_buffer),
        _extension(GL,EXT,texture_snorm),
        _extension(GL,NV,primitive_restart)};
    static const std::vector<Extension> extensions320{
        _extension(GL,ARB,geometry_shader4),
        _extension(GL,ARB,depth_clamp),
        _extension(GL,ARB,draw_elements_base_vertex),
        _extension(GL,ARB,fragment_coord_conventions),
        _extension(GL,ARB,provoking_vertex),
        _extension(GL,ARB,seamless_cube_map),
        _extension(GL,ARB,sync),
        _extension(GL,ARB,texture_multisample),
        _extension(GL,ARB,vertex_array_bgra)};
    static const std::vector<Extension> extensions330{
        _extension(GL,ARB,instanced_arrays),
        _extension(GL,ARB,blend_func_extended),
        _extension(GL,ARB,explicit_attrib_location),
        _extension(GL,ARB,occlusion_query2),
        _extension(GL,ARB,sampler_objects),
        _extension(GL,ARB,shader_bit_encoding),
        _extension(GL,ARB,texture_rgb10_a2ui),
        _extension(GL,ARB,texture_swizzle),
        _extension(GL,ARB,timer_query),
        _extension(GL,ARB,vertex_type_2_10_10_10_rev)};
    static const std::vector<Extension> extensions400{
        _extension(GL,ARB,draw_buffers_blend),
        _extension(GL,ARB,sample_shading),
        _extension(GL,ARB,texture_cube_map_array),
        _extension(GL,ARB,texture_gather),
        _extension(GL,ARB,texture_query_lod),
        _extension(GL,ARB,draw_indirect),
        _extension(GL,ARB,gpu_shader5),
        _extension(GL,ARB,gpu_shader_fp64),
        _extension(GL,ARB,shader_subroutine),
        _extension(GL,ARB,tessellation_shader),
        _extension(GL,ARB,texture_buffer_object_rgb32),
        _extension(GL,ARB,transform_feedback2),
        _extension(GL,ARB,transform_feedback3)};
    static const std::vector<Extension> extensions410{
        _extension(GL,ARB,ES2_compatibility),
        _extension(GL,ARB,get_program_binary),
        _extension(GL,ARB,separate_shader_objects),
        _extension(GL,ARB,shader_precision),
        _extension(GL,ARB,vertex_attrib_64bit),
        _extension(GL,ARB,viewport_array)};
    static const std::vector<Extension> extensions420{
        _extension(GL,ARB,texture_compression_bptc),
        _extension(GL,ARB,base_instance),
        _extension(GL,ARB,shading_language_420pack),
        _extension(GL,ARB,transform_feedback_instanced),
        _extension(GL,ARB,compressed_texture_pixel_storage),
        _extension(GL,ARB,conservative_depth),
        _extension(GL,ARB,internalformat_query),
        _extension(GL,ARB,map_buffer_alignment),
        _extension(GL,ARB,shader_atomic_counters),
        _extension(GL,ARB,shader_image_load_store),
        _extension(GL,ARB,texture_storage)};
    static const std::vector<Extension> extensions430;
    #undef _extension
    #else
    static const std::vector<Extension> extensions;
    static const std::vector<Extension> extensionsES200;
    static const std::vector<Extension> extensionsES300;
    #endif

    switch(version) {
        case Version::None:  return extensions;
        #ifndef MAGNUM_TARGET_GLES
        case Version::GL210: return empty;
        case Version::GL300: return extensions300;
        case Version::GL310: return extensions310;
        case Version::GL320: return extensions320;
        case Version::GL330: return extensions330;
        case Version::GL400: return extensions400;
        /* case Version::GLES200: */
        case Version::GL410: return extensions410;
        case Version::GL420: return extensions420;
        /* case Version::GLES300: */
        case Version::GL430: return extensions430;
        #else
        case Version::GLES200: return extensionsES200;
        case Version::GLES300: return extensionsES300;
        #endif
    }

    return empty;
}

Context* Context::_current = nullptr;

Context::Context() {
    /* Version */
    #ifndef MAGNUM_TARGET_GLES2
    glGetIntegerv(GL_MAJOR_VERSION, &_majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &_minorVersion);
    #else
    _majorVersion = 2;
    _minorVersion = 0;
    #endif
    _version = static_cast<Version>(_majorVersion*100+_minorVersion*10);

    /* Get first future (not supported) version */
    vector<Version> versions{
        #ifndef MAGNUM_TARGET_GLES
        Version::GL300,
        Version::GL310,
        Version::GL320,
        Version::GL330,
        Version::GL400,
        Version::GL410,
        Version::GL420,
        Version::GL430,
        #else
        Version::GLES200,
        Version::GLES300,
        #endif
        Version::None
    };
    size_t future = 0;
    while(versions[future] != Version::None && !isVersionSupported(_version))
        ++future;

    /* List of extensions from future versions (extensions from current and
       previous versions should be supported automatically, so we don't need
       to check for them) */
    unordered_map<string, Extension> futureExtensions;
    for(size_t i = future; i != versions.size(); ++i)
        for(const Extension& extension: Extension::extensions(versions[i]))
            futureExtensions.insert(make_pair(extension._string, extension));

    /* Check for presence of extensions in future versions */
    #ifndef MAGNUM_TARGET_GLES
    if(isVersionSupported(Version::GL300)) {
    #else
    if(isVersionSupported(Version::GLES300)) {
    #endif
        #ifndef MAGNUM_TARGET_GLES2
        GLuint index = 0;
        const char* extension;
        while((extension = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, index++)))) {
            auto found = futureExtensions.find(extension);
            if(found != futureExtensions.end()) {
                _supportedExtensions.push_back(found->second);
                extensionStatus.set(found->second._index);
            }
        }
        #endif

    /* OpenGL 2.1 / OpenGL ES 2.0 doesn't have glGetStringi() */
    } else {
        /* Don't crash when glGetString() returns nullptr */
        const char* e = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
        if(e) {
            vector<string> extensions = Corrade::Utility::split(e, ' ');
            for(const string& extension: extensions) {
                auto found = futureExtensions.find(extension);
                if(found != futureExtensions.end()) {
                    _supportedExtensions.push_back(found->second);
                    extensionStatus.set(found->second._index);
                }
            }
        }
    }

    /* Set this context as current */
    CORRADE_ASSERT(!_current, "Context: Another context currently active", );
    _current = this;

    /* Initialize state tracker */
    _state = new Implementation::State;

    /* Initialize functionality based on current OpenGL version and extensions */
    AbstractShaderProgram::initializeContextBasedFunctionality(this);
    AbstractTexture::initializeContextBasedFunctionality(this);
    Buffer::initializeContextBasedFunctionality(this);
    #ifndef MAGNUM_TARGET_GLES
    BufferedTexture::initializeContextBasedFunctionality(this);
    #endif
    DebugMarker::initializeContextBasedFunctionality(this);
    IndexedMesh::initializeContextBasedFunctionality(this);
    Mesh::initializeContextBasedFunctionality(this);
}

Context::~Context() {
    CORRADE_ASSERT(_current == this, "Context: Cannot destroy context which is not currently active", );
    delete _state;
    _current = nullptr;
}

Version Context::supportedVersion(initializer_list<Version> versions) const {
    for(auto version: versions)
        if(isVersionSupported(version)) return version;

    #ifndef MAGNUM_TARGET_GLES
    return Version::GL210;
    #else
    return Version::GLES200;
    #endif
}

}
