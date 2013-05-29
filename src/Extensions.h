#ifndef Magnum_Extensions_h
#define Magnum_Extensions_h
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

/** @file
 * @brief Namespace Magnum::Extensions
 */

#include "Context.h"

namespace Magnum {

/**
@brief Compile-time information about OpenGL extensions

Each extension is `struct` named hierarchically by prefix, vendor and
extension name, for example `GL::APPLE::vertex_array_object`. Each struct has
the same public methods as Extension class (requiredVersion(), coreVersion()
and string(), but these structs are better suited for compile-time decisions
rather than %Extension instances. See Context::isExtensionSupported() for
example usage.

This namespace is built by default and found by default in CMake. See
@ref building and @ref cmake for more information.
@see MAGNUM_ASSERT_EXTENSION_SUPPORTED()
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
        _extension(GL,AMD,vertex_shader_layer,          GL210,  None) // #417
        _extension(GL,AMD,shader_trinary_minmax,        GL210,  None) // #428
    } namespace APPLE {
        _extension(GL,APPLE,flush_buffer_range,         GL210, GL300) // #321
        _extension(GL,APPLE,vertex_array_object,        GL210, GL300) // #273
    } namespace ARB {
        _extension(GL,ARB,texture_rectangle,            GL210, GL310) // #38
        _extension(GL,ARB,color_buffer_float,           GL210, GL300) // #39
        _extension(GL,ARB,half_float_pixel,             GL210, GL300) // #40
        _extension(GL,ARB,texture_float,                GL210, GL300) // #41
        _extension(GL,ARB,depth_buffer_float,           GL210, GL300) // #43
        _extension(GL,ARB,draw_instanced,               GL210, GL310) // #44
        _extension(GL,ARB,geometry_shader4,             GL210, GL320) // #47
        _extension(GL,ARB,instanced_arrays,             GL210, GL330) // #49
        _extension(GL,ARB,map_buffer_range,             GL210, GL300) // #50
        _extension(GL,ARB,texture_buffer_object,        GL210, GL310) // #51
        _extension(GL,ARB,texture_rg,                   GL210, GL300) // #53
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
        _extension(GL,ARB,texture_storage,              GL210, GL420) // #117
        _extension(GL,ARB,arrays_of_arrays,             GL210, GL430) // #120
        _extension(GL,ARB,clear_buffer_object,          GL210, GL430) // #121
        _extension(GL,ARB,compute_shader,               GL420, GL430) // #122
        _extension(GL,ARB,copy_image,                   GL210, GL430) // #123
        _extension(GL,ARB,texture_view,                 GL210, GL430) // #124
        _extension(GL,ARB,vertex_attrib_binding,        GL210, GL430) // #125
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
    } namespace EXT {
        _extension(GL,EXT,texture_filter_anisotropic,   GL210,  None) // #187
        _extension(GL,EXT,framebuffer_object,           GL210, GL300) // #310
        _extension(GL,EXT,packed_depth_stencil,         GL210, GL300) // #312
        _extension(GL,EXT,framebuffer_blit,             GL210, GL300) // #316
        _extension(GL,EXT,framebuffer_multisample,      GL210, GL300) // #317
        _extension(GL,EXT,gpu_shader4,                  GL210, GL300) // #326
        _extension(GL,EXT,packed_float,                 GL210, GL300) // #328
        _extension(GL,EXT,texture_array,                GL210, GL300) // #329
        _extension(GL,EXT,texture_compression_rgtc,     GL210, GL300) // #332
        _extension(GL,EXT,texture_shared_exponent,      GL210, GL300) // #333
        _extension(GL,EXT,framebuffer_sRGB,             GL210, GL300) // #337
        _extension(GL,EXT,draw_buffers2,                GL210, GL300) // #340
        _extension(GL,EXT,texture_integer,              GL210, GL300) // #343
        _extension(GL,EXT,transform_feedback,           GL210, GL300) // #352
        _extension(GL,EXT,direct_state_access,          GL210,  None) // #353
        _extension(GL,EXT,texture_snorm,                GL300, GL310) // #365
    } namespace GREMEDY {
        _extension(GL,GREMEDY,string_marker,            GL210,  None) // #311
    } namespace INTEL {
        /* INTEL_map_texture not supported */                         // #429
    } namespace KHR {
        _extension(GL,KHR,debug,                        GL210, GL430) // #119
    } namespace NV {
        _extension(GL,NV,half_float,                    GL210, GL300) // #283
        _extension(GL,NV,primitive_restart,             GL210, GL310) // #285
        _extension(GL,NV,depth_buffer_float,            GL210, GL300) // #334
        _extension(GL,NV,conditional_render,            GL210, GL300) // #346
        /* NV_draw_texture not supported */                           // #430
    }
    /* IMPORTANT: if this line is > 188 (57 + size), don't forget to update array size in Context.h */
    #else
    #line 1
    namespace ANGLE {
        _extension(GL,ANGLE,framebuffer_blit,       GLES200, GLES300) // #83
        _extension(GL,ANGLE,framebuffer_multisample, GLES200, GLES300) // #84
        _extension(GL,ANGLE,depth_texture,          GLES200, GLES300) // #138
    } namespace APPLE {
        _extension(GL,APPLE,framebuffer_multisample, GLES200, GLES300) // #78
        _extension(GL,APPLE,texture_format_BGRA8888, GLES200,   None) // #79
    } namespace ARM {
        _extension(GL,ARM,rgba8,                    GLES200, GLES300) // #82
    } namespace EXT {
        _extension(GL,EXT,texture_filter_anisotropic, GLES200,  None) // #41
        _extension(GL,EXT,texture_type_2_10_10_10_REV, GLES200, GLES300) // #42
        _extension(GL,EXT,texture_format_BGRA8888,  GLES200,    None) // #51
        _extension(GL,EXT,discard_framebuffer,      GLES200, GLES300) // #64
        _extension(GL,EXT,blend_minmax,             GLES200, GLES300) // #65
        _extension(GL,EXT,read_format_bgra,         GLES200,    None) // #66
        _extension(GL,EXT,debug_marker,             GLES200,    None) // #99
        _extension(GL,EXT,occlusion_query_boolean,  GLES200, GLES300) // #100
        _extension(GL,EXT,separate_shader_objects,  GLES200,    None) // #101
        _extension(GL,EXT,texture_rg,               GLES200, GLES300) // #103
        _extension(GL,EXT,sRGB,                     GLES200,    None) // #105
        _extension(GL,EXT,robustness,               GLES200,    None) // #105
        _extension(GL,EXT,texture_storage,          GLES200, GLES300) // #108
        _extension(GL,EXT,map_buffer_range,         GLES200, GLES300) // #121
        _extension(GL,EXT,disjoint_timer_query,     GLES200,    None) // #150
    } namespace NV {
        _extension(GL,NV,draw_buffers,              GLES200, GLES300) // #91
        _extension(GL,NV,read_buffer,               GLES200, GLES300) // #93
        _extension(GL,NV,read_buffer_front,         GLES200,    None) // #93
        _extension(GL,NV,read_depth,                GLES200, GLES300) // #94
        _extension(GL,NV,read_stencil,              GLES200,    None) // #94
        _extension(GL,NV,read_depth_stencil,        GLES200, GLES300) // #94
        _extension(GL,NV,framebuffer_blit,          GLES200, GLES300) // #142
        _extension(GL,NV,framebuffer_multisample,   GLES200, GLES300) // #143
        _extension(GL,NV,texture_border_clamp,      GLES200,    None) // #149
    } namespace OES {
        _extension(GL,OES,depth24,                  GLES200, GLES300) // #24
        _extension(GL,OES,depth32,                  GLES200,    None) // #25
        _extension(GL,OES,element_index_uint,       GLES200, GLES300) // #26
        _extension(GL,OES,mapbuffer,                GLES200,    None) // #29
        _extension(GL,OES,rgb8_rgba8,               GLES200, GLES300) // #30
        _extension(GL,OES,stencil1,                 GLES200,    None) // #31
        _extension(GL,OES,stencil4,                 GLES200,    None) // #32
        _extension(GL,OES,texture_3D,               GLES200,    None) // #34
        _extension(GL,OES,texture_half_float_linear, GLES200, GLES300) // #35
        _extension(GL,OES,texture_float_linear,     GLES200, GLES300) // #35
        _extension(GL,OES,texture_half_float,       GLES200, GLES300) // #36
        _extension(GL,OES,texture_float,            GLES200, GLES300) // #36
        _extension(GL,OES,vertex_half_float,        GLES200, GLES300) // #38
        _extension(GL,OES,packed_depth_stencil,     GLES200, GLES300) // #43
        _extension(GL,OES,depth_texture,            GLES200, GLES300) // #44
        _extension(GL,OES,standard_derivatives,     GLES200, GLES300) // #45
        _extension(GL,OES,vertex_array_object,      GLES200, GLES300) // #71
        _extension(GL,OES,required_internalformat,  GLES200, GLES300) // #?
    }
    #endif
}
#undef _extension
#endif

}

}

#endif
