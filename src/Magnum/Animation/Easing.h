#ifndef Magnum_Animation_Easing_h
#define Magnum_Animation_Easing_h
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

/** @file
 * @brief Struct @ref Magnum::Animation::BasicEasing, typedef @ref Magnum::Animation::Easing, @ref Magnum::Animation::Easingd
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Constants.h"
#include "Magnum/Animation/Animation.h"

namespace Magnum { namespace Animation {

/**
@brief Easing functions
@m_since_latest

@m_keywords{Easing Tweening Inbetweening}

A collection of predefined [easing / tweening](https://en.wikipedia.org/wiki/Inbetweening)
functions for adding life to animation interpolation. Meant to be used through
the @ref Easing and @ref Easingd typedefs.

This library is built as part of Magnum by default. To use this library with
CMake, find the `Magnum` package and link to the `Magnum::Magnum` target:

@code{.cmake}
find_package(Magnum REQUIRED)

# ...
target_link_libraries(your-app PRIVATE Magnum::Magnum)
@endcode

See @ref building, @ref cmake and @ref animation for more information.

@m_class{m-row m-container-inflate}

@parblock

@m_div{m-col-l-2 m-push-l-2 m-col-m-3 m-col-t-6 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-linear-thumb.svg
@ref linear() @m_class{m-label m-primary} **B** @m_class{m-label m-success} **E**
@m_enddiv

@m_div{m-col-l-2 m-push-l-2 m-col-m-3 m-col-t-6 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-step-thumb.svg
@ref step() @m_class{m-label m-success} **E**
@m_enddiv

@m_div{m-col-l-2 m-push-l-2 m-col-m-3 m-col-t-6 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-smoothstep-thumb.svg
@ref smoothstep() @m_class{m-label m-primary} **B** @m_class{m-label m-success} **E**
@m_enddiv

@m_div{m-col-l-2 m-push-l-2 m-col-m-3 m-col-t-6 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-smootherstep-thumb.svg
@ref smootherstep() @m_class{m-label m-success} **E**
@m_enddiv

@endparblock

@m_class{m-row m-container-inflate}

@parblock

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-quadraticin-thumb.svg
@ref quadraticIn() @m_class{m-label m-primary} **B** @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-quadraticout-thumb.svg
@ref quadraticOut() @m_class{m-label m-primary} **B** @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-quadraticinout-thumb.svg
@ref quadraticInOut() @m_class{m-label m-info} **B** @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-cubicin-thumb.svg
@ref cubicIn() @m_class{m-label m-primary} **B** @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-cubicout-thumb.svg
@ref cubicOut() @m_class{m-label m-primary} **B** @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-cubicinout-thumb.svg
@ref cubicInOut() @m_class{m-label m-info} **B** @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-quarticin-thumb.svg
@ref quarticIn() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-quarticout-thumb.svg
@ref quarticOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-quarticinout-thumb.svg
@ref quarticInOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-quinticin-thumb.svg
@ref quinticIn() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-quinticout-thumb.svg
@ref quinticOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-quinticinout-thumb.svg
@ref quinticInOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-sinein-thumb.svg
@ref sineIn() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-sineout-thumb.svg
@ref sineOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-sineinout-thumb.svg
@ref sineInOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-circularin-thumb.svg
@ref circularIn()
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-circularout-thumb.svg
@ref circularOut()
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-circularinout-thumb.svg
@ref circularInOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-exponentialin-thumb.svg
@ref exponentialIn() @m_class{m-label m-success} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-exponentialout-thumb.svg
@ref exponentialOut() @m_class{m-label m-success} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-exponentialinout-thumb.svg
@ref exponentialInOut() @m_class{m-label m-success} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-elasticin-thumb.svg
@ref elasticIn() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-elasticout-thumb.svg
@ref elasticOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-elasticinout-thumb.svg
@ref elasticInOut() @m_class{m-label m-success} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-backin-thumb.svg
@ref backIn() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-backout-thumb.svg
@ref backOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-backinout-thumb.svg
@ref backInOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-bouncein-thumb.svg
@ref bounceIn() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-bounceout-thumb.svg
@ref bounceOut() @m_class{m-label m-danger} **E**
@m_enddiv

@m_div{m-col-l-2 m-col-t-4 m-text-center m-nopadt m-nopadx}
@htmlinclude easings-bounceinout-thumb.svg
@ref bounceInOut() @m_class{m-label m-danger} **E**
@m_enddiv

@endparblock

The easing function is meant to be used to modify the interpolation factor, such as:

@snippet Animation.cpp Easing-factor

The @ref Animation library also provides the @ref ease() utility that combines
the interpolator together with the easing function:

@snippet Animation.cpp Easing-ease

@section Animation-Easing-equations Equations

Every function documentation shows a plot of its behavior, together with a
direction in which it extrapolates. Green color means the extrapolation goes in
a reasonable monotonic direction outside of the range (also denoted by
@m_class{m-label m-success} **E** in the above list), red color means the
extrapolation is defined, but behaves in a probably unwanted or non-monotonic
way (denoted by @m_class{m-label m-danger} **E** in the above list). If neither
is present, it means the function is not defined outside of the @f$ [ 0; 1 ] @f$
range and produces a NaN. You may want to ensure the factor stays in bounds,
using either @ref Math::clamp() or the @ref easeClamped() function --- the
following two expressions are equivalent:

@snippet Animation.cpp Easing-clamp

Out-function @f$ f_\text{out} @f$ for a corresponding in-function @f$ f_\text{in} @f$
is defined as the following, the equations in the docs usually just show the
final derived form. Similarly goes for combined in-/out-function
@f$ f_\text{inout} @f$: @f[
    \begin{array}{rcl}
        f_\text{out}(x) & = & 1 - f_\text{in}(1 - x) \\[5pt]
        f_\text{inout}(x) & = & \left. \begin{cases}
            \frac{1}{2} f_\text{in}(2x), & x < 0.5 \\
            \frac{1}{2} (1 + f_\text{out}(2x - 1)), & x \ge 0.5
        \end{cases} \right\} = \begin{cases}
            \frac{1}{2} f_\text{in}(2x), & x < 0.5 \\
            1 - \frac{1}{2} f_\text{in}(2 - 2x), & x \ge 0.5
        \end{cases}
    \end{array}
@f]

Easing functions defined by simple polynomials can have an *exact* (denoted
by @m_class{m-label m-primary} **B** in the above list) or approximate (denoted
by @m_class{m-label m-info} **B** in the above list) cubic @ref Math::Bezier
curve representation (and thus, in turn, convertible to Cubic Hermite splines
using @ref Math::CubicHermite::fromBezier()). If that's the case, given
function documentation also lists the corresponding Bézier representation and
plots it with a thin blue line. The curve is always normalized to go from
@f$ (0, 0)^T @f$ to @f$ (1, 1)^T @f$, apply arbitrary transformation to each
point as needed:

@snippet Animation.cpp Easing-bezier-transform

@section Animation-Easing-references References

Functions follow the common naming from Robert Penner's Easing functions,
http://robertpenner.com/easing/. Implementation based on and inspired by
https://easings.net/,
[https://github.com/warrenm/AHEasing](https://github.com/warrenm/AHEasing/blob/master/AHEasing/easing.c),
[https://github.com/bkaradzic/bx](https://github.com/bkaradzic/bx/blob/master/include/bx/inline/easing.inl),
https://blog.demofox.org/2014/08/28/one-dimensional-bezier-curves/.
*/
template<class T> struct BasicEasing {
    /**
     * @brief Linear
     *
     * @htmlinclude easings-linear.svg
     *
     * @f[
     *      y = x
     * @f]
     *
     * One possible *exact* Bézier representation:
     *
     * @snippet easings.cpp linear
     */
    static T linear(T t) { return t; }

    /**
     * @brief Step
     *
     * Similar to @ref Math::select(), but does the step in the middle of the
     * range instead of at the end. Implementation matching the GLSL
     * @glsl step() @ce function with @cpp edge = T(0.5) @ce.
     *
     * @htmlinclude easings-step.svg
     *
     * @f[
     *      y = \begin{cases}
     *          0, & x < 0.5 \\
     *          1, & x \ge 0.5
     *      \end{cases}
     * @f]
     *
     * @m_keyword{step(),GLSL step(),}
     * @see @ref smoothstep(), @ref smootherstep()
     */
    static T step(T t) { return t < T(0.5) ? T(0.0) : T(1.0); }

    /**
     * @brief [Smoothstep](https://en.wikipedia.org/wiki/Smoothstep).
     *
     * Implementation matching the GLSL @glsl smoothstep() @ce function.
     * Combine with @ref Math::lerp() to get the equivalent result:
     *
     * @snippet Animation.cpp Easing-smoothstep
     *
     * @htmlinclude easings-smoothstep.svg
     *
     * @f[
     *      y = \begin{cases}
     *          0, & x < 0 \\
     *          3x^2 - 2x^3, & x \in [0, 1] \\
     *          1, & x > 1
     *      \end{cases}
     * @f]
     *
     * *Exact* Bézier representation:
     *
     * @snippet easings.cpp smoothstep
     *
     * @m_keyword{smoothstep(),GLSL smoothstep(),}
     * @see @ref smootherstep(), @ref Math::clamp()
     */
    static T smoothstep(T t) {
        /* Deliberately *not* using Math::clamp() because that would drag in
           unneeded vector headers */
        if(t <= T(0.0)) return T(0.0);
        if(t >= T(1.0)) return T(1.0);
        return (T(3.0) - T(2.0)*t)*t*t;
    }

    /**
     * @brief [Smootherstep](https://en.wikipedia.org/wiki/Smoothstep#Variations).
     *
     * Improved version of @ref smoothstep() by [Ken Perlin](https://en.wikipedia.org/wiki/Ken_Perlin).
     *
     * @htmlinclude easings-smootherstep.svg
     *
     * @f[
     *      y = \begin{cases}
     *          0, & x < 0 \\
     *          6x^5 - 15x^4 + 10x^3, & x \in [0, 1] \\
     *          1, & x > 1
     *      \end{cases}
     * @f]
     *
     * @see @ref Math::clamp()
     */
    static T smootherstep(T t) {
        /* Deliberately *not* using Math::clamp() because that would drag in
           unneeded vector headers */
        if(t <= T(0.0)) return T(0.0);
        if(t >= T(1.0)) return T(1.0);
        return t*t*t*(t*(t*T(6.0) - T(15.0)) + T(10.0));
    }

    /**
     * @brief Quadratic in
     *
     * @htmlinclude easings-quadraticin.svg
     *
     * @f[
     *      y = x^2
     * @f]
     *
     * *Exact* Bézier representation:
     *
     * @snippet easings.cpp quadraticIn
     *
     * @see @ref cubicIn(), @ref quarticIn(), @ref quinticIn()
     */
    static T quadraticIn(T t) { return t*t; }

    /**
     * @brief Quadratic out
     *
     * @htmlinclude easings-quadraticout.svg
     *
     * @f[
     *      y = 1 - (1 - x)^2 = (2 - x) x
     * @f]
     *
     * *Exact* Bézier representation:
     *
     * @snippet easings.cpp quadraticOut
     *
     * @see @ref cubicOut(), @ref quarticOut(), @ref quinticOut()
     */
    static T quadraticOut(T t) { return -t*(t - T(2.0)); }

    /**
     * @brief Quadratic in and out
     *
     * Combination of @ref quadraticIn() and @ref quadraticOut().
     *
     * @htmlinclude easings-quadraticinout.svg
     *
     * @f[
     *      y = \begin{cases}
     *          2 x^2, & x < 0.5 \\
     *          1 - 2 (1 - x)^2, & x \ge 0.5
     *      \end{cases}
     * @f]
     *
     * Approximate Bézier representation:
     *
     * @snippet easings.cpp quadraticInOut
     *
     * @see @ref cubicInOut(), @ref quarticInOut(), @ref quinticInOut()
     */
    static T quadraticInOut(T t) {
        if(t < T(0.5)) return T(2.0)*t*t;

        const T inv = T(1.0) - t;
        return T(1.0) - T(2.0)*inv*inv;
    }

    /**
     * @brief Cubic in
     *
     * @htmlinclude easings-cubicin.svg
     *
     * @f[
     *      y = x^3
     * @f]
     *
     * *Exact* Bézier representation:
     *
     * @snippet easings.cpp cubicIn
     *
     * @see @ref quadraticIn(), @ref quarticIn(), @ref quinticIn()
     */
    static T cubicIn(T t) { return t*t*t; }

    /**
     * @brief Cubic out
     *
     * @htmlinclude easings-cubicout.svg
     *
     * @f[
     *      y = 1 - (1 - x)^3
     * @f]
     *
     * *Exact* Bézier representation:
     *
     * @snippet easings.cpp cubicOut
     *
     * @see @ref quadraticOut(), @ref quarticOut(), @ref quinticOut()
     */
    static T cubicOut(T t) {
        const T inv = t - T(1.0);
        return inv*inv*inv + T(1.0);
    }

    /**
     * @brief Cubic in and out
     *
     * Combination of @ref cubicIn() and @ref cubicOut().
     *
     * @htmlinclude easings-cubicinout.svg
     *
     * @f[
     *      y = \begin{cases}
     *          4 x^3, & x < 0.5 \\
     *          1 - 4 (1 - x)^3, & x \ge 0.5
     *      \end{cases}
     * @f]
     *
     * Approximate Bézier representation:
     *
     * @snippet easings.cpp cubicInOut
     *
     * @see @ref quadraticInOut(), @ref quarticInOut(), @ref quinticInOut()
     */
    static T cubicInOut(T t) {
        if(t < T(0.5)) return T(4.0)*t*t*t;

        const T inv = T(1.0) - t;
        return T(1.0) - T(4.0)*inv*inv*inv;
    }

    /**
     * @brief Quartic in
     *
     * @htmlinclude easings-quarticin.svg
     *
     * @f[
     *      y = x^4
     * @f]
     *
     * @see @ref quadraticIn(), @ref cubicIn(), @ref quinticIn()
     */
    static T quarticIn(T t) {
        /* Not just t*t*t*t, since compile can't optimize it on its own to just
           two multiplication without breaking precision. So doing that
           explicitly. */
        const T tt = t*t;
        return tt*tt;
    }

    /**
     * @brief Quartic out
     *
     * @htmlinclude easings-quarticout.svg
     *
     * @f[
     *      y = 1 - (1 - x)^4
     * @f]
     *
     * @see @ref quadraticOut(), @ref cubicOut(), @ref quinticOut()
     */
    static T quarticOut(T t) {
        /* Instead of t*t*t*t suggesting the optimization as described above */
        const T inv = T(1.0) - t;
        const T quad = inv*inv;
        return T(1.0) - quad*quad;
    }

    /**
     * @brief Quartic in and out
     *
     * Combination of @ref quarticIn() and @ref quarticOut().
     *
     * @htmlinclude easings-quarticinout.svg
     *
     * @f[
     *      y = \begin{cases}
     *          8 x^4, & x < 0.5 \\
     *          1 - 8 (1 - x)^4, & x \ge 0.5
     *      \end{cases}
     * @f]
     *
     * @see @ref quadraticInOut(), @ref cubicInOut(), @ref quinticInOut()
     */
    static T quarticInOut(T t) {
        /* Instead of t*t*t*t suggesting the optimization as described above */

        if(t < T(0.5)) {
            const T tt = t*t;
            return 8*tt*tt;
        }

        const T inv = T(1.0) - t;
        const T quad = inv*inv;
        return T(1.0) - T(8.0)*quad*quad;
    }

    /**
     * @brief Quintic in
     *
     * @htmlinclude easings-quinticin.svg
     *
     * @f[
     *      y = x^5
     * @f]
     *
     * @see @ref quadraticIn(), @ref cubicIn(), @ref quarticIn()
     */
    static T quinticIn(T t) {
        /* Instead of t*t*t*t*t suggesting the optimization as described
           above */
        const T tt = t*t;
        return tt*t*tt;
    }

    /**
     * @brief Quintic out
     *
     * @htmlinclude easings-quinticout.svg
     *
     * @f[
     *      y = 1 - (1 - x)^5
     * @f]
     *
     * @see @ref quadraticOut(), @ref cubicOut(), @ref quarticOut()
     */
    static T quinticOut(T t) {
        /* Instead of t*t*t*t*t suggesting the optimization as described
           above */
        const T inv = t - T(1.0);
        const T quad = inv*inv;
        return T(1.0) + quad*inv*quad;
    }

    /**
     * @brief Quintic in and out
     *
     * Combination of @ref quinticIn() and @ref quinticOut().
     *
     * @htmlinclude easings-quinticinout.svg
     *
     * @f[
     *      y = \begin{cases}
     *          16 x^5, & x < 0.5 \\
     *          1 - 16 (1 - x)^5, & x \ge 0.5
     *      \end{cases}
     * @f]
     *
     * @see @ref quadraticInOut(), @ref cubicInOut(), @ref quarticInOut()
     */
    static T quinticInOut(T t) {
        /* Instead of t*t*t*t*t suggesting the optimization as described
           above */

        if(t < T(0.5)) {
            const T tt = t*t;
            return T(16.0)*tt*t*tt;
        }

        const T inv = T(1.0) - t;
        const T quad = inv*inv;
        return T(1.0) - T(16.0)*quad*inv*quad;
    }

    /**
     * @brief Sine in
     *
     * @htmlinclude easings-sinein.svg
     *
     * @f[
     *      y = 1 + \sin(\frac{\pi}{2} (x - 1))
     * @f]
     *
     * @see @ref circularIn()
     */
    static T sineIn(T t) {
        return T(1.0) + std::sin(Math::Constants<T>::piHalf()*(t - T(1.0)));
    }

    /**
     * @brief Sine out
     *
     * @htmlinclude easings-sineout.svg
     *
     * @f[
     *      y = \sin(\frac{\pi}{2} x)
     * @f]
     *
     * @see @ref circularOut()
     */
    static T sineOut(T t) {
        return std::sin(Math::Constants<T>::piHalf()*t);
    }

    /**
     * @brief Sine in and out
     *
     * Combination of @ref sineIn() and @ref sineOut().
     *
     * @htmlinclude easings-sineinout.svg
     *
     * @f[
     *      y = \frac{1}{2} (1 - \cos(\pi x))
     * @f]
     *
     * @see @ref circularInOut()
     */
    static T sineInOut(T t) {
        return T(0.5)*(T(1.0) - std::cos(t*Math::Constants<T>::pi()));
    }

    /**
     * @brief Circular in
     *
     * @htmlinclude easings-circularin.svg
     *
     * @f[
     *      y = 1 - \sqrt{1 - x^2}
     * @f]
     *
     * @see @ref sineIn()
     */
    static T circularIn(T t) {
        return T(1.0) - std::sqrt(T(1.0) - t*t);
    }

    /**
     * @brief Circular out
     *
     * @htmlinclude easings-circularout.svg
     *
     * @f[
     *      y = \sqrt{(2 - x) x}
     * @f]
     *
     * @see @ref sineOut()
     */
    static T circularOut(T t) {
        return std::sqrt((T(2.0) - t)*t);
    }

    /**
     * @brief Circular in and out
     *
     * Combination of @ref circularIn() and @ref circularOut().
     *
     * @htmlinclude easings-circularinout.svg
     *
     * @f[
     *      y = \begin{cases}
     *          \frac{1}{2} (1 - \sqrt{1 - (2x)^2}), & x < 0.5 \\
     *          \frac{1}{2} (1 + \sqrt{1 - (2x - 2)^2}), & x \ge 0.5
     *      \end{cases}
     * @f]
     *
     * @see @ref sineInOut()
     */
    static T circularInOut(T t) {
        if(t < T(0.5)) return T(0.5)*(T(1.0) - std::sqrt(T(1.0) - T(4.0)*t*t));
        return T(0.5)*(T(1.0) + std::sqrt(T(-4.0)*t*t + T(8.0)*t - T(3.0)));
    }

    /**
     * @brief Exponential in
     *
     * Contrary to Robert Penner's book but consistently with other
     * implementations has a special case for @f$ x \le 0 @f$, because
     * @f$ 2^{-10} = 0.0009765625 @f$ otherwise.
     *
     * @htmlinclude easings-exponentialin.svg
     *
     * @f[
     *      y = \begin{cases}
     *          0, & x \le 0 \\
     *          2^{10(x - 1)}, & x \ne 0
     *      \end{cases}
     * @f]
     *
     * @todo could be done better like with the sRGB curve, but should I
     *      bother?
     */
    static T exponentialIn(T t) {
        return t <= T(0.0) ? T(0.0) : std::pow(T(2.0), T(10.0)*(t - T(1.0)));
    }

    /**
     * @brief Exponential out
     *
     * Contrary to Robert Penner's book but consistently with other
     * implementations has a special case for @f$ x \ge 1 @f$, because
     * @f$ 2^{-10} = 0.0009765625 @f$ otherwise.
     *
     * @htmlinclude easings-exponentialout.svg
     *
     * @f[
     *      y = \begin{cases}
     *          2^{-10 x}, & x < 1 \\
     *          1, & x \ge 1
     *      \end{cases}
     * @f]
     *
     * @todo could be done better like with the sRGB curve, but should I
     *      bother?
     */
    static T exponentialOut(T t) {
        return t >= T(1.0) ? T(1.0) : T(1.0) - std::pow(T(2.0), -T(10.0)*t);
    }

    /**
     * @brief Exponential in and out
     *
     * Combination of @ref exponentialIn() and @ref exponentialOut(). Contrary
     * to Robert Penner's book but consistently with other implementations has
     * a special case for @f$ x \notin \{0, 1\} @f$, because
     * @f$ 2^{-10} = 0.0009765625 @f$ otherwise.
     *
     * @htmlinclude easings-exponentialinout.svg
     *
     * @f[
     *      y = \begin{cases}
     *          0, & x \le 0 \\
     *          \frac{1}{2} 2^{20 x - 10}, & x \in (0, 0.5) \\
     *          1 - \frac{1}{2} 2^{10 - 20 x}, & x \in [0.5, 1) \\
     *          1, & x \ge 1
     *      \end{cases}
     * @f]
     */
    static T exponentialInOut(T t) {
        if(t <= T(0.0)) return T(0.0);
        if(t < T(0.5)) return T(0.5)*std::pow(T(2.0), T(20.0)*t - T(10.0));
        if(t < T(1.0)) return T(1.0) - T(0.5)*std::pow(T(2.0), T(10.0) -T(20.0)*t);
        return T(1.0);
    }

    /**
     * @brief Elastic in
     *
     * Combines @ref sineIn() and @ref exponentialIn().
     *
     * @htmlinclude easings-elasticin.svg
     *
     * @f[
     *      y = 2^{10 (p - 1)} \sin(13 \frac{\pi}{2} x)
     * @f]
     */
    static T elasticIn(T t) {
        return std::pow(T(2.0), T(10.0)*(t - T(1.0)))*std::sin(T(13.0)*Math::Constants<T>::piHalf()*t);
    }

    /**
     * @brief Elastic out
     *
     * Combines @ref sineOut() and @ref exponentialOut().
     *
     * @htmlinclude easings-elasticout.svg
     *
     * @f[
     *      y = 1 - 2^{-10 x} \sin(13 \frac{\pi}{2} (x + 1))
     * @f]
     */
    static T elasticOut(T t) {
        return T(1.0) - std::pow(T(2.0), T(-10.0)*t)*std::sin(T(13.0)*Math::Constants<T>::piHalf()*(t + T(1.0)));
    }

    /**
     * @brief Elastic in and out
     *
     * Combination of @ref elasticIn() and @ref elasticOut() (or
     * @ref sineInOut() and @ref exponentialInOut()).
     *
     * @htmlinclude easings-elasticinout.svg
     *
     * @f[
     *      y = \begin{cases}
     *          \frac{1}{2} 2^{10 (2x - 1)} \sin(13 \pi x), & x < 0.5 \\
     *          1 - \frac{1}{2} 2^{10 (1 - 2x)} \sin(13 \pi x), & x \ge 0.5
     *      \end{cases}
     * @f]
     */
    static T elasticInOut(T t) {
        if(t < T(0.5))
            return T(0.5)*std::pow(T(2.0), T(10.0)*(T(2.0)*t - T(1.0)))*std::sin(T(13.0)*Math::Constants<T>::pi()*t);
        return T(1.0) - T(0.5)*std::pow(T(2.0), T(10.0)*(T(1.0) - T(2.0)*t))*std::sin(T(13.0)*Math::Constants<T>::pi()*t);
    }

    /**
     * @brief Back in
     *
     * @htmlinclude easings-backin.svg
     *
     * @f[
     *      y = x^3 - x \sin(\pi x)
     * @f]
     */
    static T backIn(T t) {
        return t*(t*t - std::sin(Math::Constants<T>::pi()*t));
    }

    /**
     * @brief Back out
     *
     * @htmlinclude easings-backout.svg
     *
     * @f[
     *      y = 1 - ((1 - x)^3 - (1 - x) \sin(\pi (1 - x)))
     * @f]
     */
    static T backOut(T t) {
        const T inv = T(1.0) - t;
        return 1 - inv*(inv*inv - std::sin(Math::Constants<T>::pi()*inv));
    }

    /**
     * @brief Back in and out
     *
     * Combination of @ref backIn() and @ref backOut().
     *
     * @htmlinclude easings-backinout.svg
     *
     * @f[
     *      y = \begin{cases}
     *          \frac{1}{2} ((2x)^3 - 2x \sin(2 \pi x)),
     *              & x < 0.5 \\
     *          1 - \frac{1}{2} ((2 - 2x)^3 - (2 - 2x) \sin(\pi (2 - 2x)),
     *              & x \ge 0.5
     *      \end{cases}
     * @f]
     */
    static T backInOut(T t) {
        if(t < T(0.5)) {
            const T t2 = T(2.0)*t;
            return T(0.5)*t2*(t2*t2 - std::sin(Math::Constants<T>::pi()*t2));
        }

        const T inv = T(2.0) - T(2.0)*t;
        return T(1.0) - T(0.5)*inv*(inv*inv - std::sin(Math::Constants<T>::pi()*inv));
    }

    /**
     * @brief Bounce in
     *
     * @htmlinclude easings-bouncein.svg
     */
    static T bounceIn(T t) {
        return T(1.0) - bounceOut(T(1.0) - t);
    }

    /**
     * @brief Bounce out
     *
     * @htmlinclude easings-bounceout.svg
     */
    static T bounceOut(T t) {
        if(t < T(4.0)/T(11.0)) return (T(121.0)*t*t)/T(16.0);
        if(t < T(8.0)/T(11.0)) return T(363.0)/T(40.0)*t*t - T(99.0)/T(10.0)*t + T(17.0)/T(5.0);
        if(t < T(9.0)/T(10.0)) return T(4356.0)/T(361.0)*t*t - T(35442.0)/T(1805.0)*t + T(16061.0)/T(1805.0);
        return T(54.0)/T(5.0)*t*t - T(513.0)/T(25.0)*t + T(268.0)/T(25.0);
    }

    /**
     * @brief Bounce in and out
     *
     * Combination of @ref bounceIn() and @ref bounceOut().
     *
     * @htmlinclude easings-bounceinout.svg
     */
    static T bounceInOut(T t) {
        if(t < T(0.5)) return T(0.5)*bounceIn(T(2.0)*t);
        return T(0.5)*bounceOut(T(2.0)*t - T(1.0)) + T(0.5);
    }
};

/**
@brief Float easing functions

@see @ref Easingd
*/
typedef BasicEasing<Float> Easing;

/**
@brief Double easing functions
@m_since_latest

@see @ref Easing
*/
typedef BasicEasing<Double> Easingd;

}}

#endif
