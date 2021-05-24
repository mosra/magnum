#ifndef Magnum_Vk_DescriptorPool_h
#define Magnum_Vk_DescriptorPool_h
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
 * @brief Class @ref Magnum::Vk::DescriptorPool
 * @m_since_latest
 */

#include <utility>

#include "Magnum/Tags.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Descriptor pool
@m_since_latest

Wraps @type_vk_keyword{DescriptorPool}, which is used for allocating descriptor
sets.

@section Vk-DescriptorPool-creation Descriptor pool creation

The @ref DescriptorPoolCreateInfo class takes a maximum number of descriptor
sets that can be allocated from a pool and then a list of total available
descriptor counts for desired @ref DescriptorType "DescriptorTypes". The
following snippet creates a pool allowing to allocate at most 8 descriptor sets
with 24 sampler bindings and 16 uniform bindings:

@snippet MagnumVk.cpp DescriptorPool-creation

With a descriptor pool created, you can allocate descriptor sets from it. See
the @ref DescriptorSet class for details.
*/
class MAGNUM_VK_EXPORT DescriptorPool {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device            Vulkan device the descriptor pool is
         *      created on
         * @param handle            The @type_vk{DescriptorPool} handle
         * @param flags             Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * a descriptor pool created using a constructor, the Vulkan descriptor
         * pool is by default not deleted on destruction, use @p flags for
         * different behavior.
         *
         * @m_class{m-note m-warning}
         *
         * @par
         *      Note that descriptor sets allocated using a pool wrapped by
         *      this function have no way to know if the pool was created with
         *      @ref DescriptorPoolCreateInfo::Flag::FreeDescriptorSet and thus
         *      won't be freeing themselves on destruction. If you need such
         *      behavior on these, re-wrap the allocated sets with appropriate
         *      @ref HandleFlags again using @ref DescriptorSet::wrap().
         *
         * @see @ref release()
         */
        static DescriptorPool wrap(Device& device, VkDescriptorPool handle, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param device    Vulkan device to create the descriptor pool on
         * @param info      Descriptor pool creation info
         *
         * @see @fn_vk_keyword{CreateDescriptorPool}
         */
        explicit DescriptorPool(Device& device, const DescriptorPoolCreateInfo& info);

        /**
         * @brief Construct without creating the fence
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit DescriptorPool(NoCreateT);

        /** @brief Copying is not allowed */
        DescriptorPool(const DescriptorPool&) = delete;

        /** @brief Move constructor */
        DescriptorPool(DescriptorPool&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{DescriptorPool} handle, unless the
         * instance was created using @ref wrap() without
         * @ref HandleFlag::DestroyOnDestruction specified.
         * @see @fn_vk_keyword{DestroyDescriptorPool}, @ref release()
         */
        ~DescriptorPool();

        /** @brief Copying is not allowed */
        DescriptorPool& operator=(const DescriptorPool&) = delete;

        /** @brief Move assignment */
        DescriptorPool& operator=(DescriptorPool&& other) noexcept;

        /** @brief Underlying @type_vk{DescriptorPool} handle */
        VkDescriptorPool handle() { return _handle; }
        /** @overload */
        operator VkDescriptorPool() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Allocate a single descriptor set
         *
         * If @p layout contains a binding with
         * @ref DescriptorSetLayoutBinding::Flag::VariableDescriptorCount set,
         * the allocated descriptor count will be @cpp 0 @ce. Use
         * @ref allocate(VkDescriptorSetLayout, UnsignedInt) in that case
         * instead.
         *
         * If allocation fails due to exhaustion of pool memory or due to
         * fragmentation, the function aborts with an error message. For
         * graceful handling of such failures use
         * @ref tryAllocate(VkDescriptorSetLayout) instead.
         * @see @ref reset(), @fn_vk_keyword{AllocateDescriptorSets}
         */
        DescriptorSet allocate(VkDescriptorSetLayout layout);

        /**
         * @brief Try to allocate a single descriptor set
         *
         * Compared to @ref allocate(VkDescriptorSetLayout), if the allocation
         * fails with @ref Result::ErrorOutOfPoolMemory or
         * @ref Result::ErrorFragmentedPool, @ref Containers::NullOpt is
         * returned instead of aborting to allow the application to recover and
         * choose a different strategy.
         *
         * If Vulkan 1.1 is not supported by the device and the
         * @vk_extension{KHR,maintenance1} extension isn't enabled on the
         * device, allocation failures are treated as user error the driver is
         * free to do basically anything. Fortunately most implementations
         * support this extension nowadays and so it should be safe to assume
         * @ref Result::ErrorOutOfPoolMemory gets properly returned in case of
         * a failure.
         */
        Containers::Optional<DescriptorSet> tryAllocate(VkDescriptorSetLayout layout);

        /**
         * @brief Allocate a single descriptor set with a variable descriptor count
         *
         * Compared to @ref allocate(VkDescriptorSetLayout), the
         * @p variableDescriptorCount is used for a binding that was created
         * with @ref DescriptorSetLayoutBinding::Flag::VariableDescriptorCount
         * and is expected to not be larger than the the count specified in the
         * layout.
         *
         * If allocation fails due to exhaustion of pool memory or due to
         * fragmentation, the function aborts with an error message. For
         * graceful handling of such failures use
         * @ref tryAllocate(VkDescriptorSetLayout, UnsignedInt) instead.
         * @see @type_vk_keyword{DescriptorSetVariableDescriptorCountAllocateInfo}
         * @requires_vk_feature @ref DeviceFeature::DescriptorBindingVariableDescriptorCount
         */
        DescriptorSet allocate(VkDescriptorSetLayout layout, UnsignedInt variableDescriptorCount);

        /**
         * @brief Try to allocate a single descriptor set with a variable descriptor count
         *
         * Compared to @ref allocate(VkDescriptorSetLayout, UnsignedInt), if
         * the allocation fails with @ref Result::ErrorOutOfPoolMemory or
         * @ref Result::ErrorFragmentedPool, @ref Containers::NullOpt is
         * returned instead of aborting to allow the application to recover and
         * choose a different strategy.
         *
         * If Vulkan 1.1 is not supported by the device and the
         * @vk_extension{KHR,maintenance1} extension isn't enabled on the
         * device, allocation failures are treated as user error the driver is
         * free to do basically anything. Fortunately most implementations
         * support this extension nowadays and so it should be safe to assume
         * @ref Result::ErrorOutOfPoolMemory gets properly returned in case of
         * a failure.
         */
        Containers::Optional<DescriptorSet> tryAllocate(VkDescriptorSetLayout layout, UnsignedInt variableDescriptorCount);

        /**
         * @brief Reset the pool
         *
         * Frees all descriptor sets allocated from this pool, making it empty
         * again.
         *
         * @m_class{m-block m-warning}
         *
         * @par DescriptorSet destruction order
         *      All @ref DescriptorSet instances returned from @ref allocate()
         *      / @ref tryAllocate() become invalid after calling this
         *      function. While by default the @ref DescriptorSet does nothing
         *      and such behavior is fine, for a pool with
         *      @ref DescriptorPoolCreateInfo::Flag::FreeDescriptorSet
         *      enabled this would mean @fn_vk{FreeDescriptorSets} gets called
         *      with invalid descriptor set handles. To prevent that from
         *      happening either ensure all @ref DescriptorSet instances are
         *      gone by the time you call @ref reset(), or explicitly call
         *      @ref DescriptorSet::release() on each to make them empty
         *      without freeing anything.
         *
         * @see @ref allocate(),
         *      @ref DescriptorPoolCreateInfo::Flag::FreeDescriptorSet,
         *      @fn_vk{ResetDescriptorPool}
         */
        void reset();

        /**
         * @brief Release the underlying Vulkan descriptor pool
         *
         * Releases ownership of the Vulkan descriptor pool and returns its
         * handle so @fn_vk{DestroyDescriptorPool} is not called on
         * destruction. The internal state is then equivalent to moved-from
         * state.
         * @see @ref wrap()
         */
        VkDescriptorPool release();

    private:
        MAGNUM_VK_LOCAL std::pair<Result, DescriptorSet> allocateInternal(VkDescriptorSetLayout layout);
        MAGNUM_VK_LOCAL std::pair<Result, DescriptorSet> allocateInternal(VkDescriptorSetLayout layout, UnsignedInt variableDescriptorCount);

        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkDescriptorPool _handle;
        HandleFlags _flags;
        bool _freeAllocatedSets;
};

}}

#endif
