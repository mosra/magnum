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

#include "Math/Vector.h"

namespace Magnum { namespace Math {

/** @todo Properly test all constexpr */

/**
@brief Rectangular matrix
@tparam cols    Column count
@tparam rows    Row count
@tparam T       Data type

See @ref matrix-vector for brief introduction. See also Matrix (square) and
Vector.

The data are stored in column-major order, to reflect that, all indices in
math formulas are in reverse order (i.e. @f$ \boldsymbol A_{ji} @f$ instead
of @f$ \boldsymbol A_{ij} @f$).
*/
template<std::size_t cols, std::size_t rows, class T> class RectangularMatrix {
    static_assert(cols != 0 && rows != 0, "RectangularMatrix cannot have zero elements");

    public:
        typedef T Type;                         /**< @brief Data type */
        const static std::size_t Cols = cols;   /**< @brief %Matrix column count */
        const static std::size_t Rows = rows;   /**< @brief %Matrix row count */

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

        /** @brief Construct zero-filled matrix */
        inline constexpr /*implicit*/ RectangularMatrix() {}

        /**
         * @brief Construct matrix from column vectors
         * @param first First column vector
         * @param next  Next column vectors
         *
         * @todo Creating matrix from arbitrary combination of matrices with n rows
         */
        template<class ...U> inline constexpr /*implicit*/ RectangularMatrix(const Vector<rows, T>& first, const U&... next): _data{first, next...} {
            static_assert(sizeof...(next)+1 == cols, "Improper number of arguments passed to RectangularMatrix constructor");
        }

        /**
         * @brief Construct matrix from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * RectangularMatrix<4, 1, float> floatingPoint(1.3f, 2.7f, -15.0f, 7.0f);
         * RectangularMatrix<4, 1, std::int8_t> integral(floatingPoint);
         * // integral == {1, 2, -15, 7}
         * @endcode
         */
        template<class U> inline constexpr explicit RectangularMatrix(const RectangularMatrix<cols, rows, U>& other): RectangularMatrix(typename Implementation::GenerateSequence<cols>::Type(), other) {}

        /** @brief Copy constructor */
        inline constexpr RectangularMatrix(const RectangularMatrix<cols, rows, T>&) = default;

        /** @brief Assignment operator */
        inline RectangularMatrix<cols, rows, T>& operator=(const RectangularMatrix<cols, rows, T>&) = default;

        /**
         * @brief Raw data
         * @return One-dimensional array of `size*size` length in column-major
         *      order.
         *
         * @see operator[]
         */
        inline T* data() { return reinterpret_cast<T*>(_data); }
        inline constexpr const T* data() const { return reinterpret_cast<const T*>(_data); } /**< @overload */

        /**
         * @brief %Matrix column
         *
         * Particular elements can be accessed using Vector::operator[], e.g.:
         * @code
         * RectangularMatrix<4, 3, float> m;
         * float a = m[2][1];
         * @endcode
         *
         * @see data()
         */
        inline Vector<rows, T>& operator[](std::size_t col) {
            return _data[col];
        }
        /** @overload */
        inline constexpr const Vector<rows, T>& operator[](std::size_t col) const {
            return _data[col];
        }

        /** @brief Equality comparison */
        inline bool operator==(const RectangularMatrix<cols, rows, T>& other) const {
            for(std::size_t i = 0; i != cols; ++i)
                if(_data[i] != other._data[i]) return false;

            return true;
        }

        /**
         * @brief Non-equality operator
         *
         * @see Vector::operator<(), Vector::operator<=(), Vector::operator>=(),
         *      Vector::operator>()
         */
        inline constexpr bool operator!=(const RectangularMatrix<cols, rows, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Add and assign matrix
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = \boldsymbol A_j + \boldsymbol B_j
         * @f]
         */
        RectangularMatrix<cols, rows, T>& operator+=(const RectangularMatrix<cols, rows, T>& other) {
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] += other._data[i];

            return *this;
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
         * @brief Negated matrix
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = -\boldsymbol A_j
         * @f]
         */
        RectangularMatrix<cols, rows, T> operator-() const {
            RectangularMatrix<cols, rows, T> out;

            for(std::size_t i = 0; i != cols; ++i)
                out._data[i] = -_data[i];

            return out;
        }

        /**
         * @brief Subtract and assign matrix
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = \boldsymbol A_j - \boldsymbol B_j
         * @f]
         */
        RectangularMatrix<cols, rows, T>& operator-=(const RectangularMatrix<cols, rows, T>& other) {
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] -= other._data[i];

            return *this;
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
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = a \boldsymbol A_j
         * @f]
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>&>::type operator*=(U number) {
        #else
        template<class U> RectangularMatrix<cols, rows, T>& operator*=(U number) {
        #endif
            for(std::size_t i = 0; i != cols; ++i)
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
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = \frac{\boldsymbol A_j} a
         * @f]
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>&>::type operator/=(U number) {
        #else
        template<class U> RectangularMatrix<cols, rows, T>& operator/=(U number) {
        #endif
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] /= number;

            return *this;
        }

        /**
         * @brief Multiply matrix
         *
         * @f[
         *      (\boldsymbol {AB})_{ji} = \sum_{k=0}^{m-1} \boldsymbol A_{ki} \boldsymbol B_{jk}
         * @f]
         */
        template<std::size_t size> RectangularMatrix<size, rows, T> operator*(const RectangularMatrix<size, cols, T>& other) const {
            RectangularMatrix<size, rows, T> out;

            for(std::size_t col = 0; col != size; ++col)
                for(std::size_t row = 0; row != rows; ++row)
                    for(std::size_t pos = 0; pos != cols; ++pos)
                        out[col][row] += (*this)[pos][row]*other[col][pos];

            return out;
        }

        /**
         * @brief Multiply vector
         *
         * Internally the same as multiplying with one-column matrix, but
         * returns vector. @f[
         *      (\boldsymbol {Aa})_i = \sum_{k=0}^{m-1} \boldsymbol A_{ki} \boldsymbol a_k
         * @f]
         */
        Vector<rows, T> operator*(const Vector<rows, T>& other) const {
            return operator*(RectangularMatrix<1, rows, T>(other))[0];
        }

        /** @brief Transposed matrix */
        RectangularMatrix<rows, cols, T> transposed() const {
            RectangularMatrix<rows, cols, T> out;

            for(std::size_t col = 0; col != cols; ++col)
                for(std::size_t row = 0; row != rows; ++row)
                    out[row][col] = (*this)[col][row];

            return out;
        }

    private:
        /* Implementation for RectangularMatrix<cols, rows, T>::RectangularMatrix(const RectangularMatrix<cols, rows, U>&) */
        template<class U, std::size_t ...sequence> inline constexpr explicit RectangularMatrix(Implementation::Sequence<sequence...>, const RectangularMatrix<cols, rows, U>& matrix): _data{Vector<rows, T>(matrix[sequence])...} {}

        Vector<rows, T> _data[cols];
};

/** @relates RectangularMatrix
@brief Multiply number with matrix

Same as RectangularMatrix::operator*(U) const.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<std::size_t cols, std::size_t rows, class T, class U> inline RectangularMatrix<cols, rows, T> operator*(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#else
template<std::size_t cols, std::size_t rows, class T, class U> inline typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>>::type operator*(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#endif
    return matrix*number;
}

/** @relates RectangularMatrix
@brief Divide matrix with number and invert

The computation is done column-wise. @f[
    \boldsymbol B_j = \frac a {\boldsymbol A_j}
@f]
@see RectangularMatrix::operator/(U) const
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<std::size_t cols, std::size_t rows, class T, class U> RectangularMatrix<cols, rows, T> operator/(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#else
template<std::size_t cols, std::size_t rows, class T, class U> typename std::enable_if<std::is_arithmetic<U>::value, RectangularMatrix<cols, rows, T>>::type operator/(U number, const RectangularMatrix<cols, rows, T>& matrix) {
#endif
    RectangularMatrix<cols, rows, T> out;

    for(std::size_t i = 0; i != cols; ++i)
        out[i] = number/matrix[i];

    return out;
}

/** @relates RectangularMatrix
@brief Multiply vector with rectangular matrix

Internally the same as multiplying one-column matrix with one-row matrix. @f[
    (\boldsymbol {aA})_{ji} = \boldsymbol a_i \boldsymbol A_j
@f]
@see RectangularMatrix::operator*(const RectangularMatrix<size, cols, T>&) const
*/
template<std::size_t size, std::size_t cols, class T> inline RectangularMatrix<cols, size, T> operator*(const Vector<size, T>& vector, const RectangularMatrix<cols, 1, T>& matrix) {
    return RectangularMatrix<1, size, T>(vector)*matrix;
}

/** @debugoperator{Magnum::Math::RectangularMatrix} */
template<std::size_t cols, std::size_t rows, class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::RectangularMatrix<cols, rows, T>& value) {
    debug << "Matrix(";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, false);
    for(std::size_t row = 0; row != rows; ++row) {
        if(row != 0) debug << ",\n       ";
        for(std::size_t col = 0; col != cols; ++col) {
            if(col != 0) debug << ", ";
            debug << value[col][row];
        }
    }
    debug << ")";
    debug.setFlag(Corrade::Utility::Debug::SpaceAfterEachValue, true);
    return debug;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Explicit instantiation for types used in OpenGL */
/* Square matrices */
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 2, float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 3, float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 4, float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 2, double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 3, double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 4, double>&);
#endif

/* Rectangular matrices */
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 3, float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 2, float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 4, float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 2, float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 4, float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 3, float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 3, double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 2, double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 4, double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 2, double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 4, double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 3, double>&);
#endif

#define MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(cols, rows, ...)   \
    inline constexpr static __VA_ARGS__& from(T* data) {                    \
        return *reinterpret_cast<__VA_ARGS__*>(data);                       \
    }                                                                       \
    inline constexpr static const __VA_ARGS__& from(const T* data) {        \
        return *reinterpret_cast<const __VA_ARGS__*>(data);                 \
    }                                                                       \
                                                                            \
    inline __VA_ARGS__& operator=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator=(other);           \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    inline __VA_ARGS__ operator-() const {                                  \
        return Math::RectangularMatrix<cols, rows, T>::operator-();         \
    }                                                                       \
    inline __VA_ARGS__& operator+=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator+=(other);          \
        return *this;                                                       \
    }                                                                       \
    inline __VA_ARGS__ operator+(const Math::RectangularMatrix<cols, rows, T>& other) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator+(other);    \
    }                                                                       \
    inline __VA_ARGS__& operator-=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator-=(other);          \
        return *this;                                                       \
    }                                                                       \
    inline __VA_ARGS__ operator-(const Math::RectangularMatrix<cols, rows, T>& other) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator-(other);    \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__&>::type operator*=(U number) { \
        Math::RectangularMatrix<cols, rows, T>::operator*=(number);         \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__>::type operator*(U number) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator*(number);   \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__&>::type operator/=(U number) { \
        Math::RectangularMatrix<cols, rows, T>::operator/=(number);         \
        return *this;                                                       \
    }                                                                       \
    template<class U> inline typename std::enable_if<std::is_arithmetic<U>::value, __VA_ARGS__>::type operator/(U number) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator/(number);   \
    }
#endif

}}

namespace Corrade { namespace Utility {

/** @configurationvalue{Magnum::Math::RectangularMatrix} */
template<std::size_t cols, std::size_t rows, class T> struct ConfigurationValue<Magnum::Math::RectangularMatrix<cols, rows, T>> {
    ConfigurationValue() = delete;

    /** @brief Writes elements separated with spaces */
    static std::string toString(const Magnum::Math::RectangularMatrix<cols, rows, T>& value, ConfigurationValueFlags flags) {
        std::string output;

        for(std::size_t row = 0; row != rows; ++row) {
            for(std::size_t col = 0; col != cols; ++col) {
                if(!output.empty()) output += ' ';
                output += ConfigurationValue<T>::toString(value[col][row], flags);
            }
        }

        return output;
    }

    /** @brief Reads elements separated with whitespace */
    static Magnum::Math::RectangularMatrix<cols, rows, T> fromString(const std::string& stringValue, ConfigurationValueFlags flags) {
        Magnum::Math::RectangularMatrix<cols, rows, T> result;

        std::size_t oldpos = 0, pos = std::string::npos, i = 0;
        do {
            pos = stringValue.find(' ', oldpos);
            std::string part = stringValue.substr(oldpos, pos-oldpos);

            if(!part.empty()) {
                result[i%cols][i/cols] = ConfigurationValue<T>::fromString(part, flags);
                ++i;
            }

            oldpos = pos+1;
        } while(pos != std::string::npos);

        return result;
    }
};

#ifndef DOXYGEN_GENERATING_OUTPUT
/* Square matrices */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, float>>;
#ifndef MAGNUM_TARGET_GLES
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, double>>;
#endif

/* Rectangular matrices */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, float>>;
#ifndef MAGNUM_TARGET_GLES
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, double>>;
#endif
#endif

}}

#endif
