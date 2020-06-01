#ifndef Magnum_Math_Algorithms_Qr_h
#define Magnum_Math_Algorithms_Qr_h
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
 * @brief Function @ref Magnum::Math::Algorithms::qr()
 */

#include "Magnum/Math/Algorithms/GramSchmidt.h"

namespace Magnum { namespace Math { namespace Algorithms {

/**
@brief QR decomposition

Calculated using the [Gram-Schmidt process](https://en.wikipedia.org/wiki/QR_decomposition#Using_the_Gram–Schmidt_process),
in particular the [modifed Gram-Schmidt](https://en.wikipedia.org/wiki/Gram–Schmidt_process#Numerical_stability)
from @ref gramSchmidtOrthonormalize(). Given the input matrix
@f$ \boldsymbol{A} = (\boldsymbol{a}_1, \cdots, \boldsymbol{a}_n) @f$ and the set of
column vectors @f$ \boldsymbol{e}_k @f$ coming from the Gram-Schmidt process,
the resulting @f$ \boldsymbol{Q} @f$ and @f$ \boldsymbol{R} @f$ matrices are as
follows: @f[
    \begin{array}{rcl}
        \boldsymbol{Q} & = & \left( \boldsymbol{e}_1, \cdots, \boldsymbol{e}_n \right) \\[10pt]
        \boldsymbol{R} & = & \begin{pmatrix}
            \boldsymbol{e}_1 \cdot \boldsymbol{a}_1 & \boldsymbol{e}_1 \cdot \boldsymbol{a}_2 &  \boldsymbol{e}_1 \cdot \boldsymbol{a}_3 & \ldots \\
            0 & \boldsymbol{e}_2 \cdot \boldsymbol{a}_2 & \boldsymbol{e}_2 \cdot \boldsymbol{a}_3 & \ldots \\
            0 & 0 & \boldsymbol{e}_3 \cdot \boldsymbol{a}_3 & \ldots \\
            \vdots & \vdots & \vdots & \ddots
        \end{pmatrix}
    \end{array}
@f]

One possible use is to decompose a transformation matrix into separate rotation
and scaling/shear parts. Note, however, that the decomposition is not unique.
See the [associated test case](https://github.com/mosra/magnum/blob/master/src/Magnum/Math/Algorithms/Test/QrTest.cpp)
for an example.
@see @ref svd(), @ref Matrix3::rotationShear(), @ref Matrix4::rotationShear()
*/
template<std::size_t size, class T> std::pair<Matrix<size, T>, Matrix<size, T>> qr(const Matrix<size, T>& matrix) {
    const Matrix<size, T> q = gramSchmidtOrthonormalize(matrix);
    Matrix<size, T> r{ZeroInit};
    for(std::size_t k = 0; k != size; ++k) {
        for(std::size_t j = 0; j <= k; ++j) {
            r[k][j] = Math::dot(q[j], matrix[k]);
        }
    }

    return {q, r};
}

}}}

#endif
