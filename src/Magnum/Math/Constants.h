#ifndef Magnum_Math_Constants_h
#define Magnum_Math_Constants_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <cmath>

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
     * @brief @f$ \pi @f$.
     *
     * @f[
     *      \pi = 180 \degree
     * @f]
     * @see @ref piHalf(), @ref piQuarter(), @ref tau(), @ref Deg, @ref Rad
     */
    static constexpr T pi();

    /**
     * @brief Half of a @f$ \pi @f$
     *
     * @f[
     *      \frac{\pi}{2} = 90 \degree
     * @f]
     * @see @ref pi(), @ref piQuarter(), @ref tau(), @ref Deg, @ref Rad
     */
    static constexpr T piHalf();

    /**
     * @brief Quarter of a @f$ \pi @f$
     *
     * @f[
     *      \frac{\pi}{4} = 45 \degree
     * @f]
     * @see @ref pi(), @ref piHalf(), @ref sqrtHalf(), @ref tau(), @ref Deg,
     *      @ref Rad
     */
    static constexpr T piQuarter();

    /**
     * @brief @f$ \tau @f$.
     *
     * Or two pi. See the [Tau manifesto](https://www.tauday.com/tau-manifesto).
     *
     * @f[
     *      \tau = 2 \pi = 360 \degree
     * @f]
     * @see @ref pi(), @ref piHalf(), @ref piQuarter(), @ref Deg, @ref Rad
     */
    static constexpr T tau();

    /**
     * @brief Euler's number
     *
     * @f[
     *      e = \ln (1)
     * @f]
     * @see @ref log(), @ref exp()
     */
    static constexpr T e();

    /**
     * @brief Square root of 2
     *
     * @f[
     *      \sqrt{2}
     * @f]
     * @see @ref sqrt3(), @ref sqrtHalf()
     */
    static constexpr T sqrt2();

    /**
     * @brief Square root of 3
     *
     * @f[
     *      \sqrt{3}
     * @f]
     * @see @ref sqrt2(), @ref sqrtHalf()
     */
    static constexpr T sqrt3();

    /**
     * @brief Square root of @f$ \frac{1}{2} @f$
     *
     * @f[
     *      \frac{\sqrt{2}}{2} = \frac{1}{\sqrt{2}} = \sin(45 \degree) = \cos(45 \degree)
     * @f]
     * @see @ref sqrt2(), @ref sqrt3(), @ref piQuarter()
     */
    static constexpr T sqrtHalf();

    /**
     * @brief Quiet NaN
     *
     * @see @ref isNan()
     */
    static constexpr T nan();

    /**
     * @brief Positive @f$ \infty @f$.
     *
     * @see @ref isInf()
     */
    static constexpr T inf();
    #endif
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct Constants<Double> {
    Constants() = delete;

    static constexpr Double pi()        { return 3.141592653589793; }
    static constexpr Double piHalf()    { return 1.570796326794897; }
    static constexpr Double piQuarter() { return 0.785398163397448; }
    static constexpr Double tau()       { return 6.283185307179586; }
    static constexpr Double e()         { return 2.718281828459045; }
    static constexpr Double sqrt2()     { return 1.414213562373095; }
    static constexpr Double sqrt3()     { return 1.732050807568877; }
    static constexpr Double sqrtHalf()  { return 0.707106781186547; }

    static constexpr Double nan()   { return Double(NAN); }
    static constexpr Double inf()   { return HUGE_VAL; }
};
template<> struct Constants<Float> {
    Constants() = delete;

    static constexpr Float pi()         { return 3.141592654f; }
    static constexpr Float piHalf()     { return 1.570796327f; }
    static constexpr Float piQuarter()  { return 0.785398163f; }
    static constexpr Float tau()        { return 6.283185307f; }
    static constexpr Float e()          { return 2.718281828f; }
    static constexpr Float sqrt2()      { return 1.414213562f; }
    static constexpr Float sqrt3()      { return 1.732050808f; }
    static constexpr Float sqrtHalf()   { return 0.707106781f; }

    static constexpr Float nan()    { return NAN; }
    static constexpr Float inf()    { return HUGE_VALF; }
};
#endif

}}

#endif
