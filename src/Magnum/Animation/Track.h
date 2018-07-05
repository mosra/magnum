#ifndef Magnum_Animation_Track_h
#define Magnum_Animation_Track_h
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
 * @brief Class @ref Magnum::Animation::Track, @ref Magnum::Animation::TrackView
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/Animation/Animation.h"
#include "Magnum/Animation/Interpolation.h"

namespace Magnum { namespace Animation {

/**
@brief Animation track
@tparam K       Key type
@tparam V       Value type

Immutable storage of keyframe + value pairs.

@section Animation-Track-usage Basic usage

Animation track is defined by a list of keyframes (time+value pairs),
interpolator function and extrapolation behavior.

@snippet MagnumAnimation.cpp Track-usage

@section Animation-Track-interpolators Types and interpolators

The track supports arbitrary types for keys, values and interpolators. These
are common combinations:

@m_class{m-fullwidth}

Interpolation type  | Value type        | Result type   | Interpolator
------------------- | ----------------- | ------------- | ------------
Constant            | any `V`           | `V`           | @ref Math::select()
Linear  | @cpp bool @ce <b></b> | @cpp bool @ce <b></b> | @ref Math::select()
Linear  | @ref Math::BoolVector | @ref Math::BoolVector | @ref Math::select()
Linear              | any scalar `V`    | `V`           | @ref Math::lerp()
Linear              | any vector `V`    | `V`           | @ref Math::lerp()
Linear              | @ref Math::Quaternion | @ref Math::Quaternion | @ref Math::lerp(const Quaternion<T>&, const Quaternion<T>&, T) "Math::lerp()"
Spherical linear    | @ref Math::Quaternion | @ref Math::Quaternion | @ref Math::slerp(const Quaternion<T>&, const Quaternion<T>&, T) "Math::slerp()"
Screw linear        | @ref Math::DualQuaternion | @ref Math::DualQuaternion | @ref Math::sclerp(const DualQuaternion<T>&, const DualQuaternion<T>&, T) "Math::sclerp()"

@section Animation-Track-performance Performance tuning

The snippet shown above is convenience-oriented at a cost of sacrificing some
performance. You have the following options:

@subsection Animation-Track-performance-hint Keyframe hinting

The @ref Track and @ref TrackView classes are fully stateless and the
@ref at(K) const function performs a linear search for matching keyframe from
the beginning every time. You can use @ref at(K, std::size_t&) const to
remember last used keyframe index and pass it in the next iteration as a hint:

@snippet MagnumAnimation.cpp Track-performance-hint

@subsection Animation-Track-performance-strict Strict interpolation

While it's possible to have different @ref Extrapolation modes for frames
outside of the track range with graceful handling of single- or zero-frame
animations, the additional checks have some impact. The @ref atStrict() has
implicit @ref Extrapolation::Extrapolated behavior and assumes there are always
at least two keyframes, resulting in more compact interpolation code. If your
animation data satisfy the prerequisites, simply use it in place of @ref at():

@snippet MagnumAnimation.cpp Track-performance-strict

@subsection Animation-Track-performance-cache Cache-efficient data layout

Usually multiple tracks (translation, rotation, scaling) are combined together
to form a single animation. In order to achieve better data layout, consider
interleaving the data and passing them using
@ref Corrade::Containers::StridedArrayView to multiple @ref TrackView
instead of having data duplicated scattered across disjoint allocations of
@ref Track instances:

@snippet MagnumAnimation.cpp Track-performance-cache

@experimental
*/
template<class K, class V> class Track {
    public:
        /** @brief Key type */
        typedef K KeyType;

        /** @brief Value type */
        typedef V ValueType;

        /** @brief Animation result type */
        typedef ResultOf<V> ResultType;

        /** @brief Interpolation function */
        typedef ResultType(*Interpolator)(const ValueType&, const ValueType&, Float);

        /**
         * @brief Constructor
         * @param data          Keyframe data
         * @param interpolator  Interpolator function
         * @param before        Extrapolation behavior
         * @param after         Extrapolation behavior after
         *
         * The keyframe data are assumed to be stored in sorted order. It's not
         * an error to have two successive keyframes with the same frame value.
         */
        explicit Track(Containers::Array<std::pair<K, V>>&& data, Interpolator interpolator, Extrapolation before, Extrapolation after) noexcept: _data{std::move(data)}, _interpolator{interpolator}, _before{before}, _after{after} {}

        /** @overload */
        explicit Track(std::initializer_list<std::pair<K, V>> data, Interpolator interpolator, Extrapolation before, Extrapolation after): Track<K, V>{Containers::Array<std::pair<K, V>>{Containers::InPlaceInit, data}, interpolator, before, after} {}

        /** @overload
         * Equivalent to calling @ref Track(Containers::Array<std::pair<K, V>>&&, Interpolator, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit Track(Containers::Array<std::pair<K, V>>&& data, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Constant) noexcept: Track<K, V>{std::move(data), interpolator, extrapolation, extrapolation} {}

        /** @overload */
        explicit Track(std::initializer_list<std::pair<K, V>> data, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Constant): Track<K, V>{Containers::Array<std::pair<K, V>>{Containers::InPlaceInit, data}, interpolator, extrapolation} {}

        /** @brief Copying is not allowed */
        Track(const Track<K, V>&) = delete;

        /** @brief Move constructor */
        Track(Track<K, V>&&) = default;

        /** @brief Copying is not allowed */
        Track<K, V>& operator=(const Track<K, V>&) = delete;

        /** @brief Move constructor */
        Track<K, V>& operator=(Track<K, V>&&) = default;

        /** @brief Conversion to a view */
        operator TrackView<K, V>() const noexcept {
            return TrackView<K, V>{_data, _interpolator, _before, _after};
        }

        /** @brief Interpolation function */
        Interpolator interpolator() const { return _interpolator; }

        /**
         * @brief Extrapolation behavior before first keyframe
         *
         * @see @ref after(), @ref at()
         */
        Extrapolation before() const { return _before; }

        /**
         * @brief Extrapolation behavior after last keyframe
         *
         * @see @ref before(), @ref at()
         */
        Extrapolation after() const { return _after; }

        /**
         * @brief Keyframe data
         *
         * @see @ref keys(), @ref values(), @ref operator[]()
         */
        Containers::ArrayView<const std::pair<K, V>> data() const { return _data; }

        /**
         * @brief Key data
         *
         * @see @ref data(), @ref values(), @ref operator[]()
         */
        Containers::StridedArrayView<const K> keys() const {
            return _data ? Containers::StridedArrayView<const K>{&_data[0].first, _data.size(), sizeof(std::pair<K, V>)} : nullptr;
        }

        /**
         * @brief Value data
         *
         * @see @ref data(), @ref keys(), @ref operator[]()
         */
        Containers::StridedArrayView<const V> values() const {
            return _data ? Containers::StridedArrayView<const V>{&_data[0].second, _data.size(), sizeof(std::pair<K, V>)} : nullptr;
        }

        /** @brief Keyframe access */
        const std::pair<K, V>& operator[](std::size_t i) const { return _data[i]; }

        /**
         * @brief Animated value at a given time
         *
         * Calls @ref interpolate(), see its documentation for more
         * information. Note that this function performs a linear search every
         * time, use @ref at(K, std::size_t&) const to supply a search hint.
         * @see @ref atStrict()
         */
        ResultOf<V> at(K frame) const {
            std::size_t hint{};
            return at(frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * Calls @ref interpolate(), see its documentation for more
         * information.
         * @see @ref at(K) const, @ref atStrict(K, std::size_t&) const
         */
        ResultOf<V> at(K frame, std::size_t& hint) const {
            return interpolate(keys(), values(), _before, _after, _interpolator, frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * A faster version of @ref at(K, std::size_t&) const with some
         * restrictions. Calls @ref interpolateStrict(), see its documentation
         * for more information.
         */
        ResultOf<V> atStrict(K frame, std::size_t& hint) const {
            return interpolateStrict(keys(), values(), _interpolator, frame, hint);
        }

    private:
        Containers::Array<std::pair<K, V>> _data;
        Interpolator _interpolator;
        Extrapolation _before, _after;
};

/**
@brief Animation track view
@tparam K       Key type
@tparam V       Value type

Unlike @ref Track this is a non-owning view onto keyframe + value pairs. See
its documentation for more information.
@experimental
*/
template<class K, class V> class TrackView {
    public:
        /** @brief Key type */
        typedef K KeyType;

        /** @brief Value type */
        typedef V ValueType;

        /** @brief Animation result type */
        typedef ResultOf<V> ResultType;

        /** @brief Interpolation function */
        typedef ResultType(*Interpolator)(const ValueType&, const ValueType&, Float);

        /**
         * @brief Constructor
         * @param keys          Frame keys
         * @param values        Frame values
         * @param interpolator  Interpolation function
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         *
         * The keyframe data are assumed to be stored in sorted order. It's not
         * an error to have two successive keyframes with the same frame value.
         */
        constexpr explicit TrackView(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, Interpolator interpolator, Extrapolation before, Extrapolation after) noexcept: _keys{keys}, _values{values}, _interpolator{interpolator}, _before{before}, _after{after} {}

        /** @overload
         * Equivalent to calling @ref TrackView(const Containers::StridedArrayView<const K>&, const Containers::StridedArrayView<const V>&, Interpolator, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        constexpr explicit TrackView(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Extrapolated) noexcept: TrackView<K, V>{keys, values, interpolator, extrapolation, extrapolation} {}

        /**
         * @brief Construct from an interleaved array
         * @param data          Keyframe data
         * @param interpolator  Interpolation function
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         *
         * Converts @p data to a pair of strided array views and calls
         * @ref TrackView(const Containers::StridedArrayView<const K>&, const Containers::StridedArrayView<const V>&, Interpolator, Extrapolation, Extrapolation).
         */
        constexpr explicit TrackView(Containers::ArrayView<const std::pair<K, V>> data, Interpolator interpolator, Extrapolation before, Extrapolation after) noexcept: _keys{data ? &data[0].first : nullptr, data.size(), sizeof(std::pair<K, V>)}, _values{data ? &data[0].second : nullptr, data.size(), sizeof(std::pair<K, V>)}, _interpolator{interpolator}, _before{before}, _after{after} {}

        /** @overload
         * Equivalent to calling @ref TrackView(Containers::ArrayView<const std::pair<K, V>>, Interpolator, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        constexpr explicit TrackView(Containers::ArrayView<const std::pair<K, V>> data, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Extrapolated) noexcept: TrackView<K, V>{data, interpolator, extrapolation, extrapolation} {}

        /**
         * @brief Extrapolation behavior before first keyframe
         *
         * @see @ref after(), @ref at(), @ref atStrict()
         */
        Extrapolation before() const { return _before; }

        /**
         * @brief Extrapolation behavior after last keyframe
         *
         * @see @ref before(), @ref at(), @ref atStrict()
         */
        Extrapolation after() const { return _after; }

        /** @brief Interpolation function */
        Interpolator interpolator() const { return _interpolator; }

        /**
         * @brief Key data
         *
         * @see @ref values(), @ref operator[]()
         */
        Containers::StridedArrayView<const K> keys() const { return _keys; }

        /**
         * @brief Value data
         *
         * @see @ref keys(), @ref operator[]()
         */
        Containers::StridedArrayView<const V> values() const { return _values; }

        /** @brief Keyframe access */
        std::pair<K, V> operator[](std::size_t i) const {
            return {_keys[i], _values[i]};
        }

        /**
         * @brief Animated value at a given time
         *
         * Calls @ref interpolate(), see its documentation for more
         * information. Note that this function performs a linear search every
         * time, use @ref at(K, std::size_t&) const to supply a search hint.
         * @see @ref atStrict(K, std::size_t&) const
         */
        ResultOf<V> at(K frame) const {
            std::size_t hint{};
            return at(frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * Calls @ref interpolate(), see its documentation for more
         * information.
         * @see @ref at(K) const, @ref atStrict(K, std::size_t&) const
         */
        ResultOf<V> at(K frame, std::size_t& hint) const {
            return interpolate(_keys, _values, _before, _after, _interpolator, frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * A faster version of @ref at(K, std::size_t&) const with some
         * restrictions. Calls @ref interpolateStrict(), see its documentation
         * for more information.
         */
        ResultOf<V> atStrict(K frame, std::size_t& hint) const {
            return interpolateStrict(_keys, _values, _interpolator, frame, hint);
        }

    private:
        Containers::StridedArrayView<const K> _keys;
        Containers::StridedArrayView<const V> _values;
        Interpolator _interpolator;
        Extrapolation _before, _after;
};

}}

#endif
