#ifndef Magnum_Shaders_Implementation_lineMiterLimit_h
#define Magnum_Shaders_Implementation_lineMiterLimit_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Utility/Assert.h>

#include "Magnum/Math/Functions.h"

namespace Magnum { namespace Shaders { namespace Implementation {

/* These helpers, along with tests, are copied verbatim between the Shaders and
   Ui libraries */

inline Float lineMiterLengthLimit(const char*
    #if !defined(CORRADE_NO_ASSERT) && !defined(CORRADE_STANDARD_ASSERT)
    const name
    #endif
, const Float limit) {
    CORRADE_ASSERT(limit >= 1.0f && !Math::isInf(limit),
        name << "expected a finite value greater than or equal to 1, got" << limit, {});
    /* Calculate the half-angle from the length and return a cosine of it */
    return Math::cos(2.0f*Math::asin(1.0f/limit));
}

inline Float lineMiterAngleLimit(const char*
    #if !defined(CORRADE_NO_ASSERT) && !defined(CORRADE_STANDARD_ASSERT)
    const name
    #endif
, const Rad limit) {
    using namespace Math::Literals;
    CORRADE_ASSERT(limit > 0.0_radf && limit <= Rad{Constants::pi()},
        name << "expected a value greater than 0° and less than or equal to 180°, got" << Float(Deg(limit)) << Debug::nospace << "°", {});
    /* Return a cosine of the angle */
    return Math::cos(limit);
}

}}}

#endif
