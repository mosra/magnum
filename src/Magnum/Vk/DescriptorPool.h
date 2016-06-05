#ifndef Magnum_Vk_DescriptorPool_h
#define Magnum_Vk_DescriptorPool_h
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
 * @brief Class @ref Magnum::Vk::DescriptorPool
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/DescriptorSet.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

class MAGNUM_VK_EXPORT DescriptorPoolCreateInfo {
        friend class DescriptorPool;
public:
        DescriptorPoolCreateInfo(Device& device):
            _device{device}, _poolSizes{}
        {
        }

        DescriptorPoolCreateInfo& setPoolSize(DescriptorType type, UnsignedInt size) {
            _poolSizes.push_back(VkDescriptorPoolSize{VkDescriptorType(type), size});
            return *this;
        }

private:

        Device& _device;
        std::vector<VkDescriptorPoolSize> _poolSizes;

};

class MAGNUM_VK_EXPORT DescriptorPool {
    public:

        DescriptorPool(Device& device, UnsignedLong maxSets, const DescriptorPoolCreateInfo& ci):
                     _device{device}
        {
            VkDescriptorPoolCreateInfo createInfo = {
                VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, nullptr, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                maxSets,
                ci._poolSizes.size(), ci._poolSizes.data()
            };

            VkResult err = vkCreateDescriptorPool(_device, &createInfo, nullptr, &_descriptorPool);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        /** @brief Copying is not allowed */
        DescriptorPool(const DescriptorPool&) = delete;

        /** @brief Move constructor */
        DescriptorPool(DescriptorPool&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyDescriptorPool}
         */
        ~DescriptorPool();

        /** @brief Copying is not allowed */
        DescriptorPool& operator=(const DescriptorPool&) = delete;

        /** @brief Move assignment is not allowed */
        DescriptorPool& operator=(DescriptorPool&&) = delete;

        operator VkDescriptorPool() const {
            return _descriptorPool;
        }

        std::unique_ptr<DescriptorSet> allocateDescriptorSet(const DescriptorSetLayout& layout) {
            VkDescriptorSetLayout vkLayout = layout;
            VkDescriptorSetAllocateInfo allocateInfo = {
                VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr,
                *this,
                1,
                &vkLayout
            };

            VkDescriptorSet set;
            VkResult err = vkAllocateDescriptorSets(_device, &allocateInfo, &set);
            MAGNUM_VK_ASSERT_ERROR(err);

            return std::unique_ptr<DescriptorSet>(new DescriptorSet{_device, *this, set});
        }

    private:
        Device& _device;
        VkDescriptorPool _descriptorPool;
};

}}

#endif
