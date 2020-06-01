#ifndef Magnum_Math_Math_h
#define Magnum_Math_Math_h
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
 * @brief Forward declarations for the @ref Magnum::Math namespace
 */

#include <cstddef>

#include "Magnum/Types.h"

namespace Magnum { namespace Math {

/** @todo Denormals to zero */

#ifndef DOXYGEN_GENERATING_OUTPUT
template<std::size_t> class BoolVector;

template<class> struct Constants;

template<class> class Complex;
template<class> class Dual;
template<class> class DualComplex;
template<class> class DualQuaternion;

template<class> class Frustum;

template<std::size_t, class> class Matrix;
template<class T> using Matrix2x2 = Matrix<2, T>;
template<class T> using Matrix3x3 = Matrix<3, T>;
template<class T> using Matrix4x4 = Matrix<4, T>;

template<class> class Matrix3;
template<class> class Matrix4;

template<class> class Quaternion;

template<std::size_t, std::size_t, class> class RectangularMatrix;
template<class T> using Matrix2x3 = RectangularMatrix<2, 3, T>;
template<class T> using Matrix3x2 = RectangularMatrix<3, 2, T>;
template<class T> using Matrix2x4 = RectangularMatrix<2, 4, T>;
template<class T> using Matrix4x2 = RectangularMatrix<4, 2, T>;
template<class T> using Matrix3x4 = RectangularMatrix<3, 4, T>;
template<class T> using Matrix4x3 = RectangularMatrix<4, 3, T>;

template<template<class> class, class> class Unit;
template<class> class Deg;
template<class> class Rad;

class Half;

template<std::size_t, class> class Vector;
template<class> class Vector2;
template<class> class Vector3;
template<class> class Vector4;

template<class> struct ColorHsv;
template<class> class Color3;
template<class> class Color4;

template<UnsignedInt, UnsignedInt, class> class Bezier;
template<UnsignedInt dimensions, class T> using QuadraticBezier = Bezier<2, dimensions, T>;
template<UnsignedInt dimensions, class T> using CubicBezier = Bezier<3, dimensions, T>;
template<class T> using QuadraticBezier2D = QuadraticBezier<2, T>;
template<class T> using QuadraticBezier3D = QuadraticBezier<3, T>;
template<class T> using CubicBezier2D = CubicBezier<2, T>;
template<class T> using CubicBezier3D = CubicBezier<3, T>;

template<class> class CubicHermite;
template<class T> using CubicHermite1D = CubicHermite<T>;
template<class T> using CubicHermite2D = CubicHermite<Vector2<T>>;
template<class T> using CubicHermite3D = CubicHermite<Vector3<T>>;
template<class T> using CubicHermiteComplex = CubicHermite<Complex<T>>;
template<class T> using CubicHermiteQuaternion = CubicHermite<Quaternion<T>>;

template<UnsignedInt, class> class Range;
template<class T> using Range1D = Range<1, T>;
template<class> class Range2D;
template<class> class Range3D;

namespace Implementation {
    template<class> struct StrictWeakOrdering;
}
#endif

}}

#endif
