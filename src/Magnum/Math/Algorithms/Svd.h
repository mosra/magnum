#ifndef Magnum_Math_Algorithms_Svd_h
#define Magnum_Math_Algorithms_Svd_h
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
 * @brief Function @ref Magnum::Math::Algorithms::svd()
 */

#include <tuple>

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Matrix.h"

namespace Magnum { namespace Math { namespace Algorithms {

namespace Implementation {

template<class T> T pythagoras(T a, T b) {
    T absa = std::abs(a);
    T absb = std::abs(b);
    if(absa > absb)
        return absa*std::sqrt(T(1) + Math::pow<2>(absb/absa));
    else if(absb == T(0)) /** @todo epsilon comparison? */
        return T(0);
    else
        return absb*std::sqrt(T(1) + Math::pow<2>(absa/absb));
}

template<class T> constexpr T smallestDelta();
template<> constexpr Float smallestDelta<Float>() { return 1.0e-32f; }
template<> constexpr Double smallestDelta<Double>() { return 1.0e-64; }

}

/**
@brief Singular Value Decomposition

Performs [Thin SVD](https://en.wikipedia.org/wiki/Singular-value_decomposition#Thin_SVD)
on given matrix where @p rows >= `cols`: @f[
    \boldsymbol{M} = \boldsymbol{U} \boldsymbol{\Sigma} \boldsymbol{V}^*
@f]

Returns first @p cols column vectors of @f$ \boldsymbol{U} @f$, diagonal of
@f$ \boldsymbol{\Sigma} @f$ and non-transposed @f$ \boldsymbol{V} @f$. If the
solution doesn't converge, returns zero matrices.

Full @f$ \boldsymbol{U} @f$, @f$ \boldsymbol{\Sigma} @f$ matrices and original
@f$ \boldsymbol{M} @f$ matrix can be reconstructed from the values as
following:

@snippet MagnumMathAlgorithms.cpp svd

One possible use is to decompose a transformation matrix into separate rotation
and scaling parts. Note, however, that the decomposition is not unique. See the
[associated test case](https://github.com/mosra/magnum/blob/master/src/Magnum/Math/Algorithms/Test/SvdTest.cpp)
for an example. Implementation based on *Golub, G. H.; Reinsch, C. (1970).
"Singular value decomposition and least squares solutions"*.
@see @ref qr(), @ref Matrix3::rotationShear(), @ref Matrix4::rotationShear()
*/
/* The matrix is passed by value because it is changed inside */
template<std::size_t cols, std::size_t rows, class T> std::tuple<RectangularMatrix<cols, rows, T>, Vector<cols, T>, Matrix<cols, T>> svd(RectangularMatrix<cols, rows, T> m) {
    static_assert(rows >= cols, "Unsupported matrix aspect ratio");
    static_assert(T(1)+TypeTraits<T>::epsilon() > T(1), "Epsilon too small");
    constexpr T tol = Implementation::smallestDelta<T>()/TypeTraits<T>::epsilon();
    static_assert(tol > T(0), "Tol too small");
    constexpr std::size_t maxIterations = 50;

    Matrix<cols, T> v{ZeroInit};
    Vector<cols, T> e, q;

    /* Householder's reduction to bidiagonal form */
    T g = T(0);
    T epsilonX = T(0);
    for(std::size_t i = 0; i != cols; ++i) {
        const std::size_t l = i+1;

        e[i] = g;
        T s1 = T(0);
        for(std::size_t j = i; j != rows; ++j)
            s1 += Math::pow<2>(m[i][j]);
        if(s1 > tol) {
            const T f = m[i][i];
            g = (f < T(0) ? std::sqrt(s1) : -std::sqrt(s1));
            const T h = f*g - s1;
            m[i][i] = f - g;

            for(std::size_t j = l; j != cols; ++j) {
                T s = T(0);
                for(std::size_t k = i; k != rows; ++k)
                    s += m[i][k]*m[j][k];
                const T n = s/h;
                for(std::size_t k = i; k != rows; ++k)
                    m[j][k] += n*m[i][k];
            }
        } else g = T(0);

        q[i] = g;
        T s2 = T(0);
        for(std::size_t j = l; j != cols; ++j)
            s2 += Math::pow<2>(m[j][i]);
        if(s2 > tol) {
            const T f = m[i+1][i];
            g = (f < T(0) ? std::sqrt(s2) : -std::sqrt(s2));
            const T h = f*g - s2;
            m[i+1][i] = f - g;

            for(std::size_t j = l; j != cols; ++j)
                e[j] = m[j][i]/h;

            for(std::size_t j = l; j != rows; ++j) {
                T s = T(0);
                for(std::size_t k = l; k != cols; ++k)
                    s += m[k][j]*m[k][i];
                for(std::size_t k = l; k != cols; ++k)
                    m[k][j] += s*e[k];
            }

        } else g = T(0);

        const T y = std::abs(q[i]) + std::abs(e[i]);
        if(y > epsilonX) epsilonX = y;
    }

    /* Accumulation of right hand transformations */
    for(std::size_t l = cols; l != 0; --l) {
        const std::size_t i = l-1;

        if(g != T(0)) { /** @todo epsilon check? */
            const T h = g*m[i+1][i];

            for(std::size_t j = l; j != cols; ++j)
                v[i][j] = m[j][i]/h;

            for(std::size_t j = l; j != cols; ++j) {
                T s = T(0);
                for(std::size_t k = l; k != cols; ++k)
                    s += m[k][i]*v[j][k];
                for(std::size_t k = l; k != cols; ++k)
                    v[j][k] += s*v[i][k];
            }
        }

        for(std::size_t j = l; j != cols; ++j)
            v[j][i] = v[i][j] = T(0);

        v[i][i] = T(1);
        g = e[i];
    }

    /* Accumulation of left hand transformations */
    for(std::size_t l = cols; l != 0; --l) {
        const std::size_t i = l-1;

        for(std::size_t j = l; j != cols; ++j)
            m[j][i] = T(0);

        const T d = q[i];
        if(d != T(0)) { /** @todo epsilon check? */
            const T h = m[i][i]*d;
            for(std::size_t j = l; j != cols; ++j) {
                T s = T(0);
                for(std::size_t k = l; k != rows; ++k)
                    s += m[i][k]*m[j][k];
                const T f = s/h;
                for(std::size_t k = i; k != rows; ++k)
                    m[j][k] += f*m[i][k];
            }

            for(std::size_t j = i; j != rows; ++j)
                m[i][j] /= d;

        } else for(std::size_t j = i; j != rows; ++j)
            m[i][j] = T(0);

        m[i][i] += T(1);
    }

    /* Diagonalization of the bidiagonal form */
    const T epsilon = TypeTraits<T>::epsilon()*epsilonX;
    for(std::size_t k2 = cols; k2 != 0; --k2) {
        const std::size_t k = k2 - 1;

        for(std::size_t iteration = 0; iteration != maxIterations; ++iteration) {
            /* Test for splitting */
            bool doCancellation = true;
            std::size_t l = 0;
            for(std::size_t l2 = k2; l2 != 0; --l2) {
                l = l2 - 1;
                if(std::abs(e[l]) <= epsilon) {
                    doCancellation = false;
                    break;
                } else if(std::abs(q[l-1]) <= epsilon) {
                    break;
                }
            }

            /* Cancellation */
            if(doCancellation) {
                const std::size_t l1 = l - 1;
                T c = T(0);
                T s = T(1);
                for(std::size_t i = l; i != k+1; ++i) {
                    CORRADE_INTERNAL_ASSERT(i <= k+1);

                    const T f = s*e[i];
                    e[i] = c*e[i];
                    if(std::abs(f) <= epsilon) break;

                    const T j = q[i];
                    const T h = Implementation::pythagoras(f, j);
                    q[i] = h;
                    c = j/h;
                    s = -f/h;

                    const Vector<rows, T> a = m[l1];
                    const Vector<rows, T> b = m[i];
                    m[l1] = a*c+b*s;
                    m[i] = -a*s+b*c;
                }
            }

            /* Test for convergence */
            const T z = q[k];
            if(l == k) {
                /* Invert to non-negative */
                if(z < T(0)) {
                    q[k] = -z;
                    v[k] = -v[k];
                }

                break;

            /* Exceeded iteration count, done */
            } else if(iteration >= maxIterations-1) {
                Corrade::Utility::Error() << "Magnum::Math::Algorithms::svd(): no convergence";
                return std::make_tuple(RectangularMatrix<cols, rows, T>{}, Vector<cols, T>{}, Matrix<cols, T>{ZeroInit});
            }

            /* Shift from bottom 2x2 minor */
            const T y = q[k-1];
            const T h = e[k];
            const T d = e[k-1];
            T x = q[l];
            T f = ((y - z)*(y + z) + (d - h)*(d + h))/(T(2)*h*y);
            const T b = Implementation::pythagoras(f, T(1));
            if(f < T(0))
                f = ((x - z)*(x + z) + h*(y/(f - b) - h))/x;
            else
                f = ((x - z)*(x + z) + h*(y/(f + b) - h))/x;

            /* Next QR transformation */
            /** @todo isn't this extractable elsewhere? */
            T c = T(1);
            T s = T(1);
            for(std::size_t i = l+1; i != k+1; ++i) {
                CORRADE_INTERNAL_ASSERT(i <= k+1);

                const T g1 = c*e[i];
                const T h1 = s*e[i];
                const T y1 = q[i];

                const T z1 = Implementation::pythagoras(f, h1);
                e[i-1] = z1;
                c = f/z1;
                s = h1/z1;
                f = x*c + g1*s;

                const T g2 = -x*s + g1*c;
                const T h2 = y1*s;
                const T y2 = y1*c;

                const Vector<cols, T> a1 = v[i-1];
                const Vector<cols, T> b1 = v[i];
                v[i-1] = a1*c+b1*s;
                v[i] = -a1*s+b1*c;

                const T z2 = Implementation::pythagoras(f, h2);
                q[i-1] = z2;
                c = f/z2;
                s = h2/z2;
                f = c*g2 + s*y2;
                x = -s*g2 + c*y2;

                const Vector<rows, T> a2 = m[i-1];
                const Vector<rows, T> b2 = m[i];
                m[i-1] = a2*c+b2*s;
                m[i] = -a2*s+b2*c;
            }

            e[l] = T(0);
            e[k] = f;
            q[k] = x;
        }
    }

    return std::make_tuple(m, q, v);
}

}}}

#endif
