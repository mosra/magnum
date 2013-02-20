#ifndef Magnum_Math_Unit_h
#define Magnum_Math_Unit_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Math::Unit
 */

#include "Math/MathTypeTraits.h"

namespace Magnum { namespace Math {

/**
@brief Base class for units
@tparam T Underlying data type

@see Deg, Rad
*/
template<template<class> class Derived, class T> class Unit {
    template<template<class> class, class> friend class Unit;

    public:
        typedef T Type;             /**< @brief Underlying data type */

        /** @brief Default constructor */
        inline constexpr /*implicit*/ Unit(): value(T(0)) {}

        /** @brief Explicit conversion from unitless type */
        inline constexpr explicit Unit(T value): value(value) {}

        /** @brief Construct from another underlying type */
        template<class U> inline constexpr explicit Unit(Unit<Derived, U> value): value(value.value) {}

        /** @brief Explicit conversion to underlying type */
        inline constexpr explicit operator T() const { return value; }

        /** @brief Equality comparison */
        inline constexpr bool operator==(Unit<Derived, T> other) const {
            return MathTypeTraits<T>::equals(value, other.value);
        }

        /** @brief Non-equality comparison */
        inline constexpr bool operator!=(Unit<Derived, T> other) const {
            return !operator==(other);
        }

        /** @brief Less than comparison */
        inline constexpr bool operator<(Unit<Derived, T> other) const {
            return value < other.value;
        }

        /** @brief Greater than comparison */
        inline constexpr bool operator>(Unit<Derived, T> other) const {
            return value > other.value;
        }

        /** @brief Less than or equal comparison */
        inline constexpr bool operator<=(Unit<Derived, T> other) const {
            return !operator>(other);
        }

        /** @brief Greater than or equal comparison */
        inline constexpr bool operator>=(Unit<Derived, T> other) const {
            return !operator<(other);
        }

        /** @brief Negated value */
        inline constexpr Unit<Derived, T> operator-() const {
            return Unit<Derived, T>(-value);
        }

        /** @brief Add and assign value */
        inline Unit<Derived, T>& operator+=(Unit<Derived, T> other) {
            value += other.value;
            return *this;
        }

        /** @brief Add value */
        inline constexpr Unit<Derived, T> operator+(Unit<Derived, T> other) const {
            return Unit<Derived, T>(value + other.value);
        }

        /** @brief Subtract and assign value */
        inline Unit<Derived, T>& operator-=(Unit<Derived, T> other) {
            value -= other.value;
            return *this;
        }

        /** @brief Subtract value */
        inline constexpr Unit<Derived, T> operator-(Unit<Derived, T> other) const {
            return Unit<Derived, T>(value - other.value);
        }

        /** @brief Multiply with number and assign */
        inline Unit<Derived, T>& operator*=(T number) {
            value *= number;
            return *this;
        }

        /** @brief Multiply with number */
        inline constexpr Unit<Derived, T> operator*(T number) const {
            return Unit<Derived, T>(value*number);
        }

        /** @brief Divide with number and assign */
        inline Unit<Derived, T>& operator/=(T number) {
            value /= number;
            return *this;
        }

        /** @brief Divide with number */
        inline constexpr Unit<Derived, T> operator/(T number) const {
            return Unit<Derived, T>(value/number);
        }

        /** @brief Ratio of two values */
        inline constexpr T operator/(Unit<Derived, T> other) const {
            return value/other.value;
        }

    private:
        T value;
};

/** @relates Unit
@brief Multiply number with value
*/
template<template<class> class Derived, class T> inline constexpr Unit<Derived, T> operator*(typename std::common_type<T>::type number, const Unit<Derived, T>& value) {
    return value*number;
}

}}

#endif
