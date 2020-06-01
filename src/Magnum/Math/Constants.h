#ifndef Magnum_Math_Constants_h
#define Magnum_Math_Constants_h
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
 * @brief Class @ref Magnum::Math::Constants
 */

#include <Corrade/Utility/StlMath.h>

#include "Magnum/Types.h"

namespace Magnum { namespace Math {

/**
@brief Numeric constants

@see @ref Magnum::Constants, @ref Magnum::Constantsd
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T> struct Constants {
    /* See TypeTraits for answer why these are functions and not constants. */
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
     * If you think this is wrong, note that
     * [Python has it too](https://www.python.org/dev/peps/pep-0628/).
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
};
#else
template<class> struct Constants;
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef CORRADE_TARGET_EMSCRIPTEN
template<> struct Constants<long double> {
    /* Needed by Deg->Rad conversion, which is needed by a test for
       __builtin_sincos. Hopefully nobody else needs those, so it's just pi.
       21-digit string representation gives back the same value on rountrip.
       https://en.wikipedia.org/wiki/Extended_precision#Working_range
       Value taken using Wolfram Alpha.          1.23456789012345678901 */
    static constexpr long double pi()   { return 3.14159265358979323846l; }
};
#endif

template<> struct Constants<Double> {
    Constants() = delete;

    /* 17-digit string representation gives back the same value on rountrip.
       https://en.wikipedia.org/wiki/Double-precision_floating-point_format
       Values taken using Wolfram Alpha.         1.2345678901234567 */
    static constexpr Double pi()        { return 3.1415926535897932; }
    static constexpr Double piHalf()    { return 1.5707963267948966; }
    static constexpr Double piQuarter() { return 0.7853981633974483; }
    static constexpr Double tau()       { return 6.2831853071795864; }
    static constexpr Double e()         { return 2.7182818284590452; }
    static constexpr Double sqrt2()     { return 1.4142135623730950; }
    static constexpr Double sqrt3()     { return 1.7320508075688773; }
    static constexpr Double sqrtHalf()  { return 0.7071067811865475; }

    static constexpr Double nan() {
        /* For some reason the NAN macro is not constexpr when using clang-cl,
           but the builtin (which is used in std::numeric_limits) is. According
           to the test, NAN is constexpr when using MSVC itself, except on MSVC
           2015.*/
        #ifdef CORRADE_TARGET_CLANG_CL
        return __builtin_nan("0");
        #else
        return Double(NAN);
        #endif
    }
    static constexpr Double inf() {
        /* Same as above, but only for clang-cl 8. 9 has that fixed */
        #if defined(CORRADE_TARGET_CLANG_CL) && __clang_major__ < 9
        return __builtin_huge_val();
        #else
        return HUGE_VAL;
        #endif
    }
};
template<> struct Constants<Float> {
    Constants() = delete;

    /* 9-digit string representation gives back the same value on rountrip.
       https://en.wikipedia.org/wiki/Single-precision_floating-point_format
       Values rounded from the above.            1.23456789 */
    static constexpr Float pi()         { return 3.141592654f; }
    static constexpr Float piHalf()     { return 1.570796327f; }
    static constexpr Float piQuarter()  { return 0.785398163f; }
    static constexpr Float tau()        { return 6.283185307f; }
    static constexpr Float e()          { return 2.718281828f; }
    static constexpr Float sqrt2()      { return 1.414213562f; }
    static constexpr Float sqrt3()      { return 1.732050808f; }
    static constexpr Float sqrtHalf()   { return 0.707106781f; }

    static constexpr Float nan() {
        /* For some reason the NAN macro is not constexpr when using clang-cl,
           but the builtin (which is used in std::numeric_limits) is. According
           to the test, NAN is constexpr when using MSVC itself, except on MSVC
           2015.*/
        #ifdef CORRADE_TARGET_CLANG_CL
        return __builtin_nanf("0");
        #else
        return NAN;
        #endif
    }
    static constexpr Float inf() {
        /* Same as above, but only for clang-cl 8. 9 has that fixed */
        #if defined(CORRADE_TARGET_CLANG_CL) && __clang_major__ < 9
        return __builtin_huge_valf();
        #else
        return HUGE_VALF;
        #endif
    }
};
#endif

}}

#endif
