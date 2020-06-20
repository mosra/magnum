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

#include "Extensions.h"

#include <Corrade/Containers/ArrayView.h>

namespace Magnum { namespace Vk {

namespace {

/* When adding a new list, InstanceExtension::extensions() and
   Instance::initialize() needs to be adapted. Binary search is performed on
   the extensions, thus they have to be sorted alphabetically. */
constexpr InstanceExtension InstanceExtensions[] {
    Extensions::EXT::debug_report{},
    Extensions::EXT::debug_utils{},
    Extensions::EXT::validation_features{},
};
constexpr InstanceExtension InstanceExtensions11[] {
    Extensions::KHR::device_group_creation{},
    Extensions::KHR::external_fence_capabilities{},
    Extensions::KHR::external_memory_capabilities{},
    Extensions::KHR::external_semaphore_capabilities{},
    Extensions::KHR::get_physical_device_properties2{},
};
/* No Vulkan 1.2 instance extensions */

}

Containers::ArrayView<const InstanceExtension> InstanceExtension::extensions(const Version version) {
    switch(version) {
        case Version::None: return Containers::arrayView(InstanceExtensions);
        case Version::Vk10: return nullptr;
        case Version::Vk11: return Containers::arrayView(InstanceExtensions11);
        case Version::Vk12: return nullptr;
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

namespace {

/* When adding a new list, Extension::extensions() needs to be adapted. Binary
   search is performed on the extensions, thus they have to be sorted
   alphabetically. */
constexpr Extension DeviceExtensions[] {
    Extensions::EXT::debug_marker{},
    Extensions::EXT::index_type_uint8{},
    Extensions::EXT::texture_compression_astc_hdr{},
    Extensions::IMG::format_pvrtc{},
};
constexpr Extension DeviceExtensions11[] {
  //Extensions::KHR::16bit_storage{},
    Extensions::KHR::bind_memory2{},
    Extensions::KHR::dedicated_allocation{},
    Extensions::KHR::descriptor_update_template{},
    Extensions::KHR::device_group{},
    Extensions::KHR::external_fence{},
    Extensions::KHR::external_memory{},
    Extensions::KHR::external_semaphore{},
    Extensions::KHR::get_memory_requirements2{},
    Extensions::KHR::maintenance1{},
    Extensions::KHR::maintenance2{},
    Extensions::KHR::maintenance3{},
    Extensions::KHR::multiview{},
    Extensions::KHR::relaxed_block_layout{},
    Extensions::KHR::sampler_ycbcr_conversion{},
    Extensions::KHR::shader_draw_parameters{},
    Extensions::KHR::storage_buffer_storage_class{},
    Extensions::KHR::variable_pointers{},
};
constexpr Extension DeviceExtensions12[] {
    Extensions::EXT::descriptor_indexing{},
    Extensions::EXT::host_query_reset{},
    Extensions::EXT::sampler_filter_minmax{},
    Extensions::EXT::scalar_block_layout{},
    Extensions::EXT::separate_stencil_usage{},
    Extensions::EXT::shader_viewport_index_layer{},
  //Extensions::KHR::8bit_storage{},
    Extensions::KHR::buffer_device_address{},
    Extensions::KHR::create_renderpass2{},
    Extensions::KHR::depth_stencil_resolve{},
    Extensions::KHR::draw_indirect_count{},
    Extensions::KHR::driver_properties{},
    Extensions::KHR::image_format_list{},
    Extensions::KHR::imageless_framebuffer{},
    Extensions::KHR::sampler_mirror_clamp_to_edge{},
    Extensions::KHR::separate_depth_stencil_layouts{},
    Extensions::KHR::shader_atomic_int64{},
    Extensions::KHR::shader_float16_int8{},
    Extensions::KHR::shader_float_controls{},
    Extensions::KHR::shader_subgroup_extended_types{},
    Extensions::KHR::spirv_1_4{},
    Extensions::KHR::timeline_semaphore{},
    Extensions::KHR::uniform_buffer_standard_layout{},
    Extensions::KHR::vulkan_memory_model{},
};

}

Containers::ArrayView<const Extension> Extension::extensions(const Version version) {
    switch(version) {
        case Version::None: return Containers::arrayView(DeviceExtensions);
        case Version::Vk10: return nullptr;
        case Version::Vk11: return Containers::arrayView(DeviceExtensions11);
        case Version::Vk12: return Containers::arrayView(DeviceExtensions12);
    }

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

}}
