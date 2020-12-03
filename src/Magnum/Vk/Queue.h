#ifndef Magnum_Vk_Queue_h
#define Magnum_Vk_Queue_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @m_since_latest
 */

#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Queue
@m_since_latest

Wraps a @type_vk_keyword{Queue}. See @ref Device class docs for an introduction
on how to create a queue.
@see @ref DeviceCreateInfo::addQueues()
*/
class MAGNUM_VK_EXPORT Queue {
    public:
        /**
         * @brief Wrap existing Vulkan queue
         * @param device        Vulkan device
         * @param handle        The @type_vk{Queue} handle
         *
         * The @p handle is expected to be originating from @p device. Unlike
         * with other handle types, the @type_vk{Queue} handles don't have to
         * be destroyed at the end, so there's no equivalent of e.g.
         * @ref Device::release() or @ref Device::handleFlags().
         */
        static Queue wrap(Device& device, VkQueue handle) {
            Queue out{NoCreate};
            out._device = &device;
            out._handle = handle;
            return out;
        }

        /**
         * @brief Construct without creating the instance
         *
         * This is the expected way to create a queue that's later populated
         * on @ref Device creation through @ref DeviceCreateInfo::addQueues().
         */
        explicit Queue(NoCreateT): _device{}, _handle{} {}

        /** @brief Underlying @type_vk{Queue} handle */
        VkQueue handle() { return _handle; }
        /** @overload */
        operator VkQueue() { return _handle; }

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkQueue _handle;
};

}}

#endif
