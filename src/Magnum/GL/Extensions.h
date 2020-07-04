#ifndef Magnum_GL_Extensions_h
#define Magnum_GL_Extensions_h
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

/** @file
 * @brief Namespace @ref Magnum::GL::Extensions
 */

#include "Magnum/GL/Version.h"

namespace Magnum { namespace GL {

/* Standard Android build system thinks that it's okay to define unmangled
   unprefixed macros. I think that whoever did that needs to be punished,
   becuase I am then not able to use that identifier for extension names.
   Use CORRADE_TARGET_ANDROID here instead. */
#ifdef ANDROID
#undef ANDROID
#endif

/**
@brief Compile-time information about OpenGL extensions

Each extension is a @cpp struct @ce named hierarchically by prefix, vendor and
extension name taken from list at @ref opengl-support, for example
`GL::Extensions::ARB::texture_storage`. Note that, unless said otherwise,
desktop extensions are available only on desktop build, OpenGL ES 2.0
extensions which are part of ES 3.0 are available only on
@ref MAGNUM_TARGET_GLES2 "OpenGL ES 2.0 builds", WebGL 1.0 extensions which are
part of WebGL 2.0 are available only on @ref MAGNUM_TARGET_GLES2 "WebGL 1.0"
builds, OpenGL ES extensions are available only on
@ref MAGNUM_TARGET_GLES "OpenGL ES builds" and WebGL extensions are available
only on @ref MAGNUM_TARGET_WEBGL "WebGL builds".

Each struct has the same public methods as the @ref Extension class
(@ref Extension::requiredVersion() "requiredVersion()",
@ref Extension::coreVersion() "coreVersion()" and @ref Extension::string() "string()"),
but these structs are better suited for compile-time decisions rather than
@ref Extension instances. See @ref Context::isExtensionSupported() for example
usage.

This library is built if `WITH_GL` is enabled when building Magnum. To use this
library with CMake, request the `GL` component of the `Magnum` package and link
to the `Magnum::GL` target:

@code{.cmake}
find_package(Magnum REQUIRED GL)

# ...
target_link_libraries(your-app PRIVATE Magnum::GL)
@endcode

See @ref building, @ref cmake and @ref opengl for more information.
@see @ref MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED()
*/
namespace Extensions {

#ifndef DOXYGEN_GENERATING_OUTPUT
#define _extension(index, vendor, extension, _requiredVersion, _coreVersion) \
    struct extension {                                                      \
        enum: std::size_t { Index = index };                                \
        constexpr static Version requiredVersion() { return Version::_requiredVersion; } \
        constexpr static Version coreVersion() { return Version::_coreVersion; } \
        constexpr static const char* string() { return "GL_" #vendor "_" #extension; } \
    };

#ifndef MAGNUM_TARGET_GLES
namespace AMD {
    _extension(  0,AMD,transform_feedback3_lines_triangles, GL210, None) // #397
    _extension(  1,AMD,sample_positions,                GL210,  None) // #405
    _extension(  2,AMD,vertex_shader_layer,             GL210,  None) // #417
    _extension(  3,AMD,shader_trinary_minmax,           GL210,  None) // #428
    _extension(  4,AMD,shader_explicit_vertex_parameter, GL210, None) // #485
} namespace ARB {
    _extension( 10,ARB,texture_rectangle,               GL210, GL310) // #38
    _extension( 11,ARB,color_buffer_float,              GL210, GL300) // #39
    _extension( 12,ARB,half_float_pixel,                GL210, GL300) // #40
    _extension( 13,ARB,texture_float,                   GL210, GL300) // #41
    _extension( 14,ARB,depth_buffer_float,              GL210, GL300) // #43
    _extension( 15,ARB,draw_instanced,                  GL210, GL310) // #44
    _extension( 16,ARB,framebuffer_object,              GL210, GL300) // #45
    _extension( 17,ARB,framebuffer_sRGB,                GL210, GL300) // #46
    _extension( 18,ARB,geometry_shader4,                GL210, GL320) // #47
    _extension( 19,ARB,half_float_vertex,               GL210, GL300) // #48
    _extension( 20,ARB,instanced_arrays,                GL210, GL330) // #49
    _extension( 21,ARB,map_buffer_range,                GL210, GL300) // #50
    _extension( 22,ARB,texture_buffer_object,           GL210, GL310) // #51
    _extension( 23,ARB,texture_rg,                      GL210, GL300) // #53
    _extension( 24,ARB,vertex_array_object,             GL210, GL300) // #54
    _extension( 25,ARB,uniform_buffer_object,           GL210, GL310) // #57
    _extension( 26,ARB,copy_buffer,               /*?*/ GL210, GL310) // #59
    _extension( 27,ARB,depth_clamp,               /*?*/ GL210, GL320) // #61
    _extension( 28,ARB,draw_elements_base_vertex, /*?*/ GL210, GL320) // #62
    _extension( 29,ARB,fragment_coord_conventions, /*?*/ GL210, GL320) // #63
    _extension( 30,ARB,provoking_vertex,          /*?*/ GL210, GL320) // #64
    _extension( 31,ARB,seamless_cube_map,               GL210, GL320) // #65
    _extension( 32,ARB,sync,                            GL310, GL320) // #66
    _extension( 33,ARB,texture_multisample,       /*?*/ GL210, GL320) // #67
    _extension( 34,ARB,vertex_array_bgra,               GL210, GL320) // #68
    _extension( 35,ARB,draw_buffers_blend,              GL210, GL400) // #69
    _extension( 36,ARB,sample_shading,                  GL210, GL400) // #70
    _extension( 37,ARB,texture_cube_map_array,    /*?*/ GL210, GL400) // #71
    _extension( 38,ARB,texture_gather,                  GL210, GL400) // #72
    _extension( 39,ARB,texture_query_lod,               GL210, GL400) // #73
    _extension( 40,ARB,texture_compression_bptc,        GL310, GL420) // #77
    _extension( 41,ARB,blend_func_extended,             GL210, GL330) // #78
    _extension( 42,ARB,explicit_attrib_location,        GL210, GL330) // #79
    _extension( 43,ARB,occlusion_query2,                GL210, GL330) // #80
    _extension( 44,ARB,sampler_objects,                 GL210, GL330) // #81
    _extension( 45,ARB,shader_bit_encoding,       /*?*/ GL210, GL330) // #82
    _extension( 46,ARB,texture_rgb10_a2ui,              GL210, GL330) // #83
    _extension( 47,ARB,texture_swizzle,           /*?*/ GL210, GL330) // #84
    _extension( 48,ARB,timer_query,               /*?*/ GL210, GL330) // #85
    _extension( 49,ARB,vertex_type_2_10_10_10_rev,      GL210, GL330) // #86
    _extension( 50,ARB,draw_indirect,                   GL310, GL400) // #87
    _extension( 51,ARB,gpu_shader5,                     GL320, GL400) // #88
    _extension( 52,ARB,gpu_shader_fp64,                 GL320, GL400) // #89
    _extension( 53,ARB,shader_subroutine,               GL320, GL400) // #90
    _extension( 54,ARB,tessellation_shader,             GL320, GL400) // #91
    _extension( 55,ARB,texture_buffer_object_rgb32, /*?*/ GL210, GL400) // #92
    _extension( 56,ARB,transform_feedback2,             GL210, GL400) // #93
    _extension( 57,ARB,transform_feedback3,             GL210, GL400) // #94
    _extension( 58,ARB,ES2_compatibility,         /*?*/ GL210, GL410) // #95
    _extension( 59,ARB,get_program_binary,              GL300, GL410) // #96
    _extension( 60,ARB,separate_shader_objects,         GL210, GL410) // #97
    _extension( 61,ARB,shader_precision,                GL400, GL410) // #98
    _extension( 62,ARB,vertex_attrib_64bit,             GL300, GL410) // #99
    _extension( 63,ARB,viewport_array,                  GL210, GL410) // #100
    _extension( 64,ARB,robustness,                      GL210,  None) // #105
    _extension( 65,ARB,base_instance,                   GL210, GL420) // #107
    _extension( 66,ARB,shading_language_420pack,        GL300, GL420) // #108
    _extension( 67,ARB,transform_feedback_instanced,    GL210, GL420) // #109
    _extension( 68,ARB,compressed_texture_pixel_storage,GL210, GL420) // #110
    _extension( 69,ARB,conservative_depth,              GL300, GL420) // #111
    _extension( 70,ARB,internalformat_query,            GL210, GL420) // #112
    _extension( 71,ARB,map_buffer_alignment,            GL210, GL420) // #113
    _extension( 72,ARB,shader_atomic_counters,          GL300, GL420) // #114
    _extension( 73,ARB,shader_image_load_store,         GL300, GL420) // #115
    _extension( 74,ARB,shading_language_packing,  /*?*/ GL210, GL420) // #116
    _extension( 75,ARB,texture_storage,                 GL210, GL420) // #117
    _extension( 76,ARB,arrays_of_arrays,                GL210, GL430) // #120
    _extension( 77,ARB,clear_buffer_object,             GL210, GL430) // #121
    _extension( 78,ARB,compute_shader,                  GL420, GL430) // #122
    _extension( 79,ARB,copy_image,                      GL210, GL430) // #123
    _extension( 80,ARB,texture_view,                    GL210, GL430) // #124
    _extension( 81,ARB,vertex_attrib_binding,           GL210, GL430) // #125
    _extension( 82,ARB,robustness_isolation,            GL210,  None) // #126
    _extension( 83,ARB,robustness_application_isolation, GL210, None) // #126
    _extension( 84,ARB,robustness_share_group_isolation, GL210, None) // #126
    _extension( 85,ARB,ES3_compatibility,               GL330, GL430) // #127
    _extension( 86,ARB,explicit_uniform_location,       GL210, GL430) // #128
    _extension( 87,ARB,fragment_layer_viewport,         GL300, GL430) // #129
    _extension( 88,ARB,framebuffer_no_attachments,      GL210, GL430) // #130
    _extension( 89,ARB,internalformat_query2,           GL210, GL430) // #131
    _extension( 90,ARB,invalidate_subdata,              GL210, GL430) // #132
    _extension( 91,ARB,multi_draw_indirect,             GL310, GL430) // #133
    _extension( 92,ARB,program_interface_query,         GL210, GL430) // #134
    _extension( 93,ARB,robust_buffer_access_behavior,   GL210, GL430) // #135
    _extension( 94,ARB,shader_image_size,               GL420, GL430) // #136
    _extension( 95,ARB,shader_storage_buffer_object,    GL400, GL430) // #137
    _extension( 96,ARB,stencil_texturing,               GL210, GL430) // #138
    _extension( 97,ARB,texture_buffer_range,            GL210, GL430) // #139
    _extension( 98,ARB,texture_query_levels,            GL300, GL430) // #140
    _extension( 99,ARB,texture_storage_multisample,     GL210, GL430) // #141
    _extension(100,ARB,buffer_storage,            /*?*/ GL430, GL440) // #144
    _extension(101,ARB,clear_texture,                   GL210, GL440) // #145
    _extension(102,ARB,enhanced_layouts,                GL310, GL440) // #146
    _extension(103,ARB,multi_bind,                      GL300, GL440) // #147
    _extension(104,ARB,query_buffer_object,             GL210, GL440) // #148
    _extension(105,ARB,texture_mirror_clamp_to_edge,    GL210, GL440) // #149
    _extension(106,ARB,texture_stencil8,                GL210, GL440) // #150
    _extension(107,ARB,vertex_type_10f_11f_11f_rev,     GL300, GL440) // #151
    _extension(108,ARB,bindless_texture,                GL400,  None) // #152
    _extension(109,ARB,compute_variable_group_size,     GL420,  None) // #153
    _extension(110,ARB,indirect_parameters,             GL420, GL460) // #154
    _extension(111,ARB,seamless_cubemap_per_texture,    GL320,  None) // #155
    _extension(112,ARB,shader_draw_parameters,          GL310, GL460) // #156
    _extension(113,ARB,shader_group_vote,               GL420, GL460) // #157
    _extension(114,ARB,sparse_texture,                  GL210,  None) // #158
    _extension(115,ARB,ES3_1_compatibility,             GL440, GL450) // #159
    _extension(116,ARB,clip_control,                    GL210, GL450) // #160
    _extension(117,ARB,conditional_render_inverted,     GL300, GL450) // #161
    _extension(118,ARB,cull_distance,                   GL300, GL450) // #162
    _extension(119,ARB,derivative_control,              GL400, GL450) // #163
    _extension(120,ARB,direct_state_access,             GL210, GL450) // #164
    _extension(121,ARB,get_texture_sub_image,           GL210, GL450) // #165
    _extension(122,ARB,shader_texture_image_samples,    GL430, GL450) // #166
    _extension(123,ARB,texture_barrier,                 GL210, GL450) // #167
    _extension(124,ARB,pipeline_statistics_query,       GL300, GL460) // #171
    _extension(125,ARB,sparse_buffer,                   GL210,  None) // #172
    _extension(126,ARB,transform_feedback_overflow_query, GL300, GL460) // #173
    _extension(127,ARB,ES3_2_compatibility,             GL450,  None) // #177
    _extension(128,ARB,sample_locations,                GL450,  None) // #181
    _extension(129,ARB,shader_atomic_counter_ops,       GL300, GL460) // #182
    _extension(130,ARB,gl_spirv,                        GL330, GL460) // #190
    _extension(131,ARB,polygon_offset_clamp,            GL330, GL460) // #193
    _extension(132,ARB,spirv_extensions,                GL330, GL460) // #194
    _extension(133,ARB,texture_filter_anisotropic,      GL210, GL460) // #195
} namespace ATI {
    _extension(134,ATI,texture_mirror_once,             GL210,  None) // #221
} namespace EXT {
    _extension(139,EXT,texture_filter_anisotropic,      GL210,  None) // #187
    _extension(140,EXT,texture_compression_s3tc,        GL210,  None) // #198
    /* EXT_framebuffer_object, EXT_packed_depth_stencil, EXT_framebuffer_blit,
       EXT_framebuffer_multisample replaced with ARB_framebuffer_object */
    _extension(141,EXT,texture_mirror_clamp,            GL210,  None) // #298
    _extension(142,EXT,texture_compression_dxt1,        GL210,  None) // #309
    _extension(143,EXT,gpu_shader4,                     GL210, GL300) // #326
    _extension(144,EXT,packed_float,                    GL210, GL300) // #328
    _extension(145,EXT,texture_array,                   GL210, GL300) // #329
    _extension(146,EXT,texture_compression_rgtc,        GL210, GL300) // #332
    _extension(147,EXT,texture_shared_exponent,         GL210, GL300) // #333
    _extension(148,EXT,draw_buffers2,                   GL210, GL300) // #340
    _extension(149,EXT,texture_integer,                 GL210, GL300) // #343
    _extension(150,EXT,transform_feedback,              GL210, GL300) // #352
    _extension(151,EXT,texture_snorm,                   GL300, GL310) // #365
    _extension(152,EXT,texture_sRGB_decode,             GL210,  None) // #402
    _extension(153,EXT,shader_integer_mix,              GL300,  None) // #437
    _extension(154,EXT,debug_label,                     GL210,  None) // #439
    _extension(155,EXT,debug_marker,                    GL210,  None) // #440
    _extension(156,EXT,texture_sRGB_R8,                 GL210,  None) // #534
} namespace GREMEDY {
    _extension(157,GREMEDY,string_marker,               GL210,  None) // #311
} namespace KHR {
    _extension(160,KHR,texture_compression_astc_ldr,    GL210,  None) // #118
    _extension(161,KHR,texture_compression_astc_hdr,    GL210,  None) // #118
    _extension(162,KHR,debug,                           GL210, GL430) // #119
    _extension(163,KHR,context_flush_control,           GL210, GL450) // #168
    _extension(164,KHR,robust_buffer_access_behavior,   GL320,  None) // #169
    _extension(165,KHR,robustness,                      GL320, GL450) // #170
    _extension(166,KHR,blend_equation_advanced,         GL210,  None) // #174
    _extension(167,KHR,blend_equation_advanced_coherent, GL210, None) // #174
    _extension(168,KHR,no_error,                        GL210, GL460) // #175
    _extension(169,KHR,texture_compression_astc_sliced_3d, GL210, None) // #189
} namespace MAGNUM {
    _extension(170,MAGNUM,shader_vertex_id,             GL300, GL300)
} namespace NV {
    _extension(175,NV,primitive_restart,                GL210, GL310) // #285
    _extension(176,NV,depth_buffer_float,               GL210, GL300) // #334
    _extension(177,NV,conditional_render,               GL210, GL300) // #346
    /* NV_draw_texture not supported */                               // #430
    _extension(178,NV,sample_locations,                 GL210,  None) // #472
    _extension(179,NV,fragment_shader_barycentric,      GL450,  None) // #526
} namespace OVR {
    _extension(185,OVR,multiview,                       GL300,  None) // #478
    _extension(186,OVR,multiview2,                      GL300,  None) // #479
}
#elif defined(MAGNUM_TARGET_WEBGL)
namespace ANGLE {
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 1,ANGLE,instanced_arrays,           GLES200, GLES300) // #19
    #endif
} namespace EXT {
    _extension( 2,EXT,texture_filter_anisotropic,   GLES200,    None) // #11
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 3,EXT,color_buffer_half_float,      GLES200,    None) // #14
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 4,EXT,frag_depth,                   GLES200, GLES300) // #16
    _extension( 5,EXT,sRGB,                         GLES200, GLES300) // #17
    _extension( 6,EXT,blend_minmax,                 GLES200, GLES300) // #25
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    /* Replaced by EXT_disjoint_timer_query_webgl2 in WebGL 2 */
    _extension( 7,EXT,disjoint_timer_query,         GLES200,    None) // #26
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 8,EXT,shader_texture_lod,           GLES200, GLES300) // #27
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    /* Replaces WEBGL_color_buffer_float from WebGL 1 */
    _extension( 9,EXT,color_buffer_float,           GLES300,    None) // #31
    /* Replaces WEBGL_disjoint_timer_query from WebGL 1 */
    _extension(10,EXT,disjoint_timer_query_webgl2,  GLES300,    None) // #33
    #endif
    _extension(11,EXT,float_blend,                  GLES200,    None) // #35
    _extension(12,EXT,texture_compression_rgtc,     GLES200,    None) // #38
    _extension(13,EXT,texture_compression_bptc,     GLES200,    None) // #39
    #ifndef MAGNUM_TARGET_GLES2
    _extension(14,EXT,clip_cull_distance,           GLES300,    None) // #43
    #endif
    _extension(15,EXT,texture_norm16,               GLES200,    None) // #44
    #ifndef MAGNUM_TARGET_GLES2
    _extension(16,EXT,draw_buffers_indexed,         GLES300,    None) // #45
    #endif
} namespace OES {
    #ifdef MAGNUM_TARGET_GLES2
    _extension(20,OES,texture_float,                GLES200, GLES300) // #1
    _extension(21,OES,texture_half_float,           GLES200, GLES300) // #2
    _extension(22,OES,standard_derivatives,         GLES200, GLES300) // #4
    _extension(23,OES,vertex_array_object,          GLES200, GLES300) // #5
    _extension(24,OES,element_index_uint,           GLES200, GLES300) // #10
    #endif
    _extension(25,OES,texture_float_linear,         GLES200,    None) // #20
    #ifdef MAGNUM_TARGET_GLES2
    _extension(26,OES,texture_half_float_linear,    GLES200, GLES300) // #21
    _extension(27,OES,fbo_render_mipmap,            GLES200, GLES300) // #28
    #endif
} namespace OVR {
    #ifndef MAGNUM_TARGET_GLES2
    _extension(30,OVR,multiview2,                   GLES300,    None) // #36
    #endif
} namespace WEBGL {
    _extension(31,WEBGL,lose_context,               GLES200,    None) // #3
    _extension(32,WEBGL,debug_renderer_info,        GLES200,    None) // #6
    _extension(33,WEBGL,debug_shaders,              GLES200,    None) // #7
    _extension(34,WEBGL,compressed_texture_s3tc,    GLES200,    None) // #8
    #ifdef MAGNUM_TARGET_GLES2
    _extension(35,WEBGL,depth_texture,              GLES200, GLES300) // #9
    #endif
    _extension(36,WEBGL,compressed_texture_pvrtc,   GLES200,    None) // #13
    #ifdef MAGNUM_TARGET_GLES2
    /* Subsumed by the EXT_color_buffer_float extension in WebGL 2, so
       not exposing it on WebGL 2 builds even though it's not in core */
    _extension(37,WEBGL,color_buffer_float,         GLES200,    None) // #14
    _extension(38,WEBGL,draw_buffers,               GLES200, GLES300) // #18
    #endif
    _extension(39,WEBGL,compressed_texture_astc,    GLES200,    None) // #30
    _extension(40,WEBGL,compressed_texture_s3tc_srgb, GLES200,  None) // #32
    _extension(41,WEBGL,multi_draw,                 GLES200,    None) // #40
    _extension(42,WEBGL,blend_equation_advanced_coherent,GLES200,None) // #42
    #ifndef MAGNUM_TARGET_GLES2
    _extension(43,WEBGL,draw_instanced_base_vertex_base_instance,GLES300,None) // #46
    _extension(44,WEBGL,multi_draw_instanced_base_vertex_base_instance,GLES300,None) // #47
    #endif
} namespace MAGNUM {
    #ifndef MAGNUM_TARGET_GLES2
    _extension(45,MAGNUM,shader_vertex_id,          GLES300, GLES300)
    #endif
}
#else
namespace ANDROID {
    #ifndef MAGNUM_TARGET_GLES2
    _extension(  1,ANDROID,extension_pack_es31a,    GLES310,    None) // #187
    #endif
} namespace ANGLE {
    #ifdef MAGNUM_TARGET_GLES2
    _extension(  2,ANGLE,framebuffer_blit,          GLES200, GLES300) // #83
    _extension(  3,ANGLE,framebuffer_multisample,   GLES200, GLES300) // #84
    _extension(  4,ANGLE,instanced_arrays,          GLES200, GLES300) // #109
    #endif
    _extension(  5,ANGLE,texture_compression_dxt1,  GLES200,    None) // #111
    _extension(  6,ANGLE,texture_compression_dxt3,  GLES200,    None) // #111
    _extension(  7,ANGLE,texture_compression_dxt5,  GLES200,    None) // #111
    #ifdef MAGNUM_TARGET_GLES2
    _extension(  8,ANGLE,depth_texture,             GLES200, GLES300) // #138
    #endif
} namespace APPLE {
    #ifdef MAGNUM_TARGET_GLES2
    _extension(  9,APPLE,framebuffer_multisample,   GLES200, GLES300) // #78
    #endif
    _extension( 10,APPLE,texture_format_BGRA8888,   GLES200,    None) // #79
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 11,APPLE,texture_max_level,         GLES200, GLES300) // #80
    #endif
    _extension( 12,APPLE,clip_distance,             GLES200,    None) // #193
} namespace ARM {
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 13,ARM,rgba8,                       GLES200, GLES300) // #82
    #endif
    _extension( 14,ARM,shader_framebuffer_fetch,    GLES200,    None) // #165
    _extension( 15,ARM,shader_framebuffer_fetch_depth_stencil, GLES200, None) // #166
} namespace EXT {
    _extension( 19,EXT,texture_filter_anisotropic,  GLES200,    None) // #41
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 20,EXT,texture_type_2_10_10_10_REV, GLES200, GLES300) // #42
    #endif
    _extension( 21,EXT,texture_compression_dxt1,    GLES200,    None) // #49
    _extension( 22,EXT,texture_format_BGRA8888,     GLES200,    None) // #51
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 23,EXT,discard_framebuffer,         GLES200, GLES300) // #64
    _extension( 24,EXT,blend_minmax,                GLES200, GLES300) // #65
    #endif
    _extension( 25,EXT,read_format_bgra,            GLES200,    None) // #66
    _extension( 26,EXT,multi_draw_arrays,           GLES200,    None) // #67
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 27,EXT,shader_texture_lod,          GLES200, GLES300) // #77
    _extension( 28,EXT,unpack_subimage,             GLES200, GLES300) // #90
    #endif
    _extension( 29,EXT,color_buffer_half_float,     GLES200, GLES320) // #97
    _extension( 30,EXT,debug_label,                 GLES200,    None) // #98
    _extension( 31,EXT,debug_marker,                GLES200,    None) // #99
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 32,EXT,occlusion_query_boolean,     GLES200, GLES300) // #100
    #endif
    _extension( 33,EXT,separate_shader_objects,     GLES200,    None) // #101
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 34,EXT,shadow_samplers,             GLES200, GLES300) // #102
    _extension( 35,EXT,texture_rg,                  GLES200, GLES300) // #103
    _extension( 36,EXT,sRGB,                        GLES200, GLES300) // #105
    #endif
    _extension( 37,EXT,multisampled_render_to_texture, GLES200, None) // #106
    _extension( 38,EXT,robustness,                  GLES200,    None) // #107
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 39,EXT,texture_storage,             GLES200, GLES300) // #108
    _extension( 40,EXT,map_buffer_range,            GLES200, GLES300) // #121
    #endif
    _extension( 41,EXT,shader_framebuffer_fetch,    GLES200,    None) // #122
    #ifndef MAGNUM_TARGET_GLES2
    _extension( 42,EXT,color_buffer_float,          GLES300, GLES320) // #137
    #endif
    _extension( 43,EXT,disjoint_timer_query,        GLES200,    None) // #150
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 44,EXT,draw_buffers,                GLES200, GLES300) // #151
    #endif
    _extension( 45,EXT,texture_sRGB_decode,         GLES200,    None) // #152
    _extension( 46,EXT,sRGB_write_control,          GLES200,    None) // #153
    _extension( 47,EXT,texture_compression_s3tc,    GLES200,    None) // #154
    _extension( 48,EXT,pvrtc_sRGB,                  GLES200,    None) // #155
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 49,EXT,instanced_arrays,            GLES200, GLES300) // #156
    _extension( 50,EXT,draw_instanced,              GLES200, GLES300) // #157
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension( 51,EXT,shader_integer_mix,          GLES300,    None) // #161
    _extension( 52,EXT,copy_image,                  GLES300, GLES320) // #175
    #endif
    _extension( 53,EXT,draw_buffers_indexed,        GLES200, GLES320) // #176
    #ifndef MAGNUM_TARGET_GLES2
    _extension( 54,EXT,geometry_shader,             GLES310, GLES320) // #177
    _extension( 55,EXT,gpu_shader5,                 GLES310, GLES320) // #178
    _extension( 56,EXT,shader_io_blocks,            GLES310, GLES320) // #180
    _extension( 57,EXT,tessellation_shader,         GLES310, GLES320) // #181
    #endif
    _extension( 58,EXT,texture_border_clamp,        GLES200, GLES320) // #182
    #ifndef MAGNUM_TARGET_GLES2
    _extension( 59,EXT,texture_buffer,              GLES310, GLES320) // #183
    _extension( 60,EXT,texture_cube_map_array,      GLES310, GLES320) // #184
    _extension( 61,EXT,primitive_bounding_box,      GLES310, GLES320) // #186
    _extension( 62,EXT,texture_norm16,              GLES310,    None) // #207
    _extension( 63,EXT,texture_sRGB_R8,             GLES300,    None) // #221
    _extension( 64,EXT,texture_sRGB_RG8,            GLES300,    None) // #223
    #endif
    _extension( 65,EXT,polygon_offset_clamp,        GLES200,    None) // #252
    #ifndef MAGNUM_TARGET_GLES2
    _extension( 66,EXT,clip_cull_distance,          GLES300,    None) // #257
    _extension( 67,EXT,texture_compression_rgtc,    GLES300,    None) // #286
    _extension( 68,EXT,texture_compression_bptc,    GLES300,    None) // #287
    #endif
    _extension( 69,EXT,texture_compression_s3tc_srgb, GLES200,  None) // #289
} namespace IMG {
    _extension( 70,IMG,texture_compression_pvrtc,   GLES200,    None) // #54
} namespace KHR {
    _extension( 80,KHR,texture_compression_astc_ldr,GLES200, GLES320) // #117
    _extension( 81,KHR,texture_compression_astc_hdr,GLES200,    None) // #117
    _extension( 82,KHR,debug,                       GLES200, GLES320) // #118
    _extension( 83,KHR,blend_equation_advanced,     GLES200, GLES320) // #168
    _extension( 84,KHR,blend_equation_advanced_coherent, GLES200, None) // #168
    _extension( 85,KHR,robustness,                  GLES200, GLES320) // #170
    _extension( 86,KHR,robust_buffer_access_behavior, GLES200, GLES320) // #189
    _extension( 87,KHR,context_flush_control,       GLES200,    None) // #191
    _extension( 88,KHR,no_error,                    GLES200,    None) // #243
    _extension( 89,KHR,texture_compression_astc_sliced_3d, GLES200, None) // #249
} namespace NV {
    #ifdef MAGNUM_TARGET_GLES2
    _extension(100,NV,draw_buffers,                 GLES200, GLES300) // #91
    _extension(101,NV,fbo_color_attachments,        GLES200, GLES300) // #92
    _extension(102,NV,read_buffer,                  GLES200, GLES300) // #93
    #endif
    _extension(103,NV,read_buffer_front,            GLES200,    None) // #93
    _extension(104,NV,read_depth,                   GLES200,    None) // #94
    _extension(105,NV,read_stencil,                 GLES200,    None) // #94
    _extension(106,NV,read_depth_stencil,           GLES200,    None) // #94
    #ifdef MAGNUM_TARGET_GLES2
    _extension(107,NV,pack_subimage,                GLES200, GLES300) // #132
    _extension(108,NV,draw_instanced,               GLES200, GLES300) // #141
    _extension(109,NV,framebuffer_blit,             GLES200, GLES300) // #142
    _extension(110,NV,framebuffer_multisample,      GLES200, GLES300) // #143
    _extension(111,NV,instanced_arrays,             GLES200, GLES300) // #145
    _extension(112,NV,shadow_samplers_array,        GLES200, GLES300) // #146
    _extension(113,NV,shadow_samplers_cube,         GLES200, GLES300) // #147
    #endif
    _extension(114,NV,texture_border_clamp,         GLES200,    None) // #149
    #ifndef MAGNUM_TARGET_GLES2
    _extension(115,NV,shader_noperspective_interpolation, GLES300, None) // #201
    #endif
    _extension(116,NV,sample_locations,             GLES200,    None) // #235
    _extension(117,NV,polygon_mode,                 GLES200,    None) // #238
    #ifndef MAGNUM_TARGET_GLES2
    _extension(118,NV,fragment_shader_barycentric,  GLES320,    None) // #316
    #endif
} namespace OES {
    #ifdef MAGNUM_TARGET_GLES2
    _extension(120,OES,depth24,                     GLES200, GLES300) // #24
    #endif
    _extension(121,OES,depth32,                     GLES200,    None) // #25
    #ifdef MAGNUM_TARGET_GLES2
    _extension(122,OES,element_index_uint,          GLES200, GLES300) // #26
    _extension(123,OES,fbo_render_mipmap,           GLES200, GLES300) // #27
    #endif
    _extension(124,OES,mapbuffer,                   GLES200,    None) // #29
    #ifdef MAGNUM_TARGET_GLES2
    _extension(125,OES,rgb8_rgba8,                  GLES200, GLES300) // #30
    #endif
    _extension(126,OES,stencil1,                    GLES200,    None) // #31
    _extension(127,OES,stencil4,                    GLES200,    None) // #32
    #ifdef MAGNUM_TARGET_GLES2
    _extension(128,OES,texture_3D,                  GLES200, GLES300) // #34
    _extension(129,OES,texture_half_float_linear,   GLES200, GLES300) // #35
    #endif
    _extension(130,OES,texture_float_linear,        GLES200,    None) // #35
    #ifdef MAGNUM_TARGET_GLES2
    _extension(131,OES,texture_half_float,          GLES200, GLES300) // #36
    _extension(132,OES,texture_float,               GLES200, GLES300) // #36
    _extension(133,OES,texture_npot,                GLES200, GLES300) // #37
    _extension(134,OES,vertex_half_float,           GLES200, GLES300) // #38
    _extension(135,OES,packed_depth_stencil,        GLES200, GLES300) // #43
    _extension(136,OES,depth_texture,               GLES200, GLES300) // #44
    _extension(137,OES,standard_derivatives,        GLES200, GLES300) // #45
    _extension(138,OES,vertex_array_object,         GLES200, GLES300) // #71
    _extension(139,OES,required_internalformat,     GLES200, GLES300) // #115
    _extension(140,OES,surfaceless_context,         GLES200, GLES300) // #116
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(141,OES,texture_compression_astc,    GLES300,    None) // #162
    _extension(142,OES,sample_shading,              GLES300, GLES320) // #169
    _extension(143,OES,sample_variables,            GLES300, GLES320) // #170
    _extension(144,OES,shader_image_atomic,         GLES310, GLES320) // #171
    _extension(145,OES,shader_multisample_interpolation, GLES300, GLES320) // #172
    #endif
    _extension(146,OES,texture_stencil8,            GLES200, GLES320) // #173
    #ifndef MAGNUM_TARGET_GLES2
    _extension(147,OES,texture_storage_multisample_2d_array, GLES310, GLES320) // #174
    #endif
} namespace OVR {
    #ifndef MAGNUM_TARGET_GLES2
    _extension(148,OVR,multiview,                   GLES300,    None) // #241
    _extension(149,OVR,multiview2,                  GLES300,    None) // #242
    #endif
} namespace MAGNUM {
    #ifndef MAGNUM_TARGET_GLES2
    _extension(150,MAGNUM,shader_vertex_id,         GLES300, GLES300)
    #endif
}
#endif
#undef _extension
#endif

}

}}

#endif
