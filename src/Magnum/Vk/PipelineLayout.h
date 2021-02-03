#ifndef Magnum_Vk_PipelineLayout_h
#define Magnum_Vk_PipelineLayout_h
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
 * @brief Class @ref Magnum::Vk::PipelineLayout
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
@brief Pipeline layout
@m_since_latest

Wraps a @type_vk_keyword{PipelineLayout}. Used in a @ref Pipeline.
*/
class MAGNUM_VK_EXPORT PipelineLayout {
    public:
        /**
         * @brief Wrap existing Vulkan handle
         * @param device            Vulkan device the pipeline layout is
         *      created on
         * @param handle            The @type_vk{PipelineLayout} handle
         * @param flags             Handle flags
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * a pipeline layout created using a constructor, the Vulkan pipeline
         * layout is by default not deleted on destruction, use @p flags for
         * different behavior.
         * @see @ref release()
         */
        static PipelineLayout wrap(Device& device, VkPipelineLayout handle, HandleFlags flags = {});

        /**
         * @brief Constructor
         * @param device    Vulkan device to create the pipeline layout on
         * @param info      Pipeline layout creation info
         *
         * @see @fn_vk_keyword{CreatePipelineLayout}
         */
        explicit PipelineLayout(Device& device, const PipelineLayoutCreateInfo& info);

        /**
         * @brief Construct without creating the pipeline layout
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit PipelineLayout(NoCreateT);

        /** @brief Copying is not allowed */
        PipelineLayout(const PipelineLayout&) = delete;

        /** @brief Move constructor */
        PipelineLayout(PipelineLayout&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{PipelineLayout} handle, unless the
         * instance was created using @ref wrap() without
         * @ref HandleFlag::DestroyOnDestruction specified.
         * @see @fn_vk_keyword{DestroyPipelineLayout}, @ref release()
         */
        ~PipelineLayout();

        /** @brief Copying is not allowed */
        PipelineLayout& operator=(const PipelineLayout&) = delete;

        /** @brief Move assignment */
        PipelineLayout& operator=(PipelineLayout&& other) noexcept;

        /** @brief Underlying @type_vk{PipelineLayout} handle */
        VkPipelineLayout handle() { return _handle; }
        /** @overload */
        operator VkPipelineLayout() { return _handle; }

        /** @brief Handle flags */
        HandleFlags handleFlags() const { return _flags; }

        /**
         * @brief Release the underlying Vulkan pipeline layout
         *
         * Releases ownership of the Vulkan pipeline layout and returns its
         * handle so @fn_vk{DestroyPipelineLayout} is not called on
         * destruction. The internal state is then equivalent to moved-from
         * state.
         * @see @ref wrap()
         */
        VkPipelineLayout release();

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkPipelineLayout _handle;
        HandleFlags _flags;
};

}}

#endif
