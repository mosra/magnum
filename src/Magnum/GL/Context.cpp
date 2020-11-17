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

#include "Context.h"

#include <algorithm>
#include <iostream> /* for initialization log redirection */
#include <string>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/GL/AbstractFramebuffer.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/AbstractTexture.h"
#include "Magnum/GL/Buffer.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/GL/BufferTexture.h"
#endif
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/DebugOutput.h"
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

#if defined(CORRADE_TARGET_WINDOWS) && defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) && !defined(CORRADE_TARGET_WINDOWS_RT)
#include "Magnum/Implementation/WindowsWeakSymbol.h"
#endif

namespace Magnum { namespace GL {

/* When adding a new list, Extension::extensions() and Context::Context() needs
   to be adapted. Binary search is performed on the extensions, thus they have
   to be sorted alphabetically. */
namespace {

#define _extension(vendor, extension)                           \
    {Extensions::vendor::extension::Index, Extensions::vendor::extension::requiredVersion(), Extensions::vendor::extension::coreVersion(), Extensions::vendor::extension::string()}
#ifndef MAGNUM_TARGET_GLES
constexpr Extension ExtensionList[]{
    _extension(AMD,sample_positions),
    _extension(AMD,shader_explicit_vertex_parameter),
    _extension(AMD,shader_trinary_minmax),
    _extension(AMD,transform_feedback3_lines_triangles),
    _extension(AMD,vertex_shader_layer),
    _extension(ARB,ES3_2_compatibility),
    _extension(ARB,bindless_texture),
    _extension(ARB,compute_variable_group_size),
    _extension(ARB,robustness),
    _extension(ARB,robustness_application_isolation),
    _extension(ARB,robustness_isolation),
    _extension(ARB,robustness_share_group_isolation),
    _extension(ARB,sample_locations),
    _extension(ARB,seamless_cubemap_per_texture),
    _extension(ARB,sparse_buffer),
    _extension(ARB,sparse_texture),
    _extension(ATI,texture_mirror_once),
    _extension(EXT,debug_label),
    _extension(EXT,debug_marker),
    _extension(EXT,shader_integer_mix),
    _extension(EXT,texture_compression_dxt1),
    _extension(EXT,texture_compression_s3tc),
    _extension(EXT,texture_filter_anisotropic),
    _extension(EXT,texture_mirror_clamp),
    _extension(EXT,texture_sRGB_R8),
    _extension(EXT,texture_sRGB_decode),
    _extension(GREMEDY,string_marker),
    _extension(KHR,blend_equation_advanced),
    _extension(KHR,blend_equation_advanced_coherent),
    _extension(KHR,texture_compression_astc_hdr),
    _extension(KHR,texture_compression_astc_ldr),
    _extension(KHR,texture_compression_astc_sliced_3d),
    _extension(NV,fragment_shader_barycentric),
    _extension(NV,sample_locations),
    _extension(OVR,multiview),
    _extension(OVR,multiview2)};
constexpr Extension ExtensionList300[]{
    _extension(ARB,color_buffer_float),
    _extension(ARB,depth_buffer_float),
    _extension(ARB,framebuffer_object),
    _extension(ARB,framebuffer_sRGB),
    _extension(ARB,half_float_pixel),
    _extension(ARB,half_float_vertex),
    _extension(ARB,map_buffer_range),
    _extension(ARB,texture_float),
    _extension(ARB,texture_rg),
    _extension(ARB,vertex_array_object),
    _extension(EXT,draw_buffers2),
    _extension(EXT,gpu_shader4),
    _extension(EXT,packed_float),
    _extension(EXT,texture_array),
    _extension(EXT,texture_compression_rgtc),
    _extension(EXT,texture_integer),
    _extension(EXT,texture_shared_exponent),
    _extension(EXT,transform_feedback),
    _extension(MAGNUM,shader_vertex_id),
    _extension(NV,conditional_render),
    _extension(NV,depth_buffer_float)};
constexpr Extension ExtensionList310[]{
    _extension(ARB,copy_buffer),
    _extension(ARB,draw_instanced),
    _extension(ARB,texture_buffer_object),
    _extension(ARB,texture_rectangle),
    _extension(ARB,uniform_buffer_object),
    _extension(EXT,texture_snorm),
    _extension(NV,primitive_restart)};
constexpr Extension ExtensionList320[]{
    _extension(ARB,depth_clamp),
    _extension(ARB,draw_elements_base_vertex),
    _extension(ARB,fragment_coord_conventions),
    _extension(ARB,geometry_shader4),
    _extension(ARB,provoking_vertex),
    _extension(ARB,seamless_cube_map),
    _extension(ARB,sync),
    _extension(ARB,texture_multisample),
    _extension(ARB,vertex_array_bgra)};
constexpr Extension ExtensionList330[]{
    _extension(ARB,blend_func_extended),
    _extension(ARB,explicit_attrib_location),
    _extension(ARB,instanced_arrays),
    _extension(ARB,occlusion_query2),
    _extension(ARB,sampler_objects),
    _extension(ARB,shader_bit_encoding),
    _extension(ARB,texture_rgb10_a2ui),
    _extension(ARB,texture_swizzle),
    _extension(ARB,timer_query),
    _extension(ARB,vertex_type_2_10_10_10_rev)};
constexpr Extension ExtensionList400[]{
    _extension(ARB,draw_buffers_blend),
    _extension(ARB,draw_indirect),
    _extension(ARB,gpu_shader5),
    _extension(ARB,gpu_shader_fp64),
    _extension(ARB,sample_shading),
    _extension(ARB,shader_subroutine),
    _extension(ARB,tessellation_shader),
    _extension(ARB,texture_buffer_object_rgb32),
    _extension(ARB,texture_cube_map_array),
    _extension(ARB,texture_gather),
    _extension(ARB,texture_query_lod),
    _extension(ARB,transform_feedback2),
    _extension(ARB,transform_feedback3)};
constexpr Extension ExtensionList410[]{
    _extension(ARB,ES2_compatibility),
    _extension(ARB,get_program_binary),
    _extension(ARB,separate_shader_objects),
    _extension(ARB,shader_precision),
    _extension(ARB,vertex_attrib_64bit),
    _extension(ARB,viewport_array)};
constexpr Extension ExtensionList420[]{
    _extension(ARB,base_instance),
    _extension(ARB,compressed_texture_pixel_storage),
    _extension(ARB,conservative_depth),
    _extension(ARB,internalformat_query),
    _extension(ARB,map_buffer_alignment),
    _extension(ARB,shader_atomic_counters),
    _extension(ARB,shader_image_load_store),
    _extension(ARB,shading_language_420pack),
    /* Mentioned in GLSL 4.20 specs as newly added */
    _extension(ARB,shading_language_packing),
    _extension(ARB,texture_compression_bptc),
    _extension(ARB,texture_storage),
    _extension(ARB,transform_feedback_instanced)};
constexpr Extension ExtensionList430[]{
    _extension(ARB,ES3_compatibility),
    _extension(ARB,arrays_of_arrays),
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
constexpr Extension ExtensionList440[]{
    _extension(ARB,buffer_storage),
    _extension(ARB,clear_texture),
    _extension(ARB,enhanced_layouts),
    _extension(ARB,multi_bind),
    _extension(ARB,query_buffer_object),
    _extension(ARB,texture_mirror_clamp_to_edge),
    _extension(ARB,texture_stencil8),
    _extension(ARB,vertex_type_10f_11f_11f_rev)};
constexpr Extension ExtensionList450[]{
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
constexpr Extension ExtensionList460[]{
    _extension(ARB,gl_spirv),
    _extension(ARB,indirect_parameters),
    _extension(ARB,pipeline_statistics_query),
    _extension(ARB,polygon_offset_clamp),
    _extension(ARB,shader_atomic_counter_ops),
    _extension(ARB,shader_draw_parameters),
    _extension(ARB,shader_group_vote),
    _extension(ARB,spirv_extensions),
    _extension(ARB,texture_filter_anisotropic),
    _extension(ARB,transform_feedback_overflow_query),
    _extension(KHR,no_error)};
#elif defined(MAGNUM_TARGET_WEBGL)
constexpr Extension ExtensionList[]{
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,clip_cull_distance),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,color_buffer_float),
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    _extension(EXT,disjoint_timer_query),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,disjoint_timer_query_webgl2),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,draw_buffers_indexed),
    #endif
    _extension(EXT,float_blend),
    _extension(EXT,texture_compression_bptc),
    _extension(EXT,texture_compression_rgtc),
    _extension(EXT,texture_filter_anisotropic),
    _extension(EXT,texture_norm16),
    _extension(OES,texture_float_linear),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(OVR,multiview2),
    #endif
    _extension(WEBGL,blend_equation_advanced_coherent),
    _extension(WEBGL,compressed_texture_astc),
    _extension(WEBGL,compressed_texture_pvrtc),
    _extension(WEBGL,compressed_texture_s3tc),
    _extension(WEBGL,compressed_texture_s3tc_srgb),
    _extension(WEBGL,debug_renderer_info),
    _extension(WEBGL,debug_shaders),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(WEBGL,draw_instanced_base_vertex_base_instance),
    #endif
    _extension(WEBGL,lose_context),
    _extension(WEBGL,multi_draw),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(WEBGL,multi_draw_instanced_base_vertex_base_instance)
    #endif
};
constexpr Extension ExtensionListES300[]{
    #ifdef MAGNUM_TARGET_GLES2
    _extension(ANGLE,instanced_arrays),
    _extension(EXT,blend_minmax),
    _extension(EXT,color_buffer_half_float),
    _extension(EXT,frag_depth),
    _extension(EXT,sRGB),
    _extension(EXT,shader_texture_lod),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(MAGNUM,shader_vertex_id),
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    _extension(OES,element_index_uint),
    _extension(OES,fbo_render_mipmap),
    _extension(OES,standard_derivatives),
    _extension(OES,texture_float),
    _extension(OES,texture_half_float),
    _extension(OES,texture_half_float_linear),
    _extension(OES,vertex_array_object),
    _extension(WEBGL,color_buffer_float),
    _extension(WEBGL,depth_texture),
    _extension(WEBGL,draw_buffers)
    #endif
};
#else
constexpr Extension ExtensionList[]{
    #ifndef MAGNUM_TARGET_GLES2
    _extension(ANDROID,extension_pack_es31a),
    #endif
    _extension(ANGLE,texture_compression_dxt1),
    _extension(ANGLE,texture_compression_dxt3),
    _extension(ANGLE,texture_compression_dxt5),
    _extension(APPLE,clip_distance),
    _extension(APPLE,texture_format_BGRA8888),
    _extension(ARM,shader_framebuffer_fetch),
    _extension(ARM,shader_framebuffer_fetch_depth_stencil),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,clip_cull_distance),
    #endif
    _extension(EXT,debug_label),
    _extension(EXT,debug_marker),
    _extension(EXT,disjoint_timer_query),
    _extension(EXT,multi_draw_arrays),
    _extension(EXT,multisampled_render_to_texture),
    _extension(EXT,polygon_offset_clamp),
    _extension(EXT,pvrtc_sRGB),
    _extension(EXT,read_format_bgra),
    _extension(EXT,robustness),
    _extension(EXT,sRGB_write_control),
    _extension(EXT,separate_shader_objects),
    _extension(EXT,shader_framebuffer_fetch),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,shader_integer_mix),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,texture_compression_bptc),
    #endif
    _extension(EXT,texture_compression_dxt1),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,texture_compression_rgtc),
    #endif
    _extension(EXT,texture_compression_s3tc),
    _extension(EXT,texture_compression_s3tc_srgb),
    _extension(EXT,texture_filter_anisotropic),
    _extension(EXT,texture_format_BGRA8888),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,texture_norm16),
    _extension(EXT,texture_sRGB_R8),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,texture_sRGB_RG8),
    #endif
    _extension(EXT,texture_sRGB_decode),
    _extension(IMG,texture_compression_pvrtc),
    _extension(KHR,blend_equation_advanced_coherent),
    _extension(KHR,context_flush_control),
    _extension(KHR,no_error),
    _extension(KHR,texture_compression_astc_hdr),
    _extension(KHR,texture_compression_astc_sliced_3d),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(NV,fragment_shader_barycentric),
    #endif
    _extension(NV,polygon_mode),
    _extension(NV,read_buffer_front),
    _extension(NV,read_depth),
    _extension(NV,read_depth_stencil),
    _extension(NV,read_stencil),
    _extension(NV,sample_locations),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(NV,shader_noperspective_interpolation),
    #endif
    _extension(NV,texture_border_clamp),
    _extension(OES,depth32),
    _extension(OES,mapbuffer),
    _extension(OES,stencil1),
    _extension(OES,stencil4),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(OES,texture_compression_astc),
    #endif
    _extension(OES,texture_float_linear),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(OVR,multiview),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(OVR,multiview2),
    #endif
    };
constexpr Extension ExtensionListES300[]{
    #ifdef MAGNUM_TARGET_GLES2
    _extension(ANGLE,depth_texture),
    _extension(ANGLE,framebuffer_blit),
    _extension(ANGLE,framebuffer_multisample),
    _extension(ANGLE,instanced_arrays),
    _extension(APPLE,framebuffer_multisample),
    _extension(APPLE,texture_max_level),
    _extension(ARM,rgba8),
    _extension(EXT,blend_minmax),
    _extension(EXT,discard_framebuffer),
    _extension(EXT,draw_buffers),
    _extension(EXT,draw_instanced),
    _extension(EXT,instanced_arrays),
    _extension(EXT,map_buffer_range),
    _extension(EXT,occlusion_query_boolean),
    _extension(EXT,sRGB),
    _extension(EXT,shader_texture_lod),
    _extension(EXT,shadow_samplers),
    _extension(EXT,texture_rg),
    _extension(EXT,texture_storage),
    _extension(EXT,texture_type_2_10_10_10_REV),
    _extension(EXT,unpack_subimage),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(MAGNUM,shader_vertex_id),
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    _extension(NV,draw_buffers),
    _extension(NV,draw_instanced),
    _extension(NV,fbo_color_attachments),
    _extension(NV,framebuffer_blit),
    _extension(NV,framebuffer_multisample),
    _extension(NV,instanced_arrays),
    _extension(NV,pack_subimage),
    _extension(NV,read_buffer),
    _extension(NV,shadow_samplers_array),
    _extension(NV,shadow_samplers_cube),
    _extension(OES,depth24),
    _extension(OES,depth_texture),
    _extension(OES,element_index_uint),
    _extension(OES,fbo_render_mipmap),
    _extension(OES,packed_depth_stencil),
    _extension(OES,required_internalformat),
    _extension(OES,rgb8_rgba8),
    _extension(OES,standard_derivatives),
    _extension(OES,surfaceless_context),
    _extension(OES,texture_3D),
    _extension(OES,texture_float),
    _extension(OES,texture_half_float),
    _extension(OES,texture_half_float_linear),
    _extension(OES,texture_npot),
    _extension(OES,vertex_array_object),
    _extension(OES,vertex_half_float),
    #endif
    };
constexpr Extension ExtensionListES320[]{
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,color_buffer_float),
    #endif
    _extension(EXT,color_buffer_half_float),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,copy_image),
    #endif
    _extension(EXT,draw_buffers_indexed),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,geometry_shader),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,gpu_shader5),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,primitive_bounding_box),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,shader_io_blocks),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,tessellation_shader),
    #endif
    _extension(EXT,texture_border_clamp),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,texture_buffer),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(EXT,texture_cube_map_array),
    #endif
    _extension(KHR,blend_equation_advanced),
    _extension(KHR,debug),
    _extension(KHR,robust_buffer_access_behavior),
    _extension(KHR,robustness),
    _extension(KHR,texture_compression_astc_ldr),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(OES,sample_shading),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(OES,sample_variables),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(OES,shader_image_atomic),
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(OES,shader_multisample_interpolation),
    #endif
    _extension(OES,texture_stencil8),
    #ifndef MAGNUM_TARGET_GLES2
    _extension(OES,texture_storage_multisample_2d_array)
    #endif
    };
#endif
#undef _extension

}

Containers::ArrayView<const Extension> Extension::extensions(Version version) {
    switch(version) {
        /* GCC 4.8 needs an explicit cast */
        case Version::None:  return Containers::arrayView(ExtensionList);
        #ifndef MAGNUM_TARGET_GLES
        case Version::GL210: return {};
        case Version::GL300: return Containers::arrayView(ExtensionList300);
        case Version::GL310: return Containers::arrayView(ExtensionList310);
        case Version::GL320: return Containers::arrayView(ExtensionList320);
        case Version::GL330: return Containers::arrayView(ExtensionList330);
        case Version::GL400: return Containers::arrayView(ExtensionList400);
        case Version::GL410: return Containers::arrayView(ExtensionList410);
        case Version::GL420: return Containers::arrayView(ExtensionList420);
        case Version::GL430: return Containers::arrayView(ExtensionList430);
        case Version::GL440: return Containers::arrayView(ExtensionList440);
        case Version::GL450: return Containers::arrayView(ExtensionList450);
        case Version::GL460: return Containers::arrayView(ExtensionList460);
        case Version::GLES200:
        case Version::GLES300:
        case Version::GLES310:
        case Version::GLES320: return {};
        #else
        case Version::GLES200: return {};
        case Version::GLES300: return Containers::arrayView(ExtensionListES300);
        #ifndef MAGNUM_TARGET_WEBGL
        case Version::GLES310: return {};
        case Version::GLES320: return Containers::arrayView(ExtensionListES320);
        #endif
        #endif
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

#if !defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) || defined(CORRADE_TARGET_WINDOWS)
/* (Of course) can't be in an unnamed namespace in order to export it below
   (except for Windows, where we do extern "C" so this doesn't matter) */
namespace {
#endif

#ifdef CORRADE_BUILD_MULTITHREADED
CORRADE_THREAD_LOCAL
#endif
#if defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) && !defined(CORRADE_TARGET_WINDOWS)
/* On static builds that get linked to multiple shared libraries and then used
   in a single app we want to ensure there's just one global symbol. On Linux
   it's apparently enough to just export, macOS needs the weak attribute.
   Windows handled differently below. */
CORRADE_VISIBILITY_EXPORT
    #ifdef __GNUC__
    __attribute__((weak))
    #else
    /* uh oh? the test will fail, probably */
    #endif
#endif
Context* currentContext = nullptr;

#if !defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) || defined(CORRADE_TARGET_WINDOWS)
}
#endif

/* Windows can't have a symbol both thread-local and exported, moreover there
   isn't any concept of weak symbols. Exporting thread-local symbols can be
   worked around by exporting a function that then returns a reference to a
   non-exported thread-local symbol; and finally GetProcAddress() on
   GetModuleHandle(nullptr) "emulates" the weak linking as it's guaranteed to
   pick up the same symbol of the final exe independently of the DLL it was
   called from. To avoid #ifdef hell in code below, the currentContext is
   redefined to return a value from this uniqueness-ensuring function. */
#if defined(CORRADE_TARGET_WINDOWS) && defined(MAGNUM_BUILD_STATIC_UNIQUE_GLOBALS) && !defined(CORRADE_TARGET_WINDOWS_RT)
/* Clang-CL complains that the function has a return type incompatible with C.
   I don't care, I only need an unmangled name to look up later at runtime. */
#ifdef CORRADE_TARGET_CLANG_CL
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif
extern "C" CORRADE_VISIBILITY_EXPORT Context*& magnumGLUniqueCurrentContext();
extern "C" CORRADE_VISIBILITY_EXPORT Context*& magnumGLUniqueCurrentContext() {
    return currentContext;
}
#ifdef CORRADE_TARGET_CLANG_CL
#pragma clang diagnostic pop
#endif

namespace {

Context*& windowsCurrentContext() {
    /* A function-local static to ensure it's only initialized once without any
       race conditions among threads */
    static Context*&(*const uniqueGlobals)() = reinterpret_cast<Context*&(*)()>(Magnum::Implementation::windowsWeakSymbol("magnumGLUniqueCurrentContext", reinterpret_cast<void*>(magnumGLUniqueCurrentContext)));
    return uniqueGlobals();
}

}

#define currentContext windowsCurrentContext()
#endif

bool Context::hasCurrent() { return currentContext; }

Context& Context::current() {
    CORRADE_ASSERT(currentContext, "GL::Context::current(): no current context", *currentContext);
    return *currentContext;
}

void Context::makeCurrent(Context* context) { currentContext = context; }

Context::Context(NoCreateT, Int argc, const char** argv, void functionLoader(Context&)): Context{NoCreate, Utility::Arguments{"magnum"}, argc, argv, functionLoader} {}

Context::Context(NoCreateT, Utility::Arguments& args, Int argc, const char** argv, void functionLoader(Context&)): _functionLoader{functionLoader}, _version{Version::None} {
    /* Parse arguments */
    CORRADE_INTERNAL_ASSERT(args.prefix() == "magnum");
    args.addOption("disable-workarounds").setHelp("disable-workarounds", "driver workarounds to disable\n      (see https://doc.magnum.graphics/magnum/opengl-workarounds.html for detailed info)", "LIST")
        .addOption("disable-extensions").setHelp("disable-extensions", "API extensions to disable", "LIST")
        .addOption("gpu-validation", "off").setHelp("gpu-validation", "GPU validation using KHR_debug (if present)", "off|on")
        .addOption("log", "default").setHelp("log", "console logging", "default|quiet|verbose")
        .setFromEnvironment("disable-workarounds")
        .setFromEnvironment("disable-extensions")
        .setFromEnvironment("gpu-validation")
        .setFromEnvironment("log")
        .parse(argc, argv);

    /* Decide how to display initialization log */
    if(args.value("log") == "verbose" || args.value("log") == "VERBOSE")
        _internalFlags |= InternalFlag::DisplayVerboseInitializationLog;
    else if(!(args.value("log") == "quiet" || args.value("log") == "QUIET"))
        _internalFlags |= InternalFlag::DisplayInitializationLog;

    /* Decide whether to enable GPU validation */
    if(args.value("gpu-validation") == "on" || args.value("gpu-validation") == "ON")
        _internalFlags |= InternalFlag::GpuValidation;

    /* Disable driver workarounds */
    for(auto&& workaround: Utility::String::splitWithoutEmptyParts(args.value("disable-workarounds")))
        disableDriverWorkaround(workaround);

    /* Disable extensions */
    for(auto&& extension: Utility::String::splitWithoutEmptyParts(args.value("disable-extensions")))
        _disabledExtensions.push_back(extension);
}

Context::Context(Context&& other) noexcept: _version{other._version},
    #ifndef MAGNUM_TARGET_WEBGL
    _flags{other._flags},
    #endif
    _extensionRequiredVersion{Containers::NoInit},
    _extensionStatus{other._extensionStatus},
    _supportedExtensions{std::move(other._supportedExtensions)},
    _state{std::move(other._state)},
    _detectedDrivers{std::move(other._detectedDrivers)}
{
    /* StaticArray is deliberately non-copyable */
    for(std::size_t i = 0; i != Implementation::ExtensionCount; ++i)
        _extensionRequiredVersion[i] = other._extensionRequiredVersion[i];

    if(currentContext == &other) currentContext = this;
}

Context::~Context() {
    if(currentContext == this) currentContext = nullptr;
}

void Context::create() {
    /* Hard exit if the context cannot be created */
    if(!tryCreate()) std::exit(1);
}

bool Context::tryCreate() {
    CORRADE_ASSERT(_version == Version::None,
        "Platform::Context::tryCreate(): context already created", false);

    /* Load GL function pointers. Pass this instance to it so it can use it for
       potential driver-specific workarounds. */
    if(_functionLoader) _functionLoader(*this);

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

    constexpr struct {
        Version version;
        Containers::ArrayView<const Extension> extensions;
    } versions[]{
        #ifndef MAGNUM_TARGET_GLES
        {Version::GL300, Containers::arrayView(ExtensionList300)},
        {Version::GL310, Containers::arrayView(ExtensionList310)},
        {Version::GL320, Containers::arrayView(ExtensionList320)},
        {Version::GL330, Containers::arrayView(ExtensionList330)},
        {Version::GL400, Containers::arrayView(ExtensionList400)},
        {Version::GL410, Containers::arrayView(ExtensionList410)},
        {Version::GL420, Containers::arrayView(ExtensionList420)},
        {Version::GL430, Containers::arrayView(ExtensionList430)},
        {Version::GL440, Containers::arrayView(ExtensionList440)},
        {Version::GL450, Containers::arrayView(ExtensionList450)},
        {Version::GL460, Containers::arrayView(ExtensionList460)},
        #else
        {Version::GLES300, Containers::arrayView(ExtensionListES300)},
        #ifndef MAGNUM_TARGET_WEBGL
        /* No extensions in ES 3.1 */
        {Version::GLES320, Containers::arrayView(ExtensionListES320)},
        #endif
        #endif
        {Version::None, Containers::arrayView(ExtensionList)}
    };

    /* Get first future (not supported) version */
    std::size_t future = 0;
    while(versions[future].version != Version::None && isVersionSupported(versions[future].version))
        ++future;

    /* Mark all extensions from past versions as supported */
    for(std::size_t i = 0; i != future; ++i)
        for(const Extension& extension: versions[i].extensions)
            _extensionStatus.set(extension.index(), true);

    /* Check for presence of future and vendor extensions */
    const std::vector<std::string> extensions = extensionStrings();
    for(const std::string& extension: extensions) {
        for(std::size_t i = future; i != Containers::arraySize(versions); ++i)  {
            const auto found = std::lower_bound(versions[i].extensions.begin(), versions[i].extensions.end(), extension, [](const Extension& a, const std::string& b) { return a.string() < b; });
            if(found != versions[i].extensions.end() && found->string() == extension) {
                _supportedExtensions.push_back(*found);
                _extensionStatus.set(found->index(), true);
            }
        }
    }

    /* Reset minimal required version to Version::None for whole array */
    for(auto& i: _extensionRequiredVersion) i = Version::None;

    /* Initialize required versions from extension info */
    for(const auto& version: versions)
        for(const Extension& extension: version.extensions)
            _extensionRequiredVersion[extension.index()] = extension.requiredVersion();

    /* Setup driver workarounds (increase required version for particular
       extensions), see Implementation/driverWorkarounds.cpp */
    setupDriverWorkarounds();

    /* Set this context as current */
    CORRADE_ASSERT(!currentContext, "GL::Context: Another context currently active", false);
    currentContext = this;

    /* Decide whether to print the initialization output or not */
    std::ostream* output = _internalFlags & InternalFlag::DisplayInitializationLog ? Debug::output() : nullptr;

    /* Print some info and initialize state tracker (which also prints some
       more info). Mesa's renderer string has a space at the end, trim that. */
    Debug{output} << "Renderer:" << Utility::String::trim(rendererString()) << "by" << vendorString();
    Debug{output} << "OpenGL version:" << versionString();

    /* Disable extensions as requested by the user */
    if(!_disabledExtensions.empty()) {
        bool headerPrinted = false;

        /* Disable extensions that are known and supported and print a message
           for each */
        for(auto&& extension: _disabledExtensions) {
            for(const auto& version: versions)  {
                const auto found = std::lower_bound(version.extensions.begin(), version.extensions.end(), extension, [](const Extension& a, const std::string& b) { return a.string() < b; });
                /* No error message here because some of the extensions could
                   be from Vulkan or OpenAL. That also means we print the
                   header only when we actually have something to say */
                if(found == version.extensions.end() || found->string() != extension)
                    continue;

                _extensionRequiredVersion[found->index()] = Version::None;
                if(!headerPrinted) {
                    Debug{output} << "Disabling extensions:";
                    headerPrinted = true;
                }
                Debug{output} << "   " << extension;
            }
        }
    }

    _state.emplace(*this, output);

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

    /* Enable GPU validation, if requested */
    if(_internalFlags & InternalFlag::GpuValidation) {
        #ifndef MAGNUM_TARGET_WEBGL
        if(isExtensionSupported<Extensions::KHR::debug>()) {
            Renderer::enable(Renderer::Feature::DebugOutput);
            Renderer::enable(Renderer::Feature::DebugOutputSynchronous);
            DebugOutput::setDefaultCallback();

            if((detectedDriver() & DetectedDriver::Amd) && !(flags() & Flag::Debug)) {
                Warning{} << "GL::Context: GPU validation on AMD drivers requires debug context to work properly";
            } else if(_internalFlags >= InternalFlag::DisplayVerboseInitializationLog) {
                Debug{} << "GL::Context: enabling GPU validation";
            }

        } else Warning{} << "GL::Context: GPU validation requested, but GL_KHR_debug not supported";
        #else
        Warning{} << "GL::Context: GPU validation is not available on WebGL";
        #endif
    }

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

    /* If we have GL 3.0 / GLES 3.0 at least, ask the new way. Otherwise don't
       even attempt to query GL_NUM_EXTENSIONS as that would cause a GL error
       on GL 2.1. Happens with Mesa's zink that's just 2.1 currently (Apr 2020)
       even though for other backends Mesa exposes this. */
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES3
    if(isVersionSupported(Version::GL300))
    #endif
    {
        GLint extensionCount = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
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
        extensions = Utility::String::splitWithoutEmptyParts(Utility::String::fromArray(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))), ' ');
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

    if(value == Implementation::ContextState::CoreProfile::Initial) {
        /* GL < 3.2 is never a core profile, moreover querying
           GL_CONTEXT_PROFILE_MASK would result in a GL error, so don't do
           that */
        if(_version < Version::GL320)
            value = Implementation::ContextState::CoreProfile::Compatibility;
        else value = (this->*state.isCoreProfileImplementation)() ?
            Implementation::ContextState::CoreProfile::Core :
            Implementation::ContextState::CoreProfile::Compatibility;
    }

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
    #ifndef MAGNUM_TARGET_GLES2
    /* Unbind a PBO (if any) to avoid confusing external GL code that is not
       aware of those. Doing this before all buffer state is reset so we can
       reuse the knowledge in our state tracker and unbind only if Magnum
       actually bound a PBO before. */
    if(states & State::UnbindPixelBuffer) {
        Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
        Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    }
    #endif

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
    debug << "GL::Context::Flag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Context::Flag::value: return debug << "::" #value;
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

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
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
    debug << "GL::Context::DetectedDriver" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case Context::DetectedDriver::value: return debug << "::" #value;
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
        _c(Svga3D)
        #ifdef MAGNUM_TARGET_GLES
        _c(SwiftShader)
        #endif
        #endif
        #ifdef CORRADE_TARGET_ANDROID
        _c(ArmMali)
        #endif
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(GLint(value)) << Debug::nospace << ")";
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
        Context::DetectedDriver::Svga3D,
        #ifdef MAGNUM_TARGET_GLES
        Context::DetectedDriver::SwiftShader
        #endif
        #endif
    });
}
#endif

}}
