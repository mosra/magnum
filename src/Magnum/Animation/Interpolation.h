#ifndef Magnum_Animation_Interpolation_h
#define Magnum_Animation_Interpolation_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
 * @brief Alias @ref Magnum::Animation::ResultOf, enum @ref Magnum::Animation::Extrapolation, function @ref Magnum::Animation::interpolate(), @ref Magnum::Animation::interpolateStrict()
 */

#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Functions.h"
#ifdef CORRADE_MSVC2015_COMPATIBILITY
#include "Magnum/Animation/Animation.h" /* ResultOf alias on MSVC 2015 */
#endif

namespace Magnum { namespace Animation {

namespace Implementation {
    template<class T> struct TypeTraits {
        typedef T ResultType;
    };
}

/**
@brief Animation result type for given value type

Result of interpolating two `V` values (for example interpolating two
@ref Color3 values gives back a @ref Color3 again, but interpolating a spline
does not result in a spline).
@experimental
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class V> using ResultOf = typename Implementation::TypeTraits<V>::ResultType;
#endif

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
template<class K, class V> ResultOf<V> interpolate(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, Extrapolation before, Extrapolation after, ResultOf<V>(*interpolator)(const V&, const V&, Float), K frame, std::size_t& hint);

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
template<class K, class V> ResultOf<V> interpolateStrict(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, ResultOf<V>(*interpolator)(const V&, const V&, Float), K frame, std::size_t& hint);

template<class K, class V> ResultOf<V> interpolate(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, const Extrapolation before, const Extrapolation after, ResultOf<V>(*const interpolator)(const V&, const V&, Float), K frame, std::size_t& hint) {
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
        Math::lerpInverted(keys[hint], keys[hint + 1], frame));
}

template<class K, class V> ResultOf<V> interpolateStrict(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, ResultOf<V>(*const interpolator)(const V&, const V&, Float), const K frame, std::size_t& hint) {
    CORRADE_ASSERT(keys.size() >= 2, "Animation::interpolateStrict(): at least two keyframes required", {});
    CORRADE_ASSERT(keys.size() == values.size(), "Animation::interpolateStrict(): keys and values don't have the same size", {});

    /* Rewind from the beginning if hint is too late */
    if(hint >= keys.size() || frame < keys[hint]) hint = 0;

    /* Go through the keys until we find a pair that is around given time */
    while(hint + 2 < keys.size() && frame >= keys[hint + 1])
        ++hint;

    return interpolator(values[hint], values[hint + 1],
        Math::lerpInverted(keys[hint], keys[hint + 1], frame));
}

}}

#endif
