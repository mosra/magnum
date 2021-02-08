#ifndef Magnum_Vk_Implementation_DeviceState_h
#define Magnum_Vk_Implementation_DeviceState_h
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

#include <utility>

#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/* Declared here instead of in Vk.h because it makes no sense to store it
   anywhere */
class RenderPassCreateInfo;

namespace Implementation {

struct DeviceState {
    explicit DeviceState(Device& device, Containers::Array<std::pair<Containers::StringView, bool>>& encounteredWorkarounds);

    void(*getDeviceQueueImplementation)(Device&, const VkDeviceQueueInfo2&, VkQueue&);

    /** @todo put the rest eventually into a dedicated buffer / image state /
        ... struct? */

    void(*getBufferMemoryRequirementsImplementation)(Device&, const VkBufferMemoryRequirementsInfo2&, VkMemoryRequirements2&);
    void(*getImageMemoryRequirementsImplementation)(Device&, const VkImageMemoryRequirementsInfo2&, VkMemoryRequirements2&);
    VkResult(*bindBufferMemoryImplementation)(Device&, UnsignedInt, const VkBindBufferMemoryInfo*);
    VkResult(*bindImageMemoryImplementation)(Device&, UnsignedInt, const VkBindImageMemoryInfo*);

    VkResult(*createRenderPassImplementation)(Device&, const RenderPassCreateInfo&, const VkAllocationCallbacks*, VkRenderPass*);
    void(*cmdBeginRenderPassImplementation)(CommandBuffer&, const VkRenderPassBeginInfo&, const VkSubpassBeginInfo&);
    void(*cmdNextSubpassImplementation)(CommandBuffer&, const VkSubpassEndInfo&, const VkSubpassBeginInfo&);
    void(*cmdEndRenderPassImplementation)(CommandBuffer&, const VkSubpassEndInfo&);

    void(*cmdBindVertexBuffersImplementation)(CommandBuffer&, UnsignedInt, UnsignedInt, const VkBuffer*, const UnsignedLong*, const UnsignedLong*);

    void(*cmdCopyBufferImplementation)(CommandBuffer&, const CopyBufferInfo&);
    void(*cmdCopyImageImplementation)(CommandBuffer&, const CopyImageInfo&);
    void(*cmdCopyBufferToImageImplementation)(CommandBuffer&, const CopyBufferToImageInfo&);
    void(*cmdCopyImageToBufferImplementation)(CommandBuffer&, const CopyImageToBufferInfo&);
};

}}}

#endif
