#ifndef Magnum_Math_Algorithms_GaussJordan_h
#define Magnum_Math_Algorithms_GaussJordan_h
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
 * @brief Function @ref Magnum::Math::Algorithms::gaussJordanInPlaceTransposed(), @ref Magnum::Math::Algorithms::gaussJordanInPlace(), @ref Magnum::Math::Algorithms::gaussJordanInverted()
 */

#include "Magnum/Math/Matrix.h"

namespace Magnum { namespace Math { namespace Algorithms {

/**
@brief In-place Gauss-Jordan elimination on transposed matrices
@param a     Transposed left side of augmented matrix
@param t     Transposed right side of augmented matrix
@return True if @p a is regular, false if @p a is singular (and thus the
    system cannot be solved).

As [Gauss-Jordan elimination](https://en.wikipedia.org/wiki/Gaussian_elimination)
works on rows and matrices in OpenGL are column-major, it is more efficient to
operate on transposed matrices and treat columns as rows. See also
@ref gaussJordanInPlace() which works with non-transposed matrices.

The function eliminates matrix @p a and solves @p t in place. For efficiency
reasons, only pure Gaussian elimination is done on @p a and the final
backsubstitution is done only on @p t, as @p a would always end with identity
matrix anyway.

Based on an ultra-compact Python code by Jarno Elonen,
http://elonen.iki.fi/code/misc-notes/python-gaussj/index.html.
*/
template<std::size_t size, std::size_t rows, class T> bool gaussJordanInPlaceTransposed(RectangularMatrix<size, size, T>& a, RectangularMatrix<size, rows, T>& t) {
    for(std::size_t row = 0; row != size; ++row) {
        /* Find max pivot */
        std::size_t rowMax = row;
        for(std::size_t row2 = row+1; row2 != size; ++row2)
            if(std::abs(a[row2][row]) > std::abs(a[rowMax][row]))
                rowMax = row2;

        /* Swap the rows */
        using std::swap;
        swap(a[row], a[rowMax]);
        swap(t[row], t[rowMax]);

        /* Singular */
        if(TypeTraits<T>::equals(a[row][row], T(0)))
            return false;

        /* Eliminate column */
        for(std::size_t row2 = row+1; row2 != size; ++row2) {
            T c = a[row2][row]/a[row][row];

            a[row2] -= a[row]*c;
            t[row2] -= t[row]*c;
        }
    }

    /* Backsubstitute */
    for(std::size_t row = size; row != 0; --row) {
        T c = T(1)/a[row-1][row-1];

        for(std::size_t row2 = 0; row2 != row-1; ++row2)
            t[row2] -= t[row-1]*a[row2][row-1]*c;

        /* Normalize the row */
        t[row-1] *= c;
    }

    return true;
}

/**
@brief In-place Gauss-Jordan elimination

Transposes the matrices, calls @ref gaussJordanInPlaceTransposed() on them and
then transposes them back.
*/
template<std::size_t size, std::size_t cols, class T> bool gaussJordanInPlace(RectangularMatrix<size, size, T>& a, RectangularMatrix<cols, size, T>& t) {
    a = a.transposed();
    RectangularMatrix<size, cols, T> tTransposed = t.transposed();

    bool ret = gaussJordanInPlaceTransposed(a, tTransposed);

    a = a.transposed();
    t = tTransposed.transposed();

    return ret;
}

/**
@brief Gauss-Jordan matrix inversion

Uses the [Gauss-Jordan elimination](https://en.wikipedia.org/wiki/Gaussian_elimination#Finding_the_inverse_of_a_matrix) to perform a matrix
inversion. Since @f$ (\boldsymbol{A}^{-1})^T = (\boldsymbol{A}^T)^{-1} @f$,
passes @p matrix and an identity matrix to @ref gaussJordanInPlaceTransposed();
returning the inverted matrix. Expects that the matrix is invertible.
@see @ref Matrix::inverted()
*/
template<std::size_t size, class T> Matrix<size, T> gaussJordanInverted(Matrix<size, T> matrix) {
    Matrix<size, T> inverted{Math::IdentityInit};
    CORRADE_INTERNAL_ASSERT_OUTPUT(gaussJordanInPlaceTransposed(matrix, inverted));
    return inverted;
}

}}}

#endif
