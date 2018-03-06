#ifndef Magnum_Extensions_h
#define Magnum_Extensions_h
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

Each extension is a `struct` named hierarchically by prefix, vendor and
extension name taken from list at @ref opengl-support, for example
`Extensions::GL::ARB::texture_storage`. Note that desktop extensions are
available only on desktop build, OpenGL ES 2.0 extensions which are part of ES
3.0 are available only on @ref MAGNUM_TARGET_GLES2 "OpenGL ES 2.0 build",
vendor OpenGL ES extensions are available only on
@ref MAGNUM_TARGET_GLES "OpenGL ES builds" and WebGL extensions are available
only on @ref MAGNUM_TARGET_WEBGL "WebGL builds".

Each struct has the same public methods as the @ref Extension class
(@ref Extension::requiredVersion() "requiredVersion()",
@ref Extension::coreVersion() "coreVersion()" and @ref Extension::string() "string()"),
but these structs are better suited for compile-time decisions rather than
@ref Extension instances. See @ref Context::isExtensionSupported() for example
usage.

This library is built by default. To use this library with CMake, you need to
find the `Magnum` package and link to the `Magnum::Magnum` target:

@code{.cmake}
find_package(Magnum REQUIRED)

# ...
target_link_libraries(your-app Magnum::Magnum)
@endcode

See @ref building and @ref cmake for more information.
@see @ref MAGNUM_ASSERT_EXTENSION_SUPPORTED()
*/
namespace Extensions {

#ifndef DOXYGEN_GENERATING_OUTPUT
#define _extension(index, prefix, vendor, extension, _requiredVersion, _coreVersion) \
    struct extension {                                                      \
        enum: std::size_t { Index = index };                                \
        constexpr static Version requiredVersion() { return Version::_requiredVersion; } \
        constexpr static Version coreVersion() { return Version::_coreVersion; } \
        constexpr static const char* string() { return #prefix "_" #vendor "_" #extension; } \
    };

namespace GL {

#ifndef MAGNUM_TARGET_GLES
namespace AMD {
    _extension(  0,GL,AMD,transform_feedback3_lines_triangles, GL210, None) // #397
    _extension(  1,GL,AMD,vertex_shader_layer,          GL210,  None) // #417
    _extension(  2,GL,AMD,shader_trinary_minmax,        GL210,  None) // #428
} namespace ARB {
    _extension( 10,GL,ARB,texture_rectangle,            GL210, GL310) // #38
    _extension( 11,GL,ARB,color_buffer_float,           GL210, GL300) // #39
    _extension( 12,GL,ARB,half_float_pixel,             GL210, GL300) // #40
    _extension( 13,GL,ARB,texture_float,                GL210, GL300) // #41
    _extension( 14,GL,ARB,depth_buffer_float,           GL210, GL300) // #43
    _extension( 15,GL,ARB,draw_instanced,               GL210, GL310) // #44
    _extension( 16,GL,ARB,framebuffer_object,           GL210, GL300) // #45
    _extension( 17,GL,ARB,framebuffer_sRGB,             GL210, GL300) // #46
    _extension( 18,GL,ARB,geometry_shader4,             GL210, GL320) // #47
    _extension( 19,GL,ARB,half_float_vertex,            GL210, GL300) // #48
    _extension( 20,GL,ARB,instanced_arrays,             GL210, GL330) // #49
    _extension( 21,GL,ARB,map_buffer_range,             GL210, GL300) // #50
    _extension( 22,GL,ARB,texture_buffer_object,        GL210, GL310) // #51
    _extension( 23,GL,ARB,texture_rg,                   GL210, GL300) // #53
    _extension( 24,GL,ARB,vertex_array_object,          GL210, GL300) // #54
    _extension( 25,GL,ARB,uniform_buffer_object,        GL210, GL310) // #57
    _extension( 26,GL,ARB,copy_buffer,            /*?*/ GL210, GL310) // #59
    _extension( 27,GL,ARB,depth_clamp,            /*?*/ GL210, GL320) // #61
    _extension( 28,GL,ARB,draw_elements_base_vertex, /*?*/ GL210, GL320) // #62
    _extension( 29,GL,ARB,fragment_coord_conventions, /*?*/ GL210, GL320) // #63
    _extension( 30,GL,ARB,provoking_vertex,       /*?*/ GL210, GL320) // #64
    _extension( 31,GL,ARB,seamless_cube_map,            GL210, GL320) // #65
    _extension( 32,GL,ARB,sync,                         GL310, GL320) // #66
    _extension( 33,GL,ARB,texture_multisample,    /*?*/ GL210, GL320) // #67
    _extension( 34,GL,ARB,vertex_array_bgra,            GL210, GL320) // #68
    _extension( 35,GL,ARB,draw_buffers_blend,           GL210, GL400) // #69
    _extension( 36,GL,ARB,sample_shading,               GL210, GL400) // #70
    _extension( 37,GL,ARB,texture_cube_map_array, /*?*/ GL210, GL400) // #71
    _extension( 38,GL,ARB,texture_gather,               GL210, GL400) // #72
    _extension( 39,GL,ARB,texture_query_lod,            GL210, GL400) // #73
    _extension( 40,GL,ARB,texture_compression_bptc,     GL310, GL420) // #77
    _extension( 41,GL,ARB,blend_func_extended,          GL210, GL330) // #78
    _extension( 42,GL,ARB,explicit_attrib_location,     GL210, GL330) // #79
    _extension( 43,GL,ARB,occlusion_query2,             GL210, GL330) // #80
    _extension( 44,GL,ARB,sampler_objects,              GL210, GL330) // #81
    _extension( 45,GL,ARB,shader_bit_encoding,    /*?*/ GL210, GL330) // #82
    _extension( 46,GL,ARB,texture_rgb10_a2ui,           GL210, GL330) // #83
    _extension( 47,GL,ARB,texture_swizzle,        /*?*/ GL210, GL330) // #84
    _extension( 48,GL,ARB,timer_query,            /*?*/ GL210, GL330) // #85
    _extension( 49,GL,ARB,vertex_type_2_10_10_10_rev,   GL210, GL330) // #86
    _extension( 50,GL,ARB,draw_indirect,                GL310, GL400) // #87
    _extension( 51,GL,ARB,gpu_shader5,                  GL320, GL400) // #88
    _extension( 52,GL,ARB,gpu_shader_fp64,              GL320, GL400) // #89
    _extension( 53,GL,ARB,shader_subroutine,            GL320, GL400) // #90
    _extension( 54,GL,ARB,tessellation_shader,          GL320, GL400) // #91
    _extension( 55,GL,ARB,texture_buffer_object_rgb32, /*?*/ GL210, GL400) // #92
    _extension( 56,GL,ARB,transform_feedback2,          GL210, GL400) // #93
    _extension( 57,GL,ARB,transform_feedback3,          GL210, GL400) // #94
    _extension( 58,GL,ARB,ES2_compatibility,      /*?*/ GL210, GL410) // #95
    _extension( 59,GL,ARB,get_program_binary,           GL300, GL410) // #96
    _extension( 60,GL,ARB,separate_shader_objects,      GL210, GL410) // #97
    _extension( 61,GL,ARB,shader_precision,             GL400, GL410) // #98
    _extension( 62,GL,ARB,vertex_attrib_64bit,          GL300, GL410) // #99
    _extension( 63,GL,ARB,viewport_array,               GL210, GL410) // #100
    _extension( 64,GL,ARB,robustness,                   GL210,  None) // #105
    _extension( 65,GL,ARB,base_instance,                GL210, GL420) // #107
    _extension( 66,GL,ARB,shading_language_420pack,     GL300, GL420) // #108
    _extension( 67,GL,ARB,transform_feedback_instanced, GL210, GL420) // #109
    _extension( 68,GL,ARB,compressed_texture_pixel_storage, GL210, GL420) // #110
    _extension( 69,GL,ARB,conservative_depth,           GL300, GL420) // #111
    _extension( 70,GL,ARB,internalformat_query,         GL210, GL420) // #112
    _extension( 71,GL,ARB,map_buffer_alignment,         GL210, GL420) // #113
    _extension( 72,GL,ARB,shader_atomic_counters,       GL300, GL420) // #114
    _extension( 73,GL,ARB,shader_image_load_store,      GL300, GL420) // #115
    _extension( 74,GL,ARB,shading_language_packing, /*?*/ GL210, GL420) // #116
    _extension( 75,GL,ARB,texture_storage,              GL210, GL420) // #117
    _extension( 76,GL,ARB,arrays_of_arrays,             GL210, GL430) // #120
    _extension( 77,GL,ARB,clear_buffer_object,          GL210, GL430) // #121
    _extension( 78,GL,ARB,compute_shader,               GL420, GL430) // #122
    _extension( 79,GL,ARB,copy_image,                   GL210, GL430) // #123
    _extension( 80,GL,ARB,texture_view,                 GL210, GL430) // #124
    _extension( 81,GL,ARB,vertex_attrib_binding,        GL210, GL430) // #125
    _extension( 82,GL,ARB,robustness_isolation,         GL210,  None) // #126
    _extension( 83,GL,ARB,robustness_application_isolation, GL210, None) // #126
    _extension( 84,GL,ARB,robustness_share_group_isolation, GL210, None) // #126
    _extension( 85,GL,ARB,ES3_compatibility,            GL330, GL430) // #127
    _extension( 86,GL,ARB,explicit_uniform_location,    GL210, GL430) // #128
    _extension( 87,GL,ARB,fragment_layer_viewport,      GL300, GL430) // #129
    _extension( 88,GL,ARB,framebuffer_no_attachments,   GL210, GL430) // #130
    _extension( 89,GL,ARB,internalformat_query2,        GL210, GL430) // #131
    _extension( 90,GL,ARB,invalidate_subdata,           GL210, GL430) // #132
    _extension( 91,GL,ARB,multi_draw_indirect,          GL310, GL430) // #133
    _extension( 92,GL,ARB,program_interface_query,      GL210, GL430) // #134
    _extension( 93,GL,ARB,robust_buffer_access_behavior,GL210, GL430) // #135
    _extension( 94,GL,ARB,shader_image_size,            GL420, GL430) // #136
    _extension( 95,GL,ARB,shader_storage_buffer_object, GL400, GL430) // #137
    _extension( 96,GL,ARB,stencil_texturing,            GL210, GL430) // #138
    _extension( 97,GL,ARB,texture_buffer_range,         GL210, GL430) // #139
    _extension( 98,GL,ARB,texture_query_levels,         GL300, GL430) // #140
    _extension( 99,GL,ARB,texture_storage_multisample,  GL210, GL430) // #141
    _extension(100,GL,ARB,buffer_storage,         /*?*/ GL430, GL440) // #144
    _extension(101,GL,ARB,clear_texture,                GL210, GL440) // #145
    _extension(102,GL,ARB,enhanced_layouts,             GL310, GL440) // #146
    _extension(103,GL,ARB,multi_bind,                   GL300, GL440) // #147
    _extension(104,GL,ARB,query_buffer_object,          GL210, GL440) // #148
    _extension(105,GL,ARB,texture_mirror_clamp_to_edge, GL210, GL440) // #149
    _extension(106,GL,ARB,texture_stencil8,             GL210, GL440) // #150
    _extension(107,GL,ARB,vertex_type_10f_11f_11f_rev,  GL300, GL440) // #151
    _extension(108,GL,ARB,bindless_texture,             GL400,  None) // #152
    _extension(109,GL,ARB,compute_variable_group_size,  GL420,  None) // #153
    _extension(110,GL,ARB,indirect_parameters,          GL420, GL460) // #154
    _extension(111,GL,ARB,seamless_cubemap_per_texture, GL320,  None) // #155
    _extension(112,GL,ARB,shader_draw_parameters,       GL310, GL460) // #156
    _extension(113,GL,ARB,shader_group_vote,            GL420, GL460) // #157
    _extension(114,GL,ARB,sparse_texture,               GL210,  None) // #158
    _extension(115,GL,ARB,ES3_1_compatibility,          GL440, GL450) // #159
    _extension(116,GL,ARB,clip_control,                 GL210, GL450) // #160
    _extension(117,GL,ARB,conditional_render_inverted,  GL300, GL450) // #161
    _extension(118,GL,ARB,cull_distance,                GL300, GL450) // #162
    _extension(119,GL,ARB,derivative_control,           GL400, GL450) // #163
    _extension(120,GL,ARB,direct_state_access,          GL210, GL450) // #164
    _extension(121,GL,ARB,get_texture_sub_image,        GL210, GL450) // #165
    _extension(122,GL,ARB,shader_texture_image_samples, GL430, GL450) // #166
    _extension(123,GL,ARB,texture_barrier,              GL210, GL450) // #167
    _extension(124,GL,ARB,pipeline_statistics_query,    GL300, GL460) // #171
    _extension(125,GL,ARB,sparse_buffer,                GL210,  None) // #172
    _extension(126,GL,ARB,transform_feedback_overflow_query, GL300, None) // #173
    _extension(127,GL,ARB,ES3_2_compatibility,          GL450,  None) // #177
    _extension(128,GL,ARB,shader_atomic_counter_ops,    GL300, GL460) // #182
    _extension(129,GL,ARB,gl_spirv,                     GL330, GL460) // #190
    _extension(130,GL,ARB,polygon_offset_clamp,         GL330, GL460) // #193
    _extension(131,GL,ARB,spirv_extensions,             GL330, GL460) // #194
    _extension(132,GL,ARB,texture_filter_anisotropic,   GL210, GL460) // #195
} namespace ATI {
    _extension(133,GL,ATI,texture_mirror_once,          GL210,  None) // #221
} namespace EXT {
    _extension(140,GL,EXT,texture_filter_anisotropic,   GL210,  None) // #187
    _extension(141,GL,EXT,texture_compression_s3tc,     GL210,  None) // #198
    /* EXT_framebuffer_object, EXT_packed_depth_stencil, EXT_framebuffer_blit,
       EXT_framebuffer_multisample replaced with ARB_framebuffer_object */
    _extension(142,GL,EXT,texture_mirror_clamp,         GL210,  None) // #298
    _extension(143,GL,EXT,gpu_shader4,                  GL210, GL300) // #326
    _extension(144,GL,EXT,packed_float,                 GL210, GL300) // #328
    _extension(145,GL,EXT,texture_array,                GL210, GL300) // #329
    _extension(146,GL,EXT,texture_compression_rgtc,     GL210, GL300) // #332
    _extension(147,GL,EXT,texture_shared_exponent,      GL210, GL300) // #333
    _extension(148,GL,EXT,draw_buffers2,                GL210, GL300) // #340
    _extension(149,GL,EXT,texture_integer,              GL210, GL300) // #343
    _extension(150,GL,EXT,transform_feedback,           GL210, GL300) // #352
    _extension(151,GL,EXT,direct_state_access,          GL210,  None) // #353
    _extension(152,GL,EXT,texture_snorm,                GL300, GL310) // #365
    _extension(153,GL,EXT,texture_sRGB_decode,          GL210,  None) // #402
    _extension(154,GL,EXT,shader_integer_mix,           GL300,  None) // #437
    _extension(155,GL,EXT,debug_label,                  GL210,  None) // #439
    _extension(156,GL,EXT,debug_marker,                 GL210,  None) // #440
} namespace GREMEDY {
    _extension(157,GL,GREMEDY,string_marker,            GL210,  None) // #311
} namespace KHR {
    _extension(160,GL,KHR,texture_compression_astc_ldr, GL210,  None) // #118
    _extension(161,GL,KHR,texture_compression_astc_hdr, GL210,  None) // #118
    _extension(162,GL,KHR,debug,                        GL210, GL430) // #119
    _extension(163,GL,KHR,context_flush_control,        GL210, GL450) // #168
    _extension(164,GL,KHR,robust_buffer_access_behavior, GL320, None) // #169
    _extension(165,GL,KHR,robustness,                   GL320, GL450) // #170
    _extension(166,GL,KHR,blend_equation_advanced,      GL210,  None) // #174
    _extension(167,GL,KHR,blend_equation_advanced_coherent, GL210, None) // #174
    _extension(168,GL,KHR,no_error,                     GL210,  None) // #175
} namespace NV {
    _extension(169,GL,NV,primitive_restart,             GL210, GL310) // #285
    _extension(170,GL,NV,depth_buffer_float,            GL210, GL300) // #334
    _extension(171,GL,NV,conditional_render,            GL210, GL300) // #346
    /* NV_draw_texture not supported */                               // #430
}
/* IMPORTANT: if this line is > 329 (73 + size), don't forget to update array size in Context.h */
#elif defined(MAGNUM_TARGET_WEBGL)
namespace ANGLE {
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 1,GL,ANGLE,instanced_arrays,        GLES200, GLES300) // #19
    #endif
} namespace EXT {
    _extension( 2,GL,EXT,texture_filter_anisotropic, GLES200,  None) // #11
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 3,GL,EXT,color_buffer_half_float,   GLES200,    None) // #14
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 4,GL,EXT,sRGB,                      GLES200, GLES300) // #17
    _extension( 5,GL,EXT,blend_minmax,              GLES200, GLES300) // #25
    #endif
    _extension( 6,GL,EXT,disjoint_timer_query,      GLES200,    None) // #26
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 7,GL,EXT,shader_texture_lod,        GLES200, GLES300) // #27
    #endif
    _extension( 8,GL,EXT,color_buffer_float,        GLES300,    None) // #31
} namespace OES {
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 9,GL,OES,texture_float,             GLES200, GLES300) // #1
    _extension(10,GL,OES,texture_half_float,        GLES200, GLES300) // #2
    _extension(11,GL,OES,standard_derivatives,      GLES200, GLES300) // #4
    _extension(12,GL,OES,vertex_array_object,       GLES200, GLES300) // #5
    _extension(13,GL,OES,element_index_uint,        GLES200, GLES300) // #10
    #endif
    _extension(14,GL,OES,texture_float_linear,      GLES200,    None) // #20
    #ifdef MAGNUM_TARGET_GLES2
    _extension(15,GL,OES,texture_half_float_linear, GLES200, GLES300) // #21
    _extension(16,GL,OES,fbo_render_mipmap,         GLES200, GLES300) // #28
    #endif
} namespace WEBGL {
    _extension(17,GL,WEBGL,compressed_texture_s3tc, GLES200,   None) // #8
    #ifdef MAGNUM_TARGET_GLES2
    _extension(18,GL,WEBGL,depth_texture,           GLES200, GLES300) // #9
    _extension(19,GL,WEBGL,color_buffer_float,      GLES200,    None) // #14
    _extension(20,GL,WEBGL,draw_buffers,            GLES200, GLES300) // #18
    #endif
}
#else
namespace ANDROID {
    #ifndef MAGNUM_TARGET_GLES2
    _extension(  1,GL,ANDROID,extension_pack_es31a, GLES310,    None) // #187
    #endif
} namespace ANGLE {
    #ifdef MAGNUM_TARGET_GLES2
    _extension(  2,GL,ANGLE,framebuffer_blit,       GLES200, GLES300) // #83
    _extension(  3,GL,ANGLE,framebuffer_multisample, GLES200, GLES300) // #84
    _extension(  4,GL,ANGLE,instanced_arrays,       GLES200, GLES300) // #109
    _extension(  5,GL,ANGLE,depth_texture,          GLES200, GLES300) // #138
    #endif
} namespace APPLE {
    #ifdef MAGNUM_TARGET_GLES2
    _extension(  6,GL,APPLE,framebuffer_multisample, GLES200, GLES300) // #78
    #endif
    _extension(  7,GL,APPLE,texture_format_BGRA8888, GLES200,   None) // #79
    #ifdef MAGNUM_TARGET_GLES2
    _extension(  8,GL,APPLE,texture_max_level,      GLES200,    None) // #80
    #endif
} namespace ARM {
    #ifdef MAGNUM_TARGET_GLES2
    _extension(  9,GL,ARM,rgba8,                    GLES200, GLES300) // #82
    #endif
    _extension( 10,GL,ARM,shader_framebuffer_fetch, GLES200,    None) // #165
    _extension( 11,GL,ARM,shader_framebuffer_fetch_depth_stencil, GLES200, None) // #166
} namespace EXT {
    _extension( 20,GL,EXT,texture_filter_anisotropic, GLES200,  None) // #41
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 21,GL,EXT,texture_type_2_10_10_10_REV, GLES200, GLES300) // #42
    #endif
    _extension( 22,GL,EXT,texture_format_BGRA8888,  GLES200,    None) // #51
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 23,GL,EXT,discard_framebuffer,      GLES200, GLES300) // #64
    _extension( 24,GL,EXT,blend_minmax,             GLES200, GLES300) // #65
    #endif
    _extension( 25,GL,EXT,read_format_bgra,         GLES200,    None) // #66
    _extension( 26,GL,EXT,multi_draw_arrays,        GLES200,    None) // #67
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 27,GL,EXT,shader_texture_lod,       GLES200, GLES300) // #77
    _extension( 28,GL,EXT,unpack_subimage,          GLES200, GLES300) // #90
    #endif
    _extension( 29,GL,EXT,color_buffer_half_float,  GLES200, GLES320) // #97
    _extension( 30,GL,EXT,debug_label,              GLES200,    None) // #98
    _extension( 31,GL,EXT,debug_marker,             GLES200,    None) // #99
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 32,GL,EXT,occlusion_query_boolean,  GLES200, GLES300) // #100
    #endif
    _extension( 33,GL,EXT,separate_shader_objects,  GLES200,    None) // #101
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 34,GL,EXT,shadow_samplers,          GLES200, GLES300) // #102
    _extension( 35,GL,EXT,texture_rg,               GLES200, GLES300) // #103
    _extension( 36,GL,EXT,sRGB,                     GLES200, GLES300) // #105
    #endif
    _extension( 37,GL,EXT,multisampled_render_to_texture, GLES200, None) // #106
    _extension( 38,GL,EXT,robustness,               GLES200,    None) // #107
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 39,GL,EXT,texture_storage,          GLES200, GLES300) // #108
    _extension( 40,GL,EXT,map_buffer_range,         GLES200, GLES300) // #121
    #endif
    _extension( 41,GL,EXT,shader_framebuffer_fetch, GLES200,    None) // #122
    #ifndef MAGNUM_TARGET_GLES2
    _extension( 42,GL,EXT,color_buffer_float,       GLES300, GLES320) // #137
    #endif
    _extension( 43,GL,EXT,disjoint_timer_query,     GLES200,    None) // #150
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 44,GL,EXT,draw_buffers,             GLES200, GLES300) // #151
    #endif
    _extension( 45,GL,EXT,texture_sRGB_decode,      GLES200,    None) // #152
    _extension( 46,GL,EXT,sRGB_write_control,       GLES200,    None) // #153
    _extension( 47,GL,EXT,texture_compression_s3tc, GLES200,    None) // #154
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 48,GL,EXT,instanced_arrays,         GLES200, GLES300) // #156
    _extension( 49,GL,EXT,draw_instanced,           GLES200, GLES300) // #157
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension( 50,GL,EXT,shader_integer_mix,       GLES300,    None) // #161
    _extension( 51,GL,EXT,copy_image,               GLES300, GLES320) // #175
    #endif
    _extension( 52,GL,EXT,draw_buffers_indexed,     GLES200, GLES320) // #176
    #ifndef MAGNUM_TARGET_GLES2
    _extension( 53,GL,EXT,geometry_shader,          GLES310, GLES320) // #177
    _extension( 54,GL,EXT,gpu_shader5,              GLES310, GLES320) // #178
    _extension( 55,GL,EXT,shader_io_blocks,         GLES310, GLES320) // #180
    _extension( 56,GL,EXT,tessellation_shader,      GLES310, GLES320) // #181
    #endif
    _extension( 57,GL,EXT,texture_border_clamp,     GLES200, GLES320) // #182
    #ifndef MAGNUM_TARGET_GLES2
    _extension( 58,GL,EXT,texture_buffer,           GLES310, GLES320) // #183
    _extension( 59,GL,EXT,texture_cube_map_array,   GLES310, GLES320) // #184
    _extension( 60,GL,EXT,primitive_bounding_box,   GLES310, GLES320) // #186
    #endif
    _extension( 61,GL,EXT,polygon_offset_clamp,     GLES200,    None) // #252
} namespace KHR {
    _extension( 70,GL,KHR,texture_compression_astc_ldr, GLES200, GLES320) // #117
    _extension( 71,GL,KHR,texture_compression_astc_hdr, GLES200, None) // #117
    _extension( 72,GL,KHR,debug,                    GLES200, GLES320) // #118
    _extension( 73,GL,KHR,blend_equation_advanced,  GLES200, GLES320) // #168
    _extension( 74,GL,KHR,blend_equation_advanced_coherent, GLES200, None) // #168
    _extension( 75,GL,KHR,robustness,               GLES200, GLES320) // #170
    _extension( 76,GL,KHR,robust_buffer_access_behavior, GLES200, GLES320) // #189
    _extension( 77,GL,KHR,context_flush_control,    GLES200,    None) // #191
    _extension( 78,GL,KHR,no_error,                 GLES200,    None) // #243
} namespace NV {
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 80,GL,NV,draw_buffers,              GLES200, GLES300) // #91
    _extension( 81,GL,NV,fbo_color_attachments,     GLES200, GLES300) // #92
    _extension( 82,GL,NV,read_buffer,               GLES200, GLES300) // #93
    #endif
    _extension( 83,GL,NV,read_buffer_front,         GLES200,    None) // #93
    _extension( 84,GL,NV,read_depth,                GLES200,    None) // #94
    _extension( 85,GL,NV,read_stencil,              GLES200,    None) // #94
    _extension( 86,GL,NV,read_depth_stencil,        GLES200,    None) // #94
    #ifdef MAGNUM_TARGET_GLES2
    _extension( 87,GL,NV,pack_subimage,             GLES200, GLES300) // #132
    _extension( 88,GL,NV,draw_instanced,            GLES200, GLES300) // #141
    _extension( 89,GL,NV,framebuffer_blit,          GLES200, GLES300) // #142
    _extension( 90,GL,NV,framebuffer_multisample,   GLES200, GLES300) // #143
    _extension( 91,GL,NV,instanced_arrays,          GLES200, GLES300) // #145
    _extension( 92,GL,NV,shadow_samplers_array,     GLES200, GLES300) // #146
    _extension( 93,GL,NV,shadow_samplers_cube,      GLES200, GLES300) // #147
    #endif
    _extension( 94,GL,NV,texture_border_clamp,      GLES200,    None) // #149
    #ifndef MAGNUM_TARGET_GLES2
    _extension( 95,GL,NV,shader_noperspective_interpolation, GLES300, None) // #201
    #endif
    _extension( 96,GL,NV,polygon_mode,              GLES200,    None) // #238
} namespace OES {
    #ifdef MAGNUM_TARGET_GLES2
    _extension(100,GL,OES,depth24,                  GLES200, GLES300) // #24
    #endif
    _extension(101,GL,OES,depth32,                  GLES200,    None) // #25
    #ifdef MAGNUM_TARGET_GLES2
    _extension(102, GL,OES,element_index_uint,       GLES200, GLES300) // #26
    _extension(103,GL,OES,fbo_render_mipmap,        GLES200, GLES300) // #27
    #endif
    _extension(104,GL,OES,mapbuffer,                GLES200,    None) // #29
    #ifdef MAGNUM_TARGET_GLES2
    _extension(105,GL,OES,rgb8_rgba8,               GLES200, GLES300) // #30
    #endif
    _extension(106,GL,OES,stencil1,                 GLES200,    None) // #31
    _extension(107,GL,OES,stencil4,                 GLES200,    None) // #32
    #ifdef MAGNUM_TARGET_GLES2
    _extension(108,GL,OES,texture_3D,               GLES200, GLES300) // #34
    _extension(109,GL,OES,texture_half_float_linear, GLES200, GLES300) // #35
    #endif
    _extension(110,GL,OES,texture_float_linear,     GLES200,    None) // #35
    #ifdef MAGNUM_TARGET_GLES2
    _extension(111,GL,OES,texture_half_float,       GLES200, GLES300) // #36
    _extension(112,GL,OES,texture_float,            GLES200, GLES300) // #36
    _extension(113,GL,OES,texture_npot,             GLES200, GLES300) // #37
    _extension(114,GL,OES,vertex_half_float,        GLES200, GLES300) // #38
    _extension(115,GL,OES,packed_depth_stencil,     GLES200, GLES300) // #43
    _extension(116,GL,OES,depth_texture,            GLES200, GLES300) // #44
    _extension(117,GL,OES,standard_derivatives,     GLES200, GLES300) // #45
    _extension(118,GL,OES,vertex_array_object,      GLES200, GLES300) // #71
    _extension(119,GL,OES,required_internalformat,  GLES200, GLES300) // #115
    _extension(120,GL,OES,surfaceless_context,      GLES200, GLES300) // #116
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    _extension(121,GL,OES,sample_shading,           GLES300, GLES320) // #169
    _extension(122,GL,OES,sample_variables,         GLES300, GLES320) // #170
    _extension(123,GL,OES,shader_image_atomic,      GLES310, GLES320) // #171
    _extension(124,GL,OES,shader_multisample_interpolation, GLES300, GLES320) // #172
    #endif
    _extension(125,GL,OES,texture_stencil8,         GLES200, GLES320) // #173
    #ifndef MAGNUM_TARGET_GLES2
    _extension(126,GL,OES,texture_storage_multisample_2d_array, GLES310, GLES320) // #174
    #endif
}
#endif

}
#undef _extension
#endif

}

}

#endif
