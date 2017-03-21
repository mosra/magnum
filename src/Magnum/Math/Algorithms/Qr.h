#ifndef Magnum_Math_Algorithms_Qr_h
#define Magnum_Math_Algorithms_Qr_h
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
 * @brief Function @ref Magnum::Math::Algorithms::qr()
 */

#include "Magnum/Math/Matrix.h"

namespace Magnum { namespace Math { namespace Algorithms {

/**
@brief QR decomposition

Calculated using classic Gram-Schmidt process.
*/
template<std::size_t size, class T> std::pair<Matrix<size, T>, Matrix<size, T>> qr(const Matrix<size, T>& matrix) {
    Matrix<size, T> q, r;

    for(std::size_t k = 0; k != size; ++k) {
        Vector<size, T> p = matrix[k];
        for(std::size_t j = 0; j != k; ++j) {
            r[k][j] = Math::dot(p, q[j]);
            p -= q[j]*r[k][j];
        }
        r[k][k] = p.length();
        q[k] = p/r[k][k];
    }

    return {q, r};
}

}}}

#endif
