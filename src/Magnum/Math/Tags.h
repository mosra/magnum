#ifndef Magnum_Math_Tags_h
#define Magnum_Math_Tags_h
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
 * @brief Tag type @ref Magnum::Math::ZeroInitT, @ref Magnum::Math::IdentityInitT, tag @ref Magnum::Math::ZeroInit, @ref Magnum::Math::IdentityInit
 */

#include <Corrade/Containers/Tags.h>

#include "Magnum/Tags.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Math {

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief No initialization tag type
@m_deprecated_since{2020,06} Use @ref Magnum::NoInitT instead.
*/
typedef CORRADE_DEPRECATED("use Magnum::NoInitT instead") Magnum::NoInitT NoInitT;
#endif

/**
@brief Zero initialization tag type

Used to distinguish construction with all elements set to zero.
@see @ref ZeroInit
*/
/* Explicit constructor to avoid ambiguous calls when using {} */
struct ZeroInitT {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    struct Init{};
    constexpr explicit ZeroInitT(Init) {}
    #endif
};

/**
@brief Identity initialization tag type

Used to distinguish construction with transformation set to identity.
@see @ref IdentityInit
*/
/* Explicit constructor to avoid ambiguous calls when using {} */
struct IdentityInitT {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    struct Init{};
    constexpr explicit IdentityInitT(Init) {}
    #endif
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief No initialization tag
@m_deprecated_since{2020,06} Use @ref Magnum::NoInit instead.
*/
/** @todo when removing, clean up all Magnum::NoInit in Math to be NoInit again */
/* Explicit constructor to avoid ambiguous calls when using {} */
constexpr CORRADE_DEPRECATED("use Magnum::NoInit instead") Magnum::NoInitT NoInit{Magnum::NoInitT::Init{}};
#endif

/**
@brief Zero initialization tag

Use for construction with all elements set to zero.
*/
constexpr ZeroInitT ZeroInit{ZeroInitT::Init{}};

/**
@brief Identity initialization tag

Use for construction with transformation set to identity.
*/
constexpr IdentityInitT IdentityInit{IdentityInitT::Init{}};

}}

#endif
