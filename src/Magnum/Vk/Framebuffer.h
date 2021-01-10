#ifndef Magnum_Vk_Framebuffer_h
#define Magnum_Vk_Framebuffer_h
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
 * @brief Class @ref Magnum::Vk::Framebuffer
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
@brief Framebuffer
@m_since_latest

Wraps a @type_vk_keyword{Framebuffer}, which connects a @ref RenderPass
together with concrete @ref ImageView "ImageViews" for attachments.

@section Vk-Framebuffer-creation Framebuffer creation

A framebuffer is created using @ref FramebufferCreateInfo that takes a
previously-created @ref RenderPass together with @ref ImageView "ImageViews"
onto @ref Image "Images" of desired sizes and compatible formats for all its attachments:

@snippet MagnumVk.cpp Framebuffer-creation
*/
class MAGNUM_VK_EXPORT Framebuffer {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device            Vulkan device the framebuffer is created on
         * @param handle            The @type_vk{Framebuffer} handle
         * @param size              Width, height and layer count of the
         *      framebuffer. Available through @ref size() afterwards.
         * @param flags             Handle flags
         *
         * The @p handle is expected to be originating from @p device. The
         * @p size parameter is used for convenience @ref RenderPass recording
         * later. If it's unknown, pass a default-constructed value --- you
         * will then be able to only being a render pass by specifying a
         * concrete size in @ref RenderPassBeginInfo.
         *
         * Unlike a framebuffer created using a constructor, the Vulkan
         * framebuffer is by default not deleted on destruction, use @p flags
         * for different behavior.
         * @see @ref release()
         */
        static Framebuffer wrap(Device& device, VkFramebuffer handle, const Vector3i& size, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param device    Vulkan device to create the framebuffer on
         * @param info      Framebuffer creation info
         *
         * @see @fn_vk_keyword{CreateFramebuffer}
         */
        explicit Framebuffer(Device& device, const FramebufferCreateInfo& info);

        /**
         * @brief Construct without creating the framebuffer
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Framebuffer(NoCreateT);

        /** @brief Copying is not allowed */
        Framebuffer(const Framebuffer&) = delete;

        /** @brief Move constructor */
        Framebuffer(Framebuffer&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{Framebuffer} handle, unless the
         * instance was created using @ref wrap() without
         * @ref HandleFlag::DestroyOnDestruction specified.
         * @see @fn_vk_keyword{DestroyFramebuffer}, @ref release()
         */
        ~Framebuffer();

        /** @brief Copying is not allowed */
        Framebuffer& operator=(const Framebuffer&) = delete;

        /** @brief Move assignment */
        Framebuffer& operator=(Framebuffer&& other) noexcept;

        /** @brief Underlying @type_vk{Framebuffer} handle */
        VkFramebuffer handle() { return _handle; }
        /** @overload */
        operator VkFramebuffer() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /** @brief Framebuffer size */
        Vector3i size() const;

        /**
         * @brief Release the underlying Vulkan framebuffer
         *
         * Releases ownership of the Vulkan framebuffer and returns its handle
         * so @fn_vk{DestroyFramebuffer} is not called on destruction. The
         * internal state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkFramebuffer release();

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkFramebuffer _handle;
        HandleFlags _flags;
        /* This is probably extremely stupid and will fire back later,
           nevertheless -- on 64bit there's 7 padding bytes after flags, which
           we can reuse to store framebuffer size. According to gpuinfo.org,
           maxFramebufferWidth/Height is 32768 in late 2020, which fits into
           16 bits, and a framebuffer of that size is 4 GB of memory. I don't
           expect this growing over 64k (16 GB) anytime soon.

           Additionally (which is probably also stupid), this is not using
           Vector3us but instead a plain array to avoid the include
           dependency. */
        UnsignedShort _size[3];
};

}}

#endif
