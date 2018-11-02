/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "Magnum/GL/AbstractFramebuffer.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/AbstractTexture.h"
#include "Magnum/GL/Buffer.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/GL/BufferTexture.h"
#endif
#include "Magnum/GL/DefaultFramebuffer.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/ContextState.h"
#include "Magnum/GL/Implementation/BufferState.h"
#include "Magnum/GL/Implementation/FramebufferState.h"
#include "Magnum/GL/Implementation/MeshState.h"
#include "Magnum/GL/Implementation/RendererState.h"
#include "Magnum/GL/Implementation/ShaderProgramState.h"
#include "Magnum/GL/Implementation/TextureState.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/Implementation/TransformFeedbackState.h"
#endif

namespace Magnum { namespace GL {

const std::vector<Extension>& Extension::extensions(Version version) {
    #define _extension(vendor, extension)                           \
        {Extensions::vendor::extension::Index, Extensions::vendor::extension::requiredVersion(), Extensions::vendor::extension::coreVersion(), Extensions::vendor::extension::string()}
    static const std::vector<Extension> empty;
    #ifndef MAGNUM_TARGET_GLES
    static const std::vector<Extension> extensions{
        _extension(AMD,transform_feedback3_lines_triangles),
        _extension(AMD,vertex_shader_layer),
        _extension(AMD,shader_trinary_minmax),
        _extension(ARB,robustness),
        _extension(ARB,robustness_isolation),
        _extension(ARB,robustness_application_isolation),
        _extension(ARB,robustness_share_group_isolation),
        _extension(ARB,bindless_texture),
        _extension(ARB,compute_variable_group_size),
        _extension(ARB,seamless_cubemap_per_texture),
        _extension(ARB,sparse_texture),
        _extension(ARB,sparse_buffer),
        _extension(ARB,ES3_2_compatibility),
        _extension(ATI,texture_mirror_once),
        _extension(EXT,texture_filter_anisotropic),
        _extension(EXT,texture_compression_s3tc),
        _extension(EXT,texture_mirror_clamp),
        _extension(EXT,direct_state_access),
        _extension(EXT,texture_sRGB_decode),
        _extension(EXT,shader_integer_mix),
        _extension(EXT,debug_label),
        _extension(EXT,debug_marker),
        _extension(GREMEDY,string_marker),
        _extension(KHR,texture_compression_astc_ldr),
        _extension(KHR,texture_compression_astc_hdr),
        _extension(KHR,blend_equation_advanced),
        _extension(KHR,blend_equation_advanced_coherent)};
    static const std::vector<Extension> extensions300{
        _extension(ARB,map_buffer_range),
        _extension(ARB,color_buffer_float),
        _extension(ARB,half_float_pixel),
        _extension(ARB,texture_float),
        _extension(ARB,depth_buffer_float),
        _extension(ARB,texture_rg),
        _extension(ARB,vertex_array_object),
        _extension(ARB,framebuffer_object),
        _extension(ARB,framebuffer_sRGB),
        _extension(ARB,half_float_vertex),
        _extension(EXT,gpu_shader4),
        _extension(EXT,packed_float),
        _extension(EXT,texture_array),
        _extension(EXT,texture_compression_rgtc),
        _extension(EXT,texture_shared_exponent),
        _extension(EXT,draw_buffers2),
        _extension(EXT,texture_integer),
        _extension(EXT,transform_feedback),
        _extension(NV,depth_buffer_float),
        _extension(NV,conditional_render)};
    static const std::vector<Extension> extensions310{
        _extension(ARB,texture_rectangle),
        _extension(ARB,draw_instanced),
        _extension(ARB,texture_buffer_object),
        _extension(ARB,uniform_buffer_object),
        _extension(ARB,copy_buffer),
        _extension(EXT,texture_snorm),
        _extension(NV,primitive_restart)};
    static const std::vector<Extension> extensions320{
        _extension(ARB,geometry_shader4),
        _extension(ARB,depth_clamp),
        _extension(ARB,draw_elements_base_vertex),
        _extension(ARB,fragment_coord_conventions),
        _extension(ARB,provoking_vertex),
        _extension(ARB,seamless_cube_map),
        _extension(ARB,sync),
        _extension(ARB,texture_multisample),
        _extension(ARB,vertex_array_bgra)};
    static const std::vector<Extension> extensions330{
        _extension(ARB,instanced_arrays),
        _extension(ARB,blend_func_extended),
        _extension(ARB,explicit_attrib_location),
        _extension(ARB,occlusion_query2),
        _extension(ARB,sampler_objects),
        _extension(ARB,shader_bit_encoding),
        _extension(ARB,texture_rgb10_a2ui),
        _extension(ARB,texture_swizzle),
        _extension(ARB,timer_query),
        _extension(ARB,vertex_type_2_10_10_10_rev)};
    static const std::vector<Extension> extensions400{
        _extension(ARB,draw_buffers_blend),
        _extension(ARB,sample_shading),
        _extension(ARB,texture_cube_map_array),
        _extension(ARB,texture_gather),
        _extension(ARB,texture_query_lod),
        _extension(ARB,draw_indirect),
        _extension(ARB,gpu_shader5),
        _extension(ARB,gpu_shader_fp64),
        _extension(ARB,shader_subroutine),
        _extension(ARB,tessellation_shader),
        _extension(ARB,texture_buffer_object_rgb32),
        _extension(ARB,transform_feedback2),
        _extension(ARB,transform_feedback3)};
    static const std::vector<Extension> extensions410{
        _extension(ARB,ES2_compatibility),
        _extension(ARB,get_program_binary),
        _extension(ARB,separate_shader_objects),
        _extension(ARB,shader_precision),
        _extension(ARB,vertex_attrib_64bit),
        _extension(ARB,viewport_array)};
    static const std::vector<Extension> extensions420{
        _extension(ARB,texture_compression_bptc),
        _extension(ARB,base_instance),
        _extension(ARB,shading_language_420pack),
        _extension(ARB,transform_feedback_instanced),
        _extension(ARB,compressed_texture_pixel_storage),
        _extension(ARB,conservative_depth),
        _extension(ARB,internalformat_query),
        _extension(ARB,map_buffer_alignment),
        _extension(ARB,shader_atomic_counters),
        _extension(ARB,shader_image_load_store),
        /* Mentioned in GLSL 4.20 specs as newly added */
        _extension(ARB,shading_language_packing),
        _extension(ARB,texture_storage)};
    static const std::vector<Extension> extensions430{
        _extension(ARB,arrays_of_arrays),
        _extension(ARB,ES3_compatibility),
        _extension(ARB,clear_buffer_object),
        _extension(ARB,compute_shader),
        _extension(ARB,copy_image),
        _extension(ARB,explicit_uniform_location),
        _extension(ARB,fragment_layer_viewport),
        _extension(ARB,framebuffer_no_attachments),
        _extension(ARB,internalformat_query2),
        _extension(ARB,invalidate_subdata),
        _extension(ARB,multi_draw_indirect),
        _extension(ARB,program_interface_query),
        _extension(ARB,robust_buffer_access_behavior),
        _extension(ARB,shader_image_size),
        _extension(ARB,shader_storage_buffer_object),
        _extension(ARB,stencil_texturing),
        _extension(ARB,texture_buffer_range),
        _extension(ARB,texture_query_levels),
        _extension(ARB,texture_storage_multisample),
        _extension(ARB,texture_view),
        _extension(ARB,vertex_attrib_binding),
        _extension(KHR,debug)};
    static const std::vector<Extension> extensions440{
        _extension(ARB,buffer_storage),
        _extension(ARB,clear_texture),
        _extension(ARB,enhanced_layouts),
        _extension(ARB,multi_bind),
        _extension(ARB,query_buffer_object),
        _extension(ARB,texture_mirror_clamp_to_edge),
        _extension(ARB,texture_stencil8),
        _extension(ARB,vertex_type_10f_11f_11f_rev)};
    static const std::vector<Extension> extensions450{
        _extension(ARB,ES3_1_compatibility),
        _extension(ARB,clip_control),
        _extension(ARB,conditional_render_inverted),
        _extension(ARB,cull_distance),
        _extension(ARB,derivative_control),
        _extension(ARB,direct_state_access),
        _extension(ARB,get_texture_sub_image),
        _extension(ARB,shader_texture_image_samples),
        _extension(ARB,texture_barrier),
        _extension(KHR,context_flush_control),
        _extension(KHR,robustness)};
    static const std::vector<Extension> extensions460{
        _extension(ARB,indirect_parameters),
        _extension(ARB,shader_draw_parameters),
        _extension(ARB,shader_group_vote),
        _extension(ARB,pipeline_statistics_query),
        _extension(ARB,transform_feedback_overflow_query),
        _extension(ARB,shader_atomic_counter_ops),
        _extension(ARB,gl_spirv),
        _extension(ARB,polygon_offset_clamp),
        _extension(ARB,spirv_extensions),
        _extension(ARB,texture_filter_anisotropic),
        _extension(KHR,no_error)};
    #elif defined(MAGNUM_TARGET_WEBGL)
    static const std::vector<Extension> extensions{
        _extension(EXT,texture_filter_anisotropic),
        _extension(EXT,disjoint_timer_query),
        _extension(EXT,color_buffer_float),
        _extension(OES,texture_float_linear),
        _extension(WEBGL,compressed_texture_s3tc)};
    #ifdef MAGNUM_TARGET_GLES2
    static const std::vector<Extension> extensionsES300{
        _extension(ANGLE,instanced_arrays),
        _extension(EXT,color_buffer_half_float),
        _extension(EXT,sRGB),
        _extension(EXT,blend_minmax),
        _extension(EXT,shader_texture_lod),
        _extension(OES,texture_float),
        _extension(OES,texture_half_float),
        _extension(OES,standard_derivatives),
        _extension(OES,vertex_array_object),
        _extension(OES,element_index_uint),
        _extension(OES,texture_half_float_linear),
        _extension(OES,fbo_render_mipmap),
        _extension(WEBGL,depth_texture),
        _extension(WEBGL,color_buffer_float),
        _extension(WEBGL,draw_buffers)};
    #endif
    #else
    static const std::vector<Extension> extensions{
        #ifndef MAGNUM_TARGET_GLES2
        _extension(ANDROID,extension_pack_es31a),
        #endif
        _extension(APPLE,texture_format_BGRA8888),
        _extension(ARM,shader_framebuffer_fetch),
        _extension(ARM,shader_framebuffer_fetch_depth_stencil),
        _extension(EXT,texture_filter_anisotropic),
        _extension(EXT,texture_format_BGRA8888),
        _extension(EXT,read_format_bgra),
        _extension(EXT,multi_draw_arrays),
        _extension(EXT,debug_label),
        _extension(EXT,debug_marker),
        _extension(EXT,separate_shader_objects),
        _extension(EXT,multisampled_render_to_texture),
        _extension(EXT,robustness),
        _extension(EXT,shader_framebuffer_fetch),
        _extension(EXT,disjoint_timer_query),
        _extension(EXT,texture_sRGB_decode),
        _extension(EXT,sRGB_write_control),
        _extension(EXT,texture_compression_s3tc),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(EXT,shader_integer_mix),
        #endif
        _extension(EXT,polygon_offset_clamp),
        _extension(KHR,texture_compression_astc_hdr),
        _extension(KHR,blend_equation_advanced_coherent),
        _extension(KHR,context_flush_control),
        _extension(KHR,no_error),
        _extension(NV,read_buffer_front),
        _extension(NV,read_depth),
        _extension(NV,read_stencil),
        _extension(NV,read_depth_stencil),
        _extension(NV,texture_border_clamp),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(NV,shader_noperspective_interpolation),
        #endif
        _extension(NV,polygon_mode),
        _extension(OES,depth32),
        _extension(OES,mapbuffer),
        _extension(OES,stencil1),
        _extension(OES,stencil4),
        _extension(OES,texture_float_linear)};
    #ifdef MAGNUM_TARGET_GLES2
    static const std::vector<Extension> extensionsES300{
        _extension(ANGLE,framebuffer_blit),
        _extension(ANGLE,framebuffer_multisample),
        _extension(ANGLE,instanced_arrays),
        _extension(ANGLE,depth_texture),
        _extension(APPLE,framebuffer_multisample),
        _extension(APPLE,texture_max_level),
        _extension(ARM,rgba8),
        _extension(EXT,texture_type_2_10_10_10_REV),
        _extension(EXT,discard_framebuffer),
        _extension(EXT,blend_minmax),
        _extension(EXT,shader_texture_lod),
        _extension(EXT,unpack_subimage),
        _extension(EXT,occlusion_query_boolean),
        _extension(EXT,shadow_samplers),
        _extension(EXT,texture_rg),
        _extension(EXT,sRGB),
        _extension(EXT,texture_storage),
        _extension(EXT,map_buffer_range),
        _extension(EXT,draw_buffers),
        _extension(EXT,instanced_arrays),
        _extension(EXT,draw_instanced),
        _extension(NV,draw_buffers),
        _extension(NV,fbo_color_attachments),
        _extension(NV,read_buffer),
        _extension(NV,pack_subimage),
        _extension(NV,draw_instanced),
        _extension(NV,framebuffer_blit),
        _extension(NV,framebuffer_multisample),
        _extension(NV,instanced_arrays),
        _extension(NV,shadow_samplers_array),
        _extension(NV,shadow_samplers_cube),
        _extension(OES,depth24),
        _extension(OES,element_index_uint),
        _extension(OES,fbo_render_mipmap),
        _extension(OES,rgb8_rgba8),
        _extension(OES,texture_3D),
        _extension(OES,texture_half_float_linear),
        _extension(OES,texture_half_float),
        _extension(OES,texture_float),
        _extension(OES,texture_npot),
        _extension(OES,vertex_half_float),
        _extension(OES,packed_depth_stencil),
        _extension(OES,depth_texture),
        _extension(OES,standard_derivatives),
        _extension(OES,vertex_array_object),
        _extension(OES,required_internalformat),
        _extension(OES,surfaceless_context)};
    #endif
    static const std::vector<Extension> extensionsES320{
        _extension(EXT,color_buffer_half_float),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(EXT,color_buffer_float),
        _extension(EXT,copy_image),
        #endif
        _extension(EXT,draw_buffers_indexed),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(EXT,geometry_shader),
        _extension(EXT,gpu_shader5),
        _extension(EXT,shader_io_blocks),
        _extension(EXT,tessellation_shader),
        #endif
        _extension(EXT,texture_border_clamp),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(EXT,texture_buffer),
        _extension(EXT,texture_cube_map_array),
        _extension(EXT,primitive_bounding_box),
        #endif
        _extension(KHR,texture_compression_astc_ldr),
        _extension(KHR,debug),
        _extension(KHR,blend_equation_advanced),
        _extension(KHR,robustness),
        _extension(KHR,robust_buffer_access_behavior),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(OES,sample_shading),
        _extension(OES,sample_variables),
        _extension(OES,shader_image_atomic),
        _extension(OES,shader_multisample_interpolation),
        #endif
        _extension(OES,texture_stencil8),
        #ifndef MAGNUM_TARGET_GLES2
        _extension(OES,texture_storage_multisample_2d_array)
        #endif
        };
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
        case Version::GL460: return extensions460;
        case Version::GLES200:
        case Version::GLES300:
        case Version::GLES310:
        case Version::GLES320: return empty;
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
        case Version::GLES320: return extensionsES320;
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
    CORRADE_ASSERT(currentContext, "GL::Context::current(): no current context", *currentContext);
    return *currentContext;
}

Context::Context(NoCreateT, Int argc, const char** argv, void functionLoader()): Context{NoCreate, Utility::Arguments{"magnum"}, argc, argv, functionLoader} {}

Context::Context(NoCreateT, Utility::Arguments& args, Int argc, const char** argv, void functionLoader()): _functionLoader{functionLoader}, _version{Version::None} {
    /* Parse arguments */
    CORRADE_INTERNAL_ASSERT(args.prefix() == "magnum");
    args.addOption("disable-workarounds")
        .setHelp("disable-workarounds", "driver workarounds to disable\n      (see https://doc.magnum.graphics/magnum/opengl-workarounds.html for detailed info)", "LIST")
        .addOption("disable-extensions").setHelp("disable-extensions", "OpenGL extensions to disable", "LIST")
        .addOption("log", "default").setHelp("log", "console logging", "default|quiet|verbose")
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
        Error{} << "GL::Context: unsupported version string:" << version;
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
            "GL::Context: cannot retrieve OpenGL version:" << versionNumberError, false);
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
            Error{} << "GL::Context: unsupported version string:" << version;
            return false;
        }
    }
    #endif

    /* Compose the version enum */
    _version = GL::version(majorVersion, minorVersion);

    /* Check that version retrieval went right */
    #ifndef CORRADE_NO_ASSERT
    const auto error = Renderer::error();
    CORRADE_ASSERT(error == Renderer::Error::NoError,
        "GL::Context: cannot retrieve OpenGL version:" << error, false);
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
        Error{} << "GL::Context: unsupported OpenGL version" << std::make_pair(majorVersion, minorVersion);
        #else
        Error{} << "GL::Context: unsupported OpenGL ES version" << std::make_pair(majorVersion, minorVersion);
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
        Version::GL460,
        #else
        Version::GLES200,
        Version::GLES300,
        #ifndef MAGNUM_TARGET_WEBGL
        Version::GLES310,
        Version::GLES320,
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
            _extensionStatus.set(extension.index());

    /* List of extensions from future versions (extensions from current and
       previous versions should be supported automatically, so we don't need
       to check for them) */
    std::unordered_map<std::string, Extension> futureExtensions;
    for(std::size_t i = future; i != versions.size(); ++i)
        for(const Extension& extension: Extension::extensions(versions[i]))
            futureExtensions.emplace(extension.string(), extension);

    /* Check for presence of future and vendor extensions */
    const std::vector<std::string> extensions = extensionStrings();
    for(const std::string& extension: extensions) {
        const auto found = futureExtensions.find(extension);
        if(found != futureExtensions.end()) {
            _supportedExtensions.push_back(found->second);
            _extensionStatus.set(found->second.index());
        }
    }

    /* Reset minimal required version to Version::None for whole array */
    for(auto& i: _extensionRequiredVersion) i = Version::None;

    /* Initialize required versions from extension info */
    for(const auto version: versions)
        for(const Extension& extension: Extension::extensions(version))
            _extensionRequiredVersion[extension.index()] = extension.requiredVersion();

    /* Setup driver workarounds (increase required version for particular
       extensions), see Implementation/driverWorkarounds.cpp */
    setupDriverWorkarounds();

    /* Set this context as current */
    CORRADE_ASSERT(!currentContext, "GL::Context: Another context currently active", false);
    currentContext = this;

    /* Decide whether to print the initialization output or not */
    std::ostream* output = _displayInitializationLog ? Debug::output() : nullptr;

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
                allExtensions.emplace(extension.string(), extension);

        /* Disable extensions that are known and supported and print a message
           for each */
        for(auto&& extension: _disabledExtensions) {
            auto found = allExtensions.find(extension);
            /** @todo Error message here? I should not clutter the output at this point */
            if(found == allExtensions.end()) continue;

            _extensionRequiredVersion[found->second.index()] = Version::None;
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
        return isExtensionSupported<Extensions::ARB::ES2_compatibility>();
    if(version == Version::GLES300)
        return isExtensionSupported<Extensions::ARB::ES3_compatibility>();
    if(version == Version::GLES310)
        return isExtensionSupported<Extensions::ARB::ES3_1_compatibility>();
    if(version == Version::GLES320)
        return isExtensionSupported<Extensions::ARB::ES3_2_compatibility>();
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

    #ifndef MAGNUM_TARGET_GLES
    /* Bind a scratch VAO for external GL code that is not VAO-aware and just
       enables vertex attributes on the default VAO. Generate it on-demand as
       we don't expect this case to be used very often. */
    if(states & State::BindScratchVao) {
        if(!_state->mesh->scratchVAO)
            glGenVertexArrays(1, &_state->mesh->scratchVAO);

        _state->mesh->bindVAOImplementation(_state->mesh->scratchVAO);

    /* Otherwise just unbind the current VAO and leave the the default */
    } else
    #endif
    if(states & State::MeshVao)
        _state->mesh->bindVAOImplementation(0);

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
        #define _c(value) case Context::Flag::value: return debug << "GL::Context::Flag::" #value;
        _c(Debug)
        #ifndef MAGNUM_TARGET_GLES
        _c(ForwardCompatible)
        #endif
        _c(NoError)
        #ifndef MAGNUM_TARGET_GLES2
        _c(RobustAccess)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "GL::Context::Flag(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Context::Flags value) {
    return Containers::enumSetDebugOutput(debug, value, "GL::Context::Flags{}", {
        Context::Flag::Debug,
        #ifndef MAGNUM_TARGET_GLES
        Context::Flag::ForwardCompatible,
        #endif
        Context::Flag::NoError,
        #ifndef MAGNUM_TARGET_GLES2
        Context::Flag::RobustAccess
        #endif
    });
}
#endif

Debug& operator<<(Debug& debug, const Context::DetectedDriver value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Context::DetectedDriver::value: return debug << "GL::Context::DetectedDriver::" #value;
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

    return debug << "GL::Context::DetectedDriver(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const Context::DetectedDrivers value) {
    return Containers::enumSetDebugOutput(debug, value, "GL::Context::DetectedDrivers{}", {
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

}}
