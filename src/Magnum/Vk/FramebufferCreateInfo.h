#ifndef Magnum_Vk_FramebufferCreateInfo_h
#define Magnum_Vk_FramebufferCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::FramebufferCreateInfo
 * @m_since_latest
 */

#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Framebuffer creation info
@m_since_latest

Wraps a @type_vk_keyword{FramebufferCreateInfo}. See
@ref Vk-Framebuffer-creation "Framebuffer creation" for usage information.
*/
class MAGNUM_VK_EXPORT FramebufferCreateInfo {
    public:
        /**
         * @brief Framebuffer creation flag
         *
         * Wraps @type_vk_keyword{FramebufferCreateFlagBits}.
         * @see @ref Flags, @ref FramebufferCreateInfo(VkRenderPass, Containers::ArrayView<const Containers::Reference<ImageView>>, const Vector3i&, Flags)
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /** @todo VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT, 1.2, plus a
                dedicated attachment-less constructor enabling that implicitly */
        };

        /**
         * @brief Framebuffer creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{FramebufferCreateFlags}.
         * @see @ref FramebufferCreateInfo(VkRenderPass, Containers::ArrayView<const Containers::Reference<ImageView>>, const Vector3i&, Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Construct a multi-layer framebuffer
         * @param renderPass    A @ref RenderPass or a raw Vulkan render pass
         *      handle the framebuffer is compatible with
         * @param attachments   Image views corresponding to all attachments
         *      listed in @ref RenderPassCreateInfo::setAttachments(). All
         *      images the views are created from are expected to have been
         *      created with @ref ImageUsage::ColorAttachment /
         *      @ref ImageUsage::DepthStencilAttachment /
         *      @ref ImageUsage::InputAttachment based on what they are
         *      attached to.
         * @param size          Width, height and layer count of the
         *      framebuffer. Available through @ref Framebuffer::size()
         *      afterwards.
         * @param flags         Framebuffer creation flags
         *
         * The following @type_vk{FramebufferCreateInfo} fields are pre-filled
         * in addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `renderPass`
         * -    `attachmentCount` and `pAttachments` to a copy of
         *      @p attachments
         * -    `width`, `height` and `layers` to @p size
         */
        explicit FramebufferCreateInfo(VkRenderPass renderPass, Containers::ArrayView<const Containers::Reference<ImageView>> attachments, const Vector3i& size, Flags flags = {});

        /** @overload */
        explicit FramebufferCreateInfo(VkRenderPass renderPass, std::initializer_list<Containers::Reference<ImageView>> attachments, const Vector3i& size, Flags flags = {});

        /**
         * @brief Construct a single-layer framebuffer
         *
         * Equivalent to calling @ref FramebufferCreateInfo(VkRenderPass, Containers::ArrayView<const Containers::Reference<ImageView>>, const Vector3i&, Flags)
         * with the last dimension set to @cpp 1 @ce.
         */
        explicit FramebufferCreateInfo(VkRenderPass renderPass, Containers::ArrayView<const Containers::Reference<ImageView>> attachments, const Vector2i& size, Flags flags = {});

        /** @overload */
        explicit FramebufferCreateInfo(VkRenderPass renderPass, std::initializer_list<Containers::Reference<ImageView>> attachments, const Vector2i& size, Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit FramebufferCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit FramebufferCreateInfo(const VkFramebufferCreateInfo& info);

        /** @brief Copying is not allowed */
        FramebufferCreateInfo(const FramebufferCreateInfo&) = delete;

        /** @brief Move constructor */
        FramebufferCreateInfo(FramebufferCreateInfo&& other) noexcept;

        ~FramebufferCreateInfo();

        /** @brief Copying is not allowed */
        FramebufferCreateInfo& operator=(const FramebufferCreateInfo&) = delete;

        /** @brief Move assignment */
        FramebufferCreateInfo& operator=(FramebufferCreateInfo&& other) noexcept;

        /** @brief Underlying @type_vk{FramebufferCreateInfo} structure */
        VkFramebufferCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkFramebufferCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkFramebufferCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkFramebufferCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkFramebufferCreateInfo*() const { return &_info; }

    private:
        VkFramebufferCreateInfo _info;
        struct State;
        Containers::Pointer<State> _state;
};

CORRADE_ENUMSET_OPERATORS(FramebufferCreateInfo::Flags)

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/Framebuffer.h"

#endif
