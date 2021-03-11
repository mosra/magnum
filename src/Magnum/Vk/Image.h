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

#include <initializer_list>
#include <Corrade/Containers/ArrayTuple.h>

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

Wraps @type_vk_keyword{ImageLayout}.
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
     *
     * @see @ref ImageUsage::Storage
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
     * @see @ref SubpassDescription::setColorAttachments()
     */
    ColorAttachment = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

    /**
     * Layout optimal for a read/write depth/stencil attachment, not guaranteed
     * to be usable for anything else.
     *
     * Only valid for images created with
     * @ref ImageUsage::DepthStencilAttachment.
     * @see @ref SubpassDescription::setDepthStencilAttachment()
     */
    DepthStencilAttachment = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,

    /**
     * Layout optimal for read-only access in a shader sampler, combined
     * image/sampler or input attachment; not guaranteed to be usable for
     * anything else.
     *
     * Only valid for images created with @ref ImageUsage::Sampled or
     * @ref ImageUsage::InputAttachment.
     * @see @ref DescriptorType::CombinedImageSampler,
     *      @ref DescriptorType::SampledImage,
     *      @ref DescriptorType::InputAttachment
     */
    ShaderReadOnly = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

    /**
     * Layout optimal for transfer sources; not guaranteed to be usable for
     * anything else.
     *
     * Only valid for images created with @ref ImageUsage::TransferSource.
     * @see @ref CommandBuffer::copyImage(),
     *      @ref CommandBuffer::copyImageToBuffer()
     */
    TransferSource = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,

    /**
     * Layout optimal for transfer destination; not guaranteed to be usable for
     * anything else.
     *
     * Only valid for images created with @ref ImageUsage::TransferDestination.
     * @see @ref CommandBuffer::clearColorImage(),
     *      @ref CommandBuffer::clearDepthStencilImage(),
     *      @ref CommandBuffer::clearDepthImage(),
     *      @ref CommandBuffer::clearStencilImage(),
     *      @ref CommandBuffer::copyImage(),
     *      @ref CommandBuffer::copyBufferToImage()
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

@section Vk-Image-usage Image usage

@subsection Vk-Image-usage-clear Clearing image data

Usually an image is cleared implicitly at the start of a render pass using
@ref AttachmentLoadOperation::Clear for the corresponding attachment and
specifying the clear color using @ref RenderPassBeginInfo::clearColor() /
@ref RenderPassBeginInfo::clearDepthStencil() "clearDepthStencil()". If you
need to do a clear outside of a render pass, it can be done using
@ref CommandBuffer::clearColorImage() /
@ref CommandBuffer::clearDepthStencilImage() "clearDepthStencilImage()" /
@ref CommandBuffer::clearDepthImage() "clearDepthImage()" /
@ref CommandBuffer::clearStencilImage() "clearStencilImage()". In most cases
you'll also need to perform a layout transition first using a
@ref CommandBuffer::pipelineBarrier(PipelineStages, PipelineStages, Containers::ArrayView<const ImageMemoryBarrier>, DependencyFlags) "pipelineBarrier()":

@snippet MagnumVk.cpp Image-usage-clear

@todoc expand with @fn_vk{CmdClearAttachments} when exposed

@subsection Vk-Image-usage-copy Copying image data

The most common image copy operation is uploading texture data from a
host-visible buffer to a device-local image. This is the preferred workflow
over using a host-visible linear image directly, since linear images are poorly
supported, have suboptimal access performance, and host-visible memory usually
isn't the fastest for device access. Similarly, for downloading a rendered
framebuffer back to the host it's recommended to linearize to a buffer instead
of rendering to a linear image, which isn't widely supported.

The copy is done using @ref CommandBuffer::copyBufferToImage() /
@ref CommandBuffer::copyImageToBuffer() "copyImageToBuffer()". For convenience,
you're encouraged to use the @ref BufferImageCopy1D, @ref BufferImageCopy2D
etc. constructors that will correctly set the remaining parameters for certain
image type. In most cases you'll also need to add two
@ref CommandBuffer::pipelineBarrier(PipelineStages, PipelineStages, Containers::ArrayView<const ImageMemoryBarrier>, DependencyFlags) "pipelineBarrier()"
commands to perform a layout transition before, and make the memory visible for
subsequent operations after. For example:

@snippet MagnumVk.cpp Image-usage-copy-from-buffer

Alternatively you can use @ref CopyBufferToImageInfo1D /
@ref CopyImageToBufferInfo1D, @ref CopyBufferToImageInfo2D /
@ref CopyImageToBufferInfo2D etc., as both @ref CopyBufferToImageInfo /
@ref CopyImageToBufferInfo and @ref BufferImageCopy (sub)classes have implicit
constructors. This can be handy when uploading multiple regions --- for example
uploading all mip levels of an image at the same time:

@snippet MagnumVk.cpp Image-usage-copy-from-buffer-multiple

Image/image copy is possible as well and is done using
@ref CommandBuffer::copyImage(). Because there's a lot of combinations of
source and destination image types, no convenience classes are provided in that
case. Together with a layout transition
@ref CommandBuffer::pipelineBarrier(PipelineStages, PipelineStages, Containers::ArrayView<const ImageMemoryBarrier>, DependencyFlags) "pipelineBarrier()"
for both images it could look like this:

@snippet MagnumVk.cpp Image-usage-copy-from-image

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

/**
@brief Image copy region
@m_since_latest

Wraps a @type_vk_keyword{ImageCopy2KHR}. This calss is subsequently passed to a
@ref CopyImageInfo and then used in @ref CommandBuffer::copyImage(). See
@ref Vk-Image-usage-copy for usage information and examples.

@section Vk-ImageCopy-compatibility Compatibility with VkImageCopy

While the class operates on the @type_vk{ImageCopy2KHR} structure that's
provided by the @vk_extension{KHR,copy_commands2} extension, conversion from
and to @type_vk{ImageCopy} is provided to some extent --- you can create
a @ref ImageCopy from it, call various methods on the instance and then get a
@type_vk{ImageCopy} back again using @ref vkImageCopy().

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{ImageCopy2KHR} fields and then perform the conversion instead of
editing the resulting @type_vk{ImageCopy}, as additional safety checks may be
done during the conversion to ensure no information is lost.

@see @ref BufferCopy, @ref BufferImageCopy
*/
class MAGNUM_VK_EXPORT ImageCopy {
    public:
        /**
         * @brief Constructor
         * @param aspects                   Aspects to copy between the images
         * @param sourceLevel               Source image mip level
         * @param sourceLayerOffset         Source image layer offset
         * @param sourceLayerCount          Source image layer count
         * @param sourceOffset              Source image offset
         * @param destinationLevel          Destination image level
         * @param destinationLayerOffset    Destination image layer offset
         * @param destinationLayerCount     Destination image layer count.
         *      Should be the same as @p sourceLayerCount except when copying
         *      data between 2D (array) and 3D images, in which case layers of
         *      one image correspond to depth of the other.
         * @param destinationOffset         Destination image offset
         * @param size                      Size of the copied region
         *
         * The following @type_vk{ImageCopy2KHR} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    @cpp srcSubresource.aspectMask @ce to @p aspects
         * -    @cpp srcSubresource.mipLevel @ce to @p sourceLevel
         * -    @cpp srcSubresource.baseArrayLayer @ce to @p sourceLayerOffset
         * -    @cpp srcSubresource.layerCount @ce to @p sourceLayerCount
         * -    `srcOffset` to @p sourceOffset
         * -    @cpp dstSubresource.aspectMask @ce to @p aspects
         * -    @cpp dstSubresource.mipLevel @ce to @p destinationLevel
         * -    @cpp dstSubresource.baseArrayLayer @ce to
         *      @p destinationLayerOffset
         * -    @cpp dstSubresource.layerCount @ce to @p destinationLayerCount
         * -    `dstOffset` to @p destinationOffset
         * -    `extent` to @p size
         *
         * There are various restrictions on @p sourceLayerCount,
         * @p destinationLayerCount and @p size depending on whether the source
         * and destination images have additional dimensions or layers.
         * However, because there is many possible combinations, this class
         * doesn't provide convenience classes for particular image types like
         * @ref BufferImageCopy does.
         *
         * @todo specifying different source/destination aspect, only for
         *      multi-planar image copies
         */
        /*implicit*/ ImageCopy(ImageAspects aspects, Int sourceLevel, Int sourceLayerOffset, Int sourceLayerCount, const Vector3i& sourceOffset, Int destinationLevel, Int destinationLayerOffset, Int destinationLayerCount, const Vector3i& destinationOffset, const Vector3i& size);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit ImageCopy(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit ImageCopy(const VkImageCopy2KHR& copy);

        /**
         * @brief Construct from a @type_vk{ImageCopy}
         *
         * Compared to the above, fills the common subset of
         * @type_vk{ImageCopy2KHR}, sets `sType` and zero-fills `pNext`.
         * @see @ref vkImageCopy()
         */
        explicit ImageCopy(const VkImageCopy& copy);

        /**
         * @brief Corresponding @type_vk{ImageCopy} structure
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support the @vk_extension{KHR,copy_commands2} extensions. See
         * @ref Vk-ImageCopy-compatibility for more information.
         * @see @ref ImageCopy(const VkImageCopy&),
         *      @ref CopyBufferInfo::vkBufferCopies(),
         *      @ref BufferCopy::vkBufferCopy(),
         *      @ref CopyImageInfo::vkImageCopies(),
         *      @ref CopyImageToBufferInfo::vkBufferImageCopies(),
         *      @ref CopyBufferToImageInfo::vkBufferImageCopies(),
         *      @ref BufferImageCopy::vkBufferImageCopy()
         */
        VkImageCopy vkImageCopy() const;

        /** @brief Underlying @type_vk{ImageCopy2KHR} structure */
        VkImageCopy2KHR& operator*() { return _copy; }
        /** @overload */
        const VkImageCopy2KHR& operator*() const { return _copy; }
        /** @overload */
        VkImageCopy2KHR* operator->() { return &_copy; }
        /** @overload */
        const VkImageCopy2KHR* operator->() const { return &_copy; }
        /** @overload */
        operator const VkImageCopy2KHR*() const { return &_copy; }

        /**
         * @overload
         *
         * The class is implicitly convertible to a reference in addition to
         * a pointer because the type is commonly used in arrays as well, which
         * would be annoying to do with a pointer conversion.
         */
        operator const VkImageCopy2KHR&() const { return _copy; }

    private:
        VkImageCopy2KHR _copy;
};

/**
@brief Image copy command
@m_since_latest

Wraps a @type_vk_keyword{CopyImageInfo2KHR}. This class is subsequently used in
@ref CommandBuffer::copyImage(). See @ref Vk-Image-usage-copy for more
information.

@section Vk-CopyImageInfo-compatibility Compatibility with vkCmdCopyImager()

While the class operates on the @type_vk{CopyImageInfo2KHR} structure that's
provided by the @vk_extension{KHR,copy_commands2} extension, conversion from
and to the set of parameters accepted by @fn_vk{CmdCopyImage} is provided to
some extent --- you can create @ref ImageCopy instances out of
@type_vk{ImageCopy} structures, pass them together with the rest to
@ref CopyImageInfo and then get a @type_vk{ImageCopy} list back again using
@ref vkImageCopies().

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{CopyImageInfo2KHR} fields and then perform the conversion instead of
editing the resulting @type_vk{ImageCopy} list, as additional safety checks may
be done during the conversion to ensure no information is lost.

@see @ref CopyBufferInfo, @ref CopyBufferToImageInfo,
    @ref CopyImageToBufferInfo
*/
class MAGNUM_VK_EXPORT CopyImageInfo {
    public:
        /**
         * @brief Constructor
         * @param source            Source @ref Image or a raw Vulkan image
         *      handle. Expected to have been created with
         *      @ref ImageUsage::TransferSource and a @ref PixelFormat usable
         *      for transfer source.
         * @param sourceLayout      Source image layout. Can be either
         *      @ref ImageLayout::General or @ref ImageLayout::TransferSource.
         * @param destination       Destination @ref Image or a raw Vulkan
         *      image handle. Expected to have been created with
         *      @ref ImageUsage::TransferDestination and a @ref PixelFormat
         *      usable for transfer destination.
         * @param destinationLayout Destination image layout. Can be either
         *      @ref ImageLayout::General or
         *      @ref ImageLayout::TransferDestination.
         * @param regions           Regions to copy. THere has to be at least
         *      one.
         *
         * The following @type_vk{CopyImageInfo2KHR} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `srcImage` to @p source
         * -    `srcImageLayout` to @p sourceLayout
         * -    `dstImage` to @p destination
         * -    `dstImageLayout` to @p destinationLayout
         * -    `regionCount` and `pRegions` to @p regions
         *
         * @todoc mention ImageLayout::SharedPresent being allowed once the
         *      extension is exposed
         */
        /*implicit*/ CopyImageInfo(VkImage source, ImageLayout sourceLayout, VkImage destination, ImageLayout destinationLayout, Containers::ArrayView<const ImageCopy> regions);
        /** @overload */
        /*implicit*/ CopyImageInfo(VkImage source, ImageLayout sourceLayout, VkImage destination, ImageLayout destinationLayout, std::initializer_list<ImageCopy> regions);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit CopyImageInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit CopyImageInfo(const VkCopyImageInfo2KHR& info);

        /**
         * @brief Corresponding @type_vk{ImageCopy} structures
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support the @vk_extension{KHR,copy_commands2} extension. See
         * @ref Vk-CopyImageInfo-compatibility for more information.
         * @see @ref CopyBufferInfo::vkBufferCopies(),
         *      @ref BufferCopy::vkBufferCopy(),
         *      @ref ImageCopy::vkImageCopy(),
         *      @ref CopyBufferToImageInfo::vkBufferImageCopies(),
         *      @ref CopyImageToBufferInfo::vkBufferImageCopies(),
         *      @ref BufferImageCopy::vkBufferImageCopy()
         */
        Containers::Array<VkImageCopy> vkImageCopies() const;

        /** @brief Underlying @type_vk{CopyImageInfo2KHR} structure */
        VkCopyImageInfo2KHR& operator*() { return _info; }
        /** @overload */
        const VkCopyImageInfo2KHR& operator*() const { return _info; }
        /** @overload */
        VkCopyImageInfo2KHR* operator->() { return &_info; }
        /** @overload */
        const VkCopyImageInfo2KHR* operator->() const { return &_info; }
        /** @overload */
        operator const VkCopyImageInfo2KHR*() const { return &_info; }

    private:
        VkCopyImageInfo2KHR _info;
        Containers::ArrayTuple _data;
};

/**
@brief Buffer / image copy region
@m_since_latest

Wraps a @type_vk_keyword{BufferImageCopy2KHR}. This class is subsequently
passed to a @ref CopyBufferToImageInfo / @ref CopyImageToBufferInfo and then
used in @ref CommandBuffer::copyBufferToImage() /
@ref CommandBuffer::copyImageToBuffer(). See @ref Vk-Image-usage-copy for usage
information and examples.

@section Vk-BufferImageCopy-compatibility Compatibility with VkBufferImageCopy

While the class operates on the @type_vk{BufferImageCopy2KHR} structure that's
provided by the @vk_extension{KHR,copy_commands2} extension, conversion from
and to @type_vk{BufferImageCopy} is provided to some extent --- you can create
a @ref BufferImageCopy from it, call various methods on the instance and then
get a @type_vk{BufferImageCopy} back again using @ref vkBufferImageCopy().

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{BufferImageCopy2KHR} fields and then perform the conversion instead
of editing the resulting @type_vk{BufferImageCopy}, as additional safety
checks may be done during the conversion to ensure no information is lost.

@see @ref BufferImageCopy1D, @ref BufferImageCopy2D, @ref BufferImageCopy3D,
    @ref BufferImageCopy1DArray, @ref BufferImageCopy2DArray,
    @ref BufferImageCopyCubeMap, @ref BufferImageCopyCubeMapArray,
    @ref BufferCopy, @ref ImageCopy
*/
class MAGNUM_VK_EXPORT BufferImageCopy {
    public:
        /**
         * @brief Constructor
         * @param bufferOffset      Buffer offset in bytes
         * @param bufferRowLength   Row length when linearized in the buffer,
         *      in pixels. Use @cpp 0 @ce for tightly packed rows, which is
         *      equivalent to width of @p imageRange.
         * @param bufferImageHeight Image height when linearized in the buffer,
         *      in pixels. Using @cpp 0 @ce for tightly packed images, which is
         *      equivalent to height of @p imageRange.
         * @param imageAspect       Image aspect to copy. Only one aspect can
         *      be copied at a time, so this is not @ref ImageAspects.
         * @param imageLevel        Image mip level
         * @param imageLayerOffset  Image layer offset
         * @param imageLayerCount   Image layer count
         * @param imageRange        Image range
         *
         * The following @type_vk{BufferImageCopy2KHR} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `bufferOffset`
         * -    `bufferRowLength`
         * -    `bufferImageHeight`
         * -    @cpp imageSubresource.aspectMask @ce to @p imageAspect
         * -    @cpp imageSubresource.mipLevel @ce to @p imageLevel
         * -    @cpp imageSubresource.baseArrayLayer @ce to @p imageLayerOffset
         * -    @cpp imageSubresource.layerCount @ce to @p imageLayerCount
         * -    `imageOffset` to @p imageRange @ref Math::Range::min() "min()"
         * -    `imageExtent` to @p imageRange @ref Math::Range::size() "size()"
         *
         * There are various restrictions on @p imageLayerCount and
         * @p imageRange depending on whether the image has additional
         * dimensions or layers and you're encouraged to make use of
         * @ref BufferImageCopy1D, @ref BufferImageCopy3D,
         * @ref BufferImageCopy3D, @ref BufferImageCopy1DArray,
         * @ref BufferImageCopy2DArray, @ref BufferImageCopyCubeMap and
         * @ref BufferImageCopyCubeMapArray convenience classes instead of this
         * constructor.
         */
        /*implicit*/ BufferImageCopy(UnsignedLong bufferOffset, UnsignedInt bufferRowLength, UnsignedInt bufferImageHeight, ImageAspect imageAspect, Int imageLevel, Int imageLayerOffset, Int imageLayerCount, const Range3Di& imageRange);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit BufferImageCopy(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit BufferImageCopy(const VkBufferImageCopy2KHR& copy);

        /**
         * @brief Construct from a @type_vk{BufferImageCopy}
         *
         * Compared to the above, fills the common subset of
         * @type_vk{BufferImageCopy2KHR}, sets `sType` and zero-fills `pNext`.
         * @see @ref vkBufferImageCopy()
         */
        explicit BufferImageCopy(const VkBufferImageCopy& copy);

        /**
         * @brief Corresponding @type_vk{BufferImageCopy} structure
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support the @vk_extension{KHR,copy_commands2} extensions. See
         * @ref Vk-BufferImageCopy-compatibility for more information.
         * @see @ref BufferImageCopy(const VkBufferImageCopy&),
         *      @ref CopyBufferInfo::vkBufferCopies(),
         *      @ref BufferCopy::vkBufferCopy(),
         *      @ref CopyImageInfo::vkImageCopies(),
         *      @ref ImageCopy::vkImageCopy(),
         *      @ref CopyImageToBufferInfo::vkBufferImageCopies(),
         *      @ref CopyBufferToImageInfo::vkBufferImageCopies()
         */
        VkBufferImageCopy vkBufferImageCopy() const;

        /** @brief Underlying @type_vk{BufferImageCopy2KHR} structure */
        VkBufferImageCopy2KHR& operator*() { return _copy; }
        /** @overload */
        const VkBufferImageCopy2KHR& operator*() const { return _copy; }
        /** @overload */
        VkBufferImageCopy2KHR* operator->() { return &_copy; }
        /** @overload */
        const VkBufferImageCopy2KHR* operator->() const { return &_copy; }
        /** @overload */
        operator const VkBufferImageCopy2KHR*() const { return &_copy; }

        /**
         * @overload
         *
         * The class is implicitly convertible to a reference in addition to
         * a pointer because the type is commonly used in arrays as well, which
         * would be annoying to do with a pointer conversion.
         */
        operator const VkBufferImageCopy2KHR&() const { return _copy; }

    private:
        VkBufferImageCopy2KHR _copy;
};

/**
@brief Convenience constructor for buffer / 1D image copy region
@m_since_latest

Compared to the base @ref BufferImageCopy::BufferImageCopy() "BufferImageCopy constructor"
sets `bufferRowLength` and `bufferImageHeight` to @cpp 0 @ce,
@cpp imageSubresource.baseArrayLayer @ce to @cpp 0 @ce,
@cpp imageSubresource.layerCount @ce to @cpp 1 @ce, Y and Z coordinate of
`imageOffset` to @cpp 0 @ce and Y and Z coordinate of `imageExtent` to
@cpp 1 @ce.
*/
class MAGNUM_VK_EXPORT BufferImageCopy1D: public BufferImageCopy {
    public:
        /** @brief Constructor */
        /*implicit*/ BufferImageCopy1D(UnsignedLong bufferOffset, ImageAspect imageAspect, Int imageLevel, const Range1Di& imageRange);
};

/**
@brief Convenience constructor for buffer / 2D image copy region
@m_since_latest

Compared to the base @ref BufferImageCopy::BufferImageCopy() "BufferImageCopy constructor"
sets `bufferImageHeight` to @cpp 0 @ce, @cpp imageSubresource.baseArrayLayer @ce
to @cpp 0 @ce, @cpp imageSubresource.layerCount @ce to @cpp 1 @ce, Z coordinate
of `imageOffset` to @cpp 0 @ce and Z coordinate of `imageExtent` to @cpp 1 @ce.
*/
class MAGNUM_VK_EXPORT BufferImageCopy2D: public BufferImageCopy {
    public:
        /** @brief Constructor */
        /*implicit*/ BufferImageCopy2D(UnsignedLong bufferOffset, UnsignedInt bufferRowLength, ImageAspect imageAspect, Int imageLevel, const Range2Di& imageRange);

        /**
         * @brief Copy tightly packed rows
         *
         * Equivalent to calling @ref BufferImageCopy2D(UnsignedLong, UnsignedInt, ImageAspect, Int, const Range2Di&)
         * with @p bufferRowLength set to @cpp 0 @ce.
         */
        /*implicit*/ BufferImageCopy2D(UnsignedLong bufferOffset, ImageAspect imageAspect, Int imageLevel, const Range2Di& imageRange): BufferImageCopy2D{bufferOffset, 0, imageAspect, imageLevel, imageRange} {}
};

/**
@brief Convenience constructor for buffer / 3D image copy region
@m_since_latest

Compared to the base @ref BufferImageCopy::BufferImageCopy() "BufferImageCopy constructor"
sets @cpp imageSubresource.baseArrayLayer @ce to @cpp 0 @ce and
@cpp imageSubresource.layerCount @ce to @cpp 1 @ce.
*/
class MAGNUM_VK_EXPORT BufferImageCopy3D: public BufferImageCopy {
    public:
        /** @brief Constructor */
        /*implicit*/ BufferImageCopy3D(UnsignedLong bufferOffset, UnsignedInt bufferRowLength, UnsignedInt bufferImageHeight, ImageAspect imageAspect, Int imageLevel, const Range3Di& imageRange);

        /**
         * @brief Copy tightly packed images
         *
         * Equivalent to calling @ref BufferImageCopy3D(UnsignedLong, UnsignedInt, UnsignedInt, ImageAspect, Int, const Range3Di&)
         * with @p bufferRowLength and @p bufferImageHeight set to @cpp 0 @ce.
         */
        /*implicit*/ BufferImageCopy3D(UnsignedLong bufferOffset, ImageAspect imageAspect, Int imageLevel, const Range3Di& imageRange): BufferImageCopy3D{bufferOffset, 0, 0, imageAspect, imageLevel, imageRange} {}
};

/**
@brief Convenience constructor for buffer / 1D array image copy region
@m_since_latest

Compared to the base @ref BufferImageCopy::BufferImageCopy() "BufferImageCopy constructor"
sets `bufferImageHeight` to @cpp 0 @ce, @cpp imageSubresource.baseArrayLayer @ce
to @cpp imageRange.min().y() @ce, @cpp imageSubresource.layerCount @ce to
@cpp imageRange.sizeY() @ce, Y and Z coordinate of `imageOffset` to @cpp 0 @ce
and Y and Z coordinate of `imageExtent` to @cpp 1 @ce.
*/
class MAGNUM_VK_EXPORT BufferImageCopy1DArray: public BufferImageCopy {
    public:
        /** @brief Constructor */
        /*implicit*/ BufferImageCopy1DArray(UnsignedLong bufferOffset, UnsignedInt bufferRowLength, ImageAspect imageAspect, Int imageLevel, const Range2Di& imageRange);

        /**
         * @brief Copy tightly packed images
         *
         * Equivalent to calling @ref BufferImageCopy1DArray(UnsignedLong, UnsignedInt, ImageAspect, Int, const Range2Di&)
         * with @p bufferRowLength set to @cpp 0 @ce.
         */
        /*implicit*/ BufferImageCopy1DArray(UnsignedLong bufferOffset, ImageAspect imageAspect, Int imageLevel, const Range2Di& imageRange): BufferImageCopy1DArray{bufferOffset, 0, imageAspect, imageLevel, imageRange} {}
};

/**
@brief Convenience constructor for buffer / 2D array image copy region
@m_since_latest

Compared to the base @ref BufferImageCopy::BufferImageCopy() "BufferImageCopy constructor"
sets @cpp imageSubresource.baseArrayLayer @ce to @cpp imageRange.min().z() @ce,
@cpp imageSubresource.layerCount @ce to @cpp imageRange.sizeZ() @ce Z
coordinate of `imageOffset` to @cpp 0 @ce and Z coordinate of `imageExtent` to
@cpp 1 @ce.
*/
class MAGNUM_VK_EXPORT BufferImageCopy2DArray: public BufferImageCopy {
    public:
        /** @brief Constructor */
        /*implicit*/ BufferImageCopy2DArray(UnsignedLong bufferOffset, UnsignedInt bufferRowLength, UnsignedInt bufferImageHeight, ImageAspect imageAspect, Int imageLevel, const Range3Di& imageRange);

        /**
         * @brief Copy tightly packed images
         *
         * Equivalent tocalling @ref BufferImageCopy2DArray(UnsignedLong, UnsignedInt, UnsignedInt, ImageAspect, Int, const Range3Di&)
         * with @p bufferRowLength and @p bufferImageHeight set to @cpp 0 @ce.
         */
        /*implicit*/ BufferImageCopy2DArray(UnsignedLong bufferOffset, ImageAspect imageAspect, Int imageLevel, const Range3Di& imageRange): BufferImageCopy2DArray{bufferOffset, 0, 0, imageAspect, imageLevel, imageRange} {}
};

/**
@brief Convenience constructor for buffer / cube map image copy region
@m_since_latest

Compared to the base @ref BufferImageCopy::BufferImageCopy() "BufferImageCopy constructor"
sets @cpp imageSubresource.baseArrayLayer @ce to @cpp 0 @ce,
@cpp imageSubresource.layerCount @to @cpp 6 @ce, Z coordinate of `imageOffset`
to @cpp 0 @ce and Z coordinate of `imageExtent` to @cpp 1 @ce.
*/
class MAGNUM_VK_EXPORT BufferImageCopyCubeMap: public BufferImageCopy {
    public:
        /** @brief Constructor */
        /*implicit*/ BufferImageCopyCubeMap(UnsignedLong bufferOffset, UnsignedInt bufferRowLength, UnsignedInt bufferImageHeight, ImageAspect imageAspect, Int imageLevel, const Range2Di& imageRange);

        /**
         * @brief Copy tightly packed images
         *
         * Equivalent tocalling @ref BufferImageCopyCubeMap(UnsignedLong, UnsignedInt, UnsignedInt, ImageAspect, Int, const Range2Di&)
         * with @p bufferRowLength and @p bufferImageHeight set to @cpp 0 @ce.
         */
        /*implicit*/ BufferImageCopyCubeMap(UnsignedLong bufferOffset, ImageAspect imageAspect, Int imageLevel, const Range2Di& imageRange): BufferImageCopyCubeMap{bufferOffset, 0, 0, imageAspect, imageLevel, imageRange} {}
};

/**
@brief Convenience constructor for buffer / cube map image copy region
@m_since_latest

Compared to the base @ref BufferImageCopy::BufferImageCopy() "BufferImageCopy constructor"
sets @cpp imageSubresource.baseArrayLayer @ce to @cpp imageRange.min().z() @ce,
@cpp imageSubresource.layerCount @to @cpp imageRange.sizeZ() @ce, Z coordinate
of `imageOffset` to @cpp 0 @ce and Z coordinate of `imageExtent` to @cpp 1 @ce.
*/
class MAGNUM_VK_EXPORT BufferImageCopyCubeMapArray: public BufferImageCopy {
    public:
        /** @brief Constructor */
        /*implicit*/ BufferImageCopyCubeMapArray(UnsignedLong bufferOffset, UnsignedInt bufferRowLength, UnsignedInt bufferImageHeight, ImageAspect imageAspect, Int imageLevel, const Range3Di& imageRange);

        /**
         * @brief Copy tightly packed images
         *
         * Equivalent tocalling @ref BufferImageCopyCubeMapArray(UnsignedLong, UnsignedInt, UnsignedInt, ImageAspect, Int, const Range3Di&)
         * with @p bufferRowLength and @p bufferImageHeight set to @cpp 0 @ce.
         */
        /*implicit*/ BufferImageCopyCubeMapArray(UnsignedLong bufferOffset, ImageAspect imageAspect, Int imageLevel, const Range3Di& imageRange): BufferImageCopyCubeMapArray{bufferOffset, 0, 0, imageAspect, imageLevel, imageRange} {}
};

/**
@brief Buffer to image copy command
@m_since_latest

Wraps a @type_vk_keyword{CopyBufferToImageInfo2KHR}. This class is subsequently
used in @ref CommandBuffer::copyBufferToImage(). See @ref Vk-Image-usage-copy
for more information.

@section Vk-CopyBufferToImageInfo-compatibility Compatibility with vkCmdCopyBufferToImage()

While the class operates on the @type_vk{CopyBufferToImageInfo2KHR} structure
that's provided by the @vk_extension{KHR,copy_commands2} extension, conversion
from and to the set of parameters accepted by @fn_vk{CmdCopyBufferToImage} is
provided to some extent --- you can create @ref BufferImageCopy instances out
of @type_vk{BufferImageCopy} structures, pass them together with the rest to
@ref CopyBufferToImageInfo and then get a @type_vk{BufferImageCopy} list back
again using @ref vkBufferImageCopies().

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{CopyBufferToImageInfo2KHR} fields and then perform the conversion
instead of editing the resulting @type_vk{BufferImageCopy} list, as additional
safety checks may be done during the conversion to ensure no information is
lost.

@see @ref CopyBufferToImageInfo1D, @ref CopyBufferToImageInfo2D,
    @ref CopyBufferToImageInfo3D, @ref CopyBufferToImageInfo1DArray,
    @ref CopyBufferToImageInfo2DArray, @ref CopyBufferToImageInfoCubeMap,
    @ref CopyBufferToImageInfoCubeMapArray, @ref CopyBufferInfo,
    @ref CopyImageInfo, @ref CopyImageToBufferInfo
*/
class MAGNUM_VK_EXPORT CopyBufferToImageInfo {
    public:
        /**
         * @brief Constructor
         * @param source            Source @ref Buffer or a raw Vulkan buffer
         *      handle. Expected to have been created with
         *      @ref BufferUsage::TransferSource.
         * @param destination       Destination @ref Image or a raw Vulkan
         *      image handle. Expected to have been created with
         *      @ref ImageUsage::TransferDestination and a @ref PixelFormat
         *      usable for transfer destination.
         * @param destinationLayout Destination image layout. Can be either
         *      @ref ImageLayout::General or
         *      @ref ImageLayout::TransferDestination.
         * @param regions           Regions to copy. There has to be at least
         *      one.
         *
         * The following @type_vk{CopyBufferToImageInfo2KHR} fields are
         * pre-filled in addition to `sType`, everything else is zero-filled:
         *
         * -    `srcBuffer` to @p source
         * -    `dstImage` to @p destination
         * -    `dstImageLayout` to @p destinationLayout
         * -    `regionCount` and `pRegions` to @p regions
         *
         * @todoc mention ImageLayout::SharedPresent being allowed once the
         *      extension is exposed
         */
        /*implicit*/ CopyBufferToImageInfo(VkBuffer source, VkImage destination, ImageLayout destinationLayout, Containers::ArrayView<const BufferImageCopy> regions);
        /** @overload */
        /*implicit*/ CopyBufferToImageInfo(VkBuffer source, VkImage destination, ImageLayout destinationLayout, std::initializer_list<BufferImageCopy> regions);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit CopyBufferToImageInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit CopyBufferToImageInfo(const VkCopyBufferToImageInfo2KHR& info);

        /**
         * @brief Corresponding @type_vk{BufferImageCopy} structures
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support the @vk_extension{KHR,copy_commands2} extension. See
         * @ref Vk-CopyBufferToImageInfo-compatibility for more information.
         * @see @ref CopyBufferInfo::vkBufferCopies(),
         *      @ref BufferCopy::vkBufferCopy(),
         *      @ref CopyImageInfo::vkImageCopies(),
         *      @ref ImageCopy::vkImageCopy(),
         *      @ref CopyImageToBufferInfo::vkBufferImageCopies(),
         *      @ref BufferImageCopy::vkBufferImageCopy()
         */
        Containers::Array<VkBufferImageCopy> vkBufferImageCopies() const;

        /** @brief Underlying @type_vk{CopyBufferToImageInfo2KHR} structure */
        VkCopyBufferToImageInfo2KHR& operator*() { return _info; }
        /** @overload */
        const VkCopyBufferToImageInfo2KHR& operator*() const { return _info; }
        /** @overload */
        VkCopyBufferToImageInfo2KHR* operator->() { return &_info; }
        /** @overload */
        const VkCopyBufferToImageInfo2KHR* operator->() const { return &_info; }
        /** @overload */
        operator const VkCopyBufferToImageInfo2KHR*() const { return &_info; }

    private:
        VkCopyBufferToImageInfo2KHR _info;
        Containers::ArrayTuple _data;
};

/**
@brief Convenience constructor for a buffer to 1D image copy command
@m_since_latest

Compared to the base @ref CopyBufferToImageInfo::CopyBufferToImageInfo() "CopyBufferToImageInfo constructor"
accepts a list of @ref BufferImageCopy1D convenience structures that are more
suited for copying 1D images. See @ref Vk-Image-usage-copy for example usage of
these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyBufferToImageInfo1D: public CopyBufferToImageInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyBufferToImageInfo1D(VkBuffer source, VkImage destination, ImageLayout destinationLayout, Containers::ArrayView<const BufferImageCopy1D> regions);
        /** @overload */
        /*implicit*/ CopyBufferToImageInfo1D(VkBuffer source, VkImage destination, ImageLayout destinationLayout, std::initializer_list<BufferImageCopy1D> regions);
};

/**
@brief Convenience constructor for a buffer to 2D image copy command
@m_since_latest

Compared to the base @ref CopyBufferToImageInfo::CopyBufferToImageInfo() "CopyBufferToImageInfo constructor"
accepts a list of @ref BufferImageCopy2D convenience structures that are more
suited for copying 2D images. See @ref Vk-Image-usage-copy for example usage of
these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyBufferToImageInfo2D: public CopyBufferToImageInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyBufferToImageInfo2D(VkBuffer source, VkImage destination, ImageLayout destinationLayout, Containers::ArrayView<const BufferImageCopy2D> regions);
        /** @overload */
        /*implicit*/ CopyBufferToImageInfo2D(VkBuffer source, VkImage destination, ImageLayout destinationLayout, std::initializer_list<BufferImageCopy2D> regions);
};

/**
@brief Convenience constructor for a buffer to 3D image copy command
@m_since_latest

Compared to the base @ref CopyBufferToImageInfo::CopyBufferToImageInfo() "CopyBufferToImageInfo constructor"
accepts a list of @ref BufferImageCopy3D convenience structures that are more
suited for copying 3D images. See @ref Vk-Image-usage-copy for example usage of
these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyBufferToImageInfo3D: public CopyBufferToImageInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyBufferToImageInfo3D(VkBuffer source, VkImage destination, ImageLayout destinationLayout, Containers::ArrayView<const BufferImageCopy3D> regions);
        /** @overload */
        /*implicit*/ CopyBufferToImageInfo3D(VkBuffer source, VkImage destination, ImageLayout destinationLayout, std::initializer_list<BufferImageCopy3D> regions);
};

/**
@brief Convenience constructor for a buffer to 1D array image copy command
@m_since_latest

Compared to the base @ref CopyBufferToImageInfo::CopyBufferToImageInfo() "CopyBufferToImageInfo constructor"
accepts a list of @ref BufferImageCopy1DArray convenience structures that are
more suited for copying 1D array images. See @ref Vk-Image-usage-copy for
example usage of these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyBufferToImageInfo1DArray: public CopyBufferToImageInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyBufferToImageInfo1DArray(VkBuffer source, VkImage destination, ImageLayout destinationLayout, Containers::ArrayView<const BufferImageCopy1DArray> regions);
        /** @overload */
        /*implicit*/ CopyBufferToImageInfo1DArray(VkBuffer source, VkImage destination, ImageLayout destinationLayout, std::initializer_list<BufferImageCopy1DArray> regions);
};

/**
@brief Convenience constructor for a buffer to 2D array image copy command
@m_since_latest

Compared to the base @ref CopyBufferToImageInfo::CopyBufferToImageInfo() "CopyBufferToImageInfo constructor"
accepts a list of @ref BufferImageCopy2DArray convenience structures that are
more suited for copying 2D array images. See @ref Vk-Image-usage-copy for
example usage of these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyBufferToImageInfo2DArray: public CopyBufferToImageInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyBufferToImageInfo2DArray(VkBuffer source, VkImage destination, ImageLayout destinationLayout, Containers::ArrayView<const BufferImageCopy2DArray> regions);
        /** @overload */
        /*implicit*/ CopyBufferToImageInfo2DArray(VkBuffer source, VkImage destination, ImageLayout destinationLayout, std::initializer_list<BufferImageCopy2DArray> regions);
};

/**
@brief Convenience constructor for a buffer to cube map image copy command
@m_since_latest

Compared to the base @ref CopyBufferToImageInfo::CopyBufferToImageInfo() "CopyBufferToImageInfo constructor"
accepts a list of @ref BufferImageCopyCubeMap convenience structures that are
more suited for copying cube map images. See @ref Vk-Image-usage-copy for
example usage of these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyBufferToImageInfoCubeMap: public CopyBufferToImageInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyBufferToImageInfoCubeMap(VkBuffer source, VkImage destination, ImageLayout destinationLayout, Containers::ArrayView<const BufferImageCopyCubeMap> regions);
        /** @overload */
        /*implicit*/ CopyBufferToImageInfoCubeMap(VkBuffer source, VkImage destination, ImageLayout destinationLayout, std::initializer_list<BufferImageCopyCubeMap> regions);
};

/**
@brief Convenience constructor for a buffer to cube map array image copy command
@m_since_latest

Compared to the base @ref CopyBufferToImageInfo::CopyBufferToImageInfo() "CopyBufferToImageInfo constructor"
accepts a list of @ref BufferImageCopyCubeMapArray convenience structures that
are more suited for copying cube map images. See @ref Vk-Image-usage-copy for
example usage of these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyBufferToImageInfoCubeMapArray: public CopyBufferToImageInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyBufferToImageInfoCubeMapArray(VkBuffer source, VkImage destination, ImageLayout destinationLayout, Containers::ArrayView<const BufferImageCopyCubeMapArray> regions);
        /** @overload */
        /*implicit*/ CopyBufferToImageInfoCubeMapArray(VkBuffer source, VkImage destination, ImageLayout destinationLayout, std::initializer_list<BufferImageCopyCubeMapArray> regions);
};

/**
@brief Image to buffer copy command
@m_since_latest

Wraps a @type_vk_keyword{CopyImageToBufferInfo2KHR}. This class is subsequently
used in @ref CommandBuffer::copyImageToBuffer(). See @ref Vk-Image-usage-copy
for more information.

@section Vk-CopyImageToBufferInfo-compatibility Compatibility with vkCmdCopyImageToBuffer()

While the class operates on the @type_vk{CopyImageToBufferInfo2KHR} structure
that's provided by the @vk_extension{KHR,copy_commands2} extension, conversion
from and to the set of parameters accepted by @fn_vk{CmdCopyImageToBuffer} is
provided to some extent --- you can create @ref BufferImageCopy instances out
of @type_vk{BufferImageCopy} structures, pass them together with the rest to
@ref CopyImageToBufferInfo and then get a @type_vk{BufferImageCopy} list back
again using @ref vkBufferImageCopies().

For direct editing of the Vulkan structure, it's recommended to edit the
@type_vk{CopyImageToBufferInfo2KHR} fields and then perform the conversion
instead of editing the resulting @type_vk{BufferImageCopy} list, as additional
safety checks may be done during the conversion to ensure no information is
lost.

@see @ref CopyImageToBufferInfo1D, @ref CopyImageToBufferInfo2D,
    @ref CopyImageToBufferInfo3D, @ref CopyImageToBufferInfo1DArray,
    @ref CopyImageToBufferInfo2DArray, @ref CopyImageToBufferInfoCubeMap,
    @ref CopyImageToBufferInfoCubeMapArray, @ref CopyBufferInfo,
    @ref CopyImageInfo, @ref CopyBufferToImageInfo
*/
class MAGNUM_VK_EXPORT CopyImageToBufferInfo {
    public:
        /**
         * @brief Constructor
         * @param source            Source @ref Image or a raw Vulkan image
         *      handle. Expected to have been created with
         *      @ref ImageUsage::TransferSource and a @ref PixelFormat usable
         *      for transfer source.
         * @param sourceLayout      Source image layout. Can be either
         *      @ref ImageLayout::General or @ref ImageLayout::TransferSource.
         * @param destination       Destination @ref Buffer or a raw Vulkan
         *      buffer handle. Expected to have been created with
         *      @ref BufferUsage::TransferDestination.
         * @param regions           Regions to copy. There has to be at least
         *      one.
         *
         * The following @type_vk{CopyBufferToImageInfo2KHR} fields are
         * pre-filled in addition to `sType`, everything else is zero-filled:
         *
         * -    `srcImage` to @p source
         * -    `srcImageLayout` to @p sourceLayout
         * -    `dstBuffer` to @p destination
         * -    `regionCount` and `pRegions` to @p regions
         *
         * @todoc mention ImageLayout::SharedPresent being allowed once the
         *      extension is exposed
         */
        /*implicit*/ CopyImageToBufferInfo(VkImage source, ImageLayout sourceLayout, VkBuffer destination, Containers::ArrayView<const BufferImageCopy> regions);
        /** @overload */
        /*implicit*/ CopyImageToBufferInfo(VkImage source, ImageLayout sourceLayout, VkBuffer destination, std::initializer_list<BufferImageCopy> regions);

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit CopyImageToBufferInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit CopyImageToBufferInfo(const VkCopyImageToBufferInfo2KHR& info);

        /**
         * @brief Corresponding @type_vk{BufferImageCopy} structures
         *
         * Provided for compatibility with Vulkan implementations that don't
         * support the @vk_extension{KHR,copy_commands2} extension. See
         * @ref Vk-CopyImageToBufferInfo-compatibility for more information.
         * @see @ref CopyBufferInfo::vkBufferCopies(),
         *      @ref BufferCopy::vkBufferCopy(),
         *      @ref CopyImageInfo::vkImageCopies(),
         *      @ref ImageCopy::vkImageCopy(),
         *      @ref CopyBufferToImageInfo::vkBufferImageCopies(),
         *      @ref BufferImageCopy::vkBufferImageCopy()
         */
        Containers::Array<VkBufferImageCopy> vkBufferImageCopies() const;

        /** @brief Underlying @type_vk{CopyImageToBufferInfo2KHR} structure */
        VkCopyImageToBufferInfo2KHR& operator*() { return _info; }
        /** @overload */
        const VkCopyImageToBufferInfo2KHR& operator*() const { return _info; }
        /** @overload */
        VkCopyImageToBufferInfo2KHR* operator->() { return &_info; }
        /** @overload */
        const VkCopyImageToBufferInfo2KHR* operator->() const { return &_info; }
        /** @overload */
        operator const VkCopyImageToBufferInfo2KHR*() const { return &_info; }

    private:
        VkCopyImageToBufferInfo2KHR _info;
        Containers::ArrayTuple _data;
};

/**
@brief Convenience constructor for a 1D image to buffer copy command
@m_since_latest

Compared to the base @ref CopyImageToBufferInfo::CopyImageToBufferInfo() "CopyImageToBufferInfo constructor"
accepts a list of @ref BufferImageCopy1D convenience structures that are more
suited for copying 1D images. See @ref Vk-Image-usage-copy for example usage of
these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyImageToBufferInfo1D: public CopyImageToBufferInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyImageToBufferInfo1D(VkImage source, ImageLayout sourceLayout, VkBuffer destination, Containers::ArrayView<const BufferImageCopy1D> regions);
        /** @overload */
        /*implicit*/ CopyImageToBufferInfo1D(VkImage source, ImageLayout sourceLayout, VkBuffer destination, std::initializer_list<BufferImageCopy1D> regions);
};

/**
@brief Convenience constructor for a 2D image to buffer copy command
@m_since_latest

Compared to the base @ref CopyImageToBufferInfo::CopyImageToBufferInfo() "CopyImageToBufferInfo constructor"
accepts a list of @ref BufferImageCopy2D convenience structures that are more
suited for copying 2D images. See @ref Vk-Image-usage-copy for example usage of
these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyImageToBufferInfo2D: public CopyImageToBufferInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyImageToBufferInfo2D(VkImage source, ImageLayout sourceLayout, VkBuffer destination, Containers::ArrayView<const BufferImageCopy2D> regions);
        /** @overload */
        /*implicit*/ CopyImageToBufferInfo2D(VkImage source, ImageLayout sourceLayout, VkBuffer destination, std::initializer_list<BufferImageCopy2D> regions);
};

/**
@brief Convenience constructor for a 3D image to buffer copy command
@m_since_latest

Compared to the base @ref CopyImageToBufferInfo::CopyImageToBufferInfo() "CopyImageToBufferInfo constructor"
accepts a list of @ref BufferImageCopy3D convenience structures that are more
suited for copying 3D images. See @ref Vk-Image-usage-copy for example usage of
these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyImageToBufferInfo3D: public CopyImageToBufferInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyImageToBufferInfo3D(VkImage source, ImageLayout sourceLayout, VkBuffer destination, Containers::ArrayView<const BufferImageCopy3D> regions);
        /** @overload */
        /*implicit*/ CopyImageToBufferInfo3D(VkImage source, ImageLayout sourceLayout, VkBuffer destination, std::initializer_list<BufferImageCopy3D> regions);
};

/**
@brief Convenience constructor for a 1D array image to buffer copy command
@m_since_latest

Compared to the base @ref CopyImageToBufferInfo::CopyImageToBufferInfo() "CopyImageToBufferInfo constructor"
accepts a list of @ref BufferImageCopy1DArray convenience structures that are
more suited for copying 1D array images. See @ref Vk-Image-usage-copy for
example usage of these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyImageToBufferInfo1DArray: public CopyImageToBufferInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyImageToBufferInfo1DArray(VkImage source, ImageLayout sourceLayout, VkBuffer destination, Containers::ArrayView<const BufferImageCopy1DArray> regions);
        /** @overload */
        /*implicit*/ CopyImageToBufferInfo1DArray(VkImage source, ImageLayout sourceLayout, VkBuffer destination, std::initializer_list<BufferImageCopy1DArray> regions);
};

/**
@brief Convenience constructor for a 2D array image to buffer copy command
@m_since_latest

Compared to the base @ref CopyImageToBufferInfo::CopyImageToBufferInfo() "CopyImageToBufferInfo constructor"
accepts a list of @ref BufferImageCopy2DArray convenience structures that are
more suited for copying 2D array images. See @ref Vk-Image-usage-copy for
example usage of these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyImageToBufferInfo2DArray: public CopyImageToBufferInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyImageToBufferInfo2DArray(VkImage source, ImageLayout sourceLayout, VkBuffer destination, Containers::ArrayView<const BufferImageCopy2DArray> regions);
        /** @overload */
        /*implicit*/ CopyImageToBufferInfo2DArray(VkImage source, ImageLayout sourceLayout, VkBuffer destination, std::initializer_list<BufferImageCopy2DArray> regions);
};

/**
@brief Convenience constructor for a cube map image to buffer copy command
@m_since_latest

Compared to the base @ref CopyImageToBufferInfo::CopyImageToBufferInfo() "CopyImageToBufferInfo constructor"
accepts a list of @ref BufferImageCopyCubeMap convenience structures that are
more suited for copying cube map images. See @ref Vk-Image-usage-copy for
example usage of these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyImageToBufferInfoCubeMap: public CopyImageToBufferInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyImageToBufferInfoCubeMap(VkImage source, ImageLayout sourceLayout, VkBuffer destination, Containers::ArrayView<const BufferImageCopyCubeMap> regions);
        /** @overload */
        /*implicit*/ CopyImageToBufferInfoCubeMap(VkImage source, ImageLayout sourceLayout, VkBuffer destination, std::initializer_list<BufferImageCopyCubeMap> regions);
};

/**
@brief Convenience constructor for a cube map array image to buffer copy command
@m_since_latest

Compared to the base @ref CopyImageToBufferInfo::CopyImageToBufferInfo() "CopyImageToBufferInfo constructor"
accepts a list of @ref BufferImageCopyCubeMapArray convenience structures that
are more suited for copying cube map array images. See @ref Vk-Image-usage-copy
for example usage of these convenience classes.
*/
class MAGNUM_VK_EXPORT CopyImageToBufferInfoCubeMapArray: public CopyImageToBufferInfo {
    public:
        /** @brief Constructor */
        /*implicit*/ CopyImageToBufferInfoCubeMapArray(VkImage source, ImageLayout sourceLayout, VkBuffer destination, Containers::ArrayView<const BufferImageCopyCubeMapArray> regions);
        /** @overload */
        /*implicit*/ CopyImageToBufferInfoCubeMapArray(VkImage source, ImageLayout sourceLayout, VkBuffer destination, std::initializer_list<BufferImageCopyCubeMapArray> regions);
};

}}

#endif
