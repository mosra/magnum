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

#include "Queue.h"


namespace Magnum { namespace Vk {

Queue::~Queue() {
}

Queue& Queue::submit(const CommandBuffer& cmdBuffer) {
    const VkCommandBuffer cb = cmdBuffer;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cb;

    VkResult err = vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE);
    MAGNUM_VK_ASSERT_ERROR(err);

    return *this;
}

Queue& Queue::submit(const CommandBuffer& cmdBuffer,
              std::vector<std::reference_wrapper<Semaphore>> waitSemaphores,
              std::vector<std::reference_wrapper<Semaphore>> signalSemaphores) {
    const VkCommandBuffer cb = cmdBuffer;

    VkPipelineStageFlags pipelineDstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; // TODO: Expose
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask = &pipelineDstStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cb;

    std::vector<VkSemaphore> waitSems;
    waitSems.reserve(waitSemaphores.size());
    for(Semaphore& sem : waitSemaphores) {
        waitSems.push_back(sem.vkSemaphore());
    }

    std::vector<VkSemaphore> sigSems;
    sigSems.reserve(waitSemaphores.size());
    for(Semaphore& sem : signalSemaphores) {
        sigSems.push_back(sem.vkSemaphore());
    }
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSems.data();
    submitInfo.signalSemaphoreCount = signalSemaphores.size();
    submitInfo.pSignalSemaphores = sigSems.data();

    VkResult err = vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE);
    MAGNUM_VK_ASSERT_ERROR(err);

    return *this;
}

}}
