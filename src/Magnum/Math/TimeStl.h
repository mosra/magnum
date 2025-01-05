#ifndef Magnum_Math_TimeStl_h
#define Magnum_Math_TimeStl_h
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
@brief STL @ref std::chrono compatibility for @ref Magnum::Math::Nanoseconds
@m_since_latest

Including this header allows you to convert a
@ref Magnum::Math::Nanoseconds from and to @ref std::chrono::duration and
@ref std::chrono::time_point. See @ref Math-Nanoseconds-stl for more
information.
*/

#include <chrono>

#include "Magnum/Math/Time.h"

namespace Magnum { namespace Math { namespace Implementation {

/* There's no NanosecondsConverter<Long, std::time_t> because this is a typedef
   to some integral type, which when simply picks the Nanoseconds(Long)
   constructor and not a NanosecondsConverter. C types yay. */

template<class Rep, std::intmax_t num, std::intmax_t denom> struct NanosecondsConverter<Long, std::chrono::duration<Rep, std::ratio<num, denom>>> {
    constexpr static Nanoseconds<Long> from(std::chrono::duration<Rep, std::ratio<num, denom>> other) {
        /* The Rep can be floating-point, truncate just the integral part but
           only after converting to nanoseconds */
        return Nanoseconds<Long>{Long(other.count()*num*(1000000000ll/denom))};
    }
    /* No to() because it can be an integer type, losing precision */
    /** @todo add a floating-point-only to() once desirable */
};
template<class Rep> struct NanosecondsConverter<Long, std::chrono::duration<Rep, std::nano>> {
    constexpr static Nanoseconds<Long> from(std::chrono::duration<Rep, std::nano> other) {
        /* The Rep can be floating-point, truncate just the integral part --
           we don't have anything for sub-nanosecond precision */
        return Nanoseconds<Long>{Long(other.count())};
    }
    constexpr static std::chrono::duration<Rep, std::nano> to(Nanoseconds<Long> other) {
        return std::chrono::duration<Rep, std::nano>{Long(other)};
    }
};

template<class Clock, class Rep, std::intmax_t num, std::intmax_t denom> struct NanosecondsConverter<Long, std::chrono::time_point<Clock, std::chrono::duration<Rep, std::ratio<num, denom>>>> {
    constexpr static Nanoseconds<Long> from(std::chrono::time_point<Clock, std::chrono::duration<Rep, std::ratio<num, denom>>> other) {
        return Nanoseconds<Long>{other.time_since_epoch().count()*num*(1000000000ll/denom)};
    }
    /* No to() because it's an integer type, losing precision */
    /** @todo is there even any std::chrono::time_point that would use a FP
        duration type? */
};
template<class Clock, class Rep> struct NanosecondsConverter<Long, std::chrono::time_point<Clock, std::chrono::duration<Rep, std::nano>>> {
    constexpr static Nanoseconds<Long> from(std::chrono::time_point<Clock, std::chrono::duration<Rep, std::nano>> other) {
        return Nanoseconds<Long>{other.time_since_epoch().count()};
    }
    constexpr static std::chrono::time_point<Clock, std::chrono::duration<Rep, std::nano>> to(Nanoseconds<Long> other) {
        return std::chrono::time_point<Clock, std::chrono::duration<Rep, std::nano>>{std::chrono::duration<Rep, std::nano>{Long(other)}};
    }
};

}}}

#endif
