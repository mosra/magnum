#ifndef Magnum_Vk_Extensions_h
#define Magnum_Vk_Extensions_h
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
 * @brief Namespace @ref Magnum::Vk::Extensions, class @ref Magnum::Vk::InstanceExtension, @ref Magnum::Vk::Extension
 * @m_since_latest
 */

#include <Corrade/Containers/StringView.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/TypeTraits.h"
#include "Magnum/Vk/Version.h"

namespace Magnum { namespace Vk {

/**
@brief Compile-time information about Vulkan instance and device extensions
@m_since_latest

Each extension is a @cpp struct @ce named hierarchically by prefix, vendor and
extension name taken from list at @ref vulkan-support, for example
@cpp Vk::Extensions::EXT::debug_report @ce.

Each struct has the same public methods as the @ref InstanceExtension /
@ref Extension class (@ref Extension::requiredVersion() "requiredVersion()",
@ref Extension::coreVersion() "coreVersion()" and @ref Extension::string() "string()"),
but these structs are better suited for compile-time decisions rather than
@ref Extension instances. See @ref Instance::isExtensionEnabled() for example
usage.

This library is built if `WITH_VK` is enabled when building Magnum. To use this
library with CMake, you need to request the `Vk` component of the `Magnum`
package and link to the `Magnum::Vk` target:

@code{.cmake}
find_package(Magnum REQUIRED Vk)

# ...
target_link_libraries(your-app Magnum::Vk)
@endcode

See @ref building, @ref cmake and @ref vulkan for more information.
*/
namespace Extensions {

#ifndef DOXYGEN_GENERATING_OUTPUT

#define _extension(index, vendor, extension, _requiredVersion, _coreVersion) \
    struct extension {                                                      \
        enum: std::size_t { InstanceIndex = index };                        \
        constexpr static Version requiredVersion() { return Version::_requiredVersion; } \
        constexpr static Version coreVersion() { return Version::_coreVersion; } \
        constexpr static Containers::StringView string() {                  \
            using namespace Containers::Literals;                           \
            return "VK_" #vendor "_" #extension ""_s;                 \
        }                                                                   \
    };
namespace EXT {
    /** @todo remove EXT_debug_report when all platforms have EXT_debug_utils
        (my Huawei P10 doesn't have it) */
    _extension(0,  EXT,debug_report,                        Vk10, None) // #12
    _extension(1,  EXT,debug_utils,                         Vk10, None) // #129
    _extension(2,  EXT,validation_features,                 Vk10, None) // #248
} namespace KHR {
    _extension(10, KHR,get_physical_device_properties2,     Vk10, Vk11) // #60
    _extension(11, KHR,device_group_creation,               Vk10, Vk11) // #71
    _extension(12, KHR,external_memory_capabilities,        Vk10, Vk11) // #72
    _extension(13, KHR,external_semaphore_capabilities,     Vk10, Vk11) // #77
    _extension(14, KHR,external_fence_capabilities,         Vk10, Vk11) // #113
}
#undef _extension

#define _extension(index, vendor, extension, _requiredVersion, _coreVersion) \
    struct extension {                                                      \
        enum: std::size_t { Index = index };                                \
        constexpr static Version requiredVersion() { return Version::_requiredVersion; } \
        constexpr static Version coreVersion() { return Version::_coreVersion; } \
        constexpr static Containers::StringView string() {                  \
            using namespace Containers::Literals;                           \
            return "VK_" #vendor "_" #extension ""_s;                       \
        }                                                                   \
    };
namespace EXT {
    /** @todo remove EXT_debug_marker when all platforms have EXT_debug_utils
        (my Huawei P10 doesn't have it) */
    _extension(0,  EXT,debug_marker,                        Vk10, None) // #23
    _extension(1,  EXT,texture_compression_astc_hdr,        Vk10, None) // #67
    _extension(2,  EXT,sampler_filter_minmax,               Vk10, Vk12) // #131
    _extension(3,  EXT,descriptor_indexing,                 Vk10, Vk12) // #162
    _extension(4,  EXT,shader_viewport_index_layer,         Vk10, Vk12) // #163
    _extension(5,  EXT,scalar_block_layout,                 Vk10, Vk12) // #222
    _extension(6,  EXT,separate_stencil_usage,              Vk10, Vk12) // #247
    _extension(7,  EXT,host_query_reset,                    Vk10, Vk12) // #262
    _extension(8,  EXT,index_type_uint8,                    Vk10, None) // #266
} namespace IMG {
    _extension(20, IMG,format_pvrtc,                        Vk10, None) // #55
} namespace KHR {
    _extension(30, KHR,sampler_mirror_clamp_to_edge,        Vk10, Vk12) // #15
    _extension(31, KHR,multiview,                           Vk10, Vk11) // #54
    _extension(32, KHR,device_group,                        Vk10, Vk11) // #61
    _extension(33, KHR,shader_draw_parameters,              Vk10, Vk11) // #64
    _extension(34, KHR,maintenance1,                        Vk10, Vk11) // #70
    _extension(35, KHR,external_semaphore,                  Vk10, Vk11) // #78
    _extension(36, KHR,shader_float16_int8,                 Vk10, Vk12) // #83
  //_extension(37, KHR,16bit_storage,                       VK10, Vk11) // #84
    _extension(38, KHR,descriptor_update_template,          Vk10, Vk11) // #86
    _extension(39, KHR,external_memory,                     Vk10, Vk11) // #73
    _extension(40, KHR,imageless_framebuffer,               Vk10, Vk12) // #109
    _extension(41, KHR,create_renderpass2,                  Vk10, Vk12) // #110
    _extension(42, KHR,external_fence,                      Vk10, Vk11) // #114
    _extension(43, KHR,maintenance2,                        Vk10, Vk11) // #118
    _extension(44, KHR,variable_pointers,                   Vk10, Vk11) // #121
    _extension(45, KHR,dedicated_allocation,                Vk10, Vk11) // #128
    _extension(46, KHR,storage_buffer_storage_class,        Vk10, Vk11) // #142
    _extension(47, KHR,relaxed_block_layout,                Vk10, Vk11) // #145
    _extension(48, KHR,get_memory_requirements2,            Vk10, Vk11) // #147
    _extension(49, KHR,image_format_list,                   Vk10, Vk12) // #148
    _extension(50, KHR,sampler_ycbcr_conversion,            Vk10, Vk11) // #157
    _extension(51, KHR,bind_memory2,                        Vk10, Vk11) // #158
    _extension(52, KHR,maintenance3,                        Vk10, Vk11) // #169
    _extension(53, KHR,draw_indirect_count,                 Vk10, Vk12) // #170
    _extension(54, KHR,shader_subgroup_extended_types,      Vk11, Vk12) // #176
  //_extension(55, KHR,8bit_storage,                        Vk10, Vk12) // #178
    _extension(56, KHR,shader_atomic_int64,                 Vk10, Vk12) // #181
    _extension(57, KHR,driver_properties,                   Vk10, Vk12) // #197
    _extension(58, KHR,shader_float_controls,               Vk10, Vk12) // #198
    _extension(59, KHR,depth_stencil_resolve,               Vk10, Vk12) // #200
    _extension(60, KHR,timeline_semaphore,                  Vk10, Vk12) // #208
    _extension(61, KHR,vulkan_memory_model,                 Vk10, Vk12) // #212
    _extension(62, KHR,spirv_1_4,                           Vk11, Vk12) // #237
    _extension(63, KHR,separate_depth_stencil_layouts,      Vk10, Vk12) // #242
    _extension(64, KHR,uniform_buffer_standard_layout,      Vk10, Vk12) // #254
    _extension(65, KHR,buffer_device_address,               Vk10, Vk12) // #258
}
#undef _extension

#endif

}

/**
@brief Run-time information about a Vulkan instance extension
@m_since_latest

Encapsulates runtime information about a Vulkan extension, such as name string,
minimal required Vulkan version and version in which the extension was adopted
to core.

See also the @ref Extensions namespace, which contain compile-time information
about Vulkan extensions.
*/
class MAGNUM_VK_EXPORT InstanceExtension {
    public:
        /** @brief All known instance extensions for given Vulkan version */
        static Containers::ArrayView<const InstanceExtension> extensions(Version version);

        /** @brief Internal unique extension index */
        constexpr std::size_t index() const { return _index; }

        /** @brief Minimal version required by this extension */
        constexpr Version requiredVersion() const { return _requiredVersion; }

        /** @brief Version in which this extension was adopted to core */
        constexpr Version coreVersion() const { return _coreVersion; }

        /**
         * @brief Extension string
         *
         * The returned view is a global memory.
         */
        constexpr Containers::StringView string() const { return _string; }

        /** @brief Construct from a compile-time instance extension */
        template<class E, class = typename std::enable_if<Implementation::IsInstanceExtension<E>::value>::type> constexpr /*implicit*/ InstanceExtension(const E&): _index{E::InstanceIndex}, _requiredVersion{E::requiredVersion()}, _coreVersion{E::coreVersion()}, _string{E::string()} {}

    private:
        std::size_t _index;
        Version _requiredVersion;
        Version _coreVersion;
        Containers::StringView _string;
};

/**
@brief Run-time information about a Vulkan device extension
@m_since_latest

Encapsulates runtime information about a Vulkan extension, such as name string,
minimal required Vulkan version and version in which the extension was adopted
to core.

See also the @ref Extensions namespace, which contain compile-time information
about Vulkan extensions.
*/
class MAGNUM_VK_EXPORT Extension {
    public:
        /** @brief All known device extensions for given Vulkan version */
        static Containers::ArrayView<const Extension> extensions(Version version);

        /** @brief Internal unique extension index */
        constexpr std::size_t index() const { return _index; }

        /** @brief Minimal version required by this extension */
        constexpr Version requiredVersion() const { return _requiredVersion; }

        /** @brief Version in which this extension was adopted to core */
        constexpr Version coreVersion() const { return _coreVersion; }

        /** @brief Extension string */
        constexpr Containers::StringView string() const { return _string; }

        /** @brief Construct from a compile-time device extension */
        /** @todo prohibit conversion from GL/AL extensions also? */
        template<class E, class = typename std::enable_if<Implementation::IsExtension<E>::value>::type> constexpr /*implicit*/ Extension(const E&): _index{E::Index}, _requiredVersion{E::requiredVersion()}, _coreVersion{E::coreVersion()}, _string{E::string()} {}

    private:
        std::size_t _index;
        Version _requiredVersion;
        Version _coreVersion;
        Containers::StringView _string;
};

}}

#endif
