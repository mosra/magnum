#ifndef Magnum_Array_h
#define Magnum_Array_h
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
 * @brief Class @ref Magnum::Array, @ref Magnum::Array1D, @ref Magnum::Array2D, @ref Magnum::Array3D
 */

#include <type_traits>

#include "Magnum/Magnum.h"
#include "Magnum/Math/BoolVector.h" /* for Math::Implementation::Sequence */

namespace Magnum {

/**
@brief Array
@tparam dimensions  Dimension count
@tparam T           Data type

Similar to @ref Math::Vector, but more suitable for storing enum values which
don't need any math operations and fuzzy comparison (e.g. enum values). Unlike
@ref Math::Vector this class has implicit constructor from one value.
@see @ref Array1D, @ref Array2D, @ref Array3D
*/
template<UnsignedInt dimensions, class T> class Array {
    public:
        typedef T Type;                 /**< @brief Data type */

        enum: UnsignedInt {
            Dimensions = dimensions     /**< Dimension count */
        };

        /**
         * @brief Default constructor
         *
         * Sets all components to their default-constructed values.
         */
        constexpr /*implicit*/ Array(): _data() {}

        /**
         * @brief Initializer-list constructor
         * @param first     First value
         * @param next      Next values
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> constexpr /*implicit*/ Array(T first, U... next);
        #else
        template<class ...U, class V = typename std::enable_if<sizeof...(U)+1 == dimensions, T>::type> constexpr /*implicit*/ Array(T first, U... next): _data{first, next...} {}
        #endif

        /** @brief Construct array with one value for all fields */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        constexpr /*implicit*/ Array(T value);
        #else
        template<class U, class V = typename std::enable_if<std::is_same<T, U>::value && dimensions != 1, T>::type>
        constexpr /*implicit*/ Array(U value): Array(typename Math::Implementation::GenerateSequence<dimensions>::Type(), value) {}
        #endif

        /** @brief Equality */
        bool operator==(const Array<dimensions, T>& other) const {
            for(UnsignedInt i = 0; i != dimensions; ++i)
                if(_data[i] != other._data[i]) return false;
            return true;
        }

        /** @brief Non-equality */
        bool operator!=(const Array<dimensions, T>& other) const {
            return !operator==(other);
        }

        /** @brief Value at given position */
        T& operator[](UnsignedInt pos) { return _data[pos]; }
        constexpr T operator[](UnsignedInt pos) const { return _data[pos]; } /**< @overload */

        /**
         * @brief Raw data
         * @return One-dimensional array of `dimensions` length
         */
        T* data() { return _data; }
        constexpr const T* data() const { return _data; } /**< @overload */

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        T _data[dimensions];

    private:
        /* Implementation for Array<dimensions, T>::Array(U) */
        template<std::size_t ...sequence> constexpr explicit Array(Math::Implementation::Sequence<sequence...>, T value): _data{Math::Implementation::repeat(value, sequence)...} {}
};

/**
@brief One-dimensional array
@tparam T           Data type
*/
template<class T> class Array1D: public Array<1, T> {
    public:
        /** @copydoc Array::Array() */
        constexpr /*implicit*/ Array1D() = default;

        /**
         * @brief Constructor
         * @param x     X component
         */
        constexpr /*implicit*/ Array1D(T x): Array<1, T>(x) {}

        /** @brief Copy constructor */
        constexpr Array1D(const Array<1, T>& other): Array<1, T>(other) {}

        T& x() { return Array<1, T>::_data[0]; } /**< @brief X component */
        constexpr T x() const { return Array<1, T>::_data[0]; } /**< @overload */
};

/**
@brief Two-dimensional array
@tparam T           Data type
*/
template<class T> class Array2D: public Array<2, T> {
    public:
        /** @copydoc Array::Array() */
        constexpr /*implicit*/ Array2D() = default;

        /**
         * @brief Constructor
         * @param x     X component
         * @param y     Y component
         */
        constexpr /*implicit*/ Array2D(T x, T y): Array<2, T>(x, y) {}

        /** @brief Initializer-list constructor */
        constexpr /*implicit*/ Array2D(T value): Array<2, T>(value, value) {}

        /** @brief Copy constructor */
        constexpr Array2D(const Array<2, T>& other): Array<2, T>(other) {}

        T& x() { return Array<2, T>::_data[0]; } /**< @brief X component */
        constexpr T x() const { return Array<2, T>::_data[0]; } /**< @overload */
        T& y() { return Array<2, T>::_data[1]; } /**< @brief Y component */
        constexpr T y() const { return Array<2, T>::_data[1]; } /**< @overload */
};

/**
@brief Three-dimensional array
@tparam T           Data type
*/
template<class T> class Array3D: public Array<3, T> {
    public:
        /** @copydoc Array::Array() */
        constexpr /*implicit*/ Array3D() {}

        /**
         * @brief Constructor
         * @param x     X component
         * @param y     Y component
         * @param z     Z component
         */
        constexpr /*implicit*/ Array3D(T x, T y, T z): Array<3, T>(x, y, z) {}

        /** @brief Initializer-list constructor */
        constexpr /*implicit*/ Array3D(T value): Array<3, T>(value, value, value) {}

        /** @brief Copy constructor */
        constexpr Array3D(const Array<3, T>& other): Array<3, T>(other) {}

        T& x() { return Array<3, T>::_data[0]; } /**< @brief X component */
        constexpr T x() const { return Array<3, T>::_data[0]; } /**< @overload */
        T& y() { return Array<3, T>::_data[1]; } /**< @brief Y component */
        constexpr T y() const { return Array<3, T>::_data[1]; } /**< @overload */
        T& z() { return Array<3, T>::_data[2]; } /**< @brief Z component */
        constexpr T z() const { return Array<3, T>::_data[2]; } /**< @overload */

        /**
         * @brief XY part of the array
         * @return First two components of the array
         */
        Array2D<T>& xy() { return reinterpret_cast<Array2D<T>&>(*this); }
        constexpr Array2D<T> xy() const {
            return {Array<3, T>::_data[0], Array<3, T>::_data[1]};
        } /**< @overload */
};

/** @debugoperator{Array} */
template<UnsignedInt dimensions, class T> Debug& operator<<(Debug& debug, const Array<dimensions, T>& value) {
    debug << "Array(" << Debug::nospace;
    for(UnsignedInt i = 0; i != dimensions; ++i) {
        if(i != 0) debug << Debug::nospace << ",";
        debug << value[i];
    }
    return debug << Debug::nospace << ")";
}

/** @debugoperator{Array1D} */
template<class T> inline Debug& operator<<(Debug& debug, const Array1D<T>& value) {
    return debug << static_cast<const Array<1, T>&>(value);
}

/** @debugoperator{Array2D} */
template<class T> inline Debug& operator<<(Debug& debug, const Array2D<T>& value) {
    return debug << static_cast<const Array<2, T>&>(value);
}

/** @debugoperator{Array3D} */
template<class T> inline Debug& operator<<(Debug& debug, const Array3D<T>& value) {
    return debug << static_cast<const Array<3, T>&>(value);
}

}

#endif
