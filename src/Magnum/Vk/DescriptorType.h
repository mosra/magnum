#ifndef Magnum_Vk_DescriptorType_h
#define Magnum_Vk_DescriptorType_h
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
 * @brief Enum @ref Magnum::Vk::DescriptorType
 * @m_since_latest
 */

#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Descriptor type
@m_since_latest

Wraps @type_vk_keyword{DescriptorType}.
@see @ref DescriptorSetLayoutBinding, @ref DescriptorPoolCreateInfo
@m_enum_values_as_keywords
*/
enum class DescriptorType: Int {
    /**
     * @ref Sampler.
     *
     * @see @ref DescriptorType::CombinedImageSampler
     */
    Sampler = VK_DESCRIPTOR_TYPE_SAMPLER,

    /**
     * @ref Sampler combined with an @ref Image.
     *
     * @m_class{m-note m-success}
     *
     * @par
     *      On some implementations it may be more efficient to sample from an
     *      a combined image sampler than a separate
     *      @ref DescriptorType::Sampler and @ref DescriptorType::SampledImage.
     *
     * The image is expected to have been created with @ref ImageUsage::Sampled
     * and be in either @ref ImageLayout::General or
     * @ref ImageLayout::ShaderReadOnly.
     */
    CombinedImageSampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,

    /**
     * Sampled @ref Image.
     *
     * The image is expected to have been created with @ref ImageUsage::Sampled
     * and be in either @ref ImageLayout::General or
     * @ref ImageLayout::ShaderReadOnly.
     * @see @ref DescriptorType::CombinedImageSampler
     */
    SampledImage = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,

    /**
     * Storage @ref Image.
     *
     * The image is expected to have been created with @ref ImageUsage::Storage
     * and be in @ref ImageLayout::General.
     */
    StorageImage = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,

    /**
     * Uniform texel buffer view.
     *
     * The buffer is expected to have been created with
     * @ref BufferUsage::UniformTexelBuffer.
     */
    UniformTexelBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,

    /**
     * Storage texel buffer view.
     *
     * The buffer is expected to have been created with
     * @ref BufferUsage::StorageTexelBuffer.
     */
    StorageTexelBuffer = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,

    /**
     * Uniform @ref Buffer.
     *
     * The buffer is expected to have been created with
     * @ref BufferUsage::UniformBuffer.
     */
    UniformBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,

    /**
     * Storage @ref Buffer.
     *
     * The buffer is expected to have been created with
     * @ref BufferUsage::StorageBuffer.
     */
    StorageBuffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,

    /**
     * Uniform @ref Buffer with a dynamic offset.
     *
     * The buffer is expected to have been created with
     * @ref BufferUsage::UniformBuffer.
     */
    UniformBufferDynamic = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,

    /**
     * Storage @ref Buffer with a dynamic offset.
     *
     * The buffer is expected to have been created with
     * @ref BufferUsage::StorageBuffer.
     */
    StorageBufferDynamic = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,

    /**
     * Input @ref Image attachment.
     *
     * The image is expected to have been created with
     * @ref ImageUsage::InputAttachment and be in @ref ImageLayout::General.
     */
    InputAttachment = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,

    /**
     * Acceleration structure.
     *
     * @requires_vk_feature @ref DeviceFeature::AccelerationStructure
     */
    AccelerationStructure = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR
};

/**
@debugoperatorenum{DescriptorType}
@m_since_latest
*/
MAGNUM_VK_EXPORT Debug& operator<<(Debug& debug, DescriptorType value);

}}

#endif
