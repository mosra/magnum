#ifndef Magnum_Vk_CommandBuffer_h
#define Magnum_Vk_CommandBuffer_h
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
 * @brief Class @ref Magnum::Vk::CommandBuffer
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/RenderPass.h"
#include "Magnum/Vk/Framebuffer.h"
#include "Magnum/Vk/visibility.h"

#include "vulkan.h"

namespace Magnum { namespace Vk {

class CommandPool;

class MAGNUM_VK_EXPORT CommandBuffer {
    public:

        enum class Level: Int {
            Primary = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            Secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY
        };

        enum class SubpassContents: UnsignedInt {
            Inline = VK_SUBPASS_CONTENTS_INLINE,
            SecondaryCommandBuffers = VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS,
        };

        CommandBuffer(VkCommandBuffer buffer, CommandPool& commandPool):
            _cmdBuffer{buffer},
            _pool{commandPool}
        {
        }

        /** @brief Copying is not allowed */
        CommandBuffer(const CommandBuffer&) = delete;

        /** @brief Move constructor */
        CommandBuffer(CommandBuffer&& other);

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyCommandBuffer}
         */
        ~CommandBuffer();

        /** @brief Copying is not allowed */
        CommandBuffer& operator=(const CommandBuffer&) = delete;

        /** @brief Move assignment is not allowed */
        CommandBuffer& operator=(CommandBuffer&&) = delete;

        operator VkCommandBuffer() const {
            return _cmdBuffer;
        }

        CommandBuffer& begin() {
            VkCommandBufferBeginInfo cmdBufInfo = {};
            cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            vkBeginCommandBuffer(_cmdBuffer, &cmdBufInfo);

            return *this;
        }

        CommandBuffer& end() {
            VkResult err = vkEndCommandBuffer(_cmdBuffer);
            MAGNUM_VK_ASSERT_ERROR(err);

            return *this;
        }

        CommandBuffer& beginRenderPass(SubpassContents subpassContents, RenderPass& renderPass,
                                       Vk::Framebuffer& framebuffer, Range2Di renderArea,
                                       const std::vector<VkClearValue>& clearValues) {
            VkRenderPassBeginInfo renderPassBeginInfo {
                VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                nullptr,
                renderPass,
                framebuffer,
                VkRect2D{{renderArea.left(), renderArea.bottom()}, {renderArea.sizeX(), renderArea.sizeY()}},
                clearValues.size(),
                clearValues.data()};

            vkCmdBeginRenderPass(_cmdBuffer, &renderPassBeginInfo, VkSubpassContents(subpassContents));

            return *this;
        }

        CommandBuffer& endRenderPass() {
            vkCmdEndRenderPass(_cmdBuffer);
            return *this;
        }

    private:
        VkCommandBuffer _cmdBuffer;
        CommandPool& _pool;
};

}}

#endif
