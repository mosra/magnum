#ifndef Magnum_Math_Point2D_h
#define Magnum_Math_Point2D_h
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
 * @brief Class Magnum::Math::Point2D
 */

#include "Vector3.h"

namespace Magnum { namespace Math {

/**
@brief Two-dimensional homogeneous coordinates
@tparam T   Data type

Same as Vector3, except that constructors have default value for Z component
set to one. See also @ref matrix-vector for brief introduction.
@see Magnum::Point2D, Point3D
@configurationvalueref{Magnum::Math::Point2D}
*/
template<class T> class Point2D: public Vector3<T> {
    public:
        /**
         * @brief Default constructor
         *
         * @f[
         *      \boldsymbol p = (0, 0, 1)^T
         * @f]
         */
        inline constexpr /*implicit*/ Point2D(): Vector3<T>(T(0), T(0), T(1)) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol p = (x, y, z)^T
         * @f]
         */
        inline constexpr /*implicit*/ Point2D(T x, T y, T z = T(1)): Vector3<T>(x, y, z) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol p = (v_x, v_y, z)^T
         * @f]
         */
        inline constexpr /*implicit*/ Point2D(const Vector2<T>& xy, T z): Vector3<T>(xy, z) {}

        /**
         * @brief Construct 2D point from 2D vector
         *
         * @f[
         *      \boldsymbol p = (v_x, v_y, 1)^T
         * @f]
         */
        inline constexpr explicit Point2D(const Vector2<T>& xy): Vector3<T>(xy, T(1)) {}

        /** @copydoc Vector::Vector(const Vector<size, U>&) */
        template<class U> inline constexpr explicit Point2D(const Vector<3, U>& other): Vector3<T>(other) {}

        /** @brief Copy constructor */
        inline constexpr Point2D(const Vector<3, T>& other): Vector3<T>(other) {}

        /**
         * @brief Vector part of the point
         *
         * Equivalent to calling xy(). Useful for seamless 2D/3D integration.
         * @see Point3D::vector()
         */
        inline Vector2<T>& vector() { return Vector3<T>::xy(); }
        inline constexpr Vector2<T> vector() const { return Vector3<T>::xy(); } /**< @overload */

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Point2D, 3)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Point2D, 3)

/** @debugoperator{Magnum::Math::Point2D} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Point2D<T>& value) {
    return debug << static_cast<const Vector<3, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Point2D} */
    template<class T> struct ConfigurationValue<Magnum::Math::Point2D<T>>: public ConfigurationValue<Magnum::Math::Vector<3, T>> {};
}}

#endif
