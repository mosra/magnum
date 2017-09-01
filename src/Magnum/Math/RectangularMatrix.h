#ifndef Magnum_Math_RectangularMatrix_h
#define Magnum_Math_RectangularMatrix_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::Math::RectangularMatrix, typedef @ref Magnum::Math::Matrix2x3, @ref Magnum::Math::Matrix3x2, @ref Magnum::Math::Matrix2x4, @ref Magnum::Math::Matrix4x2, @ref Magnum::Math::Matrix3x4, @ref Magnum::Math::Matrix4x3
 */

#include "Magnum/Math/Vector.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<std::size_t, std::size_t, class, class> struct RectangularMatrixConverter;
}

/**
@brief Rectangular matrix
@tparam cols    Column count
@tparam rows    Row count
@tparam T       Underlying data type

See @ref matrix-vector for brief introduction. See also @ref Matrix (square)
and @ref Vector.

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
        typedef T Type;         /**< @brief Underlying data type */

        enum: std::size_t {
            Cols = cols,        /**< Matrix column count */
            Rows = rows,        /**< Matrix row count */

            /**
             * Size of matrix diagonal
             * @see @ref fromDiagonal(), @ref diagonal()
             */
            DiagonalSize = (cols < rows ? cols : rows)
        };

        /**
         * @brief Matrix from array
         * @return Reference to the data as if it was matrix, thus doesn't
         *      perform any copying.
         *
         * @attention Use with caution, the function doesn't check whether the
         *      array is long enough.
         */
        static RectangularMatrix<cols, rows, T>& from(T* data) {
            return *reinterpret_cast<RectangularMatrix<cols, rows, T>*>(data);
        }
        /** @overload */
        static const RectangularMatrix<cols, rows, T>& from(const T* data) {
            return *reinterpret_cast<const RectangularMatrix<cols, rows, T>*>(data);
        }

        /**
         * @brief Construct matrix from vector
         *
         * Rolls the vector into matrix, i.e. first `rows` elements of the
         * vector will make first column of resulting matrix.
         * @see @ref toVector()
         */
        static RectangularMatrix<cols, rows, T> fromVector(const Vector<cols*rows, T>& vector) {
            return *reinterpret_cast<const RectangularMatrix<cols, rows, T>*>(vector.data());
        }

        /**
         * @brief Construct diagonal matrix
         *
         * @see @ref diagonal()
         */
        constexpr static RectangularMatrix<cols, rows, T> fromDiagonal(const Vector<DiagonalSize, T>& diagonal) noexcept {
			return RectangularMatrix(std::make_index_sequence<cols>{}, diagonal);
        }

        /** @brief Construct zero-filled matrix */
        constexpr /*implicit*/ RectangularMatrix(ZeroInitT = ZeroInit) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : RectangularMatrix<cols, rows, T>{std::make_index_sequence<cols>{}, ZeroInit}
            #endif
            {}

        /** @brief Construct matrix without initializing the contents */
        explicit RectangularMatrix(NoInitT) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : RectangularMatrix<cols, rows, T>{std::make_index_sequence<cols>{}, NoInit}
            #endif
            {}

        /** @brief Construct matrix from column vectors */
        template<class ...U> constexpr /*implicit*/ RectangularMatrix(const Vector<rows, T>& first, const U&... next) noexcept: _data{first, next...} {
            static_assert(sizeof...(next)+1 == cols, "Improper number of arguments passed to RectangularMatrix constructor");
        }

        /** @brief Construct matrix with one value for all components */
        constexpr explicit RectangularMatrix(T value) noexcept
            /** @todoc remove workaround when doxygen is sane */
            #ifndef DOXYGEN_GENERATING_OUTPUT
            : RectangularMatrix{std::make_index_sequence<cols>{}, value}
            #endif
            {}

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
        template<class U> constexpr explicit RectangularMatrix(const RectangularMatrix<cols, rows, U>& other) noexcept: RectangularMatrix(std::make_index_sequence<cols>{}, other) {}

        /** @brief Construct matrix from external representation */
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<cols, rows, T, U>::from(std::declval<U>()))> constexpr explicit RectangularMatrix(const U& other): RectangularMatrix(Implementation::RectangularMatrixConverter<cols, rows, T, U>::from(other)) {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ RectangularMatrix(const RectangularMatrix<cols, rows, T>&) noexcept = default;

        /** @brief Convert matrix to external representation */
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<cols, rows, T, U>::to(std::declval<RectangularMatrix<cols, rows, T>>()))> constexpr explicit operator U() const {
            return Implementation::RectangularMatrixConverter<cols, rows, T, U>::to(*this);
        }

        /**
         * @brief Raw data
         * @return One-dimensional array of `cols*rows` length in column-major
         *      order.
         *
         * @see @ref operator[]()
         */
        T* data() { return _data[0].data(); }
        constexpr const T* data() const { return _data[0].data(); } /**< @overload */

        /**
         * @brief Matrix column
         *
         * Particular elements can be accessed using @ref Vector::operator[](),
         * e.g.:
         * @code
         * RectangularMatrix<4, 3, Float> m;
         * Float a = m[2][1];
         * @endcode
         *
         * @see @ref row(), @ref data()
         */
        Vector<rows, T>& operator[](std::size_t col) { return _data[col]; }
        /* returns const& so [][] operations are also constexpr */
        constexpr const Vector<rows, T>& operator[](std::size_t col) const { return _data[col]; } /**< @overload */

        /**
         * @brief Matrix row
         *
         * Consider using @ref transposed() when accessing rows frequently, as
         * this is slower than accessing columns due to the way the matrix is
         * stored.
         * @see @ref setRow(), @ref operator[]()
         */
        Vector<cols, T> row(std::size_t row) const;

        /**
         * @brief Set matrix row
         *
         * Consider using @ref transposed() when accessing rows frequently, as
         * this is slower than accessing columns due to the way the matrix is
         * stored.
         * @see @ref row(), @ref operator[]()
         */
        void setRow(std::size_t row, const Vector<cols, T>& data);

        /** @brief Equality comparison */
        bool operator==(const RectangularMatrix<cols, rows, T>& other) const {
            for(std::size_t i = 0; i != cols; ++i)
                if(_data[i] != other._data[i]) return false;

            return true;
        }

        /**
         * @brief Non-equality operator
         *
         * @see @ref Vector::operator<(), @ref Vector::operator<=(),
         *      @ref Vector::operator>=(), @ref Vector::operator>()
         */
        bool operator!=(const RectangularMatrix<cols, rows, T>& other) const {
            return !operator==(other);
        }

        /**
         * @brief Component-wise less than
         *
         * Calls @ref Vector::operator<() on @ref toVector().
         */
        BoolVector<cols*rows> operator<(const RectangularMatrix<cols, rows, T>& other) const {
            return toVector() < other.toVector();
        }

        /**
         * @brief Component-wise less than or equal
         *
         * Calls @ref Vector::operator<=() on @ref toVector().
         */
        BoolVector<cols*rows> operator<=(const RectangularMatrix<cols, rows, T>& other) const {
            return toVector() <= other.toVector();
        }

        /**
         * @brief Component-wise greater than or equal
         *
         * Calls @ref Vector::operator>=() on @ref toVector().
         */
        BoolVector<cols*rows> operator>=(const RectangularMatrix<cols, rows, T>& other) const {
            return toVector() >= other.toVector();
        }

        /**
         * @brief Component-wise greater than
         *
         * Calls @ref Vector::operator>() on @ref toVector().
         */
        BoolVector<cols*rows> operator>(const RectangularMatrix<cols, rows, T>& other) const {
            return toVector() > other.toVector();
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
         * @see @ref operator+=()
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
         * @see @ref operator-=()
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
         * @see @ref operator*=(T), @ref operator*(T, const RectangularMatrix<cols, rows, T>&)
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
         * @see @ref operator/=(T),
         *      @ref operator/(T, const RectangularMatrix<cols, rows, T>&)
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
         * @f[
         *      \boldsymbol{A}^T_ij = \boldsymbol{A}_ji
         * @f]
         * @see @ref row(), @ref flippedCols(), @ref flippedRows()
         */
        RectangularMatrix<rows, cols, T> transposed() const;

        /**
         * @brief Matrix with flipped cols
         *
         * The order of columns is reversed.
         * @see @ref transposed(), @ref flippedRows(), @ref Vector::flipped()
         */
        constexpr RectangularMatrix<cols, rows, T> flippedCols() const {
            return flippedColsInternal(std::make_index_sequence<cols>{});
        }

        /**
         * @brief Matrix with flipped rows
         *
         * The order of rows is reversed.
         * @see @ref transposed(), @ref flippedCols(), @ref Vector::flipped()
         */
        constexpr RectangularMatrix<cols, rows, T> flippedRows() const {
            return flippedRowsInternal(std::make_index_sequence<cols>{});
        }

        /**
         * @brief Values on diagonal
         *
         * @see @ref fromDiagonal()
         */
        constexpr Vector<DiagonalSize, T> diagonal() const;

        /**
         * @brief Convert matrix to vector
         *
         * Returns the matrix unrolled into one large vector, i.e. first column
         * of the matrix will make first `rows` elements of resulting vector.
         * Useful for performing vector operations with the matrix (e.g.
         * summing the elements etc.).
         * @see @ref fromVector()
         */
        Vector<rows*cols, T> toVector() const {
            return *reinterpret_cast<const Vector<rows*cols, T>*>(data());
        }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /* Implementation for RectangularMatrix<cols, rows, T>::fromDiagonal() and Matrix<size, T>(IdentityInitT, T) */
        template<std::size_t ...sequence> constexpr explicit RectangularMatrix(std::index_sequence<sequence...>, const Vector<DiagonalSize, T>& diagonal);

        /* Implementation for RectangularMatrix<cols, rows, T>::RectangularMatrix(T) and Matrix<size, T>(T) */
        /* MSVC 2015 can't handle {} here */
        template<std::size_t ...sequence> constexpr explicit RectangularMatrix(std::index_sequence<sequence...>, T value) noexcept: _data{Vector<rows, T>((static_cast<void>(sequence), value))...} {}

    private:
        /* Implementation for RectangularMatrix<cols, rows, T>::RectangularMatrix(const RectangularMatrix<cols, rows, U>&) */
        template<class U, std::size_t ...sequence> constexpr explicit RectangularMatrix(std::index_sequence<sequence...>, const RectangularMatrix<cols, rows, U>& matrix) noexcept: _data{Vector<rows, T>(matrix[sequence])...} {}

        /* Implementation for RectangularMatrix<cols, rows, T>::RectangularMatrix(ZeroInitT) and RectangularMatrix<cols, rows, T>::RectangularMatrix(NoInitT) */
        /* MSVC 2015 can't handle {} here */
        template<class U, std::size_t ...sequence> constexpr explicit RectangularMatrix(std::index_sequence<sequence...>, U) noexcept: _data{Vector<rows, T>((static_cast<void>(sequence), U{typename U::Init{}}))...} {}

        template<std::size_t ...sequence> constexpr RectangularMatrix<cols, rows, T> flippedColsInternal(std::index_sequence<sequence...>) const {
            return {(*this)[cols - 1 - sequence]...};
        }

        template<std::size_t ...sequence> constexpr RectangularMatrix<cols, rows, T> flippedRowsInternal(std::index_sequence<sequence...>) const {
            return {(*this)[sequence].flipped()...};
        }

        template<std::size_t ...sequence> constexpr Vector<DiagonalSize, T> diagonalInternal(std::index_sequence<sequence...>) const;

        Vector<rows, T> _data[cols];
};

/**
@brief Matrix with 2 columns and 3 rows

Convenience alternative to `RectangularMatrix<2, 3, T>`. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix2x3, @ref Magnum::Matrix2x3d
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix2x3 = RectangularMatrix<2, 3, T>;
#endif

/**
@brief Matrix with 3 columns and 2 rows

Convenience alternative to `RectangularMatrix<3, 2, T>`. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix3x2, @ref Magnum::Matrix3x2d
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix3x2 = RectangularMatrix<3, 2, T>;
#endif

/**
@brief Matrix with 2 columns and 4 rows

Convenience alternative to `RectangularMatrix<2, 4, T>`. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix2x4, @ref Magnum::Matrix2x4d
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix2x4 = RectangularMatrix<2, 4, T>;
#endif

/**
@brief Matrix with 4 columns and 2 rows

Convenience alternative to `RectangularMatrix<4, 2, T>`. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix4x2, @ref Magnum::Matrix4x2d
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix4x2 = RectangularMatrix<4, 2, T>;
#endif

/**
@brief Matrix with 3 columns and 4 rows

Convenience alternative to `RectangularMatrix<3, 4, T>`. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix3x4, @ref Magnum::Matrix3x4d
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix3x4 = RectangularMatrix<3, 4, T>;
#endif

/**
@brief Matrix with 4 columns and 3 rows

Convenience alternative to `RectangularMatrix<4, 3, T>`. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix4x3, @ref Magnum::Matrix4x3d
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix4x3 = RectangularMatrix<4, 3, T>;
#endif

/** @relates RectangularMatrix
@brief Multiply number with matrix

Same as @ref RectangularMatrix::operator*(T) const.
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
@see @ref RectangularMatrix::operator/(T) const
*/
template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<cols, rows, T> operator/(
    #ifdef DOXYGEN_GENERATING_OUTPUT
    T
    #else
    typename std::common_type<T>::type
    #endif
    number, const RectangularMatrix<cols, rows, T>& matrix)
{
    RectangularMatrix<cols, rows, T> out{NoInit};

    for(std::size_t i = 0; i != cols; ++i)
        out[i] = number/matrix[i];

    return out;
}

/** @relates RectangularMatrix
@brief Multiply vector with rectangular matrix

Internally the same as multiplying one-column matrix with one-row matrix. @f[
    (\boldsymbol {aA})_{ji} = \boldsymbol a_i \boldsymbol A_j
@f]
@see @ref RectangularMatrix::operator*(const RectangularMatrix<size, cols, T>&) const
*/
template<std::size_t size, std::size_t cols, class T> inline RectangularMatrix<cols, size, T> operator*(const Vector<size, T>& vector, const RectangularMatrix<cols, 1, T>& matrix) {
    return RectangularMatrix<1, size, T>(vector)*matrix;
}

/** @debugoperator{Magnum::Math::RectangularMatrix} */
template<std::size_t cols, std::size_t rows, class T> Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug& debug, const Magnum::Math::RectangularMatrix<cols, rows, T>& value) {
    debug << "Matrix(" << Corrade::Utility::Debug::nospace;
    for(std::size_t row = 0; row != rows; ++row) {
        if(row != 0) debug << Corrade::Utility::Debug::nospace << ",\n      ";
        for(std::size_t col = 0; col != cols; ++col) {
            if(col != 0) debug << Corrade::Utility::Debug::nospace << ",";
            debug << value[col][row];
        }
    }
    return debug << Corrade::Utility::Debug::nospace << ")";
}

/* Explicit instantiation for commonly used types */
#ifndef DOXYGEN_GENERATING_OUTPUT
/* Square matrices */
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<2, 2, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<3, 3, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<4, 4, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<2, 2, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<3, 3, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<4, 4, Double>&);

/* Rectangular matrices */
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<2, 3, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<3, 2, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<2, 4, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<4, 2, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<3, 4, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<4, 3, Float>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<2, 3, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<3, 2, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<2, 4, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<4, 2, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<3, 4, Double>&);
extern template MAGNUM_EXPORT Corrade::Utility::Debug& operator<<(Corrade::Utility::Debug&, const RectangularMatrix<4, 3, Double>&);

#define MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(cols, rows, ...)   \
    static __VA_ARGS__& from(T* data) {                                     \
        return *reinterpret_cast<__VA_ARGS__*>(data);                       \
    }                                                                       \
    static const __VA_ARGS__& from(const T* data) {                         \
        return *reinterpret_cast<const __VA_ARGS__*>(data);                 \
    }                                                                       \
    constexpr static __VA_ARGS__ fromDiagonal(const Vector<Math::RectangularMatrix<cols, rows, T>::DiagonalSize, T>& diagonal) { \
        return Math::RectangularMatrix<cols, rows, T>::fromDiagonal(diagonal); \
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
    }                                                                       \
    constexpr __VA_ARGS__ flippedCols() const {                             \
        return Math::RectangularMatrix<cols, rows, T>::flippedCols();       \
    }                                                                       \
    constexpr __VA_ARGS__ flippedRows() const {                             \
        return Math::RectangularMatrix<cols, rows, T>::flippedRows();       \
    }                                                                       \

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

namespace Implementation {
    template<std::size_t rows, std::size_t i, class T, std::size_t ...sequence> constexpr Vector<rows, T> diagonalMatrixColumn2(std::index_sequence<sequence...>, const T& number) {
        return {(sequence == i ? number : T(0))...};
    }
    template<std::size_t rows, std::size_t i, class T> constexpr Vector<rows, T> diagonalMatrixColumn(const T& number) {
        return diagonalMatrixColumn2<rows, i, T>(std::make_index_sequence<rows>{}, number);
    }
}

template<std::size_t cols, std::size_t rows, class T> template<std::size_t ...sequence> constexpr RectangularMatrix<cols, rows, T>::RectangularMatrix(std::index_sequence<sequence...>, const Vector<DiagonalSize, T>& diagonal): _data{Implementation::diagonalMatrixColumn<rows, sequence>(sequence < DiagonalSize ? diagonal[sequence] : T{})...} {}

template<std::size_t cols, std::size_t rows, class T> inline Vector<cols, T> RectangularMatrix<cols, rows, T>::row(std::size_t row) const {
    Vector<cols, T> out;

    for(std::size_t i = 0; i != cols; ++i)
        out[i] = _data[i][row];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> inline void RectangularMatrix<cols, rows, T>::setRow(std::size_t row, const Vector<cols, T>& data) {
    for(std::size_t i = 0; i != cols; ++i)
        _data[i][row] = data[i];
}

template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<cols, rows, T> RectangularMatrix<cols, rows, T>::operator-() const {
    RectangularMatrix<cols, rows, T> out;

    for(std::size_t i = 0; i != cols; ++i)
        out._data[i] = -_data[i];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> template<std::size_t size> inline RectangularMatrix<size, rows, T> RectangularMatrix<cols, rows, T>::operator*(const RectangularMatrix<size, cols, T>& other) const {
    RectangularMatrix<size, rows, T> out{ZeroInit};

    for(std::size_t col = 0; col != size; ++col)
        for(std::size_t row = 0; row != rows; ++row)
            for(std::size_t pos = 0; pos != cols; ++pos)
                out[col][row] += _data[pos][row]*other._data[col][pos];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<rows, cols, T> RectangularMatrix<cols, rows, T>::transposed() const {
    RectangularMatrix<rows, cols, T> out{NoInit};

    for(std::size_t col = 0; col != cols; ++col)
        for(std::size_t row = 0; row != rows; ++row)
            out[row][col] = _data[col][row];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> constexpr auto RectangularMatrix<cols, rows, T>::diagonal() const -> Vector<DiagonalSize, T> { return diagonalInternal(std::make_index_sequence<DiagonalSize>{}); }

#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::size_t cols, std::size_t rows, class T> template<std::size_t ...sequence> constexpr auto RectangularMatrix<cols, rows, T>::diagonalInternal(std::index_sequence<sequence...>) const -> Vector<DiagonalSize, T> {
    return {(*this)[sequence][sequence]...};
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
        } while(pos != std::string::npos && i != cols*rows);

        return result;
    }
};

/* Explicit instantiation for commonly used types */
#if !defined(DOXYGEN_GENERATING_OUTPUT) && !defined(__MINGW32__)
/* Square matrices */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 4, Magnum::Double>>;

/* Rectangular matrices */
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, Magnum::Float>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 3, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<2, 4, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 2, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<3, 4, Magnum::Double>>;
extern template struct MAGNUM_EXPORT ConfigurationValue<Magnum::Math::RectangularMatrix<4, 3, Magnum::Double>>;
#endif

}}

#endif
