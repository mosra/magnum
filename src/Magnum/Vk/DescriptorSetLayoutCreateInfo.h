#ifndef Magnum_Vk_DescriptorSetLayoutCreateInfo_h
#define Magnum_Vk_DescriptorSetLayoutCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::DescriptorSetLayoutCreateInfo, @ref Magnum::Vk::DescriptorSetLayoutBinding
 * @m_since_latest
 */

/* While I'm not commonly including the Reference headers, the constructor of
   DescriptorSetLayoutCreateInfo needs this type and so users would be forced
   to include it every time they include this header. Or forget to do so and
   wonder why the code is not compiling, which especially when combined with
   the fact that one has to wrap the DescriptorSetLayoutBinding instances in
   double {{}}'s might be a bit too much to handle. */
#include <Corrade/Containers/AnyReference.h>

#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Tags.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Descriptor set layout binding
@m_since_latest

Wraps a @type_vk_keyword{DescriptorSetLayoutBinding} together with
associated @type_vk_keyword{DescriptorBindingFlags}. See
@ref Vk-DescriptorSetLayout-creation "Descriptor set layout creation"
for usage information.
*/
class MAGNUM_VK_EXPORT DescriptorSetLayoutBinding {
    public:
        /**
         * @brief Descriptor set layout binding flag
         *
         * Wraps @type_vk_keyword{DescriptorBindingFlagBits}.
         * @see @ref Flags, @ref DescriptorSetLayoutBinding()
         * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /**
             * If descriptors in this binding are updated between
             * binding them in a command buffer and a @ref Queue::submit(), the
             * submission will use the most recently set descriptors for
             * the binding and the updates do not invalidate the command
             * buffer.
             *
             * Descriptor set layouts using this flag can be only allocated
             * from a @ref DescriptorPool that has
             * @ref DescriptorPoolCreateInfo::Flag::UpdateAfterBind set as
             * well.
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
            UpdateAfterBind = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,

            /**
             * Descriptors not used by the command buffer can be updated
             * after binding them in a command buffer and while the command
             * buffer is pending execution. If set together with
             * @ref Flag::PartiallyBound, any descriptors that are not
             * dynamically used can be updated, if alone then only descriptors
             * statically not used can be updated.
             * @requires_vk_feature @ref DeviceFeature::DescriptorBindingUpdateUnusedWhilePending
             */
            UpdateUnusedWhilePending = VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT,

            /**
             * Descriptors in the binding that are not dynamically used don't
             * need to contain valid descriptors when consumed.
             * @requires_vk_feature @ref DeviceFeature::DescriptorBindingPartiallyBound
             */
            PartiallyBound = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT,

            /**
             * This descriptor binding has a variable size that will be
             * specified in @ref DescriptorPool::allocate(VkDescriptorSetLayout, UnsignedInt),
             * and the @p descriptorCount value specified in the constructor is
             * treated as an upper bound.
             *
             * Allowed only on the last binding number in the layout, not
             * allowed on a @ref DescriptorType::UniformBufferDynamic or
             * @ref DescriptorType::StorageBufferDynamic.
             * @requires_vk_feature @ref DeviceFeature::DescriptorBindingVariableDescriptorCount
             */
            VariableDescriptorCount = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT
        };

        /**
         * @brief Descriptor set layout creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{DescriptorBindingFlags}.
         * @see @ref DescriptorSetLayoutBinding()
         * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing}
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param binding           Binding corresponding to a particular
         *      binding in a shader. Has to be unique in the layout.
         * @param descriptorType    Descriptor type
         * @param descriptorCount   Number of descriptors contained in the
         *      binding. If the shader binding is not an array, use @cpp 1 @ce,
         *      zero is allowed as well. For a @ref Flag::VariableDescriptorCount
         *      this is used as an upper bound and a concrete size is specified
         *      in @ref DescriptorPool::allocate(VkDescriptorSetLayout, UnsignedInt).
         * @param stages            Shader stages that access the binding.
         *      Use @cpp ~Vk::ShaderStages{} @ce to specify that all stages
         *      may access the binding.
         * @param flags             Flags
         *
         * The following @type_vk{DescriptorSetLayoutBinding} fields are
         * pre-filled, everything else is zero-filled:
         *
         * -    `binding`
         * -    `descriptorType`
         * -    `descriptorCount`
         * -    `stageFlags` to @p stages
         *
         * If @p flags are non-empty, a @type_vk{DescriptorBindingFlags} field
         * is saved and then subsequently available through @ref flags().
         * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing} if
         *      @p flags are non-empty
         */
        /*implicit*/ DescriptorSetLayoutBinding(UnsignedInt binding, DescriptorType descriptorType, UnsignedInt descriptorCount = 1, ShaderStages stages = ~ShaderStages{}, Flags flags = {});

        /**
         * @brief Construct with immutable samplers
         * @param binding           Binding corresponding to a particular
         *      binding in a shader
         * @param descriptorType    Descriptor type. Should be either
         *      @ref DescriptorType::Sampler or
         *      @ref DescriptorType::CombinedImageSampler.
         * @param immutableSamplers Immutable samplers. Allowed to be empty.
         * @param stages            Shader stages that access the binding.
         *      Use @cpp ~Vk::ShaderStages{} @ce to specify that all stages
         *      may access the binding.
         * @param flags             Flags
         *
         * The following @type_vk{DescriptorSetLayoutBinding} fields are
         * pre-filled, everything else is zero-filled:
         *
         * -    `binding`
         * -    `descriptorType`
         * -    `descriptorCount` to @cpp immutableSamplers.size() @ce
         * -    `pImmutableSamplers` to a copy of @p immutableSamplers
         * -    `stageFlags` to @p stages
         *
         * If @p flags are non-empty, a @type_vk{DescriptorBindingFlags} field
         * is saved and then subsequently available through @ref flags().
         * @requires_vk12 Extension @vk_extension{EXT,descriptor_indexing} if
         *      @p flags are non-empty
         */
        /*implicit*/ DescriptorSetLayoutBinding(UnsignedInt binding, DescriptorType descriptorType, Containers::ArrayView<const VkSampler> immutableSamplers, ShaderStages stages = ~ShaderStages{}, Flags flags = {});
        /** @overload */
        /*implicit*/ DescriptorSetLayoutBinding(UnsignedInt binding, DescriptorType descriptorType, std::initializer_list<VkSampler> immutableSamplers, ShaderStages stages = ~ShaderStages{}, Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that the structure has to be fully initialized afterwards in
         * order to be usable.
         */
        explicit DescriptorSetLayoutBinding(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit DescriptorSetLayoutBinding(const VkDescriptorSetLayoutBinding& info, VkDescriptorBindingFlags flags = {});

        /** @brief Copying is not allowed */
        DescriptorSetLayoutBinding(const DescriptorSetLayoutBinding&) = delete;

        /** @brief Move constructor */
        DescriptorSetLayoutBinding(DescriptorSetLayoutBinding&& other) noexcept;

        ~DescriptorSetLayoutBinding();

        /** @brief Copying is not allowed */
        DescriptorSetLayoutBinding& operator=(const DescriptorSetLayoutBinding&) = delete;

        /** @brief Move assignment */
        DescriptorSetLayoutBinding& operator=(DescriptorSetLayoutBinding&& other) noexcept;

        /** @brief Underlying @type_vk{DescriptorSetLayoutBinding} structure */
        VkDescriptorSetLayoutBinding& operator*() { return _binding; }
        /** @overload */
        const VkDescriptorSetLayoutBinding& operator*() const { return _binding; }
        /** @overload */
        VkDescriptorSetLayoutBinding* operator->() { return &_binding; }
        /** @overload */
        const VkDescriptorSetLayoutBinding* operator->() const { return &_binding; }
        /** @overload */
        operator const VkDescriptorSetLayoutBinding*() const { return &_binding; }

        /**
         * @overload
         *
         * The class is implicitly convertible to a reference in addition to
         * a pointer because the type is commonly used in arrays as well, which
         * would be annoying to do with a pointer conversion.
         */
        operator const VkDescriptorSetLayoutBinding&() const { return _binding; }

        /** @brief Underlying @type_vk{DescriptorBindingFlags} enum set */
        VkDescriptorBindingFlags& flags() { return _flags; }
        /** @overload */
        VkDescriptorBindingFlags flags() const { return _flags; }

    private:
        VkDescriptorSetLayoutBinding _binding;
        Containers::ArrayTuple _data;
        VkDescriptorBindingFlags _flags;
};

CORRADE_ENUMSET_OPERATORS(DescriptorSetLayoutBinding::Flags)

/**
@brief Descriptor set layout creation info
@m_since_latest

Wraps a @type_vk_keyword{DescriptorSetLayoutCreateInfo} together with
@type_vk_keyword{DescriptorSetLayoutBindingFlagsCreateInfo}. See
@ref Vk-DescriptorSetLayout-creation "Descriptor set layout creation"
for usage information.
*/
class MAGNUM_VK_EXPORT DescriptorSetLayoutCreateInfo {
    public:
        /**
         * @brief Descriptor set layout creation flag
         *
         * Wraps @type_vk_keyword{DescriptorSetLayoutCreateFlagBits}.
         * @see @ref Flags, @ref DescriptorSetLayoutCreateInfo()
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /** @todo all the flags from extensions and 1.2 */
        };

        /**
         * @brief Descriptor set layout creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{DescriptorSetLayoutCreateFlags}.
         * @see @ref DescriptorSetLayoutCreateInfo()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param bindings      Descriptor set layout bindings. Allowed to be
         *      empty.
         * @param flags         Descriptor set layout creation flags
         *
         * The following @type_vk{DescriptorSetLayoutCreateInfo} fields are
         * pre-filled in addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `bindingCount` and `pBindings` to a copy of @p bindings
         *
         * If any of the @p bindings has @ref DescriptorSetLayoutBinding::flags()
         * non-empty, a @type_vk{DescriptorSetLayoutBindingFlagsCreateInfo}
         * structure is referenced from the `pNext` chain of
         * @type_vk{DescriptorSetLayoutCreateInfo}, with the following fields
         * set in addition to `sType`, everything else zero-filled:
         *
         * -    `bindingCount` to @cpp binding.size() @ce
         * -    `pBindingFlags` to a list of all
         *      @ref DescriptorSetLayoutBinding::flags() from @p bindings
         */
        explicit DescriptorSetLayoutCreateInfo(Containers::ArrayView<const Containers::AnyReference<const DescriptorSetLayoutBinding>> bindings, Flags flags = {});
        /** @overload */
        explicit DescriptorSetLayoutCreateInfo(std::initializer_list<Containers::AnyReference<const DescriptorSetLayoutBinding>> bindings, Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit DescriptorSetLayoutCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit DescriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutCreateInfo& info);

        /** @brief Copying is not allowed */
        DescriptorSetLayoutCreateInfo(const DescriptorSetLayoutCreateInfo&) = delete;

        /** @brief Move constructor */
        DescriptorSetLayoutCreateInfo(DescriptorSetLayoutCreateInfo&& other) noexcept;

        ~DescriptorSetLayoutCreateInfo();

        /** @brief Copying is not allowed */
        DescriptorSetLayoutCreateInfo& operator=(const DescriptorSetLayoutCreateInfo&) = delete;

        /** @brief Move assignment */
        DescriptorSetLayoutCreateInfo& operator=(DescriptorSetLayoutCreateInfo&& other) noexcept;

        /** @brief Underlying @type_vk{DescriptorSetLayoutCreateInfo} structure */
        VkDescriptorSetLayoutCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkDescriptorSetLayoutCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkDescriptorSetLayoutCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkDescriptorSetLayoutCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkDescriptorSetLayoutCreateInfo*() const { return &_info; }

    private:
        VkDescriptorSetLayoutCreateInfo _info;
        Containers::ArrayTuple _data;
};

CORRADE_ENUMSET_OPERATORS(DescriptorSetLayoutCreateInfo::Flags)

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/DescriptorSetLayout.h"

#endif
