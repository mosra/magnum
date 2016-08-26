#ifndef Magnum_Vk_Semaphore_h
#define Magnum_Vk_Semaphore_h
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
 * @brief Class @ref Magnum::Vk::Semaphore
 */

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Device.h"


namespace Magnum { namespace Vk {

class MAGNUM_VK_EXPORT Semaphore {
    public:

        explicit Semaphore(NoCreateT) noexcept;

        /** @brief Constructor */
        explicit Semaphore(Device& device):
            _semaphore(VK_NULL_HANDLE),
            _device(&device)
        {
            constexpr VkSemaphoreCreateInfo semaphoreCreateInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0};
            VkResult err = vkCreateSemaphore(_device->vkDevice(), &semaphoreCreateInfo, nullptr, &_semaphore);
            MAGNUM_VK_ASSERT_ERROR(err);
        }

        /**
         * @brief Destructor
         *
         * @see @fn_vk{DestroySemaphore}
         */
        ~Semaphore() {
            /* NoCreate constructor initializes _device with nullptr */
            if (_device != nullptr) {
                vkDestroySemaphore(_device->vkDevice(), _semaphore, nullptr);
            }
        }

        /** @brief Copying is not allowed */
        Semaphore(const Semaphore& sem) = delete;

        /** @brief Move assignment */
        Semaphore& operator=(Semaphore&& sem) noexcept {
            std::swap(_semaphore, sem._semaphore);
            std::swap(_device, sem._device);

            return *this;
        }

        /** @brief Move constructor */
        Semaphore(Semaphore&& sem): _semaphore{sem}, _device{sem._device} {
            sem._semaphore = VK_NULL_HANDLE;
        }

        operator VkSemaphore() const {
            return _semaphore;
        }

    private:
        VkSemaphore _semaphore;
        Device* _device;
};

}}

#endif
