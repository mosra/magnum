#ifndef Magnum_Vk_ComputePipelineCreateInfo_h
#define Magnum_Vk_ComputePipelineCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::ComputePipelineCreateInfo
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Compute pipeline creation info
@m_since_latest

Wraps a @type_vk_keyword{ComputePipelineCreateInfo}. See
@ref Vk-Pipeline-creation-compute "Compute pipeline creation" for usage
information.
@see @ref RasterizationPipelineCreateInfo
*/
class MAGNUM_VK_EXPORT ComputePipelineCreateInfo {
    public:
        /**
         * @brief Compute pipeline creation flag
         *
         * Wraps the compute-related subset of
         * @type_vk_keyword{PipelineCreateFlagBits}.
         * @see @ref Flags, @ref ComputePipelineCreateInfo(const ShaderSet&, VkPipelineLayout, Flags)
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /**
             * Create the pipeline without optimization.
             *
             * @m_class{m-note m-success}
             *
             * @par
             *      Setting this flag on single-use pipelines might help
             *      drivers pick a better tradeoff between CPU time spent
             *      optimizing the pipeline and GPU time spent executing it.
             */
            DisableOptimization = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT,

            /**
             * Allow derivatives to be subsequently created from this pipeline.
             */
            AllowDerivatives = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT,

            /** Derivative of a pipeline created earlier. */
            Derivative = VK_PIPELINE_CREATE_DERIVATIVE_BIT
        };

        /**
         * @brief Compite pipeline creation flags
         *
         * Type-safe wrapper for the compute-related subset of
         * @type_vk_keyword{PipelineCreateFlags}.
         * @see @ref ComputePipelineCreateInfo(const ShaderSet&, VkPipelineLayout, Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param shaderSet         Shader to use for this pipeline. Expected
         *      to have just one entry.
         * @param pipelineLayout    A @ref PipelineLayout or a raw Vulkan
         *      pipeline layout handle
         * @param flags             Compute pipeline creation flags
         *
         * Note that the @p shaderSet structure internals are referenced, not
         * copied, and thus have to stay in scope until the @ref Pipeline
         * object is created.
         *
         * The following @type_vk{ComputePipelineCreateInfo} fields are
         * pre-filled in addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `stage` to @p shaderSet
         * -    `layout` to @p pipelineLayout
         */
        explicit ComputePipelineCreateInfo(const ShaderSet& shaderSet, VkPipelineLayout pipelineLayout, Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field nor the nested structure
         * pointers are set --- the structure has to be fully initialized
         * afterwards in order to be usable.
         */
        explicit ComputePipelineCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit ComputePipelineCreateInfo(const VkComputePipelineCreateInfo& info);

        /** @brief Underlying @type_vk{GraphicsPipelineCreateInfo} structure */
        VkComputePipelineCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkComputePipelineCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkComputePipelineCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkComputePipelineCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkComputePipelineCreateInfo*() const { return &_info; }

    private:
        VkComputePipelineCreateInfo _info;
};

CORRADE_ENUMSET_OPERATORS(ComputePipelineCreateInfo::Flags)

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/Pipeline.h"

#endif
