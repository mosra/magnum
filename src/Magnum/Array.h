#ifndef Magnum_Array_h
#define Magnum_Array_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#ifdef MAGNUM_BUILD_DEPRECATED
/** @file
 * @brief Class @ref Magnum::Array, @ref Magnum::Array1D, @ref Magnum::Array2D, @ref Magnum::Array3D
 * @m_deprecated_since_latest Use @ref Magnum/Math/Vector3.h instead.
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <type_traits>
#include <Corrade/Containers/sequenceHelpers.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/Macros.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector3.h"

/* No CORRADE_DEPRECATED_FILE() as a lot of other headers include this for
   implicit conversions */

namespace Magnum {

namespace Implementation {
    template<class T> constexpr T repeat(T value, std::size_t) { return value; }
}

/**
@brief Array
@tparam dimensions  Dimension count
@tparam T           Data type
@m_deprecated_since_latest Use @ref Math::Vector instead.

Unlike @ref Math::Vector this class has implicit constructor from one value.
@see @ref Array1D, @ref Array2D, @ref Array3D
*/
template<UnsignedInt dimensions, class T> class CORRADE_DEPRECATED("use Math::Vector instead") Array {
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
        constexpr /*implicit*/ Array(U value): Array(typename Containers::Implementation::GenerateSequence<dimensions>::Type{}, value) {}
        #endif

        /** @brief Convert to a vector */
        /*implicit*/ operator Math::Vector<dimensions, T>() const {
            return Math::Vector<dimensions, T>::from(_data);
        }

        CORRADE_IGNORE_DEPRECATED_PUSH
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
        CORRADE_IGNORE_DEPRECATED_POP

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
        template<std::size_t ...sequence> constexpr explicit Array(Containers::Implementation::Sequence<sequence...>, T value): _data{Implementation::repeat(value, sequence)...} {}
};

CORRADE_IGNORE_DEPRECATED_PUSH
/**
@brief One-dimensional array
@tparam T           Data type
@m_deprecated_since_latest Use @ref Math::Vector or @ref Containers::Array1
    instead.
*/
template<class T> class CORRADE_DEPRECATED("use Math::Vector or Containers::Array1 instead") Array1D: public Array<1, T> {
    public:
        /** @copydoc Array::Array() */
        constexpr /*implicit*/ Array1D() = default;

        /**
         * @brief Constructor
         * @param x     X component
         */
        constexpr /*implicit*/ Array1D(T x): Array<1, T>(x) {}

        /** @brief Construct from a vector */
        constexpr /*implicit*/ Array1D(const Math::Vector<1, T>& vector): Array1D{vector[0]} {}

        T& x() { return Array<1, T>::_data[0]; } /**< @brief X component */
        constexpr T x() const { return Array<1, T>::_data[0]; } /**< @overload */
};

/**
@brief Two-dimensional array
@tparam T           Data type
@m_deprecated_since_latest Use @ref Math::Vector2 or @ref Containers::Array2
    instead.
*/
template<class T> class CORRADE_DEPRECATED("use Math::Vector2 or Containers::Array2 instead")  Array2D: public Array<2, T> {
    public:
        /** @copydoc Array::Array() */
        constexpr /*implicit*/ Array2D() = default;

        /**
         * @brief Constructor
         * @param x     X component
         * @param y     Y component
         */
        constexpr /*implicit*/ Array2D(T x, T y): Array<2, T>(x, y) {}

        /** @brief Construct from a vector */
        constexpr /*implicit*/ Array2D(const Math::Vector2<T>& vector): Array2D{vector.x(), vector.y()} {}

        /** @brief Initializer-list constructor */
        constexpr /*implicit*/ Array2D(T value): Array<2, T>(value, value) {}

        /** @brief Convert to a vector */
        /*implicit*/ operator Math::Vector2<T>() const {
            return Math::Vector2<T>::from(Array<2, T>::_data);
        }

        T& x() { return Array<2, T>::_data[0]; } /**< @brief X component */
        constexpr T x() const { return Array<2, T>::_data[0]; } /**< @overload */
        T& y() { return Array<2, T>::_data[1]; } /**< @brief Y component */
        constexpr T y() const { return Array<2, T>::_data[1]; } /**< @overload */
};

/**
@brief Three-dimensional array
@tparam T           Data type
@m_deprecated_since_latest Use @ref Math::Vector3 or @ref Containers::Array3
    instead.
*/
template<class T> class CORRADE_DEPRECATED("use Math::Vector3 or Containers::Array3 instead") Array3D: public Array<3, T> {
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

        /** @brief Construct from a vector */
        constexpr /*implicit*/ Array3D(const Math::Vector3<T>& vector): Array3D{vector.x(), vector.y(), vector.z()} {}

        /** @brief Initializer-list constructor */
        constexpr /*implicit*/ Array3D(T value): Array<3, T>(value, value, value) {}

        /** @brief Convert to a vector */
        /*implicit*/ operator Math::Vector3<T>() const {
            return Math::Vector3<T>::from(Array<3, T>::_data);
        }

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

/**
@debugoperator{Array}
@m_deprecated_since_latest Use @ref Math::Vector instead.
*/
template<UnsignedInt dimensions, class T> Debug& operator<<(Debug& debug, const Array<dimensions, T>& value) {
    debug << "Array(" << Debug::nospace;
    for(UnsignedInt i = 0; i != dimensions; ++i) {
        if(i != 0) debug << Debug::nospace << ",";
        debug << value[i];
    }
    return debug << Debug::nospace << ")";
}

/**
@debugoperator{Array1D}
@m_deprecated_since_latest Use @ref Math::Vector instead.
*/
template<class T> inline Debug& operator<<(Debug& debug, const Array1D<T>& value) {
    return debug << static_cast<const Array<1, T>&>(value);
}

/**
@debugoperator{Array2D}
@m_deprecated_since_latest Use @ref Math::Vector2 instead.
*/
template<class T> inline Debug& operator<<(Debug& debug, const Array2D<T>& value) {
    return debug << static_cast<const Array<2, T>&>(value);
}

/**
@debugoperator{Array3D}
@m_deprecated_since_latest Use @ref Math::Vector3 instead.
*/
template<class T> inline Debug& operator<<(Debug& debug, const Array3D<T>& value) {
    return debug << static_cast<const Array<3, T>&>(value);
}
CORRADE_IGNORE_DEPRECATED_POP

}
#else
#error use Magnum/Math/Vector3.h instead
#endif

#endif
