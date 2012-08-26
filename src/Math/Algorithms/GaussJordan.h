#ifndef Magnum_Math_Algorithms_GaussJordan_h
#define Magnum_Math_Algorithms_GaussJordan_h
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
 * @brief Class GaussJordan
 */

#include "Math/RectangularMatrix.h"

namespace Magnum { namespace Math { namespace Algorithms {

/**
@brief Gauss-Jordan elimination

Based on ultra-compact Python code by Jarno Elonen,
http://elonen.iki.fi/code/misc-notes/python-gaussj/index.html.
*/
class GaussJordan {
    public:
        /**
         * @brief Eliminate transposed matrices in place
         * @param a     Transposed left side of augmented matrix
         * @param t     Transposed right side of augmented matrix
         * @return True if @p a is regular, false if @p a is singular (and
         *      thus the system cannot be solved).
         *
         * As Gauss-Jordan elimination works on rows and matrices in OpenGL
         * are column-major, it is more efficient to operate on transposed
         * matrices and treat columns as rows. See also inPlace() which works
         * with non-transposed matrices.
         *
         * The function eliminates matrix @p a and solves @p t in place. For
         * efficiency reasons, only pure Gaussian elimination is done on @p a
         * and the final backsubstitution is done only on @p t, as @p a would
         * always end with identity matrix anyway.
         */
        template<size_t size, size_t rows, class T> static bool inPlaceTransposed(RectangularMatrix<size, size, T>& a, RectangularMatrix<size, rows, T>& t);

        /**
         * @brief Eliminate in place
         *
         * Transposes the matrices, calls inPlaceTransposed() on them and then
         * transposes them back.
         */
        template<size_t size, size_t cols, class T> static bool inPlace(RectangularMatrix<size, size, T>& a, RectangularMatrix<cols, size, T>& t) {
            a = a.transposed();
            RectangularMatrix<size, cols, T> tTransposed = t.transposed();

            bool ret = inPlaceTransposed(a, tTransposed);

            a = a.transposed();
            t = tTransposed.transposed();

            return ret;
        }
};

template<size_t size, size_t cols, class T> bool GaussJordan::inPlaceTransposed(RectangularMatrix<size, size, T>& a, RectangularMatrix<size, cols, T>& t) {
    for(size_t row = 0; row != size; ++row) {
        /* Find max pivot */
        size_t rowMax = row;
        for(size_t row2 = row+1; row2 != size; ++row2)
            if(std::abs(a(row2, row)) > std::abs(a(rowMax, row)))
                rowMax = row2;

        /* Swap the rows */
        std::swap(a[row], a[rowMax]);
        std::swap(t[row], t[rowMax]);

        /* Singular */
        if(MathTypeTraits<T>::equals(a(row, row), 0))
            return false;

        /* Eliminate column */
        for(size_t row2 = row+1; row2 != size; ++row2) {
            T c = a(row2, row)/a(row, row);

            a[row2] -= a[row]*c;
            t[row2] -= t[row]*c;
        }
    }

    /* Backsubstitute */
    for(size_t row = size; row != 0; --row) {
        T c = T(1)/a(row-1, row-1);

        for(size_t row2 = 0; row2 != row-1; ++row2)
            t[row2] -= t[row-1]*a(row2, row-1)*c;

        /* Normalize the row */
        t[row-1] *= c;
    }

    return true;
}

}}}

#endif
