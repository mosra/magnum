#ifndef Magnum_Vk_BufferCreateInfo_h
#define Magnum_Vk_BufferCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::BufferCreateInfo, enum @ref Magnum::Vk::BufferUsage, enum set @ref Magnum::Vk::BufferUsages
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
@brief Buffer usage
@m_since_latest

Wraps a @type_vk_keyword{BufferUsageFlagBits}.
@see @ref BufferUsages, @ref BufferCreateInfo
@m_enum_values_as_keywords
*/
enum class BufferUsage: UnsignedInt {
    /**
     * Source of a transfer command.
     * @see @ref CommandBuffer::copyBuffer(),
     *      @ref CommandBuffer::copyBufferToImage()
     */
    TransferSource = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,

    /**
     * Destination of a transfer command.
     * @see @ref CommandBuffer::fillBuffer(),
     *      @ref CommandBuffer::copyBuffer(),
     *      @ref CommandBuffer::copyImageToBuffer()
     */
    TransferDestination = VK_BUFFER_USAGE_TRANSFER_DST_BIT,

    /**
     * Suitable for creating a uniform texel buffer view.
     * @see @ref DescriptorType::UniformTexelBuffer
     */
    UniformTexelBuffer = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,

    /**
     * Suitable for creating a storage texel buffer view.
     * @see @ref DescriptorType::StorageTexelBuffer
     */
    StorageTexelBuffer = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,

    /**
     * Suitable for a uniform buffer.
     * @see @ref DescriptorType::UniformBuffer
     */
    UniformBuffer = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,

    /**
     * Suitable for a storage buffer.
     * @see @ref DescriptorType::StorageBuffer
     */
    StorageBuffer = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,

    /**
     * Suitable for an index buffer.
     * @see @ref Mesh::setIndexBuffer()
     */
    IndexBuffer = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,

    /**
     * Suitable for a vertex buffer.
     * @see @ref Mesh::addVertexBuffer()
     */
    VertexBuffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,

    /** Suitable for a indirect draw buffer */
    IndirectBuffer = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,

    /** @todo VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, 1.2 */

    /**
     * Suitable for a ray tracing shader binding table
     * @requires_vk_feature @ref DeviceFeature::RayTracingPipeline
     */
    ShaderBindingTable = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,

    /**
     * Suitable as a read-only input to an acceleration structure build
     * @requires_vk_feature @ref DeviceFeature::AccelerationStructure
     */
    AccelerationStructureBuildInputReadOnly = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,

    /**
     * Suitable for an acceleration structure storage space
     * @requires_vk_feature @ref DeviceFeature::AccelerationStructure
     */
    AccelerationStructureStorage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR
};

/**
@brief Buffer usages
@m_since_latest

Type-safe wrapper for @type_vk_keyword{BufferUsageFlags}.
@see @ref BufferCreateInfo
*/
typedef Containers::EnumSet<BufferUsage> BufferUsages;

CORRADE_ENUMSET_OPERATORS(BufferUsages)

/**
@brief Buffer creation info
@m_since_latest

Wraps a @type_vk_keyword{BufferCreateInfo}. See
@ref Vk-Buffer-creation "Buffer creation" for usage information.
*/
class MAGNUM_VK_EXPORT BufferCreateInfo {
    public:
        /**
         * @brief Buffer creation flag
         *
         * Wraps @type_vk_keyword{BufferCreateFlagBits}.
         * @see @ref Flags, @ref BufferCreateInfo()
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /** @todo sparse binding, protected ... */
        };

        /**
         * @brief Buffer creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{BufferCreateFlags}.
         * @see @ref BufferCreateInfo()
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param usages    Desired buffer usage. At least one flag is
         *      required.
         * @param size      Buffer size
         * @param flags     Buffer creation flags
         *
         * The following @type_vk{BufferCreateInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         * -    `size`
         * -    `usage` to @p usages
         * -    `sharingMode` to @val_vk{SHARING_MODE_EXCLUSIVE,SharingMode}
         */
        explicit BufferCreateInfo(BufferUsages usages, UnsignedLong size, Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit BufferCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit BufferCreateInfo(const VkBufferCreateInfo& info);

        /** @brief Underlying @type_vk{BufferCreateInfo} structure */
        VkBufferCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkBufferCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkBufferCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkBufferCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkBufferCreateInfo*() const { return &_info; }

    private:
        VkBufferCreateInfo _info;
};

CORRADE_ENUMSET_OPERATORS(BufferCreateInfo::Flags)

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/Buffer.h"

#endif
