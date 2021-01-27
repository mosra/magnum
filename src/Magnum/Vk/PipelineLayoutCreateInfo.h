#ifndef Magnum_Vk_PipelineLayoutCreateInfo_h
#define Magnum_Vk_PipelineLayoutCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::PipelineLayoutCreateInfo
 * @m_since_latest
 */

#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Pipeline layout creation info
@m_since_latest

Wraps a @type_vk_keyword{PipelineLayoutCreateInfo}.
*/
class MAGNUM_VK_EXPORT PipelineLayoutCreateInfo {
    public:
        /**
         * @brief Constructor
         *
         * The following @type_vk{PipelineLayoutCreateInfo} fields are
         * pre-filled in addition to `sType`, everything else is zero-filled:
         *
         * -    <em>(none)</em>
         */
        explicit PipelineLayoutCreateInfo();

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit PipelineLayoutCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit PipelineLayoutCreateInfo(const VkPipelineLayoutCreateInfo& info);

        /** @brief Underlying @type_vk{PipelineLayoutCreateInfo} structure */
        VkPipelineLayoutCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkPipelineLayoutCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkPipelineLayoutCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkPipelineLayoutCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkPipelineLayoutCreateInfo*() const { return &_info; }

    private:
        VkPipelineLayoutCreateInfo _info;
};

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/PipelineLayout.h"

#endif
