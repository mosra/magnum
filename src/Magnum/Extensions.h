#ifndef Magnum_Extensions_h
#define Magnum_Extensions_h
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

/** @file
 * @brief Namespace @ref Magnum::Extensions
 */

#include "Magnum/Version.h"

namespace Magnum {

/* Standard Android build system thinks that it's okay to define unmangled
   unprefixed macros. I think that whoever did that needs to be punished,
   becuase I am then not able to use that identifier for extension names.
   Use CORRADE_TARGET_ANDROID here instead. */
#ifdef ANDROID
#undef ANDROID
#endif

/**
@brief Compile-time information about OpenGL extensions

Each extension is `struct` named hierarchically by prefix, vendor and
extension name taken from list at @ref opengl-support, for example
`GL::ARB::texture_storage`. Note that desktop extensions are available only on
desktop build, OpenGL ES 2.0 extensions which are part of ES 3.0 are available
only on @ref MAGNUM_TARGET_GLES2 "OpenGL ES 2.0 build" and vendor OpenGL ES
extensions are available only on @ref MAGNUM_TARGET_GLES "OpenGL ES builds".

Each struct has the same public methods as @ref Extension class
(@ref Extension::requiredVersion() "requiredVersion()",
@ref Extension::coreVersion() "coreVersion()" and @ref Extension::string() "string()"),
but these structs are better suited for compile-time decisions rather than
@ref Extension instances. See @ref Context::isExtensionSupported() for example
usage.

This namespace is built by default. To use it, you need to link to
`Magnum::Magnum` target. See @ref building and @ref cmake for more information.
@see @ref MAGNUM_ASSERT_EXTENSION_SUPPORTED()
@todo Manual indices for extensions, this has gaps
*/
namespace Extensions {

#ifndef DOXYGEN_GENERATING_OUTPUT
#define _extension(prefix, vendor, extension, _requiredVersion, _coreVersion) \
    struct extension {                                                      \
        enum: std::size_t { Index = __LINE__-1 };                                \
        constexpr static Version requiredVersion() { return Version::_requiredVersion; } \
        constexpr static Version coreVersion() { return Version::_coreVersion; } \
        constexpr static const char* string() { return #prefix "_" #vendor "_" #extension; } \
    };

/* IMPORTANT: don't forget to add new extensions also in Context.cpp */

namespace GL {
    #ifndef MAGNUM_TARGET_GLES
    #line 1
    namespace AMD {
        _extension(GL,AMD,transform_feedback3_lines_triangles, GL210, None) // #397
        _extension(GL,AMD,vertex_shader_layer,          GL210,  None) // #417
        _extension(GL,AMD,shader_trinary_minmax,        GL210,  None) // #428
    } namespace ARB {
        _extension(GL,ARB,texture_rectangle,            GL210, GL310) // #38
        _extension(GL,ARB,color_buffer_float,           GL210, GL300) // #39
        _extension(GL,ARB,half_float_pixel,             GL210, GL300) // #40
        _extension(GL,ARB,texture_float,                GL210, GL300) // #41
        _extension(GL,ARB,depth_buffer_float,           GL210, GL300) // #43
        _extension(GL,ARB,draw_instanced,               GL210, GL310) // #44
        _extension(GL,ARB,framebuffer_object,           GL210, GL300) // #45
        _extension(GL,ARB,framebuffer_sRGB,             GL210, GL300) // #46
        _extension(GL,ARB,geometry_shader4,             GL210, GL320) // #47
        _extension(GL,ARB,half_float_vertex,            GL210, GL300) // #48
        _extension(GL,ARB,instanced_arrays,             GL210, GL330) // #49
        _extension(GL,ARB,map_buffer_range,             GL210, GL300) // #50
        _extension(GL,ARB,texture_buffer_object,        GL210, GL310) // #51
        _extension(GL,ARB,texture_rg,                   GL210, GL300) // #53
        _extension(GL,ARB,vertex_array_object,          GL210, GL300) // #54
        _extension(GL,ARB,uniform_buffer_object,        GL210, GL310) // #57
        _extension(GL,ARB,copy_buffer,            /*?*/ GL210, GL310) // #59
        _extension(GL,ARB,depth_clamp,            /*?*/ GL210, GL320) // #61
        _extension(GL,ARB,draw_elements_base_vertex, /*?*/ GL210, GL320) // #62
        _extension(GL,ARB,fragment_coord_conventions, /*?*/ GL210, GL320) // #63
        _extension(GL,ARB,provoking_vertex,       /*?*/ GL210, GL320) // #64
        _extension(GL,ARB,seamless_cube_map,            GL210, GL320) // #65
        _extension(GL,ARB,sync,                         GL310, GL320) // #66
        _extension(GL,ARB,texture_multisample,    /*?*/ GL210, GL320) // #67
        _extension(GL,ARB,vertex_array_bgra,            GL210, GL320) // #68
        _extension(GL,ARB,draw_buffers_blend,           GL210, GL400) // #69
        _extension(GL,ARB,sample_shading,               GL210, GL400) // #70
        _extension(GL,ARB,texture_cube_map_array, /*?*/ GL210, GL400) // #71
        _extension(GL,ARB,texture_gather,               GL210, GL400) // #72
        _extension(GL,ARB,texture_query_lod,            GL210, GL400) // #73
        _extension(GL,ARB,texture_compression_bptc,     GL310, GL420) // #77
        _extension(GL,ARB,blend_func_extended,          GL210, GL330) // #78
        _extension(GL,ARB,explicit_attrib_location,     GL210, GL330) // #79
        _extension(GL,ARB,occlusion_query2,             GL210, GL330) // #80
        _extension(GL,ARB,sampler_objects,              GL210, GL330) // #81
        _extension(GL,ARB,shader_bit_encoding,    /*?*/ GL210, GL330) // #82
        _extension(GL,ARB,texture_rgb10_a2ui,           GL210, GL330) // #83
        _extension(GL,ARB,texture_swizzle,        /*?*/ GL210, GL330) // #84
        _extension(GL,ARB,timer_query,            /*?*/ GL210, GL330) // #85
        _extension(GL,ARB,vertex_type_2_10_10_10_rev,   GL210, GL330) // #86
        _extension(GL,ARB,draw_indirect,                GL310, GL400) // #87
        _extension(GL,ARB,gpu_shader5,                  GL320, GL400) // #88
        _extension(GL,ARB,gpu_shader_fp64,              GL320, GL400) // #89
        _extension(GL,ARB,shader_subroutine,            GL320, GL400) // #90
        _extension(GL,ARB,tessellation_shader,          GL320, GL400) // #91
        _extension(GL,ARB,texture_buffer_object_rgb32, /*?*/ GL210, GL400) // #92
        _extension(GL,ARB,transform_feedback2,          GL210, GL400) // #93
        _extension(GL,ARB,transform_feedback3,          GL210, GL400) // #94
        _extension(GL,ARB,ES2_compatibility,      /*?*/ GL210, GL410) // #95
        _extension(GL,ARB,get_program_binary,           GL300, GL410) // #96
        _extension(GL,ARB,separate_shader_objects,      GL210, GL410) // #97
        _extension(GL,ARB,shader_precision,             GL400, GL410) // #98
        _extension(GL,ARB,vertex_attrib_64bit,          GL300, GL410) // #99
        _extension(GL,ARB,viewport_array,               GL210, GL410) // #100
        _extension(GL,ARB,robustness,                   GL210,  None) // #105
        _extension(GL,ARB,base_instance,                GL210, GL420) // #107
        _extension(GL,ARB,shading_language_420pack,     GL300, GL420) // #108
        _extension(GL,ARB,transform_feedback_instanced, GL210, GL420) // #109
        _extension(GL,ARB,compressed_texture_pixel_storage, GL210, GL420) // #110
        _extension(GL,ARB,conservative_depth,           GL300, GL420) // #111
        _extension(GL,ARB,internalformat_query,         GL210, GL420) // #112
        _extension(GL,ARB,map_buffer_alignment,         GL210, GL420) // #113
        _extension(GL,ARB,shader_atomic_counters,       GL300, GL420) // #114
        _extension(GL,ARB,shader_image_load_store,      GL300, GL420) // #115
        _extension(GL,ARB,shading_language_packing, /*?*/ GL210, GL420) // #116
        _extension(GL,ARB,texture_storage,              GL210, GL420) // #117
        _extension(GL,ARB,arrays_of_arrays,             GL210, GL430) // #120
        _extension(GL,ARB,clear_buffer_object,          GL210, GL430) // #121
        _extension(GL,ARB,compute_shader,               GL420, GL430) // #122
        _extension(GL,ARB,copy_image,                   GL210, GL430) // #123
        _extension(GL,ARB,texture_view,                 GL210, GL430) // #124
        _extension(GL,ARB,vertex_attrib_binding,        GL210, GL430) // #125
        _extension(GL,ARB,robustness_isolation,         GL210,  None) // #126
        _extension(GL,ARB,robustness_application_isolation, GL210, None) // #126
        _extension(GL,ARB,robustness_share_group_isolation, GL210, None) // #126
        _extension(GL,ARB,ES3_compatibility,            GL330, GL430) // #127
        _extension(GL,ARB,explicit_uniform_location,    GL210, GL430) // #128
        _extension(GL,ARB,fragment_layer_viewport,      GL300, GL430) // #129
        _extension(GL,ARB,framebuffer_no_attachments,   GL210, GL430) // #130
        _extension(GL,ARB,internalformat_query2,        GL210, GL430) // #131
        _extension(GL,ARB,invalidate_subdata,           GL210, GL430) // #132
        _extension(GL,ARB,multi_draw_indirect,          GL310, GL430) // #133
        _extension(GL,ARB,program_interface_query,      GL210, GL430) // #134
        _extension(GL,ARB,robust_buffer_access_behavior,GL210, GL430) // #135
        _extension(GL,ARB,shader_image_size,            GL420, GL430) // #136
        _extension(GL,ARB,shader_storage_buffer_object, GL400, GL430) // #137
        _extension(GL,ARB,stencil_texturing,            GL210, GL430) // #138
        _extension(GL,ARB,texture_buffer_range,         GL210, GL430) // #139
        _extension(GL,ARB,texture_query_levels,         GL300, GL430) // #140
        _extension(GL,ARB,texture_storage_multisample,  GL210, GL430) // #141
        _extension(GL,ARB,buffer_storage,         /*?*/ GL430, GL440) // #144
        _extension(GL,ARB,clear_texture,                GL210, GL440) // #145
        _extension(GL,ARB,enhanced_layouts,             GL310, GL440) // #146
        _extension(GL,ARB,multi_bind,                   GL300, GL440) // #147
        _extension(GL,ARB,query_buffer_object,          GL210, GL440) // #148
        _extension(GL,ARB,texture_mirror_clamp_to_edge, GL210, GL440) // #149
        _extension(GL,ARB,texture_stencil8,             GL210, GL440) // #150
        _extension(GL,ARB,vertex_type_10f_11f_11f_rev,  GL300, GL440) // #151
        _extension(GL,ARB,bindless_texture,             GL400,  None) // #152
        _extension(GL,ARB,compute_variable_group_size,  GL420,  None) // #153
        _extension(GL,ARB,indirect_parameters,          GL420,  None) // #154
        _extension(GL,ARB,seamless_cubemap_per_texture, GL320,  None) // #155
        _extension(GL,ARB,shader_draw_parameters,       GL310,  None) // #156
        _extension(GL,ARB,shader_group_vote,            GL420,  None) // #157
        _extension(GL,ARB,sparse_texture,               GL210,  None) // #158
        _extension(GL,ARB,ES3_1_compatibility,          GL440, GL450) // #159
        _extension(GL,ARB,clip_control,                 GL210, GL450) // #160
        _extension(GL,ARB,conditional_render_inverted,  GL300, GL450) // #161
        _extension(GL,ARB,cull_distance,                GL300, GL450) // #162
        _extension(GL,ARB,derivative_control,           GL400, GL450) // #163
        _extension(GL,ARB,direct_state_access,          GL210, GL450) // #164
        _extension(GL,ARB,get_texture_sub_image,        GL210, GL450) // #165
        _extension(GL,ARB,shader_texture_image_samples, GL430, GL450) // #166
        _extension(GL,ARB,texture_barrier,              GL210, GL450) // #167
        _extension(GL,ARB,pipeline_statistics_query,    GL300,  None) // #171
        _extension(GL,ARB,sparse_buffer,                GL210,  None) // #172
        _extension(GL,ARB,transform_feedback_overflow_query, GL300, None) // #173
    } namespace ATI {
        _extension(GL,ATI,texture_mirror_once,          GL210,  None) // #221
    } namespace EXT {
        _extension(GL,EXT,texture_filter_anisotropic,   GL210,  None) // #187
        _extension(GL,EXT,texture_compression_s3tc,     GL210,  None) // #198
        /* EXT_framebuffer_object, EXT_packed_depth_stencil, EXT_framebuffer_blit,
           EXT_framebuffer_multisample replaced with ARB_framebuffer_object */
        _extension(GL,EXT,texture_mirror_clamp,         GL210,  None) // #298
        _extension(GL,EXT,gpu_shader4,                  GL210, GL300) // #326
        _extension(GL,EXT,packed_float,                 GL210, GL300) // #328
        _extension(GL,EXT,texture_array,                GL210, GL300) // #329
        _extension(GL,EXT,texture_compression_rgtc,     GL210, GL300) // #332
        _extension(GL,EXT,texture_shared_exponent,      GL210, GL300) // #333
        _extension(GL,EXT,draw_buffers2,                GL210, GL300) // #340
        _extension(GL,EXT,texture_integer,              GL210, GL300) // #343
        _extension(GL,EXT,transform_feedback,           GL210, GL300) // #352
        _extension(GL,EXT,direct_state_access,          GL210,  None) // #353
        _extension(GL,EXT,texture_snorm,                GL300, GL310) // #365
        _extension(GL,EXT,texture_sRGB_decode,          GL210,  None) // #402
        _extension(GL,EXT,shader_integer_mix,           GL300,  None) // #437
        _extension(GL,EXT,debug_label,                  GL210,  None) // #439
        _extension(GL,EXT,debug_marker,                 GL210,  None) // #440
    } namespace GREMEDY {
        _extension(GL,GREMEDY,string_marker,            GL210,  None) // #311
    } namespace KHR {
        _extension(GL,KHR,texture_compression_astc_ldr, GL210,  None) // #118
        _extension(GL,KHR,texture_compression_astc_hdr, GL210,  None) // #118
        _extension(GL,KHR,debug,                        GL210, GL430) // #119
        _extension(GL,KHR,context_flush_control,        GL210, GL450) // #168
        _extension(GL,KHR,robustness,                   GL320, GL450) // #170
        _extension(GL,KHR,blend_equation_advanced,      GL210,  None) // #174
        _extension(GL,KHR,blend_equation_advanced_coherent, GL210, None) // #174
        _extension(GL,KHR,no_error,                     GL210,  None) // #175
    } namespace NV {
        _extension(GL,NV,primitive_restart,             GL210, GL310) // #285
        _extension(GL,NV,depth_buffer_float,            GL210, GL300) // #334
        _extension(GL,NV,conditional_render,            GL210, GL300) // #346
        /* NV_draw_texture not supported */                           // #430
    }
    /* IMPORTANT: if this line is > 329 (73 + size), don't forget to update array size in Context.h */
    #elif defined(MAGNUM_TARGET_WEBGL)
    #line 1
    namespace ANGLE {
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,ANGLE,instanced_arrays,       GLES200, GLES300) // #19
        #endif
    } namespace EXT {
        _extension(GL,EXT,texture_filter_anisotropic, GLES200,  None) // #11
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,sRGB,                     GLES200, GLES300) // #17
        _extension(GL,EXT,blend_minmax,             GLES200, GLES300) // #25
        #endif
        _extension(GL,EXT,disjoint_timer_query,     GLES200,    None) // #26
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,shader_texture_lod,       GLES200, GLES300) // #27
        #endif
    } namespace OES {
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,OES,texture_float,            GLES200, GLES300) // #1
        _extension(GL,OES,texture_half_float,       GLES200, GLES300) // #2
        _extension(GL,OES,standard_derivatives,     GLES200, GLES300) // #4
        _extension(GL,OES,vertex_array_object,      GLES200, GLES300) // #5
        _extension(GL,OES,element_index_uint,       GLES200, GLES300) // #10
        _extension(GL,OES,texture_float_linear,     GLES200, GLES300) // #20
        _extension(GL,OES,texture_half_float_linear, GLES200, GLES300) // #21
        _extension(GL,OES,fbo_render_mipmap,        GLES200, GLES300) // #28
        #endif
    } namespace WEBGL {
        _extension(GL,WEBGL,compressed_texture_s3tc, GLES200,   None) // #8
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,WEBGL,depth_texture,          GLES200, GLES300) // #9
        _extension(GL,WEBGL,draw_buffers,           GLES200, GLES300) // #18
        #endif
    }
    #else
    #line 1
    namespace ANDROID {
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,ANDROID,extension_pack_es31a, GLES310,    None) // #187
        #endif
    } namespace ANGLE {
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,ANGLE,framebuffer_blit,       GLES200, GLES300) // #83
        _extension(GL,ANGLE,framebuffer_multisample, GLES200, GLES300) // #84
        _extension(GL,ANGLE,instanced_arrays,       GLES200, GLES300) // #109
        _extension(GL,ANGLE,depth_texture,          GLES200, GLES300) // #138
        #endif
    } namespace APPLE {
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,APPLE,framebuffer_multisample, GLES200, GLES300) // #78
        #endif
        _extension(GL,APPLE,texture_format_BGRA8888, GLES200,   None) // #79
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,APPLE,texture_max_level,      GLES200,    None) // #80
        #endif
    } namespace ARM {
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,ARM,rgba8,                    GLES200, GLES300) // #82
        #endif
        _extension(GL,ARM,shader_framebuffer_fetch, GLES200,    None) // #165
        _extension(GL,ARM,shader_framebuffer_fetch_depth_stencil, GLES200, None) // #166
    } namespace EXT {
        _extension(GL,EXT,texture_filter_anisotropic, GLES200,  None) // #41
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,texture_type_2_10_10_10_REV, GLES200, GLES300) // #42
        #endif
        _extension(GL,EXT,texture_format_BGRA8888,  GLES200,    None) // #51
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,discard_framebuffer,      GLES200, GLES300) // #64
        _extension(GL,EXT,blend_minmax,             GLES200, GLES300) // #65
        #endif
        _extension(GL,EXT,read_format_bgra,         GLES200,    None) // #66
        _extension(GL,EXT,multi_draw_arrays,        GLES200,    None) // #67
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,shader_texture_lod,       GLES200, GLES300) // #77
        _extension(GL,EXT,unpack_subimage,          GLES200, GLES300) // #90
        #endif
        _extension(GL,EXT,debug_label,              GLES200,    None) // #98
        _extension(GL,EXT,debug_marker,             GLES200,    None) // #99
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,occlusion_query_boolean,  GLES200, GLES300) // #100
        #endif
        _extension(GL,EXT,separate_shader_objects,  GLES200,    None) // #101
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,shadow_samplers,          GLES200, GLES300) // #102
        _extension(GL,EXT,texture_rg,               GLES200, GLES300) // #103
        _extension(GL,EXT,sRGB,                     GLES200, GLES300) // #105
        #endif
        _extension(GL,EXT,multisampled_render_to_texture, GLES200, None) // #106
        _extension(GL,EXT,robustness,               GLES200,    None) // #107
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,texture_storage,          GLES200, GLES300) // #108
        _extension(GL,EXT,map_buffer_range,         GLES200, GLES300) // #121
        #endif
        _extension(GL,EXT,shader_framebuffer_fetch, GLES200,    None) // #122
        _extension(GL,EXT,disjoint_timer_query,     GLES200,    None) // #150
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,draw_buffers,             GLES200, GLES300) // #151
        #endif
        _extension(GL,EXT,texture_sRGB_decode,      GLES200,    None) // #152
        _extension(GL,EXT,sRGB_write_control,       GLES200,    None) // #153
        _extension(GL,EXT,texture_compression_s3tc, GLES200,    None) // #154
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,instanced_arrays,         GLES200, GLES300) // #156
        _extension(GL,EXT,draw_instanced,           GLES200, GLES300) // #157
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,shader_integer_mix,       GLES300,    None) // #161
        _extension(GL,EXT,copy_image,               GLES300,    None) // #175
        #endif
        _extension(GL,EXT,draw_buffers_indexed,     GLES200,    None) // #176
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,geometry_shader,          GLES310,    None) // #177
        _extension(GL,EXT,gpu_shader5,              GLES310,    None) // #178
        _extension(GL,EXT,shader_io_blocks,         GLES310,    None) // #180
        _extension(GL,EXT,tessellation_shader,      GLES310,    None) // #181
        #endif
        _extension(GL,EXT,texture_border_clamp,     GLES200,    None) // #182
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,EXT,texture_buffer,           GLES310,    None) // #183
        _extension(GL,EXT,texture_cube_map_array,   GLES310,    None) // #184
        _extension(GL,EXT,primitive_bounding_box,   GLES310,    None) // #186
        #endif
    } namespace KHR {
        _extension(GL,KHR,texture_compression_astc_ldr, GLES200, None) // #117
        _extension(GL,KHR,texture_compression_astc_hdr, GLES200, None) // #117
        _extension(GL,KHR,debug,                    GLES200,    None) // #118
        _extension(GL,KHR,blend_equation_advanced,  GLES200,    None) // #168
        _extension(GL,KHR,blend_equation_advanced_coherent, GLES200, None) // #168
        _extension(GL,KHR,robustness,               GLES200,    None) // #170
        _extension(GL,KHR,robust_buffer_access_behavior, GLES200, None) // #189
        _extension(GL,KHR,context_flush_control,    GLES200,    None) // #191
        _extension(GL,KHR,no_error,                 GLES200,    None) // #243
    } namespace NV {
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,NV,draw_buffers,              GLES200, GLES300) // #91
        _extension(GL,NV,fbo_color_attachments,     GLES200, GLES300) // #92
        _extension(GL,NV,read_buffer,               GLES200, GLES300) // #93
        #endif
        _extension(GL,NV,read_buffer_front,         GLES200,    None) // #93
        _extension(GL,NV,read_depth,                GLES200,    None) // #94
        _extension(GL,NV,read_stencil,              GLES200,    None) // #94
        _extension(GL,NV,read_depth_stencil,        GLES200,    None) // #94
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,NV,pack_subimage,             GLES200, GLES300) // #132
        _extension(GL,NV,draw_instanced,            GLES200, GLES300) // #141
        _extension(GL,NV,framebuffer_blit,          GLES200, GLES300) // #142
        _extension(GL,NV,framebuffer_multisample,   GLES200, GLES300) // #143
        _extension(GL,NV,instanced_arrays,          GLES200, GLES300) // #145
        _extension(GL,NV,shadow_samplers_array,     GLES200, GLES300) // #146
        _extension(GL,NV,shadow_samplers_cube,      GLES200, GLES300) // #147
        #endif
        _extension(GL,NV,texture_border_clamp,      GLES200,    None) // #149
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,NV,shader_noperspective_interpolation, GLES300, None) // #201
        #endif
        _extension(GL,NV,polygon_mode,              GLES200,    None) // #238
    } namespace OES {
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,OES,depth24,                  GLES200, GLES300) // #24
        #endif
        _extension(GL,OES,depth32,                  GLES200,    None) // #25
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,OES,element_index_uint,       GLES200, GLES300) // #26
        _extension(GL,OES,fbo_render_mipmap,        GLES200, GLES300) // #27
        #endif
        _extension(GL,OES,mapbuffer,                GLES200,    None) // #29
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,OES,rgb8_rgba8,               GLES200, GLES300) // #30
        #endif
        _extension(GL,OES,stencil1,                 GLES200,    None) // #31
        _extension(GL,OES,stencil4,                 GLES200,    None) // #32
        #ifdef MAGNUM_TARGET_GLES2
        _extension(GL,OES,texture_3D,               GLES200, GLES300) // #34
        _extension(GL,OES,texture_half_float_linear, GLES200, GLES300) // #35
        _extension(GL,OES,texture_float_linear,     GLES200, GLES300) // #35
        _extension(GL,OES,texture_half_float,       GLES200, GLES300) // #36
        _extension(GL,OES,texture_float,            GLES200, GLES300) // #36
        _extension(GL,OES,texture_npot,             GLES200, GLES300) // #37
        _extension(GL,OES,vertex_half_float,        GLES200, GLES300) // #38
        _extension(GL,OES,packed_depth_stencil,     GLES200, GLES300) // #43
        _extension(GL,OES,depth_texture,            GLES200, GLES300) // #44
        _extension(GL,OES,standard_derivatives,     GLES200, GLES300) // #45
        _extension(GL,OES,vertex_array_object,      GLES200, GLES300) // #71
        _extension(GL,OES,required_internalformat,  GLES200, GLES300) // #115
        _extension(GL,OES,surfaceless_context,      GLES200, GLES300) // #116
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,OES,sample_shading,           GLES300,    None) // #169
        _extension(GL,OES,sample_variables,         GLES300,    None) // #170
        _extension(GL,OES,shader_image_atomic,      GLES310,    None) // #171
        _extension(GL,OES,shader_multisample_interpolation, GLES300, None) // #172
        #endif
        _extension(GL,OES,texture_stencil8,         GLES200,    None) // #173
        #ifndef MAGNUM_TARGET_GLES2
        _extension(GL,OES,texture_storage_multisample_2d_array, GLES310, None) // #174
        #endif
    }
    #endif
}
#undef _extension
#endif

}

}

#endif
