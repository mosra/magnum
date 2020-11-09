#ifndef Magnum_Tags_h
#define Magnum_Tags_h
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
 * @brief Tag type @ref Magnum::NoInitT, @ref Magnum::NoCreateT, tag @ref Magnum::NoInit, @ref Magnum::NoCreate
 */

#include <Corrade/Containers/Tags.h>

#include "Magnum/configure.h"

namespace Magnum {

/**
@brief No initialization tag type
@m_since{2020,06}

Used to distinguish construction with no initialization at all.
@see @ref NoInit
*/
typedef Corrade::Containers::NoInitT NoInitT;

/**
@brief No creation tag type

Used to distinguish construction without creating the underlying OpenGL /
Vulkan / ... object.
@see @ref NoCreate
*/
typedef Corrade::Containers::NoCreateT NoCreateT;

/**
@brief No allocation tag type
@m_since_latest

Used to distinguish construction without allocating memory.
@see @ref NoAllocate
*/
/* Explicit constructor to avoid ambiguous calls when using {} */
struct NoAllocateT {
    #ifndef DOXYGEN_GENERATING_OUTPUT
    struct Init{};
    constexpr explicit NoAllocateT(Init) {}
    #endif
};

/**
@brief No initialization tag
@m_since{2020,06}

Use for construction with no initialization at all.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
constexpr NoInitT NoInit{};
#else
using Corrade::Containers::NoInit;
#endif

/**
@brief No creation tag

Use for construction without creating the underlying OpenGL / Vulkan / ...
object. Note that calling anything on objects created this way is not defined
(and not checked or guarded in any way) and may result in crashes. If you want
delayed object creation with safety checks (however with some extra memory
overhead), wrap the objects in an @ref Corrade::Containers::Optional.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
constexpr NoCreateT NoCreate{};
#else
using Corrade::Containers::NoCreate;
#endif

/**
@brief No allocation tag
@m_since_latest

Use for construction without allocating memory.
*/
constexpr NoAllocateT NoAllocate{NoAllocateT::Init{}};

}

#endif
