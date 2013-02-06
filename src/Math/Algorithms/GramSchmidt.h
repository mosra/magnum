#ifndef Magnum_Math_Algorithms_GramSchmidt_h
#define Magnum_Math_Algorithms_GramSchmidt_h
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
 * @brief Function Magnum::Math::Algorithms::gramSchmidtOrthogonalizeInPlace(), Magnum::Math::Algorithms::gramSchmidtOrthogonalize(), Magnum::Math::Algorithms::gramSchmidtOrthonormalizeInPlace(), Magnum::Math::Algorithms::gramSchmidtOrthonormalize()
 */

#include "Math/RectangularMatrix.h"

namespace Magnum { namespace Math { namespace Algorithms {

/**
@brief In-place Gram-Schmidt matrix orthogonalization
@param[in,out] matrix   %Matrix to perform orthogonalization on
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

Unlike gramSchmidtOrthogonalizeInPlace() returns the modified matrix instead
of performing the orthogonalization in-place.
*/
template<std::size_t cols, std::size_t rows, class T> RectangularMatrix<cols, rows, T> gramSchmidtOrthogonalize(RectangularMatrix<cols, rows, T> matrix) {
    gramSchmidtOrthogonalizeInPlace(matrix);
    return matrix;
}

/**
@brief In-place Gram-Schmidt matrix orthonormalization
@param[in,out] matrix   %Matrix to perform orthonormalization on
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

Unlike gramSchmidtOrthonormalizeInPlace() returns the modified matrix instead
of performing the orthonormalization in-place.
*/
template<std::size_t cols, std::size_t rows, class T> RectangularMatrix<cols, rows, T> gramSchmidtOrthonormalize(RectangularMatrix<cols, rows, T> matrix) {
    gramSchmidtOrthonormalizeInPlace(matrix);
    return matrix;
}

}}}

#endif
