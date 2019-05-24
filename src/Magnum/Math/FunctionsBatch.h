#ifndef Magnum_Math_FunctionsBatch_h
#define Magnum_Math_FunctionsBatch_h
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
 * @brief Batch functions usable with scalar and vector types
 */

#include <initializer_list>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Math/Functions.h"

namespace Magnum { namespace Math {

/**
@{ @name Batch functions

These functions process an ubounded range of values, as opposed to single
vectors or scalars.
*/

/**
@brief If any number in the range is a positive or negative infinity

For scalar types returns @cpp true @ce as soon as it finds an infinite value,
@cpp false @ce otherwise. For vector types, returns @ref BoolVector with bits
set to @cpp 1 @ce if any value has that component infinite. If the range is
empty, returns @cpp false @ce or a @ref BoolVector with no bits set.
@see @ref isInf(T), @ref Constants::inf()
*/
template<class T> auto isInf(Corrade::Containers::ArrayView<const T> range) -> decltype(isInf(std::declval<T>())) {
    if(range.empty()) return {};

    /* For scalars, this loop exits once any value is infinity. For vectors
       the loop accumulates the bits and exits as soon as all bits are set
       or the input is exhausted */
    auto out = isInf(range[0]); /* bool or BoolVector */
    for(std::size_t i = 1; i != range.size(); ++i) {
        if(out) break;
        out = out || isInf(range[i]);
    }

    return out;
}

/** @overload */
template<class T> inline auto isInf(std::initializer_list<T> list) -> decltype(isInf(std::declval<T>())) {
    return isInf(Corrade::Containers::arrayView(list.begin(), list.size()));
}

/** @overload */
template<class T, std::size_t size> inline auto isInf(const T(&array)[size]) -> decltype(isInf(std::declval<T>())) {
    return isInf(Corrade::Containers::arrayView(array));
}

/**
@brief If any number in the range is a NaN

For scalar types returns @cpp true @ce as soon as it finds a NaN value,
@cpp false @ce otherwise. For vector types, returns @ref BoolVector with bits
set to @cpp 1 @ce if any value has that component NaN. If the range is empty,
returns @cpp false @ce or a @ref BoolVector with no bits set.
@see @ref isNan(T), @ref Constants::nan()
*/
template<class T> inline auto isNan(Corrade::Containers::ArrayView<const T> range) -> decltype(isNan(std::declval<T>())) {
    if(range.empty()) return {};

    /* For scalars, this loop exits once any value is infinity. For vectors
       the loop accumulates the bits and exits as soon as all bits are set
       or the input is exhausted */
    auto out = isNan(range[0]); /* bool or BoolVector */
    for(std::size_t i = 1; i != range.size(); ++i) {
        if(out) break;
        out = out || isNan(range[i]);
    }

    return out;
}

/** @overload */
template<class T> inline auto isNan(std::initializer_list<T> list) -> decltype(isInf(std::declval<T>())) {
    return isNan(Corrade::Containers::arrayView(list.begin(), list.size()));
}

/** @overload */
template<class T, std::size_t size> inline bool isNan(const T(&array)[size]) {
    return isNan(Corrade::Containers::arrayView(array));
}

/**
@brief Minimum of a range

If the range is empty, returns default-constructed value.
@see @ref min(T, T)
*/
template<class T> inline T min(Corrade::Containers::ArrayView<const T> range) {
    if(range.empty()) return {};

    T out(range[0]);
    for(std::size_t i = 1; i != range.size(); ++i)
        out = Math::min(out, range[i]);
    return out;
}

/** @overload */
template<class T> inline T min(std::initializer_list<T> list) {
    return min(Corrade::Containers::arrayView(list.begin(), list.size()));
}

/** @overload */
template<class T, std::size_t size> inline T min(const T(&array)[size]) {
    return min(Corrade::Containers::arrayView(array));
}

/**
@brief Maximum of a range

If the range is empty, returns default-constructed value.
*/
template<class T> inline T max(Corrade::Containers::ArrayView<const T> range) {
    if(range.empty()) return {};

    T out(range[0]);
    for(std::size_t i = 1; i != range.size(); ++i)
        out = Math::max(out, range[i]);
    return out;
}

/** @overload */
template<class T> inline T max(std::initializer_list<T> list) {
    return max(Corrade::Containers::arrayView(list.begin(), list.size()));
}

/** @overload */
template<class T, std::size_t size> inline T max(const T(&array)[size]) {
    return max(Corrade::Containers::arrayView(array));
}

namespace Implementation {
    template<class T> inline typename std::enable_if<IsScalar<T>::value, void>::type minmax(T& min, T& max, T value) {
        if(value < min)
            min = value;
        else if(value > max)
            max = value;
    }
    template<std::size_t size, class T> inline void minmax(Vector<size, T>& min, Vector<size, T>& max, const Vector<size, T>& value) {
        for(std::size_t i = 0; i != size; ++i)
            minmax(min[i], max[i], value[i]);
    }
}

/**
@brief Minimum and maximum of a range

If the range is empty, returns default-constructed values.
@see @ref Range::Range(const std::pair<VectorType, VectorType>&)
*/
template<class T> inline std::pair<T, T> minmax(Corrade::Containers::ArrayView<const T> range) {
    if(range.empty()) return {};

    T min{range[0]}, max{range[0]};
    for(std::size_t i = 1; i != range.size(); ++i)
        Implementation::minmax(min, max, range[i]);

    return {min, max};
}

/** @overload */
template<class T> inline std::pair<T, T> minmax(std::initializer_list<T> list) {
    return minmax(Corrade::Containers::arrayView(list.begin(), list.size()));
}

/** @overload */
template<class T, std::size_t size> inline std::pair<T, T> minmax(const T(&array)[size]) {
    return minmax(Corrade::Containers::arrayView(array));
}

/*@}*/

}}

#endif
