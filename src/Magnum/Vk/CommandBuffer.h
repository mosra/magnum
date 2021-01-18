#ifndef Magnum_Vk_CommandBuffer_h
#define Magnum_Vk_CommandBuffer_h
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
 * @brief Class @ref Magnum::Vk::CommandBuffer, @ref Magnum::Vk::CommandBufferBeginInfo, enum @ref Magnum::Vk::CommandPoolResetFlag, enum set @ref Magnum::Vk::CommandPoolResetFlags
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Tags.h"
#include "Magnum/Magnum.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation { struct DeviceState; }

/**
@brief Command buffer begin info
@m_since_latest

Wraps a @type_vk_keyword{CommandBufferBeginInfo}. See
@ref Vk-CommandBuffer-usage "Command buffer usage" for more information.
@see @ref CommandBuffer::begin()
*/
class MAGNUM_VK_EXPORT CommandBufferBeginInfo {
    public:
        /**
         * @brief Command buffer begin flag
         *
         * Wraps @type_vk_keyword{CommandBufferUsageFlagBits}.
         * @see @ref Flags, @ref CommandBufferBeginInfo(Flags)
         */
        enum class Flag: UnsignedInt {
            /**
             * Each recording will be submitted only once and the command
             * buffer will be reset and recorded again between each submission.
             *
             * @m_class{m-note m-success}
             *
             * @par
             *      Setting this flag on single-use command buffers might help
             *      drivers pick a better tradeoff between CPU time spent
             *      optimizing the commands and GPU time spent executing them.
             */
            OneTimeSubmit = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,

            /**
             * Specifies that a @ref CommandBufferLevel::Secondary buffer is
             * entirely inside a render pass. Ignored for
             * @ref CommandBufferLevel::Primary command buffers (the default).
             */
            RenderPassContinue = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,

            /**
             * A command buffer can be resubmitted to a queue while it is in
             * the pending state, and recorded into multiple primary command
             * buffers.
             */
            SimultaneousUse = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
        };

        /**
         * @brief Command buffer begin flags
         *
         * Type-safe wrapper for @type_vk_keyword{CommandBufferUsageFlags}.
         * @see @ref CommandBufferBeginInfo(Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags         Command buffer begin flags
         *
         * The following @type_vk{CommandBufferBeginInfo} fields are pre-filled
         * in addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         */
        /* cmd.begin(CommandBufferBeginInfo::Flag::OneTimeSubmit) doesn't work
           anyway (would need an extra conversion from Flag to Flags), so no
           point in making this implicit. */
        explicit CommandBufferBeginInfo(Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit CommandBufferBeginInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit CommandBufferBeginInfo(const VkCommandBufferBeginInfo& info);

        /** @brief Underlying @type_vk{CommandBufferBeginInfo} structure */
        VkCommandBufferBeginInfo& operator*() { return _info; }
        /** @overload */
        const VkCommandBufferBeginInfo& operator*() const { return _info; }
        /** @overload */
        VkCommandBufferBeginInfo* operator->() { return &_info; }
        /** @overload */
        const VkCommandBufferBeginInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkCommandBufferBeginInfo*() const { return &_info; }

    private:
        VkCommandBufferBeginInfo _info;
};

CORRADE_ENUMSET_OPERATORS(CommandBufferBeginInfo::Flags)

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

Wraps a @type_vk_keyword{CommandBuffer}.

@section Vk-CommandBuffer-allocation Command buffer allocation and recycling

A command buffer instance is allocated from a @ref CommandPool as shown below.
Each command buffers is freed at the end of the @ref CommandBuffer instance
lifetime, you can also put all allocated buffers back to initial state by
calling @ref CommandPool::reset(), or alternatively reset each buffer
separately using @ref reset(), if the pool was created with
@ref CommandPoolCreateInfo::Flag::ResetCommandBuffer.

@snippet MagnumVk.cpp CommandBuffer-allocation

@section Vk-CommandBuffer-usage Command buffer recording and submit

A command buffer recording is initiated with @ref begin(), after which you can
execute commands that are allowed outside a render pass. A render pass is
delimited with @ref beginRenderPass() and @ref endRenderPass(), see
@ref Vk-RenderPass-usage for details. Once a recording is done, call
@ref end(). You can (but don't have to) use method chaining:

@snippet MagnumVk.cpp CommandBuffer-usage

Once recorded, the command buffer can be submitted to a compatible @ref Queue
that was set up at @ref Vk-Device-creation "device creation time". Usually
you'd want to wait on the submit completion with a @link Fence @endlink:

@snippet MagnumVk.cpp CommandBuffer-usage-submit
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

        /**
         * @brief Begin command buffer recording
         * @return Reference to self (for method chaining)
         *
         * @see @fn_vk_keyword{BeginCommandBuffer}
         */
        CommandBuffer& begin(const CommandBufferBeginInfo& info = CommandBufferBeginInfo{});

        /**
         * @brief End command buffer recording
         *
         * As this function is expected to be called last, it doesn't return a
         * reference to self for method chaining.
         * @see @fn_vk_keyword{EndCommandBuffer}
         */
        void end();

        /**
         * @brief Begin a new render pass
         * @return Reference to self (for method chaining)
         *
         * If Vulkan 1.2 is not supported and the
         * @vk_extension{KHR,create_renderpass2} extension is not enabled on
         * the device, only the `contents` field from @p beginInfo is used to
         * begin the render pass.
         * @see @fn_vk_keyword{CmdBeginRenderPass2},
         *      @fn_vk_keyword{CmdBeginRenderPass}
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        CommandBuffer& beginRenderPass(const RenderPassBeginInfo& info, const SubpassBeginInfo& beginInfo = SubpassBeginInfo{});
        #else
        /* To avoid dependency on RenderPass.h */
        CommandBuffer& beginRenderPass(const RenderPassBeginInfo& info, const SubpassBeginInfo& beginInfo);
        CommandBuffer& beginRenderPass(const RenderPassBeginInfo& info);
        #endif

        /**
         * @brief Transition to the next subpass of a render pass
         * @return Reference to self (for method chaining)
         *
         * If Vulkan 1.2 is not supported and the
         * @vk_extension{KHR,create_renderpass2} extension is not enabled on
         * the device, @p endInfo is ignored and only the `contents` field from
         * @p beginInfo is used to begin the next subpass
         * @see @fn_vk_keyword{CmdNextSubpass2},
         *      @fn_vk_keyword{CmdNextSubpass}
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        CommandBuffer& nextSubpass(const SubpassEndInfo& endInfo = SubpassEndInfo{}, const SubpassBeginInfo& beginInfo = SubpassBeginInfo{});
        /** @overload */
        CommandBuffer& nextSubpass(const SubpassBeginInfo& beginInfo);
        #else
        /* To avoid dependency on RenderPass.h */
        CommandBuffer& nextSubpass(const SubpassEndInfo& endInfo, const SubpassBeginInfo& beginInfo);
        CommandBuffer& nextSubpass(const SubpassEndInfo& endInfo);
        CommandBuffer& nextSubpass(const SubpassBeginInfo& beginInfo);
        CommandBuffer& nextSubpass();
        #endif

        /**
         * @brief Transition to the next subpass of a render pass
         * @return Reference to self (for method chaining)
         *
         * If Vulkan 1.2 is not supported and the
         * @vk_extension{KHR,create_renderpass2} extension is not enabled on
         * the device, @p endInfo is ignored.
         * @see @fn_vk_keyword{CmdEndRenderPass2},
         *      @fn_vk_keyword{CmdEndRenderPass}
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        CommandBuffer& endRenderPass(const SubpassEndInfo& endInfo = SubpassEndInfo{});
        #else
        CommandBuffer& endRenderPass(const SubpassEndInfo& endInfo);
        CommandBuffer& endRenderPass();
        #endif

    private:
        friend CommandPool;
        friend Implementation::DeviceState;

        MAGNUM_VK_LOCAL static void beginRenderPassImplementationDefault(CommandBuffer& self, const VkRenderPassBeginInfo& info, const VkSubpassBeginInfo& beginInfo);
        MAGNUM_VK_LOCAL static void beginRenderPassImplementationKHR(CommandBuffer& self, const VkRenderPassBeginInfo& info, const VkSubpassBeginInfo& beginInfo);
        MAGNUM_VK_LOCAL static void beginRenderPassImplementation12(CommandBuffer& self, const VkRenderPassBeginInfo& info, const VkSubpassBeginInfo& beginInfo);

        MAGNUM_VK_LOCAL static void nextSubpassImplementationDefault(CommandBuffer& self, const VkSubpassEndInfo& endInfo, const VkSubpassBeginInfo& beginInfo);
        MAGNUM_VK_LOCAL static void nextSubpassImplementationKHR(CommandBuffer& self, const VkSubpassEndInfo& endInfo, const VkSubpassBeginInfo& beginInfo);
        MAGNUM_VK_LOCAL static void nextSubpassImplementation12(CommandBuffer& self, const VkSubpassEndInfo& endInfo, const VkSubpassBeginInfo& beginInfo);

        MAGNUM_VK_LOCAL static void endRenderPassImplementationDefault(CommandBuffer& self, const VkSubpassEndInfo& endInfo);
        MAGNUM_VK_LOCAL static void endRenderPassImplementationKHR(CommandBuffer& self, const VkSubpassEndInfo& endInfo);
        MAGNUM_VK_LOCAL static void endRenderPassImplementation12(CommandBuffer& self, const VkSubpassEndInfo& endInfo);

        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkCommandPool _pool; /* Used only for vkFreeCommandBuffers() */
        VkCommandBuffer _handle;
        HandleFlags _flags;
};

}}

#endif
