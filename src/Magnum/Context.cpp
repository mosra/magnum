/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#ifndef MAGNUM_TARGET_GLES
#include <algorithm> /* std::find in isCoreProfileImplementationNV() */
#endif
#include <iostream> /* for initialization log redirection */
#include <string>
#include <unordered_map>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Utility/Arguments.h>
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
#include "Implementation/ContextState.h"
#include "Implementation/BufferState.h"
#include "Implementation/FramebufferState.h"
#include "Implementation/MeshState.h"
#include "Implementation/RendererState.h"
#include "Implementation/ShaderProgramState.h"
#include "Implementation/TextureState.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Implementation/TransformFeedbackState.h"
#endif

namespace Magnum {

const std::vector<Extension>& Extension::extensions(Version version) {
    #define _extension(prefix, vendor, extension)                           \
        {Extensions::prefix::vendor::extension::Index, Extensions::prefix::vendor::extension::requiredVersion(), Extensions::prefix::vendor::extension::coreVersion(), Extensions::prefix::vendor::extension::string()}
    static const std::vector<Extension> empty;
    #ifndef MAGNUM_TARGET_GLES
    static const std::vector<Extension> extensions{
        _extension(GL,AMD,transform_feedback3_lines_triangles),
        _extension(GL,AMD,vertex_shader_layer),
        _extension(GL,AMD,shader_trinary_minmax),
        _extension(GL,ARB,robustness),
        _extension(GL,ARB,robustness_isolation),
        _extension(GL,ARB,robustness_application_isolation),
        _extension(GL,ARB,robustness_share_group_isolation),
        _extension(GL,ARB,bindless_texture),
        _extension(GL,ARB,compute_variable_group_size),
        _extension(GL,ARB,indirect_parameters),
        _extension(GL,ARB,seamless_cubemap_per_texture),
        _extension(GL,ARB,shader_draw_parameters),
        _extension(GL,ARB,shader_group_vote),
        _extension(GL,ARB,sparse_texture),
        _extension(GL,ARB,pipeline_statistics_query),
        _extension(GL,ARB,sparse_buffer),
        _extension(GL,ARB,transform_feedback_overflow_query),
        _extension(GL,ATI,texture_mirror_once),
        _extension(GL,EXT,texture_filter_anisotropic),
        _extension(GL,EXT,texture_compression_s3tc),
        _extension(GL,EXT,texture_mirror_clamp),
        _extension(GL,EXT,direct_state_access),
        _extension(GL,EXT,texture_sRGB_decode),
        _extension(GL,EXT,shader_integer_mix),
        _extension(GL,EXT,debug_label),
        _extension(GL,EXT,debug_marker),
        _extension(GL,GREMEDY,string_marker),
        _extension(GL,KHR,texture_compression_astc_ldr),
        _extension(GL,KHR,texture_compression_astc_hdr),
        _extension(GL,KHR,blend_equation_advanced),
        _extension(GL,KHR,blend_equation_advanced_coherent),
        _extension(GL,KHR,no_error)};
    static const std::vector<Extension> extensions300{
        _extension(GL,ARB,map_buffer_range),
        _extension(GL,ARB,color_buffer_float),
        _extension(GL,ARB,half_float_pixel),
        _extension(GL,ARB,texture_float),
        _extension(GL,ARB,depth_buffer_float),
        _extension(GL,ARB,texture_rg),
        _extension(GL,ARB,vertex_array_object),
        _extension(GL,ARB,framebuffer_object),
        _extension(GL,ARB,framebuffer_sRGB),
        _extension(GL,ARB,half_float_vertex),
        _extension(GL,EXT,gpu_shader4),
        _extension(GL,EXT,packed_float),
        _extension(GL,EXT,texture_array),
        _extension(GL,EXT,texture_compression_rgtc),
        _extension(GL,EXT,texture_shared_exponent),
        _extension(GL,EXT,draw_buffers2),
        _extension(GL,EXT,texture_integer),
        _extension(GL,EXT,transform_feedback),
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
        _extension(GL,ARB,vertex_attrib_binding),
        _extension(GL,KHR,debug)};
    static const std::vector<Extension> extensions440{
        _extension(GL,ARB,buffer_storage),
        _extension(GL,ARB,clear_texture),
        _extension(GL,ARB,enhanced_layouts),
        _extension(GL,ARB,multi_bind),
        _extension(GL,ARB,query_buffer_object),
        _extension(GL,ARB,texture_mirror_clamp_to_edge),
        _extension(GL,ARB,texture_stencil8),
        _extension(GL,ARB,vertex_type_10f_11f_11f_rev)};
    static const std::vector<Extension> extensions450{
        _extension(GL,ARB,ES3_1_compatibility),
        _extension(GL,ARB,clip_control),
        _extension(GL,ARB,conditional_render_inverted),
        _extension(GL,ARB,cull_distance),
        _extension(GL,ARB,derivative_control),
        _extension(GL,ARB,direct_state_access),
        _extension(GL,ARB,get_texture_sub_image),
        _extension(GL,ARB,shader_texture_image_samples),
        _extension(GL,ARB,texture_barrier),
        _extension(GL,KHR,context_flush_control),
        _extension(GL,KHR,robustness)};
    #elif defined(MAGNUM_TARGET_WEBGL)
    static const std::vector<Extension> extensions{
        _extension(GL,EXT,texture_filter_anisotropic),
        _extension(GL,EXT,disjoint_timer_query),
        _extension(GL,WEBGL,compressed_texture_s3tc)};
    #ifdef MAGNUM_TARGET_GLES2
    static const std::vector<Extension> extensionsES300{
        _extension(GL,ANGLE,instanced_arrays),
        _extension(GL,EXT,sRGB),
        _extension(GL,EXT,blend_minmax),
        _extension(GL,EXT,shader_texture_lod),
        _extension(GL,OES,texture_float),
        _extension(GL,OES,texture_half_float),
        _extension(GL,OES,standard_derivatives),
        _extension(GL,OES,vertex_array_object),
        _extension(GL,OES,element_index_uint),
        _extension(GL,OES,texture_float_linear),
        _extension(GL,OES,texture_half_float_linear),
        _extension(GL,OES,fbo_render_mipmap),
        _extension(GL,WEBGL,depth_texture),
        _extension(GL,WEBGL,draw_buffers)};
    #endif
    #else
    static const std::vector<Extension> extensions{
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,ANDROID,extension_pack_es31a),
        #endif
        _extension(GL,APPLE,texture_format_BGRA8888),
        _extension(GL,ARM,shader_framebuffer_fetch),
        _extension(GL,ARM,shader_framebuffer_fetch_depth_stencil),
        _extension(GL,EXT,texture_filter_anisotropic),
        _extension(GL,EXT,texture_format_BGRA8888),
        _extension(GL,EXT,read_format_bgra),
        _extension(GL,EXT,multi_draw_arrays),
        _extension(GL,EXT,debug_label),
        _extension(GL,EXT,debug_marker),
        _extension(GL,EXT,shader_framebuffer_fetch),
        _extension(GL,EXT,disjoint_timer_query),
        _extension(GL,EXT,texture_sRGB_decode),
        _extension(GL,EXT,sRGB_write_control),
        _extension(GL,EXT,texture_compression_s3tc),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,shader_integer_mix),
        _extension(GL,EXT,copy_image),
        #endif
        _extension(GL,EXT,draw_buffers_indexed),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,geometry_shader),
        _extension(GL,EXT,gpu_shader5),
        _extension(GL,EXT,shader_io_blocks),
        _extension(GL,EXT,tessellation_shader),
        #endif
        _extension(GL,EXT,texture_border_clamp),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,texture_buffer),
        _extension(GL,EXT,texture_cube_map_array),
        _extension(GL,EXT,primitive_bounding_box),
        #endif
        _extension(GL,EXT,separate_shader_objects),
        _extension(GL,EXT,multisampled_render_to_texture),
        _extension(GL,EXT,robustness),
        _extension(GL,KHR,texture_compression_astc_ldr),
        _extension(GL,KHR,texture_compression_astc_hdr),
        _extension(GL,KHR,debug),
        _extension(GL,KHR,blend_equation_advanced),
        _extension(GL,KHR,blend_equation_advanced_coherent),
        _extension(GL,KHR,robustness),
        _extension(GL,KHR,robust_buffer_access_behavior),
        _extension(GL,KHR,context_flush_control),
        _extension(GL,KHR,no_error),
        _extension(GL,NV,read_buffer_front),
        _extension(GL,NV,read_depth),
        _extension(GL,NV,read_stencil),
        _extension(GL,NV,read_depth_stencil),
        _extension(GL,NV,texture_border_clamp),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,NV,shader_noperspective_interpolation),
        #endif
        _extension(GL,NV,polygon_mode),
        _extension(GL,OES,depth32),
        _extension(GL,OES,mapbuffer),
        _extension(GL,OES,stencil1),
        _extension(GL,OES,stencil4),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,OES,sample_shading),
        _extension(GL,OES,sample_variables),
        _extension(GL,OES,shader_image_atomic),
        _extension(GL,OES,shader_multisample_interpolation),
        #endif
        _extension(GL,OES,texture_stencil8),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,OES,texture_storage_multisample_2d_array)
        #endif
        };
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
        _extension(GL,EXT,unpack_subimage),
        _extension(GL,EXT,occlusion_query_boolean),
        _extension(GL,EXT,shadow_samplers),
        _extension(GL,EXT,texture_rg),
        _extension(GL,EXT,sRGB),
        _extension(GL,EXT,texture_storage),
        _extension(GL,EXT,map_buffer_range),
        _extension(GL,EXT,draw_buffers),
        _extension(GL,EXT,instanced_arrays),
        _extension(GL,EXT,draw_instanced),
        _extension(GL,NV,draw_buffers),
        _extension(GL,NV,fbo_color_attachments),
        _extension(GL,NV,read_buffer),
        _extension(GL,NV,pack_subimage),
        _extension(GL,NV,draw_instanced),
        _extension(GL,NV,framebuffer_blit),
        _extension(GL,NV,framebuffer_multisample),
        _extension(GL,NV,instanced_arrays),
        _extension(GL,NV,shadow_samplers_array),
        _extension(GL,NV,shadow_samplers_cube),
        _extension(GL,OES,depth24),
        _extension(GL,OES,element_index_uint),
        _extension(GL,OES,fbo_render_mipmap),
        _extension(GL,OES,rgb8_rgba8),
        _extension(GL,OES,texture_3D),
        _extension(GL,OES,texture_half_float_linear),
        _extension(GL,OES,texture_float_linear),
        _extension(GL,OES,texture_half_float),
        _extension(GL,OES,texture_float),
        _extension(GL,OES,texture_npot),
        _extension(GL,OES,vertex_half_float),
        _extension(GL,OES,packed_depth_stencil),
        _extension(GL,OES,depth_texture),
        _extension(GL,OES,standard_derivatives),
        _extension(GL,OES,vertex_array_object),
        _extension(GL,OES,required_internalformat),
        _extension(GL,OES,surfaceless_context)};
    #endif
    #endif
    #undef _extension

    switch(version) {
        case Version::None:  return extensions;
        #ifndef MAGNUM_TARGET_GLES
        case Version::GL210: return empty;
        case Version::GL300: return extensions300;
        case Version::GL310: return extensions310;
        case Version::GL320: return extensions320;
        case Version::GL330: return extensions330;
        case Version::GL400: return extensions400;
        case Version::GL410: return extensions410;
        case Version::GL420: return extensions420;
        case Version::GL430: return extensions430;
        case Version::GL440: return extensions440;
        case Version::GL450: return extensions450;
        case Version::GLES200:
        case Version::GLES300:
        case Version::GLES310: return empty;
        #else
        case Version::GLES200: return empty;
        case Version::GLES300:
            #ifdef MAGNUM_TARGET_GLES2
            return extensionsES300;
            #else
            return empty;
            #endif
        #ifndef MAGNUM_TARGET_WEBGL
        case Version::GLES310: return empty;
        #endif
        #endif
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

namespace {
    #ifdef MAGNUM_BUILD_MULTITHREADED
    #if !defined(CORRADE_GCC47_COMPATIBILITY) && !defined(CORRADE_TARGET_APPLE)
    thread_local
    #else
    __thread
    #endif
    #endif
    Context* currentContext = nullptr;
}

bool Context::hasCurrent() { return currentContext; }

Context& Context::current() {
    CORRADE_ASSERT(currentContext, "Context::current(): no current context", *currentContext);
    return *currentContext;
}

Context::Context(NoCreateT, Int argc, const char** argv, void functionLoader()): _functionLoader{functionLoader}, _version{Version::None} {
    /* Parse arguments */
    Utility::Arguments args{"magnum"};
    args.addOption("disable-workarounds")
        .setHelp("disable-workarounds", "driver workarounds to disable\n      (see src/Magnum/Implementation/driverSpecific.cpp for detailed info)", "LIST")
        .addOption("disable-extensions").setHelp("disable-extensions", "OpenGL extensions to disable", "LIST")
        .addOption("log", "default").setHelp("log", "Console logging", "default|quiet")
        .setFromEnvironment("disable-workarounds")
        .setFromEnvironment("disable-extensions")
        .setFromEnvironment("log")
        .parse(argc, argv);

    /* Decide whether to display initialization log */
    _displayInitializationLog = !(args.value("log") == "quiet" || args.value("log") == "QUIET");

    /* Disable driver workarounds */
    for(auto&& workaround: Utility::String::splitWithoutEmptyParts(args.value("disable-workarounds")))
        disableDriverWorkaround(workaround);

    /* Disable extensions */
    for(auto&& extension: Utility::String::splitWithoutEmptyParts(args.value("disable-extensions")))
        _disabledExtensions.push_back(extension);
}

Context::Context(Context&& other): _version{other._version},
    #ifndef MAGNUM_TARGET_WEBGL
    _flags{other._flags},
    #endif
    _extensionRequiredVersion(other._extensionRequiredVersion),
    _extensionStatus{other._extensionStatus},
    _supportedExtensions{std::move(other._supportedExtensions)},
    _state{other._state},
    _detectedDrivers{std::move(other._detectedDrivers)}
{
    other._state = nullptr;
    if(currentContext == &other) currentContext = this;
}

Context::~Context() {
    delete _state;

    if(currentContext == this) currentContext = nullptr;
}

void Context::create() {
    /* Hard exit if the context cannot be created */
    if(!tryCreate()) std::exit(1);
}

bool Context::tryCreate() {
    CORRADE_ASSERT(_version == Version::None,
        "Platform::Context::tryCreate(): context already created", false);

    /* Load GL function pointers */
    if(_functionLoader) _functionLoader();

    /* Initialize to something predictable to avoid crashes on improperly
       created contexts */
    GLint majorVersion = 0, minorVersion = 0;

    /* Get version on ES 3.0+/WebGL 2.0+ */
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_GLES2)

    /* ES 3.0+ */
    #ifndef MAGNUM_TARGET_WEBGL
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

    /* WebGL 2.0, treat it as ES 3.0 */
    #else
    const std::string version = versionString();
    if(version.find("WebGL 2") == std::string::npos) {
        Error() << "Context: unsupported version string:" << version;
        return false;
    }
    majorVersion = 3;
    minorVersion = 0;
    #endif

    /* On GL 2.1 and ES 2.0 there is no GL_{MAJOR,MINOR}_VERSION, we have to
       parse version string. On desktop GL we have no way to check version
       without version (duh) so we work around that by checking for invalid
       enum error. */
    #else
    #ifndef MAGNUM_TARGET_GLES2
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    const auto versionNumberError = Renderer::error();
    if(versionNumberError == Renderer::Error::NoError)
        glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    else
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES2
        CORRADE_ASSERT(versionNumberError == Renderer::Error::InvalidEnum,
            "Context: cannot retrieve OpenGL version:" << versionNumberError, false);
        #endif

        /* Allow ES2 context on driver that reports ES3 as supported */
        const std::string version = versionString();
        #ifndef MAGNUM_TARGET_GLES
        if(version.compare(0, 3, "2.1") == 0)
        #elif defined(MAGNUM_TARGET_WEBGL)
        /* Internet Explorer currently has 0.94 */
        if(version.find("WebGL 1") != std::string::npos ||
           version.find("WebGL 0") != std::string::npos)
        #else
        if(version.find("OpenGL ES 2.0") != std::string::npos ||
           /* It is possible to use Magnum compiled for ES2 on ES3 contexts */
           version.find("OpenGL ES 3.") != std::string::npos)
        #endif
        {
            majorVersion = 2;
            #ifndef MAGNUM_TARGET_GLES
            minorVersion = 1;
            #else
            minorVersion = 0;
            #endif
        } else {
            Error() << "Context: unsupported version string:" << version;
            return false;
        }
    }
    #endif

    /* Compose the version enum */
    _version = Magnum::version(majorVersion, minorVersion);

    /* Check that version retrieval went right */
    #ifndef CORRADE_NO_ASSERT
    const auto error = Renderer::error();
    CORRADE_ASSERT(error == Renderer::Error::NoError,
        "Context: cannot retrieve OpenGL version:" << error, false);
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
        Error() << "Context: unsupported OpenGL version" << std::make_pair(majorVersion, minorVersion);
        #else
        Error() << "Context: unsupported OpenGL ES version" << std::make_pair(majorVersion, minorVersion);
        #endif

        /* Reset the version so the context is not marked as successfully created */
        _version = Version::None;
        return false;
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
        Version::GL450,
        #else
        Version::GLES200,
        Version::GLES300,
        #ifndef MAGNUM_TARGET_WEBGL
        Version::GLES310,
        #endif
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
            _extensionStatus.set(extension._index);

    /* List of extensions from future versions (extensions from current and
       previous versions should be supported automatically, so we don't need
       to check for them) */
    std::unordered_map<std::string, Extension> futureExtensions;
    for(std::size_t i = future; i != versions.size(); ++i)
        for(const Extension& extension: Extension::extensions(versions[i]))
            futureExtensions.emplace(extension._string, extension);

    /* Check for presence of future and vendor extensions */
    const std::vector<std::string> extensions = extensionStrings();
    for(const std::string& extension: extensions) {
        const auto found = futureExtensions.find(extension);
        if(found != futureExtensions.end()) {
            _supportedExtensions.push_back(found->second);
            _extensionStatus.set(found->second._index);
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
    CORRADE_ASSERT(!currentContext, "Context: Another context currently active", false);
    currentContext = this;

    /* Decide whether to print the initialization output or not */
    std::ostream* output = _displayInitializationLog ? &std::cout : nullptr;

    /* Print some info and initialize state tracker (which also prints some
       more info) */
    Debug{output} << "Renderer:" << rendererString() << "by" << vendorString();
    Debug{output} << "OpenGL version:" << versionString();

    /* Disable extensions as requested by the user */
    if(!_disabledExtensions.empty()) {
        Debug{output} << "Disabling extensions:";

        /* Put remaining extensions into the hashmap for faster lookup */
        std::unordered_map<std::string, Extension> allExtensions{std::move(futureExtensions)};
        for(std::size_t i = 0; i != future; ++i)
            for(const Extension& extension: Extension::extensions(versions[i]))
                allExtensions.emplace(extension._string, extension);

        /* Disable extensions that are known and supported and print a message
           for each */
        for(auto&& extension: _disabledExtensions) {
            auto found = allExtensions.find(extension);
            /** @todo Error message here? I should not clutter the output at this point */
            if(found == allExtensions.end()) continue;

            _extensionRequiredVersion[found->second._index] = Version::None;
            Debug{output} << "   " << extension;
        }
    }

    _state = new Implementation::State{*this, output};

    /* Print a list of used workarounds */
    if(!_driverWorkarounds.empty()) {
        Debug{output} << "Using driver workarounds:";
        for(const auto& workaround: _driverWorkarounds)
            if(!workaround.second) Debug(output) << "   " << workaround.first;
    }

    /* Initialize functionality based on current OpenGL version and extensions */
    /** @todo Get rid of these */
    DefaultFramebuffer::initializeContextBasedFunctionality(*this);
    Renderer::initializeContextBasedFunctionality();

    /* Everything okay */
    return true;
}

std::string Context::vendorString() const {
    return Utility::String::fromArray(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
}

std::string Context::rendererString() const {
    return Utility::String::fromArray(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
}

std::string Context::versionString() const {
    return Utility::String::fromArray(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

std::string Context::shadingLanguageVersionString() const {
    return Utility::String::fromArray(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
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
        versions.emplace_back(reinterpret_cast<const char*>(glGetStringi(GL_SHADING_LANGUAGE_VERSION, i)));
    return versions;
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
            extensions.emplace_back(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));
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

#ifndef MAGNUM_TARGET_GLES
bool Context::isCoreProfile() {
    return isCoreProfileInternal(*_state->context);
}

bool Context::isCoreProfileInternal(Implementation::ContextState& state) {
    Implementation::ContextState::CoreProfile& value = state.coreProfile;

    if(value == Implementation::ContextState::CoreProfile::Initial)
        value = (this->*state.isCoreProfileImplementation)() ?
            Implementation::ContextState::CoreProfile::Core :
            Implementation::ContextState::CoreProfile::Compatibility;

    return value == Implementation::ContextState::CoreProfile::Core;
}

bool Context::isCoreProfileImplementationDefault() {
    GLint value = 0;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &value);
    return value & GL_CONTEXT_CORE_PROFILE_BIT;
}

bool Context::isCoreProfileImplementationNV() {
    auto extensions = extensionStrings();
    return std::find(extensions.begin(), extensions.end(), "GL_ARB_compatibility") == extensions.end();
}
#endif

bool Context::isVersionSupported(Version version) const {
    #ifndef MAGNUM_TARGET_GLES
    if(version == Version::GLES200)
        return isExtensionSupported<Extensions::GL::ARB::ES2_compatibility>();
    if(version == Version::GLES300)
        return isExtensionSupported<Extensions::GL::ARB::ES3_compatibility>();
    if(version == Version::GLES310)
        return isExtensionSupported<Extensions::GL::ARB::ES3_1_compatibility>();
    #endif

    return _version >= version;
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

    if(states & State::PixelStorage) {
        _state->renderer->unpackPixelStorage.reset();
        _state->renderer->packPixelStorage.reset();
    }

    /* Nothing to reset for renderer yet */

    if(states & State::Shaders) {
        /* Nothing to reset for shaders */
        _state->shaderProgram->reset();
    }

    if(states & State::Textures)
        _state->texture->reset();
    #ifndef MAGNUM_TARGET_GLES2
    if(states & State::TransformFeedback)
        _state->transformFeedback->reset();
    #endif
}

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_WEBGL
Debug& operator<<(Debug& debug, const Context::Flag value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Context::Flag::value: return debug << "Context::Flag::" #value;
        _c(Debug)
        _c(NoError)
        #ifndef MAGNUM_TARGET_GLES
        _c(RobustAccess)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Context::Flag(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Context::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "Context::Flags{}", {
        Context::Flag::Debug,
        Context::Flag::NoError,
        #ifndef MAGNUM_TARGET_GLES
        Context::Flag::RobustAccess
        #endif
    });
}
#endif

Debug& operator<<(Debug& debug, const Context::DetectedDriver value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Context::DetectedDriver::value: return debug << "Context::DetectedDriver::" #value;
        #ifndef MAGNUM_TARGET_WEBGL
        _c(Amd)
        #endif
        #ifdef MAGNUM_TARGET_GLES
        _c(Angle)
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        _c(IntelWindows)
        _c(Mesa)
        _c(NVidia)
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        _c(Svga3D)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Context::DetectedDriver(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Context::DetectedDrivers value) {
    return Containers::enumSetDebugOutput(debug, value, "Context::DetectedDrivers{}", {
        #ifndef MAGNUM_TARGET_WEBGL
        Context::DetectedDriver::Amd,
        #endif
        #ifdef MAGNUM_TARGET_GLES
        Context::DetectedDriver::Angle,
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        Context::DetectedDriver::IntelWindows,
        Context::DetectedDriver::Mesa,
        Context::DetectedDriver::NVidia,
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        Context::DetectedDriver::Svga3D
        #endif
    });
}
#endif

}
