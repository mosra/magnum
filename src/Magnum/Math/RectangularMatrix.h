#ifndef Magnum_Math_RectangularMatrix_h
#define Magnum_Math_RectangularMatrix_h
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
 * @brief Class @ref Magnum::Math::RectangularMatrix, alias @ref Magnum::Math::Matrix2x3, @ref Magnum::Math::Matrix3x2, @ref Magnum::Math::Matrix2x4, @ref Magnum::Math::Matrix4x2, @ref Magnum::Math::Matrix3x4, @ref Magnum::Math::Matrix4x3
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
    @ref Matrix3x4, @ref Matrix4x3, @ref Magnum::Matrix2x3,
    @ref Magnum::Matrix2x3d, @ref Magnum::Matrix2x3h, @ref Magnum::Matrix2x3b,
    @ref Magnum::Matrix2x3s, @ref Magnum::Matrix3x2, @ref Magnum::Matrix3x2d,
    @ref Magnum::Matrix3x2h, @ref Magnum::Matrix3x2b, @ref Magnum::Matrix3x2s,
    @ref Magnum::Matrix2x4, @ref Magnum::Matrix2x4d, @ref Magnum::Matrix2x4h,
    @ref Magnum::Matrix2x4b, @ref Magnum::Matrix2x4s, @ref Magnum::Matrix4x2,
    @ref Magnum::Matrix4x2d, @ref Magnum::Matrix4x2h, @ref Magnum::Matrix4x2b,
    @ref Magnum::Matrix4x2s, @ref Magnum::Matrix3x4, @ref Magnum::Matrix3x4d,
    @ref Magnum::Matrix3x4h, @ref Magnum::Matrix3x4b, @ref Magnum::Matrix3x4s,
    @ref Magnum::Matrix4x3, @ref Magnum::Matrix4x3d, @ref Magnum::Matrix4x3h,
    @ref Magnum::Matrix4x3b, @ref Magnum::Matrix4x3s
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
         * @brief Matrix from an array
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
         * @brief Construct a matrix from a vector
         *
         * Rolls the vector into matrix, i.e. first `rows` elements of the
         * vector will make first column of resulting matrix.
         * @see @ref toVector()
         */
        static RectangularMatrix<cols, rows, T> fromVector(const Vector<cols*rows, T>& vector) {
            return *reinterpret_cast<const RectangularMatrix<cols, rows, T>*>(vector.data());
        }

        /**
         * @brief Construct a diagonal matrix
         *
         * @see @ref diagonal()
         */
        constexpr static RectangularMatrix<cols, rows, T> fromDiagonal(const Vector<DiagonalSize, T>& diagonal) noexcept {
            return RectangularMatrix(typename Implementation::GenerateSequence<cols>::Type(), diagonal);
        }

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref RectangularMatrix(ZeroInitT).
         */
        constexpr /*implicit*/ RectangularMatrix() noexcept: RectangularMatrix<cols, rows, T>{typename Implementation::GenerateSequence<cols>::Type{}, ZeroInit} {}

        /** @brief Construct a zero-filled matrix */
        constexpr explicit RectangularMatrix(ZeroInitT) noexcept: RectangularMatrix<cols, rows, T>{typename Implementation::GenerateSequence<cols>::Type{}, ZeroInit} {}

        /** @brief Construct without initializing the contents */
        explicit RectangularMatrix(Magnum::NoInitT) noexcept: RectangularMatrix<cols, rows, T>{typename Implementation::GenerateSequence<cols>::Type{}, Magnum::NoInit} {}

        /** @brief Construct from column vectors */
        template<class ...U> constexpr /*implicit*/ RectangularMatrix(const Vector<rows, T>& first, const U&... next) noexcept: _data{first, next...} {
            static_assert(sizeof...(next)+1 == cols, "Improper number of arguments passed to RectangularMatrix constructor");
        }

        /** @brief Construct with one value for all components */
        constexpr explicit RectangularMatrix(T value) noexcept: RectangularMatrix{typename Implementation::GenerateSequence<cols>::Type(), value} {}

        /**
         * @brief Construct matrix from another of different type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         *
         * @snippet MagnumMath.cpp RectangularMatrix-conversion
         */
        template<class U> constexpr explicit RectangularMatrix(const RectangularMatrix<cols, rows, U>& other) noexcept: RectangularMatrix(typename Implementation::GenerateSequence<cols>::Type(), other) {}

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
         * @brief Column at given position
         *
         * Particular elements can be accessed using @ref Vector::operator[](),
         * e.g.:
         *
         * @snippet MagnumMath.cpp RectangularMatrix-access
         *
         * @see @ref row(), @ref data()
         */
        Vector<rows, T>& operator[](std::size_t col) { return _data[col]; }
        /* returns const& so [][] operations are also constexpr */
        constexpr const Vector<rows, T>& operator[](std::size_t col) const { return _data[col]; } /**< @overload */

        /**
         * @brief Row at given position
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
         * @brief Add and assign a matrix
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
         * @brief Add a matrix
         *
         * @see @ref operator+=()
         */
        RectangularMatrix<cols, rows, T> operator+(const RectangularMatrix<cols, rows, T>& other) const {
            return RectangularMatrix<cols, rows, T>(*this)+=other;
        }

        /**
         * @brief Subtract and assign a matrix
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
         * @brief Subtract a matrix
         *
         * @see @ref operator-=()
         */
        RectangularMatrix<cols, rows, T> operator-(const RectangularMatrix<cols, rows, T>& other) const {
            return RectangularMatrix<cols, rows, T>(*this)-=other;
        }

        /**
         * @brief Multiply with a scalar and assign
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = a \boldsymbol A_j
         * @f]
         */
        RectangularMatrix<cols, rows, T>& operator*=(T scalar) {
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] *= scalar;

            return *this;
        }

        /**
         * @brief Multiply with a scalar
         *
         * @see @ref operator*=(T), @ref operator*(T, const RectangularMatrix<cols, rows, T>&)
         */
        RectangularMatrix<cols, rows, T> operator*(T scalar) const {
            return RectangularMatrix<cols, rows, T>(*this) *= scalar;
        }

        /**
         * @brief Divide with a scalar and assign
         *
         * The computation is done column-wise in-place. @f[
         *      \boldsymbol A_j = \frac{\boldsymbol A_j} a
         * @f]
         */
        RectangularMatrix<cols, rows, T>& operator/=(T scalar) {
            for(std::size_t i = 0; i != cols; ++i)
                _data[i] /= scalar;

            return *this;
        }

        /**
         * @brief Divide with a scalar
         *
         * @see @ref operator/=(T),
         *      @ref operator/(T, const RectangularMatrix<cols, rows, T>&)
         */
        RectangularMatrix<cols, rows, T> operator/(T scalar) const {
            return RectangularMatrix<cols, rows, T>(*this) /= scalar;
        }

        /**
         * @brief Multiply a matrix
         *
         * @f[
         *      (\boldsymbol {AB})_{ji} = \sum_{k=0}^{m-1} \boldsymbol A_{ki} \boldsymbol B_{jk}
         * @f]
         * @m_keyword{outerProduct(),GLSL outerProduct(),}
         */
        template<std::size_t size> RectangularMatrix<size, rows, T> operator*(const RectangularMatrix<size, cols, T>& other) const;

        /**
         * @brief Multiply a vector
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
         * @m_keyword{transpose(),GLSL transpose(),}
         */
        RectangularMatrix<rows, cols, T> transposed() const;

        /**
         * @brief Matrix with flipped cols
         *
         * The order of columns is reversed.
         * @see @ref transposed(), @ref flippedRows(), @ref Vector::flipped()
         */
        constexpr RectangularMatrix<cols, rows, T> flippedCols() const {
            return flippedColsInternal(typename Implementation::GenerateSequence<cols>::Type{});
        }

        /**
         * @brief Matrix with flipped rows
         *
         * The order of rows is reversed.
         * @see @ref transposed(), @ref flippedCols(), @ref Vector::flipped()
         */
        constexpr RectangularMatrix<cols, rows, T> flippedRows() const {
            return flippedRowsInternal(typename Implementation::GenerateSequence<cols>::Type{});
        }

        /**
         * @brief Values on diagonal
         *
         * @see @ref fromDiagonal()
         */
        constexpr Vector<DiagonalSize, T> diagonal() const {
            /* NVCC (from CUDA) has problems compiling this function under
               Windows when there's a separate definition due to DiagonalSize
               (see https://github.com/mosra/magnum/issues/345 for details) */
            return diagonalInternal(typename Implementation::GenerateSequence<DiagonalSize>::Type());
        }

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
        template<std::size_t ...sequence> constexpr explicit RectangularMatrix(Implementation::Sequence<sequence...>, const Vector<DiagonalSize, T>& diagonal);

        /* Implementation for RectangularMatrix<cols, rows, T>::RectangularMatrix(T) and Matrix<size, T>(T) */
        /* MSVC 2015 can't handle {} here */
        template<std::size_t ...sequence> constexpr explicit RectangularMatrix(Implementation::Sequence<sequence...>, T value) noexcept: _data{Vector<rows, T>((static_cast<void>(sequence), value))...} {}

    private:
        /* These two needed to access _data to speed up debug builds,
           Matrix::ij() needs access to different Matrix sizes */
        template<std::size_t, class> friend class Matrix;
        template<std::size_t, class> friend struct Implementation::MatrixDeterminant;

        /* Implementation for RectangularMatrix<cols, rows, T>::RectangularMatrix(const RectangularMatrix<cols, rows, U>&) */
        template<class U, std::size_t ...sequence> constexpr explicit RectangularMatrix(Implementation::Sequence<sequence...>, const RectangularMatrix<cols, rows, U>& matrix) noexcept: _data{Vector<rows, T>(matrix[sequence])...} {}

        /* Implementation for RectangularMatrix<cols, rows, T>::RectangularMatrix(ZeroInitT) and RectangularMatrix<cols, rows, T>::RectangularMatrix(NoInitT) */
        /* MSVC 2015 can't handle {} here */
        template<class U, std::size_t ...sequence> constexpr explicit RectangularMatrix(Implementation::Sequence<sequence...>, U) noexcept: _data{Vector<rows, T>((static_cast<void>(sequence), U{typename U::Init{}}))...} {}

        template<std::size_t ...sequence> constexpr RectangularMatrix<cols, rows, T> flippedColsInternal(Implementation::Sequence<sequence...>) const {
            return {_data[cols - 1 - sequence]...};
        }

        template<std::size_t ...sequence> constexpr RectangularMatrix<cols, rows, T> flippedRowsInternal(Implementation::Sequence<sequence...>) const {
            return {_data[sequence].flipped()...};
        }

        template<std::size_t ...sequence> constexpr Vector<DiagonalSize, T> diagonalInternal(Implementation::Sequence<sequence...>) const;

        Vector<rows, T> _data[cols];
};

/**
@brief Matrix with 2 columns and 3 rows

Convenience alternative to @cpp RectangularMatrix<2, 3, T> @ce. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix2x3, @ref Magnum::Matrix2x3d, @ref Magnum::Matrix2x3h,
    @ref Magnum::Matrix2x3b, @ref Magnum::Matrix2x3s
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix2x3 = RectangularMatrix<2, 3, T>;
#endif

/**
@brief Matrix with 3 columns and 2 rows

Convenience alternative to @cpp RectangularMatrix<3, 2, T> @ce. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix3x2, @ref Magnum::Matrix3x2d, @ref Magnum::Matrix3x2h,
    @ref Magnum::Matrix3x2b, @ref Magnum::Matrix3x2s
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix3x2 = RectangularMatrix<3, 2, T>;
#endif

/**
@brief Matrix with 2 columns and 4 rows

Convenience alternative to @cpp RectangularMatrix<2, 4, T> @ce. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix2x4, @ref Magnum::Matrix2x4d, @ref Magnum::Matrix2x4h,
    @ref Magnum::Matrix2x4b, @ref Magnum::Matrix2x4s
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix2x4 = RectangularMatrix<2, 4, T>;
#endif

/**
@brief Matrix with 4 columns and 2 rows

Convenience alternative to @cpp RectangularMatrix<4, 2, T> @ce. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix4x2, @ref Magnum::Matrix4x2d, @ref Magnum::Matrix4x2h,
    @ref Magnum::Matrix4x2b, @ref Magnum::Matrix4x2s
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix4x2 = RectangularMatrix<4, 2, T>;
#endif

/**
@brief Matrix with 3 columns and 4 rows

Convenience alternative to @cpp RectangularMatrix<3, 4, T> @ce. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix3x4, @ref Magnum::Matrix3x4d, @ref Magnum::Matrix3x4h,
    @ref Magnum::Matrix3x4b, @ref Magnum::Matrix3x4s
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix3x4 = RectangularMatrix<3, 4, T>;
#endif

/**
@brief Matrix with 4 columns and 3 rows

Convenience alternative to @cpp RectangularMatrix<4, 3, T> @ce. See
@ref RectangularMatrix for more information.
@see @ref Magnum::Matrix4x3, @ref Magnum::Matrix4x3d, @ref Magnum::Matrix4x3h,
    @ref Magnum::Matrix4x3b, @ref Magnum::Matrix4x3s
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix4x3 = RectangularMatrix<4, 3, T>;
#endif

/** @relates RectangularMatrix
@brief Multiply a scalar with a matrix

Same as @ref RectangularMatrix::operator*(T) const.
*/
template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<cols, rows, T> operator*(
    #ifdef DOXYGEN_GENERATING_OUTPUT
    T
    #else
    typename std::common_type<T>::type
    #endif
    scalar, const RectangularMatrix<cols, rows, T>& matrix)
{
    return matrix*scalar;
}

/** @relates RectangularMatrix
@brief Divide a matrix with a scalar and invert

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
    scalar, const RectangularMatrix<cols, rows, T>& matrix)
{
    RectangularMatrix<cols, rows, T> out{Magnum::NoInit};

    for(std::size_t i = 0; i != cols; ++i)
        out[i] = scalar/matrix[i];

    return out;
}

/** @relates RectangularMatrix
@brief Multiply a vector with a rectangular matrix

Internally the same as multiplying one-column matrix with one-row matrix. @f[
    (\boldsymbol {aA})_{ji} = \boldsymbol a_i \boldsymbol A_j
@f]
@see @ref RectangularMatrix::operator*(const RectangularMatrix<size, cols, T>&) const
*/
template<std::size_t size, std::size_t cols, class T> inline RectangularMatrix<cols, size, T> operator*(const Vector<size, T>& vector, const RectangularMatrix<cols, 1, T>& matrix) {
    return RectangularMatrix<1, size, T>(vector)*matrix;
}

#ifndef CORRADE_NO_DEBUG
/** @debugoperator{RectangularMatrix} */
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
#endif
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
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
    template<std::size_t rows, std::size_t i, class T, std::size_t ...sequence> constexpr Vector<rows, T> diagonalMatrixColumn2(Implementation::Sequence<sequence...>, const T& number) {
        return {(sequence == i ? number : T(0))...};
    }
    template<std::size_t rows, std::size_t i, class T> constexpr Vector<rows, T> diagonalMatrixColumn(const T& number) {
        return diagonalMatrixColumn2<rows, i, T>(typename Implementation::GenerateSequence<rows>::Type(), number);
    }
}

template<std::size_t cols, std::size_t rows, class T> template<std::size_t ...sequence> constexpr RectangularMatrix<cols, rows, T>::RectangularMatrix(Implementation::Sequence<sequence...>, const Vector<DiagonalSize, T>& diagonal): _data{Implementation::diagonalMatrixColumn<rows, sequence>(sequence < DiagonalSize ? diagonal[sequence] : T{})...} {}

template<std::size_t cols, std::size_t rows, class T> inline Vector<cols, T> RectangularMatrix<cols, rows, T>::row(std::size_t row) const {
    Vector<cols, T> out;

    /* Using ._data[] instead of [] to avoid function call indirection
       on debug builds (saves a lot, yet doesn't obfuscate too much) */
    for(std::size_t i = 0; i != cols; ++i)
        out[i] = _data[i]._data[row];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> inline void RectangularMatrix<cols, rows, T>::setRow(std::size_t row, const Vector<cols, T>& data) {
    /* Using ._data[] instead of [] to avoid function call indirection
       on debug builds (saves a lot, yet doesn't obfuscate too much) */
    for(std::size_t i = 0; i != cols; ++i)
        _data[i]._data[row] = data._data[i];
}

template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<cols, rows, T> RectangularMatrix<cols, rows, T>::operator-() const {
    RectangularMatrix<cols, rows, T> out;

    for(std::size_t i = 0; i != cols; ++i)
        out._data[i] = -_data[i];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> template<std::size_t size> inline RectangularMatrix<size, rows, T> RectangularMatrix<cols, rows, T>::operator*(const RectangularMatrix<size, cols, T>& other) const {
    RectangularMatrix<size, rows, T> out{ZeroInit};

    /* Using ._data[] instead of [] to avoid function call indirection
       on debug builds (saves a lot, yet doesn't obfuscate too much) */
    for(std::size_t col = 0; col != size; ++col)
        for(std::size_t row = 0; row != rows; ++row)
            for(std::size_t pos = 0; pos != cols; ++pos)
                out._data[col]._data[row] += _data[pos]._data[row]*other._data[col]._data[pos];

    return out;
}

template<std::size_t cols, std::size_t rows, class T> inline RectangularMatrix<rows, cols, T> RectangularMatrix<cols, rows, T>::transposed() const {
    RectangularMatrix<rows, cols, T> out{Magnum::NoInit};

    /* Using ._data[] instead of [] to avoid function call indirection
       on debug builds (saves a lot, yet doesn't obfuscate too much) */
    for(std::size_t col = 0; col != cols; ++col)
        for(std::size_t row = 0; row != rows; ++row)
            out._data[row]._data[col] = _data[col]._data[row];

    return out;
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::size_t cols, std::size_t rows, class T> template<std::size_t ...sequence> constexpr auto RectangularMatrix<cols, rows, T>::diagonalInternal(Implementation::Sequence<sequence...>) const -> Vector<DiagonalSize, T> {
    return {_data[sequence][sequence]...};
}
#endif

namespace Implementation {

template<std::size_t cols, std::size_t rows, class T> struct StrictWeakOrdering<RectangularMatrix<cols, rows, T>> {
    bool operator()(const RectangularMatrix<cols, rows, T>& a, const RectangularMatrix<cols, rows, T>& b) const {
        StrictWeakOrdering<Vector<rows, T>> o;
        for(std::size_t i = 0; i < cols; ++i) {
            if(o(a[i], b[i]))
                return true;
            if(o(b[i], a[i]))
                return false;
        }

        return false; /* a and b are equivalent */
    }
};

}

}}

#endif
