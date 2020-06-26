#ifndef Magnum_Math_FunctionsBatch_h
#define Magnum_Math_FunctionsBatch_h
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
 * @brief Batch functions usable with scalar and vector types
 */

#include <initializer_list>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Math/Functions.h"

namespace Magnum { namespace Math {

namespace Implementation {

/** @todo Utility/Algorithms.h has a similar (but different) variant of this,
    maybe turn that into some public utility once we have one more use case? */
template<class T, class View = decltype(Corrade::Containers::Implementation::ErasedArrayViewConverter<typename std::remove_reference<T&&>::type>::from(std::declval<T&&>()))> static auto stridedArrayViewTypeFor(T&&) -> typename View::Type;
template<class T> static T stridedArrayViewTypeFor(const Corrade::Containers::ArrayView<T>&);
template<class T> static T stridedArrayViewTypeFor(const Corrade::Containers::StridedArrayView1D<T>&);

}

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
template<class T> auto isInf(const Corrade::Containers::StridedArrayView1D<const T>& range) -> decltype(isInf(std::declval<T>())) {
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

/**
@overload
@m_since{2020,06}

Converts @p range to @ref Corrade::Containers::StridedArrayView1D and calls the
above overload. Works with any type that's convertible to
@ref Corrade::Containers::StridedArrayView.
*/
template<class Iterable, class T = decltype(Implementation::stridedArrayViewTypeFor(std::declval<Iterable&&>()))> inline auto isInf(Iterable&& range) -> decltype(isInf(std::declval<T>())) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return isInf<T>(Corrade::Containers::StridedArrayView1D<const T>{range});
}

/** @overload */
template<class T> inline auto isInf(std::initializer_list<T> list) -> decltype(isInf(std::declval<T>())) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return isInf<T>(Corrade::Containers::stridedArrayView(list));
}

/** @overload */
template<class T, std::size_t size> inline auto isInf(const T(&array)[size]) -> decltype(isInf(std::declval<T>())) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return isInf<T>(Corrade::Containers::StridedArrayView1D<const T>{array});
}

/**
@brief If any number in the range is a NaN

For scalar types returns @cpp true @ce as soon as it finds a NaN value,
@cpp false @ce otherwise. For vector types, returns @ref BoolVector with bits
set to @cpp 1 @ce if any value has that component NaN. If the range is empty,
returns @cpp false @ce or a @ref BoolVector with no bits set.
@see @ref isNan(T), @ref Constants::nan()
*/
template<class T> inline auto isNan(const Corrade::Containers::StridedArrayView1D<const T>& range) -> decltype(isNan(std::declval<T>())) {
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

/**
@overload
@m_since{2020,06}

Converts @p range to @ref Corrade::Containers::StridedArrayView1D and calls the
above overload. Works with any type that's convertible to
@ref Corrade::Containers::StridedArrayView.
*/
/* See isInf() for why arrayView() and not stridedArrayView() */
template<class Iterable, class T = decltype(Implementation::stridedArrayViewTypeFor(std::declval<Iterable&&>()))> inline auto isNan(Iterable&& range) -> decltype(isNan(std::declval<T>())) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return isNan<T>(Corrade::Containers::StridedArrayView1D<const T>{range});
}

/** @overload */
template<class T> inline auto isNan(std::initializer_list<T> list) -> decltype(isNan(std::declval<T>())) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return isNan<T>(Corrade::Containers::stridedArrayView(list));
}

/** @overload */
template<class T, std::size_t size> inline auto isNan(const T(&array)[size]) -> decltype(isNan(std::declval<T>())) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return isNan<T>(Corrade::Containers::StridedArrayView1D<const T>{array});
}

namespace Implementation {
    /* Non-floating-point types, the first is a non-NaN for sure */
    template<class T, bool any> constexpr std::pair<std::size_t, T> firstNonNan(Corrade::Containers::StridedArrayView1D<const T> range, std::false_type, std::integral_constant<bool, any>) {
        return {0, range.front()};
    }
    /* Floating-point scalars, return the first that's not NaN */
    template<class T> inline std::pair<std::size_t, T> firstNonNan(Corrade::Containers::StridedArrayView1D<const T> range, std::true_type, std::false_type) {
        /* Find the first non-NaN value to compare against. If all are NaN,
           return the last value so the following loop in min/max/minmax()
           doesn't even execute. */
        for(std::size_t i = 0; i != range.size(); ++i)
            if(!isNan(range[i])) return {i, range[i]};
        return {range.size() - 1, range.back()};
    }
    /* Floating-point vectors. Try to gather non-NaN values for each component
       and exit as soon as all are found (or the input is exhausted). Return
       the index of first item with at least one non-NaN value as we need to go
       through all at least partially valid values again anyway in order to
       apply the min/max/minmax operation. I expect the cases of heavily
       NaN-filled vectors (and thus the need to loop twice through most of the
       range) to be very rare, so this shouldn't be a problem. */
    template<class T> inline std::pair<std::size_t, T> firstNonNan(Corrade::Containers::StridedArrayView1D<const T> range, std::true_type, std::true_type) {
        T out = range[0];
        std::size_t firstValid = 0;
        for(std::size_t i = 1; i != range.size(); ++i) {
            BoolVector<T::Size> nans = isNan(out);
            if(nans.none()) break;
            if(nans.all() && firstValid + 1 == i) ++firstValid;
            out = Math::lerp(out, range[i], isNan(out));
        }
        return {firstValid, out};
    }
}

/**
@brief Minimum of a range

If the range is empty, returns default-constructed value. <em>NaN</em>s are
ignored, unless the range is all <em>NaN</em>s.
@see @ref min(T, T), @ref isNan(const Corrade::Containers::StridedArrayView1D<const T>&)
*/
template<class T> inline T min(const Corrade::Containers::StridedArrayView1D<const T>& range) {
    if(range.empty()) return {};

    std::pair<std::size_t, T> iOut = Implementation::firstNonNan(range, IsFloatingPoint<T>{}, IsVector<T>{});
    for(++iOut.first; iOut.first != range.size(); ++iOut.first)
        iOut.second = Math::min(iOut.second, range[iOut.first]);

    return iOut.second;
}

/**
@overload
@m_since{2020,06}

Converts @p range to @ref Corrade::Containers::StridedArrayView1D and calls the
above overload. Works with any type that's convertible to
@ref Corrade::Containers::StridedArrayView.
*/
template<class Iterable, class T = decltype(Implementation::stridedArrayViewTypeFor(std::declval<Iterable&&>()))> inline T min(Iterable&& range) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return min<T>(Corrade::Containers::StridedArrayView1D<const T>{range});
}

/** @overload */
template<class T> inline T min(std::initializer_list<T> list) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return min<T>(Corrade::Containers::stridedArrayView(list));
}

/** @overload */
template<class T, std::size_t size> inline T min(const T(&array)[size]) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return min<T>(Corrade::Containers::StridedArrayView1D<const T>{array});
}

/**
@brief Maximum of a range

If the range is empty, returns default-constructed value. <em>NaN</em>s are
ignored, unless the range is all <em>NaN</em>s.
@see @ref max(T, T), @ref isNan(const Corrade::Containers::StridedArrayView1D<const T>&)
*/
template<class T> inline T max(const Corrade::Containers::StridedArrayView1D<const T>& range) {
    if(range.empty()) return {};

    std::pair<std::size_t, T> iOut = Implementation::firstNonNan(range, IsFloatingPoint<T>{}, IsVector<T>{});
    for(++iOut.first; iOut.first != range.size(); ++iOut.first)
        iOut.second = Math::max(iOut.second, range[iOut.first]);

    return iOut.second;
}

/**
@overload
@m_since{2020,06}

Converts @p range to @ref Corrade::Containers::StridedArrayView1D and calls the
above overload. Works with any type that's convertible to
@ref Corrade::Containers::StridedArrayView.
*/
template<class Iterable, class T = decltype(Implementation::stridedArrayViewTypeFor(std::declval<Iterable&&>()))> inline T max(Iterable&& range) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return max<T>(Corrade::Containers::StridedArrayView1D<const T>{range});
}

/** @overload */
template<class T> inline T max(std::initializer_list<T> list) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return max<T>(Corrade::Containers::stridedArrayView(list));
}

/** @overload */
template<class T, std::size_t size> inline T max(const T(&array)[size]) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return max<T>(Corrade::Containers::StridedArrayView1D<const T>{array});
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

If the range is empty, returns default-constructed values. <em>NaN</em>s are
ignored, unless the range is all <em>NaN</em>s.
@see @ref minmax(T, T),
    @ref Range::Range(const std::pair<VectorType, VectorType>&),
    @ref isNan(const Corrade::Containers::StridedArrayView1D<const T>&)
*/
template<class T> inline std::pair<T, T> minmax(const Corrade::Containers::StridedArrayView1D<const T>& range) {
    if(range.empty()) return {};

    std::pair<std::size_t, T> iOut = Implementation::firstNonNan(range, IsFloatingPoint<T>{}, IsVector<T>{});
    T min{iOut.second}, max{iOut.second};
    for(++iOut.first; iOut.first != range.size(); ++iOut.first)
        Implementation::minmax(min, max, range[iOut.first]);

    return {min, max};
}

/**
@overload
@m_since{2020,06}

Converts @p range to @ref Corrade::Containers::StridedArrayView1D and calls the
above overload. Works with any type that's convertible to
@ref Corrade::Containers::StridedArrayView.
*/
template<class Iterable, class T = decltype(Implementation::stridedArrayViewTypeFor(std::declval<Iterable&&>()))> inline std::pair<T, T> minmax(Iterable&& range) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return minmax<T>(Corrade::Containers::StridedArrayView1D<const T>{range});
}

/** @overload */
template<class T> inline std::pair<T, T> minmax(std::initializer_list<T> list) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return minmax<T>(Corrade::Containers::stridedArrayView(list));
}

/** @overload */
template<class T, std::size_t size> inline std::pair<T, T> minmax(const T(&array)[size]) {
    /* Specifying the template explicitly to avoid recursion into the generic
       function again */
    return minmax<T>(Corrade::Containers::StridedArrayView1D<const T>{array});
}

/* Since 1.8.17, the original short-hand group closing doesn't work anymore.
   FFS. */
/**
 * @}
 */

}}

#endif
