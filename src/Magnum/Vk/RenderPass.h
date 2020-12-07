#ifndef Magnum_Vk_RenderPass_h
#define Magnum_Vk_RenderPass_h
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
 * @brief Class @ref Magnum::Vk::AttachmentDescription, @ref Magnum::Vk::AttachmentReference, @ref Magnum::Vk::SubpassDescription, @ref Magnum::Vk::SubpassDependency, @ref Magnum::Vk::RenderPassCreateInfo, @ref Magnum::Vk::RenderPass, enum @ref Magnum::Vk::AttachmentLoadOperation, @ref Magnum::Vk::AttachmentStoreOperation
 */

#include <utility>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Attachment load operation
@m_since_latest

Wraps a @type_vk_keyword{AttachmentLoadOp}, specifies how previous contents of
an attached image within the render area are treated at the beginning of a
subpass.
@see @ref AttachmentStoreOperation, @ref AttachmentDescription
@m_enum_values_as_keywords
*/
enum class AttachmentLoadOperation: Int {
    /**
     * Previous contents are preserved. This is the conservative default when
     * using the @ref AttachmentDescription::AttachmentDescription(VkFormat, Int, Flags)
     * constructor.
     *
     * This value is also guaranteed to be @cpp 0 @ce, which means you're
     * encouraged to simply use @cpp {} @ce in function calls and elsewhere.
     */
    Load = VK_ATTACHMENT_LOAD_OP_LOAD,

    /**
     * Previous contents are cleared to a value specified when a render pass
     * instance is begun.
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      Compared to @ref AttachmentLoadOperation::Load, if you don't need
     *      the previous contents, this can avoid a potentially expensive
     *      memory load on certain architectures.
     */
    Clear = VK_ATTACHMENT_LOAD_OP_CLEAR,

    /**
     * Previous contents don't need to be preserved.
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      Compared to @ref AttachmentLoadOperation::Load and
     *      @ref AttachmentLoadOperation::Clear, if you will be fully
     *      overwriting the contents anyway, this can avoid a potentially
     *      expensive memory load or clear operation.
     */
    DontCare = VK_ATTACHMENT_LOAD_OP_DONT_CARE
};

/**
@brief Attachment load operation
@m_since_latest

Wraps a @type_vk_keyword{AttachmentStoreOp}, specifies how contents of an
attached image generated during the render pass within the render area are
treated at the end of a subpass.
@see @ref AttachmentLoadOperation, @ref AttachmentDescription
@m_enum_values_as_keywords
*/
enum class AttachmentStoreOperation: Int {
    /**
     * Generated contents are written to memory. This is the conservative
     * default when using the @ref AttachmentDescription::AttachmentDescription(VkFormat, Int, Flags)
     * constructor.
     *
     * This value is also guaranteed to be @cpp 0 @ce, which means you're
     * encouraged to simply use @cpp {} @ce in function calls and elsewhere.
     */
    Store = VK_ATTACHMENT_STORE_OP_STORE,

    /**
     * Generated contentgs don't need to be preserved.
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      Compared to @ref AttachmentStoreOperation::Store, if the attachment
     *      was only used temporarily during a subpass, this can avoid a
     *      potentially expensive memory store operation on certain
     *      architectures.
     */
    DontCare = VK_ATTACHMENT_STORE_OP_DONT_CARE
};

/**
@brief Attachment description
@m_since_latest

Wraps a @type_vk_keyword{AttachmentDescription2}. This class is subsequently
passed to a @ref RenderPass, see its documentation for a high-level usage
information.

@section Vk-AttachmentDescription-compatibility Compatibility with VkAttachmentDescription

While the class operates on the @type_vk{AttachmentDescription2} structure
that's new in Vulkan 1.2 or is provided by the
@vk_extension{KHR,create_renderpass2} extenstion, conversion from and to
@type_vk{AttachmentDescription} is provided to some extent --- you can create
an @ref AttachmentDescription from it, call various methods on the instance and
then get a @type_vk{AttachmentDescription} back again using
@ref vkAttachmentDescription().

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{AttachmentDescription2} fields and then perform the conversion
instead of editing the resulting @type_vk{AttachmentDescription}.

Please note that the conversion to @type_vk{AttachmentDescription} will ignore
all fields that are present only in @type_vk{AttachmentDescription2} and its
substructures --- in particular, the whole `pNext` pointer chain is omitted.
When performing the conversion it's your responsibility to ensure nothing
significant was in the fields that were left out.
*/
class MAGNUM_VK_EXPORT AttachmentDescription {
    public:
        /**
         * @brief Attachment description flag
         *
         * Wraps @type_vk_keyword{AttachmentDescriptionFlagBits}.
         * @see @ref Flags, @ref AttachmentDescription()
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /** Aliases the same device memory as other attachments */
            MayAlias = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT
        };

        /**
         * @brief Attachment description flags
         *
         * Type-safe wrapper for @type_vk_keyword{AttachmentDescriptionFlags}.
         * @see @ref AttachmentDescription()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param format            Image format
         * @param loadOperation     How previous attachment contents are
         *      treated at the beginning of a subpass
         * @param storeOperation    How previous attachment contents are
         *      treated at the beginning of a subpass
         * @param initialLayout     Initial image layout. Can only be
         *      @ref ImageLayout::General, @ref ImageLayout::ShaderReadOnly,
         *      @ref ImageLayout::TransferSource,
         *      @ref ImageLayout::TransferDestination, and
         *      @ref ImageLayout::ColorAttachment in case of a color @p format
         *      or @ref ImageLayout::DepthStencilAttachment in case of a
         *      depth/stencil @p format.
         * @param finalLayout       Final image layout. Can only be
         *      @ref ImageLayout::General, @ref ImageLayout::ShaderReadOnly,
         *      @ref ImageLayout::TransferSource,
         *      @ref ImageLayout::TransferDestination, and
         *      @ref ImageLayout::ColorAttachment in case of a color @p format
         *      or @ref ImageLayout::DepthStencilAttachment in case of a
         *      depth/stencil @p format.
         * @param samples           Sample count
         * @param flags             Attachment description flags
         *
         * The following @type_vk{AttachmentDescription} fields are pre-filled
         * in addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `format`
         * -    `samples`
         * -    `loadOp` to @p loadOperation
         * -    `storeOp` to @p storeOperation
         * -    `initialLayout`
         * -    `finalLayout`
         *
         * See also @ref AttachmentDescription(VkFormat, std::pair<AttachmentLoadOperation, AttachmentLoadOperation>, std::pair<AttachmentStoreOperation, AttachmentStoreOperation>, ImageLayout, ImageLayout, Int, Flags)
         * for a constructing a combined depth/stencil attachment description.
         */
        /*implicit*/ AttachmentDescription(VkFormat format, AttachmentLoadOperation loadOperation, AttachmentStoreOperation storeOperation, ImageLayout initialLayout, ImageLayout finalLayout, Int samples = 1, Flags flags = {});

        /**
         * @brief Construct with implicit conservative layout
         *
         * Equivalent to calling @ref AttachmentDescription(VkFormat, AttachmentLoadOperation, AttachmentStoreOperation, ImageLayout, ImageLayout, Int, Flags)
         * with both @p initialLayout and @p finalLayout set to
         * @ref ImageLayout::General.
         */
        /*implicit*/ AttachmentDescription(VkFormat format, AttachmentLoadOperation loadOperation, AttachmentStoreOperation storeOperation, Int samples = 1, Flags flags = {});

        /**
         * @brief Construct for a combined depth/stencil attachment
         * @param format                        Image format
         * @param depthStencilLoadOperation     How previous depth and stencil
         *      attachment contents are treated at the beginning of a subpass
         * @param depthStencilStoreOperation    How generated depth and stencil
         *      attachment contents are treated at the end of a subpass
         * @param initialLayout                 Initial image layout. Can only
         *      be @ref ImageLayout::General, @ref ImageLayout::ShaderReadOnly,
         *      @ref ImageLayout::TransferSource,
         *      @ref ImageLayout::TransferDestination, and
         *      @ref ImageLayout::DepthStencilAttachment
         * @param finalLayout                   Final image layout. Can only be
         *      @ref ImageLayout::General, @ref ImageLayout::ShaderReadOnly,
         *      @ref ImageLayout::TransferSource,
         *      @ref ImageLayout::TransferDestination, and
         *      @ref ImageLayout::DepthStencilAttachment
         * @param samples                       Sample count
         * @param flags                         Attachment description flags
         *
         * The following @type_vk{AttachmentDescription} fields are pre-filled
         * in addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `format`
         * -    `samples`
         * -    `loadOp` and `stencilLoadOp` to @p loadOperation
         * -    `storeOp` and `stencilStoreOp` to @p storeOperation
         * -    `initialLayout`
         * -    `finalLayout`
         *
         * @todo Implement @vk_extension{KHR,separate_depth_stencil_layouts}
         *      and provide a pair of layouts as well
         */
        /*implicit*/ AttachmentDescription(VkFormat format, std::pair<AttachmentLoadOperation, AttachmentLoadOperation> depthStencilLoadOperation, std::pair<AttachmentStoreOperation, AttachmentStoreOperation> depthStencilStoreOperation, ImageLayout initialLayout, ImageLayout finalLayout, Int samples = 1, Flags flags = {});

        /**
         * @brief Construct for a combined depth/stencil attachment with implicit conservative layout
         *
         * Equivalent to calling @ref AttachmentDescription(VkFormat, std::pair<AttachmentLoadOperation, AttachmentLoadOperation>, std::pair<AttachmentStoreOperation, AttachmentStoreOperation>, ImageLayout, ImageLayout, Int, Flags)
         * with both @p initialLayout and @p finalLayout set to
         * @ref ImageLayout::General.
         */
        /*implicit*/ AttachmentDescription(VkFormat format, std::pair<AttachmentLoadOperation, AttachmentLoadOperation> depthStencilLoadOperation, std::pair<AttachmentStoreOperation, AttachmentStoreOperation> depthStencilStoreOperation, Int samples = 1, Flags flags = {});

        /**
         * @brief Construct with implicit conservative load/store operation and layout
         *
         * Equivalent to calling @ref AttachmentDescription(VkFormat, std::pair<AttachmentLoadOperation, AttachmentLoadOperation>, std::pair<AttachmentStoreOperation, AttachmentStoreOperation>, ImageLayout, ImageLayout, Int, Flags)
         * with @ref AttachmentLoadOperation::Load and
         * @ref AttachmentStoreOperation::Store and both @p initialLayout and
         * @p finalLayout set to @ref ImageLayout::General.
         */
        /*implicit*/ AttachmentDescription(VkFormat format, Int samples = 1, Flags flags = {}): AttachmentDescription{format, AttachmentLoadOperation{}, AttachmentStoreOperation{}, samples, flags} {}

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit AttachmentDescription(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit AttachmentDescription(const VkAttachmentDescription2& description);

        /**
         * @brief Construct from a @type_vk{AttachmentDescription}
         *
         * Compared to the above, fills the common subset of
         * @type_vk{AttachmentDescription2}, sets `sType` and zero-fills
         * `pNext`.
         * @see @ref vkAttachmentDescription()
         */
        explicit AttachmentDescription(const VkAttachmentDescription& description);

        /** @brief Underlying @type_vk{AttachmentDescription2} structure */
        VkAttachmentDescription2& operator*() { return _description; }
        /** @overload */
        const VkAttachmentDescription2& operator*() const { return _description; }
        /** @overload */
        VkAttachmentDescription2* operator->() { return &_description; }
        /** @overload */
        const VkAttachmentDescription2* operator->() const { return &_description; }
        /** @overload */
        operator const VkAttachmentDescription2*() const { return &_description; }

        /**
         * @overload
         *
         * The class is implicitly convertible to a reference in addition to
         * a pointer because the type is commonly used in arrays as well, which
         * would be annoying to do with a pointer conversion.
         */
        operator const VkAttachmentDescription2&() const { return _description; }

        /**
         * @brief Corresponding @type_vk{AttachmentDescription} structure
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support version 1.2 or the @vk_extension{KHR,create_renderpass2}
         * extension. See @ref Vk-AttachmentDescription-compatibility for more
         * information.
         * @see @ref AttachmentDescription(const VkAttachmentDescription&),
         *      @ref AttachmentReference::vkAttachmentReference(),
         *      @ref SubpassDescription::vkSubpassDescription(),
         *      @ref SubpassDependency::vkSubpassDependency(),
         *      @ref RenderPassCreateInfo::vkRenderPassCreateInfo()
         */
        VkAttachmentDescription vkAttachmentDescription() const;

    private:
        VkAttachmentDescription2 _description;
};

CORRADE_ENUMSET_OPERATORS(AttachmentDescription::Flags)

/**
@brief Attachment reference
@m_since_latest

Wraps a @type_vk_keyword{AttachmentReference2}. Used to reference attachments
inside a @ref SubpassDescription, which is subsequently passed to a
@ref RenderPass. See its documentation for a high-level overview.

@section Vk-AttachmentReference-compatibility Compatibility with VkAttachmentReference

While the class operates on the @type_vk{AttachmentReference2} structure that's
new in Vulkan 1.2 or is provided by the @vk_extension{KHR,create_renderpass2}
extenstion, conversion from and to @type_vk{AttachmentReference} is provided to
some extent --- you can create an @ref AttachmentReference from it, call
various methods on the instance and then get a @type_vk{AttachmentReference}
back again using @ref vkAttachmentReference().

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{AttachmentReference2} fields and then perform the conversion instead
of editing the resulting @type_vk{AttachmentReference}.

Please note that the conversion to @type_vk{AttachmentReference} will ignore
all fields that are present only in @type_vk{AttachmentReference2} --- in
particular, the whole `pNext` pointer chain is omitted. When performing the
conversion it's your responsibility to ensure nothing significant was in the
fields that were left out.
*/
class MAGNUM_VK_EXPORT AttachmentReference {
    public:
        /**
         * @brief Constructor
         * @param attachment    Attachment index from the list passed to
         *      @ref RenderPassCreateInfo::setAttachments()
         * @param layout        Image layout. Should correspond to what's
         *      passed to @p initialLayout and @p finalLayout in
         *      @ref AttachmentDescription constructor
         *
         * The following @type_vk{AttachmentReference2} fields are pre-filled
         * in addition to `sType`, everything else is zero-filled:
         *
         * -    `attachment`
         * -    `layout`
         */
        /*implicit*/ AttachmentReference(UnsignedInt attachment, ImageLayout layout =
            #ifdef DOXYGEN_GENERATING_OUTPUT
            /* To avoid Image.h dependency */
            ImageLayout::General
            #else
            ImageLayout(VK_IMAGE_LAYOUT_GENERAL)
            #endif
        );

        /**
         * @brief Construct with no attachment
         *
         * The following @type_vk{AttachmentReference2} fields are pre-filled
         * in addition to `sType`, everything else is zero-filled:
         *
         * -    `attachment` to @def_vk{ATTACHMENT_UNUSED}
         * -    `layout` to @ref ImageLayout::Undefined
         */
        /*implicit*/ AttachmentReference();

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit AttachmentReference(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit AttachmentReference(const VkAttachmentReference2& reference);

        /**
         * @brief Construct from a @type_vk{AttachmentReference}
         *
         * Compared to the above, fills the common subset of
         * @type_vk{AttachmentReference2}, sets `sType` and zero-fills `pNext`
         * and `aspectMask`.
         * @see @ref vkAttachmentReference()
         */
        explicit AttachmentReference(const VkAttachmentReference& reference);

        /** @brief Underlying @type_vk{AttachmentReference2} structure */
        VkAttachmentReference2& operator*() { return _reference; }
        /** @overload */
        const VkAttachmentReference2& operator*() const { return _reference; }
        /** @overload */
        VkAttachmentReference2* operator->() { return &_reference; }
        /** @overload */
        const VkAttachmentReference2* operator->() const { return &_reference; }
        /** @overload */
        operator const VkAttachmentReference2*() const { return &_reference; }

        /**
         * @overload
         *
         * The class is implicitly convertible to a reference in addition to
         * a pointer because the type is commonly used in arrays as well, which
         * would be annoying to do with a pointer conversion.
         */
        operator const VkAttachmentReference2&() const { return _reference; }

        /**
         * @brief Corresponding @type_vk{AttachmentReference} structure
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support version 1.2 or the @vk_extension{KHR,create_renderpass2}
         * extension. See @ref Vk-AttachmentReference-compatibility for more
         * information.
         * @see @ref AttachmentReference(const VkAttachmentReference&),
         *      @ref AttachmentDescription::vkAttachmentDescription(),
         *      @ref SubpassDescription::vkSubpassDescription(),
         *      @ref SubpassDependency::vkSubpassDependency(),
         *      @ref RenderPassCreateInfo::vkRenderPassCreateInfo()
         */
        VkAttachmentReference vkAttachmentReference() const;

    private:
        VkAttachmentReference2 _reference;
};

/**
@brief Subpass description
@m_since_latest

Wraps a @type_vk_keyword{SubpassDescription2}. This class is subsequently
passed to a @ref RenderPass, see its documentation for a high-level usage
information.

@section Vk-SubpassDescription-compatibility Compatibility with VkSubpassDescription

While the class operates on the @type_vk{SubpassDescription2} structure that's
new in Vulkan 1.2 or is provided by the @vk_extension{KHR,create_renderpass2}
extenstion, conversion from and to @type_vk{SubpassDescription} is provided to
some extent --- you can create a @ref SubpassDescription from it, call various
methods on the instance and then get a @type_vk{SubpassDescription} back again
using @ref vkSubpassDescription(). Note that, because of the nested data
references, some internal pointers may still point to the originating instance,
so be sure to keep it in scope for as long as needed.

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{SubpassDescription2} fields and then perform the conversion instead of
editing the resulting @type_vk{SubpassDescription}.

Please note that the conversion to @type_vk{SubpassDescription} will ignore all
fields that are present only in @type_vk{SubpassDescription2} and its
substructures --- in particular, the whole `pNext` pointer chain is omitted.
When performing the conversion it's your responsibility to ensure nothing
significant was in the fields that were left out.
*/
class MAGNUM_VK_EXPORT SubpassDescription {
    public:
        /**
         * @brief Subpass description flag
         *
         * Wraps @type_vk_keyword{SubpassDescriptionFlagBits}.
         * @see @ref Flags, @ref SubpassDescription()
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {};

        /**
         * @brief Subpass description flags
         *
         * Type-safe wrapper for @type_vk_keyword{SubpassDescriptionFlags}.
         * @see @ref SubpassDescription()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         *
         * The following @type_vk{SubpassDescription2} fields are pre-filled
         * in addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `pipelineBindPoint` to @val_vk{PIPELINE_BIND_POINT_GRAPHICS,PipelineBindPoint}
         *
         * Use @ref setInputAttachments(), @ref setColorAttachments(),
         * @ref setDepthStencilAttachment() and @ref setPreserveAttachments()
         * to set attachments. Note that a subpass without any attachment is
         * valid as well.
         */
        explicit SubpassDescription(Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit SubpassDescription(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit SubpassDescription(const VkSubpassDescription2& description);

        /**
         * @brief Construct from a @type_vk{SubpassDescription}
         *
         * Compared to the above, fills the common subset of
         * @type_vk{SubpassDescription2}, sets `sType` and zero-fills `pNext`
         * and `viewMask`.
         * @see @ref vkSubpassDescription()
         */
        explicit SubpassDescription(const VkSubpassDescription& description);

        /** @brief Copying is not allowed */
        SubpassDescription(const SubpassDescription&) = delete;

        /** @brief Move constructor */
        SubpassDescription(SubpassDescription&&) noexcept;

        ~SubpassDescription();

        /** @brief Copying is not allowed */
        SubpassDescription& operator=(const SubpassDescription&) = delete;

        /** @brief Move assignment */
        SubpassDescription& operator=(SubpassDescription&&) noexcept;

        /**
         * @brief Set input attachments
         * @return Reference to self (for method chaining)
         */
        SubpassDescription& setInputAttachments(Containers::ArrayView<const AttachmentReference> attachments) &;
        /** @overload */
        SubpassDescription&& setInputAttachments(Containers::ArrayView<const AttachmentReference> attachments) &&;
        /** @overload */
        SubpassDescription& setInputAttachments(std::initializer_list<AttachmentReference> attachments) &;
        /** @overload */
        SubpassDescription&& setInputAttachments(std::initializer_list<AttachmentReference> attachments) &&;

        /**
         * @brief Set color attachments
         * @return Reference to self (for method chaining)
         *
         * The @p resolveAttachments list is expected to be either empty or
         * have the same size as @p attachments. If non-empty, each item has to
         * have the same format as the corresponding item in @p attachments.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        SubpassDescription& setColorAttachments(Containers::ArrayView<const AttachmentReference> attachments, Containers::ArrayView<const AttachmentReference> resolveAttachments = {}) &;
        /** @overload */
        SubpassDescription&& setColorAttachments(Containers::ArrayView<const AttachmentReference> attachments, Containers::ArrayView<const AttachmentReference> resolveAttachments = {}) &&;
        #else
        /* So we don't need to include ArrayView */
        SubpassDescription& setColorAttachments(Containers::ArrayView<const AttachmentReference> attachments, Containers::ArrayView<const AttachmentReference> resolveAttachments) &;
        SubpassDescription&& setColorAttachments(Containers::ArrayView<const AttachmentReference> attachments, Containers::ArrayView<const AttachmentReference> resolveAttachments) &&;
        SubpassDescription& setColorAttachments(Containers::ArrayView<const AttachmentReference> attachments) &;
        SubpassDescription&& setColorAttachments(Containers::ArrayView<const AttachmentReference> attachments) &&;
        #endif
        /** @overload */
        SubpassDescription& setColorAttachments(std::initializer_list<AttachmentReference> attachments, std::initializer_list<AttachmentReference> resolveAttachments = {}) &;
        /** @overload */
        SubpassDescription&& setColorAttachments(std::initializer_list<AttachmentReference> attachments, std::initializer_list<AttachmentReference> resolveAttachments = {}) &&;

        /**
         * @brief Set depth/stencil attachment
         * @return Reference to self (for method chaining)
         *
         * Calling this function with a default-constructed
         * @ref AttachmentReference is equivalent to not calling it at all, and
         * both mean there's no depth/stencil attachment.
         */
        SubpassDescription& setDepthStencilAttachment(AttachmentReference attachment) &;
        /** @overload */
        SubpassDescription&& setDepthStencilAttachment(AttachmentReference attachment) &&;

        /**
         * @brief Set preserve attachments
         * @return Reference to self (for method chaining)
         *
         * The @p attachment values are indices into the list passed to
         *      @ref RenderPassCreateInfo::setAttachments()
         */
        SubpassDescription& setPreserveAttachments(Containers::ArrayView<const UnsignedInt> attachments) &;
        /** @overload */
        SubpassDescription&& setPreserveAttachments(Containers::ArrayView<const UnsignedInt> attachments) &&;
        /** @overload */
        SubpassDescription& setPreserveAttachments(Containers::Array<UnsignedInt>&& attachments) &;
        /** @overload */
        SubpassDescription&& setPreserveAttachments(Containers::Array<UnsignedInt>&& attachments) &&;
        /** @overload */
        SubpassDescription& setPreserveAttachments(std::initializer_list<UnsignedInt> attachments) &;
        /** @overload */
        SubpassDescription&& setPreserveAttachments(std::initializer_list<UnsignedInt> attachments) &&;

        /** @brief Underlying @type_vk{SubpassDescription2} structure */
        VkSubpassDescription2& operator*() { return _description; }
        /** @overload */
        const VkSubpassDescription2& operator*() const { return _description; }
        /** @overload */
        VkSubpassDescription2* operator->() { return &_description; }
        /** @overload */
        const VkSubpassDescription2* operator->() const { return &_description; }
        /** @overload */
        operator const VkSubpassDescription2*() const { return &_description; }

        /**
         * @overload
         *
         * The class is implicitly convertible to a reference in addition to
         * a pointer because the type is commonly used in arrays as well, which
         * would be annoying to do with a pointer conversion.
         */
        operator const VkSubpassDescription2&() const { return _description; }

        /**
         * @brief Corresponding @type_vk{SubpassDescription} structure
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support version 1.2 or the @vk_extension{KHR,create_renderpass2}
         * extension. Because the type references structures not present in
         * @type_vk{SubpassDescription2}, it's returned wrapped it in a
         * single-item array with the extra data appended at the end of the
         * allocation. Note that, however, some internal pointers such as
         * `pPreserveAttachments` may still point to the originating
         * @ref SubpassDescription instance, the returned allocation is not
         * completely standalone. See @ref Vk-SubpassDescription-compatibility
         * for more information.
         * @see @ref SubpassDescription(const VkSubpassDescription&),
         *      @ref AttachmentDescription::vkAttachmentDescription(),
         *      @ref AttachmentReference::vkAttachmentReference(),
         *      @ref SubpassDependency::vkSubpassDependency(),
         *      @ref RenderPassCreateInfo::vkRenderPassCreateInfo()
         */
        Containers::Array<VkSubpassDescription> vkSubpassDescription() const;

    private:
        friend class RenderPassCreateInfo;

        template<class T> void setInputAttachmentsInternal(Containers::ArrayView<const T> attachments);
        template<class T> void setColorAttachmentsInternal(Containers::ArrayView<const T> attachments, Containers::ArrayView<const T> resolveAttachments);
        VkSubpassDescription2 _description;
        struct State;
        Containers::Pointer<State> _state;
};

CORRADE_ENUMSET_OPERATORS(SubpassDescription::Flags)

/**
@brief Subpass dependency
@m_since_latest

Wraps a @type_vk_keyword{SubpassDependency2}. This class is subsequently
passed to a @ref RenderPass, see its documentation for a high-level usage
information.

@section Vk-SubpassDependency-compatibility Compatibility with VkSubpassDependency

While the class operates on the @type_vk{SubpassDependency2} structure that's
new in Vulkan 1.2 or is provided by the @vk_extension{KHR,create_renderpass2}
extenstion, conversion from and to @type_vk{SubpassDependency} is provided to
some extent --- you can create a @ref SubpassDependency from it, call various
methods on the instance and then get a @type_vk{SubpassDependency} back again
using @ref vkSubpassDependency().

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{SubpassDependency2} fields and then perform the conversion instead
of editing the resulting @type_vk{SubpassDependency}.

Please note that the conversion to @type_vk{SubpassDependency} will ignore
all fields that are present only in @type_vk{SubpassDependency2} --- in
particular, the whole `pNext` pointer chain is omitted. When performing the
conversion it's your responsibility to ensure nothing significant was in the
fields that were left out.
*/
class MAGNUM_VK_EXPORT SubpassDependency {
    public:
        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit SubpassDependency(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit SubpassDependency(const VkSubpassDependency2& dependency);

        /**
         * @brief Construct from a @type_vk{SubpassDependency}
         *
         * Compared to the above, fills the common subset of
         * @type_vk{SubpassDescription2}, sets `sType` and zero-fills `pNext`
         * and `viewOffset`.
         * @see @ref vkSubpassDependency()
         */
        explicit SubpassDependency(const VkSubpassDependency& dependency);

        /** @brief Underlying @type_vk{SubpassDependency2} structure */
        VkSubpassDependency2& operator*() { return _dependency; }
        /** @overload */
        const VkSubpassDependency2& operator*() const { return _dependency; }
        /** @overload */
        VkSubpassDependency2* operator->() { return &_dependency; }
        /** @overload */
        const VkSubpassDependency2* operator->() const { return &_dependency; }
        /** @overload */
        operator const VkSubpassDependency2*() const { return &_dependency; }

        /**
         * @overload
         *
         * The class is implicitly convertible to a reference in addition to
         * a pointer because the type is commonly used in arrays as well, which
         * would be annoying to do with a pointer conversion.
         */
        operator const VkSubpassDependency2&() const { return _dependency; }

        /**
         * @brief Corresponding @type_vk{SubpassDependency} structure
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support version 1.2 or the @vk_extension{KHR,create_renderpass2}
         * extension. See @ref Vk-SubpassDependency-compatibility for more
         * information.
         * @see @ref SubpassDependency(const VkSubpassDependency&),
         *      @ref AttachmentDescription::vkAttachmentDescription(),
         *      @ref AttachmentReference::vkAttachmentReference(),
         *      @ref SubpassDescription::vkSubpassDescription(),
         *      @ref RenderPassCreateInfo::vkRenderPassCreateInfo()
         */
        VkSubpassDependency vkSubpassDependency() const;

    private:
        VkSubpassDependency2 _dependency;
};

/**
@brief Render pass creation info
@m_since_latest

Wraps a @type_vk_keyword{RenderPassCreateInfo2} /
@type_vk_keyword{RenderPassCreateInfo}. See @ref RenderPass for usage
information.

@section Vk-RenderPassCreateInfo-compatibility Compatibility with VkRenderPassCreateInfo

While the class operates on the @type_vk{RenderPassCreateInfo2} structure
that's new in Vulkan 1.2 or is provided by the
@vk_extension{KHR,create_renderpass2} extenstion, conversion from and to
@type_vk{RenderPassCreateInfo} is provided to some extent ---- you can create a
@ref RenderPassCreateInfo from it, call various methods on the instance and
then get a @type_vk{RenderPassCreateInfo} back again using
@ref vkRenderPassCreateInfo(). Note that, because of the nested data
references, some internal pointers may still point to the originating instance,
so be sure to keep it in scope for as long as needed.

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{RenderPassCreateInfo2} fields and then perform the conversion instead
of editing the resulting @type_vk{RenderPassCreateInfo}.

Please note that the conversion to @type_vk{RenderPassCreateInfo} will ignore
all fields that are present only in @type_vk{RenderPassCreateInfo2} and its
substructures --- in particular, `pCorrelatedViewMasks` are omitted. When
performing the conversion it's your responsibility to ensure nothing
significant was in the fields that were left out.
*/
class MAGNUM_VK_EXPORT RenderPassCreateInfo {
    public:
        /**
         * @brief Render pass creation flag
         *
         * Wraps @type_vk_keyword{RenderPassCreateFlagBits}.
         * @see @ref Flags, @ref RenderPassCreateInfo()
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {};

        /**
         * @brief Render pass creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{RenderPassCreateFlags}.
         * @see @ref RenderPassCreateInfo()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags         Render pass creation flags
         *
         * The following @type_vk{RenderPassCreateInfo2} fields are pre-filled
         * in addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         *
         * You need to call @ref addSubpass() at least once for a valid setup.
         */
        explicit RenderPassCreateInfo(Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit RenderPassCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit RenderPassCreateInfo(const VkRenderPassCreateInfo2& info);

        /**
         * @brief Construct from a @type_vk{RenderPassCreateInfo}
         *
         * Compared to the above, fills the common subset of
         * @type_vk{RenderPassCreateInfo2}, sets `sType`, zero-fills
         * `correlatedViewMaskCount` and `pCorrelatedViewMasks` and then calls
         * @ref setAttachments(), @ref addSubpass() and @ref setDependencies()
         * with instances created using
         * @ref AttachmentDescription::AttachmentDescription(const VkAttachmentDescription&),
         * @ref SubpassDescription::SubpassDescription(const VkSubpassDescription&)
         * and @ref SubpassDependency::SubpassDependency(const VkSubpassDependency&).
         *
         * @attention The `pNext` member is currently taken as-is even though
         *      @type_vk{RenderPassCreateInfo2} accepts only a subset of the
         *      structure chain allowed by @type_vk{RenderPassCreateInfo}. This
         *      may change in the future, however now you have to take care to
         *      not list disallowed structures in the chain.
         *
         * @todo handle @type_vk{RenderPassInputAttachmentAspectCreateInfo}
         *      (@vk_extension{KHR,maintenance2}) and others here
         * @see @ref vkRenderPassCreateInfo()
         */
        explicit RenderPassCreateInfo(const VkRenderPassCreateInfo& info);

        /** @brief Copying is not allowed */
        RenderPassCreateInfo(const RenderPassCreateInfo&) = delete;

        /** @brief Move constructor */
        RenderPassCreateInfo(RenderPassCreateInfo&& other) noexcept;

        ~RenderPassCreateInfo();

        /** @brief Copying is not allowed */
        RenderPassCreateInfo& operator=(const RenderPassCreateInfo&) = delete;

        /** @brief Move assignment */
        RenderPassCreateInfo& operator=(RenderPassCreateInfo&& other) noexcept;

        /**
         * @brief Set attachments
         * @return Reference to self (for method chaining)
         *
         * Subsequent calls to this function will *replace* the previous set,
         * not append to it.
         */
        RenderPassCreateInfo& setAttachments(Containers::ArrayView<const AttachmentDescription> attachments);
        /** @overload */
        RenderPassCreateInfo& setAttachments(std::initializer_list<AttachmentDescription> attachments);

        /**
         * @brief Add a subpass
         * @return Reference to self (for method chaining)
         *
         * At least one subpass has to be added.
         *
         * @m_class{m-note m-dim}
         *
         * @par
         *      Unlike @ref setAttachments() and @ref setDependencies(), due
         *      to nested allocations inside @ref SubpassDescription, it's more
         *      efficient to *move* the instances one by one than having to
         *      deep-copy a list.
         */
        RenderPassCreateInfo& addSubpass(SubpassDescription&& subpass);

        /**
         * @brief Set subpass dependencies
         * @return Reference to self (for method chaining)
         *
         * ubsequent calls to this function will *replace* the previous set,
         * not append to it.
         */
        RenderPassCreateInfo& setDependencies(Containers::ArrayView<const SubpassDependency> dependencies);
        /** @overload */
        RenderPassCreateInfo& setDependencies(std::initializer_list<SubpassDependency> dependencies);

        /** @brief Underlying @type_vk{ShaderModuleCreateInfo} structure */
        VkRenderPassCreateInfo2& operator*() { return _info; }
        /** @overload */
        const VkRenderPassCreateInfo2& operator*() const { return _info; }
        /** @overload */
        VkRenderPassCreateInfo2* operator->() { return &_info; }
        /** @overload */
        const VkRenderPassCreateInfo2* operator->() const { return &_info; }
        /** @overload */
        operator const VkRenderPassCreateInfo2*() const { return &_info; }

        /**
         * @brief Corresponding @type_vk{RenderPassCreateInfo} structure
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support version 1.2 or the @vk_extension{KHR,create_renderpass2}
         * extension. Because the type references structures not present in
         * @type_vk{RenderPassCreateInfo2}, it's returned wrapped it in a
         * single-item array with the extra data appended at the end of the
         * allocation. Note that, however, some internal pointers such as
         * `pNext` may still point to the originating @ref RenderPassCreateInfo
         * instance, the returned allocation is not completely standalone. See
         * @ref Vk-RenderPassCreateInfo-compatibility for more information.
         *
         * @attention The `pNext` member is currently taken as-is without
         *      converting the @type_vk{RenderPassCreateInfo2}-only fields to
         *      extra structures in the `pNext` chain for
         *      @type_vk{RenderPassCreateInfo}. This may change in the future,
         *      however now you have to take care to do needed modifications
         *      yourself afterwards.
         *
         * @see @ref RenderPassCreateInfo(const VkRenderPassCreateInfo&),
         *      @ref AttachmentDescription::vkAttachmentDescription(),
         *      @ref AttachmentReference::vkAttachmentReference(),
         *      @ref SubpassDescription::vkSubpassDescription(),
         *      @ref SubpassDependency::vkSubpassDependency()
         */
        Containers::Array<VkRenderPassCreateInfo> vkRenderPassCreateInfo() const;

    private:
        template<class T> void setAttachmentsInternal(Containers::ArrayView<const T> attachments);
        template<class T> void setDependenciesInternal(Containers::ArrayView<const T> dependencies);

        VkRenderPassCreateInfo2 _info;
        struct State;
        Containers::Pointer<State> _state;
};

CORRADE_ENUMSET_OPERATORS(RenderPassCreateInfo::Flags)

namespace Implementation { struct DeviceState; }

/**
@brief Render pass
@m_since_latest

Wraps a @type_vk_keyword{RenderPass}, represents a collection of attachment
descriptions, subpasses and their dependencies. The render pass description is
independent of any specific image views used for attachments, these two are
connected together in a framebuffer.

@section Vk-RenderPass-usage Basic usage

A render pass is a set of attachments, described by @ref AttachmentDescription
instances, subpasses operating on those attachments, described by a
@ref SubpassDescription using @ref AttachmentReference instances, and subpass
dependencies, described by @ref SubpassDependency.

A render pass has to have at least one subpass. It's common to have just one
subpass but while the subpass isn't required to operate on any attachments,
such case is rather rare. Following is a simple setup for one subpass operating
on a color and a combined depth/stencil attachment. The main parameter an
@ref AttachmentDescription needs is attachment format; the numbers passed to
@ref SubpassDescription::setColorAttachments() and
@ref SubpassDescription::setDepthStencilAttachment() are indices into the
@ref RenderPassCreateInfo::setAttachments() array, and it's actually
@ref AttachmentReference instances:

@snippet MagnumVk.cpp RenderPass-usage

The above again does a conservative estimate that you'd want to preserve the
attachment contents between render passes. Usually you'd want to clear the
framebuffer first instead of reusing its previous contents, which is done by
passing appropriate @ref AttachmentLoadOperation /
@ref AttachmentStoreOperation to the @ref AttachmentDescription constructor.
@ref AttachmentLoadOperation::Load and @ref AttachmentStoreOperation::Store are
conveniently the zero values, which means you can use @cpp {} @ce instead of
typing them out in full:

@snippet MagnumVk.cpp RenderPass-usage-load-store

Vulkan makes heavy use of image layouts for optimal memory access
and in all the cases above, @ref ImageLayout::General is used as an implicit
conservative layout. It's guaranteed to work for all device access, but it
might not always be optimal. A complete description of image layouts and their
use is out of scope of this reference, but for example, if the attached images
would be always only used as a render target, the above setup could be made
more optimal by explicitly specifying both a concrete initial and final layout
in the @ref AttachmentDescription constructors and in
each @link AttachmentReference @endlink:

@snippet MagnumVk.cpp RenderPass-usage-layout
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

}}

#endif
