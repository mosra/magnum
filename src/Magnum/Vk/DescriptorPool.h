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
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkDescriptorPool _handle;
        HandleFlags _flags;
};

}}

#endif
