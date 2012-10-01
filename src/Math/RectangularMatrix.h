#ifndef Magnum_Math_RectangularMatrix_h
#define Magnum_Math_RectangularMatrix_h
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
 * @brief Class Magnum::Math::RectangularMatrix
 */

#include <cmath>
#include <limits>
#include <Utility/Debug.h>
#include <Utility/Configuration.h>

#include "MathTypeTraits.h"

namespace Magnum { namespace Math {

template<size_t, size_t, class> class RectangularMatrix;

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<size_t ...> struct Sequence {};

    /* E.g. GenerateSequence<3>::Type is Sequence<0, 1, 2> */
    template<size_t N, size_t ...sequence> struct GenerateSequence:
        GenerateSequence<N-1, N-1, sequence...> {};

    template<size_t ...sequence> struct GenerateSequence<0, sequence...> {
        typedef Sequence<sequence...> Type;
    };

    /* Implementation for RectangularMatrix<cols, rows, T>::from(const RectangularMatrix<cols, rows, U>&) */
    template<class T, class U, size_t c, size_t ...sequence> inline constexpr Math::RectangularMatrix<c, sizeof...(sequence)/c, T> rectangularMatrixFrom(Sequence<sequence...>, const Math::RectangularMatrix<c, sizeof...(sequence)/c, U>& matrix) {
        return {T(matrix.data()[sequence])...};
    }
}
#endif

template<size_t size, class T> class Vector;

/**
@brief Rectangular matrix
@tparam cols    Column count
@tparam rows    Row count
@tparam T       Data type

See @ref matrix-vector for brief introduction. See also Matrix (square) and
Vector.
*/
template<size_t cols, size_t rows, class T> class RectangularMatrix {
    static_assert(cols != 0 && rows != 0, "Matrix cannot have zero elements");

    public:
        typedef T Type;                     /**< @brief Data type */
        const static size_t Cols = cols;    /**< @brief %Matrix column count */
        const static size_t Rows = rows;    /**< @brief %Matrix row count */

        /**
         * @brief %Matrix from array
         * @return Reference to the data as if it was Matrix, thus doesn't
         *      perform any copying.
         *
         * @attention Use with caution, the function doesn't check whether the
         *      array is long enough.
         */
        inline constexpr static RectangularMatrix<cols, rows, T>& from(T* data) {
            return *reinterpret_cast<RectangularMatrix<cols, rows, T>*>(data);
        }
        /** @overload */
        inline constexpr static const RectangularMatrix<cols, rows, T>& from(const T* data) {
            return *reinterpret_cast<const RectangularMatrix<cols, rows, T>*>(data);
        }

        /**
         * @brief %Matrix from column vectors
         * @param first First column vector
         * @param next  Next column vectors
         */
        template<class ...U> inline constexpr static RectangularMatrix<cols, rows, T> from(const Vector<rows, T>& first, const U&... next) {
            static_assert(sizeof...(next)+1 == cols, "Improper number of arguments passed to Matrix from Vector constructor");
            return from(typename Implementation::GenerateSequence<rows>::Type(), first, next...);
        }

        /**
         * @brief %Matrix from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * RectangularMatrix<4, 1, float> floatingPoint(1.3f, 2.7f, -15.0f, 7.0f);
         * RectangularMatrix<4, 1, int> integral(RectangularMatrix<4, 1, int>::from(floatingPoint));
         * // integral == {1, 2, -15, 7}
         * @endcode
         */
        template<class U> inline constexpr static RectangularMatrix<cols, rows, T> from(const RectangularMatrix<cols, rows, U>& other) {
            return Implementation::rectangularMatrixFrom<T, U>(typename Implementation::GenerateSequence<cols*rows>::Type(), other);
        }

        /** @brief Zero-filled matrix constructor */
        inline constexpr RectangularMatrix(): _data() {}

        /**
         * @brief Initializer-list constructor
         * @param first First value
         * @param next  Next values
         *
         * Note that the values are in column-major order.
         * @todoc Remove workaround when Doxygen supports uniform initialization
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class ...U> inline constexpr RectangularMatrix(T first, U... next): _data{first, next...} {
            static_assert(sizeof...(next)+1 == cols*rows, "Improper number of arguments passed to Matrix constructor");
        }
        #else
        template<class ...U> inline constexpr RectangularMatrix(T first, U... next);
        #endif

        /** @brief Copy constructor */
        inline constexpr RectangularMatrix(const RectangularMatrix<cols, rows, T>&) = default;

        /** @brief Assignment operator */
        inline RectangularMatrix<cols, rows, T>& operator=(const RectangularMatrix<cols, rows, T>&) = default;

        /**
         * @brief Raw data
         * @return One-dimensional array of `size*size` length in column-major
         *      order.
         */
        inline T* data() { return _data; }
        inline constexpr const T* data() const { return _data; } /**< @overload */

        /**
         * @brief %Matrix column
         *
         * For accessing individual elements prefer to use operator(), as it
         * is guaranteed to not involve unnecessary conversions.
         */
        inline Vector<rows, T>& operator[](size_t col) {
            return Vector<rows, T>::from(_data+col*rows);
        }
        /** @overload */
        inline constexpr const Vector<rows, T>& operator[](size_t col) const {
            return Vector<rows, T>::from(_data+col*rows);
        }

        /**
         * @brief Element on given position
         *
         * Prefer this instead of using `[][]`.
         * @see operator[]
         */
        inline T& operator()(size_t col, size_t row) {
            return _data[col*rows+row];
        }
        /** @overload */
        inline constexpr const T& operator()(size_t col, size_t row) const {
            return _data[col*rows+row];
        }

        /** @brief Equality operator */
        inline bool operator==(const RectangularMatrix<cols, rows, T>& other) const {
            for(size_t i = 0; i != cols*rows; ++i)
                if(!MathTypeTraits<T>::equals(_data[i], other._data[i])) return false;

            return true;
        }

        /** @brief Non-equality operator */
        inline constexpr bool operator!=(const RectangularMatrix<cols, rows, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Add matrix
         *
         * @see operator+=()
         */
        inline RectangularMatrix<cols, rows, T> operator+(const RectangularMatrix<cols, rows, T>& other) const {
            return RectangularMatrix<cols, rows, T>(*this)+=other;
        }

        /**
         * @brief Add and assign matrix
         *
         * More efficient than operator+(), because it does the computation
         * in-place.
         */
        RectangularMatrix<cols, rows, T>& operator+=(const RectangularMatrix<cols, rows, T>& other) {
            for(size_t i = 0; i != cols*rows; ++i)
                _data[i] += other._data[i];

            return *this;
        }

        /** @brief Negative matrix */
        RectangularMatrix<cols, rows, T> operator-() const {
            RectangularMatrix<cols, rows, T> out;

            for(size_t i = 0; i != cols*rows; ++i)
                out._data[i] = -_data[i];

            return out;
        }

        /**
         * @brief Subtract matrix
         *
         * @see operator-=()
         */
        inline RectangularMatrix<cols, rows, T> operator-(const RectangularMatrix<cols, rows, T>& other) const {
            return RectangularMatrix<cols, rows, T>(*this)-=other;
        }

        /**
         * @brief Subtract and assign matrix
         *
         * More efficient than operator-(), because it does the computation
         * in-place.
         */
        RectangularMatrix<cols, rows, T>& operator-=(const RectangularMatrix<cols, rows, T>& other) {
            for(size_t i = 0; i != cols*rows; ++i)
                _data[i] -= other._data[i];

            return *this;
        }

        /**
         * @brief Multiply matrix with number
         *
         * @see operator*=(U), operator*(U, const RectangularMatrix<cols, rows, T>&)
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>>::type operator*(U number) const {
        #else
        template<class U> inline RectangularMatrix<cols, rows, T> operator*(U number) const {
        #endif
            return RectangularMatrix<cols, rows, T>(*this)*=number;
        }

        /**
         * @brief Multiply matrix with number and assign
         *
         * More efficient than operator*(U), because it does the computation
         * in-place.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>&>::type operator*=(U number) {
        #else
        template<class U> RectangularMatrix<cols, rows, T>& operator*=(U number) {
        #endif
            for(size_t i = 0; i != cols*rows; ++i)
                _data[i] *= number;

            return *this;
        }

        /**
         * @brief Divide matrix with number
         *
         * @see operator/=(), operator/(U, const RectangularMatrix<cols, rows, T>&)
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>>::type operator/(U number) const {
        #else
        template<class U> inline RectangularMatrix<cols, rows, T> operator/(U number) const {
        #endif
            return RectangularMatrix<cols, rows, T>(*this)/=number;
        }

        /**
         * @brief Divide matrix with number and assign
         *
         * More efficient than operator/(), because it does the computation
         * in-place.
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>&>::type operator/=(U number) {
        #else
        template<class U> RectangularMatrix<cols, rows, T>& operator/=(U number) {
        #endif
            for(size_t i = 0; i != cols*rows; ++i)
                _data[i] /= number;

            return *this;
        }

        /** @brief Multiply matrix */
        template<size_t size> RectangularMatrix<size, rows, T> operator*(const RectangularMatrix<size, cols, T>& other) const {
            RectangularMatrix<size, rows, T> out;

            for(size_t col = 0; col != size; ++col)
                for(size_t row = 0; row != rows; ++row)
                    for(size_t pos = 0; pos != cols; ++pos)
                        out(col, row) += (*this)(pos, row)*other(col, pos);

            return out;
        }

        /**
         * @brief Multiply vector
         *
         * Internally the same as multiplying with one-column matrix, but
         * returns vector.
         */
        Vector<rows, T> operator*(const Vector<rows, T>& other) const {
            return operator*(static_cast<RectangularMatrix<1, rows, T>>(other));
        }

        /** @brief Transposed matrix */
        RectangularMatrix<rows, cols, T> transposed() const {
            RectangularMatrix<rows, cols, T> out;

            for(size_t col = 0; col != cols; ++col)
                for(size_t row = 0; row != rows; ++row)
                    out(row, col) = (*this)(col, row);

            return out;
        }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        T _data[rows*cols];
    #endif

    private:
        template<size_t ...sequence, class ...U> inline constexpr static RectangularMatrix<cols, rows, T> from(Implementation::Sequence<sequence...> s, const Vector<rows, T>& first, U... next) {
            return from(s, next..., first[sequence]...);
        }
        template<size_t ...sequence, class ...U> inline constexpr static RectangularMatrix<cols, rows, T> from(Implementation::Sequence<sequence...>, T first, U... next) {
            return RectangularMatrix<cols, rows, T>(first, next...);
        }
};

/** @relates RectangularMatrix
@brief Multiply number with matrix

@see RectangularMatrix::operator*(U) const
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<size_t cols, size_t rows, class T, class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>>::type operator*(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#else
template<size_t cols, size_t rows, class T, class U> inline RectangularMatrix<cols, rows, T> operator*(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#endif
    return matrix*number;
}

/** @relates RectangularMatrix
@brief Divide matrix with number and invert

Example:
@code
RectangularMatrix<2, 3, float> mat(1.0f, 2.0f, -4.0f, 8.0f, -1.0f, 0.5f);
RectangularMatrix<2, 3, float> another = 1.0f/mat; // {1.0f, 0.5f, -0.25f, 0.128f, -1.0f, 2.0f}
@endcode
@see RectangularMatrix::operator/()
*/
#ifndef DOXYGEN_GENERATING_OUTPUT
template<size_t cols, size_t rows, class T, class U> typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>>::type operator/(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#else
template<size_t cols, size_t rows, class T, class U> RectangularMatrix<cols, rows, T> operator/(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#endif
    RectangularMatrix<cols, rows, T> out;

    for(size_t i = 0; i != cols*rows; ++i)
        out.data()[i] = number/matrix.data()[i];

    return out;
}

/** @debugoperator{Magnum::Math::RectangularMatrix} */
template<size_t cols, size_t rows, class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::RectangularMatrix<cols, rows, T>& value) {
    debug << "Matrix(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    for(size_t row = 0; row != rows; ++row) {
        if(row != 0) debug << ",\n       ";
        for(size_t col = 0; col != cols; ++col) {
            if(col != 0) debug << ", ";
            debug << typename MathTypeTraits<T>::NumericType(value[col][row]);
        }
    }
    debug << ')';
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(cols, rows, ...)   \
    inline constexpr static __VA_ARGS__& from(T* data) {                    \
        return *reinterpret_cast<__VA_ARGS__*>(data);                       \
    }                                                                       \
    inline constexpr static const __VA_ARGS__& from(const T* data) {        \
        return *reinterpret_cast<const __VA_ARGS__*>(data);                 \
    }                                                                       \
    template<class ...U> inline constexpr static __VA_ARGS__ from(const Math::Vector<rows, T>& first, const U&... next) { \
        return Math::RectangularMatrix<cols, rows, T>::from(first, next...); \
    }                                                                       \
    template<class U> inline constexpr static RectangularMatrix<cols, rows, T> from(const Math::RectangularMatrix<cols, rows, U>& other) { \
        return Math::RectangularMatrix<cols, rows, T>::from(other);         \
    }                                                                       \
                                                                            \
    inline __VA_ARGS__& operator=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator=(other);           \
        return *this;                                                       \
    }

#define MAGNUM_RECTANGULARMATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(cols, rows, ...) \
    inline __VA_ARGS__ operator-() const {                                  \
        return Math::RectangularMatrix<cols, rows, T>::operator-();         \
    }                                                                       \
    inline __VA_ARGS__ operator+(const Math::RectangularMatrix<cols, rows, T>& other) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator+(other);    \
    }                                                                       \
    inline __VA_ARGS__& operator+=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator+=(other);          \
        return *this;                                                       \
    }                                                                       \
    inline __VA_ARGS__ operator-(const Math::RectangularMatrix<cols, rows, T>& other) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator-(other);    \
    }                                                                       \
    inline __VA_ARGS__& operator-=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator-=(other);          \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__>::type operator*(U number) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator*(number);   \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__&>::type operator*=(U number) { \
        Math::RectangularMatrix<cols, rows, T>::operator*=(number);         \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__>::type operator/(U number) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator/(number);   \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__&>::type operator/=(U number) { \
        Math::RectangularMatrix<cols, rows, T>::operator/=(number);         \
        return *this;                                                       \
    }
#endif

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::RectangularMatrix} */
template<size_t cols, size_t rows, class T> struct ConfigurationValue<Magnum::Math::RectangularMatrix<cols, rows, T>> {
    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::RectangularMatrix<cols, rows, T>& value, int flags = 0) {
        std::string output;

        for(size_t row = 0; row != rows; ++row) {
            for(size_t col = 0; col != cols; ++col) {
                if(!output.empty()) output += ' ';
                output += ConfigurationValue<T>::toString(value(col, row), flags);
            }
        }

        return output;
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::RectangularMatrix<cols, rows, T> fromString(const std::string& stringValue, int flags = 0) {
        Magnum::Math::RectangularMatrix<cols, rows, T> result;
        std::istringstream in(stringValue);

        for(size_t row = 0; row != rows; ++row) {
            for(size_t col = 0; col != cols; ++col) {
                std::string num;
                in >> num;
                result(col, row) = ConfigurationValue<T>::fromString(num, flags);
            }
        }

        return result;
    }
};

}}

/* Include also Vector, so the definition is complete */
#include "Vector.h"

#endif
