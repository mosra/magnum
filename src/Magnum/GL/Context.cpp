/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Debug.h>

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

using namespace Containers::Literals;

/* When adding a new list, Extension::extensions() and Context::Context() needs
   to be adapted. Binary search is performed on the extensions, thus they have
   to be sorted alphabetically. */
namespace {

#ifndef MAGNUM_TARGET_GLES
constexpr Extension ExtensionList[]{
    Extensions::AMD::sample_positions{},
    Extensions::AMD::shader_explicit_vertex_parameter{},
    Extensions::AMD::shader_trinary_minmax{},
    Extensions::AMD::transform_feedback3_lines_triangles{},
    Extensions::AMD::vertex_shader_layer{},
    Extensions::ARB::ES3_2_compatibility{},
    Extensions::ARB::bindless_texture{},
    Extensions::ARB::compute_variable_group_size{},
    Extensions::ARB::robustness{},
    Extensions::ARB::robustness_application_isolation{},
    Extensions::ARB::robustness_isolation{},
    Extensions::ARB::robustness_share_group_isolation{},
    Extensions::ARB::sample_locations{},
    Extensions::ARB::seamless_cubemap_per_texture{},
    Extensions::ARB::sparse_buffer{},
    Extensions::ARB::sparse_texture{},
    Extensions::ATI::texture_mirror_once{},
    Extensions::EXT::debug_label{},
    Extensions::EXT::debug_marker{},
    Extensions::EXT::shader_integer_mix{},
    Extensions::EXT::texture_compression_dxt1{},
    Extensions::EXT::texture_compression_s3tc{},
    Extensions::EXT::texture_filter_anisotropic{},
    Extensions::EXT::texture_mirror_clamp{},
    Extensions::EXT::texture_sRGB_R8{},
    Extensions::EXT::texture_sRGB_decode{},
    Extensions::GREMEDY::string_marker{},
    Extensions::KHR::blend_equation_advanced{},
    Extensions::KHR::blend_equation_advanced_coherent{},
    Extensions::KHR::texture_compression_astc_hdr{},
    Extensions::KHR::texture_compression_astc_ldr{},
    Extensions::KHR::texture_compression_astc_sliced_3d{},
    Extensions::NV::fragment_shader_barycentric{},
    Extensions::NV::sample_locations{},
    Extensions::OVR::multiview{},
    Extensions::OVR::multiview2{}
};
constexpr Extension ExtensionList300[]{
    Extensions::ARB::color_buffer_float{},
    Extensions::ARB::depth_buffer_float{},
    Extensions::ARB::framebuffer_object{},
    Extensions::ARB::framebuffer_sRGB{},
    Extensions::ARB::half_float_pixel{},
    Extensions::ARB::half_float_vertex{},
    Extensions::ARB::map_buffer_range{},
    Extensions::ARB::texture_float{},
    Extensions::ARB::texture_rg{},
    Extensions::ARB::vertex_array_object{},
    Extensions::EXT::draw_buffers2{},
    Extensions::EXT::gpu_shader4{},
    Extensions::EXT::packed_float{},
    Extensions::EXT::texture_array{},
    Extensions::EXT::texture_compression_rgtc{},
    Extensions::EXT::texture_integer{},
    Extensions::EXT::texture_shared_exponent{},
    Extensions::EXT::transform_feedback{},
    Extensions::MAGNUM::shader_vertex_id{},
    Extensions::NV::conditional_render{},
    Extensions::NV::depth_buffer_float{}
};
constexpr Extension ExtensionList310[]{
    Extensions::ARB::copy_buffer{},
    Extensions::ARB::draw_instanced{},
    Extensions::ARB::texture_buffer_object{},
    Extensions::ARB::texture_rectangle{},
    Extensions::ARB::uniform_buffer_object{},
    Extensions::EXT::texture_snorm{},
    Extensions::NV::primitive_restart{}
};
constexpr Extension ExtensionList320[]{
    Extensions::ARB::depth_clamp{},
    Extensions::ARB::draw_elements_base_vertex{},
    Extensions::ARB::fragment_coord_conventions{},
    Extensions::ARB::geometry_shader4{},
    Extensions::ARB::provoking_vertex{},
    Extensions::ARB::seamless_cube_map{},
    Extensions::ARB::sync{},
    Extensions::ARB::texture_multisample{},
    Extensions::ARB::vertex_array_bgra{}
};
constexpr Extension ExtensionList330[]{
    Extensions::ARB::blend_func_extended{},
    Extensions::ARB::explicit_attrib_location{},
    Extensions::ARB::instanced_arrays{},
    Extensions::ARB::occlusion_query2{},
    Extensions::ARB::sampler_objects{},
    Extensions::ARB::shader_bit_encoding{},
    Extensions::ARB::texture_rgb10_a2ui{},
    Extensions::ARB::texture_swizzle{},
    Extensions::ARB::timer_query{},
    Extensions::ARB::vertex_type_2_10_10_10_rev{}
};
constexpr Extension ExtensionList400[]{
    Extensions::ARB::draw_buffers_blend{},
    Extensions::ARB::draw_indirect{},
    Extensions::ARB::gpu_shader5{},
    Extensions::ARB::gpu_shader_fp64{},
    Extensions::ARB::sample_shading{},
    Extensions::ARB::shader_subroutine{},
    Extensions::ARB::tessellation_shader{},
    Extensions::ARB::texture_buffer_object_rgb32{},
    Extensions::ARB::texture_cube_map_array{},
    Extensions::ARB::texture_gather{},
    Extensions::ARB::texture_query_lod{},
    Extensions::ARB::transform_feedback2{},
    Extensions::ARB::transform_feedback3{}
};
constexpr Extension ExtensionList410[]{
    Extensions::ARB::ES2_compatibility{},
    Extensions::ARB::get_program_binary{},
    Extensions::ARB::separate_shader_objects{},
    Extensions::ARB::shader_precision{},
    Extensions::ARB::vertex_attrib_64bit{},
    Extensions::ARB::viewport_array{}
};
constexpr Extension ExtensionList420[]{
    Extensions::ARB::base_instance{},
    Extensions::ARB::compressed_texture_pixel_storage{},
    Extensions::ARB::conservative_depth{},
    Extensions::ARB::internalformat_query{},
    Extensions::ARB::map_buffer_alignment{},
    Extensions::ARB::shader_atomic_counters{},
    Extensions::ARB::shader_image_load_store{},
    Extensions::ARB::shading_language_420pack{},
    /* Mentioned in GLSL 4.20 specs as newly added */
    Extensions::ARB::shading_language_packing{},
    Extensions::ARB::texture_compression_bptc{},
    Extensions::ARB::texture_storage{},
    Extensions::ARB::transform_feedback_instanced{}
};
constexpr Extension ExtensionList430[]{
    Extensions::ARB::ES3_compatibility{},
    Extensions::ARB::arrays_of_arrays{},
    Extensions::ARB::clear_buffer_object{},
    Extensions::ARB::compute_shader{},
    Extensions::ARB::copy_image{},
    Extensions::ARB::explicit_uniform_location{},
    Extensions::ARB::fragment_layer_viewport{},
    Extensions::ARB::framebuffer_no_attachments{},
    Extensions::ARB::internalformat_query2{},
    Extensions::ARB::invalidate_subdata{},
    Extensions::ARB::multi_draw_indirect{},
    Extensions::ARB::program_interface_query{},
    Extensions::ARB::robust_buffer_access_behavior{},
    Extensions::ARB::shader_image_size{},
    Extensions::ARB::shader_storage_buffer_object{},
    Extensions::ARB::stencil_texturing{},
    Extensions::ARB::texture_buffer_range{},
    Extensions::ARB::texture_query_levels{},
    Extensions::ARB::texture_storage_multisample{},
    Extensions::ARB::texture_view{},
    Extensions::ARB::vertex_attrib_binding{},
    Extensions::KHR::debug{}
};
constexpr Extension ExtensionList440[]{
    Extensions::ARB::buffer_storage{},
    Extensions::ARB::clear_texture{},
    Extensions::ARB::enhanced_layouts{},
    Extensions::ARB::multi_bind{},
    Extensions::ARB::query_buffer_object{},
    Extensions::ARB::texture_mirror_clamp_to_edge{},
    Extensions::ARB::texture_stencil8{},
    Extensions::ARB::vertex_type_10f_11f_11f_rev{}
};
constexpr Extension ExtensionList450[]{
    Extensions::ARB::ES3_1_compatibility{},
    Extensions::ARB::clip_control{},
    Extensions::ARB::conditional_render_inverted{},
    Extensions::ARB::cull_distance{},
    Extensions::ARB::derivative_control{},
    Extensions::ARB::direct_state_access{},
    Extensions::ARB::get_texture_sub_image{},
    Extensions::ARB::shader_texture_image_samples{},
    Extensions::ARB::texture_barrier{},
    Extensions::KHR::context_flush_control{},
    Extensions::KHR::robustness{}
};
constexpr Extension ExtensionList460[]{
    Extensions::ARB::gl_spirv{},
    Extensions::ARB::indirect_parameters{},
    Extensions::ARB::pipeline_statistics_query{},
    Extensions::ARB::polygon_offset_clamp{},
    Extensions::ARB::shader_atomic_counter_ops{},
    Extensions::ARB::shader_draw_parameters{},
    Extensions::ARB::shader_group_vote{},
    Extensions::ARB::spirv_extensions{},
    Extensions::ARB::texture_filter_anisotropic{},
    Extensions::ARB::transform_feedback_overflow_query{},
    Extensions::KHR::no_error{}
};
#elif defined(MAGNUM_TARGET_WEBGL)
constexpr Extension ExtensionList[]{
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::clip_cull_distance{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::color_buffer_float{},
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    Extensions::EXT::disjoint_timer_query{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::disjoint_timer_query_webgl2{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::draw_buffers_indexed{},
    #endif
    Extensions::EXT::float_blend{},
    Extensions::EXT::texture_compression_bptc{},
    Extensions::EXT::texture_compression_rgtc{},
    Extensions::EXT::texture_filter_anisotropic{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::texture_norm16{},
    #endif
    Extensions::OES::texture_float_linear{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::OVR::multiview2{},
    #endif
    Extensions::WEBGL::blend_equation_advanced_coherent{},
    Extensions::WEBGL::compressed_texture_astc{},
    Extensions::WEBGL::compressed_texture_pvrtc{},
    Extensions::WEBGL::compressed_texture_s3tc{},
    Extensions::WEBGL::compressed_texture_s3tc_srgb{},
    Extensions::WEBGL::debug_renderer_info{},
    Extensions::WEBGL::debug_shaders{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::WEBGL::draw_instanced_base_vertex_base_instance{},
    #endif
    Extensions::WEBGL::lose_context{},
    Extensions::WEBGL::multi_draw{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance{},
    #endif
};
constexpr Extension ExtensionListES300[]{
    #ifdef MAGNUM_TARGET_GLES2
    Extensions::ANGLE::instanced_arrays{},
    Extensions::EXT::blend_minmax{},
    Extensions::EXT::color_buffer_half_float{},
    Extensions::EXT::frag_depth{},
    Extensions::EXT::sRGB{},
    Extensions::EXT::shader_texture_lod{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::MAGNUM::shader_vertex_id{},
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    Extensions::OES::element_index_uint{},
    Extensions::OES::fbo_render_mipmap{},
    Extensions::OES::standard_derivatives{},
    Extensions::OES::texture_float{},
    Extensions::OES::texture_half_float{},
    Extensions::OES::texture_half_float_linear{},
    Extensions::OES::vertex_array_object{},
    Extensions::WEBGL::color_buffer_float{},
    Extensions::WEBGL::depth_texture{},
    Extensions::WEBGL::draw_buffers{},
    #endif
};
#else
constexpr Extension ExtensionList[]{
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::ANDROID::extension_pack_es31a{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::ANGLE::base_vertex_base_instance{},
    #endif
    Extensions::ANGLE::multi_draw{},
    Extensions::ANGLE::texture_compression_dxt1{},
    Extensions::ANGLE::texture_compression_dxt3{},
    Extensions::ANGLE::texture_compression_dxt5{},
    Extensions::APPLE::clip_distance{},
    Extensions::APPLE::texture_format_BGRA8888{},
    Extensions::ARM::shader_framebuffer_fetch{},
    Extensions::ARM::shader_framebuffer_fetch_depth_stencil{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::clip_cull_distance{},
    #endif
    Extensions::EXT::debug_label{},
    Extensions::EXT::debug_marker{},
    Extensions::EXT::disjoint_timer_query{},
    Extensions::EXT::draw_elements_base_vertex{},
    Extensions::EXT::multi_draw_arrays{},
    Extensions::EXT::multisampled_render_to_texture{},
    Extensions::EXT::polygon_offset_clamp{},
    Extensions::EXT::pvrtc_sRGB{},
    Extensions::EXT::read_format_bgra{},
    Extensions::EXT::robustness{},
    Extensions::EXT::sRGB_write_control{},
    Extensions::EXT::separate_shader_objects{},
    Extensions::EXT::shader_framebuffer_fetch{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::shader_integer_mix{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::texture_compression_bptc{},
    #endif
    Extensions::EXT::texture_compression_dxt1{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::texture_compression_rgtc{},
    #endif
    Extensions::EXT::texture_compression_s3tc{},
    Extensions::EXT::texture_compression_s3tc_srgb{},
    Extensions::EXT::texture_filter_anisotropic{},
    Extensions::EXT::texture_format_BGRA8888{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::texture_norm16{},
    Extensions::EXT::texture_sRGB_R8{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::texture_sRGB_RG8{},
    #endif
    Extensions::EXT::texture_sRGB_decode{},
    Extensions::IMG::texture_compression_pvrtc{},
    Extensions::KHR::blend_equation_advanced_coherent{},
    Extensions::KHR::context_flush_control{},
    Extensions::KHR::no_error{},
    Extensions::KHR::texture_compression_astc_hdr{},
    Extensions::KHR::texture_compression_astc_sliced_3d{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::NV::fragment_shader_barycentric{},
    #endif
    Extensions::NV::polygon_mode{},
    Extensions::NV::read_buffer_front{},
    Extensions::NV::read_depth{},
    Extensions::NV::read_depth_stencil{},
    Extensions::NV::read_stencil{},
    Extensions::NV::sample_locations{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::NV::shader_noperspective_interpolation{},
    #endif
    Extensions::NV::texture_border_clamp{},
    Extensions::OES::depth32{},
    Extensions::OES::draw_elements_base_vertex{},
    Extensions::OES::mapbuffer{},
    Extensions::OES::stencil1{},
    Extensions::OES::stencil4{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::OES::texture_compression_astc{},
    #endif
    Extensions::OES::texture_float_linear{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::OVR::multiview{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::OVR::multiview2{},
    #endif
    };
constexpr Extension ExtensionListES300[]{
    #ifdef MAGNUM_TARGET_GLES2
    Extensions::ANGLE::depth_texture{},
    Extensions::ANGLE::framebuffer_blit{},
    Extensions::ANGLE::framebuffer_multisample{},
    Extensions::ANGLE::instanced_arrays{},
    Extensions::APPLE::framebuffer_multisample{},
    Extensions::APPLE::texture_max_level{},
    Extensions::ARM::rgba8{},
    Extensions::EXT::blend_minmax{},
    Extensions::EXT::discard_framebuffer{},
    Extensions::EXT::draw_buffers{},
    Extensions::EXT::draw_instanced{},
    Extensions::EXT::instanced_arrays{},
    Extensions::EXT::map_buffer_range{},
    Extensions::EXT::occlusion_query_boolean{},
    Extensions::EXT::sRGB{},
    Extensions::EXT::shader_texture_lod{},
    Extensions::EXT::shadow_samplers{},
    Extensions::EXT::texture_rg{},
    Extensions::EXT::texture_storage{},
    Extensions::EXT::texture_type_2_10_10_10_REV{},
    Extensions::EXT::unpack_subimage{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::MAGNUM::shader_vertex_id{},
    #endif
    #ifdef MAGNUM_TARGET_GLES2
    Extensions::NV::draw_buffers{},
    Extensions::NV::draw_instanced{},
    Extensions::NV::fbo_color_attachments{},
    Extensions::NV::framebuffer_blit{},
    Extensions::NV::framebuffer_multisample{},
    Extensions::NV::instanced_arrays{},
    Extensions::NV::pack_subimage{},
    Extensions::NV::read_buffer{},
    Extensions::NV::shadow_samplers_array{},
    Extensions::NV::shadow_samplers_cube{},
    Extensions::OES::depth24{},
    Extensions::OES::depth_texture{},
    Extensions::OES::element_index_uint{},
    Extensions::OES::fbo_render_mipmap{},
    Extensions::OES::packed_depth_stencil{},
    Extensions::OES::required_internalformat{},
    Extensions::OES::rgb8_rgba8{},
    Extensions::OES::standard_derivatives{},
    Extensions::OES::surfaceless_context{},
    Extensions::OES::texture_3D{},
    Extensions::OES::texture_float{},
    Extensions::OES::texture_half_float{},
    Extensions::OES::texture_half_float_linear{},
    Extensions::OES::texture_npot{},
    Extensions::OES::vertex_array_object{},
    Extensions::OES::vertex_half_float{},
    #endif
    };
constexpr Extension ExtensionListES320[]{
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::color_buffer_float{},
    #endif
    Extensions::EXT::color_buffer_half_float{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::copy_image{},
    #endif
    Extensions::EXT::draw_buffers_indexed{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::geometry_shader{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::gpu_shader5{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::primitive_bounding_box{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::shader_io_blocks{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::tessellation_shader{},
    #endif
    Extensions::EXT::texture_border_clamp{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::texture_buffer{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::EXT::texture_cube_map_array{},
    #endif
    Extensions::KHR::blend_equation_advanced{},
    Extensions::KHR::debug{},
    Extensions::KHR::robust_buffer_access_behavior{},
    Extensions::KHR::robustness{},
    Extensions::KHR::texture_compression_astc_ldr{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::OES::sample_shading{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::OES::sample_variables{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::OES::shader_image_atomic{},
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::OES::shader_multisample_interpolation{},
    #endif
    Extensions::OES::texture_stencil8{},
    #ifndef MAGNUM_TARGET_GLES2
    Extensions::OES::texture_storage_multisample_2d_array{}
    #endif
    };
#endif

#ifdef CORRADE_MSVC2015_COMPATIBILITY
/* MSVC 2015 ICEs in the loop below if this is constexpr. Don't, then. */
const
#else
constexpr
#endif
struct {
    Version version;
    Containers::ArrayView<const Extension> extensions;
} KnownExtensionsForVersion[]{
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

const Extension* findExtension(const Containers::StringView extension, const std::size_t since = 0) {
    for(std::size_t i = since; i != Containers::arraySize(KnownExtensionsForVersion); ++i) {
        const auto found = std::lower_bound(KnownExtensionsForVersion[i].extensions.begin(), KnownExtensionsForVersion[i].extensions.end(), extension, [](const Extension& a, const Containers::StringView& b) {
            return a.string() < b;
        });
        if(found != KnownExtensionsForVersion[i].extensions.end() && found->string() == extension)
            return found;
    }

    return {};
}

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
        .addOption("gpu-validation", "off").setHelp("gpu-validation", "GPU validation using KHR_debug (if present)", "off|on|no-error")
        .addOption("log", "default").setHelp("log", "console logging", "default|quiet|verbose")
        .setFromEnvironment("disable-workarounds")
        .setFromEnvironment("disable-extensions")
        .setFromEnvironment("gpu-validation")
        .setFromEnvironment("log")
        .parse(argc, argv);

    /* Decide how to display initialization log */
    if(args.value("log") == "verbose" || args.value("log") == "VERBOSE")
        _configurationFlags |= Configuration::Flag::VerboseLog;
    else if(args.value("log") == "quiet" || args.value("log") == "QUIET")
        _configurationFlags |= Configuration::Flag::QuietLog;

    /* Decide whether to enable GPU validation / no error context */
    if(args.value("gpu-validation") == "on" || args.value("gpu-validation") == "ON")
        _configurationFlags |= Configuration::Flag::GpuValidation;
    else if(args.value("gpu-validation") == "no-error")
        _configurationFlags |= Configuration::Flag::GpuValidationNoError;

    /* If there are any disabled workarounds, save them until tryCreate() uses
       them. The disableWorkaround() function saves the internal string view
       instead of the one passed from the command line so we don't need to
       bother with String allocations. */
    const Containers::StringView disabledWorkarounds = args.value<Containers::StringView>("disable-workarounds");
    if(!disabledWorkarounds.isEmpty()) {
        const Containers::Array<Containers::StringView> split = disabledWorkarounds.splitOnWhitespaceWithoutEmptyParts();
        arrayReserve(_driverWorkarounds, split.size());
        for(const Containers::StringView workaround: split)
            disableDriverWorkaround(workaround);
    }

    /* Disable extensions. Here we search for them among the known extensions
       and store the Extension objects instead, which avoids the string copying
       and another binary search in tryCreate(). */
    const Containers::StringView disabledExtensions = args.value<Containers::StringView>("disable-extensions");
    if(!disabledExtensions.isEmpty()) {
        const Containers::Array<Containers::StringView> split = disabledExtensions.splitOnWhitespaceWithoutEmptyParts();
        arrayReserve(_disabledExtensions, split.size());
        for(const Containers::StringView extension: split) {
            if(const Extension* found = findExtension(extension)) {
                arrayAppend(_disabledExtensions, *found);
            }
        }
    }
}

Context::Context(Context&& other) noexcept:
    _functionLoader{other._functionLoader},
    _version{other._version},
    #ifndef MAGNUM_TARGET_WEBGL
    _flags{other._flags},
    #endif
    _extensionStatus{other._extensionStatus},
    _extensionRequiredVersion{other._extensionRequiredVersion},
    #ifdef MAGNUM_BUILD_DEPRECATED
    _supportedExtensions{std::move(other._supportedExtensions)},
    #endif
    _state{other._state},
    _detectedDrivers{std::move(other._detectedDrivers)},
    _driverWorkarounds{std::move(other._driverWorkarounds)},
    _disabledExtensions{std::move(other._disabledExtensions)},
    _configurationFlags{other._configurationFlags}
{
    if(currentContext == &other) currentContext = this;
}

Context::~Context() {
    if(currentContext == this) currentContext = nullptr;
}

void Context::create(const Configuration& configuration) {
    /* Hard exit if the context cannot be created */
    if(!tryCreate(configuration)) std::exit(1);
}

bool Context::tryCreate(const Configuration& configuration) {
    CORRADE_ASSERT(_version == Version::None,
        "Platform::Context::tryCreate(): context already created", false);

    /* Merge the configuration with parameters passed on the command line /
       environment. For the log command-line gets a priority -- if it says
       quiet, it'll override the verbose setting from the configuration; if
       it says verbose, the quiet setting from the configuration will be
       ignored */
    if((configuration.flags() & Configuration::Flag::VerboseLog) && !(_configurationFlags & Configuration::Flag::QuietLog))
        _configurationFlags |= Configuration::Flag::VerboseLog;
    else if((configuration.flags() & Configuration::Flag::QuietLog) && !(_configurationFlags & Configuration::Flag::VerboseLog))
        _configurationFlags |= Configuration::Flag::QuietLog;

    /* GPU validation is enabled if either enables it */
    if(configuration.flags() & Configuration::Flag::GpuValidation)
        _configurationFlags |= Configuration::Flag::GpuValidation;
    if(configuration.flags() & Configuration::Flag::GpuValidationNoError)
        _configurationFlags |= Configuration::Flag::GpuValidationNoError;

    /* Same for windowless */
    if(configuration.flags() & Configuration::Flag::Windowless)
        _configurationFlags |= Configuration::Flag::Windowless;

    /* Driver workarounds get merged. Not using disableDriverWorkaround() here
       since the Configuration already contains the internal string views. */
    for(const Containers::StringView workaround: configuration.disabledWorkarounds())
        arrayAppend(_driverWorkarounds, InPlaceInit, workaround, true);

    /* Extensions get merged also. Here we had the chance to force users to
       give us the predefined extension types so no need to search for their
       IDs */
    for(const Extension& extension: configuration.disabledExtensions())
        arrayAppend(_disabledExtensions, extension);

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
    const Containers::StringView version = versionString();
    if(!version.contains("WebGL 2"_s)) {
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
        const Containers::StringView version = versionString();
        #ifndef MAGNUM_TARGET_GLES
        if(version.hasPrefix("2.1"_s))
        #elif defined(MAGNUM_TARGET_WEBGL)
        /* Internet Explorer currently has 0.94 */
        if(version.contains("WebGL 1"_s) ||
           version.contains("WebGL 0"_s))
        #else
        if(version.contains("OpenGL ES 2.0"_s) ||
           /* It is possible to use Magnum compiled for ES2 on ES3 contexts */
           version.contains("OpenGL ES 3."_s))
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

    /* Get first future (not supported) version */
    std::size_t future = 0;
    while(KnownExtensionsForVersion[future].version != Version::None && isVersionSupported(KnownExtensionsForVersion[future].version))
        ++future;

    /* Mark all extensions from past versions as supported */
    for(std::size_t i = 0; i != future; ++i)
        for(const Extension& extension: KnownExtensionsForVersion[i].extensions)
            _extensionStatus.set(extension.index(), true);

    /* Check for presence of future and vendor extensions */
    const Containers::Array<Containers::StringView> extensions = extensionStrings();
    for(const Containers::StringView extension: extensions) {
        if(const Extension* found = findExtension(extension, future)) {
            #ifdef MAGNUM_BUILD_DEPRECATED
            arrayAppend(_supportedExtensions, *found);
            #endif
            _extensionStatus.set(found->index(), true);
        }
    }

    /* Reset minimal required version to Version::None for whole array */
    for(auto& i: _extensionRequiredVersion) i = Version::None;

    /* Initialize required versions from extension info */
    for(const auto& version: KnownExtensionsForVersion)
        for(const Extension& extension: version.extensions)
            _extensionRequiredVersion[extension.index()] = extension.requiredVersion();

    /* Setup driver workarounds (increase required version for particular
       extensions), see Implementation/driverWorkarounds.cpp */
    setupDriverWorkarounds();

    /* Set this context as current */
    CORRADE_ASSERT(!currentContext, "GL::Context: Another context currently active", false);
    currentContext = this;

    /* Decide whether to print the initialization output or not */
    std::ostream* output = _configurationFlags & Configuration::Flag::QuietLog ? nullptr : Debug::output();

    /* Print some info and initialize state tracker (which also prints some
       more info). Mesa's renderer string has a space at the end, trim that. */
    Debug{output} << "Renderer:" << rendererString().trimmed() << "by" << vendorString();
    Debug{output} << "OpenGL version:" << versionString();

    /* Disable extensions as requested by the user */
    if(!_disabledExtensions.empty()) {
        Debug{output} << "Disabling extensions:";

        for(const Extension& extension: _disabledExtensions) {
            _extensionRequiredVersion[extension.index()] = Version::None;
            Debug{output} << "   " << extension.string();
        }
    }

    std::pair<Containers::ArrayTuple, Implementation::State&> state = Implementation::State::allocate(*this, output);
    _stateData = std::move(state.first);
    _state = &state.second;

    /* Print a list of used workarounds */
    if(!_driverWorkarounds.empty()) {
        Debug{output} << "Using driver workarounds:";
        for(const auto& workaround: _driverWorkarounds)
            if(!workaround.second) Debug(output) << "   " << workaround.first;
    }

    /** @todo Get rid of these */
    /* Initialize functionality based on current OpenGL version and extensions.
       If we are on a windowless context don't touch the default framebuffer
       to avoid potential race conditions with default framebuffer on another
       thread. */
    if(!(_configurationFlags & Configuration::Flag::Windowless))
        DefaultFramebuffer::initializeContextBasedFunctionality(*this);
    Renderer::initializeContextBasedFunctionality();

    /* Enable GPU validation, if requested */
    if(_configurationFlags & Configuration::Flag::GpuValidation) {
        #ifndef MAGNUM_TARGET_WEBGL
        if(isExtensionSupported<Extensions::KHR::debug>()) {
            Renderer::enable(Renderer::Feature::DebugOutput);
            Renderer::enable(Renderer::Feature::DebugOutputSynchronous);
            DebugOutput::setDefaultCallback();

            if((detectedDriver() & DetectedDriver::Amd) && !(flags() & Flag::Debug)) {
                Warning{} << "GL::Context: GPU validation on AMD drivers requires debug context to work properly";
            } else if(_configurationFlags & Configuration::Flag::VerboseLog) {
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

Containers::StringView Context::vendorString() const {
    return {reinterpret_cast<const char*>(glGetString(GL_VENDOR)), Containers::StringViewFlag::Global};
}

Containers::StringView Context::rendererString() const {
    return {reinterpret_cast<const char*>(glGetString(GL_RENDERER)), Containers::StringViewFlag::Global};
}

Containers::StringView Context::versionString() const {
    return {reinterpret_cast<const char*>(glGetString(GL_VERSION)), Containers::StringViewFlag::Global};
}

Containers::StringView Context::shadingLanguageVersionString() const {
    return {reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)), Containers::StringViewFlag::Global};
}

Containers::Array<Containers::StringView> Context::shadingLanguageVersionStrings() const {
    #ifndef MAGNUM_TARGET_GLES
    GLint versionCount = 0;
    glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &versionCount);

    /* If zero, the implementation doesn't yet support this query (< GL4.3) */
    /** @todo doesn't this throw a GL error? some better handling? */
    if(versionCount) {
        /* Get all of them */
        Containers::Array<Containers::StringView> versions{std::size_t(versionCount)};
        for(GLint i = 0; i != versionCount; ++i)
            versions[i] = {reinterpret_cast<const char*>(glGetStringi(GL_SHADING_LANGUAGE_VERSION, i)), Containers::StringViewFlag::Global};
        return versions;
    }
    #endif

    return Containers::array({shadingLanguageVersionString()});
}

Containers::Array<Containers::StringView> Context::extensionStrings() const {
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
        Containers::Array<Containers::StringView> extensions{std::size_t(extensionCount)};
        for(GLint i = 0; i != extensionCount; ++i)
            extensions[i] = {reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)), Containers::StringViewFlag::Global};
        return extensions;
    }
    #ifndef MAGNUM_TARGET_GLES3
    else
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES3
    /* OpenGL 2.1 / OpenGL ES 2.0 doesn't have glGetStringi() */
    return Containers::StringView{reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)), Containers::StringViewFlag::Global}.splitOnWhitespaceWithoutEmptyParts();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
bool Context::isCoreProfile() {
    return isCoreProfileInternal(_state->context);
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
        _state->buffer.reset();
    if(states & State::Framebuffers)
        _state->framebuffer.reset();
    if(states & State::Meshes)
        _state->mesh.reset();

    #ifndef MAGNUM_TARGET_GLES
    /* Bind a scratch VAO for external GL code that is not VAO-aware and just
       enables vertex attributes on the default VAO. Generate it on-demand as
       we don't expect this case to be used very often. */
    if(states & State::BindScratchVao) {
        if(!_state->mesh.scratchVAO)
            glGenVertexArrays(1, &_state->mesh.scratchVAO);

        _state->mesh.bindVAOImplementation(_state->mesh.scratchVAO);

    /* Otherwise just unbind the current VAO and leave the the default */
    } else
    #endif
    if(states & State::MeshVao)
        _state->mesh.bindVAOImplementation(0);

    if(states & State::PixelStorage) {
        _state->renderer.unpackPixelStorage.reset();
        _state->renderer.packPixelStorage.reset();
    }

    /* Nothing to reset for renderer yet */

    if(states & State::Shaders) {
        /* Nothing to reset for shaders */
        _state->shaderProgram.reset();
    }

    if(states & State::Textures)
        _state->texture.reset();
    #ifndef MAGNUM_TARGET_GLES2
    if(states & State::TransformFeedback)
        _state->transformFeedback.reset();
    #endif
}

Context::Configuration::Configuration() = default;

Context::Configuration::Configuration(const Configuration& other): _flags{other._flags} {
    addDisabledWorkarounds(other._disabledWorkarounds);
    addDisabledExtensions(other._disabledExtensions);
}

Context::Configuration::Configuration(Configuration&&) noexcept = default;

Context::Configuration::~Configuration() = default;

Context::Configuration& Context::Configuration::operator=(const Configuration& other) {
    _flags = other._flags;
    arrayResize(_disabledWorkarounds, 0);
    /** @todo arrayClear(), ffs */
    arrayResize(_disabledExtensions, NoInit, 0);
    addDisabledWorkarounds(other._disabledWorkarounds);
    addDisabledExtensions(other._disabledExtensions);
    return *this;
}

Context::Configuration& Context::Configuration::operator=(Configuration&&) noexcept = default;

Containers::ArrayView<const Containers::StringView> Context::Configuration::disabledWorkarounds() const {
    return _disabledWorkarounds;
}

Containers::ArrayView<const Extension> Context::Configuration::disabledExtensions() const {
    return _disabledExtensions;
}

Context::Configuration& Context::Configuration::addDisabledWorkarounds(std::initializer_list<Containers::StringView> workarounds) {
    return addDisabledWorkarounds(Containers::arrayView(workarounds));
}

Context::Configuration& Context::Configuration::addDisabledExtensions(Containers::ArrayView<const Extension> extensions) {
    arrayAppend(_disabledExtensions, extensions);
    return *this;
}

Context::Configuration& Context::Configuration::addDisabledExtensions(std::initializer_list<Extension> extensions) {
    return addDisabledExtensions(Containers::arrayView(extensions));
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
