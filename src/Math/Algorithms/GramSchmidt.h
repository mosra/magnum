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
 * @brief Function Magnum::Math::Algorithms::gramSchmidtInPlace(), Magnum::Math::Algorithms::gramSchmidt()
 */

#include "Math/RectangularMatrix.h"

namespace Magnum { namespace Math { namespace Algorithms {

/**
@brief Gram-Schmidt matrix orthonormalization
@param[in,out] matrix   Matrix to perform orthonormalization on
*/
template<std::size_t cols, std::size_t rows, class T> void gramSchmidtInPlace(RectangularMatrix<cols, rows, T>& matrix) {
    static_assert(cols <= rows, "Unsupported matrix aspect ratio");
    for(std::size_t i = 0; i != cols; ++i) {
        matrix[i] = matrix[i].normalized();
        for(std::size_t j = i+1; j != cols; ++j)
            matrix[j] -= matrix[j].projected(matrix[i]);
    }
}

/**
@brief Gram-Schmidt matrix orthonormalization

Unlike gramSchmidtInPlace() returns the modified matrix instead of performing
the orthonormalization in-place.
*/
template<std::size_t cols, std::size_t rows, class T> RectangularMatrix<cols, rows, T> gramSchmidt(RectangularMatrix<cols, rows, T> matrix) {
    gramSchmidtInPlace(matrix);
    return matrix;
}

}}}

#endif
