#ifndef Magnum_Math_constants_h
#define Magnum_Math_constants_h
/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Constants
 */

namespace Magnum { namespace Math {

/** @brief Pi */
#define PI 3.1415926535

/** @brief Maximal tolerance when comparing floats */
#define EPSILON 1.0e-6

/**
 * @brief Angle in degrees
 *
 * Function to make angle entering less error-prone. Converts the value to
 * radians at compile time. For example @c deg(180.0f) is converted to @c 3.14f.
 */
template<class T> inline constexpr T deg(T value) { return value*PI/180; }

/**
 * @brief Angle in radians
 *
 * See also deg().
 */
template<class T> inline constexpr T rad(T value) { return value; }

}}

#endif
