#ifndef Magnum_Vk_Fence_h
#define Magnum_Vk_Fence_h
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
 * @brief Class @ref Magnum::Vk::Fence
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Fence
@m_since_latest

Wraps a @type_vk_keyword{Fence}.

@section Vk-Fence-creation Fence creation

A fence doesn't need any extra parameters for construction and can be
constructed directly using @ref Fence(Device&, const FenceCreateInfo&), leaving
the @p info parameter at its default. If you want to pass additional parameters
to it, include the @ref FenceCreateInfo class as usual:

@snippet MagnumVk.cpp Fence-creation
*/
class MAGNUM_VK_EXPORT Fence {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device            Vulkan device the fence is created on
         * @param handle            The @type_vk{Fence} handle
         * @param flags             Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * a fence created using a constructor, the Vulkan fence is by default
         * not deleted on destruction, use @p flags for different behavior.
         * @see @ref release()
         */
        static Fence wrap(Device& device, VkFence handle, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param device    Vulkan device to create the fence on
         * @param info      Fence creation info
         *
         * @see @fn_vk_keyword{CreateFence}
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit Fence(Device& device, const FenceCreateInfo& info = FenceCreateInfo{});
        #else
        explicit Fence(Device& device, const FenceCreateInfo& info);
        explicit Fence(Device& device);
        #endif

        /**
         * @brief Construct without creating the fence
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Fence(NoCreateT);

        /** @brief Copying is not allowed */
        Fence(const Fence&) = delete;

        /** @brief Move constructor */
        Fence(Fence&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{Fence} handle, unless the instance was
         * created using @ref wrap() without
         * @ref HandleFlag::DestroyOnDestruction specified.
         * @see @fn_vk_keyword{DestroyFence}, @ref release()
         */
        ~Fence();

        /** @brief Copying is not allowed */
        Fence& operator=(const Fence&) = delete;

        /** @brief Move assignment */
        Fence& operator=(Fence&& other) noexcept;

        /** @brief Underlying @type_vk{Fence} handle */
        VkFence handle() { return _handle; }
        /** @overload */
        operator VkFence() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Release the underlying Vulkan fence
         *
         * Releases ownership of the Vulkan fence and returns its handle so
         * @fn_vk{DestroyFence} is not called on destruction. The internal
         * state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkFence release();

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkFence _handle;
        HandleFlags _flags;
};

}}

#endif
