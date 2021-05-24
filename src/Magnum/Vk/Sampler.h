#ifndef Magnum_Vk_Sampler_h
#define Magnum_Vk_Sampler_h
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
 * @brief Class @ref Magnum::Vk::Sampler
 * @m_since_latest
 */

#include "Magnum/Tags.h"
#include "Magnum/Vk/Handle.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Sampler
@m_since_latest

Wraps a @type_vk_keyword{Sampler}, which describes how shaders perform
texturing.

@section Vk-Sampler-creation Sampler creation

The default-constructed @ref SamplerCreateInfo uses a conservative setup with
nearest neighbor filtering and will produce valid results with no need to set
up anything else:

@snippet MagnumVk.cpp Sampler-creation

Usually, however, you'll want to set up filtering and mip level selection at
least, along with other properties:

@snippet MagnumVk.cpp Sampler-creation-linear

*/
class MAGNUM_VK_EXPORT Sampler {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device            Vulkan device the sampler is created on
         * @param handle            The @type_vk{Sampler} handle
         * @param flags             Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * a sampler created using a constructor, the Vulkan sampler is by
         * default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        static Sampler wrap(Device& device, VkSampler handle, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param device    Vulkan device to create the sampler on
         * @param info      Sampler creation info
         *
         * @see @fn_vk_keyword{CreateSampler}
         */
        explicit Sampler(Device& device, const SamplerCreateInfo& info);

        /**
         * @brief Construct without creating the sampler
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Sampler(NoCreateT);

        /** @brief Copying is not allowed */
        Sampler(const Sampler&) = delete;

        /** @brief Move constructor */
        Sampler(Sampler&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{Sampler} handle, unless the instance
         * was created using @ref wrap() without
         * @ref HandleFlag::DestroyOnDestruction specified.
         * @see @fn_vk_keyword{DestroySampler}, @ref release()
         */
        ~Sampler();

        /** @brief Copying is not allowed */
        Sampler& operator=(const Sampler&) = delete;

        /** @brief Move assignment */
        Sampler& operator=(Sampler&& other) noexcept;

        /** @brief Underlying @type_vk{Sampler} handle */
        VkSampler handle() { return _handle; }
        /** @overload */
        operator VkSampler() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Release the underlying Vulkan sampler
         *
         * Releases ownership of the Vulkan sampler and returns its handle so
         * @fn_vk{DestroySampler} is not called on destruction. The internal
         * state is then equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkSampler release();

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkSampler _handle;
        HandleFlags _flags;
};

}}

#endif
