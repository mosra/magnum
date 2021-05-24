#ifndef Magnum_Vk_FenceCreateInfo_h
#define Magnum_Vk_FenceCreateInfo_h
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
 * @brief Class @ref Magnum::Vk::FenceCreateInfo
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Vk/visibility.h"
#include "Magnum/Vk/Vk.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk {

/**
@brief Fence creation info
@m_since_latest

Wraps a @type_vk_keyword{FenceCreateInfo}. See
@ref Vk-Fence-creation "Fence creation" for usage information.
*/
class MAGNUM_VK_EXPORT FenceCreateInfo {
    public:
        /**
         * @brief Fence creation flag
         *
         * Wraps @type_vk_keyword{FenceCreateFlagBits}.
         * @see @ref Flags, @ref FenceCreateInfo(Flags)
         * @m_enum_values_as_keywords
         */
        enum class Flag: UnsignedInt {
            /**
             * Create the fence in a signaled state.
             *
             * @ref Fence::status(), @ref Fence::reset()
             */
            Signaled = VK_FENCE_CREATE_SIGNALED_BIT
        };

        /**
         * @brief Fence creation flags
         *
         * Type-safe wrapper for @type_vk_keyword{FenceCreateFlags}.
         * @see @ref FenceCreateInfo(Flags)
         */
        typedef Containers::EnumSet<Flag> Flags;

        /**
         * @brief Constructor
         * @param flags         Fence creation flags
         *
         * The following @type_vk{FenceCreateInfo} fields are pre-filled in
         * addition to `sType`, everything else is zero-filled:
         *
         * -    `flags`
         */
        /* Fence{device, FenceCreateInfo::Flag::Signaled} doesn't work anyway
           (would need an extra conversion from Flag to Flags), so no point in
           making this implicit. */
        explicit FenceCreateInfo(Flags flags = {});

        /**
         * @brief Construct without initializing the contents
         *
         * Note that not even the `sType` field is set --- the structure has to
         * be fully initialized afterwards in order to be usable.
         */
        explicit FenceCreateInfo(NoInitT) noexcept;

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly created
         * instance will not modify the original data nor the pointed-to data.
         */
        explicit FenceCreateInfo(const VkFenceCreateInfo& info);

        /** @brief Underlying @type_vk{FenceCreateInfo} structure */
        VkFenceCreateInfo& operator*() { return _info; }
        /** @overload */
        const VkFenceCreateInfo& operator*() const { return _info; }
        /** @overload */
        VkFenceCreateInfo* operator->() { return &_info; }
        /** @overload */
        const VkFenceCreateInfo* operator->() const { return &_info; }
        /** @overload */
        operator const VkFenceCreateInfo*() const { return &_info; }

    private:
        VkFenceCreateInfo _info;
};

CORRADE_ENUMSET_OPERATORS(FenceCreateInfo::Flags)

}}

/* Make the definition complete -- it doesn't make sense to have a CreateInfo
   without the corresponding object anyway. */
#include "Magnum/Vk/Fence.h"

#endif
