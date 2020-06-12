#ifndef Magnum_Math_Unit_h
#define Magnum_Math_Unit_h
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
 * @brief Class @ref Magnum::Math::Unit
 */

#include "Magnum/Math/TypeTraits.h"
#include "Magnum/Math/Tags.h"

namespace Magnum { namespace Math {

/**
@brief Base class for units
@tparam T Underlying data type

@see @ref Deg, @ref Rad
*/
template<template<class> class Derived, class T> class Unit {
    template<template<class> class, class> friend class Unit;

    public:
        typedef T Type;             /**< @brief Underlying data type */

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Unit(ZeroInitT).
         */
        constexpr /*implicit*/ Unit() noexcept: _value(T(0)) {}

        /** @brief Construct a zero value */
        constexpr explicit Unit(ZeroInitT) noexcept: _value(T(0)) {}

        /** @brief Construct without initializing the contents */
        explicit Unit(Magnum::NoInitT) noexcept {}

        /** @brief Explicit conversion from a unitless type */
        constexpr explicit Unit(T value) noexcept: _value(value) {}

        /** @brief Construct from another underlying type */
        template<class U> constexpr explicit Unit(Unit<Derived, U> value) noexcept: _value(T(value._value)) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Unit(const Unit<Derived, T>& other) noexcept = default;

        /** @brief Explicit conversion to underlying type */
        constexpr explicit operator T() const { return _value; }

        /** @brief Equality comparison */
        constexpr bool operator==(Unit<Derived, T> other) const {
            return TypeTraits<T>::equals(_value, other._value);
        }

        /** @brief Non-equality comparison */
        constexpr bool operator!=(Unit<Derived, T> other) const {
            return !operator==(other);
        }

        /** @brief Less than comparison */
        constexpr bool operator<(Unit<Derived, T> other) const {
            return _value < other._value;
        }

        /** @brief Greater than comparison */
        constexpr bool operator>(Unit<Derived, T> other) const {
            return _value > other._value;
        }

        /** @brief Less than or equal comparison */
        constexpr bool operator<=(Unit<Derived, T> other) const {
            return !operator>(other);
        }

        /** @brief Greater than or equal comparison */
        constexpr bool operator>=(Unit<Derived, T> other) const {
            return !operator<(other);
        }

        /** @brief Negated value */
        constexpr Unit<Derived, T> operator-() const {
            return Unit<Derived, T>(-_value);
        }

        /** @brief Add and assign a value */
        Unit<Derived, T>& operator+=(Unit<Derived, T> other) {
            _value += other._value;
            return *this;
        }

        /** @brief Add a value */
        constexpr Unit<Derived, T> operator+(Unit<Derived, T> other) const {
            return Unit<Derived, T>(_value + other._value);
        }

        /** @brief Subtract and assign a value */
        Unit<Derived, T>& operator-=(Unit<Derived, T> other) {
            _value -= other._value;
            return *this;
        }

        /** @brief Subtract a value */
        constexpr Unit<Derived, T> operator-(Unit<Derived, T> other) const {
            return Unit<Derived, T>(_value - other._value);
        }

        /** @brief Multiply with a number and assign */
        Unit<Derived, T>& operator*=(T number) {
            _value *= number;
            return *this;
        }

        /** @brief Multiply with a number */
        constexpr Unit<Derived, T> operator*(T number) const {
            return Unit<Derived, T>(_value*number);
        }

        /** @brief Divide with a number and assign */
        Unit<Derived, T>& operator/=(T number) {
            _value /= number;
            return *this;
        }

        /** @brief Divide with a number */
        constexpr Unit<Derived, T> operator/(T number) const {
            return Unit<Derived, T>(_value/number);
        }

        /** @brief Ratio of two values */
        constexpr T operator/(Unit<Derived, T> other) const {
            return _value/other._value;
        }

    private:
        T _value;
};

/** @relates Unit
@brief Multiply number with value
*/
template<template<class> class Derived, class T> constexpr Unit<Derived, T> operator*(typename std::common_type<T>::type number, const Unit<Derived, T>& value) {
    return value*number;
}

}}

#endif
