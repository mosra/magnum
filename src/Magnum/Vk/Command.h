#ifndef Magnum_Vk_Command_h
#define Magnum_Vk_Command_h
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
 * @brief Namespace @ref Magnum::Vk::Cmd
 */

#include "vulkan.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Tags.h"
#include "Magnum/Magnum.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/Pipeline.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Image.h"

namespace Magnum { namespace Vk {

template<typename Lambda>
CommandBuffer& operator << (CommandBuffer& cmdBuffer, const Lambda& lambda) {
    lambda(cmdBuffer);
    return cmdBuffer;
}

namespace Cmd {

inline auto begin() {
    return [](CommandBuffer& cmdBuffer){
        cmdBuffer.begin();
    };
}

inline auto end() {
    return [](CommandBuffer& cmdBuffer){
        cmdBuffer.end();
    };
}

inline auto setScissor(UnsignedInt firstScissor, const std::initializer_list<Range2Di>& ranges) {
    return [firstScissor, &ranges](VkCommandBuffer cmdBuffer){
        Corrade::Containers::Array<VkRect2D> vkRects(ranges.size());

        UnsignedInt i = 0;
        for(auto& range: ranges) {
            vkRects[i] = {{range.left(), range.bottom()}, {UnsignedInt(range.sizeX()), UnsignedInt(range.sizeY())}};
            ++i;
        }
        vkCmdSetScissor(cmdBuffer, firstScissor, vkRects.size(), vkRects.data());
    };
}

inline auto setViewport(UnsignedInt firstViewport, const std::vector<VkViewport>& viewports) {
    return [firstViewport, &viewports](VkCommandBuffer cmdBuffer){
        vkCmdSetViewport(cmdBuffer, firstViewport, viewports.size(), viewports.data());
    };
}

inline auto pipelineBarrier(PipelineStageFlags srcStageMask,
                     PipelineStageFlags dstStageMask,
                     const std::vector<VkMemoryBarrier>& memoryBarriers,
                     const std::vector<VkBufferMemoryBarrier>& bufferMemoryBarriers,
                     const std::vector<ImageMemoryBarrier>& imageMemoryBarriers) {
    return [srcStageMask, dstStageMask, &memoryBarriers, &bufferMemoryBarriers, &imageMemoryBarriers](VkCommandBuffer cmdBuffer){
        vkCmdPipelineBarrier(cmdBuffer,
                             VkPipelineStageFlags(srcStageMask), VkPipelineStageFlags(dstStageMask),
                             0, memoryBarriers.size(),
                             memoryBarriers.data(),
                             bufferMemoryBarriers.size(),
                             bufferMemoryBarriers.data(),
                             imageMemoryBarriers.size(),
                             reinterpret_cast<const VkImageMemoryBarrier*>(imageMemoryBarriers.data())

        );
    };
}

}}}

#endif
