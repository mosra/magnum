#ifndef Magnum_Array_h
#define Magnum_Array_h
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
 * @brief Class Magnum::Array, Magnum::Array1D, Magnum::Array2D, Magnum::Array3D
 */

#include <cstdint>
#include <type_traits>
#include <Utility/Debug.h>

namespace Magnum {

/**
@brief %Array
@tparam dimensions  Dimension count
@tparam T           Data type

Similar to Math::Vector, but more suitable for storing enum values which don't
need any math operations and fuzzy comparison (e.g. enum values). Unlike
Math::Vector this class has non-explicit constructor from one value.
@see Array1D, Array2D, Array3D
*/
template<std::uint8_t dimensions, class T> class Array {
    public:
        typedef T Type;                                     /**< @brief Data type */
        const static std::uint8_t Dimensions = dimensions;  /**< @brief Dimension count */

        /**
         * @brief Default constructor
         *
         * Sets all components to their default-constructed values
         */
        inline constexpr /*implicit*/ Array(): _data() {}

        /**
         * @brief Initializer-list constructor
         * @param first     First value
         * @param next      Next values
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr /*implicit*/ Array(T first, T second, U... next): _data{first, second, next...} {
            static_assert(sizeof...(next)+2 == dimensions, "Improper number of arguments passed to Array constructor");
        }
        template<class U = T> inline constexpr /*implicit*/ Array(typename std::enable_if<std::is_same<T, U>::value && dimensions == 1, U>::type first): _data{first} {}
        #else
        template<class ...U> inline constexpr /*implicit*/ Array(T first, U... next);
        #endif

        /**
         * @brief Constructor
         * @param value Value for all fields
         */
        template<class U, class = typename std::enable_if<std::is_same<T, U>::value && dimensions != 1, U>::type> inline /*implicit*/ Array(U value) {
            for(std::uint8_t i = 0; i != dimensions; ++i)
                _data[i] = value;
        }

        /** @brief Equality */
        inline bool operator==(const Array<dimensions, T>& other) const {
            for(std::uint8_t i = 0; i != dimensions; ++i)
                if(_data[i] != other._data[i]) return false;
            return true;
        }

        /** @brief Non-equality */
        inline bool operator!=(const Array<dimensions, T>& other) const {
            return !operator==(other);
        }

        /** @brief Value at given position */
        inline T& operator[](std::uint8_t pos) { return _data[pos]; }
        inline constexpr T operator[](std::uint8_t pos) const { return _data[pos]; } /**< @overload */

        /**
         * @brief Raw data
         * @return One-dimensional array of `dimensions` length
         */
        inline T* data() { return _data; }
        inline constexpr const T* data() const { return _data; } /**< @overload */

    private:
        T _data[dimensions];
};

/**
@brief One-dimensional array
@tparam T           Data type
*/
template<class T> class Array1D: public Array<1, T> {
    public:
        /** @copydoc Array::Array() */
        inline constexpr /*implicit*/ Array1D() = default;

        /**
         * @brief Constructor
         * @param x     X component
         */
        inline constexpr /*implicit*/ Array1D(T x): Array<1, T>(x) {}

        /** @brief Copy constructor */
        inline constexpr Array1D(const Array<1, T>& other): Array<1, T>(other) {}

        inline T& x() { return (*this)[0]; }                  /**< @brief X component */
        inline constexpr T x() const { return (*this)[0]; }   /**< @overload */
};

/**
@brief Two-dimensional array
@tparam T           Data type
*/
template<class T> class Array2D: public Array<2, T> {
    public:
        /** @copydoc Array::Array() */
        inline constexpr /*implicit*/ Array2D() = default;

        /**
         * @brief Constructor
         * @param x     X component
         * @param y     Y component
         */
        inline constexpr /*implicit*/ Array2D(T x, T y): Array<2, T>(x, y) {}

        /** @copydoc Array::Array(U) */
        inline constexpr /*implicit*/ Array2D(T value): Array<2, T>(value, value) {}

        /** @brief Copy constructor */
        inline constexpr Array2D(const Array<2, T>& other): Array<2, T>(other) {}

        inline T& x() { return (*this)[0]; }                  /**< @brief X component */
        inline constexpr T x() const { return (*this)[0]; }   /**< @overload */
        inline T& y() { return (*this)[1]; }                  /**< @brief Y component */
        inline constexpr T y() const { return (*this)[1]; }   /**< @overload */
};

/**
@brief Three-dimensional array
@tparam T           Data type
*/
template<class T> class Array3D: public Array<3, T> {
    public:
        /** @copydoc Array::Array() */
        inline constexpr /*implicit*/ Array3D() {}

        /**
         * @brief Constructor
         * @param x     X component
         * @param y     Y component
         * @param z     Z component
         */
        inline constexpr /*implicit*/ Array3D(T x, T y, T z): Array<3, T>(x, y, z) {}

        /** @copydoc Array::Array(U) */
        inline constexpr /*implicit*/ Array3D(T value): Array<3, T>(value, value, value) {}

        /** @brief Copy constructor */
        inline constexpr Array3D(const Array<3, T>& other): Array<3, T>(other) {}

        inline T& x() { return (*this)[0]; }                  /**< @brief X component */
        inline constexpr T x() const { return (*this)[0]; }   /**< @overload */
        inline T& y() { return (*this)[1]; }                  /**< @brief Y component */
        inline constexpr T y() const { return (*this)[1]; }   /**< @overload */
        inline T& z() { return (*this)[2]; }                  /**< @brief Z component */
        inline constexpr T z() const { return (*this)[2]; }   /**< @overload */
};

/** @debugoperator{Magnum::Array} */
template<std::uint8_t dimensions, class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Array<dimensions, T>& value) {
    debug << "Array(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    for(std::uint8_t i = 0; i != dimensions; ++i) {
        if(i != 0) debug << ", ";
        debug << value[i];
    }
    debug << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

/** @debugoperator{Magnum::Array1D} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Array1D<T>& value) {
    return debug << static_cast<const Array<1, T>&>(value);
}

/** @debugoperator{Magnum::Array2D} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Array2D<T>& value) {
    return debug << static_cast<const Array<2, T>&>(value);
}

/** @debugoperator{Magnum::Array3D} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Array3D<T>& value) {
    return debug << static_cast<const Array<3, T>&>(value);
}

}

#endif
