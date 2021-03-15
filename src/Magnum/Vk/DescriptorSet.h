#ifndef Magnum_Vk_DescriptorSet_h
#define Magnum_Vk_DescriptorSet_h
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
 * @brief Class @ref Magnum::Vk::DescriptorSet
 * @m_since_latest
 */

#include "Magnum/Tags.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Descriptor set
@m_since_latest

Wraps a @type_vk_keyword{DescriptorSet}. A descriptor set is allocated from a
@ref DescriptorPool for a particular @ref DescriptorSetLayout and specifies
what descriptors (such as uniform buffers or samplers) are bound to shaders.

@section Vk-DescriptorSet-allocation Descriptor set allocation

Given a @ref DescriptorSetLayout and a compatible @ref DescriptorPool with
enough free slots, asingle descriptor set for given layout can be allocated
with @ref DescriptorPool::allocate():

@snippet MagnumVk.cpp DescriptorSet-allocation

When allocating more than what the pool has, the @ref DescriptorPool::allocate()
function aborts with an error message. In cases where the application is very
dynamic and cannot predict that a pools is large enough, you can use
@ref DescriptorPool::tryAllocate() instead and handle the failure gracefully
--- for example by recycling unused sets or by allocating from a different
pool:

@snippet MagnumVk.cpp DescriptorSet-allocation-try

@subsection Vk-DescriptorSet-allocation-free Freeing descriptor sets

By default, the @ref DescriptorSet destructor is a no-op and descriptor sets
are all freed together on a call to @ref DescriptorPool::reset(). At that point
all existing @ref DescriptorSet instances become invalid. Alternatively, the
pool can be created with @ref DescriptorPoolCreateInfo::Flag::FreeDescriptorSet,
which then makes a @ref DescriptorSet free itself on destruction, allowing more
descriptor sets to be allocated without resetting the whole pool. Using this
flag however can cause allocation to fail also due to pool fragmentation, not
just when exhausing all available resources:

@snippet MagnumVk.cpp DescriptorSet-allocation-free

@subsection Vk-DescriptorSet-allocation-variable Variable descriptor count allocation

If the descriptor set layout contains a descriptor with variable count (there
has to be at most one and it has to be the last binding), a concrete count is
specified in the call to @ref DescriptorPool::allocate(VkDescriptorSetLayout, UnsignedInt). Here the fragment shader can access up to 8 sampled images and
we're allocating four:

@snippet MagnumVk.cpp DescriptorSet-allocation-variable
*/
class MAGNUM_VK_EXPORT DescriptorSet {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device            Vulkan device the descriptor pool is
         *      created on
         * @param pool              Vulkan descriptor pool the set is
         *      allocated from
         * @param handle            The @type_vk{DescriptorSet} handle
         * @param flags             Handle flags
         *
         * The @p handle is expected to be originating from @p device. The
         * Vulkan descriptor set is by default not freed on destruction --- if
         * the handle comes from a pool with
         * @ref DescriptorPoolCreateInfo::Flag::FreeDescriptorSet set and you
         * want it to be freed on destruction, pass
         * @ref HandleFlag::DestroyOnDestruction to @p flags.
         * @see @ref release()
         */
        static DescriptorSet wrap(Device& device, VkDescriptorPool pool, VkDescriptorSet handle, HandleFlags flags = {});

        /**
         * @brief Construct without creating the fence
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit DescriptorSet(NoCreateT);

        /** @brief Copying is not allowed */
        DescriptorSet(const DescriptorSet&) = delete;

        /** @brief Move constructor */
        DescriptorSet(DescriptorSet&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Frees associated @type_vk{DescriptorSet} handle if it was allocated
         * from a pool with @ref DescriptorPoolCreateInfo::Flag::FreeDescriptorSet
         * set or if it was created using @ref wrap() with
         * @ref HandleFlag::DestroyOnDestruction specified. Otherwise does
         * nothing.
         * @see @fn_vk_keyword{FreeDescriptorSets}, @ref release()
         */
        ~DescriptorSet();

        /** @brief Copying is not allowed */
        DescriptorSet& operator=(const DescriptorSet&) = delete;

        /** @brief Move assignment */
        DescriptorSet& operator=(DescriptorSet&& other) noexcept;

        /** @brief Underlying @type_vk{DescriptorSet} handle */
        VkDescriptorSet handle() { return _handle; }
        /** @overload */
        operator VkDescriptorSet() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Release the underlying Vulkan descriptor set
         *
         * Releases ownership of the Vulkan descriptor set and returns its
         * handle so @fn_vk{FreeDescriptorSets} is not called on destruction.
         * The internal state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkDescriptorSet release();

    private:
        friend DescriptorPool;

        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkDescriptorPool _pool;
        VkDescriptorSet _handle;
        HandleFlags _flags;
};

}}

#endif
