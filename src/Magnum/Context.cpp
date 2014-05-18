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

#include "Context.h"

#include <string>
#include <unordered_map>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/String.h>

#include "Magnum/AbstractFramebuffer.h"
#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/AbstractTexture.h"
#include "Magnum/Buffer.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/BufferTexture.h"
#endif
#include "Magnum/DefaultFramebuffer.h"
#include "Magnum/Extensions.h"
#include "Magnum/Framebuffer.h"
#include "Magnum/Mesh.h"
#include "Magnum/Renderbuffer.h"
#include "Magnum/Renderer.h"

#include "Implementation/State.h"
#include "Implementation/BufferState.h"
#include "Implementation/FramebufferState.h"
#include "Implementation/MeshState.h"
#include "Implementation/ShaderProgramState.h"
#include "Implementation/TextureState.h"

namespace Magnum {

const std::vector<Extension>& Extension::extensions(Version version) {
    #define _extension(prefix, vendor, extension)                           \
        {Extensions::prefix::vendor::extension::Index, Extensions::prefix::vendor::extension::requiredVersion(), Extensions::prefix::vendor::extension::coreVersion(), Extensions::prefix::vendor::extension::string()}
    static const std::vector<Extension> empty;
    #ifndef MAGNUM_TARGET_GLES
    static const std::vector<Extension> extensions{
        _extension(GL,AMD,vertex_shader_layer),
        _extension(GL,AMD,shader_trinary_minmax),
        _extension(GL,ARB,robustness),
        _extension(GL,ATI,texture_mirror_once),
        _extension(GL,EXT,texture_filter_anisotropic),
        _extension(GL,EXT,texture_mirror_clamp),
        _extension(GL,EXT,direct_state_access),
        _extension(GL,EXT,texture_sRGB_decode),
        _extension(GL,EXT,shader_integer_mix),
        _extension(GL,EXT,debug_label),
        _extension(GL,EXT,debug_marker),
        _extension(GL,GREMEDY,string_marker)};
    static const std::vector<Extension> extensions300{
        _extension(GL,APPLE,flush_buffer_range),
        _extension(GL,APPLE,vertex_array_object),
        _extension(GL,ARB,map_buffer_range),
        _extension(GL,ARB,color_buffer_float),
        _extension(GL,ARB,half_float_pixel),
        _extension(GL,ARB,texture_float),
        _extension(GL,ARB,depth_buffer_float),
        _extension(GL,ARB,texture_rg),
        _extension(GL,ARB,framebuffer_object),
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
        /* Mentioned in GLSL 4.20 specs as newly added */
        _extension(GL,ARB,shading_language_packing),
        _extension(GL,ARB,texture_storage)};
    static const std::vector<Extension> extensions430{
        _extension(GL,ARB,arrays_of_arrays),
        _extension(GL,ARB,ES3_compatibility),
        _extension(GL,ARB,clear_buffer_object),
        _extension(GL,ARB,compute_shader),
        _extension(GL,ARB,copy_image),
        _extension(GL,KHR,debug),
        _extension(GL,ARB,explicit_uniform_location),
        _extension(GL,ARB,fragment_layer_viewport),
        _extension(GL,ARB,framebuffer_no_attachments),
        _extension(GL,ARB,internalformat_query2),
        _extension(GL,ARB,invalidate_subdata),
        _extension(GL,ARB,multi_draw_indirect),
        _extension(GL,ARB,program_interface_query),
        _extension(GL,ARB,robust_buffer_access_behavior),
        _extension(GL,ARB,shader_image_size),
        _extension(GL,ARB,shader_storage_buffer_object),
        _extension(GL,ARB,stencil_texturing),
        _extension(GL,ARB,texture_buffer_range),
        _extension(GL,ARB,texture_query_levels),
        _extension(GL,ARB,texture_storage_multisample),
        _extension(GL,ARB,texture_view),
        _extension(GL,ARB,vertex_attrib_binding)};
    static const std::vector<Extension> extensions440{
        _extension(GL,ARB,buffer_storage),
        _extension(GL,ARB,clear_texture),
        _extension(GL,ARB,enhanced_layouts),
        _extension(GL,ARB,multi_bind),
        _extension(GL,ARB,query_buffer_object),
        _extension(GL,ARB,texture_mirror_clamp_to_edge),
        _extension(GL,ARB,texture_stencil8),
        _extension(GL,ARB,vertex_type_10f_11f_11f_rev)};
    #undef _extension
    #else
    static const std::vector<Extension> extensions{
        _extension(GL,APPLE,texture_format_BGRA8888),
        _extension(GL,CHROMIUM,map_sub),
        _extension(GL,EXT,texture_filter_anisotropic),
        _extension(GL,EXT,texture_format_BGRA8888),
        _extension(GL,EXT,read_format_bgra),
        _extension(GL,EXT,multi_draw_arrays),
        _extension(GL,EXT,debug_label),
        _extension(GL,EXT,debug_marker),
        _extension(GL,EXT,disjoint_timer_query),
        _extension(GL,EXT,texture_sRGB_decode),
        _extension(GL,EXT,separate_shader_objects),
        _extension(GL,EXT,sRGB),
        _extension(GL,EXT,multisampled_render_to_texture),
        _extension(GL,EXT,robustness),
        _extension(GL,KHR,debug),
        _extension(GL,NV,read_buffer_front),
        _extension(GL,NV,read_depth),
        _extension(GL,NV,read_stencil),
        _extension(GL,NV,read_depth_stencil),
        _extension(GL,NV,texture_border_clamp),
        _extension(GL,OES,depth32),
        _extension(GL,OES,mapbuffer),
        _extension(GL,OES,stencil1),
        _extension(GL,OES,stencil4)};
    #ifdef MAGNUM_TARGET_GLES2
    static const std::vector<Extension> extensionsES300{
        _extension(GL,ANGLE,framebuffer_blit),
        _extension(GL,ANGLE,framebuffer_multisample),
        _extension(GL,ANGLE,instanced_arrays),
        _extension(GL,ANGLE,depth_texture),
        _extension(GL,APPLE,framebuffer_multisample),
        _extension(GL,APPLE,texture_max_level),
        _extension(GL,ARM,rgba8),
        _extension(GL,EXT,texture_type_2_10_10_10_REV),
        _extension(GL,EXT,discard_framebuffer),
        _extension(GL,EXT,blend_minmax),
        _extension(GL,EXT,shader_texture_lod),
        _extension(GL,EXT,occlusion_query_boolean),
        _extension(GL,EXT,shadow_samplers),
        _extension(GL,EXT,texture_rg),
        _extension(GL,EXT,texture_storage),
        _extension(GL,EXT,map_buffer_range),
        _extension(GL,EXT,instanced_arrays),
        _extension(GL,EXT,draw_instanced),
        _extension(GL,NV,draw_buffers),
        _extension(GL,NV,fbo_color_attachments),
        _extension(GL,NV,read_buffer),
        _extension(GL,NV,draw_instanced),
        _extension(GL,NV,framebuffer_blit),
        _extension(GL,NV,framebuffer_multisample),
        _extension(GL,NV,instanced_arrays),
        _extension(GL,NV,shadow_samplers_array),
        _extension(GL,NV,shadow_samplers_cube),
        _extension(GL,OES,depth24),
        _extension(GL,OES,element_index_uint),
        _extension(GL,OES,rgb8_rgba8),
        _extension(GL,OES,texture_3D),
        _extension(GL,OES,texture_half_float_linear),
        _extension(GL,OES,texture_float_linear),
        _extension(GL,OES,texture_half_float),
        _extension(GL,OES,texture_float),
        _extension(GL,OES,vertex_half_float),
        _extension(GL,OES,packed_depth_stencil),
        _extension(GL,OES,depth_texture),
        _extension(GL,OES,standard_derivatives),
        _extension(GL,OES,vertex_array_object),
        _extension(GL,OES,required_internalformat),
        _extension(GL,OES,surfaceless_context)};
    #endif
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
        case Version::GL440: return extensions440;
        #else
        case Version::GLES200: return empty;
        case Version::GLES300:
            #ifdef MAGNUM_TARGET_GLES2
            return extensionsES300;
            #else
            return empty;
            #endif
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE();
}

Context* Context::_current = nullptr;

Context::Context() {
    #ifndef MAGNUM_TARGET_GLES
    /* Init glLoadGen. Ignore functions that failed to load (described by
       `ogl_LOAD_SUCCEEDED + n` return code), as we requested the latest OpenGL
       with many vendor extensions and there won't ever be a driver supporting
       everything possible. */
    if(ogl_LoadFunctions() == ogl_LOAD_FAILED) {
        Error() << "ExtensionWrangler: cannot initialize glLoadGen";
        std::exit(64);
    }
    #endif

    /* Get version */
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)
    glGetIntegerv(GL_MAJOR_VERSION, &_majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &_minorVersion);
    #else

    /* On GL 2.1 and ES 2.0 there is no GL_{MAJOR,MINOR}_VERSION, we have to
       parse version string. On desktop GL we have no way to check version
       without version (duh) so we work around that by checking for invalid
       enum error. */
    #ifndef MAGNUM_TARGET_GLES2
    glGetIntegerv(GL_MAJOR_VERSION, &_majorVersion);
    const auto versionNumberError = Renderer::error();
    if(versionNumberError == Renderer::Error::NoError)
        glGetIntegerv(GL_MINOR_VERSION, &_minorVersion);
    else
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES2
        CORRADE_ASSERT(versionNumberError == Renderer::Error::InvalidEnum,
            "Context: cannot retrieve OpenGL version:" << versionNumberError, );
        #endif

        /* Allow ES2 context on driver that reports ES3 as supported */
        const std::string version = versionString();
        #ifndef MAGNUM_TARGET_GLES
        if(version.compare(0, 4, "2.1 ") == 0)
        #elif defined(MAGNUM_TARGET_WEBGL)
        if(version.find("WebGL 1") != std::string::npos)
        #else
        if(version.find("OpenGL ES 2.0") != std::string::npos ||
           version.find("OpenGL ES 3.") != std::string::npos)
        #endif
        {
            _majorVersion = 2;
            #ifndef MAGNUM_TARGET_GLES
            _minorVersion = 1;
            #else
            _minorVersion = 0;
            #endif
        } else {
            Error() << "Context: unsupported version string:" << version;
            std::exit(65);
        }
    }
    #endif

    /* Compose the version enum */
    _version = Magnum::version(_majorVersion, _minorVersion);

    /* Check that version retrieval went right */
    #ifndef CORRADE_NO_ASSERT
    const auto error = Renderer::error();
    CORRADE_ASSERT(error == Renderer::Error::NoError,
        "Context: cannot retrieve OpenGL version:" << error, );
    #endif

    /* Check that the version is supported (now it probably is, but be sure) */
    #ifndef MAGNUM_TARGET_GLES
    if(!isVersionSupported(Version::GL210))
    #elif defined(MAGNUM_TARGET_GLES2)
    if(_version != Version::GLES200)
    #else
    if(!isVersionSupported(Version::GLES300))
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        Error() << "Context: unsupported OpenGL version" << Magnum::version(_version);
        #else
        Error() << "Context: unsupported OpenGL ES version" << Magnum::version(_version);
        #endif
        std::exit(66);
    }

    /* Context flags are supported since GL 3.0 */
    #ifndef MAGNUM_TARGET_GLES
    /**
     * @todo According to KHR_debug specs this should be also present in ES2
     *      if KHR_debug is available, but in headers it is nowhere to be found
     */
    if(isVersionSupported(Version::GL300))
        glGetIntegerv(GL_CONTEXT_FLAGS, reinterpret_cast<GLint*>(&_flags));
    #endif

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
        Version::GLES200,
        Version::GLES300,
        #endif
        Version::None
    };

    /* Get first future (not supported) version */
    std::size_t future = 0;
    while(versions[future] != Version::None && isVersionSupported(versions[future]))
        ++future;

    /* Mark all extensions from past versions as supported */
    for(std::size_t i = 0; i != future; ++i)
        for(const Extension& extension: Extension::extensions(versions[i]))
            extensionStatus.set(extension._index);

    /* List of extensions from future versions (extensions from current and
       previous versions should be supported automatically, so we don't need
       to check for them) */
    std::unordered_map<std::string, Extension> futureExtensions;
    for(std::size_t i = future; i != versions.size(); ++i)
        for(const Extension& extension: Extension::extensions(versions[i]))
            #ifndef CORRADE_GCC46_COMPATIBILITY
            futureExtensions.emplace(extension._string, extension);
            #else
            futureExtensions.insert({extension._string, extension});
            #endif

    /* Check for presence of future and vendor extensions */
    const std::vector<std::string> extensions = extensionStrings();
    for(const std::string& extension: extensions) {
        const auto found = futureExtensions.find(extension);
        if(found != futureExtensions.end()) {
            _supportedExtensions.push_back(found->second);
            extensionStatus.set(found->second._index);
        }
    }

    /* Reset minimal required version to Version::None for whole array */
    for(auto& i: _extensionRequiredVersion) i = Version::None;

    /* Initialize required versions from extension info */
    for(const auto version: versions)
        for(const Extension& extension: Extension::extensions(version))
            _extensionRequiredVersion[extension._index] = extension._requiredVersion;

    /* Setup driver workarounds (increase required version for particular
       extensions), see Implementation/driverWorkarounds.cpp */
    setupDriverWorkarounds();

    /* Set this context as current */
    CORRADE_ASSERT(!_current, "Context: Another context currently active", );
    _current = this;

    /* Initialize state tracker */
    _state = new Implementation::State(*this);

    /* Initialize functionality based on current OpenGL version and extensions */
    /** @todo Get rid of these */
    DefaultFramebuffer::initializeContextBasedFunctionality(*this);
    Renderer::initializeContextBasedFunctionality();
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

std::vector<std::string> Context::extensionStrings() const {
    std::vector<std::string> extensions;

    #ifndef MAGNUM_TARGET_GLES2
    GLint extensionCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
    #ifndef MAGNUM_TARGET_GLES3
    if(extensionCount || isVersionSupported(Version::GL300))
    #endif
    {
        extensions.reserve(extensionCount);
        for(GLint i = 0; i != extensionCount; ++i)
            extensions.push_back(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));
    }
    #ifndef MAGNUM_TARGET_GLES3
    else
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES3
    /* OpenGL 2.1 / OpenGL ES 2.0 doesn't have glGetStringi() */
    {
        /* Don't crash when glGetString() returns nullptr (i.e. don't trust the
           old implementations) */
        const char* e = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
        if(e) extensions = Utility::String::splitWithoutEmptyParts(e, ' ');
    }
    #endif

    return extensions;
}

Version Context::supportedVersion(std::initializer_list<Version> versions) const {
    for(auto version: versions)
        if(isVersionSupported(version)) return version;

    #ifndef MAGNUM_TARGET_GLES
    return Version::GL210;
    #else
    return Version::GLES200;
    #endif
}

void Context::resetState(const States states) {
    if(states & State::Buffers)
        _state->buffer->reset();
    if(states & State::Framebuffers)
        _state->framebuffer->reset();
    if(states & State::Meshes)
        _state->mesh->reset();

    /* Nothing to reset for renderer yet */

    if(states & State::Shaders) {
        /* Nothing to reset for shaders */
        _state->shaderProgram->reset();
    }

    if(states & State::Textures)
        _state->texture->reset();
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, const Context::Flag value) {
    switch(value) {
        #define _c(value) case Context::Flag::value: return debug << "Context::Flag::" #value;
        _c(Debug)
        #ifndef MAGNUM_TARGET_GLES
        _c(RobustAccess)
        #endif
        #undef _c
    }

    return debug << "Context::Flag::(invalid)";
}
#endif

}
