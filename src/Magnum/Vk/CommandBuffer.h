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

#include <initializer_list>
#include <Corrade/Containers/BigEnumSet.h>
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
         * @brief Dynamic states used by currently bound rasterization pipeline
         *
         * If no rasterization pipeline is bound or there are no dynamic states
         * on the currently bound one, returns an empty set.
         * @see @ref bindPipeline()
         */
        DynamicRasterizationStates dynamicRasterizationStates() const {
            return _dynamicRasterizationStates;
        }

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
         * the device, @p beginInfo has to have the `pNext` chain empty and
         * only the `contents` field from it is used to begin the render pass.
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
         * the device, both @p endInfo and @p beginInfo have to have the
         * `pNext` chain empty and only the `contents` field from @p beginInfo
         * is used to begin the next subpass.
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
         * the device, @p endInfo has to have the `pNext` chain empty.
         * @see @fn_vk_keyword{CmdEndRenderPass2},
         *      @fn_vk_keyword{CmdEndRenderPass}
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        CommandBuffer& endRenderPass(const SubpassEndInfo& endInfo = SubpassEndInfo{});
        #else
        CommandBuffer& endRenderPass(const SubpassEndInfo& endInfo);
        CommandBuffer& endRenderPass();
        #endif

        /**
         * @brief Bind a pipeline
         * @return Reference to self (for method chaining)
         *
         * Can be called both inside and outside a render pass. If the pipeline
         * is a rasterization pipeline, the set of its dynamic states is stored
         * in @ref dynamicRasterizationStates() for use by drawing and other
         * commands. See @ref Vk-Pipeline-usage for a usage example.
         * @see @fn_vk_keyword{CmdBindPipeline}
         */
        CommandBuffer& bindPipeline(Pipeline& pipeline);

        /**
         * @brief Insert an execution barrier with optional memory dependencies
         * @param sourceStages          Source stages. Has to contain at least
         *      one stage.
         * @param destinationStages     Destination stages. Has to contain at
         *      least one stage.
         * @param memoryBarriers        Global memory barriers, affecting all
         *      memory
         * @param bufferMemoryBarriers  Buffer memory barriers, restricted to
         *      a particular buffer
         * @param imageMemoryBarriers   Image memory barriers, restricted to a
         *      particular image. These are also used for performing
         *      @ref ImageLayout transitions.
         * @param dependencyFlags       Dependency flags
         * @return Reference to self (for method chaining)
         *
         * Can be called both inside and outside a render pass. When called
         * inside a render pass:
         *
         * -    the render pass has to contain at least one
         *      @ref SubpassDependency to itself, with scopes that are a
         *      superset of scopes defined here
         * -    @p bufferMemoryBarriers has to be empty,
         * -    all images in @p imageMemoryBarriers have to be contained
         *      in both @ref SubpassDescription::setInputAttachments() and
         *      @ref SubpassDescription::setColorAttachments() /
         *      @ref SubpassDescription::setDepthStencilAttachment(),
         * -    and old and new @ref ImageLayout in @p imageMemoryBarriers have
         *      to be equal
         *
         * See @ref Vk-Buffer-usage-copy, @ref Vk-Image-usage-clear and
         * @ref Vk-Image-usage-copy for usage examples.
         * @see @fn_vk_keyword{CmdPipelineBarrier}
         */
        CommandBuffer& pipelineBarrier(PipelineStages sourceStages, PipelineStages destinationStages, Containers::ArrayView<const MemoryBarrier> memoryBarriers, Containers::ArrayView<const BufferMemoryBarrier> bufferMemoryBarriers, Containers::ArrayView<const ImageMemoryBarrier> imageMemoryBarriers, DependencyFlags dependencyFlags = {});
        /** @overload */
        CommandBuffer& pipelineBarrier(PipelineStages sourceStages, PipelineStages destinationStages, std::initializer_list<MemoryBarrier> memoryBarriers, std::initializer_list<BufferMemoryBarrier> bufferMemoryBarriers, std::initializer_list<ImageMemoryBarrier> imageMemoryBarriers, DependencyFlags dependencyFlags = {});

        /**
         * @brief Insert an execution barrier without memory dependencies
         * @return Reference to self (for method chaining)
         *
         * Equivalent to calling @ref pipelineBarrier(PipelineStages, PipelineStages, Containers::ArrayView<const MemoryBarrier>, Containers::ArrayView<const BufferMemoryBarrier>, Containers::ArrayView<const ImageMemoryBarrier>, DependencyFlags)
         * with empty @p memoryBarriers, @p bufferBarriers and
         * @p imageBarriers. Useful when an execution barrier is enough, for
         * example when you need reads from one stage to finish before another
         * stage starts writing to the same location.
         */
        CommandBuffer& pipelineBarrier(PipelineStages sourceStages, PipelineStages destinationStages, DependencyFlags dependencyFlags = {});

        /**
         * @brief Insert a global memory dependency
         * @return Reference to self (for method chaining)
         *
         * Equivalent to calling @ref pipelineBarrier(PipelineStages, PipelineStages, Containers::ArrayView<const MemoryBarrier>, Containers::ArrayView<const BufferMemoryBarrier>, Containers::ArrayView<const ImageMemoryBarrier>, DependencyFlags)
         * with empty @p bufferBarriers and @p imageBarriers.
         */
        CommandBuffer& pipelineBarrier(PipelineStages sourceStages, PipelineStages destinationStages, Containers::ArrayView<const MemoryBarrier> memoryBarriers, DependencyFlags dependencyFlags = {});
        /** @overload */
        CommandBuffer& pipelineBarrier(PipelineStages sourceStages, PipelineStages destinationStages, std::initializer_list<MemoryBarrier> memoryBarriers, DependencyFlags dependencyFlags = {});

        /**
         * @brief Insert a buffer memory dependency
         * @return Reference to self (for method chaining)
         *
         * Equivalent to calling @ref pipelineBarrier(PipelineStages, PipelineStages, Containers::ArrayView<const MemoryBarrier>, Containers::ArrayView<const BufferMemoryBarrier>, Containers::ArrayView<const ImageMemoryBarrier>, DependencyFlags)
         * with empty @p memoryBarriers and @p imageBarriers. See
         * @ref Vk-Buffer-usage-copy for usage examples.
         */
        CommandBuffer& pipelineBarrier(PipelineStages sourceStages, PipelineStages destinationStages, Containers::ArrayView<const BufferMemoryBarrier> bufferMemoryBarriers, DependencyFlags dependencyFlags = {});
        /** @overload */
        CommandBuffer& pipelineBarrier(PipelineStages sourceStages, PipelineStages destinationStages, std::initializer_list<BufferMemoryBarrier> bufferMemoryBarriers, DependencyFlags dependencyFlags = {});

        /**
         * @brief Insert an image memory dependency
         * @return Reference to self (for method chaining)
         *
         * Equivalent to calling @ref pipelineBarrier(PipelineStages, PipelineStages, Containers::ArrayView<const MemoryBarrier>, Containers::ArrayView<const BufferMemoryBarrier>, Containers::ArrayView<const ImageMemoryBarrier>, DependencyFlags)
         * with empty @p memoryBarriers and @p bufferBarriers. See
         * @ref Vk-Image-usage-clear and @ref Vk-Image-usage-copy for usage
         * examples.
         */
        CommandBuffer& pipelineBarrier(PipelineStages sourceStages, PipelineStages destinationStages, Containers::ArrayView<const ImageMemoryBarrier> imageMemoryBarriers, DependencyFlags dependencyFlags = {});
        /** @overload */
        CommandBuffer& pipelineBarrier(PipelineStages sourceStages, PipelineStages destinationStages, std::initializer_list<ImageMemoryBarrier> imageMemoryBarriers, DependencyFlags dependencyFlags = {});

        /**
         * @brief Fill a buffer region with a fixed value
         * @param buffer    Source @p Buffer or a raw Vulkan buffer handle to
         *      fill. Expected to have been created with
         *      @ref BufferUsage::TransferDestination.
         * @param offset    Offset of the region to fill, in bytes
         * @param size      Size of the region to fill, in bytes
         * @param value     A 4-byte value to repeat in the region, interpreted
         *      accoding to the machine endianness. If @p size is not divisible
         *      by 4, the last remaining bytes are not filled.
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. See @ref Vk-Buffer-usage-fill
         * for a usage example.
         * @see @ref clearColorImage(), @ref clearDepthStencilImage(),
         *      @ref clearDepthImage(), @ref clearStencilImage(),
         *      @fn_vk_keyword{CmdFillBuffer}
         */
        CommandBuffer& fillBuffer(VkBuffer buffer, UnsignedLong offset, UnsignedLong size, UnsignedInt value);

        /**
         * @brief Fill the whole buffer with a fixed value
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. Equivalent to
         * @ref fillBuffer(VkBuffer, UnsignedLong, UnsignedLong, UnsignedInt)
         * with @p offset set to @cpp 0 @ce and @p size to @def_vk{WHOLE_SIZE}.
         */
        CommandBuffer& fillBuffer(VkBuffer buffer, UnsignedInt value) {
            return fillBuffer(buffer, 0, VK_WHOLE_SIZE, value);
        }

        /**
         * @brief Clear a floating-point or normalized color image
         * @param image         An @ref Image or a raw Vulkan image handle to
         *      clear. Expected to have been created with
         *      @ref ImageUsage::TransferDestination and a floating-point or
         *      normalized non-compressed @ref PixelFormat usable for transfer
         *      destination.
         * @param layout        Image layout. Can be either
         *      @ref ImageLayout::General or
         *      @ref ImageLayout::TransferDestination.
         * @param color         Color to clear the image with
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. For clearing inside a render
         * pass you can either specify @ref AttachmentLoadOperation::Clear for
         * a particular attachment, which will clear it on render pass begin
         * (see @ref Vk-RenderPass-usage for an example), or use
         * @fn_vk{CmdClearAttachments} which allows you to clear at any time
         * inside a render pass. See @ref Vk-Image-usage-clear for a usage
         * example.
         *
         * A single @type_vk{ImageSubresourceRange} is passed to the command,
         * with the following fields are set:
         *
         * -    `aspectMask` to @val_vk{IMAGE_ASPECT_COLOR,ImageAspectFlagBits}
         * -    `baseMipLevel` to @cpp 0 @ce
         * -    `levelCount` to @def_vk{REMAINING_MIP_LEVELS}
         * -    `baseArrayLayer` to @cpp 0 @ce
         * -    `layerCount` to @def_vk{REMAINING_ARRAY_LAYERS}
         *
         * @attention This function currently clears all layers and mip levels
         *      of the image, with no ability to specify particular layer/level
         *      ranges. For that please use the Vulkan API directly.
         *
         * @see @fn_vk_keyword{CmdClearColorImage}
         *
         * @todoc mention ImageLayout::SharedPresent being allowed once the
         *      extension is exposed
         */
        CommandBuffer& clearColorImage(VkImage image, ImageLayout layout, const Color4& color);

        /**
         * @brief Clear a signed integral color image
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. Behaves like
         * @ref clearColorImage(VkImage, ImageLayout, const Color4&), except
         * that it's meant for images with a signed integral non-compressed
         * @ref PixelFormat.
         */
        CommandBuffer& clearColorImage(VkImage image, ImageLayout layout, const Vector4i& color);

        /**
         * @brief Clear an unsigned integral color image
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. Behaves like
         * @ref clearColorImage(VkImage, ImageLayout, const Color4&), except
         * that it's meant for images with an unsigned integral non-compressed
         * @ref PixelFormat.
         */
        CommandBuffer& clearColorImage(VkImage image, ImageLayout layout, const Vector4ui& color);

        /**
         * @brief Clear a combined depth/stencil image
         * @param image         An @ref Image or a raw Vulkan image handle to
         *      clear. Expected to have been be created with
         *      @ref ImageUsage::TransferDestination and a combined
         *      depth/stencil @ref PixelFormat usable for transfer destination.
         * @param layout        Image layout. Can be either
         *      @ref ImageLayout::General or
         *      @ref ImageLayout::TransferDestination.
         * @param depth         Depth value to clear with
         * @param stencil       Stencil value to clear with
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. For clearing inside a render
         * pass you can either specify @ref AttachmentLoadOperation::Clear for
         * a particular attachment, which will clear it on render pass begin
         * (see @ref Vk-RenderPass-usage for an example), or use
         * @fn_vk{CmdClearAttachments} which allows you to clear at any point
         * inside a render pass. For clearing a depth-only or a stencil-only
         * image (or just a depth orstencil buffer of a combined depth/stencil
         * image) use @ref clearDepthImage() or @ref clearStencilImage()
         * instead. See @ref Vk-Image-usage-clear for a usage example.
         *
         * A single @type_vk{ImageSubresourceRange} is passed to the command,
         * with the following fields are set:
         *
         * -    `aspectMask` to @val_vk{IMAGE_ASPECT_DEPTH,ImageAspectFlagBits}
         *      and @val_vk{IMAGE_ASPECT_STENCIL,ImageAspectFlagBits}
         * -    `baseMipLevel` to @cpp 0 @ce
         * -    `levelCount` to @def_vk{REMAINING_MIP_LEVELS}
         * -    `baseArrayLayer` to @cpp 0 @ce
         * -    `layerCount` to @def_vk{REMAINING_ARRAY_LAYERS}
         *
         * @attention This function currently clears all layers and mip levels
         *      of the image, with no ability to specify particular layer/level
         *      ranges. For that please use the Vulkan API directly.
         *
         * @see @fn_vk_keyword{CmdClearDepthStencilImage}
         */
        CommandBuffer& clearDepthStencilImage(VkImage image, ImageLayout layout, Float depth, UnsignedInt stencil);

        /**
         * @brief Clear a depth-only image
         * @param image         An @ref Image or a raw Vulkan image handle to
         *      clear. Expected to have been be created with
         *      @ref ImageUsage::TransferDestination and a depth-only
         *      @ref PixelFormat usable for transfer destination.
         * @param layout        Image layout. Can be either
         *      @ref ImageLayout::General or
         *      @ref ImageLayout::TransferDestination.
         * @param depth         Depth value to clear with
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. For clearing inside a render
         * pass you can either specify @ref AttachmentLoadOperation::Clear for
         * a particular attachment, which will clear it on render pass begin
         * (see @ref Vk-RenderPass-usage for an example), or use
         * @fn_vk{CmdClearAttachments} which allows you to clear at any point
         * inside a render pass. For clearing a combined depth/stencil image
         * use @ref clearDepthStencilImage(), for clearing a stencil-only image
         * or just the stencil buffer of a combined depth/stencil image use
         * @ref clearStencilImage() instead. See @ref Vk-Image-usage-clear for
         * a usage example.
         *
         * A single @type_vk{ImageSubresourceRange} is passed to the command,
         * with the following fields are set:
         *
         * -    `aspectMask` to @val_vk{IMAGE_ASPECT_DEPTH,ImageAspectFlagBits}
         * -    `baseMipLevel` to @cpp 0 @ce
         * -    `levelCount` to @def_vk{REMAINING_MIP_LEVELS}
         * -    `baseArrayLayer` to @cpp 0 @ce
         * -    `layerCount` to @def_vk{REMAINING_ARRAY_LAYERS}
         *
         * @attention This function currently clears all layers and mip levels
         *      of the image, with no ability to specify particular layer/level
         *      ranges. For that please use the Vulkan API directly.
         *
         * @see @fn_vk_keyword{CmdClearDepthStencilImage}
         */
        CommandBuffer& clearDepthImage(VkImage image, ImageLayout layout, Float depth);

        /**
         * @brief Clear a stencil-only image
         * @param image         An @ref Image or a raw Vulkan image handle to
         *      clear. Expected to have been be created with
         *      @ref ImageUsage::TransferDestination and a stencil-only
         *      @ref PixelFormat usable for transfer destination.
         * @param layout        Image layout. Can be either
         *      @ref ImageLayout::General or
         *      @ref ImageLayout::TransferDestination.
         * @param stencil       Stencil value to clear with
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. For clearing inside a render
         * pass you can either specify @ref AttachmentLoadOperation::Clear for
         * a particular attachment, which will clear it on render pass begin
         * (see @ref Vk-RenderPass-usage for an example), or use
         * @fn_vk{CmdClearAttachments} which allows you to clear at any point
         * inside a render pass. For clearing a combined depth/stencil image
         * use @ref clearDepthStencilImage(), for clearing a depth-only image
         * or just the depth buffer of a combined depth/stencil image use
         * @ref clearDepthImage() instead. See @ref Vk-Image-usage-clear for a
         * usage example.
         *
         * A single @type_vk{ImageSubresourceRange} is passed to the command,
         * with the following fields set:
         *
         * -    `aspectMask` to @val_vk{IMAGE_ASPECT_STENCIL,ImageAspectFlagBits}
         * -    `baseMipLevel` to @cpp 0 @ce
         * -    `levelCount` to @def_vk{REMAINING_MIP_LEVELS}
         * -    `baseArrayLayer` to @cpp 0 @ce
         * -    `layerCount` to @def_vk{REMAINING_ARRAY_LAYERS}
         *
         * @attention This function currently clears all layers and mip levels
         *      of the image, with no ability to specify particular layer/level
         *      ranges. For that please use the Vulkan API directly.
         *
         * @see @fn_vk_keyword{CmdClearDepthStencilImage}
         */
        CommandBuffer& clearStencilImage(VkImage image, ImageLayout layout, UnsignedInt stencil);

        /** @todo clearAttachments(), I'm too lazy to expose all the needed
            structures right now */

        /**
         * @brief Copy data between buffer regions
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. If the
         * @vk_extension{KHR,copy_commands2} extension is not supported and
         * enabled on the device, the `pNext` chain in @p info and its
         * substructures has to be empty. See @ref Vk-Buffer-usage-copy for
         * details and usage examples.
         * @see @fn_vk_keyword{CmdCopyBuffer2KHR},
         *      @fn_vk_keyword{CmdCopyBuffer}
         */
        CommandBuffer& copyBuffer(const CopyBufferInfo& info);

        /**
         * @brief Copy data between images
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. If the
         * @vk_extension{KHR,copy_commands2} extension is not supported
         * and enabled on the device, the `pNext` chain in @p info and its
         * substructures has to be empty. See @ref Vk-Image-usage-copy for
         * details and usage examples.
         * @see @fn_vk_keyword{CmdCopyImage2KHR},
         *      @fn_vk_keyword{CmdCopyImage}
         */
        CommandBuffer& copyImage(const CopyImageInfo& info);

        /**
         * @brief Copy data from a buffer into an image
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. If the
         * @vk_extension{KHR,copy_commands2} extension is not supported and
         * enabled on the device, the `pNext` chain in @p info and its
         * substructures has to be empty. See @ref Vk-Image-usage-copy for
         * details and usage examples.
         * @see @fn_vk_keyword{CmdCopyBufferToImage2KHR},
         *      @fn_vk_keyword{CmdCopyBufferToImage}
         */
        CommandBuffer& copyBufferToImage(const CopyBufferToImageInfo& info);

        /**
         * @brief Copy image data into a buffer
         * @return Reference to self (for method chaining)
         *
         * Allowed only outside of a render pass. If the
         * @vk_extension{KHR,copy_commands2} extension is not supported and
         * enabled on the device, the `pNext` chain in @p info and its
         * substructures has to be empty. See @ref Vk-Image-usage-copy for
         * details and usage examples.
         * @see @fn_vk_keyword{CmdCopyImageToBuffer2KHR},
         *      @fn_vk_keyword{CmdCopyImageToBuffer}
         */
        CommandBuffer& copyImageToBuffer(const CopyImageToBufferInfo& info);

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

        MAGNUM_VK_LOCAL static void copyBufferImplementationDefault(CommandBuffer& self, const CopyBufferInfo& info);
        MAGNUM_VK_LOCAL static void copyBufferImplementationKHR(CommandBuffer& self, const CopyBufferInfo& info);

        MAGNUM_VK_LOCAL static void copyImageImplementationDefault(CommandBuffer& self, const CopyImageInfo& info);
        MAGNUM_VK_LOCAL static void copyImageImplementationSwiftShader(CommandBuffer& self, const CopyImageInfo& info);
        MAGNUM_VK_LOCAL static void copyImageImplementationKHR(CommandBuffer& self, const CopyImageInfo& info);

        MAGNUM_VK_LOCAL static void copyBufferToImageImplementationDefault(CommandBuffer& self, const CopyBufferToImageInfo& info);
        MAGNUM_VK_LOCAL static void copyBufferToImageImplementationSwiftShader(CommandBuffer& self, const CopyBufferToImageInfo& info);
        MAGNUM_VK_LOCAL static void copyBufferToImageImplementationKHR(CommandBuffer& self, const CopyBufferToImageInfo& info);

        MAGNUM_VK_LOCAL static void copyImageToBufferImplementationDefault(CommandBuffer& self, const CopyImageToBufferInfo& info);
        MAGNUM_VK_LOCAL static void copyImageToBufferImplementationSwiftShader(CommandBuffer& self, const CopyImageToBufferInfo& info);
        MAGNUM_VK_LOCAL static void copyImageToBufferImplementationKHR(CommandBuffer& self, const CopyImageToBufferInfo& info);

        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkCommandPool _pool; /* Used only for vkFreeCommandBuffers() */
        VkCommandBuffer _handle;
        HandleFlags _flags;
        DynamicRasterizationStates _dynamicRasterizationStates;
};

}}

#endif
