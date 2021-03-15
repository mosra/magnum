#ifndef Magnum_Vk_DescriptorPoolCreateInfo_h
#define Magnum_Vk_DescriptorPoolCreateInfo_h
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

/** @file
 * @brief Class @ref Magnum::Vk::DescriptorPoolCreateInfo
 * @m_since_latest
 */

#include <utility>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Descriptor pool creation info
@m_since_latest

Wraps a @type_vk_keyword{DescriptorPoolCreateInfo} and the nested
@type_vk_keyword{DescriptorPoolSize} structures. See
@ref Vk-DescriptorPool-creation "Descriptor pool creation" for usage
information.
*/
class MAGNUM_VK_EXPORT DescriptorPoolCreateInfo {
    public:
        /**
         * @brief Descriptor pool creation flag
         *
         * Wraps @type_vk_keyword{DescriptorPoolCreateFlagBits}.
         * @see @ref Flags, @ref DescriptorPoolCreateInfo()
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /**
             * Allow individual descriptor sets to be freed instead of just the
             * whole pool.
             * @todoc reference relevant functions once they exist
             *
             * @m_class{m-note m-success}
             *
             * @par
             *      Not using this flag may help the driver to use simpler
             *      per-pool allocators instead of per-set. With this flag set,
             *      descriptor pool fragmentation may occur, which can result
             *      in @ref DescriptorPool::allocate() failures even if the
             *      number of individual free descriptors in the pool is large
             *      enough.
             *
             * @see @ref Result::ErrorFragmentedPool
             */
            FreeDescriptorSet = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,

            /**
             * Descriptor sets allocated from this pool can use bindings with
             * the @ref DescriptorSetLayoutBinding::Flag::UpdateAfterBind set.
             * @requires_vk_feature @ref DeviceFeature::DescriptorBindingSampledImageUpdateAfterBind
             *      if used on a @ref DescriptorType::CombinedImageSampler /
             *      @relativeref{DescriptorType,SampledImage}
             * @requires_vk_feature @ref DeviceFeature::DescriptorBindingStorageImageUpdateAfterBind
             *      if used on a @ref DescriptorType::StorageImage
             * @requires_vk_feature @ref DeviceFeature::DescriptorBindingUniformTexelBufferUpdateAfterBind
             *      if used on a @ref DescriptorType::UniformTexelBuffer
             * @requires_vk_feature @ref DeviceFeature::DescriptorBindingStorageTexelBufferUpdateAfterBind
             *      if used on a @ref DescriptorType::StorageTexelBuffer
             * @requires_vk_feature @ref DeviceFeature::DescriptorBindingUniformBufferUpdateAfterBind
             *      if used on a @ref DescriptorType::UniformBuffer /
             *      @relativeref{DescriptorType,UniformBufferDynamic}
             * @requires_vk_feature @ref DeviceFeature::DescriptorBindingStorageBufferUpdateAfterBind
             *      if used on a @ref DescriptorType::StorageBuffer /
             *      @relativeref{DescriptorType,StorageBufferDynamic}
             */
            UpdateAfterBind = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT
        };

        /**
         * @brief DescriptorPool creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{DescriptorPoolCreateFlags}.
         * @see @ref DescriptorPoolCreateInfo()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param maxSets       Maximum count of descriptor sets that can be
         *      allocated from this pool. Has to be at least one.
         * @param poolSizes     Pool sizes for each descriptor type. There has
         *      to be at least one, and pool sizes can't be zero.
         * @param flags         DescriptorPool creation flags
         *
         * The following @type_vk{DescriptorPoolCreateInfo} fields are
         * pre-filled in addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `maxSets`
         * -    `poolSizeCount` and `pPoolSizes` to @p poolSizes converted to
         *      a list of @type_vk{DescriptorPoolSize} structures
         */
        explicit DescriptorPoolCreateInfo(UnsignedInt maxSets, Containers::ArrayView<const std::pair<DescriptorType, UnsignedInt>> poolSizes, Flags flags = {});
        /** @overload */
        explicit DescriptorPoolCreateInfo(UnsignedInt maxSets, std::initializer_list<std::pair<DescriptorType, UnsignedInt>> poolSizes, Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit DescriptorPoolCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit DescriptorPoolCreateInfo(const VkDescriptorPoolCreateInfo& info);

        /** @brief Copying is not allowed */
        DescriptorPoolCreateInfo(const DescriptorPoolCreateInfo&) = delete;

        /** @brief Move constructor */
        DescriptorPoolCreateInfo(DescriptorPoolCreateInfo&& other) noexcept;

        ~DescriptorPoolCreateInfo();

        /** @brief Copying is not allowed */
        DescriptorPoolCreateInfo& operator=(const DescriptorPoolCreateInfo&) = delete;

        /** @brief Move assignment */
        DescriptorPoolCreateInfo& operator=(DescriptorPoolCreateInfo&& other) noexcept;

        /** @brief Underlying @type_vk{DescriptorPoolCreateInfo} structure */
        VkDescriptorPoolCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkDescriptorPoolCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkDescriptorPoolCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkDescriptorPoolCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkDescriptorPoolCreateInfo*() const { return &_info; }

    private:
        VkDescriptorPoolCreateInfo _info;
        Containers::ArrayTuple _data;
};

CORRADE_ENUMSET_OPERATORS(DescriptorPoolCreateInfo::Flags)

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/DescriptorPool.h"

#endif
