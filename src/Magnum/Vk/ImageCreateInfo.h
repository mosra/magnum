#ifndef Magnum_Vk_ImageCreateInfo_h
#define Magnum_Vk_ImageCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::ImageCreateInfo, @ref Magnum::Vk::ImageCreateInfo1D, @ref Magnum::Vk::ImageCreateInfo2D, @ref Magnum::Vk::ImageCreateInfo3D, @ref Magnum::Vk::ImageCreateInfo1DArray, @ref Magnum::Vk::ImageCreateInfo2DArray, @ref Magnum::Vk::ImageCreateInfoCubeMap, @ref Magnum::Vk::ImageCreateInfoCubeMapArray, enum @ref Magnum::Vk::ImageUsage, enum set @ref Magnum::Vk::ImageUsages
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/DimensionTraits.h"
#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. Compared to other cases we do this
   on top as we need the ImageLayout definition here. */
#include "Magnum/Vk/Image.h"

namespace Magnum { namespace Vk {

/**
@brief Image usage
@m_since_latest

Wraps a @type_vk_keyword{ImageUsageFlagBits}.
@see @ref ImageUsages, @ref ImageCreateInfo
@m_enum_values_as_keywords
*/
enum class ImageUsage: UnsignedInt {
    /**
     * Source of a transfer command
     *
     * @see @ref ImageLayout::TransferSource,
     *      @ref CommandBuffer::copyImage(),
     *      @ref CommandBuffer::copyImageToBuffer()
     */
    TransferSource = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,

    /**
     * Destination of a transfer command
     *
     * @see @ref ImageLayout::TransferDestination,
     *      @ref CommandBuffer::clearColorImage(),
     *      @ref CommandBuffer::clearDepthStencilImage(),
     *      @ref CommandBuffer::clearDepthImage(),
     *      @ref CommandBuffer::clearStencilImage(),
     *      @ref CommandBuffer::copyImage(),
     *      @ref CommandBuffer::copyBufferToImage()
     */
    TransferDestination = VK_IMAGE_USAGE_TRANSFER_DST_BIT,

    /**
     * Sampled by a shader.
     *
     * Not all pixel formats support sampling, see @ref PixelFormat for more
     * information.
     * @see @ref ImageLayout::ShaderReadOnly,
     *      @ref DescriptorType::SampledImage
     */
    Sampled = VK_IMAGE_USAGE_SAMPLED_BIT,

    /**
     * Shader storage.
     *
     * Not all pixel formats support shader storage, with some requiring the
     * @ref DeviceFeature::ShaderStorageImageExtendedFormats feature. See
     * @ref PixelFormat for more information.
     * @see @ref ImageLayout::General,
     *      @ref DescriptorType::StorageImage
     */
    Storage = VK_IMAGE_USAGE_STORAGE_BIT,

    /**
     * Color attachment
     *
     * Not all pixel formats support color attachment, see @ref PixelFormat for
     * more information.
     * @see @ref ImageLayout::ColorAttachment,
     *      @ref SubpassDescription::setColorAttachments()
     */
    ColorAttachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

    /**
     * Depth/stencil attachment
     *
     * Note that only one of @ref PixelFormat::Depth24Unorm /
     * @ref PixelFormat::Depth32F and @ref PixelFormat::Depth24UnormStencil8UI
     * / @ref PixelFormat::Depth32FStencil8UI is guaranteed to support
     * depth/stencil attachment.
     * @see @ref ImageLayout::DepthStencilAttachment,
     *      @ref SubpassDescription::setDepthStencilAttachment()
     */
    DepthStencilAttachment = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,

    /** Transient attachment */
    TransientAttachment = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,

    /**
     * Input attachment in a shader or framebuffer
     *
     * @see @ref ImageLayout::ShaderReadOnly,
     *      @ref DescriptorType::InputAttachment,
     *      @ref SubpassDescription::setInputAttachments()
     */
    InputAttachment = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
};

/**
@brief Image usages
@m_since_latest

Type-safe wrapper for @type_vk_keyword{ImageUsageFlags}.
@see @ref ImageCreateInfo
*/
typedef Containers::EnumSet<ImageUsage> ImageUsages;

CORRADE_ENUMSET_OPERATORS(ImageUsages)

/**
@brief Image creation info
@m_since_latest

Wraps a @type_vk_keyword{ImageCreateInfo}. See
@ref Vk-Image-creation "Image creation" for usage information.
@see @ref ImageCreateInfo1D, @ref ImageCreateInfo2D, @ref ImageCreateInfo3D,
    @ref ImageCreateInfo1DArray, @ref ImageCreateInfo2DArray,
    @ref ImageCreateInfoCubeMap, @ref ImageCreateInfoCubeMapArray
*/
class MAGNUM_VK_EXPORT ImageCreateInfo {
    public:
        /**
         * @brief Image creation flag
         *
         * Wraps @type_vk_keyword{ImageCreateFlagBits}.
         * @see @ref Flags, @ref ImageCreateInfo(VkImageType, ImageUsages, PixelFormat, const Vector3i&, Int, Int, Int, ImageLayout, Flags)
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /** @todo sparse binding/residency/aliased */

            /**
             * Allow creating a view of different format
             *
             * @todo implement @vk_extension{KHR,image_format_list}
             */
            MutableFormat = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT,

            /** Allow creating a cube map view */
            CubeCompatible = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,

            /** @todo alias, 2D array compatible ... (Vulkan 1.1+) */
        };

        /**
         * @brief Image creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{ImageCreateFlags}.
         * @see @ref ImageCreateInfo(VkImageType, ImageUsages, PixelFormat, const Vector3i&, Int, Int, Int, ImageLayout, Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param type          Image type
         * @param usages        Desired image usage. At least one flag is
         *      required.
         * @param format        Image format. Available through
         *      @ref Image::format() afterwards.
         * @param size          Image size
         * @param layers        Array layer count
         * @param levels        Mip level count
         * @param samples       Sample count
         * @param initialLayout Initial layout. Can be only either
         *      @ref ImageLayout::Undefined or @ref ImageLayout::Preinitialized.
         * @param flags         Image creation flags
         *
         * The following @type_vk{ImageCreateInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `imageType` to @p type
         * -    `format`
         * -    `extent` to @p size
         * -    `mipLevels` to @p levels
         * -    `arrayLayers` to @p layers
         * -    `samples`
         * -    `tiling` to @val_vk{IMAGE_TILING_OPTIMAL,ImageTiling}
         * -    `usage` to @p usages
         * -    `sharingMode` to @val_vk{SHARING_MODE_EXCLUSIVE,SharingMode}
         * -    `initialLayout`
         *
         * There are various restrictions on @p size, @p layers, @p levels for
         * a particular @p type --- for common image types you're encouraged to
         * make use of @ref ImageCreateInfo1D, @ref ImageCreateInfo2D,
         * @ref ImageCreateInfo3D, @ref ImageCreateInfo1DArray,
         * @ref ImageCreateInfo2DArray, @ref ImageCreateInfoCubeMap and
         * @ref ImageCreateInfoCubeMapArray convenience classes instead of
         * this constructor.
         */
        explicit ImageCreateInfo(VkImageType type, ImageUsages usages, PixelFormat format, const Vector3i& size, Int layers, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {});
        /** @overload */
        explicit ImageCreateInfo(VkImageType type, ImageUsages usages, Magnum::PixelFormat format, const Vector3i& size, Int layers, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {});
        /** @overload */
        explicit ImageCreateInfo(VkImageType type, ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector3i& size, Int layers, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {});

        /* No overload w/o initialLayout here as the general public is expected
           to use the convenience classes anyway */

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit ImageCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit ImageCreateInfo(const VkImageCreateInfo& info);

        /** @brief Underlying @type_vk{ImageCreateInfo} structure */
        VkImageCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkImageCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkImageCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkImageCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkImageCreateInfo*() const { return &_info; }

    private:
        VkImageCreateInfo _info;
};

CORRADE_ENUMSET_OPERATORS(ImageCreateInfo::Flags)

/**
@brief Convenience constructor for 1D images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_1D,ImageType} with the last two `size` components and
`layers` set to @cpp 1 @ce. You can use both @ref ImageViewCreateInfo1D and
@ref ImageViewCreateInfo1DArray for view creation, although the array will need
to have only one layer.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, PixelFormat, const Vector3i&, Int, Int, Int, ImageLayout, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfo1D: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfo1D(ImageUsages usages, PixelFormat format, Int size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_1D, usages, format, {size, 1, 1}, 1, levels, samples, initialLayout, flags} {}
        /** @overload */
        explicit ImageCreateInfo1D(ImageUsages usages, Magnum::PixelFormat format, Int size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_1D, usages, format, {size, 1, 1}, 1, levels, samples, initialLayout, flags} {}
        /** @overload */
        explicit ImageCreateInfo1D(ImageUsages usages, Magnum::CompressedPixelFormat format, Int size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_1D, usages, format, {size, 1, 1}, 1, levels, samples, initialLayout, flags} {}

        /** @overload
         *
         * Equivalent to the above with @p initialLayout set to
         * @ref ImageLayout::Undefined.
         */
        explicit ImageCreateInfo1D(ImageUsages usages, PixelFormat format, Int size, Int levels, Int samples, Flags flags): ImageCreateInfo1D{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfo1D(ImageUsages usages, Magnum::PixelFormat format, Int size, Int levels, Int samples, Flags flags): ImageCreateInfo1D{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfo1D(ImageUsages usages, Magnum::CompressedPixelFormat format, Int size, Int levels, Int samples, Flags flags): ImageCreateInfo1D{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
};

/**
@brief Convenience constructor for 2D images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_2D,ImageType} with the last `size` component and `layers`
set to @cpp 1 @ce. You can use both @ref ImageViewCreateInfo2D and
@ref ImageViewCreateInfo2DArray for view creation, although the array will need
to have only one layer.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, PixelFormat, const Vector3i&, Int, Int, Int, ImageLayout, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfo2D: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfo2D(ImageUsages usages, PixelFormat format, const Vector2i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size, 1}, 1, levels, samples, initialLayout, flags} {}
        /** @overload */
        explicit ImageCreateInfo2D(ImageUsages usages, Magnum::PixelFormat format, const Vector2i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size, 1}, 1, levels, samples, initialLayout, flags} {}
        /** @overload */
        explicit ImageCreateInfo2D(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector2i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size, 1}, 1, levels, samples, initialLayout, flags} {}

        /** @overload
         *
         * Equivalent to the above with @p initialLayout set to
         * @ref ImageLayout::Undefined.
         */
        explicit ImageCreateInfo2D(ImageUsages usages, PixelFormat format, const Vector2i& size, Int levels, Int samples, Flags flags): ImageCreateInfo2D{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfo2D(ImageUsages usages, Magnum::PixelFormat format, const Vector2i& size, Int levels, Int samples, Flags flags): ImageCreateInfo2D{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfo2D(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector2i& size, Int levels, Int samples, Flags flags): ImageCreateInfo2D{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
};

/**
@brief Convenience constructor for 3D images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_3D,ImageType} with `layers` set to @cpp 1 @ce. Use
@ref ImageViewCreateInfo3D for view creation.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, PixelFormat, const Vector3i&, Int, Int, Int, ImageLayout, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfo3D: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfo3D(ImageUsages usages, PixelFormat format, const Vector3i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_3D, usages, format, size, 1, levels, samples, initialLayout, flags} {}
        /** @overload */
        explicit ImageCreateInfo3D(ImageUsages usages, Magnum::PixelFormat format, const Vector3i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_3D, usages, format, size, 1, levels, samples, initialLayout, flags} {}
        /** @overload */
        explicit ImageCreateInfo3D(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector3i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_3D, usages, format, size, 1, levels, samples, initialLayout, flags} {}

        /** @overload
         *
         * Equivalent to the above with @p initialLayout set to
         * @ref ImageLayout::Undefined.
         */
        explicit ImageCreateInfo3D(ImageUsages usages, PixelFormat format, const Vector3i& size, Int levels, Int samples, Flags flags): ImageCreateInfo3D{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfo3D(ImageUsages usages, Magnum::PixelFormat format, const Vector3i& size, Int levels, Int samples, Flags flags): ImageCreateInfo3D{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfo3D(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector3i& size, Int levels, Int samples, Flags flags): ImageCreateInfo3D{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
};

/**
@brief Convenience constructor for 1D array images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_1D,ImageType} with the last two `size` components set to
@cpp 1 @ce and `layers` set to @cpp size.y() @ce. You can use both
@ref ImageViewCreateInfo1D and @ref ImageViewCreateInfo1DArray for view
creation.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, PixelFormat, const Vector3i&, Int, Int, Int, ImageLayout, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfo1DArray: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfo1DArray(ImageUsages usages, PixelFormat format, const Vector2i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_1D, usages, format, {size.x(), 1, 1}, size.y(), levels, samples, initialLayout, flags} {}
        /** @overload */
        explicit ImageCreateInfo1DArray(ImageUsages usages, Magnum::PixelFormat format, const Vector2i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_1D, usages, format, {size.x(), 1, 1}, size.y(), levels, samples, initialLayout, flags} {}
        /** @overload */
        explicit ImageCreateInfo1DArray(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector2i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_1D, usages, format, {size.x(), 1, 1}, size.y(), levels, samples, initialLayout, flags} {}

        /** @overload
         *
         * Equivalent to the above with @p initialLayout set to
         * @ref ImageLayout::Undefined.
         */
        explicit ImageCreateInfo1DArray(ImageUsages usages, PixelFormat format, const Vector2i& size, Int levels, Int samples, Flags flags): ImageCreateInfo1DArray{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfo1DArray(ImageUsages usages, Magnum::PixelFormat format, const Vector2i& size, Int levels, Int samples, Flags flags): ImageCreateInfo1DArray{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfo1DArray(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector2i& size, Int levels, Int samples, Flags flags): ImageCreateInfo1DArray{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
};

/**
@brief Convenience constructor for 2D array images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_2D,ImageType} with the last `size` component set to
@cpp 1 @ce and `layers` set to @cpp size.z() @ce. You can use both
@ref ImageViewCreateInfo2D, @ref ImageViewCreateInfo2DArray for view creation
and if you set @ref Flag::CubeCompatible a @ref ImageViewCreateInfoCubeMap as
well, although in that case it's better to use @ref ImageCreateInfoCubeMap that
does this automatically.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, PixelFormat, const Vector3i&, Int, Int, Int, ImageLayout, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfo2DArray: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfo2DArray(ImageUsages usages, PixelFormat format, const Vector3i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size.xy(), 1}, size.z(), levels, samples, initialLayout, flags} {}
        /** @overload */
        explicit ImageCreateInfo2DArray(ImageUsages usages, Magnum::PixelFormat format, const Vector3i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size.xy(), 1}, size.z(), levels, samples, initialLayout, flags} {}
        /** @overload */
        explicit ImageCreateInfo2DArray(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector3i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size.xy(), 1}, size.z(), levels, samples, initialLayout, flags} {}

        /** @overload
         *
         * Equivalent to the above with @p initialLayout set to
         * @ref ImageLayout::Undefined.
         */
        explicit ImageCreateInfo2DArray(ImageUsages usages, PixelFormat format, const Vector3i& size, Int levels, Int samples, Flags flags): ImageCreateInfo2DArray{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfo2DArray(ImageUsages usages, Magnum::PixelFormat format, const Vector3i& size, Int levels, Int samples, Flags flags): ImageCreateInfo2DArray{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfo2DArray(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector3i& size, Int levels, Int samples, Flags flags): ImageCreateInfo2DArray{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
};

/**
@brief Convenience constructor for cube map images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_2D,ImageType} with the last `size` component set to
@cpp 1 @ce, `layers` set to @cpp 6 @ce and `flags` additionally having
@ref Flag::CubeCompatible. You can use any of @ref ImageViewCreateInfo2D,
@ref ImageViewCreateInfo2DArray, @ref ImageViewCreateInfoCubeMap or
@ref ImageViewCreateInfoCubeMapArray for view creation, although the last one
will need to have exactly six layers, and requires
@ref DeviceFeature::ImageCubeArray to be enabled.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, PixelFormat, const Vector3i&, Int, Int, Int, ImageLayout, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfoCubeMap: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfoCubeMap(ImageUsages usages, PixelFormat format, const Vector2i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size, 1}, 6, levels, samples, initialLayout, flags|Flag::CubeCompatible} {}
        /** @overload */
        explicit ImageCreateInfoCubeMap(ImageUsages usages, Magnum::PixelFormat format, const Vector2i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size, 1}, 6, levels, samples, initialLayout, flags|Flag::CubeCompatible} {}
        /** @overload */
        explicit ImageCreateInfoCubeMap(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector2i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size, 1}, 6, levels, samples, initialLayout, flags|Flag::CubeCompatible} {}

        /** @overload
         *
         * Equivalent to the above with @p initialLayout set to
         * @ref ImageLayout::Undefined.
         */
        explicit ImageCreateInfoCubeMap(ImageUsages usages, PixelFormat format, const Vector2i& size, Int levels, Int samples, Flags flags): ImageCreateInfoCubeMap{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfoCubeMap(ImageUsages usages, Magnum::PixelFormat format, const Vector2i& size, Int levels, Int samples, Flags flags): ImageCreateInfoCubeMap{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfoCubeMap(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector2i& size, Int levels, Int samples, Flags flags): ImageCreateInfoCubeMap{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
};

/**
@brief Convenience constructor for cube map array images
@m_since_latest

Compared to the base @ref ImageCreateInfo constructor creates an image of type
@val_vk{IMAGE_TYPE_2D,ImageType} with the last `size` component set to
@cpp 1 @ce, `layers` set to @cpp size.y() @ce and `flags` additionally having
@ref Flag::CubeCompatible. You can use any of @ref ImageViewCreateInfo2D,
@ref ImageViewCreateInfo2DArray, @ref ImageViewCreateInfoCubeMap or
@ref ImageViewCreateInfoCubeMapArray for view creation, note the last
requires @ref DeviceFeature::ImageCubeArray to be enabled.

Note that same as with the
@ref ImageCreateInfo::ImageCreateInfo(VkImageType, ImageUsages, PixelFormat, const Vector3i&, Int, Int, Int, ImageLayout, Flags)
constructor, at least one @ref ImageUsage value is required.
*/
class ImageCreateInfoCubeMapArray: public ImageCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageCreateInfoCubeMapArray(ImageUsages usages, PixelFormat format, const Vector3i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size.xy(), 1}, size.z(), levels, samples, initialLayout, flags|Flag::CubeCompatible} {}
        /** @overload */
        explicit ImageCreateInfoCubeMapArray(ImageUsages usages, Magnum::PixelFormat format, const Vector3i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size.xy(), 1}, size.z(), levels, samples, initialLayout, flags|Flag::CubeCompatible} {}
        /** @overload */
        explicit ImageCreateInfoCubeMapArray(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector3i& size, Int levels, Int samples = 1, ImageLayout initialLayout = ImageLayout::Undefined, Flags flags = {}): ImageCreateInfo{VK_IMAGE_TYPE_2D, usages, format, {size.xy(), 1}, size.z(), levels, samples, initialLayout, flags|Flag::CubeCompatible} {}

        /** @overload
         *
         * Equivalent to the above with @p initialLayout set to
         * @ref ImageLayout::Undefined.
         */
        explicit ImageCreateInfoCubeMapArray(ImageUsages usages, PixelFormat format, const Vector3i& size, Int levels, Int samples, Flags flags): ImageCreateInfoCubeMapArray{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfoCubeMapArray(ImageUsages usages, Magnum::PixelFormat format, const Vector3i& size, Int levels, Int samples, Flags flags): ImageCreateInfoCubeMapArray{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
        /** @overload */
        explicit ImageCreateInfoCubeMapArray(ImageUsages usages, Magnum::CompressedPixelFormat format, const Vector3i& size, Int levels, Int samples, Flags flags): ImageCreateInfoCubeMapArray{usages, format, size, levels, samples, ImageLayout::Undefined, flags} {}
};

}}

/* Image.h included at the top */

#endif
