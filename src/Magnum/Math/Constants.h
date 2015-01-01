#ifndef Magnum_Math_Constants_h
#define Magnum_Math_Constants_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

/** @file
 * @brief Class @ref Magnum::Math::Constants
 */

#include "Magnum/Types.h"

namespace Magnum { namespace Math {

/**
@brief Numeric constants

@see @ref Magnum::Constants, @ref Magnum::Constantsd
*/
template<class T> struct Constants {
    Constants() = delete;

    /* See TypeTraits for answer why these are functions and not constants. */
    #ifdef DOXYGEN_GENERATING_OUTPUT
    /**
     * @brief Pi
     *
     * @see @ref tau(), @ref Deg, @ref Rad
     */
    static constexpr T pi();

    /**
     * @brief Tau
     *
     * Or two pi.
     * @see @ref pi(), @ref Deg, @ref Rad
     */
    static constexpr T tau();

    static constexpr T sqrt2(); /**< @brief Square root of 2 */
    static constexpr T sqrt3(); /**< @brief Square root of 3 */
    #endif
};

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_TARGET_GLES
template<> struct Constants<Double> {
    Constants() = delete;

    static constexpr Double pi()        { return 3.141592653589793; }
    static constexpr Double tau()       { return 6.283185307179586; }
    static constexpr Double sqrt2()     { return 1.414213562373095; }
    static constexpr Double sqrt3()     { return 1.732050807568877; }
};
#endif
template<> struct Constants<Float> {
    Constants() = delete;

    static constexpr Float pi()         { return 3.141592654f; }
    static constexpr Float tau()        { return 6.283185307f; }
    static constexpr Float sqrt2()      { return 1.414213562f; }
    static constexpr Float sqrt3()      { return 1.732050808f; }
};
#endif

}}

#endif
