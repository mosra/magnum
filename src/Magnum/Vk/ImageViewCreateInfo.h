#ifndef Magnum_Vk_ImageViewCreateInfo_h
#define Magnum_Vk_ImageViewCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::ImageViewCreateInfo, @ref Magnum::Vk::ImageViewCreateInfo1D, @ref Magnum::Vk::ImageViewCreateInfo2D, @ref Magnum::Vk::ImageViewCreateInfo3D, @ref Magnum::Vk::ImageViewCreateInfo1DArray, @ref Magnum::Vk::ImageViewCreateInfo2DArray, @ref Magnum::Vk::ImageViewCreateInfoCubeMap, @ref Magnum::Vk::ImageViewCreateInfoCubeMapArray
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Image view creation info
@m_since_latest

Wraps a @type_vk_keyword{ImageViewCreateInfo}. See
@ref Vk-ImageView-creation "Image creation" for usage information.
@see @ref ImageViewCreateInfo1D, @ref ImageViewCreateInfo2D,
    @ref ImageViewCreateInfo3D, @ref ImageViewCreateInfo1DArray,
    @ref ImageViewCreateInfo2DArray, @ref ImageViewCreateInfoCubeMap,
    @ref ImageViewCreateInfoCubeMapArray
*/
class MAGNUM_VK_EXPORT ImageViewCreateInfo {
    public:
        /**
         * @brief Image view creation flag
         *
         * Wraps @type_vk_keyword{ImageViewCreateFlagBits}.
         * @see @ref Flags, @ref ImageViewCreateInfo(VkImageViewType, VkImage, PixelFormat, UnsignedInt, UnsignedInt, UnsignedInt, UnsignedInt, Flags)
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /** @todo @vk_extension{EXT,fragment_density_map}, @vk_extension{EXT,fragment_density_map2} */
        };

        /**
         * @brief ImageView creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{ImageViewCreateFlags}.
         * @see @ref ImageViewCreateInfo(VkImageViewType, VkImage, PixelFormat, UnsignedInt, UnsignedInt, UnsignedInt, UnsignedInt, Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param type          Image view type
         * @param image         Image to create a view on
         * @param format        View format
         * @param layerOffset   Layer offset
         * @param layerCount    Layer count
         * @param levelOffset   Mip level offset
         * @param levelCount    Mip level count
         * @param flags         Image view creation flags
         *
         * The following @type_vk{ImageViewCreateInfo} are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `image`
         * -    `viewType` to @p type
         * -    `format`
         * -    `subresourceRange.aspectMask` to @ref ImageAspect::Depth /
         *      @ref ImageAspect::Stencil if @p format is depth / stencil or
         *      both and @ref ImageAspect::Color otherwise
         * -    `subresourceRange.levelOffset` to @p levelOffset
         * -    `subresourceRange.levelCount` to @p levelCount
         * -    `subresourceRange.baseArrayLayer` to @p layerOffset
         * -    `subresourceRange.layerCount` to @p layerCount
         *
         * The @p type and @p format has to be compatible with @p image and
         * the mip level and layer sub-range has to be in bounds. For creating
         * a view that exactly matches the image type and format, use the
         * @ref ImageViewCreateInfo(VkImageViewType, Image&, UnsignedInt, UnsignedInt, UnsignedInt, UnsignedInt, Flags)
         * constructor. Otherwise, similarly as with @ref ImageCreateInfo, for
         * common view types you're encouraged to make use of
         * @ref ImageViewCreateInfo1D, @ref ImageViewCreateInfo2D,
         * @ref ImageViewCreateInfo3D, @ref ImageViewCreateInfo1DArray,
         * @ref ImageViewCreateInfo2DArray, @ref ImageViewCreateInfoCubeMap and
         * @ref ImageViewCreateInfoCubeMapArray convenience classes instead of
         * this constructor.
         */
        explicit ImageViewCreateInfo(VkImageViewType type, VkImage image, PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {});
        /** @overload */
        explicit ImageViewCreateInfo(VkImageViewType type, VkImage image, Magnum::PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {});
        /** @overload */
        explicit ImageViewCreateInfo(VkImageViewType type, VkImage image, Magnum::CompressedPixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {});

        /**
         * @brief Construct with format matching given image
         *
         * Compared to @ref ImageViewCreateInfo(VkImageViewType, VkImage, PixelFormat, UnsignedInt, UnsignedInt, UnsignedInt, UnsignedInt, Flags)
         * the @p format is taken from @ref Image::format().
         */
        explicit ImageViewCreateInfo(VkImageViewType type, Image& image, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit ImageViewCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit ImageViewCreateInfo(const VkImageViewCreateInfo& info);

        /** @brief Underlying @type_vk{BufferCreateInfo} structure */
        VkImageViewCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkImageViewCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkImageViewCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkImageViewCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkImageViewCreateInfo*() const { return &_info; }

    private:
        VkImageViewCreateInfo _info;
};

CORRADE_ENUMSET_OPERATORS(ImageViewCreateInfo::Flags)

/**
@brief Convenience constructor for 1D image views
@m_since_latest

Compared to the base @ref ImageViewCreateInfo constructor creates a view of
type @val_vk{IMAGE_VIEW_TYPE_1D,ImageViewType} with @p layerCount set to
@cpp 1 @ce. The original image is expected to be a
@val_vk{IMAGE_TYPE_1D,ImageType} (e.g., created using @ref ImageCreateInfo1D or
@ref ImageCreateInfo1DArray).
*/
class ImageViewCreateInfo1D: public ImageViewCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageViewCreateInfo1D(VkImage image, PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_1D, image, format, layerOffset, 1, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfo1D(VkImage image, Magnum::PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_1D, image, format, layerOffset, 1, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfo1D(VkImage image, Magnum::CompressedPixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_1D, image, format, layerOffset, 1, levelOffset, levelCount, flags} {}

        /** @overload
         *
         * Compared to the above, @p format is taken from @ref Image::format().
         */
        explicit ImageViewCreateInfo1D(Image& image, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_1D, image, layerOffset, 1, levelOffset, levelCount, flags} {}
};

/**
@brief Convenience constructor for 2D image views
@m_since_latest

Compared to the base @ref ImageViewCreateInfo constructor creates a view of
type @val_vk{IMAGE_VIEW_TYPE_2D,ImageViewType} with @p layerCount set to
@cpp 1 @ce. The original image is expected to be a
@val_vk{IMAGE_TYPE_2D,ImageType} (e.g., created using @ref ImageCreateInfo2D,
@ref ImageCreateInfo2DArray, @ref ImageCreateInfoCubeMap or
@ref ImageCreateInfoCubeMapArray).
@todo also mention 3D once we have Flag::2DArrayCompatible
    (@vk_extension{KHR,maintenance1}) exposed, restricted additionally with
    @vk_extension{KHR,portability_subset}
*/
class ImageViewCreateInfo2D: public ImageViewCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageViewCreateInfo2D(VkImage image, PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_2D, image, format, layerOffset, 1, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfo2D(VkImage image, Magnum::PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_2D, image, format, layerOffset, 1, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfo2D(VkImage image, Magnum::CompressedPixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_2D, image, format, layerOffset, 1, levelOffset, levelCount, flags} {}

        /** @overload
         *
         * Compared to the above, @p format is taken from @ref Image::format().
         */
        explicit ImageViewCreateInfo2D(Image& image, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_2D, image, layerOffset, 1, levelOffset, levelCount, flags} {}
};

/**
@brief Convenience constructor for 3D image views
@m_since_latest

Compared to the base @ref ImageViewCreateInfo constructor creates a view of
type @val_vk{IMAGE_VIEW_TYPE_3D,ImageViewType} with @p layerCount set to
@cpp 1 @ce. The original image is expected to be a
@val_vk{IMAGE_TYPE_2D,ImageType} (e.g., created using @ref ImageCreateInfo3D).
*/
class ImageViewCreateInfo3D: public ImageViewCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageViewCreateInfo3D(VkImage image, PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_3D, image, format, layerOffset, 1, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfo3D(VkImage image, Magnum::PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_3D, image, format, layerOffset, 1, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfo3D(VkImage image, Magnum::CompressedPixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_3D, image, format, layerOffset, 1, levelOffset, levelCount, flags} {}

        /** @overload
         *
         * Compared to the above, @p format is taken from @ref Image::format().
         */
        explicit ImageViewCreateInfo3D(Image& image, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_3D, image, layerOffset, 1, levelOffset, levelCount, flags} {}
};

/**
@brief Convenience constructor for 1D array image views
@m_since_latest

Compared to the base @ref ImageViewCreateInfo constructor creates a view of
type @val_vk{IMAGE_VIEW_TYPE_1D_ARRAY,ImageViewType}. The original image is
expected to be a @val_vk{IMAGE_TYPE_1D,ImageType} (e.g., created using
@ref ImageCreateInfo1D or @ref ImageCreateInfo1DArray) with enough array layers
to fit the required subrange.
*/
class ImageViewCreateInfo1DArray: public ImageViewCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageViewCreateInfo1DArray(VkImage image, PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_1D_ARRAY, image, format, layerOffset, layerCount, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfo1DArray(VkImage image, Magnum::PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_1D_ARRAY, image, format, layerOffset, layerCount, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfo1DArray(VkImage image, Magnum::CompressedPixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_1D_ARRAY, image, format, layerOffset, layerCount, levelOffset, levelCount, flags} {}

        /** @overload
         *
         * Compared to the above, @p format is taken from @ref Image::format().
         */
        explicit ImageViewCreateInfo1DArray(Image& image, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_1D_ARRAY, image, layerOffset, layerCount, levelOffset, levelCount, flags} {}
};

/**
@brief Convenience constructor for 2D array image views
@m_since_latest

Compared to the base @ref ImageViewCreateInfo constructor creates a view of
type @val_vk{IMAGE_VIEW_TYPE_2D_ARRAY,ImageViewType}. The original image is
expected to be a @val_vk{IMAGE_TYPE_2D,ImageType} (e.g., created using
@ref ImageCreateInfo2D, @ref ImageCreateInfoCubeMap or
@ref ImageCreateInfoCubeMapArray) with enough array layers to fit the required
subrange.
@todo also mention 3D once we have Flag::2DArrayCompatible
    (@vk_extension{KHR,maintenance1}) exposed, restricted additionally with
    @vk_extension{KHR,portability_subset}
*/
class ImageViewCreateInfo2DArray: public ImageViewCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageViewCreateInfo2DArray(VkImage image, PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_2D_ARRAY, image, format, layerOffset, layerCount, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfo2DArray(VkImage image, Magnum::PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_2D_ARRAY, image, format, layerOffset, layerCount, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfo2DArray(VkImage image, Magnum::CompressedPixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_2D_ARRAY, image, format, layerOffset, layerCount, levelOffset, levelCount, flags} {}

        /** @overload
         *
         * Compared to the above, @p format is taken from @ref Image::format().
         */
        explicit ImageViewCreateInfo2DArray(Image& image, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_2D_ARRAY, image, layerOffset, layerCount, levelOffset, levelCount, flags} {}
};

/**
@brief Convenience constructor for cube map image views
@m_since_latest

Compared to the base @ref ImageViewCreateInfo constructor creates a view of
type @val_vk{IMAGE_VIEW_TYPE_CUBE,ImageViewType} with @p layerCount set to
@cpp 6 @ce. The original image is expected to be a
@val_vk{IMAGE_TYPE_2D,ImageType} with @ref ImageCreateInfo::Flag::CubeCompatible
set (e.g., created using @ref ImageCreateInfoCubeMap or
@ref ImageCreateInfoCubeMapArray).
*/
class ImageViewCreateInfoCubeMap: public ImageViewCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageViewCreateInfoCubeMap(VkImage image, PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_CUBE, image, format, layerOffset, 6, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfoCubeMap(VkImage image, Magnum::PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_CUBE, image, format, layerOffset, 6, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfoCubeMap(VkImage image, Magnum::CompressedPixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_CUBE, image, format, layerOffset, 6, levelOffset, levelCount, flags} {}

        /** @overload
         *
         * Compared to the above, @p format is taken from @ref Image::format().
         */
        explicit ImageViewCreateInfoCubeMap(Image& image, UnsignedInt layerOffset = 0, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_CUBE, image, layerOffset, 6, levelOffset, levelCount, flags} {}
};

/**
@brief Convenience constructor for cube map array image views
@m_since_latest

Compared to the base @ref ImageViewCreateInfo constructor creates a view of
type @val_vk{IMAGE_VIEW_TYPE_CUBE_ARRAY,ImageViewType}. The original image is
expected to be a @val_vk{IMAGE_TYPE_2D,ImageType} with
@ref ImageCreateInfo::Flag::CubeCompatible set (e.g., created using
@ref ImageCreateInfoCubeMap or @ref ImageCreateInfoCubeMapArray) with enough
array layers to fit the required subrange.
@requires_vk_feature @ref DeviceFeature::ImageCubeArray
*/
class ImageViewCreateInfoCubeMapArray: public ImageViewCreateInfo {
    public:
        /** @brief Constructor */
        explicit ImageViewCreateInfoCubeMapArray(VkImage image, PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_CUBE_ARRAY, image, format, layerOffset, layerCount, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfoCubeMapArray(VkImage image, Magnum::PixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_CUBE_ARRAY, image, format, layerOffset, layerCount, levelOffset, levelCount, flags} {}
        /** @overload */
        explicit ImageViewCreateInfoCubeMapArray(VkImage image, Magnum::CompressedPixelFormat format, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_CUBE_ARRAY, image, format, layerOffset, layerCount, levelOffset, levelCount, flags} {}

        /** @overload
         *
         * Compared to the above, @p format is taken from @ref Image::format().
         */
        explicit ImageViewCreateInfoCubeMapArray(Image& image, UnsignedInt layerOffset = 0, UnsignedInt layerCount = VK_REMAINING_ARRAY_LAYERS, UnsignedInt levelOffset = 0, UnsignedInt levelCount = VK_REMAINING_MIP_LEVELS, Flags flags = {}): ImageViewCreateInfo{VK_IMAGE_VIEW_TYPE_CUBE_ARRAY, image, layerOffset, layerCount, levelOffset, levelCount, flags} {}
};

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/ImageView.h"

#endif
