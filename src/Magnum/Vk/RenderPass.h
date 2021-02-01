#ifndef Magnum_Vk_RenderPass_h
#define Magnum_Vk_RenderPass_h
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
 * @brief Class @ref Magnum::Vk::RenderPass, @ref Magnum::Vk::RenderPassBeginInfo, @ref Magnum::Vk::SubpassBeginInfo, @ref Magnum::Vk::SubpassEndInfo, enum @ref Magnum::Vk::SubpassContents
 * @m_since_latest
 */

#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation { struct DeviceState; }

/**
@brief Render pass
@m_since_latest

Wraps a @type_vk_keyword{RenderPass}, represents a collection of attachment
descriptions, subpasses and their dependencies. The render pass description is
independent of any specific image views used for attachments, these two are
connected together in a @ref Framebuffer.

@section Vk-RenderPass-creation Render pass creation

A @ref RenderPassCreateInfo consists of:

-   a set of attachments, described by @ref AttachmentDescription instances,
-   subpasses operating on those attachments, described by a
    @ref SubpassDescription using @ref AttachmentReference instances,
-   and subpass dependencies, described by @ref SubpassDependency.

A render pass has to have at least one subpass. It's common to have just one
subpass, but while the subpass isn't required to operate on any attachments,
such case is rather rare. Following is a simple setup for one subpass operating
on a color and a combined depth/stencil attachment. Each
@ref AttachmentDescription describes a concrete attachment and particular
@ref SubpassDescription::setColorAttachments() /
@ref SubpassDescription::setDepthStencilAttachment() "setColorAttachments()"
then reference it by an index.

In addition to a format, each @ref AttachmentDescription has to specify an
@ref AttachmentLoadOperation that happens at the render pass begin --- whether
we want to preserve the contents, clear them or discard --- and similarly an
@ref AttachmentStoreOperation at the end. Then, the @ref ImageLayout parameters
specify what implicit layout transitions (if any) need to happen between start
of the render pass and the first subpass, between subpasses and between last
subpass and end of the render pass.

The following snippet shows attachment and subpass setup for a single-pass
color/depth render. Commonly the image is used in some way after the render
pass, in this case it'll get transferred to the host, as indicated by
@ref ImageLayout::TransferSource. That will also need an explicit subpass
dependency, as explained below.

@snippet MagnumVk.cpp RenderPass-creation

<b></b>

@m_class{m-note m-info}

@par
    @ref ImageLayout::Undefined also doubles as a way of saying "transition
    from whatever layout is there" --- when we render to an attachment for the
    first time, it will most probably be in @ref ImageLayout::Undefined, but
    subsequently it won't anymore and instead be
    @ref ImageLayout::ColorAttachment,
    @ref ImageLayout::DepthStencilAttachment "DepthStencilAttachment" or just
    any other. With @ref ImageLayout::Undefined we don't need to explicitly
    handle that case.

@subsection Vk-RenderPass-creation-subpass-dependencies Subpass dependencies

As shown above, the application is required to specify @ref ImageLayout at the
start and end of the render pass as well as layouts in which the attachments
are expected to be inside a particular subpass, and Vulkan then takes care of
doing layout transitions at a proper time. If a transition from one layout to
another needs to occur, a @ref SubpassDependency defines when, and if none is
explicitly specified, an implicit dependency is added by Vulkan. The above
setup can be visualized as this, with implicit transitions shown as arrows:

@dotfile vk-renderpass-layouts.dot

The implicit dependencies added by Vulkan only ensure that the transition from
@ref ImageLayout::Undefined to @ref ImageLayout::ColorAttachment "ColorAttachment"
/ @ref ImageLayout::DepthStencilAttachment "DepthStencilAttachment" happen *at
some point* before the start of the renderpass, and the transition to
@ref ImageLayout::TransferSource "TransferSource" is *eventually* done as well.
In this case the initial transition is fine; for the transfer layout transition
we however need it to happen before we do the actual transfer:

@snippet MagnumVk.cpp RenderPass-dependencies

@section Vk-RenderPass-usage Render pass recording

A render pass recording inside a @ref CommandBuffer is begun with
@ref CommandBuffer::beginRenderPass(), to which point Vulkan schedules all
implicit layout transitions. Together with a render pass you need to supply a
compatible @ref Framebuffer containing actual image views for each attachment
and a clear value for each attachment that had
@ref AttachmentLoadOperation::Clear specified. After that you can execute
@ref CommandBuffer commands that are allowed inside a render pass:

@snippet MagnumVk.cpp RenderPass-usage-begin

Advancing to the next subpass (if any) can be done with
@ref CommandBuffer::nextSubpass() "nextSubpass()", and finally
@ref CommandBuffer::endRenderPass() "endRenderPass()" ends the render pass. As
with render pass begin, these make Vulkan schedule implicit layout transitions
between subpasses and at render pass end.

@snippet MagnumVk.cpp RenderPass-usage-end
*/
class MAGNUM_VK_EXPORT RenderPass {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device    Vulkan device the render pass is created on
         * @param handle    The @type_vk{RenderPass} handle
         * @param flags     Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * a render pass created using a constructor, the Vulkan render pass is
         * by default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static RenderPass wrap(Device& device, VkRenderPass handle, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param device    Vulkan device to create the render pass on
         * @param info      Render pass creation info
         *
         * If Vulkan 1.2 is not supported and the
         * @vk_extension{KHR,create_renderpass2} extension is not enabled on
         * @p device, only the subset provided by
         * @ref RenderPassCreateInfo::vkRenderPassCreateInfo() is used to
         * create the render pass.
         * @see @fn_vk_keyword{CreateRenderPass2},
         *      @fn_vk_keyword{CreateRenderPass}
         */
        explicit RenderPass(Device& device, const RenderPassCreateInfo& info);

        /**
         * @brief Construct without creating the render pass
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit RenderPass(NoCreateT);

        /** @brief Copying is not allowed */
        RenderPass(const RenderPass&) = delete;

        /** @brief Move constructor */
        RenderPass(RenderPass&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{RenderPass} handle, unless the instance
         * was created using @ref wrap() without
         * @ref HandleFlag::DestroyOnDestruction specified.
         * @see @fn_vk_keyword{DestroyRenderPass}, @ref release()
         */
        ~RenderPass();

        /** @brief Copying is not allowed */
        RenderPass& operator=(const RenderPass&) = delete;

        /** @brief Move assignment */
        RenderPass& operator=(RenderPass&& other) noexcept;

        /** @brief Underlying @type_vk{RenderPass} handle */
        VkRenderPass handle() { return _handle; }
        /** @overload */
        operator VkRenderPass() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Release the underlying Vulkan render pass
         *
         * Releases ownership of the Vulkan render pass and returns its handle
         * so @fn_vk{DestroyRenderPass} is not called on destruction. The
         * internal state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkRenderPass release();

    private:
        friend Implementation::DeviceState;

        MAGNUM_VK_LOCAL static VkResult createImplementationDefault(Device&, const RenderPassCreateInfo&, const VkAllocationCallbacks*, VkRenderPass*);
        MAGNUM_VK_LOCAL static VkResult createImplementationKHR(Device&, const RenderPassCreateInfo&, const VkAllocationCallbacks*, VkRenderPass*);
        MAGNUM_VK_LOCAL static VkResult createImplementation12(Device&, const RenderPassCreateInfo&, const VkAllocationCallbacks*, VkRenderPass*);

        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkRenderPass _handle;
        HandleFlags _flags;
};

/**
@brief Render pass begin info
@m_since_latest

Wraps a @type_vk_keyword{RenderPassBeginInfo}. See @ref Vk-RenderPass-usage for
more information.
@see @ref CommandBuffer::beginRenderPass()
*/
class MAGNUM_VK_EXPORT RenderPassBeginInfo {
    public:
        /**
         * @brief Constructor
         * @param renderPass        A @ref RenderPass or a raw Vulkan render
         *      pass handle to begin an instance of
         * @param framebuffer       A @ref Framebuffer or a raw Vulkan
         *      framebuffer containing the attachments used with @p renderPass
         * @param renderArea        Render area affected by the render pass
         *      instance
         *
         * The following @type_vk{RenderPassBeginInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `renderPass`
         * -    `framebuffer`
         * -    `renderArea`
         *
         * If there are attachments with @ref AttachmentLoadOperation::Clear
         * passed to @ref RenderPassCreateInfo::setAttachments() of
         * @p renderPass, you need to call @ref clearColor() /
         * @ref clearDepthStencil() with an attachment index corresponding to
         * each of them.
         */
        explicit RenderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer framebuffer, const Range2Di& renderArea);

        /**
         * @brief Construct for a render pass spanning the whole framebuffer area
         *
         * Equivalent to calling @ref RenderPassBeginInfo(VkRenderPass, VkFramebuffer, const Range2Di&)
         * with @p renderArea spanning the whole @ref Framebuffer::size().
         */
        explicit RenderPassBeginInfo(VkRenderPass renderPass, Framebuffer& framebuffer);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit RenderPassBeginInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit RenderPassBeginInfo(const VkRenderPassBeginInfo& info);

        /** @brief Copying is not allowed */
        RenderPassBeginInfo(const RenderPassBeginInfo&) = delete;

        /** @brief Move constructor */
        RenderPassBeginInfo(RenderPassBeginInfo&& other) noexcept;

        ~RenderPassBeginInfo();

        /** @brief Copying is not allowed */
        RenderPassBeginInfo& operator=(const RenderPassBeginInfo&) = delete;

        /** @brief Move assignment */
        RenderPassBeginInfo& operator=(RenderPassBeginInfo&& other) noexcept;

        /**
         * @brief Clear a floating-point or normalized color attachment
         * @return Reference to self (for method chaining)
         *
         * @see @ref AttachmentLoadOperation::Clear,
         *      @ref CommandBuffer::clearColorImage(VkImage, ImageLayout, const Color4&)
         */
        RenderPassBeginInfo& clearColor(UnsignedInt attachment, const Color4& color);

        /**
         * @brief Clear a signed integral color attachment
         * @return Reference to self (for method chaining)
         *
         * @see @ref AttachmentLoadOperation::Clear,
         *      @ref CommandBuffer::clearColorImage(VkImage, ImageLayout, const Vector4i&)
         */
        RenderPassBeginInfo& clearColor(UnsignedInt attachment, const Vector4i& color);

        /**
         * @brief Clear an unsigned integral color attachment
         * @return Reference to self (for method chaining)
         *
         * @see @ref AttachmentLoadOperation::Clear,
         *      @ref CommandBuffer::clearColorImage(VkImage, ImageLayout, const Vector4ui&)
         */
        RenderPassBeginInfo& clearColor(UnsignedInt attachment, const Vector4ui& color);

        /**
         * @brief Clear a depth/stencil attachment
         * @return Reference to self (for method chaining)
         *
         * If the attachment is not a combined depth/stencil format, the unused
         * value is ignored.
         * @see @ref AttachmentLoadOperation::Clear,
         *      @ref CommandBuffer::clearDepthStencilImage(VkImage, ImageLayout, Float, UnsignedInt),
         *      @ref CommandBuffer::clearDepthImage(VkImage, ImageLayout, Float),
         *      @ref CommandBuffer::clearStencilImage(VkImage, ImageLayout, UnsignedInt)
         */
        RenderPassBeginInfo& clearDepthStencil(UnsignedInt attachment, Float depth, UnsignedInt stencil);

        /** @brief Underlying @type_vk{RenderPassBeginInfo} structure */
        VkRenderPassBeginInfo& operator*() { return _info; }
        /** @overload */
        const VkRenderPassBeginInfo& operator*() const { return _info; }
        /** @overload */
        VkRenderPassBeginInfo* operator->() { return &_info; }
        /** @overload */
        const VkRenderPassBeginInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkRenderPassBeginInfo*() const { return &_info; }

    private:
        RenderPassBeginInfo& clearInternal(UnsignedInt attachment, const VkClearValue& value);

        VkRenderPassBeginInfo _info;
        struct State;
        Containers::Pointer<State> _state;
};

/**
@brief Subpass contents
@m_since_latest

Wraps @type_vk{SubpassContents}.
@see @ref SubpassBeginInfo::SubpassBeginInfo(),
    @ref CommandBuffer::beginRenderPass(), @ref CommandBuffer::nextSubpass()
*/
enum class SubpassContents: Int {
    /**
     * Contents of the subpass will be recorded inline in the primary command
     * buffer. @ref CommandBufferLevel::Secondary command buffers must not
     * be executed within the subpass. This is the default used in the
     * @ref SubpassBeginInfo constructor and consequently
     * @ref CommandBuffer::beginRenderPass() / @ref CommandBuffer::nextSubpass().
     */
    Inline = VK_SUBPASS_CONTENTS_INLINE,

    /**
     * Subpass contents are recorded in @ref CommandBufferLevel::Secondary
     * command buffers that will be called from the primary command buffer.
     * @todoc reference @fn_vk{CmdExecuteCommands} when exposed and mention
     *      it's the only allowed command until nextsubpass / renderpass end
     */
    SecondaryCommandBuffers = VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
};

/**
@brief Subpass begin info
@m_since_latest

Wraps @type_vk{SubpassBeginInfo}. See @ref Vk-RenderPass-usage for more
information.
@see @ref CommandBuffer::beginRenderPass(), @ref CommandBuffer::nextSubpass()
*/
class MAGNUM_VK_EXPORT SubpassBeginInfo {
    public:
        /**
         * @brief Constructor
         * @param contents      How commands in the subpass will be provided
         *
         * The following @type_vk{SubpassBeginInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `contents`
         */
        explicit SubpassBeginInfo(SubpassContents contents = SubpassContents::Inline);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit SubpassBeginInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit SubpassBeginInfo(const VkSubpassBeginInfo& info);

        /** @brief Underlying @type_vk{SubpassBeginInfo} structure */
        VkSubpassBeginInfo& operator*() { return _info; }
        /** @overload */
        const VkSubpassBeginInfo& operator*() const { return _info; }
        /** @overload */
        VkSubpassBeginInfo* operator->() { return &_info; }
        /** @overload */
        const VkSubpassBeginInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkSubpassBeginInfo*() const { return &_info; }

    private:
        VkSubpassBeginInfo _info;
};

/**
@brief Subpass end info
@m_since_latest

Wraps @type_vk{SubpassEndInfo}. See @ref Vk-RenderPass-usage for more
information.
@see @ref CommandBuffer::endRenderPass(), @ref CommandBuffer::nextSubpass()
*/
class MAGNUM_VK_EXPORT SubpassEndInfo {
    public:
        /**
         * @brief Constructor
         *
         * The following @type_vk{SubpassEndInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    <em>(none)</em>
         */
        explicit SubpassEndInfo();

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit SubpassEndInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit SubpassEndInfo(const VkSubpassEndInfo& info);

        /** @brief Underlying @type_vk{SubpassEndInfo} structure */
        VkSubpassEndInfo& operator*() { return _info; }
        /** @overload */
        const VkSubpassEndInfo& operator*() const { return _info; }
        /** @overload */
        VkSubpassEndInfo* operator->() { return &_info; }
        /** @overload */
        const VkSubpassEndInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkSubpassEndInfo*() const { return &_info; }

    private:
        VkSubpassEndInfo _info;
};

}}

#endif
