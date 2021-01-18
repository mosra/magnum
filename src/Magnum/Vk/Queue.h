#ifndef Magnum_Vk_Queue_h
#define Magnum_Vk_Queue_h
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
 * @brief Class @ref Magnum::Vk::Queue
 * @m_since_latest
 */

#include <Corrade/Containers/Pointer.h>

#include "Magnum/Tags.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Queue
@m_since_latest

Wraps a @type_vk_keyword{Queue}. See @ref Vk-Device-creation for information
about how queues are created and retrieved from a device and
@ref Vk-CommandBuffer-usage for an overview of recording and submitting
command buffers to a queue.
@see @ref DeviceCreateInfo::addQueues(), @ref submit()
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
        static Queue wrap(Device& device, VkQueue handle);

        /**
         * @brief Construct without creating the instance
         *
         * This is the expected way to create a queue that's later populated
         * on @ref Device creation through @ref DeviceCreateInfo::addQueues().
         */
        explicit Queue(NoCreateT);

        /* The class *technically* doesn't need to be move-only, it's done only
           for consistency and to make room for possible future move-only
           state. If move-only proves to be annoying, it might get removed --
           going the other way would be much more painful and breaking user
           code. */

        /** @brief Copying is not allowed */
        Queue(const Queue&) = delete;

        /** @brief Move constructor */
        Queue(Queue&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Queues are associated with the device they come from, so no explicit
         * destruction is needed.
         */
        ~Queue();

        /** @brief Copying is not allowed */
        Queue& operator=(const Queue&) = delete;

        /** @brief Move assignment */
        Queue& operator=(Queue&& other) noexcept;

        /** @brief Underlying @type_vk{Queue} handle */
        VkQueue handle() { return _handle; }
        /** @overload */
        operator VkQueue() { return _handle; }

        /**
         * @brief Submit a sequence of semaphores or command buffers to a queue
         * @param infos  Submit info structures, each specifying a command
         *      buffer submission batch
         * @param fence A @ref Fence or a raw Vulkan fence handle to be
         *      signaled once all submitted command buffers have completed
         *      execution. Pass a @cpp {} @ce or a
         *      @ref Fence::Fence(NoCreateT) "NoCreate"'d @ref Fence to not
         *      signal anything.
         *
         * @see @fn_vk_keyword{QueueSubmit}
         */
        void submit(Containers::ArrayView<const Containers::Reference<const SubmitInfo>> infos, VkFence fence);
        /** @overload */
        void submit(std::initializer_list<Containers::Reference<const SubmitInfo>> infos, VkFence fence);

        /**
         * @brief Submit a sequence of semaphores or command buffers to a queue and return a new fence to wait on
         *
         * Compared to @ref submit(Containers::ArrayView<const Containers::Reference<const SubmitInfo>>, VkFence)
         * creates a new @ref Fence and returns it for a more convenient
         * one-off submission.
         *
         * @m_class{m-note m-success}
         *
         * @par
         *      When submitting multiple times it's recommended to
         *      @ref Fence::reset() "reset()" an existing fence and reuse it
         *      instead of letting this function create a new one every time.
         */
        Fence submit(Containers::ArrayView<const Containers::Reference<const SubmitInfo>> infos);
        /** @overload */
        Fence submit(std::initializer_list<Containers::Reference<const SubmitInfo>> infos);

    private:
        /* Can't be a reference because of the NoCreate constructor */
        Device* _device;

        VkQueue _handle;
};

/**
@brief Queue submit info
@m_since_latest

Wraps a @type_vk_keyword{SubmitInfo}.
*/
class MAGNUM_VK_EXPORT SubmitInfo {
    public:
        /**
         * @brief Constructor
         *
         * The following @type_vk{SubmitInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    *(none)*
         *
         * @see @ref setCommandBuffers()
         */
        explicit SubmitInfo();

        /** @todo implicit constructor taking the command buffer list directly,
            needs AnyReference first because queue.submit({{a}}) would now fail
            on deleted Reference(T&&) */

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit SubmitInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit SubmitInfo(const VkSubmitInfo& info);

        /** @brief Copying is not allowed */
        SubmitInfo(const SubmitInfo&) = delete;

        /** @brief Move constructor */
        SubmitInfo(SubmitInfo&& other) noexcept;

        ~SubmitInfo();

        /** @brief Copying is not allowed */
        SubmitInfo& operator=(const SubmitInfo&) = delete;

        /** @brief Move assignment */
        SubmitInfo& operator=(SubmitInfo&& other) noexcept;

        /**
         * @brief Set command buffers to execute in the batch
         * @return Reference to self (for method chaining)
         */
        SubmitInfo& setCommandBuffers(Containers::ArrayView<const VkCommandBuffer> buffers);
        /** @overload */
        SubmitInfo& setCommandBuffers(std::initializer_list<VkCommandBuffer> buffers);

        /** @brief Underlying @type_vk{SubmitInfo} structure */
        VkSubmitInfo& operator*() { return _info; }
        /** @overload */
        const VkSubmitInfo& operator*() const { return _info; }
        /** @overload */
        VkSubmitInfo* operator->() { return &_info; }
        /** @overload */
        const VkSubmitInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkSubmitInfo*() const { return &_info; }

        /**
         * @overload
         *
         * The class is implicitly convertible to a reference in addition to
         * a pointer because the type is commonly used in arrays as well, which
         * would be annoying to do with a pointer conversion.
         */
        operator const VkSubmitInfo&() const { return _info; }

    private:
        VkSubmitInfo _info;

        struct State;
        Containers::Pointer<State> _state;
};

}}

#endif
