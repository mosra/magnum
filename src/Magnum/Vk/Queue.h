#ifndef Magnum_Vk_Queue_h
#define Magnum_Vk_Queue_h
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
 * @brief Class @ref Magnum::Vk::Queue
 */

#include "Magnum/Magnum.h"
#include "Magnum/Vk/CommandBuffer.h"
#include "Magnum/Vk/Instance.h"
#include "Magnum/Vk/Device.h"
#include "Magnum/Vk/Semaphore.h"
#include "Magnum/Vk/visibility.h"


namespace Magnum { namespace Vk {

class MAGNUM_VK_EXPORT Queue {
    public:

        /** @brief Copying is not allowed */
        Queue(const Queue&) = delete;

        /** @brief Move constructor */
        Queue(Queue&& other);

        Queue(Device& device, UnsignedInt familyIndex, UnsignedInt queueIndex): _device{device} {
            vkGetDeviceQueue(_device, familyIndex, queueIndex, &_queue);
        }

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroyQueue}
         */
        ~Queue();

        /** @brief Copying is not allowed */
        Queue& operator=(const Queue&) = delete;

        /** @brief Move assignment is not allowed */
        Queue& operator=(Queue&&) = delete;

        VkQueue vkQueue() {
            return _queue;
        }

        Queue& waitIdle() {
            VkResult err = vkQueueWaitIdle(_queue);
            MAGNUM_VK_ASSERT_ERROR(err);

            return *this;
        }

        Queue& submit(const CommandBuffer& cmdBuffer);
        Queue& submit(const CommandBuffer& cmdBuffer, std::vector<std::reference_wrapper<Semaphore>> waitSemaphores, std::vector<std::reference_wrapper<Semaphore>> signalSemaphores);

        /**
         * @brief The device this queue was created for.
         */
        Device& device() {
            return _device;
        }

    private:
        VkQueue _queue;
        Device& _device;
};

}}

#endif
