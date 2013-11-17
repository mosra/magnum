#ifndef Magnum_Math_RectangularMatrix_h
#define Magnum_Math_RectangularMatrix_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Math::RectangularMatrix
 */

#include "Math/Vector.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<std::size_t, std::size_t, class, class> struct RectangularMatrixConverter;
}

/**
@brief Rectangular matrix
@tparam cols    Column count
@tparam rows    Row count
@tparam T       Underlying data type

See @ref matrix-vector for brief introduction. See also Matrix (square) and
Vector.

The data are stored in column-major order, to reflect that, all indices in
math formulas are in reverse order (i.e. @f$ \boldsymbol A_{ji} @f$ instead
of @f$ \boldsymbol A_{ij} @f$).
@see @ref Matrix2x3, @ref Matrix3x2, @ref Matrix2x4, @ref Matrix4x2,
    @ref Matrix3x4, @ref Matrix4x3
*/
template<std::size_t cols, std::size_t rows, class T> class RectangularMatrix {
    static_assert(cols != 0 && rows != 0, "RectangularMatrix cannot have zero elements");

    template<std::size_t, std::size_t, class> friend class RectangularMatrix;

    public:
        typedef T Type;                         /**< @brief Underlying data type */
        const static std::size_t Cols = cols;   /**< @brief %Matrix column count */
        const static std::size_t Rows = rows;   /**< @brief %Matrix row count */

        /**
         * @brief Size of matrix diagonal
         *
         * @see fromDiagonal(), diagonal()
         */
        const static std::size_t DiagonalSize = (cols < rows ? cols : rows);

        /**
         * @brief %Matrix from array
         * @return Reference to the data as if it was Matrix, thus doesn't
         *      perform any copying.
         *
         * @attention Use with caution, the function doesn't check whether the
         *      array is long enough.
         */
        constexpr static RectangularMatrix<cols, rows, T>& from(T* data) {
            return *reinterpret_cast<RectangularMatrix<cols, rows, T>*>(data);
        }
        /** @overload */
        constexpr static const RectangularMatrix<cols, rows, T>& from(const T* data) {
            return *reinterpret_cast<const RectangularMatrix<cols, rows, T>*>(data);
        }

        /**
         * @brief Construct diagonal matrix
         *
         * @see diagonal()
         * @todo make this constexpr
         */
        static RectangularMatrix<cols, rows, T> fromDiagonal(const Vector<DiagonalSize, T>& diagonal);

        /**
         * @brief Construct matrix from vector
         *
         * Rolls the vector into matrix, i.e. first `rows` elements of the
         * vector will make first column of resulting matrix.
         * @see toVector()
         */
        static RectangularMatrix<cols, rows, T> fromVector(const Vector<cols*rows, T>& vector) {
            return *reinterpret_cast<const RectangularMatrix<cols, rows, T>*>(vector.data());
        }

        /** @brief Construct zero-filled matrix */
        constexpr /*implicit*/ RectangularMatrix() {}

        /**
         * @brief Construct matrix from column vectors
         * @param first First column vector
         * @param next  Next column vectors
         *
         * @todo Creating matrix from arbitrary combination of matrices with n rows
         */
        template<class ...U> constexpr /*implicit*/ RectangularMatrix(const Vector<rows, T>& first, const U&... next): _data{first, next...} {
            static_assert(sizeof...(next)+1 == cols, "Improper number of arguments passed to RectangularMatrix constructor");
        }

        /**
         * @brief Construct matrix from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         * @code
         * RectangularMatrix<4, 1, Float> floatingPoint(1.3f, 2.7f, -15.0f, 7.0f);
         * RectangularMatrix<4, 1, Byte> integral(floatingPoint);
         * // integral == {1, 2, -15, 7}
         * @endcode
         */
        #ifndef CORRADE_GCC46_COMPATIBILITY
        template<class U> constexpr explicit RectangularMatrix(const RectangularMatrix<cols, rows, U>& other): RectangularMatrix(typename Implementation::GenerateSequence<cols>::Type(), other) {}
        #else
        template<class U> explicit RectangularMatrix(const RectangularMatrix<cols, rows, U>& other) {
            *this = RectangularMatrix(typename Implementation::GenerateSequence<cols>::Type(), other);
        }
        #endif

        /** @brief Construct matrix from external representation */
        #ifndef CORRADE_GCC46_COMPATIBILITY
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<cols, rows, T, U>::from(std::declval<U>()))> constexpr explicit RectangularMatrix(const U& other): RectangularMatrix(Implementation::RectangularMatrixConverter<cols, rows, T, U>::from(other)) {}
        #else
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<cols, rows, T, U>::from(std::declval<U>()))> explicit RectangularMatrix(const U& other) {
            *this = Implementation::RectangularMatrixConverter<cols, rows, T, U>::from(other);
        }
        #endif

        /** @brief Copy constructor */
        constexpr RectangularMatrix(const RectangularMatrix<cols, rows, T>&) = default;

        /** @brief Assignment operator */
        RectangularMatrix<cols, rows, T>& operator=(const RectangularMatrix<cols, rows, T>&) = default;

        /** @brief Convert matrix to external representation */
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<cols, rows, T, U>::to(std::declval<RectangularMatrix<cols, rows, T>>()))> constexpr explicit operator U() const {
            /** @bug Why this is not constexpr under GCC 4.6? */
            return Implementation::RectangularMatrixConverter<cols, rows, T, U>::to(*this);
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of `cols*rows` length in column-major
         *      order.
         *
         * @see operator[]
         */
        T* data()
        #ifndef CORRADE_GCC47_COMPATIBILITY
        &
        #endif
        { return _data[0].data(); }

        /** @overload */
        constexpr const T* data()
        #ifndef CORRADE_GCC47_COMPATIBILITY
        const &
        #else
        const
        #endif
        { return _data[0].data(); }

        /**
         * @brief %Matrix column
         *
         * Particular elements can be accessed using Vector::operator[], e.g.:
         * @code
         * RectangularMatrix<4, 3, Float> m;
         * Float a = m[2][1];
         * @endcode
         *
         * @see row(), data()
         */
        Vector<rows, T>& operator[](std::size_t col) { return _data[col]; }
        constexpr const Vector<rows, T>& operator[](std::size_t col) const { return _data[col]; } /**< @overload */

        /**
         * @brief %Matrix row
         *
         * Consider using transposed() when accessing rows frequently, as this
         * is slower than accessing columns due to the way the matrix is stored.
         * @see operator[]()
         */
        Vector<cols, T> row(std::size_t row) const;

        /** @brief Equality comparison */
        bool operator==(const RectangularMatrix<cols, rows, T>& other) const {
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
        bool operator!=(const RectangularMatrix<cols, rows, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Negated matrix
         *
         * The computation is done column-wise. @f[
         *      \boldsymbol B_j = -\boldsymbol A_j
         * @f]
         */
        RectangularMatrix<cols, rows, T> operator-() const;

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
        RectangularMatrix<cols, rows, T> operator+(const RectangularMatrix<cols, rows, T>& other) const {
            return RectangularMatrix<cols, rows, T>(*this)+=other;
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
        RectangularMatrix<cols, rows, T> operator-(const RectangularMatrix<cols, rows, T>& other) const {
            return RectangularMatrix<cols, rows, T>(*this)-=other;
        }

        /**
         * @brief Multiply matrix with number and assign
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = a \boldsymbol A_j
         * @f]
         */
        RectangularMatrix<cols, rows, T>& operator*=(T number) {
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] *= number;

            return *this;
        }

        /**
         * @brief Multiply matrix with number
         *
         * @see operator*=(T), operator*(T, const RectangularMatrix<cols, rows, T>&)
         */
        RectangularMatrix<cols, rows, T> operator*(T number) const {
            return RectangularMatrix<cols, rows, T>(*this) *= number;
        }

        /**
         * @brief Divide matrix with number and assign
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = \frac{\boldsymbol A_j} a
         * @f]
         */
        RectangularMatrix<cols, rows, T>& operator/=(T number) {
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] /= number;

            return *this;
        }

        /**
         * @brief Divide matrix with number
         *
         * @see operator/=(T),
         *      operator/(T, const RectangularMatrix<cols, rows, T>&)
         */
        RectangularMatrix<cols, rows, T> operator/(T number) const {
            return RectangularMatrix<cols, rows, T>(*this) /= number;
        }

        /**
         * @brief Multiply matrix
         *
         * @f[
         *      (\boldsymbol {AB})_{ji} = \sum_{k=0}^{m-1} \boldsymbol A_{ki} \boldsymbol B_{jk}
         * @f]
         */
        template<std::size_t size> RectangularMatrix<size, rows, T> operator*(const RectangularMatrix<size, cols, T>& other) const;

        /**
         * @brief Multiply vector
         *
         * Internally the same as multiplying with one-column matrix, but
         * returns vector. @f[
         *      (\boldsymbol {Aa})_i = \sum_{k=0}^{m-1} \boldsymbol A_{ki} \boldsymbol a_k
         * @f]
         */
        Vector<rows, T> operator*(const Vector<cols, T>& other) const {
            return operator*(RectangularMatrix<1, cols, T>(other))[0];
        }

        /**
         * @brief Transposed matrix
         *
         * @see row()
         */
        RectangularMatrix<rows, cols, T> transposed() const;

        /**
         * @brief Values on diagonal
         *
         * @see fromDiagonal()
         * @todo constexpr
         */
        Vector<DiagonalSize, T> diagonal() const;

        /**
         * @brief Convert matrix to vector
         *
         * Returns the matrix unrolled into one large vector, i.e. first column
         * of the matrix will make first `rows` elements of resulting vector.
         * Useful for performing vector operations with the matrix (e.g.
         * summing the elements etc.).
         * @see fromVector()
         */
        Vector<rows*cols, T> toVector() const {
            return *reinterpret_cast<const Vector<rows*cols, T>*>(data());
        }

    private:
        /* Implementation for RectangularMatrix<cols, rows, T>::RectangularMatrix(const RectangularMatrix<cols, rows, U>&) */
        template<class U, std::size_t ...sequence> constexpr explicit RectangularMatrix(Implementation::Sequence<sequence...>, const RectangularMatrix<cols, rows, U>& matrix): _data{Vector<rows, T>(matrix[sequence])...} {}

        Vector<rows, T> _data[cols];
};

#ifndef CORRADE_GCC46_COMPATIBILITY
/**
@brief Matrix with 2 columns and 3 rows

Convenience alternative to <tt>%RectangularMatrix<2, 3, T></tt>. See
@ref RectangularMatrix for more information.
@note Not available on GCC < 4.7. Use <tt>%RectangularMatrix<2, 3, T></tt>
    instead.
@see @ref Magnum::Matrix2x3, @ref Magnum::Matrix2x3d
*/
template<class T> using Matrix2x3 = RectangularMatrix<2, 3, T>;

/**
@brief Matrix with 3 columns and 2 rows

Convenience alternative to <tt>%RectangularMatrix<3, 2, T></tt>. See
@ref RectangularMatrix for more information.
@note Not available on GCC < 4.7. Use <tt>%RectangularMatrix<3, 2, T></tt>
    instead.
@see @ref Magnum::Matrix3x2, @ref Magnum::Matrix3x2d
*/
template<class T> using Matrix3x2 = RectangularMatrix<3, 2, T>;

/**
@brief Matrix with 2 columns and 4 rows

Convenience alternative to <tt>%RectangularMatrix<2, 4, T></tt>. See
@ref RectangularMatrix for more information.
@note Not available on GCC < 4.7. Use <tt>%RectangularMatrix<2, 4, T></tt>
    instead.
@see @ref Magnum::Matrix2x4, @ref Magnum::Matrix2x4d
*/
template<class T> using Matrix2x4 = RectangularMatrix<2, 4, T>;

/**
@brief Matrix with 4 columns and 2 rows

Convenience alternative to <tt>%RectangularMatrix<4, 2, T></tt>. See
@ref RectangularMatrix for more information.
@note Not available on GCC < 4.7. Use <tt>%RectangularMatrix<4, 2, T></tt>
    instead.
@see @ref Magnum::Matrix4x2, @ref Magnum::Matrix4x2d
*/
template<class T> using Matrix4x2 = RectangularMatrix<4, 2, T>;

/**
@brief Matrix with 3 columns and 4 rows

Convenience alternative to <tt>%RectangularMatrix<3, 4, T></tt>. See
@ref RectangularMatrix for more information.
@note Not available on GCC < 4.7. Use <tt>%RectangularMatrix<3, 4, T></tt>
    instead.
@see @ref Magnum::Matrix3x4, @ref Magnum::Matrix3x4d
*/
template<class T> using Matrix3x4 = RectangularMatrix<3, 4, T>;

/**
@brief Matrix with 4 columns and 3 rows

Convenience alternative to <tt>%RectangularMatrix<4, 3, T></tt>. See
@ref RectangularMatrix for more information.
@note Not available on GCC < 4.7. Use <tt>%RectangularMatrix<4, 3, T></tt>
    instead.
@see @ref Magnum::Matrix4x3, @ref Magnum::Matrix4x3d
*/
template<class T> using Matrix4x3 = RectangularMatrix<4, 3, T>;
#endif

/** @relates RectangularMatrix
@brief Multiply number with matrix

Same as RectangularMatrix::operator*(T) const.
*/
template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<cols, rows, T> operator*(
    #ifdef DOXYGEN_GENERATING_OUTPUT
    T
    #else
    typename std::common_type<T>::type
    #endif
    number, const RectangularMatrix<cols, rows, T>& matrix)
{
    return matrix*number;
}

/** @relates RectangularMatrix
@brief Divide matrix with number and invert

The computation is done column-wise. @f[
    \boldsymbol B_j = \frac a {\boldsymbol A_j}
@f]
@see RectangularMatrix::operator/(T) const
*/
template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<cols, rows, T> operator/(
    #ifdef DOXYGEN_GENERATING_OUTPUT
    T
    #else
    typename std::common_type<T>::type
    #endif
    number, const RectangularMatrix<cols, rows, T>& matrix)
{
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
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 2, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 3, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 4, Float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 2, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 3, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 4, Double>&);
#endif

/* Rectangular matrices */
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 3, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 2, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 4, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 2, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 4, Float>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 3, Float>&);
#ifndef MAGNUM_TARGET_GLES
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 3, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 2, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<2, 4, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 2, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<3, 4, Double>&);
extern template Corrade::Utility::Debug MAGNUM_EXPORT operator<<(Corrade::Utility::Debug, const RectangularMatrix<4, 3, Double>&);
#endif

#define MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(cols, rows, ...)   \
    constexpr static __VA_ARGS__& from(T* data) {                           \
        return *reinterpret_cast<__VA_ARGS__*>(data);                       \
    }                                                                       \
    constexpr static const __VA_ARGS__& from(const T* data) {               \
        return *reinterpret_cast<const __VA_ARGS__*>(data);                 \
    }                                                                       \
                                                                            \
    __VA_ARGS__ operator-() const {                                         \
        return Math::RectangularMatrix<cols, rows, T>::operator-();         \
    }                                                                       \
    __VA_ARGS__& operator+=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator+=(other);          \
        return *this;                                                       \
    }                                                                       \
    __VA_ARGS__ operator+(const Math::RectangularMatrix<cols, rows, T>& other) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator+(other);    \
    }                                                                       \
    __VA_ARGS__& operator-=(const Math::RectangularMatrix<cols, rows, T>& other) { \
        Math::RectangularMatrix<cols, rows, T>::operator-=(other);          \
        return *this;                                                       \
    }                                                                       \
    __VA_ARGS__ operator-(const Math::RectangularMatrix<cols, rows, T>& other) const { \
        return Math::RectangularMatrix<cols, rows, T>::operator-(other);    \
    }                                                                       \
    __VA_ARGS__& operator*=(T number) {                                     \
        Math::RectangularMatrix<cols, rows, T>::operator*=(number);         \
        return *this;                                                       \
    }                                                                       \
    __VA_ARGS__ operator*(T number) const {                                 \
        return Math::RectangularMatrix<cols, rows, T>::operator*(number);   \
    }                                                                       \
    __VA_ARGS__& operator/=(T number) {                                     \
        Math::RectangularMatrix<cols, rows, T>::operator/=(number);         \
        return *this;                                                       \
    }                                                                       \
    __VA_ARGS__ operator/(T number) const {                                 \
        return Math::RectangularMatrix<cols, rows, T>::operator/(number);   \
    }

#define MAGNUM_MATRIX_OPERATOR_IMPLEMENTATION(...)                          \
    template<std::size_t size, class T> inline __VA_ARGS__ operator*(typename std::common_type<T>::type number, const __VA_ARGS__& matrix) { \
        return number*static_cast<const Math::RectangularMatrix<size, size, T>&>(matrix); \
    }                                                                       \
    template<std::size_t size, class T> inline __VA_ARGS__ operator/(typename std::common_type<T>::type number, const __VA_ARGS__& matrix) { \
        return number/static_cast<const Math::RectangularMatrix<size, size, T>&>(matrix); \
    }                                                                       \
    template<std::size_t size, class T> inline __VA_ARGS__ operator*(const Vector<size, T>& vector, const RectangularMatrix<size, 1, T>& matrix) { \
        return Math::RectangularMatrix<1, size, T>(vector)*matrix;          \
    }

#define MAGNUM_MATRIXn_OPERATOR_IMPLEMENTATION(size, Type)                  \
    template<class T> inline Type<T> operator*(typename std::common_type<T>::type number, const Type<T>& matrix) { \
        return number*static_cast<const Math::RectangularMatrix<size, size, T>&>(matrix); \
    }                                                                       \
    template<class T> inline Type<T> operator/(typename std::common_type<T>::type number, const Type<T>& matrix) { \
        return number/static_cast<const Math::RectangularMatrix<size, size, T>&>(matrix); \
    }                                                                       \
    template<class T> inline Type<T> operator*(const Vector<size, T>& vector, const RectangularMatrix<size, 1, T>& matrix) { \
        return Math::RectangularMatrix<1, size, T>(vector)*matrix;          \
    }
#endif

template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<cols, rows, T> RectangularMatrix<cols, rows, T>::fromDiagonal(const Vector<DiagonalSize, T>& diagonal) {
    RectangularMatrix<cols, rows, T> out;

    for(std::size_t i = 0; i != DiagonalSize; ++i)
        out[i][i] = diagonal[i];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> inline Vector<cols, T> RectangularMatrix<cols, rows, T>::row(std::size_t row) const {
    Vector<cols, T> out;

    for(std::size_t i = 0; i != cols; ++i)
        out[i] = _data[i][row];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<cols, rows, T> RectangularMatrix<cols, rows, T>::operator-() const {
    RectangularMatrix<cols, rows, T> out;

    for(std::size_t i = 0; i != cols; ++i)
        out._data[i] = -_data[i];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> template<std::size_t size> inline RectangularMatrix<size, rows, T> RectangularMatrix<cols, rows, T>::operator*(const RectangularMatrix<size, cols, T>& other) const {
    RectangularMatrix<size, rows, T> out;

    for(std::size_t col = 0; col != size; ++col)
        for(std::size_t row = 0; row != rows; ++row)
            for(std::size_t pos = 0; pos != cols; ++pos)
                out[col][row] += _data[pos][row]*other._data[col][pos];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<rows, cols, T> RectangularMatrix<cols, rows, T>::transposed() const {
    RectangularMatrix<rows, cols, T> out;

    for(std::size_t col = 0; col != cols; ++col)
        for(std::size_t row = 0; row != rows; ++row)
            out[row][col] = _data[col][row];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> auto RectangularMatrix<cols, rows, T>::diagonal() const -> Vector<DiagonalSize, T> {
    Vector<DiagonalSize, T> out;

    for(std::size_t i = 0; i != DiagonalSize; ++i)
        out[i] = _data[i][i];

    return out;
}

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
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, Magnum::Float>>;
#ifndef MAGNUM_TARGET_GLES
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, Magnum::Double>>;
#endif

/* Rectangular matrices */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, Magnum::Float>>;
#ifndef MAGNUM_TARGET_GLES
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, Magnum::Double>>;
#endif
#endif

}}

#endif
