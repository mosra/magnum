#ifndef Magnum_Vk_CommandBuffer_h
#define Magnum_Vk_CommandBuffer_h
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
 * @brief Class @ref Magnum::Vk::CommandBuffer, enum @ref Magnum::Vk::CommandPoolResetFlag, enum set @ref Magnum::Vk::CommandPoolResetFlags
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Reference.h>

#include "Magnum/Tags.h"
#include "Magnum/Magnum.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Command buffer reset flag
@m_since_latest

Wraps @type_vk_keyword{CommandBufferResetFlagBits}.
@m_enum_values_as_keywords
@see @ref CommandBufferResetFlags, @ref CommandBuffer::reset(),
    @ref CommandPoolResetFlag, @ref CommandPool::reset()
*/
enum class CommandBufferResetFlag: UnsignedInt {
    /** Recycle all resources from the command pool back to the system */
    ReleaseResources = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
};

/**
@brief Command buffer reset flags
@m_since_latest

Wraps @type_vk_keyword{CommandBufferResetFlags}.
@see @ref CommandBuffer::reset(), @ref CommandPoolResetFlags,
    @ref CommandPool::reset()
*/
typedef Containers::EnumSet<CommandBufferResetFlag> CommandBufferResetFlags;

CORRADE_ENUMSET_OPERATORS(CommandBufferResetFlags)

/**
@brief Command buffer
@m_since_latest

Wraps a @type_vk_keyword{CommandBuffer}. A command buffer instance is usually
allocated from a @ref CommandPool, see its documentation for usage information.
*/
class MAGNUM_VK_EXPORT CommandBuffer {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device        Vulkan device the command buffer is created on
         * @param pool          Command pool the buffer is allocated from
         * @param handle        The @type_vk{CommandBuffer} handle
         * @param flags         Handle flags
         *
         * The @p handle is expected to be of an existing Vulkan command
         * buffer. Unlike a command buffer allocated using
         * @ref CommandPool::allocate(), the Vulkan command buffer is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         */
        static CommandBuffer wrap(Device& device, VkCommandPool pool, VkCommandBuffer handle, HandleFlags flags = {});

        /**
         * @brief Construct without creating the instance
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit CommandBuffer(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        CommandBuffer(const CommandBuffer&) = delete;

        /** @brief Move constructor */
        CommandBuffer(CommandBuffer&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Frees associated @type_vk{CommandBuffer} handle, unless the
         * instance was created using @ref wrap() without @ref HandleFlag::DestroyOnDestruction
         * specified.
         * @see @fn_vk_keyword{FreeCommandBuffers}, @ref release()
         */
        ~CommandBuffer();

        /** @brief Copying is not allowed */
        CommandBuffer& operator=(const CommandBuffer&) = delete;

        /** @brief Move assignment */
        CommandBuffer& operator=(CommandBuffer&& other) noexcept;

        /** @brief Underlying @type_vk{CommandBuffer} handle */
        VkCommandBuffer handle() { return _handle; }
        /** @overload */
        operator VkCommandBuffer() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Reset the command buffer
         *
         * This operation is allowed only if the originating @ref CommandPool
         * was created with @ref CommandPoolCreateInfo::Flag::ResetCommandBuffer.
         * If not, the only way to reset is to reset the whole command pool
         * using @ref CommandPool::reset().
         * @see @fn_vk_keyword{ResetCommandBuffer}
         */
        void reset(CommandBufferResetFlags flags = {});

        /**
         * @brief Release the underlying Vulkan command buffer
         *
         * Releases ownership of the Vulkan command buffer and returns its
         * handle so @fn_vk{FreeCommandBuffers} is not called on destruction.
         * The internal state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkCommandBuffer release();

    private:
        friend CommandPool;

        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkCommandPool _pool; /* Used only for vkFreeCommandBuffers() */
        VkCommandBuffer _handle;
        HandleFlags _flags;
};

}}

#endif
