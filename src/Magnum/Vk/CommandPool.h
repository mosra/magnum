#ifndef Magnum_Vk_CommandPool_h
#define Magnum_Vk_CommandPool_h
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
 * @brief Class @ref Magnum::Vk::CommandPool, enum @ref Magnum::Vk::CommandBufferLevel, @ref Magnum::Vk::CommandPoolResetFlag, enum set @ref Magnum::Vk::CommandPoolResetFlags
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Tags.h"
#include "Magnum/Magnum.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Command buffer level
@m_since_latest

Wraps a @type_vk_keyword{CommandBufferLevel}.
@m_enum_values_as_keywords
@see @ref CommandPool::allocate()
*/
enum class CommandBufferLevel: Int {
    /** Primary command buffer */
    Primary = VK_COMMAND_BUFFER_LEVEL_PRIMARY,

    /** Secondary command buffer */
    Secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY
};

/**
@brief Command buffer reset flag
@m_since_latest

Wraps @type_vk_keyword{CommandPoolResetFlagBits}.
@m_enum_values_as_keywords
@see @ref CommandPoolResetFlags, @ref CommandPool::reset(),
    @ref CommandBufferResetFlag, @ref CommandBuffer::reset()
*/
enum class CommandPoolResetFlag: UnsignedInt {
    /** Recycle all resources from the command pool back to the system */
    ReleaseResources = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT
};

/**
@brief Command pool reset flags
@m_since_latest

Wraps @type_vk_keyword{CommandPoolResetFlags}.
@see @ref CommandPool::reset(), @ref CommandBufferResetFlags,
    @ref CommandBuffer::reset()
*/
typedef Containers::EnumSet<CommandPoolResetFlag> CommandPoolResetFlags;

CORRADE_ENUMSET_OPERATORS(CommandPoolResetFlags)

/**
@brief Command pool
@m_since_latest

Wraps a @type_vk_keyword{CommandPool} and handles allocation of
@ref CommandBuffer "CommandBuffer"s.

@section Vk-CommandPool-creation Command pool creation

A @ref CommandPoolCreateInfo doesn't need many inputs --- the only required is
queue family index coming from @ref DeviceProperties of the device it's created
on:

@snippet MagnumVk.cpp CommandPool-creation

After that, you can allocate command buffers and use them. See
@ref CommandBuffer class docs for details.
*/
class MAGNUM_VK_EXPORT CommandPool {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device        Vulkan device the command pool is created on
         * @param handle        The @type_vk{CommandPool} handle
         * @param flags         Handle flags
         *
         * The @p handle is expected to be of an existing Vulkan command pool.
         * Unlike a command pool created using a constructor, the Vulkan
         * command pool is by default not deleted on destruction, use @p flags
         * for different behavior.
         */
        static CommandPool wrap(Device& device, VkCommandPool handle, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param device    Vulkan device to create the command pool on
         * @param info      Command pool creation info
         *
         * @see @fn_vk_keyword{CreateCommandPool}
         */
        explicit CommandPool(Device& device, const CommandPoolCreateInfo& info);

        /**
         * @brief Construct without creating the instance
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit CommandPool(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        CommandPool(const CommandPool&) = delete;

        /** @brief Move constructor */
        CommandPool(CommandPool&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{CommandPool} handle, unless the
         * instance was created using @ref wrap() without @ref HandleFlag::DestroyOnDestruction
         * specified.
         * @see @fn_vk_keyword{DestroyCommandPool}, @ref release()
         */
        ~CommandPool();

        /** @brief Copying is not allowed */
        CommandPool& operator=(const CommandPool&) = delete;

        /** @brief Move assignment */
        CommandPool& operator=(CommandPool&& other) noexcept;

        /** @brief Underlying @type_vk{CommandPool} handle */
        VkCommandPool handle() { return _handle; }
        /** @overload */
        operator VkCommandPool() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Allocate a single command buffer
         *
         * @see @fn_vk_keyword{AllocateCommandBuffers}
         */
        CommandBuffer allocate(CommandBufferLevel level = CommandBufferLevel::Primary);

        /**
         * @brief Reset the command pool
         *
         * All command buffers allocated from this command pool are reset as
         * well. See @ref CommandBuffer::reset() for a way to reset a single
         * command buffer.
         * @see @fn_vk_keyword{ResetCommandPool}
         */
        void reset(CommandPoolResetFlags flags = {});

        /**
         * @brief Release the underlying Vulkan command pool
         *
         * Releases ownership of the Vulkan command pool and returns its handle
         * so @fn_vk{DestroyCommandPool} is not called on destruction. The
         * internal state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkCommandPool release();

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkCommandPool _handle;
        HandleFlags _flags;
};

}}

#endif
