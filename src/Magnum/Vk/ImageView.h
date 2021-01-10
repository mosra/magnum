#ifndef Magnum_Vk_ImageView_h
#define Magnum_Vk_ImageView_h
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
 * @brief Class @ref Magnum::Vk::ImageView
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Image view
@m_since_latest

View onto an @ref Image. Wraps a @type_vk_keyword{ImageView}.

@section Vk-ImageView-creation Image view creation

Similarly to @ref Image, a view is created by passing one of the
@ref ImageViewCreateInfo subclasses with desired layer/level range to the
@ref Image constructor. Commonly you'd use the same @ref ImageViewCreateInfo
subclass as @ref ImageCreateInfo, but other combinations are possible as well
--- see docs of each subclass for more information.

@snippet MagnumVk.cpp ImageView-creation

While it would be *technically* possible to infer a view type from the
originating @ref Image and thus avoid having to specify the type twice, the
mapping isn't crystal clear in all cases and could cause hard-to-detect issues
if you suddenly get an unexpected view type. Thus the view type is required to
be always explicitly specified.
*/
class MAGNUM_VK_EXPORT ImageView {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device            Vulkan device the image view is created on
         * @param handle            The @type_vk{ImageView} handle
         * @param flags             Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * an image view created using a constructor, the Vulkan image view is
         * by default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static ImageView wrap(Device& device, VkImageView handle, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param device    Vulkan device to create the image on
         * @param info      Image creation info
         *
         * @see @fn_vk_keyword{CreateImageView}
         */
        explicit ImageView(Device& device, const ImageViewCreateInfo& info);

        /**
         * @brief Construct without creating the image
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit ImageView(NoCreateT);

        /** @brief Copying is not allowed */
        ImageView(const ImageView&) = delete;

        /** @brief Move constructor */
        ImageView(ImageView&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{ImageView} handle, unless the instance
         * was created using @ref wrap() without
         * @ref HandleFlag::DestroyOnDestruction specified.
         * @see @fn_vk_keyword{DestroyImageView}, @ref release()
         */
        ~ImageView();

        /** @brief Copying is not allowed */
        ImageView& operator=(const ImageView&) = delete;

        /** @brief Move assignment */
        ImageView& operator=(ImageView&& other) noexcept;

        /** @brief Underlying @type_vk{ImageView} handle */
        VkImageView handle() { return _handle; }
        /** @overload */
        operator VkImageView() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Release the underlying Vulkan image view
         *
         * Releases ownership of the Vulkan image view and returns its handle
         * so @fn_vk{DestroyImageView} is not called on destruction. The
         * internal state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkImageView release();

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkImageView _handle;
        HandleFlags _flags;
};

}}

#endif
