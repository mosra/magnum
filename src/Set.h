#ifndef Magnum_Set_h
#define Magnum_Set_h
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
 * @brief Class Magnum::Set
 */

namespace Magnum {

/** @ingroup utility
@brief %Set
@tparam T Enum type
@tparam U Underlying type of the enum

Provides strongly-typed set-like functionality for strongly typed enums, such
as binary OR and AND operations. The only requirement for enum type is that
all the values must be binary exclusive.

Desired usage is via typedef'ing and then calling SET_OPERATORS() macro with
the resulting type as parameter to have all the operators implemented.
@code
enum class State: unsigned char {
    Ready = 0x01,
    Waiting = 0x02,
    Done = 0x04
};

typedef Set<State, unsigned char> States;
SET_OPERATORS(States)
@endcode
*/
template<class T, class U> class Set {
    public:
        typedef T Type;             /**< @brief Enum type */
        typedef U UnderlyingType;   /**< @brief Underlying type of the enum */

        /** @brief Create empty set */
        inline constexpr Set(): value() {}

        /** @brief Create set from one value */
        inline constexpr Set(T value): value(static_cast<UnderlyingType>(value)) {}

        /** @brief Union of two sets */
        inline constexpr Set<T, U> operator|(Set<T, U> other) const {
            return Set<T, U>(value | other.value);
        }

        /** @brief Union two sets and assign */
        inline Set<T, U>& operator|=(Set<T, U> other) {
            value |= other.value;
            return *this;
        }

        /** @brief Intersection of two sets */
        inline constexpr Set<T, U> operator&(Set<T, U> other) const {
            return Set<T, U>(value & other.value);
        }

        /** @brief Intersect two sets and assign */
        inline Set<T, U>& operator&=(Set<T, U> other) {
            value &= other.value;
            return *this;
        }

        /** @brief Set complement */
        inline constexpr Set<T, U> operator~() const {
            return Set<T, U>(~value);
        }

        /** @brief Value in underlying type */
        inline constexpr explicit operator UnderlyingType() const {
            return value;
        }

    private:
        inline constexpr explicit Set(UnderlyingType type): value(type) {}

        UnderlyingType value;
};

/** @hideinitializer
@brief Define out-of-class operators for given Set implementation
*/
#define SET_OPERATORS(class)                                                \
    inline constexpr class operator|(class::Type a, class b) {              \
        return b | a;                                                       \
    }                                                                       \
    inline constexpr class operator&(class::Type a, class b) {              \
        return b & a;                                                       \
    }

}

#endif
