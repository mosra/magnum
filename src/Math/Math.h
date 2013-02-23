#ifndef Magnum_Math_Math_h
#define Magnum_Math_Math_h
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
 * @brief Forward declarations for Magnum::Math namespace
 */

#include <cstddef>

namespace Magnum { namespace Math {

/** @todoc Remove `ifndef` when Doxygen is sane again */
#ifndef DOXYGEN_GENERATING_OUTPUT
/* Class Constants used only statically */

template<class T> class Complex;
template<class T> class Dual;
template<class> class DualQuaternion;

template<std::size_t, class> class Matrix;
template<class> class Matrix3;
template<class> class Matrix4;

template<class> class Quaternion;

template<std::size_t, std::size_t, class> class RectangularMatrix;

template<template<class> class, class> class Unit;
template<class> class Deg;
template<class> class Rad;

template<std::size_t, class> class Vector;
template<class> class Vector2;
template<class> class Vector3;
template<class> class Vector4;

namespace Geometry {
    template<class> class Rectangle;
}
#endif

}}

#endif
