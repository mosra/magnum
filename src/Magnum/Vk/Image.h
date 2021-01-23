#ifndef Magnum_Vk_Image_h
#define Magnum_Vk_Image_h
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
 * @brief Class @ref Magnum::Vk::Image, enum @ref Magnum::Vk::ImageLayout
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Memory.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

namespace Implementation { struct DeviceState; }

/**
@brief Image layout
@m_since_latest

@see @ref ImageCreateInfo
@m_enum_values_as_keywords
*/
enum class ImageLayout: Int {
    /**
     * Undefined. Can be used as the initial layout in @ref ImageCreateInfo
     * structures (and there it's the default) and as the initial layout in
     * render pass @ref AttachmentDescription (in which case it tells the
     * driver that we don't care about the previous contents). Images in this
     * layout are not accessible by the device, the image has to be
     * transitioned to a defined layout such as @ref ImageLayout::General
     * first; contents of the memory are not guaranteed to be preserved during
     * the transition.
     * @see @ref ImageLayout::Preinitialized
     */
    Undefined = VK_IMAGE_LAYOUT_UNDEFINED,

    /**
     * Preinitialized. Can only be used as the initial layout in
     * @ref ImageCreateInfo structures. Compared to
     * @ref ImageLayout::Undefined, contents of the memory are guaranteed to be
     * preserved during a transition to a defined layout and thus this layout
     * is intended for populating image contents by the host.
     *
     * Usable only for images created with
     * @val_vk{IMAGE_TILING_LINEAR,ImageTiling}, usually with just one sample
     * and possibly other restrictions.
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      In order to be populated from the host, such images need to be
     *      allocated from @ref MemoryFlag::HostVisible memory, which on
     *      discrete GPUs is not fast for device access and there's thus
     *      recommended to go through a staging buffer instead. For integrated
     *      GPUs however, going directly through a linear preinitialized image
     *      *might* be better to avoid a memory usage spike and a potentially
     *      expensive copy.
     */
    Preinitialized = VK_IMAGE_LAYOUT_PREINITIALIZED,

    /**
     * General layout, supports all types of device access.
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      While this layout will always work, it's recommended to pick a
     *      stricter layout where appropriate, as it may result in better
     *      performance.
     */
    General = VK_IMAGE_LAYOUT_GENERAL,

    /* The _OPTIMAL suffixes are dropped because it doesn't seem that there
       would be any _UNOPTIMAL or whatever variants anytime soon, so this is
       redundant. If that time comes, we can always deprecate and rename. */

    /**
     * Layout optimal for a color or resolve attachment, not guaranteed to be
     * usable for anything else.
     *
     * Only valid for images created with @ref ImageUsage::ColorAttachment.
     */
    ColorAttachment = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

    /**
     * Layout optimal for a read/write depth/stencil attachment, not guaranteed
     * to be usable for anything else.
     *
     * Only valid for images created with
     * @ref ImageUsage::DepthStencilAttachment.
     */
    DepthStencilAttachment = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,

    /**
     * Layout optimal for read-only access in a shader sampler, combined
     * image/sampler or input attachment; not guaranteed to be usable for
     * anything else.
     *
     * Only valid for images created with @ref ImageUsage::Sampled or
     * @ref ImageUsage::InputAttachment.
     */
    ShaderReadOnly = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

    /**
     * Layout optimal for transfer sources; not guaranteed to be usable for
     * anything else.
     *
     * Only valid for images created with @ref ImageUsage::TransferSource.
     */
    TransferSource = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,

    /**
     * Layout optimal for transfer destination; not guaranteed to be usable for
     * anything else.
     *
     * Only valid for images created with @ref ImageUsage::TransferDestination.
     */
    TransferDestination = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,

    /** @todo remaining ones from @vk_extension{KHR,maintenance2} (1.1),
        @vk_extension{KHR,separate_depth_stencil_layouts} (1.2) */
};

/**
@brief Image aspect
@m_since_latest

Wraps @type_vk_keyword{ImageAspectFlagBits}.
@see @ref ImageAspects, @ref imageAspectsFor(),
    @ref ImageViewCreateInfo::ImageViewCreateInfo()
@m_enum_values_as_keywords
*/
enum class ImageAspect: UnsignedInt {
    Color = VK_IMAGE_ASPECT_COLOR_BIT,      /**< Color */
    Depth = VK_IMAGE_ASPECT_DEPTH_BIT,      /**< Depth */
    Stencil = VK_IMAGE_ASPECT_STENCIL_BIT   /**< Stencil */

    /** @todo metadata (sparse?), YCbCr properties */
};

/**
@debugoperatorenum{ImageAspect}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& out, ImageAspect value);

/**
@brief Image aspects
@m_since_latest

Type-safe wrapper for @type_vk_keyword{ImageAspectFlags}.
@see @ref imageAspectsFor(), @ref ImageViewCreateInfo::ImageViewCreateInfo()
*/
typedef Containers::EnumSet<ImageAspect> ImageAspects;

CORRADE_ENUMSET_OPERATORS(ImageAspects)

/**
@debugoperatorenum{ImageAspects}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& out, ImageAspects value);

/**
@brief Image aspects corresponding to given pixel format
@m_since_latest

Returns @ref ImageAspect::Depth for a depth format, @ref ImageAspect::Stencil
for a stencil format, a combination of both for a combined depth/stencil format
and @ref ImageAspect::Color otherwise. Expects that the format is not
undefined.
*/
MAGNUM_VK_EXPORT ImageAspects imageAspectsFor(PixelFormat format);

/**
@overload
@m_since_latest
*/
MAGNUM_VK_EXPORT ImageAspects imageAspectsFor(Magnum::PixelFormat format);

/**
@brief Image
@m_since_latest

Wraps a @type_vk_keyword{Image} and its memory.

@section Vk-Image-creation Image creation

Pass one of the @ref ImageCreateInfo subclasses depending on desired image type
with desired usage, format, size and other propoerties to the @ref Image
constructor together with specifying @ref MemoryFlags for memory allocation.

@snippet MagnumVk.cpp Image-creation

@attention At this point, a dedicated allocation is used, subsequently
    accessible through @ref dedicatedMemory(). This behavior may change in the
    future.

With an @ref Image ready, you may want to proceed to @ref ImageView creation.

@subsection Vk-Image-creation-custom-allocation Custom memory allocation

Using @ref Image(Device&, const ImageCreateInfo&, NoAllocateT), the image will
be created without any memory attached. Image memory requirements can be
subsequently queried using @ref memoryRequirements() and an allocated memory
bound with @ref bindMemory(). See @ref Memory for further details about memory allocation.

@snippet MagnumVk.cpp Image-creation-custom-allocation

Using @ref bindDedicatedMemory() instead of @ref bindMemory() will transfer
ownership of the @ref Memory to the image instance, making it subsequently
available through @ref dedicatedMemory(). This matches current behavior of the
@ref Image(Device&, const ImageCreateInfo&, MemoryFlags) constructor shown
above, except that you have more control over choosing and allocating the
memory.

@see @ref Buffer
*/
class MAGNUM_VK_EXPORT Image {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device            Vulkan device the image is created on
         * @param handle            The @type_vk{Image} handle
         * @param format            Image format. Available through
         *      @ref format() afterwards.
         * @param flags             Handle flags
         *
         * The @p handle is expected to be originating from @p device. The
         * @p format parameter is used for convenience @ref ImageView creation.
         * If it's unknown, use a @cpp PixelFormat{} @ce --- you will then be
         * able to only create image views by passing a concrete format to
         * @ref ImageViewCreateInfo.
         *
         * Unlike an image created using a constructor, the Vulkan image is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static Image wrap(Device& device, VkImage handle, PixelFormat format, HandleFlags flags = {});
        /** @overload */
        static Image wrap(Device& device, VkImage handle, Magnum::PixelFormat format, HandleFlags flags = {});
        /** @overload */
        static Image wrap(Device& device, VkImage handle, Magnum::CompressedPixelFormat format, HandleFlags flags = {});

        /**
         * @brief Construct an image without allocating
         * @param device    Vulkan device to create the image on
         * @param info      Image creation info
         *
         * Use @ref memoryRequirements(), @ref Memory and @ref bindMemory() to
         * bind a memory (sub)allocation to the image.
         * @see @ref Image(Device&, const ImageCreateInfo&, MemoryFlags),
         *      @fn_vk_keyword{CreateImage}
         */
        explicit Image(Device& device, const ImageCreateInfo& info, NoAllocateT);

        /**
         * @brief Construct an image
         * @param device        Vulkan device to create the image on
         * @param info          Image creation info
         * @param memoryFlags   Memory allocation flags
         *
         * Compared to @ref Image(Device&, const ImageCreateInfo&, NoAllocateT)
         * allocates a memory satisfying @p memoryFlags as well.
         *
         * @attention At this point, a dedicated allocation is used,
         *      subsequently accessible through @ref dedicatedMemory(). This
         *      behavior may change in the future.
         */
        explicit Image(Device& device, const ImageCreateInfo& info, MemoryFlags memoryFlags);

        /**
         * @brief Construct without creating the image
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Image(NoCreateT);

        /** @brief Copying is not allowed */
        Image(const Image&) = delete;

        /** @brief Move constructor */
        Image(Image&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{Image} handle, unless the instance
         * was created using @ref wrap() without @ref HandleFlag::DestroyOnDestruction
         * specified.
         * @see @fn_vk_keyword{DestroyImage}, @ref release()
         */
        ~Image();

        /** @brief Copying is not allowed */
        Image& operator=(const Image&) = delete;

        /** @brief Move assignment */
        Image& operator=(Image&& other) noexcept;

        /** @brief Underlying @type_vk{Image} handle */
        VkImage handle() { return _handle; }
        /** @overload */
        operator VkImage() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /** @brief Image format */
        PixelFormat format() const { return _format; }

        /**
         * @brief Image memory requirements
         *
         * @see @ref bindMemory(), @fn_vk_keyword{GetImageMemoryRequirements2},
         *      @fn_vk_keyword{GetImageMemoryRequirements}
         */
        MemoryRequirements memoryRequirements() const;

        /**
         * @brief Bind image memory
         *
         * Assumes that @p memory type, the amount of @p memory at @p offset
         * and @p offset alignment corresponds to image memory requirements.
         * @see @ref memoryRequirements(), @ref bindDedicatedMemory(),
         *      @fn_vk_keyword{BindImageMemory2},
         *      @fn_vk_keyword{BindImageMemory}
         */
        void bindMemory(Memory& memory, UnsignedLong offset);

        /**
         * @brief Bind a dedicated image memory
         *
         * Equivalent to @ref bindMemory() with @p offset set to @cpp 0 @ce,
         * with the additional effect that @p memory ownership transfers to the
         * image and is then available through @ref dedicatedMemory().
         */
        void bindDedicatedMemory(Memory&& memory);

        /**
         * @brief Whether the image has a dedicated memory
         *
         * Returns @cpp true @ce if the image memory was bound using
         * @ref bindDedicatedMemory(), @cpp false @ce otherwise.
         * @see @ref dedicatedMemory()
         */
        bool hasDedicatedMemory() const;

        /**
         * @brief Dedicated image memory
         *
         * Expects that the image has a dedicated memory.
         * @see @ref hasDedicatedMemory()
         */
        Memory& dedicatedMemory();

        /**
         * @brief Release the underlying Vulkan image
         *
         * Releases ownership of the Vulkan image and returns its handle so
         * @fn_vk{DestroyImage} is not called on destruction. The internal
         * state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkImage release();

    private:
        friend Implementation::DeviceState;

        MAGNUM_VK_LOCAL static void getMemoryRequirementsImplementationDefault(Device& device, const VkImageMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements);
        MAGNUM_VK_LOCAL static void getMemoryRequirementsImplementationKHR(Device& device, const VkImageMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements);
        MAGNUM_VK_LOCAL static void getMemoryRequirementsImplementation11(Device& device, const VkImageMemoryRequirementsInfo2& info, VkMemoryRequirements2& requirements);

        MAGNUM_VK_LOCAL static VkResult bindMemoryImplementationDefault(Device& device, UnsignedInt count, const VkBindImageMemoryInfo* infos);
        MAGNUM_VK_LOCAL static VkResult bindMemoryImplementationKHR(Device& device, UnsignedInt count, const VkBindImageMemoryInfo* infos);
        MAGNUM_VK_LOCAL static VkResult bindMemoryImplementation11(Device& device, UnsignedInt count, const VkBindImageMemoryInfo* infos);

        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkImage _handle;
        HandleFlags _flags;

        /* On 64-bit there would be a 7 byte padding after _flags anyway, we
           can use that to store information about image format for convenient
           view creation. On 32-bit it won't fit, but the extra memory use is
           still worth the advantages.

           Originally I wanted to store a desired VkImageViewType here as well,
           but the logic to what actually should be the view type is rather
           involved and not safe to rely on (e.g., implicit view type would be
           2D_ARRAY if there's more than one layer and then if you'd use just
           one layer it suddenly becomes just 2D, breaking everything). */
        PixelFormat _format;

        Memory _dedicatedMemory;
};

}}

#endif
