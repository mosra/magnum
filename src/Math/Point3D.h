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
@configurationvalueref{Magnum::Math::Point3D}
*/
template<class T> class Point3D: public Vector4<T> {
    public:
        /**
         * @brief Default constructor
         *
         * X, Y and Z components are set to zero, W is set to one.
         */
        inline constexpr Point3D(): Vector4<T>(T(0), T(0), T(0), T(1)) {}

        /** @brief Copy constructor */
        inline constexpr Point3D(const RectangularMatrix<1, 4, T>& other): Vector4<T>(other) {}

        /**
         * @brief Constructor
         * @param x     X component
         * @param y     Y component
         * @param z     Z component
         * @param w     W component
         */
        inline constexpr Point3D(T x, T y, T z, T w = T(1)): Vector4<T>(x, y, z, w) {}

        /**
         * @brief Constructor
         * @param xyz   Three-component vector
         * @param w     W component
         */
        inline constexpr Point3D(const Vector<3, T>& xyz, T w = T(1)): Vector4<T>(xyz, w) {}

        /**
         * @brief Vector part of the point
         *
         * Equivalent to calling xyz(). Useful for seamless 2D/3D integration.
         * @see Point2D::vector()
         */
        inline Vector3<T>& vector() { return Vector4<T>::xyz(); }
        inline constexpr Vector3<T> vector() const { return Vector4<T>::xyz(); } /**< @overload */

        MAGNUM_VECTOR_SUBCLASS_IMPLEMENTATION(Point3D, 4)
        MAGNUM_RECTANGULARMATRIX_SUBCLASS_OPERATOR_IMPLEMENTATION(1, 4, Point3D<T>)
};

MAGNUM_VECTOR_SUBCLASS_OPERATOR_IMPLEMENTATION(Point3D, 4)

/** @debugoperator{Magnum::Math::Point3D} */
template<class T> Corrade::Utility::Debug operator<<(Corrade::Utility::Debug debug, const Magnum::Math::Point3D<T>& value) {
    return debug << static_cast<const Magnum::Math::Vector<4, T>&>(value);
}

}}

namespace Corrade { namespace Utility {
    /** @configurationvalue{Magnum::Math::Point3D} */
    template<class T> struct ConfigurationValue<Magnum::Math::Point3D<T>>: public ConfigurationValue<Magnum::Math::Vector<4, T>> {};
}}

#endif
