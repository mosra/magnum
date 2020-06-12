#ifndef Magnum_Math_Matrix_h
#define Magnum_Math_Matrix_h
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
 * @brief Class @ref Magnum::Math::Matrix, alias @ref Magnum::Math::Matrix2x2, @ref Magnum::Math::Matrix3x3, @ref Magnum::Math::Matrix4x4
 */

#include "Magnum/Math/RectangularMatrix.h"

namespace Magnum { namespace Math {

namespace Implementation {
    template<std::size_t, class> struct MatrixDeterminant;

    template<std::size_t size, std::size_t col, std::size_t otherSize, class T, std::size_t ...row> constexpr Vector<size, T> valueOrIdentityVector(Sequence<row...>, const RectangularMatrix<otherSize, otherSize, T>& other) {
        return {(col < otherSize && row < otherSize ? other[col][row] :
            col == row ? T{1} : T{0})...};
    }

    template<std::size_t size, std::size_t col, std::size_t otherSize, class T> constexpr Vector<size, T> valueOrIdentityVector(const RectangularMatrix<otherSize, otherSize, T>& other) {
        return valueOrIdentityVector<size, col>(typename Implementation::GenerateSequence<size>::Type(), other);
    }
}

/**
@brief Square matrix
@tparam size    Matrix size
@tparam T       Data type

See @ref matrix-vector for brief introduction.

@configurationvalueref{Magnum::Math::Matrix}
@see @ref Matrix2x2, @ref Matrix3x3, @ref Matrix4x4, @ref Magnum::Matrix2x2,
    @ref Magnum::Matrix2x2d, @ref Magnum::Matrix2x2h, @ref Magnum::Matrix2x2b,
    @ref Magnum::Matrix2x2s, @ref Magnum::Matrix3x3, @ref Magnum::Matrix3x3d,
    @ref Magnum::Matrix3x3h, @ref Magnum::Matrix3x3b, @ref Magnum::Matrix3x3s,
    @ref Magnum::Matrix4x4, @ref Magnum::Matrix4x4d, @ref Magnum::Matrix4x4h,
    @ref Magnum::Matrix4x4b, @ref Magnum::Matrix4x4s
 */
template<std::size_t size, class T> class Matrix: public RectangularMatrix<size, size, T> {
    public:
        enum: std::size_t {
            Size = size         /**< Matrix size */
        };

        /**
         * @brief Default constructor
         *
         * Equivalent to @ref Matrix(IdentityInitT, T).
         */
        constexpr /*implicit*/ Matrix() noexcept: RectangularMatrix<size, size, T>{typename Implementation::GenerateSequence<size>::Type(), Vector<size, T>(T(1))} {}

        /**
         * @brief Construct an identity matrix
         *
         * The @p value allows you to specify a value on diagonal.
         * @see @ref fromDiagonal()
         */
        constexpr explicit Matrix(IdentityInitT, T value = T(1)) noexcept: RectangularMatrix<size, size, T>{typename Implementation::GenerateSequence<size>::Type(), Vector<size, T>(value)} {}

        /** @copydoc RectangularMatrix::RectangularMatrix(ZeroInitT) */
        constexpr explicit Matrix(ZeroInitT) noexcept: RectangularMatrix<size, size, T>{ZeroInit} {}

        /** @copydoc RectangularMatrix::RectangularMatrix(Magnum::NoInitT) */
        constexpr explicit Matrix(Magnum::NoInitT) noexcept: RectangularMatrix<size, size, T>{Magnum::NoInit} {}

        /** @brief Construct from column vectors */
        template<class ...U> constexpr /*implicit*/ Matrix(const Vector<size, T>& first, const U&... next) noexcept: RectangularMatrix<size, size, T>(first, next...) {}

        /** @brief Construct with one value for all elements */
        constexpr explicit Matrix(T value) noexcept: RectangularMatrix<size, size, T>{typename Implementation::GenerateSequence<size>::Type(), value} {}

        /**
         * @brief Construct from a matrix of adifferent type
         *
         * Performs only default casting on the values, no rounding or
         * anything else. Example usage:
         *
         * @snippet MagnumMath.cpp Matrix-conversion
         */
        template<class U> constexpr explicit Matrix(const RectangularMatrix<size, size, U>& other) noexcept: RectangularMatrix<size, size, T>(other) {}

        /** @brief Construct matrix from external representation */
        template<class U, class V = decltype(Implementation::RectangularMatrixConverter<size, size, T, U>::from(std::declval<U>()))> constexpr explicit Matrix(const U& other): RectangularMatrix<size, size, T>(Implementation::RectangularMatrixConverter<size, size, T, U>::from(other)) {}

        /**
         * @brief Construct by slicing or expanding a matrix of different size
         *
         * If the other matrix is larger, takes only the first @cpp size @ce
         * columns and rows from it; if the other matrix is smaller, it's
         * expanded to an identity (ones on diagonal, zeros elsewhere).
         */
        template<std::size_t otherSize> constexpr explicit Matrix(const RectangularMatrix<otherSize, otherSize, T>& other) noexcept: Matrix<size, T>{typename Implementation::GenerateSequence<size>::Type(), other} {}

        /** @brief Copy constructor */
        constexpr /*implicit*/ Matrix(const RectangularMatrix<size, size, T>& other) noexcept: RectangularMatrix<size, size, T>(other) {}

        /**
         * @brief Whether the matrix is orthogonal
         *
         * Returns @cpp true @ce if all basis vectors have unit length and are
         * orthogonal to each other. In other words, when its transpose is
         * equal to its inverse: @f[
         *      Q^T = Q^{-1}
         * @f]
         * @see @ref transposed(), @ref inverted(),
         *      @ref Matrix3::isRigidTransformation(),
         *      @ref Matrix4::isRigidTransformation(),
         *      @ref Algorithms::gramSchmidtOrthogonalizeInPlace(),
         *      @ref Algorithms::gramSchmidtOrthonormalizeInPlace()
         */
        bool isOrthogonal() const;

        /**
         * @brief Trace of the matrix
         *
         * @f[
         * tr(A) = \sum_{i=1}^n a_{i,i}
         * @f]
         */
        T trace() const { return RectangularMatrix<size, size, T>::diagonal().sum(); }

        /**
         * @brief Matrix without given column and row
         *
         * For the following matrix @f$ \boldsymbol{M} @f$,
         * @f$ \boldsymbol{M}_{3,2} @f$ is defined as: @f[
         *  \begin{array}{rcl}
         *      \boldsymbol{M} & = & \begin{pmatrix}
         *          \,\,\,1 & 4 & 7 \\
         *          \,\,\,3 & 0 & 5 \\
         *          -1 & 9 & \!11 \\
         *      \end{pmatrix} \\[2em]
         *      \boldsymbol{M}_{2,3} & = & \begin{pmatrix}
         *          \,\,1 & 4 & \Box\, \\
         *          \,\Box & \Box & \Box\, \\
         *          -1 & 9 & \Box\, \\
         *      \end{pmatrix} = \begin{pmatrix}
         *          \,\,\,1 & 4\, \\
         *          -1 & 9\, \\
         *      \end{pmatrix}
         *  \end{array}
         * @f]
         *
         * @see @ref cofactor(), @ref adjugate(), @ref determinant()
         */
        Matrix<size-1, T> ij(std::size_t skipCol, std::size_t skipRow) const;

        /**
         * @brief Cofactor
         * @m_since{2019,10}
         *
         * Cofactor @f$ C_{i,j} @f$ of a matrix @f$ \boldsymbol{M} @f$ is
         * defined as @f$ C_{i,j} = (-1)^{i + j} \det \boldsymbol{M}_{i,j} @f$,
         * with @f$ \boldsymbol{M}_{i,j} @f$ being @f$ \boldsymbol{M} @f$
         * without the i-th column and j-th row. For example, calculating
         * @f$ C_{3,2} @f$ of @f$ \boldsymbol{M} @f$ defined as @f[
         *  \boldsymbol{M} = \begin{pmatrix}
         *      \,\,\,1 & 4 & 7 \\
         *      \,\,\,3 & 0 & 5 \\
         *      -1 & 9 & \!11 \\
         *  \end{pmatrix}
         * @f]
         *
         * @m_class{m-noindent}
         *
         * would be @f[
         *  C_{3,2} = (-1)^{2 + 3} \det \begin{pmatrix}
         *      \,\,1 & 4 & \Box\, \\
         *      \,\Box & \Box & \Box\, \\
         *      -1 & 9 & \Box\, \\
         *  \end{pmatrix} = -\det \begin{pmatrix}
         *      \,\,\,1 & 4\, \\
         *      -1 & 9\, \\
         *  \end{pmatrix} = -(9-(-4)) = -13
         * @f]
         *
         * @see @ref ij(), @ref comatrix(), @ref adjugate()
         */
        T cofactor(std::size_t col, std::size_t row) const;

        /**
         * @brief Matrix of cofactors
         * @m_since{2019,10}
         *
         * A cofactor matrix @f$ \boldsymbol{C} @f$ of a matrix
         * @f$ \boldsymbol{M} @f$ is defined as the following, with each
         * @f$ C_{i,j} @f$ calculated as in @ref cofactor(). @f[
         *  \boldsymbol C = \begin{pmatrix}
         *      C_{1,1}  & C_{2,1} & \cdots &   C_{n,1} \\
         *      C_{1,2}  & C_{2,2} & \cdots &   C_{n,2} \\
         *      \vdots & \vdots & \ddots & \vdots \\
         *      C_{1,n}  & C_{2,n} & \cdots &  C_{n,n}
         *  \end{pmatrix}
         * @f]
         *
         * @see @ref Matrix4::normalMatrix(), @ref ij(), @ref adjugate()
         */
        Matrix<size, T> comatrix() const;

        /**
         * @brief Adjugate matrix
         * @m_since{2019,10}
         *
         * @f$ adj(A) @f$. Transpose of a @ref comatrix(), used for example to
         * calculate an @ref inverted() matrix.
         */
        Matrix<size, T> adjugate() const;

        /**
         * @brief Determinant
         *
         * Returns 0 if the matrix is noninvertible and 1 if the matrix is
         * orthogonal. Computed recursively using
         * <a href="https://en.wikipedia.org/wiki/Determinant#Laplace's_formula_and_the_adjugate_matrix">Laplace's formula</a>: @f[
         *      \det \boldsymbol{A} = \sum_{j=1}^n (-1)^{i+j} a_{i,j} \det \boldsymbol{A}_{i,j}
         * @f] @f$ \boldsymbol{A}_{i,j} @f$ is @f$ \boldsymbol{A} @f$ without
         * the i-th column and j-th row. The formula is recursed down to a 2x2
         * matrix, where the determinant is calculated directly: @f[
         *      \det \boldsymbol{A} = a_{0, 0} a_{1, 1} - a_{1, 0} a_{0, 1}
         * @f]
         *
         * @see @ref ij()
         */
        T determinant() const { return Implementation::MatrixDeterminant<size, T>()(*this); }

        /**
         * @brief Inverted matrix
         *
         * Calculated using <a href="https://en.wikipedia.org/wiki/Cramer's_rule">Cramer's rule</a> and @ref adjugate(), or equivalently
         * using a @ref comatrix(): @f[
         *      \boldsymbol{A}^{-1} = \frac{1}{\det \boldsymbol{A}} adj(\boldsymbol{A}) = \frac{1}{\det \boldsymbol{A}} \boldsymbol{C}^T
         * @f]
         * See @ref invertedOrthogonal(), @ref Matrix3::invertedRigid() and
         * @ref Matrix4::invertedRigid() which are faster alternatives for
         * particular matrix types.
         * @see @ref Algorithms::gaussJordanInverted(),
         *      @ref Matrix4::normalMatrix()
         * @m_keyword{inverse(),GLSL inverse(),}
         */
        Matrix<size, T> inverted() const;

        /**
         * @brief Inverted orthogonal matrix
         *
         * Equivalent to @ref transposed(), expects that the matrix is
         * orthogonal. @f[
         *      \boldsymbol{A}^{-1} = \boldsymbol{A}^T
         * @f]
         * @see @ref inverted(), @ref isOrthogonal(),
         *      @ref Matrix3::invertedRigid(),
         *      @ref Matrix4::invertedRigid()
         */
        Matrix<size, T> invertedOrthogonal() const {
            CORRADE_ASSERT(isOrthogonal(),
                "Math::Matrix::invertedOrthogonal(): the matrix is not orthogonal:" << Corrade::Utility::Debug::Debug::newline << *this, {});
            return RectangularMatrix<size, size, T>::transposed();
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Reimplementation of functions to return correct type */
        Matrix<size, T> operator*(const Matrix<size, T>& other) const {
            return RectangularMatrix<size, size, T>::operator*(other);
        }
        template<std::size_t otherCols> RectangularMatrix<otherCols, size, T> operator*(const RectangularMatrix<otherCols, size, T>& other) const {
            return RectangularMatrix<size, size, T>::operator*(other);
        }
        Vector<size, T> operator*(const Vector<size, T>& other) const {
            return RectangularMatrix<size, size, T>::operator*(other);
        }
        Matrix<size, T> transposed() const {
            return RectangularMatrix<size, size, T>::transposed();
        }
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_IMPLEMENTATION(size, size, Matrix<size, T>)
        #endif

    private:
        friend struct Implementation::MatrixDeterminant<size, T>;

        /* Implementation for RectangularMatrix<cols, rows, T>::RectangularMatrix(const RectangularMatrix<cols, rows, U>&) */
        template<std::size_t otherSize, std::size_t ...col> constexpr explicit Matrix(Implementation::Sequence<col...>, const RectangularMatrix<otherSize, otherSize, T>& other) noexcept: RectangularMatrix<size, size, T>{Implementation::valueOrIdentityVector<size, col>(other)...} {}
};

/**
@brief 2x2 matrix

Convenience alternative to `Matrix<2, T>`. See @ref Matrix for more
information.
@see @ref Magnum::Matrix2x2, @ref Magnum::Matrix2x2d, @ref Magnum::Matrix2x2h,
    @ref Magnum::Matrix2x2b, @ref Magnum::Matrix2x2s
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix2x2 = Matrix<2, T>;
#endif

/**
@brief 3x3 matrix

Convenience alternative to `Matrix<3, T>`. See @ref Matrix for more
information. Note that this is different from @ref Matrix3, which contains
additional functions for transformations in 2D.
@see @ref Magnum::Matrix3x3, @ref Magnum::Matrix3x3d, @ref Magnum::Matrix3x3h,
    @ref Magnum::Matrix3x3b, @ref Magnum::Matrix3x3s
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix3x3 = Matrix<3, T>;
#endif

/**
@brief 4x4 matrix

Convenience alternative to `Matrix<4, T>`. See @ref Matrix for more
information. Note that this is different from @ref Matrix4, which contains
additional functions for transformations in 3D.
@see @ref Magnum::Matrix4x4, @ref Magnum::Matrix4x4d, @ref Magnum::Matrix4x4h,
    @ref Magnum::Matrix4x4b, @ref Magnum::Matrix4x4s
*/
#ifndef CORRADE_MSVC2015_COMPATIBILITY /* Multiple definitions still broken */
template<class T> using Matrix4x4 = Matrix<4, T>;
#endif

MAGNUM_MATRIX_OPERATOR_IMPLEMENTATION(Matrix<size, T>)

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_MATRIX_SUBCLASS_IMPLEMENTATION(size, Type, VectorType)       \
    VectorType<T>& operator[](std::size_t col) {                            \
        return static_cast<VectorType<T>&>(Matrix<size, T>::operator[](col)); \
    }                                                                       \
    constexpr const VectorType<T> operator[](std::size_t col) const {       \
        return VectorType<T>(Matrix<size, T>::operator[](col));             \
    }                                                                       \
    VectorType<T> row(std::size_t row) const {                              \
        return VectorType<T>(Matrix<size, T>::row(row));                    \
    }                                                                       \
                                                                            \
    Type<T> operator*(const Matrix<size, T>& other) const {                 \
        return Matrix<size, T>::operator*(other);                           \
    }                                                                       \
    template<std::size_t otherCols> RectangularMatrix<otherCols, size, T> operator*(const RectangularMatrix<otherCols, size, T>& other) const { \
        return Matrix<size, T>::operator*(other);                           \
    }                                                                       \
    VectorType<T> operator*(const Vector<size, T>& other) const {           \
        return Matrix<size, T>::operator*(other);                           \
    }                                                                       \
                                                                            \
    Type<T> transposed() const { return Matrix<size, T>::transposed(); }    \
    constexpr VectorType<T> diagonal() const { return Matrix<size, T>::diagonal(); } \
    Type<T> inverted() const { return Matrix<size, T>::inverted(); }        \
    Type<T> invertedOrthogonal() const {                                    \
        return Matrix<size, T>::invertedOrthogonal();                       \
    }

namespace Implementation {

template<std::size_t size, class T> struct MatrixDeterminant {
    T operator()(const Matrix<size, T>& m) {
        T out(0);

        /* Using ._data[] instead of [] to avoid function call indirection on
           debug builds (saves a lot, yet doesn't obfuscate too much) */
        for(std::size_t col = 0; col != size; ++col)
            out += m._data[col]._data[0]*m.cofactor(col, 0);

        return out;
    }

    T operator()(const Matrix<size + 1, T>& m, const std::size_t skipCol, const std::size_t skipRow) {
        return m.ij(skipCol, skipRow).determinant();
    }
};

/* This is not *critically* needed here (the specializations for 2x2 and 1x1
   are technically enough to make things work), but together with the raw data
   access it speeds up the debug build five times, so I think it's worth to
   have it */
template<class T> struct MatrixDeterminant<3, T> {
    constexpr T operator()(const Matrix<3, T>& m) const {
        /* Using ._data[] instead of [] to avoid function call indirection
           on debug builds (saves a lot, yet doesn't obfuscate too much) */
        return m._data[0]._data[0]*((m._data[1]._data[1]*m._data[2]._data[2]) - (m._data[2]._data[1]*m._data[1]._data[2])) -
            m._data[0]._data[1]*(m._data[1]._data[0]*m._data[2]._data[2] - m._data[2]._data[0]*m._data[1]._data[2]) +
            m._data[0]._data[2]*(m._data[1]._data[0]*m._data[2]._data[1] - m._data[2]._data[0]*m._data[1]._data[1]);
    }

    /* Used internally by cofactor(), basically just an inlined variant of
       ij(skipCol, skipRow).determinant() */
    constexpr T operator()(const Matrix<4, T>& m, const std::size_t skipCol, const std::size_t skipRow) const {
        #define _col(i) _data[i + (i >= skipCol)]
        #define _row(i) _data[i + (i >= skipRow)]
        return
            m._col(0)._row(0)*((m._col(1)._row(1)*m._col(2)._row(2)) - (m._col(2)._row(1)*m._col(1)._row(2))) -
            m._col(0)._row(1)*(m._col(1)._row(0)*m._col(2)._row(2) - m._col(2)._row(0)*m._col(1)._row(2)) +
            m._col(0)._row(2)*(m._col(1)._row(0)*m._col(2)._row(1) - m._col(2)._row(0)*m._col(1)._row(1));
        #undef _col
        #undef _row
    }
};

template<class T> struct MatrixDeterminant<2, T> {
    constexpr T operator()(const Matrix<2, T>& m) const {
        /* Using ._data[] instead of [] to avoid function call indirection
           on debug builds (saves a lot, yet doesn't obfuscate too much) */
        return m._data[0]._data[0]*m._data[1]._data[1] - m._data[1]._data[0]*m._data[0]._data[1];
    }

    /* Used internally by cofactor(), basically just an inlined variant of
       ij(skipCol, skipRow).determinant() */
    constexpr T operator()(const Matrix<3, T>& m, const std::size_t skipCol, const std::size_t skipRow) const {
        #define _col(i) _data[i + (i >= skipCol)]
        #define _row(i) _data[i + (i >= skipRow)]
        return m._col(0)._row(0)*m._col(1)._row(1) - m._col(1)._row(0)*m._col(0)._row(1);
        #undef _col
        #undef _row
    }
};

template<class T> struct MatrixDeterminant<1, T> {
    constexpr T operator()(const Matrix<1, T>& m) const {
        /* Using ._data[] instead of [] to avoid function call indirection
           on debug builds (saves a lot, yet doesn't obfuscate too much) */
        return m._data[0]._data[0];
    }

    /* Used internally by cofactor(), basically just an inlined variant of
       ij(skipCol, skipRow).determinant() */
    constexpr T operator()(const Matrix<2, T>& m, const std::size_t skipCol, const std::size_t skipRow) const {
        return m._data[0 + (0 >= skipCol)]._data[0 + (0 >= skipRow)];
    }
};

template<std::size_t size, class T> struct StrictWeakOrdering<Matrix<size, T>>: StrictWeakOrdering<RectangularMatrix<size, size, T>> {};

}
#endif

template<std::size_t size, class T> bool Matrix<size, T>::isOrthogonal() const {
    /* Using ._data[] instead of [] to avoid function call indirection on debug
       builds (saves a lot, yet doesn't obfuscate too much) */

    /* Normality */
    for(std::size_t i = 0; i != size; ++i)
        if(!RectangularMatrix<size, size, T>::_data[i].isNormalized()) return false;

    /* Orthogonality */
    for(std::size_t i = 0; i != size-1; ++i)
        for(std::size_t j = i+1; j != size; ++j)
            if(dot(RectangularMatrix<size, size, T>::_data[i], RectangularMatrix<size, size, T>::_data[j]) > TypeTraits<T>::epsilon())
                return false;

    return true;
}

template<std::size_t size, class T> Matrix<size-1, T> Matrix<size, T>::ij(const std::size_t skipCol, const std::size_t skipRow) const {
    Matrix<size-1, T> out{Magnum::NoInit};

    /* Using ._data[] instead of [] to avoid function call indirection on debug
       builds (saves a lot, yet doesn't obfuscate too much) */
    for(std::size_t col = 0; col != size-1; ++col)
        for(std::size_t row = 0; row != size-1; ++row)
            out._data[col]._data[row] = RectangularMatrix<size, size, T>::
                _data[col + (col >= skipCol)]
               ._data[row + (row >= skipRow)];

    return out;
}

template<std::size_t size, class T> T Matrix<size, T>::cofactor(std::size_t col, std::size_t row) const {
    return (((row+col) & 1) ? -1 : 1)*Implementation::MatrixDeterminant<size - 1, T>()(*this, col, row);
}

template<std::size_t size, class T> Matrix<size, T> Matrix<size, T>::comatrix() const {
    Matrix<size, T> out{Magnum::NoInit};

    /* Using ._data[] instead of [] to avoid function call indirection on debug
       builds (saves a lot, yet doesn't obfuscate too much) */
    for(std::size_t col = 0; col != size; ++col)
        for(std::size_t row = 0; row != size; ++row)
            out._data[col]._data[row] = cofactor(col, row);

    return out;
}

template<std::size_t size, class T> Matrix<size, T> Matrix<size, T>::adjugate() const {
    Matrix<size, T> out{Magnum::NoInit};

    /* Same as comatrix(), except using cofactor(row, col) instead of
       cofactor(col, row). Could also be just comatrix().transpose() but since
       this is used in inverted(), each extra operation hurts. */
    for(std::size_t col = 0; col != size; ++col)
        for(std::size_t row = 0; row != size; ++row)
            out._data[col]._data[row] = cofactor(row, col);

    return out;
}

template<std::size_t size, class T> Matrix<size, T> Matrix<size, T>::inverted() const {
    return adjugate()/determinant();
}

}}

#endif
