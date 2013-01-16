#ifndef Magnum_Math_Constants_h
#define Magnum_Math_Constants_h
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
 * @brief Class Magnum::Math::Constants, functions Magnum::Math::deg(), Magnum::Math::rad()
 */

namespace Magnum { namespace Math {

/**
@brief Numeric constants

@see Magnum::Constants
*/
template<class T> struct Constants {
    Constants() = delete;

    /* See MathTypeTraits for answer why these are functions and not constants. */
    #ifdef DOXYGEN_GENERATING_OUTPUT
    /**
     * @brief Pi
     *
     * @see deg(), rad()
     */
    static inline constexpr T pi();

    static inline constexpr T sqrt2();  /**< @brief Square root of 2 */
    static inline constexpr T sqrt3();  /**< @brief Square root of 3 */
    #endif
};

#ifndef DOXYGEN_GENERATING_OUTPUT
template<> struct Constants<double> {
    Constants() = delete;

    static inline constexpr double pi()    { return 3.141592653589793; }
    static inline constexpr double sqrt2() { return 1.414213562373095; }
    static inline constexpr double sqrt3() { return 1.732050807568877; }
};
template<> struct Constants<float> {
    Constants() = delete;

    static inline constexpr float pi()    { return 3.141592654f; }
    static inline constexpr float sqrt2() { return 1.414213562f; }
    static inline constexpr float sqrt3() { return 1.732050808f; }
};
#endif

/**
@brief Angle in degrees

Function to make angle entering less error-prone. Converts the value to
radians at compile time. For example `deg(180.0f)` is converted to `3.14f`.

Usable for entering e.g. rotation:
@code
Matrix4::rotation(deg(30.0f), Vector3::yAxis());
@endcode

@see Magnum::deg(), Constants, rad()
*/
template<class T> inline constexpr T deg(T value) { return value*Constants<T>::pi()/180; }

/**
@brief Angle in radians

See deg() for more information.
@see Magnum::rad()
*/
template<class T> inline constexpr T rad(T value) { return value; }

}}

#endif
