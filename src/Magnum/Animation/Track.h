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
 * @brief Class @ref Magnum::Animation::Track, @ref Magnum::Animation::TrackViewStorage, @ref Magnum::Animation::TrackView
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/Animation/Animation.h"
#include "Magnum/Animation/Interpolation.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace Animation {

/**
@brief Animation track
@tparam K       Key type
@tparam V       Value type
@tparam R       Result type

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

It's also possible to supply a generic interpolation behavior by passing the
@ref Interpolation enum to the constructor. In case the interpolator function
is not passed in as well, it's autodetected using @ref interpolatorFor(). See
its documentation for more information. The @ref Interpolation enum is then
stored in @ref interpolation() and acts as a hint for desired interpolation
behavior for users who might want to use their own interpolator.

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

@subsection Animation-Track-performance-interpolator Interpolator function choice

The interpolator function has a direct effect on animation performance. You can
choose a less complex interpolator (constant instead of linear or linear
instead of spheric linear, for example) either during construction or passing
it directly to @ref at() / @ref atStrict(). The @ref interpolator() can act as
a hint on what kind of function should be chosen. Depending on how the track
was constructed, passing the interpolator directly to @ref at() / @ref atStrict()
usually also results in it being inlined by the compiler and thus faster than
an indirect function call.

Note that when constructing the track by just passing @ref Interpolator to the
constructor, the function is chosen by @ref interpolatorFor(), which favors
correctness over performance. See its documentation for more information.
@experimental
*/
template<class K, class V, class R
    #ifdef DOXYGEN_GENERATING_OUTPUT
    = ResultOf<V>
    #endif
> class Track {
    public:
        /** @brief Key type */
        typedef K KeyType;

        /** @brief Value type */
        typedef V ValueType;

        /** @brief Animation result type */
        typedef R ResultType;

        /** @brief Interpolation function */
        typedef ResultType(*Interpolator)(const ValueType&, const ValueType&, Float);

        /**
         * @brief Construct an empty track
         *
         * The @ref data(), @ref keys(), @ref values() and @ref interpolator()
         * functions return @cpp nullptr @ce, @ref at() always returns a
         * default-constructed value.
         */
        explicit Track() noexcept: _data{}, _interpolator{}, _before{}, _after{} {}

        /**
         * @brief Construct with custom interpolator
         * @param data          Keyframe data
         * @param interpolator  Interpolator function
         * @param before        Extrapolation behavior
         * @param after         Extrapolation behavior after
         *
         * The keyframe data are assumed to be stored in sorted order. It's not
         * an error to have two successive keyframes with the same frame value.
         * The @ref interpolation() field is set to @ref Interpolation::Custom.
         * See @ref Track(Containers::Array<std::pair<K, V>>&&, Interpolation, Interpolator, Extrapolation, Extrapolation) or
         * @ref Track(Containers::Array<std::pair<K, V>>&&, Interpolation, Extrapolation, Extrapolation)
         * for an alternative.
         */
        explicit Track(Containers::Array<std::pair<K, V>>&& data, Interpolator interpolator, Extrapolation before, Extrapolation after) noexcept: _data{std::move(data)}, _interpolator{interpolator}, _interpolation{Interpolation::Custom}, _before{before}, _after{after} {}

        /** @overload */
        explicit Track(std::initializer_list<std::pair<K, V>> data, Interpolator interpolator, Extrapolation before, Extrapolation after): Track<K, V, R>{Containers::Array<std::pair<K, V>>{Containers::InPlaceInit, data}, interpolator, before, after} {}

        /** @overload
         * Equivalent to calling @ref Track(Containers::Array<std::pair<K, V>>&&, Interpolator, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit Track(Containers::Array<std::pair<K, V>>&& data, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Constant) noexcept: Track<K, V, R>{std::move(data), interpolator, extrapolation, extrapolation} {}

        /** @overload */
        explicit Track(std::initializer_list<std::pair<K, V>> data, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Constant): Track<K, V, R>{Containers::Array<std::pair<K, V>>{Containers::InPlaceInit, data}, interpolator, extrapolation, extrapolation} {}

        /**
         * @brief Construct with both generic and custom interpolator
         * @param data          Keyframe data
         * @param interpolation Interpolation behavior
         * @param interpolator  Interpolator function
         * @param before        Extrapolation behavior
         * @param after         Extrapolation behavior after
         *
         * The keyframe data are assumed to be stored in sorted order. It's not
         * an error to have two successive keyframes with the same frame value.
         * @p interpolation acts as a behavior hint to users that might want to
         * supply their own interpolator function to @ref at() or
         * @ref atStrict().
         */
        explicit Track(Containers::Array<std::pair<K, V>>&& data, Interpolation interpolation, Interpolator interpolator, Extrapolation before, Extrapolation after) noexcept: _data{std::move(data)}, _interpolator{interpolator}, _interpolation{interpolation}, _before{before}, _after{after} {}

        /** @overload */
        explicit Track(std::initializer_list<std::pair<K, V>> data, Interpolation interpolation, Interpolator interpolator, Extrapolation before, Extrapolation after): Track<K, V, R>{Containers::Array<std::pair<K, V>>{Containers::InPlaceInit, data}, interpolation, interpolator, before, after} {}

        /** @overload
         * Equivalent to calling @ref Track(Containers::Array<std::pair<K, V>>&&, Interpolation, Interpolator, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit Track(Containers::Array<std::pair<K, V>>&& data, Interpolation interpolation, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Constant) noexcept: Track<K, V, R>{std::move(data), interpolation, interpolator, extrapolation, extrapolation} {}

        /** @overload */
        explicit Track(std::initializer_list<std::pair<K, V>> data, Interpolation interpolation, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Constant): Track<K, V, R>{Containers::Array<std::pair<K, V>>{Containers::InPlaceInit, data}, interpolation, interpolator, extrapolation} {}

        /**
         * @brief Construct with generic interpolation behavior
         * @param data          Keyframe data
         * @param interpolation Interpolation behavior
         * @param before        Extrapolation behavior
         * @param after         Extrapolation behavior after
         *
         * The keyframe data are assumed to be stored in sorted order. It's not
         * an error to have two successive keyframes with the same frame value.
         * The @ref interpolator() function is autodetected from
         * @p interpolation using @ref interpolatorFor(). See its documentation
         * for more information.
         */
        explicit Track(Containers::Array<std::pair<K, V>>&& data, Interpolation interpolation, Extrapolation before, Extrapolation after) noexcept: _data{std::move(data)}, _interpolator{interpolatorFor<V, R>(interpolation)}, _interpolation{interpolation}, _before{before}, _after{after} {}

        /** @overload */
        explicit Track(std::initializer_list<std::pair<K, V>> data, Interpolation interpolation, Extrapolation before, Extrapolation after): Track<K, V, R>{Containers::Array<std::pair<K, V>>{Containers::InPlaceInit, data}, interpolation, before, after} {}

        /** @overload
         * Equivalent to calling @ref Track(Containers::Array<std::pair<K, V>>&&, Interpolation, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit Track(Containers::Array<std::pair<K, V>>&& data, Interpolation interpolation, Extrapolation extrapolation = Extrapolation::Constant) noexcept: Track<K, V, R>{std::move(data), interpolation, extrapolation, extrapolation} {}

        /** @overload */
        explicit Track(std::initializer_list<std::pair<K, V>> data, Interpolation interpolation, Extrapolation extrapolation = Extrapolation::Constant): Track<K, V, R>{Containers::Array<std::pair<K, V>>{Containers::InPlaceInit, data}, interpolation, extrapolation, extrapolation} {}

        /** @brief Copying is not allowed */
        Track(const Track<K, V, R>&) = delete;

        /** @brief Move constructor */
        Track(Track<K, V, R>&&) = default;

        /** @brief Copying is not allowed */
        Track<K, V, R>& operator=(const Track<K, V, R>&) = delete;

        /** @brief Move constructor */
        Track<K, V, R>& operator=(Track<K, V, R>&&) = default;

        /** @brief Conversion to a view */
        operator TrackView<K, V, R>() const noexcept {
            return TrackView<K, V, R>{_data, _interpolation, _interpolator, _before, _after};
        }

        /**
         * @brief Interpolation behavior
         *
         * Acts as a behavior hint to users that might want to supply their own
         * interpolator function to @ref at() or @ref atStrict().
         * @see @ref interpolator()
         */
        Interpolation interpolation() const { return _interpolation; }

        /**
         * @brief Interpolation function
         *
         * @see @ref interpolation()
         */
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
         * @brief Duration of the track
         *
         * Calculated from first and last keyframe. If there are no keyframes,
         * a default-constructed value is returned. Use @ref Math::join() to
         * calculate combined duration for a set of tracks.
         */
        Math::Range1D<K> duration() const {
            return _data.empty() ? Math::Range1D<K>{} : Math::Range1D<K>{_data.front().first, _data.back().first};
        }

        /** @brief Keyframe count */
        std::size_t size() const { return _data.size(); }

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

        /**
         * @brief Keyframe access
         *
         * @see @ref size()
         */
        const std::pair<K, V>& operator[](std::size_t i) const { return _data[i]; }

        /**
         * @brief Animated value at a given time
         *
         * Calls @ref interpolate(), see its documentation for more
         * information. Note that this function performs a linear search every
         * time, use @ref at(K, std::size_t&) const to supply a search hint.
         * @see @ref atStrict(K, std::size_t&) const,
         *      @ref at(Interpolator, K) const
         */
        R at(K frame) const {
            std::size_t hint{};
            return at(_interpolator, frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * Calls @ref interpolate(), see its documentation for more
         * information.
         * @see @ref at(K) const, @ref atStrict(K, std::size_t&) const,
         *      @ref at(Interpolator, K, std::size_t&) const
         */
        R at(K frame, std::size_t& hint) const {
            return at(_interpolator, frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * Unlike @ref at(K) const calls @ref interpolate() with
         * @p interpolator, overriding the interpolator function set in
         * constructor. See its documentation for more information.
         * @see @ref at(K, std::size_t&) const,
         *      @ref atStrict(Interpolator, K, std::size_t&) const
         */
        R at(Interpolator interpolator, K frame) const {
            std::size_t hint{};
            return at(interpolator, frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * Unlike @ref at(K, std::size_t&) const calls @ref interpolate() with
         * @p interpolator, overriding the interpolator function set in
         * constructor. See its documentation for more information.
         * @see @ref atStrict(Interpolator, K, std::size_t&) const
         */
        R at(Interpolator interpolator, K frame, std::size_t& hint) const {
            return interpolate(keys(), values(), _before, _after, interpolator, frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * A faster version of @ref at(K, std::size_t&) const with some
         * restrictions. Calls @ref interpolateStrict(), see its documentation
         * for more information.
         * @see @ref atStrict(Interpolator, K, std::size_t&) const
         */
        R atStrict(K frame, std::size_t& hint) const {
            return atStrict(_interpolator, frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * Unlike @ref atStrict(K, std::size_t&) const calls @ref interpolate()
         * with @p interpolator, overriding the interpolator function set in
         * constructor. See its documentation for more information.
         * @see @ref at(K, std::size_t&) const
         */
        R atStrict(Interpolator interpolator, K frame, std::size_t& hint) const {
            return interpolateStrict(keys(), values(), interpolator, frame, hint);
        }

    private:
        Containers::Array<std::pair<K, V>> _data;
        Interpolator _interpolator;
        Interpolation _interpolation;
        Extrapolation _before, _after;
};

/**
@brief Type-erased track view storage

Cast to @ref TrackView of correct type to use.
@experimental
*/
class TrackViewStorage {
    public:
        constexpr /*implicit*/ TrackViewStorage() noexcept: _keys{}, _values{}, _interpolator{}, _interpolation{}, _before{}, _after{} {}

    private:
        template<class, class, class> friend class TrackView;

        template<class K, class V, class R> explicit TrackViewStorage(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, Interpolation interpolation, R(*interpolator)(const V&, const V&, Float), Extrapolation before, Extrapolation after) noexcept: _keys{reinterpret_cast<const Containers::StridedArrayView<const char>&>(keys)}, _values{reinterpret_cast<const Containers::StridedArrayView<const char>&>(values)}, _interpolator{reinterpret_cast<void(*)()>(interpolator)}, _interpolation{interpolation}, _before{before}, _after{after} {}

        Containers::StridedArrayView<const char> _keys;
        Containers::StridedArrayView<const char> _values;
        void(*_interpolator)(void);
        Interpolation _interpolation;
        Extrapolation _before, _after;
};

/**
@brief Animation track view
@tparam K       Key type
@tparam V       Value type
@tparam R       Result type

Unlike @ref Track this is a non-owning view onto keyframe + value pairs. See
its documentation for more information.
@experimental
*/
template<class K, class V, class R
    #ifdef DOXYGEN_GENERATING_OUTPUT
    = ResultOf<V>
    #endif
> class TrackView: public TrackViewStorage {
    public:
        /** @brief Key type */
        typedef K KeyType;

        /** @brief Value type */
        typedef V ValueType;

        /** @brief Animation result type */
        typedef R ResultType;

        /** @brief Interpolation function */
        typedef ResultType(*Interpolator)(const ValueType&, const ValueType&, Float);

        /**
         * @brief Construct an empty track
         *
         * The @ref keys(), @ref values() and @ref interpolator() functions
         * return @cpp nullptr @ce, @ref at() always returns a
         * default-constructed value.
         */
        explicit TrackView() noexcept {}

        /**
         * @brief Construct with custom interpolator
         * @param keys          Frame keys
         * @param values        Frame values
         * @param interpolator  Interpolation function
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         *
         * The keyframe data are assumed to be stored in sorted order. It's not
         * an error to have two successive keyframes with the same frame value.
         * The @ref interpolation() field is set to @ref Interpolation::Custom.
         * See @ref TrackView(const Containers::StridedArrayView<const K>&, const Containers::StridedArrayView<const V>&, Interpolation, Interpolator, Extrapolation, Extrapolation) or
         * @ref TrackView(const Containers::StridedArrayView<const K>&, const Containers::StridedArrayView<const V>&, Interpolation, Extrapolation, Extrapolation)
         * for an alternative.
         */
        explicit TrackView(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, Interpolator interpolator, Extrapolation before, Extrapolation after) noexcept: TrackViewStorage{keys, values, Interpolation::Custom, interpolator, before, after} {}

        /** @overload
         * Equivalent to calling @ref TrackView(const Containers::StridedArrayView<const K>&, const Containers::StridedArrayView<const V>&, Interpolator, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit TrackView(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Extrapolated) noexcept: TrackView<K, V, R>{keys, values, interpolator, extrapolation, extrapolation} {}

        /**
         * @brief Construct with custom interpolator from an interleaved array
         * @param data          Keyframe data
         * @param interpolator  Interpolation function
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         *
         * Converts @p data to a pair of strided array views and calls
         * @ref TrackView(const Containers::StridedArrayView<const K>&, const Containers::StridedArrayView<const V>&, Interpolator, Extrapolation, Extrapolation).
         */
        explicit TrackView(Containers::ArrayView<const std::pair<K, V>> data, Interpolator interpolator, Extrapolation before, Extrapolation after) noexcept: TrackView<K, V, R>{Containers::StridedArrayView<const K>{data ? &data[0].first : nullptr, data.size(), sizeof(std::pair<K, V>)}, Containers::StridedArrayView<const V>{data ? &data[0].second : nullptr, data.size(), sizeof(std::pair<K, V>)}, interpolator, before, after} {}

        /** @overload
         * Equivalent to calling @ref TrackView(Containers::ArrayView<const std::pair<K, V>>, Interpolator, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit TrackView(Containers::ArrayView<const std::pair<K, V>> data, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Extrapolated) noexcept: TrackView<K, V, R>{data, interpolator, extrapolation, extrapolation} {}

        /**
         * @brief Construct with both generic and custom interpolator
         * @param keys          Frame keys
         * @param values        Frame values
         * @param interpolation Interpolation behavior
         * @param interpolator  Interpolator function
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         *
         * The keyframe data are assumed to be stored in sorted order. It's not
         * an error to have two successive keyframes with the same frame value.
         * @p interpolation acts as a behavior hint to users that might want to
         * supply their own interpolator function to @ref at() or
         * @ref atStrict().
         */
        explicit TrackView(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, Interpolation interpolation, Interpolator interpolator, Extrapolation before, Extrapolation after) noexcept: TrackViewStorage{keys, values, interpolation, interpolator, before, after} {}

        /** @overload
         * Equivalent to calling @ref TrackView(const Containers::StridedArrayView<const K>&, const Containers::StridedArrayView<const V>&, Interpolation, Interpolator, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit TrackView(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, Interpolation interpolation, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Extrapolated) noexcept: TrackView<K, V, R>{keys, values, interpolation, interpolator, extrapolation, extrapolation} {}

        /**
         * @brief Construct with both generic and custom interpolator from an interleaved array
         * @param data          Keyframe data
         * @param interpolation Interpolation behavior
         * @param interpolator  Interpolator function
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         *
         * Converts @p data to a pair of strided array views and calls
         * @ref TrackView(const Containers::StridedArrayView<const K>&, const Containers::StridedArrayView<const V>&, Interpolator, Extrapolation, Extrapolation).
         */
        explicit TrackView(Containers::ArrayView<const std::pair<K, V>> data, Interpolation interpolation, Interpolator interpolator, Extrapolation before, Extrapolation after) noexcept: TrackViewStorage{Containers::StridedArrayView<const K>{data ? &data[0].first : nullptr, data.size(), sizeof(std::pair<K, V>)}, Containers::StridedArrayView<const V>{data ? &data[0].second : nullptr, data.size(), sizeof(std::pair<K, V>)}, interpolation, interpolator, before, after} {}

        /** @overload
         * Equivalent to calling @ref TrackView(Containers::ArrayView<const std::pair<K, V>>, Interpolation, Interpolator, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit TrackView(Containers::ArrayView<const std::pair<K, V>> data, Interpolation interpolation, Interpolator interpolator, Extrapolation extrapolation = Extrapolation::Extrapolated) noexcept: TrackView<K, V, R>{data, interpolation, interpolator, extrapolation, extrapolation} {}

        /**
         * @brief Construct with generic interpolation behavior
         * @param keys          Frame keys
         * @param values        Frame values
         * @param interpolation Interpolation behavior
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         *
         * The keyframe data are assumed to be stored in sorted order. It's not
         * an error to have two successive keyframes with the same frame value.
         * The @ref interpolator() function is autodetected from
         * @p interpolation using @ref interpolatorFor(). See its documentation
         * for more information.
         */
        explicit TrackView(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, Interpolation interpolation, Extrapolation before, Extrapolation after) noexcept: TrackViewStorage{keys, values, interpolation, interpolatorFor<V, R>(interpolation), before, after} {}

        /** @overload
         * Equivalent to calling @ref TrackView(const Containers::StridedArrayView<const K>&, const Containers::StridedArrayView<const V>&, Interpolation, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit TrackView(const Containers::StridedArrayView<const K>& keys, const Containers::StridedArrayView<const V>& values, Interpolation interpolation, Extrapolation extrapolation = Extrapolation::Extrapolated) noexcept: TrackView<K, V, R>{keys, values, interpolation, extrapolation, extrapolation} {}

        /**
         * @brief Construct with generic interpolation behavior from an interleaved array
         * @param data          Keyframe data
         * @param interpolation Interpolation behavior
         * @param before        Extrapolation behavior before
         * @param after         Extrapolation behavior after
         *
         * Converts @p data to a pair of strided array views and calls
         * @ref TrackView(const Containers::StridedArrayView<const K>&, const Containers::StridedArrayView<const V>&, Interpolator, Extrapolation, Extrapolation).
         */
        explicit TrackView(Containers::ArrayView<const std::pair<K, V>> data, Interpolation interpolation, Extrapolation before, Extrapolation after) noexcept: TrackView<K, V, R>{Containers::StridedArrayView<const K>{data ? &data[0].first : nullptr, data.size(), sizeof(std::pair<K, V>)}, Containers::StridedArrayView<const V>{data ? &data[0].second : nullptr, data.size(), sizeof(std::pair<K, V>)}, interpolation, before, after} {}

        /** @overload
         * Equivalent to calling @ref TrackView(Containers::ArrayView<const std::pair<K, V>>, Interpolation, Extrapolation, Extrapolation)
         * with both @p before and @p after set to @p extrapolation.
         */
        explicit TrackView(Containers::ArrayView<const std::pair<K, V>> data, Interpolation interpolation, Extrapolation extrapolation = Extrapolation::Extrapolated) noexcept: TrackView<K, V, R>{data, interpolation, extrapolation, extrapolation} {}

        /**
         * @brief Interpolation behavior
         *
         * Acts as a behavior hint to users that might want to supply their own
         * interpolator function to @ref at() or @ref atStrict().
         * @see @ref interpolator()
         */
        Interpolation interpolation() const { return _interpolation; }

        /**
         * @brief Interpolation function
         *
         * @see @ref interpolation()
         */
        Interpolator interpolator() const {
            return reinterpret_cast<Interpolator>(_interpolator);
        }

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

        /**
         * @brief Duration of the track
         *
         * Calculated from first and last keyframe. If there are no keyframes,
         * a default-constructed value is returned. Use @ref Math::join() to
         * calculate combined duration for a set of tracks.
         */
        Math::Range1D<K> duration() const {
            return _keys.empty() ? Math::Range1D<K>{} : Math::Range1D<K>{keys().front(), keys().back()};
        }

        /** @brief Keyframe count */
        std::size_t size() const { return _keys.size(); }

        /**
         * @brief Key data
         *
         * @see @ref values(), @ref operator[]()
         */
        Containers::StridedArrayView<const K> keys() const {
            return reinterpret_cast<const Containers::StridedArrayView<const K>&>(_keys);
        }

        /**
         * @brief Value data
         *
         * @see @ref keys(), @ref operator[]()
         */
        Containers::StridedArrayView<const V> values() const {
            return reinterpret_cast<const Containers::StridedArrayView<const V>&>(_values);
        }

        /**
         * @brief Keyframe access
         *
         * @see @ref size()
         */
        std::pair<K, V> operator[](std::size_t i) const {
            return {keys()[i], values()[i]};
        }

        /**
         * @brief Animated value at a given time
         *
         * Calls @ref interpolate(), see its documentation for more
         * information. Note that this function performs a linear search every
         * time, use @ref at(K, std::size_t&) const to supply a search hint.
         * @see @ref atStrict(K, std::size_t&) const,
         *      @ref at(Interpolator, K) const
         */
        R at(K frame) const {
            std::size_t hint{};
            return at(interpolator(), frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * Calls @ref interpolate(), see its documentation for more
         * information.
         * @see @ref at(K) const, @ref atStrict(K, std::size_t&) const,
         *      @ref at(Interpolator, K, std::size_t&) const
         */
        R at(K frame, std::size_t& hint) const {
            return at(interpolator(), frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * Unlike @ref at(K) const calls @ref interpolate() with
         * @p interpolator, overriding the interpolator function set in
         * constructor. See its documentation for more information.
         * @see @ref at(K, std::size_t&) const,
         *      @ref atStrict(Interpolator, K, std::size_t&) const
         */
        R at(Interpolator interpolator, K frame) const {
            std::size_t hint{};
            return at(interpolator, frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * Unlike @ref at(K, std::size_t&) const calls @ref interpolate() with
         * @p interpolator, overriding the interpolator function set in
         * constructor. See its documentation for more information.
         * @see @ref atStrict(Interpolator, K, std::size_t&) const
         */
        R at(Interpolator interpolator, K frame, std::size_t& hint) const {
            return interpolate(keys(), values(), _before, _after, interpolator, frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * A faster version of @ref at(K, std::size_t&) const with some
         * restrictions. Calls @ref interpolateStrict(), see its documentation
         * for more information.
         * @see @ref atStrict(Interpolator, K, std::size_t&) const
         */
        R atStrict(K frame, std::size_t& hint) const {
            return atStrict(interpolator(), frame, hint);
        }

        /**
         * @brief Animated value at a given time
         *
         * Unlike @ref atStrict(K, std::size_t&) const calls @ref interpolate()
         * with @p interpolator, overriding the interpolator function set in
         * constructor. See its documentation for more information.
         * @see @ref at(K, std::size_t&) const
         */
        R atStrict(Interpolator interpolator, K frame, std::size_t& hint) const {
            return interpolateStrict(keys(), values(), interpolator, frame, hint);
        }
};

}}

#endif
