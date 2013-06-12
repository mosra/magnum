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

#include "Context.h"

#include <string>
#include <unordered_map>
#include <Utility/Debug.h>
#include <Utility/String.h>

#include "AbstractFramebuffer.h"
#include "AbstractShaderProgram.h"
#include "AbstractTexture.h"
#include "Buffer.h"
#include "BufferTexture.h"
#include "DebugMarker.h"
#include "DefaultFramebuffer.h"
#include "Extensions.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "Renderbuffer.h"
#include "Renderer.h"

#include "Implementation/State.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
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
#endif

const std::vector<Extension>& Extension::extensions(Version version) {
    #define _extension(prefix, vendor, extension)                           \
        {Extensions::prefix::vendor::extension::Index, Extensions::prefix::vendor::extension::requiredVersion(), Extensions::prefix::vendor::extension::coreVersion(), Extensions::prefix::vendor::extension::string()}
    static const std::vector<Extension> empty;
    #ifndef MAGNUM_TARGET_GLES
    static const std::vector<Extension> extensions{
        _extension(GL,AMD,vertex_shader_layer),             // done
        _extension(GL,AMD,shader_trinary_minmax),           // done
        _extension(GL,ARB,robustness),                      // done
        _extension(GL,EXT,texture_filter_anisotropic),      // done
        _extension(GL,EXT,direct_state_access),
        _extension(GL,GREMEDY,string_marker)};              // done
    static const std::vector<Extension> extensions300{
        /**
         * @todo Remove as it doesn't have all functionality present in GL 3.0
         *      and leave only ARB_map_buffer_range?
         */
        _extension(GL,APPLE,flush_buffer_range),            // done
        _extension(GL,APPLE,vertex_array_object),           // done
        _extension(GL,ARB,map_buffer_range),                // done, replaces APPLE_flush_buffer_range
        _extension(GL,ARB,color_buffer_float),
        _extension(GL,ARB,half_float_pixel),                // done
        _extension(GL,ARB,texture_float),                   // done
        _extension(GL,ARB,depth_buffer_float),              // done
        _extension(GL,ARB,texture_rg),                      // done
        _extension(GL,ARB,framebuffer_object),
        _extension(GL,EXT,gpu_shader4),
        _extension(GL,EXT,packed_float),                    // done
        _extension(GL,EXT,texture_array),
        _extension(GL,EXT,texture_compression_rgtc),        // done
        _extension(GL,EXT,texture_shared_exponent),         // done
        _extension(GL,EXT,framebuffer_sRGB),
        _extension(GL,EXT,draw_buffers2),
        _extension(GL,EXT,texture_integer),
        _extension(GL,EXT,transform_feedback),
        _extension(GL,NV,half_float),                       // done
        _extension(GL,NV,depth_buffer_float),
        _extension(GL,NV,conditional_render)};              // done
    static const std::vector<Extension> extensions310{
        _extension(GL,ARB,texture_rectangle),
        _extension(GL,ARB,draw_instanced),
        _extension(GL,ARB,texture_buffer_object),
        _extension(GL,ARB,uniform_buffer_object),
        _extension(GL,ARB,copy_buffer),                     // done
        _extension(GL,EXT,texture_snorm),                   // done
        _extension(GL,NV,primitive_restart)};
    static const std::vector<Extension> extensions320{
        _extension(GL,ARB,geometry_shader4),
        _extension(GL,ARB,depth_clamp),                     // done
        _extension(GL,ARB,draw_elements_base_vertex),
        _extension(GL,ARB,fragment_coord_conventions),      // done
        _extension(GL,ARB,provoking_vertex),                // done
        _extension(GL,ARB,seamless_cube_map),               // done
        _extension(GL,ARB,sync),
        _extension(GL,ARB,texture_multisample),
        _extension(GL,ARB,vertex_array_bgra)};              // done
    static const std::vector<Extension> extensions330{
        _extension(GL,ARB,instanced_arrays),
        _extension(GL,ARB,blend_func_extended),
        _extension(GL,ARB,explicit_attrib_location),        // done
        _extension(GL,ARB,occlusion_query2),                // done
        _extension(GL,ARB,sampler_objects),
        _extension(GL,ARB,shader_bit_encoding),             // done
        _extension(GL,ARB,texture_rgb10_a2ui),              // done
        _extension(GL,ARB,texture_swizzle),
        _extension(GL,ARB,timer_query),
        _extension(GL,ARB,vertex_type_2_10_10_10_rev)};     // done
    static const std::vector<Extension> extensions400{
        _extension(GL,ARB,draw_buffers_blend),
        _extension(GL,ARB,sample_shading),
        _extension(GL,ARB,texture_cube_map_array),          // done
        _extension(GL,ARB,texture_gather),
        _extension(GL,ARB,texture_query_lod),               // done
        _extension(GL,ARB,draw_indirect),
        _extension(GL,ARB,gpu_shader5),
        _extension(GL,ARB,gpu_shader_fp64),                 // done
        _extension(GL,ARB,shader_subroutine),
        _extension(GL,ARB,tessellation_shader),
        _extension(GL,ARB,texture_buffer_object_rgb32),     // done
        _extension(GL,ARB,transform_feedback2),
        _extension(GL,ARB,transform_feedback3)};
    static const std::vector<Extension> extensions410{
        _extension(GL,ARB,ES2_compatibility),
        _extension(GL,ARB,get_program_binary),
        _extension(GL,ARB,separate_shader_objects),
        _extension(GL,ARB,shader_precision),                // done
        _extension(GL,ARB,vertex_attrib_64bit),             // done
        _extension(GL,ARB,viewport_array)};
    static const std::vector<Extension> extensions420{
        _extension(GL,ARB,texture_compression_bptc),        // done
        _extension(GL,ARB,base_instance),
        _extension(GL,ARB,shading_language_420pack),        // done
        _extension(GL,ARB,transform_feedback_instanced),
        _extension(GL,ARB,compressed_texture_pixel_storage),
        _extension(GL,ARB,conservative_depth),              // done
        _extension(GL,ARB,internalformat_query),
        _extension(GL,ARB,map_buffer_alignment),
        _extension(GL,ARB,shader_atomic_counters),
        _extension(GL,ARB,shader_image_load_store),
        _extension(GL,ARB,texture_storage)};
    static const std::vector<Extension> extensions430{
        _extension(GL,ARB,arrays_of_arrays),                // done
        _extension(GL,ARB,ES3_compatibility),
        _extension(GL,ARB,clear_buffer_object),
        _extension(GL,ARB,compute_shader),
        _extension(GL,ARB,copy_image),
        _extension(GL,KHR,debug),
        _extension(GL,ARB,explicit_uniform_location),
        _extension(GL,ARB,fragment_layer_viewport),         // done
        _extension(GL,ARB,framebuffer_no_attachments),
        _extension(GL,ARB,internalformat_query2),
        _extension(GL,ARB,invalidate_subdata),              // done
        _extension(GL,ARB,multi_draw_indirect),
        _extension(GL,ARB,program_interface_query),
        _extension(GL,ARB,robust_buffer_access_behavior),   // done
        _extension(GL,ARB,shader_image_size),               // done
        _extension(GL,ARB,shader_storage_buffer_object),
        _extension(GL,ARB,stencil_texturing),
        _extension(GL,ARB,texture_buffer_range),            // done
        _extension(GL,ARB,texture_query_levels),            // done
        _extension(GL,ARB,texture_storage_multisample),
        _extension(GL,ARB,texture_view),
        _extension(GL,ARB,vertex_attrib_binding)};
    #undef _extension
    #else
    static const std::vector<Extension> extensions{
        _extension(GL,APPLE,texture_format_BGRA8888),
        _extension(GL,EXT,texture_filter_anisotropic),
        _extension(GL,EXT,texture_format_BGRA8888),
        _extension(GL,EXT,read_format_bgra),
        _extension(GL,EXT,debug_marker),
        _extension(GL,EXT,disjoint_timer_query),
        _extension(GL,EXT,separate_shader_objects),
        _extension(GL,EXT,sRGB),
        /**
         * @todo Support also IMG_multisampled_render_to_texture? It has
         *      different enum values (!)
         */
        _extension(GL,EXT,multisampled_render_to_texture),
        _extension(GL,EXT,robustness),
        _extension(GL,KHR,debug),
        _extension(GL,NV,read_buffer_front),
        _extension(GL,NV,read_stencil),
        _extension(GL,NV,texture_border_clamp),             // done
        _extension(GL,OES,depth32),
        _extension(GL,OES,mapbuffer),
        _extension(GL,OES,stencil1),
        _extension(GL,OES,stencil4),
        _extension(GL,OES,texture_3D)};
    static const std::vector<Extension> extensionsES300{
        _extension(GL,ANGLE,framebuffer_blit),
        _extension(GL,ANGLE,framebuffer_multisample),
        _extension(GL,ANGLE,depth_texture),                 // done
        _extension(GL,APPLE,framebuffer_multisample),
        _extension(GL,ARM,rgba8),
        _extension(GL,EXT,texture_type_2_10_10_10_REV),
        _extension(GL,EXT,discard_framebuffer),
        _extension(GL,EXT,blend_minmax),
        _extension(GL,EXT,occlusion_query_boolean),
        _extension(GL,EXT,texture_rg),
        _extension(GL,EXT,texture_storage),
        _extension(GL,EXT,map_buffer_range),
        _extension(GL,NV,draw_buffers),
        _extension(GL,NV,read_buffer),
        _extension(GL,NV,read_depth),
        _extension(GL,NV,read_depth_stencil),
        _extension(GL,NV,framebuffer_blit),                 // done
        _extension(GL,NV,framebuffer_multisample),
        _extension(GL,OES,depth24),
        _extension(GL,OES,element_index_uint),
        _extension(GL,OES,rgb8_rgba8),
        _extension(GL,OES,texture_half_float_linear),
        _extension(GL,OES,texture_float_linear),
        _extension(GL,OES,texture_half_float),
        _extension(GL,OES,texture_float),
        _extension(GL,OES,vertex_half_float),
        _extension(GL,OES,packed_depth_stencil),
        _extension(GL,OES,depth_texture),
        _extension(GL,OES,standard_derivatives),            // done
        _extension(GL,OES,vertex_array_object),
        _extension(GL,OES,required_internalformat),
        _extension(GL,OES,surfaceless_context)};            // done
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
        case Version::GLES200: return empty;
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

    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(isVersionSupported(Version::GL210), "Context: unsupported OpenGL version" << Int(_version), );
    #elif defined(MAGNUM_TARGET_GLES2)
    CORRADE_ASSERT(isVersionSupported(Version::GLES200), "Context: unsupported OpenGL ES version" << Int(_version), );
    #else
    CORRADE_ASSERT(isVersionSupported(Version::GLES300), "Context: unsupported OpenGL ES version" << Int(_version), );
    #endif

    /* Context flags are supported since GL 3.0 */
    #ifndef MAGNUM_TARGET_GLES
    /**
     * @todo According to KHR_debug specs this should be also present in ES2
     *      if KHR_debug is available, but in headers it is nowhere to be found
     */
    if(isVersionSupported(Version::GL300))
        glGetIntegerv(GL_CONTEXT_FLAGS, reinterpret_cast<GLint*>(&_flags));
    #endif

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
        #else
        Version::GLES200,
        Version::GLES300,
        #endif
        Version::None
    };
    std::size_t future = 0;
    while(versions[future] != Version::None && isVersionSupported(versions[future]))
        ++future;

    /* List of extensions from future versions (extensions from current and
       previous versions should be supported automatically, so we don't need
       to check for them) */
    std::unordered_map<std::string, Extension> futureExtensions;
    for(std::size_t i = future; i != versions.size(); ++i) {
        const std::vector<Extension>& extensions = Extension::extensions(versions[i]);
        for(auto it = extensions.begin(); it != extensions.end(); ++it)
            futureExtensions.insert(std::make_pair(it->_string, *it));
    }

    /* Check for presence of extensions in future versions */
    #ifndef MAGNUM_TARGET_GLES2
    GLint extensionCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
    #ifndef MAGNUM_TARGET_GLES3
    if(extensionCount || isVersionSupported(Version::GL300))
    #endif
    {
        _supportedExtensions.reserve(extensionCount);
        for(GLint i = 0; i != extensionCount; ++i) {
            const std::string extension(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));
            auto found = futureExtensions.find(extension);
            if(found != futureExtensions.end()) {
                _supportedExtensions.push_back(found->second);
                extensionStatus.set(found->second._index);
            }
        }
    }
    #ifndef MAGNUM_TARGET_GLES3
    else
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES3
    /* OpenGL 2.1 / OpenGL ES 2.0 doesn't have glGetStringi() */
    {
        /* Don't crash when glGetString() returns nullptr */
        const char* e = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
        if(e) {
            std::vector<std::string> extensions = Utility::String::split(e, ' ');
            for(auto it = extensions.begin(); it != extensions.end(); ++it) {
                auto found = futureExtensions.find(*it);
                if(found != futureExtensions.end()) {
                    _supportedExtensions.push_back(found->second);
                    extensionStatus.set(found->second._index);
                }
            }
        }
    }
    #endif

    /* Set this context as current */
    CORRADE_ASSERT(!_current, "Context: Another context currently active", );
    _current = this;

    /* Initialize state tracker */
    _state = new Implementation::State;

    /* Initialize functionality based on current OpenGL version and extensions */
    AbstractFramebuffer::initializeContextBasedFunctionality(this);
    AbstractShaderProgram::initializeContextBasedFunctionality(this);
    AbstractTexture::initializeContextBasedFunctionality(this);
    Buffer::initializeContextBasedFunctionality(this);
    #ifndef MAGNUM_TARGET_GLES
    BufferTexture::initializeContextBasedFunctionality(this);
    #endif
    DebugMarker::initializeContextBasedFunctionality(this);
    DefaultFramebuffer::initializeContextBasedFunctionality(this);
    Framebuffer::initializeContextBasedFunctionality(this);
    Mesh::initializeContextBasedFunctionality(this);
    Renderbuffer::initializeContextBasedFunctionality(this);
    Renderer::initializeContextBasedFunctionality(this);
}

Context::~Context() {
    CORRADE_ASSERT(_current == this, "Context: Cannot destroy context which is not currently active", );
    delete _state;
    _current = nullptr;
}

std::vector<std::string> Context::shadingLanguageVersionStrings() const {
    #ifndef MAGNUM_TARGET_GLES
    GLint versionCount = 0;
    glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &versionCount);

    /* The implementation doesn't yet support this query (< OpenGL 4.3) */
    if(!versionCount)
        return {shadingLanguageVersionString()};

    /* Get all of them */
    std::vector<std::string> versions;
    versions.reserve(versionCount);
    for(GLint i = 0; i != versionCount; ++i)
        versions.push_back(reinterpret_cast<const char*>(glGetStringi(GL_SHADING_LANGUAGE_VERSION, i)));
    return std::move(versions);
    #else
    return {shadingLanguageVersionString()};
    #endif
}

Version Context::supportedVersion(std::initializer_list<Version> versions) const {
    for(auto it = versions.begin(); it != versions.end(); ++it)
        if(isVersionSupported(*it)) return *it;

    #ifndef MAGNUM_TARGET_GLES
    return Version::GL210;
    #else
    return Version::GLES200;
    #endif
}

}
