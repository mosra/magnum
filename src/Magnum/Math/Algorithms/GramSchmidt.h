#ifndef Magnum_Math_Algorithms_GramSchmidt_h
#define Magnum_Math_Algorithms_GramSchmidt_h
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
 * @brief Function @ref Magnum::Math::Algorithms::gramSchmidtOrthogonalizeInPlace(), @ref Magnum::Math::Algorithms::gramSchmidtOrthogonalize(), @ref Magnum::Math::Algorithms::gramSchmidtOrthonormalizeInPlace(), @ref Magnum::Math::Algorithms::gramSchmidtOrthonormalize()
 */

#include "Magnum/Math/RectangularMatrix.h"

namespace Magnum { namespace Math { namespace Algorithms {

/**
@brief In-place Gram-Schmidt matrix orthogonalization
@param[in,out] matrix   Matrix to perform orthogonalization on
*/
template<std::size_t cols, std::size_t rows, class T> void gramSchmidtOrthogonalizeInPlace(RectangularMatrix<cols, rows, T>& matrix) {
    static_assert(cols <= rows, "Unsupported matrix aspect ratio");
    for(std::size_t i = 0; i != cols; ++i) {
        for(std::size_t j = i+1; j != cols; ++j)
            matrix[j] -= matrix[j].projected(matrix[i]);
    }
}

/**
@brief Gram-Schmidt matrix orthogonalization

Unlike @ref gramSchmidtOrthogonalizeInPlace() returns the modified matrix
instead of performing the orthogonalization in-place.
*/
template<std::size_t cols, std::size_t rows, class T> RectangularMatrix<cols, rows, T> gramSchmidtOrthogonalize(RectangularMatrix<cols, rows, T> matrix) {
    gramSchmidtOrthogonalizeInPlace(matrix);
    return matrix;
}

/**
@brief In-place Gram-Schmidt matrix orthonormalization
@param[in,out] matrix   Matrix to perform orthonormalization on
*/
template<std::size_t cols, std::size_t rows, class T> void gramSchmidtOrthonormalizeInPlace(RectangularMatrix<cols, rows, T>& matrix) {
    static_assert(cols <= rows, "Unsupported matrix aspect ratio");
    for(std::size_t i = 0; i != cols; ++i) {
        matrix[i] = matrix[i].normalized();
        for(std::size_t j = i+1; j != cols; ++j)
            matrix[j] -= matrix[j].projectedOntoNormalized(matrix[i]);
    }
}

/**
@brief Gram-Schmidt matrix orthonormalization

Unlike @ref gramSchmidtOrthonormalizeInPlace() returns the modified matrix
instead of performing the orthonormalization in-place.
*/
template<std::size_t cols, std::size_t rows, class T> RectangularMatrix<cols, rows, T> gramSchmidtOrthonormalize(RectangularMatrix<cols, rows, T> matrix) {
    gramSchmidtOrthonormalizeInPlace(matrix);
    return matrix;
}

}}}

#endif
