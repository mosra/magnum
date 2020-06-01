#ifndef Magnum_Animation_Interpolation_h
#define Magnum_Animation_Interpolation_h
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
 * @brief Alias @ref Magnum::Animation::ResultOf, enum @ref Magnum::Animation::Interpolation. @ref Magnum::Animation::Extrapolation, function @ref Magnum::Animation::interpolatorFor(), @ref Magnum::Animation::interpolate(), @ref Magnum::Animation::interpolateStrict(), @ref Magnum::Animation::ease(), @ref Magnum::Animation::easeClamped() @ref Magnum::Animation::unpack(), @ref Magnum::Animation::unpackEase(), @ref Magnum::Animation::unpackEaseClamped()
 */

#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Animation/Animation.h"

namespace Magnum { namespace Animation {

/**
@brief Animation interpolation

Describes the general desired way to interpolate animation keyframes. The
concrete choice of interpolator function is in user's hands.
@see @ref interpolatorFor()
@experimental
*/
enum class Interpolation: UnsignedByte {
    /**
     * Constant interpolation.
     *
     * @see @ref Math::select()
     */
    Constant,

    /**
     * Linear interpolation.
     *
     * @see @ref Math::lerp(), @ref Math::slerp(), @ref Math::sclerp()
     */
    Linear,

    /**
     * Spline interpolation.
     *
     * @see @ref Math::splerp()
     */
    Spline,

    /**
     * Custom interpolation. An user-supplied interpolation function should be
     * used.
     */
    Custom
};

/** @debugoperatorenum{Interpolation} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Interpolation value);

/**
@brief Animation result type for given value type

Result of interpolating two `V` values (for example interpolating two
@ref Color3 values gives back a @ref Color3 again, but interpolating a
@ref Magnum::CubicHermite2D "CubicHermite2D" spline results in
@ref Magnum::Vector2 "Vector2").
@experimental
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class V> using ResultOf = typename Implementation::ResultTraits<V>::Type;
#endif

/**
@brief Interpolator function for given type
@tparam V           Value type
@tparam R           Result type

Expects that @p interpolation is not @ref Interpolation::Custom. Favors
output correctness over performance, supply custom interpolator functions for
faster but potentially less correct results.

@m_class{m-fullwidth}

Interpolation       | Value type        | Result type   | Interpolator
------------------- | ----------------- | ------------- | ------------
@ref Interpolation::Constant "Constant" | any `V`  | `V`           | @ref Math::select()
@ref Interpolation::Constant "Constant" | @ref Math::CubicHermite "Math::CubicHermite<T>"  | `T` | @ref Math::select(const CubicHermite<T>&, const CubicHermite<T>&, U) "Math::select()"
@ref Interpolation::Linear "Linear" | @cpp bool @ce <b></b> | @cpp bool @ce <b></b> | @ref Math::select()
@ref Interpolation::Linear "Linear" | @ref Math::BoolVector | @ref Math::BoolVector | @ref Math::select()
@ref Interpolation::Linear "Linear" | any scalar `V` | `V`       | @ref Math::lerp()
@ref Interpolation::Linear "Linear" | any vector `V` | `V`       | @ref Math::lerp()
@ref Interpolation::Linear "Linear" | @ref Math::Complex | @ref Math::Complex | @ref Math::slerp(const Complex<T>&, const Complex<T>&, T) "Math::slerp()"
@ref Interpolation::Linear "Linear" | @ref Math::Quaternion | @ref Math::Quaternion | @ref Math::slerpShortestPath(const Quaternion<T>&, const Quaternion<T>&, T) "Math::slerpShortestPath()"
@ref Interpolation::Linear "Linear" | @ref Math::DualQuaternion | @ref Math::DualQuaternion | @ref Math::sclerpShortestPath(const DualQuaternion<T>&, const DualQuaternion<T>&, T) "Math::sclerpShortestPath()"
@ref Interpolation::Linear "Linear" | @ref Math::CubicHermite "Math::CubicHermite<T>" | `T` | @ref Math::lerp(const CubicHermite<T>&, const CubicHermite<T>&, U) "Math::lerp()"
@ref Interpolation::Linear "Linear" | @ref Math::CubicHermiteComplex | @ref Math::Complex | @ref Math::lerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T) "Math::lerp()"
@ref Interpolation::Linear "Linear" | @ref Math::CubicHermiteQuaternion | @ref Math::Quaternion | @ref Math::lerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T) "Math::lerp()"
@ref Interpolation::Spline "Spline" | @ref Math::CubicHermite "Math::CubicHermite<T>" | `T` | @ref Math::splerp(const CubicHermite<T>&, const CubicHermite<T>&, U) "Math::splerp()"
@ref Interpolation::Spline "Spline" | @ref Math::CubicHermiteComplex | @ref Math::Complex | @ref Math::splerp(const CubicHermiteComplex<T>&, const CubicHermiteComplex<T>&, T) "Math::splerp()"
@ref Interpolation::Spline "Spline" | @ref Math::CubicHermiteQuaternion | @ref Math::Quaternion | @ref Math::splerp(const CubicHermiteQuaternion<T>&, const CubicHermiteQuaternion<T>&, T) "Math::splerp()"

@see @ref interpolate(), @ref interpolateStrict(),
    @ref transformations-interpolation, @ref Trade::animationInterpolatorFor()
@experimental
*/
template<class V, class R = ResultOf<V>> auto interpolatorFor(Interpolation interpolation) -> R(*)(const V&, const V&, Float);

/**
@brief Animation extrapolation behavior

Describes what value is returned for frames outside of keyframe range for given
track (frame lower than first keyframe or frame larger or equal to last
keyframe).
@see @ref interpolate(), @ref Track::before(), @ref Track::after(),
    @ref TrackView::before(), @ref TrackView::after()
@experimental
*/
enum class Extrapolation: UnsignedByte {
    /**
     * Values of first two / last two keyframes are extrapolated. In case
     * there is only one keyframe, it's passed to both inputs of the
     * interpolator. Implicit behavior in @ref interpolateStrict().
     */
    Extrapolated,

    /**
     * Value of first/last keyframe is used. In other words, for the first
     * keyframe the interpolator is called with first two keyframes and
     * interpolation factor set to `0.0f`; for the last keyframe the
     * interpolator is called with last two keyframes and interpolation factor
     * set to `1.0f`. In case there is only one keyframe, it's passed to both
     * inputs of the interpolator.
     */
    Constant,

    /** Default-constructed value is returned. */
    DefaultConstructed,

    /** @todo repeat? that would duplicate the play count feature though */
};

/** @debugoperatorenum{Extrapolation} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Extrapolation value);

/**
@brief Interpolate animation value
@tparam K           Key type
@tparam V           Value type
@tparam R           Result type
@param keys         Keys
@param values       Values
@param before       Extrapolation mode before first keyframe
@param after        Extrapolation mode after last keyframe
@param interpolator Interpolator function
@param frame        Frame at which to interpolate
@param hint         Hint for keyframe search

Does a linear search over the keyframes until it finds last keyframe which is
not larger than @p frame. Once the keyframe is found, reference to it and the immediately following keyframe is passed to @p interpolator along with
calculated interpolation factor, returning the interpolated value.

-   In case the first keyframe is already larger than @p frame or @p frame is
    larger or equal to the last keyframe, either the first two or last two
    keyframes are used and value is extrapolated according to @p before /
    @p after.
-   In case only one keyframe is present, its value is used for both sides of
    the interpolator.
-   In case no keyframes are present, default-constructed value is returned.

The @p hint parameter hints where to start the linear search and is updated
with keyframe index matching @p frame. If @p frame is earlier than @p hint, the
search is restarted from the beginning.

Used internally from @ref Track::at() / @ref TrackView::at(), see @ref Track
documentation for more information.

@see @ref interpolateStrict(), @ref Math::select(), @ref Math::lerp(),
    @ref Math::slerp(), @ref Math::sclerp()
@experimental
*/
template<class K, class V, class R = ResultOf<V>> R interpolate(const Containers::StridedArrayView1D<const K>& keys, const Containers::StridedArrayView1D<const V>& values, Extrapolation before, Extrapolation after, R(*interpolator)(const V&, const V&, Float), K frame, std::size_t& hint);

/**
@brief Interpolate animation value with strict constraints

Does a linear search over the keyframes until it finds last keyframe which is
not larger than @p frame. Once the keyframe is found, reference to it and the immediately following keyframe is passed to @p interpolator along with
calculated interpolation factor, returning the interpolated value. The @p hint
parameter hints where to start the linear search and is updated with keyframe
index matching @p frame. If @p frame is earlier than @p hint, the search is
restarted from the beginning.

This is a stricter but more performant version of @ref interpolate() with
implicit @ref Extrapolation::Extrapolated behavior. Expects that there are
always at least two keyframes.

Used internally from @ref Track::atStrict() / @ref TrackView::atStrict(), see
@ref Track documentation for more information.

@see @ref Math::select(), @ref Math::lerp(), @ref Math::slerp(),
    @ref Math::sclerp()
@experimental
*/
template<class K, class V, class R = ResultOf<V>> R interpolateStrict(const Containers::StridedArrayView1D<const K>& keys, const Containers::StridedArrayView1D<const V>& values, R(*interpolator)(const V&, const V&, Float), K frame, std::size_t& hint);

/**
@brief Combine easing function and an interpolator

Useful to create a new function out of one of the interpolators from
@ref transformations-interpolation and an easing function from @ref Easing. For
example, the following two expressions give the same result:

@snippet MagnumAnimation.cpp ease

@see @ref unpack(), @ref unpackEase()
*/
template<class V, ResultOf<V>(*interpolator)(const V&, const V&, Float), Float(*easer)(Float)> constexpr auto ease() -> ResultOf<V>(*)(const V&, const V&, Float) {
    return [](const V& a, const V& b, Float t) { return interpolator(a, b, easer(t)); };
}

/**
@brief Combine easing function and an interpolator

In addition to @ref ease() clamps value coming to @p easer to range
@f$ [0 ; 1] @f$. Useful when extrapolating using @ref Easing functions that
have bad behavior outside of this range.
*/
template<class V, ResultOf<V>(*interpolator)(const V&, const V&, Float), Float(*easer)(Float)> constexpr auto easeClamped() -> ResultOf<V>(*)(const V&, const V&, Float) {
    return [](const V& a, const V& b, Float t) { return interpolator(a, b, easer(Math::clamp(t, 0.0f, 1.0f))); };
}

/**
@brief Combine unpacking function and an interpolator

Similar to @ref ease(), but for adding an unpacker function to interpolator
inputs instead of modifying the interpolator phase. The following two
expressions give the same result:

@snippet MagnumAnimation.cpp unpack

@see @ref unpackEase()
*/
template<class T, class V, ResultOf<V>(*interpolator)(const V&, const V&, Float), V(*unpacker)(const T&)> constexpr auto unpack() -> ResultOf<V>(*)(const V&, const V&, Float) {
    return [](const V& a, const V& b, Float t) { return interpolator(unpacker(a), unpacker(b), t); };
}

/**
@brief Combine unpacking and easing functions with an interpolator

Combination of @ref ease() and @ref unpack(), creating a function that first
unpack the interpolator inputs, then modifies the interpolator phase and
finally passes that to the interpolator function. The following two expressions
give the same result:

@snippet MagnumAnimation.cpp unpackEase
*/
template<class T, class V, ResultOf<V>(*interpolator)(const V&, const V&, Float), V(*unpacker)(const T&), Float(*easer)(Float)> constexpr auto unpackEase() -> ResultOf<V>(*)(const V&, const V&, Float) {
    return [](const V& a, const V& b, Float t) { return interpolator(unpacker(a), unpacker(b), easer(t)); };
}

/**
@brief Combine easing function and an interpolator

In addition to @ref unpackEase() clamps value coming to @p easer to range
@f$ [0 ; 1] @f$. Useful when extrapolating with @ref Easing functions that have
bad behavior outside of this range.
*/
template<class T, class V, ResultOf<V>(*interpolator)(const V&, const V&, Float), V(*unpacker)(const T&), Float(*easer)(Float)> constexpr auto unpackEaseClamped() -> ResultOf<V>(*)(const V&, const V&, Float) {
    return [](const V& a, const V& b, Float t) { return interpolator(unpacker(a), unpacker(b), easer(Math::clamp(t, 0.0f, 1.0f))); };
}

namespace Implementation {

/* Generic types where result type is the same as value type */
template<class V> struct ResultTraits {
    typedef typename std::remove_const<V>::type Type;
};
template<class T> struct ResultTraits<Math::CubicHermite<T>> {
    typedef T Type;
};
template<class T> struct ResultTraits<const Math::CubicHermite<T>> {
    typedef T Type;
};
template<class V> struct TypeTraits<V, V> {
    typedef V(*Interpolator)(const V&, const V&, Float);

    static Interpolator interpolator(Interpolation interpolation);
};
template<class V> auto TypeTraits<V, V>::interpolator(Interpolation interpolation) -> Interpolator {
    switch(interpolation) {
        case Interpolation::Constant: return Math::select;
        case Interpolation::Linear: return Math::lerp;

        case Interpolation::Spline:
        case Interpolation::Custom: ; /* nope */
    }

    CORRADE_ASSERT_UNREACHABLE("Animation::interpolatorFor(): can't deduce interpolator function for" << interpolation, {});
}

/* Specialization for booleans (no linear interpolation) */
template<class T> struct TypeTraitsBool {
    typedef T(*Interpolator)(const T&, const T&, Float);

    static Interpolator interpolator(Interpolation interpolation);
};
template<class T> auto TypeTraitsBool<T>::interpolator(Interpolation interpolation) -> Interpolator {
    switch(interpolation) {
        case Interpolation::Constant:
        case Interpolation::Linear: return Math::select;

        case Interpolation::Spline:
        case Interpolation::Custom: ; /* nope */
    }

    CORRADE_ASSERT_UNREACHABLE("Animation::interpolatorFor(): can't deduce interpolator function for" << interpolation, {});
}
template<> struct TypeTraits<bool, bool>: TypeTraitsBool<bool> {};
template<std::size_t size> struct TypeTraits<Math::BoolVector<size>, Math::BoolVector<size>>: TypeTraitsBool<Math::BoolVector<size>> {};

/* Complex, preferring slerp() as it is more precise */
template<class T> struct
#ifndef CORRADE_TARGET_CLANG_CL
/* Clang-CL complains that it's ignored if it's on the class, so putting it
   on the function instead. However MSVC doesn't like that, so doing this only
   for Clang-CL. */
MAGNUM_EXPORT
#endif
TypeTraits<Math::Complex<T>, Math::Complex<T>> {
    typedef Math::Complex<T>(*Interpolator)(const Math::Complex<T>&, const Math::Complex<T>&, Float);

    static
    #ifdef CORRADE_TARGET_CLANG_CL
    MAGNUM_EXPORT
    #endif
    Interpolator interpolator(Interpolation interpolation);
};

/* Quaternions and dual quaternions, preferring slerp() as it is more precise */
template<class T> struct
#ifndef CORRADE_TARGET_CLANG_CL
/* Clang-CL complains that it's ignored if it's on the class, so putting it
   on the function instead. However MSVC doesn't like that, so doing this only
   for Clang-CL. */
MAGNUM_EXPORT
#endif
TypeTraits<Math::Quaternion<T>, Math::Quaternion<T>> {
    typedef Math::Quaternion<T>(*Interpolator)(const Math::Quaternion<T>&, const Math::Quaternion<T>&, Float);

    static
    #ifdef CORRADE_TARGET_CLANG_CL
    MAGNUM_EXPORT
    #endif
    Interpolator interpolator(Interpolation interpolation);
};
template<class T> struct
#ifndef CORRADE_TARGET_CLANG_CL
/* Clang-CL complains that it's ignored if it's on the class, so putting it
   on the function instead. However MSVC doesn't like that, so doing this only
   for Clang-CL. */
MAGNUM_EXPORT
#endif
TypeTraits<Math::DualQuaternion<T>, Math::DualQuaternion<T>> {
    typedef Math::DualQuaternion<T>(*Interpolator)(const Math::DualQuaternion<T>&, const Math::DualQuaternion<T>&, Float);

    static
    #ifdef CORRADE_TARGET_CLANG_CL
    MAGNUM_EXPORT
    #endif
    Interpolator interpolator(Interpolation interpolation);
};

/* Cubic Hermite spline point has a different result type */
template<class T> struct
#ifndef CORRADE_TARGET_CLANG_CL
/* Clang-CL complains that it's ignored if it's on the class, so putting it
   on the function instead. However MSVC doesn't like that, so doing this only
   for Clang-CL. */
MAGNUM_EXPORT
#endif
TypeTraits<Math::CubicHermite<T>, T> {
    typedef T(*Interpolator)(const Math::CubicHermite<T>&, const Math::CubicHermite<T>&, Float);

    static
    #ifdef CORRADE_TARGET_CLANG_CL
    MAGNUM_EXPORT
    #endif
    Interpolator interpolator(Interpolation interpolation);
};

}

/* Needs to be defined later so it can pick up the TypeTraits definitions */
template<class V, class R> auto interpolatorFor(Interpolation interpolation) -> R(*)(const V&, const V&, Float) {
    return Implementation::TypeTraits<typename std::remove_const<V>::type, R>::interpolator(interpolation);
}

template<class K, class V, class R> R interpolate(const Containers::StridedArrayView1D<const K>& keys, const Containers::StridedArrayView1D<const V>& values, const Extrapolation before, const Extrapolation after, R(*const interpolator)(const V&, const V&, Float), K frame, std::size_t& hint) {
    CORRADE_ASSERT(keys.size() == values.size(), "Animation::interpolate(): keys and values don't have the same size", {});

    /* No data, return default-constructed value */
    if(!keys.size()) return {};

    /* Only one frame, return it verbatim (or default-constructed, if desired) */
    if(keys.size() == 1) {
        if((frame < keys[0] && before == Extrapolation::DefaultConstructed) ||
           (frame > keys[0] && after == Extrapolation::DefaultConstructed))
            return {};

        return interpolator(values[0], values[0], 0.0f);
    }

    /* Rewind from the beginning if hint is too late */
    if(hint >= keys.size() || frame < keys[hint]) hint = 0;

    /* Go through the keys until we find a pair that is around given time */
    while(hint + 2 < keys.size() && frame >= keys[hint + 1])
        ++hint;

    /* Special extrapolation outside of range. Usual extrapolation is handled
       below. */
    if(frame < keys[hint]) {
        if(before == Extrapolation::DefaultConstructed) return {};
        if(before == Extrapolation::Constant) frame = keys[hint];
    } else if(frame >= keys[hint + 1]) {
        if(after == Extrapolation::DefaultConstructed) return {};
        if(after == Extrapolation::Constant) frame = keys[hint + 1];
    }

    return interpolator(values[hint], values[hint + 1],
        Math::lerpInverted(Float(keys[hint]), Float(keys[hint + 1]), Float(frame)));
}

template<class K, class V, class R> R interpolateStrict(const Containers::StridedArrayView1D<const K>& keys, const Containers::StridedArrayView1D<const V>& values, R(*const interpolator)(const V&, const V&, Float), const K frame, std::size_t& hint) {
    CORRADE_ASSERT(keys.size() >= 2, "Animation::interpolateStrict(): at least two keyframes required", {});
    CORRADE_ASSERT(keys.size() == values.size(), "Animation::interpolateStrict(): keys and values don't have the same size", {});

    /* Rewind from the beginning if hint is too late */
    if(hint >= keys.size() || frame < keys[hint]) hint = 0;

    /* Go through the keys until we find a pair that is around given time */
    while(hint + 2 < keys.size() && frame >= keys[hint + 1])
        ++hint;

    return interpolator(values[hint], values[hint + 1],
        Math::lerpInverted(Float(keys[hint]), Float(keys[hint + 1]), Float(frame)));
}

}}

#endif
