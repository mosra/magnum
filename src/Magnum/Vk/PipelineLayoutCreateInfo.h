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

#include <Corrade/Containers/ArrayTuple.h>

#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Pipeline layout creation info
@m_since_latest

Wraps a @type_vk_keyword{PipelineLayoutCreateInfo}. See
@ref Vk-PipelineLayout-creation "Pipeline layout creation" for usage
information.
*/
class MAGNUM_VK_EXPORT PipelineLayoutCreateInfo {
    public:
        /* VkPipelineLayoutCreateFlagBits is currently empty, so no reason
           to expose */

        /**
         * @brief Constructor
         * @param descriptorSetLayouts  Descriptor set layouts used in this
         *      pipeline layout
         *
         * The following @type_vk{PipelineLayoutCreateInfo} fields are
         * pre-filled in addition to `sType`, everything else is zero-filled:
         *
         * -    `setLayoutCount` and `pSetLayouts` to a copy of
         *      @p descriptorSetLayouts
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit PipelineLayoutCreateInfo(Containers::ArrayView<const VkDescriptorSetLayout> descriptorSetLayouts = {});
        #else /* so we don't need to include ArrayView */
        explicit PipelineLayoutCreateInfo(Containers::ArrayView<const VkDescriptorSetLayout> descriptorSetLayouts);
        explicit PipelineLayoutCreateInfo();
        #endif
        /** @overload */
        explicit PipelineLayoutCreateInfo(std::initializer_list<VkDescriptorSetLayout> descriptorSetLayouts);

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

        /** @brief Copying is not allowed */
        PipelineLayoutCreateInfo(const PipelineLayoutCreateInfo&) = delete;

        /** @brief Move constructor */
        PipelineLayoutCreateInfo(PipelineLayoutCreateInfo&& other) noexcept;

        ~PipelineLayoutCreateInfo();

        /** @brief Copying is not allowed */
        PipelineLayoutCreateInfo& operator=(const PipelineLayoutCreateInfo&) = delete;

        /** @brief Move assignment */
        PipelineLayoutCreateInfo& operator=(PipelineLayoutCreateInfo&& other) noexcept;

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
        Containers::ArrayTuple _data;
};

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/PipelineLayout.h"

#endif
