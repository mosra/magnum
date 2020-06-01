#ifndef Magnum_Animation_Easing_h
#define Magnum_Animation_Easing_h
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
 * @brief Namespace @ref Magnum::Animation::Easing
 */

#include "Magnum/Magnum.h"
#include "Magnum/Math/Constants.h"
#include "Magnum/Animation/Animation.h"

namespace Magnum { namespace Animation {

/**
@brief Easing functions

@m_keywords{Tweening Inbetweening}

A collection of predefined [easing / tweening](https://en.wikipedia.org/wiki/Inbetweening)
functions for adding life to animation interpolation.

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

@snippet MagnumAnimation.cpp Easing-factor

The @ref Animation library also provides the @ref ease() utility that combines
the interpolator together with the easing function:

@snippet MagnumAnimation.cpp Easing-ease

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

@snippet MagnumAnimation.cpp Easing-clamp

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

@snippet MagnumAnimation.cpp Easing-bezier-transform

@section Animation-Easing-references References

Functions follow the common naming from Robert Penner's Easing functions,
http://robertpenner.com/easing/. Implementation based on and inspired by
https://easings.net/,
[https://github.com/warrenm/AHEasing](https://github.com/warrenm/AHEasing/blob/master/AHEasing/easing.c),
[https://github.com/bkaradzic/bx](https://github.com/bkaradzic/bx/blob/master/include/bx/inline/easing.inl),
https://blog.demofox.org/2014/08/28/one-dimensional-bezier-curves/.
*/
namespace Easing {

/**
@brief Linear

@htmlinclude easings-linear.svg

@f[
    y = x
@f]

One possible *exact* Bézier representation:

@snippet easings.cpp linear

*/
inline Float linear(Float t) { return t; }

/**
@brief Step

Similar to @ref Math::select(), but does the step in the middle of the range
instead of at the end. Implementation matching the GLSL @glsl step() @ce
function with @cpp edge = 0.5f @ce.

@htmlinclude easings-step.svg

@f[
    y = \begin{cases}
        0, & x < 0.5 \\
        1, & x \ge 0.5
    \end{cases}
@f]

@m_keyword{step(),GLSL step(),}
@see @ref smoothstep(), @ref smootherstep()
*/
inline Float step(Float t) { return t < 0.5f ? 0.0f : 1.0f; }

/**
@brief [Smoothstep](https://en.wikipedia.org/wiki/Smoothstep).

Implementation matching the GLSL @glsl smoothstep() @ce function. Combine with
@ref Math::lerp() to get the equivalent result:

@snippet MagnumAnimation.cpp Easing-smoothstep

@htmlinclude easings-smoothstep.svg

@f[
    y = \begin{cases}
        0, & x < 0 \\
        3x^2 - 2x^3, & x \in [0, 1] \\
        1, & x > 1
    \end{cases}
@f]

* *Exact* Bézier representation:

@snippet easings.cpp smoothstep

@m_keyword{smoothstep(),GLSL smoothstep(),}
@see @ref smootherstep(), @ref Math::clamp()
*/
inline Float smoothstep(Float t) {
    /* Deliberately *not* using Math::clamp() because that would drag in
       unneeded vector headers */
    if(t <= 0.0f) return 0.0f;
    if(t >= 1.0f) return 1.0f;
    return (3.0f - 2.0f*t)*t*t;
}

/**
@brief [Smootherstep](https://en.wikipedia.org/wiki/Smoothstep#Variations).

Improved version of @ref smoothstep() by [Ken Perlin](https://en.wikipedia.org/wiki/Ken_Perlin).

@htmlinclude easings-smootherstep.svg

@f[
    y = \begin{cases}
        0, & x < 0 \\
        6x^5 - 15x^4 + 10x^3, & x \in [0, 1] \\
        1, & x > 1
    \end{cases}
@f]

@see @ref Math::clamp()
*/
inline Float smootherstep(Float t) {
    /* Deliberately *not* using Math::clamp() because that would drag in
       unneeded vector headers */
    if(t <= 0.0f) return 0.0f;
    if(t >= 1.0f) return 1.0f;
    return t*t*t*(t*(t* 6.0f - 15.0f) + 10.0f);
}

/**
@brief Quadratic in

@htmlinclude easings-quadraticin.svg

@f[
    y = x^2
@f]

* *Exact* Bézier representation:

@snippet easings.cpp quadraticIn

@see @ref cubicIn(), @ref quarticIn(), @ref quinticIn()
*/
inline Float quadraticIn(Float t) { return t*t; }

/**
@brief Quadratic out

@htmlinclude easings-quadraticout.svg

@f[
    y = 1 - (1 - x)^2 = (2 - x) x
@f]

* *Exact* Bézier representation:

@snippet easings.cpp quadraticOut

@see @ref cubicOut(), @ref quarticOut(), @ref quinticOut()
*/
inline Float quadraticOut(Float t) { return -t*(t - 2.0f); }

/**
@brief Quadratic in and out

Combination of @ref quadraticIn() and @ref quadraticOut().

@htmlinclude easings-quadraticinout.svg

@f[
    y = \begin{cases}
        2 x^2, & x < 0.5 \\
        1 - 2 (1 - x)^2, & x \ge 0.5
    \end{cases}
@f]

Approximate Bézier representation:

@snippet easings.cpp quadraticInOut

@see @ref cubicInOut(), @ref quarticInOut(), @ref quinticInOut()
*/
inline Float quadraticInOut(Float t) {
    if(t < 0.5f) return 2.0f*t*t;

    const Float inv = 1.0f - t;
    return 1.0f - 2.0f*inv*inv;
}

/**
@brief Cubic in

@htmlinclude easings-cubicin.svg

@f[
    y = x^3
@f]

* *Exact* Bézier representation:

@snippet easings.cpp cubicIn

@see @ref quadraticIn(), @ref quarticIn(), @ref quinticIn()
*/
inline Float cubicIn(Float t) { return t*t*t; }

/**
@brief Cubic out

@htmlinclude easings-cubicout.svg

@f[
    y = 1 - (1 - x)^3
@f]

* *Exact* Bézier representation:

@snippet easings.cpp cubicOut

@see @ref quadraticOut(), @ref quarticOut(), @ref quinticOut()
*/
inline Float cubicOut(Float t) {
    const Float inv = t - 1.0f;
    return inv*inv*inv + 1.0f;
}

/**
@brief Cubic in and out

Combination of @ref cubicIn() and @ref cubicOut().

@htmlinclude easings-cubicinout.svg

@f[
    y = \begin{cases}
        4 x^3, & x < 0.5 \\
        1 - 4 (1 - x)^3, & x \ge 0.5
    \end{cases}
@f]

Approximate Bézier representation:

@snippet easings.cpp cubicInOut

@see @ref quadraticInOut(), @ref quarticInOut(), @ref quinticInOut()
*/
inline Float cubicInOut(Float t) {
    if(t < 0.5f) return 4.0f*t*t*t;

    const Float inv = 1.0f - t;
    return 1.0f - 4.0f*inv*inv*inv;
}

/**
@brief Quartic in

@htmlinclude easings-quarticin.svg

@f[
    y = x^4
@f]

@see @ref quadraticIn(), @ref cubicIn(), @ref quinticIn()
*/
inline Float quarticIn(Float t) {
    /* Not just t*t*t*t, since compile can't optimize it on its own to just two
       multiplication without breaking precision. So doing that explicitly. */
    const Float tt = t*t;
    return tt*tt;
}

/**
@brief Quartic out

@htmlinclude easings-quarticout.svg

@f[
    y = 1 - (1 - x)^4
@f]

@see @ref quadraticOut(), @ref cubicOut(), @ref quinticOut()
*/
inline Float quarticOut(Float t) {
    /* Instead of t*t*t*t suggesting the optimization as described above */
    const Float inv = 1.0f - t;
    const Float quad = inv*inv;
    return 1.0f - quad*quad;
}

/**
@brief Quartic in and out

Combination of @ref quarticIn() and @ref quarticOut().

@htmlinclude easings-quarticinout.svg

@f[
    y = \begin{cases}
        8 x^4, & x < 0.5 \\
        1 - 8 (1 - x)^4, & x \ge 0.5
    \end{cases}
@f]

@see @ref quadraticInOut(), @ref cubicInOut(), @ref quinticInOut()
*/
inline Float quarticInOut(Float t) {
    /* Instead of t*t*t*t suggesting the optimization as described above */

    if(t < 0.5f) {
        const Float tt = t*t;
        return 8*tt*tt;
    }

    const Float inv = 1.0f - t;
    const Float quad = inv*inv;
    return 1.0f - 8.0f*quad*quad;
}

/**
@brief Quintic in

@htmlinclude easings-quinticin.svg

@f[
    y = x^5
@f]

@see @ref quadraticIn(), @ref cubicIn(), @ref quarticIn()
*/
inline Float quinticIn(Float t) {
    /* Instead of t*t*t*t*t suggesting the optimization as described above */
    const Float tt = t*t;
    return tt*t*tt;
}

/**
@brief Quintic out

@htmlinclude easings-quinticout.svg

@f[
    y = 1 - (1 - x)^5
@f]

@see @ref quadraticOut(), @ref cubicOut(), @ref quarticOut()
*/
inline Float quinticOut(Float t) {
    /* Instead of t*t*t*t*t suggesting the optimization as described above */
    const Float inv = t - 1.0f;
    const Float quad = inv*inv;
    return 1.0f + quad*inv*quad;
}

/**
@brief Quintic in and out

Combination of @ref quinticIn() and @ref quinticOut().

@htmlinclude easings-quinticinout.svg

@f[
    y = \begin{cases}
        16 x^5, & x < 0.5 \\
        1 - 16 (1 - x)^5, & x \ge 0.5
    \end{cases}
@f]

@see @ref quadraticInOut(), @ref cubicInOut(), @ref quarticInOut()
*/
inline Float quinticInOut(Float t) {
    /* Instead of t*t*t*t*t suggesting the optimization as described above */

    if(t < 0.5f) {
        const Float tt = t*t;
        return 16.0f*tt*t*tt;
    }

    const Float inv = 1.0f - t;
    const Float quad = inv*inv;
    return 1.0f - 16.0f*quad*inv*quad;
}

/**
@brief Sine in

@htmlinclude easings-sinein.svg

@f[
    y = 1 + \sin(\frac{\pi}{2} (x - 1))
@f]

@see @ref circularIn()
*/
inline Float sineIn(Float t) {
    return 1.0f + std::sin(Constants::piHalf()*(t - 1.0f));
}

/**
@brief Sine out

@htmlinclude easings-sineout.svg

@f[
    y = \sin(\frac{\pi}{2} x)
@f]

@see @ref circularOut()
*/
inline Float sineOut(Float t) {
    return std::sin(Constants::piHalf()*t);
}

/**
@brief Sine in and out

Combination of @ref sineIn() and @ref sineOut().

@htmlinclude easings-sineinout.svg

@f[
    y = \frac{1}{2} (1 - \cos(\pi x))
@f]

@see @ref circularInOut()
*/
inline Float sineInOut(Float t) {
    return 0.5f*(1.0f - std::cos(t*Constants::pi()));
}

/**
@brief Circular in

@htmlinclude easings-circularin.svg

@f[
    y = 1 - \sqrt{1 - x^2}
@f]

@see @ref sineIn()
*/
inline Float circularIn(Float t) {
    return 1.0f - std::sqrt(1.0f - t*t);
}

/**
@brief Circular out

@htmlinclude easings-circularout.svg

@f[
    y = \sqrt{(2 - x) x}
@f]

@see @ref sineOut()
*/
inline Float circularOut(Float t) {
    return std::sqrt((2.0f - t)*t);
}

/**
@brief Circular in and out

Combination of @ref circularIn() and @ref circularOut().

@htmlinclude easings-circularinout.svg

@f[
    y = \begin{cases}
        \frac{1}{2} (1 - \sqrt{1 - (2x)^2}), & x < 0.5 \\
        \frac{1}{2} (1 + \sqrt{1 - (2x - 2)^2}), & x \ge 0.5
    \end{cases}
@f]

@see @ref sineInOut()
*/
inline Float circularInOut(Float t) {
    if(t < 0.5f) return 0.5f*(1.0f - std::sqrt(1.0f - 4.0f*t*t));
    return 0.5f*(1.0f + std::sqrt(-4.0f*t*t + 8.0f*t - 3.0f));
}

/**
@brief Exponential in

Contrary to Robert Penner's book but consistently with other implementations
has a special case for @f$ x \le 0 @f$, because @f$ 2^{-10} = 0.0009765625 @f$
otherwise.

@htmlinclude easings-exponentialin.svg

@f[
    y = \begin{cases}
        0, & x \le 0 \\
        2^{10(x - 1)}, & x \ne 0
    \end{cases}
@f]

@todo could be done better like with the sRGB curve, but should I bother?
*/
inline Float exponentialIn(Float t) {
    return t <= 0.0f ? 0.0f : std::pow(2.0f, 10.0f * (t - 1.0f));
}

/**
@brief Exponential out

Contrary to Robert Penner's book but consistently with other implementations
has a special case for @f$ x \ge 1 @f$, because @f$ 2^{-10} = 0.0009765625 @f$
otherwise.

@htmlinclude easings-exponentialout.svg

@f[
    y = \begin{cases}
        2^{-10 x}, & x < 1 \\
        1, & x \ge 1
    \end{cases}
@f]

@todo could be done better like with the sRGB curve, but should I bother?
*/
inline Float exponentialOut(Float t) {
    return t >= 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f*t);
}

/**
@brief Exponential in and out

Combination of @ref exponentialIn() and @ref exponentialOut(). Contrary to
Robert Penner's book but consistently with other implementations has a special
case for @f$ x \notin \{0, 1\} @f$, because @f$ 2^{-10} = 0.0009765625 @f$
otherwise.

@htmlinclude easings-exponentialinout.svg

@f[
    y = \begin{cases}
        0, & x \le 0 \\
        \frac{1}{2} 2^{20 x - 10}, & x \in (0, 0.5) \\
        1 - \frac{1}{2} 2^{10 - 20 x}, & x \in [0.5, 1) \\
        1, & x \ge 1
    \end{cases}
@f]
*/
inline Float exponentialInOut(Float t) {
    if(t <= 0.0f) return 0.0f;
    if(t < 0.5f) return 0.5f*std::pow(2.0f, 20.0f*t - 10.0f);
    if(t < 1.0f) return 1.0f - 0.5f*std::pow(2.0f, 10.0f - 20.0f*t);
    return 1.0f;
}

/**
@brief Elastic in

Combines @ref sineIn() and @ref exponentialIn().

@htmlinclude easings-elasticin.svg

@f[
    y = 2^{10 (p - 1)} \sin(13 \frac{\pi}{2} x)
@f]
*/
inline Float elasticIn(Float t) {
    return std::pow(2.0f, 10.0f*(t - 1.0f))*std::sin(13.0f*Constants::piHalf()*t);
}

/**
@brief Elastic out

Combines @ref sineOut() and @ref exponentialOut().

@htmlinclude easings-elasticout.svg

@f[
    y = 1 - 2^{-10 x} \sin(13 \frac{\pi}{2} (x + 1))
@f]
*/
inline Float elasticOut(Float t) {
    return 1.0f - std::pow(2.0f, -10.0f*t)*std::sin(13.0f*Constants::piHalf()*(t + 1.0f));
}

/**
@brief Elastic in and out

Combination of @ref elasticIn() and @ref elasticOut() (or @ref sineInOut() and
@ref exponentialInOut()).

@htmlinclude easings-elasticinout.svg

@f[
    y = \begin{cases}
        \frac{1}{2} 2^{10 (2x - 1)} \sin(13 \pi x), & x < 0.5 \\
        1 - \frac{1}{2} 2^{10 (1 - 2x)} \sin(13 \pi x), & x \ge 0.5
    \end{cases}
@f]
*/
inline Float elasticInOut(Float t) {
    if(t < 0.5f)
        return 0.5f*std::pow(2.0f, 10.0f*(2.0f*t - 1.0f))*std::sin(13.0f*Constants::pi()*t);
    return 1.0f - 0.5f*std::pow(2.0f, 10.0f*(1.0f - 2.0f*t))*std::sin(13.0f*Constants::pi()*t);
}

/**
@brief Back in

@htmlinclude easings-backin.svg

@f[
    y = x^3 - x \sin(\pi x)
@f]
*/
inline Float backIn(Float t) {
    return t*(t*t - std::sin(Constants::pi()*t));
}

/**
@brief Back out

@htmlinclude easings-backout.svg

@f[
    y = 1 - ((1 - x)^3 - (1 - x) \sin(\pi (1 - x)))
@f]
*/
inline Float backOut(Float t) {
    const Float inv = 1.0f - t;
    return 1 - inv*(inv*inv - std::sin(Constants::pi()*inv));
}

/**
@brief Back in and out

Combination of @ref backIn() and @ref backOut().

@htmlinclude easings-backinout.svg

@f[
    y = \begin{cases}
        \frac{1}{2} ((2x)^3 - 2x \sin(2 \pi x)), & x < 0.5 \\
        1 - \frac{1}{2} ((2 - 2x)^3 - (2 - 2x) \sin(\pi (2 - 2x)), & x \ge 0.5
    \end{cases}
@f]
*/
inline Float backInOut(Float t) {
    if(t < 0.5f) {
        const Float t2 = 2.0f*t;
        return 0.5f*t2*(t2*t2 - std::sin(Constants::pi()*t2));
    }

    const Float inv = 2.0f - 2.0f*t;
    return 1.0f - 0.5f*inv*(inv*inv - std::sin(Constants::pi()*inv));
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Float bounceOut(Float);
#endif

/**
@brief Bounce in

@htmlinclude easings-bouncein.svg
*/
inline Float bounceIn(Float t) {
    return 1.0f - bounceOut(1.0f - t);
}

/**
@brief Bounce out

@htmlinclude easings-bounceout.svg
*/
inline Float bounceOut(Float t) {
    if(t < 4.0f/11.0f) return (121.0f*t*t)/16.0f;
    if(t < 8.0f/11.0f) return 363.0f/40.0f*t*t - 99.0f/10.0f*t + 17.0f/5.0f;
    if(t < 9.0f/10.0f) return 4356.0f/361.0f*t*t - 35442.0f/1805.0f*t + 16061.0f/1805.0f;
    return 54.0f/5.0f*t*t - 513.0f/25.0f*t + 268.0f/25.0f;
}

/**
@brief Bounce in and out

Combination of @ref bounceIn() and @ref bounceOut().

@htmlinclude easings-bounceinout.svg
*/
inline Float bounceInOut(Float t) {
    if(t < 0.5f) return 0.5f*bounceIn(2.0f*t);
    return 0.5f*bounceOut(2.0f*t - 1.0f) + 0.5f;
}

}}}

#endif
