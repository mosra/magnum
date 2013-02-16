#ifndef Magnum_Math_Point3D_h
#define Magnum_Math_Point3D_h
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
 * @brief Class Magnum::Math::Point3D
 */

#include "Vector4.h"

namespace Magnum { namespace Math {

/**
@brief Three-dimensional homogeneous coordinates
@tparam T   Data type

Same as Vector4, except that constructors have default value for W component
set to one. See also @ref matrix-vector for brief introduction.
@see Magnum::Point3D, Point2D
@configurationvalueref{Magnum::Math::Point3D}
*/
template<class T> class Point3D: public Vector4<T> {
    public:
        /**
         * @brief Default constructor
         *
         * @f[
         *      \boldsymbol p = (0, 0, 0, 1)^T
         * @f]
         */
        inline constexpr /*implicit*/ Point3D(): Vector4<T>(T(0), T(0), T(0), T(1)) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol p = (x, y, z, w)^T
         * @f]
         */
        inline constexpr /*implicit*/ Point3D(T x, T y, T z, T w = T(1)): Vector4<T>(x, y, z, w) {}

        /**
         * @brief Constructor
         *
         * @f[
         *      \boldsymbol p = (v_x, v_y, v_z, w)^T
         * @f]
         */
        inline constexpr /*implicit*/ Point3D(const Vector3<T>& xyz, T w): Vector4<T>(xyz, w) {}

        /**
         * @brief Construct 3D point from 3D vector
         *
         * @f[
         *      \boldsymbol p = (v_x, v_y, v_z, 1)^T
         * @f]
         */
        inline constexpr explicit Point3D(const Vector3<T>& xyz): Vector4<T>(xyz, T(1)) {}

        /** @copydoc Vector::Vector(const Vector<size, U>&) */
        template<class U> inline constexpr explicit Point3D(const Vector<4, U>& other): Vector4<T>(other) {}

        /** @brief Copy constructor */
        inline constexpr Point3D(const Vector<4, T>& other): Vector4<T>(other) {}

        /**
         * @brief Vector part of the point
         *
         * Equivalent to calling xyz(). Useful for seamless 2D/3D integration.
         * @see Point2D::vector()
         */
        inline Vector3<T>& vector() { return Vector4<T>::xyz(); }
        inline constexpr Vector3<T> vector() const { return Vector4<T>::xyz(); } /**< @overload */

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Point3D, 4)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Point3D, 4)

/** @debugoperator{Magnum::Math::Point3D} */
template<class T> inline Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Point3D<T>& value) {
    return debug << static_cast<const Vector<4, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Point3D} */
    template<class T> struct ConfigurationValue<Magnum::Math::Point3D<T>>: public ConfigurationValue<Magnum::Math::Vector<4, T>> {};
}}

#endif
