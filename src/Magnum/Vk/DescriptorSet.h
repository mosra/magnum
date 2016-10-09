#ifndef Magnum_Vk_DescriptorSet_h
#define Magnum_Vk_DescriptorSet_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Class @ref Magnum::Vk::DescriptorSet
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/ShaderStage.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

class DescriptorPool;
class ShadereStageFlags;

enum class DescriptorType: UnsignedInt {
    Sampler = VK_DESCRIPTOR_TYPE_SAMPLER,
    CombinedImageSampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    SampledImage = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    StorageImage = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    UniformTexelBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
    StorageTexelBuffer = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    UniformBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    StorageBuffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    UniformBufferDynamic = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    StorageBufferDynamic = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    InputAttachment = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
};

struct DescriptorSetLayoutBinding {
    public:

        DescriptorSetLayoutBinding(UnsignedInt binding, DescriptorType type,
                                   UnsignedInt descriptorsCount, ShaderStageFlags flags): // TODO: Immutable samplers
            _layoutBinding{
                binding, VkDescriptorType(type), descriptorsCount,
                VkShaderStageFlags(flags), nullptr
            }
        {}

    private:
         VkDescriptorSetLayoutBinding _layoutBinding;
};

class MAGNUM_VK_EXPORT DescriptorSetLayout {
    public:

        DescriptorSetLayout(Device& device,
                            std::initializer_list<DescriptorSetLayoutBinding> bindings):
                     _device{device}
        {
            const std::vector<DescriptorSetLayoutBinding> bndgs{bindings};
            VkDescriptorSetLayoutCreateInfo descLayout = {
                VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                nullptr, 0,
                UnsignedInt(bndgs.size()),
                reinterpret_cast<const VkDescriptorSetLayoutBinding*>(bndgs.data())
            };

            VkResult err = vkCreateDescriptorSetLayout(_device, &descLayout, nullptr, &_layout);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        /** @brief Copying is not allowed */
        DescriptorSetLayout(const DescriptorSetLayout&) = delete;

        /** @brief Move constructor */
        DescriptorSetLayout(DescriptorSetLayout&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyDescriptorSet}
         */
        ~DescriptorSetLayout() {
            vkDestroyDescriptorSetLayout(_device, _layout, nullptr);
        }

        /** @brief Copying is not allowed */
        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

        /** @brief Move assignment is not allowed */
        DescriptorSetLayout& operator=(DescriptorSetLayout&&) = delete;

        operator VkDescriptorSetLayout() const {
            return _layout;
        }

    private:
        Device& _device;
        VkDescriptorSetLayout _layout;
};

class MAGNUM_VK_EXPORT DescriptorSet {
    public:

        DescriptorSet(Device& device, DescriptorPool& pool, VkDescriptorSet vkDescriptorSet):
                     _device{device},
                     _pool{pool},
                     _descriptorSet{vkDescriptorSet}
        {
        }

        /** @brief Copying is not allowed */
        DescriptorSet(const DescriptorSet&) = delete;

        /** @brief Move constructor */
        DescriptorSet(DescriptorSet&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyDescriptorSet}
         */
        ~DescriptorSet();

        /** @brief Copying is not allowed */
        DescriptorSet& operator=(const DescriptorSet&) = delete;

        /** @brief Move assignment is not allowed */
        DescriptorSet& operator=(DescriptorSet&&) = delete;

        operator VkDescriptorSet() const {
            return _descriptorSet;
        }

    private:
        Device& _device;
        DescriptorPool& _pool;
        VkDescriptorSet _descriptorSet;
};

}}

#endif
