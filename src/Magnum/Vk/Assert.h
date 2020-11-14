#ifndef Magnum_Vk_Assert_h
#define Magnum_Vk_Assert_h
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
 * @brief Macro @ref MAGNUM_VK_INTERNAL_ASSERT_RESULT()
 * @m_since_latest
 */

#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Utility/Assert.h>

#include "Magnum/configure.h"

#if !defined(CORRADE_NO_ASSERT) && !defined(MAGNUM_VK_INTERNAL_ASSERT_RESULT)
#ifndef CORRADE_STANDARD_ASSERT
#include <cstdlib>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Vk/Result.h"
#elif !defined(NDEBUG)
#include <cassert>
#endif
#endif

/**
@brief Assert that a Vulkan function call succeeds
@m_since_latest

Compared to using @ref CORRADE_INTERNAL_ASSERT_OUTPUT() to verify that
@cpp call == VK_SUCCESS @ce, this macro also prints the result value. Otherwise
the behavior is the same, including interactions with
@ref CORRADE_STANDARD_ASSERT and @ref CORRADE_NO_ASSERT. Works with both plain
Vulkan functions returning @type_vk{Result} and APIs returning
@ref Magnum::Vk::Result "Vk::Result".

You can override this implementation by placing your own
@cpp #define MAGNUM_VK_INTERNAL_ASSERT_RESULT @ce before including the
@ref Magnum/Vk/Assert.h header.
*/
#ifndef MAGNUM_VK_INTERNAL_ASSERT_RESULT
#if defined(CORRADE_NO_ASSERT) || (defined(CORRADE_STANDARD_ASSERT) && defined(NDEBUG))
#define MAGNUM_VK_INTERNAL_ASSERT_RESULT(call)                              \
    static_cast<void>(call)
#elif defined(CORRADE_STANDARD_ASSERT)
#define MAGNUM_VK_INTERNAL_ASSERT_RESULT(call)                              \
    assert(Magnum::Vk::Result(call) == Magnum::Vk::Result::Success)
#else
#define MAGNUM_VK_INTERNAL_ASSERT_RESULT(call)                              \
    do {                                                                    \
        const Magnum::Vk::Result _CORRADE_HELPER_PASTE(magnumVkResult, __LINE__) = Magnum::Vk::Result(call);         \
        if(_CORRADE_HELPER_PASTE(magnumVkResult, __LINE__) != Magnum::Vk::Result::Success) {                                                       \
            Corrade::Utility::Error{Corrade::Utility::Error::defaultOutput()} << "Call " #call " failed with" << _CORRADE_HELPER_PASTE(magnumVkResult, __LINE__) << "at " __FILE__ ":" CORRADE_LINE_STRING; \
            std::abort();                                                   \
        }                                                                   \
    } while(false)
#endif
#endif

#endif
